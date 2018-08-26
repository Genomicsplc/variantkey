"""Tests for binsearch module."""


import variantkey as bs
import os
from unittest import TestCase

nitems = 251

testDataCol8 = [
    (0, 250, 0x00,   0,   0,   1,   1,   2,   1),
    (1, 250, 0x00,   1,   1,   0,   1,   2,   1),
    (0, 250, 0x01,   2,   2,   1,   2,   3,   2),
    (0, 250, 0x0f,  16,  16,  15,  16,  17,  16),
    (0, 250, 0x10,  17,  17,  16,  17,  18,  17),
    (0, 250, 0x1f,  32,  32,  31,  32,  33,  32),
    (0, 250, 0x20,  33,  33,  32,  33,  34,  33),
    (0, 250, 0x2f,  48,  48,  47,  48,  49,  48),
    (0, 250, 0x30,  49,  49,  48,  49,  50,  49),
    (0, 250, 0x3f,  64,  64,  63,  64,  65,  64),
    (0, 250, 0x40,  65,  65,  64,  65,  66,  65),
    (0, 250, 0x4f,  80,  80,  79,  80,  81,  80),
    (0, 250, 0x50,  81,  81,  80,  81,  82,  81),
    (0, 250, 0x5f,  96,  96,  95,  96,  97,  96),
    (0, 250, 0x60,  97,  97,  96,  97,  98,  97),
    (0, 250, 0x6f, 112, 112, 111, 112, 113, 112),
    (0, 250, 0x70, 113, 113, 112, 113, 114, 113),
    (0, 250, 0x7f, 128, 128, 127, 128, 129, 128),
    (0, 250, 0x80, 129, 129, 128, 129, 130, 129),
    (0, 250, 0x8f, 144, 144, 143, 144, 145, 144),
    (0, 250, 0x90, 145, 145, 144, 145, 146, 145),
    (0, 250, 0x9f, 160, 160, 159, 160, 161, 160),
    (0, 250, 0xa0, 161, 161, 160, 161, 162, 161),
    (0, 250, 0xaf, 176, 176, 175, 176, 177, 176),
    (0, 250, 0xb0, 177, 177, 176, 177, 178, 177),
    (0, 250, 0xbf, 192, 192, 191, 192, 193, 192),
    (0, 250, 0xc0, 193, 193, 192, 193, 194, 193),
    (0, 250, 0xcf, 208, 208, 207, 208, 209, 208),
    (0, 250, 0xd0, 209, 209, 208, 209, 210, 209),
    (0, 250, 0xdf, 224, 224, 223, 224, 225, 224),
    (0, 250, 0xe0, 225, 225, 224, 225, 226, 225),
    (0, 250, 0xef, 240, 240, 239, 240, 241, 240),
    (0, 250, 0xf0, 241, 241, 240, 241, 242, 241),
    (0, 250, 0xf8, 249, 249, 248, 249, 250, 249),
    (0, 250, 0xff, 250, 250, 249, 250, 251, 250),
    (0, 250, 0xf9, 251, 250, 249, 251, 250, 249),
    (0,  50, 0x70,  51,  51,  50,  51,  51,  50),
    (150, 250, 0x70, 251, 150, 149, 251, 150, 149),
]

testDataColSub8 = [
    (0, 250, 0x00,   0,   0,   1,   1,   2,   1),
    (1, 250, 0x00,   1,   1,   0,   1,   2,   1),
    (0, 250, 0x01,   2,   2,   1,   2,   3,   2),
    (0, 250, 0x0f,  16,  16,  15,  16,  17,  16),
    (0, 250, 0x10,  17,  17,  16,  17,  18,  17),
    (0, 250, 0x1f,  32,  32,  31,  32,  33,  32),
    (0, 250, 0x20,  33,  33,  32,  33,  34,  33),
    (0, 250, 0x2f,  48,  48,  47,  48,  49,  48),
    (0, 250, 0x30,  49,  49,  48,  49,  50,  49),
    (0, 250, 0x3f,  64,  64,  63,  64,  65,  64),
    (0, 250, 0x40,  65,  65,  64,  65,  66,  65),
    (0, 250, 0x4f,  80,  80,  79,  80,  81,  80),
    (0, 250, 0x50,  81,  81,  80,  81,  82,  81),
    (0, 250, 0x5f,  96,  96,  95,  96,  97,  96),
    (0, 250, 0x60,  97,  97,  96,  97,  98,  97),
    (0, 250, 0x6f, 112, 112, 111, 112, 113, 112),
    (0, 250, 0x70, 113, 113, 112, 113, 114, 113),
    (0, 250, 0x7f, 128, 128, 127, 128, 129, 128),
    (0, 250, 0x80, 129, 129, 128, 129, 130, 129),
    (0, 250, 0x8f, 144, 144, 143, 144, 145, 144),
    (0, 250, 0x90, 145, 145, 144, 145, 146, 145),
    (0, 250, 0x9f, 160, 160, 159, 160, 161, 160),
    (0, 250, 0xa0, 161, 161, 160, 161, 162, 161),
    (0, 250, 0xaf, 176, 176, 175, 176, 177, 176),
    (0, 250, 0xb0, 177, 177, 176, 177, 178, 177),
    (0, 250, 0xbf, 192, 192, 191, 192, 193, 192),
    (0, 250, 0xc0, 193, 193, 192, 193, 194, 193),
    (0, 250, 0xcf, 208, 208, 207, 208, 209, 208),
    (0, 250, 0xd0, 209, 209, 208, 209, 210, 209),
    (0, 250, 0xdf, 224, 224, 223, 224, 225, 224),
    (0, 250, 0xe0, 225, 225, 224, 225, 226, 225),
    (0, 250, 0xef, 240, 240, 239, 240, 241, 240),
    (0, 250, 0xf0, 241, 241, 240, 241, 242, 241),
    (0, 250, 0xf8, 249, 249, 248, 249, 250, 249),
    (0, 250, 0xff, 250, 250, 249, 250, 251, 250),
    (0, 250, 0xf9, 251, 250, 249, 251, 250, 249),
    (0,  50, 0x70,  51,  51,  50,  51,  51,  50),
    (150, 250, 0x70, 251, 150, 149, 251, 150, 149),
]

