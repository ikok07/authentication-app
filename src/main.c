#include <stdio.h>
#include "../include/auth.h"

void executeOption(int option) {

}

void listOptions() {
    int option = 0;
    do {

        executeOption(option);
    } while (option >= 0);

}


int main(void) {

    printf("Welcome to the Clothes App\n");

    bool isAuth = authenticate();
    while (!isAuth) {
        isAuth = authenticate();
    }

    listOptions();

    return 0;
}
