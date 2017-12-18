"""Tests for libpybinsearch module."""


import libpybinsearch as bs
import os
import time
from unittest import TestCase

testData8 = [
    (6, 0, 99, 0x27, 0, 0, 1, 18, 19, 18),
    (6, 0, 99, 0x33, 87, 87, 86, 99, 100, 99),
    (0, 0, 99, 0x00, 0, 0, 1, 99, 100, 99),
    (6, 0, 99, 0xff, 100, 100, 99, 100, 100, 99),
    (6, 0, 99, 0x27, 0, 0, 1, 18, 19, 18),
    (6, 13, 99, 0x27, 13, 13, 12, 18, 19, 18),
    (6, 14, 99, 0x27, 14, 14, 13, 18, 19, 18),
    (6, 0, 0, 0x01, 1, 0, 0, 1, 0, 0),
    (6, 0, 0, 0xf0, 1, 1, 0, 1, 1, 0),
    (6, 99, 99, 0x01, 100, 99, 98, 100, 99, 98),
    (6, 99, 99, 0xf0, 100, 100, 99, 100, 100, 99),
    (6, 0, 99, 0x28, 19, 19, 18, 44, 45, 44),
    (6, 0, 99, 0x2A, 57, 57, 56, 61, 62, 61),
]

testData16 = [
    (6, 0, 99, 0x2722, 0, 0, 1, 0, 1, 0),
    (6, 0, 99, 0x33f5, 99, 99, 98, 99, 100, 99),
    (0, 0, 99, 0x0001, 100, 100, 99, 100, 100, 99),
    (6, 0, 99, 0xf7f3, 100, 100, 99, 100, 100, 99),
    (6, 0, 99, 0x27f3, 13, 13, 12, 14, 15, 14),
    (6, 13, 99, 0x27f3, 13, 13, 12, 14, 15, 14),
    (6, 14, 99, 0x27f3, 14, 14, 13, 14, 15, 14),
    (6, 0, 0, 0x0001, 1, 0, 0, 1, 0, 0),
    (6, 0, 0, 0xfff0, 1, 1, 0, 1, 1, 0),
    (6, 99, 99, 0x0001, 100, 99, 98, 100, 99, 98),
    (6, 99, 99, 0xfff0, 100, 100, 99, 100, 100, 99),
    (6, 0, 99, 0x2805, 100, 19, 18, 100, 19, 18),
    (6, 0, 99, 0x28ff, 100, 45, 44, 100, 45, 44),
]
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

testData128 = [
    (4, 0, 99, 0x000027225fb6e591, 0x6eb7abd92e3deb1d, 0, 0, 1, 0, 1, 0),
    (4, 0, 99, 0x000027c07b9621ec, 0x01f886390c06811d, 10, 10, 9, 10, 11, 10),
    (4, 0, 0, 0x000027225fb6e591, 0x6eb7abd92e3deb1a, 1, 0, 0, 1, 0, 0),
    (4, 0, 99, 0x000027c30981ef0f, 0x500126c20c059eb5, 12, 12, 11, 12, 13, 12),
    (4, 0, 99, 0x000027f35fb6e591, 0x6eb7abd90889e85e, 13, 13, 12, 13, 14, 13),
    (4, 0, 99, 0x000027f3d41a0ce2, 0xdf116bbc0bf2cf80, 100, 14, 13, 100, 14, 13),
    (4, 0, 99, 0x000027f53b9e3036, 0x5103b7a62e3fbbcc, 100, 16, 15, 100, 16, 15),
    (4, 0, 99, 0x000027f690c4deff, 0x765f63b80bf00517, 16, 16, 15, 16, 17, 16),
    (4, 0, 99, 0x000033f522a78fd9, 0x1acc7b430ac5ca22, 99, 99, 98, 99, 100, 99),
    (4, 0, 0, 0x0000000000000001, 0x0000000000000001, 1, 0, 0, 1, 0, 0),
    (4, 0, 0, 0xfffffffffffffff0, 0xfffffffffffffff0, 1, 1, 0, 1, 1, 0),
    (4, 0, 99, 0x000028060981ef0f, 0x500126c22f813253, 100, 19, 18, 100, 19, 18),
    (4, 0, 99, 0x000028fca24c9148, 0x830a986a0be5c095, 100, 45, 44, 100, 45, 44),
]