testDataCol16 = [
    (0, 250, 0x0000,   0,   0,   1,   0,   1,   0),
    (1, 250, 0x0001,   1,   1,   0,   1,   2,   1),
    (0, 250, 0x0102,   2,   2,   1,   2,   3,   2),
    (0, 250, 0x0f10,  16,  16,  15,  16,  17,  16),
    (0, 250, 0x1011,  17,  17,  16,  17,  18,  17),
    (0, 250, 0x1f20,  32,  32,  31,  32,  33,  32),
    (0, 250, 0x2021,  33,  33,  32,  33,  34,  33),
    (0, 250, 0x2f30,  48,  48,  47,  48,  49,  48),
    (0, 250, 0x3031,  49,  49,  48,  49,  50,  49),
    (0, 250, 0x3f40,  64,  64,  63,  64,  65,  64),
    (0, 250, 0x4041,  65,  65,  64,  65,  66,  65),
    (0, 250, 0x4f50,  80,  80,  79,  80,  81,  80),
    (0, 250, 0x5051,  81,  81,  80,  81,  82,  81),
    (0, 250, 0x5f60,  96,  96,  95,  96,  97,  96),
    (0, 250, 0x6061,  97,  97,  96,  97,  98,  97),
    (0, 250, 0x6f70, 112, 112, 111, 112, 113, 112),
    (0, 250, 0x7071, 113, 113, 112, 113, 114, 113),
    (0, 250, 0x7f80, 128, 128, 127, 128, 129, 128),
    (0, 250, 0x8081, 129, 129, 128, 129, 130, 129),
    (0, 250, 0x8f90, 144, 144, 143, 144, 145, 144),
    (0, 250, 0x9091, 145, 145, 144, 145, 146, 145),
    (0, 250, 0x9fa0, 160, 160, 159, 160, 161, 160),
    (0, 250, 0xa0a1, 161, 161, 160, 161, 162, 161),
    (0, 250, 0xafb0, 176, 176, 175, 176, 177, 176),
    (0, 250, 0xb0b1, 177, 177, 176, 177, 178, 177),
    (0, 250, 0xbfc0, 192, 192, 191, 192, 193, 192),
    (0, 250, 0xc0c1, 193, 193, 192, 193, 194, 193),
    (0, 250, 0xcfd0, 208, 208, 207, 208, 209, 208),
    (0, 250, 0xd0d1, 209, 209, 208, 209, 210, 209),
    (0, 250, 0xdfe0, 224, 224, 223, 224, 225, 224),
    (0, 250, 0xe0e1, 225, 225, 224, 225, 226, 225),
    (0, 250, 0xeff0, 240, 240, 239, 240, 241, 240),
    (0, 250, 0xf0f1, 241, 241, 240, 241, 242, 241),
    (0, 250, 0xf8f9, 249, 249, 248, 249, 250, 249),
    (0, 250, 0xffff, 250, 250, 249, 250, 251, 250),
    (0, 250, 0xf9f9, 251, 250, 249, 251, 250, 249),
    (0,  50, 0x7071,  51,  51,  50,  51,  51,  50),
    (150, 250, 0x7071, 251, 150, 149, 251, 150, 149),
]

