"""Tests for variantkey64 module."""


import variantkey as bs
import os
import time
from unittest import TestCase


testData = [
    (0, 0X00000001, 0X01, 0X000FDDC2, 0X0A50E0, 0X01000FDDC20A50E0),
    (1, 0X00000007, 0X09, 0X001173EF, 0X038400, 0X09001173EF038400),
    (2, 0X0000000B, 0X09, 0X00130A1C, 0X038400, 0X0900130A1C038400),
    (3, 0X00000061, 0X16, 0X0014A049, 0XCC6140, 0X160014A049CC6140),
    (4, 0X00000065, 0X16, 0X00163676, 0X109C20, 0X1600163676109C20),
    (5, 0X000003E5, 0X16, 0X0017CCA3, 0X13D0E0, 0X160017CCA313D0E0),
    (6, 0X000003F1, 0X19, 0X001962D0, 0X13D0E0, 0X19001962D013D0E0),
    (7, 0X000026F5, 0X19, 0X001AF8FD, 0X0A0400, 0X19001AF8FD0A0400),
    (8, 0X000186A3, 0X19, 0X001C8F2A, 0X038400, 0X19001C8F2A038400),
    (9, 0X00019919, 0X19, 0X001E2557, 0X01D000, 0X19001E255701D000),
]


class TestFunctions(TestCase):

    @classmethod
    def setUpClass(cls):
        global rvsrc, rvfd, rvsize
        inputfile = os.path.realpath(
            os.path.dirname(
                os.path.realpath(__file__)) +
            "/../../test/data/rsid_variantkey64.10.bin")
        rvsrc, rvfd, rvsize = bs.mmap_binfile(inputfile)
        if rvfd < 0 or rvsize != 120:
            assert False, "Unable to open the rsid_variantkey64.10.bin file"
        global vrsrc, vrfd, vrsize
        inputfile = os.path.realpath(
            os.path.dirname(
                os.path.realpath(__file__)) +
            "/../../test/data/variantkey64_rsid.10.bin")
        vrsrc, vrfd, vrsize = bs.mmap_binfile(inputfile)
        if vrfd < 0 or vrsize != 120:
            assert False, "Unable to open the variantkey64_rsid.10.bin file"

    @classmethod
    def tearDownClass(cls):
        global rvsrc, rvfd, rvsize
        h = bs.munmap_binfile(rvsrc, rvfd, rvsize)
        if h != 0:
            assert False, "Error while closing the rsid_variantkey64.10.bin memory-mapped file"
        global vrsrc, vrfd, vrsize
        h = bs.munmap_binfile(vrsrc, vrfd, vrsize)
        if h != 0:
            assert False, "Error while closing the variantkey64_rsid.10.bin memory-mapped file"

    def test_get_vr64_rsid(self):
        for item, rsid, chrom, pos, refalt, vkey in testData:
            xrsid = bs.get_vr64_rsid(vrsrc, item)
            self.assertEqual(xrsid, rsid)

    def test_get_rv64_variantkey(self):
        for item, rsid, chrom, pos, refalt, vkey in testData:
            vk = bs.get_rv64_variantkey(rvsrc, item)
            self.assertEqual(vk, vkey)

    def test_find_rv64_variantkey_by_rsid(self):
        for item, rsid, chrom, pos, refalt, vkey in testData:
            vk, first = bs.find_rv64_variantkey_by_rsid(rvsrc, 0, 9, rsid)
            self.assertEqual(vk, vkey)
            self.assertEqual(first, item)

    def test_find_rv64_variantkey_by_rsid_notfound(self):
        vk, first = bs.find_rv64_variantkey_by_rsid(rvsrc, 0, 9, 0xfffffff0)
        self.assertEqual(vk, 0)
        self.assertEqual(first, 10)

    def test_find_vr64_rsid_by_variantkey(self):
        for item, rsid, chrom, pos, refalt, vkey in testData:
            rx, first = bs.find_vr64_rsid_by_variantkey(vrsrc, 0, 9, vkey)
            self.assertEqual(rx, rsid)
            self.assertEqual(first, item)

    def test_find_vr64_rsid_by_variantkey_notfound(self):
        rx, first = bs.find_vr64_rsid_by_variantkey(vrsrc, 0, 9, 0xfffffffffffffff0)
        self.assertEqual(rx, 0)
        self.assertEqual(first, 10)

    def test_find_vr64_chrom_range(self):
        xrsid, xfirst, xlast = bs.find_vr64_chrom_range(vrsrc, 0, 9, testData[3][2])
        self.assertEqual(xrsid, testData[3][1])
        self.assertEqual(xfirst, 3)
        self.assertEqual(xlast, 5)

    def test_find_vr64_chrom_range_notfound(self):
        xrsid, xfirst, xlast = bs.find_vr64_chrom_range(vrsrc, 0, 8, 0xfffffffe)
        self.assertEqual(xrsid, 0)
        self.assertEqual(xfirst, 9)
        self.assertEqual(xlast, 8)

    def test_find_vr64_pos_range(self):
        xrsid, xfirst, xlast = bs.find_vr64_pos_range(vrsrc, 6, 9, testData[7][3], 0xfffffff0)
        self.assertEqual(xrsid, testData[7][1])
        self.assertEqual(xfirst, 7)
        self.assertEqual(xlast, 9)

    def test_find_vr64_pos_range_notfound(self):
        xrsid, xfirst, xlast = bs.find_vr64_pos_range(vrsrc, 0, 9, 0xffffff00, 0xfffffff0)
        self.assertEqual(xrsid, 0)
        self.assertEqual(xfirst, 10)
        self.assertEqual(xlast, 9)

    def test_find_vr64_chrompos_range(self):
        xrsid, xfirst, xlast = bs.find_vr64_chrompos_range(vrsrc, 0, 9, testData[6][2], testData[7][3], testData[8][3])
        self.assertEqual(xrsid, testData[7][1])
        self.assertEqual(xfirst, 7)
        self.assertEqual(xlast, 8)

    def test_find_vr64_chrompos_range_notfound(self):
        xrsid, xfirst, xlast = bs.find_vr64_chrompos_range(vrsrc, 0, 9, 0xfffffff0, 0xffffff00, 0xfffffff0)
        self.assertEqual(xrsid, 0)
        self.assertEqual(xfirst, 10)
        self.assertEqual(xlast, 9)


