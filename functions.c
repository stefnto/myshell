#include "functions.h"
#include "helperfunc.h"
#include "sighandler.h"
#include "builtIn.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdbool.h> 
#include <glob.h>
#include <sys/wait.h>
#include <readline/readline.h>
#include <readline/history.h>

char* readInput(){
    char *buffer = readline("in-mysh-now:>");
    
    if (buffer == NULL)
        return NULL;
    if (strlen(buffer) != 0){
        add_history(buffer);
        return buffer;
    } else return NULL;
}

void getToken(char *input, int *argc, char **argv){

    // copy input to temp char array to use strtok
    char input_temp[INPUT_SIZE];
    strcpy(input_temp, input);

    // get first token of input, delimiter is whitespace
    char *token = strtok(input_temp, " ");
   
    while( token != NULL ){
        // allocate memory for argument or symbol put in command
        if ( ( argv[*argc] = malloc(strlen(token) + 1 ) ) == NULL){
            perror(" Allocation failure ");
            continue;
        }
        strcpy(argv[(*argc)++], token);
        token = strtok(NULL, " ");
        continue;
    }
    if (*argc >= ARGM_NUM){
        perror(" Too many arguments, can't handle ");
        exit(3); // exit(3) means too many arguments provided
    }
    else
        argv[*argc] = NULL; // set end of strings in argv
}

void copyToken(int *argc1, char **argv1, int*argc2, char **argv2){
    *argc2 = *argc1;
    for(int i = 0; i < (*argc1); i++){
        argv2[i] = malloc(strlen(argv1[i])+1);
        strcpy(argv2[i], argv1[i]);
    }
    argv2[*argc2] = NULL;
}

void concatInput(int *argc1, char **argv1, int concat_pos, int *argc2, char **argv2){
    int length = 0;

    // set end of string in argv1
    argv1[*argc1 + *argc2 - 1] = NULL;

    // move arguments from position next to the position of concatenation X-1 places right
    // where X is the number of arguments of argv2 being concatenated and -1 cuz concat_pos element
    // will be removed from the resulting argv
    for(int i = (*argc1 - 1); i > concat_pos; i--){
        if (argv1[i] != NULL){
            argv1[i + *argc2 - 1] = malloc(strlen(argv1[i])+1);
            strcpy(argv1[i + *argc2 - 1], argv1[i]);
            free(argv1[i]);
        }
    }
    // aconcat argv2 to argv1
    for(int i = concat_pos; i < (concat_pos + *argc2); i++){
        argv1[i] = malloc(strlen(argv2[i - concat_pos])+1);
        strcpy(argv1[i], argv2[i - concat_pos]);
    }
    *argc1 += *argc2 - 1;
}

char ** checkWildCharacters(int argc, char **argv, int *argcNew){
    char **argvNew = NULL;
    int prev_wildchar_pos = 0, wild_char_counter = 0, args_added = 0;
    
    for (int i=0; i <= ( argc + args_added - wild_char_counter); i++){
                
        char *pchr;
        if (argvNew == NULL){

            if (argv[i] == NULL)
                continue;
            else if ( ( pchr = strchr(argv[i], '*') ) != NULL  ||  ( pchr = strchr(argv[i], '?') ) != NULL ){
                wild_char_counter++;
                *argcNew = getWildCharacters(argc, argv, argv[i], i, &argvNew, &args_added);
            }
        }
        else {
            
            if ( ( pchr = strchr(argvNew[i], '*') ) != NULL  ||  ( pchr = strchr(argvNew[i], '?') ) != NULL ){
                wild_char_counter++;
                *argcNew = getWildCharacters(*argcNew-1, argvNew, argvNew[i], i, &argvNew, &args_added);
            }
        }
    } 
    return argvNew;
}

int getWildCharacters(int argc, char **argv, char *pchr, int argv_wildchar_pos, char ***argvNew, int *args_added){
    glob_t gstruck;
    char **tmp = argv, **tmp2 = *argvNew;

    int err = glob(pchr, GLOB_ERR, NULL, &gstruck);
    
    *argvNew = concatArgv(argv, argc, argv_wildchar_pos, gstruck.gl_pathv, gstruck.gl_pathc);

    // free stuff
    if (tmp2 != NULL){
        freeArgv(argc, tmp);
    }

    // total args_added from all calls of getWildCharacters() (-1 because wild char array cell gets removed in concatenation process)
    *args_added += gstruck.gl_pathc - 1;
    
    // arguments number of new array
    int argcNew = argc + gstruck.gl_pathc - 1;

    globfree(&gstruck);
    return argcNew;
}