testDataColSub16 = [
    (0, 250, 0x0000,   0,   0,   1,   0,   1,   0),
    (1, 250, 0x0001,   1,   1,   0,   1,   2,   1),
    (0, 250, 0x0102,   2,   2,   1,   2,   3,   2),
    (0, 250, 0x0f10,  16,  16,  15,  16,  17,  16),
    (0, 250, 0x1011,  17,  17,  16,  17,  18,  17),
    (0, 250, 0x1f20,  32,  32,  31,  32,  33,  32),
    (0, 250, 0x2021,  33,  33,  32,  33,  34,  33),
    (0, 250, 0x2f30,  48,  48,  47,  48,  49,  48),
    (0, 250, 0x3031,  49,  49,  48,  49,  50,  49),
    (0, 250, 0x3f40,  64,  64,  63,  64,  65,  64),
    (0, 250, 0x4041,  65,  65,  64,  65,  66,  65),
    (0, 250, 0x4f50,  80,  80,  79,  80,  81,  80),
    (0, 250, 0x5051,  81,  81,  80,  81,  82,  81),
    (0, 250, 0x5f60,  96,  96,  95,  96,  97,  96),
    (0, 250, 0x6061,  97,  97,  96,  97,  98,  97),
    (0, 250, 0x6f70, 112, 112, 111, 112, 113, 112),
    (0, 250, 0x7071, 113, 113, 112, 113, 114, 113),
    (0, 250, 0x7f80, 128, 128, 127, 128, 129, 128),
    (0, 250, 0x8081, 129, 129, 128, 129, 130, 129),
    (0, 250, 0x8f90, 144, 144, 143, 144, 145, 144),
    (0, 250, 0x9091, 145, 145, 144, 145, 146, 145),
    (0, 250, 0x9fa0, 160, 160, 159, 160, 161, 160),
    (0, 250, 0xa0a1, 161, 161, 160, 161, 162, 161),
    (0, 250, 0xafb0, 176, 176, 175, 176, 177, 176),
    (0, 250, 0xb0b1, 177, 177, 176, 177, 178, 177),
    (0, 250, 0xbfc0, 192, 192, 191, 192, 193, 192),
    (0, 250, 0xc0c1, 193, 193, 192, 193, 194, 193),
    (0, 250, 0xcfd0, 208, 208, 207, 208, 209, 208),
    (0, 250, 0xd0d1, 209, 209, 208, 209, 210, 209),
    (0, 250, 0xdfe0, 224, 224, 223, 224, 225, 224),
    (0, 250, 0xe0e1, 225, 225, 224, 225, 226, 225),
    (0, 250, 0xeff0, 240, 240, 239, 240, 241, 240),
    (0, 250, 0xf0f1, 241, 241, 240, 241, 242, 241),
    (0, 250, 0xf8f9, 249, 249, 248, 249, 250, 249),
    (0, 250, 0xffff, 250, 250, 249, 250, 251, 250),
    (0, 250, 0xf9f9, 251, 250, 249, 251, 250, 249),
    (0,  50, 0x7071,  51,  51,  50,  51,  51,  50),
    (150, 250, 0x7071, 251, 150, 149, 251, 150, 149),
]

testDataCol32 = [
    (0, 250, 0x00000000,   0,   0,   1,   0,   1,   0),
    (1, 250, 0x00010203,   1,   1,   0,   1,   2,   1),
    (0, 250, 0x01020304,   2,   2,   1,   2,   3,   2),
    (0, 250, 0x0f101112,  16,  16,  15,  16,  17,  16),
    (0, 250, 0x10111213,  17,  17,  16,  17,  18,  17),
    (0, 250, 0x1f202122,  32,  32,  31,  32,  33,  32),
    (0, 250, 0x20212223,  33,  33,  32,  33,  34,  33),
    (0, 250, 0x2f303132,  48,  48,  47,  48,  49,  48),
    (0, 250, 0x30313233,  49,  49,  48,  49,  50,  49),
    (0, 250, 0x3f404142,  64,  64,  63,  64,  65,  64),
    (0, 250, 0x40414243,  65,  65,  64,  65,  66,  65),
    (0, 250, 0x4f505152,  80,  80,  79,  80,  81,  80),
    (0, 250, 0x50515253,  81,  81,  80,  81,  82,  81),
    (0, 250, 0x5f606162,  96,  96,  95,  96,  97,  96),
    (0, 250, 0x60616263,  97,  97,  96,  97,  98,  97),
    (0, 250, 0x6f707172, 112, 112, 111, 112, 113, 112),
    (0, 250, 0x70717273, 113, 113, 112, 113, 114, 113),
    (0, 250, 0x7f808182, 128, 128, 127, 128, 129, 128),
    (0, 250, 0x80818283, 129, 129, 128, 129, 130, 129),
    (0, 250, 0x8f909192, 144, 144, 143, 144, 145, 144),
    (0, 250, 0x90919293, 145, 145, 144, 145, 146, 145),
    (0, 250, 0x9fa0a1a2, 160, 160, 159, 160, 161, 160),
    (0, 250, 0xa0a1a2a3, 161, 161, 160, 161, 162, 161),
    (0, 250, 0xafb0b1b2, 176, 176, 175, 176, 177, 176),
    (0, 250, 0xb0b1b2b3, 177, 177, 176, 177, 178, 177),
    (0, 250, 0xbfc0c1c2, 192, 192, 191, 192, 193, 192),
    (0, 250, 0xc0c1c2c3, 193, 193, 192, 193, 194, 193),
    (0, 250, 0xcfd0d1d2, 208, 208, 207, 208, 209, 208),
    (0, 250, 0xd0d1d2d3, 209, 209, 208, 209, 210, 209),
    (0, 250, 0xdfe0e1e2, 224, 224, 223, 224, 225, 224),
    (0, 250, 0xe0e1e2e3, 225, 225, 224, 225, 226, 225),
    (0, 250, 0xeff0f1f2, 240, 240, 239, 240, 241, 240),
    (0, 250, 0xf0f1f2f3, 241, 241, 240, 241, 242, 241),
    (0, 250, 0xf8f9fafb, 249, 249, 248, 249, 250, 249),
    (0, 250, 0xffffffff, 250, 250, 249, 250, 251, 250),
    (0, 250, 0xf9f9fafb, 251, 250, 249, 251, 250, 249),
    (0,  50, 0x70717273,  51,  51,  50,  51,  51,  50),
    (150, 250, 0x70717273, 251, 150, 149, 251, 150, 149),
]