class TestBenchmark(object):

    global setup

    def setup():
        global rvsrc, rvfd, rvsize
        if rvfd >= 0:
            pass
        bs.munmap_binfile(rvsrc, rvfd, rvsize)
        inputfile = os.path.realpath(
            os.path.dirname(
                os.path.realpath(__file__)) +
            "/../../test/data/rsid_variantkey64.10.bin")
        rvsrc, rvfd, rvsize = bs.mmap_binfile(inputfile)
        if rvfd < 0 or rvsize != 120:
            assert False, "Unable to open the rsid_variantkey64.10.bin file"
        global vrsrc, vrfd, vrsize
        if vrfd >= 0:
            pass
        bs.munmap_binfile(vrsrc, vrfd, vrsize)
        inputfile = os.path.realpath(
            os.path.dirname(
                os.path.realpath(__file__)) +
            "/../../test/data/variantkey64_rsid.10.bin")
        vrsrc, vrfd, vrsize = bs.mmap_binfile(inputfile)
        if vrfd < 0 or vrsize != 120:
            assert False, "Unable to open the variantkey64_rsid.10.bin file"

    def test_get_vr64_rsid(self, benchmark):
        benchmark.pedantic(
            bs.get_vr64_rsid,
            args=[vrsrc, 3],
            setup=setup,
            iterations=1,
            rounds=10000)

    def test_get_rv64_variantkey(self, benchmark):
        benchmark.pedantic(
            bs.get_rv64_variantkey,
            args=[rvsrc, 3],
            setup=setup,
            iterations=1,
            rounds=10000)

    def test_find_rv64_variantkey_by_rsid(self, benchmark):
        benchmark.pedantic(
            bs.find_rv64_variantkey_by_rsid,
            args=[rvsrc, 0, 9, 0x2F81F575],
            setup=setup,
            iterations=1,
            rounds=10000)

    def test_find_vr64_rsid_by_variantkey(self, benchmark):
        benchmark.pedantic(
            bs.find_vr64_rsid_by_variantkey,
            args=[vrsrc, 0, 9, 0x160017CCA313d0e0],
            setup=setup,
            iterations=1,
            rounds=10000)

    def test_find_vr64_chrompos_range(self, benchmark):
        benchmark.pedantic(
            bs.find_vr64_chrompos_range,
            args=[vrsrc, 0, 9, 0X00000019, 0X001AF8FD, 0X001C8F2A],
            setup=setup,
            iterations=1,
            rounds=10000)

    def test_tearDown(self):
        global rvsrc, rvfd, rvsize
        h = bs.munmap_binfile(rvsrc, rvfd, rvsize)
        rvfd = -1
        rvsize = 0
        if h != 0:
            assert False, "Error while closing the rsid_variantkey64.10.bin memory-mapped file"
        global vrsrc, vrfd, vrsize
        h = bs.munmap_binfile(vrsrc, vrfd, vrsize)
        vrfd = -1
        vrsize = 0
        if h != 0:
            assert False, "Error while closing the variantkey64_rsid.10.bin memory-mapped file"
