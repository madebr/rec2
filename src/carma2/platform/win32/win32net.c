#include "win32net.h"

#include "win32.h"

#include "errors.h"
#include "platform.h"

#include "c2_string.h"

#include <winsock.h>
#include <wsipx.h>

#define BROADCAST_HEADER "CAR2MSG"
#define JOINABLE_GAMES_CAPACITY 16

C2_HOOK_VARIABLE_IMPLEMENT_ARRAY(char, gPathNetworkIni, 240, 0x006b3380);
C2_HOOK_VARIABLE_IMPLEMENT(int, gPathNetworkIniValid, 0x006ad4c4);
C2_HOOK_VARIABLE_IMPLEMENT(tU16, gIpx_sock_addr, 0x006ac12c);
C2_HOOK_VARIABLE_IMPLEMENT(struct sockaddr, gLocal_address, 0x006ac310);
C2_HOOK_VARIABLE_IMPLEMENT(struct sockaddr, gLocal_address_2, 0x006abf08);
C2_HOOK_VARIABLE_IMPLEMENT(struct sockaddr*, gPtr_local_address, 0x006ac358);
C2_HOOK_VARIABLE_IMPLEMENT(struct sockaddr, gListen_address, 0x006ac118);
C2_HOOK_VARIABLE_IMPLEMENT(struct sockaddr*, gPtr_listen_address, 0x006ac340);
C2_HOOK_VARIABLE_IMPLEMENT(struct sockaddr, gBroadcast_address, 0x006ac300);
C2_HOOK_VARIABLE_IMPLEMENT(struct sockaddr*, gPtr_broadcast_address, 0x006abf00);
C2_HOOK_VARIABLE_IMPLEMENT(WSADATA, gWSA_data, 0x006ac170);
C2_HOOK_VARIABLE_IMPLEMENT_INIT(SOCKET, gSocket, 0x006619b0, -1);
C2_HOOK_VARIABLE_IMPLEMENT_ARRAY(char, gLocal_address_text, 32, 0x006ac320);
C2_HOOK_VARIABLE_IMPLEMENT(int, gNumber_of_networks, 0x006ac128);
C2_HOOK_VARIABLE_IMPLEMENT(int, gMsg_header_strlen, 0x006abf04);
C2_HOOK_VARIABLE_IMPLEMENT(int, gSpecial_server, 0x006ac578);
C2_HOOK_VARIABLE_IMPLEMENT_ARRAY_ADV(char, gIpx_network_numbers, [16][4], 0x006ac130);
C2_HOOK_VARIABLE_IMPLEMENT_ARRAY(char, gSend_buffer, 512, 0x006ac360);
C2_HOOK_VARIABLE_IMPLEMENT_ARRAY(char, gReceive_buffer, 512, 0x006abf18);

void C2_HOOK_FASTCALL PDNetObtainSystemUserName(char* pName, int pMax_length) {
    char buffer[16];
    DWORD size;
    char *p;

    dr_dprintf("PDNetObtainSystemUserName()");

    if (GetComputerNameA(buffer, &size) && size != 0) {
        c2_strncpy(pName, buffer, pMax_length - 1);
        pName[pMax_length - 1] = '\0';
        while ((p = c2_strpbrk(pName, "_=(){}[]<>!$%^&*/:@~;'#,?\\|`\"")) != NULL) {
            *p = '-';
        }
    }
}
C2_HOOK_FUNCTION(0x0051a190, PDNetObtainSystemUserName)

void C2_HOOK_FASTCALL NetNowIPXLocalTarget2String(char* pString, struct sockaddr* pSock_addr) {

    c2_sprintf(pString,
        "%2.2x%2.2x%2.2x%2.2x:%2.2x%2.2x%2.2x%2.2x%2.2x%2.2x,%2.2x%2.2x",
        pSock_addr->sa_data[0],
        pSock_addr->sa_data[1],
        pSock_addr->sa_data[2],
        pSock_addr->sa_data[3],
        pSock_addr->sa_data[4],
        pSock_addr->sa_data[5],
        pSock_addr->sa_data[6],
        pSock_addr->sa_data[7],
        pSock_addr->sa_data[8],
        pSock_addr->sa_data[9],
        pSock_addr->sa_data[10],
        pSock_addr->sa_data[11]);
}
C2_HOOK_FUNCTION(0x0051a010, NetNowIPXLocalTarget2String)

