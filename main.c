#include "functions.h"
#include "helperfunc.h"
#include "sighandler.h"
#include "builtIn.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <readline/readline.h>
#include <readline/history.h>

int main(){
    
    // initialize interactive variables to use history functions
    using_history();
    stifle_history(HISTORY_NUMS);

    // used for current command
    char *input, *argv[ARGM_NUM];

    // manages multiple commands in case of pipe or ;
    char ***argvPtr = NULL;

    // handles history and other builtIn commands
    char **commandHistory[HISTORY_NUMS] = { NULL };
    int commandHistoryCounter = 0, commandHistoryCharNum[HISTORY_NUMS] = { 0 };
    BUILTIN builtinCommand = NONE;

    // handles aliases
    struct alias *myAliases[ALIASES_NUM];
    int aliasCounter = 0;
    initAlias(myAliases);
    
    while(1){
        // shell will ignore CTRL+C and CTRL+Z and pass to any process that is running in it
        ignore();
        int argc = 0, status;

        // handles ";" and "&" characters
        int semicln_counter = 0, bgprocesses_counter = 0, bgprocesses_countertmp = 0;
        int semicln_found[ARGM_NUM] = { 0 }, bgprocesses[ARGM_NUM] = { 0 };
        
        // in case of wild characters make new argv
        char **argvNew = NULL;
        int argcNew = 0;

        if ( waitpid(-1, &status, WNOHANG) > 0)
            printf("Backcground command finished\n");

        // read input
        if ( ( input = readInput() ) == NULL ){
            continue;
        }
        
        // tokenize input
        getToken(input, &argc, argv);

        add_myhistory(argc, argv, commandHistory, &commandHistoryCounter, commandHistoryCharNum);

        // first check if any built in command has been put
        builtinCommand = checkBuiltIn(argc, argv);
        if (builtinCommand == HIST){
            printHistory(commandHistory, &commandHistoryCounter, commandHistoryCharNum); // invoked when writing command "myHistory"
        }
        // execute command or commands in "myHistory X" (1 <= X <= HISTORY_NUMS)
        else if (builtinCommand == HIST_SPECIFIC){
            char *ptr;
            int tempargc = strtol(argv[1], &ptr, 10);

            char *argvtmp[ARGM_NUM];
            int argctmp = 0;
            
            // make a copy of command in history, so as to call AliastoArgv() with the copy
            // which will get altered if any aliases are found
            copyToken(&commandHistoryCharNum[tempargc-1], commandHistory[tempargc-1], &argctmp, argvtmp);

            printCommand(commandHistoryCharNum[tempargc-1], commandHistory[tempargc-1]);

            AliastoArgv(myAliases, argvtmp, &argctmp);

            argvNew = checkWildCharacters(argctmp, argvtmp, &argcNew);

            if (argcNew > 0){
                argvPtr = checkMultipleCommands(argcNew, argvNew, &semicln_counter, &bgprocesses_counter, semicln_found, bgprocesses);
            }
            else
                argvPtr = checkMultipleCommands(argctmp, argvtmp, 
                        &semicln_counter, &bgprocesses_counter, semicln_found, bgprocesses);   

            userCommand(&semicln_found[0], argvPtr[0], bgprocesses_counter, &bgprocesses_countertmp, bgprocesses);

            // command has multiple commands seperated by ";"
            for (int i=1; i<=semicln_counter; i++){
                int args = semicln_found[i] - semicln_found[i-1] - 1;
                userCommand(&args, argvPtr[i], bgprocesses_counter, &bgprocesses_countertmp, bgprocesses);
            }
            // free the temporary allocated array
            freeToken(argctmp, argvtmp);
        }
        else if (builtinCommand == CD){
            char *path;
            if (argc > 1)
                path = argv[1];
            else if ((path = getenv("HOME")) == NULL)
                path = ".";

            if (chdir(path) == -1)
                printf("cd: %s: No such directory\n", path);        
        }
        else if (builtinCommand == EXIT){
            // free everything that was dynamically allocated
            free(input);
            freeToken(argc, argv);
            freeAliases(myAliases);

            exit(0);
        }
        else if (builtinCommand == ADD_AL){
            int err = addAlias(argv[1], argv, myAliases);
            if (err == -1)
                printf("Error adding alias\n");
        }
        else if (builtinCommand == DEST_AL){
            int err = destroyAlias(argv[1], myAliases);
            if (err == -1)
                printf("Error destroying alias\n");
        }
        else if (builtinCommand == PRT_AL)
            printAlias(myAliases);
        else if (builtinCommand == NONE_B_IN){
            
            AliastoArgv(myAliases, argv, &argc);
            argvNew = checkWildCharacters(argc, argv, &argcNew);
            
            if (argcNew > 0 ){
                argvPtr = checkMultipleCommands(argcNew, argvNew, &semicln_counter, &bgprocesses_counter, semicln_found, bgprocesses);
            }
            else {
                argvPtr = checkMultipleCommands(argc, argv, &semicln_counter, &bgprocesses_counter, semicln_found, bgprocesses);
            }

            userCommand(&semicln_found[0], argvPtr[0], bgprocesses_counter, &bgprocesses_countertmp, bgprocesses);
            for (int i=1; i<=semicln_counter; i++){
                int args = semicln_found[i] - semicln_found[i-1] - 1;
                userCommand(&args, argvPtr[i], bgprocesses_counter, &bgprocesses_countertmp, bgprocesses);
            }
        }

        free(input);
        freeToken(argc, argv);
        freeArgv(argcNew, argvNew);

    }
    freeAliases(myAliases);
}