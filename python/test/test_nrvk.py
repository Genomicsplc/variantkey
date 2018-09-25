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
]


class TestFunctions(TestCase):

    @classmethod
    def setUpClass(cls):
        global mf, mc, nrows
        inputfile = os.path.realpath(
            os.path.dirname(
                os.path.realpath(__file__)) +
            "/../../c/test/data/nrvk.10.bin")
        mf, mc, nrows = bs.mmap_nrvk_file(inputfile)
        if nrows <= 0:
            assert False, "Unable to open the nrvk.10.bin file"

    @classmethod
    def tearDownClass(cls):
        global mf, mc, nrows
        h = bs.munmap_binfile(mf)
        if h != 0:
            assert False, "Error while closing the nrvk.10.bin memory-mapped file"

    def test_find_ref_alt_by_variantkey(self):
        for vkey, chrom, pos, ralen, sizeref, sizealt, csp, cep, ref, alt in testData:
            oref, oalt, osizeref, osizealt, oralen = bs.find_ref_alt_by_variantkey(mc, vkey)
            self.assertEqual(oref, ref)
            self.assertEqual(oalt, alt)
            self.assertEqual(osizeref, sizeref)
            self.assertEqual(osizealt, sizealt)
            self.assertEqual(oralen, (ralen - 2))

    def test_reverse_variantkey(self):
        for vkey, chrom, pos, ralen, sizeref, sizealt, csp, cep, ref, alt in testData:
            ochrom, opos, oref, oalt, osizeref, osizealt, oralen = bs.reverse_variantkey(mc, vkey)
            self.assertEqual(ochrom, chrom)
            self.assertEqual(opos, pos)
            self.assertEqual(oref, ref)
            self.assertEqual(oalt, alt)
            self.assertEqual(osizeref, sizeref)
            self.assertEqual(osizealt, sizealt)
            self.assertEqual(oralen, (ralen - 2))

    def test_get_variantkey_ref_length(self):
        for vkey, chrom, pos, ralen, sizeref, sizealt, csp, cep, ref, alt in testData:
            osizeref = bs.get_variantkey_ref_length(mc, vkey)
            self.assertEqual(osizeref, sizeref)

    def test_get_variantkey_ref_length_reversible(self):
        osizeref = bs.get_variantkey_ref_length(mc, 0x1800925199160000)
        self.assertEqual(osizeref, 3)

    def test_get_variantkey_ref_length_not_found(self):
        osizeref = bs.get_variantkey_ref_length(mc, 0xffffffffffffffff)
        self.assertEqual(osizeref, 0)

    def test_get_variantkey_endpos(self):
        for vkey, chrom, pos, ralen, sizeref, sizealt, csp, cep, ref, alt in testData:
            endpos = bs.get_variantkey_endpos(mc, vkey)
            self.assertEqual(endpos, (pos + sizeref))

    def test_get_variantkey_chrom_startpos(self):
        for vkey, chrom, pos, ralen, sizeref, sizealt, csp, cep, ref, alt in testData:
            res = bs.get_variantkey_chrom_startpos(vkey)
            self.assertEqual(res, csp)

    def test_get_variantkey_chrom_endpos(self):
        for vkey, chrom, pos, ralen, sizeref, sizealt, csp, cep, ref, alt in testData:
            res = bs.get_variantkey_chrom_endpos(mc, vkey)
            self.assertEqual(res, cep)

    def test_nrvk_bin_to_tsv(self):
        fsize = bs.nrvk_bin_to_tsv(mc, "nrvk.test")
        self.assertEqual(fsize, 305)


class TestBenchmark(object):

    global setup

    def setup():
        global mf, mc, nrows
        bs.munmap_binfile(mf)
        inputfile = os.path.realpath(
            os.path.dirname(
                os.path.realpath(__file__)) +
            "/../../c/test/data/nrvk.10.bin")
        mf, mc, nrows = bs.mmap_nrvk_file(inputfile)
        if nrows <= 0:
            assert False, "Unable to open the nrvk.10.bin file"

    def test_find_ref_alt_by_variantkey(self, benchmark):
        benchmark.pedantic(
            bs.find_ref_alt_by_variantkey,
            args=[mc, 0xb000c35b64690b25],
            setup=setup,
            iterations=1,
            rounds=10000)

    def test_reverse_variantkey(self, benchmark):
        benchmark.pedantic(
            bs.reverse_variantkey,
            args=[mc, 0xb000c35b64690b25],
            setup=setup,
            iterations=1,
            rounds=10000)

    def test_tearDown(self):
        global mf
        h = bs.munmap_binfile(mf)
        mffd = -1
        mfsize = 0
        if h != 0:
            assert False, "Error while closing the nrvk.10.bin memory-mapped file"
