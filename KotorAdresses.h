#pragma once
#include <windows.h>
#include "types.h"

#define ADDRESS_BASE 0x00400000
#define ADDRESS_APP_MANAGER 0x007a39fc

#define OFFSET_CLIENT 0x4
#define OFFSET_SERVER 0x8
#define OFFSET_INTERNAL 0x4
#define OFFSET_SERVER_GAME_OBJ_ARR 0x1005c
#define OFFSET_CLIENT_PLAYER_ID 0x20
#define OFFSET_AREA_GAME_OBJECT_ARRAY 0x74
#define OFFSET_AREA_GAME_OBJECT_ARRAY_LENGTH 0x78
#define OFFSET_GAME_OBJECT_AREA_ID 0x8c
#define OFFSET_GAME_OBJECT_TYPE 0x8
#define OFFSET_CSWSDOOR_CORNERS 0x350
#define OFFSET_CSWSDOOR_LINKED_TO_FLAGS 0x384
#define OFFSET_CSWSDOOR_LINKED_TO_MODULE 0x390
#define OFFSET_CEXOSTRING_LENGTH 0x4
#define OFFSET_CSWSOBJECT_X_POS 0x90

uint getGOAIndexFromServerID(uint id) {
    return uint(((int)id >> 0x1f) * -0x1000 + (id & 0xfff));
}

uint clientToServerID(uint client_id) {
    return client_id & 0x7fffffff;
}

uint serverToClientID(uint server_id) {
    return server_id | 0x80000000;
}
