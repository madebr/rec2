#include "win32net.h"

#include "win32.h"

#include "52-errors.h"
#include "globvrpb.h"
#include "01-network.h"
#include "platform.h"

#include "c2_string.h"

#include <winsock.h>
#include <wsipx.h>

#define BROADCAST_HEADER "CAR2MSG"
#define JOINABLE_GAMES_CAPACITY 16


// GLOBAL: CARMA2_HW 0x006b3380
char gPathNetworkIni[240];

// GLOBAL: CARMA2_HW 0x006ad4c4
int gPathNetworkIniValid;

// GLOBAL: CARMA2_HW 0x006ac12c
tU16 gIpx_sock_addr;

// GLOBAL: CARMA2_HW 0x006ac310
struct sockaddr gLocal_address;

// GLOBAL: CARMA2_HW 0x006abf08
struct sockaddr gLocal_address_2;

// GLOBAL: CARMA2_HW 0x006ac358
struct sockaddr* gPtr_local_address; /* FIXME: rename to gLocal_addr_ipx? */

// GLOBAL: CARMA2_HW 0x006ac118
struct sockaddr gListen_address;

// GLOBAL: CARMA2_HW 0x006ac340
struct sockaddr* gPtr_listen_address;  /* FIXME: rename to gRemote_addr_ipx? */

// GLOBAL: CARMA2_HW 0x006ac300
struct sockaddr gBroadcast_address;

// GLOBAL: CARMA2_HW 0x006ac348
struct sockaddr gLast_received_addr;

// GLOBAL: CARMA2_HW 0x006abf00
struct sockaddr* gPtr_broadcast_address;

// GLOBAL: CARMA2_HW 0x006ac170
WSADATA gWSA_data;

// GLOBAL: CARMA2_HW 0x006619b0
SOCKET gSocket = SOCKET_ERROR;

// GLOBAL: CARMA2_HW 0x006ac320
char gLocal_address_text[32];

// GLOBAL: CARMA2_HW 0x006ac128
int gNumber_of_networks;

// GLOBAL: CARMA2_HW 0x006abf04
int gMsg_header_strlen;

// GLOBAL: CARMA2_HW 0x006ac578
int gSpecial_server;

// GLOBAL: CARMA2_HW 0x006ac130
char gIpx_network_numbers [16][4];

// GLOBAL: CARMA2_HW 0x006ac360
char gSend_buffer[512];

// GLOBAL: CARMA2_HW 0x006abf18
char gReceive_buffer[512];

// GLOBAL: CARMA2_HW 0x006ac35c
int gNumber_of_hosts;

// GLOBAL: CARMA2_HW 0x006ac574
tPD_net_game_info* gJoinable_games;

// FUNCTION: CARMA2_HW 0x0051a190
void C2_HOOK_FASTCALL PDNetObtainSystemUserName(char* pName, int pMax_length) {
    char buffer[16];
    DWORD size;
    char *p;

    dr_dprintf("PDNetObtainSystemUserName()");

    if (GetComputerNameA(buffer, &size) && size != 0) {
        strncpy(pName, buffer, pMax_length - 1);
        pName[pMax_length - 1] = '\0';
        while ((p = strpbrk(pName, "_=(){}[]<>!$%^&*/:@~;'#,?\\|`\"")) != NULL) {
            *p = '-';
        }
    }
}

