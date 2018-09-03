// VariantKey Python wrapper
//
// pyvariantkey.h
//
// @category   Libraries
// @author     Nicola Asuni <nicola.asuni@genomicsplc.com>
// @copyright  2017-2018 GENOMICS plc
// @license    MIT (see LICENSE)
// @link       https://github.com/genomicsplc/variantkey
//
// LICENSE
//
// Copyright (c) 2017-2018 GENOMICS plc
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.

#define PY_SSIZE_T_CLEAN
#include <Python.h>

// VARIANTKEY
static PyObject *py_encode_chrom(PyObject *self, PyObject *args, PyObject *keywds);
static PyObject *py_decode_chrom(PyObject *self, PyObject *args, PyObject *keywds);
static PyObject *py_encode_refalt(PyObject *self, PyObject *args, PyObject *keywds);
static PyObject *py_decode_refalt(PyObject *self, PyObject *args, PyObject *keywds);
static PyObject *py_encode_variantkey(PyObject *self, PyObject *args, PyObject *keywds);
static PyObject *py_extract_variantkey_chrom(PyObject *self, PyObject *args, PyObject *keywds);
static PyObject *py_extract_variantkey_pos(PyObject *self, PyObject *args, PyObject *keywds);
static PyObject *py_extract_variantkey_refalt(PyObject *self, PyObject *args, PyObject *keywds);
static PyObject *py_decode_variantkey(PyObject *self, PyObject *args, PyObject *keywds);
static PyObject *py_variantkey(PyObject *self, PyObject *args, PyObject *keywds);
static PyObject *py_variantkey_range(PyObject *self, PyObject *args, PyObject *keywds);
static PyObject *py_compare_variantkey_chrom(PyObject *self, PyObject *args, PyObject *keywds);
static PyObject *py_compare_variantkey_chrom_pos(PyObject *self, PyObject *args, PyObject *keywds);
static PyObject *py_variantkey_hex(PyObject *self, PyObject *args, PyObject *keywds);
static PyObject *py_parse_variantkey_hex(PyObject *self, PyObject *args, PyObject *keywds);

// BINSEARCH
static PyObject *py_mmap_binfile(PyObject *self, PyObject *args, PyObject *keywds);
static PyObject *py_munmap_binfile(PyObject *self, PyObject *args, PyObject *keywds);
static PyObject *py_get_address(PyObject *self, PyObject *args, PyObject *keywds);
static PyObject *py_find_first_be_uint8(PyObject *self, PyObject *args, PyObject *keywds);
static PyObject *py_find_first_be_uint16(PyObject *self, PyObject *args, PyObject *keywds);
static PyObject *py_find_first_be_uint32(PyObject *self, PyObject *args, PyObject *keywds);
static PyObject *py_find_first_be_uint64(PyObject *self, PyObject *args, PyObject *keywds);
static PyObject *py_find_last_be_uint8(PyObject *self, PyObject *args, PyObject *keywds);
static PyObject *py_find_last_be_uint16(PyObject *self, PyObject *args, PyObject *keywds);
static PyObject *py_find_last_be_uint32(PyObject *self, PyObject *args, PyObject *keywds);
static PyObject *py_find_last_be_uint64(PyObject *self, PyObject *args, PyObject *keywds);
static PyObject *py_find_first_sub_be_uint8(PyObject *self, PyObject *args, PyObject *keywds);
static PyObject *py_find_first_sub_be_uint16(PyObject *self, PyObject *args, PyObject *keywds);
static PyObject *py_find_first_sub_be_uint32(PyObject *self, PyObject *args, PyObject *keywds);
static PyObject *py_find_first_sub_be_uint64(PyObject *self, PyObject *args, PyObject *keywds);
static PyObject *py_find_last_sub_be_uint8(PyObject *self, PyObject *args, PyObject *keywds);
static PyObject *py_find_last_sub_be_uint16(PyObject *self, PyObject *args, PyObject *keywds);
static PyObject *py_find_last_sub_be_uint32(PyObject *self, PyObject *args, PyObject *keywds);
static PyObject *py_find_last_sub_be_uint64(PyObject *self, PyObject *args, PyObject *keywds);
static PyObject *py_has_next_be_uint8(PyObject *self, PyObject *args, PyObject *keywds);
static PyObject *py_has_next_be_uint16(PyObject *self, PyObject *args, PyObject *keywds);
static PyObject *py_has_next_be_uint32(PyObject *self, PyObject *args, PyObject *keywds);
static PyObject *py_has_next_be_uint64(PyObject *self, PyObject *args, PyObject *keywds);
static PyObject *py_has_next_sub_be_uint8(PyObject *self, PyObject *args, PyObject *keywds);
static PyObject *py_has_next_sub_be_uint16(PyObject *self, PyObject *args, PyObject *keywds);
static PyObject *py_has_next_sub_be_uint32(PyObject *self, PyObject *args, PyObject *keywds);
static PyObject *py_has_next_sub_be_uint64(PyObject *self, PyObject *args, PyObject *keywds);
static PyObject *py_has_prev_be_uint8(PyObject *self, PyObject *args, PyObject *keywds);
static PyObject *py_has_prev_be_uint16(PyObject *self, PyObject *args, PyObject *keywds);
static PyObject *py_has_prev_be_uint32(PyObject *self, PyObject *args, PyObject *keywds);
static PyObject *py_has_prev_be_uint64(PyObject *self, PyObject *args, PyObject *keywds);
static PyObject *py_has_prev_sub_be_uint8(PyObject *self, PyObject *args, PyObject *keywds);
static PyObject *py_has_prev_sub_be_uint16(PyObject *self, PyObject *args, PyObject *keywds);
static PyObject *py_has_prev_sub_be_uint32(PyObject *self, PyObject *args, PyObject *keywds);
static PyObject *py_has_prev_sub_be_uint64(PyObject *self, PyObject *args, PyObject *keywds);
static PyObject *py_find_first_le_uint8(PyObject *self, PyObject *args, PyObject *keywds);
static PyObject *py_find_first_le_uint16(PyObject *self, PyObject *args, PyObject *keywds);
static PyObject *py_find_first_le_uint32(PyObject *self, PyObject *args, PyObject *keywds);
static PyObject *py_find_first_le_uint64(PyObject *self, PyObject *args, PyObject *keywds);
static PyObject *py_find_last_le_uint8(PyObject *self, PyObject *args, PyObject *keywds);
static PyObject *py_find_last_le_uint16(PyObject *self, PyObject *args, PyObject *keywds);
static PyObject *py_find_last_le_uint32(PyObject *self, PyObject *args, PyObject *keywds);
static PyObject *py_find_last_le_uint64(PyObject *self, PyObject *args, PyObject *keywds);
static PyObject *py_find_first_sub_le_uint8(PyObject *self, PyObject *args, PyObject *keywds);
static PyObject *py_find_first_sub_le_uint16(PyObject *self, PyObject *args, PyObject *keywds);
static PyObject *py_find_first_sub_le_uint32(PyObject *self, PyObject *args, PyObject *keywds);
static PyObject *py_find_first_sub_le_uint64(PyObject *self, PyObject *args, PyObject *keywds);
static PyObject *py_find_last_sub_le_uint8(PyObject *self, PyObject *args, PyObject *keywds);
static PyObject *py_find_last_sub_le_uint16(PyObject *self, PyObject *args, PyObject *keywds);
static PyObject *py_find_last_sub_le_uint32(PyObject *self, PyObject *args, PyObject *keywds);
static PyObject *py_find_last_sub_le_uint64(PyObject *self, PyObject *args, PyObject *keywds);
static PyObject *py_has_next_le_uint8(PyObject *self, PyObject *args, PyObject *keywds);
static PyObject *py_has_next_le_uint16(PyObject *self, PyObject *args, PyObject *keywds);
static PyObject *py_has_next_le_uint32(PyObject *self, PyObject *args, PyObject *keywds);
static PyObject *py_has_next_le_uint64(PyObject *self, PyObject *args, PyObject *keywds);
static PyObject *py_has_next_sub_le_uint8(PyObject *self, PyObject *args, PyObject *keywds);
static PyObject *py_has_next_sub_le_uint16(PyObject *self, PyObject *args, PyObject *keywds);
static PyObject *py_has_next_sub_le_uint32(PyObject *self, PyObject *args, PyObject *keywds);
static PyObject *py_has_next_sub_le_uint64(PyObject *self, PyObject *args, PyObject *keywds);
static PyObject *py_has_prev_le_uint8(PyObject *self, PyObject *args, PyObject *keywds);
static PyObject *py_has_prev_le_uint16(PyObject *self, PyObject *args, PyObject *keywds);
static PyObject *py_has_prev_le_uint32(PyObject *self, PyObject *args, PyObject *keywds);
static PyObject *py_has_prev_le_uint64(PyObject *self, PyObject *args, PyObject *keywds);
static PyObject *py_has_prev_sub_le_uint8(PyObject *self, PyObject *args, PyObject *keywds);
static PyObject *py_has_prev_sub_le_uint16(PyObject *self, PyObject *args, PyObject *keywds);
static PyObject *py_has_prev_sub_le_uint32(PyObject *self, PyObject *args, PyObject *keywds);
static PyObject *py_has_prev_sub_le_uint64(PyObject *self, PyObject *args, PyObject *keywds);
static PyObject *py_col_find_first_uint8(PyObject *self, PyObject *args, PyObject *keywds);
static PyObject *py_col_find_first_uint16(PyObject *self, PyObject *args, PyObject *keywds);
static PyObject *py_col_find_first_uint32(PyObject *self, PyObject *args, PyObject *keywds);
static PyObject *py_col_find_first_uint64(PyObject *self, PyObject *args, PyObject *keywds);
static PyObject *py_col_find_last_uint8(PyObject *self, PyObject *args, PyObject *keywds);
static PyObject *py_col_find_last_uint16(PyObject *self, PyObject *args, PyObject *keywds);
static PyObject *py_col_find_last_uint32(PyObject *self, PyObject *args, PyObject *keywds);
static PyObject *py_col_find_last_uint64(PyObject *self, PyObject *args, PyObject *keywds);
static PyObject *py_col_find_first_sub_uint8(PyObject *self, PyObject *args, PyObject *keywds);
static PyObject *py_col_find_first_sub_uint16(PyObject *self, PyObject *args, PyObject *keywds);
static PyObject *py_col_find_first_sub_uint32(PyObject *self, PyObject *args, PyObject *keywds);
static PyObject *py_col_find_first_sub_uint64(PyObject *self, PyObject *args, PyObject *keywds);
static PyObject *py_col_find_last_sub_uint8(PyObject *self, PyObject *args, PyObject *keywds);
static PyObject *py_col_find_last_sub_uint16(PyObject *self, PyObject *args, PyObject *keywds);
static PyObject *py_col_find_last_sub_uint32(PyObject *self, PyObject *args, PyObject *keywds);
static PyObject *py_col_find_last_sub_uint64(PyObject *self, PyObject *args, PyObject *keywds);
static PyObject *py_col_has_next_uint8(PyObject *self, PyObject *args, PyObject *keywds);
static PyObject *py_col_has_next_uint16(PyObject *self, PyObject *args, PyObject *keywds);
static PyObject *py_col_has_next_uint32(PyObject *self, PyObject *args, PyObject *keywds);
static PyObject *py_col_has_next_uint64(PyObject *self, PyObject *args, PyObject *keywds);
static PyObject *py_col_has_next_sub_uint8(PyObject *self, PyObject *args, PyObject *keywds);
static PyObject *py_col_has_next_sub_uint16(PyObject *self, PyObject *args, PyObject *keywds);
static PyObject *py_col_has_next_sub_uint32(PyObject *self, PyObject *args, PyObject *keywds);
static PyObject *py_col_has_next_sub_uint64(PyObject *self, PyObject *args, PyObject *keywds);
static PyObject *py_col_has_prev_uint8(PyObject *self, PyObject *args, PyObject *keywds);
static PyObject *py_col_has_prev_uint16(PyObject *self, PyObject *args, PyObject *keywds);
static PyObject *py_col_has_prev_uint32(PyObject *self, PyObject *args, PyObject *keywds);
static PyObject *py_col_has_prev_uint64(PyObject *self, PyObject *args, PyObject *keywds);
static PyObject *py_col_has_prev_sub_uint8(PyObject *self, PyObject *args, PyObject *keywds);
static PyObject *py_col_has_prev_sub_uint16(PyObject *self, PyObject *args, PyObject *keywds);
static PyObject *py_col_has_prev_sub_uint32(PyObject *self, PyObject *args, PyObject *keywds);
static PyObject *py_col_has_prev_sub_uint64(PyObject *self, PyObject *args, PyObject *keywds);

// RSIDVAR
static PyObject *py_get_vr_rsid(PyObject *self, PyObject *args, PyObject *keywds);
static PyObject *py_get_rv_variantkey(PyObject *self, PyObject *args, PyObject *keywds);
static PyObject *py_find_rv_variantkey_by_rsid(PyObject *self, PyObject *args, PyObject *keywds);
static PyObject *py_get_next_rv_variantkey_by_rsid(PyObject *self, PyObject *args, PyObject *keywds);
static PyObject *py_find_all_rv_variantkey_by_rsid(PyObject *self, PyObject *args, PyObject *keywds);
static PyObject *py_find_vr_rsid_by_variantkey(PyObject *self, PyObject *args, PyObject *keywds);
static PyObject *py_find_vr_chrompos_range(PyObject *self, PyObject *args, PyObject *keywds);

// NRVK
static PyObject *py_find_ref_alt_by_variantkey(PyObject *self, PyObject *args, PyObject *keywds);
static PyObject *py_reverse_variantkey(PyObject *self, PyObject *args, PyObject *keywds);
static PyObject *py_get_variantkey_ref_length(PyObject *self, PyObject *args, PyObject *keywds);
static PyObject *py_get_variantkey_endpos(PyObject *self, PyObject *args, PyObject *keywds);
static PyObject *py_get_variantkey_chrom_startpos(PyObject *self, PyObject *args, PyObject *keywds);
static PyObject *py_get_variantkey_chrom_endpos(PyObject *self, PyObject *args, PyObject *keywds);
static PyObject *py_nrvk_bin_to_tsv(PyObject *self, PyObject *args, PyObject *keywds);

// GENOREF
static PyObject *py_load_genoref_index(PyObject *self, PyObject *args, PyObject *keywds);
static PyObject *py_get_genoref_seq(PyObject *self, PyObject *args, PyObject *keywds);
static PyObject *py_check_reference(PyObject *self, PyObject *args, PyObject *keywds);
static PyObject *py_flip_allele(PyObject *self, PyObject *args, PyObject *keywds);
static PyObject *py_normalize_variant(PyObject *self, PyObject *args, PyObject *keywds);

