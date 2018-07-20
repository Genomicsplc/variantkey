// Python variantkey Module
//
// @category   Libraries
// @author     Nicola Asuni <nicola.asuni@genomicsplc.com>
// @license    MIT (see LICENSE)
// @link       https://github.com/genomicsplc/variantkey

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
static PyObject *py_find_first_uint8(PyObject *self, PyObject *args, PyObject *keywds);
static PyObject *py_find_first_uint16(PyObject *self, PyObject *args, PyObject *keywds);
static PyObject *py_find_first_uint32(PyObject *self, PyObject *args, PyObject *keywds);
static PyObject *py_find_first_uint64(PyObject *self, PyObject *args, PyObject *keywds);
static PyObject *py_find_last_uint8(PyObject *self, PyObject *args, PyObject *keywds);
static PyObject *py_find_last_uint16(PyObject *self, PyObject *args, PyObject *keywds);
static PyObject *py_find_last_uint32(PyObject *self, PyObject *args, PyObject *keywds);
static PyObject *py_find_last_uint64(PyObject *self, PyObject *args, PyObject *keywds);
static PyObject *py_find_first_sub_uint8(PyObject *self, PyObject *args, PyObject *keywds);
static PyObject *py_find_first_sub_uint16(PyObject *self, PyObject *args, PyObject *keywds);
static PyObject *py_find_first_sub_uint32(PyObject *self, PyObject *args, PyObject *keywds);
static PyObject *py_find_first_sub_uint64(PyObject *self, PyObject *args, PyObject *keywds);
static PyObject *py_find_last_sub_uint8(PyObject *self, PyObject *args, PyObject *keywds);
static PyObject *py_find_last_sub_uint16(PyObject *self, PyObject *args, PyObject *keywds);
static PyObject *py_find_last_sub_uint32(PyObject *self, PyObject *args, PyObject *keywds);
static PyObject *py_find_last_sub_uint64(PyObject *self, PyObject *args, PyObject *keywds);
static PyObject *py_has_next_uint8(PyObject *self, PyObject *args, PyObject *keywds);
static PyObject *py_has_next_uint16(PyObject *self, PyObject *args, PyObject *keywds);
static PyObject *py_has_next_uint32(PyObject *self, PyObject *args, PyObject *keywds);
static PyObject *py_has_next_uint64(PyObject *self, PyObject *args, PyObject *keywds);
static PyObject *py_has_next_sub_uint8(PyObject *self, PyObject *args, PyObject *keywds);
static PyObject *py_has_next_sub_uint16(PyObject *self, PyObject *args, PyObject *keywds);
static PyObject *py_has_next_sub_uint32(PyObject *self, PyObject *args, PyObject *keywds);
static PyObject *py_has_next_sub_uint64(PyObject *self, PyObject *args, PyObject *keywds);
static PyObject *py_has_prev_uint8(PyObject *self, PyObject *args, PyObject *keywds);
static PyObject *py_has_prev_uint16(PyObject *self, PyObject *args, PyObject *keywds);
static PyObject *py_has_prev_uint32(PyObject *self, PyObject *args, PyObject *keywds);
static PyObject *py_has_prev_uint64(PyObject *self, PyObject *args, PyObject *keywds);
static PyObject *py_has_prev_sub_uint8(PyObject *self, PyObject *args, PyObject *keywds);
static PyObject *py_has_prev_sub_uint16(PyObject *self, PyObject *args, PyObject *keywds);
static PyObject *py_has_prev_sub_uint32(PyObject *self, PyObject *args, PyObject *keywds);
static PyObject *py_has_prev_sub_uint64(PyObject *self, PyObject *args, PyObject *keywds);

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

// GENOREF
static PyObject *py_load_genoref_index(PyObject *self, PyObject *args, PyObject *keywds);
static PyObject *py_get_genoref_seq(PyObject *self, PyObject *args, PyObject *keywds);
static PyObject *py_check_reference(PyObject *self, PyObject *args, PyObject *keywds);
static PyObject *py_flip_allele(PyObject *self, PyObject *args, PyObject *keywds);
static PyObject *py_normalize_variant(PyObject *self, PyObject *args, PyObject *keywds);

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
"code :\n"\
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
"chrom :\n"\
"    Encoded Chromosome (see encode_chrom).\n"\
"pos :\n"\
"    Position. The reference position, with the first base having position 0.\n"\
"refalt :\n"\
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
"vk :\n"\
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
"vk :\n"\
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
"vk :\n"\
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
"vk :\n"\
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
"chrom :\n"\
"    Chromosome. An identifier from the reference genome, no white-space or leading zeros permitted.\n"\
"pos :\n"\
"    Position. The reference position, with the first base having position 0.\n"\
"ref :\n"\
"    Reference allele. String containing a sequence of nucleotide letters. The value in the pos field refers to the position of the first nucleotide in the String. Characters must be A-Z, a-z or *\n"\
"alt :\n"\
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
"chrom :\n"\
"    Chromosome encoded number.\n"\
"pos_min :\n"\
"    Start reference position, with the first base having position 0.\n"\
"pos_max :\n"\
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
"vka :\n"\
"    The first VariantKey to be compared.\n"\
"vkb :\n"\
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
"vka :\n"\
"    The first VariantKey to be compared.\n"\
"vkb :\n"\
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
"vk :\n"\
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
"vs :\n"\
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

