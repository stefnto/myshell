#ifndef HELPERFUNC_H_
#define HELPERFUNC_H_

// returns a char array with the arguments from firstarg to lastarg-1 (lastarg is the symbol met)
char **makeArgv(const int ,const int , char **);

// returns a * pointing to an array of pointers than point to arrays of char
char ***makePipeArgv(int, char **, int *);

// concatenate second char** to first char**
char **concatArgv(char **, int , int , char** , int );

// deallocates memory allocated from makeArgv()
void freeArgv(const int, char **);

// deallocates memory allocated from makePipeArgv()
void freePipeArgv(const int, const int *, char ***);

// deallocates initial argument vector so new command finds non allocated argument vector
void freeToken(const int , char **);

void printArgv(const int, const char **);

void printPipeArgv(const int, const int *, const char ***);

void printCommand(const int , char **);

// void initToken(const int , char **);

#endif
