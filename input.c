#include "input.h"
#include <stdlib.h>
#include <stdio.h>
#include <readline/readline.h>
#include <readline/history.h>

char* getUserInput()
{
    char* prompt = getenv("PROMPT");
    char* buffer; // = malloc(sizeof (char) * BUFFER_SIZE);
    buffer = readline(prompt);
    return buffer;
}