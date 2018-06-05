// Python VariantKey Module
//
// @category   Libraries
// @author     Nicola Asuni <nicola.asuni@genomicsplc.com>
// @license    MIT (see LICENSE)
// @link       https://github.com/genomicsplc/variantkey

#include <Python.h>
#include "../../c/src/variantkey.h"
#include "../../c/src/binsearch.h"
#include "../../c/src/rsidvar.h"
#include "../../c/src/nrvk.h"
#include "../../c/src/genoref.h"
#include "pyvariantkey.h"

#ifndef Py_UNUSED /* This is already defined for Python 3.4 onwards */
#ifdef __GNUC__
#define Py_UNUSED(name) _unused_ ## name __attribute__((unused))
#else
#define Py_UNUSED(name) _unused_ ## name
#endif
#endif

#define ALLELE_BUFFSIZE 12
#define ALLELE_MAXSIZE 256

static PyObject* py_encode_chrom(PyObject *Py_UNUSED(ignored), PyObject *args)
{
    const char *chrom;
    int sizechrom;
    if (!PyArg_ParseTuple(args, "s#", &chrom, &sizechrom))
        return NULL;
    uint8_t h = encode_chrom(chrom, (size_t)sizechrom);
    return Py_BuildValue("B", h);
}

static PyObject* py_decode_chrom(PyObject *Py_UNUSED(ignored), PyObject *args)
{
    uint8_t code;
    if (!PyArg_ParseTuple(args, "B", &code))
        return NULL;
    char chrom[3] = "";
    decode_chrom(code, chrom);
    return Py_BuildValue("y", chrom);
}

static PyObject* py_encode_refalt(PyObject *Py_UNUSED(ignored), PyObject *args)
{
    const char *ref, *alt;
    int sizeref, sizealt;
    if (!PyArg_ParseTuple(args, "s#s#", &ref, &sizeref, &alt, &sizealt))
        return NULL;
    uint32_t h = encode_refalt(ref, (size_t)sizeref, alt, (size_t)sizealt);
    return Py_BuildValue("I", h);
}

static PyObject* py_decode_refalt(PyObject *Py_UNUSED(ignored), PyObject *args)
{
    PyObject *result;
    uint32_t code;
    if (!PyArg_ParseTuple(args, "I", &code))
        return NULL;
    char ref[ALLELE_BUFFSIZE] = "", alt[ALLELE_BUFFSIZE] = "";
    size_t sizeref = 0, sizealt = 0;
    decode_refalt(code, ref, &sizeref, alt, &sizealt);
    result = PyTuple_New(4);
    PyTuple_SetItem(result, 0, Py_BuildValue("y", ref));
    PyTuple_SetItem(result, 1, Py_BuildValue("y", alt));
    PyTuple_SetItem(result, 2, Py_BuildValue("K", sizeref));
    PyTuple_SetItem(result, 3, Py_BuildValue("K", sizealt));
    return result;
}

static PyObject* py_encode_variantkey(PyObject *Py_UNUSED(ignored), PyObject *args)
{
    uint8_t chrom;
    uint32_t pos, refalt;
    if (!PyArg_ParseTuple(args, "BII", &chrom, &pos, &refalt))
        return NULL;
    uint64_t h = encode_variantkey(chrom, pos, refalt);
    return Py_BuildValue("K", h);
}

static PyObject* py_extract_variantkey_chrom(PyObject *Py_UNUSED(ignored), PyObject *args)
{
    uint64_t code;
    if (!PyArg_ParseTuple(args, "K", &code))
        return NULL;
    uint8_t h = extract_variantkey_chrom(code);
    return Py_BuildValue("B", h);
}

static PyObject* py_extract_variantkey_pos(PyObject *Py_UNUSED(ignored), PyObject *args)
{
    uint64_t code;
    if (!PyArg_ParseTuple(args, "K", &code))
        return NULL;
    uint32_t h = extract_variantkey_pos(code);
    return Py_BuildValue("I", h);
}

