"""Tests for variantkey esid module."""

# test_esid.py
# @category   Libraries
# @author     Nicola Asuni <nicola.asuni@genomicsplc.com>
# @copyright  2017-2018 GENOMICS plc
# @license    MIT (see LICENSE)
# @link       https://github.com/genomicsplc/variantkey

import pyvariantkey.variantkey as pyvk
import numpy as np
import os
from unittest import TestCase


# 0:size, 1:start, 2:esize, 3:esid, 4:hsid, 5:estr, 6:istr
esidTestData = np.array([
    (36,  0, 10, 0xa411493515597619, 0xb3a5fdb8808cb7dc, b"0123456789", b"0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ"),
    (36,  1, 10, 0xa4524d45565d8661, 0xb3a5fdb8808cb7dc, b"123456789A", b"0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ"),
    (36, 10, 10, 0xa8628e49669e8a6a, 0xd93e382010f46f32, b"ABCDEFGHIJ", b"0223456789ABCDEFGHIJKLMNOPQRSTUVWXYZ"),
    (36, 25, 10, 0xac31cb3d35db7e39, 0xbfc379f4a71cb3be, b"PQRSTUVWXY", b"0133456789ABCDEFGHIJKLMNOPQRSTUVWXYZ"),
    (36, 26, 10, 0xac72cf4d76df8e7a, 0xdba299d06b54215d, b"QRSTUVWXYZ", b"1123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ"),
    (36, 35,  1, 0x1ec0000000000000, 0x996dcdc8ef7c89c1, b"[",          b"0123456789ABCDEFGHIJKLMNOPQRSTUVWXY["),
    (6,  0,  6, 0x6411493515000000, 0xa81279395d9c891e, b"012345",     b"012345"),
    (6,  1,  5, 0x54524d4540000000, 0xa81279395d9c891e, b"12345",      b"012345"),
    (6,  3,  3, 0x34d4540000000000, 0xa81279395d9c891e, b"345",        b"012345"),
    (6,  5,  1, 0x1540000000000000, 0xa81279395d9c891e, b"5",          b"012345"),
    (10,  0, 10, 0xafc1083105187209, 0x838c148028a06998, b"_!\"#$%&'()", b" !\"#$%&'()"),
    (10,  0, 10, 0xa28b30d38f411493, 0x96e42cf7aefeccf0, b"*+,-./0123", b"*+,-./0123"),
    (10,  0, 10, 0xa51559761969b71d, 0xb391c7f288e446ee, b"456789:;<=", b"456789:;<="),
    (10,  0, 10, 0xa79f8218a39259a7, 0xcc0de20381405e0a, b">?@ABCDEFG", b">?@ABCDEFG"),
    (10,  0, 10, 0xaa29aabb2dbafc31, 0xbbfc630eacb6c552, b"HIJKLMNOPQ", b"HIJKLMNOPQ"),
    (10,  0, 10, 0xacb3d35db7e39ebb, 0x803eda0a1781a117, b"RSTUVWXYZ[", b"RSTUVWXYZ["),
    (10,  0, 10, 0xaf3dfbf8218a3925, 0x878ef2947ee3cc2b, b"\\]^_@ABCDE", b"\\]^_`abcde"),
    (10,  0, 10, 0xa9a7a29aabb2dbaf, 0xbaadaef07826969b, b"FGHIJKLMNO", b"fghijklmno"),
    (10,  0, 10, 0xac31cb3d35db7e39, 0xd3da4d5e28be3590, b"PQRSTUVWXY", b"pqrstuvwxy"),
    (6,  0,  6, 0x6ebbf3dfbf000000, 0xeadc752a50c5b850, b"Z[\\]^_",    b"z{|}~\t"),
    (15,  0, 10, 0xa4524d45565d8452, 0x9af6ee553ba41827, b"1234567812", b"123456781234567"),
    (14,  0, 10, 0xa4524d45565d8452, 0xc94e10e9fe153fd2, b"1234567812", b"12345678123456"),
    (13,  0, 10, 0xa4524d45565d8452, 0x9b56411c7abdefe6, b"1234567812", b"1234567812345"),
    (12,  0, 10, 0xa4524d45565d8452, 0xcbec35944ff1c863, b"1234567812", b"123456781234"),
    (11,  0, 10, 0xa4524d45565d8452, 0x929920298cf56b9b, b"1234567812", b"12345678123"),
    (10,  0, 10, 0xa4524d45565d8452, 0xbdf006d50a33ad90, b"1234567812", b"1234567812"),
    (9,  0,  9, 0x94524d45565d8440, 0xde094182e93557ae, b"123456781",  b"123456781"),
    (8,  0,  8, 0x84524d45565d8000, 0xccbc926a73ece95c, b"12345678",   b"12345678"),
    (7,  0,  7, 0x74524d45565c0000, 0xd18b960e2f99c279, b"1234567",    b"1234567"),
    (6,  0,  6, 0x64524d4556000000, 0x811c9c02fcc22096, b"123456",     b"123456"),
    (5,  0,  5, 0x54524d4540000000, 0xd863467dedd1cab1, b"12345",      b"12345"),
    (4,  0,  4, 0x44524d4000000000, 0xbef404ecb71e4cd9, b"1234",       b"1234"),
    (3,  0,  3, 0x34524c0000000000, 0xfad470644116fa54, b"123",        b"123"),
    (2,  0,  2, 0x2452000000000000, 0xd94ae1e7173e781d, b"12",         b"12"),
    (1,  0,  1, 0x1440000000000000, 0xfc73100baa96ad81, b"1",          b"1"),
    (0,  0,  0, 0x0000000000000000, 0x8000000000000000, b"",           b""),
])