char ***checkMultipleCommands(int argc, char **argv, int *semicln_counter, int *bgprocesses_counter,
    int *semicln_found, int *bgprocesses){
    for (int i=0; i<argc; i++){
        
        int last_char = strlen(argv[i]);
        
        // used when only 1 command needs to be on background (i.e: ls -l &)
        // or when last command needs to be on background (i.e: ls -l ; cat file &)
        if ( strcmp(&argv[i][last_char-1], "&") == 0){
            bgprocesses[*bgprocesses_counter] = i;
            (*bgprocesses_counter)++;
            break;
        }
        if ( strcmp(&argv[i][last_char-1], ";") == 0 ){
            if ( (strlen(argv[i]) > 1) && (argv[i][last_char-2] == '&') ){
                bgprocesses[*bgprocesses_counter] = i;
                (*bgprocesses_counter)++;
            }
            semicln_found[*semicln_counter] = i;
            (*semicln_counter)++;
        }
        semicln_found[*semicln_counter] = i+1;
    }

    return makePipeArgv(*semicln_counter, argv, semicln_found);
}

void userCommand(int* argc, char **argv, int bgprocesses_counter, int *bgprocesses_countertmp, int *bgprocesses){
    
    bool prcsexec = false;
    int pipe_found[ARGM_NUM] = { 0 }, pipe_counter = 0;

    // look if command includes pipes
    for (int i = 0; i < *argc; i++){
        if ( strcmp( *(argv+i), "|" ) == 0 ){
            
            pipe_found[pipe_counter] = i;
            pipe_counter++;
            prcsexec = true;
        }
        pipe_found[pipe_counter] = i+1;
    }

    if (prcsexec){
        invokeProcessPiped(argc, argv, pipe_counter, pipe_found, bgprocesses_counter, bgprocesses_countertmp, bgprocesses);
    }
    else searchRedirection(argc, argv, false, bgprocesses_counter, bgprocesses_countertmp, bgprocesses);
}

void searchRedirection(int* argc, char **argv, bool isPiped, int bgprocesses_counter, int *bgprocesses_countertmp, int *bgprocesses){
    int last_char = 0;
    bool prcsexec = false;

    for(int i = 0; i < *argc; i++){
        // check if command has > redirection
        if ( strcmp( *(argv+i), ">" ) == 0 ){
            prcsexec = true;
            char **argvNew = makeArgv(last_char, i, argv);
            
            // invoke is called with argvNew, that contains argument list till character found
            // that will be used to later call execvp()
            // also, here argcNew = i
            invokeProcess(&i, argvNew, argv[i+1], NULL, GT, NO_SYMBOLS, OUT, isPiped, bgprocesses_counter, bgprocesses_countertmp, bgprocesses);
            freeArgv(i+1-last_char, argvNew); // (i+1-last_char) equals size of array made with makeArgv()
            last_char++;
        }

        // check if command has >> redirection
        if ( strcmp( *(argv+i), ">>" ) == 0 ){
            prcsexec = true;
            char **argvNew = makeArgv(last_char, i, argv);
            invokeProcess(&i, argvNew, argv[i+1], NULL, GTGT, NO_SYMBOLS, OUT, isPiped, bgprocesses_counter, bgprocesses_countertmp, bgprocesses);
            freeArgv(i+1-last_char, argvNew);
            last_char++;
        }

        // check if command has < redirection
        if ( strcmp( *(argv+i), "<" ) == 0 ){
            prcsexec = true;

            char **argvNew = makeArgv(last_char, i, argv);
            
            // command has output redirection after input redirection
            if (i+2 < *argc){
                if ( strcmp( *(argv+i+2), ">" ) == 0){
                    invokeProcess(&i, argvNew, argv[i+3], argv[i+1], LT, GT, IN, isPiped, bgprocesses_counter, bgprocesses_countertmp, bgprocesses);
                }
                if ( strcmp( *(argv+i+2), ">>" ) == 0){
                    invokeProcess(&i, argvNew, argv[i+3], argv[i+1], LT, GTGT, IN, isPiped, bgprocesses_counter, bgprocesses_countertmp, bgprocesses);
                }
            }
            else
                invokeProcess(&i, argvNew, NULL, argv[i+1], LT, NO_SYMBOLS, IN, isPiped, bgprocesses_counter, bgprocesses_countertmp, bgprocesses);
            freeArgv(i+1-last_char,argvNew);
            last_char++;
            break;
        }

        // execute command that had no >, >>, <, &, | or ; symbols
        if (i == *argc-1 && prcsexec == false){
            invokeProcess(&i, argv, NULL, NULL, NO_SYMBOLS, NO_SYMBOLS, NONE, isPiped, bgprocesses_counter, bgprocesses_countertmp, bgprocesses);
        }
    }
}

