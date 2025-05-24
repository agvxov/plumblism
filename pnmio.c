/*
 * File             : pnmio.c                                                                                                                    
 * Description: I/O facilities for PBM, PGM, PPM (PNM) binary and ASCII images.
 * Author         : Nikolaos Kavvadias <nikolaos.kavvadias@gmail.com>                                
 * Copyright    : (C) Nikolaos Kavvadias 2012-2022
 * Website        : http://www.nkavvadias.com                                                        
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

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <math.h>
#include "pnmio.h"

#ifdef DEBUG
# define debug_print(...) fprintf(stderr, __VA_ARGS__)
#else
# define debug_print(...)
#endif

#include "dictate.h"

#define    MAXLINE          1024
#define    LITTLE_ENDIAN   -1
#define    BIG_ENDIAN       1
#define    GREYSCALE_TYPE   0 /* used for PFM */
#define    RGB_TYPE         1 /* used for PFM */     

static inline int ReadFloat(FILE *fptr, float *f, int swap);
static inline int WriteFloat(FILE *fptr, float *f, int swap);
static inline int floatEqualComparison(float A, float B, float maxRelDiff);

/* Every problem is a parsing problem, if you hate yourself enough.
 *                                      - Anon; all rights reserved
 */
typedef enum {
    INITIAL,
    IN_NUMBER,
    IN_COMMENT,
} state_t;
#define DIGIT '0':                                    \
              case '1': case '2': case '3': case '4': \
              case '5': case '6': case '7': case '8': \
              case '9'
#define WS    ' ': case '\t': case '\n': case '\v': case '\f': case '\r'
#define BEGIN(s) state = s
#define DIGIT_BUFFER_TO_INT(b, n, r) \
  r = 0;                                       \
  for (int i = 0; i < n; i++) { \
      r = r * 10 + (b[i] - '0');          \
  }


// NOTE: 
//  There is an edgacase where a number is followed by a comment
//  > 10#comment\n10
//  Is this allowed?
//  Does this yield 10-10 or 1010?
//  We dont know!

int get_pnm_type(FILE * f) {
    int pnm_type = 0;
    char magic[MAXLINE];
    char line[MAXLINE];

    magic[0] = fgetc(f);
    magic[1] = fgetc(f);
 
    if (magic[0] != 'P') { goto error; }

    // XXX: cant we just change the enum values?
    // alternatively, i could use a perfect has function.
    switch (magic[1]) {
        case '1':
        case '2':
        case '3':
        case '4':
        case '5':
        case '6':
        case '7': pnm_type = magic[1] - '1'; break;
        case 'F': pnm_type = PFM_RGB       ; break;
        case 'f': pnm_type = PFM_GREYSCALE ; break;
        default: goto error;
    }

    return pnm_type;

 error:
    fprintf(stderr, "Error: Unknown PNM/PFM file; wrong magic number!\n");
    exit(1);
}

//static
int lex_header(FILE * f) {
    int r;
    char digit_buffer[12];
    int digit_buffer_empty_top = 0;

    state_t state = INITIAL;

    int c;
    do {
        c = fgetc(f);
        switch (state) {
            case INITIAL: {
                switch (c) {
                    case DIGIT: {
                        BEGIN(IN_NUMBER);
                        digit_buffer[digit_buffer_empty_top++] = c;
                    } break;
                    case '#': BEGIN(IN_COMMENT); break;
                    case WS: { ; } break;
                    default: return -1;
                }
            } break;

            case IN_NUMBER: {
                switch (c) {
                    case DIGIT: {
                        digit_buffer[digit_buffer_empty_top++] = c;
                    } break;
                    case WS: {
                        DIGIT_BUFFER_TO_INT(digit_buffer, digit_buffer_empty_top, r);
                    } return r;
                    default: return -1;
                }
            } break;

            case IN_COMMENT: {
                if (c == '\n') { BEGIN(INITIAL); }
            } break;
        }
    } while (c != EOF);

    return -1;
}

int read_pbm_header(FILE * f, int * w, int * h, int * is_ascii) {
    char magic[2];

    rewind(f);
    magic[0] = fgetc(f);
    magic[1] = fgetc(f);

    if (w       ) { *w        = lex_header(f); if (*w == -1) { return -1; } }
    if (h       ) { *h        = lex_header(f); if (*h == -1) { return -1; } }
    if (is_ascii) { *is_ascii = magic[1] < '4';  }

    if (*w == -1
    ||  *h == -1) {
        return 0;
    }

    return (*w) * (*h) * sizeof(int);
}

