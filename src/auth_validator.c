//
// Created by Kok_PC on 21.7.2024 г..
//

#include "../include/auth_validator.h"

#include <string.h>

char *validate_pass(char *password) {
    if (strlen(password) < 6) return "Password must be atleast 6 characters!";
    return NULL;
}
