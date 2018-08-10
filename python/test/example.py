#!/usr/bin/env python3

# Usage example for VariantKey Python wrapper
# https://github.com/genomicsplc/variantkey

import variantkey as vk


# BASIC VARIANTKEY FUNCTIONS
# --------------------------

print(vk.encode_chrom(b'X'))
# 23

print(vk.decode_chrom(23))
# b'X'

print(vk.encode_refalt(ref=b'AC', alt=b'GT'))
# 286097408

print(vk.decode_refalt(286097408))
# (b'AC', b'GT', 2, 2)
# Return values are: REF, ALT, REF length, ALT length

print(vk.encode_variantkey(chrom=23, pos=12345, refalt=286097408))
# 13258623813950472192

print(vk.extract_variantkey_chrom(13258623813950472192))
# 23

print(vk.extract_variantkey_pos(13258623813950472192))
# 12345

print(vk.extract_variantkey_refalt(13258623813950472192))
# 286097408

print(vk.decode_variantkey(13258623813950472192))
# (23, 12345, 286097408)

print(vk.variantkey(chrom=b'X', pos=12345, ref=b'AC', alt=b'GT'))
# 13258623813950472192

print(vk.variantkey(chrom='X', pos=12345, ref='AC', alt='GT'))
# 13258623813950472192

print(vk.variantkey(b'X', 12345, b'AC', b'GT'))
# 13258623813950472192

print(vk.variantkey('X', 12345, 'AC', 'GT'))
# 13258623813950472192

print(vk.variantkey_range(chrom=23, pos_min=1234, pos_max=5678))
# (13258599952973561856, 13258609498538377215)
# Return values are: VK min, VK max

print(vk.compare_variantkey_chrom(13258599952973561856, 13258609498538377215))
# 0

print(vk.compare_variantkey_chrom_pos(13258599952973561856, 13258609498538377215))
# -1

print(vk.variantkey_hex(13258623813950472192))
# b'b800181c910d8000'

print(vk.parse_variantkey_hex(b'b800181c910d8000'))
# 13258623813950472192


# /\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\


# GENOREF FUNCTIONS
# -----------------

# Load the reference genome binary file.
# The input reference binary files can be generated from a FASTA file using the resources/tools/fastabin.sh script.
# This example uses the "c/test/data/genoref.bin".
mfsrc, mffd, mfsize, mflast = vk.mmap_binfile('genoref.bin')
if mffd < 0:
    assert False, "Unable to open the genoref.bin file"

# Load the file index
mfidx = vk.load_genoref_index(mfsrc)

print(vk.get_genoref_seq(mfsrc, mfidx, chrom=23, pos=0))
# b'A'

print(vk.check_reference(mfsrc, mfidx, chrom=23, pos=0, ref='A'))
# 0

print(vk.flip_allele(b'ATCGMKRYBVDHWSNatcgmkrybvdhwsn'))
# b'TAGCKMYRVBHDWSNTAGCKMYRVBHDWSN'

# Normalize a variant - this function should be used before generating a new VariantKey
print(vk.normalize_variant(mfsrc, mfidx, chrom=13, pos=2, ref='CDE', alt='CFE'))
# (48, 3, b'D', b'F', 1, 1)
# Return values are: code, POS, REF, ALT, REF length, ALT length

vk.munmap_binfile(mfsrc, mffd, mfsize)


# /\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\


# NRVK
# ----

# Load the lookup table for non-reversible variantkeys.
# The input binary files can be generated from a normalized VCF file using the resources/tools/vkhexbin.sh script.
# The VCF file can be normalized using the `resources/tools/vcfnorm.sh` script.
# This example uses the "c/test/data/vknr.10.bin".
mfsrc, mffd, mfsize, mflast = vk.mmap_binfile('vknr.10.bin')
if mffd < 0:
    assert False, "Unable to open the vknr.10.bin file"

print(vk.find_ref_alt_by_variantkey(mfsrc, mflast, vk=0x2000c3521f1c15ab))
# (b'ACGTACGT', b'ACGT', 8, 4, 12)
# Return values are: REF, ALT, REF length, ALT length, REF+ALT length

# Reverse all VariantKeys, including the ones that are not directly reversible by using a lookup table.
print(vk.reverse_variantkey(mfsrc, mflast, vk=0x2000c3521f1c15ab))
# (b'4', 100004, b'ACGTACGT', b'ACGT', 8, 4, 12)
# Return values are: CHROM, POS, REF, ALT, REF length, ALT length, REF+ALT length

print(vk.get_variantkey_ref_length(mfsrc, mflast, vk=0x2000c3521f1c15ab))
# 8

print(vk.get_variantkey_endpos(mfsrc, mflast, vk=0x2000c3521f1c15ab))
# 100012

