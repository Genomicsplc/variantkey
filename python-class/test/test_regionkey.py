"""Tests for variantkey regionkey module."""

# test_regionkey.py
# @category   Libraries
# @author     Nicola Asuni <nicola.asuni@genomicsplc.com>
# @copyright  2017-2018 GENOMICS plc
# @license    MIT (see LICENSE)
# @link       https://github.com/genomicsplc/variantkey

import pyvariantkey.variantkey as pyvk
import numpy as np
import os
from unittest import TestCase

# 0:chrom, 1:startpos, 2:endpos, 3:strand, 4:echrom, 5:estrand, 6:rk, 7:rs, 8:chrom_startpos, 9:chrom_endpos
regionsTestData = np.array([
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
], dtype=np.string_)

# 0:res, 1:a_chrom, 2:b_chrom, 3:a_startpos, 4:b_startpos, 5:a_endpos, 6:b_endpos, 7:a_rk, 8:a_vk, 9:b_rk
overlapTestData = np.array([
    (0,  1,  2, 5, 5,  7, 7, 0x0800000280000038, 0x0800000290920000, 0x1000000280000038),  # different chromosome
    (0,  1,  1, 0, 3,  2, 7, 0x0800000000000010, 0x0800000010920000, 0x0800000180000038),  # -[-]|---|----
    (0,  2,  2, 1, 3,  3, 7, 0x1000000080000018, 0x1000000090920000, 0x1000000180000038),  # --[-]---|----
    (1,  3,  3, 2, 3,  4, 7, 0x1800000100000020, 0x1800000110920000, 0x1800000180000038),  # ---[|]--|----
    (1,  4,  4, 3, 3,  5, 7, 0x2000000180000028, 0x2000000190920000, 0x2000000180000038),  # ----[-]-|----
    (1,  5,  5, 4, 3,  6, 7, 0x2800000200000030, 0x2800000210920000, 0x2800000180000038),  # ----|[-]|----
    (1,  6,  6, 5, 3,  7, 7, 0x3000000280000038, 0x3000000290920000, 0x3000000180000038),  # ----|-[ ]----
    (1, 10, 10, 6, 3,  8, 7, 0x5000000300000040, 0x5000000310920000, 0x5000000180000038),  # ----|--[|]---
    (0, 22, 22, 7, 3,  9, 7, 0xb000000380000048, 0xb000000390920000, 0xb000000180000038),  # ----|---[-]--
    (0, 23, 23, 8, 3, 10, 7, 0xb800000400000050, 0xb800000410920000, 0xb800000180000038),  # ----|---|[-]-
    (1, 24, 24, 2, 3,  8, 7, 0xc000000100000040, 0xc000000130911200, 0xc000000180000038),  # ---[|---|]---
    (1, 25, 25, 3, 3,  7, 7, 0xc800000180000038, 0xc8000001a0912000, 0xc800000180000038),  # ----[---]----
], dtype=np.string_)


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

    @classmethod
    def tearDownClass(cls):
        global npvk
        npvk.close()

    def test_encode_region_strand(self):
        h = npvk.encode_region_strand(regionsTestData[:, 3])
        np.testing.assert_array_equal(h, regionsTestData[:, 5].astype(np.uint8))

    def test_decode_region_strand(self):
        h = npvk.decode_region_strand(regionsTestData[:, 5])
        np.testing.assert_array_equal(h, regionsTestData[:, 3].astype(np.int16))

    def test_encode_regionkey(self):
        h = npvk.encode_regionkey(regionsTestData[:, 4], regionsTestData[:, 1], regionsTestData[:, 2], regionsTestData[:, 5])
        np.testing.assert_array_equal(h, regionsTestData[:, 6].astype(np.uint64))

    def test_extract_regionkey_chrom(self):
        h = npvk.extract_regionkey_chrom(regionsTestData[:, 6])
        np.testing.assert_array_equal(h, regionsTestData[:, 4].astype(np.uint8))

    def test_extract_regionkey_startpos(self):
        h = npvk.extract_regionkey_startpos(regionsTestData[:, 6])
        np.testing.assert_array_equal(h, regionsTestData[:, 1].astype(np.uint32))

    def test_extract_regionkey_endpos(self):
        h = npvk.extract_regionkey_endpos(regionsTestData[:, 6])
        np.testing.assert_array_equal(h, regionsTestData[:, 2].astype(np.uint32))

    def test_extract_regionkey_strand(self):
        h = npvk.extract_regionkey_strand(regionsTestData[:, 6])
        np.testing.assert_array_equal(h, regionsTestData[:, 5].astype(np.uint8))

    def test_decode_regionkey(self):
        echrom, startpos, endpos, estrand = npvk.decode_regionkey(regionsTestData[:, 6])
        np.testing.assert_array_equal(echrom, regionsTestData[:, 4].astype(np.uint8))
        np.testing.assert_array_equal(startpos, regionsTestData[:, 1].astype(np.uint32))
        np.testing.assert_array_equal(endpos, regionsTestData[:, 2].astype(np.uint32))
        np.testing.assert_array_equal(estrand, regionsTestData[:, 5].astype(np.uint8))

    def test_reverse_regionkey(self):
        chrom, startpos, endpos, strand = npvk.reverse_regionkey(regionsTestData[:, 6])
        np.testing.assert_array_equal(chrom, regionsTestData[:, 0].astype('|S2'))
        np.testing.assert_array_equal(startpos, regionsTestData[:, 1].astype(np.uint32))
        np.testing.assert_array_equal(endpos, regionsTestData[:, 2].astype(np.uint32))
        np.testing.assert_array_equal(strand, regionsTestData[:, 3].astype(np.int16))

    def test_regionkey(self):
        h = npvk.regionkey(regionsTestData[:, 0], regionsTestData[:, 1], regionsTestData[:, 2], regionsTestData[:, 3])
        np.testing.assert_array_equal(h, regionsTestData[:, 6].astype(np.uint64))

    def test_regionkey_hex(self):
        h = npvk.regionkey_hex(regionsTestData[:, 6])
        np.testing.assert_array_equal(h, regionsTestData[:, 7].astype('|S16'))

    def test_parse_regionkey_hex(self):
        h = npvk.parse_regionkey_hex(regionsTestData[:, 7])
        np.testing.assert_array_equal(h, regionsTestData[:, 6].astype(np.uint64))

    def test_get_regionkey_chrom_startpos(self):
        h = npvk.get_regionkey_chrom_startpos(regionsTestData[:, 6])
        np.testing.assert_array_equal(h, regionsTestData[:, 8].astype(np.uint64))

    def test_get_regionkey_chrom_endpos(self):
        h = npvk.get_regionkey_chrom_endpos(regionsTestData[:, 6])
        np.testing.assert_array_equal(h, regionsTestData[:, 9].astype(np.uint64))

    def test_are_overlapping_regions(self):
        h = npvk.are_overlapping_regions(overlapTestData[:, 1], overlapTestData[:, 3], overlapTestData[:, 5], overlapTestData[:, 2], overlapTestData[:, 4], overlapTestData[:, 6])
        np.testing.assert_array_equal(h, overlapTestData[:, 0].astype(np.uint8))

    def test_are_overlapping_region_regionkey(self):
        h = npvk.are_overlapping_region_regionkey(overlapTestData[:, 1], overlapTestData[:, 3], overlapTestData[:, 5], overlapTestData[:, 9])
        np.testing.assert_array_equal(h, overlapTestData[:, 0].astype(np.uint8))

    def test_are_overlapping_regionkeys(self):
        h = npvk.are_overlapping_regionkeys(overlapTestData[:, 7], overlapTestData[:, 9])
        np.testing.assert_array_equal(h, overlapTestData[:, 0].astype(np.uint8))

    def test_are_overlapping_variantkey_regionkey(self):
        h = npvk.are_overlapping_variantkey_regionkey(overlapTestData[:, 8], overlapTestData[:, 9])
        np.testing.assert_array_equal(h, overlapTestData[:, 0].astype(np.uint8))

    def test_variantkey_to_regionkey(self):
        h = npvk.variantkey_to_regionkey(overlapTestData[:, 8])
        np.testing.assert_array_equal(h, overlapTestData[:, 7].astype(np.uint64))
