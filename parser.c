#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include "parser.h"

char* cleanInput(char* buffer)
{
//    int positionOfNewLine = 0;
//    while(buffer[positionOfNewLine] != 10)
//    {
//        positionOfNewLine++;
//    }
//    buffer[positionOfNewLine] = 0;
//    return buffer;
}

char **parser(char *input2)
{
    char* input = cleanInput(input2);

    int bufferSize = 64;
    int bufferIncrements = 64;
    int counter = 0;
    //tokenises words which are seperated by a space
    char* delimiter = " ";

    char *token;
    char **tokenList = malloc(bufferSize * sizeof(char*));
    if(!tokenList)
    {
        fprintf(stderr, "Error allocating memory to token pointer array");
        exit(EXIT_FAILURE);
    }
    
    //strtok is used to tokenise
    token = strtok(input, delimiter);
    while(token != NULL)
    {
        tokenList[counter] = token;
        counter++;

        //dynamically allocates more memory if the input is larger than the buffer
        if(counter >= bufferSize)
        {
            bufferSize += bufferIncrements;
            tokenList = realloc(tokenList, bufferSize * sizeof(char*));
        }
        token = strtok(NULL, delimiter);
    }
    //prints tokenList for debugging
    for(int i = 0; i < 10; i++)
    {
        //printf("\n%s", tokenList[i]);
    }
    printf("\n");
    tokenList[counter] = NULL;
    return tokenList;
}

char **undoQuotes(char** tokenList)
{
    //location of first quote
    int firstQuote = 0;
    bool firstQuoteFound = false;
    //location of second quote
    int secondQuote = 0;
    int listSize = 0;
    //calculating amount of tokens in list
    while(tokenList[listSize] != NULL)
        listSize++;

    for(int i = 0; i < listSize; i++)
    {
        if(tokenList[i][0] == '"' && !firstQuoteFound) 
        {
            firstQuote = i;
            firstQuoteFound = true;
        }
        else if(tokenList[i][strlen(tokenList[i]) -1] == '"' && firstQuoteFound) 
            secondQuote = i;
    }

    //quotes are not needed anymore so they are removed
    removeQuotes(tokenList);

    //concatenates all the strings in quotes to the first string in the quotes
    //int i = 0;
//    if(secondQuote - firstQuote > 1)
//    {
//        while(secondQuote - i > firstQuote)
//        {
//            strcat(tokenList[secondQuote - i], "_");
//            strncat(tokenList[secondQuote - i - 1], tokenList[secondQuote-i], strlen(tokenList[secondQuote-i]));
//            i++;
//        }
//
//        //clears the rest of the tokenlist of the redundant strings
//        for(int counter = 1; counter <= i; counter++)
//            tokenList[firstQuote+counter] = NULL;
//
//    }
    if(secondQuote - firstQuote > 0)
    {
        int i;
        for (i = 1; firstQuote + i <= secondQuote; i++) {
            strcat(tokenList[firstQuote], " ");
            strcat(tokenList[firstQuote], tokenList[firstQuote + i]);
        }
        for (int counter = 1; counter <= i; counter++)
            tokenList[firstQuote + counter] = NULL;
    }
}

char **removeQuotes(char** tokenList)
{
    int listSize = 0;
    //calculating amount of tokens in list
    while(tokenList[listSize] != NULL)
        listSize++;

    //removes quotes as they are not needed anymore
    for(int i = 0; i < listSize; i++)
    {
        for(int j = 0; j < strlen(tokenList[i]); j++)
        {
            char* token = tokenList[i];
            if(token[j] == '"')
            {
                int stringPos = j;
                while(stringPos < strlen(tokenList[i]))
                {
                    token[stringPos] = token[stringPos + 1];
                    stringPos++;
                }
            }
        }
    }
}

bool checkForPipeline(char** tokenList)
{
    int listSize = 0;
    //calculating amount of tokens in list
    while(tokenList[listSize] != NULL)
        listSize++;

    for(int i = 0; i < listSize; i++)
    {
        if(strcmp(tokenList[i], "|") == 0)
            return true;
    }
    return false;
}

bool checkForAmpersand(char** tokenList)
{
    int listSize = 0;
    //calculating amount of tokens in list
    while(tokenList[listSize] != NULL)
        listSize++;

    for(int i = 0; i < listSize; i++)
    {
        if(strcmp(tokenList[i], "&&") == 0)
            return true;
    }
    return false;
}