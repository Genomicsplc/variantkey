// Python variantkey Module
//
// @category   Libraries
// @author     Nicola Asuni <nicola.asuni@genomicsplc.com>
// @license    MIT (see LICENSE)
// @link       https://github.com/genomicsplc/variantkey

#define PY_SSIZE_T_CLEAN
#include <Python.h>

// VARIANTKEY
static PyObject *py_encode_chrom(PyObject *self, PyObject *args);
static PyObject *py_decode_chrom(PyObject *self, PyObject *args);
static PyObject *py_encode_refalt(PyObject *self, PyObject *args);
static PyObject *py_decode_refalt(PyObject *self, PyObject *args);
static PyObject *py_encode_variantkey(PyObject *self, PyObject *args);
static PyObject *py_extract_variantkey_chrom(PyObject *self, PyObject *args);
static PyObject *py_extract_variantkey_pos(PyObject *self, PyObject *args);
static PyObject *py_extract_variantkey_refalt(PyObject *self, PyObject *args);
static PyObject *py_decode_variantkey(PyObject *self, PyObject *args);
static PyObject *py_variantkey(PyObject *self, PyObject *args);
static PyObject *py_variantkey_range(PyObject *self, PyObject *args);
static PyObject *py_compare_variantkey_chrom(PyObject *self, PyObject *args);
static PyObject *py_compare_variantkey_chrom_pos(PyObject *self, PyObject *args);
static PyObject *py_variantkey_hex(PyObject *self, PyObject *args);
static PyObject *py_parse_variantkey_hex(PyObject *self, PyObject *args);

// BINSEARCH
static PyObject *py_mmap_binfile(PyObject *self, PyObject *args);
static PyObject *py_munmap_binfile(PyObject *self, PyObject *args);
static PyObject *py_get_address(PyObject *self, PyObject *args);
static PyObject *py_find_first_uint8(PyObject *self, PyObject *args);
static PyObject *py_find_first_uint16(PyObject *self, PyObject *args);
static PyObject *py_find_first_uint32(PyObject *self, PyObject *args);
static PyObject *py_find_first_uint64(PyObject *self, PyObject *args);
static PyObject *py_find_last_uint8(PyObject *self, PyObject *args);
static PyObject *py_find_last_uint16(PyObject *self, PyObject *args);
static PyObject *py_find_last_uint32(PyObject *self, PyObject *args);
static PyObject *py_find_last_uint64(PyObject *self, PyObject *args);
static PyObject *py_find_first_sub_uint8(PyObject *self, PyObject *args);
static PyObject *py_find_first_sub_uint16(PyObject *self, PyObject *args);
static PyObject *py_find_first_sub_uint32(PyObject *self, PyObject *args);
static PyObject *py_find_first_sub_uint64(PyObject *self, PyObject *args);
static PyObject *py_find_last_sub_uint8(PyObject *self, PyObject *args);
static PyObject *py_find_last_sub_uint16(PyObject *self, PyObject *args);
static PyObject *py_find_last_sub_uint32(PyObject *self, PyObject *args);
static PyObject *py_find_last_sub_uint64(PyObject *self, PyObject *args);
static PyObject *py_has_next_uint8(PyObject *self, PyObject *args);
static PyObject *py_has_next_uint16(PyObject *self, PyObject *args);
static PyObject *py_has_next_uint32(PyObject *self, PyObject *args);
static PyObject *py_has_next_uint64(PyObject *self, PyObject *args);
static PyObject *py_has_next_sub_uint8(PyObject *self, PyObject *args);
static PyObject *py_has_next_sub_uint16(PyObject *self, PyObject *args);
static PyObject *py_has_next_sub_uint32(PyObject *self, PyObject *args);
static PyObject *py_has_next_sub_uint64(PyObject *self, PyObject *args);
static PyObject *py_has_prev_uint8(PyObject *self, PyObject *args);
static PyObject *py_has_prev_uint16(PyObject *self, PyObject *args);
static PyObject *py_has_prev_uint32(PyObject *self, PyObject *args);
static PyObject *py_has_prev_uint64(PyObject *self, PyObject *args);
static PyObject *py_has_prev_sub_uint8(PyObject *self, PyObject *args);
static PyObject *py_has_prev_sub_uint16(PyObject *self, PyObject *args);
static PyObject *py_has_prev_sub_uint32(PyObject *self, PyObject *args);
static PyObject *py_has_prev_sub_uint64(PyObject *self, PyObject *args);

