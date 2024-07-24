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