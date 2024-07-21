cmake_minimum_required(VERSION 3.28)
project(clothing_app C)

set(CMAKE_C_STANDARD 17)

add_executable(clothing_app src/main.c
        src/auth.c
        include/auth.h
        src/http_handler.c
        include/http_handler.h
        src/auth_validator.c
        include/auth_validator.h)

find_package(CURL REQUIRED)
target_link_libraries(clothing_app PRIVATE CURL::libcurl)
