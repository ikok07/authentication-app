#include <stdio.h>
#include <sodium.h>
#include <string.h>

#include "../include/auth.h"
#include "../include/utils.h"
#include "../include/encryption.h"


void executeOption(int option) {

}

void listOptions() {
    int option = 0;
    do {

        executeOption(option);
    } while (option >= 0);

}


int main(void) {
    setbuf(stdout, NULL);
    loadEnvFile();
    if (sodium_init() < 0) {
        perror("Failed to load sodium library!");
        return 1;
    }
    printf("Welcome to the Clothes App\n");

    authenticate();

    listOptions();

    return 0;
}
