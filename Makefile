CC= gcc
CCOPTS= --std=gnu99 -g -Wall
AR= ar

OBJS= mmu.o

HEAD= globals.h mmu.h

%.o:	%.c $(HEAD)
	$(CC) $(CCOPTS) -c -o $@  $<
.phony: clean all

all: test lib.a

lib.a: $(OBJS)
	$(AR) -rcs $@ $^
	$(RM) $(OBJS)

test: test.c $(OBJS)
	$(CC) $(CCOPTS) -o $@ $^

clean:
	rm -rf *.o swap_file.bin *~ lib.a test