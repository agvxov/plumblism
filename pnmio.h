/*
 * File       : pnmio.h                                                        
 * Description: Header file for pnmio.c.
 * Author     : Nikolaos Kavvadias <nikolaos.kavvadias@gmail.com>                
 * Copyright  : (C) Nikolaos Kavvadias 2012-2022
 * Website    : http://www.nkavvadias.com                            
 *                                                                          
 * This file is part of libpnmio, and is distributed under the terms of the  
 * Modified BSD License.
 *
 * A copy of the Modified BSD License is included with this distribution 
 * in the file LICENSE.
 * libpnmio is free software: you can redistribute it and/or modify it under the
 * terms of the Modified BSD License. 
 * libpnmio is distributed in the hope that it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE. See the Modified BSD License for more details.
 * 
 * You should have received a copy of the Modified BSD License along with 
 * libpnmio. If not, see <http://www.gnu.org/licenses/>. 
 */
 
#ifndef PNMIO_H
#define PNMIO_H

#include <stdio.h>

/* Legend:
 *  PBM -> Protable Bit Map
 *  PGM -> Protable Grey Map
 *  PPM -> Protable Pix Map
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

#define IS_BIGENDIAN(x)   ((*(char*)&x) == 0)
#define IS_LITTLE_ENDIAN  (1 == *(unsigned char *)&(const int){1})

/* PNM/PFM API. */
int get_pnm_type(FILE *f);

int read_pbm_header(FILE * f, int * w, int * h, int * is_ascii);
int read_pgm_header(FILE * f, int * w, int * h, int * intensity, int *is_ascii);
int read_ppm_header(FILE * f, int * w, int * h, int * intensity, int *is_ascii);
int read_pfm_header(FILE * f, int * w, int * h, int * img_type, int *endianess);

int read_pbm_data(FILE * f, int * b, int is_ascii);
int read_pgm_data(FILE * f, int * b, int is_ascii);
int read_ppm_data(FILE * f, int * b, int is_ascii);
int read_pfm_data(FILE * f, float *img_in, int img_type, int endianess);

int write_pbm_file(FILE * f, const int * b, int w, int h, int is_ascii);
int write_pgm_file(FILE * f, const int * b, int w, int h, int intensity, int is_ascii);
int write_ppm_file(FILE * f, const int * b, int w, int h, int intensity, int is_ascii);
int write_pfm_file(FILE * f, float *img_out, int x_size, int y_size, int img_type, int endianess);

#endif
