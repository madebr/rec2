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
