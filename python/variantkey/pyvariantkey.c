// Python VariantKey Module
//
// @category   Libraries
// @author     Nicola Asuni <nicola.asuni@genomicsplc.com>
// @license    MIT (see LICENSE)
// @link       https://github.com/genomicsplc/variantkey

#include <Python.h>
#include "variantkey.h"
#include "binsearch.h"
#include "rsidvar.h"
#include "pyvariantkey.h"

#ifndef Py_UNUSED /* This is already defined for Python 3.4 onwards */
#ifdef __GNUC__
#define Py_UNUSED(name) _unused_ ## name __attribute__((unused))
#else
#define Py_UNUSED(name) _unused_ ## name
#endif
#endif

static PyObject* py_encode_chrom(PyObject *Py_UNUSED(ignored), PyObject *args)
{
    const char *chrom;
    if (!PyArg_ParseTuple(args, "s", &chrom))
        return NULL;
    uint8_t h = encode_chrom(chrom, strlen(chrom));
    return Py_BuildValue("B", h);
}

static PyObject* py_decode_chrom(PyObject *Py_UNUSED(ignored), PyObject *args)
{
    uint8_t code;
    if (!PyArg_ParseTuple(args, "B", &code))
        return NULL;
    char chrom[3] = "";
    decode_chrom(code, chrom);
    return Py_BuildValue("s", chrom);
}

static PyObject* py_encode_refalt(PyObject *Py_UNUSED(ignored), PyObject *args)
{
    const char *ref, *alt;
    if (!PyArg_ParseTuple(args, "ss", &ref, &alt))
        return NULL;
    uint32_t h = encode_refalt(ref, strlen(ref), alt, strlen(alt));
    return Py_BuildValue("I", h);
}

static PyObject* py_decode_refalt(PyObject *Py_UNUSED(ignored), PyObject *args)
{
    PyObject *result;
    uint32_t code;
    if (!PyArg_ParseTuple(args, "I", &code))
        return NULL;
    char ref[6] = "";
    char alt[6] = "";
    size_t sizeref = 0;
    size_t sizealt = 0;
    decode_refalt(code, ref, &sizeref, alt, &sizealt);
    result = PyTuple_New(4);
    PyTuple_SetItem(result, 0, Py_BuildValue("s", ref));
    PyTuple_SetItem(result, 1, Py_BuildValue("s", alt));
    PyTuple_SetItem(result, 2, Py_BuildValue("K", sizeref));
    PyTuple_SetItem(result, 3, Py_BuildValue("K", sizealt));
    return result;
}

static PyObject* py_variantkey(PyObject *Py_UNUSED(ignored), PyObject *args)
{
    const char *chrom, *ref, *alt;
    uint32_t pos;
    if (!PyArg_ParseTuple(args, "sIss", &chrom, &pos, &ref, &alt))
        return NULL;
    uint64_t h = variantkey(chrom, strlen(chrom), pos, ref, strlen(ref), alt, strlen(alt));
    return Py_BuildValue("K", h);
}

static PyObject* py_variantkey_string(PyObject *Py_UNUSED(ignored), PyObject *args)
{
    uint64_t code;
    if (!PyArg_ParseTuple(args, "K", &code))
        return NULL;
    char str[17];
    variantkey_string(code, str);
    return PyBytes_FromString(str);
}

static PyObject* py_parse_variantkey_string(PyObject *Py_UNUSED(ignored), PyObject *args)
{
    const char *vs;
    if (!PyArg_ParseTuple(args, "s#", &vs))
        return NULL;
    uint64_t h = parse_variantkey_string(vs);
    return Py_BuildValue("K", h);
}

