#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <malloc.h>
#include <stdlib.h>
#include <limits.h>
#include <wait.h>


int getCommand(int* isInBackground);
int isBuiltIn(char* command);
void handleBuiltIn(char* command[], size_t numOfArg);

void handleCommandJobs(char* command[], size_t numOfArg);
void handleCommandHistory(char* command[], size_t numOfArg);
void handleCommandCd(char* command[], size_t numOfArg);
void handleCommandExit();

void handleEcho(char* command[100], size_t numOfArg);
void updateJobs();

int stop = 0;
char* listCommand[100];
char previousPWD[PATH_MAX];
char currentPWD[PATH_MAX];


// data for commands 'jobs' and 'history'
// in the first byte there is a status ( 0==RUNNING, 1==DONE for child process), and in the next bytes the command
char history[100][101];
int processID[100];
int numberOfCommands = 0;

int main(int argc, char **argv) {


    /****************************
     Loop to run commands one after the other
     ***************************/
    while (!stop) {

        printf("$ ");
        fflush(stdout);
        bzero(listCommand, 100);

        int isInBackground = 0;
        int numOfArg = getCommand(&isInBackground);

        // If the command is 'echo' then it needed to remove the Apostrophes
        if (strcmp(listCommand[0], "echo")==0)
            handleEcho(listCommand, numOfArg);

        // If the command is built-in
        if (isBuiltIn(listCommand[0])) {
            handleBuiltIn(listCommand, numOfArg);
            // Marking the command as DONE
            history[numberOfCommands][0] = '3';
        } else {
            // If the command is not built-in then creating a child process
            pid_t pid;
            pid = fork();
            processID[numberOfCommands] = pid;
            if (pid == 0) {
                // the child process
                int execStatus = execvp(listCommand[0], listCommand);
                if (execStatus < 0) {
                    printf("exec failed\n");
                    exit(0);
                }
            } else if (pid < 0) {
                // if the command fork failed
                printf("fork failed\n");
            } else {
                // the father process
                // check if it is foreground or background
                if (!isInBackground)
                    waitpid(pid, NULL,0);
            }
        }
        numberOfCommands++;

    }

    return 0;
}


/**
 * The function gets command by 'scanf' and checks if the command is in background or not.
 * @param isInBackground
 * @return number of argument
 */

int getCommand(int* isInBackground) {

    char command[100];
    // preparing to get a command
    char* commandSplit;
    // getting a command
    scanf(" %[^\n]s", command);

    /******* for "jobs" and "history" *******/
    // copy the command to history list and check if it is background or foreground
    int len = strlen(command);
    int j;
    for (j = 0; j < len; j++){
        // if '&' exist then the command will run as background
        if (command[j] == '&'){
            history[numberOfCommands][j] = '\0';
            command[j-1] = '\0';
            *isInBackground = 1;
            break;
        }
        history[numberOfCommands][j+1] = command[j];
    }
    history[numberOfCommands][j+2] = '\0';
    /***************************************/


    // split into words
    int i = 0;
    commandSplit = strtok(command, " ");
    while (commandSplit != NULL) {
        listCommand[i] = malloc(strlen((commandSplit+1)) * sizeof(char));
        strcpy(listCommand[i], commandSplit);
        commandSplit = strtok(NULL, " ");
        i++;
    }

    /******* for "jobs" and "history" *******/
    // to know if the command need to be in list jobs. 2 if not, and 0 otherwise
    if (isBuiltIn(command))
        history[numberOfCommands][0] = '2';
    else
        history[numberOfCommands][0] = '0';
    /***************************************/

    return i;
}


/**
 * checking if the command is built-in
 * @param command
 * @return boolean answer
 */
int isBuiltIn(char* command) {
    if (strcmp(command, "jobs") == 0 ||
    strcmp(command, "history") == 0 ||
    strcmp(command, "cd") == 0 ||
    strcmp(command, "exit") == 0)
        return 1;
    return 0;
}


/**
 * If the command is built-in then the function calls to the right function of the command
 * @param command
 * @param numOfArg
 */