// REGIONKEY
static PyObject *py_encode_region_strand(PyObject *self, PyObject *args, PyObject *keywds);
static PyObject *py_decode_region_strand(PyObject *self, PyObject *args, PyObject *keywds);
static PyObject *py_encode_regionkey(PyObject *self, PyObject *args, PyObject *keywds);
static PyObject *py_extract_regionkey_chrom(PyObject *self, PyObject *args, PyObject *keywds);
static PyObject *py_extract_regionkey_startpos(PyObject *self, PyObject *args, PyObject *keywds);
static PyObject *py_extract_regionkey_endpos(PyObject *self, PyObject *args, PyObject *keywds);
static PyObject *py_extract_regionkey_strand(PyObject *self, PyObject *args, PyObject *keywds);
static PyObject *py_decode_regionkey(PyObject *self, PyObject *args, PyObject *keywds);
static PyObject *py_reverse_regionkey(PyObject *self, PyObject *args, PyObject *keywds);
static PyObject *py_regionkey(PyObject *self, PyObject *args, PyObject *keywds);
static PyObject *py_regionkey_hex(PyObject *self, PyObject *args, PyObject *keywds);
static PyObject *py_parse_regionkey_hex(PyObject *self, PyObject *args, PyObject *keywds);
static PyObject *py_get_regionkey_chrom_startpos(PyObject *self, PyObject *args, PyObject *keywds);
static PyObject *py_get_regionkey_chrom_endpos(PyObject *self, PyObject *args, PyObject *keywds);
static PyObject *py_are_overlapping_regions(PyObject *self, PyObject *args, PyObject *keywds);
static PyObject *py_are_overlapping_region_regionkey(PyObject *self, PyObject *args, PyObject *keywds);
static PyObject *py_are_overlapping_regionkeys(PyObject *self, PyObject *args, PyObject *keywds);
static PyObject *py_are_overlapping_variantkey_regionkey(PyObject *self, PyObject *args, PyObject *keywds);
static PyObject *py_variantkey_to_regionkey(PyObject *self, PyObject *args, PyObject *keywds);

// ESID
static PyObject *py_encode_string_id(PyObject *self, PyObject *args, PyObject *keywds);
static PyObject *py_encode_string_num_id(PyObject *self, PyObject *args, PyObject *keywds);
static PyObject *py_decode_string_id(PyObject *self, PyObject *args, PyObject *keywds);
static PyObject *py_hash_string_id(PyObject *self, PyObject *args, PyObject *keywds);

PyMODINIT_FUNC initvariantkey(void);

// VARIANTKEY

#define PYENCODECHROM_DOCSTRING "Returns chromosome numerical encoding.\n"\
"\n"\
"Parameters\n"\
"----------\n"\
"chrom : str or bytes\n"\
"    Chromosome. An identifier from the reference genome, no white-space permitted.\n"\
"\n"\
"Returns\n"\
"-------\n"\
"int :\n"\
"    CHROM code\n"\
"\n"\
"Example\n"\
"-------\n"\
">>> encode_chrom('X')\n"\
"23"

#define PYDECODECHROM_DOCSTRING "Decode the chromosome numerical code.\n"\
"\n"\
"Parameters\n"\
"----------\n"\
"code : int\n"\
"    CHROM code.\n"\
"\n"\
"Returns\n"\
"-------\n"\
"bytes :\n"\
"    Chromosome string\n"\
"\n"\
"Example\n"\
"-------\n"\
">>> decode_chrom(23)\n"\
"b'X'"

#define PYENCODEREFALT_DOCSTRING "Returns reference+alternate numerical encoding.\n"\
"\n"\
"Parameters\n"\
"----------\n"\
"ref : str or bytes\n"\
"    Reference allele. String containing a sequence of nucleotide letters. The value in the pos field refers to the position of the first nucleotide in the String. Characters must be A-Z, a-z or *\n"\
"alt : str or bytes\n"\
"    Alternate non-reference allele string. Characters must be A-Z, a-z or *\n"\
"\n"\
"Returns\n"\
"-------\n"\
"int :\n"\
"    code\n"\
"\n"\
"Example\n"\
"-------\n"\
">>> encode_refalt(ref=b'AC', alt=b'GT')\n"\
"286097408"

#define PYDECODEREFALT_DOCSTRING "Decode the 32 bit REF+ALT code if reversible (if it has 11 or less bases in total and only contains ACGT letters).\n"\
"\n"\
"Parameters\n"\
"----------\n"\
"code : int\n"\
"    REF+ALT code\n"\
"\n"\
"Returns\n"\
"-------\n"\
"tuple:\n"\
"    - REF\n"\
"    - ALT\n"\
"    - REF length\n"\
"    - ALT length\n"\
"\n"\
"Example\n"\
"-------\n"\
">>> decode_refalt(286097408)\n"\
"(b'AC', b'GT', 2, 2)"

#define PYENCODEVARIANTKEY_DOCSTRING "Returns a 64 bit variant key based on the pre-encoded CHROM, POS (0-based) and REF+ALT.\n"\
"\n"\
"Parameters\n"\
"----------\n"\
"chrom : int\n"\
"    Encoded Chromosome (see encode_chrom).\n"\
"pos : int\n"\
"    Position. The reference position, with the first base having position 0.\n"\
"refalt : int\n"\
"    Encoded Reference + Alternate (see encode_refalt).\n"\
"\n"\
"Returns\n"\
"-------\n"\
"int:\n"\
"    VariantKey 64 bit code.\n"\
"\n"\
"Example\n"\
"-------\n"\
">>> encode_variantkey(chrom=23, pos=12345, refalt=286097408)\n"\
"13258623813950472192"

#define PYEXTRACTVARIANTKEYCHROM_DOCSTRING "Extract the CHROM code from VariantKey.\n"\
"\n"\
"Parameters\n"\
"----------\n"\
"vk : int\n"\
"    VariantKey code.\n"\
"\n"\
"Returns\n"\
"-------\n"\
"int :\n"\
"    CHROM code.\n"\
"\n"\
"Example\n"\
"-------\n"\
">>> extract_variantkey_chrom(13258623813950472192)\n"\
"23"

#define PYEXTRACTVARIANTKEYPOS_DOCSTRING "Extract the POS code from VariantKey.\n"\
"\n"\
"Parameters\n"\
"----------\n"\
"vk : int\n"\
"    VariantKey code.\n"\
"\n"\
"Returns\n"\
"-------\n"\
"int :\n"\
"    Position.\n"\
"\n"\
"Example\n"\
"-------\n"\
">>> extract_variantkey_pos(13258623813950472192)\n"\
"12345"

#define PYEXTRACTVARIANTKEYREFALT_DOCSTRING "Extract the REF+ALT code from VariantKey.\n"\
"\n"\
"Parameters\n"\
"----------\n"\
"vk : int\n"\
"    VariantKey code.\n"\
"\n"\
"Returns\n"\
"-------\n"\
"int :\n"\
"    REF+ALT code.\n"\
"\n"\
"Example\n"\
"-------\n"\
">>> extract_variantkey_refalt(13258623813950472192)\n"\
"286097408"

#define PYDECODEVARIANTKEY_DOCSTRING "Decode a VariantKey code and returns the components.\n"\
"\n"\
"Parameters\n"\
"----------\n"\
"vk : int\n"\
"    VariantKey code.\n"\
"\n"\
"Returns\n"\
"-------\n"\
"tuple : int\n"\
"    - CHROM code\n"\
"    - POS\n"\
"    - REF+ALT code\n"\
"\n"\
"Example\n"\
"-------\n"\
">>> decode_variantkey(13258623813950472192)\n"\
"(23, 12345, 286097408)"

#define PYVARIANTKEY_DOCSTRING "Returns a 64 bit variant key based on CHROM, POS (0-based), REF, ALT.\n"\
"\n"\
"Parameters\n"\
"----------\n"\
"chrom : str or bytes\n"\
"    Chromosome. An identifier from the reference genome, no white-space or leading zeros permitted.\n"\
"pos : int\n"\
"    Position. The reference position, with the first base having position 0.\n"\
"ref : str or bytes\n"\
"    Reference allele. String containing a sequence of nucleotide letters. The value in the pos field refers to the position of the first nucleotide in the String. Characters must be A-Z, a-z or *\n"\
"alt : str or bytes\n"\
"    Alternate non-reference allele string. Characters must be A-Z, a-z or *\n"\
"\n"\
"Returns\n"\
"-------\n"\
"int:\n"\
"    VariantKey 64 bit code.\n"\
"\n"\
"Example\n"\
"-------\n"\
">>> variantkey(chrom=b'X', pos=12345, ref=b'AC', alt=b'GT')\n"\
"13258623813950472192\n"\
">>> variantkey(chrom='X', pos=12345, ref='AC', alt='GT')\n"\
"13258623813950472192\n"\
">>> variantkey(b'X', 12345, b'AC', b'GT')\n"\
"13258623813950472192\n"\
">>> variantkey('X', 12345, 'AC', 'GT')\n"\
"13258623813950472192"

#define PYVARIANTKEYRANGE_DOCSTRING "Returns minimum and maximum VariantKeys for range searches.\n"\
"\n"\
"Parameters\n"\
"----------\n"\
"chrom : int\n"\
"    Chromosome encoded number.\n"\
"pos_min : int\n"\
"    Start reference position, with the first base having position 0.\n"\
"pos_max : int\n"\
"    End reference position, with the first base having position 0.\n"\
"\n"\
"Returns\n"\
"-------\n"\
"tuple : int\n"\
"    - VariantKey min value\n"\
"    - VariantKey max value\n"\
"\n"\
"Example\n"\
"-------\n"\
">>> variantkey_range(chrom=23, pos_min=1234, pos_max=5678)\n"\
"(13258599952973561856, 13258609498538377215)"

#define PYCOMPAREVARIANTKEYCHROM_DOCSTRING "Compares two VariantKeys by chromosome only.\n"\
"\n"\
"Parameters\n"\
"----------\n"\
"vka : int\n"\
"    The first VariantKey to be compared.\n"\
"vkb : int\n"\
"    The second VariantKey to be compared.\n"\
"\n"\
"Returns\n"\
"-------\n"\
"int :\n"\
"    -1 if the first chromosome is smaller than the second, 0 if they are equal and 1 if the first is greater than the second.\n"\
"\n"\
"Example\n"\
"-------\n"\
">>> compare_variantkey_chrom(13258599952973561856, 13258609498538377215)\n"\
"0"

#define PYCOMPAREVARIANTKEYCHROMPOS_DOCSTRING "Compares two VariantKeys by chromosome and position.\n"\
"\n"\
"Parameters\n"\
"----------\n"\
"vka : int\n"\
"    The first VariantKey to be compared.\n"\
"vkb : int\n"\
"    The second VariantKey to be compared.\n"\
"\n"\
"Returns\n"\
"-------\n"\
"int :\n"\
"    -1 if the first CHROM+POS is smaller than the second, 0 if they are equal and 1 if the first is greater than the second.\n"\
"\n"\
"Example\n"\
"-------\n"\
">>> compare_variantkey_chrom_pos(13258599952973561856, 13258609498538377215)\n"\
"-1"

#define PYVARIANTKEYHEX_DOCSTRING "Returns VariantKey hexadecimal string (16 characters).\n"\
"\n"\
"Parameters\n"\
"----------\n"\
"vk : int\n"\
"    VariantKey code.\n"\
"\n"\
"Returns\n"\
"-------\n"\
"bytes:\n"\
"    VariantKey hexadecimal string.\n"\
"\n"\
"Example\n"\
"-------\n"\
">>> variantkey_hex(13258623813950472192)\n"\
"b'b800181c910d8000'"

#define PYPARSEVARIANTKEYSTRING_DOCSTRING "Parses a VariantKey hexadecimal string and returns the code.\n"\
"\n"\
"Parameters\n"\
"----------\n"\
"vs : str or bytes\n"\
"    VariantKey hexadecimal string (it must contain 16 hexadecimal characters).\n"\
"\n"\
"Returns\n"\
"-------\n"\
"int :\n"\
"    VariantKey 64 bit code.\n"\
"\n"\
"Example\n"\
"-------\n"\
">>> parse_variantkey_hex(b'b800181c910d8000')\n"\
"13258623813950472192"

// BINSEARCH
// ----------

#define PYMMAPBINFILE_DOCSTRING "Memory map the specified file.\n"\
"\n"\
"Parameters\n"\
"----------\n"\
"file : str\n"\
"    Path to the file to map.\n"\
"\n"\
"Returns\n"\
"-------\n"\
"tuple :\n"\
"    - Pointer to the memory map.\n"\
"    - File descriptor.\n"\
"    - File size in bytes.\n"\
"    - Index of the last element (if set as last 4 bytes) or it can be used as index size."

#define PYMUNMAPBINFILE_DOCSTRING "Unmap and close the memory-mapped file.\n"\
"\n"\
"Parameters\n"\
"----------\n"\
"mfsrc : obj\n"\
"    Pointer to the memory mapped file.\n"\
"fd : int\n"\
"    File descriptor.\n"\
"size : int\n"\
"    File size.\n"\
"\n"\
"Returns\n"\
"-------\n"\
"int:\n"\
"    On success returns 0, on failure -1."

#define PYGETADDRESS_DOCSTRING "Returns the absolute file address position of the specified item (binary block).\n"\
"\n"\
"Parameters\n"\
"----------\n"\
"blklen : int\n"\
"    Length of the binary block in bytes.\n"\
"blkpos : int\n"\
"    Indicates the position of the value to search inside a binary block.\n"\
"item : int\n"\
"    Item number to search.\n"\
"\n"\
"Returns\n"\
"-------\n"\
"int:\n"\
"    First byte position of the specified item number."

// ----------

#define PYFINDFIRSTUINT8_DOCSTRING "Search for the first occurrence of a 8 bit unsigned integer on a memory mapped binary file containing adjacent blocks of sorted binary data. The 8 bit values in the file must be sorted in ascending order.\n"\
"\n"\
"Parameters\n"\
"----------\n"\
"mfsrc : obj\n"\
"    Pointer to the memory mapped file.\n"\
"offset : int\n"\
"    Byte offset from where the first item starts.\n"\
"blklen : int\n"\
"    Length of the binary block in bytes.\n"\
"blkpos : int\n"\
"    Indicates the position of the value to search inside a binary block.\n"\
"first : int\n"\
"    First element of the range to search (min value = 0).\n"\
"last : int\n"\
"    Last element of the range to search (max value = nitems - 1).\n"\
"search : int\n"\
"    Unsigned 8 bit number to search.\n"\
"\n"\
"Returns\n"\
"-------\n"\
"tuple :\n"\
"    - Item number if found or (last + 1) if not found.\n"\
"    - Position of the 'first' iterator.\n"\
"    - Position of the 'last' iterator."

