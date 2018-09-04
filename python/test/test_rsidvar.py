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


# item, chrom, pos, refalt, rsid, vkey
testData = [
    (0, 0X01, 0X0004F44B, 0X00338000, 0X00000001, 0X08027A2580338000),
    (1, 0X09, 0X000143FC, 0X439E3918, 0X00000007, 0X4800A1FE439E3918),
    (2, 0X09, 0X000143FC, 0X7555EB16, 0X0000000B, 0X4800A1FE7555EB16),
    (3, 0X10, 0X000204E8, 0X003A0000, 0X00000061, 0X80010274003A0000),
    (4, 0X10, 0X0002051A, 0X00138000, 0X00000065, 0X8001028D00138000),
    (5, 0X10, 0X00020532, 0X007A0000, 0X000003E5, 0X80010299007A0000),
    (6, 0X14, 0X000256C4, 0X003A0000, 0X000003F1, 0XA0012B62003A0000),
    (7, 0X14, 0X000256C5, 0X00708000, 0X000026F5, 0XA0012B6280708000),
    (8, 0X14, 0X000256CB, 0X63256692, 0X000186A3, 0XA0012B65E3256692),
    (9, 0X14, 0X000256CF, 0X55439803, 0X00019919, 0XA0012B67D5439803),
]


class TestFunctions(TestCase):

    @classmethod
    def setUpClass(cls):
        global rvmf, rvmc, rvnrows
        inputfile = os.path.realpath(
            os.path.dirname(
                os.path.realpath(__file__)) +
            "/../../c/test/data/rsvk.10.bin")
        rvmf, rvmc, rvnrows = bs.mmap_rsvk_file(inputfile, [4, 8])
        if rvnrows <= 0:
            assert False, "Unable to open the rsvk.10.bin file"
        global rvmmf, rvmmc, rvmnrows
        inputfile = os.path.realpath(
            os.path.dirname(
                os.path.realpath(__file__)) +
            "/../../c/test/data/rsvk.m.10.bin")
        rvmmf, rvmmc, rvmnrows = bs.mmap_rsvk_file(inputfile, [])
        if rvmnrows <= 0:
            assert False, "Unable to open the rsvk.m.10.bin file"
        global vrmf, vrmc, vrnrows
        inputfile = os.path.realpath(
            os.path.dirname(
                os.path.realpath(__file__)) +
            "/../../c/test/data/vkrs.10.bin")
        vrmf, vrmc, vrnrows = bs.mmap_vkrs_file(inputfile, [8, 4])
        if vrnrows <= 0:
            assert False, "Unable to open the vkrs.10.bin file"

    @classmethod
    def tearDownClass(cls):
        global rvmf, rvmc, rvnrows
        h = bs.munmap_binfile(rvmf)
        if h != 0:
            assert False, "Error while closing the rsvk.10.bin memory-mapped file"
        global rvmmf, rvmmc, rvnrows
        h = bs.munmap_binfile(rvmmf)
        if h != 0:
            assert False, "Error while closing the rsvk.10.bin memory-mapped file"
        global vrmf, vrmc, vrnrows
        h = bs.munmap_binfile(vrmf)
        if h != 0:
            assert False, "Error while closing the vkrs.10.bin memory-mapped file"

    def test_find_rv_variantkey_by_rsid(self):
        for item, _, _, _, rsid, vkey in testData:
            vk, first = bs.find_rv_variantkey_by_rsid(rvmc, 0, rvnrows, rsid)
            self.assertEqual(vk, vkey)
            self.assertEqual(first, item)

    def test_find_rv_variantkey_by_rsid_notfound(self):
        vk, first = bs.find_rv_variantkey_by_rsid(rvmc, 0, rvnrows, 0xfffffff0)
        self.assertEqual(vk, 0)
        self.assertEqual(first, 10)

    def test_get_next_rv_variantkey_by_rsid(self):
        vk, pos = bs.get_next_rv_variantkey_by_rsid(rvmc, 2, rvnrows, 0x00000061)
        self.assertEqual(vk, 0x80010274003A0000)
        self.assertEqual(pos, 3)
        vk, pos = bs.get_next_rv_variantkey_by_rsid(rvmc, pos, rvnrows, 0x00000061)
        self.assertEqual(vk, 0)
        self.assertEqual(pos, 4)

    def test_find_all_rv_variantkey_by_rsid(self):
        vks = bs.find_all_rv_variantkey_by_rsid(rvmmc, 0, rvnrows, 0x00000003)
        self.assertEqual(len(vks), 3)
        self.assertEqual(vks[0], 0x80010274003A0000)
        self.assertEqual(vks[1], 0x8001028D00138000)
        self.assertEqual(vks[2], 0x80010299007A0000)

    def test_find_all_rv_variantkey_by_rsid_notfound(self):
        vks = bs.find_all_rv_variantkey_by_rsid(rvmmc, 0, rvnrows, 0xfffffff0)
        self.assertEqual(len(vks), 0)

    def test_find_vr_rsid_by_variantkey(self):
        for item, _, _, _, rsid, vkey in testData:
            rx, first = bs.find_vr_rsid_by_variantkey(vrmc, 0, vrnrows, vkey)
            self.assertEqual(rx, rsid)
            self.assertEqual(first, item)

    def test_find_vr_rsid_by_variantkey_notfound(self):
        rx, first = bs.find_vr_rsid_by_variantkey(vrmc, 0, vrnrows, 0xfffffffffffffff0)
        self.assertEqual(rx, 0)
        self.assertEqual(first, 10)

    def test_find_vr_chrompos_range(self):
        xrsid, xfirst, xlast = bs.find_vr_chrompos_range(vrmc, 0, vrnrows, testData[6][1], testData[7][2], testData[8][2])
        self.assertEqual(xrsid, testData[7][4])
        self.assertEqual(xfirst, 7)
        self.assertEqual(xlast, 8)

    def test_find_vr_chrompos_range_notfound(self):
        xrsid, xfirst, xlast = bs.find_vr_chrompos_range(vrmc, 0, vrnrows, 0xff, 0xffffff00, 0xfffffff0)
        self.assertEqual(xrsid, 0)
        self.assertEqual(xfirst, 10)
        self.assertEqual(xlast, 10)


