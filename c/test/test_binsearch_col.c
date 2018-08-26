// Nicola Asuni

#if __STDC_VERSION__ >= 199901L
#define _XOPEN_SOURCE 600
#else
#define _XOPEN_SOURCE 500
#endif

#include <stdio.h>
#include <string.h>
#include <time.h>
#include <errno.h>
#include <sys/mman.h>
#include "../src/binsearch.h"

#define TEST_DATA_SIZE 38
#define TEST_DATA_ITEMS 251

typedef struct t_test_col_uint8_t
{
    uint64_t first;
    uint64_t last;
    uint8_t search;
    uint64_t foundFirst;
    uint64_t foundFFirst;
    uint64_t foundFLast;
    uint64_t foundLast;
    uint64_t foundLFirst;
    uint64_t foundLLast;
} t_test_col_uint8_t;

// bytes=1, bitstart=0, bitend=7, bitmask=00000000000000ff, rshift=0

static t_test_col_uint8_t test_col_data_uint8_t[] =
{
    {  0, 250, 0x00,   0,   0,   1,   1,   2,   1},
    {  1, 250, 0x00,   1,   1,   0,   1,   2,   1},
    {  0, 250, 0x01,   2,   2,   1,   2,   3,   2},
    {  0, 250, 0x0f,  16,  16,  15,  16,  17,  16},
    {  0, 250, 0x10,  17,  17,  16,  17,  18,  17},
    {  0, 250, 0x1f,  32,  32,  31,  32,  33,  32},
    {  0, 250, 0x20,  33,  33,  32,  33,  34,  33},
    {  0, 250, 0x2f,  48,  48,  47,  48,  49,  48},
    {  0, 250, 0x30,  49,  49,  48,  49,  50,  49},
    {  0, 250, 0x3f,  64,  64,  63,  64,  65,  64},
    {  0, 250, 0x40,  65,  65,  64,  65,  66,  65},
    {  0, 250, 0x4f,  80,  80,  79,  80,  81,  80},
    {  0, 250, 0x50,  81,  81,  80,  81,  82,  81},
    {  0, 250, 0x5f,  96,  96,  95,  96,  97,  96},
    {  0, 250, 0x60,  97,  97,  96,  97,  98,  97},
    {  0, 250, 0x6f, 112, 112, 111, 112, 113, 112},
    {  0, 250, 0x70, 113, 113, 112, 113, 114, 113},
    {  0, 250, 0x7f, 128, 128, 127, 128, 129, 128},
    {  0, 250, 0x80, 129, 129, 128, 129, 130, 129},
    {  0, 250, 0x8f, 144, 144, 143, 144, 145, 144},
    {  0, 250, 0x90, 145, 145, 144, 145, 146, 145},
    {  0, 250, 0x9f, 160, 160, 159, 160, 161, 160},
    {  0, 250, 0xa0, 161, 161, 160, 161, 162, 161},
    {  0, 250, 0xaf, 176, 176, 175, 176, 177, 176},
    {  0, 250, 0xb0, 177, 177, 176, 177, 178, 177},
    {  0, 250, 0xbf, 192, 192, 191, 192, 193, 192},
    {  0, 250, 0xc0, 193, 193, 192, 193, 194, 193},
    {  0, 250, 0xcf, 208, 208, 207, 208, 209, 208},
    {  0, 250, 0xd0, 209, 209, 208, 209, 210, 209},
    {  0, 250, 0xdf, 224, 224, 223, 224, 225, 224},
    {  0, 250, 0xe0, 225, 225, 224, 225, 226, 225},
    {  0, 250, 0xef, 240, 240, 239, 240, 241, 240},
    {  0, 250, 0xf0, 241, 241, 240, 241, 242, 241},
    {  0, 250, 0xf8, 249, 249, 248, 249, 250, 249},
    {  0, 250, 0xff, 250, 250, 249, 250, 251, 250},
    {  0, 250, 0xf9, 251, 250, 249, 251, 250, 249},
    {  0,  50, 0x70,  51,  51,  50,  51,  51,  50},
    {150, 250, 0x70, 251, 150, 149, 251, 150, 149},
};

static t_test_col_uint8_t test_col_data_sub_uint8_t[] =
{
    {  0, 250, 0x00,   0,   0,   1,   1,   2,   1},
    {  1, 250, 0x00,   1,   1,   0,   1,   2,   1},
    {  0, 250, 0x01,   2,   2,   1,   2,   3,   2},
    {  0, 250, 0x0f,  16,  16,  15,  16,  17,  16},
    {  0, 250, 0x10,  17,  17,  16,  17,  18,  17},
    {  0, 250, 0x1f,  32,  32,  31,  32,  33,  32},
    {  0, 250, 0x20,  33,  33,  32,  33,  34,  33},
    {  0, 250, 0x2f,  48,  48,  47,  48,  49,  48},
    {  0, 250, 0x30,  49,  49,  48,  49,  50,  49},
    {  0, 250, 0x3f,  64,  64,  63,  64,  65,  64},
    {  0, 250, 0x40,  65,  65,  64,  65,  66,  65},
    {  0, 250, 0x4f,  80,  80,  79,  80,  81,  80},
    {  0, 250, 0x50,  81,  81,  80,  81,  82,  81},
    {  0, 250, 0x5f,  96,  96,  95,  96,  97,  96},
    {  0, 250, 0x60,  97,  97,  96,  97,  98,  97},
    {  0, 250, 0x6f, 112, 112, 111, 112, 113, 112},
    {  0, 250, 0x70, 113, 113, 112, 113, 114, 113},
    {  0, 250, 0x7f, 128, 128, 127, 128, 129, 128},
    {  0, 250, 0x80, 129, 129, 128, 129, 130, 129},
    {  0, 250, 0x8f, 144, 144, 143, 144, 145, 144},
    {  0, 250, 0x90, 145, 145, 144, 145, 146, 145},
    {  0, 250, 0x9f, 160, 160, 159, 160, 161, 160},
    {  0, 250, 0xa0, 161, 161, 160, 161, 162, 161},
    {  0, 250, 0xaf, 176, 176, 175, 176, 177, 176},
    {  0, 250, 0xb0, 177, 177, 176, 177, 178, 177},
    {  0, 250, 0xbf, 192, 192, 191, 192, 193, 192},
    {  0, 250, 0xc0, 193, 193, 192, 193, 194, 193},
    {  0, 250, 0xcf, 208, 208, 207, 208, 209, 208},
    {  0, 250, 0xd0, 209, 209, 208, 209, 210, 209},
    {  0, 250, 0xdf, 224, 224, 223, 224, 225, 224},
    {  0, 250, 0xe0, 225, 225, 224, 225, 226, 225},
    {  0, 250, 0xef, 240, 240, 239, 240, 241, 240},
    {  0, 250, 0xf0, 241, 241, 240, 241, 242, 241},
    {  0, 250, 0xf8, 249, 249, 248, 249, 250, 249},
    {  0, 250, 0xff, 250, 250, 249, 250, 251, 250},
    {  0, 250, 0xf9, 251, 250, 249, 251, 250, 249},
    {  0,  50, 0x70,  51,  51,  50,  51,  51,  50},
    {150, 250, 0x70, 251, 150, 149, 251, 150, 149},
};

