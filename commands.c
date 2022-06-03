#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "commands.h"
#include "parser.h"
#include "errorHandler.h"
#include "alias.h"

#pragma clang diagnostic push
#pragma ide diagnostic ignored "OCUnusedMacroInspection"

extern char **environ;
char** shellVarName;
char** shellVarValue;
int shellVarListSize = 8;

#define ANSI_COLOR_RED     "\x1b[31m\0"
#define ANSI_COLOR_GREEN   "\x1b[32m\0"
#define ANSI_COLOR_YELLOW  "\x1b[33m\0"
#define ANSI_COLOR_BLUE    "\x1b[34m\0"
#define ANSI_COLOR_MAGENTA "\x1b[35m\0"
#define ANSI_COLOR_CYAN    "\x1b[36m\0"
#define ANSI_COLOR_RESET   "\x1b[0m\0"
#define ANSI_COLOR_RESET_BOLD   "\x1b[22m\0"
#define ANSI_COLOR_BOLD   "\x1b[1m\0"
#define ANSI_COLOR_INVERSE   "\x1b[7m\0"
#define ANSI_COLOR_RESET_INVERSE   "\x1b[27m\0"
#define ANSI_COLOR_ITALIC   "\x1b[3m\0"
#define ANSI_COLOR_UNDERLINE   "\x1b[4m\0"
#define ANSI_COLOR_UNDERLINE_RESET   "\x1b[24m\0"
#define ANSI_COLOR_ROOT_PATH   "\x1b[38;5;252m\0"

#define HOST_NAME_MAX 20
char* prompt;
char tempPath[1024];

char* getRootPath() {
    getcwd(tempPath, 1024);
    int pathLength = (int)strlen(tempPath);
    int slashPosition = 0;
    for (int i = 0; i < pathLength; i++) {
        if (tempPath[i] == 47) {
            slashPosition = i;
        }
    }
    char *path = &tempPath[slashPosition + 1];
    char* homeWithSlash = getenv("HOME");
    char* home = &homeWithSlash[6];
    if (strcmp(home, path) == 0)
    {
        tempPath[slashPosition+1] = 126;
        tempPath[slashPosition+2] = 0;
    }
    return &tempPath[slashPosition+1];
}

void setPrompt()
{
    //set prompt
    char* userName = getenv("USERNAME");
    char hostName[HOST_NAME_MAX];
    char* bold = ANSI_COLOR_BOLD;
    char* reset = ANSI_COLOR_RESET;
    char* resetBold = ANSI_COLOR_RESET_BOLD;
    char* rootColour = ANSI_COLOR_ROOT_PATH;
    char* rootPath = getRootPath();
    gethostname(hostName, HOST_NAME_MAX);
    prompt = malloc(
            strlen(bold)
            + strlen ("[")
            + strlen(userName)
            + strlen("@")
            + HOST_NAME_MAX
            + strlen(" ")
            + strlen(rootColour)
            + strlen(rootPath)
            + strlen(reset)
            + strlen(bold)
            + sizeof ("]")
            + sizeof (": ")
            + strlen(reset) + 1); //1 for null terminator

    prompt[0] = 0; //strcat concatenates to a null terminator, so first character of prompt should be null terminator
    strcat(prompt, bold);
    strcat(prompt, "[");
    strcat(prompt,userName);
    strcat(prompt, "@");
    strcat(prompt, hostName);
    strcat(prompt, " ");
    strcat(prompt, rootColour);
    strcat(prompt, rootPath);
    strcat(prompt, reset);
    strcat(prompt, bold);
    strcat(prompt,"]");
    strcat(prompt,"$ ");
    strcat(prompt,resetBold);
    setenv("PROMPT", prompt, 1);
    free(prompt);
}

void initShell()
{
    setPrompt();
    //set gnome-terminal window title
    printf ("\e]2;Smash-1.2\a");
    //init shell variables
    initShellVariables();
    createAliases();
}

