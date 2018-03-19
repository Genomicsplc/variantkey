"""Tests for libpyvariantkey module."""


import libpyvariantkey as bs
import os
import time
from unittest import TestCase

testDataRV = [
	(0, 0x00000001, 0x8b29d2c7, 0x00000005, 0x00006edf, 0x387351cb),
	(1, 0x00000002, 0x8b29d2c7, 0x00000007, 0x0022e9c0, 0xd0b64ba3),
	(2, 0x00000003, 0x8b29d2c7, 0x00000004, 0x0000ec56, 0xcef1000f),
	(3, 0x00000004, 0x8b29d2c7, 0x00000008, 0x00028f54, 0x8a59f7ae),
	(4, 0x00000005, 0x8b29d2c7, 0x00000003, 0x000124a3, 0x4c61400f),
	(5, 0x00000006, 0x8b29d2c7, 0x00000009, 0x000143fc, 0x6bddcdbc),
	(6, 0x00000007, 0x8b29d2c7, 0x00000002, 0x00006d6d, 0x181d293a),
	(7, 0x00000008, 0x8b29d2c7, 0x0000000a, 0x00019015, 0x387351cb),
	(8, 0x00000009, 0x8b29d2c7, 0x00000001, 0x0004f442, 0x387351cb),
	(9, 0x0000000a, 0x8b29d2c7, 0x0000000b, 0x00032edc, 0x145dc65c),
]

testDataVR = [
	(0, 0x8b29d2c7, 0x00000001, 0x0004f442, 0x387351cb, 0x00000009),
	(1, 0x8b29d2c7, 0x00000002, 0x00006d6d, 0x181d293a, 0x00000007),
	(2, 0x8b29d2c7, 0x00000003, 0x000124a3, 0x4c61400f, 0x00000005),
	(3, 0x8b29d2c7, 0x00000004, 0x0000ec56, 0xcef1000f, 0x00000003),
	(4, 0x8b29d2c7, 0x00000005, 0x00006edf, 0x387351cb, 0x00000001),
	(5, 0x8b29d2c7, 0x00000007, 0x0022e9c0, 0xd0b64ba3, 0x00000002),
	(6, 0x8b29d2c7, 0x00000008, 0x00028f54, 0x8a59f7ae, 0x00000004),
	(7, 0x8b29d2c7, 0x00000009, 0x000143fc, 0x6bddcdbc, 0x00000006),
	(8, 0x8b29d2c7, 0x0000000a, 0x00019015, 0x387351cb, 0x00000008),
	(9, 0x8b29d2c7, 0x0000000b, 0x00032edc, 0x145dc65c, 0x0000000a),
]