testDataColSub32 = [
    (0, 250, 0x00000000,   0,   0,   1,   0,   1,   0),
    (1, 250, 0x00000102,   1,   1,   0,   1,   2,   1),
    (0, 250, 0x00000203,   2,   2,   1,   2,   3,   2),
    (0, 250, 0x00001011,  16,  16,  15,  16,  17,  16),
    (0, 250, 0x00001112,  17,  17,  16,  17,  18,  17),
    (0, 250, 0x00002021,  32,  32,  31,  32,  33,  32),
    (0, 250, 0x00002122,  33,  33,  32,  33,  34,  33),
    (0, 250, 0x00003031,  48,  48,  47,  48,  49,  48),
    (0, 250, 0x00003132,  49,  49,  48,  49,  50,  49),
    (0, 250, 0x00004041,  64,  64,  63,  64,  65,  64),
    (0, 250, 0x00004142,  65,  65,  64,  65,  66,  65),
    (0, 250, 0x00005051,  80,  80,  79,  80,  81,  80),
    (0, 250, 0x00005152,  81,  81,  80,  81,  82,  81),
    (0, 250, 0x00006061,  96,  96,  95,  96,  97,  96),
    (0, 250, 0x00006162,  97,  97,  96,  97,  98,  97),
    (0, 250, 0x00007071, 112, 112, 111, 112, 113, 112),
    (0, 250, 0x00007172, 113, 113, 112, 113, 114, 113),
    (0, 250, 0x00008081, 128, 128, 127, 128, 129, 128),
    (0, 250, 0x00008182, 129, 129, 128, 129, 130, 129),
    (0, 250, 0x00009091, 144, 144, 143, 144, 145, 144),
    (0, 250, 0x00009192, 145, 145, 144, 145, 146, 145),
    (0, 250, 0x0000a0a1, 160, 160, 159, 160, 161, 160),
    (0, 250, 0x0000a1a2, 161, 161, 160, 161, 162, 161),
    (0, 250, 0x0000b0b1, 176, 176, 175, 176, 177, 176),
    (0, 250, 0x0000b1b2, 177, 177, 176, 177, 178, 177),
    (0, 250, 0x0000c0c1, 192, 192, 191, 192, 193, 192),
    (0, 250, 0x0000c1c2, 193, 193, 192, 193, 194, 193),
    (0, 250, 0x0000d0d1, 208, 208, 207, 208, 209, 208),
    (0, 250, 0x0000d1d2, 209, 209, 208, 209, 210, 209),
    (0, 250, 0x0000e0e1, 224, 224, 223, 224, 225, 224),
    (0, 250, 0x0000e1e2, 225, 225, 224, 225, 226, 225),
    (0, 250, 0x0000f0f1, 240, 240, 239, 240, 241, 240),
    (0, 250, 0x0000f1f2, 241, 241, 240, 241, 242, 241),
    (0, 250, 0x0000f9fa, 249, 249, 248, 249, 250, 249),
    (0, 250, 0x0000ffff, 250, 250, 249, 250, 251, 250),
    (0, 250, 0x0000f9fa, 249, 249, 248, 249, 250, 249),
    (0,  50, 0x00007172,  51,  51,  50,  51,  51,  50),
    (150, 250, 0x00007172, 251, 150, 149, 251, 150, 149),
]

testDataCol64 = [
    (0, 250, 0x0000000000000000,   0,   0,   1,   0,   1,   0),
    (1, 250, 0x0001020304050607,   1,   1,   0,   1,   2,   1),
    (0, 250, 0x0102030405060708,   2,   2,   1,   2,   3,   2),
    (0, 250, 0x0f10111213141516,  16,  16,  15,  16,  17,  16),
    (0, 250, 0x1011121314151617,  17,  17,  16,  17,  18,  17),
    (0, 250, 0x1f20212223242526,  32,  32,  31,  32,  33,  32),
    (0, 250, 0x2021222324252627,  33,  33,  32,  33,  34,  33),
    (0, 250, 0x2f30313233343536,  48,  48,  47,  48,  49,  48),
    (0, 250, 0x3031323334353637,  49,  49,  48,  49,  50,  49),
    (0, 250, 0x3f40414243444546,  64,  64,  63,  64,  65,  64),
    (0, 250, 0x4041424344454647,  65,  65,  64,  65,  66,  65),
    (0, 250, 0x4f50515253545556,  80,  80,  79,  80,  81,  80),
    (0, 250, 0x5051525354555657,  81,  81,  80,  81,  82,  81),
    (0, 250, 0x5f60616263646566,  96,  96,  95,  96,  97,  96),
    (0, 250, 0x6061626364656667,  97,  97,  96,  97,  98,  97),
    (0, 250, 0x6f70717273747576, 112, 112, 111, 112, 113, 112),
    (0, 250, 0x7071727374757677, 113, 113, 112, 113, 114, 113),
    (0, 250, 0x7f80818283848586, 128, 128, 127, 128, 129, 128),
    (0, 250, 0x8081828384858687, 129, 129, 128, 129, 130, 129),
    (0, 250, 0x8f90919293949596, 144, 144, 143, 144, 145, 144),
    (0, 250, 0x9091929394959697, 145, 145, 144, 145, 146, 145),
    (0, 250, 0x9fa0a1a2a3a4a5a6, 160, 160, 159, 160, 161, 160),
    (0, 250, 0xa0a1a2a3a4a5a6a7, 161, 161, 160, 161, 162, 161),
    (0, 250, 0xafb0b1b2b3b4b5b6, 176, 176, 175, 176, 177, 176),
    (0, 250, 0xb0b1b2b3b4b5b6b7, 177, 177, 176, 177, 178, 177),
    (0, 250, 0xbfc0c1c2c3c4c5c6, 192, 192, 191, 192, 193, 192),
    (0, 250, 0xc0c1c2c3c4c5c6c7, 193, 193, 192, 193, 194, 193),
    (0, 250, 0xcfd0d1d2d3d4d5d6, 208, 208, 207, 208, 209, 208),
    (0, 250, 0xd0d1d2d3d4d5d6d7, 209, 209, 208, 209, 210, 209),
    (0, 250, 0xdfe0e1e2e3e4e5e6, 224, 224, 223, 224, 225, 224),
    (0, 250, 0xe0e1e2e3e4e5e6e7, 225, 225, 224, 225, 226, 225),
    (0, 250, 0xeff0f1f2f3f4f5f6, 240, 240, 239, 240, 241, 240),
    (0, 250, 0xf0f1f2f3f4f5f6f7, 241, 241, 240, 241, 242, 241),
    (0, 250, 0xf8f9fafbfcfdfeff, 249, 249, 248, 249, 250, 249),
    (0, 250, 0xffffffffffffffff, 250, 250, 249, 250, 251, 250),
    (0, 250, 0xf9f9fafbfcfdfeff, 251, 250, 249, 251, 250, 249),
    (0,  50, 0x7071727374757677,  51,  51,  50,  51,  51,  50),
    (150, 250, 0x7071727374757677, 251, 150, 149, 251, 150, 149),
]