# 0:size, 1:esize, 2:esid, 3:estr, 4:istr
esidNumTestData = np.array([
    (1,  1, 0x1680000000000000, b":",                     b":"),
    (1,  1, 0x1840000000000000, b"A",                     b"A"),
    (2,  2, 0x285a000000000000, b"A:",                    b"A:"),
    (2,  2, 0x2691000000000000, b":1",                    b":1"),
    (2,  2, 0x2862000000000000, b"AB",                    b"Ab"),
    (3,  3, 0x3862680000000000, b"AB:",                   b"Ab:"),
    (3,  3, 0x38628c0000000000, b"ABC",                   b"AbC"),
    (4,  4, 0x48628da000000000, b"ABC:",                  b"AbC:"),
    (4,  4, 0x48628e4000000000, b"ABCD",                  b"AbCd"),
    (5,  5, 0x58628e4680000000, b"ABCD:",                 b"AbCd:"),
    (5,  5, 0x58628e4940000000, b"ABCDE",                 b"AbCdE"),
    (6,  6, 0x68628e495a000000, b"ABCDE:",                b"AbCdE:"),
    (6,  6, 0x68628e4966000000, b"ABCDEF",                b"AbCdEf"),
    (7,  7, 0x78628e4966680000, b"ABCDEF:",               b"AbCdEf:"),
    (7,  7, 0x78628e49669c0000, b"ABCDEFG",               b"AbCdEfG"),
    (8,  8, 0x88628e49669da000, b"ABCDEFG:",              b"AbCdEfG:"),
    (8,  8, 0x88628e49669e8000, b"ABCDEFGH",              b"AbCdEfGh"),
    (9,  9, 0x98628e49669e8680, b"ABCDEFGH:",             b"AbCdEfGh:"),
    (5,  5, 0x58628da400000000, b"ABC:0",                 b"AbC:0"),
    (5,  5, 0x58628da440000000, b"ABC:1",                 b"AbC:1"),
    (12, 12, 0xd8628c0000bc614e, b"ABC:12345678",          b"AbC:12345678"),
    (13, 13, 0xd8628c0008bc614e, b"ABC:012345678",         b"AbC:012345678"),
    (14, 14, 0xd8628c0010bc614e, b"ABC:0012345678",        b"AbC:0012345678"),
    (15, 15, 0xd8628c0018bc614e, b"ABC:00012345678",       b"AbC:00012345678"),
    (16, 16, 0xd8628c0020bc614e, b"ABC:000012345678",      b"AbC:000012345678"),
    (17, 17, 0xd8628c0028bc614e, b"ABC:0000012345678",     b"AbC:0000012345678"),
    (18, 18, 0xd8628c0030bc614e, b"ABC:00000012345678",    b"AbC:00000012345678"),
    (19, 19, 0xd8628c0038bc614e, b"ABC:000000012345678",   b"AbC:000000012345678"),
    (20, 19, 0xd8628c0038bc614e, b"ABC:000000012345678",   b"AbC:0000000012345678"),
    (21, 19, 0xd8628c0038bc614e, b"ABC:000000012345678",   b"AbC:00000000012345678"),
    (22, 19, 0xd8628c0038bc614e, b"ABC:000000012345678",   b"AbC:000000000012345678"),
    (23, 19, 0xd8628c0038bc614e, b"ABC:000000012345678",   b"AbC:0000000000012345678"),
    (7,  7, 0x78628e495a400000, b"ABCDE:0",               b"AbCdE:0"),
    (7,  7, 0x78628e495a440000, b"ABCDE:1",               b"AbCdE:1"),
    (14, 14, 0xf8628e4940bc614e, b"ABCDE:12345678",        b"AbCdE:12345678"),
    (15, 15, 0xf8628e4948bc614e, b"ABCDE:012345678",       b"AbCdE:012345678"),
    (16, 16, 0xf8628e4950bc614e, b"ABCDE:0012345678",      b"AbCdE:0012345678"),
    (17, 17, 0xf8628e4958bc614e, b"ABCDE:00012345678",     b"AbCdE:00012345678"),
    (18, 18, 0xf8628e4960bc614e, b"ABCDE:000012345678",    b"AbCdE:000012345678"),
    (19, 19, 0xf8628e4968bc614e, b"ABCDE:0000012345678",   b"AbCdE:0000012345678"),
    (20, 20, 0xf8628e4970bc614e, b"ABCDE:00000012345678",  b"AbCdE:00000012345678"),
    (21, 21, 0xf8628e4978bc614e, b"ABCDE:000000012345678", b"AbCdE:000000012345678"),
    (22, 21, 0xf8628e4978bc614e, b"ABCDE:000000012345678", b"AbCdE:0000000012345678"),
    (23, 21, 0xf8628e4978bc614e, b"ABCDE:000000012345678", b"AbCdE:00000000012345678"),
    (24, 21, 0xf8628e4978bc614e, b"ABCDE:000000012345678", b"AbCdE:000000000012345678"),
    (25, 21, 0xf8628e4978bc614e, b"ABCDE:000000012345678", b"AbCdE:0000000000012345678"),
    (9,  9, 0x98628e49669da400, b"ABCDEFG:0",             b"AbCdEfG:0"),
    (9,  9, 0x98628e49669da440, b"ABCDEFG:1",             b"AbCdEfG:1"),
    (16, 14, 0xf8628e4940bc614e, b"ABCDE:12345678",        b"AbCdEfG:12345678"),
    (17, 15, 0xf8628e4948bc614e, b"ABCDE:012345678",       b"AbCdEfG:012345678"),
    (18, 16, 0xf8628e4950bc614e, b"ABCDE:0012345678",      b"AbCdEfG:0012345678"),
    (19, 17, 0xf8628e4958bc614e, b"ABCDE:00012345678",     b"AbCdEfG:00012345678"),
    (20, 18, 0xf8628e4960bc614e, b"ABCDE:000012345678",    b"AbCdEfG:000012345678"),
    (21, 19, 0xf8628e4968bc614e, b"ABCDE:0000012345678",   b"AbCdEfG:0000012345678"),
    (22, 20, 0xf8628e4970bc614e, b"ABCDE:00000012345678",  b"AbCdEfG:00000012345678"),
    (23, 21, 0xf8628e4978bc614e, b"ABCDE:000000012345678", b"AbCdEfG:000000012345678"),
    (24, 21, 0xf8628e4978bc614e, b"ABCDE:000000012345678", b"AbCdEfG:0000000012345678"),
    (25, 21, 0xf8628e4978bc614e, b"ABCDE:000000012345678", b"AbCdEfG:00000000012345678"),
    (26, 21, 0xf8628e4978bc614e, b"ABCDE:000000012345678", b"AbCdEfG:000000000012345678"),
    (27, 21, 0xf8628e4978bc614e, b"ABCDE:000000012345678", b"AbCdEfG:0000000000012345678"),
])


