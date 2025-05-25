#ifndef PLUMBLISM_H
#define PLUMBLISM_H

#include <stdio.h>

/* Legend:
 *  PBM -> Portable Bit Map
 *  PGM -> Portable Grey Map
 *  PPM -> Portable Pix Map
 *
 *  PBMplus -> PBM, PGM and PPM
 *  PNM -> Portable Any Map; synonym for PBMplus,
 *          I have no clue who came up with it
 */

enum {
    PBM_ASCII     =  1,
    PGM_ASCII     =  2,
    PPM_ASCII     =  3,
    PBM_BINARY    =  4,
    PGM_BINARY    =  5,
    PPM_BINARY    =  6,
    PAM           =  7, /* reserved */
                        /* 8-15: reserved */
    PFM_RGB       = 16, /* F */
    PFM_GREYSCALE = 17, /* f */
};

enum {
    PNM_BIT_ASCII  = 1,
    PNM_BIT_BINARY,
    PNM_GRE_ASCII,
    PNM_GRE_BINARY,
    PNM_PIX_ASCII,
    PNM_PIX_BINARY,
};

#define IS_BIGENDIAN(x)   ((*(char*)&x) == 0)
#define IS_LITTLE_ENDIAN  (1 == *(unsigned char *)&(const int){1})

/* PNM API */
int get_pnm_type(FILE * f);

// XXX is_ascii makes no sense
int read_pnm_header(FILE * f, int * w, int * h, int * intensity, int * is_ascii, int type);

int read_pnm_data(FILE * f, int * b, int type);

int write_pbm_file(FILE * f, const int * b, int w, int h, int is_ascii);
int write_pgm_file(FILE * f, const int * b, int w, int h, int intensity, int is_ascii);
int write_ppm_file(FILE * f, const int * b, int w, int h, int intensity, int is_ascii);

/* PFM API */
int read_pfm_header(FILE * f, int * w, int * h, int * img_type, int * endianess);
int read_pfm_data(FILE * f, float *img_in, int img_type, int endianess);
int write_pfm_file(FILE * f, float *img_out, int x_size, int y_size, int img_type, int endianess);

#endif
