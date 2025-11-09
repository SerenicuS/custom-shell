#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <direct.h>
#include <windows.h>
#include <shellapi.h>


// OK TYPES
#define SHELL_OK_GENERAL 0
#define SHELL_OK_DELETE_FILE 1
#define SHELL_OK_CREATE_FILE 2
#define SHELL_OK_RETURN_DIRECTORY 3
#define SHELL_OK_LISTED_FILES 4
#define SHELL_OK_MOVE_DIRECTORY 5
#define SHELL_OK_TERMINATE 6
#define SHELL_OK_CREATE_DIRECTORY 7
#define SHELL_OK_DELETE_DIRECTORY 8
#define SHELL_OK_READ_FILE 9
#define SHELL_OK_OPENED_FILE 10

// ERROR TYPES
#define SHELL_ERROR_GENERAL 30
#define SHELL_ERROR_BAD_ARGS 31
#define SHELL_ERROR_TOO_MANY_ARGS 32
#define SHELL_ERROR_SYSTEM 33
#define SHELL_ERROR_FILE_DOES_NOT_EXIST 34
#define SHELL_ERROR_PERMISSION_DENIED 35
#define SHELL_ERROR_ROOT_DIRECTORY 36
#define SHELL_ERROR_LISTED_FILES_DOES_NOT_EXIST 37




void pseudo_start(char *userName);
char* pseudo_register(int *keyPtr);


