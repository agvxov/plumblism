.PHONY: test

CFLAGS := -Isource/ -std=c99 -Wall -Wpedantic -O2

main: lib randimg test

lib:
	${CC} ${CFLAGS} -shared -fPIC source/plumblism.c -o object/libplumblism.so
	${CC} ${CFLAGS} -c source/plumblism.c -o object/plumblism.o
	${AR} rcs object/libplumblism.a object/plumblism.o

randimg:
	${CC} ${CFLAGS} -o randimg.out tool/randimg.c source/plumblism.c
	./randimg.out --ascii --o random.out.pgm

test:
	${CC} -o test.out test/test.c source/plumblism.c -Isource -ldictate -std=c23 -ggdb -fsanitize=address
	./test.out
	cat test.out.pbm

man:
	ts-md2man documentation/netpbm.5.md > object/netpbm.3

clean:
	-${RM} object/*{.o,.so,.a}
	-${RM} test/*.out
