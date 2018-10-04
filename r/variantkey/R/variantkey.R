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

ERR_INPUT_LENGTH <- "Error: vector inputs must have the same length."

#' Returns chromosome encoding.
#' @param chrom Chromosome. An identifier from the reference genome, no white-space or leading zeros permitted.
#' @useDynLib variantkey R_encode_chrom
#' @export
EncodeChrom <- function(chrom) {
    ret <- integer(length(chrom))
    return(.Call("R_encode_chrom", as.character(chrom), ret))
}

#' Decode the CHROM code.
#' @param code Chromosome numerical code.
#' @useDynLib variantkey R_decode_chrom
#' @export
DecodeChrom <- function(code) {
    ret <- character(length(code))
    return(.Call("R_decode_chrom", as.integer(code), ret))
}

#' Returns reference+alternate encoding.
#' @param ref Reference allele. String containing a sequence of nucleotide letters.
#' @param alt Alternate non-reference allele string.
#' @useDynLib variantkey R_encode_refalt
#' @export
EncodeRefAlt <- function(ref, alt) {
    n <- length(ref)
    if (n != length(alt)) {
        stop(ERR_INPUT_LENGTH)
    }
    ret <- integer(n)
    return(.Call("R_encode_refalt", as.character(ref), as.character(alt), ret))
}

#' Decode the 32 bit REF+ALT code if reversible (if it has 11 or less bases in total and only contains ACGT letters).
#' @param code REF+ALT code
#' @useDynLib variantkey R_decode_refalt
#' @export
DecodeRefAlt <- function(code) {
    ref <- character(length(code))
    alt <- character(length(code))
    return(.Call("R_decode_refalt", as.integer(code), ref, alt))
}

#' Returns a 64 bit variant key based on pre-encoded CHROM, POS (0-base) and REF+ALT.
#' @param chrom   Encoded Chromosome (see EncodeChrom)
#' @param pos     Position. The reference position, with the first base having position 0.
#' @param refalt  Encoded Reference + Alternate (see EncodeRefAlt)
#' @useDynLib   variantkey R_encode_variantkey
#' @export
EncodeVariantKey <- function(chrom, pos, refalt) {
    n <- length(chrom)
    if ((n != length(pos)) || (n != length(refalt))) {
        stop(ERR_INPUT_LENGTH)
    }
    ret <- uint64(n)
    return(.Call("R_encode_variantkey", as.integer(chrom), as.integer(pos), as.integer(refalt), ret))
}

#' Extract the CHROM code from VariantKey.
#' @param vk VariantKey code.
#' @useDynLib   variantkey R_extract_variantkey_chrom
#' @export
ExtractVariantKeyChrom <- function(vk) {
    ret <- integer(length(vk))
    return(.Call("R_extract_variantkey_chrom", as.unit64(vk), ret))
}

#' Extract the POS code from VariantKey.
#' @param vk VariantKey code.
#' @useDynLib   variantkey R_extract_variantkey_pos
#' @export
ExtractVariantKeyPos <- function(vk) {
    ret <- integer(length(vk))
    return(.Call("R_extract_variantkey_pos", as.unit64(vk), ret))
}

#' Extract the REF+ALT code from VariantKey.
#' @param vk VariantKey code.
#' @useDynLib   variantkey R_extract_variantkey_refalt
#' @export
ExtractVariantKeyRefAlt <- function(vk) {
    ret <- integer(length(vk))
    return(.Call("R_extract_variantkey_refalt", as.unit64(vk), ret))
}

#' Decode a VariantKey code and returns the components.
#' @param code VariantKey code.
#' @param vk   Decoded variantkey structure.
#' @useDynLib   variantkey R_decode_variantkey
#' @export
DecodeVariantKey <- function(vk) {
    n <- length(vk)
    chrom <- integer(n)
    pos <- integer(n)
    refalt <- integer(n)
    return(.Call("R_decode_variantkey", as.unit64(vk), chrom, pos, refalt))
}

