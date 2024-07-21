//
// Created by Kok_PC on 21.7.2024 г..
//

#include "../include/auth.h"

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <cjson/cJSON.h>

#include "../include/http_handler.h"
#include "../include/auth_validator.h"

bool save_credentials(char *token) {

}

bool login() {
    char email[100];
    char password[100];
    printf("Enter your email: ");
    scanf_s("%s", email);
    printf("Enter your password: ");
    scanf_s("%s", password);

    // make api request to login
}

bool signup() {
    char username[100];
    char email[100];
    char password[100];
    char confirm_password[100];
    printf("Enter your username (max 100 characters): ");
    scanf_s("%s", username);
    printf("Enter your email: ");
    scanf_s("%s", email);

    char *passwd_validation;
    do {
        printf("Enter your password (6 - 100 characters): ");
        scanf_s("%s", password);
        passwd_validation = validate_pass(password);
    } while (passwd_validation != NULL);

    do {
        printf("Confirm your password: ");
        scanf_s("%s", confirm_password);
        if (strcmp(confirm_password, password) != 0) printf("Passwords don't match!");
    } while (strcmp(confirm_password, password) != 0);

    // make api request to sign up
    cJSON *json = cJSON_CreateObject();
    cJSON_AddStringToObject(json, "username", username);
    cJSON_AddStringToObject(json, "email", email);
    cJSON_AddStringToObject(json, "password", password);
    cJSON_AddStringToObject(json, "confirmPassword", confirm_password);
    char *json_str = cJSON_Print(json);
    char *response;
    int result = make_request(
        "http://localhost:8080/api/v1/user/signup",
        POST,
        json_str,
        NULL,
        0,
        NULL,
        0,
        &response
        );
    if (result != 0) return false;

    cJSON *json_response = cJSON_Parse(response);
    if (json_response == NULL) {
        perror("Sign Up failed. Please try again!");
        return false;
    }
    cJSON *token = cJSON_GetObjectItemCaseSensitive(json_response, "token");
    if (token == NULL || token->valuestring == NULL) {
        perror("Sign Up failed. Please try again!");
        return false;
    }

    return save_credentials(token->valuestring);
}

bool authenticate() {
    bool isAuth = false;

    // check if user is authenticated and modify variable
    if (isAuth) return true;

    while (!isAuth) {
        int option;
        printf("Enter or create your account:\n");
        printf("1. Log in\n");
        printf("2. Sign up\n");
        printf("3. Exit\n");
        scanf_s("%d", &option);

        switch (option) {
            case 1:
                isAuth = login();
            case 2:
                isAuth = signup();
            case 3:
                exit(1);
            default:
                printf("Invalid option selected!");
        }
    }

}