// RSIDVAR
static PyObject *py_get_vr_rsid(PyObject *self, PyObject *args);
static PyObject *py_get_rv_variantkey(PyObject *self, PyObject *args);
static PyObject *py_find_rv_variantkey_by_rsid(PyObject *self, PyObject *args);
static PyObject *py_get_next_rv_variantkey_by_rsid(PyObject *self, PyObject *args);
static PyObject *py_find_all_rv_variantkey_by_rsid(PyObject *self, PyObject *args);
static PyObject *py_find_vr_rsid_by_variantkey(PyObject *self, PyObject *args);
static PyObject *py_find_vr_chrompos_range(PyObject *self, PyObject *args);

// NRVK
static PyObject *py_find_ref_alt_by_variantkey(PyObject *self, PyObject *args);
static PyObject *py_reverse_variantkey(PyObject *self, PyObject *args);

// GENOREF
static PyObject *py_load_genoref_index(PyObject *self, PyObject *args);
static PyObject *py_get_genoref_seq(PyObject *self, PyObject *args);
static PyObject *py_check_reference(PyObject *self, PyObject *args);
static PyObject *py_flip_allele(PyObject *self, PyObject *args);
static PyObject *py_normalize_variant(PyObject *self, PyObject *args);

PyMODINIT_FUNC initvariantkey(void);

// VARIANTKEY
#define PYENCODECHROM_DOCSTRING "Returns chromosome numerical encoding."
#define PYDECODECHROM_DOCSTRING "Decode the chromosome numerical code."
#define PYENCODEREFALT_DOCSTRING "Returns reference+alternate numerical encoding."
#define PYDECODEREFALT_DOCSTRING "Decode the 32 bit REF+ALT code if reversible (if it has 11 or less bases in total and only contains ACGT letters)."
#define PYENCODEVARIANTKEY_DOCSTRING "Returns a 64 bit variant key based on pre-encoded CHROM, POS (0-based), REF+ALT."
#define PYEXTRACTVARIANTKEYCHROM_DOCSTRING "Extract the CHROM code from VariantKey."
#define PYEXTRACTVARIANTKEYPOS_DOCSTRING "Extract the POS code from VariantKey."
#define PYEXTRACTVARIANTKEYREFALT_DOCSTRING "Extract the REF+ALT code from VariantKey."
#define PYDECODEVARIANTKEY_DOCSTRING "Decode a VariantKey code and returns the components."
#define PYVARIANTKEY_DOCSTRING "Returns a 64 bit variant key based on CHROM, POS (0-based), REF, ALT."
#define PYVARIANTKEYRANGE_DOCSTRING "Returns minimum and maximum VariantKeys for range searches."
#define PYCOMPAREVARIANTKEYCHROM_DOCSTRING "Compares two VariantKeys by chromosome only."
#define PYCOMPAREVARIANTKEYCHROMPOS_DOCSTRING "Compares two VariantKeys by chromosome and position."
#define PYVARIANTKEYHEX_DOCSTRING "Returns VariantKey hexadecimal string (16 characters)."
#define PYPARSEVARIANTKEYSTRING_DOCSTRING "Parses a VariantKey hexadecimal string and returns the code."