#define PYFINDFIRSTUINT16_DOCSTRING "Search for the first occurrence of a 16 bit unsigned integer on a memory mapped binary file containing adjacent blocks of sorted binary data. The 16 bit values in the file must be sorted in ascending order.\n"\
"\n"\
"Parameters\n"\
"----------\n"\
"mfsrc : obj\n"\
"    Pointer to the memory mapped file.\n"\
"offset : int\n"\
"    Byte offset from where the first item starts.\n"\
"blklen : int\n"\
"    Length of the binary block in bytes.\n"\
"blkpos : int\n"\
"    Indicates the position of the value to search inside a binary block.\n"\
"first : int\n"\
"    First element of the range to search (min value = 0).\n"\
"last : int\n"\
"    Last element of the range to search (max value = nitems - 1).\n"\
"search : int\n"\
"    Unsigned 16 bit number to search.\n"\
"\n"\
"Returns\n"\
"-------\n"\
"tuple :\n"\
"    - Item number if found or (last + 1) if not found.\n"\
"    - Position of the 'first' iterator.\n"\
"    - Position of the 'last' iterator."

#define PYFINDFIRSTUINT32_DOCSTRING "Search for the first occurrence of a 32 bit unsigned integer on a memory mapped binary file containing adjacent blocks of sorted binary data. The 32 bit values in the file must be sorted in ascending order.\n"\
"\n"\
"Parameters\n"\
"----------\n"\
"mfsrc : obj\n"\
"    Pointer to the memory mapped file.\n"\
"offset : int\n"\
"    Byte offset from where the first item starts.\n"\
"blklen : int\n"\
"    Length of the binary block in bytes.\n"\
"blkpos : int\n"\
"    Indicates the position of the value to search inside a binary block.\n"\
"first : int\n"\
"    First element of the range to search (min value = 0).\n"\
"last : int\n"\
"    Last element of the range to search (max value = nitems - 1).\n"\
"search : int\n"\
"    Unsigned 32 bit number to search.\n"\
"\n"\
"Returns\n"\
"-------\n"\
"tuple :\n"\
"    - Item number if found or (last + 1) if not found.\n"\
"    - Position of the 'first' iterator.\n"\
"    - Position of the 'last' iterator."

#define PYFINDFIRSTUINT64_DOCSTRING "Search for the first occurrence of a 64 bit unsigned integer on a memory mapped binary file containing adjacent blocks of sorted binary data. The 64 bit values in the file must be sorted in ascending order.\n"\
"\n"\
"Parameters\n"\
"----------\n"\
"mfsrc : obj\n"\
"    Pointer to the memory mapped file.\n"\
"offset : int\n"\
"    Byte offset from where the first item starts.\n"\
"blklen : int\n"\
"    Length of the binary block in bytes.\n"\
"blkpos : int\n"\
"    Indicates the position of the value to search inside a binary block.\n"\
"first : int\n"\
"    First element of the range to search (min value = 0).\n"\
"last : int\n"\
"    Last element of the range to search (max value = nitems - 1).\n"\
"search : int\n"\
"    Unsigned 64 bit number to search.\n"\
"\n"\
"Returns\n"\
"-------\n"\
"tuple :\n"\
"    - Item number if found or (last + 1) if not found.\n"\
"    - Position of the 'first' iterator.\n"\
"    - Position of the 'last' iterator."

// ----------

#define PYFINDLASTUINT8_DOCSTRING "Search for the last occurrence of a 8 bit unsigned integer on a memory mapped binary file containing adjacent blocks of sorted binary data. The 8 bit values in the file must be sorted in ascending order.\n"\
"\n"\
"Parameters\n"\
"----------\n"\
"mfsrc : obj\n"\
"    Pointer to the memory mapped file.\n"\
"offset : int\n"\
"    Byte offset from where the first item starts.\n"\
"blklen : int\n"\
"    Length of the binary block in bytes.\n"\
"blkpos : int\n"\
"    Indicates the position of the value to search inside a binary block.\n"\
"first : int\n"\
"    First element of the range to search (min value = 0).\n"\
"last : int\n"\
"    Last element of the range to search (max value = nitems - 1).\n"\
"search : int\n"\
"    Unsigned 8 bit number to search.\n"\
"\n"\
"Returns\n"\
"-------\n"\
"tuple :\n"\
"    - Item number if found or (last + 1) if not found.\n"\
"    - Position of the 'first' iterator.\n"\
"    - Position of the 'last' iterator."

#define PYFINDLASTUINT16_DOCSTRING "Search for the last occurrence of a 16 bit unsigned integer on a memory mapped binary file containing adjacent blocks of sorted binary data. The 16 bit values in the file must be sorted in ascending order.\n"\
"\n"\
"Parameters\n"\
"----------\n"\
"mfsrc : obj\n"\
"    Pointer to the memory mapped file.\n"\
"offset : int\n"\
"    Byte offset from where the first item starts.\n"\
"blklen : int\n"\
"    Length of the binary block in bytes.\n"\
"blkpos : int\n"\
"    Indicates the position of the value to search inside a binary block.\n"\
"first : int\n"\
"    First element of the range to search (min value = 0).\n"\
"last : int\n"\
"    Last element of the range to search (max value = nitems - 1).\n"\
"search : int\n"\
"    Unsigned 16 bit number to search.\n"\
"\n"\
"Returns\n"\
"-------\n"\
"tuple :\n"\
"    - Item number if found or (last + 1) if not found.\n"\
"    - Position of the 'first' iterator.\n"\
"    - Position of the 'last' iterator."

#define PYFINDLASTUINT32_DOCSTRING "Search for the last occurrence of a 32 bit unsigned integer on a memory mapped binary file containing adjacent blocks of sorted binary data. The 32 bit values in the file must be sorted in ascending order.\n"\
"\n"\
"Parameters\n"\
"----------\n"\
"mfsrc : obj\n"\
"    Pointer to the memory mapped file.\n"\
"offset : int\n"\
"    Byte offset from where the first item starts.\n"\
"blklen : int\n"\
"    Length of the binary block in bytes.\n"\
"blkpos : int\n"\
"    Indicates the position of the value to search inside a binary block.\n"\
"first : int\n"\
"    First element of the range to search (min value = 0).\n"\
"last : int\n"\
"    Last element of the range to search (max value = nitems - 1).\n"\
"search : int\n"\
"    Unsigned 32 bit number to search.\n"\
"\n"\
"Returns\n"\
"-------\n"\
"tuple :\n"\
"    - Item number if found or (last + 1) if not found.\n"\
"    - Position of the 'first' iterator.\n"\
"    - Position of the 'last' iterator."

#define PYFINDLASTUINT64_DOCSTRING "Search for the last occurrence of a 64 bit unsigned integer on a memory mapped binary file containing adjacent blocks of sorted binary data. The 64 bit values in the file must be sorted in ascending order.\n"\
"\n"\
"Parameters\n"\
"----------\n"\
"mfsrc : obj\n"\
"    Pointer to the memory mapped file.\n"\
"offset : int\n"\
"    Byte offset from where the first item starts.\n"\
"blklen : int\n"\
"    Length of the binary block in bytes.\n"\
"blkpos : int\n"\
"    Indicates the position of the value to search inside a binary block.\n"\
"first : int\n"\
"    First element of the range to search (min value = 0).\n"\
"last : int\n"\
"    Last element of the range to search (max value = nitems - 1).\n"\
"search : int\n"\
"    Unsigned 64 bit number to search.\n"\
"\n"\
"Returns\n"\
"-------\n"\
"tuple :\n"\
"    - Item number if found or (last + 1) if not found.\n"\
"    - Position of the 'first' iterator.\n"\
"    - Position of the 'last' iterator."

// ----------

#define PYFINDFIRSTSUBUINT8_DOCSTRING "Search for the first occurrence of a bit set contained in a 8 bit unsigned integer on a memory mapped binary file containing adjacent blocks of sorted binary data. The 8 bit values in the file must be sorted in ascending order.\n"\
"\n"\
"Parameters\n"\
"----------\n"\
"mfsrc : obj\n"\
"    Pointer to the memory mapped file.\n"\
"offset : int\n"\
"    Byte offset from where the first item starts.\n"\
"blklen : int\n"\
"    Length of the binary block in bytes.\n"\
"blkpos : int\n"\
"    Indicates the position of the value to search inside a binary block.\n"\
"bitstart : int\n"\
"    First bit position to consider (min 0).\n"\
"bitend : int\n"\
"    Last bit position to consider (max 7).\n"\
"first : int\n"\
"    First element of the range to search (min value = 0).\n"\
"last : int\n"\
"    Last element of the range to search (max value = nitems - 1).\n"\
"search : int\n"\
"    Unsigned 8 bit number to search.\n"\
"\n"\
"Returns\n"\
"-------\n"\
"tuple :\n"\
"    - Item number if found or (last + 1) if not found.\n"\
"    - Position of the 'first' iterator.\n"\
"    - Position of the 'last' iterator."

#define PYFINDFIRSTSUBUINT16_DOCSTRING "Search for the first occurrence of a bit set contained in a 16 bit unsigned integer on a memory mapped binary file containing adjacent blocks of sorted binary data. The 16 bit values in the file must be sorted in ascending order.\n"\
"\n"\
"Parameters\n"\
"----------\n"\
"mfsrc : obj\n"\
"    Pointer to the memory mapped file.\n"\
"offset : int\n"\
"    Byte offset from where the first item starts.\n"\
"blklen : int\n"\
"    Length of the binary block in bytes.\n"\
"blkpos : int\n"\
"    Indicates the position of the value to search inside a binary block.\n"\
"bitstart : int\n"\
"    First bit position to consider (min 0).\n"\
"bitend : int\n"\
"    Last bit position to consider (max 15).\n"\
"first : int\n"\
"    First element of the range to search (min value = 0).\n"\
"last : int\n"\
"    Last element of the range to search (max value = nitems - 1).\n"\
"search : int\n"\
"    Unsigned 16 bit number to search.\n"\
"\n"\
"Returns\n"\
"-------\n"\
"tuple :\n"\
"    - Item number if found or (last + 1) if not found.\n"\
"    - Position of the 'first' iterator.\n"\
"    - Position of the 'last' iterator."

#define PYFINDFIRSTSUBUINT32_DOCSTRING "Search for the first occurrence of a bit set contained in a 32 bit unsigned integer on a memory mapped binary file containing adjacent blocks of sorted binary data. The 32 bit values in the file must be sorted in ascending order.\n"\
"\n"\
"Parameters\n"\
"----------\n"\
"mfsrc : obj\n"\
"    Pointer to the memory mapped file.\n"\
"offset : int\n"\
"    Byte offset from where the first item starts.\n"\
"blklen : int\n"\
"    Length of the binary block in bytes.\n"\
"blkpos : int\n"\
"    Indicates the position of the value to search inside a binary block.\n"\
"bitstart : int\n"\
"    First bit position to consider (min 0).\n"\
"bitend : int\n"\
"    Last bit position to consider (max 31).\n"\
"first : int\n"\
"    First element of the range to search (min value = 0).\n"\
"last : int\n"\
"    Last element of the range to search (max value = nitems - 1).\n"\
"search : int\n"\
"    Unsigned 32 bit number to search.\n"\
"\n"\
"Returns\n"\
"-------\n"\
"tuple :\n"\
"    - Item number if found or (last + 1) if not found.\n"\
"    - Position of the 'first' iterator.\n"\
"    - Position of the 'last' iterator."

#define PYFINDFIRSTSUBUINT64_DOCSTRING "Search for the first occurrence of a bit set contained in a 64 bit unsigned integer on a memory mapped binary file containing adjacent blocks of sorted binary data. The 64 bit values in the file must be sorted in ascending order.\n"\
"\n"\
"Parameters\n"\
"----------\n"\
"mfsrc : obj\n"\
"    Pointer to the memory mapped file.\n"\
"offset : int\n"\
"    Byte offset from where the first item starts.\n"\
"blklen : int\n"\
"    Length of the binary block in bytes.\n"\
"blkpos : int\n"\
"    Indicates the position of the value to search inside a binary block.\n"\
"bitstart : int\n"\
"    First bit position to consider (min 0).\n"\
"bitend : int\n"\
"    Last bit position to consider (max 63).\n"\
"first : int\n"\
"    First element of the range to search (min value = 0).\n"\
"last : int\n"\
"    Last element of the range to search (max value = nitems - 1).\n"\
"search : int\n"\
"    Unsigned 64 bit number to search.\n"\
"\n"\
"Returns\n"\
"-------\n"\
"tuple :\n"\
"    - Item number if found or (last + 1) if not found.\n"\
"    - Position of the 'first' iterator.\n"\
"    - Position of the 'last' iterator."

// ----------

#define PYFINDLASTSUBUINT8_DOCSTRING "Search for the last occurrence of a bit set contained in a 8 bit unsigned integer on a memory mapped binary file containing adjacent blocks of sorted binary data. The 8 bit values in the file must be sorted in ascending order.\n"\
"\n"\
"Parameters\n"\
"----------\n"\
"mfsrc : obj\n"\
"    Pointer to the memory mapped file.\n"\
"offset : int\n"\
"    Byte offset from where the first item starts.\n"\
"blklen : int\n"\
"    Length of the binary block in bytes.\n"\
"blkpos : int\n"\
"    Indicates the position of the value to search inside a binary block.\n"\
"bitstart : int\n"\
"    First bit position to consider (min 0).\n"\
"bitend : int\n"\
"    Last bit position to consider (max 7).\n"\
"first : int\n"\
"    First element of the range to search (min value = 0).\n"\
"last : int\n"\
"    Last element of the range to search (max value = nitems - 1).\n"\
"search : int\n"\
"    Unsigned 8 bit number to search.\n"\
"\n"\
"Returns\n"\
"-------\n"\
"tuple :\n"\
"    - Item number if found or (last + 1) if not found.\n"\
"    - Position of the 'first' iterator.\n"\
"    - Position of the 'last' iterator."

#define PYFINDLASTSUBUINT16_DOCSTRING "Search for the last occurrence of a bit set contained in a 16 bit unsigned integer on a memory mapped binary file containing adjacent blocks of sorted binary data. The 16 bit values in the file must be sorted in ascending order.\n"\
"\n"\
"Parameters\n"\
"----------\n"\
"mfsrc : obj\n"\
"    Pointer to the memory mapped file.\n"\
"offset : int\n"\
"    Byte offset from where the first item starts.\n"\
"blklen : int\n"\
"    Length of the binary block in bytes.\n"\
"blkpos : int\n"\
"    Indicates the position of the value to search inside a binary block.\n"\
"bitstart : int\n"\
"    First bit position to consider (min 0).\n"\
"bitend : int\n"\
"    Last bit position to consider (max 15).\n"\
"first : int\n"\
"    First element of the range to search (min value = 0).\n"\
"last : int\n"\
"    Last element of the range to search (max value = nitems - 1).\n"\
"search : int\n"\
"    Unsigned 16 bit number to search.\n"\
"\n"\
"Returns\n"\
"-------\n"\
"tuple :\n"\
"    - Item number if found or (last + 1) if not found.\n"\
"    - Position of the 'first' iterator.\n"\
"    - Position of the 'last' iterator."