int read_pgm_header(FILE * f, int * w, int * h, int * intensity, int * is_ascii) {
    char magic[2];

    rewind(f);
    magic[0] = fgetc(f);
    magic[1] = fgetc(f);

    if (w        ) { *w         = lex_header(f); if (*w         == -1) { return -1; } }
    if (h        ) { *h         = lex_header(f); if (*h         == -1) { return -1; } }
    if (intensity) { *intensity = lex_header(f); if (*intensity == -1) { return -1; } }
    if (is_ascii ) { *is_ascii  = magic[1] % 2;  }

    return (*w) * (*h) * sizeof(int);
}

int read_ppm_header(FILE * f, int * w, int * h, int * intensity, int * is_ascii) {
    return read_pgm_header(f, w, h, intensity, is_ascii);
}

int read_pbm_data(FILE * f, int * b, int is_ascii) {
    int r = 0;
    
    int c;
    if (is_ascii) {
        state_t state = INITIAL;
        while ((c = fgetc(f)) != EOF) {
            switch (state) {
                case INITIAL: {
                    switch (c) {
                        case '0': case '1': {
                            b[r++] = c - '0';
                        } break;
                        case WS: { ; } break;
                        case '#': { BEGIN(IN_COMMENT); } break;
                        default: return -1;
                    }
                } break;

                case IN_COMMENT: {
                    if (c == '\n') { BEGIN(INITIAL); }
                } break;
            }
        } 
    } else {
        while ((c = fgetc(f)) != EOF) {
            for (int i = 0; i < 8; i++) {
                b[r++] = (c >> (7-i)) & 0x1;
            }
        } 
    }
    
    return r;
}

int read_pgm_data(FILE * f, int * b, int is_ascii) {
    int r = 0;

    char digit_buffer[12];
    int digit_buffer_empty_top = 0;
    
    int c;
    if (is_ascii) {
        state_t state = INITIAL;
        while ((c = fgetc(f)) != EOF) {
            switch (state) {
                case INITIAL: {
                    switch (c) {
                        case DIGIT: {
                            digit_buffer[digit_buffer_empty_top++] = c;
                        } break;

                        case WS: {
                            int i;
                            DIGIT_BUFFER_TO_INT(digit_buffer, digit_buffer_empty_top, i);
                            b[r++] = i;
                        } break;

                        case '#': { BEGIN(IN_COMMENT); } break;

                        default: return -1;
                    }
                } break;
                case IN_COMMENT: {
                    if (c == '\n') { BEGIN(INITIAL); }
                } break;
            }
        } 
    } else {
        while ((c = fgetc(f)) != EOF) {
            b[r++] = c;
        } 
    }

    return r;
}

int read_ppm_data(FILE *f, int * b, int is_ascii) {
    int b_i = 0;
    char digit_buffer[12];
    int digit_buffer_empty_top = 0;
    
    int c;
    if (is_ascii) {
        state_t state = INITIAL;
        while ((c = fgetc(f)) != EOF) {
            switch (state) {
                case INITIAL: {
                    switch (c) {
                        case DIGIT: {
                            digit_buffer[digit_buffer_empty_top++] = c;
                        } break;
                        case WS: {
                            int i;
                            DIGIT_BUFFER_TO_INT(digit_buffer, digit_buffer_empty_top, i);
                            b[b_i++] = i;
                        } break;
                        case '#': { BEGIN(IN_COMMENT); } break;
                        default: return -1;
                    }
                } break;
                case IN_COMMENT: {
                    if (c == '\n') { BEGIN(INITIAL); }
                } break;
            }
        }
    } else {
        while (!feof(f)) {
            b[b_i++] = fgetc(f);
        }
    }

    return (b_i % 3 == 0 ? b_i : -1);
}

int write_pbm_file(FILE * f, const int * b, int w, int h, int is_ascii) {
    // Magic
    fputs(is_ascii ? "P1 " : "P4 ", f);

    // Header
    fprintf(f, "%d %d\n", w, h);
    
    // Data
    if (is_ascii) {
        for (int i = 0; i < w*h; i++) {
            fprintf(f, "%d", b[i]);
            if ((i + 1) % w == 0) {
                fputc('\n', f);
            }
        }
    } else {
        for (int i = 0; i < w*h; i++) {
            int v = 0;
            for (int h = 0; h < 8; h++) {
                v |= (b[i] << (7-h));
            }
            fputc('0' + v, f);
        }
    }
}

