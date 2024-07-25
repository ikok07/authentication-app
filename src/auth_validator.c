//
// Created by Kok_PC on 21.7.2024 г..
//

#include "../include/auth_validator.h"

#include <stdbool.h>
#include <string.h>
#include <jwt.h>
#include <stdlib.h>

int validate_token(const char *token) {

}

char *validate_pass(const char *password) {
    if (strlen(password) < 6) return "Password must be atleast 6 characters!";

    bool has_uppercase = false;
    bool has_special_symbol = false;


    for (int i = 0; i < strlen(password); i++) {
        if (password[i] >= 33 && password[i] <= 64) has_special_symbol = true;
        if (password[i] >= 65 && password[i] <= 90) has_uppercase = true;
    }

    if (!has_uppercase) return "Password must contain atleast one uppercase letter!";
    if (!has_special_symbol) return "Password must contain atleast one special symbol!";
    return NULL;
}

int validate_jwt(char *token) {
    char *key = getenv("JWT_KEY");
    jwt_t *jwt = NULL;
    int result = jwt_decode(&jwt, token, (unsigned char *)key, (int)strlen(key));
    if (result != 0) {
        perror("Failed to validate JWT");
        return -1;
    }
    long exp = jwt_get_grant_int(jwt, "exp");

    time_t curr_time = time(NULL);
    double secs = difftime(curr_time, exp);
    if (secs > 0) return -2;

    return 0;
}
