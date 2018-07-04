"""Tests for binsearch module."""


import variantkey as bs
import os
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

testDataSub8 = [
    (6, 0, 99, 0x4, 0, 0, 1, 18, 19, 18),
    (6, 0, 99, 0x6, 80, 80, 79, 99, 100, 99),
    (0, 0, 99, 0x0, 0, 0, 1, 99, 100, 99),
    (6, 0, 99, 0x1f, 100, 100, 99, 100, 100, 99),
    (6, 0, 99, 0x4, 0, 0, 1, 18, 19, 18),
    (6, 13, 99, 0x4, 13, 13, 12, 18, 19, 18),
    (6, 14, 99, 0x4, 14, 14, 13, 18, 19, 18),
    (6, 0, 0, 0x0, 1, 0, 0, 1, 0, 0),
    (6, 0, 0, 0x1e, 1, 1, 0, 1, 1, 0),
    (6, 99, 99, 0x0, 100, 99, 98, 100, 99, 98),
    (6, 99, 99, 0x1e, 100, 100, 99, 100, 100, 99),
    (6, 0, 99, 0x5, 19, 19, 18, 79, 80, 79),
    (6, 0, 99, 0x5, 19, 19, 18, 79, 80, 79),
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

testDataSub16 = [
    (6, 0, 99, 0x4e4, 0, 0, 1, 0, 1, 0),
    (6, 0, 99, 0x67e, 99, 99, 98, 99, 100, 99),
    (0, 0, 99, 0x0, 0, 0, 1, 99, 100, 99),
    (6, 0, 99, 0x1efe, 100, 100, 99, 100, 100, 99),
    (6, 0, 99, 0x4fe, 13, 13, 12, 16, 17, 16),
    (6, 13, 99, 0x4fe, 13, 13, 12, 16, 17, 16),
    (6, 14, 99, 0x4fe, 14, 14, 13, 16, 17, 16),
    (6, 0, 0, 0x0, 1, 0, 0, 1, 0, 0),
    (6, 0, 0, 0x1ffe, 1, 1, 0, 1, 1, 0),
    (6, 99, 99, 0x0, 100, 99, 98, 100, 99, 98),
    (6, 99, 99, 0x1ffe, 100, 100, 99, 100, 100, 99),
    (6, 0, 99, 0x500, 19, 19, 18, 20, 21, 20),
    (6, 0, 99, 0x51f, 41, 41, 40, 44, 45, 44),
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
        global src, fd, size, last
        inputfile = os.path.realpath(
            os.path.dirname(
                os.path.realpath(__file__)) +
            "/../../c/test/data/test_data.bin")
        src, fd, size, last = bs.mmap_binfile(inputfile)
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
            rp, rf, rl = bs.find_first_uint8(src, 20, blkpos, first, last, search)
            self.assertEqual(rp, fF)
            self.assertEqual(rf, fFF)
            self.assertEqual(rl, fFL)
            numitems = fL - fF + 1
            if (rp <= last) and (numitems > 0):
                pos = rp
                ret = True
                counter = 0
                while ret:
                    ret, pos = bs.has_next_uint8(src, 20, blkpos, pos, last, search)
                    counter = counter + 1
                self.assertEqual(counter, numitems)

    def test_find_first_uint16(self):
        for blkpos, first, last, search, fF, fFF, fFL, fL, fLF, fLL in testData16:
            rp, rf, rl = bs.find_first_uint16(src, 20, blkpos, first, last, search)
            self.assertEqual(rp, fF)
            self.assertEqual(rf, fFF)
            self.assertEqual(rl, fFL)
            numitems = fL - fF + 1
            if (rp <= last) and (numitems > 0):
                pos = rp
                ret = True
                counter = 0
                while ret:
                    ret, pos = bs.has_next_uint16(src, 20, blkpos, pos, last, search)
                    counter = counter + 1
                self.assertEqual(counter, numitems)

    def test_find_first_uint32(self):
        for blkpos, first, last, search, fF, fFF, fFL, fL, fLF, fLL in testData32:
            rp, rf, rl = bs.find_first_uint32(src, 20, blkpos, first, last, search)
            self.assertEqual(rp, fF)
            self.assertEqual(rf, fFF)
            self.assertEqual(rl, fFL)
            numitems = fL - fF + 1
            if (rp <= last) and (numitems > 0):
                pos = rp
                ret = True
                counter = 0
                while ret:
                    ret, pos = bs.has_next_uint32(src, 20, blkpos, pos, last, search)
                    counter = counter + 1
                self.assertEqual(counter, numitems)

    def test_find_first_uint64(self):
        for blkpos, first, last, search, fF, fFF, fFL, fL, fLF, fLL in testData64:
            rp, rf, rl = bs.find_first_uint64(src, 20, blkpos, first, last, search)
            self.assertEqual(rp, fF)
            self.assertEqual(rf, fFF)
            self.assertEqual(rl, fFL)
            numitems = fL - fF + 1
            if (rp <= last) and (numitems > 0):
                pos = rp
                ret = True
                counter = 0
                while ret:
                    ret, pos = bs.has_next_uint64(src, 20, blkpos, pos, last, search)
                    counter = counter + 1
                self.assertEqual(counter, numitems)

    def test_find_last_uint8(self):
        for blkpos, first, last, search, fF, fFF, fFL, fL, fLF, fLL in testData8:
            rp, rf, rl = bs.find_last_uint8(src, 20, blkpos, first, last, search)
            self.assertEqual(rp, fL)
            self.assertEqual(rf, fLF)
            self.assertEqual(rl, fLL)
            numitems = fL - fF + 1
            if (rp <= last) and (numitems > 0):
                pos = rp
                ret = True
                counter = 0
                while ret:
                    ret, pos = bs.has_prev_uint8(src, 20, blkpos, first, pos, search)
                    counter = counter + 1
                self.assertEqual(counter, numitems)

    def test_find_last_uint16(self):
        for blkpos, first, last, search, fF, fFF, fFL, fL, fLF, fLL in testData16:
            rp, rf, rl = bs.find_last_uint16(src, 20, blkpos, first, last, search)
            self.assertEqual(rp, fL)
            self.assertEqual(rf, fLF)
            self.assertEqual(rl, fLL)
            numitems = fL - fF + 1
            if (rp <= last) and (numitems > 0):
                pos = rp
                ret = True
                counter = 0
                while ret:
                    ret, pos = bs.has_prev_uint16(src, 20, blkpos, first, pos, search)
                    counter = counter + 1
                self.assertEqual(counter, numitems)

    def test_find_last_uint32(self):
        for blkpos, first, last, search, fF, fFF, fFL, fL, fLF, fLL in testData32:
            rp, rf, rl = bs.find_last_uint32(src, 20, blkpos, first, last, search)
            self.assertEqual(rp, fL)
            self.assertEqual(rf, fLF)
            self.assertEqual(rl, fLL)
            numitems = fL - fF + 1
            if (rp <= last) and (numitems > 0):
                pos = rp
                ret = True
                counter = 0
                while ret:
                    ret, pos = bs.has_prev_uint32(src, 20, blkpos, first, pos, search)
                    counter = counter + 1
                self.assertEqual(counter, numitems)

    def test_find_last_uint64(self):
        for blkpos, first, last, search, fF, fFF, fFL, fL, fLF, fLL in testData64:
            rp, rf, rl = bs.find_last_uint64(src, 20, blkpos, first, last, search)
            self.assertEqual(rp, fL)
            self.assertEqual(rf, fLF)
            self.assertEqual(rl, fLL)
            numitems = fL - fF + 1
            if (rp <= last) and (numitems > 0):
                pos = rp
                ret = True
                counter = 0
                while ret:
                    ret, pos = bs.has_prev_uint64(src, 20, blkpos, first, pos, search)
                    counter = counter + 1
                self.assertEqual(counter, numitems)

    def test_find_first_sub_uint8(self):
        for blkpos, first, last, search, fF, fFF, fFL, fL, fLF, fLL in testDataSub8:
            rp, rf, rl = bs.find_first_sub_uint8(
                src, 20, blkpos, 2, 4, first, last, search)
            self.assertEqual(rp, fF)
            self.assertEqual(rf, fFF)
            self.assertEqual(rl, fFL)
            numitems = fL - fF + 1
            if (rp <= last) and (numitems > 0):
                pos = rp
                ret = True
                counter = 0
                while ret:
                    ret, pos = bs.has_next_sub_uint8(src, 20, blkpos, 2, 4, pos, last, search)
                    counter = counter + 1
                self.assertEqual(counter, numitems)

    def test_find_first_sub_uint16(self):
        for blkpos, first, last, search, fF, fFF, fFL, fL, fLF, fLL in testDataSub16:
            rp, rf, rl = bs.find_first_sub_uint16(
                src, 20, blkpos, 2, 12, first, last, search)
            self.assertEqual(rp, fF)
            self.assertEqual(rf, fFF)
            self.assertEqual(rl, fFL)
            numitems = fL - fF + 1
            if (rp <= last) and (numitems > 0):
                pos = rp
                ret = True
                counter = 0
                while ret:
                    ret, pos = bs.has_next_sub_uint16(src, 20, blkpos, 2, 12, pos, last, search)
                    counter = counter + 1
                self.assertEqual(counter, numitems)

    def test_find_first_sub_uint32(self):
        for blkpos, first, last, search, fF, fFF, fFL, fL, fLF, fLL in testDataSub32:
            rp, rf, rl = bs.find_first_sub_uint32(
                src, 20, blkpos, 2, 28, first, last, search)
            self.assertEqual(rp, fF)
            self.assertEqual(rf, fFF)
            self.assertEqual(rl, fFL)
            numitems = fL - fF + 1
            if (rp <= last) and (numitems > 0):
                pos = rp
                ret = True
                counter = 0
                while ret:
                    ret, pos = bs.has_next_sub_uint32(src, 20, blkpos, 2, 28, pos, last, search)
                    counter = counter + 1
                self.assertEqual(counter, numitems)

    def test_find_first_sub_uint64(self):
        for blkpos, first, last, search, fF, fFF, fFL, fL, fLF, fLL in testDataSub64:
            rp, rf, rl = bs.find_first_sub_uint64(
                src, 20, blkpos, 2, 60, first, last, search)
            self.assertEqual(rp, fF)
            self.assertEqual(rf, fFF)
            self.assertEqual(rl, fFL)
            numitems = fL - fF + 1
            if (rp <= last) and (numitems > 0):
                pos = rp
                ret = True
                counter = 0
                while ret:
                    ret, pos = bs.has_next_sub_uint64(src, 20, blkpos, 2, 60, pos, last, search)
                    counter = counter + 1
                self.assertEqual(counter, numitems)

    def test_find_last_sub_uint8(self):
        for blkpos, first, last, search, fF, fFF, fFL, fL, fLF, fLL in testDataSub8:
            rp, rf, rl = bs.find_last_sub_uint8(
                src, 20, blkpos, 2, 4, first, last, search)
            self.assertEqual(rp, fL)
            self.assertEqual(rf, fLF)
            self.assertEqual(rl, fLL)
            numitems = fL - fF + 1
            if (rp <= last) and (numitems > 0):
                pos = rp
                ret = True
                counter = 0
                while ret:
                    ret, pos = bs.has_prev_sub_uint8(src, 20, blkpos, 2, 4, first, pos, search)
                    counter = counter + 1
                self.assertEqual(counter, numitems)

    def test_find_last_sub_uint16(self):
        for blkpos, first, last, search, fF, fFF, fFL, fL, fLF, fLL in testDataSub16:
            rp, rf, rl = bs.find_last_sub_uint16(
                src, 20, blkpos, 2, 12, first, last, search)
            self.assertEqual(rp, fL)
            self.assertEqual(rf, fLF)
            self.assertEqual(rl, fLL)
            numitems = fL - fF + 1
            if (rp <= last) and (numitems > 0):
                pos = rp
                ret = True
                counter = 0
                while ret:
                    ret, pos = bs.has_prev_sub_uint16(src, 20, blkpos, 2, 12, first, pos, search)
                    counter = counter + 1
                self.assertEqual(counter, numitems)

    def test_find_last_sub_uint32(self):
        for blkpos, first, last, search, fF, fFF, fFL, fL, fLF, fLL in testDataSub32:
            rp, rf, rl = bs.find_last_sub_uint32(
                src, 20, blkpos, 2, 28, first, last, search)
            self.assertEqual(rp, fL)
            self.assertEqual(rf, fLF)
            self.assertEqual(rl, fLL)
            numitems = fL - fF + 1
            if (rp <= last) and (numitems > 0):
                pos = rp
                ret = True
                counter = 0
                while ret:
                    ret, pos = bs.has_prev_sub_uint32(src, 20, blkpos, 2, 28, first, pos, search)
                    counter = counter + 1
                self.assertEqual(counter, numitems)

    def test_find_last_sub_uint64(self):
        for blkpos, first, last, search, fF, fFF, fFL, fL, fLF, fLL in testDataSub64:
            rp, rf, rl = bs.find_last_sub_uint64(
                src, 20, blkpos, 2, 60, first, last, search)
            self.assertEqual(rp, fL)
            self.assertEqual(rf, fLF)
            self.assertEqual(rl, fLL)
            numitems = fL - fF + 1
            if (rp <= last) and (numitems > 0):
                pos = rp
                ret = True
                counter = 0
                while ret:
                    ret, pos = bs.has_prev_sub_uint64(src, 20, blkpos, 2, 60, first, pos, search)
                    counter = counter + 1
                self.assertEqual(counter, numitems)


class TestBenchmark(object):

    global setup

    def setup():
        global src, fd, size, last
        if fd >= 0:
            pass
        bs.munmap_binfile(src, fd, size)
        inputfile = os.path.realpath(
            os.path.dirname(
                os.path.realpath(__file__)) +
            "/../../c/test/data/test_data.bin")
        src, fd, size, last = bs.mmap_binfile(inputfile)
        if fd < 0 or size != 2000:
            assert False, "Unable to open the file"

    def test_find_first_uint8_benchmark(self, benchmark):
        benchmark.pedantic(
            bs.find_first_uint8,
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

    def test_find_first_uint32_benchmark(self, benchmark):
        benchmark.pedantic(
            bs.find_first_uint32,
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

    def test_find_last_uint8_benchmark(self, benchmark):
        benchmark.pedantic(
            bs.find_last_uint8,
            args=[src, 20, 6, 0, 99, 0x27],
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

    def test_find_last_uint32_benchmark(self, benchmark):
        benchmark.pedantic(
            bs.find_last_uint32,
            args=[src, 20, 4, 0, 99, 0x000027f3],
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

    def test_find_first_sub_uint8_benchmark(self, benchmark):
        benchmark.pedantic(
            bs.find_first_sub_uint8,
            args=[src, 20, 6, 0, 7, 0, 99, 0x27],
            setup=setup,
            iterations=1,
            rounds=10000)

    def test_find_first_sub_uint16_benchmark(self, benchmark):
        benchmark.pedantic(
            bs.find_first_sub_uint16,
            args=[src, 20, 6, 0, 15, 0, 99, 0x27f3],
            setup=setup,
            iterations=1,
            rounds=10000)

    def test_find_first_sub_uint32_benchmark(self, benchmark):
        benchmark.pedantic(
            bs.find_first_sub_uint32,
            args=[src, 20, 4, 0, 31, 0, 99, 0x000027f3],
            setup=setup,
            iterations=1,
            rounds=10000)

    def test_find_first_sub_uint64_benchmark(self, benchmark):
        benchmark.pedantic(
            bs.find_first_sub_uint64,
            args=[src, 20, 4, 0, 63, 0, 99, 0x000027f35fb6e591],
            setup=setup,
            iterations=1,
            rounds=10000)

    def test_find_last_sub_uint8_benchmark(self, benchmark):
        benchmark.pedantic(
            bs.find_last_sub_uint8,
            args=[src, 20, 6, 0, 7, 0, 99, 0x27],
            setup=setup,
            iterations=1,
            rounds=10000)

    def test_find_last_sub_uint16_benchmark(self, benchmark):
        benchmark.pedantic(
            bs.find_last_sub_uint16,
            args=[src, 20, 6, 0, 15, 0, 99, 0x27f3],
            setup=setup,
            iterations=1,
            rounds=10000)

    def test_find_last_sub_uint32_benchmark(self, benchmark):
        benchmark.pedantic(
            bs.find_last_sub_uint32,
            args=[src, 20, 4, 0, 31, 0, 99, 0x000027f3],
            setup=setup,
            iterations=1,
            rounds=10000)

    def test_find_last_sub_uint64_benchmark(self, benchmark):
        benchmark.pedantic(
            bs.find_last_sub_uint64,
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