void invokeProcess(const int* argc, char **argv, const char *destfile, 
    const char *srcfile, CHARFLAG flag, CHARFLAG secondflag, REDIR redirection, bool isPiped,
    int bgprocesses_counter, int *bgprocesses_countertmp, int *bgprocesses){

    int srcfd = STDIN_FILENO, destfd = STDOUT_FILENO;    
    int saved_stdin, saved_stdout;

    // process will write to file
    if (redirection == OUT){
        if (flag == GT)
            destfd = open(destfile, O_WRONLY | O_CREAT | O_TRUNC, S_IRWXU);
        if (flag == GTGT)
            destfd = open(destfile, O_WRONLY | O_CREAT | O_APPEND , S_IRWXU);
            
        // save stdout, replace it with opened file and restore it later    
        saved_stdout = dup(STDOUT_FILENO);
        dup2(destfd, STDOUT_FILENO);
        close(destfd);

        if (!isPiped)
            executeProcess(argc, argv, bgprocesses_counter, bgprocesses_countertmp, bgprocesses);
        else {

            if ( execvp(argv[0], argv) < 0 ){
                fprintf(stderr, "%s: command not found\n", argv[0]);
                exit(1); // execvp failure
            }
        }

        dup2(saved_stdout, STDOUT_FILENO);
        close(saved_stdout);
    }
    else if (redirection == IN && flag == LT){ // process will read from file
        srcfd = open(srcfile, O_RDONLY);

        // output redirection exists after input redirection
        if (destfile != NULL){
            if (secondflag == GT)
                destfd = open(destfile, O_WRONLY | O_CREAT | O_TRUNC, S_IRWXU);
            if (secondflag == GTGT)
                destfd = open(destfile, O_WRONLY | O_CREAT | O_APPEND , S_IRWXU);
            
            // save stdout, replace it with opened file and restore it later    
            saved_stdout = dup(STDOUT_FILENO);
            dup2(destfd, STDOUT_FILENO);
            close(destfd);
        }
        
        // save stdin, replace it with opened file and restore it later
        saved_stdin = dup(STDIN_FILENO);
        dup2(srcfd, STDIN_FILENO);
        close(srcfd);

        if (!isPiped)
            executeProcess(argc, argv, bgprocesses_counter, bgprocesses_countertmp, bgprocesses); 
        else {
            if ( execvp(argv[0], argv) < 0){
                fprintf(stderr, "%s: command not found\n", argv[0]);
                exit(1); // execvp failure
            }
        } 

        if (destfile != NULL){
            dup2(saved_stdout, STDOUT_FILENO);
            close(saved_stdout);
        }
        
        dup2(saved_stdin, STDIN_FILENO);
        close(saved_stdin);
    }
    else if (destfile == NULL && srcfile == NULL && redirection == NONE && flag == NO_SYMBOLS){
        if (!isPiped)
            executeProcess(argc, argv, bgprocesses_counter, bgprocesses_countertmp, bgprocesses);
        else {
            if ( execvp(argv[0], argv) < 0 ){
                fprintf(stderr, "%s: command not found\n", argv[0]);
                exit(1); // execvp failure
           }
        }
    }
}

