// Python variantkey Module
//
// @category   Libraries
// @author     Nicola Asuni <nicola.asuni@genomicsplc.com>
// @license    MIT (see LICENSE)
// @link       https://github.com/genomicsplc/variantkey

#define PY_SSIZE_T_CLEAN
#include "Python.h"

// VARIANTKEY
static PyObject *py_encode_assembly_32bit(PyObject *self, PyObject *args);
static PyObject *py_encode_chrom_32bit(PyObject *self, PyObject *args);
static PyObject *py_encode_chrom_8bit(PyObject *self, PyObject *args);
static PyObject *py_decode_chrom_32bit(PyObject *self, PyObject *args);
static PyObject *py_decode_chrom_8bit(PyObject *self, PyObject *args);
static PyObject *py_encode_refalt_32bit(PyObject *self, PyObject *args);
static PyObject *py_encode_refalt_24bit(PyObject *self, PyObject *args);
static PyObject *py_decode_refalt_32bit(PyObject *self, PyObject *args);
static PyObject *py_decode_refalt_24bit(PyObject *self, PyObject *args);
static PyObject *py_variantkey128(PyObject *self, PyObject *args);
static PyObject *py_variantkey64(PyObject *self, PyObject *args);
static PyObject *py_variantkey128_string(PyObject *self, PyObject *args);
static PyObject *py_variantkey64_string(PyObject *self, PyObject *args);
static PyObject *py_parse_variantkey128_string(PyObject *self, PyObject *args);
static PyObject *py_parse_variantkey64_string(PyObject *self, PyObject *args);
static PyObject *py_split_variantkey64(PyObject *self, PyObject *args);

// FARMHASH
static PyObject *py_farmhash64(PyObject *self, PyObject *args);
static PyObject *py_farmhash32(PyObject *self, PyObject *args);

// BINSEARCH
static PyObject *py_mmap_binfile(PyObject *self, PyObject *args);
static PyObject *py_munmap_binfile(PyObject *self, PyObject *args);
static PyObject *py_get_address(PyObject *self, PyObject *args);
static PyObject *py_find_first_uint8(PyObject *self, PyObject *args);
static PyObject *py_find_last_uint8(PyObject *self, PyObject *args);
static PyObject *py_find_first_uint16(PyObject *self, PyObject *args);
static PyObject *py_find_last_uint16(PyObject *self, PyObject *args);
static PyObject *py_find_first_uint32(PyObject *self, PyObject *args);
static PyObject *py_find_last_uint32(PyObject *self, PyObject *args);
static PyObject *py_find_first_uint64(PyObject *self, PyObject *args);
static PyObject *py_find_last_uint64(PyObject *self, PyObject *args);
static PyObject *py_find_first_uint128(PyObject *self, PyObject *args);
static PyObject *py_find_last_uint128(PyObject *self, PyObject *args);

// RSIDVAR128
static PyObject *py_get_vr128_rsid(PyObject *self, PyObject *args);
static PyObject *py_get_rv128_variantkey(PyObject *self, PyObject *args);
static PyObject *py_find_rv128_variantkey_by_rsid(PyObject *self, PyObject *args);
static PyObject *py_find_vr128_rsid_by_variantkey(PyObject *self, PyObject *args);
static PyObject *py_find_vr128_chrom_range(PyObject *self, PyObject *args);
static PyObject *py_find_vr128_pos_range(PyObject *self, PyObject *args);
static PyObject *py_find_vr128_chrompos_range(PyObject *self, PyObject *args);

// RSIDVAR64
static PyObject *py_get_vr64_rsid(PyObject *self, PyObject *args);
static PyObject *py_get_rv64_variantkey(PyObject *self, PyObject *args);
static PyObject *py_find_rv64_variantkey_by_rsid(PyObject *self, PyObject *args);
static PyObject *py_find_vr64_rsid_by_variantkey(PyObject *self, PyObject *args);
static PyObject *py_find_vr64_chrom_range(PyObject *self, PyObject *args);
static PyObject *py_find_vr64_pos_range(PyObject *self, PyObject *args);
static PyObject *py_find_vr64_chrompos_range(PyObject *self, PyObject *args);

