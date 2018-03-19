// Python VariantKey Module
//
// @category   Libraries
// @author     Nicola Asuni <nicola.asuni@genomicsplc.com>
// @license    MIT (see LICENSE)
// @link       https://github.com/genomicsplc/variantkey

#include <Python.h>
#include "farmhash64.h"
#include "variantkey.h"
#include "binsearch.h"
#include "rsidvar128.h"
#include "rsidvar64.h"
#include "pyvariantkey.h"

#ifndef Py_UNUSED /* This is already defined for Python 3.4 onwards */
#ifdef __GNUC__
#define Py_UNUSED(name) _unused_ ## name __attribute__((unused))
#else
#define Py_UNUSED(name) _unused_ ## name
#endif
#endif

static PyObject* py_encode_assembly_32bit(PyObject *Py_UNUSED(ignored), PyObject *args)
{
    const char *assembly;
    if (!PyArg_ParseTuple(args, "s", &assembly))
        return NULL;
    uint32_t h = encode_assembly_32bit(assembly);
    return Py_BuildValue("I", h);
}

static PyObject* py_encode_chrom_32bit(PyObject *Py_UNUSED(ignored), PyObject *args)
{
    const char *chrom;
    if (!PyArg_ParseTuple(args, "s", &chrom))
        return NULL;
    uint32_t h = encode_chrom_32bit(chrom);
    return Py_BuildValue("I", h);
}

static PyObject* py_encode_chrom_8bit(PyObject *Py_UNUSED(ignored), PyObject *args)
{
    const char *chrom;
    if (!PyArg_ParseTuple(args, "s", &chrom))
        return NULL;
    uint8_t h = encode_chrom_8bit(chrom);
    return Py_BuildValue("B", h);
}

static PyObject* py_decode_chrom_32bit(PyObject *Py_UNUSED(ignored), PyObject *args)
{
    uint32_t code;
    if (!PyArg_ParseTuple(args, "I", &code))
        return NULL;
    char chrom[11] = "";
    decode_chrom_32bit(code, chrom);
    return Py_BuildValue("s", chrom);
}

static PyObject* py_decode_chrom_8bit(PyObject *Py_UNUSED(ignored), PyObject *args)
{
    uint8_t code;
    if (!PyArg_ParseTuple(args, "B", &code))
        return NULL;
    char chrom[3] = "";
    decode_chrom_8bit(code, chrom);
    return Py_BuildValue("s", chrom);
}

static PyObject* py_encode_refalt_32bit(PyObject *Py_UNUSED(ignored), PyObject *args)
{
    const char *ref, *alt;
    if (!PyArg_ParseTuple(args, "ss", &ref, &alt))
        return NULL;
    uint32_t h = encode_refalt_32bit(ref, alt);
    return Py_BuildValue("I", h);
}

static PyObject* py_encode_refalt_24bit(PyObject *Py_UNUSED(ignored), PyObject *args)
{
    const char *ref, *alt;
    if (!PyArg_ParseTuple(args, "ss", &ref, &alt))
        return NULL;
    uint32_t h = encode_refalt_24bit(ref, alt);
    return Py_BuildValue("I", h);
}

static PyObject* py_decode_refalt_32bit(PyObject *Py_UNUSED(ignored), PyObject *args)
{
    PyObject *result;
    uint32_t code;
    if (!PyArg_ParseTuple(args, "I", &code))
        return NULL;
    char ref[6] = "";
    char alt[6] = "";
    decode_refalt_32bit(code, ref, alt);
    result = PyTuple_New(2);
    PyTuple_SetItem(result, 0, Py_BuildValue("s", ref));
    PyTuple_SetItem(result, 1, Py_BuildValue("s", alt));
    return result;
}

static PyObject* py_decode_refalt_24bit(PyObject *Py_UNUSED(ignored), PyObject *args)
{
    PyObject *result;
    uint32_t code;
    if (!PyArg_ParseTuple(args, "I", &code))
        return NULL;
    char ref[5] = "";
    char alt[5] = "";
    decode_refalt_24bit(code, ref, alt);
    result = PyTuple_New(2);
    PyTuple_SetItem(result, 0, Py_BuildValue("s", ref));
    PyTuple_SetItem(result, 1, Py_BuildValue("s", alt));
    return result;
}