#define PYFINDLASTSUBUINT32_DOCSTRING "Search for the last occurrence of a bit set contained in a 32 bit unsigned integer on a memory mapped binary file containing adjacent blocks of sorted binary data. The 32 bit values in the file must be sorted in ascending order.\n"\
"\n"\
"Parameters\n"\
"----------\n"\
"mfsrc : obj\n"\
"    Pointer to the memory mapped file.\n"\
"offset : int\n"\
"    Byte offset from where the first item starts.\n"\
"blklen : int\n"\
"    Length of the binary block in bytes.\n"\
"blkpos : int\n"\
"    Indicates the position of the value to search inside a binary block.\n"\
"bitstart : int\n"\
"    First bit position to consider (min 0).\n"\
"bitend : int\n"\
"    Last bit position to consider (max 31).\n"\
"first : int\n"\
"    First element of the range to search (min value = 0).\n"\
"last : int\n"\
"    Last element of the range to search (max value = nitems - 1).\n"\
"search : int\n"\
"    Unsigned 32 bit number to search.\n"\
"\n"\
"Returns\n"\
"-------\n"\
"tuple :\n"\
"    - Item number if found or (last + 1) if not found.\n"\
"    - Position of the 'first' iterator.\n"\
"    - Position of the 'last' iterator."

#define PYFINDLASTSUBUINT64_DOCSTRING "Search for the last occurrence of a bit set contained in a 64 bit unsigned integer on a memory mapped binary file containing adjacent blocks of sorted binary data. The 64 bit values in the file must be sorted in ascending order.\n"\
"\n"\
"Parameters\n"\
"----------\n"\
"mfsrc : obj\n"\
"    Pointer to the memory mapped file.\n"\
"offset : int\n"\
"    Byte offset from where the first item starts.\n"\
"blklen : int\n"\
"    Length of the binary block in bytes.\n"\
"blkpos : int\n"\
"    Indicates the position of the value to search inside a binary block.\n"\
"bitstart : int\n"\
"    First bit position to consider (min 0).\n"\
"bitend : int\n"\
"    Last bit position to consider (max 63).\n"\
"first : int\n"\
"    First element of the range to search (min value = 0).\n"\
"last : int\n"\
"    Last element of the range to search (max value = nitems - 1).\n"\
"search : int\n"\
"    Unsigned 64 bit number to search.\n"\
"\n"\
"Returns\n"\
"-------\n"\
"tuple :\n"\
"    - Item number if found or (last + 1) if not found.\n"\
"    - Position of the 'first' iterator.\n"\
"    - Position of the 'last' iterator."

// ----------

#define PYHASNEXTUINT8_DOCSTRING "Check if the next occurrence of an unsigned integer on a memory mapped binary file containing adjacent blocks of sorted binary data still matches the search value.\n"\
"\n"\
"Parameters\n"\
"----------\n"\
"mfsrc : obj\n"\
"    Pointer to the memory mapped file.\n"\
"offset : int\n"\
"    Byte offset from where the first item starts.\n"\
"blklen : int\n"\
"    Length of the binary block in bytes.\n"\
"blkpos : int\n"\
"    Indicates the position of the value to search inside a binary block.\n"\
"pos : int\n"\
"    Current item position.\n"\
"last : int\n"\
"    Last element of the range to search (max value = nitems - 1).\n"\
"search : int\n"\
"    Unsigned 8 bit number to search.\n"\
"\n"\
"Returns\n"\
"-------\n"\
"tuple :\n"\
"    - 1 if the next item is valid, 0 otherwise.\n"\
"    - Next position."

#define PYHASNEXTUINT16_DOCSTRING "Check if the next occurrence of an unsigned integer on a memory mapped binary file containing adjacent blocks of sorted binary data still matches the search value.\n"\
"\n"\
"Parameters\n"\
"----------\n"\
"mfsrc : obj\n"\
"    Pointer to the memory mapped file.\n"\
"offset : int\n"\
"    Byte offset from where the first item starts.\n"\
"blklen : int\n"\
"    Length of the binary block in bytes.\n"\
"blkpos : int\n"\
"    Indicates the position of the value to search inside a binary block.\n"\
"pos : int\n"\
"    Current item position.\n"\
"last : int\n"\
"    Last element of the range to search (max value = nitems - 1).\n"\
"search : int\n"\
"    Unsigned 16 bit number to search.\n"\
"\n"\
"Returns\n"\
"-------\n"\
"tuple :\n"\
"    - 1 if the next item is valid, 0 otherwise.\n"\
"    - Next position."

#define PYHASNEXTUINT32_DOCSTRING "Check if the next occurrence of an unsigned integer on a memory mapped binary file containing adjacent blocks of sorted binary data still matches the search value.\n"\
"\n"\
"Parameters\n"\
"----------\n"\
"mfsrc : obj\n"\
"    Pointer to the memory mapped file.\n"\
"offset : int\n"\
"    Byte offset from where the first item starts.\n"\
"blklen : int\n"\
"    Length of the binary block in bytes.\n"\
"blkpos : int\n"\
"    Indicates the position of the value to search inside a binary block.\n"\
"pos : int\n"\
"    Current item position.\n"\
"last : int\n"\
"    Last element of the range to search (max value = nitems - 1).\n"\
"search : int\n"\
"    Unsigned 32 bit number to search.\n"\
"\n"\
"Returns\n"\
"-------\n"\
"tuple :\n"\
"    - 1 if the next item is valid, 0 otherwise.\n"\
"    - Next position."

#define PYHASNEXTUINT64_DOCSTRING "Check if the next occurrence of an unsigned integer on a memory mapped binary file containing adjacent blocks of sorted binary data still matches the search value.\n"\
"\n"\
"Parameters\n"\
"----------\n"\
"mfsrc : obj\n"\
"    Pointer to the memory mapped file.\n"\
"offset : int\n"\
"    Byte offset from where the first item starts.\n"\
"blklen : int\n"\
"    Length of the binary block in bytes.\n"\
"blkpos : int\n"\
"    Indicates the position of the value to search inside a binary block.\n"\
"pos : int\n"\
"    Current item position.\n"\
"last : int\n"\
"    Last element of the range to search (max value = nitems - 1).\n"\
"search : int\n"\
"    Unsigned 64 bit number to search.\n"\
"\n"\
"Returns\n"\
"-------\n"\
"tuple :\n"\
"    - 1 if the next item is valid, 0 otherwise.\n"\
"    - Next position."

// ----------

#define PYHASNEXTSUBUINT8_DOCSTRING "Check if the next occurrence of an unsigned integer on a memory mapped binary file containing adjacent blocks of sorted binary data still matches the search value.\n"\
"\n"\
"Parameters\n"\
"----------\n"\
"mfsrc : obj\n"\
"    Pointer to the memory mapped file.\n"\
"offset : int\n"\
"    Byte offset from where the first item starts.\n"\
"blklen : int\n"\
"    Length of the binary block in bytes.\n"\
"blkpos : int\n"\
"    Indicates the position of the value to search inside a binary block.\n"\
"bitstart : int\n"\
"    First bit position to consider (min 0).\n"\
"bitend : int\n"\
"    Last bit position to consider (max 63).\n"\
"pos : int\n"\
"    Current item position.\n"\
"last : int\n"\
"    Last element of the range to search (max value = nitems - 1).\n"\
"search : int\n"\
"    Unsigned 8 bit number to search.\n"\
"\n"\
"Returns\n"\
"-------\n"\
"tuple :\n"\
"    - 1 if the next item is valid, 0 otherwise.\n"\
"    - Next position."

#define PYHASNEXTSUBUINT16_DOCSTRING "Check if the next occurrence of an unsigned integer on a memory mapped binary file containing adjacent blocks of sorted binary data still matches the search value.\n"\
"\n"\
"Parameters\n"\
"----------\n"\
"mfsrc : obj\n"\
"    Pointer to the memory mapped file.\n"\
"offset : int\n"\
"    Byte offset from where the first item starts.\n"\
"blklen : int\n"\
"    Length of the binary block in bytes.\n"\
"blkpos : int\n"\
"    Indicates the position of the value to search inside a binary block.\n"\
"bitstart : int\n"\
"    First bit position to consider (min 0).\n"\
"bitend : int\n"\
"    Last bit position to consider (max 63).\n"\
"pos : int\n"\
"    Current item position.\n"\
"last : int\n"\
"    Last element of the range to search (max value = nitems - 1).\n"\
"search : int\n"\
"    Unsigned 16 bit number to search.\n"\
"\n"\
"Returns\n"\
"-------\n"\
"tuple :\n"\
"    - 1 if the next item is valid, 0 otherwise.\n"\
"    - Next position."

#define PYHASNEXTSUBUINT32_DOCSTRING "Check if the next occurrence of an unsigned integer on a memory mapped binary file containing adjacent blocks of sorted binary data still matches the search value.\n"\
"\n"\
"Parameters\n"\
"----------\n"\
"mfsrc : obj\n"\
"    Pointer to the memory mapped file.\n"\
"offset : int\n"\
"    Byte offset from where the first item starts.\n"\
"blklen : int\n"\
"    Length of the binary block in bytes.\n"\
"blkpos : int\n"\
"    Indicates the position of the value to search inside a binary block.\n"\
"bitstart : int\n"\
"    First bit position to consider (min 0).\n"\
"bitend : int\n"\
"    Last bit position to consider (max 63).\n"\
"pos : int\n"\
"    Current item position.\n"\
"last : int\n"\
"    Last element of the range to search (max value = nitems - 1).\n"\
"search : int\n"\
"    Unsigned 32 bit number to search.\n"\
"\n"\
"Returns\n"\
"-------\n"\
"tuple :\n"\
"    - 1 if the next item is valid, 0 otherwise.\n"\
"    - Next position."

#define PYHASNEXTSUBUINT64_DOCSTRING "Check if the next occurrence of an unsigned integer on a memory mapped binary file containing adjacent blocks of sorted binary data still matches the search value.\n"\
"\n"\
"Parameters\n"\
"----------\n"\
"mfsrc : obj\n"\
"    Pointer to the memory mapped file.\n"\
"offset : int\n"\
"    Byte offset from where the first item starts.\n"\
"blklen : int\n"\
"    Length of the binary block in bytes.\n"\
"blkpos : int\n"\
"    Indicates the position of the value to search inside a binary block.\n"\
"bitstart : int\n"\
"    First bit position to consider (min 0).\n"\
"bitend : int\n"\
"    Last bit position to consider (max 63).\n"\
"pos : int\n"\
"    Current item position.\n"\
"last : int\n"\
"    Last element of the range to search (max value = nitems - 1).\n"\
"search : int\n"\
"    Unsigned 64 bit number to search.\n"\
"\n"\
"Returns\n"\
"-------\n"\
"tuple :\n"\
"    - 1 if the next item is valid, 0 otherwise.\n"\
"    - Next position."

// ----------

#define PYHASPREVUINT8_DOCSTRING "Check if the previous occurrence of an unsigned integer on a memory mapped binary file containing adjacent blocks of sorted binary data still matches the search value.\n"\
"\n"\
"Parameters\n"\
"----------\n"\
"mfsrc : obj\n"\
"    Pointer to the memory mapped file.\n"\
"offset : int\n"\
"    Byte offset from where the first item starts.\n"\
"blklen : int\n"\
"    Length of the binary block in bytes.\n"\
"blkpos : int\n"\
"    Indicates the position of the value to search inside a binary block.\n"\
"first : int\n"\
"    First element of the range to search (min value = 0).\n"\
"pos : int\n"\
"    Current item position.\n"\
"search : int\n"\
"    Unsigned 8 bit number to search.\n"\
"\n"\
"Returns\n"\
"-------\n"\
"tuple :\n"\
"    - 1 if the next item is valid, 0 otherwise.\n"\
"    - Previous position."

#define PYHASPREVUINT16_DOCSTRING "Check if the previous occurrence of an unsigned integer on a memory mapped binary file containing adjacent blocks of sorted binary data still matches the search value.\n"\
"\n"\
"Parameters\n"\
"----------\n"\
"mfsrc : obj\n"\
"    Pointer to the memory mapped file.\n"\
"offset : int\n"\
"    Byte offset from where the first item starts.\n"\
"blklen : int\n"\
"    Length of the binary block in bytes.\n"\
"blkpos : int\n"\
"    Indicates the position of the value to search inside a binary block.\n"\
"first : int\n"\
"    First element of the range to search (min value = 0).\n"\
"pos : int\n"\
"    Current item position.\n"\
"search : int\n"\
"    Unsigned 16 bit number to search.\n"\
"\n"\
"Returns\n"\
"-------\n"\
"tuple :\n"\
"    - 1 if the next item is valid, 0 otherwise.\n"\
"    - Previous position."

#define PYHASPREVUINT32_DOCSTRING "Check if the previous occurrence of an unsigned integer on a memory mapped binary file containing adjacent blocks of sorted binary data still matches the search value.\n"\
"\n"\
"Parameters\n"\
"----------\n"\
"mfsrc : obj\n"\
"    Pointer to the memory mapped file.\n"\
"offset : int\n"\
"    Byte offset from where the first item starts.\n"\
"blklen : int\n"\
"    Length of the binary block in bytes.\n"\
"blkpos : int\n"\
"    Indicates the position of the value to search inside a binary block.\n"\
"first : int\n"\
"    First element of the range to search (min value = 0).\n"\
"pos : int\n"\
"    Current item position.\n"\
"search : int\n"\
"    Unsigned 32 bit number to search.\n"\
"\n"\
"Returns\n"\
"-------\n"\
"tuple :\n"\
"    - 1 if the next item is valid, 0 otherwise.\n"\
"    - Previous position."

#define PYHASPREVUINT64_DOCSTRING "Check if the previous occurrence of an unsigned integer on a memory mapped binary file containing adjacent blocks of sorted binary data still matches the search value.\n"\
"\n"\
"Parameters\n"\
"----------\n"\
"mfsrc : obj\n"\
"    Pointer to the memory mapped file.\n"\
"offset : int\n"\
"    Byte offset from where the first item starts.\n"\
"blklen : int\n"\
"    Length of the binary block in bytes.\n"\
"blkpos : int\n"\
"    Indicates the position of the value to search inside a binary block.\n"\
"first : int\n"\
"    First element of the range to search (min value = 0).\n"\
"pos : int\n"\
"    Current item position.\n"\
"search : int\n"\
"    Unsigned 64 bit number to search.\n"\
"\n"\
"Returns\n"\
"-------\n"\
"tuple :\n"\
"    - 1 if the next item is valid, 0 otherwise.\n"\
"    - Previous position."

// ----------

