#include "sdl3_net.h"

#include "sdl3.h"

#include "errors.h"
#include "globvrpb.h"
#include "network.h"

#include <SDL3/SDL.h>

#ifdef SDL_PLATFORM_WINDOWS
#include <windows.h>
#else
#include <arpa/inet.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/socket.h>
typedef struct {} WSADATA;
#endif

#ifndef INVALID_SOCKET
#define INVALID_SOCKET -1
#endif
#ifndef SOCKET_ERROR
#define SOCKET_ERROR -1
#endif

#define BROADCAST_HEADER "CAR2MSG"
#define JOINABLE_GAMES_CAPACITY 16

int gMsg_header_strlen;
struct sockaddr_in gLocal_address;
struct sockaddr_in gLocal_address_2;
struct sockaddr_in *gPtr_local_address;
struct sockaddr_in gListen_address;
struct sockaddr_in *gPtr_listen_address;
struct sockaddr_in gBroadcast_address;
struct sockaddr_in gLast_received_addr;
struct sockaddr_in *gPtr_broadcast_address;
int gSocket = INVALID_SOCKET;
WSADATA gWSA_data;
char gSend_buffer[512];
char gLocal_address_text[32];
char gReceive_buffer[512];
int gNumber_of_hosts;
int gNumber_of_networks;
tPD_net_game_info *gJoinable_games;


void PDNetObtainSystemUserName(char *pName, int pMax_length) {
#ifdef SDL_PLATFORM_WINDOWS
    char buffer[16];
    DWORD size;

    size = SDL_arraysize(buffer);
    if (!GetComputerNameExAComputerNameDnsHostname, buffer, &size)) {
        SDL_strlcpy(pName, "<unknown>", pMax_length);
    }
    SDL_strlcpy(pName, buffer, pMax_length);
#else
    if (gethostname(pName, pMax_length) != 0) {
        SDL_strlcpy(pName, "<unknown>", pMax_length);
    }
#endif
}

#ifndef SDL_PLATFORM_WINDOWS
#define WSAEWOULDBLOCK EWOULDBLOCK
#define MAKEWORD(A,B) ((A)<<8)|((B)<<0)
static int WSAStartup(Uint16 wVersionRequested, WSADATA *lpWSAData) {
    (void)wVersionRequested;
    (void)lpWSAData;
    return 0;
}

static int WSAGetLastError(void) {
    return errno;
}

static void WSACleanup(void) {
}

static int closesocket(int handle) {
    return close(handle);
}

enum {
    FIONBIO = 44,
};

static int ioctlsocket(int handle, long cmd, void* argp) {
    int flags = fcntl(handle, F_GETFL);
    switch (cmd) {
    case FIONBIO:
        flags |= O_NONBLOCK;
        break;
    default:
        abort();
    }
    return fcntl(handle, F_SETFL, flags);
}
#endif

void NetNowIPXLocalTarget2String(char *pString, struct sockaddr_in *pSock_addr_ipx) {
    char portbuf[10];

    inet_ntop(AF_INET, &pSock_addr_ipx->sin_addr, pString, 32);
    sprintf(portbuf, ":%d", ntohs(pSock_addr_ipx->sin_port));
    strcat(pString, portbuf);
}

int PDNetInitialise(void) {
    struct linger linger;
    int broadcast;
    unsigned int namelen;

    dr_dprintf("PDNetInitialise()");

    SDL_memset(&gLocal_address, 0, sizeof(gLocal_address));
    gPtr_local_address = &gLocal_address;

    SDL_memset(&gListen_address, 0, sizeof(gListen_address));
    gPtr_listen_address = &gListen_address;

    SDL_memset(&gBroadcast_address, 0, sizeof(gListen_address));
    gPtr_broadcast_address = &gBroadcast_address;

    if (WSAStartup(MAKEWORD(1, 1), &gWSA_data) == -1) {
        /* FIXME WSAStartup returns error code */
        dr_dprintf("PDNetInitialise(): WSAStartup() failed");
        return -1;
    }

    gSocket = socket(AF_IPX, SOCK_DGRAM, AF_INET);
    if (gSocket == INVALID_SOCKET) {
        dr_dprintf("PDNetInitialise(): Failed to create socket - WSAGetLastError=%d", WSAGetLastError());
        WSACleanup();
        return -1;
    }
    broadcast = 1;
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

    if (bind(gSocket, (struct sockaddr*)&gLocal_address, sizeof(gLocal_address)) == SOCKET_ERROR) {
        dr_dprintf("Error on bind() - WSAGetLastError=%d", WSAGetLastError());
        closesocket(gSocket);
        WSACleanup();
        return -1;
    }

    namelen = sizeof(gLocal_address);
    getsockname(gSocket, (struct sockaddr* )&gLocal_address, &namelen);
    NetNowIPXLocalTarget2String(gLocal_address_text, gPtr_local_address);

    SDL_memcpy(&gLocal_address_2, gPtr_local_address, sizeof(struct sockaddr_in));
    dr_dprintf("Socket bound OK; local address is '%s'", gLocal_address_text);

    gMsg_header_strlen = SDL_strlen(BROADCAST_HEADER);
    return 0;
}