#' Returns a 64 bit variant key based on CHROM, POS (0-base), REF, ALT.
#' @param chrom Chromosome. An identifier from the reference genome, no white-space or leading zeros permitted.
#' @param pos   Position. The reference position, with the first base having position 0.
#' @param ref   Reference allele. String containing a sequence of nucleotide letters.
#' @param alt   Alternate non-reference allele string.
#' @useDynLib   variantkey R_variantkey
#' @export
VariantKey <- function(chrom, pos, ref, alt) {
    n <- length(pos)
    if ((n != length(chrom)) || (n != length(ref)) || (n != length(alt))) {
        stop(ERR_INPUT_LENGTH)
    }
    ret <- uint64(n)
    return(.Call("R_variantkey", as.character(chrom), as.integer(pos), as.character(ref), as.character(alt), ret))
}

#' Returns minimum and maximum variant keys for range searches.
#' @param chrom   Chromosome numerical code.
#' @param pos_min Start reference position, with the first base having position 0.
#' @param pos_max End reference position, with the first base having position 0.
#' @useDynLib variantkey R_variantkey_range
#' @export
VariantKeyRange <- function(chrom, pos_min, pos_max) {
    n <- length(chrom)
    if ((n != length(pos_min)) || (n != length(pos_max))) {
        stop(ERR_INPUT_LENGTH)
    }
    min <- uint64(n)
    max <- uint64(n)
    return(.Call("R_variantkey_range", as.integer(chrom), as.integer(pos_min), as.integer(pos_max), min, max))
}

#' Compares two VariantKeys by chromosome only.
#' Returns -1 if the first chromosome is smaller than the second, 0 if they are equal and 1 if the first is greater than the second.
#' @param vka    The first VariantKey to be compared.
#' @param vkb    The second VariantKey to be compared.
#' @useDynLib   variantkey R_compare_variantkey_chrom
#' @export
CompareVariantKeyChrom <- function(vka, vkb) {
    n <- length(vka)
    if (n != length(vkb)) {
        stop(ERR_INPUT_LENGTH)
    }
    ret <- integer(n)
    return(.Call("R_compare_variantkey_chrom", as.uint64(vka), as.uint64(vkb), ret))
}

#' Compares two VariantKeys by chromosome and position.
#' Return -1 if the first CHROM+POS is smaller than the second, 0 if they are equal and 1 if the first is greater than the second.
#' @param vka    The first VariantKey to be compared.
#' @param vkb    The second VariantKey to be compared.
#' @useDynLib   variantkey R_compare_variantkey_chrom_pos
#' @export
CompareVariantKeyChromPos <- function(vka, vkb) {
    n <- length(vka)
    if (n != length(vkb)) {
        stop(ERR_INPUT_LENGTH)
    }
    ret <- integer(n)
    return(.Call("R_compare_variantkey_chrom_pos", as.uint64(vka), as.uint64(vkb), ret))
}

#' Returns VariantKey hexadecimal string (16 characters).
#' @param vk    VariantKey code.
#' @useDynLib   variantkey R_variantkey_hex
#' @export
VariantKeyHex <- function(vk) {
    ret <- character(length(vk))
    return(.Call("R_variantkey_hex", as.uint64(vk), ret))
}

#' Parses a VariantKey hexadecimal string and returns the code.
#' @param hex    VariantKey hexadecimal string (it must contain 16 hexadecimal characters).
#' @useDynLib   variantkey R_parse_variantkey_hex
#' @export
ParseVariantKeyHex <- function(hex) {
    ret <- uint64(length(hex))
    return(.Call("R_parse_variantkey_hex", as.character(hex), ret))
}

# --- BINSEARCH ---

#' Unmap and close the memory-mapped file.
#' On success, munmap() returns 0, on failure -1.
#' @param mf Descriptor of memory-mapped file.
#' @useDynLib   variantkey R_munmap_binfile
#' @export
MunmapBinfile <- function(mf) {
    return(.Call("R_munmap_binfile", mf))
}

# --- RSIDVAR ---

#' Memory map the specified RSVK binary file (rsvk.bin).
#' Returns the memory mapped file object, the columns object and the number of rows.
#' @param file    Path to the file to map.
#' @param ctbytes List containing the number of bytes for each column type (i.e. 1 for uint8, 2 for uint16, 4 for uint32, 8 for uint64)
#' @useDynLib   variantkey R_mmap_rsvk_file
#' @export
MmapRSVKFile <- function(file, ctbytes) {
    return(.Call("R_mmap_rsvk_file", file, ctbytes))
}