int C2_HOOK_FASTCALL PDNetInitialise(void) {
    char default_null;
    char s[32];
    static u_long broadcast = 1;
    LINGER linger;
    int namelen;

    default_null = '\0';
    dr_dprintf("PDNetInitialise()");
    C2V(gIpx_sock_addr) = 0xfe2f;
    if (C2V(gPathNetworkIniValid) && GetPrivateProfileStringA("Network", "IPXSocket", &default_null, s, sizeof(s), C2V(gPathNetworkIni)) != 0) {
        int ipx_address;
        int count;

        count = c2_sscanf(s, "%x", &ipx_address);
        if (count != 0 && count != -1) {
            dr_dprintf("New IPX socket %4.4x", socket);
            /* byte swap */
            C2V(gIpx_sock_addr) = (ipx_address << 8) | ((ipx_address >> 8) & 0xff);
        }
    }

    c2_memset(C2V(gLocal_address).sa_data, 0, sizeof(C2V(gLocal_address).sa_data));
    C2V(gLocal_address).sa_family = AF_IPX;
    C2V(gLocal_address).sa_data[10] = C2V(gIpx_sock_addr) & 0xff;
    C2V(gLocal_address).sa_data[11] = C2V(gIpx_sock_addr) >> 8;
    C2V(gPtr_local_address) = &C2V(gLocal_address);

    c2_memset(C2V(gListen_address).sa_data, 0, sizeof(C2V(gListen_address).sa_data));
    C2V(gListen_address).sa_family = AF_IPX;
    C2V(gListen_address).sa_data[10] = C2V(gIpx_sock_addr) & 0xff;
    C2V(gListen_address).sa_data[11] = C2V(gIpx_sock_addr) >> 8;
    C2V(gPtr_listen_address) = &C2V(gListen_address);

    c2_memset(C2V(gBroadcast_address).sa_data, 0, sizeof(C2V(gListen_address).sa_data));
    C2V(gBroadcast_address).sa_family = AF_IPX;
    C2V(gBroadcast_address).sa_data[4] = 0xff;
    C2V(gBroadcast_address).sa_data[5] = 0xff;
    C2V(gBroadcast_address).sa_data[6] = 0xff;
    C2V(gBroadcast_address).sa_data[7] = 0xff;
    C2V(gBroadcast_address).sa_data[8] = 0xff;
    C2V(gBroadcast_address).sa_data[9] = 0xff;
    C2V(gBroadcast_address).sa_data[10] = C2V(gIpx_sock_addr) & 0xff;
    C2V(gBroadcast_address).sa_data[11] = C2V(gIpx_sock_addr) >> 8;
    C2V(gPtr_broadcast_address) = &C2V(gBroadcast_address);

    if (WSAStartup(MAKEWORD(1, 1), &C2V(gWSA_data)) == -1) {
        /* FIXME WSAStartup returns error code */
        dr_dprintf("PDNetInitialise(): WSAStartup() failed");
        return -1;
    }

    C2V(gSocket) = socket(AF_IPX, SOCK_DGRAM, NSPROTO_IPX);
    if (C2V(gSocket) == INVALID_SOCKET) {
        dr_dprintf("PDNetInitialise(): Failed to create socket - WSAGetLastError=%d", WSAGetLastError());
        WSACleanup();
        return -1;
    }
    setsockopt(C2V(gSocket), SOL_SOCKET, SO_BROADCAST, (const char*)&broadcast, sizeof(broadcast));

    linger.l_onoff = 1;
    linger.l_linger = 0;
    setsockopt(C2V(gSocket), SOL_SOCKET, SO_LINGER, (const char*)&linger, sizeof(linger));

    if (ioctlsocket(C2V(gSocket), FIONBIO, &broadcast) == SOCKET_ERROR) {
        dr_dprintf("Error on ioctlsocket() - WSAGetLastError=%d", WSAGetLastError());
        closesocket(C2V(gSocket));
        WSACleanup();
        return -1;
    }

    C2_HOOK_BUG_ON(sizeof(SOCKADDR_IPX) != 14);

    if (bind(C2V(gSocket), &C2V(gLocal_address), sizeof(SOCKADDR_IPX)) == SOCKET_ERROR) {
        dr_dprintf("Error on bind() - WSAGetLastError=%d", WSAGetLastError());
        closesocket(C2V(gSocket));
        WSACleanup();
        return -1;
    }

    namelen = sizeof(SOCKADDR_IPX);
    getsockname(C2V(gSocket), &C2V(gLocal_address), &namelen);
    NetNowIPXLocalTarget2String(C2V(gLocal_address_text), C2V(gPtr_local_address));

    c2_memcpy(C2V(gIpx_network_numbers)[0], C2V(gPtr_local_address)->sa_data, 4);
    C2V(gNumber_of_networks) = 1;
    if (C2V(gPathNetworkIniValid)) {
        char key[32];
        int i;

        for (i = 0; i < 16; i++) {
            c2_sprintf(key, "%s%d", "Network", i);
            if (GetPrivateProfileStringA("Network", key, &default_null, s, sizeof(s), C2V(gPathNetworkIni)) != 0) {
                int count;
                tU32 netnum;

                count = c2_sscanf(s, "%x", &netnum);
                if (count != 0 && count != -1) {
                    dr_dprintf("Found network number '%x' in INI file", netnum);
                    C2V(gIpx_network_numbers)[C2V(gNumber_of_networks)][0] = netnum >> 24;
                    C2V(gIpx_network_numbers)[C2V(gNumber_of_networks)][1] = netnum >> 16;
                    C2V(gIpx_network_numbers)[C2V(gNumber_of_networks)][2] = netnum >> 8;
                    C2V(gIpx_network_numbers)[C2V(gNumber_of_networks)][3] = netnum >> 0;
                    if (C2V(gIpx_network_numbers)[C2V(gNumber_of_networks)][0] != C2V(gPtr_local_address)->sa_data[0]
                        || C2V(gIpx_network_numbers)[C2V(gNumber_of_networks)][1] != C2V(gPtr_local_address)->sa_data[1]
                        || C2V(gIpx_network_numbers)[C2V(gNumber_of_networks)][2] != C2V(gPtr_local_address)->sa_data[2]
                        || C2V(gIpx_network_numbers)[C2V(gNumber_of_networks)][3] != C2V(gPtr_local_address)->sa_data[3]) {
                        C2V(gNumber_of_networks)++;
                    }
                }
            }
        }
        dr_dprintf("Total networks = %d", C2V(gNumber_of_networks));
    }
    c2_memcpy(&C2V(gLocal_address_2), C2V(gPtr_local_address), sizeof(SOCKADDR_IPX));
    dr_dprintf("Socket bound OK; local address is '%s'", C2V(gLocal_address_text));

    C2V(gMsg_header_strlen) = c2_strlen(BROADCAST_HEADER);
    if (c2_strstr(C2V(gLocal_address_text), "00a0240f9fac") != NULL) {
        C2V(gSpecial_server) = 1;
    }
    return 0;
}
C2_HOOK_FUNCTION(0x00519560, PDNetInitialise)