static PyObject* py_variantkey128(PyObject *Py_UNUSED(ignored), PyObject *args)
{
    PyObject *result;
    const char *assembly, *chrom, *ref, *alt;
    uint32_t pos;
    if (!PyArg_ParseTuple(args, "ssIss", &assembly, &chrom, &pos, &ref, &alt))
        return NULL;
    variantkey128_t h = variantkey128(assembly, chrom, pos, ref, alt);
    result = PyTuple_New(4);
    PyTuple_SetItem(result, 0, Py_BuildValue("I", h.assembly));
    PyTuple_SetItem(result, 1, Py_BuildValue("I", h.chrom));
    PyTuple_SetItem(result, 2, Py_BuildValue("I", h.pos));
    PyTuple_SetItem(result, 3, Py_BuildValue("I", h.refalt));
    return result;
}

static PyObject* py_variantkey64(PyObject *Py_UNUSED(ignored), PyObject *args)
{
    const char *chrom, *ref, *alt;
    uint32_t pos;
    if (!PyArg_ParseTuple(args, "sIss", &chrom, &pos, &ref, &alt))
        return NULL;
    uint64_t h = variantkey64(chrom, pos, ref, alt);
    return Py_BuildValue("K", h);
}

static PyObject* py_variantkey128_string(PyObject *Py_UNUSED(ignored), PyObject *args)
{
    uint32_t assembly, chrom, pos, refalt;
    Py_ssize_t slen = 0;
    if (!PyArg_ParseTuple(args, "IIII", &assembly, &chrom, &pos, &refalt))
        return NULL;
    variantkey128_t h = {assembly, chrom, pos, refalt};
    slen = variantkey128_string("", slen, h);
    char str[slen];
    variantkey128_string(str, slen, h);
    return PyBytes_FromString(str);
}

static PyObject* py_variantkey64_string(PyObject *Py_UNUSED(ignored), PyObject *args)
{
    uint64_t code;
    Py_ssize_t slen = 0;
    if (!PyArg_ParseTuple(args, "K", &code))
        return NULL;
    slen = variantkey64_string("", slen, code);
    char str[slen];
    variantkey64_string(str, slen, code);
    return PyBytes_FromString(str);
}

static PyObject* py_parse_variantkey128_string(PyObject *Py_UNUSED(ignored), PyObject *args)
{
    PyObject *result;
    const char *vs;
    if (!PyArg_ParseTuple(args, "s#", &vs))
        return NULL;
    variantkey128_t h = parse_variantkey128_string(vs);
    result = PyTuple_New(4);
    PyTuple_SetItem(result, 0, Py_BuildValue("I", h.assembly));
    PyTuple_SetItem(result, 1, Py_BuildValue("I", h.chrom));
    PyTuple_SetItem(result, 2, Py_BuildValue("I", h.pos));
    PyTuple_SetItem(result, 3, Py_BuildValue("I", h.refalt));
    return result;
}

static PyObject* py_parse_variantkey64_string(PyObject *Py_UNUSED(ignored), PyObject *args)
{
    const char *vs;
    if (!PyArg_ParseTuple(args, "s#", &vs))
        return NULL;
    uint64_t h = parse_variantkey64_string(vs);
    return Py_BuildValue("K", h);
}

static PyObject* py_split_variantkey64(PyObject *Py_UNUSED(ignored), PyObject *args)
{
    PyObject *result;
    uint64_t code;
    if (!PyArg_ParseTuple(args, "K", &code))
        return NULL;
    variantkey64_t h = split_variantkey64(code);
    result = PyTuple_New(3);
    PyTuple_SetItem(result, 0, Py_BuildValue("B", h.chrom));
    PyTuple_SetItem(result, 1, Py_BuildValue("I", h.pos));
    PyTuple_SetItem(result, 2, Py_BuildValue("I", h.refalt));
    return result;
}

// --- FARMHASH64 ---

static PyObject* py_farmhash64(PyObject *Py_UNUSED(ignored), PyObject *args)
{
    const char *s;
    Py_ssize_t len;
    if (!PyArg_ParseTuple(args, "s#", &s))
        return NULL;
    len = strlen(s);
    uint64_t h = farmhash64(s, len);
    return Py_BuildValue("K", h);
}

