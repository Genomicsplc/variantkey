"""Tests for binsearch module."""


import variantkey as bs
import os
import time
from unittest import TestCase

testData32 = [
    (4, 0, 99, 0x00002722, 0, 0, 1, 0, 1, 0),
    (4, 0, 99, 0x000033f5, 99, 99, 98, 99, 100, 99),
    (0, 0, 99, 0x00000001, 0, 0, 1, 99, 100, 99),
    (4, 0, 99, 0xf00027f3, 100, 100, 99, 100, 100, 99),
    (4, 0, 99, 0x000027f3, 13, 13, 12, 14, 15, 14),
    (4, 13, 99, 0x000027f3, 13, 13, 12, 14, 15, 14),
    (4, 14, 99, 0x000027f3, 14, 14, 13, 14, 15, 14),
    (4, 0, 0, 0x00000001, 1, 0, 0, 1, 0, 0),
    (4, 0, 0, 0xfffffff0, 1, 1, 0, 1, 1, 0),
    (4, 99, 99, 0x00000001, 100, 99, 98, 100, 99, 98),
    (4, 99, 99, 0xfffffff0, 100, 100, 99, 100, 100, 99),
    (4, 0, 99, 0x00002805, 100, 19, 18, 100, 19, 18),
    (4, 0, 99, 0x000028ff, 100, 45, 44, 100, 45, 44),
]

testData64 = [
    (4, 0, 99, 0x000027225fb6e591, 0, 0, 1, 0, 1, 0),
    (4, 0, 99, 0x000033f522a78fd9, 99, 99, 98, 99, 100, 99),
    (0, 0, 99, 0x0000000100002722, 0, 0, 1, 0, 1, 0),
    (4, 0, 99, 0xf0000001f00027f3, 100, 100, 99, 100, 100, 99),
    (4, 0, 99, 0x000027f35fb6e591, 13, 13, 12, 13, 14, 13),
    (0, 13, 99, 0x00000001000027f3, 13, 13, 12, 14, 15, 14),
    (0, 14, 99, 0x00000001000027f3, 14, 14, 13, 14, 15, 14),
    (4, 0, 0, 0x0000000000000001, 1, 0, 0, 1, 0, 0),
    (4, 0, 0, 0xfffffffffffffff0, 1, 1, 0, 1, 1, 0),
    (4, 99, 99, 0x0000000000000001, 100, 99, 98, 100, 99, 98),
    (4, 99, 99, 0xfffffffffffffff0, 100, 100, 99, 100, 100, 99),
    (4, 0, 99, 0x000028060981ef0a, 100, 19, 18, 100, 19, 18),
    (4, 0, 99, 0x000028fca24c9149, 100, 45, 44, 100, 45, 44),
]

testDataSub32 = [
    (4, 0, 99, 0x4e4, 0, 0, 1, 0, 1, 0),
    (4, 0, 99, 0x67e, 99, 99, 98, 99, 100, 99),
    (0, 0, 99, 0x0, 0, 0, 1, 99, 100, 99),
    (4, 0, 99, 0x60004fe, 100, 100, 99, 100, 100, 99),
    (4, 0, 99, 0x4fe, 13, 13, 12, 16, 17, 16),
    (4, 13, 99, 0x4fe, 13, 13, 12, 16, 17, 16),
    (4, 14, 99, 0x4fe, 14, 14, 13, 16, 17, 16),
    (4, 0, 0, 0x0, 1, 0, 0, 1, 0, 0),
    (4, 0, 0, 0x7fffffe, 1, 1, 0, 1, 1, 0),
    (4, 99, 99, 0x0, 100, 99, 98, 100, 99, 98),
    (4, 99, 99, 0x7fffffe, 100, 100, 99, 100, 100, 99),
    (4, 0, 99, 0x500, 19, 19, 18, 20, 21, 20),
    (4, 0, 99, 0x51f, 41, 41, 40, 44, 45, 44),
]

testDataSub64 = [
    (4, 0, 99, 0x4e44bf6dcb2, 0, 0, 1, 0, 1, 0),
    (4, 0, 99, 0x67ea454f1fb, 99, 99, 98, 99, 100, 99),
    (0, 0, 99, 0x200004e4, 0, 0, 1, 0, 1, 0),
    (4, 0, 99, 0x60000003e0004fe, 100, 100, 99, 100, 100, 99),
    (4, 0, 99, 0x4fe6bf6dcb2, 13, 13, 12, 13, 14, 13),
    (0, 13, 99, 0x200004fe, 13, 13, 12, 16, 17, 16),
    (0, 14, 99, 0x200004fe, 14, 14, 13, 16, 17, 16),
    (4, 0, 0, 0x0, 1, 0, 0, 1, 0, 0),
    (4, 0, 0, 0x7fffffffffffffe, 1, 1, 0, 1, 1, 0),
    (4, 99, 99, 0x0, 100, 99, 98, 100, 99, 98),
    (4, 99, 99, 0x7fffffffffffffe, 100, 100, 99, 100, 100, 99),
    (4, 0, 99, 0x500c1303de1, 19, 19, 18, 19, 20, 19),
    (4, 0, 99, 0x51f94499229, 44, 44, 43, 44, 45, 44),
]


