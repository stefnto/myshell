#include "builtIn.h"
#include "helperfunc.h"
#include "functions.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>

BUILTIN checkBuiltIn(int argc, char **argv){
    if ( strcmp(argv[0], "myHistory") == 0 ){
        if ( argv[1] == NULL )
            return HIST;
        else return HIST_SPECIFIC;    
    }
    else if ( strcmp(argv[0], "cd") == 0 )
        return CD;
    else if ( strcmp(argv[0], "exit") == 0 )
        return EXIT;
    else if ( strcmp(argv[0], "createalias") == 0)
        return ADD_AL;
    else if ( strcmp(argv[0], "destroyalias") == 0)
        return DEST_AL;    
    else if ( strcmp(argv[0], "myAliases") == 0)   
        return PRT_AL; 
    else return NONE_B_IN;    
}

void add_myhistory(int argc, char **argv, char ***commandHistory, int *commandHistoryCounter, 
    int *commandHistoryCharNum){
    
    // dont save "myHistory" command in commandHistory
    BUILTIN bIn = checkBuiltIn(argc, argv);
    if ( bIn != NONE_B_IN)
        return;

    // reset commandHistoryCounter
    if (*commandHistoryCounter == HISTORY_NUMS)
        *commandHistoryCounter = 0;

    // first time on commandHistoryCounter
    if (commandHistory[*commandHistoryCounter] == NULL){
        commandHistory[*commandHistoryCounter] = makeArgv(0, argc, argv);
        commandHistoryCharNum[*commandHistoryCounter] = argc;
    }
    else {
        // free previously saved command and save new one where commandHistoryCounter points
        freeArgv( commandHistoryCharNum[*commandHistoryCounter] , commandHistory[*commandHistoryCounter]);
        commandHistory[*commandHistoryCounter] = makeArgv(0, argc, argv);
        commandHistoryCharNum[*commandHistoryCounter] = argc;
    }
    (*commandHistoryCounter)++;
}

void printHistory(char ***commandHistory, int *commandHistoryCounter, int *commandHistoryCharNum){
    for(int i = 0; i < HISTORY_NUMS; i++){
        char **tempptr = commandHistory[i];
        printf("%d | ", i+1);
        for(int j = 0; j < commandHistoryCharNum[i]; j++){
            printf("%s ", tempptr[j]);
        }
        printf("\n");
    }
}

void initAlias(struct alias **myAliases){
    for(int i = 0; i < ALIASES_NUM; i++){
        myAliases[i] = malloc(sizeof(struct alias));
        myAliases[i]->written = false;
        myAliases[i]->aliasname = NULL;
        myAliases[i]->aliasvalue = NULL;
    }
}

void freeAliases(struct alias **myAliases){
    for(int i = 0; i < ALIASES_NUM; i++){
        free(myAliases[i]);
    }
}

char *getAliasValue(char **argv){
    if (argv[2] == NULL)
        return NULL;

    char *aliasv1 = argv[2];

    if (aliasv1[0] == '\"'){
        // skip " character
        aliasv1++;
        for(int i = 3; ; i++){
            // reached end of arguments
            if (argv[i] == NULL)
                break;
            
            char *ptr = strchr( argv[i], '\"');

            // second occurance of " found, concat two strings
            if (ptr != NULL){
                ptr = argv[i];
                ptr[ strlen(ptr)-1 ] = '\0'; // remove " character

                char *aliasv = NULL;

                if (i == 3){
                    aliasv = malloc( strlen(aliasv1) + strlen(ptr) + 2 );
                    aliasv[0] = '\0'; // initialize buffer for strcat
                }
                else{
                    aliasv = malloc( strlen(aliasv1) + strlen(ptr) + 1 );
                    aliasv[0] = '\0'; // initialize buffer for strcat
                }
                strcat(aliasv, aliasv1);
                if (i == 3)
                    strcat(aliasv, " ");
                strcat(aliasv, ptr);

                if ((aliasv1-1) != argv[2])
                    free(aliasv1);

                return aliasv;    
            }
            else if (ptr == NULL){
                ptr = argv[i];
                char *tmp = aliasv1;
                aliasv1 = malloc( strlen(aliasv1) + strlen(ptr) + 3 );

                strcat(aliasv1, tmp);
                strcat(aliasv1, " ");
                strcat(aliasv1, ptr);
                strcat(aliasv1, " ");

                // tmp doesn't point to argv[2]
                if ((tmp-1) != argv[2])
                    free(tmp);
            }
        }
        return NULL;
    }
    else
        return argv[2];
}