static PyObject* py_decode_variantkey(PyObject *Py_UNUSED(ignored), PyObject *args)
{
    PyObject *result;
    uint64_t code;
    if (!PyArg_ParseTuple(args, "K", &code))
        return NULL;
    variantkey_t h = decode_variantkey(code);
    result = PyTuple_New(3);
    PyTuple_SetItem(result, 0, Py_BuildValue("B", h.chrom));
    PyTuple_SetItem(result, 1, Py_BuildValue("I", h.pos));
    PyTuple_SetItem(result, 2, Py_BuildValue("I", h.refalt));
    return result;
}

// --- BINSEARCH ---

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

static PyObject* py_find_first_uint32(PyObject *Py_UNUSED(ignored), PyObject *args)
{
    PyObject *result;
    uint64_t blklen, blkpos, first, last;
    uint8_t bitstart, bitend;
    uint32_t search;
    PyObject* mfsrc = NULL;
    if (!PyArg_ParseTuple(args, "OKKBBKKI", &mfsrc, &blklen, &blkpos, &bitstart, &bitend, &first, &last, &search))
        return NULL;
    const unsigned char *src = (const unsigned char *)PyCapsule_GetPointer(mfsrc, "src");
    uint64_t h = find_first_uint32_t(src, blklen, blkpos, bitstart, bitend, &first, &last, search);
    result = PyTuple_New(3);
    PyTuple_SetItem(result, 0, Py_BuildValue("K", h));
    PyTuple_SetItem(result, 1, Py_BuildValue("K", first));
    PyTuple_SetItem(result, 2, Py_BuildValue("K", last));
    return result;
}

static PyObject* py_find_last_uint32(PyObject *Py_UNUSED(ignored), PyObject *args)
{
    PyObject *result;
    uint64_t blklen, blkpos, first, last;
    uint8_t bitstart, bitend;
    uint32_t search;
    PyObject* mfsrc = NULL;
    if (!PyArg_ParseTuple(args, "OKKBBKKI", &mfsrc, &blklen, &blkpos, &bitstart, &bitend, &first, &last, &search))
        return NULL;
    const unsigned char *src = (const unsigned char *)PyCapsule_GetPointer(mfsrc, "src");
    uint64_t h = find_last_uint32_t(src, blklen, blkpos, bitstart, bitend, &first, &last, search);
    result = PyTuple_New(3);
    PyTuple_SetItem(result, 0, Py_BuildValue("K", h));
    PyTuple_SetItem(result, 1, Py_BuildValue("K", first));
    PyTuple_SetItem(result, 2, Py_BuildValue("K", last));
    return result;
}

static PyObject* py_find_first_uint64(PyObject *Py_UNUSED(ignored), PyObject *args)
{
    PyObject *result;
    uint64_t blklen, blkpos, first, last;
    uint8_t bitstart, bitend;
    uint64_t search;
    PyObject* mfsrc = NULL;
    if (!PyArg_ParseTuple(args, "OKKBBKKK", &mfsrc, &blklen, &blkpos, &bitstart, &bitend, &first, &last, &search))
        return NULL;
    const unsigned char *src = (const unsigned char *)PyCapsule_GetPointer(mfsrc, "src");
    uint64_t h = find_first_uint64_t(src, blklen, blkpos, bitstart, bitend, &first, &last, search);
    result = PyTuple_New(3);
    PyTuple_SetItem(result, 0, Py_BuildValue("K", h));
    PyTuple_SetItem(result, 1, Py_BuildValue("K", first));
    PyTuple_SetItem(result, 2, Py_BuildValue("K", last));
    return result;
}

static PyObject* py_find_last_uint64(PyObject *Py_UNUSED(ignored), PyObject *args)
{
    PyObject *result;
    uint64_t blklen, blkpos, first, last;
    uint8_t bitstart, bitend;
    uint64_t search;
    PyObject* mfsrc = NULL;
    if (!PyArg_ParseTuple(args, "OKKBBKKK", &mfsrc, &blklen, &blkpos, &bitstart, &bitend, &first, &last, &search))
        return NULL;
    const unsigned char *src = (const unsigned char *)PyCapsule_GetPointer(mfsrc, "src");
    uint64_t h = find_last_uint64_t(src, blklen, blkpos, bitstart, bitend, &first, &last, search);
    result = PyTuple_New(3);
    PyTuple_SetItem(result, 0, Py_BuildValue("K", h));
    PyTuple_SetItem(result, 1, Py_BuildValue("K", first));
    PyTuple_SetItem(result, 2, Py_BuildValue("K", last));
    return result;
}

