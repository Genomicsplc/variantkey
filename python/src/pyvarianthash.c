// Python VariantHash Module
//
// @category   Libraries
// @author     Nicola Asuni <nicola.asuni@genomicsplc.com>
// @license    MIT (see LICENSE)
// @link       https://github.com/genomicsplc/varianthash

#include <Python.h>
#include "farmhash64.h"
#include "varianthash.h"
#include "binsearch.h"
#include "rsidvar.h"
#include "pyvarianthash.h"

#ifndef Py_UNUSED /* This is already defined for Python 3.4 onwards */
#ifdef __GNUC__
#define Py_UNUSED(name) _unused_ ## name __attribute__((unused))
#else
#define Py_UNUSED(name) _unused_ ## name
#endif
#endif

static PyObject* py_encode_chrom(PyObject *Py_UNUSED(ignored), PyObject *args)
{
    PyObject *result;
    const char *chrom;
    if (!PyArg_ParseTuple(args, "s", &chrom))
        return NULL;
    uint32_t h = encode_chrom(chrom);
    result = Py_BuildValue("I", h);
    return result;
}

static PyObject* py_encode_ref_alt(PyObject *Py_UNUSED(ignored), PyObject *args)
{
    PyObject *result;
    const char *ref, *alt;
    if (!PyArg_ParseTuple(args, "ss", &ref, &alt))
        return NULL;
    uint64_t h = encode_ref_alt(ref, alt);
    result = Py_BuildValue("K", h);
    return result;
}

static PyObject* py_variant_hash(PyObject *Py_UNUSED(ignored), PyObject *args)
{
    PyObject *result;
    const char *chrom, *ref, *alt;
    uint32_t pos;
    if (!PyArg_ParseTuple(args, "sIss", &chrom, &pos, &ref, &alt))
        return NULL;
    varhash_t h = variant_hash(chrom, pos, ref, alt);
    result = PyTuple_New(3);
    PyTuple_SetItem(result, 0, Py_BuildValue("I", h.chrom));
    PyTuple_SetItem(result, 1, Py_BuildValue("I", h.pos));
    PyTuple_SetItem(result, 2, Py_BuildValue("K", h.refalt));
    return result;
}

static PyObject* py_variant_hash_string(PyObject *Py_UNUSED(ignored), PyObject *args)
{
    PyObject *result;
    uint32_t chrom, pos;
    uint64_t refalt;
    Py_ssize_t slen = 0;
    if (!PyArg_ParseTuple(args, "IIK", &chrom, &pos, &refalt))
        return NULL;
    varhash_t h = {chrom, pos, refalt};
    slen = variant_hash_string("", slen, h);
    char str[slen];
    variant_hash_string(str, slen, h);
    result = PyBytes_FromString(str);
    return result;
}

static PyObject* py_decode_variant_hash_string(PyObject *Py_UNUSED(ignored), PyObject *args)
{
    PyObject *result;
    const char *vs;
    if (!PyArg_ParseTuple(args, "s#", &vs))
        return NULL;
    varhash_t h = decode_variant_hash_string(vs);
    result = PyTuple_New(3);
    PyTuple_SetItem(result, 0, Py_BuildValue("I", h.chrom));
    PyTuple_SetItem(result, 1, Py_BuildValue("I", h.pos));
    PyTuple_SetItem(result, 2, Py_BuildValue("K", h.refalt));
    return result;
}

static PyObject* py_farmhash64(PyObject *Py_UNUSED(ignored), PyObject *args)
{
    PyObject *result;
    const char *s;
    Py_ssize_t len;
    if (!PyArg_ParseTuple(args, "s#", &s))
        return NULL;
    len = strlen(s);
    uint64_t h = farmhash64(s, len);
    result = Py_BuildValue("K", h);
    return result;
}

static PyObject* py_mmap_binfile(PyObject *Py_UNUSED(ignored), PyObject *args)
{
    PyObject *result;
    const char *file;
    if (!PyArg_ParseTuple(args, "s", &file))
        return NULL;
    mmfile_t h = mmap_binfile(file);
    result = PyTuple_New(3);
    PyTuple_SetItem(result, 0, PyCapsule_New((void*)h.src, "src", NULL));
    PyTuple_SetItem(result, 1, Py_BuildValue("i", h.fd));
    PyTuple_SetItem(result, 2, Py_BuildValue("K", h.size));
    return result;
}

