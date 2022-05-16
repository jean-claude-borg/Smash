#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>

#define READ_END 0
#define WRITE_END 1
#define COMMAND_MAX_LENGTH 50

char** commandIndex;

void buildCommandIndex()
{
    system("cd /home/jean/dev/smash && ./buildCommandIndex");
    FILE* file = fopen("/home/jean/dev/smash/commandIndex.txt", "r");

    if(file == NULL)
    {
        printf("Error opening file");
        exit(EXIT_FAILURE);
    }

    int numberOfCommands = 0;
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
        commandIndex[i] = malloc(COMMAND_MAX_LENGTH);
        fgets(commandIndex[i], COMMAND_MAX_LENGTH, file);
    }
    //fclose causes weird problems
//    fclose(file);
}