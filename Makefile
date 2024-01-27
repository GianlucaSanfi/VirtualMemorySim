CC= gcc
CCOPTS= -std=gnu99 -g -Wall
AR= ar

OBJS= mmu.o test.o

mmu.o: mmu.c mmu.h globals.h
	$(CC) $(CCOPTS) -c mmu.c
.phony: clean all

test.o: test.c mmu.h globals.h
	$(CC) $(CCOPTS) -c test.c
.phony: clean all

all: test

test: $(OBJS)
	$(CC) $(CCOPTS) -o test $(OBJS)

clean:
	rm -rf test *.o