typedef struct t_test_col_uint16_t
{
    uint64_t first;
    uint64_t last;
    uint16_t search;
    uint64_t foundFirst;
    uint64_t foundFFirst;
    uint64_t foundFLast;
    uint64_t foundLast;
    uint64_t foundLFirst;
    uint64_t foundLLast;
} t_test_col_uint16_t;

// bytes=2, bitstart=0, bitend=15, bitmask=000000000000ffff, rshift=0

static t_test_col_uint16_t test_col_data_uint16_t[] =
{
    {  0, 250, 0x0000,   0,   0,   1,   0,   1,   0},
    {  1, 250, 0x0001,   1,   1,   0,   1,   2,   1},
    {  0, 250, 0x0102,   2,   2,   1,   2,   3,   2},
    {  0, 250, 0x0f10,  16,  16,  15,  16,  17,  16},
    {  0, 250, 0x1011,  17,  17,  16,  17,  18,  17},
    {  0, 250, 0x1f20,  32,  32,  31,  32,  33,  32},
    {  0, 250, 0x2021,  33,  33,  32,  33,  34,  33},
    {  0, 250, 0x2f30,  48,  48,  47,  48,  49,  48},
    {  0, 250, 0x3031,  49,  49,  48,  49,  50,  49},
    {  0, 250, 0x3f40,  64,  64,  63,  64,  65,  64},
    {  0, 250, 0x4041,  65,  65,  64,  65,  66,  65},
    {  0, 250, 0x4f50,  80,  80,  79,  80,  81,  80},
    {  0, 250, 0x5051,  81,  81,  80,  81,  82,  81},
    {  0, 250, 0x5f60,  96,  96,  95,  96,  97,  96},
    {  0, 250, 0x6061,  97,  97,  96,  97,  98,  97},
    {  0, 250, 0x6f70, 112, 112, 111, 112, 113, 112},
    {  0, 250, 0x7071, 113, 113, 112, 113, 114, 113},
    {  0, 250, 0x7f80, 128, 128, 127, 128, 129, 128},
    {  0, 250, 0x8081, 129, 129, 128, 129, 130, 129},
    {  0, 250, 0x8f90, 144, 144, 143, 144, 145, 144},
    {  0, 250, 0x9091, 145, 145, 144, 145, 146, 145},
    {  0, 250, 0x9fa0, 160, 160, 159, 160, 161, 160},
    {  0, 250, 0xa0a1, 161, 161, 160, 161, 162, 161},
    {  0, 250, 0xafb0, 176, 176, 175, 176, 177, 176},
    {  0, 250, 0xb0b1, 177, 177, 176, 177, 178, 177},
    {  0, 250, 0xbfc0, 192, 192, 191, 192, 193, 192},
    {  0, 250, 0xc0c1, 193, 193, 192, 193, 194, 193},
    {  0, 250, 0xcfd0, 208, 208, 207, 208, 209, 208},
    {  0, 250, 0xd0d1, 209, 209, 208, 209, 210, 209},
    {  0, 250, 0xdfe0, 224, 224, 223, 224, 225, 224},
    {  0, 250, 0xe0e1, 225, 225, 224, 225, 226, 225},
    {  0, 250, 0xeff0, 240, 240, 239, 240, 241, 240},
    {  0, 250, 0xf0f1, 241, 241, 240, 241, 242, 241},
    {  0, 250, 0xf8f9, 249, 249, 248, 249, 250, 249},
    {  0, 250, 0xffff, 250, 250, 249, 250, 251, 250},
    {  0, 250, 0xf9f9, 251, 250, 249, 251, 250, 249},
    {  0,  50, 0x7071,  51,  51,  50,  51,  51,  50},
    {150, 250, 0x7071, 251, 150, 149, 251, 150, 149},
};