void initShellVariables()
{
    shellVarName = malloc(sizeof(char*) * shellVarListSize);
    shellVarName[0] = "PATH";
    shellVarName[1] = "PROMPT";
    shellVarName[2] = "CWD";
    shellVarName[3] = "USER";
    shellVarName[4] = "HOME";
    shellVarName[5] = "SHELL";
    shellVarName[6] = "TERMINAL";
    shellVarName[7] = "EXITCODE";
    
    shellVarValue = malloc(sizeof(char*) * shellVarListSize);
    shellVarValue[0] = getenv("PATH");
    shellVarValue[1] = prompt;
    shellVarValue[2] = getenv("PWD");
    shellVarValue[3] = getenv("USER");
    shellVarValue[4] = getenv("HOME");
    shellVarValue[5] = getenv("SHELL");
    shellVarValue[6] = getenv("TERM");
    shellVarValue[7] = "";
}

char* commandList[] = {"cd", "exit", "help", "cwd", "clear", "echo", "showvar", "showenv", "export", "unset"};

int (*internals[])(char **) = 
{
    &internalCd,
    &internalExit,
    &internalHelp,
    &internalCwd,
    &internalClear,
    &internalEcho,
    &internalShowvar,
    &internalShowenv,
    &internalExport,
    &internalUnset
};

void processCommandsFromFile(char* buffer)
{
    char **list = parser(buffer);

    if(detectVarReassigns(list) == 1)return;
    varExpansion(list);
    undoQuotes(list);
    bool hasPipeline = false;
    bool hasAmpersand = false;
    hasPipeline = checkForPipeline(list);
    hasAmpersand = checkForAmpersand(list);

    executeCommands(list, hasPipeline, hasAmpersand);
}

int countInternals() 
{
    return sizeof(commandList) / sizeof(char *);
}

int internalCd(char** args)
{
    if(args[1] == NULL) { fprintf(stderr, "Error: no arguments provided\n"); return 0;}
    else if(chdir(args[1]) < 0) { fprintf(stderr, "Error: direcotry does not exist\n"); return 0;}

    //updates prompt
    setPrompt();

    //returns 1 on success
    return 1;
}

int internalHelp(char** args)
{
    fprintf(stdout ,"\nSmash shell version 1.2");
    fprintf(stdout ,"\n\nBuiltin commands: \n\ncd\nhelp\ncwd\nclear\necho\nshowvar\nshowenv\nexport\nunset\nsource\nexit\n");
    fprintf(stdout ,"\nAll the normal linux commands are also supported");
    fprintf(stdout ,"\nTo use, write a command name followed by any arguments\n\n");

    return 1;
}

int internalExit(char** args)
{
    //exits
    //exit(EXIT_SUCCESS);
    shouldExist = true;
    return 0;
}

int internalCwd(char** args)
{
    char dir[200];
    getcwd(dir, 200);
    printf("%s \n", dir);
    return 1;
}

int internalClear(char** args)
{
    //fprintf(stdout, " ");
    system("clear");
    screenCleared = true;
    return 1;
}

int internalEcho(char** args)
{
    for(int i = 1; args[i] != NULL; i++) {
        fprintf(stdout, "%s\n ", args[i]);
    }

    return 1;
}

int internalShowvar(char** args)
{
    int i = 0;
    if(args[1] == NULL)
        for(int i = 0; i < shellVarListSize; i++)
            fprintf(stdout, "\n%-12s = %s", shellVarName[i], shellVarValue[i]);
        
    else
    {
        for(int j = 0; j < shellVarListSize; j++)
            if(strcmp(args[1], shellVarName[j]) == 0)
                fprintf(stdout, "%s ", shellVarValue[j]);
    }
    fprintf(stdout, "\n");

    return 1;
}

int internalShowenv(char** args)
{
    int i = 0;

    if(args[1] == NULL)
        for (char **env = environ; *env; ++env)
            printf("%s\n", *env);
    
    else while(args[++i] != NULL)
        fprintf(stdout, "%s ", getenv(args[i]));

    return 1;
}

int internalExport(char** args)
{
    for(int j = 0; j < shellVarListSize; j++)
        if(strcmp(args[1], shellVarName[j]) == 0)
            setenv(shellVarName[j], shellVarValue[j], 1);
    
    return 1;
}

