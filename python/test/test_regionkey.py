"""Tests for regionkey module."""

# test_variantkey.py
# @category   Libraries
# @author     Nicola Asuni <nicola.asuni@genomicsplc.com>
# @copyright  2017-2018 GENOMICS plc
# @license    MIT (see LICENSE)
# @link       https://github.com/genomicsplc/variantkey

import variantkey
from unittest import TestCase

regionsTestData = [
    (b"1", 1000, 1100,  0,  1, 0, 0x080001f400002260, b"080001f400002260", 0x00000000100003e8, 0x000000001000044c),
    (b"2", 1001, 1201,  1,  2, 1, 0x100001f48000258a, b"100001f48000258a", 0x00000000200003e9, 0x00000000200004b1),
    (b"3", 1002, 1302, -1,  3, 2, 0x180001f5000028b4, b"180001f5000028b4", 0x00000000300003ea, 0x0000000030000516),
    (b"4", 1003, 1403,  0,  4, 0, 0x200001f580002bd8, b"200001f580002bd8", 0x00000000400003eb, 0x000000004000057b),
    (b"5", 1004, 1504,  1,  5, 1, 0x280001f600002f02, b"280001f600002f02", 0x00000000500003ec, 0x00000000500005e0),
    (b"10", 1005, 1605, -1, 10, 2, 0x500001f68000322c, b"500001f68000322c", 0x00000000a00003ed, 0x00000000a0000645),
    (b"22", 1006, 1706,  0, 22, 0, 0xb00001f700003550, b"b00001f700003550", 0x00000001600003ee, 0x00000001600006aa),
    (b"X", 1007, 1807,  1, 23, 1, 0xb80001f78000387a, b"b80001f78000387a", 0x00000001700003ef, 0x000000017000070f),
    (b"Y", 1008, 1908, -1, 24, 2, 0xc00001f800003ba4, b"c00001f800003ba4", 0x00000001800003f0, 0x0000000180000774),
    (b"MT", 1009, 2009,  0, 25, 0, 0xc80001f880003ec8, b"c80001f880003ec8", 0x00000001900003f1, 0x00000001900007d9),
]