int PDNetShutdown(void) {

    dr_dprintf("PDNetShutdown()");
    if (gSocket != SOCKET_ERROR) {
        closesocket(gSocket);
    }
    gSocket = SOCKET_ERROR;
    return 0;
}

int PDNetJoinGame(tNet_game_details* pGame_details, const char *pName) {
    dr_dprintf("PDNetJoinGame()");
    return 0;
}

void PDNetHostFinishGame(void) {

    dr_dprintf("PDNetHostFinishGame()");
}

void PDNetLeaveGame(void) {
    dr_dprintf("PDNetLeaveGame()");
}

void MakeMessageToSend(int pMessage_type) {
    sprintf(gSend_buffer, "XXXX%s%.1d", BROADCAST_HEADER, pMessage_type);
}

int GetMessageTypeFromMessage(const char *pMessage_str) {
    const char *real_msg;
    int msg_type_int;

    real_msg = &pMessage_str[4];
    msg_type_int = 0;

    // FIXME: "CW95MSG" value is used in and depends on platform
    if (SDL_strncmp(real_msg, BROADCAST_HEADER, gMsg_header_strlen) == 0) {
        if (SDL_isdigit(real_msg[gMsg_header_strlen])) {
            msg_type_int = real_msg[gMsg_header_strlen] - '0';
        }
        if (msg_type_int != 0 && msg_type_int < 2) {
            return msg_type_int;
        }
    }
    return 999;
}

int SameEthernetAddress(struct sockaddr_in *pAddr_ipx1, struct sockaddr_in *pAddr_ipx2) {
    return SDL_memcmp(pAddr_ipx1, pAddr_ipx2, sizeof(struct sockaddr_in)) == 0;
}

int ReceiveHostResponses(void) {
    char str[256];
    int i;
    int already_registered;
    char addr_string[32];
    unsigned int sa_len;
    int wsa_error;

    sa_len = sizeof(gListen_address);
    while (1) {
        if (recvfrom(gSocket, gReceive_buffer, sizeof(gReceive_buffer), 0, (struct sockaddr *)&gListen_address, &sa_len) == SOCKET_ERROR) {
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
            if (SameEthernetAddress((struct sockaddr_in *)gJoinable_games[i].addr, gPtr_listen_address)) {
                already_registered = 1;
                break;
            }
        }
        if (already_registered) {
            dr_dprintf("That game already registered");
            gJoinable_games[i].last_response = PDGetTotalTime();
        } else {
            dr_dprintf("Adding joinable game to slot #%d", gNumber_of_hosts);
            SDL_memcpy(gJoinable_games[gNumber_of_hosts].addr, gPtr_listen_address, sizeof(struct sockaddr));
            gJoinable_games[gNumber_of_hosts].last_response = PDGetTotalTime();
            gNumber_of_hosts++;
            dr_dprintf("Number of games found so far: %d", gNumber_of_hosts);
        }
        if (gNumber_of_hosts != 0) {
            dr_dprintf("Currently registered net games:");
            for (i = 0; i < gNumber_of_hosts; i++) {
                NetNowIPXLocalTarget2String(str, (struct sockaddr_in *)&gJoinable_games[i].addr);
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

int BroadcastMessage(void) {
    int i;
    int errors;
    char broadcast_addr_string[32];

    errors = 0;
    for (i = 0; i < gNumber_of_networks; i++) {
        NetNowIPXLocalTarget2String(broadcast_addr_string, gPtr_broadcast_address);
        dr_dprintf("Broadcasting on address '%s'", broadcast_addr_string);
        if (sendto(gSocket, gSend_buffer, SDL_strlen(gSend_buffer) + 1, 0, (struct sockaddr*)&gBroadcast_address, sizeof(gBroadcast_address)) == SOCKET_ERROR) {
            dr_dprintf("BroadcastMessage(): Error on sendto() - WSAGetLastError=%d", WSAGetLastError());
            errors = 1;
        }
    }
    return errors == 0;
}

int PDNetGetNextJoinGame(tNet_game_details *pGame, int pIndex) {
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
                        SDL_memmove(&gJoinable_games[j], &gJoinable_games[j + 1], sizeof(tPD_net_game_info));
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
    SDL_memcpy(&pGame->pd_net_info.addr, &gJoinable_games[pIndex].addr, sizeof(pGame->pd_net_info.addr));
    return 1;
}

int PDNetGetHeaderSize(void) {
    return 0;
}

tNet_message *PDNetGetNextMessage(tNet_game_details *pDetails, void **pSender_address) {
    char addr_str[32];
    unsigned int sa_len;
    int res;
    tNet_message* msg;
    char* receive_buffer;

    sa_len = sizeof(gListen_address); /* FIXME: gListen_address -> gRemote_address */
    msg = NetAllocateMessage(512);
    receive_buffer = (char*)msg;
    res = recvfrom(gSocket, receive_buffer, 512, 0, (struct sockaddr *)&gListen_address, &sa_len) != SOCKET_ERROR;
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
                        if (sendto(gSocket, gSend_buffer, SDL_strlen(gSend_buffer) + 1, 0, (struct sockaddr *)&gListen_address, sizeof(gListen_address)) == SOCKET_ERROR) {
                            dr_dprintf("PDNetGetNextMessage(): Error on sendto() - WSAGetLastError=%d", WSAGetLastError());
                        }
                    }
                    break;
                case 2:
                    // no-op
                    break;
                default:
                    dr_dprintf("PDNetGetNextMessage(): res is %d, received message type %d from '%s', passing up", res, msg->contents.raw.header.type, addr_str);
                    SDL_memcpy(&gLast_received_addr, gPtr_listen_address, sizeof(gLast_received_addr));
                    *pSender_address = &gLast_received_addr;
                    return msg;
            }
        }
    }
    msg->header.field_0x14 = 0;
    NetDisposeMessage(pDetails, msg);
    return NULL;
}