int internalUnset(char** args)
{
    for(int j = 0; j < shellVarListSize; j++)
        if(strcmp(args[1], shellVarName[j]) == 0)
            for(;j < shellVarListSize; j++)
            {
                shellVarName[j] = shellVarName[j+1];
                shellVarValue[j] = shellVarValue[j+1];

                shellVarName = realloc(shellVarName, sizeof(char*) * shellVarListSize - sizeof(char*));
                shellVarValue = realloc(shellVarValue, sizeof(char*) * shellVarListSize - sizeof(char*));
                shellVarListSize--;
            }

    unsetenv(args[1]);
    return 1;
}

int getSource(char* filePath)
{
    FILE *file = fopen(filePath, "r");
        if (file == NULL)
        {
            puts("File does not exist");
            return 1;
        }
        while (!feof(file))
        {
            char *line = NULL;
            size_t len = 0;
            while (getline(&line, &len, file) != -1)
            {
                line[strcspn(line, "\n")] = 0;
                processCommandsFromFile(line);
            }
            fclose(file);
            return 1;
        }
}

int varExpansion(char** tokenList)
{
    int listSize = 0;
    //calculating amount of tokens in list
    while(tokenList[listSize] != NULL)
        listSize++;
    
    for(int listPos = 0; listPos < listSize; listPos++)
    {
        for(int j = 0; j < strlen(tokenList[listPos]); j++)
        {
            char* token = tokenList[listPos];
            //only expands variable if not in quotation marks
            if(token[0] == '$' && token[-1] != '"')
            {   
                //removes dollar sign from string
                for(int i = 0; i < strlen(token); i++)
                {
                    token[i] = token[i+1];
                }
                token[strlen(token)] = '\0';
                
                //checks if variable is an existing shell variable 
                for(int i = 0; i < shellVarListSize; i++)
                {
                    if(strcmp(token, shellVarName[i]) == 0)
                    {
                        token = shellVarValue[i];
                        tokenList[listPos] = shellVarValue[i];
                    }
                }
            }
        }
    }
}

int reassignVar(char* var, char* newValue)
{
    bool varExists = false;
    for(int i = 0; i < shellVarListSize; i++)
    {
        if(strcmp(var, shellVarName[i]) == 0)
        {
            shellVarValue[i] = newValue;
            fprintf(stdout, "%s = %s\n", shellVarName[i], shellVarValue[i]);
            varExists = true;
        }
    }

    if(varExists == false)
    {
        shellVarName = realloc(shellVarName, (sizeof(char*) * shellVarListSize) + sizeof(char*));
        shellVarValue = realloc(shellVarValue, (sizeof(char*) * shellVarListSize) + sizeof(char*));
        shellVarListSize++;

        shellVarName[shellVarListSize-1] = var;
        shellVarValue[shellVarListSize-1] = newValue;

        //fprintf(stdout, "%s = %s\n", var, newValue);
    }
    
    return 1;
}

int detectVarReassigns(char** tokenList)
{
    int listSize = 0;
    //calculating amount of tokens in list
    while(tokenList[listSize] != NULL)
        listSize++;

    for(int i = 0; i < listSize; i++)
    {
        for(int j = 0; j < strlen(tokenList[i]); j++)
        {
            char* token = tokenList[i];
            if(token[j] == '=')
            {
                char* varName;
                char* newValue;
                char* returnValue;

                returnValue = strtok(token, "=");
                varName = returnValue;
                                
                while(returnValue != NULL) 
                {
                    returnValue = strtok(NULL, "=");

                    if(returnValue != NULL)
                        newValue = returnValue;
                }
                reassignVar(varName, newValue);

                //returns 1 if an = was detected and a shell variable was changed
                return 1;
            }
        }
    }
    //if nothing was done, returns 0
    return 0;
}

int executeCommands(char **args, bool hasPipeline, bool hasAmpersand)
{
    //checks if no command was entered
    if (args[0] == NULL)
        return 1;

//    int j = 0;
//    while(args[++j] != NULL)
//    {
//        if(strcmp(args[j], "=") == 0)
//        {
//            reassignVar(args[j-1], args[j+1]);
//            return 1;
//        }
//    }
    if(hasPipeline)
    {
        executeCommandsWithPipeline(args);
        return 1;
    }
    else if(hasAmpersand)
    {
        executeCommandsWithAmpersand(args);
        return 1;
    }
    else { executeInternalCommands(args); }
    return 1;
}