int write_pgm_file(FILE * f, const int * b, int w, int h, int intensity, int is_ascii) {
    int r = 0;

    // Magic
    fputs(is_ascii ? "P2 " : "P5 ", f);

    // Header
    fprintf(f, "%d %d %d\n", w, h, intensity);
    
    // Data
    if (is_ascii) {
        for (int i = 0; i < w*h; i++) {
            fprintf(f, "%d ", b[i]);
            if ((i + 1) % w == 0) {
                fputc('\n', f);
            }
        }
    } else {
        for (int i = 0; i < w*h; i++) {
            fprintf(f, "%c", b[i]);
        }
    }

    return r;
}

int write_ppm_file(FILE * f, const int * b, int w, int h, int intensity, int is_ascii) {
    int r = 0;

    // Magic
    fputs(is_ascii ? "P3 " : "P6 ", f);

    // Header
    fprintf(f, "%d %d %d\n", w, h, intensity);

    // Data
    if (is_ascii) {
        int b_i = 0;
        for (int i = 0; i < w*h; i++) {
            fprintf(f, "%d %d %d  ", 
                b[b_i++], 
                b[b_i++], 
                b[b_i++]
            );
            if ((i + 1) % w == 0) {
                fprintf(f, "\n");
            }
        }
    } else {
        for (int i = 0; i < w*h*3; i++) {
            fprintf(f, "%c", b[i]);
        }
    }
}

// ---
int write_pfm_file(FILE *f, float *img_out,
    int x_size, int y_size, 
    int img_type, int endianess) {
    int i, j, x_scaled_size, y_scaled_size;
    int swap = (endianess == 1) ? 0 : 1;
    float fendian = (endianess == 1) ? +1.0 : -1.0;
    
    x_scaled_size = x_size;
    y_scaled_size = y_size;

    /* Write the magic number string. */
    if (img_type == RGB_TYPE) {
        fprintf(f, "PF\n");
    } else if (img_type == GREYSCALE_TYPE) {
        fprintf(f, "Pf\n");
    } else {
        fprintf(stderr, "Error: Image type invalid for PFM format!\n");
        exit(1);        
    }
    /* Write the image dimensions. */
    fprintf(f, "%d %d\n", x_scaled_size, y_scaled_size);
    /* Write the endianess/scale factor as float. */
    fprintf(f, "%f\n", fendian);
    
    /* Write the image data. */
    for (i = 0; i < y_scaled_size; i++) {
        for (j = 0; j < x_scaled_size; j++) {
            if (img_type == RGB_TYPE) {
                WriteFloat(f, &img_out[3*(i*x_scaled_size+j)+0], swap);
                WriteFloat(f, &img_out[3*(i*x_scaled_size+j)+1], swap);
                WriteFloat(f, &img_out[3*(i*x_scaled_size+j)+2], swap);
            } else if (img_type == GREYSCALE_TYPE) {
                WriteFloat(f, &img_out[i*x_scaled_size+j], swap);
            }
        }
    }    
}

/* ReadFloat:
 * Read a possibly byte swapped floating-point number.
 * NOTE: Assume IEEE format.
 * Source: http://paulbourke.net/dataformats/pbmhdr/
 */
static inline
int ReadFloat(FILE *fptr, float *f, int swap) {
    unsigned char *cptr;

    if (fread(f, sizeof(float), 1, fptr) != 1) {
        return (false);
    }     
    if (swap) {
        cptr        = (unsigned char *)f;
        unsigned char tmp = cptr[0];
        cptr[0] = cptr[3];
        cptr[3] = tmp;
        tmp         = cptr[1];
        cptr[1] = cptr[2];
        cptr[2] = tmp;
    }
    return (true);
}

/* WriteFloat:
 * Write a possibly byte-swapped floating-point number.
 * NOTE: Assume IEEE format.
 */
static inline
int WriteFloat(FILE *fptr, float *f, int swap) {
    unsigned char *cptr;

    if (swap) {
        cptr        = (unsigned char*)f;
        unsigned char tmp = cptr[0];
        cptr[0] = cptr[3];
        cptr[3] = tmp;
        tmp         = cptr[1];
        cptr[1] = cptr[2];
        cptr[2] = tmp;
    }
    if (fwrite(f, sizeof(float), 1, fptr) != 1) {
        return (false);
    }    
    return (true); 
}

