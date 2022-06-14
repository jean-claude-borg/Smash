#include "alias.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "includes/list.h"

char** aliasNames;
char** aliasValues;
int numberOfAliases = 0;

void createAliases()
{
    numberOfAliases = 4;
    aliasNames = malloc(sizeof (char*) * numberOfAliases);
    aliasValues = malloc(sizeof (char*) * numberOfAliases);

    aliasNames[0] = malloc(sizeof (char) * strlen("ls"));
    aliasValues[0] = malloc(sizeof (char) * strlen("ls --color=auto -l"));

    aliasNames[1] = malloc(sizeof (char) * strlen("grep"));
    aliasValues[1] = malloc(sizeof (char) * strlen("grep --colour=auto"));

    aliasNames[2] = malloc(sizeof (char) * strlen("egrep"));
    aliasValues[2] = malloc(sizeof (char) * strlen("egrep --colour=auto"));

    aliasNames[3] = malloc(sizeof (char) * strlen("fgrep"));
    aliasValues[3] = malloc(sizeof (char) * strlen("fgrep --colour=auto"));

    //string literals can not be tokenised by strtok as they are stored in read only memory
    strcpy(aliasNames[0], "ls");
    strcpy(aliasValues[0], "ls --color=auto -l");

    strcpy(aliasNames[1], "grep");
    strcpy(aliasValues[1], "grep --colour=auto");

    strcpy(aliasNames[2], "egrep");
    strcpy(aliasValues[2], "egrep --colour=auto");

    strcpy(aliasNames[3], "fgrep");
    strcpy(aliasValues[3], "fgrep --colour=auto");
}

int getNumberOfAliases()
{
    return numberOfAliases;
}

void printAliasHelp()
{
    fprintf(stdout, "alias: inbuilt command, usage: alias [options] [args]\n");
    fprintf(stdout, "aliasName refers to the word to be replaced (example: ls)\n");
    fprintf(stdout, "aliasValue refers to the words replacing original word (example: ls --color=auto)\n");
    fprintf(stdout, "options: '-n' \t create new alias, usage: alias -n [aliasName] [aliasValue]\n");
    fprintf(stdout, "options: '-r' \t delete an alias, usage: alias -r \"[aliasValue]\"\n");
    fprintf(stdout, "options: '-h' \t prints this help message, usage: alias -h\n");
}

void printAliases()
{
    for(int i = 0; i < getNumberOfAliases(); i++)
    {
        fprintf(stdout, "%5s = '%s'\n",aliasNames[i], aliasValues[i]);
    }
}

void createNewAlias(char* aliasName, char* aliasValue)
{
    numberOfAliases++;
    aliasNames = realloc(aliasNames, sizeof (char*) * numberOfAliases);
    aliasValues = realloc(aliasValues, sizeof (char*) * numberOfAliases);

    aliasNames[numberOfAliases-1] = malloc(sizeof (char) * strlen(aliasName));
    aliasValues[numberOfAliases-1] = malloc(sizeof (char) * strlen(aliasValue));

    strcpy(aliasNames[numberOfAliases-1], aliasName);
    strcpy(aliasValues[numberOfAliases-1], aliasValue);
}

void removeAlias(char* aliasValue)
{
    for(int i = 0; i < numberOfAliases; i++)
    {
        if(strcmp(aliasValues[i], aliasValue) == 0)
        {
            int numberOfAliasesLeft = numberOfAliases - 1 - i;
            for(int j = 0; j < numberOfAliasesLeft; j++)
            {
                strcpy(aliasNames[i+j], aliasNames[i+j+1]);
                strcpy(aliasValues[i+j], aliasValues[i+j+1]);
            }
            free(aliasNames[numberOfAliases - 1]);
            free(aliasValues[numberOfAliases - 1]);
            numberOfAliases--;
            break;
        }
    }
}

char** createNewTokenlistWithAliasValues(char** tokenList, int aliasCounter, int listPos)
{
    //get number of tokens in tokenList
    int tokenListSize = 0;
    char* token = tokenList[0];
    list tokenLinkedList = createList(token);
    while(token != NULL)
    {
        tokenListSize++;
        token = tokenList[tokenListSize];

        if(token !=NULL)
            pushToList(tokenLinkedList, token);
    }

    //make copy of string to be tokenized as strtok is destructive
    char* aliasValuesCopy = malloc(sizeof (char) * strlen(aliasValues[aliasCounter]));
    strcpy(aliasValuesCopy, aliasValues[aliasCounter]);

    char* aliasToken = strtok(aliasValuesCopy, " ");
    list aliasTokenLinkedList = createList(aliasToken);

    while(aliasToken != NULL)
    {
        aliasToken = strtok(NULL, " ");
        if(aliasToken != NULL)
            pushToList(aliasTokenLinkedList, aliasToken);
    }

    //have tokenLinkedList and aliasTokenLinkedList,
    //add aliases to correct index of tokenLinkedList
    for(int i = 0; i < getListLength(aliasTokenLinkedList); i++)
    {
        char* aliasData = getDataFromListIndex(aliasTokenLinkedList, i);
        char* tokenData = getDataFromListIndex(tokenLinkedList, listPos);

        //replaces aliasName with first aliasValue
        if(i == 0)
        {
            replaceDataAtIndex(tokenLinkedList, listPos, aliasData);
        }
        else
            pushToListAt(tokenLinkedList, aliasData, listPos + i);
    }

    //copy tokenLinkedList to char** array
    char** newTokenList = malloc(sizeof (char*) * getListLength(tokenLinkedList));
    for(int i = 0; i < getListLength(tokenLinkedList); i++)
    {
        newTokenList[i] = malloc(sizeof (char) * strlen(getDataFromListIndex(tokenLinkedList, i)));
        strcpy(newTokenList[i], getDataFromListIndex(tokenLinkedList, i));
    }
    newTokenList[getListLength(tokenLinkedList)] = NULL;

    deleteList(aliasTokenLinkedList, &aliasTokenLinkedList);
    deleteList(tokenLinkedList, &tokenLinkedList);
    return newTokenList;
}

char** checkForAndReplaceAliases(char** tokenList)
{
    int listPos = 0;
    char* token = tokenList[listPos];
    while(token != NULL)
    {
        for(int i = 0; i < numberOfAliases; i++)
        {
            if(strcmp(token, aliasNames[i]) == 0)
            {
                tokenList = createNewTokenlistWithAliasValues(tokenList, i, listPos);
            }
        }
        listPos++;
        token = tokenList[listPos];
    }
    return tokenList;
}