print(vk.get_variantkey_chrom_startpos(vk=0x2000c3521f1c15ab))
# 1073841828

print(vk.get_variantkey_chrom_endpos(mfsrc, mflast, vk=0x2000c3521f1c15ab))
# 1073841836

vk.munmap_binfile(mfsrc, mffd, mfsize)


# /\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\


# RSIDVAR
# -------

# Load the lookup table for rsID to VariantKey.
# The input binary file can be generated using the resources/tools/vkhexbin.sh script.
# This example uses the "c/test/data/rsvk.10.bin".
mfsrc, mffd, mfsize, mflast = vk.mmap_binfile('rsvk.10.bin')
if mffd < 0:
    assert False, "Unable to open the rsvk.10.bin file"

print(vk.get_rv_variantkey(mfsrc, item=3))
# 9223656209074749440

print(vk.find_rv_variantkey_by_rsid(mfsrc, 0, 9, rsid=0X00000061))
# (9223656209074749440, 3)

print(vk.get_next_rv_variantkey_by_rsid(mfsrc, 2, 9, 0x00000061))
# (9223656209074749440, 3)

vk.munmap_binfile(mfsrc, mffd, mfsize)

# - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

# Load the lookup table for rsID to VariantKey.
# The input binary file can be generated using the resources/tools/vkhexbin.sh script.
# This example uses the "c/test/data/rsvk.m.10.bin".
mfsrc, mffd, mfsize, mflast = vk.mmap_binfile('rsvk.m.10.bin')
if mffd < 0:
    assert False, "Unable to open the rsvk.m.10.bin file"

print(vk.find_all_rv_variantkey_by_rsid(mfsrc, 0, 9, 0x00000003))
# [9223656209074749440, 9223656316446408704, 9223656367992733696]

vk.munmap_binfile(mfsrc, mffd, mfsize)

# - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

# Load the lookup table for VariantKey ro rsID
# The input binary file can be generated using the resources/tools/vkhexbin.sh script.
# This example uses the "c/test/data/vkrs.10.bin".
mfsrc, mffd, mfsize, mflast = vk.mmap_binfile('vkrs.10.bin')
if mffd < 0:
    assert False, "Unable to open the vkrs.10.bin file"

print(vk.get_vr_rsid(mfsrc, item=3))
# 97

print(vk.find_vr_rsid_by_variantkey(mfsrc, 0, 9, vk=0X80010274003A0000))
# (97, 3)

print(vk.find_vr_chrompos_range(mfsrc, 0, 9, 0X14, 0X000256C5, 0X000256CB))
# (9973, 7, 8)

vk.munmap_binfile(mfsrc, mffd, mfsize)


# /\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\


# REGIONKEY
# ---------

print(vk.encode_region_strand(-1))
# 2

print(vk.decode_region_strand(2))
# -1

print(vk.encode_regionkey(25, 1000, 2000, 2))
# 14411520955069251204

print(vk.extract_regionkey_chrom(0xc80001f400003e84))
# 25

print(vk.extract_regionkey_startpos(0xc80001f400003e84))
# 1000

print(vk.extract_regionkey_endpos(0xc80001f400003e84))
# 2000

print(vk.extract_regionkey_strand(0xc80001f400003e84))
# 2

print(vk.decode_regionkey(0xc80001f400003e84))
# (25, 1000, 2000, 2)

print(vk.reverse_regionkey(0xc80001f400003e84))
# (b'MT', 1000, 2000, -1)

print(vk.regionkey("MT", 1000, 2000, -1))
# 14411520955069251204

print(vk.regionkey_hex(0xc80001f400003e84))
# b'c80001f400003e84'

print(vk.parse_regionkey_hex("c80001f400003e84"))
# 14411520955069251204

print(vk.get_regionkey_chrom_startpos(0xc80001f400003e84))
# 6710887400

print(vk.get_regionkey_chrom_endpos(0xc80001f400003e84))
# 6710888400

print(vk.are_overlapping_regions(5, 4,  6, 5, 3, 7))
# 1

print(vk.are_overlapping_region_regionkey(5, 4, 6, 0x2800000180000038))
# 1

print(vk.are_overlapping_regionkeys(0x2800000200000030, 0x2800000180000038))
# 1

print(vk.are_overlapping_variantkey_regionkey(None, 0, 0x2800000210920000, 0x2800000180000038))
# 1

print(vk.variantkey_to_regionkey(None, 0, 0x2800000210920000))
# 2882303770107052080


# /\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\


# ESID
# ---------

print(vk.encode_string_id("A0A022YWF9", 0))
# 599125005131282841

print(vk.decode_string_id(0x0850850492e77999))
(b'A0A022YWF9', 10)

print(vk.hash_string_id("0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ"))
# 12945031672818874332


# /\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\
