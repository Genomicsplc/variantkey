# R VariantKey Wrapper
#
# variantkey.R
#
# @category   Tools
# @author     Nicola Asuni <nicola.asuni@genomicsplc.com>
# @copyright  2017-2018 GENOMICS plc
# @license    MIT (see LICENSE)
# @link       https://github.com/genomicsplc/variantkey
#
# LICENSE
#
# Copyright (c) 2017-2018 GENOMICS plc
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in
# all copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
# THE SOFTWARE.

#' Returns chromosome encoding.
#' @param chrom Chromosome. An identifier from the reference genome, no white-space or leading zeros permitted.
#' @useDynLib variantkey R_encode_chrom
#' @export
EncodeChrom <- function(chrom) {
    return(.Call("R_encode_chrom", chrom))
}

#' Decode the CHROM code.
#' @param code Chromosome numerical code.
#' @useDynLib variantkey R_decode_chrom
#' @export
DecodeChrom <- function(code) {
    return(.Call("R_decode_chrom", code))
}

#' Returns reference+alternate encoding.
#' @param ref Reference allele. String containing a sequence of nucleotide letters.
#' @param alt Alternate non-reference allele string.
#' @useDynLib variantkey R_encode_refalt
#' @export
EncodeRefAlt <- function(ref, alt) {
    return(.Call("R_encode_refalt", ref, alt))
}

#' Decode the 32 bit REF+ALT code if reversible (if it has 11 or less bases in total and only contains ACGT letters).
#' @param code REF+ALT code
#' @useDynLib variantkey R_decode_refalt
#' @export
DecodeRefAlt <- function(code) {
    return(.Call("R_decode_refalt", code))
}

#' Returns a 64 bit variant key based on pre-encoded CHROM, POS (0-base) and REF+ALT.
#' @param chrom   Encoded Chromosome (see EncodeChrom)
#' @param pos     Position. The reference position, with the first base having position 0.
#' @param refalt  Encoded Reference + Alternate (see EncodeRefAlt)
#' @useDynLib   variantkey R_encode_variantkey
#' @export
EncodeVariantKey <- function(chrom, pos, refalt) {
    return(.Call("R_encode_variantkey", chrom, pos, refalt))
}

#' Extract the CHROM code from VariantKey.
#' @param vk VariantKey code.
#' @useDynLib   variantkey R_extract_variantkey_chrom
#' @export
ExtractVariantKeyChrom <- function(vk) {
    return(.Call("R_extract_variantkey_chrom", vk))
}

#' Extract the POS code from VariantKey.
#' @param vk VariantKey code.
#' @useDynLib   variantkey R_extract_variantkey_pos
#' @export
ExtractVariantKeyPos <- function(vk) {
    return(.Call("R_extract_variantkey_pos", vk))
}

#' Extract the REF+ALT code from VariantKey.
#' @param vk VariantKey code.
#' @useDynLib   variantkey R_extract_variantkey_refalt
#' @export
ExtractVariantKeyRefAlt <- function(vk) {
    return(.Call("R_extract_variantkey_refalt", vk))
}

#' Decode a VariantKey code and returns the components.
#' @param code VariantKey code.
#' @param vk   Decoded variantkey structure.
#' @useDynLib   variantkey R_decode_variantkey
#' @export
DecodeVariantKey <- function(vk) {
    return(.Call("R_decode_variantkey", vk))
}

#' Returns a 64 bit variant key based on CHROM, POS (0-base), REF, ALT.
#' @param chrom Chromosome. An identifier from the reference genome, no white-space or leading zeros permitted.
#' @param pos   Position. The reference position, with the first base having position 0.
#' @param ref   Reference allele. String containing a sequence of nucleotide letters.
#' @param alt   Alternate non-reference allele string.
#' @useDynLib   variantkey R_variantkey
#' @export
VariantKey <- function(chrom, pos, ref, alt) {
    return(.Call("R_variantkey", chrom, pos, ref, alt))
}

#' Returns minimum and maximum variant keys for range searches.
#' @param chrom   Chromosome numerical code.
#' @param pos_min Start reference position, with the first base having position 0.
#' @param pos_max End reference position, with the first base having position 0.
#' @useDynLib variantkey R_variantkey_range
#' @export
VariantKeyRange <- function(chrom, pos_min, pos_max) {
    return(.Call("R_variantkey_range", chrom, pos_min, pos_max))
}