static PyObject* py_farmhash32(PyObject *Py_UNUSED(ignored), PyObject *args)
{
    const char *s;
    Py_ssize_t len;
    if (!PyArg_ParseTuple(args, "s#", &s))
        return NULL;
    len = strlen(s);
    uint32_t h = farmhash32(s, len);
    return Py_BuildValue("I", h);
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
    PyObject* mfsrc = NULL;
    if (!PyArg_ParseTuple(args, "OiK", &mfsrc, &mf.fd, &mf.size))
        return NULL;
    mf.src = (unsigned char *)PyCapsule_GetPointer(mfsrc, "src");
    int h = munmap_binfile(mf);
    return Py_BuildValue("i", h);
}

static PyObject* py_get_address(PyObject *Py_UNUSED(ignored), PyObject *args)
{
    uint64_t blklen, blkpos, item;
    if (!PyArg_ParseTuple(args, "KKK", &blklen, &blkpos, &item))
        return NULL;
    uint64_t h = get_address(blklen, blkpos, item);
    return Py_BuildValue("K", h);
}


static PyObject* py_find_first_uint8(PyObject *Py_UNUSED(ignored), PyObject *args)
{
    PyObject *result;
    uint64_t blklen, blkpos, first, last;
    uint8_t search;
    PyObject* mfsrc = NULL;
    if (!PyArg_ParseTuple(args, "OKKKKB", &mfsrc, &blklen, &blkpos, &first, &last, &search))
        return NULL;
    const unsigned char *src = (const unsigned char *)PyCapsule_GetPointer(mfsrc, "src");
    uint64_t h = find_first_uint8_t(src, blklen, blkpos, &first, &last, search);
    result = PyTuple_New(3);
    PyTuple_SetItem(result, 0, Py_BuildValue("K", h));
    PyTuple_SetItem(result, 1, Py_BuildValue("K", first));
    PyTuple_SetItem(result, 2, Py_BuildValue("K", last));
    return result;
}

static PyObject* py_find_last_uint8(PyObject *Py_UNUSED(ignored), PyObject *args)
{
    PyObject *result;
    uint64_t blklen, blkpos, first, last;
    uint8_t search;
    PyObject* mfsrc = NULL;
    if (!PyArg_ParseTuple(args, "OKKKKB", &mfsrc, &blklen, &blkpos, &first, &last, &search))
        return NULL;
    const unsigned char *src = (const unsigned char *)PyCapsule_GetPointer(mfsrc, "src");
    uint64_t h = find_last_uint8_t(src, blklen, blkpos, &first, &last, search);
    result = PyTuple_New(3);
    PyTuple_SetItem(result, 0, Py_BuildValue("K", h));
    PyTuple_SetItem(result, 1, Py_BuildValue("K", first));
    PyTuple_SetItem(result, 2, Py_BuildValue("K", last));
    return result;
}

static PyObject* py_find_first_uint16(PyObject *Py_UNUSED(ignored), PyObject *args)
{
    PyObject *result;
    uint64_t blklen, blkpos, first, last;
    uint16_t search;
    PyObject* mfsrc = NULL;
    if (!PyArg_ParseTuple(args, "OKKKKH", &mfsrc, &blklen, &blkpos, &first, &last, &search))
        return NULL;
    const unsigned char *src = (const unsigned char *)PyCapsule_GetPointer(mfsrc, "src");
    uint64_t h = find_first_uint16_t(src, blklen, blkpos, &first, &last, search);
    result = PyTuple_New(3);
    PyTuple_SetItem(result, 0, Py_BuildValue("K", h));
    PyTuple_SetItem(result, 1, Py_BuildValue("K", first));
    PyTuple_SetItem(result, 2, Py_BuildValue("K", last));
    return result;
}

static PyObject* py_find_last_uint16(PyObject *Py_UNUSED(ignored), PyObject *args)
{
    PyObject *result;
    uint64_t blklen, blkpos, first, last;
    uint16_t search;
    PyObject* mfsrc = NULL;
    if (!PyArg_ParseTuple(args, "OKKKKH", &mfsrc, &blklen, &blkpos, &first, &last, &search))
        return NULL;
    const unsigned char *src = (const unsigned char *)PyCapsule_GetPointer(mfsrc, "src");
    uint64_t h = find_last_uint16_t(src, blklen, blkpos, &first, &last, search);
    result = PyTuple_New(3);
    PyTuple_SetItem(result, 0, Py_BuildValue("K", h));
    PyTuple_SetItem(result, 1, Py_BuildValue("K", first));
    PyTuple_SetItem(result, 2, Py_BuildValue("K", last));
    return result;
}

