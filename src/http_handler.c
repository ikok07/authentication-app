//
// Created by Kok_PC on 21.7.2024 г..
//

#include <stdlib.h>
#include <curl/curl.h>
#include <stdbool.h>
#include <cjson/cJSON.h>
#include "../include/http_handler.h"


size_t read_cb(char *ptr, size_t size, size_t nmemb, void *userdata) {
    MemoryStruct *upload = userdata;
    if (size == 0 || nmemb == 0 || size * nmemb < 1 || upload->size == 0) {
        return 0;
    }

    size_t real_size = size * nmemb < upload->size ? size * nmemb : upload->size;
    memcpy(ptr, upload->memory, real_size);
    upload->memory += real_size;
    upload->size -= real_size;

    return real_size;
}

size_t write_cb(void *data, size_t size, size_t nmemb, void *userdata) {
    const size_t real_size = size * nmemb;
    MemoryStruct *response = userdata;
    char *temp_memory = (char*)realloc(response->memory, response->size + real_size + 1);
    if (temp_memory == NULL) {
        perror("Failed to allocate memory for response data!");
        return CURL_WRITEFUNC_ERROR;
    }
    response->memory = temp_memory;
    memcpy(&response->memory[response->size], data, real_size);
    response->size += real_size;
    response->memory[response->size] = '\0';
    return real_size;
}

void print_error(const char *response) {
    fprintf(stderr, "HTTP Request failed!\n");
    cJSON *json_response = cJSON_Parse(response);
    if (json_response == NULL) {
        fprintf(stderr, "Failed to parse response error in JSON format!\n");
    }
    cJSON *id = cJSON_GetObjectItemCaseSensitive(json_response, "identifier");
    cJSON *message = cJSON_GetObjectItemCaseSensitive(json_response, "message");
    if (id != NULL && id->valuestring != NULL) fprintf(stderr, "Error identifier: %s\n", id->valuestring);
    if (id != NULL && message->valuestring) fprintf(stderr, "Error message: %s\n", message->valuestring);
    fprintf(stderr, "RAW RESPONSE: %s", response);
}

void free_memory(rtype_t type, MemoryStruct upload_data, MemoryStruct response_data, struct curl_slist *headers, CURL *curl) {
    // if (type != PUT) free(upload_data.memory);
    free(response_data.memory);
    curl_slist_free_all(headers);
    curl_easy_cleanup(curl);
}

int make_request(char *url, rtype_t type, char *body, rheader_t *headerFields, size_t hnum, rquery_t *queries, size_t qnum, char **response) {
    CURL *curl = curl_easy_init();
    CURLcode result;

    char *safeUrl = (char*)malloc(strlen(url) + 1);
    strcpy(safeUrl, url);
    if (queries != NULL) {
        for (int i = 0; i < qnum; i++) {
            char *safeQueryKey = curl_easy_escape(curl, queries[i].key, 0);
            char *safeQueryValue = curl_easy_escape(curl, queries[i].value, 0);
            if (safeQueryKey == NULL || safeQueryValue == NULL) {
                perror("Failed to encode url query!");
                return 1;
            }
            queries[i].key = safeQueryKey;
            queries[i].value = safeQueryValue;
            size_t new_size = strlen(safeUrl) + strlen(safeQueryKey) + strlen(safeQueryValue) + 2 + 1;
            char *tempPtr = realloc(safeUrl, new_size);
            if (tempPtr == NULL) {
                fprintf(stderr, "Failed to allocate memory for url queries!");
                curl_free(safeQueryKey);
                curl_free(safeQueryValue);
                return 1;
            }
            safeUrl = tempPtr;
            char startSymbol = i == 0 ? '?' : '&';
            strcat(safeUrl, &startSymbol);
            strcat(safeUrl, safeQueryKey);
            strcat(safeUrl, "=");
            strcat(safeUrl, safeQueryValue);

            curl_free(safeQueryKey);
            curl_free(safeQueryValue);
        }
    }

    struct curl_slist *headers = NULL;
    headers = curl_slist_append(headers, "Content-Type: application/json");
    headers = curl_slist_append(headers, "Accept: application/json");
    if (headerFields != NULL) {
        for (int i = 0; i < hnum; i++) {
            char *header = (char*)malloc(strlen(headerFields[i].key) + strlen(headerFields[i].value));
            sprintf(header, "%s: %s", headerFields[i].key, headerFields[i].value);
            headers = curl_slist_append(headers, header);
        }
    }

    MemoryStruct upload_data;
    if (body) {
        upload_data.memory = (char*)malloc(strlen(body) + 1);
        memcpy(upload_data.memory, body, strlen(body) + 1);
        upload_data.size = strlen(body);
    }
    MemoryStruct response_data = {(char*)malloc(1), 0};

    curl_easy_setopt(curl, CURLOPT_URL, safeUrl);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_cb);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void*)&response_data);
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
    if (body != NULL) {
        switch (type) {
            case POST:
                curl_easy_setopt(curl, CURLOPT_POSTFIELDS, upload_data.memory);
            break;
            case PUT:
                curl_easy_setopt(curl, CURLOPT_UPLOAD, 1L);
                curl_easy_setopt(curl, CURLOPT_READFUNCTION, read_cb);
                curl_easy_setopt(curl, CURLOPT_READDATA, (void*)&upload_data);
                curl_easy_setopt(curl, CURLOPT_INFILESIZE, (long)upload_data.size);
            break;
            default:
                perror("Invalid request type! If you use GET do not provide body!");
                free_memory(type, upload_data, response_data, headers, curl);
            return 1;
        }
    }

    result = curl_easy_perform(curl);
    if (result != CURLE_OK) {
        perror("HTTP Request failed!");
        free_memory(type, upload_data, response_data, headers, curl);
        return 1;
    }

    int http_code = 0;
    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_code);

    char *temp_memory = realloc(*response, sizeof(size_t) * response_data.size);
    if (temp_memory == NULL) {
        perror("Failed to allocate enough memory for response!");
        free_memory(type, upload_data, response_data, headers, curl);
        return 1;
    }
    *response = temp_memory;
    memcpy(*response, response_data.memory, response_data.size + 1);
    free_memory(type, upload_data, response_data, headers, curl);

    bool responseFailed = http_code >= 300 || http_code < 200;
    if (responseFailed) print_error(*response);
    return responseFailed ? 1 : 0;
}