#' Memory map the specified VKRS binary file (vkrs.bin).
#' Returns the memory mapped file object, the columns object and the number of rows.
#' @param file    Path to the file to map.
#' @param ctbytes List containing the number of bytes for each column type (i.e. 1 for uint8, 2 for uint16, 4 for uint32, 8 for uint64)
#' @useDynLib   variantkey R_mmap_vkrs_file
#' @export
MmapVKRSFile <- function(file, ctbytes) {
    return(.Call("R_mmap_vkrs_file", file, ctbytes))
}

#' Search for the specified rsID and returns the first occurrence of VariantKey in the RV file, or zero if not found.
#' @param mc        Memory-mapped columns object as retured by MmapRSVKfile.
#' @param first     First element of the range to search (min value = 0).
#' @param last      Element (up to but not including) where to end the search (max value = nitems).
#' @param rsid      rsID to search.
#' @useDynLib   variantkey R_find_rv_variantkey_by_rsid
#' @export
FindRvVariantKeyByRsid <- function(mc, first, last, rsid) {
    n <- length(rsid)
    vk <- uint64(n)
    rfirst <- integer(n)
    return(.Call("R_find_rv_variantkey_by_rsid", mc, as.integer(first), as.integer(last), as.integer(rsid), vk, rfirst))
}

#' Get the next VariantKey for the specified rsID in the RV file, or 0 if not found
#' This function can be called in a loop to get all VariantKeys that are associated with the same rsID (if any).
#' @param mc        Memory-mapped columns object as retured by MmapRSVKfile.
#' @param pos       Current item.
#' @param last      Element (up to but not including) where to end the search (max value = nitems).
#' @param rsid      rsID to search.
#' @useDynLib   variantkey R_get_next_rv_variantkey_by_rsid
#' @export
GetNextRvVariantKeyByRsid <- function(mc, pos, last, rsid) {
    n <- length(rsid)
    vk <- uint64(n)
    rpos <- integer(n)
    return(.Call("R_get_next_rv_variantkey_by_rsid", mc, as.integer(pos), as.integer(last), as.integer(rsid), vk, rpos))
}

#' Search for the specified rsID and returns all the associated VariantKeys in the RV file.
#' NOTE: the output is limited to maximum 10 results.
#' @param mc        Memory-mapped columns object as retured by MmapRSVKfile.
#' @param first     First element of the range to search (min value = 0).
#' @param last      Element (up to but not including) where to end the search (max value = nitems).
#' @param rsid      rsID to search.
#' @useDynLib   variantkey R_find_all_rv_variantkey_by_rsid
#' @export
FindAllRvVariantKeyByRsid <- function(mc, first, last, rsid) {
    return(.Call("R_find_all_rv_variantkey_by_rsid", mc, as.integer(first), as.integer(last), as.integer(rsid)))
}

#' Search for the specified VariantKey and returns the first occurrence of rsID in the VR file, or 0 if not found
#' @param mc        Memory-mapped columns object as retured by MmapVKRSfile.
#' @param first     First element of the range to search (min value = 0).
#' @param last      Element (up to but not including) where to end the search (max value = nitems).
#' @param vk        VariantKey.
#' @useDynLib   variantkey R_find_vr_rsid_by_variantkey
#' @export
FindVrRsidByVariantKey <- function(mc, first, last, vk) {
    len <- length(vk)
    rsid <- integer(len)
    rfirst <- integer(len)
    return(.Call("R_find_vr_rsid_by_variantkey", mc, as.integer(first), as.integer(last), as.uint64(vk), rsid, rfirst))
}

#' Search for the specified CHROM-POS range and returns the first occurrence of rsID in the VR file.
#' @param mc        Memory-mapped columns object as retured by MmapVKRSfile.
#' @param first     First element of the range to search (min value = 0).
#' @param last      Element (up to but not including) where to end the search (max value = nitems).
#' @param chrom     Chromosome encoded number.
#' @param pos_min   Start reference position, with the first base having position 0.
#' @param pos_max   End reference position, with the first base having position 0.
#' @useDynLib   variantkey R_find_vr_chrompos_range
#' @export
FindVrChromposRange <- function(mc, first, last, chrom, pos_min, pos_max) {
    n <- length(chrom)
    if ((n != length(pos_min)) || (n != length(pos_max))) {
        stop(ERR_INPUT_LENGTH)
    }
    rsid <- integer(n)
    rfirst <- uint64(n)
    rlast <- integer(n)
    return(.Call("R_find_vr_chrompos_range", mc, as.integer(first), as.integer(last), as.integer(chrom), as.integer(pos_min), as.integer(pos_max), rsid, rfirst, rlast))
}

