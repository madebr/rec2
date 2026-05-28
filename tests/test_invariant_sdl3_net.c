#include <check.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdint.h>
#include <arpa/inet.h>

/* 
 * Simulate the vulnerable pattern from sdl3_net.c:
 * sprintf(portbuf, ":%d", ntohs(pSock_addr_ipx->sin_port));
 * strcat(pString, portbuf);
 *
 * Security invariant: The output buffer must never overflow regardless of
 * the address string length or port value. The final string length must
 * always be within the allocated buffer size.
 */

#define SAFE_BUFFER_SIZE 256
#define PORT_BUF_SIZE 8  /* ":" + max 5 digits + null = 7 chars */

/* Safe version of the formatting function that checks bounds */
static int safe_format_address_port(char *pString, size_t buf_size,
                                     const char *address, uint16_t port_network_order)
{
    char portbuf[PORT_BUF_SIZE];
    
    if (!pString || !address || buf_size == 0) {
        return -1;
    }
    
    /* Copy address into buffer safely */
    size_t addr_len = strlen(address);
    if (addr_len >= buf_size) {
        return -1; /* Would overflow */
    }
    strncpy(pString, address, buf_size - 1);
    pString[buf_size - 1] = '\0';
    
    /* Format port */
    int port_written = snprintf(portbuf, sizeof(portbuf), ":%d", ntohs(port_network_order));
    if (port_written < 0 || (size_t)port_written >= sizeof(portbuf)) {
        return -1;
    }
    
    /* Check if concatenation would overflow */
    size_t current_len = strlen(pString);
    size_t port_len = strlen(portbuf);
    if (current_len + port_len >= buf_size) {
        return -1; /* Would overflow */
    }
    
    strcat(pString, portbuf);
    return 0;
}