testDataColSub64 = [
    (0, 250, 0x0000000000000000,   0,   0,   1,   0,   1,   0),
    (1, 250, 0x0000000002030405,   1,   1,   0,   1,   2,   1),
    (0, 250, 0x0000000003040506,   2,   2,   1,   2,   3,   2),
    (0, 250, 0x0000000011121314,  16,  16,  15,  16,  17,  16),
    (0, 250, 0x0000000012131415,  17,  17,  16,  17,  18,  17),
    (0, 250, 0x0000000021222324,  32,  32,  31,  32,  33,  32),
    (0, 250, 0x0000000022232425,  33,  33,  32,  33,  34,  33),
    (0, 250, 0x0000000031323334,  48,  48,  47,  48,  49,  48),
    (0, 250, 0x0000000032333435,  49,  49,  48,  49,  50,  49),
    (0, 250, 0x0000000041424344,  64,  64,  63,  64,  65,  64),
    (0, 250, 0x0000000042434445,  65,  65,  64,  65,  66,  65),
    (0, 250, 0x0000000051525354,  80,  80,  79,  80,  81,  80),
    (0, 250, 0x0000000052535455,  81,  81,  80,  81,  82,  81),
    (0, 250, 0x0000000061626364,  96,  96,  95,  96,  97,  96),
    (0, 250, 0x0000000062636465,  97,  97,  96,  97,  98,  97),
    (0, 250, 0x0000000071727374, 112, 112, 111, 112, 113, 112),
    (0, 250, 0x0000000072737475, 113, 113, 112, 113, 114, 113),
    (0, 250, 0x0000000081828384, 128, 128, 127, 128, 129, 128),
    (0, 250, 0x0000000082838485, 129, 129, 128, 129, 130, 129),
    (0, 250, 0x0000000091929394, 144, 144, 143, 144, 145, 144),
    (0, 250, 0x0000000092939495, 145, 145, 144, 145, 146, 145),
    (0, 250, 0x00000000a1a2a3a4, 160, 160, 159, 160, 161, 160),
    (0, 250, 0x00000000a2a3a4a5, 161, 161, 160, 161, 162, 161),
    (0, 250, 0x00000000b1b2b3b4, 176, 176, 175, 176, 177, 176),
    (0, 250, 0x00000000b2b3b4b5, 177, 177, 176, 177, 178, 177),
    (0, 250, 0x00000000c1c2c3c4, 192, 192, 191, 192, 193, 192),
    (0, 250, 0x00000000c2c3c4c5, 193, 193, 192, 193, 194, 193),
    (0, 250, 0x00000000d1d2d3d4, 208, 208, 207, 208, 209, 208),
    (0, 250, 0x00000000d2d3d4d5, 209, 209, 208, 209, 210, 209),
    (0, 250, 0x00000000e1e2e3e4, 224, 224, 223, 224, 225, 224),
    (0, 250, 0x00000000e2e3e4e5, 225, 225, 224, 225, 226, 225),
    (0, 250, 0x00000000f1f2f3f4, 240, 240, 239, 240, 241, 240),
    (0, 250, 0x00000000f2f3f4f5, 241, 241, 240, 241, 242, 241),
    (0, 250, 0x00000000fafbfcfd, 249, 249, 248, 249, 250, 249),
    (0, 250, 0x00000000ffffffff, 250, 250, 249, 250, 251, 250),
    (0, 250, 0x00000000fafbfcfd, 249, 249, 248, 249, 250, 249),
    (0,  50, 0x0000000072737475,  51,  51,  50,  51,  51,  50),
    (150, 250, 0x0000000072737475, 251, 150, 149, 251, 150, 149),
]