#define PYHASPREVSUBUINT8_DOCSTRING "Check if the previous occurrence of an unsigned integer on a memory mapped binary file containing adjacent blocks of sorted binary data still matches the search value.\n"\
"\n"\
"Parameters\n"\
"----------\n"\
"mfsrc : obj\n"\
"    Pointer to the memory mapped file.\n"\
"offset : int\n"\
"    Byte offset from where the first item starts.\n"\
"blklen : int\n"\
"    Length of the binary block in bytes.\n"\
"blkpos : int\n"\
"    Indicates the position of the value to search inside a binary block.\n"\
"bitstart : int\n"\
"    First bit position to consider (min 0).\n"\
"bitend : int\n"\
"    Last bit position to consider (max 63).\n"\
"first : int\n"\
"    First element of the range to search (min value = 0).\n"\
"pos : int\n"\
"    Current item position.\n"\
"search : int\n"\
"    Unsigned 8 bit number to search.\n"\
"\n"\
"Returns\n"\
"-------\n"\
"tuple :\n"\
"    - 1 if the next item is valid, 0 otherwise.\n"\
"    - Previous position."

#define PYHASPREVSUBUINT16_DOCSTRING "Check if the previous occurrence of an unsigned integer on a memory mapped binary file containing adjacent blocks of sorted binary data still matches the search value.\n"\
"\n"\
"Parameters\n"\
"----------\n"\
"mfsrc : obj\n"\
"    Pointer to the memory mapped file.\n"\
"offset : int\n"\
"    Byte offset from where the first item starts.\n"\
"blklen : int\n"\
"    Length of the binary block in bytes.\n"\
"blkpos : int\n"\
"    Indicates the position of the value to search inside a binary block.\n"\
"bitstart : int\n"\
"    First bit position to consider (min 0).\n"\
"bitend : int\n"\
"    Last bit position to consider (max 63).\n"\
"first : int\n"\
"    First element of the range to search (min value = 0).\n"\
"pos : int\n"\
"    Current item position.\n"\
"search : int\n"\
"    Unsigned 16 bit number to search.\n"\
"\n"\
"Returns\n"\
"-------\n"\
"tuple :\n"\
"    - 1 if the next item is valid, 0 otherwise.\n"\
"    - Previous position."

#define PYHASPREVSUBUINT32_DOCSTRING "Check if the previous occurrence of an unsigned integer on a memory mapped binary file containing adjacent blocks of sorted binary data still matches the search value.\n"\
"\n"\
"Parameters\n"\
"----------\n"\
"mfsrc : obj\n"\
"    Pointer to the memory mapped file.\n"\
"offset : int\n"\
"    Byte offset from where the first item starts.\n"\
"blklen : int\n"\
"    Length of the binary block in bytes.\n"\
"blkpos : int\n"\
"    Indicates the position of the value to search inside a binary block.\n"\
"bitstart : int\n"\
"    First bit position to consider (min 0).\n"\
"bitend : int\n"\
"    Last bit position to consider (max 63).\n"\
"first : int\n"\
"    First element of the range to search (min value = 0).\n"\
"pos : int\n"\
"    Current item position.\n"\
"search : int\n"\
"    Unsigned 32 bit number to search.\n"\
"\n"\
"Returns\n"\
"-------\n"\
"tuple :\n"\
"    - 1 if the next item is valid, 0 otherwise.\n"\
"    - Previous position."

#define PYHASPREVSUBUINT64_DOCSTRING "Check if the previous occurrence of an unsigned integer on a memory mapped binary file containing adjacent blocks of sorted binary data still matches the search value.\n"\
"\n"\
"Parameters\n"\
"----------\n"\
"mfsrc : obj\n"\
"    Pointer to the memory mapped file.\n"\
"offset : int\n"\
"    Byte offset from where the first item starts.\n"\
"blklen : int\n"\
"    Length of the binary block in bytes.\n"\
"blkpos : int\n"\
"    Indicates the position of the value to search inside a binary block.\n"\
"bitstart : int\n"\
"    First bit position to consider (min 0).\n"\
"bitend : int\n"\
"    Last bit position to consider (max 63).\n"\
"first : int\n"\
"    First element of the range to search (min value = 0).\n"\
"pos : int\n"\
"    Current item position.\n"\
"search : int\n"\
"    Unsigned 64 bit number to search.\n"\
"\n"\
"Returns\n"\
"-------\n"\
"tuple :\n"\
"    - 1 if the next item is valid, 0 otherwise.\n"\
"    - Previous position."

// ----------

// ----------

#define PYCOLFINDFIRSTUINT8_DOCSTRING "Search for the first occurrence of a 8 bit unsigned integer on a memory buffer containing contiguos blocks of sorted binary data. The values must be encoded in Little-Endian format and sorted in ascending order. The 8 bit values in the file must be sorted in ascending order.\n"\
"\n"\
"Parameters\n"\
"----------\n"\
"mfsrc : obj\n"\
"    Pointer to the memory mapped file.\n"\
"offset : int\n"\
"    Byte offset from where the first item starts.\n"\
"first : int\n"\
"    First element of the range to search (min value = 0).\n"\
"last : int\n"\
"    Last element of the range to search (max value = nitems - 1).\n"\
"search : int\n"\
"    Unsigned 8 bit number to search.\n"\
"\n"\
"Returns\n"\
"-------\n"\
"tuple :\n"\
"    - Item number if found or (last + 1) if not found.\n"\
"    - Position of the 'first' iterator.\n"\
"    - Position of the 'last' iterator."

#define PYCOLFINDFIRSTUINT16_DOCSTRING "Search for the first occurrence of a 16 bit unsigned integer on a memory buffer containing contiguos blocks of sorted binary data. The values must be encoded in Little-Endian format and sorted in ascending order. The 16 bit values in the file must be sorted in ascending order.\n"\
"\n"\
"Parameters\n"\
"----------\n"\
"mfsrc : obj\n"\
"    Pointer to the memory mapped file.\n"\
"offset : int\n"\
"    Byte offset from where the first item starts.\n"\
"first : int\n"\
"    First element of the range to search (min value = 0).\n"\
"last : int\n"\
"    Last element of the range to search (max value = nitems - 1).\n"\
"search : int\n"\
"    Unsigned 16 bit number to search.\n"\
"\n"\
"Returns\n"\
"-------\n"\
"tuple :\n"\
"    - Item number if found or (last + 1) if not found.\n"\
"    - Position of the 'first' iterator.\n"\
"    - Position of the 'last' iterator."

#define PYCOLFINDFIRSTUINT32_DOCSTRING "Search for the first occurrence of a 32 bit unsigned integer on a memory buffer containing contiguos blocks of sorted binary data. The values must be encoded in Little-Endian format and sorted in ascending order. The 32 bit values in the file must be sorted in ascending order.\n"\
"\n"\
"Parameters\n"\
"----------\n"\
"mfsrc : obj\n"\
"    Pointer to the memory mapped file.\n"\
"offset : int\n"\
"    Byte offset from where the first item starts.\n"\
"first : int\n"\
"    First element of the range to search (min value = 0).\n"\
"last : int\n"\
"    Last element of the range to search (max value = nitems - 1).\n"\
"search : int\n"\
"    Unsigned 32 bit number to search.\n"\
"\n"\
"Returns\n"\
"-------\n"\
"tuple :\n"\
"    - Item number if found or (last + 1) if not found.\n"\
"    - Position of the 'first' iterator.\n"\
"    - Position of the 'last' iterator."

#define PYCOLFINDFIRSTUINT64_DOCSTRING "Search for the first occurrence of a 64 bit unsigned integer on a memory buffer containing contiguos blocks of sorted binary data. The values must be encoded in Little-Endian format and sorted in ascending order. The 64 bit values in the file must be sorted in ascending order.\n"\
"\n"\
"Parameters\n"\
"----------\n"\
"mfsrc : obj\n"\
"    Pointer to the memory mapped file.\n"\
"offset : int\n"\
"    Byte offset from where the first item starts.\n"\
"first : int\n"\
"    First element of the range to search (min value = 0).\n"\
"last : int\n"\
"    Last element of the range to search (max value = nitems - 1).\n"\
"search : int\n"\
"    Unsigned 64 bit number to search.\n"\
"\n"\
"Returns\n"\
"-------\n"\
"tuple :\n"\
"    - Item number if found or (last + 1) if not found.\n"\
"    - Position of the 'first' iterator.\n"\
"    - Position of the 'last' iterator."

// ----------

#define PYCOLFINDLASTUINT8_DOCSTRING "Search for the last occurrence of a 8 bit unsigned integer on a memory buffer containing contiguos blocks of sorted binary data. The values must be encoded in Little-Endian format and sorted in ascending order. The 8 bit values in the file must be sorted in ascending order.\n"\
"\n"\
"Parameters\n"\
"----------\n"\
"mfsrc : obj\n"\
"    Pointer to the memory mapped file.\n"\
"offset : int\n"\
"    Byte offset from where the first item starts.\n"\
"first : int\n"\
"    First element of the range to search (min value = 0).\n"\
"last : int\n"\
"    Last element of the range to search (max value = nitems - 1).\n"\
"search : int\n"\
"    Unsigned 8 bit number to search.\n"\
"\n"\
"Returns\n"\
"-------\n"\
"tuple :\n"\
"    - Item number if found or (last + 1) if not found.\n"\
"    - Position of the 'first' iterator.\n"\
"    - Position of the 'last' iterator."

#define PYCOLFINDLASTUINT16_DOCSTRING "Search for the last occurrence of a 16 bit unsigned integer on a memory buffer containing contiguos blocks of sorted binary data. The values must be encoded in Little-Endian format and sorted in ascending order. The 16 bit values in the file must be sorted in ascending order.\n"\
"\n"\
"Parameters\n"\
"----------\n"\
"mfsrc : obj\n"\
"    Pointer to the memory mapped file.\n"\
"offset : int\n"\
"    Byte offset from where the first item starts.\n"\
"first : int\n"\
"    First element of the range to search (min value = 0).\n"\
"last : int\n"\
"    Last element of the range to search (max value = nitems - 1).\n"\
"search : int\n"\
"    Unsigned 16 bit number to search.\n"\
"\n"\
"Returns\n"\
"-------\n"\
"tuple :\n"\
"    - Item number if found or (last + 1) if not found.\n"\
"    - Position of the 'first' iterator.\n"\
"    - Position of the 'last' iterator."

#define PYCOLFINDLASTUINT32_DOCSTRING "Search for the last occurrence of a 32 bit unsigned integer on a memory buffer containing contiguos blocks of sorted binary data. The values must be encoded in Little-Endian format and sorted in ascending order. The 32 bit values in the file must be sorted in ascending order.\n"\
"\n"\
"Parameters\n"\
"----------\n"\
"mfsrc : obj\n"\
"    Pointer to the memory mapped file.\n"\
"offset : int\n"\
"    Byte offset from where the first item starts.\n"\
"first : int\n"\
"    First element of the range to search (min value = 0).\n"\
"last : int\n"\
"    Last element of the range to search (max value = nitems - 1).\n"\
"search : int\n"\
"    Unsigned 32 bit number to search.\n"\
"\n"\
"Returns\n"\
"-------\n"\
"tuple :\n"\
"    - Item number if found or (last + 1) if not found.\n"\
"    - Position of the 'first' iterator.\n"\
"    - Position of the 'last' iterator."

#define PYCOLFINDLASTUINT64_DOCSTRING "Search for the last occurrence of a 64 bit unsigned integer on a memory buffer containing contiguos blocks of sorted binary data. The values must be encoded in Little-Endian format and sorted in ascending order. The 64 bit values in the file must be sorted in ascending order.\n"\
"\n"\
"Parameters\n"\
"----------\n"\
"mfsrc : obj\n"\
"    Pointer to the memory mapped file.\n"\
"offset : int\n"\
"    Byte offset from where the first item starts.\n"\
"first : int\n"\
"    First element of the range to search (min value = 0).\n"\
"last : int\n"\
"    Last element of the range to search (max value = nitems - 1).\n"\
"search : int\n"\
"    Unsigned 64 bit number to search.\n"\
"\n"\
"Returns\n"\
"-------\n"\
"tuple :\n"\
"    - Item number if found or (last + 1) if not found.\n"\
"    - Position of the 'first' iterator.\n"\
"    - Position of the 'last' iterator."

// ----------

#define PYCOLFINDFIRSTSUBUINT8_DOCSTRING "Search for the first occurrence of a bit set contained in a 8 bit unsigned integer on a memory buffer containing contiguos blocks of sorted binary data. The values must be encoded in Little-Endian format and sorted in ascending order. The 8 bit values in the file must be sorted in ascending order.\n"\
"\n"\
"Parameters\n"\
"----------\n"\
"mfsrc : obj\n"\
"    Pointer to the memory mapped file.\n"\
"offset : int\n"\
"    Byte offset from where the first item starts.\n"\
"bitstart : int\n"\
"    First bit position to consider (min 0).\n"\
"bitend : int\n"\
"    Last bit position to consider (max 7).\n"\
"first : int\n"\
"    First element of the range to search (min value = 0).\n"\
"last : int\n"\
"    Last element of the range to search (max value = nitems - 1).\n"\
"search : int\n"\
"    Unsigned 8 bit number to search.\n"\
"\n"\
"Returns\n"\
"-------\n"\
"tuple :\n"\
"    - Item number if found or (last + 1) if not found.\n"\
"    - Position of the 'first' iterator.\n"\
"    - Position of the 'last' iterator."

#define PYCOLFINDFIRSTSUBUINT16_DOCSTRING "Search for the first occurrence of a bit set contained in a 16 bit unsigned integer on a memory buffer containing contiguos blocks of sorted binary data. The values must be encoded in Little-Endian format and sorted in ascending order. The 16 bit values in the file must be sorted in ascending order.\n"\
"\n"\
"Parameters\n"\
"----------\n"\
"mfsrc : obj\n"\
"    Pointer to the memory mapped file.\n"\
"offset : int\n"\
"    Byte offset from where the first item starts.\n"\
"bitstart : int\n"\
"    First bit position to consider (min 0).\n"\
"bitend : int\n"\
"    Last bit position to consider (max 15).\n"\
"first : int\n"\
"    First element of the range to search (min value = 0).\n"\
"last : int\n"\
"    Last element of the range to search (max value = nitems - 1).\n"\
"search : int\n"\
"    Unsigned 16 bit number to search.\n"\
"\n"\
"Returns\n"\
"-------\n"\
"tuple :\n"\
"    - Item number if found or (last + 1) if not found.\n"\
"    - Position of the 'first' iterator.\n"\
"    - Position of the 'last' iterator."

#define PYCOLFINDFIRSTSUBUINT32_DOCSTRING "Search for the first occurrence of a bit set contained in a 32 bit unsigned integer on a memory buffer containing contiguos blocks of sorted binary data. The values must be encoded in Little-Endian format and sorted in ascending order. The 32 bit values in the file must be sorted in ascending order.\n"\
"\n"\
"Parameters\n"\
"----------\n"\
"mfsrc : obj\n"\
"    Pointer to the memory mapped file.\n"\
"offset : int\n"\
"    Byte offset from where the first item starts.\n"\
"bitstart : int\n"\
"    First bit position to consider (min 0).\n"\
"bitend : int\n"\
"    Last bit position to consider (max 31).\n"\
"first : int\n"\
"    First element of the range to search (min value = 0).\n"\
"last : int\n"\
"    Last element of the range to search (max value = nitems - 1).\n"\
"search : int\n"\
"    Unsigned 32 bit number to search.\n"\
"\n"\
"Returns\n"\
"-------\n"\
"tuple :\n"\
"    - Item number if found or (last + 1) if not found.\n"\
"    - Position of the 'first' iterator.\n"\
"    - Position of the 'last' iterator."

