#pragma once
#include "ProcessReader.h"
#include "KotorAdresses.h"
#include <vector>

#define KOTOR_EXE "swkotor.exe"

class KotorManager {
public:
    ProcessReader * pr;
    ADDR app_manager;
    ADDR client_internal;
    ADDR server_internal;
    ADDR server_game_object_array;

    void refreshAddresses() {
        if (pr)
            delete pr;
        pr = new ProcessReader(KOTOR_EXE);

        ADDR temp;

        pr->readUint(ADDRESS_APP_MANAGER, &app_manager);

        pr->readUint((app_manager + OFFSET_CLIENT), &temp);
        pr->readUint((temp + OFFSET_INTERNAL), &client_internal);

        pr->readUint((app_manager + OFFSET_SERVER), &temp);
        pr->readUint((temp + OFFSET_INTERNAL), &server_internal);

        pr->readUint((server_internal + OFFSET_SERVER_GAME_OBJ_ARR), &temp);
        pr->readUint(temp, &server_game_object_array);
    }

    KotorManager() {
        pr = nullptr;
        refreshAddresses();
    }

    ~KotorManager() {
        if (pr)
            delete pr;
        pr = nullptr;
    }

    uint getClientPlayerID() {
        if(pr->isFailed()) {
            refreshAddresses();
            pr->clearFailed();
        }
        
        uint client_player_id;
        pr->readUint((client_internal + OFFSET_CLIENT_PLAYER_ID), &client_player_id);
        return client_player_id;
    }

    ADDR getGameObjectByServerID(uint server_id) {
        if(pr->isFailed()) {
            refreshAddresses();
            pr->clearFailed();
        }

        uint goa_ptr;
        GameObjectEntry goa_entry;
        uint goa_index = getGOAIndexFromServerID(server_id);
        uint goa_offset = goa_index * sizeof(uint);
        pr->readUint(server_game_object_array + goa_offset, &goa_ptr);
        pr->readGameObjectEntry(goa_ptr, &goa_entry);
        return goa_entry.game_object_pointer;
    }

    ADDR getGameObjectByClientID(uint client_id) {
        return getGameObjectByServerID(clientToServerID(client_id));
    }

    void readCExoString(ADDR address, char * out){
        uint length;
        ADDR char_arr;
        pr->readUint(address + OFFSET_CEXOSTRING_LENGTH, &length);
        pr->readUint(address, &char_arr);
        pr->readChars(char_arr, out, length);
        out[length] = '\0';
    }

    ADDR getPlayerGameObject() {
        if(pr->isFailed()) {
            refreshAddresses();
            pr->clearFailed();
        }

        return getGameObjectByClientID(getClientPlayerID());
    }

    ADDR getAreaGameObject() {
        if(pr->isFailed()) {
            refreshAddresses();
            pr->clearFailed();
        }

        uint area_id;
        pr->readUint((getPlayerGameObject() + OFFSET_GAME_OBJECT_AREA_ID), &area_id);

        ADDR area_game_obj_ptr = getGameObjectByServerID(area_id);
        GAME_OBJECT_TYPES area_go_type;
        pr->readByte((area_game_obj_ptr + OFFSET_GAME_OBJECT_TYPE), (byte *)&area_go_type);
        if (area_go_type != AREA){
            printf("Got Bad Area Game Object!");
            pr->setFailed();
            return 0x0;
        }
        return area_game_obj_ptr;
    }

    void getAllObjectsInArea(uint * & out, uint& count) {
        if(pr->isFailed()) {
            refreshAddresses();
            pr->clearFailed();
        }

        ADDR area_game_obj_ptr = getAreaGameObject();
        
        ADDR area_obj_array_ptr;
        uint area_obj_array_length;
        pr->readUint((area_game_obj_ptr + OFFSET_AREA_GAME_OBJECT_ARRAY), &area_obj_array_ptr);
        pr->readUint((area_game_obj_ptr + OFFSET_AREA_GAME_OBJECT_ARRAY_LENGTH), &area_obj_array_length);

        out = new uint[area_obj_array_length];
        count = area_obj_array_length;
        pr->readUints(area_obj_array_ptr, out, area_obj_array_length);
    }

    void getAllDoorsWithTransitionInArea(std::vector<ADDR>& doors) {
        if(pr->isFailed()) {
            refreshAddresses();
            pr->clearFailed();
        }

        uint * area_objects = nullptr;
        uint area_objects_length; 
        getAllObjectsInArea(area_objects, area_objects_length);
        for (size_t i = 0; i < area_objects_length; ++i)
        {
            ADDR game_object = getGameObjectByServerID(area_objects[i]);
            GAME_OBJECT_TYPES obj_type;
            pr->readByte((game_object + OFFSET_GAME_OBJECT_TYPE), (byte *)&obj_type);
            if (obj_type == DOOR){
                byte linked_to_flags;
                pr->readByte((game_object + OFFSET_CSWSDOOR_LINKED_TO_FLAGS), &linked_to_flags);
                if (linked_to_flags)
                    doors.push_back(game_object);
            }
        }
    }

    void outputAllDoorBasedDLZLinesInArea() {
        if(pr->isFailed()) {
            refreshAddresses();
            pr->clearFailed();
        }
        
        std::vector<ADDR> doors;
        getAllDoorsWithTransitionInArea(doors);
        printf("Found %d door(s) with transitions\nTheir x-coords are:\n", doors.size());
        for (size_t i = 0; i < doors.size(); ++i)
        {
            char temp[100];
            GameVector corners[4];
            readCExoString(doors[i] + OFFSET_CSWSDOOR_LINKED_TO_MODULE, temp);
            pr->readVectors(doors[i] + OFFSET_CSWSDOOR_CORNERS, corners, 4);
            printf("\t%s\n", temp);
            printf("\t\t%f\t%f\t%f\t%f\n", corners[0].x, corners[1].x, corners[2].x, corners[3].x);
        }
    }
};