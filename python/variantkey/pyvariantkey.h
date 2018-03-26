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
static PyObject *py_variantkey(PyObject *self, PyObject *args);
static PyObject *py_variantkey_string(PyObject *self, PyObject *args);
static PyObject *py_parse_variantkey_string(PyObject *self, PyObject *args);
static PyObject *py_decode_variantkey(PyObject *self, PyObject *args);

// BINSEARCH
static PyObject *py_mmap_binfile(PyObject *self, PyObject *args);
static PyObject *py_munmap_binfile(PyObject *self, PyObject *args);
static PyObject *py_get_address(PyObject *self, PyObject *args);
static PyObject *py_find_first_uint32(PyObject *self, PyObject *args);
static PyObject *py_find_last_uint32(PyObject *self, PyObject *args);
static PyObject *py_find_first_uint64(PyObject *self, PyObject *args);
static PyObject *py_find_last_uint64(PyObject *self, PyObject *args);

// RSIDVAR
static PyObject *py_get_vr_rsid(PyObject *self, PyObject *args);
static PyObject *py_get_rv_variantkey(PyObject *self, PyObject *args);
static PyObject *py_find_rv_variantkey_by_rsid(PyObject *self, PyObject *args);
static PyObject *py_find_vr_rsid_by_variantkey(PyObject *self, PyObject *args);
static PyObject *py_find_vr_chrompos_range(PyObject *self, PyObject *args);

PyMODINIT_FUNC initvariantkey(void);

// VARIANTKEY
#define PYENCODECHROM_DOCSTRING "Returns chromosome encoding."
#define PYDECODECHROM_DOCSTRING "Decode the CHROM code."
#define PYENCODEREFALT_DOCSTRING "Returns reference+alternate encoding."
#define PYDECODEREFALT_DOCSTRING "Decode the 32 bit REF+ALT code if reversible (if it has less than 5 nucleotides in total)."
#define PYVARIANTKEY_DOCSTRING "Returns a 64 bit variant key based on CHROM, POS (0-base), REF, ALT."
#define PYVARIANTKEYSTRING_DOCSTRING "Returns VariantKey hexadecimal string (16 characters)."
#define PYPARSEVARIANTKEYSTRING_DOCSTRING "Parses a VariantKey hex string and returns the code."
#define PYDECODEVARIANTKEY_DOCSTRING "Decode a VariantKey code and returns the components as variantkey_t structure."

// BINSEARCH
#define PYMMAPBINFILE_DOCSTRING "Memory map the specified file."
#define PYMUNMAPBINFILE_DOCSTRING "Unmap and close the memory-mapped file."
#define PYGETADDRESS_DOCSTRING "Returns the absolute file start position of the specified item (binary block)."
#define PYFINDFIRSTUINT32_DOCSTRING "Search for the first occurrence of a 32 bit unsigned integer on a memory mapped binary file containing adjacent blocks of sorted binary data. The 32 bit values in the file must encoded in big-endian format and sorted in ascending order."
#define PYFINDLASTUINT32_DOCSTRING "Search for the last occurrence of a 32 bit unsigned integer on a memory mapped binary file containing adjacent blocks of sorted binary data. The 32 bit values in the file must encoded in big-endian format and sorted in ascending order."
#define PYFINDFIRSTUINT64_DOCSTRING "Search for the first occurrence of a 64 bit unsigned integer on a memory mapped binary file containing adjacent blocks of sorted binary data. The 64 bit values in the file must encoded in big-endian format and sorted in ascending order."
#define PYFINDLASTUINT64_DOCSTRING "Search for the last occurrence of a 64 bit unsigned integer on a memory mapped binary file containing adjacent blocks of sorted binary data. The 64 bit values in the file must encoded in big-endian format and sorted in ascending order."

// RSIDVAR
#define PYGETVRRSID_DOCSTRING "Returns the RSID at the specified position in the VR file."
#define PYGETRVVARIANTKEY_DOCSTRING "Returns the VariantKey at the specified position in the RV file."
#define PYFINDRVVARIANTKEYBYRSID_DOCSTRING "Search for the specified RSID and returns the first occurrence of VariantKey in the RV file."
#define PYFINDVRRSIDBYVARIANTKEY_DOCSTRING "Search for the specified VariantKey and returns the first occurrence of RSID in the VR file."
#define PYFINDVRCHROMPOSRANGE_DOCSTRING "Search for the specified CHROM-POS range and returns the first occurrence of RSID in the VR file."


#if defined(__SUNPRO_C) || defined(__hpux) || defined(_AIX)
#define inline
#endif

#ifdef __linux
#define inline __inline
#endif