#define PYCOLFINDFIRSTSUBUINT64_DOCSTRING "Search for the first occurrence of a bit set contained in a 64 bit unsigned integer on a memory buffer containing contiguos blocks of sorted binary data. The values must be encoded in Little-Endian format and sorted in ascending order. The 64 bit values in the file must be sorted in ascending order.\n"\
"\n"\
"Parameters\n"\
"----------\n"\
"mfsrc : obj\n"\
"    Pointer to the memory mapped file.\n"\
"offset : int\n"\
"    Byte offset from where the first item starts.\n"\
"bitstart : int\n"\
"    First bit position to consider (min 0).\n"\
"bitend : int\n"\
"    Last bit position to consider (max 63).\n"\
"first : int\n"\
"    First element of the range to search (min value = 0).\n"\
"last : int\n"\
"    Last element of the range to search (max value = nitems - 1).\n"\
"search : int\n"\
"    Unsigned 64 bit number to search.\n"\
"\n"\
"Returns\n"\
"-------\n"\
"tuple :\n"\
"    - Item number if found or (last + 1) if not found.\n"\
"    - Position of the 'first' iterator.\n"\
"    - Position of the 'last' iterator."

// ----------

#define PYCOLFINDLASTSUBUINT8_DOCSTRING "Search for the last occurrence of a bit set contained in a 8 bit unsigned integer on a memory buffer containing contiguos blocks of sorted binary data. The values must be encoded in Little-Endian format and sorted in ascending order. The 8 bit values in the file must be sorted in ascending order.\n"\
"\n"\
"Parameters\n"\
"----------\n"\
"mfsrc : obj\n"\
"    Pointer to the memory mapped file.\n"\
"offset : int\n"\
"    Byte offset from where the first item starts.\n"\
"bitstart : int\n"\
"    First bit position to consider (min 0).\n"\
"bitend : int\n"\
"    Last bit position to consider (max 7).\n"\
"first : int\n"\
"    First element of the range to search (min value = 0).\n"\
"last : int\n"\
"    Last element of the range to search (max value = nitems - 1).\n"\
"search : int\n"\
"    Unsigned 8 bit number to search.\n"\
"\n"\
"Returns\n"\
"-------\n"\
"tuple :\n"\
"    - Item number if found or (last + 1) if not found.\n"\
"    - Position of the 'first' iterator.\n"\
"    - Position of the 'last' iterator."

#define PYCOLFINDLASTSUBUINT16_DOCSTRING "Search for the last occurrence of a bit set contained in a 16 bit unsigned integer on a memory buffer containing contiguos blocks of sorted binary data. The values must be encoded in Little-Endian format and sorted in ascending order. The 16 bit values in the file must be sorted in ascending order.\n"\
"\n"\
"Parameters\n"\
"----------\n"\
"mfsrc : obj\n"\
"    Pointer to the memory mapped file.\n"\
"offset : int\n"\
"    Byte offset from where the first item starts.\n"\
"bitstart : int\n"\
"    First bit position to consider (min 0).\n"\
"bitend : int\n"\
"    Last bit position to consider (max 15).\n"\
"first : int\n"\
"    First element of the range to search (min value = 0).\n"\
"last : int\n"\
"    Last element of the range to search (max value = nitems - 1).\n"\
"search : int\n"\
"    Unsigned 16 bit number to search.\n"\
"\n"\
"Returns\n"\
"-------\n"\
"tuple :\n"\
"    - Item number if found or (last + 1) if not found.\n"\
"    - Position of the 'first' iterator.\n"\
"    - Position of the 'last' iterator."

#define PYCOLFINDLASTSUBUINT32_DOCSTRING "Search for the last occurrence of a bit set contained in a 32 bit unsigned integer on a memory buffer containing contiguos blocks of sorted binary data. The values must be encoded in Little-Endian format and sorted in ascending order. The 32 bit values in the file must be sorted in ascending order.\n"\
"\n"\
"Parameters\n"\
"----------\n"\
"mfsrc : obj\n"\
"    Pointer to the memory mapped file.\n"\
"offset : int\n"\
"    Byte offset from where the first item starts.\n"\
"bitstart : int\n"\
"    First bit position to consider (min 0).\n"\
"bitend : int\n"\
"    Last bit position to consider (max 31).\n"\
"first : int\n"\
"    First element of the range to search (min value = 0).\n"\
"last : int\n"\
"    Last element of the range to search (max value = nitems - 1).\n"\
"search : int\n"\
"    Unsigned 32 bit number to search.\n"\
"\n"\
"Returns\n"\
"-------\n"\
"tuple :\n"\
"    - Item number if found or (last + 1) if not found.\n"\
"    - Position of the 'first' iterator.\n"\
"    - Position of the 'last' iterator."

#define PYCOLFINDLASTSUBUINT64_DOCSTRING "Search for the last occurrence of a bit set contained in a 64 bit unsigned integer on a memory buffer containing contiguos blocks of sorted binary data. The values must be encoded in Little-Endian format and sorted in ascending order. The 64 bit values in the file must be sorted in ascending order.\n"\
"\n"\
"Parameters\n"\
"----------\n"\
"mfsrc : obj\n"\
"    Pointer to the memory mapped file.\n"\
"offset : int\n"\
"    Byte offset from where the first item starts.\n"\
"bitstart : int\n"\
"    First bit position to consider (min 0).\n"\
"bitend : int\n"\
"    Last bit position to consider (max 63).\n"\
"first : int\n"\
"    First element of the range to search (min value = 0).\n"\
"last : int\n"\
"    Last element of the range to search (max value = nitems - 1).\n"\
"search : int\n"\
"    Unsigned 64 bit number to search.\n"\
"\n"\
"Returns\n"\
"-------\n"\
"tuple :\n"\
"    - Item number if found or (last + 1) if not found.\n"\
"    - Position of the 'first' iterator.\n"\
"    - Position of the 'last' iterator."

// ----------

#define PYCOLHASNEXTUINT8_DOCSTRING "Check if the next occurrence of an unsigned integer on a memory buffer containing contiguos blocks of sorted binary data still matches the search value.\n"\
"\n"\
"Parameters\n"\
"----------\n"\
"mfsrc : obj\n"\
"    Pointer to the memory mapped file.\n"\
"offset : int\n"\
"    Byte offset from where the first item starts.\n"\
"pos : int\n"\
"    Current item position.\n"\
"last : int\n"\
"    Last element of the range to search (max value = nitems - 1).\n"\
"search : int\n"\
"    Unsigned 8 bit number to search.\n"\
"\n"\
"Returns\n"\
"-------\n"\
"tuple :\n"\
"    - 1 if the next item is valid, 0 otherwise.\n"\
"    - Next position."

#define PYCOLHASNEXTUINT16_DOCSTRING "Check if the next occurrence of an unsigned integer on a memory buffer containing contiguos blocks of sorted binary data still matches the search value.\n"\
"\n"\
"Parameters\n"\
"----------\n"\
"mfsrc : obj\n"\
"    Pointer to the memory mapped file.\n"\
"offset : int\n"\
"    Byte offset from where the first item starts.\n"\
"pos : int\n"\
"    Current item position.\n"\
"last : int\n"\
"    Last element of the range to search (max value = nitems - 1).\n"\
"search : int\n"\
"    Unsigned 16 bit number to search.\n"\
"\n"\
"Returns\n"\
"-------\n"\
"tuple :\n"\
"    - 1 if the next item is valid, 0 otherwise.\n"\
"    - Next position."

#define PYCOLHASNEXTUINT32_DOCSTRING "Check if the next occurrence of an unsigned integer on a memory buffer containing contiguos blocks of sorted binary data still matches the search value.\n"\
"\n"\
"Parameters\n"\
"----------\n"\
"mfsrc : obj\n"\
"    Pointer to the memory mapped file.\n"\
"offset : int\n"\
"    Byte offset from where the first item starts.\n"\
"pos : int\n"\
"    Current item position.\n"\
"last : int\n"\
"    Last element of the range to search (max value = nitems - 1).\n"\
"search : int\n"\
"    Unsigned 32 bit number to search.\n"\
"\n"\
"Returns\n"\
"-------\n"\
"tuple :\n"\
"    - 1 if the next item is valid, 0 otherwise.\n"\
"    - Next position."

#define PYCOLHASNEXTUINT64_DOCSTRING "Check if the next occurrence of an unsigned integer on a memory buffer containing contiguos blocks of sorted binary data still matches the search value.\n"\
"\n"\
"Parameters\n"\
"----------\n"\
"mfsrc : obj\n"\
"    Pointer to the memory mapped file.\n"\
"offset : int\n"\
"    Byte offset from where the first item starts.\n"\
"pos : int\n"\
"    Current item position.\n"\
"last : int\n"\
"    Last element of the range to search (max value = nitems - 1).\n"\
"search : int\n"\
"    Unsigned 64 bit number to search.\n"\
"\n"\
"Returns\n"\
"-------\n"\
"tuple :\n"\
"    - 1 if the next item is valid, 0 otherwise.\n"\
"    - Next position."

// ----------

#define PYCOLHASNEXTSUBUINT8_DOCSTRING "Check if the next occurrence of an unsigned integer on a memory buffer containing contiguos blocks of sorted binary data still matches the search value.\n"\
"\n"\
"Parameters\n"\
"----------\n"\
"mfsrc : obj\n"\
"    Pointer to the memory mapped file.\n"\
"offset : int\n"\
"    Byte offset from where the first item starts.\n"\
"bitstart : int\n"\
"    First bit position to consider (min 0).\n"\
"bitend : int\n"\
"    Last bit position to consider (max 63).\n"\
"pos : int\n"\
"    Current item position.\n"\
"last : int\n"\
"    Last element of the range to search (max value = nitems - 1).\n"\
"search : int\n"\
"    Unsigned 8 bit number to search.\n"\
"\n"\
"Returns\n"\
"-------\n"\
"tuple :\n"\
"    - 1 if the next item is valid, 0 otherwise.\n"\
"    - Next position."

#define PYCOLHASNEXTSUBUINT16_DOCSTRING "Check if the next occurrence of an unsigned integer on a memory buffer containing contiguos blocks of sorted binary data still matches the search value.\n"\
"\n"\
"Parameters\n"\
"----------\n"\
"mfsrc : obj\n"\
"    Pointer to the memory mapped file.\n"\
"offset : int\n"\
"    Byte offset from where the first item starts.\n"\
"bitstart : int\n"\
"    First bit position to consider (min 0).\n"\
"bitend : int\n"\
"    Last bit position to consider (max 63).\n"\
"pos : int\n"\
"    Current item position.\n"\
"last : int\n"\
"    Last element of the range to search (max value = nitems - 1).\n"\
"search : int\n"\
"    Unsigned 16 bit number to search.\n"\
"\n"\
"Returns\n"\
"-------\n"\
"tuple :\n"\
"    - 1 if the next item is valid, 0 otherwise.\n"\
"    - Next position."

#define PYCOLHASNEXTSUBUINT32_DOCSTRING "Check if the next occurrence of an unsigned integer on a memory buffer containing contiguos blocks of sorted binary data still matches the search value.\n"\
"\n"\
"Parameters\n"\
"----------\n"\
"mfsrc : obj\n"\
"    Pointer to the memory mapped file.\n"\
"offset : int\n"\
"    Byte offset from where the first item starts.\n"\
"bitstart : int\n"\
"    First bit position to consider (min 0).\n"\
"bitend : int\n"\
"    Last bit position to consider (max 63).\n"\
"pos : int\n"\
"    Current item position.\n"\
"last : int\n"\
"    Last element of the range to search (max value = nitems - 1).\n"\
"search : int\n"\
"    Unsigned 32 bit number to search.\n"\
"\n"\
"Returns\n"\
"-------\n"\
"tuple :\n"\
"    - 1 if the next item is valid, 0 otherwise.\n"\
"    - Next position."

#define PYCOLHASNEXTSUBUINT64_DOCSTRING "Check if the next occurrence of an unsigned integer on a memory buffer containing contiguos blocks of sorted binary data still matches the search value.\n"\
"\n"\
"Parameters\n"\
"----------\n"\
"mfsrc : obj\n"\
"    Pointer to the memory mapped file.\n"\
"offset : int\n"\
"    Byte offset from where the first item starts.\n"\
"bitstart : int\n"\
"    First bit position to consider (min 0).\n"\
"bitend : int\n"\
"    Last bit position to consider (max 63).\n"\
"pos : int\n"\
"    Current item position.\n"\
"last : int\n"\
"    Last element of the range to search (max value = nitems - 1).\n"\
"search : int\n"\
"    Unsigned 64 bit number to search.\n"\
"\n"\
"Returns\n"\
"-------\n"\
"tuple :\n"\
"    - 1 if the next item is valid, 0 otherwise.\n"\
"    - Next position."

// ----------

#define PYCOLHASPREVUINT8_DOCSTRING "Check if the previous occurrence of an unsigned integer on a memory buffer containing contiguos blocks of sorted binary data still matches the search value.\n"\
"\n"\
"Parameters\n"\
"----------\n"\
"mfsrc : obj\n"\
"    Pointer to the memory mapped file.\n"\
"offset : int\n"\
"    Byte offset from where the first item starts.\n"\
"first : int\n"\
"    First element of the range to search (min value = 0).\n"\
"pos : int\n"\
"    Current item position.\n"\
"search : int\n"\
"    Unsigned 8 bit number to search.\n"\
"\n"\
"Returns\n"\
"-------\n"\
"tuple :\n"\
"    - 1 if the next item is valid, 0 otherwise.\n"\
"    - Previous position."

#define PYCOLHASPREVUINT16_DOCSTRING "Check if the previous occurrence of an unsigned integer on a memory buffer containing contiguos blocks of sorted binary data still matches the search value.\n"\
"\n"\
"Parameters\n"\
"----------\n"\
"mfsrc : obj\n"\
"    Pointer to the memory mapped file.\n"\
"offset : int\n"\
"    Byte offset from where the first item starts.\n"\
"first : int\n"\
"    First element of the range to search (min value = 0).\n"\
"pos : int\n"\
"    Current item position.\n"\
"search : int\n"\
"    Unsigned 16 bit number to search.\n"\
"\n"\
"Returns\n"\
"-------\n"\
"tuple :\n"\
"    - 1 if the next item is valid, 0 otherwise.\n"\
"    - Previous position."

