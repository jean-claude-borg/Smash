#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h> 
#include "input.h"
#include "parser.h"
#include "commands.h"
#include "linenoise.h"

int main()
{   
    initShellVariables();
    while(true)
    {
        //prompt user and store input
        fprintf(stdout, "\n");
        char* buffer = linenoise(getenv("PROMPT"));
        //allows the up and down arrow keys to be used to access preious commands
        linenoiseHistoryAdd(buffer);
        
        //parse input
        char **list = parser(buffer);

        //if the source command is inputted by the user, the following code executes
        if(strcmp(list[0], "source") == 0)
        {
            getSource(list[1]);
            continue;
        }

        //expands any variables which are preceded by a $ sign
        varExpansion(list);
        //unparses and rejoins any tokens which were within quotes
        undoQuotes(list);

        //handles variable assigning and reassigning, if detectVarReassigns returns 1, then a variable was modified
        if(detectVarReassigns(list) == 1)
            continue;
        
        //runs commands by using seperate processes
        executeCommands(list);
        
        free(buffer);
        free(list);
    }

    return 0;
} 