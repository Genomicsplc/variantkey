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
static PyObject *py_munmap_binfile(PyObject *self, PyObject *args, PyObject *keywds);

// RSIDVAR
static PyObject *py_mmap_rsvk_file(PyObject *self, PyObject *args, PyObject *keywds);
static PyObject *py_mmap_vkrs_file(PyObject *self, PyObject *args, PyObject *keywds);
static PyObject *py_find_rv_variantkey_by_rsid(PyObject *self, PyObject *args, PyObject *keywds);
static PyObject *py_get_next_rv_variantkey_by_rsid(PyObject *self, PyObject *args, PyObject *keywds);
static PyObject *py_find_all_rv_variantkey_by_rsid(PyObject *self, PyObject *args, PyObject *keywds);
static PyObject *py_find_vr_rsid_by_variantkey(PyObject *self, PyObject *args, PyObject *keywds);
static PyObject *py_find_vr_chrompos_range(PyObject *self, PyObject *args, PyObject *keywds);

// NRVK
static PyObject *py_mmap_nrvk_file(PyObject *self, PyObject *args, PyObject *keywds);
static PyObject *py_find_ref_alt_by_variantkey(PyObject *self, PyObject *args, PyObject *keywds);
static PyObject *py_reverse_variantkey(PyObject *self, PyObject *args, PyObject *keywds);
static PyObject *py_get_variantkey_ref_length(PyObject *self, PyObject *args, PyObject *keywds);
static PyObject *py_get_variantkey_endpos(PyObject *self, PyObject *args, PyObject *keywds);
static PyObject *py_get_variantkey_chrom_startpos(PyObject *self, PyObject *args, PyObject *keywds);
static PyObject *py_get_variantkey_chrom_endpos(PyObject *self, PyObject *args, PyObject *keywds);
static PyObject *py_nrvk_bin_to_tsv(PyObject *self, PyObject *args, PyObject *keywds);

// GENOREF
static PyObject *py_mmap_genoref_file(PyObject *self, PyObject *args, PyObject *keywds);
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

#define PYMUNMAPBINFILE_DOCSTRING "Unmap and close the memory-mapped file.\n"\
"\n"\
"Parameters\n"\
"----------\n"\
"mf : obj\n"\
"    Pointer to the memory mapped file object.\n"\
"\n"\
"Returns\n"\
"-------\n"\
"int:\n"\
"    On success returns 0, on failure -1."

// ----------

// RSIDVAR

#define PYMMAPRSVKFILE_DOCSTRING "Memory map the specified RSVK binary file (rsvk.bin).\n"\
"\n"\
"Parameters\n"\
"----------\n"\
"file : str\n"\
"    Path to the file to map.\n"\
"ctbytes : int array\n"\
"    Array containing the number of bytes for each column type (i.e. 1 for uint8, 2 for uint16, 4 for uint32, 8 for uint64).\n"\
"\n"\
"Returns\n"\
"-------\n"\
"tuple :\n"\
"    - Pointer to the memory map object.\n"\
"    - Pointer to the memory mapped columns object.\n"\
"    - Number of rows."

#define PYMMAPVKRSFILE_DOCSTRING "Memory map the specified VKRS binary file (vkrs.bin).\n"\
"\n"\
"Parameters\n"\
"----------\n"\
"file : str\n"\
"    Path to the file to map.\n"\
"ctbytes : int array\n"\
"    Array containing the number of bytes for each column type (i.e. 1 for uint8, 2 for uint16, 4 for uint32, 8 for uint64).\n"\
"\n"\
"Returns\n"\
"-------\n"\
"tuple :\n"\
"    - Pointer to the memory map object.\n"\
"    - Pointer to the memory mapped columns object.\n"\
"    - Number of rows."

