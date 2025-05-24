// @BAKE gcc -o $*.out $@ source/pnmio.c -ldictate -std=c23 -ggdb
#include "source/pnmio.h"
#include <dictate.h>
#include <stdlib.h>

extern int lex_header(FILE * f);

signed main(void) {
    int e;
    //FILE * f = fopen("/home/anon/Documents/figure_1.pbm", "r");
    FILE * f = fopen("test.pbm", "r");

    int w, h, is_ascii;
    e = read_pbm_header(f, &w, &h, &is_ascii);

    if (e == -1) { return 1; }

    int * buffer = (int*)malloc(e * sizeof(int));

    e = read_pbm_data(f, buffer, is_ascii);
    if (e < w*h) { return 1; }

    for (int i = 0; i < w*h; i++) {
        dictate(buffer[i], " ");
        if ((i + 1) % w == 0) { dictate("\n"); }
    }

    buffer[12] = 1;

    FILE * f2 = fopen("test_out.pbm", "w");

    write_pbm_file(f2, buffer, w, h, is_ascii);

    fclose(f);
    fclose(f2);
    free(buffer);
    return 0;
}