PyMODINIT_FUNC initvariantkey(void);

// VARIANTKEY
#define PYENCODEASSEMBLY32BIT_DOCSTRING "Returns 32-bit genome assembly encoding."
#define PYENCODECHROM32BIT_DOCSTRING "Returns 32-bit chromosome encoding."
#define PYENCODECHROM8BIT_DOCSTRING "Returns 8-bit chromosome encoding."
#define PYDECODECHROM32BIT_DOCSTRING "Decode the 32-bit CHROM code."
#define PYDECODECHROM8BIT_DOCSTRING "Decode the 8-bit CHROM code."
#define PYENCODEREFALT32BIT_DOCSTRING "Returns 32-bit reference+alternate encoding."
#define PYENCODEREFALT24BIT_DOCSTRING "Returns 24-bit reference+alternate encoding."
#define PYDECODEREFALT32BIT_DOCSTRING "Decode the 32-bit REF+ALT code if reversible (if it has less than 5 nucleotides in total)."
#define PYDECODEREFALT24BIT_DOCSTRING "Decode the 24-bit REF+ALT code if reversible (if it has less than 4 nucleotides in total)."
#define PYVARIANTKEY128_DOCSTRING "Returns a VariantKey128 structure based on ASSEMBLY, CHROM, POS (0-base), REF, ALT."
#define PYVARIANTKEY64_DOCSTRING "Returns a 64-bit variant code based on CHROM, POS (0-base), REF, ALT."
#define PYVARIANTKEY128STRING_DOCSTRING "Returns VariantKey128 hexadecimal string (32 characters)."
#define PYVARIANTKEY64STRING_DOCSTRING "Returns VariantKey64 hexadecimal string (16 characters)."
#define PYPARSEVARIANTKEY128STRING_DOCSTRING "Parses a VariantKey128 hex string and returns the components."
#define PYPARSEVARIANTKEY64STRING_DOCSTRING "Parses a VariantKey64 hex string and returns the code."
#define PYSPLITVARIANTKEY64_DOCSTRING "Parses a VariantKey64 code and returns the components."

// FARMHASH
#define PYFARMHASH64_DOCSTRING "Returns a 64-bit fingerprint hash for a byte array.\nexample: print libpyfarmhash64.farmhash64('Lorem ipsum dolor sit amet')\n16191328082827683567"
#define PYFARMHASH32_DOCSTRING "Returns a 32-bit fingerprint hash for a byte array.\nexample: print libpyfarmhash64.farmhash32('Lorem ipsum dolor sit amet')\n2990660358"

