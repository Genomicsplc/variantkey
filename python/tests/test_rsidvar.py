"""Tests for libpyvarianthash module."""


import libpyvarianthash as bs
import os
import time
from unittest import TestCase

testDataRV = [
	(0, 0x2F81F572, 0x00000017, 0x0509C4A7, 0xCCD3C1D35DF0CE04),
	(1, 0x2F81F573, 0x00000017, 0x05098728, 0x4EECA287F5C90712),
	(2, 0x2F81F574, 0x0000000B, 0x054CAF51, 0xBB9EE46DF5C4C2E0),
	(3, 0x2F81F575, 0x00000002, 0x095E7925, 0x90C4DEFF765F63B8),
	(4, 0x2F81F576, 0x0000000F, 0x060C1468, 0x0981EF0F500126C2),
	(5, 0x2F81F577, 0x00000011, 0x0199B8D6, 0x22A78FD91ACC7B43),
	(6, 0x2F81F578, 0x0000000A, 0x07D8C4BE, 0x90C4DEFF765F63B8),
	(7, 0x2F81F579, 0x00000001, 0x0DE63E19, 0x453EA229CAA9A652),
	(8, 0x2F81F57A, 0x00000011, 0x026140E3, 0xCCD3C1D35DF0CE04),
	(9, 0x2F81F57B, 0x00000011, 0x02610DFC, 0x26CAA8F5488B8717),
]

testDataVR = [
	(0, 0x0000001A, 0x00004005, 0x26CAA8F5488B8717, 0x0277648B),
	(1, 0x0000001A, 0x00004006, 0x3D9C29BE7B4387A2, 0x020B67DE),
	(2, 0x0000001A, 0x00004006, 0xCCD3C1D35DF0CE04, 0x020B67DE),
	(3, 0x0000001A, 0x0000400E, 0x4EECA287F5C90712, 0x0849000D),
	(4, 0x0000001A, 0x0000403C, 0x0981EF0F500126C2, 0x162BA962),
	(5, 0x0000001A, 0x00004072, 0x4EECA287F5C90712, 0x167638AD),
	(6, 0x0000001A, 0x0000407F, 0x0981EF0F500126C2, 0x1649ED55),
	(7, 0x0000001A, 0x00004086, 0x0981EF0F500126C2, 0x003C1309),
	(8, 0x0000001A, 0x0000408F, 0x22A78FD91ACC7B43, 0x170E8C03),
	(9, 0x0000001A, 0x00004090, 0x0981EF0F500126C2, 0x161885A7),
]


