# if you type 'make' without arguments, this is the default
PROG = parking
all: $(PROG)

# associated object files 
HEAD    = commands.h dumpdb.h insticket.h loaddb.h splitrow.h sumlookup.h \
	      verifydb.h delticket.h freetickets.h largest.h parking.h subs.h vehlookup.h Version.h
OBJ	    = commands.o dumpdb.o insticket.o loaddb.o sumlookup.o \
	      delticket.o freetickets.o largest.o parking.o vehlookup.o
		  
# special libraries
LIB	= libpa5.a
LIBFLAGS =-L ./ $(LIB)

# select the compiler and flags you can over-ride on command line e.g. make DEBUG= 
CC      = gcc
DEBUG	= -ggdb
CSTD	=
WARN	= -Wall -Wextra
CDEFS	=
CFLAGS	= -I. $(DEBUG) $(WARN) $(CSTD) $(CDEFS)

$(OBJ):             $(HEAD)

# specify how to compile the target
$(PROG):	$(OBJ) $(LIB)
	$(CC) $(CFLAGS) $(OBJ) $(LIBFLAGS) -o $@

.PHONY: clean alltest test1 test2 test3 test4 test5 simple

LIST   = 1 2 3 4 5

simple:   $(PROG)
	-./parking -d in/DB.csv

alltest:   $(PROG)
	-./runtest $(LIST)

test1:   $(PROG)
	-./runtest 1

test2:   $(PROG)
	-./runtest 2

test3:   $(PROG)
	-./runtest 3

test4:   $(PROG)
	-./runtest 4

test5:   $(PROG)
	-./runtest 5

# remove binaries
clean:
	rm -f $(OBJ) $(PROG) $(LOBJ)