#' Compares two VariantKeys by chromosome only.
#' Returns -1 if the first chromosome is smaller than the second, 0 if they are equal and 1 if the first is greater than the second.
#' @param vka    The first VariantKey to be compared.
#' @param vkb    The second VariantKey to be compared.
#' @useDynLib   variantkey R_compare_variantkey_chrom
#' @export
CompareVariantKeyChrom <- function(vka, vkb) {
    return(.Call("R_compare_variantkey_chrom", vka, vkb))
}

#' Compares two VariantKeys by chromosome and position.
#' Return -1 if the first CHROM+POS is smaller than the second, 0 if they are equal and 1 if the first is greater than the second.
#' @param vka    The first VariantKey to be compared.
#' @param vkb    The second VariantKey to be compared.
#' @useDynLib   variantkey R_compare_variantkey_chrom_pos
#' @export
CompareVariantKeyChromPos <- function(vka, vkb) {
    return(.Call("R_compare_variantkey_chrom_pos", vka, vkb))
}

# --- BINSEARCH ---

#' Memory map the specified file.
#' @param file  Path to the file to map.
#' @param mf    Structure containing the memory mapped file.
#' @useDynLib   variantkey R_mmap_binfile
#' @export
MmapBinfile <- function(file) {
    return(.Call("R_mmap_binfile", file))
}

#' Unmap and close the memory-mapped file.
#' On success, munmap() returns 0, on failure -1.
#' @param mf Descriptor of memory-mapped file.
#' @useDynLib   variantkey R_munmap_binfile
#' @export
MunmapBinfile <- function(src, fd, size) {
    return(.Call("R_munmap_binfile", src, fd, size))
}

# --- RSIDVAR ---

#' Returns the RSID at the specified position in the VR file.
#' @param src       Address of the memory mapped binary file containing the VariantKey to rsID lookup table (vkrs.bin).
#' @param item      Binary block number.
#' @useDynLib   variantkey R_get_vr_rsid
#' @export
GetVrRsid <- function(src, item) {
    return(.Call("R_get_vr_rsid", src, item))
}

#' Returns the VariantKey at the specified position in the RV file.
#' @param src       Address of the memory mapped binary file containing the rsID to VariantKey lookup table (rsvk.bin).
#' @param item      Binary block number.
#' @useDynLib   variantkey R_get_rv_variantkey
#' @export
GetRvVariantKey <- function(src, item) {
    return(.Call("R_get_rv_variantkey", src, item))
}

#' Search for the specified rsID and returns the first occurrence of VariantKey in the RV file, or zero if not found.
#' @param src       Address of the memory mapped binary file containing the rsID to VariantKey lookup table (rsvk.bin).
#' @param first     First element of the range to search (min value = 0).
#' @param last      Last element of the range to search (max value = nitems - 1).
#' @param rsid      rsID to search.
#' @useDynLib   variantkey R_find_rv_variantkey_by_rsid
#' @export
FindRvVariantKeyByRsid <- function(src, first, last, rsid) {
    return(.Call("R_find_rv_variantkey_by_rsid", src, first, last, rsid))
}

#' Get the next VariantKey for the specified rsID in the RV file, or 0 if not found
#' This function can be called in a loop to get all VariantKeys that are associated with the same rsID (if any).
#' @param src       Address of the memory mapped binary file containing the rsID to VariantKey lookup table (rsvk.bin).
#' @param pos       Current item.
#' @param last      Last element of the range to search (max value = nitems - 1).
#' @param rsid      rsID to search.
#' @useDynLib   variantkey R_get_next_rv_variantkey_by_rsid
#' @export
GetNextRvVariantKeyByRsid <- function(src, pos, last, rsid) {
    return(.Call("R_get_next_rv_variantkey_by_rsid", src, pos, last, rsid))
}

#' Search for the specified rsID and returns all the associated VariantKeys in the RV file.
#' NOTE that the output is limited to maximum 10 results.
#' @param src       Address of the memory mapped binary file containing the rsID to VariantKey lookup table (rsvk.bin).
#' @param first     First element of the range to search (min value = 0).
#' @param last      Last element of the range to search (max value = nitems - 1).
#' @param rsid      rsID to search.
#' @useDynLib   variantkey R_find_all_rv_variantkey_by_rsid
#' @export
FindAllRvVariantKeyByRsid <- function(src, first, last, rsid) {
    return(.Call("R_find_all_rv_variantkey_by_rsid", src, first, last, rsid))
}

