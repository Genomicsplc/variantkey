"""Tests for variantkey128 module."""


import variantkey as bs
import os
import time
from unittest import TestCase


testData = [
    (0, 0X00000001, 0X8B29D2C7, 0X00000001, 0X000FDDC2, 0X014A1C00),
    (1, 0X00000007, 0X8B29D2C7, 0X00000009, 0X001173EF, 0X00708000),
    (2, 0X0000000B, 0X8B29D2C7, 0X00000009, 0X00130A1C, 0X00708000),
    (3, 0X00000061, 0X8B29D2C7, 0X00000016, 0X0014A049, 0X0413D021),
    (4, 0X00000065, 0X8B29D2C7, 0X00000016, 0X00163676, 0X02138400),
    (5, 0X000003E5, 0X8B29D2C7, 0X00000016, 0X0017CCA3, 0X027A1C00),
    (6, 0X000003F1, 0X8B29D2C7, 0X00000019, 0X001962D0, 0X027A1C00),
    (7, 0X000026F5, 0X8B29D2C7, 0X00000019, 0X001AF8FD, 0X01408000),
    (8, 0X000186A3, 0X8B29D2C7, 0X00000019, 0X001C8F2A, 0X00708000),
    (9, 0X00019919, 0X8B29D2C7, 0X00000019, 0X001E2557, 0X003A0000),
]