// --- RSIDVAR ---

static PyObject* py_get_vr_rsid(PyObject *Py_UNUSED(ignored), PyObject *args)
{
    uint64_t item;
    PyObject* mfsrc = NULL;
    if (!PyArg_ParseTuple(args, "OK", &mfsrc, &item))
        return NULL;
    const unsigned char *src = (const unsigned char *)PyCapsule_GetPointer(mfsrc, "src");
    uint32_t h = get_vr_rsid(src, item);
    return Py_BuildValue("I", h);
}

static PyObject* py_get_rv_variantkey(PyObject *Py_UNUSED(ignored), PyObject *args)
{
    uint64_t item;
    PyObject* mfsrc = NULL;
    if (!PyArg_ParseTuple(args, "OK", &mfsrc, &item))
        return NULL;
    const unsigned char *src = (const unsigned char *)PyCapsule_GetPointer(mfsrc, "src");
    uint64_t h = get_rv_variantkey(src, item);
    return Py_BuildValue("K", h);
}

static PyObject* py_find_rv_variantkey_by_rsid(PyObject *Py_UNUSED(ignored), PyObject *args)
{
    PyObject *result;
    uint64_t first, last;
    uint32_t rsid;
    PyObject* mfsrc = NULL;
    if (!PyArg_ParseTuple(args, "OKKI", &mfsrc, &first, &last, &rsid))
        return NULL;
    const unsigned char *src = (const unsigned char *)PyCapsule_GetPointer(mfsrc, "src");
    uint64_t h = find_rv_variantkey_by_rsid(src, &first, last, rsid);
    result = PyTuple_New(2);
    PyTuple_SetItem(result, 0, Py_BuildValue("K", h));
    PyTuple_SetItem(result, 1, Py_BuildValue("K", first));
    return result;
}

static PyObject* py_find_vr_rsid_by_variantkey(PyObject *Py_UNUSED(ignored), PyObject *args)
{
    PyObject *result;
    uint64_t first, last, vh;
    PyObject* mfsrc = NULL;
    if (!PyArg_ParseTuple(args, "OKKK", &mfsrc, &first, &last, &vh))
        return NULL;
    const unsigned char *src = (const unsigned char *)PyCapsule_GetPointer(mfsrc, "src");
    uint32_t h = find_vr_rsid_by_variantkey(src, &first, last, vh);
    result = PyTuple_New(2);
    PyTuple_SetItem(result, 0, Py_BuildValue("I", h));
    PyTuple_SetItem(result, 1, Py_BuildValue("K", first));
    return result;
}

static PyObject* py_find_vr_chrompos_range(PyObject *Py_UNUSED(ignored), PyObject *args)
{
    PyObject *result;
    uint64_t first, last;
    uint8_t chrom;
    uint32_t pos_start, pos_end;
    PyObject* mfsrc = NULL;
    if (!PyArg_ParseTuple(args, "OKKIII", &mfsrc, &first, &last, &chrom, &pos_start, &pos_end))
        return NULL;
    const unsigned char *src = (const unsigned char *)PyCapsule_GetPointer(mfsrc, "src");
    uint32_t h = find_vr_chrompos_range(src, &first, &last, chrom, pos_start, pos_end);
    result = PyTuple_New(3);
    PyTuple_SetItem(result, 0, Py_BuildValue("I", h));
    PyTuple_SetItem(result, 1, Py_BuildValue("K", first));
    PyTuple_SetItem(result, 2, Py_BuildValue("K", last));
    return result;
}