int (C2_HOOK_FASTCALL * PDNetShutdown_original)(void);
int C2_HOOK_FASTCALL PDNetShutdown(void) {

#if 0//defined(C2_HOOKS_ENABLED)
    return PDNetShutdown_original();
#else

    dr_dprintf("PDNetShutdown()");
    if (C2V(gSocket) != SOCKET_ERROR) {
        closesocket(C2V(gSocket));
    }
    C2V(gSocket) = SOCKET_ERROR;
    return 0;
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x00519a10, PDNetShutdown, PDNetShutdown_original)

C2_HOOK_VARIABLE_IMPLEMENT(int, gNumber_of_hosts, 0x006ac35c);
C2_HOOK_VARIABLE_IMPLEMENT(tPD_net_game_info*, gJoinable_games, 0x006ac574);

void C2_HOOK_FASTCALL PDNetStartProducingJoinList(void) {

    C2_HOOK_BUG_ON(sizeof(tPD_net_game_info) * JOINABLE_GAMES_CAPACITY != 0x140);

    dr_dprintf("PDNetStartProducingJoinList()");
    C2V(gNumber_of_hosts) = 0;
    C2V(gJoinable_games) = BrMemAllocate(sizeof(tPD_net_game_info) * JOINABLE_GAMES_CAPACITY, BR_MEMORY_APPLICATION);
    if (C2V(gJoinable_games) == NULL) {
        PDFatalError("Can't allocate memory for joinable games");
    }
}
C2_HOOK_FUNCTION(0x00519a40, PDNetStartProducingJoinList)

