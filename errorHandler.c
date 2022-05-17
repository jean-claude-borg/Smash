#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include "errorHandler.h"

#define READ_END 0
#define WRITE_END 1
#define COMMAND_MAX_LENGTH 50

char** commandIndex;
int numberOfCommands = 0;

void buildCommandIndex()
{
    system("cd /home/jean/dev/smash && ./buildCommandIndex");
    FILE* file = fopen("/home/jean/dev/smash/commandIndex.txt", "r");

    if(file == NULL)
    {
        printf("Error opening file");
        exit(EXIT_FAILURE);
    }

//    numberOfCommands = 0;
    int ch = getc(file);
    while (ch != EOF)
    {
        //putchar(ch);
        ch = getc(file);
        if(ch == 10)
            numberOfCommands++;
    }

    commandIndex = malloc(sizeof(char*) * numberOfCommands);

    fseek(file,0,SEEK_SET);
    for(int i = 0; i < numberOfCommands; i++)
    {
        commandIndex[i] = malloc(sizeof (char) * COMMAND_MAX_LENGTH);
        fgets(commandIndex[i], COMMAND_MAX_LENGTH, file);
    }
    //fclose causes weird problems
    // fclose(file);
}

void handleCommandNotFound(char* word)
{
    char* closestWord = calculateWordSimiliarity(word);
    fprintf(stdout, "\n%s: command not found, did you mean %s", word, closestWord);
}

char* calculateWordSimiliarity(char* word)
{
    int* scoreForEachCommand = calloc(numberOfCommands, sizeof (int));
    int wordLength = (int)strlen(word);
    for(int i = 0; i < numberOfCommands; i++)
    {
        int commandLength = (int)strlen(commandIndex[i]);
        for(int wordPtr = 0; wordPtr < wordLength; wordPtr++)
        {
            if(word[wordPtr] == commandIndex[i][wordPtr] && wordPtr <= commandLength)
            {
                scoreForEachCommand[i]+=200;
            }
            else
            {
                if(wordPtr <= commandLength)
                {
                    scoreForEachCommand[i] -= abs(word[wordPtr] + commandIndex[i][wordPtr]);
                }
            }
            if(wordPtr == wordLength-1)
            {
                scoreForEachCommand[i] -= (abs(commandLength - wordLength) * 10);
            }
        }
    }

    int largest = 0;
    int index = 0;
    for(int i = 0; i < numberOfCommands; i++)
    {
        if(scoreForEachCommand[i] > largest)
        {
            largest = scoreForEachCommand[i];
            index = i;
        }
    }
    return commandIndex[index];
}