#' Search for the specified VariantKey and returns the first occurrence of rsID in the VR file, or 0 if not found
#' @param src       Address of the memory mapped binary file containing the VariantKey to rsID lookup table (vkrs.bin).
#' @param first     First element of the range to search (min value = 0).
#' @param last      Last element of the range to search (max value = nitems - 1).
#' @param vk        VariantKey.
#' @useDynLib   variantkey R_find_vr_rsid_by_variantkey
#' @export
FindVrRsidByVariantKey <- function(src, first, last, vk) {
    return(.Call("R_find_vr_rsid_by_variantkey", src, first, last, vk))
}

#' Search for the specified CHROM-POS range and returns the first occurrence of rsID in the VR file.
#' @param src       Address of the memory mapped binary file containing the VariantKey to rsID lookup table (vkrs.bin).
#' @param first     First element of the range to search (min value = 0).
#' @param last      Last element of the range to search (max value = nitems - 1).
#' @param chrom     Chromosome encoded number.
#' @param pos_min   Start reference position, with the first base having position 0.
#' @param pos_max   End reference position, with the first base having position 0.
#' @useDynLib   variantkey R_find_vr_chrompos_range
#' @export
FindVrChromposRange <- function(src, first, last, chrom, pos_min, pos_max) {
    return(.Call("R_find_vr_chrompos_range", src, first, last, chrom, pos_min, pos_max))
}

# --- NRVK ---

#' Retrieve the REF and ALT strings for the specified VariantKey.
#' Return REF+ALT length or 0 if the VariantKey is not found.
#' @param src      Address of the memory mapped input file containing the VariantKey to REF+ALT lookup table (nrvk.bin).
#' @param last     Number of variants in the src file -1.
#' @param vk       VariantKey to search.
#' @useDynLib   variantkey R_find_ref_alt_by_variantkey
#' @export
FindRefAltByVariantKey <- function(src, last, vk) {
    return(.Call("R_find_ref_alt_by_variantkey", src, last, vk))
}

#' Reverse a VariantKey code and returns the normalized components.
#' @param src      Address of the memory mapped input file containing the VariantKey to REF+ALT lookup table (nrvk.bin).
#' @param last     Number of variants in the src file -1. Set this to 0 to skip the lookup table.
#' @param vk       VariantKey code.
#' @useDynLib   variantkey R_reverse_variantkey
#' @export
ReverseVariantKey <- function(src, last, vk) {
    return(.Call("R_reverse_variantkey", src, last, vk))
}

#' Retrieve the REF length for the specified VariantKey.
#' Return REF length or 0 if the VariantKey is not reversible and not found.
#' @param src      Address of the memory mapped input file containing the VariantKey to REF+ALT lookup table (nrvk.bin).
#' @param last     Number of variants in the src file -1.
#' @param vk       VariantKey.
#' @useDynLib   variantkey R_get_variantkey_ref_length
#' @export
GetVariantKeyRefLength <- function(src, last, vk) {
    return(.Call("R_get_variantkey_ref_length", src, last, vk))
}

#' Get the VariantKey end position (POS + REF length).
#' Return variant end position.
#' @param src      Address of the memory mapped input file containing the VariantKey to REF+ALT lookup table (nrvk.bin).
#' @param last     Number of variants in the src file -1.
#' @param vk       VariantKey.
#' @useDynLib   variantkey R_get_variantkey_endpos
#' @export
GetVariantKeyEndPos <- function(src, last, vk) {
    return(.Call("R_get_variantkey_endpos", src, last, vk))
}

#' Get the CHROM + START POS encoding from VariantKey.
#' @param vk       VariantKey.
#' @useDynLib   variantkey R_get_variantkey_chrom_startpos
#' @export
GetVariantKeyChromStartPos <- function(vk) {
    return(.Call("R_get_variantkey_chrom_startpos", vk))
}

#' Get the CHROM + END POS encoding from VariantKey.
#' @param src      Address of the memory mapped input file containing the VariantKey to REF+ALT lookup table (nrvk.bin).
#' @param last     Number of variants in the src file -1.
#' @param vk       VariantKey.
#' @useDynLib   variantkey R_get_variantkey_chrom_endpos
#' @export
GetVariantKeyChromEndPos <- function(src, last, vk) {
    return(.Call("R_get_variantkey_chrom_endpos", src, last, vk))
}

