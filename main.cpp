#include "KotorManager.h"

#define SWKOTOR_TEST_DWORD 0x00905a4d

int main(int argc, char* argv[]) {

    KotorManager km;
    
    int test_read;
    km.pr->readInt(ADDRESS_BASE, &test_read);
    if (test_read != SWKOTOR_TEST_DWORD) {
        printf("Failed Test Read!\nExpected: %x\nGot: %x ", SWKOTOR_TEST_DWORD, test_read);
        exit(1);
    }
    printf("Reading from swkotor.exe initialized...\n");

    while(true) {
        km.outputAllDoorBasedDLZLinesInArea();

        float x = 0.0;
        printf("\n");
        while(km.pr->readFloat(km.getPlayerGameObject() + OFFSET_CSWSOBJECT_X_POS, &x) && !km.pr->isFailed()){
            printf("\r%f", x);
        }
        printf("\rUpdating...");
        Sleep(5000);
        system("cls");
    }

    return 0;
}