// BINSEARCH
#define PYMMAPBINFILE_DOCSTRING "Memory map the specified file."
#define PYMUNMAPBINFILE_DOCSTRING "Unmap and close the memory-mapped file."
#define PYGETADDRESS_DOCSTRING "Returns the absolute file start position of the specified item (binary block)."
#define PYFINDFIRSTUINT8_DOCSTRING "Search for the first occurrence of a 8 bit unsigned integer on a memory mapped binary file containing adjacent blocks of sorted binary data. The 8 bit values in the file must encoded in big-endian format and sorted in ascending order."
#define PYFINDLASTUINT8_DOCSTRING "Search for the last occurrence of a 8 bit unsigned integer on a memory mapped binary file containing adjacent blocks of sorted binary data. The 8 bit values in the file must encoded in big-endian format and sorted in ascending order."
#define PYFINDFIRSTUINT16_DOCSTRING "Search for the first occurrence of a 16 bit unsigned integer on a memory mapped binary file containing adjacent blocks of sorted binary data. The 16 bit values in the file must encoded in big-endian format and sorted in ascending order."
#define PYFINDLASTUINT16_DOCSTRING "Search for the last occurrence of a 16 bit unsigned integer on a memory mapped binary file containing adjacent blocks of sorted binary data. The 16 bit values in the file must encoded in big-endian format and sorted in ascending order."
#define PYFINDFIRSTUINT32_DOCSTRING "Search for the first occurrence of a 32 bit unsigned integer on a memory mapped binary file containing adjacent blocks of sorted binary data. The 32 bit values in the file must encoded in big-endian format and sorted in ascending order."
#define PYFINDLASTUINT32_DOCSTRING "Search for the last occurrence of a 32 bit unsigned integer on a memory mapped binary file containing adjacent blocks of sorted binary data. The 32 bit values in the file must encoded in big-endian format and sorted in ascending order."
#define PYFINDFIRSTUINT64_DOCSTRING "Search for the first occurrence of a 64 bit unsigned integer on a memory mapped binary file containing adjacent blocks of sorted binary data. The 64 bit values in the file must encoded in big-endian format and sorted in ascending order."
#define PYFINDLASTUINT64_DOCSTRING "Search for the last occurrence of a 64 bit unsigned integer on a memory mapped binary file containing adjacent blocks of sorted binary data. The 64 bit values in the file must encoded in big-endian format and sorted in ascending order."
#define PYFINDFIRSTUINT128_DOCSTRING "Search for the first occurrence of a 128 bit unsigned integer on a memory mapped binary file containing adjacent blocks of sorted binary data. The 128 bit values in the file must encoded in big-endian format and sorted in ascending order."
#define PYFINDLASTUINT128_DOCSTRING "Search for the last occurrence of a 128 bit unsigned integer on a memory mapped binary file containing adjacent blocks of sorted binary data. The 128 bit values in the file must encoded in big-endian format and sorted in ascending order."

// RSIDVAR128
#define PYGETVR128RSID_DOCSTRING "Returns the RSID at the specified position."
#define PYGETRV128VARIANTKEY_DOCSTRING "Returns the VariantKey128 at the specified position."
#define PYFINDRV128VARIANTKEYBYRSID_DOCSTRING "Search for the specified RSID and returns the first occurrence of VariantKey128."
#define PYFINDVR128RSIDBYVARIANTKEY_DOCSTRING "Search for the specified VariantKey and returns the first occurrence of RSID."
#define PYFINDVR128CHROMRANGE_DOCSTRING "Search for the specified CHROM range and returns the first occurrence of RSID."
#define PYFINDVR128POSRANGE_DOCSTRING "Search for the specified POS range and returns the first occurrence of RSID."
#define PYFINDVR128CHROMPOSRANGE_DOCSTRING "Search for the specified CHROM-POS range and returns the first occurrence of RSID."

// RSIDVAR64
#define PYGETVR64RSID_DOCSTRING "Returns the RSID at the specified position."
#define PYGETRV64VARIANTKEY_DOCSTRING "Returns the VariantKey64 at the specified position."
#define PYFINDRV64VARIANTKEYBYRSID_DOCSTRING "Search for the specified RSID and returns the first occurrence of VariantKey64."
#define PYFINDVR64RSIDBYVARIANTKEY_DOCSTRING "Search for the specified VariantKey and returns the first occurrence of RSID."
#define PYFINDVR64CHROMRANGE_DOCSTRING "Search for the specified CHROM range and returns the first occurrence of RSID."
#define PYFINDVR64POSRANGE_DOCSTRING "Search for the specified POS range and returns the first occurrence of RSID."
#define PYFINDVR64CHROMPOSRANGE_DOCSTRING "Search for the specified CHROM-POS range and returns the first occurrence of RSID."


#if defined(__SUNPRO_C) || defined(__hpux) || defined(_AIX)
#define inline
#endif

#ifdef __linux
#define inline __inline
#endif
