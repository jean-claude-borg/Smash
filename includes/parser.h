#pragma once

char **parser(char *input);
char **undoQuotes(char** tokenList);
char **removeQuotes(char** tokenList);
bool checkForPipeline(char** tokenList);
bool checkForAmpersand(char** tokenList);