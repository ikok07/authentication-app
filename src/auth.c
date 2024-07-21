//
// Created by Kok_PC on 21.7.2024 г..
//

#include "../include/auth.h"

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <cjson/cJSON.h>
#include <sodium/crypto_secretbox.h>

#include "../include/http_handler.h"
#include "../include/auth_validator.h"
#include "../include/encryption.h"

char *retrieve_credentials() {
    FILE *fp = fopen("../config/auth.bin", "rb");
    if (fp == NULL) {
        perror("Failed to retrieve credentials!");
        return NULL;
    }

    einfo_t *enc_info = malloc(sizeof(einfo_t));
    fread(&enc_info->encrypted_size, sizeof(size_t), 1, fp);
    fread(&enc_info->nonce, crypto_secretbox_NONCEBYTES, 1, fp);
    enc_info->encrypted_msg = malloc(enc_info->encrypted_size);
    fread(enc_info->encrypted_msg, enc_info->encrypted_size, 1, fp);

    char *credentials = malloc(enc_info->encrypted_size - crypto_secretbox_MACBYTES);
    int result = decrypt_text(credentials, *enc_info);

    if (result != 0) {
        perror("Failed to retrieve credentials!");
        free(credentials);
        free(enc_info->encrypted_msg);
        free(enc_info);
        return NULL;
    }

    fclose(fp);
    free(enc_info->encrypted_msg);
    free(enc_info);
    return credentials;
}

bool save_credentials(char *token) {
    einfo_t *enc_info = malloc(sizeof(einfo_t));
    const int result = encrypt_text(enc_info, token);
    if (result != 0) return false;

    FILE *fp = fopen("../config/auth.bin", "wb");
    if (fp == NULL) {
        perror("Failed to save credentials!");
        return false;
    }

    fwrite(&enc_info->encrypted_size, sizeof(size_t), 1, fp);
    fwrite(enc_info->nonce, crypto_secretbox_NONCEBYTES, 1 ,fp);
    fwrite(enc_info->encrypted_msg, enc_info->encrypted_size, 1, fp);

    free(enc_info->encrypted_msg);
    free(enc_info);
    fclose(fp);
    return true;
}

bool login() {
    char email[100];
    char password[100];
    printf("Enter your email: ");
    scanf_s("%s", email);
    printf("Enter your password: ");
    scanf_s("%s", password);

    // make api request to login
    return false;
}

bool signup() {
    char username[100];
    char email[100];
    char password[100];
    char confirm_password[100];
    printf("Enter your username (max 100 characters)\n");
    scanf_s(" %[^\n]%*c", username, 100);
    printf("Enter your email\n");
    scanf_s(" %s", email);

    char *passwd_validation;
    do {
        printf("Enter your password (6 - 100 characters)\n");
        scanf_s(" %s", password);
        passwd_validation = validate_pass(password);
    } while (passwd_validation != NULL);

    do {
        printf("Confirm your password:\n");
        scanf_s(" %s", confirm_password);
        if (strcmp(confirm_password, password) != 0) printf("Passwords don't match!\n");
    } while (strcmp(confirm_password, password) != 0);

    // make api request to sign up
    cJSON *json = cJSON_CreateObject();
    cJSON_AddStringToObject(json, "username", username);
    cJSON_AddStringToObject(json, "email", email);
    cJSON_AddStringToObject(json, "password", password);
    cJSON_AddStringToObject(json, "confirmPassword", confirm_password);
    char *json_str = cJSON_Print(json);
    char *response = malloc(1);
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

    if (!save_credentials(token->valuestring)) {
        perror("Sign Up failed. Please try again!");
        return false;
    }

    return true;
}

void authenticate() {
    char *token = retrieve_credentials();
    printf("%s", token);
    if (token != NULL) return;

    while (token == NULL) {
        int option;
        printf("Enter or create your account:\n");
        printf("1. Log in\n");
        printf("2. Sign up\n");
        printf("3. Exit\n");
        scanf_s("%d", &option);

        switch (option) {
            case 1:
                if (login()) token = retrieve_credentials();
                break;
            case 2:
                if (signup()) token = retrieve_credentials();
                break;
            case 3:
                exit(1);
            default:
                printf("Invalid option selected!");
        }
    }
    printf("%s", token);
    printf("Successfully entered in your account!");
}