#define PYCOLHASPREVUINT32_DOCSTRING "Check if the previous occurrence of an unsigned integer on a memory buffer containing contiguos blocks of sorted binary data still matches the search value.\n"\
"\n"\
"Parameters\n"\
"----------\n"\
"mfsrc : obj\n"\
"    Pointer to the memory mapped file.\n"\
"offset : int\n"\
"    Byte offset from where the first item starts.\n"\
"first : int\n"\
"    First element of the range to search (min value = 0).\n"\
"pos : int\n"\
"    Current item position.\n"\
"search : int\n"\
"    Unsigned 32 bit number to search.\n"\
"\n"\
"Returns\n"\
"-------\n"\
"tuple :\n"\
"    - 1 if the next item is valid, 0 otherwise.\n"\
"    - Previous position."

#define PYCOLHASPREVUINT64_DOCSTRING "Check if the previous occurrence of an unsigned integer on a memory buffer containing contiguos blocks of sorted binary data still matches the search value.\n"\
"\n"\
"Parameters\n"\
"----------\n"\
"mfsrc : obj\n"\
"    Pointer to the memory mapped file.\n"\
"offset : int\n"\
"    Byte offset from where the first item starts.\n"\
"first : int\n"\
"    First element of the range to search (min value = 0).\n"\
"pos : int\n"\
"    Current item position.\n"\
"search : int\n"\
"    Unsigned 64 bit number to search.\n"\
"\n"\
"Returns\n"\
"-------\n"\
"tuple :\n"\
"    - 1 if the next item is valid, 0 otherwise.\n"\
"    - Previous position."

// ----------

#define PYCOLHASPREVSUBUINT8_DOCSTRING "Check if the previous occurrence of an unsigned integer on a memory buffer containing contiguos blocks of sorted binary data still matches the search value.\n"\
"\n"\
"Parameters\n"\
"----------\n"\
"mfsrc : obj\n"\
"    Pointer to the memory mapped file.\n"\
"offset : int\n"\
"    Byte offset from where the first item starts.\n"\
"bitstart : int\n"\
"    First bit position to consider (min 0).\n"\
"bitend : int\n"\
"    Last bit position to consider (max 63).\n"\
"first : int\n"\
"    First element of the range to search (min value = 0).\n"\
"pos : int\n"\
"    Current item position.\n"\
"search : int\n"\
"    Unsigned 8 bit number to search.\n"\
"\n"\
"Returns\n"\
"-------\n"\
"tuple :\n"\
"    - 1 if the next item is valid, 0 otherwise.\n"\
"    - Previous position."

#define PYCOLHASPREVSUBUINT16_DOCSTRING "Check if the previous occurrence of an unsigned integer on a memory buffer containing contiguos blocks of sorted binary data still matches the search value.\n"\
"\n"\
"Parameters\n"\
"----------\n"\
"mfsrc : obj\n"\
"    Pointer to the memory mapped file.\n"\
"offset : int\n"\
"    Byte offset from where the first item starts.\n"\
"bitstart : int\n"\
"    First bit position to consider (min 0).\n"\
"bitend : int\n"\
"    Last bit position to consider (max 63).\n"\
"first : int\n"\
"    First element of the range to search (min value = 0).\n"\
"pos : int\n"\
"    Current item position.\n"\
"search : int\n"\
"    Unsigned 16 bit number to search.\n"\
"\n"\
"Returns\n"\
"-------\n"\
"tuple :\n"\
"    - 1 if the next item is valid, 0 otherwise.\n"\
"    - Previous position."

#define PYCOLHASPREVSUBUINT32_DOCSTRING "Check if the previous occurrence of an unsigned integer on a memory buffer containing contiguos blocks of sorted binary data still matches the search value.\n"\
"\n"\
"Parameters\n"\
"----------\n"\
"mfsrc : obj\n"\
"    Pointer to the memory mapped file.\n"\
"offset : int\n"\
"    Byte offset from where the first item starts.\n"\
"bitstart : int\n"\
"    First bit position to consider (min 0).\n"\
"bitend : int\n"\
"    Last bit position to consider (max 63).\n"\
"first : int\n"\
"    First element of the range to search (min value = 0).\n"\
"pos : int\n"\
"    Current item position.\n"\
"search : int\n"\
"    Unsigned 32 bit number to search.\n"\
"\n"\
"Returns\n"\
"-------\n"\
"tuple :\n"\
"    - 1 if the next item is valid, 0 otherwise.\n"\
"    - Previous position."

#define PYCOLHASPREVSUBUINT64_DOCSTRING "Check if the previous occurrence of an unsigned integer on a memory buffer containing contiguos blocks of sorted binary data still matches the search value.\n"\
"\n"\
"Parameters\n"\
"----------\n"\
"mfsrc : obj\n"\
"    Pointer to the memory mapped file.\n"\
"offset : int\n"\
"    Byte offset from where the first item starts.\n"\
"bitstart : int\n"\
"    First bit position to consider (min 0).\n"\
"bitend : int\n"\
"    Last bit position to consider (max 63).\n"\
"first : int\n"\
"    First element of the range to search (min value = 0).\n"\
"pos : int\n"\
"    Current item position.\n"\
"search : int\n"\
"    Unsigned 64 bit number to search.\n"\
"\n"\
"Returns\n"\
"-------\n"\
"tuple :\n"\
"    - 1 if the next item is valid, 0 otherwise.\n"\
"    - Previous position."

// ----------

// RSIDVAR

#define PYGETVRRSID_DOCSTRING "Returns the rsID at the specified position in the VR file.\n"\
"\n"\
"Parameters\n"\
"----------\n"\
"mfsrc : obj\n"\
"    Pointer to the memory mapped binary file containing the VariantKey to rsID lookup table (vkrs.bin).\n"\
"item : int\n"\
"    Binary block number.\n"\
"\n"\
"Returns\n"\
"-------\n"\
"int :\n"\
"    rsID 32 bit number."

#define PYGETRVVARIANTKEY_DOCSTRING "Returns the VariantKey at the specified position in the RV file.\n"\
"\n"\
"Parameters\n"\
"----------\n"\
"mfsrc : obj\n"\
"    Pointer to the memory mapped binary file containing the rsID to VariantKey lookup table (rsvk.bin).\n"\
"item : int\n"\
"    Binary block number.\n"\
"\n"\
"Returns\n"\
"-------\n"\
"int :\n"\
"    VariantKey 64 bit number."

#define PYFINDRVVARIANTKEYBYRSID_DOCSTRING "Search for the specified rsID and returns the first occurrence of VariantKey in the RV file.\n"\
"\n"\
"Parameters\n"\
"----------\n"\
"mfsrc : obj\n"\
"    Pointer to the memory mapped binary file containing the rsID to VariantKey lookup table (rsvk.bin).\n"\
"first : int\n"\
"    First element of the range to search (min value = 0).\n"\
"last : int\n"\
"    Last element of the range to search (max value = nitems - 1).\n"\
"rsid : int\n"\
"    rsID to search.\n"\
"\n"\
"Returns\n"\
"-------\n"\
"tuple :\n"\
"    - VariantKey or 0 in case not found.\n"\
"    - Item position in the file."

#define PYGETNEXTRVVARIANTKEYBYRSID_DOCSTRING "Get the next VariantKey for the specified rsID in the RV file."\
" This function should be used after find_rv_variantkey_by_rsid."\
" This function can be called in a loop to get all VariantKeys that are associated with the same rsID (if any).\n"\
"\n"\
"Parameters\n"\
"----------\n"\
"mfsrc : obj\n"\
"    Pointer to the memory mapped binary file containing the rsID to VariantKey lookup table (rsvk.bin).\n"\
"pos : int\n"\
"    Current item position.\n"\
"last : int\n"\
"    Last element of the range to search (max value = nitems - 1).\n"\
"rsid : int\n"\
"    rsID to search.\n"\
"\n"\
"Returns\n"\
"-------\n"\
"tuple :\n"\
"    - VariantKey or 0 in case not found.\n"\
"    - Item position in the file."

#define PYFINDALLRVVARIANTKEYBYRSID_DOCSTRING "Search for the specified rsID and returns all associated VariantKeys.\n"\
"\n"\
"Parameters\n"\
"----------\n"\
"mfsrc : obj\n"\
"    Pointer to the memory mapped binary file containing the rsID to VariantKey lookup table (rsvk.bin).\n"\
"first : int\n"\
"    First element of the range to search (min value = 0).\n"\
"last : int\n"\
"    Last element of the range to search (max value = nitems - 1).\n"\
"rsid : int\n"\
"    rsID to search.\n"\
"\n"\
"Returns\n"\
"-------\n"\
"tuple : int\n"\
"    - VariantKey(s)."

#define PYFINDVRRSIDBYVARIANTKEY_DOCSTRING "Search for the specified VariantKey and returns the first occurrence of rsID in the VR file.\n"\
"\n"\
"Parameters\n"\
"----------\n"\
"mfsrc : obj\n"\
"    Pointer to the memory mapped binary file containing the VariantKey to rsID lookup table (vkrs.bin).\n"\
"first : int\n"\
"    First element of the range to search (min value = 0).\n"\
"last : int\n"\
"    Last element of the range to search (max value = nitems - 1).\n"\
"vk : int\n"\
"    VariantKey.\n"\
"\n"\
"Returns\n"\
"-------\n"\
"tuple :\n"\
"    - rsID or 0 in case not found.\n"\
"    - Item position in the file."

#define PYFINDVRCHROMPOSRANGE_DOCSTRING "Search for the specified CHROM-POS range and returns the first occurrence of rsID in the VR file.\n"\
"\n"\
"Parameters\n"\
"----------\n"\
"mfsrc : obj\n"\
"    Pointer to the memory mapped binary file containing the VariantKey to rsID lookup table (vkrs.bin).\n"\
"first : int\n"\
"    First element of the range to search (min value = 0).\n"\
"last : int\n"\
"    Last element of the range to search (max value = nitems - 1).\n"\
"chrom : int\n"\
"    Chromosome encoded number.\n"\
"pos_min : int\n"\
"    Start reference position, with the first base having position 0.\n"\
"pos_max : int\n"\
"    End reference position, with the first base having position 0.\n"\
"\n"\
"Returns\n"\
"-------\n"\
"tuple :\n"\
"    - rsID or 0 in case not found\n"\
"    - Position of the first item.\n"\
"    - Position of the last item."

// ----------

// NRVK

#define PYFINDREFALTBYVARIANTKEY_DOCSTRING "Retrieve the REF and ALT strings for the specified VariantKey.\n"\
"\n"\
"Parameters\n"\
"----------\n"\
"mfsrc : obj\n"\
"    Pointer to the memory mapped input file containing the VariantKey to REF+ALT lookup table (nrvk.bin).\n"\
"last : int\n"\
"    Number of variants in the src file -1.\n"\
"vk : int\n"\
"    VariantKey to search.\n"\
"\n"\
"Returns\n"\
"-------\n"\
"tuple :\n"\
"    - REF string.\n"\
"    - ALT string.\n"\
"    - REF length.\n"\
"    - ALT length.\n"\
"    - REF+ALT length."

#define PYREVERSEVARIANTKEY_DOCSTRING "Reverse a VariantKey code and returns the normalized components.\n"\
"\n"\
"Parameters\n"\
"----------\n"\
"mfsrc : obj\n"\
"    Pointer to the memory mapped input file containing the VariantKey to REF+ALT lookup table (nrvk.bin).\n"\
"last : int\n"\
"    Number of variants in the src file -1. Set this to 0 to skip the lookup table.\n"\
"vk : int\n"\
"    VariantKey code.\n"\
"\n"\
"Returns\n"\
"-------\n"\
"tuple :\n"\
"    - CHROM string.\n"\
"    - POS.\n"\
"    - REF string.\n"\
"    - ALT string.\n"\
"    - REF length.\n"\
"    - ALT length.\n"\
"    - REF+ALT length."

#define PYGETREFLENGTHBYVARIANTKEY_DOCSTRING "Retrieve the REF length for the specified VariantKey.\n"\
"\n"\
"Parameters\n"\
"----------\n"\
"mfsrc : obj\n"\
"    Pointer to the memory mapped input file containing the VariantKey to REF+ALT lookup table (nrvk.bin).\n"\
"last : int\n"\
"    Number of variants in the src file -1.\n"\
"vk : int\n"\
"    VariantKey\n"\
"\n"\
"Returns\n"\
"-------\n"\
"int :\n"\
"    REF length or 0 if the VariantKey is not reversible and not found."

#define PYGETVARIANTKEYENDPOS_DOCSTRING "Get the VariantKey end position (POS + REF length).\n"\
"\n"\
"Parameters\n"\
"----------\n"\
"mfsrc : obj\n"\
"    Pointer to the memory mapped input file containing the VariantKey to REF+ALT lookup table (nrvk.bin).\n"\
"last : int\n"\
"    Number of variants in the src file -1.\n"\
"vk : int\n"\
"    VariantKey.\n"\
"\n"\
"Returns\n"\
"-------\n"\
"int :\n"\
"    Variant end position."

#define PYGETVARIANTKEYCHROMSTARTPOS_DOCSTRING "Get the CHROM + START POS encoding from VariantKey.\n"\
"\n"\
"Parameters\n"\
"----------\n"\
"vk : int\n"\
"    VariantKey.\n"\
"\n"\
"Returns\n"\
"-------\n"\
"int :\n"\
"    CHROM + START POS encoding."

#define PYGETVARIANTKEYCHROMENDPOS_DOCSTRING "Get the CHROM + END POS encoding from VariantKey.\n"\
"\n"\
"Parameters\n"\
"----------\n"\
"mfsrc : obj\n"\
"    Pointer to the memory mapped input file containing the VariantKey to REF+ALT lookup table (nrvk.bin).\n"\
"last : int\n"\
"    Number of variants in the src file -1.\n"\
"vk : int\n"\
"    VariantKey.\n"\
"\n"\
"Returns\n"\
"-------\n"\
"int :\n"\
"    CHROM + END POS encoding."

#define PYVKNRBINTOTSV_DOCSTRING "Convert a vrnr.bin file to a simple TSV.\n"\
"For the reverse operation see the resources/tools/nrvk.sh script.\n"\
"\n"\
"Parameters\n"\
"----------\n"\
"mfsrc : obj\n"\
"    Pointer to the memory mapped input file containing the VariantKey to REF+ALT lookup table (nrvk.bin).\n"\
"last : int\n"\
"    Number of variants in the src file -1. Set this to 0 to skip the lookup table.\n"\
"tsvfile : int\n"\
"    Output file name.\n"\
"\n"\
"Returns\n"\
"-------\n"\
"int :\n"\
"    Number of bytes written or 0 in case of error."

// ----------

// GENOREF

#define PYLOADGENOREFINDEX_DOCSTRING "Return the index object from the genome reference memory mapped file.\n"\
"\n"\
"Parameters\n"\
"----------\n"\
"mfsrc : obj\n"\
"    Pointer to the memory mapped input file containing the genome reference data (fasta.bin).\n"\
"\n"\
"Returns\n"\
"-------\n"\
"obj :\n"\
"    Index of sequences offset by chromosome number (1 to 25)."

