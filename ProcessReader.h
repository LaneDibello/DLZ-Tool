#pragma once
#include "GetProcessID.h"
#include "types.h"
#include <Psapi.h>

// Allows Access to the internal memory of `process_name`
class ProcessReader {
    int pid;
    HANDLE h;
    bool failed = false;
    
public:
    uint getModuleSize() {
        HMODULE modules[20];
        DWORD count;
        MODULEINFO info;
        if (!EnumProcessModules(h, modules, 20, &count)) {
            printf("failed to enum process modules with error: %d", GetLastError());
        }
        GetModuleInformation(h, modules[0], &info, sizeof(MODULEINFO));
        
        return info.SizeOfImage;
    }
    
    bool isFailed(){
        return failed;
    }

    void clearFailed(){
        failed = false;
    }

    void setFailed(){
        failed = true;
    }
    
    ProcessReader(const char * process_name) {
        pid = GetProcessID(process_name);

        h = OpenProcess(PROCESS_VM_READ | PROCESS_VM_WRITE | PROCESS_QUERY_INFORMATION, false, pid);

        if (h == NULL) {
            printf("ProcessReader: Failed OpenProcess with Error: %d", GetLastError());
            exit(1);
        }
    }

    void setLSB(ADDR address) {
        int value;

        // Read the current value at the address
        if (!readInt(address, &value)) {
            printf("Failed to read memory at address %X\n", address);
            return;
        }

        // Set the LSB to 0
        value &= ~1;

        // Write the modified value back to memory
        if (!WriteProcessMemory(h, (LPVOID)address, &value, sizeof(int), NULL)) {
            printf("Failed WriteProcessMemory with Error: %d\n", GetLastError());
            failed = true;
        }
    }

    bool readInt(ADDR address, int * out) {
        if (!ReadProcessMemory(h, (LPVOID)address, out, sizeof(int), NULL)) {
            // printf("Failed ReadProcessMemory with Error: %d", GetLastError());
            failed = true;
            return false;
        }
        return true;
    }

    bool readUint(ADDR address, uint * out) {
        return readUints(address, out, 1);
    }

    bool readUints(ADDR address, uint * out, size_t count) {
        if (!ReadProcessMemory(h, (LPVOID)address, out, sizeof(uint) * count, NULL)) {
            //printf("Failed ReadProcessMemory with Error: %d", GetLastError());
            failed = true;
            return false;
        }
        return true;
    }

    bool readFloat(ADDR address, float * out){
        return readFloats(address, out, 1);
    }

    bool readFloats(ADDR address, float * out, size_t count) {
        if (!ReadProcessMemory(h, (LPVOID)address, out, sizeof(float) * count, NULL)) {
            // printf("Failed ReadProcessMemory with Error: %d", GetLastError());
            failed = true;
            return false;
        }
        return true;
    }

    bool readVector(ADDR address, GameVector * out) {
        return readVectors(address, out, 1);
    }

    bool readVectors(ADDR address, GameVector * out, size_t count) {
        if (!ReadProcessMemory(h, (LPVOID)address, out, sizeof(GameVector) * count, NULL)) {
            // printf("Failed ReadProcessMemory with Error: %d", GetLastError());
            failed = true;
            return false;
        }
        return true;
    }

    bool readChar(ADDR address, char * out) {
        return readChars(address, out, 1);
    }

    bool readChars(ADDR address, char * out, uint count) {
        if (!ReadProcessMemory(h, (LPVOID)address, out, sizeof(char) * count, NULL)) {
            // printf("Failed ReadProcessMemory with Error: %d", GetLastError());
            failed = true;
            return false;
        }
        return true;
    }

    bool readByte(ADDR address, byte * out) {
        return readBytes(address, out, 1);
    }

    bool readBytes(ADDR address, byte * out, size_t count) {
        if (!ReadProcessMemory(h, (LPVOID)address, out, sizeof(byte) * count, NULL)) {
            // printf("Failed ReadProcessMemory with Error: %d", GetLastError());
            failed = true;
            return false;
        }
        return true;
    }

    bool readCString(ADDR address, char * out, size_t capacity) {
        char c = 1;
        ADDR a = address;
        for (int i = 0; i < capacity && c != '\0'; i++) {
            readChar(a + i, &c);
            out[i] = c;
        }
        out[capacity - 1] = '\0';
        return true;
    }

    bool readGameObjectEntry(ADDR address, GameObjectEntry * out) {
        if (!ReadProcessMemory(h, (LPVOID)address, out, sizeof(GameObjectEntry), NULL)) {
            // printf("Failed ReadProcessMemory with Error: %d", GetLastError());
            failed = true;
            return false;
        }
        return true;
    }
};