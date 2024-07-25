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
#include <sodium/utils.h>

#include "../include/http_handler.h"
#include "../include/auth_validator.h"
#include "../include/encryption.h"
#include "../include/user_details.h"

char *retrieve_credentials(bool ignore_err) {
    FILE *fp = fopen("../config/auth.bin", "rb");
    if (fp == NULL) {
        if (!ignore_err) perror("Failed to retrieve credentials!");
        return NULL;
    }

    einfo_t *enc_info = malloc(sizeof(einfo_t));
    if (enc_info == NULL) {
        if (!ignore_err) perror("Failed to retrieve credentials!");
        fclose(fp);
        return NULL;
    }

    fread(&enc_info->encrypted_size, sizeof(size_t), 1, fp);
    fread(enc_info->nonce, sizeof(unsigned char), crypto_secretbox_NONCEBYTES, fp);
    fread(enc_info->key, sizeof(unsigned char), crypto_secretbox_KEYBYTES, fp);
    enc_info->encrypted_msg = malloc(enc_info->encrypted_size * sizeof(unsigned char));
    fread(enc_info->encrypted_msg, sizeof(unsigned char), enc_info->encrypted_size, fp);

    char *credentials = malloc(enc_info->encrypted_size + 1);
    if (credentials == NULL) {
        free(enc_info->encrypted_msg);
        free(enc_info);
        fclose(fp);
        return NULL;
    }
    int result = decrypt_text(credentials, enc_info);

    if (result != 0) {
        if (!ignore_err) perror("Failed to retrieve credentials!");
        sodium_memzero(credentials, enc_info->encrypted_size);
        free(credentials);
        free(enc_info->encrypted_msg);
        free(enc_info);
        fclose(fp);
        return NULL;
    }

    fclose(fp);
    sodium_memzero(enc_info->encrypted_msg, enc_info->encrypted_size);
    free(enc_info->encrypted_msg);
    free(enc_info);
    return credentials;
}

int save_credentials(char *token) {
    einfo_t *enc_info = malloc(sizeof(einfo_t));
    if (enc_info == NULL) {
        perror("Failed to save credentials");
        return -1;
    }
    const int result = encrypt_text(enc_info, token);
    if (result != 0) {
        perror("Failed to save credentials");
        free(enc_info);
        return -2;
    }

    FILE *fp = fopen("../config/auth.bin", "wb");
    if (fp == NULL) {
        free(enc_info->encrypted_msg);
        free(enc_info);
        perror("Failed to save credentials!");
        return -3;
    }

    fwrite(&enc_info->encrypted_size, sizeof(size_t), 1, fp);
    fwrite(enc_info->nonce, sizeof(unsigned char), crypto_secretbox_NONCEBYTES ,fp);
    fwrite(enc_info->key, sizeof(unsigned char), crypto_secretbox_KEYBYTES ,fp);
    fwrite(enc_info->encrypted_msg, sizeof(unsigned char), enc_info->encrypted_size, fp);

    sodium_memzero(enc_info->encrypted_msg, enc_info->encrypted_size);
    free(enc_info->encrypted_msg);
    free(enc_info);
    fclose(fp);
    return 0;
}

int login() {
    char username[100] = "John Smith";
    char email[100];
    char password[100];
    char confirm_password[100] = "123Prudni@";
    printf("Enter your email:\n");
    scanf("%s", email);
    printf("Enter your password:\n");
    scanf("%s", password);


    cJSON *json = cJSON_CreateObject();
    cJSON_AddStringToObject(json, "username", username);
    cJSON_AddStringToObject(json, "email", email);
    cJSON_AddStringToObject(json, "password", password);
    cJSON_AddStringToObject(json, "confirmPassword", confirm_password);

    char *json_str = cJSON_Print(json);
    char *response = malloc(1);
    int result = make_request(
        "http://localhost:8080/api/v1/user/login",
        POST,
        json_str,
        NULL,
        0,
        NULL,
        0,
        &response
    );

    if (result != 0) {
        perror("Log In failed. Please try again!");
        return -1;
    }

    const cJSON *json_response = cJSON_Parse(response);
    if (json_response == NULL) {
        perror("Log In failed. Please try again!");
        return -2;
    }

    cJSON *token = cJSON_GetObjectItemCaseSensitive(json_response, "token");
    if (token == NULL || token->valuestring == NULL) {
        perror("Log In failed. Please try again!");
        return -3;
    }

    if (save_credentials(token->valuestring)) {
        perror("Log In failed. Please try again!");
        return -4;
    }

    has_details = fetch_user_details(&u_details);
    return 0;
}

int signup() {
    char username[100];
    char email[100];
    char password[100];
    char confirm_password[100];
    printf("Enter your username (max 100 characters)\n");
    scanf(" %[^\n]%*c", username);
    printf("Enter your email\n");
    scanf(" %s", email);

    char *passwd_validation;
    do {
        printf("Enter your password (6 - 100 characters)\n");
        scanf(" %s", password);
        passwd_validation = validate_pass(password);
        if (passwd_validation != NULL) printf("%s\n", passwd_validation);
    } while (passwd_validation != NULL);

    do {
        printf("Confirm your password:\n");
        scanf(" %s", confirm_password);
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
    const int result = make_request(
        "http://localhost:8080/api/v1/user/signup",
        POST,
        json_str,
        NULL,
        0,
        NULL,
        0,
        &response
        );
    if (result != 0) return 1;

    cJSON *json_response = cJSON_Parse(response);
    if (json_response == NULL) {
        perror("Sign Up failed. Please try again!");
        return 1;
    }
    cJSON *token = cJSON_GetObjectItemCaseSensitive(json_response, "token");
    if (token == NULL || token->valuestring == NULL) {
        perror("Sign Up failed. Please try again!");
        return 1;
    }

    if (save_credentials(token->valuestring) != 0) {
        perror("Sign Up failed. Please try again!");
        return 1;
    }

    free(response);
    return 0;
}

int logout() {
    int result = remove("../config/auth.bin");
    if (result != 0) {
        perror("Failed to logout!");
        return -1;
    }

    printf("Successfully logged out of your profile!\n");
    return 0;
}

void authenticate() {
    char *token = retrieve_credentials(true);

    if (token != NULL) {
        int result = validate_jwt(token);
        if (result != 0) {
            logout();
            has_details = fetch_user_details(&u_details);
            free(token);
            return;
        }
    }

    while (token == NULL) {
        int option;
        printf("Enter or create your account:\n");
        printf("1. Log in\n");
        printf("2. Sign up\n");
        printf("-1. Exit\n");
        scanf("%d", &option);

        switch (option) {
            case 1:
                if (login() == 0) token = retrieve_credentials(false);
                break;
            case 2:
                if (signup() == 0) token = retrieve_credentials(false);
                break;
            case -1:
                exit(1);
            default:
                printf("Invalid option selected!");
        }
    }

    printf("Successfully entered in your account!");
}