static PyObject* py_extract_variantkey_refalt(PyObject *Py_UNUSED(ignored), PyObject *args)
{
    uint64_t code;
    if (!PyArg_ParseTuple(args, "K", &code))
        return NULL;
    uint32_t h = extract_variantkey_refalt(code);
    return Py_BuildValue("I", h);
}

static PyObject* py_decode_variantkey(PyObject *Py_UNUSED(ignored), PyObject *args)
{
    PyObject *result;
    uint64_t code;
    if (!PyArg_ParseTuple(args, "K", &code))
        return NULL;
    variantkey_t h = {0};
    decode_variantkey(code, &h);
    result = PyTuple_New(3);
    PyTuple_SetItem(result, 0, Py_BuildValue("B", h.chrom));
    PyTuple_SetItem(result, 1, Py_BuildValue("I", h.pos));
    PyTuple_SetItem(result, 2, Py_BuildValue("I", h.refalt));
    return result;
}

static PyObject* py_variantkey(PyObject *Py_UNUSED(ignored), PyObject *args)
{
    const char *chrom, *ref, *alt;
    int sizechrom, sizeref, sizealt;
    uint32_t pos;
    if (!PyArg_ParseTuple(args, "s#Is#s#", &chrom, &sizechrom, &pos, &ref, &sizeref, &alt, &sizealt))
        return NULL;
    uint64_t h = variantkey(chrom, (size_t)sizechrom, pos, ref, (size_t)sizeref, alt, (size_t)sizealt);
    return Py_BuildValue("K", h);
}

static PyObject* py_variantkey_range(PyObject *Py_UNUSED(ignored), PyObject *args)
{
    PyObject *result;
    uint8_t chrom;
    uint32_t pos_min, pos_max;
    if (!PyArg_ParseTuple(args, "BII", &chrom, &pos_min, &pos_max))
        return NULL;
    vkrange_t r;
    variantkey_range(chrom, pos_min, pos_max, &r);
    result = PyTuple_New(2);
    PyTuple_SetItem(result, 0, Py_BuildValue("K", r.min));
    PyTuple_SetItem(result, 1, Py_BuildValue("K", r.max));
    return result;
}

static PyObject* py_compare_variantkey_chrom(PyObject *Py_UNUSED(ignored), PyObject *args)
{
    uint64_t vka, vkb;
    if (!PyArg_ParseTuple(args, "KK", &vka, &vkb))
        return NULL;
    int cmp = compare_variantkey_chrom(vka, vkb);
    return Py_BuildValue("i", cmp);
}

static PyObject* py_compare_variantkey_chrom_pos(PyObject *Py_UNUSED(ignored), PyObject *args)
{
    uint64_t vka, vkb;
    if (!PyArg_ParseTuple(args, "KK", &vka, &vkb))
        return NULL;
    int cmp = compare_variantkey_chrom_pos(vka, vkb);
    return Py_BuildValue("i", cmp);
}

static PyObject* py_variantkey_hex(PyObject *Py_UNUSED(ignored), PyObject *args)
{
    uint64_t code;
    if (!PyArg_ParseTuple(args, "K", &code))
        return NULL;
    char str[17];
    variantkey_hex(code, str);
    return PyBytes_FromString(str);
}

static PyObject* py_parse_variantkey_hex(PyObject *Py_UNUSED(ignored), PyObject *args)
{
    const char *vs;
    int sizevs;
    if (!PyArg_ParseTuple(args, "s#", &vs, &sizevs))
        return NULL;
    uint64_t h = 0;
    if (sizevs == 16)
    {
        h = parse_variantkey_hex(vs);
    }
    return Py_BuildValue("K", h);
}

// --- BINSEARCH ---

