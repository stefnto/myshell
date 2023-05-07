#include "sighandler.h"
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <signal.h>
#include <string.h>

__pid_t child_pid = 0;

void ignore(){
    struct sigaction act;

    act.sa_handler=SIG_IGN;
    // memset(&act, 0, sizeof(act));

    sigemptyset(&act.sa_mask);
    sigaddset(&act.sa_mask, SIGINT);
    sigaddset(&act.sa_mask, SIGTSTP);

    sigaction(SIGINT, &act, NULL);
    sigaction(SIGTSTP, &act, NULL);
}

void ignore_child(){

    struct sigaction act;

    act.sa_handler=SIG_DFL;
    // memset(&act, 0, sizeof(act));

    sigemptyset(&act.sa_mask);
    sigaddset(&act.sa_mask, SIGINT);
    sigaddset(&act.sa_mask, SIGTSTP);

    sigaction(SIGINT, &act, NULL);
    sigaction(SIGTSTP, &act, NULL);
}

void ignore_parent(){
    
    struct sigaction act;

    act.sa_handler = &handle_signal;
    // memset(&act, 0, sizeof(act));

    sigemptyset(&act.sa_mask);
    sigaddset(&act.sa_mask, SIGINT);
    sigaddset(&act.sa_mask, SIGTSTP);
    
    sigaction(SIGINT, &act, NULL);
    sigaction(SIGTSTP, &act, NULL);
}

void handle_signal(int sig){
    
    if (child_pid != 0)
        if (sig == SIGINT){
            write(STDERR_FILENO, "\nReceived SIGINT (CTRL-C) signal. Exiting...\n", 46);
            killpg(child_pid, SIGINT);
        }
        else if (sig == SIGTSTP){
            write(STDERR_FILENO, "\nReceived SIGSTP (CTRL-Z) signal. Stopping...\n", 47);
            killpg(child_pid, SIGTSTP);
        } 
}
