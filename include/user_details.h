//
// Created by Kaloyan Petkov on 24.07.24.
//

#ifndef CLOTHING_APP_USER_DETAILS_H
#define CLOTHING_APP_USER_DETAILS_H

typedef struct metadata_t {
    char creation[30];
} metadata_t;

typedef struct {
    char _id[105];
    char userId[105];
    char gender[6];
    int age;
    metadata_t *metadata;
} user_details_t;

int fetch_user_details(user_details_t **dest);

#endif //CLOTHING_APP_USER_DETAILS_H
