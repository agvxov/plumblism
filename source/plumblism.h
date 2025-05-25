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

typedef enum {
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
} pnm_type_t;

pnm_type_t get_pnm_type(FILE * f);
int read_pnm_header(FILE * f, pnm_type_t type, int * w, int * h, int * intensity);
int read_pnm_data(FILE * f, pnm_type_t type, int * b);
int write_pnm_file(FILE * f, pnm_type_t type, const int * b, int w, int h, int intensity);

#endif