// BINSEARCH
#define PYMMAPBINFILE_DOCSTRING "Memory map the specified file."
#define PYMUNMAPBINFILE_DOCSTRING "Unmap and close the memory-mapped file."
#define PYGETADDRESS_DOCSTRING "Returns the absolute file start position of the specified item (binary block)."
#define PYFINDFIRSTUINT8_DOCSTRING "Search for the first occurrence of a 8 bit unsigned integer on a memory mapped binary file containing adjacent blocks of sorted binary data. The 8 bit values in the file must encoded in big-endian format and sorted in ascending order."
#define PYFINDFIRSTUINT16_DOCSTRING "Search for the first occurrence of a 16 bit unsigned integer on a memory mapped binary file containing adjacent blocks of sorted binary data. The 16 bit values in the file must encoded in big-endian format and sorted in ascending order."
#define PYFINDFIRSTUINT32_DOCSTRING "Search for the first occurrence of a 32 bit unsigned integer on a memory mapped binary file containing adjacent blocks of sorted binary data. The 32 bit values in the file must encoded in big-endian format and sorted in ascending order."
#define PYFINDFIRSTUINT64_DOCSTRING "Search for the first occurrence of a 64 bit unsigned integer on a memory mapped binary file containing adjacent blocks of sorted binary data. The 64 bit values in the file must encoded in big-endian format and sorted in ascending order."
#define PYFINDLASTUINT8_DOCSTRING "Search for the last occurrence of a 8 bit unsigned integer on a memory mapped binary file containing adjacent blocks of sorted binary data. The 8 bit values in the file must encoded in big-endian format and sorted in ascending order."
#define PYFINDLASTUINT16_DOCSTRING "Search for the last occurrence of a 16 bit unsigned integer on a memory mapped binary file containing adjacent blocks of sorted binary data. The 16 bit values in the file must encoded in big-endian format and sorted in ascending order."
#define PYFINDLASTUINT32_DOCSTRING "Search for the last occurrence of a 32 bit unsigned integer on a memory mapped binary file containing adjacent blocks of sorted binary data. The 32 bit values in the file must encoded in big-endian format and sorted in ascending order."
#define PYFINDLASTUINT64_DOCSTRING "Search for the last occurrence of a 64 bit unsigned integer on a memory mapped binary file containing adjacent blocks of sorted binary data. The 64 bit values in the file must encoded in big-endian format and sorted in ascending order."
#define PYFINDFIRSTSUBUINT8_DOCSTRING "Search for the first occurrence of a bit set contained in a 8 bit unsigned integer on a memory mapped binary file containing adjacent blocks of sorted binary data. The 8 bit values in the file must encoded in big-endian format and sorted in ascending order."
#define PYFINDFIRSTSUBUINT16_DOCSTRING "Search for the first occurrence of a bit set contained in a 16 bit unsigned integer on a memory mapped binary file containing adjacent blocks of sorted binary data. The 16 bit values in the file must encoded in big-endian format and sorted in ascending order."
#define PYFINDFIRSTSUBUINT32_DOCSTRING "Search for the first occurrence of a bit set contained in a 32 bit unsigned integer on a memory mapped binary file containing adjacent blocks of sorted binary data. The 32 bit values in the file must encoded in big-endian format and sorted in ascending order."
#define PYFINDFIRSTSUBUINT64_DOCSTRING "Search for the first occurrence of a bit set contained in a 64 bit unsigned integer on a memory mapped binary file containing adjacent blocks of sorted binary data. The 64 bit values in the file must encoded in big-endian format and sorted in ascending order."
#define PYFINDLASTSUBUINT8_DOCSTRING "Search for the last occurrence of a bit set contained in a 8 bit unsigned integer on a memory mapped binary file containing adjacent blocks of sorted binary data. The 8 bit values in the file must encoded in big-endian format and sorted in ascending order."
#define PYFINDLASTSUBUINT16_DOCSTRING "Search for the last occurrence of a bit set contained in a 16 bit unsigned integer on a memory mapped binary file containing adjacent blocks of sorted binary data. The 16 bit values in the file must encoded in big-endian format and sorted in ascending order."
#define PYFINDLASTSUBUINT32_DOCSTRING "Search for the last occurrence of a bit set contained in a 32 bit unsigned integer on a memory mapped binary file containing adjacent blocks of sorted binary data. The 32 bit values in the file must encoded in big-endian format and sorted in ascending order."
#define PYFINDLASTSUBUINT64_DOCSTRING "Search for the last occurrence of a bit set contained in a 64 bit unsigned integer on a memory mapped binary file containing adjacent blocks of sorted binary data. The 64 bit values in the file must encoded in big-endian format and sorted in ascending order."
#define PYHASNEXTUINT8_DOCSTRING "Check if the next occurrence of an unsigned integer on a memory mapped binary file containing adjacent blocks of sorted binary data still matches the search value."
#define PYHASNEXTUINT16_DOCSTRING "Check if the next occurrence of an unsigned integer on a memory mapped binary file containing adjacent blocks of sorted binary data still matches the search value."
#define PYHASNEXTUINT32_DOCSTRING "Check if the next occurrence of an unsigned integer on a memory mapped binary file containing adjacent blocks of sorted binary data still matches the search value."
#define PYHASNEXTUINT64_DOCSTRING "Check if the next occurrence of an unsigned integer on a memory mapped binary file containing adjacent blocks of sorted binary data still matches the search value."
#define PYHASNEXTSUBUINT8_DOCSTRING "Check if the next occurrence of an unsigned integer on a memory mapped binary file containing adjacent blocks of sorted binary data still matches the search value."
#define PYHASNEXTSUBUINT16_DOCSTRING "Check if the next occurrence of an unsigned integer on a memory mapped binary file containing adjacent blocks of sorted binary data still matches the search value."
#define PYHASNEXTSUBUINT32_DOCSTRING "Check if the next occurrence of an unsigned integer on a memory mapped binary file containing adjacent blocks of sorted binary data still matches the search value."
#define PYHASNEXTSUBUINT64_DOCSTRING "Check if the next occurrence of an unsigned integer on a memory mapped binary file containing adjacent blocks of sorted binary data still matches the search value."
#define PYHASPREVUINT8_DOCSTRING "Check if the previous occurrence of an unsigned integer on a memory mapped binary file containing adjacent blocks of sorted binary data still matches the search value."
#define PYHASPREVUINT16_DOCSTRING "Check if the previous occurrence of an unsigned integer on a memory mapped binary file containing adjacent blocks of sorted binary data still matches the search value."
#define PYHASPREVUINT32_DOCSTRING "Check if the previous occurrence of an unsigned integer on a memory mapped binary file containing adjacent blocks of sorted binary data still matches the search value."
#define PYHASPREVUINT64_DOCSTRING "Check if the previous occurrence of an unsigned integer on a memory mapped binary file containing adjacent blocks of sorted binary data still matches the search value."
#define PYHASPREVSUBUINT8_DOCSTRING "Check if the previous occurrence of an unsigned integer on a memory mapped binary file containing adjacent blocks of sorted binary data still matches the search value."
#define PYHASPREVSUBUINT16_DOCSTRING "Check if the previous occurrence of an unsigned integer on a memory mapped binary file containing adjacent blocks of sorted binary data still matches the search value."
#define PYHASPREVSUBUINT32_DOCSTRING "Check if the previous occurrence of an unsigned integer on a memory mapped binary file containing adjacent blocks of sorted binary data still matches the search value."
#define PYHASPREVSUBUINT64_DOCSTRING "Check if the previous occurrence of an unsigned integer on a memory mapped binary file containing adjacent blocks of sorted binary data still matches the search value."