class TestFunctions(TestCase):

    @classmethod
    def setUpClass(cls):
        global rvsrc, rvfd, rvsize
        inputfile = os.path.realpath(
            os.path.dirname(
                os.path.realpath(__file__)) +
            "/../../test/rsid_variantkey.10.bin")
        rvsrc, rvfd, rvsize = bs.mmap_binfile(inputfile)
        if rvfd < 0 or rvsize != 200:
            assert False, "Unable to open the rsid_variantkey.10.bin file"
        global vrsrc, vrfd, vrsize
        inputfile = os.path.realpath(
            os.path.dirname(
                os.path.realpath(__file__)) +
            "/../../test/variantkey_rsid.10.bin")
        vrsrc, vrfd, vrsize = bs.mmap_binfile(inputfile)
        if vrfd < 0 or vrsize != 200:
            assert False, "Unable to open the variantkey_rsid.10.bin file"

    @classmethod
    def tearDownClass(cls):
        global rvsrc, rvfd, rvsize
        h = bs.munmap_binfile(rvsrc, rvfd, rvsize)
        if h != 0:
            assert False, "Error while closing the rsid_variantkey.10.bin memory-mapped file"
        global vrsrc, vrfd, vrsize
        h = bs.munmap_binfile(vrsrc, vrfd, vrsize)
        if h != 0:
            assert False, "Error while closing the variantkey_rsid.10.bin memory-mapped file"

    def test_get_vr_rsid(self):
        for item, assembly, chrom, pos, refalt, rsid in testDataVR:
            xrsid = bs.get_vr_rsid(vrsrc, item)
            self.assertEqual(xrsid, rsid)

    def test_get_rv_variantkey(self):
        for item, rsid, assembly, chrom, pos, refalt in testDataRV:
            xassembly, xchrom, xpos, xrefalt = bs.get_rv_variantkey(rvsrc, item)
            self.assertEqual(xassembly, assembly)
            self.assertEqual(xchrom, chrom)
            self.assertEqual(xpos, pos)
            self.assertEqual(xrefalt, refalt)

    def test_find_rv_variantkey_by_rsid(self):
        for item, rsid, assembly, chrom, pos, refalt in testDataRV:
            xassembly, xchrom, xpos, xrefalt, xitem = bs.find_rv_variantkey_by_rsid(rvsrc, 0, 9, rsid)
            self.assertEqual(xassembly, assembly)
            self.assertEqual(xchrom, chrom)
            self.assertEqual(xpos, pos)
            self.assertEqual(xrefalt, refalt)
            self.assertEqual(xitem, item)

    def test_find_rv_variantkey_by_rsid_notfound(self):
        xassembly, xchrom, xpos, xrefalt, xitem = bs.find_rv_variantkey_by_rsid(rvsrc, 0, 9, 0xfffffff0)
        self.assertEqual(xassembly, 0)
        self.assertEqual(xchrom, 0)
        self.assertEqual(xpos, 0)
        self.assertEqual(xrefalt, 0)
        self.assertEqual(xitem, 10)

    def test_find_vr_rsid_by_variantkey(self):
        for item, assembly, chrom, pos, refalt, rsid in testDataVR:
            xrsid, xitem = bs.find_vr_rsid_by_variantkey(vrsrc, 0, 9, assembly, chrom, pos, refalt)
            self.assertEqual(xrsid, rsid)
            self.assertEqual(xitem, item)

    def test_find_vr_rsid_by_variantkey_notfound(self):
        xrsid, xitem = bs.find_vr_rsid_by_variantkey(vrsrc, 0, 9, 0xfffffff0, 0xfffffff0, 0xfffffff0, 0xfffffff0)
        self.assertEqual(xrsid, 0)
        self.assertEqual(xitem, 10)

    def test_find_vr_chrompos_range(self):
        xrsid, xfirst, xlast = bs.find_vr_chrompos_range(vrsrc, 0, 9, testDataVR[4][2], testDataVR[4][3], testDataVR[4][3])
        self.assertEqual(xrsid, testDataVR[4][5])
        self.assertEqual(xfirst, 4)
        self.assertEqual(xlast, 4)

    def test_find_vr_chrompos_range_notfound(self):
        xrsid, xfirst, xlast = bs.find_vr_chrompos_range(vrsrc, 0, 9, 0xfffffff0, 0xffffff00, 0xfffffff0)
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
            "/../../test/rsid_variantkey.10.bin")
        rvsrc, rvfd, rvsize = bs.mmap_binfile(inputfile)
        if rvfd < 0 or rvsize != 200:
            assert False, "Unable to open the rsid_variantkey.10.bin file"
        global vrsrc, vrfd, vrsize
        if vrfd >= 0:
            pass
        bs.munmap_binfile(vrsrc, vrfd, vrsize)
        inputfile = os.path.realpath(
            os.path.dirname(
                os.path.realpath(__file__)) +
            "/../../test/variantkey_rsid.10.bin")
        vrsrc, vrfd, vrsize = bs.mmap_binfile(inputfile)
        if vrfd < 0 or vrsize != 200:
            assert False, "Unable to open the variantkey_rsid.10.bin file"

    def test_get_vr_rsid(self, benchmark):
        benchmark.pedantic(
            bs.get_vr_rsid,
            args=[vrsrc, 3],
            setup=setup,
            iterations=1,
            rounds=10000)

    def test_get_rv_variantkey(self, benchmark):
        benchmark.pedantic(
            bs.get_rv_variantkey,
            args=[rvsrc, 3],
            setup=setup,
            iterations=1,
            rounds=10000)

    def test_find_rv_variantkey_by_rsid(self, benchmark):
        benchmark.pedantic(
            bs.find_rv_variantkey_by_rsid,
            args=[rvsrc, 0, 9, 0x387351cb],
            setup=setup,
            iterations=1,
            rounds=10000)

    def test_find_vr_rsid_by_variantkey(self, benchmark):
        benchmark.pedantic(
            bs.find_vr_rsid_by_variantkey,
            args=[vrsrc, 0, 9, 0x8b29d2c7, 0x00000003, 0x000124a3, 0x8ffb1a03],
            setup=setup,
            iterations=1,
            rounds=10000)

    def test_find_vr_chrompos_range(self, benchmark):
        benchmark.pedantic(
            bs.find_vr_chrompos_range,
            args=[vrsrc, 0, 9, 0x00000005, 0x00006f88, 0x00006ed7],
            setup=setup,
            iterations=1,
            rounds=10000)

    def test_tearDown(self):
        global rvsrc, rvfd, rvsize
        h = bs.munmap_binfile(rvsrc, rvfd, rvsize)
        rvfd = -1
        rvsize = 0
        if h != 0:
            assert False, "Error while closing the rsid_variantkey.10.bin memory-mapped file"
        global vrsrc, vrfd, vrsize
        h = bs.munmap_binfile(vrsrc, vrfd, vrsize)
        vrfd = -1
        vrsize = 0
        if h != 0:
            assert False, "Error while closing the variantkey_rsid.10.bin memory-mapped file"
