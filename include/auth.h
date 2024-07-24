//
// Created by Kok_PC on 21.7.2024 г..
//

#ifndef AUTH_H
#define AUTH_H
#include <stdbool.h>
#include <stddef.h>

char *retrieve_credentials(bool ignore_err);
void authenticate();
int logout();

#endif //AUTH_H
