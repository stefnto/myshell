OBJS = main.o functions.o helperfunc.o sighandler.o builtIn.o
SOURCE = main.c functions.c helperfunc.c sighandler.c builtIn.c
HEADER = functions.h helperfunc.h sighandler.h builtIn.h
LIBS = -lreadline
OUT = mysh
CC = gcc
FLAGS = -g -c

$(OUT): $(OBJS)
		$(CC) -g $(OBJS) $(LIBS) -o $@

# create objective files
main.o:	main.c
		$(CC) $(FLAGS) main.c 

functions.o:	functions.c
		$(CC) $(FLAGS) functions.c

helperfunc.o:	helperfunc.c
		$(CC) $(FLAGS) helperfunc.c		

sighandler.o: sighandler.c
		$(CC) $(FLAGS) sighandler.c

builtIn.o: builtIn.c
		$(CC) $(FLAGS) builtIn.c

clean:
		rm -f $(OBJS) $(OUT)		