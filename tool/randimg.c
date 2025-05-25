#define _XOPEN_SOURCE 500
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <math.h>
#include <getopt.h>

#include <plumblism.h>

#define W_DEFAULT 256
#define H_DEFAULT 256

char * output_file_name = NULL;

typedef enum {
    PBM = 1,
    PGM,
    PPM,
} basetype_t;

bool is_ascii = false;

basetype_t basetype = PGM;

FILE * output_file;

int w = W_DEFAULT;
int h = H_DEFAULT;

static
void usage(void) {
    puts(
        "\n"
        "Usage:\n"
        "randimg [options] <outfile>\n"
        "\n"
        "Options:\n"
        "    -h:                            Print this help.\n"
        "    --pbm        : Generate a PBM image with random content (default).\n"
        "    --pgm        : Generate a PGM image with random content.\n"
        "    --ppm        : Generate a PPM image with random content.\n"
        "    --pfm        : Generate a PFM image with random content.\n"
        "    --ascii      : Emit PNM image in the ASCII format (default).\n"
        "    --binary     : Emit PNM image in the binary format.\n"
        "    --rgb        : Emit a color PFM image (default).\n"
        "    --greyscale  : Emit a greyscale PFM image.\n"
        "    -x <num>     : Value for the x-dimension of the image (default:256).\n"
        "    -y <num>     : Value for the y-dimension of the image (default:256).\n"
        "\n"
    );
}

static
void parse_opts(int argc, char * * argv) {
    static struct option long_options[] = {
        { "help",       no_argument,       0, 'h' },
        { "width",      required_argument, 0, 'x' },
        { "height",     required_argument, 0, 'y' },
        { "output",     required_argument, 0, 'o' },
        { "pbm",        no_argument,       0, PBM },
        { "pgm",        no_argument,       0, PGM },
        { "ppm",        no_argument,       0, PPM },
        { "ascii",      no_argument,       0, 'a' },
        { "binary",     no_argument,       0, 'b' },
        { 0, 0, 0, 0 }
    };

    int opt;
    int opt_index = 0;

    if (argc < 2) {
        usage();
        exit(1);
    }

    while ((opt = getopt_long(argc, argv, "hx:y:o:", long_options, &opt_index)) != -1) {
        switch (opt) {
            case 'h': {
                usage();
            } exit(0);
            case PBM:
            case PGM:
            case PPM: {
                basetype = opt;
            } break;
            case 'a': {
                is_ascii = true;
            } break;
            case 'b': {
                is_ascii = false;
            } break;
            case 'x': {
                w = atoi(optarg);
            } break;
            case 'y': {
                h = atoi(optarg);
            } break;
            case 'o': {
                output_file_name = strdup(optarg);
            } break;
            case '?':
            default: {
                fprintf(stderr, "Error: Unknown command-line option.\n");
            } exit(1);
        }
    }

    if (!output_file_name) {
        fprintf(stderr, "Error: No output file name provided.\n");
        exit(1);
    }
}

typedef void (*fill_fn)(int * b);

void fill_PBM(int * b) {
	b[0] = rand() & 0x1;
}
void fill_PGM(int * b) {
    b[0] = rand() & 0xff;
}
void fill_PPM(int * b) {
	b[0] = rand() & 0xff;
	b[1] = rand() & 0xff;
	b[2] = rand() & 0xff;
}

int main(int argc, char * argv[]) {
    int * buffer = NULL;

    // Init
    parse_opts(argc, argv);
 
    size_t size = w * h * sizeof(int);
    if (basetype == PPM) {
        size *= 3;
    }

    buffer = malloc(size);

    // Fill
    fill_fn filler = NULL;
    switch (basetype) {
      #define CASE(x) \
        case x: \
        filler = fill_ ## x; \
        break
        CASE(PBM);
        CASE(PGM);
        CASE(PPM);
      #undef CASE
    }
    
    for (int i = 0; i < w*h; i++) {
        filler(buffer + i);
    }

    // Write
    FILE * output_file = fopen(output_file_name, "w");

    if (!output_file) {
        fprintf(stderr, "Error: Failed to open output file '%s'.\n", output_file_name);
        return 1;
    }

    pnm_type_t real_type = (pnm_type_t)basetype;
    if (is_ascii) { real_type += 3; }

    write_pnm_file(output_file, real_type, buffer, w, h, 255);

    // Deinit
    fclose(output_file);
    free(buffer);

    return 0;
}