class TestFunctions(TestCase):

    @classmethod
    def setUpClass(cls):
        global src, fd, size
        inputfile = os.path.realpath(
            os.path.dirname(
                os.path.realpath(__file__)) +
            "/../../test/test_data.bin")
        src, fd, size = bs.mmap_binfile(inputfile)
        if fd < 0 or size != 2000:
            assert False, "Unable to open the file"

    @classmethod
    def tearDownClass(cls):
        global src, fd, size
        h = bs.munmap_binfile(src, fd, size)
        if h != 0:
            assert False, "Error while closing the memory-mapped file"

    def test_find_first_uint8(self):
        for blkpos, first, last, search, fF, fFF, fFL, fL, fLF, fLL in testData8:
            rp, rf, rl = bs.find_first_uint8(
                src, 20, blkpos, first, last, search)
            self.assertEqual(rp, fF)
            self.assertEqual(rf, fFF)
            self.assertEqual(rl, fFL)

    def test_find_last_uint8(self):
        for blkpos, first, last, search, fF, fFF, fFL, fL, fLF, fLL in testData8:
            rp, rf, rl = bs.find_last_uint8(
                src, 20, blkpos, first, last, search)
            self.assertEqual(rp, fL)
            self.assertEqual(rf, fLF)
            self.assertEqual(rl, fLL)

    def test_find_first_uint16(self):
        for blkpos, first, last, search, fF, fFF, fFL, fL, fLF, fLL in testData16:
            rp, rf, rl = bs.find_first_uint16(
                src, 20, blkpos, first, last, search)
            self.assertEqual(rp, fF)
            self.assertEqual(rf, fFF)
            self.assertEqual(rl, fFL)

    def test_find_last_uint16(self):
        for blkpos, first, last, search, fF, fFF, fFL, fL, fLF, fLL in testData16:
            rp, rf, rl = bs.find_last_uint16(
                src, 20, blkpos, first, last, search)
            self.assertEqual(rp, fL)
            self.assertEqual(rf, fLF)
            self.assertEqual(rl, fLL)

    def test_find_first_uint32(self):
        for blkpos, first, last, search, fF, fFF, fFL, fL, fLF, fLL in testData32:
            rp, rf, rl = bs.find_first_uint32(
                src, 20, blkpos, first, last, search)
            self.assertEqual(rp, fF)
            self.assertEqual(rf, fFF)
            self.assertEqual(rl, fFL)

    def test_find_last_uint32(self):
        for blkpos, first, last, search, fF, fFF, fFL, fL, fLF, fLL in testData32:
            rp, rf, rl = bs.find_last_uint32(
                src, 20, blkpos, first, last, search)
            self.assertEqual(rp, fL)
            self.assertEqual(rf, fLF)
            self.assertEqual(rl, fLL)

    def test_find_first_uint64(self):
        for blkpos, first, last, search, fF, fFF, fFL, fL, fLF, fLL in testData64:
            rp, rf, rl = bs.find_first_uint64(
                src, 20, blkpos, first, last, search)
            self.assertEqual(rp, fF)
            self.assertEqual(rf, fFF)
            self.assertEqual(rl, fFL)

    def test_find_last_uint64(self):
        for blkpos, first, last, search, fF, fFF, fFL, fL, fLF, fLL in testData64:
            rp, rf, rl = bs.find_last_uint64(
                src, 20, blkpos, first, last, search)
            self.assertEqual(rp, fL)
            self.assertEqual(rf, fLF)
            self.assertEqual(rl, fLL)

    def test_find_first_uint128(self):
        for blkpos, first, last, searchHi, searchLo, fF, fFF, fFL, fL, fLF, fLL in testData128:
            rp, rf, rl = bs.find_first_uint128(
                src, 20, blkpos, first, last, searchHi, searchLo)
            self.assertEqual(rp, fF)
            self.assertEqual(rf, fFF)
            self.assertEqual(rl, fFL)

    def test_find_last_uint128(self):
        for blkpos, first, last, searchHi, searchLo, fF, fFF, fFL, fL, fLF, fLL in testData128:
            rp, rf, rl = bs.find_last_uint128(
                src, 20, blkpos, first, last, searchHi, searchLo)
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
            "/../../test/test_data.bin")
        src, fd, size = bs.mmap_binfile(inputfile)
        if fd < 0 or size != 2000:
            assert False, "Unable to open the file"

    def test_find_first_uint8_benchmark(self, benchmark):
        benchmark.pedantic(
            bs.find_first_uint8,
            args=[src, 20, 6, 0, 99, 0x27],
            setup=setup,
            iterations=1,
            rounds=10000)

    def test_find_last_uint8_benchmark(self, benchmark):
        benchmark.pedantic(
            bs.find_last_uint8,
            args=[src, 20, 6, 0, 99, 0x27],
            setup=setup,
            iterations=1,
            rounds=10000)

    def test_find_first_uint16_benchmark(self, benchmark):
        benchmark.pedantic(
            bs.find_first_uint16,
            args=[src, 20, 6, 0, 99, 0x27f3],
            setup=setup,
            iterations=1,
            rounds=10000)

    def test_find_last_uint16_benchmark(self, benchmark):
        benchmark.pedantic(
            bs.find_last_uint16,
            args=[src, 20, 6, 0, 99, 0x27f3],
            setup=setup,
            iterations=1,
            rounds=10000)

    def test_find_first_uint32_benchmark(self, benchmark):
        benchmark.pedantic(
            bs.find_first_uint32,
            args=[src, 20, 4, 0, 99, 0x000027f3],
            setup=setup,
            iterations=1,
            rounds=10000)

    def test_find_last_uint32_benchmark(self, benchmark):
        benchmark.pedantic(
            bs.find_last_uint32,
            args=[src, 20, 4, 0, 99, 0x000027f3],
            setup=setup,
            iterations=1,
            rounds=10000)

    def test_find_first_uint64_benchmark(self, benchmark):
        benchmark.pedantic(
            bs.find_first_uint64,
            args=[src, 20, 4, 0, 99, 0x000027f35fb6e591],
            setup=setup,
            iterations=1,
            rounds=10000)

    def test_find_last_uint64_benchmark(self, benchmark):
        benchmark.pedantic(
            bs.find_last_uint64,
            args=[src, 20, 4, 0, 99, 0x000027f35fb6e591],
            setup=setup,
            iterations=1,
            rounds=10000)

    def test_find_first_uint128_benchmark(self, benchmark):
        benchmark.pedantic(
            bs.find_first_uint128,
            args=[src, 20, 4, 0, 99, 0x000027f35fb6e591, 0x6eb7abd90889e85e],
            setup=setup,
            iterations=1,
            rounds=10000)

    def test_find_last_uint128_benchmark(self, benchmark):
        benchmark.pedantic(
            bs.find_last_uint128,
            args=[src, 20, 4, 0, 99, 0x000027f35fb6e591, 0x6eb7abd90889e85e],
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