static PyObject* py_munmap_binfile(PyObject *Py_UNUSED(ignored), PyObject *args)
{
    mmfile_t mf;
    PyObject *result;
    PyObject* mfsrc = NULL;
    if (!PyArg_ParseTuple(args, "OiK", &mfsrc, &mf.fd, &mf.size))
        return NULL;
    mf.src = (unsigned char *)PyCapsule_GetPointer(mfsrc, "src");
    int h = munmap_binfile(mf);
    result = Py_BuildValue("i", h);
    return result;
}

static PyObject* py_get_address(PyObject *Py_UNUSED(ignored), PyObject *args)
{
    PyObject *result;
    uint64_t blklen, blkpos, item;
    if (!PyArg_ParseTuple(args, "KKK", &blklen, &blkpos, &item))
        return NULL;
    uint64_t h = get_address(blklen, blkpos, item);
    result = Py_BuildValue("K", h);
    return result;
}

static PyObject* py_find_first_uint32be(PyObject *Py_UNUSED(ignored), PyObject *args)
{
    PyObject *result;
    uint64_t blklen, blkpos, first, last;
    uint32_t search;
    PyObject* mfsrc = NULL;
    if (!PyArg_ParseTuple(args, "OKKKKI", &mfsrc, &blklen, &blkpos, &first, &last, &search))
        return NULL;
    const unsigned char *src = (const unsigned char *)PyCapsule_GetPointer(mfsrc, "src");
    uint64_t h = find_first_uint32be(src, blklen, blkpos, first, last, search);
    result = Py_BuildValue("K", h);
    return result;
}

static PyObject* py_find_last_uint32be(PyObject *Py_UNUSED(ignored), PyObject *args)
{
    PyObject *result;
    uint64_t blklen, blkpos, first, last;
    uint32_t search;
    PyObject* mfsrc = NULL;
    if (!PyArg_ParseTuple(args, "OKKKKI", &mfsrc, &blklen, &blkpos, &first, &last, &search))
        return NULL;
    const unsigned char *src = (const unsigned char *)PyCapsule_GetPointer(mfsrc, "src");
    uint64_t h = find_last_uint32be(src, blklen, blkpos, first, last, search);
    result = Py_BuildValue("K", h);
    return result;
}

static PyObject* py_find_first_uint64be(PyObject *Py_UNUSED(ignored), PyObject *args)
{
    PyObject *result;
    uint64_t blklen, blkpos, first, last;
    uint64_t search;
    PyObject* mfsrc = NULL;
    if (!PyArg_ParseTuple(args, "OKKKKK", &mfsrc, &blklen, &blkpos, &first, &last, &search))
        return NULL;
    const unsigned char *src = (const unsigned char *)PyCapsule_GetPointer(mfsrc, "src");
    uint64_t h = find_first_uint64be(src, blklen, blkpos, first, last, search);
    result = Py_BuildValue("K", h);
    return result;
}

static PyObject* py_find_last_uint64be(PyObject *Py_UNUSED(ignored), PyObject *args)
{
    PyObject *result;
    uint64_t blklen, blkpos, first, last;
    uint64_t search;
    PyObject* mfsrc = NULL;
    if (!PyArg_ParseTuple(args, "OKKKKK", &mfsrc, &blklen, &blkpos, &first, &last, &search))
        return NULL;
    const unsigned char *src = (const unsigned char *)PyCapsule_GetPointer(mfsrc, "src");
    uint64_t h = find_last_uint64be(src, blklen, blkpos, first, last, search);
    result = Py_BuildValue("K", h);
    return result;
}

static PyObject* py_find_first_uint128be(PyObject *Py_UNUSED(ignored), PyObject *args)
{
    PyObject *result;
    uint64_t blklen, blkpos, first, last;
    uint64_t search_hi, search_lo;
    PyObject* mfsrc = NULL;
    if (!PyArg_ParseTuple(args, "OKKKKKK", &mfsrc, &blklen, &blkpos, &first, &last, &search_hi, &search_lo))
        return NULL;
    const unsigned char *src = (const unsigned char *)PyCapsule_GetPointer(mfsrc, "src");
    uint64_t h = find_first_uint128be(src, blklen, blkpos, first, last, search_hi, search_lo);
    result = Py_BuildValue("K", h);
    return result;
}

