#pragma once
#include "ProcessReader.h"
#include "KotorAdresses.h"
#include <vector>

#define K2_STEAM_MODULE_SIZE 7049216
#define K2_GOG_MODULE_SIZE 7012352

class KotorManager {
public:
    ProcessReader * pr;
    ADDR app_manager;
    ADDR client_internal;
    ADDR server_internal;
    ADDR server_game_object_array;
    KotorAddresses * ka;
    int version;

    void refreshAddresses() {
        ka = new KotorAddresses(version);
        if (pr)
            delete pr;
        pr = new ProcessReader(ka->KOTOR_EXE);

        if (version == 2) {
            if (pr->getModuleSize() == K2_STEAM_MODULE_SIZE) {
                ka->swapK2SteamAddress();
            }
        }

        ADDR temp;

        pr->readUint(ka->ADDRESS_APP_MANAGER, &app_manager);

        pr->readUint((app_manager + ka->OFFSET_CLIENT), &temp);
        pr->readUint((temp + ka->OFFSET_INTERNAL), &client_internal);

        pr->readUint((app_manager + ka->OFFSET_SERVER), &temp);
        pr->readUint((temp + ka->OFFSET_INTERNAL), &server_internal);

        pr->readUint((server_internal + ka->OFFSET_SERVER_GAME_OBJ_ARR), &temp);
        pr->readUint(temp, &server_game_object_array);
    }

    KotorManager(int version = 1) {
        pr = nullptr;
        this->version = version;
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
        pr->readUint((client_internal + ka->OFFSET_CLIENT_PLAYER_ID), &client_player_id);
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
        pr->readUint(address + ka->OFFSET_CEXOSTRING_LENGTH, &length);
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
        pr->readUint((getPlayerGameObject() + ka->OFFSET_CSWSOBJECT_AREA_ID), &area_id);

        ADDR area_game_obj_ptr = getGameObjectByServerID(area_id);
        GAME_OBJECT_TYPES area_go_type;
        pr->readByte((area_game_obj_ptr + ka->OFFSET_GAME_OBJECT_TYPE), (byte *)&area_go_type);
        if (area_go_type != AREA){
            printf("Not In Module...\n");
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
        if (!area_game_obj_ptr)
            return;
        
        ADDR area_obj_array_ptr;
        uint area_obj_array_length;
        pr->readUint((area_game_obj_ptr + ka->OFFSET_AREA_GAME_OBJECT_ARRAY), &area_obj_array_ptr);
        pr->readUint((area_game_obj_ptr + ka->OFFSET_AREA_GAME_OBJECT_ARRAY_LENGTH), &area_obj_array_length);

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
        uint area_objects_length = 0; 
        getAllObjectsInArea(area_objects, area_objects_length);
        for (size_t i = 0; i < area_objects_length; ++i)
        {
            ADDR game_object = getGameObjectByServerID(area_objects[i]);
            GAME_OBJECT_TYPES obj_type;
            pr->readByte((game_object + ka->OFFSET_GAME_OBJECT_TYPE), (byte *)&obj_type);
            if (obj_type == DOOR){
                byte linked_to_flags;
                pr->readByte((game_object + ka->OFFSET_CSWSDOOR_LINKED_TO_FLAGS), &linked_to_flags);
                if (linked_to_flags)
                    doors.push_back(game_object);
            }
        }
    }

    void getAllTriggersInArea(std::vector<ADDR>& triggers) {
        if (pr->isFailed()) {
            refreshAddresses();
            pr->clearFailed();
        }

        uint* area_objects = nullptr;
        uint area_objects_length = 0;
        getAllObjectsInArea(area_objects, area_objects_length);
        for (size_t i = 0; i < area_objects_length; ++i)
        {
            ADDR game_object = getGameObjectByServerID(area_objects[i]);
            GAME_OBJECT_TYPES obj_type;
            pr->readByte((game_object + ka->OFFSET_GAME_OBJECT_TYPE), (byte*)&obj_type);
            if (obj_type == TRIGGER) {
                triggers.push_back(game_object);
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
        printf("Found %zu door(s) with transitions\nTheir x-coords are:\n", doors.size());
        for (size_t i = 0; i < doors.size(); ++i)
        {
            char temp[100];
            GameVector corners[4];
            readCExoString(doors[i] + ka->OFFSET_CSWSDOOR_LINKED_TO_MODULE, temp);
            pr->readVectors(doors[i] + ka->OFFSET_CSWSDOOR_CORNERS, corners, 4);
            printf("\t%s\n", temp);
            printf("\t\t%f\t%f\t%f\t%f\n", corners[0].x, corners[1].x, corners[2].x, corners[3].x);
        }
    }

    void outputAllTriggerBasedDLZLinesInArea() {
        if (pr->isFailed()) {
            refreshAddresses();
            pr->clearFailed();
        }

        std::vector<ADDR> triggers;
        getAllTriggersInArea(triggers);
        printf("Found %zu trigger(s)\nTheir x-coords are:\n", triggers.size());
        for (size_t i = 0; i < triggers.size(); ++i)
        {
            char temp[100];
            uint geometry_count;
            pr->readUint(triggers[i] + ka->OFFSET_CSWSTRIGGER_GEOMETRY_COUNT, &geometry_count);
            GameVector * geometry = new GameVector[geometry_count];
            ADDR geo_ptr;
            pr->readUint(triggers[i] + ka->OFFSET_CSWSTRIGGER_GEOMETRY, &geo_ptr);
            pr->readVectors(geo_ptr, geometry, geometry_count);
            readCExoString(triggers[i] + ka->OFFSET_CSWSOBJECT_TAG, temp);
            printf("\t%s\n\t", temp);
            for (size_t j = 0; j < geometry_count; ++j) {
                printf("\t%f", geometry[j].x);
            }
            printf("\n");
        }
    }
};