// RSIDVAR
#define PYGETVRRSID_DOCSTRING "Returns the rsID at the specified position in the VR file."
#define PYGETRVVARIANTKEY_DOCSTRING "Returns the VariantKey at the specified position in the RV file."
#define PYFINDRVVARIANTKEYBYRSID_DOCSTRING "Search for the specified rsID and returns the first occurrence of VariantKey in the RV file."
#define PYGETNEXTRVVARIANTKEYBYRSID_DOCSTRING "Get the next VariantKey for the specified rsID in the RV file. This function should be used after find_rv_variantkey_by_rsid. Itcan be called in a loop to get all VariantKeys that are associated with the same rsID (if any)."
#define PYFINDALLRVVARIANTKEYBYRSID_DOCSTRING "Search for the specified rsID and returns all associated VariantKeys."
#define PYFINDVRRSIDBYVARIANTKEY_DOCSTRING "Search for the specified VariantKey and returns the first occurrence of rsID in the VR file."
#define PYFINDVRCHROMPOSRANGE_DOCSTRING "Search for the specified CHROM-POS range and returns the first occurrence of rsID in the VR file."

// NRVK
#define PYFINDREFALTBYVARIANTKEY_DOCSTRING "Retrieve the REF and ALT strings for the specified VariantKey using a memory-mapped file as lookup table."
#define PYREVERSEVARIANTKEY_DOCSTRING "Reverse a VariantKey code and returns the normalized components."

// GENOREF
#define PYLOADGENOREFINDEX "Return the index object from the genome reference memory mapped file."
#define PYGETGENOREFSEQ "Returns the genome reference nucleotide at the specified chromosome and position."
#define PYCHECKREFERENCE "Check if the reference allele matches the reference genome data."
#define PYFLIPALLELE "Flip the allele nucleotides. The resulting string is always in uppercase. Support extended nucleotide letters."
#define PYNORMALIZEVARIANT "Normalize a variant. Flip alleles if required and apply the normalization algorithm described at: https://genome.sph.umich.edu/wiki/Variant_Normalization"


#if defined(__SUNPRO_C) || defined(__hpux) || defined(_AIX)
#define inline
#endif

#ifdef __linux
#define inline __inline
#endif
