#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void pseudo_start();
char* pseudo_register();


void insert_token(char* line_buffer, char* args[]) {
    char *token = strtok(line_buffer, " \t\r\n");
    int i = 0;

    while (token != NULL) {
        args[i] = token;
        i++;

        token = strtok(NULL, " \t\r\n");
    }

    args[i] = NULL;
}

int main(void) {
    int keyPressed;

    printf("Hello To my Custom Shell!\n");
    printf("It is made by \"HiveMind\" to showcase my talents ^^.\n");
    printf("Press Enter key to start using it:");

    keyPressed = getchar();
    if (keyPressed == 10) {
        pseudo_start(pseudo_register(&keyPressed));
    }

    printf("\nExiting.....");
    return 0;
}

char* pseudo_register(int *keyPtr) { // pass the int keyPressed address so that I don't need to create another one
    static char userName[10]; // Making it static to allow it to stay alive even if the function ends
    while (1) {
        printf("Let us get to know each other better.\n");
        printf("Please enter your username: ");
        fgets(userName, 10, stdin);
        userName[strcspn(userName, "\n")] = '\0';


        int length = strlen(userName);

        if (length == 0) {
            printf("Warning: Woopsie! You didn't write anything.\n");
        }
        else {
            printf("Are you sure about %s?(Enter Y):", userName);

            *keyPtr = getchar();
            if (*keyPtr == 89) {
                getchar();// To prevent issues in loop where confirming your username is also run when you are only entering your username yet after redoing your name
                return userName;
            }

            printf("Warning: Woopsie! You didn't follow instruction.\n");
        }

        if (*keyPtr != '\n'){
            getchar();// To prevent issues in loop where confirming your username is also run when you are only entering your username yet after redoing your name
        }
    }

}


void pseudo_start(char *userName) {

    char line_buffer[1024];
    char* args[64]; // not used for now

    while (1) { // Start

        printf("What do you want to do XD?\n");
        printf("%s> ", userName);
        fgets(line_buffer, 1024, stdin);
        insert_token(line_buffer, args);

        if (args[0] == NULL) {
            printf("Warning: Woopsie! You didnt enter anything\n");
            continue;
        }

        if (strcmp(args[0], "help") == 0) {
            printf("No help yet :<.\n");
        }
        else if (strcmp(args[0], "exit") == 0) {
            printf("You are leaving :<. \n");
            break;
        }


   }


}