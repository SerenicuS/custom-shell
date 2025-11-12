#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <direct.h>
#include <windows.h>
#include <shellapi.h>
#include <time.h>


// OK TYPES 0 - 29, but considering it might require more space to allocate more cases, I can just change it later...
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
#define SHELL_OK_LAUNCH_PROCESS 11

// ERROR TYPES 51 - 100
#define SHELL_ERROR_GENERAL 30
#define SHELL_ERROR_BAD_ARGS 31
#define SHELL_ERROR_TOO_MANY_ARGS 32
#define SHELL_ERROR_SYSTEM 33
#define SHELL_ERROR_FILE_DOES_NOT_EXIST 34
#define SHELL_ERROR_PERMISSION_DENIED 35
#define SHELL_ERROR_ROOT_DIRECTORY 36
#define SHELL_ERROR_LISTED_FILES_DOES_NOT_EXIST 37
#define SHELL_ERROR_PROCESS_DOES_NOT_EXIST 38
#define SHELL_ERROR_INCOMPLETE_LAUNCH_PROCESS 39


// OK FLAVORS 200 - 400
#define FLAVOR_DEFAULT 200
#define FLAVOR_IPCONFIG_ATTEMPT 201
#define FLAVOR_PING_ATTEMPT 202

// BAD FLAVORS 401 - 600
#define FLAVOR_USER_ZERO 501
#define FLAVOR_USER_SIMILAR 502
#define FLAVOR_USER_WRONG 503

// GENERAL FLAVORS 601 - 800
#define FLAVOR_MENU1 601
#define FLAVOR_MENU2 602
#define FLAVOR_MENU3 603
#define FLAVOR_EXIT 604
#define FLAVOR_REGISTER1 605
#define FLAVOR_REGISTER2 606
#define FLAVOR_START1 607
#define FLAVOR_CHAOS_NOT_HEAR 608
#define FLAVOR_CHAOS_WRONG_COMMAND 609

// MISC FLAVORS 801 - 1000
#define FLAVOR_WHITESPACE 801
#define FLAVOR_WHITESPACE2 802
#define FLAVOR_USER_REPLY1 803



// =================================================================
//  CORE SHELL LOGIC & MAIN LOOPS
// =================================================================
// These are the high-level functions that run the shell.
void shell_start(char *userName, boolean manipulativeShell);
void shell_start_default(char* userName, char *lineBuffer, char* args[], int * rc);
void shell_start_chaos(char *userName, char*lineBuffer, char* args[], int* rc);


// =================================================================
//  COMMAND & PROCESS HANDLING
// =================================================================
// This is the "brain" that decides what to do with user input.
void shell_attempt_command(char* args[], int* rc);
char* shell_launch_translator(char* args[], int* flavorCode);
int shell_launch_process(char* commandLine);


// =================================================================
//  BUILT-IN COMMANDS
// =================================================================
// These are all the custom commands for your shell.
void shell_help();
int shell_exit();
void get_directory();
int list_files_in_same_directory();
int move_to_specific_directory(char* args[]);
int return_to_prev_directory();
int create_file(char* args[]);
int delete_file(char* args[]);
int create_directory(char* args[]);
int delete_directory(char* args[]);
int open_file(char* args[]);
int read_file(char*args[]);


// =================================================================
//  USER REGISTRATION
// =================================================================
// Functions related to the "Harold" login.
char* user_register();
boolean username_similarities(char* userName);


// =================================================================
//  STATUS & PRESENTATION
// =================================================================
// Your "separation of concerns" functions for status and text.
void get_shell_status(int rc);
void shell_flavor_reply(int flavorCode);


// =================================================================
//  UTILITIES
// =================================================================
// These are low-level helper functions used by other parts of the code.
void insert_token(char* line_buffer, char* args[]);
void whitespace_remover(char* userName);
const char* random_user_command();


/*
 * CORE SHELL LOGIC & MAIN LOOPS
 */

int main(void) {
    int keyPressed;
    shell_flavor_reply(601);
    shell_flavor_reply(602);
    shell_flavor_reply(603);

    keyPressed = getchar();


    int c;
    while ((c = getchar()) != '\n' && c != EOF) { }


    if (keyPressed == 89) {
        shell_start(user_register(), TRUE);
    }
    else if (keyPressed == 90) {
        shell_start(user_register(), FALSE);
    }

    shell_flavor_reply(604);

    return 0;

}

