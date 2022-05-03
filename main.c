#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h> 
#include "parser.h"
#include "commands.h"
#include "linenoise.h"

int main()
{
    system("clear");
    initShellVariables();
    while(true)
    {
        //prompt user and store input
        char* buffer = linenoise(getenv("PROMPT"));
        //allows the up and down arrow keys to be used to access preious commands
        linenoiseHistoryAdd(buffer);

        //if user inputs nothing, skip parsing of commands
        if(buffer[0] == NULL)
            continue;

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
        //checks for pipeline character
        bool hasPipeline = false;
        hasPipeline = checkForPipeline(list);

        //handles variable assigning and reassigning, if detectVarReassigns returns 1, then a variable was modified
        if(detectVarReassigns(list) == 1)
            continue;
        
        //runs commands by using seperate processes
        executeCommands(list, hasPipeline);
        
        free(buffer);
        free(list);
    }

    return 0;
} 