static t_test_col_uint16_t test_col_data_sub_uint16_t[] =
{
    {  0, 250, 0x0000,   0,   0,   1,   0,   1,   0},
    {  1, 250, 0x0001,   1,   1,   0,   1,   2,   1},
    {  0, 250, 0x0102,   2,   2,   1,   2,   3,   2},
    {  0, 250, 0x0f10,  16,  16,  15,  16,  17,  16},
    {  0, 250, 0x1011,  17,  17,  16,  17,  18,  17},
    {  0, 250, 0x1f20,  32,  32,  31,  32,  33,  32},
    {  0, 250, 0x2021,  33,  33,  32,  33,  34,  33},
    {  0, 250, 0x2f30,  48,  48,  47,  48,  49,  48},
    {  0, 250, 0x3031,  49,  49,  48,  49,  50,  49},
    {  0, 250, 0x3f40,  64,  64,  63,  64,  65,  64},
    {  0, 250, 0x4041,  65,  65,  64,  65,  66,  65},
    {  0, 250, 0x4f50,  80,  80,  79,  80,  81,  80},
    {  0, 250, 0x5051,  81,  81,  80,  81,  82,  81},
    {  0, 250, 0x5f60,  96,  96,  95,  96,  97,  96},
    {  0, 250, 0x6061,  97,  97,  96,  97,  98,  97},
    {  0, 250, 0x6f70, 112, 112, 111, 112, 113, 112},
    {  0, 250, 0x7071, 113, 113, 112, 113, 114, 113},
    {  0, 250, 0x7f80, 128, 128, 127, 128, 129, 128},
    {  0, 250, 0x8081, 129, 129, 128, 129, 130, 129},
    {  0, 250, 0x8f90, 144, 144, 143, 144, 145, 144},
    {  0, 250, 0x9091, 145, 145, 144, 145, 146, 145},
    {  0, 250, 0x9fa0, 160, 160, 159, 160, 161, 160},
    {  0, 250, 0xa0a1, 161, 161, 160, 161, 162, 161},
    {  0, 250, 0xafb0, 176, 176, 175, 176, 177, 176},
    {  0, 250, 0xb0b1, 177, 177, 176, 177, 178, 177},
    {  0, 250, 0xbfc0, 192, 192, 191, 192, 193, 192},
    {  0, 250, 0xc0c1, 193, 193, 192, 193, 194, 193},
    {  0, 250, 0xcfd0, 208, 208, 207, 208, 209, 208},
    {  0, 250, 0xd0d1, 209, 209, 208, 209, 210, 209},
    {  0, 250, 0xdfe0, 224, 224, 223, 224, 225, 224},
    {  0, 250, 0xe0e1, 225, 225, 224, 225, 226, 225},
    {  0, 250, 0xeff0, 240, 240, 239, 240, 241, 240},
    {  0, 250, 0xf0f1, 241, 241, 240, 241, 242, 241},
    {  0, 250, 0xf8f9, 249, 249, 248, 249, 250, 249},
    {  0, 250, 0xffff, 250, 250, 249, 250, 251, 250},
    {  0, 250, 0xf9f9, 251, 250, 249, 251, 250, 249},
    {  0,  50, 0x7071,  51,  51,  50,  51,  51,  50},
    {150, 250, 0x7071, 251, 150, 149, 251, 150, 149},
};

typedef struct t_test_col_uint32_t
{
    uint64_t first;
    uint64_t last;
    uint32_t search;
    uint64_t foundFirst;
    uint64_t foundFFirst;
    uint64_t foundFLast;
    uint64_t foundLast;
    uint64_t foundLFirst;
    uint64_t foundLLast;
} t_test_col_uint32_t;

// bytes=4, bitstart=8, bitend=23, bitmask=0000000000ffffff, rshift=8

static t_test_col_uint32_t test_col_data_uint32_t[] =
{
    {  0, 250, 0x00000000,   0,   0,   1,   0,   1,   0},
    {  1, 250, 0x00010203,   1,   1,   0,   1,   2,   1},
    {  0, 250, 0x01020304,   2,   2,   1,   2,   3,   2},
    {  0, 250, 0x0f101112,  16,  16,  15,  16,  17,  16},
    {  0, 250, 0x10111213,  17,  17,  16,  17,  18,  17},
    {  0, 250, 0x1f202122,  32,  32,  31,  32,  33,  32},
    {  0, 250, 0x20212223,  33,  33,  32,  33,  34,  33},
    {  0, 250, 0x2f303132,  48,  48,  47,  48,  49,  48},
    {  0, 250, 0x30313233,  49,  49,  48,  49,  50,  49},
    {  0, 250, 0x3f404142,  64,  64,  63,  64,  65,  64},
    {  0, 250, 0x40414243,  65,  65,  64,  65,  66,  65},
    {  0, 250, 0x4f505152,  80,  80,  79,  80,  81,  80},
    {  0, 250, 0x50515253,  81,  81,  80,  81,  82,  81},
    {  0, 250, 0x5f606162,  96,  96,  95,  96,  97,  96},
    {  0, 250, 0x60616263,  97,  97,  96,  97,  98,  97},
    {  0, 250, 0x6f707172, 112, 112, 111, 112, 113, 112},
    {  0, 250, 0x70717273, 113, 113, 112, 113, 114, 113},
    {  0, 250, 0x7f808182, 128, 128, 127, 128, 129, 128},
    {  0, 250, 0x80818283, 129, 129, 128, 129, 130, 129},
    {  0, 250, 0x8f909192, 144, 144, 143, 144, 145, 144},
    {  0, 250, 0x90919293, 145, 145, 144, 145, 146, 145},
    {  0, 250, 0x9fa0a1a2, 160, 160, 159, 160, 161, 160},
    {  0, 250, 0xa0a1a2a3, 161, 161, 160, 161, 162, 161},
    {  0, 250, 0xafb0b1b2, 176, 176, 175, 176, 177, 176},
    {  0, 250, 0xb0b1b2b3, 177, 177, 176, 177, 178, 177},
    {  0, 250, 0xbfc0c1c2, 192, 192, 191, 192, 193, 192},
    {  0, 250, 0xc0c1c2c3, 193, 193, 192, 193, 194, 193},
    {  0, 250, 0xcfd0d1d2, 208, 208, 207, 208, 209, 208},
    {  0, 250, 0xd0d1d2d3, 209, 209, 208, 209, 210, 209},
    {  0, 250, 0xdfe0e1e2, 224, 224, 223, 224, 225, 224},
    {  0, 250, 0xe0e1e2e3, 225, 225, 224, 225, 226, 225},
    {  0, 250, 0xeff0f1f2, 240, 240, 239, 240, 241, 240},
    {  0, 250, 0xf0f1f2f3, 241, 241, 240, 241, 242, 241},
    {  0, 250, 0xf8f9fafb, 249, 249, 248, 249, 250, 249},
    {  0, 250, 0xffffffff, 250, 250, 249, 250, 251, 250},
    {  0, 250, 0xf9f9fafb, 251, 250, 249, 251, 250, 249},
    {  0,  50, 0x70717273,  51,  51,  50,  51,  51,  50},
    {150, 250, 0x70717273, 251, 150, 149, 251, 150, 149},
};