#define PYFINDRVVARIANTKEYBYRSID_DOCSTRING "Search for the specified rsID and returns the first occurrence of VariantKey in the RV file.\n"\
"\n"\
"Parameters\n"\
"----------\n"\
"mc : obj\n"\
"    Memory-mapped columns object as retured by mmap_rsvk_file().\n"\
"first : int\n"\
"    First element of the range to search (min value = 0).\n"\
"last : int\n"\
"    Element (up to but not including) where to end the search (max value = nitems).\n"\
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
"mc : obj\n"\
"    Memory-mapped columns object as retured by mmap_rsvk_file().\n"\
"pos : int\n"\
"    Current item position.\n"\
"last : int\n"\
"    Element (up to but not including) where to end the search (max value = nitems).\n"\
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
"mc : obj\n"\
"    Memory-mapped columns object as retured by mmap_rsvk_file().\n"\
"first : int\n"\
"    First element of the range to search (min value = 0).\n"\
"last : int\n"\
"    Element (up to but not including) where to end the search (max value = nitems).\n"\
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
"mc : obj\n"\
"    Memory-mapped columns object as retured by mmap_vkrs_file().\n"\
"first : int\n"\
"    First element of the range to search (min value = 0).\n"\
"last : int\n"\
"    Element (up to but not including) where to end the search (max value = nitems).\n"\
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
"mc : obj\n"\
"    Memory-mapped columns object as retured by mmap_vkrs_file().\n"\
"first : int\n"\
"    First element of the range to search (min value = 0).\n"\
"last : int\n"\
"    Element (up to but not including) where to end the search (max value = nitems).\n"\
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

#define PYMMAPNRVKFILE_DOCSTRING "Memory map the specified NRVK binary file (nrvk.bin).\n"\
"\n"\
"Parameters\n"\
"----------\n"\
"file : str\n"\
"    Path to the file to map.\n"\
"ctbytes : int array\n"\
"    Array containing the number of bytes for each column type (i.e. 1 for uint8, 2 for uint16, 4 for uint32, 8 for uint64).\n"\
"\n"\
"Returns\n"\
"-------\n"\
"tuple :\n"\
"    - Pointer to the memory map object.\n"\
"    - Pointer to the memory mapped columns object."

#define PYFINDREFALTBYVARIANTKEY_DOCSTRING "Retrieve the REF and ALT strings for the specified VariantKey.\n"\
"\n"\
"Parameters\n"\
"----------\n"\
"mc : obj\n"\
"    Memory-mapped columns object as retured by mmap_nrvk_file().\n"\
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
"mc : obj\n"\
"    Memory-mapped columns object as retured by mmap_nrvk_file().\n"\
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
"mc : obj\n"\
"    Memory-mapped columns object as retured by mmap_nrvk_file().\n"\
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
"mc : obj\n"\
"    Memory-mapped columns object as retured by mmap_nrvk_file().\n"\
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
"mc : obj\n"\
"    Memory-mapped columns object as retured by mmap_nrvk_file().\n"\
"vk : int\n"\
"    VariantKey.\n"\
"\n"\
"Returns\n"\
"-------\n"\
"int :\n"\
"    CHROM + END POS encoding."

#define PYNRVKBINTOTSV_DOCSTRING "Convert a vrnr.bin file to a simple TSV.\n"\
"For the reverse operation see the resources/tools/nrvk.sh script.\n"\
"\n"\
"Parameters\n"\
"----------\n"\
"mc : obj\n"\
"    Memory-mapped columns object as retured by mmap_nrvk_file().\n"\
"tsvfile : int\n"\
"    Output file name.\n"\
"\n"\
"Returns\n"\
"-------\n"\
"int :\n"\
"    Number of bytes written or 0 in case of error."

// ----------

// GENOREF

#define PYMMAPGENOREFFILE_DOCSTRING "Memory map the specified genome reference binary file (fasta.bin).\n"\
"\n"\
"Parameters\n"\
"----------\n"\
"file : str\n"\
"    Path to the file to map.\n"\
"ctbytes : int array\n"\
"    Array containing the number of bytes for each column type (i.e. 1 for uint8, 2 for uint16, 4 for uint32, 8 for uint64).\n"\
"\n"\
"Returns\n"\
"-------\n"\
"tuple :\n"\
"    - Pointer to the memory map object.\n"\
"    - File size."


#define PYGETGENOREFSEQ_DOCSTRING "Returns the genome reference nucleotide at the specified chromosome and position.\n"\
"\n"\
"Parameters\n"\
"----------\n"\
"mf : obj\n"\
"    Memory-mapped file object as retured by mmap_genoref_file().\n"\
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
"mf : obj\n"\
"    Memory-mapped file object as retured by mmap_genoref_file().\n"\
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
" Supports extended nucleotide letters."\
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
"mf : obj\n"\
"    Memory-mapped file object as retured by mmap_genoref_file().\n"\
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
"    - encoded chromosome\n"\
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
"mc : obj\n"\
"    Memory-mapped columns object as retured by mmap_nrvk_file().\n"\
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
"mc : obj\n"\
"    Memory-mapped columns object as retured by mmap_nrvk_file().\n"\
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
"strid : str or bytes\n"\
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
"strid : str or bytes\n"\
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
"strid : str or bytes\n"\
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
