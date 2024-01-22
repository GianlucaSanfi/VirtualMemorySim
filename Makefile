CC= gcc
CCOPTS= -std=gnu99 -g -Wall
AR= ar

OBJS= list_frame.o mmu.o test.o

all: test

test: $(OBJS)
	$(CC) $(CCOPTS) -o test $(OBJS)

list_frame.o: list_frame.c mmu.h list.h
	$(CC) $(CCOPTS) -c list_frame.c

mmu.o: mmu.c mmu.h
	$(CC) $(CCOPTS) -c mmu.c

test.o: test.c mmu.h
	$(CC) $(CCOPTS) -c test.c

clean:
	rm -rf test *.o