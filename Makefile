CC = gcc

DEV_CFLAGS = -g -std=c99 -Wall -pedantic
BUILD_CFLAGS = -O3 -march=native -std=c99 -Wall -pedantic
CFLAGS = ${BUILD_CFLAGS}

EXECS = testTree

GENERIC_OBJS = fast_tree.o fast_table.o

all: ${EXECS}

testTree: testTree.c fast_table.o fast_tree.o 
	${CC} ${CFLAGS} $^ -o $@

fast_table.o: fast_table.c
	${CC} ${CFLAGS} -c $^

fast_tree.o: fast_tree.c 
	${CC} ${CFLAGS} -c $^

clean:
	rm -f ${EXECS} *.o