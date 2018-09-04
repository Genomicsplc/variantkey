"""Tests for binsearch module - file formats."""


import variantkey as bs
import os
from unittest import TestCase


class TestFunctions(TestCase):

    def test_load_arrow(self):
        inputfile = os.path.realpath(
            os.path.dirname(
                os.path.realpath(__file__)) +
            "/../../c/test/data/test_data_arrow.bin")
        src, fd, size, doffset, dlength, nrows, ncols, index = bs.mmap_binfile(inputfile, [4, 8])
        if fd < 0 or size != 730:
            assert False, "Unable to open the file"
        self.assertEqual(doffset, 376)
        self.assertEqual(dlength, 136)
        self.assertEqual(nrows, 11)
        self.assertEqual(ncols, 2)
        self.assertEqual(index[0], 376)
        self.assertEqual(index[1], 424)
        h = bs.munmap_binfile(src, fd, size)
        if h != 0:
            assert False, "Error while closing the memory-mapped file"

    def test_load_feather(self):
        inputfile = os.path.realpath(
            os.path.dirname(
                os.path.realpath(__file__)) +
            "/../../c/test/data/test_data_feather.bin")
        src, fd, size, doffset, dlength, nrows, ncols, index = bs.mmap_binfile(inputfile, [4, 8])
        if fd < 0 or size != 384:
            assert False, "Unable to open the file"
        self.assertEqual(doffset, 8)
        self.assertEqual(dlength, 136)
        self.assertEqual(nrows, 11)
        self.assertEqual(ncols, 2)
        self.assertEqual(index[0], 8)
        self.assertEqual(index[1], 56)
        h = bs.munmap_binfile(src, fd, size)
        if h != 0:
            assert False, "Error while closing the memory-mapped file"

    def test_load_binsrc(self):
        inputfile = os.path.realpath(
            os.path.dirname(
                os.path.realpath(__file__)) +
            "/../../c/test/data/test_data_binsrc.bin")
        src, fd, size, doffset, dlength, nrows, ncols, index = bs.mmap_binfile(inputfile, [4, 8])
        if fd < 0 or size != 176:
            assert False, "Unable to open the file"
        self.assertEqual(doffset, 40)
        self.assertEqual(dlength, 136)
        self.assertEqual(nrows, 11)
        self.assertEqual(ncols, 2)
        self.assertEqual(index[0], 40)
        self.assertEqual(index[1], 88)
        h = bs.munmap_binfile(src, fd, size)
        if h != 0:
            assert False, "Error while closing the memory-mapped file"
