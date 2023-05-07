#ifndef BUILTIN_H_
#define BUILTIN_H_

#include <stdbool.h>

#define HISTORY_NUMS 20
#define ALIASES_NUM 30

struct alias {
    bool written;
    char *aliasname;
    char *aliasvalue;
};

typedef enum {HIST, HIST_SPECIFIC, CD, EXIT, ADD_AL, DEST_AL, PRT_AL, NONE_B_IN} BUILTIN;

// checks if any builtIn command was put (myHistory, cd, exit)
BUILTIN checkBuiltIn(int , char **);

// adds user prompt to an array for later parsing
void add_myhistory(int , char **, char ***, int *, int *);

void printHistory(char ***, int *, int *);

// initalise aliases array
void initAlias(struct alias **);

// free aliases array
void freeAliases(struct alias **);

// returns alias value
char *getAliasValue(char **);

// add alias to array
int addAlias(char *, char **, struct alias**);

int destroyAlias(char *, struct alias **);

// return alias value if name exists, otherwise NULL
char* getAlias(char *, struct alias **);

// print aliases array
void printAlias(struct alias **);

// concats alias value if it exists in char **argv
void AliastoArgv(struct alias **, char **, int *);

// returns position of alias written if it exists, or else returns -1
int checkAlias(char *, struct alias **);

// returns position where new alias will be written
// -1 if it doesn't exist, pos if alias name already exists
int getPosition(struct alias **);

#endif