#' Convert a vrnr.bin file to a simple TSV.
#' Return Number of written bytes or 0 in case of error.
#' For the reverse operation see the resources/tools/nrvk.sh script.
#' @param src      Address of the memory mapped input file containing the VariantKey to REF+ALT lookup table (nrvk.bin).
#' @param last     Number of variants in the src file -1. Set this to 0 to skip the lookup table.
#' @param tsvfile  Output tsv file name. Note that existing files will be replaced.
#' @useDynLib   variantkey R_nrvk_bin_to_tsv
#' @export
VknrBinToTsv <- function(src, last, tsvfile) {
    return(.Call("R_nrvk_bin_to_tsv", src, last, tsvfile))
}

# --- GENOREF ---

#' Load the genome reference index.
#' @param src  Address of the memory mapped input file containing the genome reference data (fasta.bin).
#' @param idx  Index of sequences offset by chromosome number (1 to 25). The index 26 contains the file length. Requires 27 elements.
#' @useDynLib   variantkey R_load_genoref_index
#' @export
LoadGenorefIndex <- function(src) {
    return(.Call("R_load_genoref_index", src))
}

#' Returns the genome reference nucleotide at the specified chromosome and position, or 0 in case of invalid position.
#' @param src     Address of the memory mapped input file containing the genome reference data (fasta.bin).
#' @param idx     Index of sequences offset by chromosome number (1 to 25).
#' @param chrom   Encoded Chromosome number (see encode_chrom).
#' @param pos     Position. The reference position, with the first base having position 0.
#' @useDynLib   variantkey R_get_genoref_seq
#' @export
GetGenorefSeq <- function(src, idx, chrom, pos) {
    return(.Call("R_get_genoref_seq", src, idx, chrom, pos))
}

#' Check if the reference allele matches the reference genome data.
#' Return Positive number in case of success, negative in case of error:
#'   *  0 the reference allele match the reference genome;
#'   *  1 the reference allele is inconsistent with the genome reference (i.e. when contains nucleotide letters other than A, C, G and T);
#'   * -1 the reference allele don't match the reference genome;
#'   * -2 the reference allele is longer than the genome reference sequence.
#' @param src     Address of the memory mapped input file containing the genome reference data (fasta.bin).
#' @param idx     Index of sequences offset by chromosome number (1 to 25).
#' @param chrom   Encoded Chromosome number (see encode_chrom).
#' @param pos     Position. The reference position, with the first base having position 0.
#' @param ref     Reference allele. String containing a sequence of nucleotide letters.
#' @useDynLib   variantkey R_check_reference
#' @export
CheckReference <- function(src, idx, chrom, pos, ref) {
    return(.Call("R_check_reference", src, idx, chrom, pos, ref))
}

#' Flip the allele nucleotides (replaces each letter with its complement).
#' The resulting string is always in uppercase.
#' Support extended nucleotide letters.
#' @param allele  Allele. String containing a sequence of nucleotide letters.
#' @param size    Length of the allele string.
#' @useDynLib   variantkey R_flip_allele
#' @export
FlipAllele <- function(allele) {
    return(.Call("R_flip_allele", allele))
}

#' Normalize a variant.
#' Flip alleles if required and apply the normalization algorithm described at:
#' https://genome.sph.umich.edu/wiki/Variant_Normalization
#' Return Positive bitmask number in case of success, negative number in case of error.
#' When positive, each bit has a different meaning when set, has defined by the NORM_* defines:
#'   * bit 0:
#'       Reference allele is inconsistent with the genome reference (i.e. when contains nucleotide letters other than A, C, G and T).
#'   * bit 1:
#'       Alleles have been swapped.
#'   * bit 2:
#'       Alleles nucleotides have been flipped (each nucleotide have been replaced with its complement).
#'   * bit 3:
#'       Alleles have been left extended.
#'   * bit 4:
#'       Alleles have been right trimmed.
#'   * bit 5:
#'       Alleles have been left trimmed.
#' @param src        Address of the memory mapped input file containing the genome reference data (fasta.bin).
#' @param idx        Index of sequences offset by chromosome number (1 to 25).
#' @param chrom      Chromosome encoded number.
#' @param pos        Position. The reference position, with the first base having position 0.
#' @param ref        Reference allele. String containing a sequence of nucleotide letters.
#' @param alt        Alternate non-reference allele string.
#' @useDynLib   variantkey R_normalize_variant
#' @export
NormalizeVariant <- function(src, idx, chrom, pos, ref, alt) {
    return(.Call("R_normalize_variant", src, idx, chrom, pos, ref, alt))
}

