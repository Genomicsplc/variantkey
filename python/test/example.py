#!/usr/bin/env python3

# Usage example for VariantKey Python wrapper
# https://github.com/genomicsplc/variantkey

import variantkey as vk


# BASIC VARIANTKEY FUNCTIONS
# --------------------------

vk.encode_chrom(b'X')
# 23

vk.decode_chrom(23)
# b'X'

vk.encode_refalt(ref=b'AC', alt=b'GT')
# 286097408

vk.decode_refalt(286097408)
# (b'AC', b'GT', 2, 2)
# Return values are: REF, ALT, REF length, ALT length

vk.encode_variantkey(chrom=23, pos=12345, refalt=286097408)
# 13258623813950472192

vk.extract_variantkey_chrom(13258623813950472192)
# 23

vk.extract_variantkey_pos(13258623813950472192)
# 12345

vk.extract_variantkey_refalt(13258623813950472192)
# 286097408

vk.decode_variantkey(13258623813950472192)
# (23, 12345, 286097408)

vk.variantkey(chrom=b'X', pos=12345, ref=b'AC', alt=b'GT')
# 13258623813950472192

vk.variantkey(chrom='X', pos=12345, ref='AC', alt='GT')
# 13258623813950472192

vk.variantkey(b'X', 12345, b'AC', b'GT')
# 13258623813950472192

vk.variantkey('X', 12345, 'AC', 'GT')
# 13258623813950472192

vk.variantkey_range(chrom=23, pos_min=1234, pos_max=5678)
# (13258599952973561856, 13258609498538377215)
# Return values are: VK min, VK max

vk.compare_variantkey_chrom(13258599952973561856, 13258609498538377215)
# 0

vk.compare_variantkey_chrom_pos(13258599952973561856, 13258609498538377215)
# -1

vk.variantkey_hex(13258623813950472192)
# b'b800181c910d8000'

vk.parse_variantkey_hex(b'b800181c910d8000')
# 13258623813950472192


# /\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\


# GENOREF FUNCTIONS
# -----------------

# Load the reference genome binary file.
# The input reference binary files can be generated from a FASTA file using the resources/tools/fastabin.sh script.
# This example uses the "c/test/data/genoref.bin".
mf, mfsize = vk.mmap_genoref_file('genoref.bin')
if mfsize <= 0:
    assert False, "Unable to open the genoref.bin file"

vk.get_genoref_seq(mf, chrom=23, pos=0)
# b'A'

vk.check_reference(mf, chrom=23, pos=0, ref='A')
# 0

vk.flip_allele(b'ATCGMKRYBVDHWSNatcgmkrybvdhwsn')
# b'TAGCKMYRVBHDWSNTAGCKMYRVBHDWSN'

# Normalize a variant - this function should be used before generating a new VariantKey
vk.normalize_variant(mf, chrom=13, pos=2, ref='CDE', alt='CFE')
# (48, 3, b'D', b'F', 1, 1)
# Return values are: code, POS, REF, ALT, REF length, ALT length

# Create normalized VariantKey
vk.normalized_variantkey(mf, chrom="13", pos=2, posindex=0, ref='CDE', alt='CFE')
# (7493989787586955617, 48)

vk.munmap_binfile(mf)


# /\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\


# NRVK
# ----

# Load the lookup table for non-reversible variantkeys.
# The input binary files can be generated from a normalized VCF file using the resources/tools/nrvk.sh script.
# The VCF file can be normalized using the `resources/tools/vcfnorm.sh` script.
# This example uses the "c/test/data/nrvk.10.bin".
mf, mc, nrows = vk.mmap_nrvk_file('nrvk.10.bin')
if nrows <= 0:
    assert False, "Unable to open the nrvk.10.bin file"

vk.find_ref_alt_by_variantkey(mc, vk=0x2000c3521f1c15ab)
# (b'ACGTACGT', b'ACGT', 8, 4, 12)
# Return values are: REF, ALT, REF length, ALT length, REF+ALT length

# Reverse all VariantKeys, including the ones that are not directly reversible by using a lookup table.
vk.reverse_variantkey(mc, vk=0x2000c3521f1c15ab)
# (b'4', 100004, b'ACGTACGT', b'ACGT', 8, 4, 12)
# Return values are: CHROM, POS, REF, ALT, REF length, ALT length, REF+ALT length

vk.get_variantkey_ref_length(mc, vk=0x2000c3521f1c15ab)
# 8

vk.get_variantkey_endpos(mc, vk=0x2000c3521f1c15ab)
# 100012

vk.get_variantkey_chrom_startpos(vk=0x2000c3521f1c15ab)
# 1073841828

vk.get_variantkey_chrom_endpos(mc, vk=0x2000c3521f1c15ab)
# 1073841836