#define PYGETGENOREFSEQ_DOCSTRING "Returns the genome reference nucleotide at the specified chromosome and position.\n"\
"\n"\
"Parameters\n"\
"----------\n"\
"mfsrc : obj\n"\
"    Pointer to the memory mapped input file containing the genome reference data (fasta.bin).\n"\
"mfidx : int\n"\
"    File index object.\n"\
"chrom : int\n"\
"    Encoded Chromosome number (see encode_chrom).\n"\
"pos : int\n"\
"    Position. The reference position, with the first base having position 0.\n"\
"\n"\
"Returns\n"\
"-------\n"\
"bytes :\n"\
"    Nucleotide letter or 0 (NULL char) in case of invalid position."

#define PYCHECKREFERENCE_DOCSTRING "Check if the reference allele matches the reference genome data.\n"\
"\n"\
"Parameters\n"\
"----------\n"\
"mfsrc : obj\n"\
"    Pointer to the memory mapped input file containing the genome reference data (fasta.bin).\n"\
"mfidx : int\n"\
"    File index object.\n"\
"chrom :\n"\
"    Encoded Chromosome number (see encode_chrom).\n"\
"pos : int\n"\
"    Position. The reference position, with the first base having position 0.\n"\
"ref : str or bytes\n"\
"    Reference allele. String containing a sequence of nucleotide letters.\n"\
"\n"\
"Returns\n"\
"-------\n"\
"int :\n"\
"    positive number in case of success, negative in case of error:\n"\
"        0 the reference allele match the reference genome;\n"\
"        1 the reference allele is inconsistent with the genome reference (i.e. when contains nucleotide letters other than A, C, G and T);\n"\
"       -1 the reference allele don't match the reference genome;\n"\
"       -2 the reference allele is longer than the genome reference sequence."

#define PYFLIPALLELE_DOCSTRING "Flip the allele nucleotides (replaces each letter with its complement).\n"\
" The resulting string is always in uppercase."\
" Support extended nucleotide letters."\
"\n"\
"Parameters\n"\
"----------\n"\
"allele : str or bytes\n"\
"    String containing a sequence of nucleotide letters.\n"\
"\n"\
"Returns\n"\
"-------\n"\
"bytes :\n"\
"    Flipped allele."

#define PYNORMALIZEVARIANT_DOCSTRING "Normalize a variant."\
" Flip alleles if required and apply the normalization algorithm described at:"\
" https://genome.sph.umich.edu/wiki/Variant_Normalization\n"\
"\n"\
"Parameters\n"\
"----------\n"\
"mfsrc : obj\n"\
"    Pointer to the memory mapped input file containing the genome reference data (fasta.bin).\n"\
"mfidx : int\n"\
"    Index of sequences offset by chromosome number (1 to 25).\n"\
"chrom : int\n"\
"    Chromosome encoded number.\n"\
"pos : int\n"\
"    Position. The reference position, with the first base having position 0.\n"\
"ref : str or bytes\n"\
"    Reference allele. String containing a sequence of nucleotide letters.\n"\
"alt : str or bytes\n"\
"    Alternate non-reference allele string.\n"\
"\n"\
"Returns\n"\
"-------\n"\
"tuple :\n"\
"    - Bitmask number in case of success, negative number in case of error. When positive, each bit has a different meaning when set:\n"\
"         - bit 0 : The reference allele is inconsistent with the genome reference (i.e. when contains nucleotide letters other than A, C, G and T).\n"\
"         - bit 1 : The alleles have been swapped.\n"\
"         - bit 2 : The alleles nucleotides have been flipped (each nucleotide have been replaced with its complement).\n"\
"         - bit 3 : Alleles have been left extended.\n"\
"         - bit 4 : Alleles have been right trimmed.\n"\
"         - bit 5 : Alleles have been left trimmed.\n"\
"    - POS.\n"\
"    - REF string.\n"\
"    - ALT string.\n"\
"    - REF length.\n"\
"    - ALT length."

// ----------

// REGIONKEY

#define PYENCODEREGIONSTRAND_DOCSTRING "Encode the strand direction (-1 > 2, 0 > 0, +1 > 1).\n"\
"\n"\
"Parameters\n"\
"----------\n"\
"strand : int\n"\
"    Strand direction (-1, 0, +1).\n"\
"\n"\
"Returns\n"\
"-------\n"\
"int :\n"\
"    Strand code."

#define PYDECODEREGIONSTRAND_DOCSTRING "Decode the strand direction code (0 > 0, 1 > +1, 2 > -1).\n"\
"\n"\
"Parameters\n"\
"----------\n"\
"strand : int\n"\
"    Strand code.\n"\
"\n"\
"Returns\n"\
"-------\n"\
"int :\n"\
"    Strand direction."

#define PYENCODEREGIONKEY_DOCSTRING "Returns a 64 bit regionkey\n"\
"\n"\
"Parameters\n"\
"----------\n"\
"chrom : int\n"\
"    Encoded Chromosome (see encode_chrom).\n"\
"startpos : int\n"\
"    Start position (zero based).\n"\
"endpos : int\n"\
"    End position (startpos + region_length).\n"\
"strand : int\n"\
"    Encoded Strand direction (-1 > 2, 0 > 0, +1 > 1)\n"\
"\n"\
"Returns\n"\
"-------\n"\
"int :\n"\
"    RegionKey 64 bit code."

#define PYEXTRACTREGIONKEYCHROM_DOCSTRING "Extract the CHROM code from RegionKey.\n"\
"\n"\
"Parameters\n"\
"----------\n"\
"rk : int\n"\
"    RegionKey code.\n"\
"\n"\
"Returns\n"\
"-------\n"\
"int :\n"\
"    CHROM code."

#define PYEXTRACTREGIONKEYSTARTPOS_DOCSTRING "Extract the START POS code from RegionKey.\n"\
"\n"\
"Parameters\n"\
"----------\n"\
"rk : int\n"\
"    RegionKey code.\n"\
"\n"\
"Returns\n"\
"-------\n"\
"int :\n"\
"    START POS."

#define PYEXTRACTREGIONKEYENDPOS_DOCSTRING "Extract the END POS code from RegionKey.\n"\
"\n"\
"Parameters\n"\
"----------\n"\
"rk : int\n"\
"    RegionKey code.\n"\
"\n"\
"Returns\n"\
"-------\n"\
"int :\n"\
"    END POS."

#define PYEXTRACTREGIONKEYSTRAND_DOCSTRING "Extract the STRAND from RegionKey.\n"\
"\n"\
"Parameters\n"\
"----------\n"\
"rk : int\n"\
"    RegionKey code.\n"\
"\n"\
"Returns\n"\
"-------\n"\
"int :\n"\
"    STRAND."

#define PYDECODEREGIONKEY_DOCSTRING "Decode a RegionKey code and returns the components as regionkey_t structure.\n"\
"\n"\
"Parameters\n"\
"----------\n"\
"rk : int\n"\
"    RegionKey code.\n"\
"\n"\
"Returns\n"\
"-------\n"\
"tuple:\n"\
"    - encodeed chromosome\n"\
"    - start position\n"\
"    - end position\n"\
"    - encoded strand"

#define PYREVERSEREGIONKEY_DOCSTRING "Reverse a RegionKey code and returns the normalized components as regionkey_rev_t structure.\n"\
"\n"\
"Parameters\n"\
"----------\n"\
"rk : int\n"\
"    RegionKey code.\n"\
"\n"\
"Returns\n"\
"-------\n"\
"tuple:\n"\
"    - chromosome\n"\
"    - start position\n"\
"    - end position\n"\
"    - strand"

#define PYREGIONKEY_DOCSTRING "Returns a 64 bit regionkey based on CHROM, START POS (0-based), END POS and STRAND.\n"\
"\n"\
"Parameters\n"\
"----------\n"\
"chrom : str or bytes\n"\
"    Chromosome. An identifier from the reference genome, no white-space or leading zeros permitted.\n"\
"sizechrom : int\n"\
"    Length of the chrom string, excluding the terminating null byte.\n"\
"startpos : int\n"\
"    Start position (zero based).\n"\
"endpos : int\n"\
"    End position (startpos + region_length).\n"\
"strand : int\n"\
"    Strand direction (-1, 0, +1)\n"\
"\n"\
"Returns\n"\
"-------\n"\
"int :\n"\
"    RegionKey 64 bit code."

#define PYREGIONKEYHEX_DOCSTRING "Returns RegionKey hexadecimal string (16 characters).\n"\
"\n"\
"Parameters\n"\
"----------\n"\
"rk : int\n"\
"    RegionKey code.\n"\
"\n"\
"Returns\n"\
"-------\n"\
"string :\n"\
"    RegionKey hexadecimal string."

#define PYPARSEREGIONKEYHEX_DOCSTRING "Parses a RegionKey hexadecimal string and returns the code.\n"\
"\n"\
"Parameters\n"\
"----------\n"\
"rs : str or bytes\n"\
"    RegionKey hexadecimal string (it must contain 16 hexadecimal characters).\n"\
"\n"\
"Returns\n"\
"-------\n"\
"int :\n"\
"    A RegionKey code."

#define PYGETREGIONKEYCHROMSTARTPOS_DOCSTRING "Get the CHROM + START POS encoding from RegionKey.\n"\
"\n"\
"Parameters\n"\
"----------\n"\
"rk : int\n"\
"    RegionKey code.\n"\
"\n"\
"Returns\n"\
"-------\n"\
"int :\n"\
"    CHROM + START POS encoding."

#define PYGETREGIONKEYCHROMENDPOS_DOCSTRING "Get the CHROM + END POS encoding from RegionKey.\n"\
"\n"\
"Parameters\n"\
"----------\n"\
"rk : int\n"\
"    RegionKey code.\n"\
"\n"\
"Returns\n"\
"-------\n"\
"int :\n"\
"    CHROM + END POS encoding."

#define PYAREOVERLAPPINGREGIONS_DOCSTRING "Check if two regions are overlapping.\n"\
"\n"\
"Parameters\n"\
"----------\n"\
"a_chrom : int\n"\
"    Region A chromosome code.\n"\
"a_startpos : int\n"\
"    Region A start position.\n"\
"a_endpos : int\n"\
"    Region A end position (startpos + region length).\n"\
"b_chrom : int\n"\
"    Region B chromosome code.\n"\
"b_startpos : int\n"\
"    Region B start position.\n"\
"b_endpos : int\n"\
"    Region B end position (startpos + region length).\n"\
"\n"\
"Returns\n"\
"-------\n"\
"int :\n"\
"    1 if the regions overlap, 0 otherwise."

#define AREOVERLAPPINGREGIONREGIONKEY_DOCSTRING "Check if a region and a regionkey are overlapping.\n"\
"\n"\
"Parameters\n"\
"----------\n"\
"chrom : int\n"\
"    Region A chromosome code.\n"\
"startpos : int\n"\
"    Region A start position.\n"\
"endpos : int\n"\
"    Region A end position (startpos + region length).\n"\
"rk : int\n"\
"    RegionKey B.\n"\
"\n"\
"Returns\n"\
"-------\n"\
"int :\n"\
"    1 if the regions overlap, 0 otherwise."

#define AREOVERLAPPINGREGIONKEYS_DOCSTRING "Check if two regionkeys are overlapping.\n"\
"\n"\
"Parameters\n"\
"----------\n"\
"rka : int\n"\
"    RegionKey A.\n"\
"rkb : int\n"\
"    RegionKey B.\n"\
"\n"\
"Returns\n"\
"-------\n"\
"int :\n"\
"    1 if the regions overlap, 0 otherwise."

#define AREOVERLAPPINGVARIANTKEYREGIONKEY_DOCSTRING "Check if variantkey and regionkey are overlapping.\n"\
"\n"\
"Parameters\n"\
"----------\n"\
"mfsrc : obj\n"\
"    Pointer to the memory mapped input file containing the VariantKey to REF+ALT lookup table (nrvk.bin).\n"\
"last : int\n"\
"    Number of variants in the src file -1.\n"\
"vk : int\n"\
"    VariantKey.\n"\
"rk : int\n"\
"    RegionKey.\n"\
"\n"\
"Returns\n"\
"-------\n"\
"int :\n"\
"    1 if the regions overlap, 0 otherwise."

#define VARIANTKEYTOREGIONKEY_DOCSTRING "Get RegionKey from VariantKey.\n"\
"\n"\
"Parameters\n"\
"----------\n"\
"mfsrc : obj\n"\
"    Pointer to the memory mapped input file containing the VariantKey to REF+ALT lookup table (nrvk.bin).\n"\
"last : int\n"\
"    Number of variants in the src file -1.\n"\
"vk : int\n"\
"    VariantKey.\n"\
"\n"\
"Returns\n"\
"-------\n"\
"int :\n"\
"    A RegionKey code."

// ----------

// ESID

#define ENCODESTRINGID_DOCSTRING "Encode maximum 10 characters of a string into a 64 bit unsigned integer.\n"\
"This function can be used to convert generic string IDs to numeric IDs.\n"\
"Parameters\n"\
"----------\n"\
"str : str or bytes\n"\
"    The string to encode. It must be maximum 10 characters long and support ASCII characters from '!' to 'z'.\n"\
"start : int\n"\
"    First character to encode, starting from 0. To encode the last 10 characters, set this value at (size - 10).\n"\
"\n"\
"Returns\n"\
"-------\n"\
"int :\n"\
"    Encoded string ID."

#define ENCODESTRINGNUMID_DOCSTRING "Encode a string composed by a character section followed by a separator\n"\
"character and a numerical section into a 64 bit unsigned integer. For example: ABCDE:0001234.\n"\
"Encodes up to 5 characters in uppercase, a number up to 2^27, and up to 7 zero padding digits.\n"\
"If the string is 10 character or less, then the encode_string_id() is used.\n"\
"Parameters\n"\
"----------\n"\
"str : str or bytes\n"\
"    The string to encode. It must be maximum 10 characters long and support ASCII characters from '!' to 'z'.\n"\
"sep : str or byte\n"\
"    Separator character between string and number.\n"\
"\n"\
"Returns\n"\
"-------\n"\
"int :\n"\
"    Encoded string ID."

#define DECODESTRINGID_DOCSTRING "Decode the encoded string ID.\n"\
"This function is the reverse of encode_string_id.\n"\
"The string is always returned in uppercase mode.\n"\
"Parameters\n"\
"----------\n"\
"esid : int\n"\
"    Encoded string ID code.\n"\
"\n"\
"Returns\n"\
"-------\n"\
"tuple:\n"\
"    - STRING\n"\
"    - STRING length"

#define HASHSTRINGID_DOCSTRING "Hash the input string into a 64 bit unsigned integer.\n"\
"This function can be used to convert long string IDs to numeric IDs.\n"\
"Parameters\n"\
"----------\n"\
"str : str or bytes\n"\
"    The string to encode.\n"\
"\n"\
"Returns\n"\
"-------\n"\
"int :\n"\
"    Hash string ID."

#if defined(__SUNPRO_C) || defined(__hpux) || defined(_AIX)
#define inline
#endif

#ifdef __linux
#define inline __inline
#endif
