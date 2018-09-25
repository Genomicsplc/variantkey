"""Tests for variantkey genoref module."""

# test_genoref.py
# @category   Libraries
# @author     Nicola Asuni <nicola.asuni@genomicsplc.com>
# @copyright  2017-2018 GENOMICS plc
# @license    MIT (see LICENSE)
# @link       https://github.com/genomicsplc/variantkey

import pyvariantkey.variantkey as pyvk
import numpy as np
import os
from unittest import TestCase


class TestFunctions(TestCase):

    @classmethod
    def setUpClass(self):
        try:
            self.npvk = pyvk.VariantKey(
                os.path.realpath(os.path.dirname(os.path.realpath(__file__)) + "/../../c/test/data/genoref.bin"),
                os.path.realpath(os.path.dirname(os.path.realpath(__file__)) + "/../../c/test/data/nrvk.10.bin"),
                os.path.realpath(os.path.dirname(os.path.realpath(__file__)) + "/../../c/test/data/rsvk.10.bin"),
                os.path.realpath(os.path.dirname(os.path.realpath(__file__)) + "/../../c/test/data/vkrs.10.bin"))
        except Exception as err:
            assert False, "Unable to initialize the class: {0}".format(err)

    def test_get_genoref_seq(self):
        chrom = np.array([1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25], dtype=np.uint8)
        ref = self.npvk.get_genoref_seq(chrom, 0)  # first
        np.testing.assert_array_equal(ref, np.repeat(b'A', 25))
        ref = self.npvk.get_genoref_seq(chrom, (26 - chrom))  # last
        e = np.array([b'Z', b'Y', b'X', b'W', b'V', b'U', b'T', b'S', b'R', b'Q', b'P', b'O', b'N', b'M', b'L', b'K', b'J', b'I', b'H', b'G', b'F', b'E', b'D', b'C', b'B'], dtype='|S1')
        np.testing.assert_array_equal(ref, e)
        ref = self.npvk.get_genoref_seq(chrom, (27 - chrom))  # invalid
        e = np.array([b'', b'', b'', b'', b'', b'', b'', b'', b'', b'', b'', b'', b'', b'', b'', b'', b'', b'', b'', b'', b'', b'', b'', b'', b''], dtype='|S1')
        np.testing.assert_array_equal(ref, e)

    def test_check_reference(self):
        # 0:exp, 1:chrom, 2:pos, 3:sizeref, 4:ref
        tdata = np.array([
            (0, 1,   0,  1, b"A"),
            (0, 1,  25,  1, b"Z"),
            (0, 25,  0,  1, b"A"),
            (0, 25,  1,  1, b"B"),
            (0, 2,   0, 25, b"ABCDEFGHIJKLmnopqrstuvwxy"),
            (-2, 1,  26,  4, b"ZABC"),
            (-1, 1,   0, 26, b"ABCDEFGHIJKLmnopqrstuvwxyJ"),
            (-1, 14,  2,  3, b"ZZZ"),
            (1, 1,   0,  1, b"N"),
            (1, 10, 13,  1, b"A"),
            (1, 1,   3,  1, b"B"),
            (1, 1,   1,  1, b"C"),
            (1, 1,   0,  1, b"D"),
            (1, 1,   3,  1, b"A"),
            (1, 1,   0,  1, b"H"),
            (1, 1,   7,  1, b"A"),
            (1, 1,   0,  1, b"V"),
            (1, 1,  21,  1, b"A"),
            (1, 1,   0,  1, b"W"),
            (1, 1,  19,  1, b"W"),
            (1, 1,  22,  1, b"A"),
            (1, 1,  22,  1, b"T"),
            (1, 1,   2,  1, b"S"),
            (1, 1,   6,  1, b"S"),
            (1, 1,  18,  1, b"C"),
            (1, 1,  18,  1, b"G"),
            (1, 1,   0,  1, b"M"),
            (1, 1,   2,  1, b"M"),
            (1, 1,  12,  1, b"A"),
            (1, 1,  12,  1, b"C"),
            (1, 1,   6,  1, b"K"),
            (1, 1,  19,  1, b"K"),
            (1, 1,  10,  1, b"G"),
            (1, 1,  10,  1, b"T"),
            (1, 1,   0,  1, b"R"),
            (1, 1,   6,  1, b"R"),
            (1, 1,  17,  1, b"A"),
            (1, 1,  17,  1, b"G"),
            (1, 1,   2,  1, b"Y"),
            (1, 1,  19,  1, b"Y"),
            (1, 1,  24,  1, b"C"),
            (1, 1,  24,  1, b"T"),
        ])
        ret = self.npvk.check_reference(tdata[:, 1], tdata[:, 2], tdata[:, 4])
        np.testing.assert_array_equal(ret, tdata[:, 0].astype(np.int_))

    def test_flip_allele(self):
        allele = np.array([b"ATCGMKRYBVDHWSNatcgmkrybvdhwsn", b"ATCGMKRYBVDHWSNatcgmkrybvdhwsn"])
        e = np.array([b"TAGCKMYRVBHDWSNTAGCKMYRVBHDWSN", b"TAGCKMYRVBHDWSNTAGCKMYRVBHDWSN"])
        ret = self.npvk.flip_allele(allele)
        np.testing.assert_array_equal(ret, e)

    def test_normalize_variant(self):
        # 0:code, 1:chrom, 2:pos, 3:epos, 4:sizeref, 5:sizealt, 6:esizeref, 7:esizealt, 8:eref, 9:ealt, 10:ref, 11:alt
        tdata = np.array([
            (-2, 1, 26, 26, 1, 1, 1, 1, b"A",   b"C",  b"A",      b"C"),      # invalid position
            (-1, 1,  0,  0, 1, 1, 1, 1, b"J",   b"C",  b"J",      b"C"),      # invalid reference
            (4, 1,  0,  0, 1, 1, 1, 1,  b"A",   b"C",  b"T",      b"G"),      # flip
            (0, 1,  0,  0, 1, 1, 1, 1,  b"A",   b"C",  b"A",      b"C"),      # OK
            (32, 13, 2,  3, 3, 2, 2, 1, b"DE",  b"D",  b"CDE",    b"CD"),     # left trim
            (48, 13, 2,  3, 3, 3, 1, 1, b"D",   b"F",  b"CDE",    b"CFE"),    # left trim + right trim
            (48, 1,  0,  2, 6, 6, 1, 1, b"C",   b"K",  b"aBCDEF", b"aBKDEF"),  # left trim + right trim
            (0, 1,  0,  0, 1, 0, 1, 0,  b"A",   b"",   b"A",      b""),       # OK
            (8, 1,  3,  2, 1, 0, 2, 1,  b"CD",  b"C",  b"D",      b""),       # left extend
            (0, 1, 24, 24, 1, 2, 1, 2,  b"Y",   b"CK", b"Y",      b"CK"),     # OK
            (2, 1,  0,  0, 1, 1, 1, 1,  b"A",   b"G",  b"G",      b"A"),      # swap
            (6, 1,  0,  0, 1, 1, 1, 1,  b"A",   b"C",  b"G",      b"T"),      # swap + flip
        ])
        ncode, npos, nref, nalt, nsizeref, nsizealt = self.npvk.normalize_variant(tdata[:, 1], tdata[:, 2], tdata[:, 10], tdata[:, 11])
        np.testing.assert_array_equal(ncode, tdata[:, 0].astype(np.int_))
        np.testing.assert_array_equal(npos, tdata[:, 3].astype(np.uint32))
        np.testing.assert_array_equal(nref, tdata[:, 8].astype('|S256'))
        np.testing.assert_array_equal(nalt, tdata[:, 9].astype('|S256'))
        np.testing.assert_array_equal(nsizeref, tdata[:, 6].astype(np.uint8))
        np.testing.assert_array_equal(nsizealt, tdata[:, 7].astype(np.uint8))
