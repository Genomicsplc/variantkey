"""Tests for variantkey rsidvar module."""

# test_rsidvar.py
# @category   Libraries
# @author     Nicola Asuni <nicola.asuni@genomicsplc.com>
# @copyright  2017-2018 GENOMICS plc
# @license    MIT (see LICENSE)
# @link       https://github.com/genomicsplc/variantkey

import pyvariantkey.variantkey as pyvk
import numpy as np
import os
from unittest import TestCase


# 0:item, 1:chrom, 2:pos, 3:refalt, 4:rsid, 5:vkey
testData = np.array([
    (0, 0x01, 0x0004f44b, 0x00338000, 0x00000001, 0x08027a2580338000),
    (1, 0x09, 0x000143fc, 0x439e3918, 0x00000007, 0x4800a1fe439e3918),
    (2, 0x09, 0x000143fc, 0x7555eb16, 0x0000000b, 0x4800a1fe7555eb16),
    (3, 0x10, 0x000204e8, 0x003a0000, 0x00000061, 0x80010274003a0000),
    (4, 0x10, 0x0002051a, 0x00138000, 0x00000065, 0x8001028d00138000),
    (5, 0x10, 0x00020532, 0x007a0000, 0x000003e5, 0x80010299007a0000),
    (6, 0x14, 0x000256c4, 0x003a0000, 0x000003f1, 0xa0012b62003a0000),
    (7, 0x14, 0x000256c5, 0x00708000, 0x000026f5, 0xa0012b6280708000),
    (8, 0x14, 0x000256cb, 0x63256692, 0x000186a3, 0xa0012b65e3256692),
    (9, 0x14, 0x000256cf, 0x55439803, 0x00019919, 0xa0012b67d5439803),
], dtype=np.string_)


class TestFunctions(TestCase):

    @classmethod
    def setUpClass(cls):
        global npvk
        try:
            npvk = pyvk.VariantKey(
                os.path.realpath(os.path.dirname(os.path.realpath(__file__)) + "/../../c/test/data/genoref.bin"),
                os.path.realpath(os.path.dirname(os.path.realpath(__file__)) + "/../../c/test/data/nrvk.10.bin"),
                os.path.realpath(os.path.dirname(os.path.realpath(__file__)) + "/../../c/test/data/rsvk.10.bin"),
                os.path.realpath(os.path.dirname(os.path.realpath(__file__)) + "/../../c/test/data/vkrs.10.bin"))
        except Exception as err:
            assert False, "Unable to initialize the class: {0}".format(err)

    def test_find_rv_variantkey_by_rsid(self):
        vk, first = npvk.find_rv_variantkey_by_rsid(testData[:, 4])
        np.testing.assert_array_equal(vk, testData[:, 5].astype(np.uint64))
        np.testing.assert_array_equal(first, testData[:, 0].astype(np.uint64))

    def test_find_rv_variantkey_by_rsid_notfound(self):
        vk, first = npvk.find_rv_variantkey_by_rsid(0xfffffff0)
        np.testing.assert_array_equal(vk, 0)
        np.testing.assert_array_equal(first, 9)

    def test_get_next_rv_variantkey_by_rsid(self):
        vk, pos = npvk.get_next_rv_variantkey_by_rsid(2, 0x00000061)
        np.testing.assert_array_equal(vk, 0x80010274003A0000)
        np.testing.assert_array_equal(pos, 3)
        vk, pos = npvk.get_next_rv_variantkey_by_rsid(pos, 0x00000061)
        np.testing.assert_array_equal(vk, 0)
        np.testing.assert_array_equal(pos, 4)

#    def test_find_all_rv_variantkey_by_rsid(self):
#        vks = npvk.find_all_rv_variantkey_by_rsid(0x00000003)
#        np.testing.assert_array_equal(len(vks), 3)
#        np.testing.assert_array_equal(vks[0], 0x80010274003A0000)
#        np.testing.assert_array_equal(vks[1], 0x8001028D00138000)
#        np.testing.assert_array_equal(vks[2], 0x80010299007A0000)

    def test_find_all_rv_variantkey_by_rsid_notfound(self):
        vks = npvk.find_all_rv_variantkey_by_rsid(0xfffffff0)
        np.testing.assert_array_equal(len(vks), 0)

    def test_find_vr_rsid_by_variantkey(self):
        rx, first = npvk.find_vr_rsid_by_variantkey(testData[:, 5])
        np.testing.assert_array_equal(rx, testData[:, 4].astype(np.uint32))
        np.testing.assert_array_equal(first, testData[:, 0].astype(np.uint64))

    def test_find_vr_rsid_by_variantkey_notfound(self):
        rx, first = npvk.find_vr_rsid_by_variantkey(0xfffffffffffffff0)
        np.testing.assert_array_equal(rx, 0)
        np.testing.assert_array_equal(first, 9)

    def test_find_vr_chrompos_range(self):
        xrsid, xfirst, xlast = npvk.find_vr_chrompos_range(0x14, 0x000256c5, 0x000256cb)
        np.testing.assert_array_equal(xrsid, 0x000026f5)
        np.testing.assert_array_equal(xfirst, 7)
        np.testing.assert_array_equal(xlast, 9)

    def test_find_vr_chrompos_range_notfound(self):
        xrsid, xfirst, xlast = npvk.find_vr_chrompos_range(0xff, 0xffffff00, 0xfffffff0)
        np.testing.assert_array_equal(xrsid, 0)
        np.testing.assert_array_equal(xfirst, 10)
        np.testing.assert_array_equal(xlast, 10)