static t_test_col_uint32_t test_col_data_sub_uint32_t[] =
{
    {  0, 250, 0x00000000,   0,   0,   1,   0,   1,   0},
    {  1, 250, 0x00000102,   1,   1,   0,   1,   2,   1},
    {  0, 250, 0x00000203,   2,   2,   1,   2,   3,   2},
    {  0, 250, 0x00001011,  16,  16,  15,  16,  17,  16},
    {  0, 250, 0x00001112,  17,  17,  16,  17,  18,  17},
    {  0, 250, 0x00002021,  32,  32,  31,  32,  33,  32},
    {  0, 250, 0x00002122,  33,  33,  32,  33,  34,  33},
    {  0, 250, 0x00003031,  48,  48,  47,  48,  49,  48},
    {  0, 250, 0x00003132,  49,  49,  48,  49,  50,  49},
    {  0, 250, 0x00004041,  64,  64,  63,  64,  65,  64},
    {  0, 250, 0x00004142,  65,  65,  64,  65,  66,  65},
    {  0, 250, 0x00005051,  80,  80,  79,  80,  81,  80},
    {  0, 250, 0x00005152,  81,  81,  80,  81,  82,  81},
    {  0, 250, 0x00006061,  96,  96,  95,  96,  97,  96},
    {  0, 250, 0x00006162,  97,  97,  96,  97,  98,  97},
    {  0, 250, 0x00007071, 112, 112, 111, 112, 113, 112},
    {  0, 250, 0x00007172, 113, 113, 112, 113, 114, 113},
    {  0, 250, 0x00008081, 128, 128, 127, 128, 129, 128},
    {  0, 250, 0x00008182, 129, 129, 128, 129, 130, 129},
    {  0, 250, 0x00009091, 144, 144, 143, 144, 145, 144},
    {  0, 250, 0x00009192, 145, 145, 144, 145, 146, 145},
    {  0, 250, 0x0000a0a1, 160, 160, 159, 160, 161, 160},
    {  0, 250, 0x0000a1a2, 161, 161, 160, 161, 162, 161},
    {  0, 250, 0x0000b0b1, 176, 176, 175, 176, 177, 176},
    {  0, 250, 0x0000b1b2, 177, 177, 176, 177, 178, 177},
    {  0, 250, 0x0000c0c1, 192, 192, 191, 192, 193, 192},
    {  0, 250, 0x0000c1c2, 193, 193, 192, 193, 194, 193},
    {  0, 250, 0x0000d0d1, 208, 208, 207, 208, 209, 208},
    {  0, 250, 0x0000d1d2, 209, 209, 208, 209, 210, 209},
    {  0, 250, 0x0000e0e1, 224, 224, 223, 224, 225, 224},
    {  0, 250, 0x0000e1e2, 225, 225, 224, 225, 226, 225},
    {  0, 250, 0x0000f0f1, 240, 240, 239, 240, 241, 240},
    {  0, 250, 0x0000f1f2, 241, 241, 240, 241, 242, 241},
    {  0, 250, 0x0000f9fa, 249, 249, 248, 249, 250, 249},
    {  0, 250, 0x0000ffff, 250, 250, 249, 250, 251, 250},
    {  0, 250, 0x0000f9fa, 249, 249, 248, 249, 250, 249},
    {  0,  50, 0x00007172,  51,  51,  50,  51,  51,  50},
    {150, 250, 0x00007172, 251, 150, 149, 251, 150, 149},
};

typedef struct t_test_col_uint64_t
{
    uint64_t first;
    uint64_t last;
    uint64_t search;
    uint64_t foundFirst;
    uint64_t foundFFirst;
    uint64_t foundFLast;
    uint64_t foundLast;
    uint64_t foundLFirst;
    uint64_t foundLLast;
} t_test_col_uint64_t;

// bytes=8, bitstart=16, bitend=47, bitmask=0000ffffffffffff, rshift=16

