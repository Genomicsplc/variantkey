"""Tests for variantkey genoref module."""

# test_genoref.py
# @category   Libraries
# @author     Nicola Asuni <nicola.asuni@genomicsplc.com>
# @copyright  2017-2018 GENOMICS plc
# @license    MIT (see LICENSE)
# @link       https://github.com/genomicsplc/variantkey

import variantkey as bs
import os
import time
from unittest import TestCase


class TestFunctions(TestCase):

    @classmethod
    def setUpClass(cls):
        global mf
        inputfile = os.path.realpath(
            os.path.dirname(
                os.path.realpath(__file__)) +
            "/../../c/test/data/genoref.bin")
        mf, mfsize = bs.mmap_genoref_file(inputfile)
        if mfsize <= 0:
            assert False, "Unable to open the genoref.bin file"

    @classmethod
    def tearDownClass(cls):
        global mf
        h = bs.munmap_binfile(mf)
        if h != 0:
            assert False, "Error while closing the genoref.bin memory-mapped file"

    def test_get_genoref_seq(self):
        for chrom in range(1, 26):
            ref = bs.get_genoref_seq(mf, chrom, 0)  # first
            self.assertEqual(b'A', ref)
            ref = bs.get_genoref_seq(mf, chrom, (26 - chrom))  # last
            self.assertEqual(chr(ord(b'Z') + 1 - chrom).encode('ascii'), ref)
            ref = bs.get_genoref_seq(mf, chrom, (27 - chrom))  # invalid
            self.assertNotEqual(0, ref)

    def test_check_reference(self):
        # exp, chrom, pos, sizeref, ref
        tdata = [
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
        ]
        for exp, chrom, pos, sizeref, ref in tdata:
            ret = bs.check_reference(mf, chrom, pos, ref)
            self.assertEqual(exp, ret)

        def test_flip_allele(self):
            allele = b"ATCGMKRYBVDHWSNatcgmkrybvdhwsn"
            expected = b"TAGCKMYRVBHDWSNTAGCKMYRVBHDWSN"
            ret = bs.flip_allele(allele)
            self.assertEqual(expected, ret)

        def test_normalize_variant(self):
            # ecode, chrom, pos, epos, sizeref, sizealt, esizeref, esizealt, eref, ealt, ref, alt
            tdata = [
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
            ]
            for ecode, chrom, pos, epos, sizeref, sizealt, esizeref, esizealt, eref, ealt, ref, alt in tdata:
                ncode, npos, nref, nalt, nsizeref, nsizealt = bs.normalize_variant(mf, chrom, pos, ref, alt)
                self.assertEqual(ecode, ncode)
                self.assertEqual(epos, npos)
                self.assertEqual(eref, nref)
                self.assertEqual(ealt, nalt)
                self.assertEqual(esizeref, nsizeref)
                self.assertEqual(esizealt, nsizealt)


class TestBenchmark(object):

    global setup

    def setup():
        global mf
        bs.munmap_binfile(mf)
        inputfile = os.path.realpath(
            os.path.dirname(
                os.path.realpath(__file__)) +
            "/../../c/test/data/genoref.bin")
        mf, mfsize = bs.mmap_genoref_file(inputfile)
        if mfsize <= 0:
            assert False, "Unable to open the genoref.bin file"

    def test_get_genoref_seq(self, benchmark):
        benchmark.pedantic(
            bs.get_genoref_seq,
            args=[mf, 13, 1],
            setup=setup,
            iterations=1,
            rounds=10000)

    def test_tearDown(self):
        global mf
        h = bs.munmap_binfile(mf)
        if h != 0:
            assert False, "Error while closing the genoref.bin memory-mapped file"
