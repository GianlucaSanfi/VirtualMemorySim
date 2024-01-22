CC= gcc
CCOPTS= -std=gnu99 -g -Wall
AR= ar

OBJS= list_frame.o mmu.o test.o

all: test

test: $(OBJS)
	$(CC) $(CCOPTS) -o test $(OBJS)

list_frame.o: list_frame.c mmu.h list.h
	$(CC) $(CCOPTS) -c list_frame.c
.phony: clean all

mmu.o: mmu.c mmu.h
	$(CC) $(CCOPTS) -c mmu.c
.phony: clean all

test.o: test.c mmu.h
	$(CC) $(CCOPTS) -c test.c
.phony: clean all

clean:
	rm -rf test *.o