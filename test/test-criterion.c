// @BAKE gcc -o $*.out $@ ../source/plumblism.c -I../source
#include <plumblism.h>

struct test_image_t {
    const char * name;
    pnm_type_t type;
    int width;
    int height;
} test_images[] = {
    // -------
    // By hand
    // -------
    {
        .name   = "test-hand-ascii.pbm",
        .type   = PNM_BIT_ASCII,
        .width  = 10,
        .height = 10,
    },
    {
        .name   = "test-hand-ascii.pgm",
        .type   = PNM_GRE_ASCII,
        .width  = 10,
        .height = 10,
    },
    {
        .name   = "test-hand-ascii.ppm",
        .type   = PNM_PIX_ASCII,
        .width  = 3,
        .height = 3,
    },
    // --------------
    // By gimp; ascii
    // --------------
    {
        .name   = "test-gimp-ascii.pbm",
        .type   = PNM_BIT_ASCII,
        .width  = 275,
        .height = 235,
    },
    {
        .name   = "test-gimp-ascii.pgm",
        .type   = PNM_GRE_ASCII,
        .width  = 275,
        .height = 235,
    },
    {
        .name   = "test-gimp-ascii.ppm",
        .type   = PNM_PIX_ASCII,
        .width  = 275,
        .height = 235,
    },
    // --------------
    // By gimp; binary
    // --------------
    {
        .name   = "test-gimp-binary.pbm",
        .type   = PNM_BIT_BINARY,
        .width  = 275,
        .height = 235,
    },
    {
        .name   = "test-gimp-binary.pgm",
        .type   = PNM_GRE_BINARY,
        .width  = 275,
        .height = 235,
    },
    {
        .name   = "test-gimp-binary.ppm",
        .type   = PNM_PIX_BINARY,
        .width  = 275,
        .height = 235,
    },
};

signed main(void) {
    return 0;
}