int PDNetSendMessageToAddress(tNet_game_details *pDetails, tNet_message *pMessage, void *pAddress) {

    /* FIXME: sizeof(struct sockaddr) == 16 */

    if (sendto(gSocket, (const char*)pMessage, pMessage->header.field_0x16, 0, (const struct sockaddr*)pAddress, sizeof(struct sockaddr)) == SOCKET_ERROR) {
        dr_dprintf("PDNetSendMessageToAddress(): Error on sendto() - WSAGetLastError=%d", WSAGetLastError());
        NetDisposeMessage(pDetails, pMessage);
        return 1;
    }
    NetDisposeMessage(pDetails,(tNet_message *)pMessage);
    return 0;
}

void PDNetStartProducingJoinList(void) {
    dr_dprintf("PDNetStartProducingJoinList()");
    gNumber_of_hosts = 0;
    gJoinable_games = BrMemAllocate(sizeof(tPD_net_game_info) * JOINABLE_GAMES_CAPACITY, BR_MEMORY_APPLICATION);
    if (gJoinable_games == NULL) {
        PDFatalError("Can't allocate memory for joinable games");
    }
}

void PDNetEndJoinList(void) {
    dr_dprintf("PDNetEndJoinList()");
    if (gJoinable_games != NULL) {
        BrMemFree(gJoinable_games);
    }
    gJoinable_games = NULL;
}

tS3_error_codes C2_HOOK_FASTCALL PDS3StartMidiChannel(tS3_channel *pChannel) {
    return eS3_error_memory;
}

int PDS3StopMidiChannel(tS3_channel *pChannel) {
    return 0;
}

int PDS3IsMIDIStopped(tS3_channel *pChannel) {
    return 1;
}

int PDS3StopCDAChannel(tS3_channel *pChannel) {
    return 0;
}

tS3_error_codes C2_HOOK_FASTCALL PDS3PlayCDAChannel(tS3_channel *pChannel) {
    return eS3_error_memory;
}

void PDS3ServiceCDA(void) {
}

void *PDS3CreateSoundBuffer(tS3_wav_info *pWav_info, void *pWav_buffer) {
    return NULL;
}

int PDS3PlaySample(tS3_channel *pChannel) {
    return 0;
}

int PDS3IsSamplePlaying(tS3_channel *pChannel) {
    return 0;
}

void PDS3ReleaseSound(tS3_descriptor *pSound_descriptor) {
}