void C2_HOOK_FASTCALL PDNetEndJoinList(void) {

    dr_dprintf("PDNetEndJoinList()");
    if (C2V(gJoinable_games) != NULL) {
        BrMemFree(C2V(gJoinable_games));
    }
    C2V(gJoinable_games) = NULL;
}
C2_HOOK_FUNCTION(0x00519a80, PDNetEndJoinList)

int C2_HOOK_FASTCALL BroadcastMessage(void) {
    int i;
    int errors;
    char broadcast_addr_string[32];

    errors = 0;
    for (i = 0; i < C2V(gNumber_of_networks); i++) {
        c2_memcpy(C2V(gPtr_broadcast_address)->sa_data, C2V(gIpx_network_numbers)[0], 4);
        NetNowIPXLocalTarget2String(broadcast_addr_string, C2V(gPtr_broadcast_address));
        dr_dprintf("Broadcasting on address '%s'", broadcast_addr_string);
        if (sendto(C2V(gSocket), C2V(gSend_buffer), c2_strlen(C2V(gSend_buffer)) + 1, 0, &C2V(gBroadcast_address), sizeof(C2V(gBroadcast_address))) == SOCKET_ERROR) {
            dr_dprintf("BroadcastMessage(): Error on sendto() - WSAGetLastError=%d", WSAGetLastError());
            errors = 1;
        }
    }
    return errors == 0;
}

void C2_HOOK_FASTCALL MakeMessageToSend(int pMessage_type) {

#if defined(REC2_FIX_BUGS)
    c2_sprintf(C2V(gSend_buffer), "XXXX%s%1d", BROADCAST_HEADER, pMessage_type);
#else
    sprintf(C2V(gSend_buffer), "XXXX%s%0.1d", BROADCAST_HEADER, pMessage_type);
#endif
}

int C2_HOOK_FASTCALL SameEthernetAddress(struct sockaddr* pAddr_ipx1, struct sockaddr* pAddr_ipx2) {

    return c2_memcmp(&pAddr_ipx1->sa_data[4], &pAddr_ipx2->sa_data[4], 6) == 0;
}

int C2_HOOK_FASTCALL GetMessageTypeFromMessage(char* pMessage_str) {
    char* real_msg;
    int msg_type_int;

    real_msg = &pMessage_str[4];
    msg_type_int = 0;

    // FIXME: "CW95MSG" value is used in and depends on platform
    if (c2_strncmp(real_msg, BROADCAST_HEADER, C2V(gMsg_header_strlen)) == 0) {
        if (isdigit(real_msg[C2V(gMsg_header_strlen)])) {
            msg_type_int = real_msg[C2V(gMsg_header_strlen)] - '0';
        }
        if (msg_type_int != 0 && msg_type_int < 2) {
            return msg_type_int;
        }
    }
    return 999;
}

