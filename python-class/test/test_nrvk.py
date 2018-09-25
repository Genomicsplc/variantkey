"""Tests for variantkey nrvk module."""

# test_nrvk.py
# @category   Libraries
# @author     Nicola Asuni <nicola.asuni@genomicsplc.com>
# @copyright  2017-2018 GENOMICS plc
# @license    MIT (see LICENSE)
# @link       https://github.com/genomicsplc/variantkey

import pyvariantkey.variantkey as pyvk
import numpy as np
import os
from unittest import TestCase

# 0:vkey, 1:chrom, 2:pos, 3:ralen, 4:sizeref, 5:sizealt, 6:csp, 7:cep, 8:ref, 9:alt
testData = np.array([
    (0x0800c35093ace339,  b"1", 100001, 0x00000004, 0x01, 0x01, 0x00000000100186a1, 0x00000000100186a2, b"N", b"A"),
    (0x1000c3517f91cdb1,  b"2", 100002, 0x0000000e, 0x0b, 0x01, 0x00000000200186a2, 0x00000000200186ad, b"AAGAAAGAAAG", b"A"),
    (0x1800c351f61f65d3,  b"3", 100003, 0x0000000e, 0x01, 0x0b, 0x00000000300186a3, 0x00000000300186a4, b"A", b"AAGAAAGAAAG"),
    (0x2000c3521f1c15ab,  b"4", 100004, 0x0000000e, 0x08, 0x04, 0x00000000400186a4, 0x00000000400186ac, b"ACGTACGT", b"ACGT"),
    (0x2800c352d8f2d5b5,  b"5", 100005, 0x0000000e, 0x04, 0x08, 0x00000000500186a5, 0x00000000500186a9, b"ACGT", b"ACGTACGT"),
    (0x5000c3553bbf9c19, b"10", 100010, 0x00000012, 0x08, 0x08, 0x00000000a00186aa, 0x00000000a00186b2, b"ACGTACGT", b"CGTACGTA"),
    (0xb000c35b64690b25, b"22", 100022, 0x0000000b, 0x08, 0x01, 0x00000001600186b6, 0x00000001600186be, b"ACGTACGT", b"N"),
    (0xb800c35bbcece603,  b"X", 100023, 0x0000000e, 0x0a, 0x02, 0x00000001700186b7, 0x00000001700186c1, b"AAAAAAAAGG", b"AG"),
    (0xc000c35c63741ee7,  b"Y", 100024, 0x0000000e, 0x02, 0x0a, 0x00000001800186b8, 0x00000001800186ba, b"AG", b"AAAAAAAAGG"),
    (0xc800c35c96c18499, b"MT", 100025, 0x00000012, 0x04, 0x0c, 0x00000001900186b9, 0x00000001900186bd, b"ACGT", b"AAACCCGGGTTT"),
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

    def test_find_ref_alt_by_variantkey(self):
        oref, oalt, osizeref, osizealt, oralen = npvk.find_ref_alt_by_variantkey(testData[:, 0])
        np.testing.assert_array_equal(oref, testData[:, 8].astype('|S256'))
        np.testing.assert_array_equal(oalt, testData[:, 9].astype('|S256'))
        np.testing.assert_array_equal(osizeref, testData[:, 4].astype(np.uint8))
        np.testing.assert_array_equal(osizealt, testData[:, 5].astype(np.uint8))
        np.testing.assert_array_equal(oralen, (testData[:, 3].astype(np.uint8) - 2))

    def test_reverse_variantkey(self):
        ochrom, opos, oref, oalt, osizeref, osizealt, oralen = npvk.reverse_variantkey(testData[:, 0])
        np.testing.assert_array_equal(ochrom, testData[:, 1].astype('|S2'))
        np.testing.assert_array_equal(opos, testData[:, 2].astype(np.uint32))
        np.testing.assert_array_equal(oref, testData[:, 8].astype('|S256'))
        np.testing.assert_array_equal(oalt, testData[:, 9].astype('|S256'))
        np.testing.assert_array_equal(osizeref, testData[:, 4].astype(np.uint8))
        np.testing.assert_array_equal(osizealt, testData[:, 5].astype(np.uint8))
        np.testing.assert_array_equal(oralen, (testData[:, 3].astype(np.uint8) - 2))

    def test_get_variantkey_ref_length(self):
        osizeref = npvk.get_variantkey_ref_length(testData[:, 0])
        np.testing.assert_array_equal(osizeref, testData[:, 4].astype(np.uint8))

    def test_get_variantkey_ref_length_reversible(self):
        osizeref = npvk.get_variantkey_ref_length(0x1800925199160000)
        np.testing.assert_array_equal(osizeref, 3)

    def test_get_variantkey_ref_length_not_found(self):
        osizeref = npvk.get_variantkey_ref_length(0xffffffffffffffff)
        np.testing.assert_array_equal(osizeref, 0)

    def test_get_variantkey_endpos(self):
        endpos = npvk.get_variantkey_endpos(testData[:, 0])
        np.testing.assert_array_equal(endpos, testData[:, 2].astype(np.uint32) + testData[:, 4].astype(np.uint32))

    def test_get_variantkey_chrom_startpos(self):
        res = npvk.get_variantkey_chrom_startpos(testData[:, 0])
        np.testing.assert_array_equal(res, testData[:, 6].astype(np.uint64))

    def test_get_variantkey_chrom_endpos(self):
        res = npvk.get_variantkey_chrom_endpos(testData[:, 0])
        np.testing.assert_array_equal(res, testData[:, 7].astype(np.uint64))

    def test_nrvk_bin_to_tsv(self):
        fsize = npvk.nrvk_bin_to_tsv("nrvk.test")
        np.testing.assert_array_equal(fsize, 305)
