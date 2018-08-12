"""Tests for variantkey esid module."""

# test_esid.py
# @category   Libraries
# @author     Nicola Asuni <nicola.asuni@genomicsplc.com>
# @copyright  2017-2018 GENOMICS plc
# @license    MIT (see LICENSE)
# @link       https://github.com/genomicsplc/variantkey

import variantkey
from unittest import TestCase

# size, start, esize, esid, hsid, estr, istr
esidTestData = [
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
]


class TestFunctions(TestCase):

    def test_encode_string_id(self):
        for _, start, _, esid, _, _, istr in esidTestData:
            h = variantkey.encode_string_id(istr, start)
            self.assertEqual(h, esid)

    def test_decode_string_id(self):
        for _, _, esize, esid, _, estr, _ in esidTestData:
            h = variantkey.decode_string_id(esid)
            self.assertEqual(h[0], estr)
            self.assertEqual(h[1], esize)

    def test_decode_string_id_error(self):
        h = variantkey.decode_string_id(0xffffffffffffffff)
        self.assertEqual(h[0], b"")
        self.assertEqual(h[1], 0)

    def test_hash_string_id(self):
        for _, _, _, _, hsid, _, istr in esidTestData:
            h = variantkey.hash_string_id(istr)
            self.assertEqual(h, hsid)


class TestBenchmark(object):

    def test_encode_string_id_benchmark(self, benchmark):
        benchmark(variantkey.encode_string_id, "ABC0123456", 0)

    def test_decode_string_id_benchmark(self, benchmark):
        benchmark(variantkey.decode_string_id, 0x08628e49669e8a6a)

    def test_hash_string_id_benchmark(self, benchmark):
        benchmark(variantkey.hash_string_id, "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ")
