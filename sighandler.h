#ifndef SIGHANDLER_H_
#define SIGHANDLER_H_

#include <sys/types.h>

extern __pid_t child_pid;

// makes shell ignore CTRL+C and CTRL+Z signals
void ignore();

void ignore_child();

// initializes handle_signal() for CTRL+C and CTRL+Z to pass to processes run in the shell
void ignore_parent();

// signal handle function
void handle_signal(int sig);

#endif
