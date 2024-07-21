//
// Created by Kok_PC on 21.7.2024 г..
//

#ifndef HTTP_HANDLER_H
#define HTTP_HANDLER_H

typedef enum {
    GET, POST, PUT, DEL
} rtype_t;

typedef struct {
    char *memory;
    size_t size;
} MemoryStruct;

typedef struct {
    char *key;
    char *value;
} rheader_t;

typedef struct {
    char *key;
    char *value;
} rquery_t;

int make_request(char *url, rtype_t type, char *body, rheader_t *headerFields, size_t hnum, rquery_t *queries, size_t qnum, char **response);

#endif //HTTP_HANDLER_H
