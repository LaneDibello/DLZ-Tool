#include "KotorManager.h"

#define SWKOTOR_TEST_DWORD 0x00905a4d

int get_running_kotor() {
    if (GetProcessID(KOTOR_1_EXE)) {
        return 1;
    }
    else if (GetProcessID(KOTOR_2_EXE)) {
        return 2;
    }
    return 0;
}

int main(int argc, char* argv[]) {
    
    printf("Waiting for kotor to be opened...\n");
    int version = 0;
    while (!version) {
        version = get_running_kotor();
    }

    KotorManager km(version);
    
    int test_read;
    km.pr->readInt(km.ka->ADDRESS_BASE, &test_read);
    if (test_read != SWKOTOR_TEST_DWORD) {
        printf("Failed Test Read!\nExpected: %x\nGot: %x ", SWKOTOR_TEST_DWORD, test_read);
        exit(1);
    }
    printf("Reading from swkotor.exe initialized...\n");
    
    
    while(true) {
        km.outputAllDoorBasedDLZLinesInArea();
        km.outputAllTriggerBasedDLZLinesInArea();

        float xpc0 = 0.0;
        float xpc1 = 0.0;
        float xpc2 = 0.0;
        uint count = 0;
        printf("\n");
        if (!km.pr->isFailed()) {
            while (
                    km.pr->readFloat(km.getPartyGameObject(0) + km.ka->OFFSET_CSWSOBJECT_X_POS, &xpc0) &&
                    km.pr->readFloat(km.getPartyGameObject(1) + km.ka->OFFSET_CSWSOBJECT_X_POS, &xpc1) &&
                    km.pr->readFloat(km.getPartyGameObject(2) + km.ka->OFFSET_CSWSOBJECT_X_POS, &xpc2) &&
                    km.pr->readUint(km.getParty(), &count) &&
                    !km.pr->isFailed()
                ) {

                printf("\rPC0 %f\tPC1 %f\tPC2 %f\tParty Count: %d\t", xpc0, xpc1, xpc2, count);

            }
        }
        printf("\rUpdating...");
        Sleep(500);
        system("cls");
    }

    return 0;
}