static PyObject* py_mmap_binfile(PyObject *Py_UNUSED(ignored), PyObject *args)
{
    PyObject *result;
    const char *file;
    if (!PyArg_ParseTuple(args, "s", &file))
        return NULL;
    mmfile_t h = {0};
    mmap_binfile(file, &h);
    result = PyTuple_New(4);
    PyTuple_SetItem(result, 0, PyCapsule_New((void*)h.src, "src", NULL));
    PyTuple_SetItem(result, 1, Py_BuildValue("i", h.fd));
    PyTuple_SetItem(result, 2, Py_BuildValue("K", h.size));
    PyTuple_SetItem(result, 3, Py_BuildValue("K", h.last));
    return result;
}

static PyObject* py_munmap_binfile(PyObject *Py_UNUSED(ignored), PyObject *args)
{
    mmfile_t mf = {0};
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

static PyObject* py_find_first_uint8(PyObject *Py_UNUSED(ignored), PyObject *args)
{
    PyObject *result;
    uint64_t blklen, blkpos, first, last;
    uint8_t bitstart, bitend;
    uint8_t search;
    PyObject* mfsrc = NULL;
    if (!PyArg_ParseTuple(args, "OKKBBKKB", &mfsrc, &blklen, &blkpos, &bitstart, &bitend, &first, &last, &search))
        return NULL;
    const unsigned char *src = (const unsigned char *)PyCapsule_GetPointer(mfsrc, "src");
    uint64_t h = find_first_uint8_t(src, blklen, blkpos, bitstart, bitend, &first, &last, search);
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
    uint8_t bitstart, bitend;
    uint8_t search;
    PyObject* mfsrc = NULL;
    if (!PyArg_ParseTuple(args, "OKKBBKKB", &mfsrc, &blklen, &blkpos, &bitstart, &bitend, &first, &last, &search))
        return NULL;
    const unsigned char *src = (const unsigned char *)PyCapsule_GetPointer(mfsrc, "src");
    uint64_t h = find_last_uint8_t(src, blklen, blkpos, bitstart, bitend, &first, &last, search);
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
    uint8_t bitstart, bitend;
    uint16_t search;
    PyObject* mfsrc = NULL;
    if (!PyArg_ParseTuple(args, "OKKBBKKH", &mfsrc, &blklen, &blkpos, &bitstart, &bitend, &first, &last, &search))
        return NULL;
    const unsigned char *src = (const unsigned char *)PyCapsule_GetPointer(mfsrc, "src");
    uint64_t h = find_first_uint16_t(src, blklen, blkpos, bitstart, bitend, &first, &last, search);
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
    uint8_t bitstart, bitend;
    uint16_t search;
    PyObject* mfsrc = NULL;
    if (!PyArg_ParseTuple(args, "OKKBBKKH", &mfsrc, &blklen, &blkpos, &bitstart, &bitend, &first, &last, &search))
        return NULL;
    const unsigned char *src = (const unsigned char *)PyCapsule_GetPointer(mfsrc, "src");
    uint64_t h = find_last_uint16_t(src, blklen, blkpos, bitstart, bitend, &first, &last, search);
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
    uint64_t first, last, vk;
    PyObject* mfsrc = NULL;
    if (!PyArg_ParseTuple(args, "OKKK", &mfsrc, &first, &last, &vk))
        return NULL;
    const unsigned char *src = (const unsigned char *)PyCapsule_GetPointer(mfsrc, "src");
    uint32_t h = find_vr_rsid_by_variantkey(src, &first, last, vk);
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
    uint32_t pos_min, pos_max;
    PyObject* mfsrc = NULL;
    if (!PyArg_ParseTuple(args, "OKKBII", &mfsrc, &first, &last, &chrom, &pos_min, &pos_max))
        return NULL;
    const unsigned char *src = (const unsigned char *)PyCapsule_GetPointer(mfsrc, "src");
    uint32_t h = find_vr_chrompos_range(src, &first, &last, chrom, pos_min, pos_max);
    result = PyTuple_New(3);
    PyTuple_SetItem(result, 0, Py_BuildValue("I", h));
    PyTuple_SetItem(result, 1, Py_BuildValue("K", first));
    PyTuple_SetItem(result, 2, Py_BuildValue("K", last));
    return result;
}

// --- NRVK ---

static PyObject* py_find_ref_alt_by_variantkey(PyObject *Py_UNUSED(ignored), PyObject *args)
{
    PyObject *result;
    uint64_t last, vk;
    PyObject* mfsrc = NULL;
    if (!PyArg_ParseTuple(args, "OKK", &mfsrc, &last, &vk))
        return NULL;
    char ref[ALLELE_MAXSIZE] = "", alt[ALLELE_MAXSIZE] = "";
    size_t sizeref = 0, sizealt = 0;
    const unsigned char *src = (const unsigned char *)PyCapsule_GetPointer(mfsrc, "src");
    size_t len = find_ref_alt_by_variantkey(src, last, vk, ref, &sizeref, alt, &sizealt);
    result = PyTuple_New(5);
    PyTuple_SetItem(result, 0, Py_BuildValue("y", ref));
    PyTuple_SetItem(result, 1, Py_BuildValue("y", alt));
    PyTuple_SetItem(result, 2, Py_BuildValue("K", sizeref));
    PyTuple_SetItem(result, 3, Py_BuildValue("K", sizealt));
    PyTuple_SetItem(result, 4, Py_BuildValue("K", len));
    return result;
}

static PyObject* py_reverse_variantkey(PyObject *Py_UNUSED(ignored), PyObject *args)
{
    PyObject *result;
    uint64_t last, vk;
    PyObject* mfsrc = NULL;
    if (!PyArg_ParseTuple(args, "OKK", &mfsrc, &last, &vk))
        return NULL;
    const unsigned char *src = (const unsigned char *)PyCapsule_GetPointer(mfsrc, "src");
    variantkey_rev_t rev = {0};
    size_t len = reverse_variantkey(src, last, vk, &rev);
    result = PyTuple_New(7);
    PyTuple_SetItem(result, 0, Py_BuildValue("y", rev.chrom));
    PyTuple_SetItem(result, 1, Py_BuildValue("I", rev.pos));
    PyTuple_SetItem(result, 2, Py_BuildValue("y", rev.ref));
    PyTuple_SetItem(result, 3, Py_BuildValue("y", rev.alt));
    PyTuple_SetItem(result, 4, Py_BuildValue("K", rev.sizeref));
    PyTuple_SetItem(result, 5, Py_BuildValue("K", rev.sizealt));
    PyTuple_SetItem(result, 6, Py_BuildValue("K", len));
    return result;
}

// --- GENOREF ---

static PyObject* py_load_genoref_index(PyObject *Py_UNUSED(ignored), PyObject *args)
{
    PyObject* mfsrc = NULL;
    if (!PyArg_ParseTuple(args, "O", &mfsrc))
        return NULL;
    const unsigned char *src = (const unsigned char *)PyCapsule_GetPointer(mfsrc, "src");
    uint32_t *idx = (uint32_t *)malloc(27 * sizeof(uint32_t));
    load_genoref_index(src, idx);
    return PyCapsule_New((void*)idx, "idx", NULL);
}

static PyObject* py_get_genoref_seq(PyObject *Py_UNUSED(ignored), PyObject *args)
{
    PyObject* mfsrc = NULL;
    PyObject* mfidx = NULL;
    uint8_t chrom;
    uint32_t pos;
    if (!PyArg_ParseTuple(args, "OOBI", &mfsrc, &mfidx, &chrom, &pos))
        return NULL;
    const unsigned char *src = (const unsigned char *)PyCapsule_GetPointer(mfsrc, "src");
    uint32_t *idx = (uint32_t *)PyCapsule_GetPointer(mfidx, "idx");
    char ref = get_genoref_seq(src, idx, chrom, pos);
    return Py_BuildValue("c", ref);
}

// ---

static PyMethodDef PyVariantKeyMethods[] =
{
    // VARIANTKEY
    {"encode_chrom", py_encode_chrom, METH_VARARGS, PYENCODECHROM_DOCSTRING},
    {"decode_chrom", py_decode_chrom, METH_VARARGS, PYDECODECHROM_DOCSTRING},
    {"encode_refalt", py_encode_refalt, METH_VARARGS, PYENCODEREFALT_DOCSTRING},
    {"decode_refalt", py_decode_refalt, METH_VARARGS, PYDECODEREFALT_DOCSTRING},
    {"encode_variantkey", py_encode_variantkey, METH_VARARGS, PYENCODEVARIANTKEY_DOCSTRING},
    {"extract_variantkey_chrom", py_extract_variantkey_chrom, METH_VARARGS, PYEXTRACTVARIANTKEYCHROM_DOCSTRING},
    {"extract_variantkey_pos", py_extract_variantkey_pos, METH_VARARGS, PYEXTRACTVARIANTKEYPOS_DOCSTRING},
    {"extract_variantkey_refalt", py_extract_variantkey_refalt, METH_VARARGS, PYEXTRACTVARIANTKEYREFALT_DOCSTRING},
    {"decode_variantkey", py_decode_variantkey, METH_VARARGS, PYDECODEVARIANTKEY_DOCSTRING},
    {"variantkey", py_variantkey, METH_VARARGS, PYVARIANTKEY_DOCSTRING},
    {"variantkey_range", py_variantkey_range, METH_VARARGS, PYVARIANTKEYRANGE_DOCSTRING},
    {"compare_variantkey_chrom", py_compare_variantkey_chrom, METH_VARARGS, PYCOMPAREVARIANTKEYCHROM_DOCSTRING},
    {"compare_variantkey_chrom_pos", py_compare_variantkey_chrom_pos, METH_VARARGS, PYCOMPAREVARIANTKEYCHROMPOS_DOCSTRING},
    {"variantkey_hex", py_variantkey_hex, METH_VARARGS, PYVARIANTKEYHEX_DOCSTRING},
    {"parse_variantkey_hex", py_parse_variantkey_hex, METH_VARARGS, PYPARSEVARIANTKEYSTRING_DOCSTRING},

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

    // RSIDVAR
    {"get_vr_rsid", py_get_vr_rsid, METH_VARARGS, PYGETVRRSID_DOCSTRING},
    {"get_rv_variantkey", py_get_rv_variantkey, METH_VARARGS, PYGETRVVARIANTKEY_DOCSTRING},
    {"find_rv_variantkey_by_rsid", py_find_rv_variantkey_by_rsid, METH_VARARGS, PYFINDRVVARIANTKEYBYRSID_DOCSTRING},
    {"find_vr_rsid_by_variantkey", py_find_vr_rsid_by_variantkey, METH_VARARGS, PYFINDVRRSIDBYVARIANTKEY_DOCSTRING},
    {"find_vr_chrompos_range", py_find_vr_chrompos_range, METH_VARARGS, PYFINDVRCHROMPOSRANGE_DOCSTRING},

    // NRVK
    {"find_ref_alt_by_variantkey", py_find_ref_alt_by_variantkey, METH_VARARGS, PYFINDREFALTBYVARIANTKEY_DOCSTRING},
    {"reverse_variantkey", py_reverse_variantkey, METH_VARARGS, PYREVERSEVARIANTKEY_DOCSTRING},

    // GENOREF
    {"load_genoref_index", py_load_genoref_index, METH_VARARGS, PYLOADGENOREFINDEX},
    {"get_genoref_seq", py_get_genoref_seq, METH_VARARGS, PYGETGENOREFSEQ},

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
