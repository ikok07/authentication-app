#include <stdio.h>
#include <sodium.h>
#include <jwt.h>

#include "../include/auth.h"
#include "../include/utils.h"
#include "../include/user_details.h"

void executeOption(int option) {
    switch (option) {
        case 1:
            if (u_details == NULL) {
                int result = create_user_details(NULL);
                if (result != 0) {
                    perror("Failed to create details");
                    return;
                }
            } else if (update_user_details(NULL) != 0) {
                perror("Failed to update details");
                return;
            }
            break;
        case 2:
            if (has_details != 0) {
                logout();
                return;
            }

            printf("------ USER DETAILS ------\n");
            user_details_t *user_details;
            int result = fetch_user_details(&user_details);
            if (result != 0) {
                perror("Failed to fetch user details");
                return;
            }
            printf("_id: %s\n", user_details->_id);
            printf("user_id: %s\n", user_details->userId);
            printf("gender: %s\n", user_details->gender);
            printf("age: %d\n\n", user_details->age);
            free(user_details);
            break;
        case 3:
            if (delete_user_details() != 0) {
                perror("Failed to delete user details");
                return;
            }
            break;
        case 4:
            logout();
            break;
        case -1:
            exit(0);
        default:
            fprintf(stderr, "Invalid option selected!");
    }
}

void listOptions() {
    int option = 0;
    do {
        authenticate();
        printf("Select option:\n");
        if (has_details == 0) printf("1. Update user details\n");
        else printf("1. Create user details\n");
        if (has_details != 0) printf("2. Log out\n");
        if (has_details == 0) printf("2. Display user details\n");
        if (has_details == 0) printf("3. Delete user details\n");
        if (has_details == 0) printf("4. Log out\n");
        printf("-1. Exit\n");
        scanf("%d", &option);
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

    listOptions();
    return 0;
}
