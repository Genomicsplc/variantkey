"""Tests for variantkey esid module."""

# test_esid.py
# @category   Libraries
# @author     Nicola Asuni <nicola.asuni@genomicsplc.com>
# @copyright  2017-2018 GENOMICS plc
# @license    MIT (see LICENSE)
# @link       https://github.com/genomicsplc/variantkey

import variantkey
from unittest import TestCase

# istr, size, start, esid, estr, esize, hsid
esidTestData = [
    (b"0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ", 36,  0, 0x0411493515597619, b"0123456789",  10, 0xb3a5fdb8808cb7dc),
    (b"0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ", 36,  1, 0x04524d45565d8661, b"123456789A",  10, 0xb3a5fdb8808cb7dc),
    (b"0223456789ABCDEFGHIJKLMNOPQRSTUVWXYZ", 36, 10, 0x08628e49669e8a6a, b"ABCDEFGHIJ",  10, 0xd93e382010f46f32),
    (b"0133456789ABCDEFGHIJKLMNOPQRSTUVWXYZ", 36, 25, 0x0c31cb3d35db7e39, b"PQRSTUVWXY",  10, 0xbfc379f4a71cb3be),
    (b"1123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ", 36, 26, 0x0c72cf4d76df8e7a, b"QRSTUVWXYZ",  10, 0xdba299d06b54215d),
    (b"0123456789ABCDEFGHIJKLMNOPQRSTUVWXY[", 36, 35, 0x0ec0000000000000, b"[",            1, 0x996dcdc8ef7c89c1),
    (b"012345",                                6,  0, 0x0411493515000000, b"012345",       6, 0xa81279395d9c891e),
    (b"012345",                                6,  1, 0x04524d4540000000, b"12345",        5, 0xa81279395d9c891e),
    (b"012345",                                6,  3, 0x04d4540000000000, b"345",          3, 0xa81279395d9c891e),
    (b"012345",                                6,  5, 0x0540000000000000, b"5",            1, 0xa81279395d9c891e),
    (b" !\"#$%&'()",                          10,  0, 0x0fc1083105187209, b"_!\"#$%&'()", 10, 0x838c148028a06998),
    (b"*+,-./0123",                           10,  0, 0x028b30d38f411493, b"*+,-./0123",  10, 0x96e42cf7aefeccf0),
    (b"456789:;<=",                           10,  0, 0x051559761969b71d, b"456789:;<=",  10, 0xb391c7f288e446ee),
    (b">?@ABCDEFG",                           10,  0, 0x079f8218a39259a7, b">?@ABCDEFG",  10, 0xcc0de20381405e0a),
    (b"HIJKLMNOPQ",                           10,  0, 0x0a29aabb2dbafc31, b"HIJKLMNOPQ",  10, 0xbbfc630eacb6c552),
    (b"RSTUVWXYZ[",                           10,  0, 0x0cb3d35db7e39ebb, b"RSTUVWXYZ[",  10, 0x803eda0a1781a117),
    (b"\\]^_`abcde",                          10,  0, 0x0f3dfbf8218a3925, b"\\]^_@ABCDE", 10, 0x878ef2947ee3cc2b),
    (b"fghijklmno",                           10,  0, 0x09a7a29aabb2dbaf, b"FGHIJKLMNO",  10, 0xbaadaef07826969b),
    (b"pqrstuvwxy",                           10,  0, 0x0c31cb3d35db7e39, b"PQRSTUVWXY",  10, 0xd3da4d5e28be3590),
    (b"z{|}~\t",                               6,  0, 0x0ebbf3dfbf000000, b"Z[\\]^_",      6, 0xeadc752a50c5b850),
    (b"123456781234567",                      15,  0, 0x04524d45565d8452, b"1234567812",  10, 0x9af6ee553ba41827),
    (b"12345678123456",                       14,  0, 0x04524d45565d8452, b"1234567812",  10, 0xc94e10e9fe153fd2),
    (b"1234567812345",                        13,  0, 0x04524d45565d8452, b"1234567812",  10, 0x9b56411c7abdefe6),
    (b"123456781234",                         12,  0, 0x04524d45565d8452, b"1234567812",  10, 0xcbec35944ff1c863),
    (b"12345678123",                          11,  0, 0x04524d45565d8452, b"1234567812",  10, 0x929920298cf56b9b),
    (b"1234567812",                           10,  0, 0x04524d45565d8452, b"1234567812",  10, 0xbdf006d50a33ad90),
    (b"123456781",                             9,  0, 0x04524d45565d8440, b"123456781",    9, 0xde094182e93557ae),
    (b"12345678",                              8,  0, 0x04524d45565d8000, b"12345678",     8, 0xccbc926a73ece95c),
    (b"1234567",                               7,  0, 0x04524d45565c0000, b"1234567",      7, 0xd18b960e2f99c279),
    (b"123456",                                6,  0, 0x04524d4556000000, b"123456",       6, 0x811c9c02fcc22096),
    (b"12345",                                 5,  0, 0x04524d4540000000, b"12345",        5, 0xd863467dedd1cab1),
    (b"1234",                                  4,  0, 0x04524d4000000000, b"1234",         4, 0xbef404ecb71e4cd9),
    (b"123",                                   3,  0, 0x04524c0000000000, b"123",          3, 0xfad470644116fa54),
    (b"12",                                    2,  0, 0x0452000000000000, b"12",           2, 0xd94ae1e7173e781d),
    (b"1",                                     1,  0, 0x0440000000000000, b"1",            1, 0xfc73100baa96ad81),
    (b"",                                      0,  0, 0x0000000000000000, b"",             0, 0x8000000000000000),
]


class TestFunctions(TestCase):

    def test_encode_string_id(self):
        for istr, _, start, esid, _, _, _ in esidTestData:
            h = variantkey.encode_string_id(istr, start)
            self.assertEqual(h, esid)

    def test_decode_string_id(self):
        for _, _, _, esid, estr, esize, _ in esidTestData:
            h = variantkey.decode_string_id(esid)
            print(esid)
            print(h)
            print(esize)
            self.assertEqual(h[0], estr)
            self.assertEqual(h[1], esize)

    def test_decode_string_id_hash(self):
        for _, _, _, _, _, _, hsid in esidTestData:
            h = variantkey.decode_string_id(hsid)
            self.assertEqual(h[0], b"")
            self.assertEqual(h[1], 0)

    def test_hash_string_id(self):
        for istr, _, _, _, _, _, hsid in esidTestData:
            h = variantkey.hash_string_id(istr)
            self.assertEqual(h, hsid)


class TestBenchmark(object):

    def test_encode_string_id_benchmark(self, benchmark):
        benchmark(variantkey.encode_string_id, "ABC0123456", 0)

    def test_decode_string_id_benchmark(self, benchmark):
        benchmark(variantkey.decode_string_id, 0x08628e49669e8a6a)

    def test_hash_string_id_benchmark(self, benchmark):
        benchmark(variantkey.hash_string_id, "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ")