void executeProcess(const int* argc, char **argv, int bgprocesses_counter, int *bgprocesses_countertmp, int *bgprocesses){
    pid_t childpid;
    int status;
    char *commandn, *commandp;

    if ( ( childpid = fork() ) == -1 ){
            perror ( " Fork fail " );
            exit (2); // exit value 2 means fork fail inside the shell
    }
    if (childpid == 0){
        // get command path if put as absolute path in input
        commandn = strchr(argv[0], '/');
        if (commandn == NULL)
            commandn = argv[0];
        else 
            commandn++;

        ignore_child();

        commandp = argv[0];

        if (execvp(commandp, argv) < 0){
            fprintf(stderr, "%s: command not found\n", commandp);
            exit(1); // execvp failure
        }
        
    }
    else {
        // make child start a new process group by changing its pgid to its pid so as to handle signaling
        child_pid = childpid;
        ignore_parent();

        // incoming process will be executed in background, shell will not wait for it, only increase counter
        if (*bgprocesses_countertmp < bgprocesses_counter){
            setpgid(childpid, childpid);
            (*bgprocesses_countertmp)++;
        }
        else
            waitpid(childpid, &status, WUNTRACED);
    }
}

void invokeProcessPiped(const int* argc, char **argv, int pipe_counter, int *pipe_found, 
    int bgprocesses_counter, int *bgprocesses_countertmp, int *bgprocesses){

    // input holds the previous pipe's read end, output indicates current pipe's write end or STDOUT
    int fd[2], input = STDIN_FILENO, output;
    int status[pipe_counter+1];
    pid_t childpid[pipe_counter+1];

    char ***pipargv = makePipeArgv(pipe_counter, argv, pipe_found);

    for (int i=0; i <= pipe_counter; i++){
        // if not last command
        if (i < pipe_counter){
            if ( pipe(fd) == -1){
                perror(" pipe call ");
                exit(4); // exit(4) means pipe call fail
            }
            output = fd[WRITE];
        } 
        else 
            output = 1;

        if ( ( childpid[i] = fork() ) == -1 ){
            perror ( " Fork fail in pipe case" );
            exit (2); // exit(2) means fork fail inside the shell
        }
        if (childpid[i] == 0){

            // if not first pipe command, redirect input to last pipe's read end
            if (input != STDIN_FILENO) {
                // printf("process %d in 1st if (prev_input != STDIN_FILENO)\n", getpid());
                if (dup2(input, STDIN_FILENO) == -1)
                    perror("dup2 error in if (input != STDIN_FILENO)\n");
                if (close(input) == -1)
                    perror("close error in if (input != STDIN_FILENO)\n");
            }
            
            // if not last command, child process must write to new pipe's write end
            if (output != STDOUT_FILENO){
                // redirect ouput to new pipe's write end
                if (dup2(fd[WRITE], STDOUT_FILENO) == -1)
                    perror("dup2 error\n");
                if (close(fd[WRITE]) == -1)
                    perror("close error\n");
                if (close(fd[READ]) == -1)
                    perror("close error\n");

            }
            
            // first command might have input redirection
            if (i == 0){
                searchRedirection(pipe_found+i, pipargv[i], true, bgprocesses_counter, bgprocesses_countertmp, bgprocesses);
            }
            // last command might have output redirection
            else if (i == pipe_counter){
                int args = ( *(pipe_found+i) - *(pipe_found+i-1) - 1);
                searchRedirection( &args, pipargv[i], true, bgprocesses_counter, bgprocesses_countertmp, bgprocesses );
            }
            else {
                if ( execvp(pipargv[i][0], pipargv[i]) < 0){
                    fprintf(stderr, "%s: command not found\n", pipargv[i][0]);
                    exit(1); // execvp failure
                }
            }

        }
        else { 
            // close pipe's write end in parent process
            if (output != STDOUT_FILENO){
                if (close(fd[WRITE]) == -1)
                    perror("parent close(fd[read]) fail\n");
            }
            // close pipe's read end in parent process (doesn't execute only in first iteration)
            if (input != STDIN_FILENO){
                if (close(input) == -1)
                    perror("parent close(fd[write]) fail\n");
            }
        
            input = fd[READ];
        }
    }   
    
    // parent must wait for child process(es) to finish
    for (int i=0; i <= pipe_counter; i++){
        waitpid(childpid[i], &status[i], WUNTRACED);
        // printf("Process %d return: %d\n", childpid[i], status[i]);
    }
}
