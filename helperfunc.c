#include "helperfunc.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

char **makeArgv(const int firstarg, const int lastarg, char **argv){
    char **argvNew = malloc( ( lastarg + 1 - firstarg ) * sizeof(char *));
    for(int i = (firstarg - firstarg); i < (lastarg - firstarg); i++){
        argvNew[i] = malloc( strlen(argv[i+firstarg])+1 );
        strcpy(argvNew[i], argv[i+firstarg]);
        // printf("argvNew[%d] = %s  ", i, argvNew[i]);
    }
    argvNew[lastarg - firstarg] = NULL;
    // printf("argvNew[%d] = %s\n", lastarg-firstarg, argvNew[lastarg-firstarg]);
    return argvNew;
}

char ***makePipeArgv(int pipe_counter, char **argv, int *pipe_found){
    // found (pipe_counter) number of pipes, need to execute (pipe_counter+1) commands
    char ***argvPipe = malloc( (pipe_counter+1) * sizeof(char*) );

    // make first array
    argvPipe[0] = makeArgv(0, pipe_found[0], argv);

    // make the remaining arrays in loop
    for (int i=1; i < pipe_counter+1; i++)
        argvPipe[i] = makeArgv(pipe_found[i-1]+1, pipe_found[i], argv);

    return argvPipe;
}

char **concatArgv(char **argv1, int argc1, int argv_wildchar_pos, char** argv2, int argc2){
    char **argvNew = malloc( (argc1 + argc2 - 1) * sizeof(char*));

    for(int i = 0; i < (argc1 + argc2 - 1); i++){
        if (i < argv_wildchar_pos){

            argvNew[i] = malloc( strlen(argv1[i])+1 );
            strcpy(argvNew[i], argv1[i]);
        }
        if (i >= argv_wildchar_pos && i < argc2 + argv_wildchar_pos){

            // start copying from first argument of argv2
            argvNew[i] = malloc( strlen(argv2[i-argv_wildchar_pos])+1 );
            strcpy(argvNew[i], argv2[i-argv_wildchar_pos]);
        }
        if (i >= argc2 + argv_wildchar_pos && i < argc1 + argc2){
                
            argvNew[i] = malloc( strlen(argv1[i - argc2 + 1])+1 );
            strcpy(argvNew[i], argv1[i - argc2 + 1]);
        }
    }

    return argvNew;
}

void printArgv(const int argc, const char **argv){
    printf("{ ");
    for(int i=0; i < argc; i++)
        printf("%s, ", argv[i]);
    printf("}\n");
}

void printPipeArgv(const int pipe_counter, const int *pipe_found, const char ***pipargv){
    printArgv( (pipe_found[0]+1), (const char**)pipargv[0] );
    for(int i = 1; i < pipe_counter+1; i++)
        printArgv( (pipe_found[i] - pipe_found[i-1]), (const char**)pipargv[i] );
}

void freeArgv(const int argc, char **argv){
    for(int i = 0; i < argc; i++)
        free(argv[i]);
    free(argv);
}

void freePipeArgv(const int pipe_counter, const int *pipe_found, char ***pipargv){
    freeArgv( (pipe_found[0]+1), pipargv[0]);

    for(int i = 1; i < pipe_counter+1; i++)
        freeArgv( (pipe_found[i] - pipe_found[i-1]), pipargv[i] );
    free(pipargv);    
}

void freeToken(const int argc, char **argv){
    for(int i = 0; i < argc; i++)
        free(argv[i]);
}

void printCommand(const int argc, char **argv){
    printf("Using command: ");
    for(int i = 0; i < argc; i++)
        printf("%s ", argv[i]);
    printf("\n");    
}