# --- REGIONKEY ---

#' Encode the strand direction (-1 > 2, 0 > 0, +1 > 1).
#' @param strand     Strand direction (-1, 0, +1).
#' @useDynLib   variantkey R_encode_region_strand
#' @export
EncodeRegionStrand <- function(strand) {
    return(.Call("R_encode_region_strand", strand))
}

#' Decode the strand direction code (0 > 0, 1 > +1, 2 > -1).
#' @param code     Strand code.
#' @useDynLib   variantkey R_decode_region_strand
#' @export
DecodeRegionStrand <- function(strand) {
    return(.Call("R_decode_region_strand", strand))
}

#' Returns a 64 bit regionkey
#' @param chrom      Encoded Chromosome (see encode_chrom).
#' @param startpos   Start position (zero based).
#' @param endpos     End position (startpos + region_length).
#' @param strand     Encoded Strand direction (-1 > 2, 0 > 0, +1 > 1)
#' @useDynLib   variantkey R_encode_regionkey
#' @export
EncodeRegionKey <- function(chrom, startpos, endpos, strand) {
    return(.Call("R_encode_regionkey", chrom, startpos, endpos, strand))
}

#' Extract the CHROM code from RegionKey.
#' @param rk RegionKey code.
#' @useDynLib   variantkey R_extract_regionkey_chrom
#' @export
ExtractRegionKeyChrom <- function(rk) {
    return(.Call("R_extract_regionkey_chrom", rk))
}

#' Extract the START POS code from RegionKey.
#' @param rk RegionKey code.
#' @useDynLib   variantkey R_extract_regionkey_startpos
#' @export
ExtractRegionKeyStartPos <- function(rk) {
    return(.Call("R_extract_regionkey_startpos", rk))
}

#' Extract the END POS code from RegionKey.
#' @param rk RegionKey code.
#' @useDynLib   variantkey R_extract_regionkey_endpos
#' @export
ExtractRegionKeyEndPos <- function(rk) {
    return(.Call("R_extract_regionkey_endpos", rk))
}

#' Extract the STRAND from RegionKey.
#' @param rk RegionKey code.
#' @useDynLib   variantkey R_extract_regionkey_strand
#' @export
ExtractRegionKeyStrand <- function(rk) {
    return(.Call("R_extract_regionkey_strand", rk))
}

#' Decode a RegionKey code and returns the components as regionkey_t structure.
#' @param rk RegionKey code.
#' @useDynLib   variantkey R_decode_regionkey
#' @export
DecodeRegionKey <- function(rk) {
    return(.Call("R_decode_regionkey", rk))
}

#' Reverse a RegionKey code and returns the normalized components as regionkey_rev_t structure.
#' @param rk       RegionKey code.
#' @useDynLib   variantkey R_reverse_regionkey
#' @export
ReverseRegionKey <- function(rk) {
    return(.Call("R_reverse_regionkey", rk))
}

#' Returns a 64 bit regionkey based on CHROM, START POS (0-based), END POS and STRAND.
#' @param chrom      Chromosome. An identifier from the reference genome, no white-space or leading zeros permitted.
#' @param sizechrom  Length of the chrom string, excluding the terminating null byte.
#' @param startpos   Start position (zero based).
#' @param endpos     End position (startpos + region_length).
#' @param strand     Strand direction (-1, 0, +1)
#' @useDynLib   variantkey R_regionkey
#' @export
RegionKey <- function(chrom, startpos, endpos, strand) {
    return(.Call("R_regionkey", chrom, startpos, endpos, strand))
}

#' Get the CHROM + START POS encoding from RegionKey.
#' @param rk RegionKey code.
#' @useDynLib   variantkey R_get_regionkey_chrom_startpos
#' @export
GetRegionKeyChromStartPos <- function(rk) {
    return(.Call("R_get_regionkey_chrom_startpos", rk))
}

#' Get the CHROM + END POS encoding from RegionKey.
#' @param rk RegionKey code.
#' @useDynLib   variantkey R_get_regionkey_chrom_endpos
#' @export
GetRegionKeyChromEndPos <- function(rk) {
    return(.Call("R_get_regionkey_chrom_endpos", rk))
}

