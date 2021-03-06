#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h> 
#include "parser.h"
#include "commands.h"
#include "linenoise.h"
#include "input.h"
#include "errorHandler.h"
#include<readline/readline.h>
#include<readline/history.h>

//screenCleared is defined in commands.h as extern
bool screenCleared;
bool shouldExit;

int main()
{
    bool userInputtedNothing = false;

    system("clear");
    screenCleared = true;
    initShell();
    buildCommandIndex();
    while(!shouldExit)
    {
        // if screen is cleared, no new line is added
        if(!screenCleared && !userInputtedNothing)
            fprintf(stdout,"\n");
        else
        {
            screenCleared = false;
            userInputtedNothing = false;
        }

        char* buffer = getUserInput();
        //if user inputs nothing, skip parsing of commands
        if(buffer == NULL || buffer[0] == 10 || buffer[0] == 0)
        {
            userInputtedNothing = true;
            continue;
        }

        //allows the up and down arrow keys to be used to access preious commands
        using_history();
        add_history(buffer);

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
        bool hasAmpersand = false;
        hasPipeline = checkForPipeline(list);
        hasAmpersand = checkForAmpersand(list);

        //handles variable assigning and reassigning, if detectVarReassigns returns 1, then a variable was modified
        
        //runs commands by using separate processes
        executeCommands(list, hasPipeline, hasAmpersand);

//        if(detectVarReassigns(list) == 1)
//            continue;
        
        free(buffer);
        free(list);
    }
    return 0;
} 