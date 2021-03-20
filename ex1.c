#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <malloc.h>
#include <stdlib.h>

int getCommand();
int isBuiltIn(char* command);
void handleBuiltIn(char* command[], size_t numOfArg);
void freeCommand();

void handleCommandJobs(char command[][100], size_t numOfArg);
void handleCommandHistory(char command[][100], size_t numOfArg);
void handleCommandCd(char command[][100], size_t numOfArg);
void handleCommandExit(char command[][100], size_t numOfArg);

int stop = 0;
char* listCommand[100];

int main(int argc, char **argv) {

    atexit(freeCommand);

    while (!stop) {
        char *args[] = {"ls", "-r", "-t", "-l", NULL };
        //int s = execvp("ls", args);
        //printf("%d\n", s);

        printf("$ ");
        fflush(stdout);
        bzero(listCommand, 100);

        //char * l = *list;


        int numOfArg = getCommand();
        //int s = execvp(listCommand[0], listCommand);

        /**********/
        //stop = 1;
        /**********/

        if (isBuiltIn(listCommand[0])) {
            handleBuiltIn(listCommand, numOfArg);
        } else {
            //execvp(listCommand[0],listCommand);
            pid_t pid;

            if ((pid = fork())==0) {
                // the child process
                int s = execvp(listCommand[0], listCommand);
                if (s<0)
                    printf("exec failed\n");
            } else if (pid < 0) {
                // if the command fork failed
                printf("fork failed\n");
            } else {
                // the father process
                // check if it is foreground or background
                usleep(100000);
            }
        }
    }

    return 0;
}

int getCommand() {

    char command[100];
    // preparing to get a command
    char* commandSplit;
    // getting a command
    scanf(" %[^\n]s", command);
    // split into words
    int i = 0;
    commandSplit = strtok(command, " ");
    while (commandSplit != NULL) {

        listCommand[i] = malloc(strlen((commandSplit+1)) * sizeof(char));
        strcpy(listCommand[i], commandSplit);
        printf("%s\n", commandSplit);
        commandSplit = strtok(NULL, " ");
        i++;
    }
    printf("%d\n",i);
    return i;
}

int isBuiltIn(char* command) {
    if (strcmp(command, "jobs") == 0 ||
    strcmp(command, "history") == 0 ||
    strcmp(command, "cd") == 0 ||
    strcmp(command, "exit") == 0)
        return 1;
    return 0;
}

void handleBuiltIn(char* command[], size_t numOfArg) {
    /*if (strcmp(command[0], "jobs") == 0)
        handleCommandJobs(command, numOfArg);
    else if (strcmp(command[0], "history") == 0) {
        handleCommandHistory(command, numOfArg);
    } else if (strcmp(command[0], "cd") == 0) {
        handleCommandCd(command, numOfArg);
    } else {
        handleCommandExit(command, numOfArg);
    }*/

}

void handleCommandJobs(char command[][100], size_t numOfArg) {

}

void handleCommandHistory(char command[][100], size_t numOfArg) {
    printf("%s\n", command[1]);

}

void handleCommandCd(char command[][100], size_t numOfArg) {

}

void handleCommandExit(char command[][100], size_t numOfArg) {

}


void freeCommand() {
    printf("hey\n");
    int i = 0;
    while (listCommand[i] != NULL) {
        free(listCommand[i]);
    }
}




/*int getCommand(char listCommand[]) {

    // preparing to get a command
    char command[100];
    bzero(command, 100);
    char* commandSplit;
    // getting a command
    scanf("%[^\n]s", listCommand);
    // split into words
    int i = 0;
    commandSplit = strtok(listCommand, " ");
    while (commandSplit != NULL) {
        //strcpy(listCommand[i], commandSplit);
        printf("%s\n", commandSplit);
        commandSplit = strtok(NULL, " ");
        i++;
    }
    printf("%d\n",i);
    return i;
}*/