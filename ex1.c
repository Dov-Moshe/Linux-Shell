#include <stdio.h>
#include <string.h>
#include <unistd.h>

int getCommand(char* command);
int isBuiltIn(char* command);
void handleBuiltIn(char* command);
int stop = 0;

int main() {


    while (!stop) {

        printf("$ ");
        char command[100];
        bzero(command, 100);
        getCommand(command);

        if (isBuiltIn(command)) {
            handleBuiltIn(command);
        } else {
            int status = fork();
            if (status < 1) {
                // if the command fork failed
                printf("fork failed\n");
            } else if (status == 0) {
                // the father process
                // check if it is foreground or background

            } else {
                // the son process
                // go to the head of the loop and get a new command
                continue;
            }
        }
    }

    return 0;
}

int getCommand(char* command) {
    char* commandSplit;
    scanf("%[^\n]s", command);
    commandSplit = strtok(command, " ");
    int i = 0;
    while (commandSplit != NULL) {
        commandSplit = strtok (NULL, " ");
        i++;
    }
    return 0;
}

int isBuiltIn(char* command) {
    if (strcmp(command, "jobs") == 0 ||
    strcmp(command, "history") == 0 ||
    strcmp(command, "cd") == 0 ||
    strcmp(command, "exit") == 0)
        return 1;
    return 0;
}

void handleBuiltIn(char* command) {

}
