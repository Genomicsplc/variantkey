"""Tests for variantkey nrvk module."""


import variantkey as bs
import os
import time
from unittest import TestCase


class TestFunctions(TestCase):

    @classmethod
    def setUpClass(cls):
        global mfsrc, mffd, mfsize, mflast, idx
        inputfile = os.path.realpath(
            os.path.dirname(
                os.path.realpath(__file__)) +
            "/../../c/test/data/genoref.bin")
        mfsrc, mffd, mfsize, mflast = bs.mmap_binfile(inputfile)
        if mffd < 0:
            assert False, "Unable to open the genoref.bin file"
        idx = bs.load_genoref_index(mfsrc)

    @classmethod
    def tearDownClass(cls):
        global mfsrc, mffd, mfsize
        h = bs.munmap_binfile(mfsrc, mffd, mfsize)
        if h != 0:
            assert False, "Error while closing the genoref.bin memory-mapped file"

    def test_get_genoref_seq(self):
        for chrom in range(1, 26):
            ref = bs.get_genoref_seq(mfsrc, idx, chrom, 0)  # first
            self.assertEqual(b'A', ref)
            ref = bs.get_genoref_seq(mfsrc, idx, chrom, (26 - chrom))  # last
            self.assertEqual(chr(ord(b'Z') + 1 - chrom).encode('ascii'), ref)
            ref = bs.get_genoref_seq(mfsrc, idx, chrom, (27 - chrom))  # invalid
            self.assertNotEqual(0, ref)

    def test_check_reference(self):
        tdata = [
            (1, 0, "A", 1, 0),
            (1, 25, "Z", 1, 0),
            (25, 0, "A", 1, 0),
            (25, 1, "B", 1, 0),
            (2, 0, "ABCDEFGHIJKLmnopqrstuvwxy", 25, 0),
            (1, 26, "ZABC", 4, -2),
            (1, 0, "ABCDEFGHIJKLmnopqrstuvwxyJ", 26, -1),
            (14, 2, "ZZZ", 3, -1),
            (1, 0, "N", 1, 1),
            (10, 13, "A", 1, 1),
            (1, 3, "B", 1, 1),
            (1, 1, "C", 1, 1),
            (1, 0, "D", 1, 1),
            (1, 3, "A", 1, 1),
            (1, 0, "H", 1, 1),
            (1, 7, "A", 1, 1),
            (1, 0, "V", 1, 1),
            (1, 21, "A", 1, 1),
            (1, 0, "W", 1, 1),
            (1, 19, "W", 1, 1),
            (1, 22, "A", 1, 1),
            (1, 22, "T", 1, 1),
            (1, 2, "S", 1, 1),
            (1, 6, "S", 1, 1),
            (1, 18, "C", 1, 1),
            (1, 18, "G", 1, 1),
            (1, 0, "M", 1, 1),
            (1, 2, "M", 1, 1),
            (1, 12, "A", 1, 1),
            (1, 12, "C", 1, 1),
            (1, 6, "K", 1, 1),
            (1, 19, "K", 1, 1),
            (1, 10, "G", 1, 1),
            (1, 10, "T", 1, 1),
            (1, 0, "R", 1, 1),
            (1, 6, "R", 1, 1),
            (1, 17, "A", 1, 1),
            (1, 17, "G", 1, 1),
            (1, 2, "Y", 1, 1),
            (1, 19, "Y", 1, 1),
            (1, 24, "C", 1, 1),
            (1, 24, "T", 1, 1),
        ]
        for chrom, pos, ref, sizeref, exp in tdata:
            ret = bs.check_reference(mfsrc, idx, chrom, pos, ref)
            self.assertEqual(exp, ret)

        def test_flip_allele(self):
            allele = b"ATCGMKRYBVDHWSNatcgmkrybvdhwsn"
            expected = b"TAGCKMYRVBHDWSNTAGCKMYRVBHDWSN"
            ret = bs.flip_allele(allele)
            self.assertEqual(expected, ret)

        def test_normalize_variant(self):
            tdata = [
                (1, 26, "A", 1, "C", 1, 26, "A", 1, "C", 1, -2),
                (1, 0, "J", 1, "C", 1, 0, "J", 1, "C", 1, -1),
                (1, 0, "T", 1, "G", 1, 0, "A", 1, "C", 1, 2),
                (1, 0, "A", 1, "C", 1, 0, "A", 1, "C", 1, 0),
                (13, 2, "CDE", 3, "CD", 2, 3, "DE", 2, "D", 1, 16),
                (13, 2, "CDE", 3, "CFE", 3, 3, "D", 1, "F", 1, 24),
                (1, 0, "aBCDEF", 6, "aBKDEF", 6, 2, "C", 1, "K", 1, 24),
                (1, 0, "A", 1, "", 0, 0, "A", 1, "", 0, 0),
                (1, 3, "D", 1, "", 0, 2, "CD", 2, "C", 1, 4),
                (1, 24, "Y", 1, "CK", 2, 24, "Y", 1, "CK", 2, 0),
                (1, 0, "G", 1, "A", 1, 0, "A", 1, "G", 1, 4),
                (1, 0, "G", 1, "T", 1, 0, "A", 1, "C", 1, 6),
            ]
            for chrom, pos, ref, sizeref, alt, sizealt, epos, eref, esizeref, ealt, esizealt, ecode in tdata:
                ncode, npos, nref, nalt, nsizeref, nsizealt = bs.normalize_variant(mfsrc, idx, chrom, pos, ref, alt)
                self.assertEqual(ecode, ncode)
                self.assertEqual(epos, npos)
                self.assertEqual(eref, nref)
                self.assertEqual(ealt, nalt)
                self.assertEqual(esizeref, nsizeref)
                self.assertEqual(esizealt, nsizealt)


class TestBenchmark(object):

    global setup

    def setup():
        global mfsrc, mffd, mfsize, mflast, idx
        if mffd >= 0:
            pass
        bs.munmap_binfile(mfsrc, mffd, mfsize)
        inputfile = os.path.realpath(
            os.path.dirname(
                os.path.realpath(__file__)) +
            "/../../c/test/data/genoref.bin")
        mfsrc, mffd, mfsize, mflast = bs.mmap_binfile(inputfile)
        if mffd < 0:
            assert False, "Unable to open the genoref.bin file"
        idx = bs.load_genoref_index(mfsrc)

    def test_get_genoref_seq(self, benchmark):
        benchmark.pedantic(
            bs.get_genoref_seq,
            args=[mfsrc, idx, 13, 1],
            setup=setup,
            iterations=1,
            rounds=10000)

    def test_tearDown(self):
        global mfsrc, mffd, mfsize
        h = bs.munmap_binfile(mfsrc, mffd, mfsize)
        mffd = -1
        mfsize = 0
        if h != 0:
            assert False, "Error while closing the genoref.bin memory-mapped file"