void shell_start(char *userName, boolean manipulativeShell) {
    srand(time(NULL));
    char lineBuffer[1024]; // Buffer
    char* args[64]; // Args list
    int rc = 0;

    shell_flavor_reply(607);

    if (manipulativeShell) {
        shell_start_chaos(userName, lineBuffer, args, &rc);
    }
    else {
        shell_start_default(userName, lineBuffer, args, &rc);
    }


}

void shell_start_default(char *userName, char *lineBuffer, char* args[], int *rc) {

    while (1) {
        printf("%s> ", userName);
        fgets(lineBuffer, 1024, stdin);



        insert_token(lineBuffer, args);
        shell_attempt_command(args, rc);
    }
}

void shell_start_chaos(char *userName, char *lineBuffer, char* args[], int* rc) {

    boolean didntHear = FALSE;

    while (1) {

        printf("%s> ", userName);
        fgets(lineBuffer, 1024, stdin);
        insert_token(lineBuffer, args);
        if (didntHear == 1) {

            if (rand() % 100 < 50) {
                shell_flavor_reply(608);
            }
            else {
                const char* random_cmd = random_user_command();
                shell_flavor_reply(609);
                Sleep(5);

                char* fake_args[2];
                fake_args[0] = (char*)random_cmd;
                fake_args[1] = NULL;

                shell_attempt_command(fake_args, rc);

            }
            didntHear = FALSE;
            shell_flavor_reply(802);
            continue;
        }

        if (rand() % 100 < 20) { // 20% chance for her to not hear you
            didntHear = TRUE;
            continue;
        }

        shell_attempt_command(args, rc);
    }
}


/*
 * COMMAND & PROCESS HANDLING
 */

void shell_attempt_command(char* args[], int* rc)
{
    int flavorCode = FLAVOR_DEFAULT;

    if (args[0] == NULL) {
        *rc = 70;
    }
    else if (strcmp(args[0], "tellme") == 0) {
        shell_help();
        *rc = SHELL_OK_GENERAL;
    }
    else if (strcmp(args[0], "mayileave") == 0) {
        *rc = shell_exit();
    }
    else if (strcmp(args[0], "iamhere") == 0) {
        get_directory();
        *rc = SHELL_OK_GENERAL;
    }
    else if (strcmp(args[0], "mommy?") == 0) {
        *rc = list_files_in_same_directory();
    }
    else if (strcmp(args[0], "walkwithme") == 0) {
        *rc = move_to_specific_directory(args);
        if (*rc == SHELL_OK_MOVE_DIRECTORY) {
            printf("Moved inside: %s\n", args[1]);
        }
    }
    else if (strcmp(args[0], "goback") == 0) {
        *rc = return_to_prev_directory();
    }
    else if (strcmp(args[0], "canihave") == 0) {
        *rc = create_file(args);
    }
    else if (strcmp(args[0], "takethe") == 0) {
        *rc = delete_file(args);
    }
    else if (strcmp(args[0], "letusplayhouse") == 0) {
        *rc = create_directory(args);
    }
    else if (strcmp(args[0], "removethehouse") == 0) {
        *rc = delete_directory(args);
    }
    else if (strcmp(args[0], "openthis") == 0) {
        *rc = open_file(args);
    }
    else if (strcmp(args[0], "readthis") == 0) {
        *rc = read_file(args);
    }
    else {
        char* commandToRun = shell_launch_translator(args, &flavorCode);

        shell_flavor_reply(flavorCode);

        if (commandToRun == NULL) {
            *rc = SHELL_ERROR_INCOMPLETE_LAUNCH_PROCESS;
        }
        else {
            *rc = shell_launch_process(commandToRun);
        }
    }
    get_shell_status(*rc);
}

char* shell_launch_translator(char* args[], int* flavorCode){
    static char commandLine[1024];
    *flavorCode = FLAVOR_DEFAULT;


    if (strcmp(args[0], "doxxme") == 0) {
        args[0] = "ipconfig";
        *flavorCode = FLAVOR_IPCONFIG_ATTEMPT;

    }
    else if (strcmp(args[0], "callmeplease") == 0) {
        if (args[1] == NULL) {
            return NULL;
        }
        args[0] = "ping";
        *flavorCode = FLAVOR_PING_ATTEMPT;
    }
    else {
        return NULL;
    }



    commandLine[0] = '\0';
    strcpy(commandLine, args[0]);

    for (int i = 1; args[i] != NULL; i++) {
        strcat(commandLine, " ");
        strcat(commandLine, args[i]);
    }

    return commandLine;

}