# --- NRVK ---

#' Memory map the specified NRVK binary file (nrvk.bin).
#' Returns the memory mapped file object, the columns object and the number of rows.
#' @param file    Path to the file to map.
#' @useDynLib   variantkey R_mmap_nrvk_file
#' @export
MmapNRVKFile <- function(file) {
    return(.Call("R_mmap_nrvk_file", file))
}

#' Retrieve the REF and ALT strings for the specified VariantKey.
#' Return REF+ALT length or 0 if the VariantKey is not found.
#' @param mc       Memory-mapped columns object as retured by MmapNRVKfile.
#' @param vk       VariantKey to search.
#' @useDynLib   variantkey R_find_ref_alt_by_variantkey
#' @export
FindRefAltByVariantKey <- function(mc, vk) {
    n <- length(vk)
    ref <- character(n)
    alt <- character(n)
    return(.Call("R_find_ref_alt_by_variantkey", mc, as.uint64(vk), ref, alt))
}

#' Reverse a VariantKey code and returns the normalized components.
#' @param mc       Memory-mapped columns object as retured by MmapNRVKfile.
#' @param vk       VariantKey code.
#' @useDynLib   variantkey R_reverse_variantkey
#' @export
ReverseVariantKey <- function(mc, vk) {
    n <- length(vk)
    chrom <- character(n)
    pos <- integer(n)
    ref <- character(n)
    alt <- character(n)
    return(.Call("R_reverse_variantkey", mc, as.uint64(vk), chrom, pos, ref, alt))
}

#' Retrieve the REF length for the specified VariantKey.
#' Return REF length or 0 if the VariantKey is not reversible and not found.
#' @param mc       Memory-mapped columns object as retured by MmapNRVKfile.
#' @param vk       VariantKey.
#' @useDynLib   variantkey R_get_variantkey_ref_length
#' @export
GetVariantKeyRefLength <- function(mc, vk) {
    ret <- integer(length(vk))
    return(.Call("R_get_variantkey_ref_length", mc, as.uint64(vk), ret))
}

#' Get the VariantKey end position (POS + REF length).
#' Return variant end position.
#' @param mc       Memory-mapped columns object as retured by MmapNRVKfile.
#' @param vk       VariantKey.
#' @useDynLib   variantkey R_get_variantkey_endpos
#' @export
GetVariantKeyEndPos <- function(mc, vk) {
    ret <- integer(length(vk))
    return(.Call("R_get_variantkey_endpos", mc, as.uint64(vk), ret))
}

#' Get the CHROM + START POS encoding from VariantKey.
#' @param vk       VariantKey.
#' @useDynLib   variantkey R_get_variantkey_chrom_startpos
#' @export
GetVariantKeyChromStartPos <- function(vk) {
    ret <- uint64(length(vk))
    return(.Call("R_get_variantkey_chrom_startpos", as.uint64(vk), ret))
}

#' Get the CHROM + END POS encoding from VariantKey.
#' @param mc       Memory-mapped columns object as retured by MmapNRVKfile.
#' @param vk       VariantKey.
#' @useDynLib   variantkey R_get_variantkey_chrom_endpos
#' @export
GetVariantKeyChromEndPos <- function(mc, vk) {
    ret <- uint64(length(vk))
    return(.Call("R_get_variantkey_chrom_endpos", mc, as.uint64(vk), ret))
}

#' Convert a vrnr.bin file to a simple TSV.
#' Return Number of written bytes or 0 in case of error.
#' For the reverse operation see the resources/tools/nrvk.sh script.
#' @param mc       Memory-mapped columns object as retured by MmapNRVKfile.
#' @param tsvfile  Output tsv file name. Note that existing files will be replaced.
#' @useDynLib   variantkey R_nrvk_bin_to_tsv
#' @export
VknrBinToTsv <- function(mc, tsvfile) {
    return(.Call("R_nrvk_bin_to_tsv", mc, tsvfile))
}

# --- GENOREF ---

#' Memory map the specified genome reference binary file (fasta.bin).
#' Returns the memory mapped file object, the columns object and the number of rows.
#' @param file    Path to the file to map.
#' @useDynLib   variantkey R_mmap_genoref_file
#' @export
MmapGenorefFile <- function(file) {
    return(.Call("R_mmap_genoref_file", file))
}

