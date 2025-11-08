#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <direct.h>
#include <windows.h>

void pseudo_start(char *userName);
char* pseudo_register(int *keyPtr);


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


        unsigned const int length = strlen(userName);

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

void pseudo_help() {
    printf( "---------------\n"
           "Terminal Commands:\n"
           "1.imdumb -> \t List Commands\n"
           "2.exit -> \t Exit the Terminal\n"
           "3.doxxme -> \t Locate current Directory\n"
           "4.whoiswithme -> \t List Files in current Directory\n"
           "5.moveto <filename> -> \t Move to another Directory\n"
           "6.getback -> \t Return to Previous Directory\n"
           "7.spawn <filename> -> \t Create File\n"
           "8.kobe! <filename> -> \t Delete File\n"
           "---------------\n"
           );

}

void pseudo_exit() {
    printf("You are Leaving :<. \n");
    exit(0);
}

void get_directory() {
    char cwd[1024];

    _getcwd(cwd, sizeof(cwd));

    printf("We are at: %s.\n", cwd);
}

void move_to_specific_directory(char* args[]) {

    if (args[2] != NULL) {
        printf("Follow the instructions like a good boy.\n");
        return;
    }

    if (args[1] != NULL) {
        if (SetCurrentDirectory(args[1])) {
            printf("Hell yeah, we moving inside: %s\n", args[1]);
        }
        else {
            printf("DAMN, the location does not exist, doxx yourself first.\n");
        }
    }
    else {
        printf("Bruh, you did not add the location >_>\n");
    }
}

void list_files_in_same_directory() {
    WIN32_FIND_DATA findFileData;
    HANDLE hFind = FindFirstFile("*", &findFileData);

    if (hFind == INVALID_HANDLE_VALUE) {
        printf("Nobody's here, you need meds bro.\n");
        return;
    }

    printf("You took your meds:\n");

    do {
        if (strcmp(findFileData.cFileName, ".") != 0 && strcmp(findFileData.cFileName, "..") != 0) {
            printf("    -%s\n", findFileData.cFileName);
        }
    }while (FindNextFile(hFind, &findFileData));

    FindClose(hFind);
}

void return_to_prev_directory() {
    if (SetCurrentDirectory("..")) {
        printf("Check at the back, the car is backing up!\n");
    }
    else {
        printf("Sorry bro, we are already at the edge of the place\n");
    }
}

void create_file(char* args[]) {
    if (args[2] != NULL) {
        printf("Follow the instructions like a good boy.");
        return;
    }

    if (args[1] != NULL) {
        FILE *file = fopen(args[1], "w");

        if (file != NULL) {
            fclose(file);
            printf("Poof! '%s' has been spawned into reality!\n", args[1]);
        }
        else {
            printf("You have no mana(Permission Denied for some BS reasons)\n");
        }
    }
    else {
        printf("You can't spawn anything unless you chant the magic word!");
    }
}

void delete_file(char* args[]) {
    if (args[2] != NULL) {
        printf("Follow the instructions like a good boy.\n");
        return;
    }

    if (args[1] != NULL) {
        if (remove(args[1]) == 0) {
            printf("April 14, 2004!\n");
        }
        else {
            printf("Bricked!\n");
        }
    }
    else {
        printf("There is no ball to be swished BRO\n");
    }
}


void pseudo_start(char *userName) {

    char line_buffer[1024];
    char* args[64];



    while (1) { // Start
        printf("%s> ", userName);
        fgets(line_buffer, 1024, stdin);
        insert_token(line_buffer, args);

        if (args[0] == NULL) {
            printf("Warning: Woopsie! You didnt enter anything\n");
            continue;
        }
        if (strcmp(args[0], "imdumb") == 0) {
            pseudo_help();
        }
        else if (strcmp(args[0], "exit") == 0) {
            pseudo_exit();
        }
        else if (strcmp(args[0], "doxxme") == 0) {
            get_directory();

        }
        else if (strcmp(args[0], "whoiswithme") == 0) {
            list_files_in_same_directory();
        }
        else if (strcmp(args[0], "moveto") == 0) {
            move_to_specific_directory(args);
        }
        else if (strcmp(args[0], "getback") == 0) {
           return_to_prev_directory();
        }
        else if (strcmp(args[0], "spawn") == 0) {
            create_file(args);
        }
        else if (strcmp(args[0], "kobe!") == 0) {
            delete_file(args);
        }
        else {
            printf("Command error :<\n");
        }
   }
}