int executeCommandsWithPipeline(char**  args)
    {
        int totalCommandsBeforePipeline = 0;
        int totalCommandsAfterPipeline = 0;
        int counter = 0;
        while(strcmp(args[counter], "|") != 0)
        {
            counter++;
            totalCommandsBeforePipeline++;
        }
        //skips over the pipeline character
        counter++;
        while(args[counter] != NULL)
        {
            counter++;
            totalCommandsAfterPipeline++;
        }

        //execvp(args[0], totalCommandsBeforePipeline-1);

        char**args1 = malloc(sizeof(char*) * totalCommandsBeforePipeline);
        char**args2 = malloc(sizeof(char*) * totalCommandsAfterPipeline);

        //splits list into 2, one list for before pipeline, one for after
        for(int i = 0; i < totalCommandsBeforePipeline; i++)
        {
            args1[i] = args[i];
            //adds a null character after the last command in args1, so that execlp works properly
            if(i == totalCommandsBeforePipeline-1){args1[i+1] = NULL;}
        }
        for(int i = 1; i <= totalCommandsAfterPipeline; i++)
            args2[i-1] = args[totalCommandsBeforePipeline+i];

        //use built in linux pipes
        pid_t pid;
        int fd[2];

        pipe(fd);
        pid = fork();
        #define READ_END 0
        #define WRITE_END 1

        if(pid < 0)
        {
            fprintf(stderr, "Error forking process\n");
            exit(EXIT_FAILURE);
        }

        if(pid==0)
        {
            dup2(fd[WRITE_END], STDOUT_FILENO);
            close(fd[WRITE_END]);
            close(fd[READ_END]);

            bool internalCommandFound = false;
            //if internal command is detected, it is run and execvp is skipped
            for (int i = 0; i < countInternals(); i++) {
                if (strcmp(args1[0], commandList[i]) == 0) {
                    (*internals[i])(args1);
                    internalCommandFound = true;
                }
            }
            if(!internalCommandFound)
            {
//                int execResult = execlp(*args1, *args1, args1[1], (char*) NULL);
                int execResult = execvp(args1[0], args1);
                if (execResult < 0) {
                    //fprintf(stderr, "Error executing command: command does not exist\n");
                    handleCommandNotFound(args1[0]);
                    exit(EXIT_FAILURE);
                }
            }
            exit(EXIT_SUCCESS);
        }
        else {
            //wait for all children of parent process to end before forking again
            int childStatus;
            waitpid(-1, &childStatus, WIFEXITED(-1));

            //second fork which spawns second process to execute the commands after pipeline character
            pid = fork();

            if (pid == 0) {
                dup2(fd[READ_END], STDIN_FILENO);
                close(fd[WRITE_END]);
                close(fd[READ_END]);

                bool internalCommandFound = false;
                //if internal command is detected, it is run and execvp is skipped
                for (int i = 0; i < countInternals(); i++) {
                    if (strcmp(args2[0], commandList[i]) == 0) {
                        (*internals[i])(args2);
                        internalCommandFound = true;
                    }
                }
                if(!internalCommandFound)
                {
//                    int execResult = execlp(*args2, *args2, args2[1], (char *) NULL);
                    int execResult = execvp(args2[0], args2);
                    if (execResult < 0) {
                        //fprintf(stderr, "Error executing command: command does not exist\n");
                        handleCommandNotFound(args2[0]);
                        perror("error: ");
                        exit(EXIT_FAILURE);
                    }
                }
                exit(EXIT_SUCCESS);
            } else {
                int status;
                close(fd[READ_END]);
                close(fd[WRITE_END]);
                waitpid(pid, &status, 0);
            }
        }
        return 1;
    }

