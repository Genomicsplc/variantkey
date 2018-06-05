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