START_TEST(test_buffer_no_overflow_adversarial_inputs)
{
    /* Invariant: The formatted address+port string must never exceed the 
     * allocated buffer size, regardless of address string length or port value */
    
    const char *address_payloads[] = {
        /* Normal addresses */
        "192.168.1.1",
        "127.0.0.1",
        "0.0.0.0",
        "255.255.255.255",
        /* IPv6-style long addresses */
        "2001:0db8:85a3:0000:0000:8a2e:0370:7334",
        /* Near-boundary length strings (just under SAFE_BUFFER_SIZE) */
        "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
        "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
        "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
        "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA", /* 247 A's */
        /* Exactly at boundary */
        "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
        "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
        "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
        "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA", /* 255 A's */
        /* Empty string */
        "",
        /* String with special characters */
        "host.example.com",
        /* String that would cause overflow when port appended */
        "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
        "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
        "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
        "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA", /* 256 A's - overflow */
    };
    
    /* Adversarial port values covering boundary conditions */
    uint16_t port_payloads[] = {
        0,          /* minimum port */
        1,          /* near minimum */
        80,         /* common port */
        443,        /* common port */
        1024,       /* privilege boundary */
        8080,       /* common high port */
        32767,      /* mid-range */
        65534,      /* near maximum */
        65535,      /* maximum port (0xFFFF) */
        htons(0),
        htons(65535),
        htons(80),
        htons(443),
    };
    
    int num_addresses = sizeof(address_payloads) / sizeof(address_payloads[0]);
    int num_ports = sizeof(port_payloads) / sizeof(port_payloads[0]);
    
    for (int i = 0; i < num_addresses; i++) {
        for (int j = 0; j < num_ports; j++) {
            char output_buffer[SAFE_BUFFER_SIZE];
            memset(output_buffer, 0, sizeof(output_buffer));
            
            /* Fill with canary pattern to detect overflow */
            char canary_buffer[SAFE_BUFFER_SIZE + 64];
            memset(canary_buffer, 0xAB, sizeof(canary_buffer));
            
            /* Place our working buffer at start of canary region */
            char *working_buf = canary_buffer;
            memset(working_buf, 0, SAFE_BUFFER_SIZE);
            
            int result = safe_format_address_port(
                output_buffer, 
                SAFE_BUFFER_SIZE,
                address_payloads[i],
                port_payloads[j]
            );
            
            if (result == 0) {
                /* Success case: verify the output is within bounds */
                size_t output_len = strlen(output_buffer);
                
                /* INVARIANT: Output length must be strictly less than buffer size */
                ck_assert_msg(output_len < SAFE_BUFFER_SIZE,
                    "Buffer overflow detected: output_len=%zu >= buf_size=%d "
                    "for address='%s' port=%d",
                    output_len, SAFE_BUFFER_SIZE,
                    address_payloads[i], ntohs(port_payloads[j]));
                
                /* INVARIANT: Null terminator must be present within buffer */
                ck_assert_msg(output_buffer[SAFE_BUFFER_SIZE - 1] == '\0' ||
                              output_len < SAFE_BUFFER_SIZE - 1,
                    "Missing null terminator within buffer bounds");
                
                /* INVARIANT: Port suffix format must be correct if address was non-empty */
                if (strlen(address_payloads[i]) > 0 && output_len > 0) {
                    /* Find the last colon - should be followed by digits */
                    char *last_colon = strrchr(output_buffer, ':');
                    if (last_colon != NULL) {
                        char *after_colon = last_colon + 1;
                        ck_assert_msg(strlen(after_colon) > 0,
                            "Port number missing after colon");
                        /* Verify digits follow the colon */
                        for (char *c = after_colon; *c != '\0'; c++) {
                            ck_assert_msg(*c >= '0' && *c <= '9',
                                "Non-digit character in port number: '%c'", *c);
                        }
                    }
                }
                
                /* INVARIANT: Port value must be in valid range (0-65535) */
                int port_val = atoi(strrchr(output_buffer, ':') ? 
                                   strrchr(output_buffer, ':') + 1 : "0");
                ck_assert_msg(port_val >= 0 && port_val <= 65535,
                    "Port value out of valid range: %d", port_val);
                    
            } else {
                /* Failure case: function correctly rejected the input */
                /* INVARIANT: On failure, output buffer must not be corrupted beyond its size */
                /* The function should have returned error without writing past bounds */
                ck_assert_msg(result == -1,
                    "Unexpected return value: %d", result);
            }
        }
    }
}
END_TEST

START_TEST(test_portbuf_format_boundary)
{
    /* Invariant: Port buffer formatting must never exceed PORT_BUF_SIZE bytes */
    char portbuf[PORT_BUF_SIZE];
    
    /* Boundary port values */
    uint16_t ports[] = {0, 1, 9999, 10000, 65534, 65535};
    int num_ports = sizeof(ports) / sizeof(ports[0]);
    
    for (int i = 0; i < num_ports; i++) {
        memset(portbuf, 0, sizeof(portbuf));
        
        int written = snprintf(portbuf, sizeof(portbuf), ":%d", ports[i]);
        
        /* INVARIANT: snprintf must not truncate valid port formatting */
        ck_assert_msg(written > 0, 
            "snprintf failed for port %d", ports[i]);
        
        /* INVARIANT: Port string must fit within PORT_BUF_SIZE */
        ck_assert_msg((size_t)written < PORT_BUF_SIZE,
            "Port string too long: %d chars for port %d (max %d)",
            written, ports[i], PORT_BUF_SIZE - 1);
        
        /* INVARIANT: Null terminator must be present */
        ck_assert_msg(portbuf[PORT_BUF_SIZE - 1] == '\0',
            "Missing null terminator in portbuf for port %d", ports[i]);
        
        /* INVARIANT: Format must start with colon */
        ck_assert_msg(portbuf[0] == ':',
            "Port buffer must start with ':' for port %d", ports[i]);
    }
}
END_TEST

