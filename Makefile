.PHONY: main test test-basic test-criterion

CFLAGS := -Isource/ -std=c99 -Wall -Wpedantic -O2

main: lib randimg test

lib:
	${CC} ${CFLAGS} -shared -fPIC source/plumblism.c -o object/libplumblism.so
	${CC} ${CFLAGS} -c source/plumblism.c -o object/plumblism.o
	${AR} rcs object/libplumblism.a object/plumblism.o

randimg:
	${CC} ${CFLAGS} -o randimg.out tool/randimg.c source/plumblism.c
	./randimg.out --ascii --o random.out.pgm

test: test-basic test-criterion

test-basic:
	${CXX} -o test.out test/test.cpp source/plumblism.c -Isource -std=c++23
	${CC} ${CFLAGS} -o test.out test/test.c source/plumblism.c -Isource -ldictate -std=c23 -ggdb -fsanitize=address
	./test.out
	cat test.out.pbm

test-criterion:
	${CC} ${CFLAGS} -o test-criterion.out test/test-criterion.c source/plumblism.c -lcriterion -std=c23
	./test-criterion.out

man:
	ts-md2man documentation/netpbm.5.md > object/netpbm.3

clean:
	-${RM} object/*{.o,.so,.a}
	-${RM} test/*.out