int addAlias(char *aliasn, char **argv, struct alias **myAliases){
    int pos = checkAlias(aliasn, myAliases);
    char *aliasv = getAliasValue(argv);

    if (aliasv == NULL)
        return -1;


    // alias name already exists, just change its value
    if ( (pos > -1) && pos < ALIASES_NUM ){

        free(myAliases[pos]->aliasvalue);

        myAliases[pos]->aliasvalue = malloc( strlen(aliasv)+1 );
        strcpy( myAliases[pos]->aliasvalue, aliasv );

        return 0; // success
    }
    else if (pos == -1){
        int aliasCounter;

        aliasCounter = getPosition(myAliases);

        // copy aliasn and aliasv to array of aliases in myAliases[aliasCounter]
        myAliases[aliasCounter]->aliasname = malloc( strlen(aliasn)+1 );
        strcpy( myAliases[aliasCounter]->aliasname, aliasn );

        myAliases[aliasCounter]->aliasvalue = malloc( strlen(aliasv)+1 );
        strcpy( myAliases[aliasCounter]->aliasvalue, aliasv );

        myAliases[aliasCounter]->written = true;

        return 0;
    }
    return -1; // failure
}

int destroyAlias(char *aliasn, struct alias **myAliases){
    for(int i = 0; i < ALIASES_NUM; i++){
        if (myAliases[i]->written == true){
            if ( strcmp(myAliases[i]->aliasname, aliasn) == 0){
                myAliases[i]->written = false;
                free(myAliases[i]->aliasname);
                free(myAliases[i]->aliasvalue);

                return 0;
            } 
        }
    }
    return -1;
}

char* getAlias(char *aliasn, struct alias **myAliases){
    if (aliasn == NULL)
        return NULL;
    for(int i = 0; i < ALIASES_NUM; i++){
        if (myAliases[i]->written == true){
            if ( strcmp(myAliases[i]->aliasname, aliasn) == 0){
                return myAliases[i]->aliasvalue;
            }
        }
    }
    return NULL;
}

void printAlias(struct alias **myAliases){
    for(int i = 0; i < ALIASES_NUM; i++){
        printf("%d | ", i+1);
        if (myAliases[i]->written == false)
            printf("None\n");
        else {
            printf("alias name: %s, alias value: %s\n", myAliases[i]->aliasname, myAliases[i]->aliasvalue);
        }    
    }
}

void AliastoArgv(struct alias **myAliases, char **argv, int *argc){
    int counter = *argc;
    char *argvNew[ARGM_NUM];
    int argcNew = 0;

    for(int i = 0; i < counter; i++){
        char *aliasv = getAlias(argv[i], myAliases);
        if (aliasv != NULL){

            

            // make new argv through tokenizing, so as to concat it to the current one
            getToken(aliasv, &argcNew, argvNew);
            concatInput(argc, argv, i, &argcNew, argvNew);
            counter += argcNew - 1;

            freeToken(argcNew, argvNew);
            argcNew = 0;
        }
    }
}

int checkAlias(char *aliasn, struct alias **myAliases){
    for(int i = 0; i < ALIASES_NUM; i++){
        if (myAliases[i]->written == true){
            if ( strcmp(myAliases[i]->aliasname, aliasn) == 0)
                return i;
        }
    }
    return -1;
}

int getPosition(struct alias **myAliases){
    for(int i = 0; i < ALIASES_NUM; i++){
        if ( myAliases[i]->written == false)
            return i;
    }
    return -1;
}