void handleBuiltIn(char* command[100], size_t numOfArg) {
    if (strcmp(command[0], "jobs") == 0)
        handleCommandJobs(command, numOfArg);
    else if (strcmp(command[0], "history") == 0) {
        handleCommandHistory(command, numOfArg);
    } else if (strcmp(command[0], "cd") == 0) {
        handleCommandCd(command, numOfArg);
    } else {
        handleCommandExit();
    }

}


/**
 * Function that handle with the command 'jobs'
 * @param command
 * @param numOfArg
 */
void handleCommandJobs(char* command[100], size_t numOfArg) {
    // before printing the jobs it needed to check if there is jobs that done
    updateJobs();
    int i;
    for (i = 0; i < numberOfCommands; i++){
        if (history[i][0] == '0')
            printf("%s\n", history[i]+1);
    }
    history[numberOfCommands][0] = '2';

}


/**
 * Helper function that update the jobs that done
 * @param command
 * @param numOfArg
 */
void updateJobs(){
    int i;
    for (i = 0; i < numberOfCommands; i++) {
        // checking only for commands that are not built-in
        if (history[i][0] == '0') {
            if (waitpid(processID[i], NULL, WNOHANG) !=0)
                history[i][0] = '1';
        }
    }
}


/**
 * Function that handle with the command 'history'
 * @param command
 * @param numOfArg
 */
void handleCommandHistory(char* command[100], size_t numOfArg) {

    // before printing the history it needed to check if there is jobs that done
    updateJobs();
    int i;
    for (i = 0; i < numberOfCommands; i++) {
        // if the status of jobs is 0 then the jobs is running else the jobs done
        if (history[i][0] == '0')
            printf("%s RUNNING\n", history[i]+1);
        else if (history[i][0] == '1' || history[i][0] == '3')
            printf("%s DONE\n", history[i]+1);
    }
    printf("history RUNNING\n");
}


/**
 * Function that handle with the command 'cd'
 * @param command
 * @param numOfArg
 */
void handleCommandCd(char* command[100], size_t numOfArg) {
    // if the command without '-' as argument
    if (command[1] == NULL || strcmp(command[1], "-") != 0) {

        // get the current path to be the previous path
        getcwd(previousPWD, sizeof(previousPWD));
        if (command[2] != NULL)
            printf("Too many arguments\n");

        // if there is no too many arguments
        else if (command[1] == NULL || command[1][0] == '~') {
            // if the command with '~' as argument
            int status = chdir(getenv("HOME"));
            if (status < 0)
                printf("chdir failed\n");
            // if there is continued to the command
            if (command[1] != NULL && command[1][1] != '\0') {
                if (command[1][1] != '/')
                    printf("chdir failed\n");
                else if (command[1][2] != '\0'){
                    command[1][0] = '.';
                    status = chdir(command[1]);
                    if (status < 0) {
                        chdir(previousPWD);
                        printf("chdir failed\n");
                        return;
                    }
                }
            }
        } else if (command[1][0] == '.') {
            // if the command with '..' as argument
            int status = chdir(command[1]);
            if (status < 0)
                printf("chdir failed\n");
        } else {
            int status = chdir(command[1]);
            if (status < 0)
                printf("chdir failed\n");
        }


    }

        // if the command with '-' as argument
    else if (command[1] != NULL && command[1][0] == '-') {
        // go to the previous path
        chdir(previousPWD);
        // switch the current path with previous path (before the changing happened)
        strcpy(previousPWD, currentPWD);
    }

    // getting the current path
    getcwd(currentPWD, sizeof(currentPWD));

}


/**
 * Function that handle with the command 'exit'
 */
void handleCommandExit() {
    exit(0);
}


/**
 * If the command is 'echo' then the function removes the Apostrophes
 * @param command
 * @param numOfArg
 */
void handleEcho(char* command[100], size_t numOfArg) {
    char* t;
    int i;
    for (i = 1; i < numOfArg; ++i) {
        t = strtok(command[i], "\"");
        char temp[100];
        strcpy(temp, t);
        strcpy(command[i], temp);
    }
}
