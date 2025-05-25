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
    /* NOTE:
     *  the order is significant,
     *  because it corresponds to the mime
     */
    PNM_BIT_ASCII = 1,
    PNM_GRE_ASCII,
    PNM_PIX_ASCII,
    PNM_BIT_BINARY,
    PNM_GRE_BINARY,
    PNM_PIX_BINARY,
};

int get_pnm_type(FILE * f);

// XXX is_ascii makes no sense
int read_pnm_header(FILE * f, int * w, int * h, int * intensity, int type);
int read_pnm_data(FILE * f, int * b, int type);
int write_pnm_file(FILE * f, const int * b, int w, int h, int intensity, int type);

#endif
