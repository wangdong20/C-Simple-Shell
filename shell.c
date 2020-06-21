//
//  main.c
//  Comp322 lab3
//
//  Created by 王栋 on 2019/9/26.
//  Copyright © 2019 CSUN. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#define MAX_LINE 80 /* The maximum length command */
#define MAX_HISTORY 10  /* The maximum number of commands in history */
#define TRUE 1
#define FALSE 0

int parseInput(char * command, const char* delim, char* args[]) {
    char *s = strdup(command);
    char *token;
    int n = 0;
    for(token = strsep(&s, delim); token != NULL; token = strsep(&s, delim)) {
        if(strlen(token) > 0) {
            args[n] = (char*) malloc(strlen(token) * sizeof(char));
            strcpy(args[n++], token);
        }
    }
    args[n] = NULL;
    free(s);
    return n;
}

int contain(char * str, int length, char c) {
    for(int i = 0; i < length; i++) {
        if(*(str + i) == c) {
            return TRUE;
        }
    }
    return FALSE;
}

void freeMemory(char * args[], int length) {
    for(int i = 0; i < length; i++) {
        if(args[i] != NULL) {
            free(args[i]);
            args[i] = NULL;
        }
    }
}

void freeHistory(char * history[], int size) {
    for(int i = 0; i < size; i++) {
        if(history[i] != NULL) {
            free(history[i]);
            history[i] = NULL;
        }
    }
}

int insertInHistory(char* history[], int size, char * command) {
    if(strcmp(command, "") != 0) {
        char * s = strdup(command);
        if(size < MAX_HISTORY) {
            history[size] = strdup(s);
            free(s);
            return size + 1;
        } else {
            for(int i = 0; i < MAX_HISTORY - 1; i++) {
                free(history[i]);
                history[i] = strdup(history[i + 1]);
            }
            free(history[MAX_HISTORY - 1]);
            history[MAX_HISTORY - 1] = strdup(s);
            free(s);
            return MAX_HISTORY;
        }
    }
    return size;
}

void printHistoryCommand(char* history[], int size) {
    for(int i = size - 1; i >= 0; i--) {
        printf("%d %s", i + 1, history[i]);
    }
}

void execute(char* args[], int index, char* history[], int size, char* command) {
    pid_t pid;
    /* fork a child process */
    pid = fork();
    if (pid < 0) { /* error occurred */
        fprintf(stderr, "Fork Failed");
        freeMemory(args, index);
        freeHistory(history, size);
        return;
    }
    else if (pid == 0) { /* child process */
        if (execvp(*args, args) < 0) {     /* execute the command  */
            printf("*** ERROR: exec failed\n");
            freeMemory(args, index);
            freeHistory(history, size);
            exit(1);
        }
    }
    else { /* parent process */
          /* parent will wait for the child to complete */
        if(contain(command, (int)strlen(command), '&') == FALSE) {
            while (wait(NULL) != pid);       /* wait for completion  */
        }
    }
}

int main(void) {
    char *args[MAX_LINE/2 + 1]; /* command line arguments */
    int should_run = 1; /* flag to determine when to exit program */
    int index = 0;
    int sizeInHistory = 0;
    char command[MAX_LINE];
    char *commandInHistory[MAX_HISTORY];
    while (should_run) {
        printf("osh> ");
        fflush(stdout);
        fgets(command, MAX_LINE, stdin);
        index = parseInput(command, " !&\n", args);
        if(args[0] != NULL && strcmp(args[0], "exit") == 0) {
            should_run = 0;
        } else if(args[0] != NULL && strcmp(args[0], "history") == 0){
            printHistoryCommand(commandInHistory, sizeInHistory);
        } else if(command[0] == '!') {
            if(strcmp(command, "!!\n") == 0) {
                if(sizeInHistory > 0) {
                    freeMemory(args, index);
                    index = parseInput(commandInHistory[sizeInHistory - 1], " !&\n", args);
                    sizeInHistory = insertInHistory(commandInHistory, sizeInHistory, commandInHistory[sizeInHistory - 1]);
                    execute(args, index, commandInHistory, sizeInHistory, command);
                } else {
                    printf("No commands in history.\n");
                }
            } else if(atoi(args[0]) != 0) {
                int num = atoi(args[0]);
                if(num > 0 && num <= sizeInHistory) {
                    freeMemory(args, index);
                    index = parseInput(commandInHistory[num - 1], " !&\n", args);
                    sizeInHistory = insertInHistory(commandInHistory, sizeInHistory, commandInHistory[num - 1]);
                    execute(args, index, commandInHistory, sizeInHistory, command);
                } else {
                    printf("No such command in history.\n");
                }
            } else {
                printf("No such command in history.\n");
            }
        } else {
            execute(args, index, commandInHistory, sizeInHistory, command);
            sizeInHistory = insertInHistory(commandInHistory, sizeInHistory, command);
        }
        memset(command, 0, strlen(command) * sizeof(char));
        freeMemory(args, index);
    }
    freeHistory(commandInHistory, sizeInHistory);
    
    return 0;
}
