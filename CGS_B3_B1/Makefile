CC=gcc
CFLAGS=-c
CCFLAGSDEBUG=-g
LFLAGS=
MYFLAG=

ifeq ($(OS),Windows_NT)	
# all : shell clean-win
MYFLAG = clean-win
else
MYFLAG = clean-nix
# all : shell clean-nix
endif

all : shell $(MYFLAG)

shell : shell.o filesys.o
	$(CC) $(LFLAGS) -o shell shell.o filesys.o

shell.o : shell.c
	$(CC) $(CFLAGS) shell.c

filesys : filesys.o
	$(CC) $(LFLAGS) -o filesys filesys.o

filesys-o : filesys.c filesys.h
	$(CC) -o filesys.c

clean-win :
	del -f *.o

clean-nix :
	rm -f *.o