class TestBenchmark(object):

    global setup

    def setup():
        global rvmf, rvmc, rvnrows
        bs.munmap_binfile(rvmf)
        inputfile = os.path.realpath(
            os.path.dirname(
                os.path.realpath(__file__)) +
            "/../../c/test/data/rsvk.10.bin")
        rvmf, rvmc, rvnrows = bs.mmap_rsvk_file(inputfile, [4, 8])
        if rvnrows <= 0:
            assert False, "Unable to open the rsvk.10.bin file"
        global vrmf, vrmc, vrnrows
        bs.munmap_binfile(vrmf)
        inputfile = os.path.realpath(
            os.path.dirname(
                os.path.realpath(__file__)) +
            "/../../c/test/data/vkrs.10.bin")
        vrmf, vrmc, vrnrows = bs.mmap_vkrs_file(inputfile, [8, 4])
        if vrnrows <= 0:
            assert False, "Unable to open the vkrs.10.bin file"

    def test_find_rv_variantkey_by_rsid(self, benchmark):
        benchmark.pedantic(
            bs.find_rv_variantkey_by_rsid,
            args=[rvmc, 0, rvnrows, 0x2F81F575],
            setup=setup,
            iterations=1,
            rounds=10000)

    def test_find_vr_rsid_by_variantkey(self, benchmark):
        benchmark.pedantic(
            bs.find_vr_rsid_by_variantkey,
            args=[vrmc, 0, vrnrows, 0X160017CCA313D0E0],
            setup=setup,
            iterations=1,
            rounds=10000)

    def test_find_vr_chrompos_range(self, benchmark):
        benchmark.pedantic(
            bs.find_vr_chrompos_range,
            args=[vrmc, 0, vrnrows, 0x19, 0x001AF8FD, 0x001C8F2A],
            setup=setup,
            iterations=1,
            rounds=10000)

    def test_tearDown(self):
        global rvmf, rvmc, rvnrows
        h = bs.munmap_binfile(rvmf)
        if h != 0:
            assert False, "Error while closing the rsvk.10.bin memory-mapped file"
        global vrmf, vrmc, vrnrows
        h = bs.munmap_binfile(vrmf)
        if h != 0:
            assert False, "Error while closing the vkrs.10.bin memory-mapped file"