int executeCommandsWithAmpersand(char**  args)
{
    int totalCommandsBeforeAmpersand = 0;
    int totalCommandsAfterAmpersand = 0;
    int counter = 0;
    while(strcmp(args[counter], "&&") != 0)
    {
        counter++;
        totalCommandsBeforeAmpersand++;
    }
    while(args[counter] != NULL)
    {
        counter++;
        totalCommandsAfterAmpersand++;
    }

    char**args1 = malloc(sizeof(char*) * totalCommandsBeforeAmpersand);
    char**args2 = malloc(sizeof(char*) * totalCommandsAfterAmpersand);

    //splits list into 2, one list for before pipeline, one for after
    for(int i = 0; i < totalCommandsBeforeAmpersand; i++) {
        args1[i] = args[i];
        //adds a null character after the last command in args1, so that execvp works properly
        if(i == totalCommandsBeforeAmpersand-1){args1[i+1] = NULL;}
    }
    for(int i = 1; i <= totalCommandsAfterAmpersand; i++)
        args2[i-1] = args[totalCommandsBeforeAmpersand + i];

    pid_t pid = fork();
    int childStatus;

    if(pid < 0)
    {
        fprintf(stderr, "Error forking process\n");
        exit(EXIT_FAILURE);
    }
        //checks for child process and calls execvp
    else if(pid == 0)
    {
        bool internalCommandFound = false;
        //if internal command is detected, it is run and execvp is skipped
        for (int i = 0; i < countInternals(); i++) {
            if (strcmp(args1[0], commandList[i]) == 0) {
                (*internals[i])(args1);
                internalCommandFound = true;
            }
        }
        if(!internalCommandFound)
        {
            int execResult = execvp(args1[0], args1);
            if (execResult < 0) {
                //fprintf(stderr, "Error executing command: command does not exist\n");
                handleCommandNotFound(args1[0]);
                exit(EXIT_FAILURE);
            }
        }
        exit(EXIT_SUCCESS);
    }
        //checks for parent process and makes it wait for child process to complete
    else
    {
        pid_t wpid;
        do
        {
            wpid = waitpid(pid, &childStatus, WUNTRACED);
        } while (!WIFEXITED(childStatus) && !WIFSIGNALED(childStatus));
    }

    pid_t child2Pid = fork();
    if(child2Pid < 0)
    {
        fprintf(stderr, "Error forking process\n");
        exit(EXIT_FAILURE);
    }
    else if(child2Pid == 0)
    {
        bool internalCommandFound = false;
        //if internal command is detected, it is run and execvp is skipped
        for (int i = 0; i < countInternals(); i++) {
            if (strcmp(args2[0], commandList[i]) == 0) {
                (*internals[i])(args2);
                internalCommandFound = true;
            }
        }
        if(!internalCommandFound)
        {
            int execResult = execvp(args2[0], args2);
            if (execResult < 0) {
                handleCommandNotFound(args2[0]);
                //fprintf(stderr, "Error executing command: command does not exist\n");
                exit(EXIT_FAILURE);
            }
        }
        exit(EXIT_SUCCESS);
    }
    else
    {
        pid_t wpid;
        do
        {
            wpid = waitpid(child2Pid, &childStatus, WUNTRACED);
        } while (!WIFEXITED(childStatus) && !WIFSIGNALED(childStatus));
    }

    return 0;
}

int executeInternalCommands(char** args) {    //executes internal commands
        for (int i = 0; i < countInternals(); i++) {
            if (strcmp(args[0], commandList[i]) == 0)
                return (*internals[i])(args);
        }
        //if command is not an internal command, treats it as a bash command
        return startProcesses(args);
    }

int startProcesses(char **args)
{
    pid_t pid = fork();
    int childStatus;

    //checks if fork was successful
    if(pid < 0)
    {
        fprintf(stderr, "Error forking process\n");
        exit(EXIT_FAILURE);
    }
        //checks for child process and calls execvp
    else if(pid == 0)
    {
        int execResult = execvp(args[0], args);
        if(execResult < 0)
        {
            //fprintf(stderr, "%s: command not found, type help for command list\n", args[0]);
            handleCommandNotFound(args[0]);
            exit(EXIT_FAILURE);
        }
    }
        //checks for parent process and makes it wait for child process to complete
    else
    {
        pid_t wpid;
        do
        {
            wpid = waitpid(pid, &childStatus, WUNTRACED);
        } while (!WIFEXITED(childStatus) && !WIFSIGNALED(childStatus));
    }
    return 1;
}


#pragma clang diagnostic pop