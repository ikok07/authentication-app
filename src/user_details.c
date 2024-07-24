//
// Created by Kaloyan Petkov on 24.07.24.
//

#include <stddef.h>
#include <stdio.h>
#include <cjson/cJSON.h>
#include <stdlib.h>
#include <strings.h>
#include "../include/user_details.h"
#include "../include/http_handler.h"
#include "../include/auth.h"

int has_details = 1;
user_details_t *u_details = NULL;

int fetch_user_details(user_details_t **dest) {
    char *token = retrieve_credentials(false);

    if (token == NULL) return -1;

    char *response = malloc(1);
    rheader_t headers[1] = {"Authorization"};
    headers[0].value = malloc(strlen("Bearer ") + strlen(token));
    sprintf(headers[0].value, "Bearer %s", token);
    int result = make_request(
            "http://localhost:8080/api/v1/user/details/",
            GET,
            NULL,
            headers,
            1,
            NULL,
            0,
            &response
            );
    if (result != 0) return -2;
    if (dest == NULL) return 0;


    cJSON *json_response = cJSON_Parse(response);
    if (json_response == NULL) return -3;

    cJSON *user_details = cJSON_GetObjectItemCaseSensitive(json_response, "userDetails");
    if (user_details == NULL) return -4;

    char *_id = cJSON_GetObjectItemCaseSensitive(user_details, "_id")->valuestring;
    char *user_id = cJSON_GetObjectItemCaseSensitive(user_details, "userId")->valuestring;
    char *gender = cJSON_GetObjectItemCaseSensitive(user_details, "gender")->valuestring;
    int age = cJSON_GetObjectItemCaseSensitive(user_details, "age")->valueint;

    cJSON *metadata = cJSON_GetObjectItemCaseSensitive(user_details, "metadata");
    char *creation = cJSON_GetObjectItemCaseSensitive(metadata, "creation")->valuestring;

    user_details_t *details = malloc(sizeof(user_details_t));
    strcpy(details->_id, _id);
    strcpy(details->userId, user_id);
    strcpy(details->gender, gender);
    details->age = age;

    details->metadata = malloc(sizeof(metadata_t));
    strcpy(details->metadata->creation, creation);

    *dest = details;
    return 0;
}

int create_user_details(user_details_t **details) {
    int age;
    int gender_option;
    printf("Enter your age:\n");
    scanf("%d", &age);
    printf("Select your gender:\n1. Male\n2. Female\n");
    scanf("%d", &gender_option);

    char *token = retrieve_credentials(false);
    if (token == NULL) return -2;

    char *response = malloc(1);
    rheader_t headers[1] = {"Authorization"};
    headers[0].value = malloc(strlen("Bearer ") + strlen(token));
    sprintf(headers[0].value, "Bearer %s", token);

    cJSON *json = cJSON_CreateObject();
    cJSON_AddNumberToObject(json, "age", age);
    cJSON_AddStringToObject(json, "gender", gender_option == 1 ? "male" : "female");

    char *json_str = cJSON_Print(json);
    int result = make_request(
            "http://localhost:8080/api/v1/user/details/",
            POST,
            json_str,
            headers,
            1,
            NULL,
            0,
            &response
            );
    if (result != 0) return -3;
    if (details == NULL) return 0;

    user_details_t *new_details = malloc(sizeof(user_details_t));
    if (new_details == NULL) return -1;
    *details = malloc(sizeof(user_details_t));
    if (*details == NULL) return -1;

    cJSON *json_response = cJSON_Parse(response);
    cJSON *data = cJSON_GetObjectItemCaseSensitive(json_response, "data");
    if (data == NULL) return -4;

    char *_id = cJSON_GetObjectItemCaseSensitive(data, "_id")->valuestring;
    char *userId = cJSON_GetObjectItemCaseSensitive(data, "userId")->valuestring;
    char *response_gender = cJSON_GetObjectItemCaseSensitive(data, "gender")->valuestring;
    int response_age = cJSON_GetObjectItemCaseSensitive(data, "age")->valueint;
    strcpy(new_details->_id, _id);
    strcpy(new_details->userId, userId);
    strcpy(new_details->gender, response_gender);
    new_details->age = response_age;

    *details = new_details;
    return 0;
}

int update_user_details(user_details_t **updated_details) {
    user_details_t *user_details;
    int u_details_result = fetch_user_details(&user_details);
    if (u_details_result != 0) {
        fprintf(stderr, "Failed to fetch current user details!");
        return -1;
    }

    char *url = malloc(strlen("http://localhost:8080/api/v1/user/details/") + 126); // account the _id length
    sprintf(url, "http://localhost:8080/api/v1/user/details/%s", user_details->_id);

    char *token = retrieve_credentials(false);
    if (token == NULL) return -2;

    rheader_t headers[1] = {"Authorization"};
    headers[0].value = malloc(strlen("Bearer ") + strlen(token));
    sprintf(headers[0].value, "Bearer %s", token);

    int age;
    int gender_option;
    printf("Enter your age:\n");
    scanf("%d", &age);
    if (age != 0) user_details->age = age;
    printf("Select your gender:\n1. Male\n2. Female\n");
    scanf("%d", &gender_option);
    if (gender_option != 0) strcpy(user_details->gender, gender_option == 1 ? "male" : "female");

    cJSON *json = cJSON_CreateObject();
    cJSON_AddNumberToObject(json, "age", user_details->age);
    cJSON_AddStringToObject(json, "gender", user_details->gender);
    char *json_str = cJSON_Print(json);

    char *response = malloc(1);
    int result = make_request(
            url,
            PUT,
            json_str,
            headers,
            1,
            NULL,
            0,
            &response
            );
    if (result != 0) return -3;
    if (updated_details == NULL) return 0;

    user_details_t *tmp = malloc(sizeof(user_details_t));
    if (tmp == NULL) return -4;

    *updated_details = tmp;
    *updated_details = user_details;
    return 0;
}

int delete_user_details() {
    user_details_t *user_details;
    int u_details_result = fetch_user_details(&user_details);
    if (u_details_result != 0) {
        fprintf(stderr, "Failed to fetch current user details!");
        return -1;
    }

    char *url = malloc(strlen("http://localhost:8080/api/v1/user/details/") + 126); // account the _id length
    sprintf(url, "http://localhost:8080/api/v1/user/details/%s", user_details->_id);

    char *token = retrieve_credentials(false);
    if (token == NULL) return -2;

    rheader_t headers[1] = {"Authorization"};
    headers[0].value = malloc(strlen("Bearer ") + strlen(token));
    sprintf(headers[0].value, "Bearer %s", token);

    char *response = malloc(1);
    int result = make_request(
            url,
            DEL,
            NULL,
            headers,
            1,
            NULL,
            0,
            &response
    );
    if (result != 0) return -3;

    return 0;
}