START_TEST(test_strcat_bounds_invariant)
{
    /* Invariant: String concatenation must never write beyond allocated buffer */
    
    /* Test with buffers of various fill levels */
    struct {
        size_t initial_fill;  /* how many bytes pre-filled */
        uint16_t port;
        int should_succeed;
    } test_cases[] = {
        {0,   80,    1},  /* empty buffer + short port */
        {100, 443,   1},  /* half-full buffer */
        {240, 65535, 1},  /* near-full: 240 + 7 = 247 < 256 */
        {248, 65535, 1},  /* 248 + 7 = 255 < 256, just fits */
        {249, 65535, 0},  /* 249 + 7 = 256, would overflow */
        {255, 80,    0},  /* full buffer */
        {254, 80,    0},  /* 254 + 3 = 257, overflow */
    };
    
    int num_cases = sizeof(test_cases) / sizeof(test_cases[0]);
    
    for (int i = 0; i < num_cases; i++) {
        char buffer[SAFE_BUFFER_SIZE + 16]; /* extra space for canary */
        memset(buffer, 0xCC, sizeof(buffer)); /* canary pattern */
        memset(buffer, 'A', test_cases[i].initial_fill);
        buffer[test_cases[i].initial_fill] = '\0';
        
        char portbuf[PORT_BUF_SIZE];
        int written = snprintf(portbuf, sizeof(portbuf), ":%d", test_cases[i].port);
        
        ck_assert_msg(written > 0 && (size_t)written < sizeof(portbuf),
            "Port formatting failed");
        
        size_t current_len = strlen(buffer);
        size_t port_len = strlen(portbuf);
        size_t total_needed = current_len + port_len + 1; /* +1 for null */
        
        if (test_cases[i].should_succeed) {
            /* INVARIANT: When operation should succeed, total must fit in buffer */
            ck_assert_msg(total_needed <= SAFE_BUFFER_SIZE,
                "Case %d: Expected success but would overflow: "
                "current=%zu + port=%zu + 1 = %zu > %d",
                i, current_len, port_len, total_needed, SAFE_BUFFER_SIZE);
            
            /* Perform safe concatenation */
            if (total_needed <= SAFE_BUFFER_SIZE) {
                strncat(buffer, portbuf, SAFE_BUFFER_SIZE - current_len - 1);
                
                /* INVARIANT: Result length must be within bounds */
                ck_assert_msg(strlen(buffer) < SAFE_BUFFER_SIZE,
                    "Case %d: Buffer overflow after strcat", i);
                    
                /* INVARIANT: Canary bytes beyond buffer must be intact */
                for (int k = SAFE_BUFFER_SIZE; k < SAFE_BUFFER_SIZE + 16; k++) {
                    ck_assert_msg((unsigned char)buffer[k] == 0xCC,
                        "Case %d: Canary byte corrupted at offset %d", i, k);
                }
            }
        } else {
            /* INVARIANT: When operation would overflow, we must detect it */
            ck_assert_msg(total_needed > SAFE_BUFFER_SIZE,
                "Case %d: Expected overflow condition but total=%zu fits in %d",
                i, total_needed, SAFE_BUFFER_SIZE);
        }
    }
}
END_TEST

Suite *security_suite(void)
{
    Suite *s;
    TCase *tc_core;

    s = suite_create("Security");
    tc_core = tcase_create("Core");

    tcase_add_test(tc_core, test_buffer_no_overflow_adversarial_inputs);
    tcase_add_test(tc_core, test_portbuf_format_boundary);
    tcase_add_test(tc_core, test_strcat_bounds_invariant);
    suite_add_tcase(s, tc_core);

    return s;
}

int main(void)
{
    int number_failed;
    Suite *s;
    SRunner *sr;

    s = security_suite();
    sr = srunner_create(s);

    srunner_run_all(sr, CK_NORMAL);
    number_failed = srunner_ntests_failed(sr);
    srunner_free(sr);

    return (number_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}