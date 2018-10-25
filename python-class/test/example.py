#!/usr/bin/env python3

# Usage example for VariantKey Python wrapper
# https://github.com/genomicsplc/variantkey

import pyvariantkey.variantkey as pyvk
import numpy as np

# Initialize the VariantKey class with support binary files.
# The binary files can be omitted but some functionalities will not be available.
try:
    vk = pyvk.VariantKey(genoref_file="genoref.bin", nrvk_file="nrvk.10.bin", rsvk_file="rsvk.10.bin", vkrs_file="vkrs.10.bin")
except Exception as err:
    assert False, "Unable to initialize the class: {0}".format(err)

# NOTE: The following functions also accept list and numpy arrays as input.


# BASIC VARIANTKEY FUNCTIONS
# --------------------------

vk.encode_chrom(b'X')
# array(23, dtype=uint8)

vk.encode_chrom(['12', b'X', b'Y', b'MT'])
# array([12, 23, 24, 25], dtype=uint8)

vk.decode_chrom(23)
# array(b'X', dtype='|S1')

vk.encode_refalt(ref=b'AC', alt=b'GT')
# array(286097408, dtype=uint32)

vk.decode_refalt(286097408)
# (array(b'AC', dtype='|S2'), array(b'GT', dtype='|S2'), array(2, dtype=uint8), array(2, dtype=uint8))
# Return values are: REF, ALT, REF length, ALT length

vk.encode_variantkey(chrom=23, pos=12345, refalt=286097408)
# array(13258623813950472192, dtype=uint64)

vk.extract_variantkey_chrom(13258623813950472192)
# array(23, dtype=uint8)

vk.extract_variantkey_pos(13258623813950472192)
# array(12345, dtype=uint32)

vk.extract_variantkey_refalt(13258623813950472192)
# array(286097408, dtype=uint32)

vk.decode_variantkey(13258623813950472192)
# (array(23, dtype=uint8), array(12345, dtype=uint32), array(286097408, dtype=uint32))

vk.variantkey(chrom=b'X', pos=12345, ref=b'AC', alt=b'GT')
# array(13258623813950472192, dtype=uint64)

vk.variantkey(chrom='X', pos=12345, ref='AC', alt='GT')
# array(13258623813950472192, dtype=uint64)

vk.variantkey(b'X', 12345, b'AC', b'GT')
# array(13258623813950472192, dtype=uint64)

vk.variantkey('X', 12345, 'AC', 'GT')
# array(13258623813950472192, dtype=uint64)

vk.variantkey_range(chrom=23, pos_min=1234, pos_max=5678)
# (array(13258599952973561856, dtype=uint64), array(13258609498538377215, dtype=uint64))
# Return values are: VK min, VK max

vk.compare_variantkey_chrom(13258599952973561856, 13258609498538377215)
# array(0)

vk.compare_variantkey_chrom_pos(13258599952973561856, 13258609498538377215)
# array(-1)

vk.variantkey_hex(13258623813950472192)
# array(b'b800181c910d8000', dtype='|S16')

vk.parse_variantkey_hex(b'b800181c910d8000')
# array(13258623813950472192, dtype=uint64)


# /\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\


# GENOREF FUNCTIONS
# -----------------

vk.get_genoref_seq(chrom=23, pos=0)
# array(b'A', dtype='|S1')

vk.check_reference(chrom=23, pos=0, ref='A')
# array(0)

vk.flip_allele(b'ATCGMKRYBVDHWSNatcgmkrybvdhwsn')
# array(b'TAGCKMYRVBHDWSNTAGCKMYRVBHDWSN', dtype='|S30')

# Normalize a variant - this function should be used before generating a new VariantKey
vk.normalize_variant(chrom=13, pos=2, ref='CDE', alt='CFE')
# (array(48), array(3, dtype=uint32), array(b'D', dtype='|S1'), array(b'F', dtype='|S1'), array(1, dtype=uint8), array(1, dtype=uint8))
# Return values are: code, POS, REF, ALT, REF length, ALT length

# Create normalized VariantKey
vk.normalized_variantkey(chrom="13", pos=2, posindex=0, ref='CDE', alt='CFE')
# (array(7493989787586955617, dtype=uint64), array(48))


# /\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\


# NRVK
# ----

vk.find_ref_alt_by_variantkey(vk=0x2000c3521f1c15ab)
# (array(b'ACGTACGT', dtype='|S8'), array(b'ACGT', dtype='|S4'), array(8, dtype=uint8), array(4, dtype=uint8), array(12, dtype=uint16))
# Return values are: REF, ALT, REF length, ALT length, REF+ALT length

# Reverse all VariantKeys, including the ones that are not directly reversible by using a lookup table.
vk.reverse_variantkey(vk=0x2000c3521f1c15ab)
# (array(b'4', dtype='|S1'), array(100004, dtype=uint32), array(b'ACGTACGT', dtype='|S8'), array(b'ACGT', dtype='|S4'), array(8, dtype=uint8), array(4, dtype=uint8), array(12, dtype=uint16))
# Return values are: CHROM, POS, REF, ALT, REF length, ALT length, REF+ALT length

