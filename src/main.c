#include <stdio.h>
#include <sodium.h>
#include <string.h>

#include "../include/auth.h"
#include "../include/utils.h"
#include "../include/encryption.h"
#include "../include/user_details.h"


void executeOption(int option, user_details_t *details) {

}

void listOptions() {
    user_details_t *details;
    int has_details = fetch_user_details(&details);

    int option = 0;
    do {
        printf("Select option:\n");
        if (has_details == 0) printf("1. Update user details\n");
        else printf("1. Create user details\n ");
        printf("2. Display user details\n");
        printf("-1. Exit\n");
        scanf("%d", &option);
        executeOption(option, details);
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
