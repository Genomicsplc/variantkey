"""Tests for variantkey rsidvar module."""

# test_rsidvar.py
# @category   Libraries
# @author     Nicola Asuni <nicola.asuni@genomicsplc.com>
# @copyright  2017-2018 GENOMICS plc
# @license    MIT (see LICENSE)
# @link       https://github.com/genomicsplc/variantkey

import variantkey as bs
import os
import time
from unittest import TestCase


testData = [
    (0, 0X00000001, 0X08027A2580338000, 0X01, 0X0004F44B, 0X00338000),
    (1, 0X00000007, 0X4800A1FE439E3918, 0X09, 0X000143FC, 0X439E3918),
    (2, 0X0000000B, 0X4800A1FE7555EB16, 0X09, 0X000143FC, 0X7555EB16),
    (3, 0X00000061, 0X80010274003A0000, 0X10, 0X000204E8, 0X003A0000),
    (4, 0X00000065, 0X8001028D00138000, 0X10, 0X0002051A, 0X00138000),
    (5, 0X000003E5, 0X80010299007A0000, 0X10, 0X00020532, 0X007A0000),
    (6, 0X000003F1, 0XA0012B62003A0000, 0X14, 0X000256C4, 0X003A0000),
    (7, 0X000026F5, 0XA0012B6280708000, 0X14, 0X000256C5, 0X00708000),
    (8, 0X000186A3, 0XA0012B65E3256692, 0X14, 0X000256CB, 0X63256692),
    (9, 0X00019919, 0XA0012B67D5439803, 0X14, 0X000256CF, 0X55439803),
]