static PyObject* py_find_first_uint32(PyObject *Py_UNUSED(ignored), PyObject *args)
{
    PyObject *result;
    uint64_t blklen, blkpos, first, last;
    uint32_t search;
    PyObject* mfsrc = NULL;
    if (!PyArg_ParseTuple(args, "OKKKKI", &mfsrc, &blklen, &blkpos, &first, &last, &search))
        return NULL;
    const unsigned char *src = (const unsigned char *)PyCapsule_GetPointer(mfsrc, "src");
    uint64_t h = find_first_uint32_t(src, blklen, blkpos, &first, &last, search);
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
    uint32_t search;
    PyObject* mfsrc = NULL;
    if (!PyArg_ParseTuple(args, "OKKKKI", &mfsrc, &blklen, &blkpos, &first, &last, &search))
        return NULL;
    const unsigned char *src = (const unsigned char *)PyCapsule_GetPointer(mfsrc, "src");
    uint64_t h = find_last_uint32_t(src, blklen, blkpos, &first, &last, search);
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
    uint64_t search;
    PyObject* mfsrc = NULL;
    if (!PyArg_ParseTuple(args, "OKKKKK", &mfsrc, &blklen, &blkpos, &first, &last, &search))
        return NULL;
    const unsigned char *src = (const unsigned char *)PyCapsule_GetPointer(mfsrc, "src");
    uint64_t h = find_first_uint64_t(src, blklen, blkpos, &first, &last, search);
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
    uint64_t search;
    PyObject* mfsrc = NULL;
    if (!PyArg_ParseTuple(args, "OKKKKK", &mfsrc, &blklen, &blkpos, &first, &last, &search))
        return NULL;
    const unsigned char *src = (const unsigned char *)PyCapsule_GetPointer(mfsrc, "src");
    uint64_t h = find_last_uint64_t(src, blklen, blkpos, &first, &last, search);
    result = PyTuple_New(3);
    PyTuple_SetItem(result, 0, Py_BuildValue("K", h));
    PyTuple_SetItem(result, 1, Py_BuildValue("K", first));
    PyTuple_SetItem(result, 2, Py_BuildValue("K", last));
    return result;
}

static PyObject* py_find_first_uint128(PyObject *Py_UNUSED(ignored), PyObject *args)
{
    PyObject *result;
    uint64_t blklen, blkpos, first, last;
    uint128_t search;
    PyObject* mfsrc = NULL;
    if (!PyArg_ParseTuple(args, "OKKKKKK", &mfsrc, &blklen, &blkpos, &first, &last, &search.lo, &search.hi))
        return NULL;
    const unsigned char *src = (const unsigned char *)PyCapsule_GetPointer(mfsrc, "src");
    uint64_t h = find_first_uint128_t(src, blklen, blkpos, &first, &last, search);
    result = PyTuple_New(3);
    PyTuple_SetItem(result, 0, Py_BuildValue("K", h));
    PyTuple_SetItem(result, 1, Py_BuildValue("K", first));
    PyTuple_SetItem(result, 2, Py_BuildValue("K", last));
    return result;
}

static PyObject* py_find_last_uint128(PyObject *Py_UNUSED(ignored), PyObject *args)
{
    PyObject *result;
    uint64_t blklen, blkpos, first, last;
    uint128_t search;
    PyObject* mfsrc = NULL;
    if (!PyArg_ParseTuple(args, "OKKKKKK", &mfsrc, &blklen, &blkpos, &first, &last, &search.lo, &search.hi))
        return NULL;
    const unsigned char *src = (const unsigned char *)PyCapsule_GetPointer(mfsrc, "src");
    uint64_t h = find_last_uint128_t(src, blklen, blkpos, &first, &last, search);
    result = PyTuple_New(3);
    PyTuple_SetItem(result, 0, Py_BuildValue("K", h));
    PyTuple_SetItem(result, 1, Py_BuildValue("K", first));
    PyTuple_SetItem(result, 2, Py_BuildValue("K", last));
    return result;
}

// --- RSIDVAR128 ---

