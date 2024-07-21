//
// Created by Kok_PC on 21.7.2024 г..
//

#include "../include/utils.h"

#include <stdio.h>
#include <stdlib.h>

void loadEnvFile() {
    FILE *fp = fopen("../.env", "r");
    if (fp == NULL) {
        fprintf(stderr, "Failed to open .env file!");
        return;
    }

    int curr_line_size = 0;
    char *curr_line = malloc(curr_line_size);
    char curr_char;
    while ((curr_char = fgetc(fp)) != EOF) {
        if (curr_char == '\n') {
            curr_line[curr_line_size] = '\0';
            if (curr_line[0] == '#') {
                curr_line_size = 0;
                curr_line = malloc(curr_line_size);
            }
            putenv(curr_line);
            curr_line_size = 0;
            curr_line = malloc(curr_line_size);
            continue;
        }
        char *temp_ptr = realloc(curr_line, ++curr_line_size);
        if (temp_ptr == NULL) {
            printf("Failed to load ENV file!");
            exit(1);
        }
        curr_line = temp_ptr;
        curr_line[curr_line_size - 1] = curr_char;
    }

    if (curr_line_size > 0) {
        curr_line[curr_line_size] = '\0';
        putenv(curr_line);
    }

    fclose(fp);
}
