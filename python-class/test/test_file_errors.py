"""Tests for variantkey file errors."""

# test_esid.py
# @category   Libraries
# @author     Nicola Asuni <nicola.asuni@genomicsplc.com>
# @copyright  2017-2018 GENOMICS plc
# @license    MIT (see LICENSE)
# @link       https://github.com/genomicsplc/variantkey

import pyvariantkey.variantkey as pyvk
from unittest import TestCase


class TestFunctions(TestCase):

    def test_class_file_error_genoref(self):
        with self.assertRaises(Exception):
            pyvk.VariantKey("/wrong/path/error.bin", None, None, None)

    def test_class_file_error_nrvk(self):
        with self.assertRaises(Exception):
            pyvk.VariantKey(None, "/wrong/path/error.bin", None, None)

    def test_class_file_error_rsvk(self):
        with self.assertRaises(Exception):
            pyvk.VariantKey(None, None, "/wrong/path/error.bin", None)

    def test_class_file_error_vkrs(self):
        with self.assertRaises(Exception):
            pyvk.VariantKey(None, None, None, "/wrong/path/error.bin")
