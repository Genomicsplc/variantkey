// Python varianthash Module
//
// @category   Libraries
// @author     Nicola Asuni <nicola.asuni@genomicsplc.com>
// @license    MIT (see LICENSE)
// @link       https://github.com/genomicsplc/varianthash

#define PY_SSIZE_T_CLEAN
#include "Python.h"

static PyObject *py_encode_assembly(PyObject *self, PyObject *args);
static PyObject *py_encode_chrom(PyObject *self, PyObject *args);
static PyObject *py_encode_ref_alt(PyObject *self, PyObject *args);
static PyObject *py_variant_hash(PyObject *self, PyObject *args);
static PyObject *py_variant_hash_string(PyObject *self, PyObject *args);
static PyObject *py_decode_variant_hash_string(PyObject *self, PyObject *args);

static PyObject *py_farmhash64(PyObject *self, PyObject *args);
static PyObject *py_farmhash32(PyObject *self, PyObject *args);

static PyObject *py_mmap_binfile(PyObject *self, PyObject *args);
static PyObject *py_munmap_binfile(PyObject *self, PyObject *args);
static PyObject *py_get_address(PyObject *self, PyObject *args);
static PyObject *py_find_first_uint32(PyObject *self, PyObject *args);
static PyObject *py_find_last_uint32(PyObject *self, PyObject *args);
static PyObject *py_find_first_uint64(PyObject *self, PyObject *args);
static PyObject *py_find_last_uint64(PyObject *self, PyObject *args);
static PyObject *py_find_first_uint128(PyObject *self, PyObject *args);
static PyObject *py_find_last_uint128(PyObject *self, PyObject *args);

PyMODINIT_FUNC initlibpyvarianthash(void);

#define PYENCODEASSBLY_DOCSTRING "Returns 32-bit assembly encoding."
#define PYENCODECHROM_DOCSTRING "Returns 32-bit chromosome encoding."
#define PYENCODEREFALT_DOCSTRING "Returns 32-bit reference+alternate hash code."
#define PYVARIANTHASH_DOCSTRING "Returns a Genetic Variant Hash based on CHROM, POS (0-base), REF, ALT."
#define PYVARIANTHASHSTRING_DOCSTRING "Returns a human-readable Genetic Variant Hash string (32 hex characters)."
#define PYDECODEVARIANTHASHSTRING_DOCSTRING "Parses a variant hash string and returns the individual components."

#define PYFARMHASH64_DOCSTRING "Returns a 64-bit fingerprint hash for a byte array.\nexample: print libpyfarmhash64.farmhash64('Lorem ipsum dolor sit amet')\n16191328082827683567"
#define PYFARMHASH32_DOCSTRING "Returns a 32-bit fingerprint hash for a byte array.\nexample: print libpyfarmhash64.farmhash32('Lorem ipsum dolor sit amet')\n2990660358"

#define PYMMAPBINFILE_DOCSTRING "Memory map the specified file."
#define PYMUNMAPBINFILE_DOCSTRING "Unmap and close the memory-mapped file."
#define PYGETADDRESS_DOCSTRING "Returns the absolute file start position of the specified item (binary block)."
#define PYFINDFIRSTUINT32_DOCSTRING "Search for the first occurrence of a 32 bit unsigned integer on a memory mapped binary file containing adjacent blocks of sorted binary data. The 32 bit values in the file must encoded in big-endian format and sorted in ascending order."
#define PYFINDLASTUINT32_DOCSTRING "Search for the last occurrence of a 32 bit unsigned integer on a memory mapped binary file containing adjacent blocks of sorted binary data. The 32 bit values in the file must encoded in big-endian format and sorted in ascending order."
#define PYFINDFIRSTUINT64_DOCSTRING "Search for the first occurrence of a 64 bit unsigned integer on a memory mapped binary file containing adjacent blocks of sorted binary data. The 64 bit values in the file must encoded in big-endian format and sorted in ascending order."
#define PYFINDLASTUINT64_DOCSTRING "Search for the last occurrence of a 64 bit unsigned integer on a memory mapped binary file containing adjacent blocks of sorted binary data. The 64 bit values in the file must encoded in big-endian format and sorted in ascending order."
#define PYFINDFIRSTUINT128_DOCSTRING "Search for the first occurrence of a 128 bit unsigned integer on a memory mapped binary file containing adjacent blocks of sorted binary data. The 128 bit values in the file must encoded in big-endian format and sorted in ascending order."
#define PYFINDLASTUINT128_DOCSTRING "Search for the last occurrence of a 128 bit unsigned integer on a memory mapped binary file containing adjacent blocks of sorted binary data. The 128 bit values in the file must encoded in big-endian format and sorted in ascending order."

#define PYGETVRRSID_DOCSTRING "Returns the RSID at the specified position."
#define PYGETRVVARHASH_DOCSTRING "Returns the VariantHash at the specified position."
#define PYFINDRVVARHASHBYRSID_DOCSTRING "Search for the specified RSID and returns the first occurrence of VariantHash."
#define PYFINDVRRSIDBYVARHASH_DOCSTRING "Search for the specified VariantHash and returns the first occurrence of RSID."
#define PYFINDVRCHROMPOSRANGE_DOCSTRING "Search for the specified CHROM-POS range and returns the first occurrence of RSID."

#if defined(__SUNPRO_C) || defined(__hpux) || defined(_AIX)
#define inline
#endif

#ifdef __linux
#define inline __inline
#endif