class TestFunctions(TestCase):

    @classmethod
    def setUpClass(cls):
        global src, fd, size, last
        inputfile = os.path.realpath(
            os.path.dirname(
                os.path.realpath(__file__)) +
            "/../../c/test/data/test_data_col.bin")
        src, fd, size, last = bs.mmap_binfile(inputfile)
        if fd < 0 or size != 3765:
            assert False, "Unable to open the file"

    @classmethod
    def tearDownClass(cls):
        global src, fd, size
        h = bs.munmap_binfile(src, fd, size)
        if h != 0:
            assert False, "Error while closing the memory-mapped file"

    def test_col_find_first_uint8(self):
        for first, last, search, fF, fFF, fFL, fL, fLF, fLL in testDataCol8:
            rp, rf, rl = bs.col_find_first_uint8(src, 0, first, last, search)
            self.assertEqual(rp, fF)
            self.assertEqual(rf, fFF)
            self.assertEqual(rl, fFL)
            numitems = fL - fF + 1
            if (rp <= last) and (numitems > 0):
                pos = rp
                ret = True
                counter = 0
                while ret:
                    ret, pos = bs.col_has_next_uint8(src, 0, pos, last, search)
                    counter = counter + 1
                self.assertEqual(counter, numitems)

    def test_col_find_first_uint16(self):
        for first, last, search, fF, fFF, fFL, fL, fLF, fLL in testDataCol16:
            rp, rf, rl = bs.col_find_first_uint16(src, nitems, first, last, search)
            self.assertEqual(rp, fF)
            self.assertEqual(rf, fFF)
            self.assertEqual(rl, fFL)
            numitems = fL - fF + 1
            if (rp <= last) and (numitems > 0):
                pos = rp
                ret = True
                counter = 0
                while ret:
                    ret, pos = bs.col_has_next_uint16(src, nitems, pos, last, search)
                    counter = counter + 1
                self.assertEqual(counter, numitems)

    def test_col_find_first_uint32(self):
        for first, last, search, fF, fFF, fFL, fL, fLF, fLL in testDataCol32:
            rp, rf, rl = bs.col_find_first_uint32(src, (nitems * 3), first, last, search)
            self.assertEqual(rp, fF)
            self.assertEqual(rf, fFF)
            self.assertEqual(rl, fFL)
            numitems = fL - fF + 1
            if (rp <= last) and (numitems > 0):
                pos = rp
                ret = True
                counter = 0
                while ret:
                    ret, pos = bs.col_has_next_uint32(src, (nitems * 3), pos, last, search)
                    counter = counter + 1
                self.assertEqual(counter, numitems)

    def test_col_find_first_uint64(self):
        for first, last, search, fF, fFF, fFL, fL, fLF, fLL in testDataCol64:
            rp, rf, rl = bs.col_find_first_uint64(src, (nitems * 7), first, last, search)
            self.assertEqual(rp, fF)
            self.assertEqual(rf, fFF)
            self.assertEqual(rl, fFL)
            numitems = fL - fF + 1
            if (rp <= last) and (numitems > 0):
                pos = rp
                ret = True
                counter = 0
                while ret:
                    ret, pos = bs.col_has_next_uint64(src, (nitems * 7), pos, last, search)
                    counter = counter + 1
                self.assertEqual(counter, numitems)

    def test_col_find_last_uint8(self):
        for first, last, search, fF, fFF, fFL, fL, fLF, fLL in testDataCol8:
            rp, rf, rl = bs.col_find_last_uint8(src, 0, first, last, search)
            self.assertEqual(rp, fL)
            self.assertEqual(rf, fLF)
            self.assertEqual(rl, fLL)
            numitems = fL - fF + 1
            if (rp <= last) and (numitems > 0):
                pos = rp
                ret = True
                counter = 0
                while ret:
                    ret, pos = bs.col_has_prev_uint8(src, 0, first, pos, search)
                    counter = counter + 1
                self.assertEqual(counter, numitems)

    def test_col_find_last_uint16(self):
        for first, last, search, fF, fFF, fFL, fL, fLF, fLL in testDataCol16:
            rp, rf, rl = bs.col_find_last_uint16(src, nitems, first, last, search)
            self.assertEqual(rp, fL)
            self.assertEqual(rf, fLF)
            self.assertEqual(rl, fLL)
            numitems = fL - fF + 1
            if (rp <= last) and (numitems > 0):
                pos = rp
                ret = True
                counter = 0
                while ret:
                    ret, pos = bs.col_has_prev_uint16(src, nitems, first, pos, search)
                    counter = counter + 1
                self.assertEqual(counter, numitems)

    def test_col_find_last_uint32(self):
        for first, last, search, fF, fFF, fFL, fL, fLF, fLL in testDataCol32:
            rp, rf, rl = bs.col_find_last_uint32(src, (nitems * 3), first, last, search)
            self.assertEqual(rp, fL)
            self.assertEqual(rf, fLF)
            self.assertEqual(rl, fLL)
            numitems = fL - fF + 1
            if (rp <= last) and (numitems > 0):
                pos = rp
                ret = True
                counter = 0
                while ret:
                    ret, pos = bs.col_has_prev_uint32(src, (nitems * 3), first, pos, search)
                    counter = counter + 1
                self.assertEqual(counter, numitems)

    def test_col_find_last_uint64(self):
        for first, last, search, fF, fFF, fFL, fL, fLF, fLL in testDataCol64:
            rp, rf, rl = bs.col_find_last_uint64(src, (nitems * 7), first, last, search)
            self.assertEqual(rp, fL)
            self.assertEqual(rf, fLF)
            self.assertEqual(rl, fLL)
            numitems = fL - fF + 1
            if (rp <= last) and (numitems > 0):
                pos = rp
                ret = True
                counter = 0
                while ret:
                    ret, pos = bs.col_has_prev_uint64(src, (nitems * 7), first, pos, search)
                    counter = counter + 1
                self.assertEqual(counter, numitems)

    def test_col_find_first_sub_uint8(self):
        for first, last, search, fF, fFF, fFL, fL, fLF, fLL in testDataColSub8:
            rp, rf, rl = bs.col_find_first_sub_uint8(src, 0, 0, 7, first, last, search)
            self.assertEqual(rp, fF)
            self.assertEqual(rf, fFF)
            self.assertEqual(rl, fFL)
            numitems = fL - fF + 1
            if (rp <= last) and (numitems > 0):
                pos = rp
                ret = True
                counter = 0
                while ret:
                    ret, pos = bs.col_has_next_sub_uint8(src, 0, 0, 7, pos, last, search)
                    counter = counter + 1
                self.assertEqual(counter, numitems)

    def test_col_find_first_sub_uint16(self):
        for first, last, search, fF, fFF, fFL, fL, fLF, fLL in testDataColSub16:
            rp, rf, rl = bs.col_find_first_sub_uint16(src, nitems, 0, 15, first, last, search)
            self.assertEqual(rp, fF)
            self.assertEqual(rf, fFF)
            self.assertEqual(rl, fFL)
            numitems = fL - fF + 1
            if (rp <= last) and (numitems > 0):
                pos = rp
                ret = True
                counter = 0
                while ret:
                    ret, pos = bs.col_has_next_sub_uint16(src, nitems, 0, 15, pos, last, search)
                    counter = counter + 1
                self.assertEqual(counter, numitems)

    def test_col_find_first_sub_uint32(self):
        for first, last, search, fF, fFF, fFL, fL, fLF, fLL in testDataColSub32:
            rp, rf, rl = bs.col_find_first_sub_uint32(src, (nitems * 3), 8, 23, first, last, search)
            self.assertEqual(rp, fF)
            self.assertEqual(rf, fFF)
            self.assertEqual(rl, fFL)
            numitems = fL - fF + 1
            if (rp <= last) and (numitems > 0):
                pos = rp
                ret = True
                counter = 0
                while ret:
                    ret, pos = bs.col_has_next_sub_uint32(src, (nitems * 3), 8, 23, pos, last, search)
                    counter = counter + 1
                self.assertEqual(counter, numitems)

    def test_col_find_first_sub_uint64(self):
        for first, last, search, fF, fFF, fFL, fL, fLF, fLL in testDataColSub64:
            rp, rf, rl = bs.col_find_first_sub_uint64(src, (nitems * 7), 16, 47, first, last, search)
            self.assertEqual(rp, fF)
            self.assertEqual(rf, fFF)
            self.assertEqual(rl, fFL)
            numitems = fL - fF + 1
            if (rp <= last) and (numitems > 0):
                pos = rp
                ret = True
                counter = 0
                while ret:
                    ret, pos = bs.col_has_next_sub_uint64(src, (nitems * 7), 16, 47, pos, last, search)
                    counter = counter + 1
                self.assertEqual(counter, numitems)

    def test_col_find_last_sub_uint8(self):
        for first, last, search, fF, fFF, fFL, fL, fLF, fLL in testDataColSub8:
            rp, rf, rl = bs.col_find_last_sub_uint8(src, 0, 0, 7, first, last, search)
            self.assertEqual(rp, fL)
            self.assertEqual(rf, fLF)
            self.assertEqual(rl, fLL)
            numitems = fL - fF + 1
            if (rp <= last) and (numitems > 0):
                pos = rp
                ret = True
                counter = 0
                while ret:
                    ret, pos = bs.col_has_prev_sub_uint8(src, 0, 0, 7, first, pos, search)
                    counter = counter + 1
                self.assertEqual(counter, numitems)

    def test_col_find_last_sub_uint16(self):
        for first, last, search, fF, fFF, fFL, fL, fLF, fLL in testDataColSub16:
            rp, rf, rl = bs.col_find_last_sub_uint16(src, nitems, 0, 15, first, last, search)
            self.assertEqual(rp, fL)
            self.assertEqual(rf, fLF)
            self.assertEqual(rl, fLL)
            numitems = fL - fF + 1
            if (rp <= last) and (numitems > 0):
                pos = rp
                ret = True
                counter = 0
                while ret:
                    ret, pos = bs.col_has_prev_sub_uint16(src, nitems, 0, 15, first, pos, search)
                    counter = counter + 1
                self.assertEqual(counter, numitems)

    def test_col_find_last_sub_uint32(self):
        for first, last, search, fF, fFF, fFL, fL, fLF, fLL in testDataColSub32:
            rp, rf, rl = bs.col_find_last_sub_uint32(src, (nitems * 3), 8, 23, first, last, search)
            self.assertEqual(rp, fL)
            self.assertEqual(rf, fLF)
            self.assertEqual(rl, fLL)
            numitems = fL - fF + 1
            if (rp <= last) and (numitems > 0):
                pos = rp
                ret = True
                counter = 0
                while ret:
                    ret, pos = bs.col_has_prev_sub_uint32(src, (nitems * 3), 8, 23, first, pos, search)
                    counter = counter + 1
                self.assertEqual(counter, numitems)

    def test_col_find_last_sub_uint64(self):
        for first, last, search, fF, fFF, fFL, fL, fLF, fLL in testDataColSub64:
            rp, rf, rl = bs.col_find_last_sub_uint64(src, (nitems * 7), 16, 47, first, last, search)
            self.assertEqual(rp, fL)
            self.assertEqual(rf, fLF)
            self.assertEqual(rl, fLL)
            numitems = fL - fF + 1
            if (rp <= last) and (numitems > 0):
                pos = rp
                ret = True
                counter = 0
                while ret:
                    ret, pos = bs.col_has_prev_sub_uint64(src, (nitems * 7), 16, 47, first, pos, search)
                    counter = counter + 1
                self.assertEqual(counter, numitems)