void insert_token(char* line_buffer, char* args[]) {
    char *token = strtok(line_buffer, " \t\r\n");
    int i = 0;

    while (token != NULL && i < 63) {
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
    printf("Do you know your name?\n");
    while (1) {
        printf("\n");
        printf("Tell me your name sweetie: ");
        fgets(userName, 10, stdin);

        if (strchr(userName, '\n') == NULL) {
            int c;
            while ((c = getchar()) != '\n' && c != EOF) {
                // This loop discards all the leftover characters
            }
        }


        userName[strcspn(userName, "\n")] = '\0';


        unsigned const int length = strlen(userName);

        if (length == 0) {
            printf("You won't speak? I am not going to leave yet...\n");
        }
        else {

            if (strcmp(userName, "Harold") != 0) {
                printf("You are not %s, you are Harold.\n", userName);
            }
            else {
                return userName;
            }


        }

    }

}

void shell_help() {
    printf( "---------------\n"
           "You are too greedy...:\n"
           "1.tellme -> \t List Commands\n"
           "2.mayileave -> \t Exit the Terminal\n"
           "3.iamhere -> \t Locate current Directory\n"
           "4.mommy? -> \t List Files in current Directory\n"
           "5.walkwithme <filename> -> \t Move to another Directory\n"
           "6.goback -> \t Return to Previous Directory\n"
           "7.canihave <filename> -> \t Create File\n"
           "8.takethe <filename> -> \t Delete File\n"
           "9.letusplayhouse <directoryname> -> \t Create a Directory\n"
           "10.openthis <filename> -> \t Open the File\n"
           "11.readthis <filename> -> \t Read the File's contents\n"
           "---------------\n"
           );

}


int shell_exit() {
    return SHELL_OK_TERMINATE;
}

void get_directory() {
    char cwd[1024];

    _getcwd(cwd, sizeof(cwd));

    printf("We are at: %s.\n", cwd);
}

int move_to_specific_directory(char* args[]) {

    if (args[2] != NULL) {
        return SHELL_ERROR_GENERAL;
    }

    if (args[1] != NULL) {
        if (SetCurrentDirectory(args[1])) {
            return SHELL_OK_MOVE_DIRECTORY;
        }
        return SHELL_ERROR_FILE_DOES_NOT_EXIST;
    }

    return SHELL_ERROR_BAD_ARGS;

}

int list_files_in_same_directory() {
    WIN32_FIND_DATA findFileData;
    HANDLE hFind = FindFirstFile("*", &findFileData);
    int bFoundUserFiles = 0;

    if (hFind == INVALID_HANDLE_VALUE) {
        return SHELL_ERROR_LISTED_FILES_DOES_NOT_EXIST;
    }

    do {
        if (strcmp(findFileData.cFileName, ".") != 0 && strcmp(findFileData.cFileName, "..") != 0) {
            printf("    -%s\n", findFileData.cFileName);
            bFoundUserFiles = 1;
        }

    }while (FindNextFile(hFind, &findFileData));

    FindClose(hFind);
    if (bFoundUserFiles == 0) {
        return SHELL_ERROR_LISTED_FILES_DOES_NOT_EXIST;
    }
    return SHELL_OK_LISTED_FILES;
}

int return_to_prev_directory() {
    if (SetCurrentDirectory("..")) {
        return SHELL_OK_RETURN_DIRECTORY;

    }

    return SHELL_ERROR_ROOT_DIRECTORY;
}

int create_file(char* args[]) {
    if (args[2] != NULL) {
        return SHELL_ERROR_GENERAL;
    }

    if (args[1] != NULL) {
        FILE *file = fopen(args[1], "w");

        if (file != NULL) {
            fclose(file);
            return SHELL_OK_CREATE_FILE;
        }

        return SHELL_ERROR_PERMISSION_DENIED;
    }

    return SHELL_ERROR_BAD_ARGS;

}

int delete_file(char* args[]) {

    if (args[2] != NULL) {

        return SHELL_ERROR_TOO_MANY_ARGS;
    }

    if (args[1] != NULL) {
        if (remove(args[1]) == 0) {
            return SHELL_OK_DELETE_FILE;

        }
        return SHELL_ERROR_FILE_DOES_NOT_EXIST;

    }

    return SHELL_ERROR_FILE_DOES_NOT_EXIST;
}

int create_directory(char* args[]) {
    if (args[1] == NULL) {
        return SHELL_ERROR_FILE_DOES_NOT_EXIST;
    }


    if (args[2] != NULL) {
        return SHELL_ERROR_TOO_MANY_ARGS;
    }

    if (CreateDirectory(args[1], NULL)) {
        return SHELL_OK_CREATE_DIRECTORY;
    }

    return SHELL_ERROR_PERMISSION_DENIED;
}

int delete_directory(char* args[]) {
    if (args[1] == NULL) {
        return SHELL_ERROR_FILE_DOES_NOT_EXIST;
    }

    if (args[2] != NULL) {
        return SHELL_ERROR_TOO_MANY_ARGS;
    }

    if (RemoveDirectory(args[1])) {
        return SHELL_OK_DELETE_DIRECTORY;
    }

    return SHELL_ERROR_PERMISSION_DENIED;
}

int open_file(char* args[]) {
    if (args[1] == NULL) {
        return SHELL_ERROR_FILE_DOES_NOT_EXIST;
    }


    if (args[2] != NULL) {
        return SHELL_ERROR_TOO_MANY_ARGS;
    }

    intptr_t result = (intptr_t)ShellExecute(
          NULL,      // No parent window
          "open",    // The "verb" - this is the magic word for "start"
          args[1],   // The file or URL to open
          NULL,      // No parameters
          NULL,      // Use the current directory
          SW_SHOWNORMAL // Just tell it to show the window
      );

    if (result > 32) {
        return SHELL_OK_OPENED_FILE;
    }
    return SHELL_ERROR_FILE_DOES_NOT_EXIST;
}

int read_file(char* args[]) {
    char line_buffer[1024];

    if (args[1] == NULL) {
        return SHELL_ERROR_FILE_DOES_NOT_EXIST;
    }


    if (args[2] != NULL) {
        return SHELL_ERROR_TOO_MANY_ARGS;
    }

    FILE *file = fopen(args[1], "r");

    if (file == NULL) {
        return SHELL_ERROR_FILE_DOES_NOT_EXIST;
    }
    printf("<");
    while (fgets(line_buffer, sizeof(line_buffer), file) != NULL) {
        printf("%s", line_buffer);
    }
    printf(">\n");
    fclose(file);
    return SHELL_OK_READ_FILE;
}


void get_shell_status(int rc) {
    switch (rc) {
        case 0:
            printf("It was successful sweetie.\n");
            break;
        case 1:
            printf("You don't like this? Fine, I will have it.\n");
            break;
        case 2:
            printf("Here sweetie, please take care of it.\n");
            break;
        case 3:
            printf("Be careful sweetie.\n");
            break;
        case 4:
            printf("You don't trust your mommy?...:\n");
            break;
        case 5:
            printf("We are here now, do you like it?\n");
            break;
        case 6:
            printf("Talk to you later sweetie. \n");
            exit(0);
        case 7:
            printf("Oh, you want to play house with me sweetie?\n");
            break;
        case 8:
            printf("You don't have to do that, we can just create more house.\n");
            break;
        case 9:
            printf("Do you like the contents of the file sweetie?\n");
            break;
        case 10:
            printf("Write what is important for you, sweet boy.\n");
            break;
        case 30:
            printf("Tell me the instructions correctly sweetie.\n");
            break;
        case 31:
            printf("You didn't complete your sentence sweetie, are you flustered?.\n");
            break;
        case 32:
            printf("Greedy Aren't you?.\n");
            break;
        case 33:
            printf("Oh my, the system crashed\n");
            exit(0);
        case 34:
            printf("Why are you asking me that doesn't exist sweetie?\n");
            break;
        case 35:
            printf("You are not allowed to do that sweetie\n");
            break;
        case 36:
            printf("This is as far as we can go sweetie.\n");
            break;
        case 37:
            printf("Hmmm, no one is here, only your mommy right?.\n");
            break;
        default:
            printf("I don't know what you want sweetie.\n");
            break;
    }
}



void pseudo_start(char *userName) {

    char line_buffer[1024];
    char* args[64];
    printf("Good boy, always listen to your mommy.\n");



    while (1) { // Start
        printf("%s> ", userName);
        fgets(line_buffer, 1024, stdin);
        insert_token(line_buffer, args);
        int rc = 0;

        if (args[0] == NULL) {
            rc = 100;

        }
        else if (strcmp(args[0], "tellme") == 0) {
            shell_help();
            continue;
        }
        else if (strcmp(args[0], "mayileave") == 0) {
            rc = shell_exit();

        }
        else if (strcmp(args[0], "iamhere") == 0) {
            get_directory();
            continue;

        }
        else if (strcmp(args[0], "mommy?") == 0) {
            rc = list_files_in_same_directory();

        }
        else if (strcmp(args[0], "walkwithme") == 0) {
            rc = move_to_specific_directory(args);

            if (rc == SHELL_OK_MOVE_DIRECTORY) {
                printf("Moved inside: %s\n", args[1]);
            }


        }
        else if (strcmp(args[0], "goback") == 0) {
           rc = return_to_prev_directory();

        }
        else if (strcmp(args[0], "canihave") == 0) {
            rc = create_file(args);

        }
        else if (strcmp(args[0], "takethe") == 0) {
            rc = delete_file(args);

        }
        else if (strcmp(args[0], "letusplayhouse") == 0) {
            rc = create_directory(args);

        }
        else if (strcmp(args[0], "removethehouse") == 0) {
            rc = delete_directory(args);

        }
        else if (strcmp(args[0], "openthis") == 0) {
            rc = open_file(args);
        }
        else if (strcmp(args[0], "readthis") == 0) {
            rc = read_file(args);
        }
        else {
            rc = 100;

        }

        get_shell_status(rc);

   }
}