vk.get_variantkey_ref_length(vk=0x2000c3521f1c15ab)
# array(8, dtype=uint8)

vk.get_variantkey_endpos(vk=0x2000c3521f1c15ab)
# array(100012, dtype=uint32)

vk.get_variantkey_chrom_startpos(vk=0x2000c3521f1c15ab)
# array(1073841828, dtype=uint64)

vk.get_variantkey_chrom_endpos(vk=0x2000c3521f1c15ab)
# array(1073841836, dtype=uint64)

# /\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\


# RSIDVAR
# -------

vk.find_rv_variantkey_by_rsid(rsid=0X00000061)
# (array(9223656209074749440, dtype=uint64), array(3, dtype=uint64))

vk.get_next_rv_variantkey_by_rsid(2, 0x00000061)
# (array(9223656209074749440, dtype=uint64), array(3, dtype=uint64))

vk.find_all_rv_variantkey_by_rsid(0x00000061)
# array([9223656209074749440], dtype=uint64)

vk.find_vr_rsid_by_variantkey(vk=0x80010274003a0000)
# (array(97, dtype=uint32), array(3, dtype=uint64))

vk.find_vr_chrompos_range(0x14, 0x000256c5, 0x000256cb)
# (array(9973, dtype=uint32), array(7, dtype=uint64), array(9, dtype=uint64))


# /\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\


# REGIONKEY
# ---------

vk.encode_region_strand(-1)
# array(2, dtype=uint8)

vk.decode_region_strand(2)
# array(-1, dtype=int16)

vk.encode_regionkey(25, 1000, 2000, 2)
# array(14411520955069251204, dtype=uint64)

vk.extract_regionkey_chrom(0xc80001f400003e84)
# array(25, dtype=uint8)

vk.extract_regionkey_startpos(0xc80001f400003e84)
# array(1000, dtype=uint32)

vk.extract_regionkey_endpos(0xc80001f400003e84)
# array(2000, dtype=uint32)

vk.extract_regionkey_strand(0xc80001f400003e84)
# array(2, dtype=uint8)

vk.decode_regionkey(0xc80001f400003e84)
# (array(25, dtype=uint8), array(1000, dtype=uint32), array(2000, dtype=uint32), array(2, dtype=uint8))

vk.reverse_regionkey(0xc80001f400003e84)
# (array(b'MT', dtype='|S2'), array(1000, dtype=uint32), array(2000, dtype=uint32), array(-1, dtype=int16))

vk.regionkey("MT", 1000, 2000, -1)
# array(14411520955069251204, dtype=uint64)

vk.extend_regionkey(14411520955069251204, 100)
# array(14411520740320887204, dtype=uint64)

vk.reverse_regionkey(14411520740320887204)
# (array(b'MT', dtype='|S2'), array(900, dtype=uint32), array(2100, dtype=uint32), array(-1, dtype=int16))

vk.regionkey_hex(0xc80001f400003e84)
# array(b'c80001f400003e84', dtype='|S16')

vk.parse_regionkey_hex("c80001f400003e84")
# array(14411520955069251204, dtype=uint64)

vk.get_regionkey_chrom_startpos(0xc80001f400003e84)
# array(6710887400, dtype=uint64)

vk.get_regionkey_chrom_endpos(0xc80001f400003e84)
# array(6710887400, dtype=uint64)

vk.are_overlapping_regions(5, 4, 6, 5, 3, 7)
# array(1, dtype=uint8

vk.are_overlapping_region_regionkey(5, 4, 6, 0x2800000180000038)
# array(1, dtype=uint8

vk.are_overlapping_regionkeys(0x2800000200000030, 0x2800000180000038)
# array(1, dtype=uint8

vk.are_overlapping_variantkey_regionkey(0x2800000210920000, 0x2800000180000038)
# array(1, dtype=uint8)

vk.variantkey_to_regionkey(0x2800000210920000)
# array(2882303770107052080, dtype=uint64)


# /\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\


# ESID
# ---------

vk.encode_string_id("A0A022YWF9", 0)
# array(12128340051199752601, dtype=uint64)

vk.decode_string_id(0xa850850492e77999)
# (array(b'A0A022YWF9', dtype='|S10'), array(10, dtype=uint8)))

vk.encode_string_num_id("ABC:0000123456", b':')
# array(15592178792074961472, dtype=uint64)

vk.decode_string_id(0xd8628c002001e240)
# (array(b'ABC:0000123456', dtype='|S14'), array(14, dtype=uint8))

vk.hash_string_id("0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ")
# array(12945031672818874332, dtype=uint64)


# /\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\

# Close all memory-mapped files
vk.close()
