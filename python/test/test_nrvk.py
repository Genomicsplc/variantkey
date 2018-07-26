"""Tests for variantkey nrvk module."""

# test_nrvk.py
# @category   Libraries
# @author     Nicola Asuni <nicola.asuni@genomicsplc.com>
# @copyright  2017-2018 GENOMICS plc
# @license    MIT (see LICENSE)
# @link       https://github.com/genomicsplc/variantkey

import variantkey as bs
import os
import time
from unittest import TestCase


testData = [
    (0x0800c35093ace339,  b"1", 100001, 0x00000004, 0x01, 0x01, b"N", b"A"),
    (0x1000c3517f91cdb1,  b"2", 100002, 0x0000000e, 0x0b, 0x01, b"AAGAAAGAAAG", b"A"),
    (0x1800c351f61f65d3,  b"3", 100003, 0x0000000e, 0x01, 0x0b, b"A", b"AAGAAAGAAAG"),
    (0x2000c3521f1c15ab,  b"4", 100004, 0x0000000e, 0x08, 0x04, b"ACGTACGT", b"ACGT"),
    (0x2800c352d8f2d5b5,  b"5", 100005, 0x0000000e, 0x04, 0x08, b"ACGT", b"ACGTACGT"),
    (0x5000c3553bbf9c19, b"10", 100010, 0x00000012, 0x08, 0x08, b"ACGTACGT", b"CGTACGTA"),
    (0xb000c35b64690b25, b"22", 100022, 0x0000000b, 0x08, 0x01, b"ACGTACGT", b"N"),
    (0xb800c35bbcece603,  b"X", 100023, 0x0000000e, 0x0a, 0x02, b"AAAAAAAAGG", b"AG"),
    (0xc000c35c63741ee7,  b"Y", 100024, 0x0000000e, 0x02, 0x0a, b"AG", b"AAAAAAAAGG"),
    (0xc800c35c96c18499, b"MT", 100025, 0x00000012, 0x04, 0x0c, b"ACGT", b"AAACCCGGGTTT"),
]


class TestFunctions(TestCase):

    @classmethod
    def setUpClass(cls):
        global mfsrc, mffd, mfsize, mflast
        inputfile = os.path.realpath(
            os.path.dirname(
                os.path.realpath(__file__)) +
            "/../../c/test/data/vknr.10.bin")
        mfsrc, mffd, mfsize, mflast = bs.mmap_binfile(inputfile)
        if mffd < 0:
            assert False, "Unable to open the vknr.10.bin file"

    @classmethod
    def tearDownClass(cls):
        global mfsrc, mffd, mfsize
        h = bs.munmap_binfile(mfsrc, mffd, mfsize)
        if h != 0:
            assert False, "Error while closing the vknr.10.bin memory-mapped file"

    def test_find_ref_alt_by_variantkey(self):
        for vkey, chrom, pos, ralen, sizeref, sizealt, ref, alt in testData:
            oref, oalt, osizeref, osizealt, oralen = bs.find_ref_alt_by_variantkey(mfsrc, mflast, vkey)
            self.assertEqual(oref, ref)
            self.assertEqual(oalt, alt)
            self.assertEqual(osizeref, sizeref)
            self.assertEqual(osizealt, sizealt)
            self.assertEqual(oralen, (ralen - 2))

    def test_reverse_variantkey(self):
        for vkey, chrom, pos, ralen, sizeref, sizealt, ref, alt in testData:
            ochrom, opos, oref, oalt, osizeref, osizealt, oralen = bs.reverse_variantkey(mfsrc, mflast, vkey)
            self.assertEqual(ochrom, chrom)
            self.assertEqual(opos, pos)
            self.assertEqual(oref, ref)
            self.assertEqual(oalt, alt)
            self.assertEqual(osizeref, sizeref)
            self.assertEqual(osizealt, sizealt)
            self.assertEqual(oralen, (ralen - 2))

    def test_vknr_bin_to_tsv(self):
        fsize = bs.vknr_bin_to_tsv(mfsrc, mflast, "vknr.test")
        self.assertEqual(fsize, 305)


class TestBenchmark(object):

    global setup

    def setup():
        global mfsrc, mffd, mfsize, mflast
        if mffd >= 0:
            pass
        bs.munmap_binfile(mfsrc, mffd, mfsize)
        inputfile = os.path.realpath(
            os.path.dirname(
                os.path.realpath(__file__)) +
            "/../../c/test/data/vknr.10.bin")
        mfsrc, mffd, mfsize, mflast = bs.mmap_binfile(inputfile)
        if mffd < 0:
            assert False, "Unable to open the vknr.10.bin file"

    def test_find_ref_alt_by_variantkey(self, benchmark):
        benchmark.pedantic(
            bs.find_ref_alt_by_variantkey,
            args=[mfsrc, mflast, 0xb000c35b64690b25],
            setup=setup,
            iterations=1,
            rounds=10000)

    def test_reverse_variantkey(self, benchmark):
        benchmark.pedantic(
            bs.reverse_variantkey,
            args=[mfsrc, mflast, 0xb000c35b64690b25],
            setup=setup,
            iterations=1,
            rounds=10000)

    def test_tearDown(self):
        global mfsrc, mffd, mfsize
        h = bs.munmap_binfile(mfsrc, mffd, mfsize)
        mffd = -1
        mfsize = 0
        if h != 0:
            assert False, "Error while closing the vknr.10.bin memory-mapped file"
