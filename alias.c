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
    aliasValues[0] = malloc(sizeof (char) * strlen("ls --color=auto"));

    aliasNames[1] = malloc(sizeof (char) * strlen("grep"));
    aliasValues[1] = malloc(sizeof (char) * strlen("grep --colour=auto"));

    aliasNames[2] = malloc(sizeof (char) * strlen("egrep"));
    aliasValues[2] = malloc(sizeof (char) * strlen("egrep --colour=auto"));

    aliasNames[3] = malloc(sizeof (char) * strlen("fgrep"));
    aliasValues[3] = malloc(sizeof (char) * strlen("fgrep --colour=auto"));

    //string literals can not be tokenised by strtok as they are stored in read only memory
    strcpy(aliasNames[0], "ls");
    strcpy(aliasValues[0], "ls --color=auto");

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

void printAliases()
{
    for(int i = 0; i < getNumberOfAliases(); i++)
    {
        fprintf(stdout, "%5s = %s\n",aliasNames[i], aliasValues[i]);
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

    for(int i = 1; i < getListLength(aliasTokenLinkedList); i++)
    {
        char* data = getDataFromListIndex(aliasTokenLinkedList, i);
        pushToListAt(tokenLinkedList, data, listPos + i);
    }

    //copy tokenLinkedList to supplied tokenList
    char** newTokenList = malloc(sizeof (char*) * getListLength(aliasTokenLinkedList));
    for(int i = 0; i < getListLength(aliasTokenLinkedList); i++)
    {
        newTokenList[i] = malloc(sizeof (char) * strlen(getDataFromListIndex(aliasTokenLinkedList, i)));
        strcpy(newTokenList[i], getDataFromListIndex(aliasTokenLinkedList, i));
    }
    newTokenList[getListLength(aliasTokenLinkedList)] = NULL;

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