static PyObject* py_get_vr128_rsid(PyObject *Py_UNUSED(ignored), PyObject *args)
{
    uint64_t item;
    PyObject* mfsrc = NULL;
    if (!PyArg_ParseTuple(args, "OK", &mfsrc, &item))
        return NULL;
    const unsigned char *src = (const unsigned char *)PyCapsule_GetPointer(mfsrc, "src");
    uint32_t h = get_vr128_rsid(src, item);
    return Py_BuildValue("I", h);
}

static PyObject* py_get_rv128_variantkey(PyObject *Py_UNUSED(ignored), PyObject *args)
{
    PyObject *result;
    uint64_t item;
    PyObject* mfsrc = NULL;
    if (!PyArg_ParseTuple(args, "OK", &mfsrc, &item))
        return NULL;
    const unsigned char *src = (const unsigned char *)PyCapsule_GetPointer(mfsrc, "src");
    variantkey128_t h = get_rv128_variantkey(src, item);
    result = PyTuple_New(4);
    PyTuple_SetItem(result, 0, Py_BuildValue("I", h.assembly));
    PyTuple_SetItem(result, 1, Py_BuildValue("I", h.chrom));
    PyTuple_SetItem(result, 2, Py_BuildValue("I", h.pos));
    PyTuple_SetItem(result, 3, Py_BuildValue("I", h.refalt));
    return result;
}

static PyObject* py_find_rv128_variantkey_by_rsid(PyObject *Py_UNUSED(ignored), PyObject *args)
{
    PyObject *result;
    uint64_t first, last;
    uint32_t rsid;
    PyObject* mfsrc = NULL;
    if (!PyArg_ParseTuple(args, "OKKI", &mfsrc, &first, &last, &rsid))
        return NULL;
    const unsigned char *src = (const unsigned char *)PyCapsule_GetPointer(mfsrc, "src");
    variantkey128_t h = find_rv128_variantkey_by_rsid(src, &first, last, rsid);
    result = PyTuple_New(5);
    PyTuple_SetItem(result, 0, Py_BuildValue("I", h.assembly));
    PyTuple_SetItem(result, 1, Py_BuildValue("I", h.chrom));
    PyTuple_SetItem(result, 2, Py_BuildValue("I", h.pos));
    PyTuple_SetItem(result, 3, Py_BuildValue("I", h.refalt));
    PyTuple_SetItem(result, 4, Py_BuildValue("K", first));
    return result;
}

static PyObject* py_find_vr128_rsid_by_variantkey(PyObject *Py_UNUSED(ignored), PyObject *args)
{
    PyObject *result;
    uint64_t first, last;
    variantkey128_t vh;
    PyObject* mfsrc = NULL;
    if (!PyArg_ParseTuple(args, "OKKIIII", &mfsrc, &first, &last, &vh.assembly, &vh.chrom, &vh.pos, &vh.refalt))
        return NULL;
    const unsigned char *src = (const unsigned char *)PyCapsule_GetPointer(mfsrc, "src");
    uint32_t h = find_vr128_rsid_by_variantkey(src, &first, last, vh);
    result = PyTuple_New(2);
    PyTuple_SetItem(result, 0, Py_BuildValue("I", h));
    PyTuple_SetItem(result, 1, Py_BuildValue("K", first));
    return result;
}

static PyObject* py_find_vr128_chrom_range(PyObject *Py_UNUSED(ignored), PyObject *args)
{
    PyObject *result;
    uint64_t first, last;
    uint32_t chrom;
    PyObject* mfsrc = NULL;
    if (!PyArg_ParseTuple(args, "OKKI", &mfsrc, &first, &last, &chrom))
        return NULL;
    const unsigned char *src = (const unsigned char *)PyCapsule_GetPointer(mfsrc, "src");
    uint32_t h = find_vr128_chrom_range(src, &first, &last, chrom);
    result = PyTuple_New(3);
    PyTuple_SetItem(result, 0, Py_BuildValue("I", h));
    PyTuple_SetItem(result, 1, Py_BuildValue("K", first));
    PyTuple_SetItem(result, 2, Py_BuildValue("K", last));
    return result;
}

