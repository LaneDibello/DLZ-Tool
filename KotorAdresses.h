#pragma once
#include <windows.h>
#include "types.h"

#define KOTOR_1_EXE "swkotor.exe"
#define KOTOR_2_EXE "swkotor2.exe"

class KotorAddresses {
public:
    char* KOTOR_EXE;
    ADDR ADDRESS_APP_MANAGER;
    ADDR OFFSET_CSWSOBJECT_AREA_ID; 
    ADDR OFFSET_CSWSDOOR_CORNERS; 
    ADDR OFFSET_CSWSDOOR_LINKED_TO_FLAGS; 
    ADDR OFFSET_CSWSDOOR_LINKED_TO_MODULE; 
    ADDR OFFSET_CSWSOBJECT_X_POS; 
    ADDR OFFSET_CSWSOBJECT_ORIENTATION;
    ADDR OFFSET_CSWSTRIGGER_GEOMETRY_COUNT; 
    ADDR OFFSET_CSWSTRIGGER_GEOMETRY; 

    ADDR ADDRESS_BASE;

    ADDR OFFSET_CLIENT;
    ADDR OFFSET_SERVER;
    ADDR OFFSET_INTERNAL;
    ADDR OFFSET_CSWPARTY;
    ADDR OFFSET_SERVER_GAME_OBJ_ARR;
    ADDR OFFSET_CLIENT_PLAYER_ID;
    ADDR OFFSET_AREA_GAME_OBJECT_ARRAY;
    ADDR OFFSET_AREA_GAME_OBJECT_ARRAY_LENGTH;
    ADDR OFFSET_GAME_OBJECT_TYPE;
    ADDR OFFSET_CEXOSTRING_LENGTH;
    ADDR OFFSET_CSWSOBJECT_TAG;

    ADDR OFFSET_CSWPARTY_PARTY_DATA;
    ADDR SIZE_CSWPARTY_PARTY_DATA;

    KotorAddresses(int version = 1) {
        if (version == 1) {
            KOTOR_EXE = new char[sizeof(KOTOR_1_EXE)];
            strcpy_s(KOTOR_EXE, sizeof(KOTOR_1_EXE), KOTOR_1_EXE);
            
            ADDRESS_APP_MANAGER = 0x007a39fc;
            OFFSET_CSWSOBJECT_AREA_ID = 0x8c;
            OFFSET_CSWSDOOR_CORNERS = 0x350;
            OFFSET_CSWSDOOR_LINKED_TO_FLAGS = 0x384;
            OFFSET_CSWSDOOR_LINKED_TO_MODULE = 0x390;
            OFFSET_CSWSOBJECT_X_POS = 0x90;
            OFFSET_CSWSOBJECT_ORIENTATION = 0x9c;
            OFFSET_CSWSTRIGGER_GEOMETRY_COUNT = 0x284;
            OFFSET_CSWSTRIGGER_GEOMETRY = 0x288;
        }
        else if (version == 2) {
            KOTOR_EXE = new char[sizeof(KOTOR_2_EXE)];
            strcpy_s(KOTOR_EXE, sizeof(KOTOR_2_EXE), KOTOR_2_EXE);
            
            ADDRESS_APP_MANAGER = 0x00a11c04;
            OFFSET_CSWSOBJECT_AREA_ID = 0x90;
            OFFSET_CSWSDOOR_CORNERS = 0x3a0;
            OFFSET_CSWSDOOR_LINKED_TO_FLAGS = 0x3d4;
            OFFSET_CSWSDOOR_LINKED_TO_MODULE = 0x3e0;
            OFFSET_CSWSOBJECT_X_POS = 0x94;
            OFFSET_CSWSTRIGGER_GEOMETRY_COUNT = 0x2c4;
            OFFSET_CSWSTRIGGER_GEOMETRY = 0x2c8;
        }
        else {
            printf("INVALID GAME VERSION %d!\n", version);
            exit(1);
        }

        ADDRESS_BASE = 0x00400000;
        OFFSET_CLIENT = 0x4;
        OFFSET_SERVER = 0x8;
        OFFSET_INTERNAL = 0x4;
        OFFSET_CSWPARTY = 0x270;
        OFFSET_CSWPARTY_PARTY_DATA = 0x24;
        SIZE_CSWPARTY_PARTY_DATA = 0x88;
        OFFSET_SERVER_GAME_OBJ_ARR = 0x1005c;
        OFFSET_CLIENT_PLAYER_ID = 0x20;
        OFFSET_AREA_GAME_OBJECT_ARRAY = 0x74;
        OFFSET_AREA_GAME_OBJECT_ARRAY_LENGTH = 0x78;
        OFFSET_GAME_OBJECT_TYPE = 0x8;
        OFFSET_CEXOSTRING_LENGTH = 0x4;
        OFFSET_CSWSOBJECT_TAG = 0x18;
    }

    void swapK2SteamAddress() {
        ADDRESS_APP_MANAGER = 0x00a1b4a4;
    }

    ~KotorAddresses() {
        delete[] KOTOR_EXE;
    }
};


uint getGOAIndexFromServerID(uint id) {
    return uint(((int)id >> 0x1f) * -0x1000 + (id & 0xfff));
}

uint clientToServerID(uint client_id) {
    return client_id & 0x7fffffff;
}

uint serverToClientID(uint server_id) {
    return server_id | 0x80000000;
}