vk.munmap_binfile(mf)


# /\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\


# RSIDVAR
# -------

# Load the lookup table for rsID to VariantKey.
# The input binary file can be generated using the resources/tools/rsvk.sh script.
# This example uses the "c/test/data/rsvk.10.bin".
mf, mc, nrows = vk.mmap_rsvk_file('rsvk.10.bin', [])
if nrows <= 0:
    assert False, "Unable to open the rsvk.10.bin file"

vk.find_rv_variantkey_by_rsid(mc, 0, nrows, rsid=0X00000061)
# (9223656209074749440, 3)

vk.get_next_rv_variantkey_by_rsid(mc, 2, nrows, 0x00000061)
# (9223656209074749440, 3)

vk.munmap_binfile(mf)

# - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

# Load the lookup table for rsID to VariantKey.
# The input binary file can be generated using the resources/tools/rsvk.sh script.
# This example uses the "c/test/data/rsvk.m.10.bin".
mf, mc, nrows = vk.mmap_rsvk_file('rsvk.m.10.bin', [])
if nrows <= 0:
    assert False, "Unable to open the rsvk.m.10.bin file"

vk.find_all_rv_variantkey_by_rsid(mc, 0, nrows, 0x00000003)
# [9223656209074749440, 9223656316446408704, 9223656367992733696]

vk.munmap_binfile(mf)

# - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

# Load the lookup table for VariantKey ro rsID
# The input binary file can be generated using the resources/tools/vkrs.sh script.
# This example uses the "c/test/data/vkrs.10.bin".
mf, mc, nrows = vk.mmap_vkrs_file('vkrs.10.bin', [])
if nrows <= 0:
    assert False, "Unable to open the vkrs.10.bin file"

vk.find_vr_rsid_by_variantkey(mc, 0, nrows, vk=0X80010274003A0000)
# (97, 3)

vk.get_next_vr_rsid_by_variantkey(mc, 2, nrows, vk=0X80010274003A0000)
# (97, 3)

vk.find_all_vr_rsid_by_variantkey(mc, 0, nrows, vk=0X80010274003A0000)
# [97]

vk.find_vr_chrompos_range(mc, 0, nrows, 0X14, 0X000256C5, 0X000256CB)
# (9973, 7, 9)

vk.munmap_binfile(mf)


# /\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\


# REGIONKEY
# ---------

vk.encode_region_strand(-1)
# 2

vk.decode_region_strand(2)
# -1

vk.encode_regionkey(25, 1000, 2000, 2)
# 14411520955069251204

vk.extract_regionkey_chrom(0xc80001f400003e84)
# 25

vk.extract_regionkey_startpos(0xc80001f400003e84)
# 1000

vk.extract_regionkey_endpos(0xc80001f400003e84)
# 2000

vk.extract_regionkey_strand(0xc80001f400003e84)
# 2

vk.decode_regionkey(0xc80001f400003e84)
# (25, 1000, 2000, 2)

vk.reverse_regionkey(0xc80001f400003e84)
# (b'MT', 1000, 2000, -1)

vk.regionkey("MT", 1000, 2000, -1)
# 14411520955069251204

vk.extend_regionkey(14411520955069251204, 100)
# 14411520740320887204

vk.reverse_regionkey(14411520740320887204)
# (b'MT', 900, 2100, -1)

vk.regionkey_hex(0xc80001f400003e84)
# b'c80001f400003e84'

vk.parse_regionkey_hex("c80001f400003e84")
# 14411520955069251204

vk.get_regionkey_chrom_startpos(0xc80001f400003e84)
# 6710887400

vk.get_regionkey_chrom_endpos(0xc80001f400003e84)
# 6710888400

vk.are_overlapping_regions(5, 4,  6, 5, 3, 7)
# 1

vk.are_overlapping_region_regionkey(5, 4, 6, 0x2800000180000038)
# 1

vk.are_overlapping_regionkeys(0x2800000200000030, 0x2800000180000038)
# 1

vk.are_overlapping_variantkey_regionkey(None, 0x2800000210920000, 0x2800000180000038)
# 1

vk.variantkey_to_regionkey(None, 0x2800000210920000)
# 2882303770107052080


# /\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\


# ESID
# ---------

vk.encode_string_id("A0A022YWF9", 0)
# 12128340051199752601

vk.decode_string_id(0xa850850492e77999)
# (b'A0A022YWF9', 10)

vk.encode_string_num_id("ABC:0000123456", b':')
# 15592178792074961472

vk.decode_string_id(0xd8628c002001e240)
# (b'ABC:0000123456', 14)

vk.hash_string_id("0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ")
# 12945031672818874332


# /\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\