class TestBenchmark(object):

    global setup

    def setup():
        global src, fd, size, last
        if fd >= 0:
            pass
        bs.munmap_binfile(src, fd, size)
        inputfile = os.path.realpath(
            os.path.dirname(
                os.path.realpath(__file__)) +
            "/../../c/test/data/test_data_col.bin")
        src, fd, size, last = bs.mmap_binfile(inputfile)
        if fd < 0 or size != 3765:
            assert False, "Unable to open the file"

    def test_col_find_first_uint8_benchmark(self, benchmark):
        benchmark.pedantic(
            bs.col_find_first_uint8,
            args=[src, 0, 0, 250, 0x2f],
            setup=setup,
            iterations=1,
            rounds=10000)

    def test_col_find_first_uint16_benchmark(self, benchmark):
        benchmark.pedantic(
            bs.col_find_first_uint16,
            args=[src, nitems, 0, 250, 0x2f30],
            setup=setup,
            iterations=1,
            rounds=10000)

    def test_col_find_first_uint32_benchmark(self, benchmark):
        benchmark.pedantic(
            bs.col_find_first_uint32,
            args=[src, (nitems * 3), 0, 250, 0x2f303132],
            setup=setup,
            iterations=1,
            rounds=10000)

    def test_col_find_first_uint64_benchmark(self, benchmark):
        benchmark.pedantic(
            bs.col_find_first_uint64,
            args=[src, (nitems * 7), 0, 250, 0x2f30313233343536],
            setup=setup,
            iterations=1,
            rounds=10000)

    def test_col_find_last_uint8_benchmark(self, benchmark):
        benchmark.pedantic(
            bs.col_find_last_uint8,
            args=[src, 0, 0, 250, 0x2f],
            setup=setup,
            iterations=1,
            rounds=10000)

    def test_col_find_last_uint16_benchmark(self, benchmark):
        benchmark.pedantic(
            bs.col_find_last_uint16,
            args=[src, nitems, 0, 250, 0x2f30],
            setup=setup,
            iterations=1,
            rounds=10000)

    def test_col_find_last_uint32_benchmark(self, benchmark):
        benchmark.pedantic(
            bs.col_find_last_uint32,
            args=[src, (nitems * 3), 0, 250, 0x2f303132],
            setup=setup,
            iterations=1,
            rounds=10000)

    def test_col_find_last_uint64_benchmark(self, benchmark):
        benchmark.pedantic(
            bs.col_find_last_uint64,
            args=[src, (nitems * 7), 0, 250, 0x2f30313233343536],
            setup=setup,
            iterations=1,
            rounds=10000)

    def test_col_find_first_sub_uint8_benchmark(self, benchmark):
        benchmark.pedantic(
            bs.col_find_first_sub_uint8,
            args=[src, 0, 0, 7, 0, 250, 0x2f],
            setup=setup,
            iterations=1,
            rounds=10000)

    def test_col_find_first_sub_uint16_benchmark(self, benchmark):
        benchmark.pedantic(
            bs.col_find_first_sub_uint16,
            args=[src, nitems, 0, 15, 0, 250, 0x2f30],
            setup=setup,
            iterations=1,
            rounds=10000)

    def test_col_find_first_sub_uint32_benchmark(self, benchmark):
        benchmark.pedantic(
            bs.col_find_first_sub_uint32,
            args=[src, (nitems * 3), 8, 23, 0, 250, 0x00003031],
            setup=setup,
            iterations=1,
            rounds=10000)

    def test_col_find_first_sub_uint64_benchmark(self, benchmark):
        benchmark.pedantic(
            bs.col_find_first_sub_uint64,
            args=[src, (nitems * 7), 16, 47, 0, 250, 0x0000000031323334],
            setup=setup,
            iterations=1,
            rounds=10000)

    def test_col_find_last_sub_uint8_benchmark(self, benchmark):
        benchmark.pedantic(
            bs.col_find_last_sub_uint8,
            args=[src, 0, 0, 7, 0, 250, 0x2f],
            setup=setup,
            iterations=1,
            rounds=10000)

    def test_col_find_last_sub_uint16_benchmark(self, benchmark):
        benchmark.pedantic(
            bs.col_find_last_sub_uint16,
            args=[src, nitems, 0, 15, 0, 250, 0x2f30],
            setup=setup,
            iterations=1,
            rounds=10000)

    def test_col_find_last_sub_uint32_benchmark(self, benchmark):
        benchmark.pedantic(
            bs.col_find_last_sub_uint32,
            args=[src, (nitems * 3), 8, 23, 0, 250, 0x00003031],
            setup=setup,
            iterations=1,
            rounds=10000)

    def test_col_find_last_sub_uint64_benchmark(self, benchmark):
        benchmark.pedantic(
            bs.col_find_last_sub_uint64,
            args=[src, (nitems * 7), 16, 47, 0, 250, 0x0000000031323334],
            setup=setup,
            iterations=1,
            rounds=10000)

    def test_col_tearDown(self):
        global src, fd, size
        h = bs.munmap_binfile(src, fd, size)
        fd = -1
        size = 0
        if h != 0:
            assert False, "Error while closing the memory-mapped file"
