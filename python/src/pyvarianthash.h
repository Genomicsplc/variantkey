// Python varianthash Module
//
// @category   Libraries
// @author     Nicola Asuni <nicola.asuni@genomicsplc.com>
// @license    MIT (see LICENSE)
// @link       https://github.com/genomicsplc/varianthash

#define PY_SSIZE_T_CLEAN
#include "Python.h"

static PyObject *py_encode_chrom(PyObject *self, PyObject *args);
static PyObject *py_encode_ref_alt(PyObject *self, PyObject *args);
static PyObject *py_variant_hash(PyObject *self, PyObject *args);
static PyObject *py_variant_hash_string(PyObject *self, PyObject *args);
static PyObject *py_decode_variant_hash_string(PyObject *self, PyObject *args);
static PyObject *py_farmhash64(PyObject *self, PyObject *args);

PyMODINIT_FUNC initlibpyvarianthash(void);

#define PYENCODECHROM_DOCSTRING "Returns 32-bit chromosome encoding."
#define PYENCODEREFALT_DOCSTRING "Returns 64-bit reference+alternate hash code."
#define PYVARIANTHASH_DOCSTRING "Returns a Genetic Variant Hash based on CHROM, POS (1-base), REF, ALT."
#define PYVARIANTHASHSTRING_DOCSTRING "Returns a human-readable Genetic Variant Hash string (32 hex characters)."
#define PYDECODEVARIANTHASHSTRING_DOCSTRING "Parses a variant hash string and returns the individual components."
#define PYFARMHASH64_DOCSTRING "Returns a 64-bit fingerprint hash for a byte array.\nexample: print libpyfarmhash64.farmhash64('Lorem ipsum dolor sit amet')\n16191328082827683567"

#if defined(__SUNPRO_C) || defined(__hpux) || defined(_AIX)
#define inline
#endif

#ifdef __linux
#define inline __inline
#endif