#' Check if two regions are overlapping.
#' Return 1 if the regions overlap, 0 otherwise.
#' @param a_chrom     Region A chromosome code.
#' @param a_startpos  Region A start position.
#' @param a_endpos    Region A end position (startpos + region length).
#' @param b_chrom     Region B chromosome code.
#' @param b_startpos  Region B start position.
#' @param b_endpos    Region B end position (startpos + region length).
#' @useDynLib   variantkey R_are_overlapping_regions
#' @export
AreOverlappingRegions <- function(a_chrom, a_startpos, a_endpos, b_chrom, b_startpos, b_endpos) {
    return(.Call("R_are_overlapping_regions", a_chrom, a_startpos, a_endpos, b_chrom, b_startpos, b_endpos))
}

#' Check if a region and a regionkey are overlapping.
#' Return 1 if the regions overlap, 0 otherwise.
#' @param chrom     Region A chromosome code.
#' @param startpos  Region A start position.
#' @param endpos    Region A end position (startpos + region length).
#' @param rk        RegionKey or region B.
#' @useDynLib   variantkey R_are_overlapping_region_regionkey
#' @export
AreOverlappingRegionRegionKey <- function(chrom, startpos, endpos, rk) {
    return(.Call("R_are_overlapping_region_regionkey", chrom, startpos, endpos, rk))
}

#' Check if two regionkeys are overlapping.
#' Return 1 if the regions overlap, 0 otherwise.
#' @param rka        RegionKey A.
#' @param rkb        RegionKey B.
#' @useDynLib   variantkey R_are_overlapping_regionkeys
#' @export
AreOverlappingRegionKeys <- function(rka, rkb) {
    return(.Call("R_are_overlapping_regionkeys", rka, rkb))
}

#' Check if variantkey and regionkey are overlapping.
#' Return 1 if the regions overlap, 0 otherwise.
#' @param src   Address of the memory mapped input file containing the VariantKey to REF+ALT lookup table (nrvk.bin).
#' @param last  Number of variants in the src file -1.
#' @param vk    VariantKey code.
#' @param rk    RegionKey code.
#' @useDynLib   variantkey R_are_overlapping_variantkey_regionkey
#' @export
AreOverlappingVariantKeyRegionKey <- function(src, last, vk, rk) {
    return(.Call("R_are_overlapping_variantkey_regionkey", src, last, vk, rk))
}

#' Get RegionKey from VariantKey.
#' @param src   Address of the memory mapped input file containing the VariantKey to REF+ALT lookup table (nrvk.bin).
#' @param last  Number of variants in the src file -1.
#' @param vk    VariantKey code.
#' @useDynLib   variantkey R_variantkey_to_regionkey
#' @export
VariantToRegionkey <- function(src, last, vk) {
    return(.Call("R_variantkey_to_regionkey", src, last, vk))
}

# --- ESID ---

#' Encode maximum 10 characters of a string into a 64 bit unsigned integer.
#' This function can be used to convert generic string IDs to numeric IDs.
#' @param str    The string to encode. It must be maximum 10 characters long and support ASCII characters from '!' to 'z'.
#' @param start  First character to encode, starting from 0. To encode the last 10 characters, set this value at (size - 10).
#' @useDynLib   variantkey R_encode_string_id
#' @export
EncodeStringID <- function(str, start) {
    return(.Call("R_encode_string_id", str, start))
}

#' Encode a string composed by a character section followed by a separator character and a numerical section
#' into a 64 bit unsigned integer. For example: "ABCDE:0001234"
#' Encodes up to 5 characters in uppercase, a number up to 2^27, and up to 7 zero padding digits.
#' If the string is 10 character or less, then the EncodeStringID() is used.
#' @param str    The string to encode. It must be maximum 10 characters long and support ASCII characters from '!' to 'z'.
#' @param sep    Separator character between string and number.
#' @useDynLib   variantkey R_encode_string_num_id
#' @export
EncodeStringNumID <- function(str, sep) {
    return(.Call("R_encode_string_num_id", str, sep))
}

#' Decode the encoded string ID.
#' This function is the reverse of encode_string_id.
#' The string is always returned in uppercase mode.
#' @param esid   Encoded string ID code.
#' @useDynLib   variantkey R_decode_string_id
#' @export
DecodeStringID <- function(esid) {
    return(.Call("R_decode_string_id", esid))
}

#' Hash the input string into a 64 bit unsigned integer.
#' This function can be used to convert long string IDs to numeric IDs.
#' @param str    The string to encode.
#' @useDynLib   variantkey R_hash_string_id
#' @export
HashStringID <- function(str) {
    return(.Call("R_hash_string_id", str))
}