class TestFunctions(TestCase):

    @classmethod
    def setUpClass(cls):
        global rvsrc, rvfd, rvsize
        inputfile = os.path.realpath(
            os.path.dirname(
                os.path.realpath(__file__)) +
            "/../../test/rsid_varhash.10.bin")
        rvsrc, rvfd, rvsize = bs.mmap_binfile(inputfile)
        if rvfd < 0 or rvsize != 200:
            assert False, "Unable to open the rsid_varhash.10.bin file"
        global vrsrc, vrfd, vrsize
        inputfile = os.path.realpath(
            os.path.dirname(
                os.path.realpath(__file__)) +
            "/../../test/varhash_rsid.10.bin")
        vrsrc, vrfd, vrsize = bs.mmap_binfile(inputfile)
        if vrfd < 0 or vrsize != 200:
            assert False, "Unable to open the varhash_rsid.10.bin file"

    @classmethod
    def tearDownClass(cls):
        global rvsrc, rvfd, rvsize
        h = bs.munmap_binfile(rvsrc, rvfd, rvsize)
        if h != 0:
            assert False, "Error while closing the rsid_varhash.10.bin memory-mapped file"
        global vrsrc, vrfd, vrsize
        h = bs.munmap_binfile(vrsrc, vrfd, vrsize)
        if h != 0:
            assert False, "Error while closing the varhash_rsid.10.bin memory-mapped file"

    def test_get_vr_rsid(self):
        for item, chrom, pos, refalt, rsid in testDataVR:
            xrsid = bs.get_vr_rsid(vrsrc, item)
            self.assertEqual(xrsid, rsid)

    def test_get_rv_varhash(self):
        for item, rsid, chrom, pos, refalt in testDataRV:
            xchrom, xpos, xrefalt = bs.get_rv_varhash(rvsrc, item)
            self.assertEqual(xchrom, chrom)
            self.assertEqual(xpos, pos)
            self.assertEqual(xrefalt, refalt)

    def test_find_rv_varhash_by_rsid(self):
        for item, rsid, chrom, pos, refalt in testDataRV:
            xchrom, xpos, xrefalt, xitem = bs.find_rv_varhash_by_rsid(rvsrc, 0, 9, rsid)
            self.assertEqual(xchrom, chrom)
            self.assertEqual(xpos, pos)
            self.assertEqual(xrefalt, refalt)
            self.assertEqual(xitem, item)

    def test_find_rv_varhash_by_rsid_notfound(self):
        xchrom, xpos, xrefalt, xitem = bs.find_rv_varhash_by_rsid(rvsrc, 0, 9, 0xfffffff0)
        self.assertEqual(xchrom, 0)
        self.assertEqual(xpos, 0)
        self.assertEqual(xrefalt, 0)
        self.assertEqual(xitem, 10)

    def test_find_vr_chrompos_range(self):
        xrsid, xfirst, xlast = bs.find_vr_chrompos_range(vrsrc, 0, 9, testDataVR[0][1], testDataVR[3][2], testDataVR[7][2])
        self.assertEqual(xrsid, testDataVR[3][4])
        self.assertEqual(xfirst, 3)
        self.assertEqual(xlast, 7)

    def test_find_vr_chrompos_range_notfound(self):
        xrsid, xfirst, xlast = bs.find_vr_chrompos_range(vrsrc, 0, 9, 0xfffffff0, 0xffffff00, 0xfffffff0)
        self.assertEqual(xrsid, 0)
        self.assertEqual(xfirst, 10)
        self.assertEqual(xlast, 9)

    def test_find_vr_rsid_by_varhash(self):
        for item, chrom, pos, refalt, rsid in testDataVR:
            xrsid, xitem = bs.find_vr_rsid_by_varhash(vrsrc, 0, 9, chrom, pos, refalt)
            self.assertEqual(xrsid, rsid)
            self.assertEqual(xitem, item)

    def test_find_vr_rsid_by_varhash_notfound(self):
        xrsid, xitem = bs.find_vr_rsid_by_varhash(vrsrc, 0, 9, 0xfffffff0, 0xfffffff0, 0xfffffffffffffff0)
        self.assertEqual(xrsid, 0)
        self.assertEqual(xitem, 10)


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
            "/../../test/rsid_varhash.10.bin")
        rvsrc, rvfd, rvsize = bs.mmap_binfile(inputfile)
        if rvfd < 0 or rvsize != 200:
            assert False, "Unable to open the rsid_varhash.10.bin file"
        global vrsrc, vrfd, vrsize
        if vrfd >= 0:
            pass
        bs.munmap_binfile(vrsrc, vrfd, vrsize)
        inputfile = os.path.realpath(
            os.path.dirname(
                os.path.realpath(__file__)) +
            "/../../test/varhash_rsid.10.bin")
        vrsrc, vrfd, vrsize = bs.mmap_binfile(inputfile)
        if vrfd < 0 or vrsize != 200:
            assert False, "Unable to open the varhash_rsid.10.bin file"

    def test_get_vr_rsid(self, benchmark):
        benchmark.pedantic(
            bs.get_vr_rsid,
            args=[
                vrsrc,
                3],
            setup=setup,
            iterations=1,
            rounds=10000)

    def test_get_rv_varhash(self, benchmark):
        benchmark.pedantic(
            bs.get_rv_varhash,
            args=[
                rvsrc,
                3],
            setup=setup,
            iterations=1,
            rounds=10000)

    def test_find_rv_varhash_by_rsid(self, benchmark):
        benchmark.pedantic(
            bs.find_rv_varhash_by_rsid,
            args=[
                rvsrc,
                0,
                9,
                0x2F81F575],
            setup=setup,
            iterations=1,
            rounds=10000)

    def test_find_vr_chrompos_range(self, benchmark):
        benchmark.pedantic(
            bs.find_vr_chrompos_range,
            args=[
                vrsrc,
                0,
                9,
                0x0000001A,
                0x0000400E,
                0x00004086],
            setup=setup,
            iterations=1,
            rounds=10000)

    def test_find_vr_rsid_by_varhash(self, benchmark):
        benchmark.pedantic(
            bs.find_vr_rsid_by_varhash,
            args=[
                vrsrc,
                0,
                9,
                0x0000001A,
                0x0000400E,
                0x4EECA287F5C90712],
            setup=setup,
            iterations=1,
            rounds=10000)

    def test_tearDown(self):
        global rvsrc, rvfd, rvsize
        h = bs.munmap_binfile(rvsrc, rvfd, rvsize)
        rvfd = -1
        rvsize = 0
        if h != 0:
            assert False, "Error while closing the rsid_varhash.10.bin memory-mapped file"
        global vrsrc, vrfd, vrsize
        h = bs.munmap_binfile(vrsrc, vrfd, vrsize)
        vrfd = -1
        vrsize = 0
        if h != 0:
            assert False, "Error while closing the varhash_rsid.10.bin memory-mapped file"