class TestFunctions(TestCase):

    def test_encode_region_strand(self):
        for _, _, _, strand, _, estrand, _, _, _, _ in regionsTestData:
            h = variantkey.encode_region_strand(strand)
            self.assertEqual(h, estrand)

    def test_decode_region_strand(self):
        for _, _, _, strand, _, estrand, _, _, _, _ in regionsTestData:
            h = variantkey.decode_region_strand(estrand)
            self.assertEqual(h, strand)

    def test_encode_regionkey(self):
        for _, startpos, endpos, _, echrom, estrand, rk, _, _, _ in regionsTestData:
            h = variantkey.encode_regionkey(echrom, startpos, endpos, estrand)
            self.assertEqual(h, rk)

    def test_extract_regionkey_chrom(self):
        for _, _, _, _, echrom, _, rk, _, _, _ in regionsTestData:
            h = variantkey.extract_regionkey_chrom(rk)
            self.assertEqual(h, echrom)

    def test_extract_regionkey_startpos(self):
        for _, startpos, _, _, _, _, rk, _, _, _ in regionsTestData:
            h = variantkey.extract_regionkey_startpos(rk)
            self.assertEqual(h, startpos)

    def test_extract_regionkey_endpos(self):
        for _, _, endpos, _, _, _, rk, _, _, _ in regionsTestData:
            h = variantkey.extract_regionkey_endpos(rk)
            self.assertEqual(h, endpos)

    def test_extract_regionkey_strand(self):
        for _, _, _, _, _, estrand, rk, _, _, _ in regionsTestData:
            h = variantkey.extract_regionkey_strand(rk)
            self.assertEqual(h, estrand)

    def test_decode_regionkey(self):
        for _, startpos, endpos, _, echrom, estrand, rk, _, _, _ in regionsTestData:
            h = variantkey.decode_regionkey(rk)
            self.assertEqual(h[0], echrom)
            self.assertEqual(h[1], startpos)
            self.assertEqual(h[2], endpos)
            self.assertEqual(h[3], estrand)

    def test_reverse_regionkey(self):
        for chrom, startpos, endpos, strand, _, _, rk, _, _, _ in regionsTestData:
            h = variantkey.reverse_regionkey(rk)
            self.assertEqual(h[0], chrom)
            self.assertEqual(h[1], startpos)
            self.assertEqual(h[2], endpos)
            self.assertEqual(h[3], strand)

    def test_regionkey(self):
        for chrom, startpos, endpos, strand, _, _, rk, _, _, _ in regionsTestData:
            h = variantkey.regionkey(chrom, startpos, endpos, strand)
            self.assertEqual(h, rk)

    def test_regionkey_hex(self):
        for _, _, _, _, _, _, rk, rs, _, _ in regionsTestData:
            h = variantkey.regionkey_hex(rk)
            self.assertEqual(h, rs)

    def test_parse_regionkey_hex(self):
        for _, _, _, _, _, _, rk, rs, _, _ in regionsTestData:
            h = variantkey.parse_regionkey_hex(rs)
            self.assertEqual(h, rk)

    def test_are_overlapping_regions(self):
        overlapTestData = [
            (1, 5,  7,  2, 5, 7, 0),  # different chromosome
            (1, 0,  2,  1, 3, 7, 0),  # -[-]|---|----
            (2, 1,  3,  2, 3, 7, 0),  # --[-]---|----
            (3, 2,  4,  3, 3, 7, 1),  # ---[|]--|----
            (4, 3,  5,  4, 3, 7, 1),  # ----[-]-|----
            (5, 4,  6,  5, 3, 7, 1),  # ----|[-]|----
            (6, 5,  7,  6, 3, 7, 1),  # ----|-[ ]----
            (10, 6,  8, 10, 3, 7, 1), # ----|--[|]---
            (22, 7,  9, 22, 3, 7, 0), # ----|---[-]--
            (23, 8, 10, 23, 3, 7, 0), # ----|---|[-]-
            (24, 2,  8, 24, 3, 7, 1), # ---[|---|]---
            (25, 3,  7, 25, 3, 7, 1), # ----[---]----
        ]
        for a_chrom, a_startpos, a_endpos, b_chrom, b_startpos, b_endpos, res in overlapTestData:
            h = variantkey.are_overlapping_regions(a_chrom, a_startpos, a_endpos, b_chrom, b_startpos, b_endpos)
            self.assertEqual(h, res)

    def test_get_regionkey_chrom_startpos(self):
        for _, _, _, _, _, _, rk, _, chrom_startpos, _ in regionsTestData:
            h = variantkey.get_regionkey_chrom_startpos(rk)
            self.assertEqual(h, chrom_startpos)

    def test_get_regionkey_chrom_endpos(self):
        for _, _, _, _, _, _, rk, _, _, chrom_endpos in regionsTestData:
            h = variantkey.get_regionkey_chrom_endpos(rk)
            self.assertEqual(h, chrom_endpos)


class TestBenchmark(object):

    def test_encode_region_strand_benchmark(self, benchmark):
        benchmark(variantkey.encode_region_strand, -1)

    def test_decode_region_strand_benchmark(self, benchmark):
        benchmark(variantkey.decode_region_strand, 2)

    def test_encode_regionkey_benchmark(self, benchmark):
        benchmark(variantkey.encode_regionkey, 25, 1000, 2000, 2)

    def test_extract_regionkey_chrom_benchmark(self, benchmark):
        benchmark(variantkey.extract_regionkey_chrom, 0x080001f400002260)

    def test_extract_regionkey_startpos_benchmark(self, benchmark):
        benchmark(variantkey.extract_regionkey_startpos, 0x080001f400002260)

    def test_extract_regionkey_endpos_benchmark(self, benchmark):
        benchmark(variantkey.extract_regionkey_endpos, 0x080001f400002260)

    def test_extract_regionkey_strand_benchmark(self, benchmark):
        benchmark(variantkey.extract_regionkey_strand, 0x080001f400002260)

    def test_decode_regionkey_benchmark(self, benchmark):
        benchmark(variantkey.decode_regionkey, 0x080001f400002260)

    def test_reverse_regionkey_benchmark(self, benchmark):
        benchmark(variantkey.reverse_regionkey, 0x080001f400002260)

    def test_regionkey_benchmark(self, benchmark):
        benchmark(variantkey.regionkey, "MT", 1000, 2000, -1)

    def test_are_overlapping_regions_benchmark(self, benchmark):
        benchmark(variantkey.are_overlapping_regions, 25, 1000, 2000, 25, 1500, 2500)

    def test_get_regionkey_chrom_startpos_benchmark(self, benchmark):
        benchmark(variantkey.get_regionkey_chrom_startpos, 0x080001f400002260)

    def test_get_regionkey_chrom_endpos_benchmark(self, benchmark):
        benchmark(variantkey.get_regionkey_chrom_endpos, 0x080001f400002260)