class TestFunctions(TestCase):

    @classmethod
    def setUpClass(cls):
        global src, fd, size
        inputfile = os.path.realpath(
            os.path.dirname(
                os.path.realpath(__file__)) +
            "/../../c/test/data/test_data.bin")
        src, fd, size, _ = bs.mmap_binfile(inputfile)
        if fd < 0 or size != 2000:
            assert False, "Unable to open the file"

    @classmethod
    def tearDownClass(cls):
        global src, fd, size
        h = bs.munmap_binfile(src, fd, size)
        if h != 0:
            assert False, "Error while closing the memory-mapped file"

    def test_find_first_uint32(self):
        for blkpos, first, last, search, fF, fFF, fFL, fL, fLF, fLL in testData32:
            rp, rf, rl = bs.find_first_uint32(
                src, 20, blkpos, 0, 31, first, last, search)
            self.assertEqual(rp, fF)
            self.assertEqual(rf, fFF)
            self.assertEqual(rl, fFL)

    def test_find_last_uint32(self):
        for blkpos, first, last, search, fF, fFF, fFL, fL, fLF, fLL in testData32:
            rp, rf, rl = bs.find_last_uint32(
                src, 20, blkpos, 0, 31, first, last, search)
            self.assertEqual(rp, fL)
            self.assertEqual(rf, fLF)
            self.assertEqual(rl, fLL)

    def test_find_first_uint64(self):
        for blkpos, first, last, search, fF, fFF, fFL, fL, fLF, fLL in testData64:
            rp, rf, rl = bs.find_first_uint64(
                src, 20, blkpos, 0, 63, first, last, search)
            self.assertEqual(rp, fF)
            self.assertEqual(rf, fFF)
            self.assertEqual(rl, fFL)

    def test_find_last_uint64(self):
        for blkpos, first, last, search, fF, fFF, fFL, fL, fLF, fLL in testData64:
            rp, rf, rl = bs.find_last_uint64(
                src, 20, blkpos, 0, 63, first, last, search)
            self.assertEqual(rp, fL)
            self.assertEqual(rf, fLF)
            self.assertEqual(rl, fLL)

    def test_find_first_uint32_sub(self):
        for blkpos, first, last, search, fF, fFF, fFL, fL, fLF, fLL in testDataSub32:
            rp, rf, rl = bs.find_first_uint32(
                src, 20, blkpos, 2, 28, first, last, search)
            self.assertEqual(rp, fF)
            self.assertEqual(rf, fFF)
            self.assertEqual(rl, fFL)

    def test_find_last_uint32_sub(self):
        for blkpos, first, last, search, fF, fFF, fFL, fL, fLF, fLL in testDataSub32:
            rp, rf, rl = bs.find_last_uint32(
                src, 20, blkpos, 2, 28, first, last, search)
            self.assertEqual(rp, fL)
            self.assertEqual(rf, fLF)
            self.assertEqual(rl, fLL)

    def test_find_first_uint64_sub(self):
        for blkpos, first, last, search, fF, fFF, fFL, fL, fLF, fLL in testDataSub64:
            rp, rf, rl = bs.find_first_uint64(
                src, 20, blkpos, 2, 60, first, last, search)
            self.assertEqual(rp, fF)
            self.assertEqual(rf, fFF)
            self.assertEqual(rl, fFL)

    def test_find_last_uint64_sub(self):
        for blkpos, first, last, search, fF, fFF, fFL, fL, fLF, fLL in testDataSub64:
            rp, rf, rl = bs.find_last_uint64(
                src, 20, blkpos, 2, 60, first, last, search)
            self.assertEqual(rp, fL)
            self.assertEqual(rf, fLF)
            self.assertEqual(rl, fLL)


class TestBenchmark(object):

    global setup

    def setup():
        global src, fd, size
        if fd >= 0:
            pass
        bs.munmap_binfile(src, fd, size)
        inputfile = os.path.realpath(
            os.path.dirname(
                os.path.realpath(__file__)) +
            "/../../c/test/data/test_data.bin")
        src, fd, size, _ = bs.mmap_binfile(inputfile)
        if fd < 0 or size != 2000:
            assert False, "Unable to open the file"

    def test_find_first_uint32_benchmark(self, benchmark):
        benchmark.pedantic(
            bs.find_first_uint32,
            args=[src, 20, 4, 0, 31, 0, 99, 0x000027f3],
            setup=setup,
            iterations=1,
            rounds=10000)

    def test_find_last_uint32_benchmark(self, benchmark):
        benchmark.pedantic(
            bs.find_last_uint32,
            args=[src, 20, 4, 0, 31, 0, 99, 0x000027f3],
            setup=setup,
            iterations=1,
            rounds=10000)

    def test_find_first_uint64_benchmark(self, benchmark):
        benchmark.pedantic(
            bs.find_first_uint64,
            args=[src, 20, 4, 0, 63, 0, 99, 0x000027f35fb6e591],
            setup=setup,
            iterations=1,
            rounds=10000)

    def test_find_last_uint64_benchmark(self, benchmark):
        benchmark.pedantic(
            bs.find_last_uint64,
            args=[src, 20, 4, 0, 63, 0, 99, 0x000027f35fb6e591],
            setup=setup,
            iterations=1,
            rounds=10000)

    def test_tearDown(self):
        global src, fd, size
        h = bs.munmap_binfile(src, fd, size)
        fd = -1
        size = 0
        if h != 0:
            assert False, "Error while closing the memory-mapped file"
