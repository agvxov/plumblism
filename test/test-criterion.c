// @BAKE gcc -o $*.out $@ ../source/plumblism.c -I../source -lcriterion
#include <criterion/criterion.h>
#include <criterion/new/assert.h>

#include <plumblism.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

constexpr size_t N_TEST_IMAGES = 9;
struct test_image_t {
    const char * name;
    pnm_type_t type;
    int width;
    int height;
} test_images[N_TEST_IMAGES] = {
    // -------
    // By hand
    // -------
    {
        .name   = "test/test-hand-ascii.pbm",
        .type   = PNM_BIT_ASCII,
        .width  = 10,
        .height = 10,
    },
    {
        .name   = "test/test-hand-ascii.pgm",
        .type   = PNM_GRE_ASCII,
        .width  = 10,
        .height = 10,
    },
    {
        .name   = "test/test-hand-ascii.ppm",
        .type   = PNM_PIX_ASCII,
        .width  = 3,
        .height = 3,
    },

    // --------------
    // By gimp; ascii
    // --------------
    {
        .name   = "test/test-gimp-ascii.pbm",
        .type   = PNM_BIT_ASCII,
        .width  = 275,
        .height = 235,
    },
    {
        .name   = "test/test-gimp-ascii.pgm",
        .type   = PNM_GRE_ASCII,
        .width  = 275,
        .height = 235,
    },
    {
        .name   = "test/test-gimp-ascii.ppm",
        .type   = PNM_PIX_ASCII,
        .width  = 275,
        .height = 235,
    },

    // --------------
    // By gimp; binary
    // --------------
    {
        .name   = "test/test-gimp-binary.pbm",
        .type   = PNM_BIT_BINARY,
        .width  = 275,
        .height = 235,
    },
    {
        .name   = "test/test-gimp-binary.pgm",
        .type   = PNM_GRE_BINARY,
        .width  = 275,
        .height = 235,
    },
    {
        .name   = "test/test-gimp-binary.ppm",
        .type   = PNM_PIX_BINARY,
        .width  = 275,
        .height = 235,
    },
};

static
int ints_per_pixel(pnm_type_t type) {
    return (type == PNM_PIX_ASCII || type == PNM_PIX_BINARY) ? 3 : 1;
}

static
void crex_assert_file_open(FILE * f, const char * s) {
    cr_assert_not_null(f, "Failed opening %s", s);
}

Test(plumblism, header_type_correctness) {
    for (size_t i = 0; i < N_TEST_IMAGES; ++i) {
        FILE * f = fopen(test_images[i].name, "r");
        crex_assert_file_open(f, test_images[i].name);

        pnm_type_t type = get_pnm_type(f);

        cr_expect(eq(int, type, test_images[i].type),
            "%s: expected type %d got %d",
            test_images[i].name,
            test_images[i].type,
            type
        );

        fclose(f);
    }
}

Test(plumblism, header_storage_requirement) {
    for (size_t i = 0; i < N_TEST_IMAGES; ++i) {
        FILE * f = fopen(test_images[i].name, "r");
        crex_assert_file_open(f, test_images[i].name);

        int expected = test_images[i].width
                     * test_images[i].height
                     * ints_per_pixel(test_images[i].type)
        ;

        int actual = read_pnm_header(f,
            test_images[i].type,
            NULL,
            NULL,
            NULL
        );

        cr_expect(eq(int, actual, expected),
            "%s: expected %d ints, got %d",
            test_images[i].name,
            expected,
            actual
        );

        fclose(f);
    }
}

Test(plumblism, write_3x3_images) {
    static const pnm_type_t formats[] = {
        PNM_BIT_ASCII,
        PNM_GRE_ASCII,
        PNM_PIX_ASCII,
        PNM_BIT_BINARY,
        PNM_GRE_BINARY,
        PNM_PIX_BINARY,
    };

    for (size_t i = 0; i < sizeof(formats)/sizeof(formats[0]); ++i) {
        pnm_type_t type = formats[i];

        int pixels[27];

        for (int j = 0; j < 27; ++j) {
            pixels[j] = j & 1;
        }

        if (type == PNM_GRE_ASCII || type == PNM_GRE_BINARY) {
            for (int j = 0; j < 9; ++j)
                pixels[j] = j * 28;
        }

        if (type == PNM_PIX_ASCII || type == PNM_PIX_BINARY) {
            for (int j = 0; j < 27; ++j)
                pixels[j] = (j * 23) % 256;
        }

        FILE * f = tmpfile();
        cr_assert_not_null(f);

        cr_assert(0 <= write_pnm_file(
            f,
            type,
            pixels,
            3,
            3,
            255
        ));

        rewind(f);

        int w, h, maxval;

        int size = read_pnm_header(
            f,
            type,
            &w,
            &h,
            &maxval
        );

        cr_expect(eq(int, w, 3));
        cr_expect(eq(int, h, 3));

        int buffer[27];

        memset(buffer, 0, sizeof(buffer));

        cr_assert(0 <= read_pnm_data(
            f,
            type,
            buffer
        ));

        cr_expect_arr_eq(pixels, buffer, size);

        fclose(f);
    }
}

Test(plumblism, read_write_read_roundtrip) {
    for (size_t i = 0; i < N_TEST_IMAGES; ++i) {
        int * original;
        int * copy;

        int w, h, maxval;
        int rw, rh, rmax;

        int size;
        int size2;

        do {
            FILE * in = fopen(test_images[i].name, "r");
            cr_assert_not_null(in);

            size = read_pnm_header(
                in,
                test_images[i].type,
                &w,
                &h,
                &maxval
            );

            cr_assert(size > 0);

            original = malloc(size * sizeof(int));
            copy     = malloc(size * sizeof(int));

            cr_assert_not_null(original);
            cr_assert_not_null(copy);

            cr_assert(0 <= read_pnm_data(
                in,
                test_images[i].type,
                original
            ));

            fclose(in);
        } while (0);

        do {
            FILE * tmp = tmpfile();
            cr_assert_not_null(tmp);

            cr_assert(0 <= write_pnm_file(
                tmp,
                test_images[i].type,
                original,
                w,
                h,
                maxval
            ));

            rewind(tmp);

            size2 = read_pnm_header(
                tmp,
                test_images[i].type,
                &rw,
                &rh,
                &rmax
            );

            cr_assert(0 <= read_pnm_data(
                tmp,
                test_images[i].type,
                copy
            ));

            fclose(tmp);
        } while (0);

        cr_expect(eq(int, size2, size));
        cr_expect(eq(int, rw, w));
        cr_expect(eq(int, rh, h));
        cr_expect(eq(int, rmax, maxval));

        cr_expect_arr_eq(original, copy, size);

        free(original);
        free(copy);
    }
}