static PyObject* py_find_vr128_pos_range(PyObject *Py_UNUSED(ignored), PyObject *args)
{
    PyObject *result;
    uint64_t first, last;
    uint32_t pos_start, pos_end;
    PyObject* mfsrc = NULL;
    if (!PyArg_ParseTuple(args, "OKKII", &mfsrc, &first, &last, &pos_start, &pos_end))
        return NULL;
    const unsigned char *src = (const unsigned char *)PyCapsule_GetPointer(mfsrc, "src");
    uint32_t h = find_vr128_pos_range(src, &first, &last, pos_start, pos_end);
    result = PyTuple_New(3);
    PyTuple_SetItem(result, 0, Py_BuildValue("I", h));
    PyTuple_SetItem(result, 1, Py_BuildValue("K", first));
    PyTuple_SetItem(result, 2, Py_BuildValue("K", last));
    return result;
}

static PyObject* py_find_vr128_chrompos_range(PyObject *Py_UNUSED(ignored), PyObject *args)
{
    PyObject *result;
    uint64_t first, last;
    uint32_t chrom, pos_start, pos_end;
    PyObject* mfsrc = NULL;
    if (!PyArg_ParseTuple(args, "OKKIII", &mfsrc, &first, &last, &chrom, &pos_start, &pos_end))
        return NULL;
    const unsigned char *src = (const unsigned char *)PyCapsule_GetPointer(mfsrc, "src");
    uint32_t h = find_vr128_chrompos_range(src, &first, &last, chrom, pos_start, pos_end);
    result = PyTuple_New(3);
    PyTuple_SetItem(result, 0, Py_BuildValue("I", h));
    PyTuple_SetItem(result, 1, Py_BuildValue("K", first));
    PyTuple_SetItem(result, 2, Py_BuildValue("K", last));
    return result;
}

// --- RSIDVAR64 ---

static PyObject* py_get_vr64_rsid(PyObject *Py_UNUSED(ignored), PyObject *args)
{
    uint64_t item;
    PyObject* mfsrc = NULL;
    if (!PyArg_ParseTuple(args, "OK", &mfsrc, &item))
        return NULL;
    const unsigned char *src = (const unsigned char *)PyCapsule_GetPointer(mfsrc, "src");
    uint32_t h = get_vr64_rsid(src, item);
    return Py_BuildValue("I", h);
}

static PyObject* py_get_rv64_variantkey(PyObject *Py_UNUSED(ignored), PyObject *args)
{
    uint64_t item;
    PyObject* mfsrc = NULL;
    if (!PyArg_ParseTuple(args, "OK", &mfsrc, &item))
        return NULL;
    const unsigned char *src = (const unsigned char *)PyCapsule_GetPointer(mfsrc, "src");
    uint64_t h = get_rv64_variantkey(src, item);
    return Py_BuildValue("K", h);
}

static PyObject* py_find_rv64_variantkey_by_rsid(PyObject *Py_UNUSED(ignored), PyObject *args)
{
    uint64_t first, last;
    uint32_t rsid;
    PyObject* mfsrc = NULL;
    if (!PyArg_ParseTuple(args, "OKKI", &mfsrc, &first, &last, &rsid))
        return NULL;
    const unsigned char *src = (const unsigned char *)PyCapsule_GetPointer(mfsrc, "src");
    uint64_t h = find_rv64_variantkey_by_rsid(src, &first, last, rsid);
    return Py_BuildValue("K", h);
}

static PyObject* py_find_vr64_rsid_by_variantkey(PyObject *Py_UNUSED(ignored), PyObject *args)
{
    PyObject *result;
    uint64_t first, last, vh;
    PyObject* mfsrc = NULL;
    if (!PyArg_ParseTuple(args, "OKKK", &mfsrc, &first, &last, &vh))
        return NULL;
    const unsigned char *src = (const unsigned char *)PyCapsule_GetPointer(mfsrc, "src");
    uint32_t h = find_vr64_rsid_by_variantkey(src, &first, last, vh);
    result = PyTuple_New(2);
    PyTuple_SetItem(result, 0, Py_BuildValue("I", h));
    PyTuple_SetItem(result, 1, Py_BuildValue("K", first));
    return result;
}

static PyObject* py_find_vr64_chrom_range(PyObject *Py_UNUSED(ignored), PyObject *args)
{
    PyObject *result;
    uint64_t first, last;
    uint8_t chrom;
    PyObject* mfsrc = NULL;
    if (!PyArg_ParseTuple(args, "OKKB", &mfsrc, &first, &last, &chrom))
        return NULL;
    const unsigned char *src = (const unsigned char *)PyCapsule_GetPointer(mfsrc, "src");
    uint32_t h = find_vr64_chrom_range(src, &first, &last, chrom);
    result = PyTuple_New(3);
    PyTuple_SetItem(result, 0, Py_BuildValue("I", h));
    PyTuple_SetItem(result, 1, Py_BuildValue("K", first));
    PyTuple_SetItem(result, 2, Py_BuildValue("K", last));
    return result;
}