#define PYFINDFIRSTUINT8_DOCSTRING "Search for the first occurrence of a 8 bit unsigned integer on a memory mapped binary file containing adjacent blocks of sorted binary data. The 8 bit values in the file must encoded in big-endian format and sorted in ascending order.\n"\
"\n"\
"Parameters\n"\
"----------\n"\
"mfsrc : obj\n"\
"    Pointer to the memory mapped file.\n"\
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

#define PYFINDFIRSTUINT16_DOCSTRING "Search for the first occurrence of a 16 bit unsigned integer on a memory mapped binary file containing adjacent blocks of sorted binary data. The 16 bit values in the file must encoded in big-endian format and sorted in ascending order.\n"\
"\n"\
"Parameters\n"\
"----------\n"\
"mfsrc : obj\n"\
"    Pointer to the memory mapped file.\n"\
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

#define PYFINDFIRSTUINT32_DOCSTRING "Search for the first occurrence of a 32 bit unsigned integer on a memory mapped binary file containing adjacent blocks of sorted binary data. The 32 bit values in the file must encoded in big-endian format and sorted in ascending order.\n"\
"\n"\
"Parameters\n"\
"----------\n"\
"mfsrc : obj\n"\
"    Pointer to the memory mapped file.\n"\
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

#define PYFINDFIRSTUINT64_DOCSTRING "Search for the first occurrence of a 64 bit unsigned integer on a memory mapped binary file containing adjacent blocks of sorted binary data. The 64 bit values in the file must encoded in big-endian format and sorted in ascending order.\n"\
"\n"\
"Parameters\n"\
"----------\n"\
"mfsrc : obj\n"\
"    Pointer to the memory mapped file.\n"\
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

#define PYFINDLASTUINT8_DOCSTRING "Search for the last occurrence of a 8 bit unsigned integer on a memory mapped binary file containing adjacent blocks of sorted binary data. The 8 bit values in the file must encoded in big-endian format and sorted in ascending order.\n"\
"\n"\
"Parameters\n"\
"----------\n"\
"mfsrc : obj\n"\
"    Pointer to the memory mapped file.\n"\
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

#define PYFINDLASTUINT16_DOCSTRING "Search for the last occurrence of a 16 bit unsigned integer on a memory mapped binary file containing adjacent blocks of sorted binary data. The 16 bit values in the file must encoded in big-endian format and sorted in ascending order.\n"\
"\n"\
"Parameters\n"\
"----------\n"\
"mfsrc : obj\n"\
"    Pointer to the memory mapped file.\n"\
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

#define PYFINDLASTUINT32_DOCSTRING "Search for the last occurrence of a 32 bit unsigned integer on a memory mapped binary file containing adjacent blocks of sorted binary data. The 32 bit values in the file must encoded in big-endian format and sorted in ascending order.\n"\
"\n"\
"Parameters\n"\
"----------\n"\
"mfsrc : obj\n"\
"    Pointer to the memory mapped file.\n"\
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

#define PYFINDLASTUINT64_DOCSTRING "Search for the last occurrence of a 64 bit unsigned integer on a memory mapped binary file containing adjacent blocks of sorted binary data. The 64 bit values in the file must encoded in big-endian format and sorted in ascending order.\n"\
"\n"\
"Parameters\n"\
"----------\n"\
"mfsrc : obj\n"\
"    Pointer to the memory mapped file.\n"\
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

#define PYFINDFIRSTSUBUINT8_DOCSTRING "Search for the first occurrence of a bit set contained in a 8 bit unsigned integer on a memory mapped binary file containing adjacent blocks of sorted binary data. The 8 bit values in the file must encoded in big-endian format and sorted in ascending order.\n"\
"\n"\
"Parameters\n"\
"----------\n"\
"mfsrc : obj\n"\
"    Pointer to the memory mapped file.\n"\
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

#define PYFINDFIRSTSUBUINT16_DOCSTRING "Search for the first occurrence of a bit set contained in a 16 bit unsigned integer on a memory mapped binary file containing adjacent blocks of sorted binary data. The 16 bit values in the file must encoded in big-endian format and sorted in ascending order.\n"\
"\n"\
"Parameters\n"\
"----------\n"\
"mfsrc : obj\n"\
"    Pointer to the memory mapped file.\n"\
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

#define PYFINDFIRSTSUBUINT32_DOCSTRING "Search for the first occurrence of a bit set contained in a 32 bit unsigned integer on a memory mapped binary file containing adjacent blocks of sorted binary data. The 32 bit values in the file must encoded in big-endian format and sorted in ascending order.\n"\
"\n"\
"Parameters\n"\
"----------\n"\
"mfsrc : obj\n"\
"    Pointer to the memory mapped file.\n"\
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