class TestFunctions(TestCase):

    @classmethod
    def setUpClass(cls):
        global rvsrc, rvfd, rvsize
        inputfile = os.path.realpath(
            os.path.dirname(
                os.path.realpath(__file__)) +
            "/../../test/data/rsid_variantkey128.10.bin")
        rvsrc, rvfd, rvsize = bs.mmap_binfile(inputfile)
        if rvfd < 0 or rvsize != 200:
            assert False, "Unable to open the rsid_variantkey128.10.bin file"
        global vrsrc, vrfd, vrsize
        inputfile = os.path.realpath(
            os.path.dirname(
                os.path.realpath(__file__)) +
            "/../../test/data/variantkey128_rsid.10.bin")
        vrsrc, vrfd, vrsize = bs.mmap_binfile(inputfile)
        if vrfd < 0 or vrsize != 200:
            assert False, "Unable to open the variantkey128_rsid.10.bin file"

    @classmethod
    def tearDownClass(cls):
        global rvsrc, rvfd, rvsize
        h = bs.munmap_binfile(rvsrc, rvfd, rvsize)
        if h != 0:
            assert False, "Error while closing the rsid_variantkey128.10.bin memory-mapped file"
        global vrsrc, vrfd, vrsize
        h = bs.munmap_binfile(vrsrc, vrfd, vrsize)
        if h != 0:
            assert False, "Error while closing the variantkey128_rsid.10.bin memory-mapped file"

    def test_get_vr128_rsid(self):
        for item, rsid, assembly, chrom, pos, refalt in testData:
            xrsid = bs.get_vr128_rsid(vrsrc, item)
            self.assertEqual(xrsid, rsid)

    def test_get_rv128_variantkey(self):
        for item, rsid, assembly, chrom, pos, refalt in testData:
            xassembly, xchrom, xpos, xrefalt = bs.get_rv128_variantkey(rvsrc, item)
            self.assertEqual(xassembly, assembly)
            self.assertEqual(xchrom, chrom)
            self.assertEqual(xpos, pos)
            self.assertEqual(xrefalt, refalt)

    def test_find_rv128_variantkey_by_rsid(self):
        for item, rsid, assembly, chrom, pos, refalt in testData:
            xassembly, xchrom, xpos, xrefalt, xitem = bs.find_rv128_variantkey_by_rsid(rvsrc, 0, 9, rsid)
            self.assertEqual(xassembly, assembly)
            self.assertEqual(xchrom, chrom)
            self.assertEqual(xpos, pos)
            self.assertEqual(xrefalt, refalt)
            self.assertEqual(xitem, item)

    def test_find_rv128_variantkey_by_rsid_notfound(self):
        xassembly, xchrom, xpos, xrefalt, xitem = bs.find_rv128_variantkey_by_rsid(rvsrc, 0, 9, 0xfffffff0)
        self.assertEqual(xassembly, 0)
        self.assertEqual(xchrom, 0)
        self.assertEqual(xpos, 0)
        self.assertEqual(xrefalt, 0)
        self.assertEqual(xitem, 10)

    def test_find_vr128_rsid_by_variantkey(self):
        for item, rsid, assembly, chrom, pos, refalt in testData:
            xrsid, xitem = bs.find_vr128_rsid_by_variantkey(vrsrc, 0, 9, assembly, chrom, pos, refalt)
            self.assertEqual(xrsid, rsid)
            self.assertEqual(xitem, item)

    def test_find_vr128_rsid_by_variantkey_notfound(self):
        xrsid, xitem = bs.find_vr128_rsid_by_variantkey(vrsrc, 0, 9, 0xfffffff0, 0xfffffff0, 0xfffffff0, 0xfffffff0)
        self.assertEqual(xrsid, 0)
        self.assertEqual(xitem, 10)

    def test_find_vr128_chrom_range(self):
        xrsid, xfirst, xlast = bs.find_vr128_chrom_range(vrsrc, 0, 9, testData[3][3])
        self.assertEqual(xrsid, testData[3][1])
        self.assertEqual(xfirst, 3)
        self.assertEqual(xlast, 5)

    def test_find_vr128_chrom_range_notfound(self):
        xrsid, xfirst, xlast = bs.find_vr128_chrom_range(vrsrc, 0, 8, 0xfffffffe)
        self.assertEqual(xrsid, 0)
        self.assertEqual(xfirst, 9)
        self.assertEqual(xlast, 8)

    def test_find_vr128_pos_range(self):
        xrsid, xfirst, xlast = bs.find_vr128_pos_range(vrsrc, 6, 9, testData[7][4], 0xfffffff0)
        self.assertEqual(xrsid, testData[7][1])
        self.assertEqual(xfirst, 7)
        self.assertEqual(xlast, 9)

    def test_find_vr128_pos_range_notfound(self):
        xrsid, xfirst, xlast = bs.find_vr128_pos_range(vrsrc, 0, 9, 0xffffff00, 0xfffffff0)
        self.assertEqual(xrsid, 0)
        self.assertEqual(xfirst, 10)
        self.assertEqual(xlast, 9)

    def test_find_vr128_chrompos_range(self):
        xrsid, xfirst, xlast = bs.find_vr128_chrompos_range(vrsrc, 0, 9, testData[6][3], testData[7][4], testData[8][4])
        self.assertEqual(xrsid, testData[7][1])
        self.assertEqual(xfirst, 7)
        self.assertEqual(xlast, 8)

    def test_find_vr128_chrompos_range_notfound(self):
        xrsid, xfirst, xlast = bs.find_vr128_chrompos_range(vrsrc, 0, 9, 0xfffffff0, 0xffffff00, 0xfffffff0)
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
            "/../../test/data/rsid_variantkey128.10.bin")
        rvsrc, rvfd, rvsize = bs.mmap_binfile(inputfile)
        if rvfd < 0 or rvsize != 200:
            assert False, "Unable to open the rsid_variantkey128.10.bin file"
        global vrsrc, vrfd, vrsize
        if vrfd >= 0:
            pass
        bs.munmap_binfile(vrsrc, vrfd, vrsize)
        inputfile = os.path.realpath(
            os.path.dirname(
                os.path.realpath(__file__)) +
            "/../../test/data/variantkey128_rsid.10.bin")
        vrsrc, vrfd, vrsize = bs.mmap_binfile(inputfile)
        if vrfd < 0 or vrsize != 200:
            assert False, "Unable to open the variantkey128_rsid.10.bin file"

    def test_get_vr128_rsid(self, benchmark):
        benchmark.pedantic(
            bs.get_vr128_rsid,
            args=[vrsrc, 3],
            setup=setup,
            iterations=1,
            rounds=10000)

    def test_get_rv128_variantkey(self, benchmark):
        benchmark.pedantic(
            bs.get_rv128_variantkey,
            args=[rvsrc, 3],
            setup=setup,
            iterations=1,
            rounds=10000)

    def test_find_rv128_variantkey_by_rsid(self, benchmark):
        benchmark.pedantic(
            bs.find_rv128_variantkey_by_rsid,
            args=[rvsrc, 0, 9, 0x2F81F575],
            setup=setup,
            iterations=1,
            rounds=10000)

    def test_find_vr128_rsid_by_variantkey(self, benchmark):
        benchmark.pedantic(
            bs.find_vr128_rsid_by_variantkey,
            args=[vrsrc, 0, 9, 0X8B29D2C7, 0X00000019, 0X001AF8FD, 0X01408000],
            setup=setup,
            iterations=1,
            rounds=10000)

    def test_find_vr128_chrompos_range(self, benchmark):
        benchmark.pedantic(
            bs.find_vr128_chrompos_range,
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
            assert False, "Error while closing the rsid_variantkey128.10.bin memory-mapped file"
        global vrsrc, vrfd, vrsize
        h = bs.munmap_binfile(vrsrc, vrfd, vrsize)
        vrfd = -1
        vrsize = 0
        if h != 0:
            assert False, "Error while closing the variantkey128_rsid.10.bin memory-mapped file"