class TestFunctions(TestCase):

    @classmethod
    def setUpClass(cls):
        global npvk
        try:
            npvk = pyvk.VariantKey(
                os.path.realpath(os.path.dirname(os.path.realpath(__file__)) + "/../../c/test/data/genoref.bin"),
                os.path.realpath(os.path.dirname(os.path.realpath(__file__)) + "/../../c/test/data/nrvk.10.bin"),
                os.path.realpath(os.path.dirname(os.path.realpath(__file__)) + "/../../c/test/data/rsvk.10.bin"),
                os.path.realpath(os.path.dirname(os.path.realpath(__file__)) + "/../../c/test/data/vkrs.10.bin"))
        except Exception as err:
            assert False, "Unable to initialize the class: {0}".format(err)

    def test_encode_string_id(self):
        h = npvk.encode_string_id(esidTestData[:, 6], esidTestData[:, 1])
        np.testing.assert_array_equal(h, esidTestData[:, 3].astype(np.uint64))

    def test_encode_string_num_id(self):
        h = npvk.encode_string_num_id(esidNumTestData[:, 4], b':')
        np.testing.assert_array_equal(h, esidNumTestData[:, 2].astype(np.uint64))

    def test_decode_string_id(self):
        esid, elen = npvk.decode_string_id(esidTestData[:, 3])
        np.testing.assert_array_equal(esid, esidTestData[:, 5].astype(np.string_))
        np.testing.assert_array_equal(elen, esidTestData[:, 2].astype(np.uint8))

    def test_decode_string_num_id(self):
        esid, elen = npvk.decode_string_id(esidNumTestData[:, 2])
        np.testing.assert_array_equal(esid, esidNumTestData[:, 3].astype(np.string_))
        np.testing.assert_array_equal(elen, esidNumTestData[:, 1].astype(np.uint8))

    def test_hash_string_id(self):
        h = npvk.hash_string_id(esidTestData[:, 6])
        np.testing.assert_array_equal(h, esidTestData[:, 4].astype(np.uint64))
