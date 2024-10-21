CC = gcc # compiler
CFLAGS = -Wall -g -std=gnu11 -O3 # compile flags
LIBS = # libs

TARG = array list nnlist
all: $(TARG)

# generates the target executable
%: %.c
	$(CC) -o $@ $^		$(CFLAGS) $(LIBS)

# cleans stuff
clean:
	rm -f $(TARG) *.o *~