#' Returns the genome reference nucleotide at the specified chromosome and position, or 0 in case of invalid position.
#' @param mc      Memory-mapped columns object as retured by MmapGenorefFile.
#' @param chrom   Encoded Chromosome number (see encode_chrom).
#' @param pos     Position. The reference position, with the first base having position 0.
#' @useDynLib   variantkey R_get_genoref_seq
#' @export
GetGenorefSeq <- function(mf, chrom, pos) {
    n <- length(chrom)
    if (n != length(pos)) {
        stop(ERR_INPUT_LENGTH)
    }
    ret <- integer(n)
    return(.Call("R_get_genoref_seq", mf, as.integer(chrom), as.integer(pos), ret))
}

#' Check if the reference allele matches the reference genome data.
#' Return Positive number in case of success, negative in case of error:
#'   *  0 the reference allele match the reference genome;
#'   *  1 the reference allele is inconsistent with the genome reference (i.e. when contains nucleotide letters other than A, C, G and T);
#'   * -1 the reference allele don't match the reference genome;
#'   * -2 the reference allele is longer than the genome reference sequence.
#' @param mc      Memory-mapped columns object as retured by MmapGenorefFile.
#' @param chrom   Encoded Chromosome number (see encode_chrom).
#' @param pos     Position. The reference position, with the first base having position 0.
#' @param ref     Reference allele. String containing a sequence of nucleotide letters.
#' @useDynLib   variantkey R_check_reference
#' @export
CheckReference <- function(mf, chrom, pos, ref) {
    n <- length(chrom)
    if ((n != length(pos)) || (n != length(ref))) {
        stop(ERR_INPUT_LENGTH)
    }
    ret <- integer(n)
    return(.Call("R_check_reference", mf, as.integer(chrom), as.integer(pos), as.character(ref), ret))
}