/* floatEqualComparison: 
 * Compare two floats and accept equality if not different than
 * maxRelDiff (a specified maximum relative difference).
 */
static inline
int floatEqualComparison(float A, float B, float maxRelDiff) {
    float largest, diff = fabs(A-B);
    A = fabs(A);
    B = fabs(B);
    largest = (B > A) ? B : A;
    return (diff <= largest * maxRelDiff);
}

/* read_pfm_header:
 * Read the header contents of a PFM (portable float map) file.
 * Returns the number of bytes that need be allocated for the image data.
 * A PFM image file follows the format:
 * [PF|Pf]
 * <X> <Y> 
 * (endianess) {R1}{G1}{B1} ... {RMAX}{GMAX}{BMAX} 
 * NOTE1: Comment lines, if allowed, start with '#'.
 # NOTE2: < > denote integer values (in decimal).
 # NOTE3: ( ) denote floating-point values (in decimal).
 # NOTE4: { } denote floating-point values (coded in binary).
 */
int read_pfm_header(FILE *f, int *img_xdim, int *img_ydim, int *img_type, int *endianess) {
    int x_val, y_val;
    unsigned int i;
    int is_rgb=0, is_greyscale=0;
    float aspect_ratio=0;
    char magic[MAXLINE];
    char line[MAXLINE];
    int count=0;
    int num_bytes=0;

    /* Read the PFM file header. */
    while (fgets(line, MAXLINE, f) != NULL) {
        int flag = 0;
        for (i = 0; i < strlen(line); i++) {
            if (isgraph(line[i]) && (flag == 0)) {
                if ((line[i] == '#') && (flag == 0)) {
                    flag = 1;
                }
            }
        }
        if (flag == 0) {
            if (count == 0) {
                count += sscanf(line, "%2s %d %d %f", magic, &x_val, &y_val, &aspect_ratio);
            } else if (count == 1) {
                count += sscanf(line, "%d %d %f", &x_val, &y_val, &aspect_ratio);
            } else if (count == 2) {
                count += sscanf(line, "%d %f", &y_val, &aspect_ratio);
            } else if (count == 3) {
                count += sscanf(line, "%f", &aspect_ratio);
            }
        }
        if (count == 4) {
            break;
        }
    }

    if (strcmp(magic, "PF") == 0) {
        is_rgb             = 1;
        is_greyscale = 0;
    } else if (strcmp(magic, "Pf") == 0) {
        is_greyscale = 0;
        is_rgb             = 1;        
    } else {
        fprintf(stderr, "Error: Input file not in PFM format!\n");
        exit(1);
    }            

    fprintf(stderr, "Info: magic=%s, x_val=%d, y_val=%d, aspect_ratio=%f\n",
        magic, x_val, y_val, aspect_ratio);

    /* FIXME: Aspect ratio different to 1.0 is not yet supported. */
    if (!floatEqualComparison(aspect_ratio, -1.0, 1E-06) &&
            !floatEqualComparison(aspect_ratio, 1.0, 1E-06)) {
        fprintf(stderr, "Error: Aspect ratio different to -1.0 or +1.0 is unsupported!\n");
        exit(1);
    }

    *img_xdim     = x_val;
    *img_ydim     = y_val;
    *img_type     = is_rgb & ~is_greyscale;
    if (aspect_ratio > 0.0) {
        *endianess = 1;
    } else {
        *endianess = -1;
    }

    num_bytes = *img_xdim * *img_ydim * sizeof(float);
    if (is_rgb) {
        num_bytes *= 3;
    }
    return num_bytes;
}

/* read_pfm_data:
 * Read the data contents of a PFM (portable float map) file.
 */
int read_pfm_data(FILE *f, float *img_in, int img_type, int endianess) {
    int i=0, c;
    int swap = (endianess == 1) ? 0 : 1;
    float r_val, g_val, b_val;
        
    /* Read the rest of the PFM file. */
    while ((c = fgetc(f)) != EOF) {
        ungetc(c, f);     
        /* Read a possibly byte-swapped float. */
        if (img_type == RGB_TYPE) {
            ReadFloat(f, &r_val, swap);
            ReadFloat(f, &g_val, swap);
            ReadFloat(f, &b_val, swap); 
            img_in[i++] = r_val;
            img_in[i++] = g_val;
            img_in[i++] = b_val;
        } else if (img_type == GREYSCALE_TYPE) {
            ReadFloat(f, &g_val, swap);
            img_in[i++] = g_val;
        }
    }
}
