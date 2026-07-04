// @BAKE gcc -o $*.out $@ ../source/plumblism.c -I../source -lcriterion
#include <criterion/criterion.h>
#include <criterion/new/assert.h>

#include <plumblism.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

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

// -----------------------------
// -----------------------------
//  _  _     _
// | || |___| |_ __  ___ _ _ ___
// | __ / -_) | '_ \/ -_) '_(_-<
// |_||_\___|_| .__/\___|_| /__/
//            |_|
// -----------------------------
// -----------------------------

static
int ints_per_pixel(pnm_type_t type) {
    return (type == PNM_PIX_ASCII || type == PNM_PIX_BINARY) ? 3 : 1;
}

static
void crex_assert_file_open(FILE * f, const char * s) {
    cr_assert_not_null(f, "Failed opening %s", s);
}

static
void test_read_image_proto(struct test_image_t image) {
    FILE * f = fopen(image.name, "r");
    crex_assert_file_open(f, image.name);

    int size = read_pnm_header(
        f,
        image.type,
        NULL,
        NULL,
        NULL
    );
    cr_assert(size > 0);

    int * buffer = malloc(size * sizeof(int));
    cr_assert_not_null(buffer);

    cr_assert(lt(int, 0, read_pnm_data(
        f,
        image.type,
        buffer
    )));

    free(buffer);
}

static
void test_write_image_proto(pnm_type_t type, int * b) {
    FILE * f = tmpfile();
    crex_assert_file_open(f, "tmpfile");

    cr_assert(lt(int, 0, write_pnm_file(
        f,
        type,
        b,
        4,
        4,
        255
    )));

    fclose(f);
}

// -------------------------------
// -------------------------------
//  _  _             _
// | || |___ __ _ __| |___ _ _ ___
// | __ / -_) _` / _` / -_) '_(_-<
// |_||_\___\__,_\__,_\___|_| /__/
// -------------------------------
// -------------------------------

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

        int actual = read_pnm_header(
            f,
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

// -------------------------------
// -------------------------------
//  ___             _ _
// | _ \___ __ _ __| (_)_ _  __ _
// |   / -_) _` / _` | | ' \/ _` |
// |_|_\___\__,_\__,_|_|_||_\__, |
//                          |___/
// -------------------------------
// -------------------------------

Test(plumblism, read_image_pbm_hand_ascii) {
    test_read_image_proto(test_images[0]);
}

Test(plumblism, read_image_pgm_hand_ascii) {
    test_read_image_proto(test_images[1]);
}

Test(plumblism, read_image_ppm_hand_ascii) {
    test_read_image_proto(test_images[2]);
}

Test(plumblism, read_image_pbm_gimp_ascii) {
    test_read_image_proto(test_images[3]);
}

Test(plumblism, read_image_pgm_gimp_ascii) {
    test_read_image_proto(test_images[4]);
}

Test(plumblism, read_image_ppm_gimp_ascii) {
    test_read_image_proto(test_images[5]);
}

Test(plumblism, read_image_pbm_gimp_binary) {
    test_read_image_proto(test_images[6]);
}

Test(plumblism, read_image_pgm_gimp_binary) {
    test_read_image_proto(test_images[7]);
}

Test(plumblism, read_image_ppm_gimp_binary) {
    test_read_image_proto(test_images[8]);
}

// -------------------------------
// -------------------------------
// __      __   _ _   _
// \ \    / / _(_) |_(_)_ _  __ _
//  \ \/\/ / '_| |  _| | ' \/ _` |
//   \_/\_/|_| |_|\__|_|_||_\__, |
//                          |___/
// -------------------------------
// -------------------------------

Test(plumblism, write_image_pbm_ascii) {
    int b[2];
    b[0] = INT_MAX;
    b[1] = INT_MAX;
    test_write_image_proto(PNM_BIT_ASCII, b);
}

Test(plumblism, write_image_pbm_binary) {
    int b[2];
    b[0] = INT_MAX;
    b[1] = INT_MAX;
    test_write_image_proto(PNM_BIT_ASCII, b);
}

Test(plumblism, write_image_pgm_ascii) {
    int b[16];
    for (int j = 0; j < 16; ++j) {
        b[j] = j * 3;
    }
    test_write_image_proto(PNM_GRE_ASCII, b);
}

Test(plumblism, write_image_pgm_binary) {
    int b[16];
    for (int j = 0; j < 16; ++j) {
        b[j] = j * 3;
    }
    test_write_image_proto(PNM_GRE_BINARY, b);
}

Test(plumblism, write_image_ppm_ascii) {
    int b[16*3];
    for (int j = 0; j < 16*3; j += 3) {
        int color = j * 3;
        b[j+0] = color;
        b[j+1] = color;
        b[j+2] = color;
    }
    test_write_image_proto(PNM_PIX_ASCII, b);
}

Test(plumblism, write_image_ppm_binary) {
    int b[16*3];
    for (int j = 0; j < 16*3; j += 3) {
        int color = j * 3;
        b[j+0] = color;
        b[j+1] = color;
        b[j+2] = color;
    }
    test_write_image_proto(PNM_PIX_BINARY, b);
}

// -------------------------------
// -------------------------------
//   ___                _
//  / __|___ _ __  _ __| |_____ __
// | (__/ _ \ '  \| '_ \ / -_) \ /
//  \___\___/_|_|_| .__/_\___/_\_|
//                |_|
// -------------------------------
// -------------------------------

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
            crex_assert_file_open(in, test_images[i].name);

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

            cr_assert(lt(int, 0, read_pnm_data(
                in,
                test_images[i].type,
                original
            )));

            fclose(in);
        } while (0);

        do {
            FILE * tmp = tmpfile();
            crex_assert_file_open(tmp, "tmpfile");

            cr_assert(lt(int, 0, write_pnm_file(
                tmp,
                test_images[i].type,
                original,
                w,
                h,
                maxval
            )));

            rewind(tmp);

            size2 = read_pnm_header(
                tmp,
                test_images[i].type,
                &rw,
                &rh,
                &rmax
            );

            cr_assert(lt(int, 0, read_pnm_data(
                tmp,
                test_images[i].type,
                copy
            )));

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
