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
#
# VariantKey by Nicola Asuni

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
ExtractVariantkeyChrom <- function(vk) {
    return(.Call("R_extract_variantkey_chrom", vk))
}

#' Extract the POS code from VariantKey.
#' @param vk VariantKey code.
#' @useDynLib   variantkey R_extract_variantkey_pos
#' @export
ExtractVariantkeyPos <- function(vk) {
    return(.Call("R_extract_variantkey_pos", vk))
}

#' Extract the REF+ALT code from VariantKey.
#' @param vk VariantKey code.
#' @useDynLib   variantkey R_extract_variantkey_refalt
#' @export
ExtractVariantkeyRefAlt <- function(vk) {
    return(.Call("R_extract_variantkey_refalt", vk))
}

#' Decode a VariantKey code and returns the components.
#' @param code VariantKey code.
#' @param vk   Decoded variantkey structure.
#' @useDynLib   variantkey R_decode_variantkey
#' @export
DecodeVariantkey <- function(vk) {
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
CompareVariantkeyChrom <- function(vka, vkb) {
    return(.Call("R_compare_variantkey_chrom", vka, vkb))
}

#' Compares two VariantKeys by chromosome and position.
#' Return -1 if the first CHROM+POS is smaller than the second, 0 if they are equal and 1 if the first is greater than the second.
#' @param vka    The first VariantKey to be compared.
#' @param vkb    The second VariantKey to be compared.
#' @useDynLib   variantkey R_compare_variantkey_chrom_pos
#' @export
CompareVariantkeyChromPos <- function(vka, vkb) {
    return(.Call("R_compare_variantkey_chrom_pos", vka, vkb))
}

#' Returns VariantKey hexadecimal string (16 characters).
#' Upon successful return, these function returns the number of characters processed (excluding the null byte used to end output to strings).
#' If the buffer size is not sufficient, then the return value is the number of characters required for
#' buffer string, including the terminating null byte.
#' @param vk    VariantKey code.
#' @param str   String buffer to be returned (it must be sized 17 bytes at least).  
#' @useDynLib   variantkey R_variantkey_hex
#' @export
VariantkeyHex <- function(vk) {
    return(.Call("R_variantkey_hex", vk))
}

#' Parses a VariantKey hexadecimal string and returns the code.
#' @param vs    VariantKey hexadecimal string (it must contain 16 hexadecimal characters).
#' @useDynLib   variantkey R_parse_variantkey_hex
#' @export
ParseVariantkeyHex <- function(vs) {
    return(.Call("R_parse_variantkey_hex", vs))
}
