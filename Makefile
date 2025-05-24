.PHONY: test

CFLAGS := -std=c99 -Wall -Wpedantic -O2

main:
	${CC} ${CFLAGS} -shared -fPIC source/plumblism.c -o object/libplumblism.so
	${CC} ${CFLAGS} -c source/plumblism.c -o object/plumblism.o
	${AR} rcs object/libplumblism.a object/plumblism.o

test:
	bake test/test.c

man:
	ts-md2man documentation/netpbm.5.md > object/netpbm.3

clean:
	-${RM} object/*{.o,.so,.a}
	-${RM} test/*.out