static t_test_col_uint64_t test_col_data_uint64_t[] =
{
    {  0, 250, 0x0000000000000000,   0,   0,   1,   0,   1,   0},
    {  1, 250, 0x0001020304050607,   1,   1,   0,   1,   2,   1},
    {  0, 250, 0x0102030405060708,   2,   2,   1,   2,   3,   2},
    {  0, 250, 0x0f10111213141516,  16,  16,  15,  16,  17,  16},
    {  0, 250, 0x1011121314151617,  17,  17,  16,  17,  18,  17},
    {  0, 250, 0x1f20212223242526,  32,  32,  31,  32,  33,  32},
    {  0, 250, 0x2021222324252627,  33,  33,  32,  33,  34,  33},
    {  0, 250, 0x2f30313233343536,  48,  48,  47,  48,  49,  48},
    {  0, 250, 0x3031323334353637,  49,  49,  48,  49,  50,  49},
    {  0, 250, 0x3f40414243444546,  64,  64,  63,  64,  65,  64},
    {  0, 250, 0x4041424344454647,  65,  65,  64,  65,  66,  65},
    {  0, 250, 0x4f50515253545556,  80,  80,  79,  80,  81,  80},
    {  0, 250, 0x5051525354555657,  81,  81,  80,  81,  82,  81},
    {  0, 250, 0x5f60616263646566,  96,  96,  95,  96,  97,  96},
    {  0, 250, 0x6061626364656667,  97,  97,  96,  97,  98,  97},
    {  0, 250, 0x6f70717273747576, 112, 112, 111, 112, 113, 112},
    {  0, 250, 0x7071727374757677, 113, 113, 112, 113, 114, 113},
    {  0, 250, 0x7f80818283848586, 128, 128, 127, 128, 129, 128},
    {  0, 250, 0x8081828384858687, 129, 129, 128, 129, 130, 129},
    {  0, 250, 0x8f90919293949596, 144, 144, 143, 144, 145, 144},
    {  0, 250, 0x9091929394959697, 145, 145, 144, 145, 146, 145},
    {  0, 250, 0x9fa0a1a2a3a4a5a6, 160, 160, 159, 160, 161, 160},
    {  0, 250, 0xa0a1a2a3a4a5a6a7, 161, 161, 160, 161, 162, 161},
    {  0, 250, 0xafb0b1b2b3b4b5b6, 176, 176, 175, 176, 177, 176},
    {  0, 250, 0xb0b1b2b3b4b5b6b7, 177, 177, 176, 177, 178, 177},
    {  0, 250, 0xbfc0c1c2c3c4c5c6, 192, 192, 191, 192, 193, 192},
    {  0, 250, 0xc0c1c2c3c4c5c6c7, 193, 193, 192, 193, 194, 193},
    {  0, 250, 0xcfd0d1d2d3d4d5d6, 208, 208, 207, 208, 209, 208},
    {  0, 250, 0xd0d1d2d3d4d5d6d7, 209, 209, 208, 209, 210, 209},
    {  0, 250, 0xdfe0e1e2e3e4e5e6, 224, 224, 223, 224, 225, 224},
    {  0, 250, 0xe0e1e2e3e4e5e6e7, 225, 225, 224, 225, 226, 225},
    {  0, 250, 0xeff0f1f2f3f4f5f6, 240, 240, 239, 240, 241, 240},
    {  0, 250, 0xf0f1f2f3f4f5f6f7, 241, 241, 240, 241, 242, 241},
    {  0, 250, 0xf8f9fafbfcfdfeff, 249, 249, 248, 249, 250, 249},
    {  0, 250, 0xffffffffffffffff, 250, 250, 249, 250, 251, 250},
    {  0, 250, 0xf9f9fafbfcfdfeff, 251, 250, 249, 251, 250, 249},
    {  0,  50, 0x7071727374757677,  51,  51,  50,  51,  51,  50},
    {150, 250, 0x7071727374757677, 251, 150, 149, 251, 150, 149},
};

static t_test_col_uint64_t test_col_data_sub_uint64_t[] =
{
    {  0, 250, 0x0000000000000000,   0,   0,   1,   0,   1,   0},
    {  1, 250, 0x0000000002030405,   1,   1,   0,   1,   2,   1},
    {  0, 250, 0x0000000003040506,   2,   2,   1,   2,   3,   2},
    {  0, 250, 0x0000000011121314,  16,  16,  15,  16,  17,  16},
    {  0, 250, 0x0000000012131415,  17,  17,  16,  17,  18,  17},
    {  0, 250, 0x0000000021222324,  32,  32,  31,  32,  33,  32},
    {  0, 250, 0x0000000022232425,  33,  33,  32,  33,  34,  33},
    {  0, 250, 0x0000000031323334,  48,  48,  47,  48,  49,  48},
    {  0, 250, 0x0000000032333435,  49,  49,  48,  49,  50,  49},
    {  0, 250, 0x0000000041424344,  64,  64,  63,  64,  65,  64},
    {  0, 250, 0x0000000042434445,  65,  65,  64,  65,  66,  65},
    {  0, 250, 0x0000000051525354,  80,  80,  79,  80,  81,  80},
    {  0, 250, 0x0000000052535455,  81,  81,  80,  81,  82,  81},
    {  0, 250, 0x0000000061626364,  96,  96,  95,  96,  97,  96},
    {  0, 250, 0x0000000062636465,  97,  97,  96,  97,  98,  97},
    {  0, 250, 0x0000000071727374, 112, 112, 111, 112, 113, 112},
    {  0, 250, 0x0000000072737475, 113, 113, 112, 113, 114, 113},
    {  0, 250, 0x0000000081828384, 128, 128, 127, 128, 129, 128},
    {  0, 250, 0x0000000082838485, 129, 129, 128, 129, 130, 129},
    {  0, 250, 0x0000000091929394, 144, 144, 143, 144, 145, 144},
    {  0, 250, 0x0000000092939495, 145, 145, 144, 145, 146, 145},
    {  0, 250, 0x00000000a1a2a3a4, 160, 160, 159, 160, 161, 160},
    {  0, 250, 0x00000000a2a3a4a5, 161, 161, 160, 161, 162, 161},
    {  0, 250, 0x00000000b1b2b3b4, 176, 176, 175, 176, 177, 176},
    {  0, 250, 0x00000000b2b3b4b5, 177, 177, 176, 177, 178, 177},
    {  0, 250, 0x00000000c1c2c3c4, 192, 192, 191, 192, 193, 192},
    {  0, 250, 0x00000000c2c3c4c5, 193, 193, 192, 193, 194, 193},
    {  0, 250, 0x00000000d1d2d3d4, 208, 208, 207, 208, 209, 208},
    {  0, 250, 0x00000000d2d3d4d5, 209, 209, 208, 209, 210, 209},
    {  0, 250, 0x00000000e1e2e3e4, 224, 224, 223, 224, 225, 224},
    {  0, 250, 0x00000000e2e3e4e5, 225, 225, 224, 225, 226, 225},
    {  0, 250, 0x00000000f1f2f3f4, 240, 240, 239, 240, 241, 240},
    {  0, 250, 0x00000000f2f3f4f5, 241, 241, 240, 241, 242, 241},
    {  0, 250, 0x00000000fafbfcfd, 249, 249, 248, 249, 250, 249},
    {  0, 250, 0x00000000ffffffff, 250, 250, 249, 250, 251, 250},
    {  0, 250, 0x00000000fafbfcfd, 249, 249, 248, 249, 250, 249},
    {  0,  50, 0x0000000072737475,  51,  51,  50,  51,  51,  50},
    {150, 250, 0x0000000072737475, 251, 150, 149, 251, 150, 149},
};