class TestFunctions(TestCase):

    @classmethod
    def setUpClass(cls):
        global rvsrc, rvfd, rvsize
        inputfile = os.path.realpath(
            os.path.dirname(
                os.path.realpath(__file__)) +
            "/../../c/test/data/rsvk.10.bin")
        rvsrc, rvfd, rvsize, _ = bs.mmap_binfile(inputfile)
        if rvfd < 0 or rvsize != 120:
            assert False, "Unable to open the rsvk.10.bin file"
        global rvmsrc, rvmfd, rvmsize
        inputfile = os.path.realpath(
            os.path.dirname(
                os.path.realpath(__file__)) +
            "/../../c/test/data/rsvk.m.10.bin")
        rvmsrc, rvmfd, rvmsize, _ = bs.mmap_binfile(inputfile)
        if rvmfd < 0 or rvmsize != 120:
            assert False, "Unable to open the rsvk.m.10.bin file"
        global vrsrc, vrfd, vrsize
        inputfile = os.path.realpath(
            os.path.dirname(
                os.path.realpath(__file__)) +
            "/../../c/test/data/vkrs.10.bin")
        vrsrc, vrfd, vrsize, _ = bs.mmap_binfile(inputfile)
        if vrfd < 0 or vrsize != 120:
            assert False, "Unable to open the vkrs.10.bin file"

    @classmethod
    def tearDownClass(cls):
        global rvsrc, rvfd, rvsize
        h = bs.munmap_binfile(rvsrc, rvfd, rvsize)
        if h != 0:
            assert False, "Error while closing the rsvk.10.bin memory-mapped file"
        global vrsrc, vrfd, vrsize
        h = bs.munmap_binfile(vrsrc, vrfd, vrsize)
        if h != 0:
            assert False, "Error while closing the vkrs.10.bin memory-mapped file"

    def test_get_vr_rsid(self):
        for item, rsid, vkey, chrom, pos, refalt in testData:
            xrsid = bs.get_vr_rsid(vrsrc, item)
            self.assertEqual(xrsid, rsid)

    def test_get_rv_variantkey(self):
        for item, rsid, vkey, chrom, pos, refalt in testData:
            vk = bs.get_rv_variantkey(rvsrc, item)
            self.assertEqual(vk, vkey)

    def test_find_rv_variantkey_by_rsid(self):
        for item, rsid, vkey, chrom, pos, refalt in testData:
            vk, first = bs.find_rv_variantkey_by_rsid(rvsrc, 0, 9, rsid)
            self.assertEqual(vk, vkey)
            self.assertEqual(first, item)

    def test_find_rv_variantkey_by_rsid_notfound(self):
        vk, first = bs.find_rv_variantkey_by_rsid(rvsrc, 0, 9, 0xfffffff0)
        self.assertEqual(vk, 0)
        self.assertEqual(first, 10)

    def test_get_next_rv_variantkey_by_rsid(self):
        vk, pos = bs.get_next_rv_variantkey_by_rsid(rvsrc, 2, 9, 0x00000061)
        self.assertEqual(vk, 0x80010274003A0000)
        self.assertEqual(pos, 3)
        vk, pos = bs.get_next_rv_variantkey_by_rsid(rvsrc, pos, 9, 0x00000061)
        self.assertEqual(vk, 0)
        self.assertEqual(pos, 4)

    def test_find_all_rv_variantkey_by_rsid(self):
        vks = bs.find_all_rv_variantkey_by_rsid(rvmsrc, 0, 9, 0x00000003)
        self.assertEqual(len(vks), 3)
        self.assertEqual(vks[0], 0x80010274003A0000)
        self.assertEqual(vks[1], 0x8001028D00138000)
        self.assertEqual(vks[2], 0x80010299007A0000)

    def test_find_all_rv_variantkey_by_rsid_notfound(self):
        vks = bs.find_all_rv_variantkey_by_rsid(rvmsrc, 0, 9, 0xfffffff0)
        self.assertEqual(len(vks), 0)

    def test_find_vr_rsid_by_variantkey(self):
        for item, rsid, vkey, chrom, pos, refalt in testData:
            rx, first = bs.find_vr_rsid_by_variantkey(vrsrc, 0, 9, vkey)
            self.assertEqual(rx, rsid)
            self.assertEqual(first, item)

    def test_find_vr_rsid_by_variantkey_notfound(self):
        rx, first = bs.find_vr_rsid_by_variantkey(vrsrc, 0, 9, 0xfffffffffffffff0)
        self.assertEqual(rx, 0)
        self.assertEqual(first, 10)

    def test_find_vr_chrompos_range(self):
        xrsid, xfirst, xlast = bs.find_vr_chrompos_range(vrsrc, 0, 9, testData[6][3], testData[7][4], testData[8][4])
        self.assertEqual(xrsid, testData[7][1])
        self.assertEqual(xfirst, 7)
        self.assertEqual(xlast, 8)

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
            "/../../c/test/data/rsvk.10.bin")
        rvsrc, rvfd, rvsize, _ = bs.mmap_binfile(inputfile)
        if rvfd < 0 or rvsize != 120:
            assert False, "Unable to open the rsvk.10.bin file"
        global vrsrc, vrfd, vrsize
        if vrfd >= 0:
            pass
        bs.munmap_binfile(vrsrc, vrfd, vrsize)
        inputfile = os.path.realpath(
            os.path.dirname(
                os.path.realpath(__file__)) +
            "/../../c/test/data/vkrs.10.bin")
        vrsrc, vrfd, vrsize, _ = bs.mmap_binfile(inputfile)
        if vrfd < 0 or vrsize != 120:
            assert False, "Unable to open the vkrs.10.bin file"

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
            args=[rvsrc, 0, 9, 0x2F81F575],
            setup=setup,
            iterations=1,
            rounds=10000)

    def test_find_vr_rsid_by_variantkey(self, benchmark):
        benchmark.pedantic(
            bs.find_vr_rsid_by_variantkey,
            args=[vrsrc, 0, 9, 0X160017CCA313D0E0],
            setup=setup,
            iterations=1,
            rounds=10000)

    def test_find_vr_chrompos_range(self, benchmark):
        benchmark.pedantic(
            bs.find_vr_chrompos_range,
            args=[vrsrc, 0, 9, 0x19, 0x001AF8FD, 0x001C8F2A],
            setup=setup,
            iterations=1,
            rounds=10000)

    def test_tearDown(self):
        global rvsrc, rvfd, rvsize
        h = bs.munmap_binfile(rvsrc, rvfd, rvsize)
        rvfd = -1
        rvsize = 0
        if h != 0:
            assert False, "Error while closing the rsvk.10.bin memory-mapped file"
        global vrsrc, vrfd, vrsize
        h = bs.munmap_binfile(vrsrc, vrfd, vrsize)
        vrfd = -1
        vrsize = 0
        if h != 0:
            assert False, "Error while closing the vkrs.10.bin memory-mapped file"