#define PYFINDFIRSTSUBUINT64_DOCSTRING "Search for the first occurrence of a bit set contained in a 64 bit unsigned integer on a memory mapped binary file containing adjacent blocks of sorted binary data. The 64 bit values in the file must encoded in big-endian format and sorted in ascending order.\n"\
"\n"\
"Parameters\n"\
"----------\n"\
"mfsrc : obj\n"\
"    Pointer to the memory mapped file.\n"\
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

#define PYFINDLASTSUBUINT8_DOCSTRING "Search for the last occurrence of a bit set contained in a 8 bit unsigned integer on a memory mapped binary file containing adjacent blocks of sorted binary data. The 8 bit values in the file must encoded in big-endian format and sorted in ascending order.\n"\
"\n"\
"Parameters\n"\
"----------\n"\
"mfsrc : obj\n"\
"    Pointer to the memory mapped file.\n"\
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

#define PYFINDLASTSUBUINT16_DOCSTRING "Search for the last occurrence of a bit set contained in a 16 bit unsigned integer on a memory mapped binary file containing adjacent blocks of sorted binary data. The 16 bit values in the file must encoded in big-endian format and sorted in ascending order.\n"\
"\n"\
"Parameters\n"\
"----------\n"\
"mfsrc : obj\n"\
"    Pointer to the memory mapped file.\n"\
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

#define PYFINDLASTSUBUINT32_DOCSTRING "Search for the last occurrence of a bit set contained in a 32 bit unsigned integer on a memory mapped binary file containing adjacent blocks of sorted binary data. The 32 bit values in the file must encoded in big-endian format and sorted in ascending order.\n"\
"\n"\
"Parameters\n"\
"----------\n"\
"mfsrc : obj\n"\
"    Pointer to the memory mapped file.\n"\
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

#define PYFINDLASTSUBUINT64_DOCSTRING "Search for the last occurrence of a bit set contained in a 64 bit unsigned integer on a memory mapped binary file containing adjacent blocks of sorted binary data. The 64 bit values in the file must encoded in big-endian format and sorted in ascending order.\n"\
"\n"\
"Parameters\n"\
"----------\n"\
"mfsrc : obj\n"\
"    Pointer to the memory mapped file.\n"\
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


// NRVK

#define PYFINDREFALTBYVARIANTKEY_DOCSTRING "Retrieve the REF and ALT strings for the specified VariantKey.\n"\
"\n"\
"Parameters\n"\
"----------\n"\
"mfsrc : obj\n"\
"    Pointer to the memory mapped input file containing the VariantKey to REF+ALT lookup table (vknr.bin).\n"\
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
"    Pointer to the memory mapped input file containing the VariantKey to REF+ALT lookup table (vknr.bin).\n"\
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

// GENOREF
#define PYLOADGENOREFINDEX "Return the index object from the genome reference memory mapped file.\n"\
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

#define PYGETGENOREFSEQ "Returns the genome reference nucleotide at the specified chromosome and position.\n"\
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

#define PYCHECKREFERENCE "Check if the reference allele matches the reference genome data.\n"\
"\n"\
"Parameters\n"\
"----------\n"\
"mfsrc :\n"\
"    Pointer to the memory mapped input file containing the genome reference data (fasta.bin).\n"\
"mfidx : int\n"\
"    File index object.\n"\
"chrom :\n"\
"    Encoded Chromosome number (see encode_chrom).\n"\
"pos :\n"\
"    Position. The reference position, with the first base having position 0.\n"\
"ref :\n"\
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

#define PYFLIPALLELE "Flip the allele nucleotides (replaces each letter with its complement).\n"\
" The resulting string is always in uppercase."\
" Support extended nucleotide letters."\
"\n"\
"Parameters\n"\
"----------\n"\
"allele :\n"\
"    String containing a sequence of nucleotide letters.\n"\
"\n"\
"Returns\n"\
"-------\n"\
"bytes :\n"\
"    Flipped allele."

#define PYNORMALIZEVARIANT "Normalize a variant."\
" Flip alleles if required and apply the normalization algorithm described at:"\
" https://genome.sph.umich.edu/wiki/Variant_Normalization\n"\
"\n"\
"Parameters\n"\
"----------\n"\
"mfsrc :\n"\
"    Pointer to the memory mapped input file containing the genome reference data (fasta.bin).\n"\
"mfidx :\n"\
"    Index of sequences offset by chromosome number (1 to 25).\n"\
"chrom :\n"\
"    Chromosome encoded number.\n"\
"pos :\n"\
"    Position. The reference position, with the first base having position 0.\n"\
"ref :\n"\
"    Reference allele. String containing a sequence of nucleotide letters.\n"\
"alt :\n"\
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

#if defined(__SUNPRO_C) || defined(__hpux) || defined(_AIX)
#define inline
#endif

#ifdef __linux
#define inline __inline
#endif