int C2_HOOK_FASTCALL ReceiveHostResponses(void) {
    char str[256];
    int i;
    int already_registered;

    char addr_string[32];
    int sa_len;
    int wsa_error;

    C2_HOOK_BUG_ON(sizeof(C2V(gReceive_buffer)) != 0x200);

    sa_len = sizeof(SOCKADDR_IPX);
    while (1) {
        if (recvfrom(C2V(gSocket), C2V(gReceive_buffer), sizeof(C2V(gReceive_buffer)), 0, &C2V(gListen_address), &sa_len) == SOCKET_ERROR) {
            break;
        }
        NetNowIPXLocalTarget2String(addr_string, C2V(gPtr_listen_address));
        dr_dprintf("ReceiveHostResponses(): Received string '%s' from %s", C2V(gReceive_buffer), addr_string);

        if (SameEthernetAddress(C2V(gPtr_local_address), C2V(gPtr_listen_address))) {
            dr_dprintf("*** Discounting the above 'cos we sent it ***");
            continue;
        }
        if (GetMessageTypeFromMessage(C2V(gReceive_buffer)) != 2) {
            dr_dprintf("*** Discounting the above 'cos it's not a host reply ***");
            continue;
        }

        dr_dprintf("*** It's a host reply! ***");
        already_registered = 0;
        for (i = 0; i < C2V(gNumber_of_hosts); i++) {
            if (SameEthernetAddress((struct sockaddr*)&C2V(gJoinable_games)[i].addr, C2V(gPtr_listen_address))) {
                already_registered = 1;
                break;
            }
        }
        if (already_registered) {
            dr_dprintf("That game already registered");
            C2V(gJoinable_games)[i].last_response = PDGetTotalTime();
        } else {
            dr_dprintf("Adding joinable game to slot #%d", C2V(gNumber_of_hosts));
            c2_memcpy(C2V(gJoinable_games)[C2V(gNumber_of_hosts)].addr, C2V(gPtr_listen_address), sizeof(struct sockaddr));
            C2V(gJoinable_games)[C2V(gNumber_of_hosts)].last_response = PDGetTotalTime();
            C2V(gNumber_of_hosts)++;
            dr_dprintf("Number of games found so far: %d", C2V(gNumber_of_hosts));
        }
        if (C2V(gNumber_of_hosts) != 0) {
            dr_dprintf("Currently registered net games:");
            for (i = 0; i < C2V(gNumber_of_hosts); i++) {
                NetNowIPXLocalTarget2String(str, (struct sockaddr*)&C2V(gJoinable_games)[i].addr);
                dr_dprintf("%d: Host addr %s", i, str);
            }
        }
    }
    wsa_error = WSAGetLastError() != WSAEWOULDBLOCK;
    if (wsa_error == 0) {
        return 1;
    }
    dr_dprintf("ReceiveHostResponses(): Error on recvfrom() - WSAGetLastError=%d", wsa_error);
    return 0;
}

int C2_HOOK_FASTCALL PDNetGetNextJoinGame(tNet_game_details* pGame, int pIndex) {
    static tU32 next_broadcast_time = 0;
    int i;
    int j;
    int number_of_hosts_has_changed;

    dr_dprintf("PDNetGetNextJoinGame(): pIndex is %d", pIndex);
    if (pIndex == 0) {
        do {
            number_of_hosts_has_changed = 0;
            for (i = 0; i < C2V(gNumber_of_hosts); i++) {
                if (C2V(gJoinable_games)[i].last_response + 10000 < (tU32)PDGetTotalTime()) {
                    number_of_hosts_has_changed = 1;
                    for (j = i; j < C2V(gNumber_of_hosts) - 1; j++) {
                        c2_memmove(&C2V(gJoinable_games)[j], &C2V(gJoinable_games)[j + 1], sizeof(tPD_net_game_info));
                    }
                }
            }
            if (number_of_hosts_has_changed) {
                C2V(gNumber_of_hosts)--;
            }
        } while (number_of_hosts_has_changed);
        if ((tU32)PDGetTotalTime() > next_broadcast_time) {
            next_broadcast_time = PDGetTotalTime() + 3000;
            MakeMessageToSend(1);
            if (BroadcastMessage() == 0) {
                dr_dprintf("PDNetGetNextJoinGame(): Error on BroadcastMessage()");
            }
        }
    }
    ReceiveHostResponses();
    if (C2V(gNumber_of_hosts) <= pIndex) {
        return 0;
    }
    dr_dprintf("PDNetGetNextJoinGame(): Adding game.");
    c2_memcpy(&pGame->pd_net_info.addr, &C2V(gJoinable_games)[pIndex].addr, sizeof(pGame->pd_net_info.addr));
    return 1;
}
C2_HOOK_FUNCTION(0x00519ab0, PDNetGetNextJoinGame)