// ---

static PyMethodDef PyVariantKeyMethods[] =
{
    // VARIANTKEY
    {"encode_chrom", py_encode_chrom, METH_VARARGS, PYENCODECHROM_DOCSTRING},
    {"decode_chrom", py_decode_chrom, METH_VARARGS, PYDECODECHROM_DOCSTRING},
    {"encode_refalt", py_encode_refalt, METH_VARARGS, PYENCODEREFALT_DOCSTRING},
    {"decode_refalt", py_decode_refalt, METH_VARARGS, PYDECODEREFALT_DOCSTRING},
    {"variantkey", py_variantkey, METH_VARARGS, PYVARIANTKEY_DOCSTRING},
    {"variantkey_string", py_variantkey_string, METH_VARARGS, PYVARIANTKEYSTRING_DOCSTRING},
    {"parse_variantkey_string", py_parse_variantkey_string, METH_VARARGS, PYPARSEVARIANTKEYSTRING_DOCSTRING},
    {"decode_variantkey", py_decode_variantkey, METH_VARARGS, PYDECODEVARIANTKEY_DOCSTRING},

    // BINSEARCH
    {"mmap_binfile", py_mmap_binfile, METH_VARARGS, PYMMAPBINFILE_DOCSTRING},
    {"munmap_binfile", py_munmap_binfile, METH_VARARGS, PYMUNMAPBINFILE_DOCSTRING},
    {"get_address", py_get_address, METH_VARARGS, PYGETADDRESS_DOCSTRING},
    {"find_first_uint32", py_find_first_uint32, METH_VARARGS, PYFINDFIRSTUINT32_DOCSTRING},
    {"find_last_uint32", py_find_last_uint32, METH_VARARGS, PYFINDLASTUINT32_DOCSTRING},
    {"find_first_uint64", py_find_first_uint64, METH_VARARGS, PYFINDFIRSTUINT64_DOCSTRING},
    {"find_last_uint64", py_find_last_uint64, METH_VARARGS, PYFINDLASTUINT64_DOCSTRING},

    // RSIDVAR
    {"get_vr_rsid", py_get_vr_rsid, METH_VARARGS, PYGETVRRSID_DOCSTRING},
    {"get_rv_variantkey", py_get_rv_variantkey, METH_VARARGS, PYGETRVVARIANTKEY_DOCSTRING},
    {"find_rv_variantkey_by_rsid", py_find_rv_variantkey_by_rsid, METH_VARARGS, PYFINDRVVARIANTKEYBYRSID_DOCSTRING},
    {"find_vr_rsid_by_variantkey", py_find_vr_rsid_by_variantkey, METH_VARARGS, PYFINDVRRSIDBYVARIANTKEY_DOCSTRING},
    {"find_vr_chrompos_range", py_find_vr_chrompos_range, METH_VARARGS, PYFINDVRCHROMPOSRANGE_DOCSTRING},

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
    "variantkey",
    NULL,
    sizeof(struct module_state),
    PyVariantKeyMethods,
    NULL,
    myextension_traverse,
    myextension_clear,
    NULL
};

#define INITERROR return NULL

PyObject* PyInit_variantkey(void)

#else
#define INITERROR return

void
initvariantkey(void)
#endif
{
#if PY_MAJOR_VERSION >= 3
    PyObject *module = PyModule_Create(&moduledef);
#else
    PyObject *module = Py_InitModule("variantkey", PyVariantKeyMethods);
#endif
    struct module_state *st = NULL;

    if (module == NULL)
        INITERROR;
    st = GETSTATE(module);

    st->error = PyErr_NewException("variantkey.Error", NULL, NULL);
    if (st->error == NULL)
    {
        Py_DECREF(module);
        INITERROR;
    }

#if PY_MAJOR_VERSION >= 3
    return module;
#endif
}