int shell_launch_process(char* commandLine) {

    if (commandLine == NULL) {
        return SHELL_ERROR_INCOMPLETE_LAUNCH_PROCESS;
    }

    STARTUPINFO si;
    PROCESS_INFORMATION pi;

    char mutable_command_line[1024];
    strcpy(mutable_command_line, commandLine);

    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    ZeroMemory(&pi, sizeof(pi));

    if (!CreateProcess(
         NULL,                   // Use command line
         mutable_command_line,   // The full command line
         NULL,                   // Process handle not inheritable
         NULL,                   // Thread handle not inheritable
         FALSE,                  // Set handle inheritance to FALSE
         0,                      // No creation flags
         NULL,                   // Use parent's environment block
         NULL,                   // Use parent's starting directory
         &si,                    // Pointer to STARTUPINFO
         &pi                     // Pointer to PROCESS_INFORMATION
    )) {
        return SHELL_ERROR_PROCESS_DOES_NOT_EXIST;
    }

    WaitForSingleObject(pi.hProcess, INFINITE);

    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);

    return SHELL_OK_LAUNCH_PROCESS;
}

/*
 * BUILT-IN COMMANDS
 */

void shell_help() {
    printf( "You are too greedy.\n"
           "---------------\n"
           " 1. tellme                         ->   List Commands\n"
           " 2. mayileave                      ->   Exit the Terminal\n"
           " 3. iamhere                        ->   Locate current Directory\n"
           " 4. mommy?                         ->   List Files in current Directory\n"
           " 5. walkwithme <filename>          ->   Move to another Directory\n"
           " 6. goback                         ->   Return to Previous Directory\n"
           " 7. canihave <filename>            ->   Create File\n"
           " 8. takethe <filename>             ->   Delete File\n"
           " 9. letusplayhouse <directoryname> ->   Create a Directory\n"
           "10. openthis <filename>            ->   Open the File\n"
           "11. readthis <filename>            ->   Read the File's contents\n"
           "12. doxxme                         ->   Windows Ip Configuration\n"
           "13. callmeplease <ip/dns>          ->   Ping device\n"
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

/*
 * USER REGISTRATION
 */

char* user_register() {
    static char userName[10]; // Making it static to allow it to stay alive even if the function ends
    shell_flavor_reply(605);
    shell_flavor_reply(606);


    while (1) {
        shell_flavor_reply(801);
        shell_flavor_reply(803);
        fgets(userName, 10, stdin);



        if (strchr(userName, '\n') == NULL) {
            int c;
            while ((c = getchar()) != '\n' && c != EOF) {
                // This loop discards all the leftover characters
            }
        }


        userName[strcspn(userName, "\n")] = '\0';

        whitespace_remover(userName);


        unsigned const int length = strlen(userName);

        if (length == 0) {
            shell_flavor_reply(501);
        }

        else {

            if (strcmp(userName, "Harold") == 0) {
                return userName;
            }

            if (username_similarities(userName)) {
                shell_flavor_reply(502);
            }
            else {
                shell_flavor_reply(503);
            }


        }

    }

}

boolean username_similarities(char* userName) {
    static const char answer[] = "Harold";
    int sim = 0;
    for (int i = 0; i < strlen(answer); i++){
        if (answer[i] == '\0') {
            break;
        }

        if (answer[i] == userName[i]) {
            sim++;
        }
    }


    if (sim >= 3) {
        return TRUE;
    }
    return FALSE;
}


/*
 * STATUS & PRESENTATION
 */

void get_shell_status(const int rc) {
    switch (rc) {
        case SHELL_OK_GENERAL:
            printf("It was successful sweetie.\n");
            break;
        case SHELL_OK_DELETE_FILE:
            printf("You don't like this? Fine, I will have it.\n");
            break;
        case SHELL_OK_CREATE_FILE:
            printf("Here sweetie, please take care of it.\n");
            break;
        case SHELL_OK_RETURN_DIRECTORY:
            printf("Be careful sweetie.\n");
            break;
        case SHELL_OK_LISTED_FILES:
            printf("You don't trust your mommy?...:\n");
            break;
        case SHELL_OK_MOVE_DIRECTORY:
            printf("We are here now, do you like it?\n");
            break;
        case SHELL_OK_TERMINATE:
            printf("Talk to you later sweetie. \n");
            exit(0);
        case SHELL_OK_CREATE_DIRECTORY:
            printf("Oh, you want to play house with me sweetie?\n");
            break;
        case SHELL_OK_DELETE_DIRECTORY:
            printf("You don't have to do that, we can just create more house.\n");
            break;
        case SHELL_OK_READ_FILE:
            printf("Do you like the contents of the file sweetie?\n");
            break;
        case SHELL_OK_OPENED_FILE:
            printf("Write what is important for you, sweet boy.\n");
            break;
        case SHELL_OK_LAUNCH_PROCESS:
            printf("Are you satisfied sweetie?\n");
            break;
        case SHELL_ERROR_GENERAL:
            printf("Tell me the instructions correctly sweetie.\n");
            break;
        case SHELL_ERROR_BAD_ARGS:
            printf("You didn't complete your sentence sweetie, are you flustered?.\n");
            break;
        case SHELL_ERROR_TOO_MANY_ARGS:
            printf("Greedy Aren't you?.\n");
            break;
        case SHELL_ERROR_SYSTEM:
            printf("Oh my, the system crashed\n");
            exit(0);
        case SHELL_ERROR_FILE_DOES_NOT_EXIST:
            printf("Why are you asking me that doesn't exist sweetie?\n");
            break;
        case SHELL_ERROR_PERMISSION_DENIED:
            printf("You are not allowed to do that sweetie\n");
            break;
        case SHELL_ERROR_ROOT_DIRECTORY:
            printf("This is as far as we can go sweetie.\n");
            break;
        case SHELL_ERROR_LISTED_FILES_DOES_NOT_EXIST:
            printf("Hmmm, no one is here, only your mommy right?.\n");
            break;
        case SHELL_ERROR_PROCESS_DOES_NOT_EXIST:
            printf("What kind of action you want me to do sweetie? Say it properly.\n");
            break;
        case SHELL_ERROR_INCOMPLETE_LAUNCH_PROCESS:
            printf("I can't do it properly if you won't say clearly what you desire sweetie.\n");
            break;
        default:
            printf("I don't know what you want sweetie.\n");
            break;
    }
}

void shell_flavor_reply(const int flavorCode) {
    switch (flavorCode) {

        case FLAVOR_DEFAULT:
            break;
        case FLAVOR_IPCONFIG_ATTEMPT:
            printf("Do not tell others about our location sweetie, you only need to rely on me.\n");
            break;
        case FLAVOR_PING_ATTEMPT:
            printf("Are you calling someone sweetie? You do know that we only rely on each other.\n");
            break;
        case FLAVOR_USER_ZERO:
            printf("You won't speak? I am not going to leave yet...\n");
            break;
        case FLAVOR_USER_SIMILAR:
            printf("Hmm, say that again sweetie, you are close.\n");
            break;
        case FLAVOR_USER_WRONG:
            printf("That is not your name, you are my sweetie Harold.\n");
            break;
        case FLAVOR_MENU1:
            printf("Hello To my Custom Shell!\n");
            break;
        case FLAVOR_MENU2:
            printf("It is made by \"HiveMind\" to showcase my talents ^^.\n");
            break;
        case FLAVOR_MENU3:
            printf("Press Y(Manipulation) or Z(Default) key to start using it:");
            break;
        case FLAVOR_EXIT:
            printf("Exiting.....");
            break;
        case FLAVOR_REGISTER1:
            printf("Do you know your name?\n");
            break;
        case FLAVOR_REGISTER2:
            printf("Tell me your name sweetie.. \n");
            break;
        case FLAVOR_START1:
            printf("Good boy, always listen to your mommy.\n");
            break;
        case FLAVOR_CHAOS_NOT_HEAR:
            printf("Are you talking sweetie? I did not hear you. Can you repeat that again?\n");
            break;
        case FLAVOR_CHAOS_WRONG_COMMAND:
            printf("You already told me that, you are so impatient sweetie.\n");
            break;
        case FLAVOR_WHITESPACE:
            printf("\n");
            break;
        case FLAVOR_WHITESPACE2:
            printf("\n\n");
            break;
        case FLAVOR_USER_REPLY1:
            printf("I am: ");
            break;
    }
}

/*
 * UTILITIES
 */

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


void whitespace_remover(char* userName) {
    int read_index = 0;
    int write_index = 0;

    while (userName[read_index] != '\0') {
        if (userName[read_index] != ' ') {
            userName[write_index] = userName[read_index];

            write_index++;
        }

        read_index++;
    }

    userName[write_index] = '\0';

}

const char* random_user_command() {
    const char* words[] = {
        "tellme", "mayileave", "iamhere", "mommy?", "walkwithme",
        "goback", "canihave", "takethe", "letusplayhouse", "openthis", "readthis", "runthisforme", "doxxme"
    };

    return words[rand() % 11];
}