#define define_test_col_find_first(T) \
int test_col_find_first_##T(mmfile_t mf) \
{ \
    int errors = 0; \
    int i; \
    const T *src = get_src_offset_##T(mf.src, (TEST_DATA_ITEMS * (sizeof(T) - 1))); \
    uint8_t nbytes = (uint8_t)sizeof(T); \
    uint8_t bitstart = ((nbytes >> 2) * 8); \
    uint8_t bitend = ((8 * nbytes) - 1 - bitstart); \
    uint64_t ffound, lfound, first, last, numitems, counter, pos; \
    for (i=0 ; i < TEST_DATA_SIZE; i++) \
    { \
        first = test_col_data_##T[i].first; \
        last = test_col_data_##T[i].last; \
        ffound = col_find_first_##T(src, &first, &last, test_col_data_##T[i].search); \
        if (ffound != test_col_data_##T[i].foundFirst) \
        { \
            fprintf(stderr, "%s (%d) Expected found %" PRIx64 ", got %" PRIx64 "\n", __func__, i, test_col_data_##T[i].foundFirst, ffound); \
            ++errors; \
        } \
        if (first != test_col_data_##T[i].foundFFirst) \
        { \
            fprintf(stderr, "%s (%d) Expected first %" PRIx64 ", got %" PRIx64 "\n", __func__, i, test_col_data_##T[i].foundFFirst, first); \
            ++errors; \
        } \
        if (last != test_col_data_##T[i].foundFLast) \
        { \
            fprintf(stderr, "%s (%d) Expected last %" PRIx64 ", got %" PRIx64 "\n", __func__, i, test_col_data_##T[i].foundFLast, last); \
            ++errors; \
        } \
        numitems = (test_col_data_##T[i].foundLast - test_col_data_##T[i].foundFirst); \
        if (ffound <= test_col_data_##T[i].last) \
        { \
            pos = ffound; \
            counter = 0; \
            while (col_has_next_##T(src, &pos, test_col_data_##T[i].last, test_col_data_##T[i].search)) \
            { \
              counter++; \
            } \
            if (counter != numitems) \
            { \
                fprintf(stderr, "%s HAS_NEXT (%d) Expected %" PRIu64 ", got %" PRIu64 "\n", __func__, i, numitems, counter); \
                ++errors; \
            } \
        } \
        first = test_col_data_sub_##T[i].first; \
        last = test_col_data_sub_##T[i].last; \
        lfound = col_find_first_sub_##T(src, bitstart, bitend, &first, &last, test_col_data_sub_##T[i].search); \
        if (lfound != test_col_data_sub_##T[i].foundFirst) \
        { \
            fprintf(stderr, "%s SUB (%d) Expected found %" PRIx64 ", got %" PRIx64 "\n", __func__, i, test_col_data_sub_##T[i].foundFirst, lfound); \
            ++errors; \
        } \
        if (first != test_col_data_sub_##T[i].foundFFirst) \
        { \
            fprintf(stderr, "%s SUB (%d) Expected first %" PRIx64 ", got %" PRIx64 "\n", __func__, i, test_col_data_sub_##T[i].foundFFirst, first); \
            ++errors; \
        } \
        if (last != test_col_data_sub_##T[i].foundFLast) \
        { \
            fprintf(stderr, "%s SUB (%d) Expected last %" PRIx64 ", got %" PRIx64 "\n", __func__, i, test_col_data_sub_##T[i].foundFLast, last); \
            ++errors; \
        } \
        numitems = (test_col_data_sub_##T[i].foundLast - test_col_data_sub_##T[i].foundFirst); \
        if (lfound <= test_col_data_sub_##T[i].last) \
        { \
            pos = lfound; \
            counter = 0; \
            while (col_has_next_sub_##T(src, bitstart, bitend, &pos, test_col_data_sub_##T[i].last, test_col_data_sub_##T[i].search)) \
            { \
              counter++; \
            } \
            if (counter != numitems) \
            { \
                fprintf(stderr, "%s HAS_NEXT_SUB (%d) Expected %" PRIu64 ", got %" PRIu64 "\n", __func__, i, numitems, counter); \
                ++errors; \
            } \
        } \
    } \
    return errors; \
}

define_test_col_find_first(uint8_t)
define_test_col_find_first(uint16_t)
define_test_col_find_first(uint32_t)
define_test_col_find_first(uint64_t)

#define define_test_col_find_last(T) \
int test_col_find_last_##T(mmfile_t mf) \
{ \
    int errors = 0; \
    int i; \
    const T *src = get_src_offset_##T(mf.src, (TEST_DATA_ITEMS * (sizeof(T) - 1))); \
    uint8_t nbytes = (uint8_t)sizeof(T); \
    uint8_t bitstart = ((nbytes >> 2) * 8); \
    uint8_t bitend = ((8 * nbytes) - 1 - bitstart); \
    uint64_t ffound, lfound, first, last, numitems, counter, pos; \
    for (i=0 ; i < TEST_DATA_SIZE; i++) \
    { \
        first = test_col_data_##T[i].first; \
        last = test_col_data_##T[i].last; \
        ffound = col_find_last_##T(src, &first, &last, test_col_data_##T[i].search); \
        if (ffound != test_col_data_##T[i].foundLast) \
        { \
            fprintf(stderr, "%s (%d) Expected found %" PRIx64 ", got %" PRIx64 "\n", __func__, i, test_col_data_##T[i].foundLast, ffound); \
            ++errors; \
        } \
        if (first != test_col_data_##T[i].foundLFirst) \
        { \
            fprintf(stderr, "%s (%d) Expected first %" PRIx64 ", got %" PRIx64 "\n", __func__, i, test_col_data_##T[i].foundLFirst, first); \
            ++errors; \
        } \
        if (last != test_col_data_##T[i].foundLLast) \
        { \
            fprintf(stderr, "%s (%d) Expected last %" PRIx64 ", got %" PRIx64 "\n", __func__, i, test_col_data_##T[i].foundLLast, last); \
            ++errors; \
        } \
        numitems = (test_col_data_##T[i].foundLast - test_col_data_##T[i].foundFirst); \
        if (ffound <= test_col_data_##T[i].last) \
        { \
            pos = ffound; \
            counter = 0; \
            while (col_has_prev_##T(src, test_col_data_sub_##T[i].first, &pos, test_col_data_##T[i].search)) \
            { \
              counter++; \
            } \
            if (counter != numitems) \
            { \
                fprintf(stderr, "%s HAS_PREV (%d) Expected %" PRIu64 ", got %" PRIu64 "\n", __func__, i, numitems, counter); \
                ++errors; \
            } \
        } \
        first = test_col_data_sub_##T[i].first; \
        last = test_col_data_sub_##T[i].last; \
        lfound = col_find_last_sub_##T(src, bitstart, bitend, &first, &last, test_col_data_sub_##T[i].search); \
        if (lfound != test_col_data_sub_##T[i].foundLast) \
        { \
            fprintf(stderr, "%s SUB (%d) Expected found %" PRIx64 ", got %" PRIx64 "\n", __func__, i, test_col_data_sub_##T[i].foundLast, lfound); \
            ++errors; \
        } \
        if (first != test_col_data_sub_##T[i].foundLFirst) \
        { \
            fprintf(stderr, "%s SUB (%d) Expected first %" PRIx64 ", got %" PRIx64 "\n", __func__, i, test_col_data_sub_##T[i].foundLFirst, first); \
            ++errors; \
        } \
        if (last != test_col_data_sub_##T[i].foundLLast) \
        { \
            fprintf(stderr, "%s SUB (%d) Expected last %" PRIx64 ", got %" PRIx64 "\n", __func__, i, test_col_data_sub_##T[i].foundLLast, last); \
            ++errors; \
        } \
        numitems = (test_col_data_sub_##T[i].foundLast - test_col_data_sub_##T[i].foundFirst); \
        if (lfound <= test_col_data_sub_##T[i].last) \
        { \
            pos = lfound; \
            counter = 0; \
            while (col_has_prev_sub_##T(src, bitstart, bitend, test_col_data_sub_##T[i].first, &pos, test_col_data_sub_##T[i].search)) \
            { \
              counter++; \
            } \
            if (counter != numitems) \
            { \
                fprintf(stderr, "%s HAS_PREV_SUB (%d) Expected %" PRIu64 ", got %" PRIu64 "\n", __func__, i, numitems, counter); \
                ++errors; \
            } \
        } \
    } \
    return errors; \
}

define_test_col_find_last(uint8_t)
define_test_col_find_last(uint16_t)
define_test_col_find_last(uint32_t)
define_test_col_find_last(uint64_t)

// returns current time in nanoseconds
uint64_t get_time()
{
    struct timespec t;
    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &t);
    return (((uint64_t)t.tv_sec * 1000000000) + (uint64_t)t.tv_nsec);
}

#define define_benchmark_col_find_first(T) \
void benchmark_col_find_first_##T(mmfile_t mf) \
{ \
    uint64_t tstart, tend; \
    uint64_t first = 0; \
    uint64_t last = (TEST_DATA_ITEMS - 1); \
    uint64_t found; \
    int i; \
    const T *src = get_src_offset_##T(mf.src, (TEST_DATA_ITEMS * (sizeof(T) - 1))); \
    int size = 10000; \
    tstart = get_time(); \
    for (i=0 ; i < size; i++) \
    { \
        first = 0; \
        last = (TEST_DATA_ITEMS - 1); \
        found = col_find_first_##T(src, &first, &last, test_col_data_##T[4].search); \
    } \
    tend = get_time(); \
    fprintf(stdout, " * %s : %lu ns/op (%" PRIx64 ")\n", __func__, (tend - tstart)/(size*4), found); \
}

define_benchmark_col_find_first(uint8_t)
define_benchmark_col_find_first(uint16_t)
define_benchmark_col_find_first(uint32_t)
define_benchmark_col_find_first(uint64_t)

#define define_benchmark_col_find_last(T) \
void benchmark_col_find_last_##T(mmfile_t mf) \
{ \
    uint64_t tstart, tend; \
    uint64_t first = 0; \
    uint64_t last = (TEST_DATA_ITEMS - 1); \
    uint64_t found; \
    int i; \
    const T *src = get_src_offset_##T(mf.src, (TEST_DATA_ITEMS * (sizeof(T) - 1))); \
    int size = 10000; \
    tstart = get_time(); \
    for (i=0 ; i < size; i++) \
    { \
        first = 0; \
        last = (TEST_DATA_ITEMS - 1); \
        found = col_find_last_##T(src, &first, &last, test_col_data_##T[4].search); \
    } \
    tend = get_time(); \
    fprintf(stdout, " * %s : %lu ns/op (%" PRIx64 ")\n", __func__, (tend - tstart)/(size*4), found); \
}

define_benchmark_col_find_last(uint8_t)
define_benchmark_col_find_last(uint16_t)
define_benchmark_col_find_last(uint32_t)
define_benchmark_col_find_last(uint64_t)

#define define_benchmark_col_find_first_sub(T) \
void benchmark_col_find_first_sub_##T(mmfile_t mf) \
{ \
    uint64_t tstart, tend; \
    uint64_t first = 0; \
    uint64_t last = (TEST_DATA_ITEMS - 1); \
    uint64_t found; \
    uint8_t nbytes = (uint8_t)sizeof(T); \
    uint8_t bitstart = ((nbytes >> 2) * 8); \
    uint8_t bitend = ((8 * nbytes) - 1 - bitstart); \
    int i; \
    const T *src = get_src_offset_##T(mf.src, (TEST_DATA_ITEMS * (sizeof(T) - 1))); \
    int size = 10000; \
    tstart = get_time(); \
    for (i=0 ; i < size; i++) \
    { \
        first = 0; \
        last = (TEST_DATA_ITEMS - 1); \
        found = col_find_first_sub_##T(src, bitstart, bitend, &first, &last, test_col_data_##T[4].search); \
    } \
    tend = get_time(); \
    fprintf(stdout, " * %s : %lu ns/op (%" PRIx64 ")\n", __func__, (tend - tstart)/(size*4), found); \
}

define_benchmark_col_find_first_sub(uint8_t)
define_benchmark_col_find_first_sub(uint16_t)
define_benchmark_col_find_first_sub(uint32_t)
define_benchmark_col_find_first_sub(uint64_t)

#define define_benchmark_col_find_last_sub(T) \
void benchmark_col_find_last_sub_##T(mmfile_t mf) \
{ \
    uint64_t tstart, tend; \
    uint64_t first = 0; \
    uint64_t last = (TEST_DATA_ITEMS - 1); \
    uint64_t found; \
    uint8_t nbytes = (uint8_t)sizeof(T); \
    uint8_t bitstart = ((nbytes >> 2) * 8); \
    uint8_t bitend = ((8 * nbytes) - 1 - bitstart); \
    int i; \
    const T *src = get_src_offset_##T(mf.src, (TEST_DATA_ITEMS * (sizeof(T) - 1))); \
    int size = 10000; \
    tstart = get_time(); \
    for (i=0 ; i < size; i++) \
    { \
        first = 0; \
        last = (TEST_DATA_ITEMS - 1); \
        found = col_find_last_sub_##T(src, bitstart, bitend, &first, &last, test_col_data_##T[4].search); \
    } \
    tend = get_time(); \
    fprintf(stdout, " * %s : %lu ns/op (%" PRIx64 ")\n", __func__, (tend - tstart)/(size*4), found); \
}

define_benchmark_col_find_last_sub(uint8_t)
define_benchmark_col_find_last_sub(uint16_t)
define_benchmark_col_find_last_sub(uint32_t)
define_benchmark_col_find_last_sub(uint64_t)

int test_col_mmap_binfile_error(const char* file)
{
    mmfile_t mf = {0};
    mmap_binfile(file, &mf);
    if (mf.src != MAP_FAILED)
    {
        fprintf(stderr, "An mmap error was expected\n");
        return 1;
    }
    return 0;
}

int test_col_munmap_binfile_error()
{
    mmfile_t mf = {0};
    int e = munmap_binfile(mf);
    if (e == 0)
    {
        fprintf(stderr, "An mummap error was expected\n");
        return 1;
    }
    return 0;
}

int main()
{
    int errors = 0;

    errors += test_col_mmap_binfile_error("ERROR");
    errors += test_col_mmap_binfile_error("/dev/null");
    errors += test_col_munmap_binfile_error();

    char *file = "test_data_col.bin"; // file containing test data

    mmfile_t mf = {0};
    mmap_binfile(file, &mf);

    if (mf.fd < 0)
    {
        fprintf(stderr, "can't open %s for reading\n", file);
        return 1;
    }
    if (mf.size == 0)
    {
        fprintf(stderr, "fstat error! [%s]\n", strerror(errno));
        return 1;
    }
    if (mf.src == MAP_FAILED)
    {
        fprintf(stderr, "mmap error! [%s]\n", strerror(errno));
        return 1;
    }

    if (mf.size != 3765)
    {
        fprintf(stderr, "Expecting 3765 bytes, got instead: %" PRIu64 "\n", mf.size);
        return 1;
    }

    errors += test_col_find_first_uint8_t(mf);
    errors += test_col_find_last_uint8_t(mf);
    errors += test_col_find_first_uint16_t(mf);
    errors += test_col_find_last_uint16_t(mf);
    errors += test_col_find_first_uint32_t(mf);
    errors += test_col_find_last_uint32_t(mf);
    errors += test_col_find_first_uint64_t(mf);
    errors += test_col_find_last_uint64_t(mf);

    benchmark_col_find_first_uint8_t(mf);
    benchmark_col_find_last_uint8_t(mf);
    benchmark_col_find_first_uint16_t(mf);
    benchmark_col_find_last_uint16_t(mf);
    benchmark_col_find_first_uint32_t(mf);
    benchmark_col_find_last_uint32_t(mf);
    benchmark_col_find_first_uint64_t(mf);
    benchmark_col_find_last_uint64_t(mf);

    benchmark_col_find_first_sub_uint8_t(mf);
    benchmark_col_find_last_sub_uint8_t(mf);
    benchmark_col_find_first_sub_uint16_t(mf);
    benchmark_col_find_last_sub_uint16_t(mf);
    benchmark_col_find_first_sub_uint32_t(mf);
    benchmark_col_find_last_sub_uint32_t(mf);
    benchmark_col_find_first_sub_uint64_t(mf);
    benchmark_col_find_last_sub_uint64_t(mf);

    int e = munmap_binfile(mf);
    if (e != 0)
    {
        fprintf(stderr, "Got %d error while unmapping the file\n", e);
        return 1;
    }

    return errors;
}