#' Flip the allele nucleotides (replaces each letter with its complement).
#' The resulting string is always in uppercase.
#' Support extended nucleotide letters.
#' @param allele  Allele. String containing a sequence of nucleotide letters.
#' @param size    Length of the allele string.
#' @useDynLib   variantkey R_flip_allele
#' @export
FlipAllele <- function(allele) {
    ref <- character(length(allele))
    return(.Call("R_flip_allele", as.character(allele), ret))
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
#' @param mc      Memory-mapped columns object as retured by MmapGenorefFile.
#' @param chrom      Chromosome encoded number.
#' @param pos        Position. The reference position, with the first base having position 0.
#' @param ref        Reference allele. String containing a sequence of nucleotide letters.
#' @param alt        Alternate non-reference allele string.
#' @useDynLib   variantkey R_normalize_variant
#' @export
NormalizeVariant <- function(mf, chrom, pos, ref, alt) {
    n <- length(chrom)
    if ((n != length(pos)) || (n != length(ref)) || (n != length(alt))) {
        stop(ERR_INPUT_LENGTH)
    }
    rpos <- integer(n)
    rref <- character(n)
    ralt <- character(n)
    return(.Call("R_normalize_variant", mf, as.integer(chrom), as.integer(pos), as.character(ref), as.character(alt), rpos, rref, ralt))
}

# --- REGIONKEY ---

#' Encode the strand direction (-1 > 2, 0 > 0, +1 > 1).
#' @param strand     Strand direction (-1, 0, +1).
#' @useDynLib   variantkey R_encode_region_strand
#' @export
EncodeRegionStrand <- function(strand) {
    ret <- integer(length(strand))
    return(.Call("R_encode_region_strand", as.integer(strand), ret))
}

#' Decode the strand direction code (0 > 0, 1 > +1, 2 > -1).
#' @param code     Strand code.
#' @useDynLib   variantkey R_decode_region_strand
#' @export
DecodeRegionStrand <- function(strand) {
    ret <- integer(length(strand))
    return(.Call("R_decode_region_strand", as.integer(strand), ret))
}

#' Returns a 64 bit regionkey
#' @param chrom      Encoded Chromosome (see encode_chrom).
#' @param startpos   Start position (zero based).
#' @param endpos     End position (startpos + region_length).
#' @param strand     Encoded Strand direction (-1 > 2, 0 > 0, +1 > 1)
#' @useDynLib   variantkey R_encode_regionkey
#' @export
EncodeRegionKey <- function(chrom, startpos, endpos, strand) {
    n <- length(chrom)
    if ((n != length(startpos)) || (n != length(endpos)) || (n != length(strand))) {
        stop(ERR_INPUT_LENGTH)
    }
    ret <- uint64(n)
    return(.Call("R_encode_regionkey", as.integer(chrom), as.integer(startpos), as.integer(endpos), as.integer(strand), ret))
}

#' Extract the CHROM code from RegionKey.
#' @param rk RegionKey code.
#' @useDynLib   variantkey R_extract_regionkey_chrom
#' @export
ExtractRegionKeyChrom <- function(rk) {
    ret <- integer(length(rk))
    return(.Call("R_extract_regionkey_chrom", as.uint64(rk), ret))
}

#' Extract the START POS code from RegionKey.
#' @param rk RegionKey code.
#' @useDynLib   variantkey R_extract_regionkey_startpos
#' @export
ExtractRegionKeyStartPos <- function(rk) {
    ret <- integer(length(rk))
    return(.Call("R_extract_regionkey_startpos", as.uint64(rk), ret))
}

#' Extract the END POS code from RegionKey.
#' @param rk RegionKey code.
#' @useDynLib   variantkey R_extract_regionkey_endpos
#' @export
ExtractRegionKeyEndPos <- function(rk) {
    ret <- integer(length(rk))
    return(.Call("R_extract_regionkey_endpos", as.uint64(rk), ret))
}

#' Extract the STRAND from RegionKey.
#' @param rk RegionKey code.
#' @useDynLib   variantkey R_extract_regionkey_strand
#' @export
ExtractRegionKeyStrand <- function(rk) {
    ret <- integer(length(rk))
    return(.Call("R_extract_regionkey_strand", as.uint64(rk), ret))
}

#' Decode a RegionKey code and returns the components as regionkey_t structure.
#' @param rk RegionKey code.
#' @useDynLib   variantkey R_decode_regionkey
#' @export
DecodeRegionKey <- function(rk) {
    n <- length(rk)
    chrom <- integer(n)
    startpos <- integer(n)
    endpos <- integer(n)
    strand <- integer(n)
    return(.Call("R_decode_regionkey", as.uint64(rk), chrom, startpos, endpos, strand))
}

#' Reverse a RegionKey code and returns the normalized components as regionkey_rev_t structure.
#' @param rk       RegionKey code.
#' @useDynLib   variantkey R_reverse_regionkey
#' @export
ReverseRegionKey <- function(rk) {
    n <- length(rk)
    chrom <- character(n)
    startpos <- integer(n)
    endpos <- integer(n)
    strand <- integer(n)
    return(.Call("R_reverse_regionkey", as.uint64(rk), chrom, startpos, endpos, strand))
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
    n <- length(chrom)
    if ((n != length(startpos)) || (n != length(endpos)) || (n != length(strand))) {
        stop(ERR_INPUT_LENGTH)
    }
    ret <- uint64(n)
    return(.Call("R_regionkey", as.character(chrom), as.integer(startpos), as.integer(endpos), as.integer(strand), ret))
}

#' Returns RegionKey hexadecimal string (16 characters).
#' @param vk    RegiontKey code.
#' @useDynLib   variantkey R_regionkey_hex
#' @export
RegionKeyHex <- function(vk) {
    ret <- character(length(vk))
    return(.Call("R_regionkey_hex", as.uint64(vk), ret))
}

#' Parses a RegionKey hexadecimal string and returns the code.
#' @param hex    RegionKey hexadecimal string (it must contain 16 hexadecimal characters).
#' @useDynLib   variantkey R_parse_regionkey_hex
#' @export
ParseRegionKeyHex <- function(hex) {
    ret <- uint64(length(hex))
    return(.Call("R_parse_regionkey_hex", as.character(hex), ret))
}

#' Get the CHROM + START POS encoding from RegionKey.
#' @param rk RegionKey code.
#' @useDynLib   variantkey R_get_regionkey_chrom_startpos
#' @export
GetRegionKeyChromStartPos <- function(rk) {
    ret <- uint64(length(rk))
    return(.Call("R_get_regionkey_chrom_startpos", as.uint64(rk), ret))
}

#' Get the CHROM + END POS encoding from RegionKey.
#' @param rk RegionKey code.
#' @useDynLib   variantkey R_get_regionkey_chrom_endpos
#' @export
GetRegionKeyChromEndPos <- function(rk) {
    ret <- uint64(length(rk))
    return(.Call("R_get_regionkey_chrom_endpos", as.uint64(rk), ret))
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
    n <- length(a_chrom)
    if ((n != length(a_startpos)) || (n != length(a_endpos)) || (n != length(b_chrom)) || (n != length(b_startpos)) || (n != length(b_endpos))) {
        stop(ERR_INPUT_LENGTH)
    }
    ret <- integer(n)
    return(.Call("R_are_overlapping_regions", as.integer(a_chrom), as.integer(a_startpos), as.integer(a_endpos), as.integer(b_chrom), as.integer(b_startpos), as.integer(b_endpos), ret))
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
    n <- length(a_chrom)
    if ((n != length(startpos)) || (n != length(endpos)) || (n != length(rk))) {
        stop(ERR_INPUT_LENGTH)
    }
    ret <- integer(n)
    return(.Call("R_are_overlapping_region_regionkey", as.integer(chrom), as.integer(startpos), as.integer(endpos), as.uint64(rk), ret))
}

#' Check if two regionkeys are overlapping.
#' Return 1 if the regions overlap, 0 otherwise.
#' @param rka        RegionKey A.
#' @param rkb        RegionKey B.
#' @useDynLib   variantkey R_are_overlapping_regionkeys
#' @export
AreOverlappingRegionKeys <- function(rka, rkb) {
    n <- length(rka)
    if (n != length(rkb)) {
        stop(ERR_INPUT_LENGTH)
    }
    ret <- integer(n)
    return(.Call("R_are_overlapping_regionkeys", as.uint64(rka), as.uint64(rkb), ret))
}

#' Check if variantkey and regionkey are overlapping.
#' Return 1 if the regions overlap, 0 otherwise.
#' @param mc    Memory-mapped columns object as retured by MmapNRVKfile.
#' @param vk    VariantKey code.
#' @param rk    RegionKey code.
#' @useDynLib   variantkey R_are_overlapping_variantkey_regionkey
#' @export
AreOverlappingVariantKeyRegionKey <- function(mc, vk, rk) {
    n <- length(vk)
    if (n != length(rk)) {
        stop(ERR_INPUT_LENGTH)
    }
    ret <- integer(n)
    return(.Call("R_are_overlapping_variantkey_regionkey", mc, as.uint64(vk), as.uint64(rk), ret))
}

#' Get RegionKey from VariantKey.
#' @param mc    Memory-mapped columns object as retured by MmapNRVKfile.
#' @param vk    VariantKey code.
#' @useDynLib   variantkey R_variantkey_to_regionkey
#' @export
VariantToRegionkey <- function(mc, vk) {
    ret <- uint64(length(vk))
    return(.Call("R_variantkey_to_regionkey", mc, as.uint64(vk), ret))
}

# --- ESID ---

#' Encode maximum 10 characters of a string into a 64 bit unsigned integer.
#' This function can be used to convert generic string IDs to numeric IDs.
#' @param str    The string to encode. It must be maximum 10 characters long and support ASCII characters from '!' to 'z'.
#' @param start  First character to encode, starting from 0. To encode the last 10 characters, set this value at (size - 10).
#' @useDynLib   variantkey R_encode_string_id
#' @export
EncodeStringID <- function(str, start) {
    ret <- uint64(length(str))
    return(.Call("R_encode_string_id", as.character(str), as.integer(start), ret))
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
    ret <- uint64(length(str))
    return(.Call("R_encode_string_num_id", as.character(str), sep, ret))
}

#' Decode the encoded string ID.
#' This function is the reverse of encode_string_id.
#' The string is always returned in uppercase mode.
#' @param esid   Encoded string ID code.
#' @useDynLib   variantkey R_decode_string_id
#' @export
DecodeStringID <- function(esid) {
    ret <- character(length(esid))
    return(.Call("R_decode_string_id", as.uint64(esid), ret))
}

#' Hash the input string into a 64 bit unsigned integer.
#' This function can be used to convert long string IDs to numeric IDs.
#' @param str    The string to encode.
#' @useDynLib   variantkey R_hash_string_id
#' @export
HashStringID <- function(str) {
    ret <- uint64(length(str))
    return(.Call("R_hash_string_id", as.character(str), ret))
}