static PyObject* py_find_vr64_pos_range(PyObject *Py_UNUSED(ignored), PyObject *args)
{
    PyObject *result;
    uint64_t first, last;
    uint32_t pos_start, pos_end;
    PyObject* mfsrc = NULL;
    if (!PyArg_ParseTuple(args, "OKKIII", &mfsrc, &first, &last, &pos_start, &pos_end))
        return NULL;
    const unsigned char *src = (const unsigned char *)PyCapsule_GetPointer(mfsrc, "src");
    uint32_t h = find_vr64_pos_range(src, &first, &last, pos_start, pos_end);
    result = PyTuple_New(3);
    PyTuple_SetItem(result, 0, Py_BuildValue("I", h));
    PyTuple_SetItem(result, 1, Py_BuildValue("K", first));
    PyTuple_SetItem(result, 2, Py_BuildValue("K", last));
    return result;
}

static PyObject* py_find_vr64_chrompos_range(PyObject *Py_UNUSED(ignored), PyObject *args)
{
    PyObject *result;
    uint64_t first, last;
    uint8_t chrom;
    uint32_t pos_start, pos_end;
    PyObject* mfsrc = NULL;
    if (!PyArg_ParseTuple(args, "OKKIII", &mfsrc, &first, &last, &chrom, &pos_start, &pos_end))
        return NULL;
    const unsigned char *src = (const unsigned char *)PyCapsule_GetPointer(mfsrc, "src");
    uint32_t h = find_vr64_chrompos_range(src, &first, &last, chrom, pos_start, pos_end);
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
    {"encode_assembly_32bit", py_encode_assembly_32bit, METH_VARARGS, PYENCODEASSEMBLY32BIT_DOCSTRING},
    {"encode_chrom_32bit", py_encode_chrom_32bit, METH_VARARGS, PYENCODECHROM32BIT_DOCSTRING},
    {"encode_chrom_8bit", py_encode_chrom_8bit, METH_VARARGS, PYENCODECHROM8BIT_DOCSTRING},
    {"decode_chrom_32bit", py_decode_chrom_32bit, METH_VARARGS, PYDECODECHROM32BIT_DOCSTRING},
    {"decode_chrom_8bit", py_decode_chrom_8bit, METH_VARARGS, PYDECODECHROM8BIT_DOCSTRING},
    {"encode_refalt_32bit", py_encode_refalt_32bit, METH_VARARGS, PYENCODEREFALT32BIT_DOCSTRING},
    {"encode_refalt_24bit", py_encode_refalt_24bit, METH_VARARGS, PYENCODEREFALT24BIT_DOCSTRING},
    {"decode_refalt_32bit", py_decode_refalt_32bit, METH_VARARGS, PYDECODEREFALT32BIT_DOCSTRING},
    {"decode_refalt_24bit", py_decode_refalt_24bit, METH_VARARGS, PYDECODEREFALT24BIT_DOCSTRING},
    {"variantkey128", py_variantkey128, METH_VARARGS, PYVARIANTKEY128_DOCSTRING},
    {"variantkey64", py_variantkey64, METH_VARARGS, PYVARIANTKEY64_DOCSTRING},
    {"variantkey128_string", py_variantkey128_string, METH_VARARGS, PYVARIANTKEY128STRING_DOCSTRING},
    {"variantkey64_string", py_variantkey64_string, METH_VARARGS, PYVARIANTKEY64STRING_DOCSTRING},
    {"parse_variantkey128_string", py_parse_variantkey128_string, METH_VARARGS, PYPARSEVARIANTKEY128STRING_DOCSTRING},
    {"parse_variantkey64_string", py_parse_variantkey64_string, METH_VARARGS, PYPARSEVARIANTKEY64STRING_DOCSTRING},
    {"split_variantkey64", py_split_variantkey64, METH_VARARGS, PYSPLITVARIANTKEY64_DOCSTRING},

    // FARMHASH
    {"farmhash64", py_farmhash64, METH_VARARGS, PYFARMHASH64_DOCSTRING},
    {"farmhash32", py_farmhash32, METH_VARARGS, PYFARMHASH32_DOCSTRING},

    // BINSEARCH
    {"mmap_binfile", py_mmap_binfile, METH_VARARGS, PYMMAPBINFILE_DOCSTRING},
    {"munmap_binfile", py_munmap_binfile, METH_VARARGS, PYMUNMAPBINFILE_DOCSTRING},
    {"get_address", py_get_address, METH_VARARGS, PYGETADDRESS_DOCSTRING},
    {"find_first_uint8", py_find_first_uint8, METH_VARARGS, PYFINDFIRSTUINT8_DOCSTRING},
    {"find_last_uint8", py_find_last_uint8, METH_VARARGS, PYFINDLASTUINT8_DOCSTRING},
    {"find_first_uint16", py_find_first_uint16, METH_VARARGS, PYFINDFIRSTUINT16_DOCSTRING},
    {"find_last_uint16", py_find_last_uint16, METH_VARARGS, PYFINDLASTUINT16_DOCSTRING},
    {"find_first_uint32", py_find_first_uint32, METH_VARARGS, PYFINDFIRSTUINT32_DOCSTRING},
    {"find_last_uint32", py_find_last_uint32, METH_VARARGS, PYFINDLASTUINT32_DOCSTRING},
    {"find_first_uint64", py_find_first_uint64, METH_VARARGS, PYFINDFIRSTUINT64_DOCSTRING},
    {"find_last_uint64", py_find_last_uint64, METH_VARARGS, PYFINDLASTUINT64_DOCSTRING},
    {"find_first_uint128", py_find_first_uint128, METH_VARARGS, PYFINDFIRSTUINT128_DOCSTRING},
    {"find_last_uint128", py_find_last_uint128, METH_VARARGS, PYFINDLASTUINT128_DOCSTRING},

    // RSIDVAR128
    {"get_vr128_rsid", py_get_vr128_rsid, METH_VARARGS, PYGETVR128RSID_DOCSTRING},
    {"get_rv128_variantkey", py_get_rv128_variantkey, METH_VARARGS, PYGETRV128VARIANTKEY_DOCSTRING},
    {"find_rv128_variantkey_by_rsid", py_find_rv128_variantkey_by_rsid, METH_VARARGS, PYFINDRV128VARIANTKEYBYRSID_DOCSTRING},
    {"find_vr128_rsid_by_variantkey", py_find_vr128_rsid_by_variantkey, METH_VARARGS, PYFINDVR128RSIDBYVARIANTKEY_DOCSTRING},
    {"find_vr128_chrom_range", py_find_vr128_chrom_range, METH_VARARGS, PYFINDVR128CHROMRANGE_DOCSTRING},
    {"find_vr128_pos_range", py_find_vr128_pos_range, METH_VARARGS, PYFINDVR128POSRANGE_DOCSTRING},
    {"find_vr128_chrompos_range", py_find_vr128_chrompos_range, METH_VARARGS, PYFINDVR128CHROMPOSRANGE_DOCSTRING},

    // RSIDVAR64
    {"get_vr64_rsid", py_get_vr64_rsid, METH_VARARGS, PYGETVR64RSID_DOCSTRING},
    {"get_rv64_variantkey", py_get_rv64_variantkey, METH_VARARGS, PYGETRV64VARIANTKEY_DOCSTRING},
    {"find_rv64_variantkey_by_rsid", py_find_rv64_variantkey_by_rsid, METH_VARARGS, PYFINDRV64VARIANTKEYBYRSID_DOCSTRING},
    {"find_vr64_rsid_by_variantkey", py_find_vr64_rsid_by_variantkey, METH_VARARGS, PYFINDVR64RSIDBYVARIANTKEY_DOCSTRING},
    {"find_vr64_chrom_range", py_find_vr64_chrom_range, METH_VARARGS, PYFINDVR64CHROMRANGE_DOCSTRING},
    {"find_vr64_pos_range(", py_find_vr64_pos_range, METH_VARARGS, PYFINDVR64POSRANGE_DOCSTRING},
    {"find_vr64_chrompos_range", py_find_vr64_chrompos_range, METH_VARARGS, PYFINDVR64CHROMPOSRANGE_DOCSTRING},

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
