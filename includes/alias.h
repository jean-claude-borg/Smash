#pragma once

void createAliases();
char** checkForAndReplaceAliases(char** tokenList);
int getNumberOfAliases();
void printAliases();
void createNewAlias(char* aliasName, char* aliasValue);