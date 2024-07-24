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
    char gender[7];
    int age;
    metadata_t *metadata;
} user_details_t;

extern int has_details;
extern user_details_t *u_details;

int fetch_user_details(user_details_t **dest);
int create_user_details(user_details_t **details);
int update_user_details(user_details_t **updated_details);
int delete_user_details();

#endif //CLOTHING_APP_USER_DETAILS_H