static PyObject* py_find_last_uint128be(PyObject *Py_UNUSED(ignored), PyObject *args)
{
    PyObject *result;
    uint64_t blklen, blkpos, first, last;
    uint64_t search_hi, search_lo;
    PyObject* mfsrc = NULL;
    if (!PyArg_ParseTuple(args, "OKKKKKK", &mfsrc, &blklen, &blkpos, &first, &last, &search_hi, &search_lo))
        return NULL;
    const unsigned char *src = (const unsigned char *)PyCapsule_GetPointer(mfsrc, "src");
    uint64_t h = find_last_uint128be(src, blklen, blkpos, first, last, search_hi, search_lo);
    result = Py_BuildValue("K", h);
    return result;
}

static PyMethodDef PyVariantHashMethods[] =
{
    {"encode_chrom", py_encode_chrom, METH_VARARGS, PYENCODECHROM_DOCSTRING},
    {"encode_ref_alt", py_encode_ref_alt, METH_VARARGS, PYENCODEREFALT_DOCSTRING},
    {"variant_hash", py_variant_hash, METH_VARARGS, PYVARIANTHASH_DOCSTRING},
    {"variant_hash_string", py_variant_hash_string, METH_VARARGS, PYVARIANTHASHSTRING_DOCSTRING},
    {"decode_variant_hash_string", py_decode_variant_hash_string, METH_VARARGS, PYDECODEVARIANTHASHSTRING_DOCSTRING},
    {"farmhash64", py_farmhash64, METH_VARARGS, PYFARMHASH64_DOCSTRING},
    {"mmap_binfile", py_mmap_binfile, METH_VARARGS, PYMMAPBINFILE_DOCSTRING},
    {"munmap_binfile", py_munmap_binfile, METH_VARARGS, PYMUNMAPBINFILE_DOCSTRING},
    {"get_address", py_get_address, METH_VARARGS, PYGETADDRESS_DOCSTRING},
    {"find_first_uint32be", py_find_first_uint32be, METH_VARARGS, PYFINDFIRSTUINT32BE_DOCSTRING},
    {"find_last_uint32be", py_find_last_uint32be, METH_VARARGS, PYFINDLASTUINT32BE_DOCSTRING},
    {"find_first_uint64be", py_find_first_uint64be, METH_VARARGS, PYFINDFIRSTUINT64BE_DOCSTRING},
    {"find_last_uint64be", py_find_last_uint64be, METH_VARARGS, PYFINDLASTUINT64BE_DOCSTRING},
    {"find_first_uint128be", py_find_first_uint128be, METH_VARARGS, PYFINDFIRSTUINT128BE_DOCSTRING},
    {"find_last_uint128be", py_find_last_uint128be, METH_VARARGS, PYFINDLASTUINT128BE_DOCSTRING},
    {NULL, NULL, 0, NULL}
};

struct module_state
{
    PyObject *error;
};

#if PY_MAJOR_VERSION >= 3
#define GETSTATE(m) ((struct module_state*)PyModule_GetState(m))
#else
#define GETSTATE(m) (&_state)
static struct module_state _state;
#endif

#if PY_MAJOR_VERSION >= 3

static int myextension_traverse(PyObject *m, visitproc visit, void *arg)
{
    Py_VISIT(GETSTATE(m)->error);
    return 0;
}

static int myextension_clear(PyObject *m)
{
    Py_CLEAR(GETSTATE(m)->error);
    return 0;
}

static struct PyModuleDef moduledef =
{
    PyModuleDef_HEAD_INIT,
    "libpyvarianthash",
    NULL,
    sizeof(struct module_state),
    PyVariantHashMethods,
    NULL,
    myextension_traverse,
    myextension_clear,
    NULL
};

#define INITERROR return NULL

PyObject* PyInit_libpyvarianthash(void)

#else
#define INITERROR return

void
initlibpyvarianthash(void)
#endif
{
#if PY_MAJOR_VERSION >= 3
    PyObject *module = PyModule_Create(&moduledef);
#else
    PyObject *module = Py_InitModule("libpyvarianthash", PyVariantHashMethods);
#endif
    struct module_state *st = NULL;

    if (module == NULL)
        INITERROR;
    st = GETSTATE(module);

    st->error = PyErr_NewException("libpyvarianthash.Error", NULL, NULL);
    if (st->error == NULL)
    {
        Py_DECREF(module);
        INITERROR;
    }

#if PY_MAJOR_VERSION >= 3
    return module;
#endif
}
