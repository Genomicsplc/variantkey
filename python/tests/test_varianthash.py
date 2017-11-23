"""Tests for libpyvarianthash module."""


import libpyvarianthash as vh
from unittest import TestCase


class TestFunctions(TestCase):

    def test_encode_chrom_num(self):
        h = vh.encode_chrom("22")
        self.assertEqual(h, 22)

    def test_encode_chrom_x(self):
        h = vh.encode_chrom("X")
        self.assertEqual(h, 23)

    def test_encode_chrom_y(self):
        h = vh.encode_chrom("Y")
        self.assertEqual(h, 24)

    def test_encode_chrom_xy(self):
        h = vh.encode_chrom("xy")
        self.assertEqual(h, 25)

    def test_encode_chrom_mt(self):
        h = vh.encode_chrom("MT")
        self.assertEqual(h, 26)

    def test_encode_ref_alt(self):
        h = vh.encode_ref_alt("TCA", "TGTCG")
        self.assertEqual(h, 0xe683c0ceef5adee0)

    def test_variant_hash(self):
        h = vh.variant_hash("6", 193330, "TCA", "TGTCG")
        self.assertEqual(h[0], 0x6)
        self.assertEqual(h[1], 0x2f332)
        self.assertEqual(h[2], 0xe683c0ceef5adee0)

    def test_variant_hash_string(self):
        h = vh.variant_hash_string(0x6, 0x2f332, 0xe683c0ceef5adee0)
        self.assertEqual(h, b'000000060002f332e683c0ceef5adee0')

    def test_decode_variant_hash_string(self):
        h = vh.decode_variant_hash_string("000000060002f332e683c0ceef5adee0")
        self.assertEqual(h[0], 0x6)
        self.assertEqual(h[1], 0x2f332)
        self.assertEqual(h[2], 0xe683c0ceef5adee0)

    def test_decode_variant_hash_bytes(self):
        h = vh.decode_variant_hash_string(b'000000060002f332e683c0ceef5adee0')
        self.assertEqual(h[0], 0x6)
        self.assertEqual(h[1], 0x2f332)
        self.assertEqual(h[2], 0xe683c0ceef5adee0)

    def test_farmhash64(self):
        h = vh.farmhash64("Lorem ipsum dolor sit amet")
        self.assertEqual(h, 0xe0b3271b22c026ef)
