#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "commands.h"
#include "parser.h"

extern char **environ;
char** shellVarName;
char** shellVarValue;
int shellVarListSize = 8;

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
    shellVarValue[1] = "smash-1.0: ";
    setenv("PROMPT", "smash-1.0: ", 1);
    shellVarValue[2] = "";
    shellVarValue[3] = getenv("USER");
    shellVarValue[4] = getenv("HOME");
    shellVarValue[5] = getenv("SHELL");
    shellVarValue[6] = "";
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
    hasPipeline = checkForPipeline(list);

    executeCommands(list, hasPipeline);
}

int countInternals() 
{
    return sizeof(commandList) / sizeof(char *);
}

int internalCd(char** args)
{
    if(args[1] == NULL)
        fprintf(stderr, "Error: no arguments provided\n");
    else if(chdir(args[1]) < 0)
        fprintf(stderr, "Error while changing directory\n");

    //returns 1 on success
    return 1;
}

int internalHelp(char** args)
{
    fprintf(stdout ,"\nThis is the smash shell version 1.0");
    fprintf(stdout ,"\nThe following built in commands are supported: cd, help, cwd, clear, echo, showvar, showenv, export, unset, source and exit");
    fprintf(stdout ,"\nTo use, write a command name followed by any arguments\n\n");

    return 1;
}

int internalExit(char** args)
{
    //exits
    exit(EXIT_SUCCESS);
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
    system("reset");
    return 1;
}

int internalEcho(char** args)
{
    int i = 0;
    while(args[i++] != NULL)
        fprintf(stdout,"\n%s ", args[i]);

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
            if(token[0] == '$' && token[0] != '"') 
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
            fprintf(stderr, "Error executing command: command does not exist\n");
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

int executeCommands(char **args, bool hasPipeline)
{
    //checks if no command was entered
    if (args[0] == NULL)
        return 1;

    int j = 0;
    while(args[++j] != NULL)
    {
        if(strcmp(args[j], "=") == 0)
        {
            reassignVar(args[j-1], args[j+1]);
            return 1;
        }
    }
    if(hasPipeline)
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
            args1[i] = args[i];
        for(int i = 1; i <= totalCommandsAfterPipeline; i++)
            args2[i-1] = args[totalCommandsBeforePipeline+i];

        //use built in linux pipes
        //execvp(args1[0], args1);
        return 1;
    }

    //executes internal commands
    for (int i = 0; i < countInternals(); i++) 
    {
        if (strcmp(args[0], commandList[i]) == 0)
            return (*internals[i])(args);
    
    }
    //if command is not an internal command, treats it as a bash command
    return startProcesses(args);
}


