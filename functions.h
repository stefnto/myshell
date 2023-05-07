#ifndef FUNCTIONS_H_
#define FUNCTIONS_H_

#define INPUT_SIZE 300
#define ARGM_NUM 31
#define READ 0
#define WRITE 1

#include <stdbool.h>

typedef enum {IN, OUT, NONE} REDIR;

typedef enum {NO_SYMBOLS, GT, GTGT, LT, PIPE, BG, SEMIC} CHARFLAG;

// returns given command and puts it on history buffer(from readline/history.h library) for later access
char* readInput();

// passes words of given command in an char** for later parsing (including symbols)
void getToken(char *, int *,  char **);

// copy first char** to second char **
void copyToken(int *, char **, int*, char **);

// concat second char** into first char**
void concatInput(int *, char **, int, int *, char **);

char **checkWildCharacters(int , char **, int *);

// translates wild characters, appends results to char ** and returns size of new char **
int getWildCharacters(int , char **, char *, int, char ***, int *);

// checks if multipleCommands are in a single user prompt and if any commands to be run in backgroun
char ***checkMultipleCommands(int , char **, int *, int *, int *, int *);

// processes user's command depending if any pipes where used
void userCommand(int *,  char **, int , int *, int *);

// is called by usercommand and tracks any redirection prompted with the user, combining it with pipes if piped command
void searchRedirection(int* , char **, bool, int, int *, int *);

// setups file descriptors for input/output
void invokeProcess(const int *,  char **, const char *, const char *, CHARFLAG, CHARFLAG, REDIR, bool, int, int *, int *);

// called by invokeProcess if command doesnt include pipes, executes execvp() with the right arguments
void executeProcess(const int *, char **, int, int *, int *);

// called when user prompts a piped command, will call searchRedirection() to check  if first command reads from file
// and/or if last command will output to file
void invokeProcessPiped(const int* argc, char **argv, int , int *, int, int *, int *);

#endif