// FUNCTION: CARMA2_HW 0x0051a010
void C2_HOOK_FASTCALL NetNowIPXLocalTarget2String(char* pString, struct sockaddr* pSock_addr) {

    sprintf(pString,
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

// FUNCTION: CARMA2_HW 0x00519560
int C2_HOOK_FASTCALL PDNetInitialise(void) {
    char default_null;
    char s[32];
    static u_long broadcast = 1;
    LINGER linger;
    int namelen;

    default_null = '\0';
    dr_dprintf("PDNetInitialise()");
    gIpx_sock_addr = 0xfe2f;
    if (gPathNetworkIniValid && GetPrivateProfileStringA("Network", "IPXSocket", &default_null, s, sizeof(s), gPathNetworkIni) != 0) {
        int ipx_address;
        int count;

        count = sscanf(s, "%x", &ipx_address);
        if (count != 0 && count != -1) {
            dr_dprintf("New IPX socket %4.4x", socket);
            /* byte swap */
            gIpx_sock_addr = (ipx_address << 8) | ((ipx_address >> 8) & 0xff);
        }
    }

    memset(gLocal_address.sa_data, 0, sizeof(gLocal_address.sa_data));
    gLocal_address.sa_family = AF_IPX;
    gLocal_address.sa_data[10] = gIpx_sock_addr & 0xff;
    gLocal_address.sa_data[11] = gIpx_sock_addr >> 8;
    gPtr_local_address = &gLocal_address;

    memset(gListen_address.sa_data, 0, sizeof(gListen_address.sa_data));
    gListen_address.sa_family = AF_IPX;
    gListen_address.sa_data[10] = gIpx_sock_addr & 0xff;
    gListen_address.sa_data[11] = gIpx_sock_addr >> 8;
    gPtr_listen_address = &gListen_address;

    memset(gBroadcast_address.sa_data, 0, sizeof(gListen_address.sa_data));
    gBroadcast_address.sa_family = AF_IPX;
    gBroadcast_address.sa_data[4] = 0xffu;
    gBroadcast_address.sa_data[5] = 0xffu;
    gBroadcast_address.sa_data[6] = 0xffu;
    gBroadcast_address.sa_data[7] = 0xffu;
    gBroadcast_address.sa_data[8] = 0xffu;
    gBroadcast_address.sa_data[9] = 0xffu;
    gBroadcast_address.sa_data[10] = gIpx_sock_addr & 0xff;
    gBroadcast_address.sa_data[11] = gIpx_sock_addr >> 8;
    gPtr_broadcast_address = &gBroadcast_address;

    if (WSAStartup(MAKEWORD(1, 1), &gWSA_data) == -1) {
        /* FIXME WSAStartup returns error code */
        dr_dprintf("PDNetInitialise(): WSAStartup() failed");
        return -1;
    }

    gSocket = socket(AF_IPX, SOCK_DGRAM, NSPROTO_IPX);
    if (gSocket == INVALID_SOCKET) {
        dr_dprintf("PDNetInitialise(): Failed to create socket - WSAGetLastError=%d", WSAGetLastError());
        WSACleanup();
        return -1;
    }
    setsockopt(gSocket, SOL_SOCKET, SO_BROADCAST, (const char*)&broadcast, sizeof(broadcast));

    linger.l_onoff = 1;
    linger.l_linger = 0;
    setsockopt(gSocket, SOL_SOCKET, SO_LINGER, (const char*)&linger, sizeof(linger));

    if (ioctlsocket(gSocket, FIONBIO, &broadcast) == SOCKET_ERROR) {
        dr_dprintf("Error on ioctlsocket() - WSAGetLastError=%d", WSAGetLastError());
        closesocket(gSocket);
        WSACleanup();
        return -1;
    }

    C2_HOOK_BUG_ON(sizeof(SOCKADDR_IPX) != 14);

    if (bind(gSocket, &gLocal_address, sizeof(SOCKADDR_IPX)) == SOCKET_ERROR) {
        dr_dprintf("Error on bind() - WSAGetLastError=%d", WSAGetLastError());
        closesocket(gSocket);
        WSACleanup();
        return -1;
    }

    namelen = sizeof(SOCKADDR_IPX);
    getsockname(gSocket, &gLocal_address, &namelen);
    NetNowIPXLocalTarget2String(gLocal_address_text, gPtr_local_address);

    memcpy(gIpx_network_numbers[0], gPtr_local_address->sa_data, 4);
    gNumber_of_networks = 1;
    if (gPathNetworkIniValid) {
        char key[32];
        int i;

        for (i = 0; i < 16; i++) {
            sprintf(key, "%s%d", "Network", i);
            if (GetPrivateProfileStringA("Network", key, &default_null, s, sizeof(s), gPathNetworkIni) != 0) {
                int count;
                tU32 netnum;

                count = sscanf(s, "%x", &netnum);
                if (count != 0 && count != -1) {
                    dr_dprintf("Found network number '%x' in INI file", netnum);
                    gIpx_network_numbers[gNumber_of_networks][0] = netnum >> 24;
                    gIpx_network_numbers[gNumber_of_networks][1] = netnum >> 16;
                    gIpx_network_numbers[gNumber_of_networks][2] = netnum >> 8;
                    gIpx_network_numbers[gNumber_of_networks][3] = netnum >> 0;
                    if (gIpx_network_numbers[gNumber_of_networks][0] != gPtr_local_address->sa_data[0]
                        || gIpx_network_numbers[gNumber_of_networks][1] != gPtr_local_address->sa_data[1]
                        || gIpx_network_numbers[gNumber_of_networks][2] != gPtr_local_address->sa_data[2]
                        || gIpx_network_numbers[gNumber_of_networks][3] != gPtr_local_address->sa_data[3]) {
                        gNumber_of_networks++;
                    }
                }
            }
        }
        dr_dprintf("Total networks = %d", gNumber_of_networks);
    }
    memcpy(&gLocal_address_2, gPtr_local_address, sizeof(SOCKADDR_IPX));
    dr_dprintf("Socket bound OK; local address is '%s'", gLocal_address_text);

    gMsg_header_strlen = strlen(BROADCAST_HEADER);
    if (strstr(gLocal_address_text, "00a0240f9fac") != NULL) {
        gSpecial_server = 1;
    }
    return 0;
}

// FUNCTION: CARMA2_HW 0x00519a10
int C2_HOOK_FASTCALL PDNetShutdown(void) {

    dr_dprintf("PDNetShutdown()");
    if (gSocket != SOCKET_ERROR) {
        closesocket(gSocket);
    }
    gSocket = SOCKET_ERROR;
    return 0;
}

// FUNCTION: CARMA2_HW 0x00519a40
void C2_HOOK_FASTCALL PDNetStartProducingJoinList(void) {

    C2_HOOK_BUG_ON(sizeof(tPD_net_game_info) * JOINABLE_GAMES_CAPACITY != 0x140);

    dr_dprintf("PDNetStartProducingJoinList()");
    gNumber_of_hosts = 0;
    gJoinable_games = BrMemAllocate(sizeof(tPD_net_game_info) * JOINABLE_GAMES_CAPACITY, BR_MEMORY_APPLICATION);
    if (gJoinable_games == NULL) {
        PDFatalError("Can't allocate memory for joinable games");
    }
}

// FUNCTION: CARMA2_HW 0x00519a80
void C2_HOOK_FASTCALL PDNetEndJoinList(void) {

    dr_dprintf("PDNetEndJoinList()");
    if (gJoinable_games != NULL) {
        BrMemFree(gJoinable_games);
    }
    gJoinable_games = NULL;
}

int C2_HOOK_FASTCALL BroadcastMessage(void) {
    int i;
    int errors;
    char broadcast_addr_string[32];

    errors = 0;
    for (i = 0; i < gNumber_of_networks; i++) {
        memcpy(gPtr_broadcast_address->sa_data, gIpx_network_numbers[0], 4);
        NetNowIPXLocalTarget2String(broadcast_addr_string, gPtr_broadcast_address);
        dr_dprintf("Broadcasting on address '%s'", broadcast_addr_string);
        if (sendto(gSocket, gSend_buffer, strlen(gSend_buffer) + 1, 0, &gBroadcast_address, sizeof(gBroadcast_address)) == SOCKET_ERROR) {
            dr_dprintf("BroadcastMessage(): Error on sendto() - WSAGetLastError=%d", WSAGetLastError());
            errors = 1;
        }
    }
    return errors == 0;
}

void C2_HOOK_FASTCALL MakeMessageToSend(int pMessage_type) {

#if defined(REC2_FIX_BUGS)
    sprintf(gSend_buffer, "XXXX%s%1d", BROADCAST_HEADER, pMessage_type);
#else
    sprintf(gSend_buffer, "XXXX%s%0.1d", BROADCAST_HEADER, pMessage_type);
#endif
}

int C2_HOOK_FASTCALL SameEthernetAddress(struct sockaddr* pAddr_ipx1, struct sockaddr* pAddr_ipx2) {

    return memcmp(&pAddr_ipx1->sa_data[4], &pAddr_ipx2->sa_data[4], 6) == 0;
}

int C2_HOOK_FASTCALL GetMessageTypeFromMessage(char* pMessage_str) {
    char* real_msg;
    int msg_type_int;

    real_msg = &pMessage_str[4];
    msg_type_int = 0;

    // FIXME: "CW95MSG" value is used in and depends on platform
    if (strncmp(real_msg, BROADCAST_HEADER, gMsg_header_strlen) == 0) {
        if (isdigit(real_msg[gMsg_header_strlen])) {
            msg_type_int = real_msg[gMsg_header_strlen] - '0';
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

    C2_HOOK_BUG_ON(sizeof(gReceive_buffer) != 0x200);

    sa_len = sizeof(SOCKADDR_IPX);
    while (1) {
        if (recvfrom(gSocket, gReceive_buffer, sizeof(gReceive_buffer), 0, &gListen_address, &sa_len) == SOCKET_ERROR) {
            break;
        }
        NetNowIPXLocalTarget2String(addr_string, gPtr_listen_address);
        dr_dprintf("ReceiveHostResponses(): Received string '%s' from %s", gReceive_buffer, addr_string);

        if (SameEthernetAddress(gPtr_local_address, gPtr_listen_address)) {
            dr_dprintf("*** Discounting the above 'cos we sent it ***");
            continue;
        }
        if (GetMessageTypeFromMessage(gReceive_buffer) != 2) {
            dr_dprintf("*** Discounting the above 'cos it's not a host reply ***");
            continue;
        }

        dr_dprintf("*** It's a host reply! ***");
        already_registered = 0;
        for (i = 0; i < gNumber_of_hosts; i++) {
            if (SameEthernetAddress((struct sockaddr*)&gJoinable_games[i].addr, gPtr_listen_address)) {
                already_registered = 1;
                break;
            }
        }
        if (already_registered) {
            dr_dprintf("That game already registered");
            gJoinable_games[i].last_response = PDGetTotalTime();
        } else {
            dr_dprintf("Adding joinable game to slot #%d", gNumber_of_hosts);
            memcpy(gJoinable_games[gNumber_of_hosts].addr, gPtr_listen_address, sizeof(struct sockaddr));
            gJoinable_games[gNumber_of_hosts].last_response = PDGetTotalTime();
            gNumber_of_hosts++;
            dr_dprintf("Number of games found so far: %d", gNumber_of_hosts);
        }
        if (gNumber_of_hosts != 0) {
            dr_dprintf("Currently registered net games:");
            for (i = 0; i < gNumber_of_hosts; i++) {
                NetNowIPXLocalTarget2String(str, (struct sockaddr*)&gJoinable_games[i].addr);
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

// FUNCTION: CARMA2_HW 0x00519ab0
int C2_HOOK_FASTCALL PDNetGetNextJoinGame(tNet_game_details* pGame, int pIndex) {
    static tU32 next_broadcast_time = 0;
    int i;
    int j;
    int number_of_hosts_has_changed;

    dr_dprintf("PDNetGetNextJoinGame(): pIndex is %d", pIndex);
    if (pIndex == 0) {
        do {
            number_of_hosts_has_changed = 0;
            for (i = 0; i < gNumber_of_hosts; i++) {
                if (gJoinable_games[i].last_response + 10000 < (tU32)PDGetTotalTime()) {
                    number_of_hosts_has_changed = 1;
                    for (j = i; j < gNumber_of_hosts - 1; j++) {
                        memmove(&gJoinable_games[j], &gJoinable_games[j + 1], sizeof(tPD_net_game_info));
                    }
                }
            }
            if (number_of_hosts_has_changed) {
                gNumber_of_hosts--;
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
    if (gNumber_of_hosts <= pIndex) {
        return 0;
    }
    dr_dprintf("PDNetGetNextJoinGame(): Adding game.");
    memcpy(&pGame->pd_net_info.addr, &gJoinable_games[pIndex].addr, sizeof(pGame->pd_net_info.addr));
    return 1;
}

// FUNCTION: CARMA2_HW 0x0051a6a0
int C2_HOOK_FASTCALL PDNetGetHeaderSize(void) {

    return 0;
}

// FUNCTION: CARMA2_HW 0x0051a0a0
int C2_HOOK_FASTCALL PDNetJoinGame(tNet_game_details* pGame_details, const char* pName) {

    dr_dprintf("PDNetJoinGame()");
    return 0;
}

// FUNCTION: CARMA2_HW 0x0051a0c0
void C2_HOOK_FASTCALL PDNetHostFinishGame(void) {

    dr_dprintf("PDNetHostFinishGame()");
}

// FUNCTION: CARMA2_HW 0x0051a0b0
void C2_HOOK_FASTCALL PDNetLeaveGame(void) {

    dr_dprintf("PDNetLeaveGame()");
}

// FUNCTION: CARMA2_HW 0x0051a380
tNet_message* C2_HOOK_FASTCALL PDNetGetNextMessage(tNet_game_details* pDetails, void** pSender_address) {
    char addr_str[32];
    int sa_len;
    int res;
    tNet_message* msg;
    char* receive_buffer;

    sa_len = 14;  /* FIXME: sizeof(gListen_address) == 16 */
    msg = NetAllocateMessage(512);
    receive_buffer = (char*)msg;
    res = recvfrom(gSocket, receive_buffer, 512, 0, &gListen_address, &sa_len) != SOCKET_ERROR;
    if (res == SOCKET_ERROR) {
        WSAGetLastError();
    } else {
        NetNowIPXLocalTarget2String(addr_str, gPtr_listen_address);
        if (!SameEthernetAddress(gPtr_local_address, gPtr_listen_address)) {
            int msg_type;

            msg_type = GetMessageTypeFromMessage(receive_buffer);
            switch (msg_type) {
            case 1:
                if (gNet_mode == eNet_mode_host) {
                    dr_dprintf("PDNetGetNextMessage(): Received '%s' from '%s', replying to joiner", receive_buffer, addr_str);
                    MakeMessageToSend(2);
                    if (sendto(gSocket, gSend_buffer, strlen(gSend_buffer) + 1, 0, &gListen_address, sizeof(gListen_address)) == SOCKET_ERROR) {
                        dr_dprintf("PDNetGetNextMessage(): Error on sendto() - WSAGetLastError=%d", WSAGetLastError());
                    }
                }
                break;
            case 2:
                // no-op
                break;
            default:
                dr_dprintf("PDNetGetNextMessage(): res is %d, received message type %d from '%s', passing up", res, msg->contents.raw.header.type, addr_str);
                memcpy(&gLast_received_addr, gPtr_listen_address, sizeof(gLast_received_addr));
                *pSender_address = &gLast_received_addr;
                return msg;
            }
        }
    }
    msg->header.field_0x14 = 0;
    NetDisposeMessage(pDetails, msg);
    return NULL;
}

// FUNCTION: CARMA2_HW 0x0051a630
int C2_HOOK_FASTCALL PDNetSendMessageToAddress(tNet_game_details* pDetails, tNet_message* pMessage, void* pAddress) {

    /* FIXME: sizeof(struct sockaddr) == 16 */

    if (sendto(gSocket, (const char*)pMessage, pMessage->header.field_0x16, 0, pAddress, 14) == SOCKET_ERROR) {
        dr_dprintf("PDNetSendMessageToAddress(): Error on sendto() - WSAGetLastError=%d", WSAGetLastError());
        NetDisposeMessage(pDetails, pMessage);
        return 1;
    }
    NetDisposeMessage(pDetails,(tNet_message *)pMessage);
    return 0;
}
