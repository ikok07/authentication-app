cmake_minimum_required(VERSION 3.28)
project(clothing_app C)

set(CMAKE_C_STANDARD 17)

include_directories(/usr/local/include)
link_directories(/usr/local/lib)

add_executable(clothing_app src/main.c
        src/auth.c
        include/auth.h
        src/http_handler.c
        include/http_handler.h
        src/auth_validator.c
        include/auth_validator.h
        src/utils.c
        include/utils.h
        src/encryption.c
        include/encryption.h
        src/user_details.c
        include/user_details.h
        include/jwt.h)

find_package(CURL REQUIRED)
target_link_libraries(clothing_app PRIVATE CURL::libcurl)

find_package(cJSON CONFIG REQUIRED)
target_link_libraries(clothing_app PRIVATE cjson)

find_package(unofficial-sodium CONFIG REQUIRED)
target_link_libraries(clothing_app PRIVATE unofficial-sodium::sodium)

target_link_libraries(clothing_app PRIVATE jwt jansson ssl crypto)

