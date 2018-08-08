// VariantKey Python wrapper
//
// pyvariantkey.c
//
// @category   Libraries
// @author     Nicola Asuni <nicola.asuni@genomicsplc.com>
// @copyright  2017-2018 GENOMICS plc
// @license    MIT (see LICENSE)
// @link       https://github.com/genomicsplc/variantkey
//
// LICENSE
//
// Copyright (c) 2017-2018 GENOMICS plc
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.

#define PY_SSIZE_T_CLEAN  /* Make "s#" use Py_ssize_t rather than int. */

#include <Python.h>
#include "../../c/src/astring.h"
#include "../../c/src/binsearch.h"
#include "../../c/src/genoref.h"
#include "../../c/src/nrvk.h"
#include "../../c/src/regionkey.h"
#include "../../c/src/rsidvar.h"
#include "../../c/src/variantkey.h"
#include "pyvariantkey.h"

#ifndef Py_UNUSED /* This is already defined for Python 3.4 onwards */
#ifdef __GNUC__
#define Py_UNUSED(name) _unused_ ## name __attribute__((unused))
#else
#define Py_UNUSED(name) _unused_ ## name
#endif
#endif

#define ALLELE_BUFFSIZE 12

static PyObject* py_encode_chrom(PyObject *Py_UNUSED(ignored), PyObject *args, PyObject *keywds)
{
    const char *chrom;
    Py_ssize_t sizechrom;
    static char *kwlist[] = {"chrom", NULL};
    if (!PyArg_ParseTupleAndKeywords(args, keywds, "s#", kwlist, &chrom, &sizechrom))
        return NULL;
    uint8_t h = encode_chrom(chrom, (size_t)sizechrom);
    return Py_BuildValue("B", h);
}

static PyObject* py_decode_chrom(PyObject *Py_UNUSED(ignored), PyObject *args, PyObject *keywds)
{
    uint8_t code;
    static char *kwlist[] = {"code", NULL};
    if (!PyArg_ParseTupleAndKeywords(args, keywds, "B", kwlist, &code))
        return NULL;
    char chrom[3] = "";
    decode_chrom(code, chrom);
    return Py_BuildValue("y", chrom);
}

static PyObject* py_encode_refalt(PyObject *Py_UNUSED(ignored), PyObject *args, PyObject *keywds)
{
    const char *ref, *alt;
    Py_ssize_t sizeref, sizealt;
    static char *kwlist[] = {"ref", "alt", NULL};
    if (!PyArg_ParseTupleAndKeywords(args, keywds, "s#s#", kwlist, &ref, &sizeref, &alt, &sizealt))
        return NULL;
    uint32_t h = encode_refalt(ref, (size_t)sizeref, alt, (size_t)sizealt);
    return Py_BuildValue("I", h);
}

static PyObject* py_decode_refalt(PyObject *Py_UNUSED(ignored), PyObject *args, PyObject *keywds)
{
    PyObject *result;
    uint32_t code;
    static char *kwlist[] = {"code", NULL};
    if (!PyArg_ParseTupleAndKeywords(args, keywds, "I", kwlist, &code))
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

static PyObject* py_encode_variantkey(PyObject *Py_UNUSED(ignored), PyObject *args, PyObject *keywds)
{
    uint8_t chrom;
    uint32_t pos, refalt;
    static char *kwlist[] = {"chrom", "pos", "refalt", NULL};
    if (!PyArg_ParseTupleAndKeywords(args, keywds, "BII", kwlist, &chrom, &pos, &refalt))
        return NULL;
    uint64_t h = encode_variantkey(chrom, pos, refalt);
    return Py_BuildValue("K", h);
}

static PyObject* py_extract_variantkey_chrom(PyObject *Py_UNUSED(ignored), PyObject *args, PyObject *keywds)
{
    uint64_t vk;
    static char *kwlist[] = {"vk", NULL};
    if (!PyArg_ParseTupleAndKeywords(args, keywds, "K", kwlist, &vk))
        return NULL;
    uint8_t h = extract_variantkey_chrom(vk);
    return Py_BuildValue("B", h);
}

static PyObject* py_extract_variantkey_pos(PyObject *Py_UNUSED(ignored), PyObject *args, PyObject *keywds)
{
    uint64_t vk;
    static char *kwlist[] = {"vk", NULL};
    if (!PyArg_ParseTupleAndKeywords(args, keywds, "K", kwlist, &vk))
        return NULL;
    uint32_t h = extract_variantkey_pos(vk);
    return Py_BuildValue("I", h);
}

static PyObject* py_extract_variantkey_refalt(PyObject *Py_UNUSED(ignored), PyObject *args, PyObject *keywds)
{
    uint64_t vk;
    static char *kwlist[] = {"vk", NULL};
    if (!PyArg_ParseTupleAndKeywords(args, keywds, "K", kwlist, &vk))
        return NULL;
    uint32_t h = extract_variantkey_refalt(vk);
    return Py_BuildValue("I", h);
}

static PyObject* py_decode_variantkey(PyObject *Py_UNUSED(ignored), PyObject *args, PyObject *keywds)
{
    PyObject *result;
    uint64_t vk;
    static char *kwlist[] = {"vk", NULL};
    if (!PyArg_ParseTupleAndKeywords(args, keywds, "K", kwlist, &vk))
        return NULL;
    variantkey_t h = {0};
    decode_variantkey(vk, &h);
    result = PyTuple_New(3);
    PyTuple_SetItem(result, 0, Py_BuildValue("B", h.chrom));
    PyTuple_SetItem(result, 1, Py_BuildValue("I", h.pos));
    PyTuple_SetItem(result, 2, Py_BuildValue("I", h.refalt));
    return result;
}

static PyObject* py_variantkey(PyObject *Py_UNUSED(ignored), PyObject *args, PyObject *keywds)
{
    const char *chrom, *ref, *alt;
    Py_ssize_t sizechrom, sizeref, sizealt;
    uint32_t pos;
    static char *kwlist[] = {"chrom", "pos", "ref", "alt", NULL};
    if (!PyArg_ParseTupleAndKeywords(args, keywds, "s#Is#s#", kwlist, &chrom, &sizechrom, &pos, &ref, &sizeref, &alt, &sizealt))
        return NULL;
    uint64_t h = variantkey(chrom, (size_t)sizechrom, pos, ref, (size_t)sizeref, alt, (size_t)sizealt);
    return Py_BuildValue("K", h);
}

static PyObject* py_variantkey_range(PyObject *Py_UNUSED(ignored), PyObject *args, PyObject *keywds)
{
    PyObject *result;
    uint8_t chrom;
    uint32_t pos_min, pos_max;
    static char *kwlist[] = {"chrom", "pos_min", "pos_max", NULL};
    if (!PyArg_ParseTupleAndKeywords(args, keywds, "BII", kwlist, &chrom, &pos_min, &pos_max))
        return NULL;
    vkrange_t r;
    variantkey_range(chrom, pos_min, pos_max, &r);
    result = PyTuple_New(2);
    PyTuple_SetItem(result, 0, Py_BuildValue("K", r.min));
    PyTuple_SetItem(result, 1, Py_BuildValue("K", r.max));
    return result;
}

static PyObject* py_compare_variantkey_chrom(PyObject *Py_UNUSED(ignored), PyObject *args, PyObject *keywds)
{
    uint64_t vka, vkb;
    static char *kwlist[] = {"vka", "vkb", NULL};
    if (!PyArg_ParseTupleAndKeywords(args, keywds, "KK", kwlist, &vka, &vkb))
        return NULL;
    int cmp = compare_variantkey_chrom(vka, vkb);
    return Py_BuildValue("i", cmp);
}

static PyObject* py_compare_variantkey_chrom_pos(PyObject *Py_UNUSED(ignored), PyObject *args, PyObject *keywds)
{
    uint64_t vka, vkb;
    static char *kwlist[] = {"vka", "vkb", NULL};
    if (!PyArg_ParseTupleAndKeywords(args, keywds, "KK", kwlist, &vka, &vkb))
        return NULL;
    int cmp = compare_variantkey_chrom_pos(vka, vkb);
    return Py_BuildValue("i", cmp);
}

static PyObject* py_variantkey_hex(PyObject *Py_UNUSED(ignored), PyObject *args, PyObject *keywds)
{
    uint64_t code;
    static char *kwlist[] = {"code", NULL};
    if (!PyArg_ParseTupleAndKeywords(args, keywds, "K", kwlist, &code))
        return NULL;
    char str[17];
    variantkey_hex(code, str);
    return PyBytes_FromString(str);
}

static PyObject* py_parse_variantkey_hex(PyObject *Py_UNUSED(ignored), PyObject *args, PyObject *keywds)
{
    const char *vs;
    Py_ssize_t sizevs;
    static char *kwlist[] = {"vs", NULL};
    if (!PyArg_ParseTupleAndKeywords(args, keywds, "s#", kwlist, &vs, &sizevs))
        return NULL;
    uint64_t h = 0;
    if (sizevs == 16)
    {
        h = parse_variantkey_hex(vs);
    }
    return Py_BuildValue("K", h);
}

// --- BINSEARCH ---

static PyObject* py_mmap_binfile(PyObject *Py_UNUSED(ignored), PyObject *args, PyObject *keywds)
{
    PyObject *result;
    const char *file;
    static char *kwlist[] = {"file", NULL};
    if (!PyArg_ParseTupleAndKeywords(args, keywds, "s", kwlist, &file))
        return NULL;
    mmfile_t h;
    mmap_binfile(file, &h);
    result = PyTuple_New(4);
    PyTuple_SetItem(result, 0, PyCapsule_New((void*)h.src, "src", NULL));
    PyTuple_SetItem(result, 1, Py_BuildValue("i", h.fd));
    PyTuple_SetItem(result, 2, Py_BuildValue("K", h.size));
    PyTuple_SetItem(result, 3, Py_BuildValue("K", h.last));
    return result;
}

static PyObject* py_munmap_binfile(PyObject *Py_UNUSED(ignored), PyObject *args, PyObject *keywds)
{
    mmfile_t mf;
    PyObject *result;
    PyObject* mfsrc = NULL;
    static char *kwlist[] = {"mfsrc", "fd", "size", NULL};
    if (!PyArg_ParseTupleAndKeywords(args, keywds, "OiK", kwlist, &mfsrc, &mf.fd, &mf.size))
        return NULL;
    mf.src = (unsigned char *)PyCapsule_GetPointer(mfsrc, "src");
    int h = munmap_binfile(mf);
    result = Py_BuildValue("i", h);
    return result;
}

static PyObject* py_get_address(PyObject *Py_UNUSED(ignored), PyObject *args, PyObject *keywds)
{
    PyObject *result;
    uint64_t blklen, blkpos, item;
    static char *kwlist[] = {"blklen", "blkpos", "item", NULL};
    if (!PyArg_ParseTupleAndKeywords(args, keywds, "KKK", kwlist, &blklen, &blkpos, &item))
        return NULL;
    uint64_t h = get_address(blklen, blkpos, item);
    result = Py_BuildValue("K", h);
    return result;
}

// ----------

static PyObject* py_find_first_uint8(PyObject *Py_UNUSED(ignored), PyObject *args, PyObject *keywds)
{
    PyObject *result;
    uint64_t blklen, blkpos, first, last;
    uint8_t search;
    PyObject* mfsrc = NULL;
    static char *kwlist[] = {"mfsrc", "blklen", "blkpos", "first", "last", "search", NULL};
    if (!PyArg_ParseTupleAndKeywords(args, keywds, "OKKKKB", kwlist, &mfsrc, &blklen, &blkpos, &first, &last, &search))
        return NULL;
    const unsigned char *src = (const unsigned char *)PyCapsule_GetPointer(mfsrc, "src");
    uint64_t h = find_first_uint8_t(src, blklen, blkpos, &first, &last, search);
    result = PyTuple_New(3);
    PyTuple_SetItem(result, 0, Py_BuildValue("K", h));
    PyTuple_SetItem(result, 1, Py_BuildValue("K", first));
    PyTuple_SetItem(result, 2, Py_BuildValue("K", last));
    return result;
}

static PyObject* py_find_first_uint16(PyObject *Py_UNUSED(ignored), PyObject *args, PyObject *keywds)
{
    PyObject *result;
    uint64_t blklen, blkpos, first, last;
    uint16_t search;
    PyObject* mfsrc = NULL;
    static char *kwlist[] = {"mfsrc", "blklen", "blkpos", "first", "last", "search", NULL};
    if (!PyArg_ParseTupleAndKeywords(args, keywds, "OKKKKH", kwlist, &mfsrc, &blklen, &blkpos, &first, &last, &search))
        return NULL;
    const unsigned char *src = (const unsigned char *)PyCapsule_GetPointer(mfsrc, "src");
    uint64_t h = find_first_uint16_t(src, blklen, blkpos, &first, &last, search);
    result = PyTuple_New(3);
    PyTuple_SetItem(result, 0, Py_BuildValue("K", h));
    PyTuple_SetItem(result, 1, Py_BuildValue("K", first));
    PyTuple_SetItem(result, 2, Py_BuildValue("K", last));
    return result;
}

static PyObject* py_find_first_uint32(PyObject *Py_UNUSED(ignored), PyObject *args, PyObject *keywds)
{
    PyObject *result;
    uint64_t blklen, blkpos, first, last;
    uint32_t search;
    PyObject* mfsrc = NULL;
    static char *kwlist[] = {"mfsrc", "blklen", "blkpos", "first", "last", "search", NULL};
    if (!PyArg_ParseTupleAndKeywords(args, keywds, "OKKKKI", kwlist, &mfsrc, &blklen, &blkpos, &first, &last, &search))
        return NULL;
    const unsigned char *src = (const unsigned char *)PyCapsule_GetPointer(mfsrc, "src");
    uint64_t h = find_first_uint32_t(src, blklen, blkpos, &first, &last, search);
    result = PyTuple_New(3);
    PyTuple_SetItem(result, 0, Py_BuildValue("K", h));
    PyTuple_SetItem(result, 1, Py_BuildValue("K", first));
    PyTuple_SetItem(result, 2, Py_BuildValue("K", last));
    return result;
}

static PyObject* py_find_first_uint64(PyObject *Py_UNUSED(ignored), PyObject *args, PyObject *keywds)
{
    PyObject *result;
    uint64_t blklen, blkpos, first, last;
    uint64_t search;
    PyObject* mfsrc = NULL;
    static char *kwlist[] = {"mfsrc", "blklen", "blkpos", "first", "last", "search", NULL};
    if (!PyArg_ParseTupleAndKeywords(args, keywds, "OKKKKK", kwlist, &mfsrc, &blklen, &blkpos, &first, &last, &search))
        return NULL;
    const unsigned char *src = (const unsigned char *)PyCapsule_GetPointer(mfsrc, "src");
    uint64_t h = find_first_uint64_t(src, blklen, blkpos, &first, &last, search);
    result = PyTuple_New(3);
    PyTuple_SetItem(result, 0, Py_BuildValue("K", h));
    PyTuple_SetItem(result, 1, Py_BuildValue("K", first));
    PyTuple_SetItem(result, 2, Py_BuildValue("K", last));
    return result;
}

// ----------

static PyObject* py_find_last_uint8(PyObject *Py_UNUSED(ignored), PyObject *args, PyObject *keywds)
{
    PyObject *result;
    uint64_t blklen, blkpos, first, last;
    uint8_t search;
    PyObject* mfsrc = NULL;
    static char *kwlist[] = {"mfsrc", "blklen", "blkpos", "first", "last", "search", NULL};
    if (!PyArg_ParseTupleAndKeywords(args, keywds, "OKKKKB", kwlist, &mfsrc, &blklen, &blkpos, &first, &last, &search))
        return NULL;
    const unsigned char *src = (const unsigned char *)PyCapsule_GetPointer(mfsrc, "src");
    uint64_t h = find_last_uint8_t(src, blklen, blkpos, &first, &last, search);
    result = PyTuple_New(3);
    PyTuple_SetItem(result, 0, Py_BuildValue("K", h));
    PyTuple_SetItem(result, 1, Py_BuildValue("K", first));
    PyTuple_SetItem(result, 2, Py_BuildValue("K", last));
    return result;
}

static PyObject* py_find_last_uint16(PyObject *Py_UNUSED(ignored), PyObject *args, PyObject *keywds)
{
    PyObject *result;
    uint64_t blklen, blkpos, first, last;
    uint16_t search;
    PyObject* mfsrc = NULL;
    static char *kwlist[] = {"mfsrc", "blklen", "blkpos", "first", "last", "search", NULL};
    if (!PyArg_ParseTupleAndKeywords(args, keywds, "OKKKKH", kwlist, &mfsrc, &blklen, &blkpos, &first, &last, &search))
        return NULL;
    const unsigned char *src = (const unsigned char *)PyCapsule_GetPointer(mfsrc, "src");
    uint64_t h = find_last_uint16_t(src, blklen, blkpos, &first, &last, search);
    result = PyTuple_New(3);
    PyTuple_SetItem(result, 0, Py_BuildValue("K", h));
    PyTuple_SetItem(result, 1, Py_BuildValue("K", first));
    PyTuple_SetItem(result, 2, Py_BuildValue("K", last));
    return result;
}

static PyObject* py_find_last_uint32(PyObject *Py_UNUSED(ignored), PyObject *args, PyObject *keywds)
{
    PyObject *result;
    uint64_t blklen, blkpos, first, last;
    uint32_t search;
    PyObject* mfsrc = NULL;
    static char *kwlist[] = {"mfsrc", "blklen", "blkpos", "first", "last", "search", NULL};
    if (!PyArg_ParseTupleAndKeywords(args, keywds, "OKKKKI", kwlist, &mfsrc, &blklen, &blkpos, &first, &last, &search))
        return NULL;
    const unsigned char *src = (const unsigned char *)PyCapsule_GetPointer(mfsrc, "src");
    uint64_t h = find_last_uint32_t(src, blklen, blkpos, &first, &last, search);
    result = PyTuple_New(3);
    PyTuple_SetItem(result, 0, Py_BuildValue("K", h));
    PyTuple_SetItem(result, 1, Py_BuildValue("K", first));
    PyTuple_SetItem(result, 2, Py_BuildValue("K", last));
    return result;
}

static PyObject* py_find_last_uint64(PyObject *Py_UNUSED(ignored), PyObject *args, PyObject *keywds)
{
    PyObject *result;
    uint64_t blklen, blkpos, first, last;
    uint64_t search;
    PyObject* mfsrc = NULL;
    static char *kwlist[] = {"mfsrc", "blklen", "blkpos", "first", "last", "search", NULL};
    if (!PyArg_ParseTupleAndKeywords(args, keywds, "OKKKKK", kwlist, &mfsrc, &blklen, &blkpos, &first, &last, &search))
        return NULL;
    const unsigned char *src = (const unsigned char *)PyCapsule_GetPointer(mfsrc, "src");
    uint64_t h = find_last_uint64_t(src, blklen, blkpos, &first, &last, search);
    result = PyTuple_New(3);
    PyTuple_SetItem(result, 0, Py_BuildValue("K", h));
    PyTuple_SetItem(result, 1, Py_BuildValue("K", first));
    PyTuple_SetItem(result, 2, Py_BuildValue("K", last));
    return result;
}

// ----------

static PyObject* py_find_first_sub_uint8(PyObject *Py_UNUSED(ignored), PyObject *args, PyObject *keywds)
{
    PyObject *result;
    uint64_t blklen, blkpos, first, last;
    uint8_t bitstart, bitend;
    uint8_t search;
    PyObject* mfsrc = NULL;
    static char *kwlist[] = {"mfsrc", "blklen", "blkpos", "bitstart", "bitend", "first", "last", "search", NULL};
    if (!PyArg_ParseTupleAndKeywords(args, keywds, "OKKBBKKB", kwlist, &mfsrc, &blklen, &blkpos, &bitstart, &bitend, &first, &last, &search))
        return NULL;
    const unsigned char *src = (const unsigned char *)PyCapsule_GetPointer(mfsrc, "src");
    uint64_t h = find_first_sub_uint8_t(src, blklen, blkpos, bitstart, bitend, &first, &last, search);
    result = PyTuple_New(3);
    PyTuple_SetItem(result, 0, Py_BuildValue("K", h));
    PyTuple_SetItem(result, 1, Py_BuildValue("K", first));
    PyTuple_SetItem(result, 2, Py_BuildValue("K", last));
    return result;
}

static PyObject* py_find_first_sub_uint16(PyObject *Py_UNUSED(ignored), PyObject *args, PyObject *keywds)
{
    PyObject *result;
    uint64_t blklen, blkpos, first, last;
    uint8_t bitstart, bitend;
    uint16_t search;
    PyObject* mfsrc = NULL;
    static char *kwlist[] = {"mfsrc", "blklen", "blkpos", "bitstart", "bitend", "first", "last", "search", NULL};
    if (!PyArg_ParseTupleAndKeywords(args, keywds, "OKKBBKKH", kwlist, &mfsrc, &blklen, &blkpos, &bitstart, &bitend, &first, &last, &search))
        return NULL;
    const unsigned char *src = (const unsigned char *)PyCapsule_GetPointer(mfsrc, "src");
    uint64_t h = find_first_sub_uint16_t(src, blklen, blkpos, bitstart, bitend, &first, &last, search);
    result = PyTuple_New(3);
    PyTuple_SetItem(result, 0, Py_BuildValue("K", h));
    PyTuple_SetItem(result, 1, Py_BuildValue("K", first));
    PyTuple_SetItem(result, 2, Py_BuildValue("K", last));
    return result;
}

static PyObject* py_find_first_sub_uint32(PyObject *Py_UNUSED(ignored), PyObject *args, PyObject *keywds)
{
    PyObject *result;
    uint64_t blklen, blkpos, first, last;
    uint8_t bitstart, bitend;
    uint32_t search;
    PyObject* mfsrc = NULL;
    static char *kwlist[] = {"mfsrc", "blklen", "blkpos", "bitstart", "bitend", "first", "last", "search", NULL};
    if (!PyArg_ParseTupleAndKeywords(args, keywds, "OKKBBKKI", kwlist, &mfsrc, &blklen, &blkpos, &bitstart, &bitend, &first, &last, &search))
        return NULL;
    const unsigned char *src = (const unsigned char *)PyCapsule_GetPointer(mfsrc, "src");
    uint64_t h = find_first_sub_uint32_t(src, blklen, blkpos, bitstart, bitend, &first, &last, search);
    result = PyTuple_New(3);
    PyTuple_SetItem(result, 0, Py_BuildValue("K", h));
    PyTuple_SetItem(result, 1, Py_BuildValue("K", first));
    PyTuple_SetItem(result, 2, Py_BuildValue("K", last));
    return result;
}

static PyObject* py_find_first_sub_uint64(PyObject *Py_UNUSED(ignored), PyObject *args, PyObject *keywds)
{
    PyObject *result;
    uint64_t blklen, blkpos, first, last;
    uint8_t bitstart, bitend;
    uint64_t search;
    PyObject* mfsrc = NULL;
    static char *kwlist[] = {"mfsrc", "blklen", "blkpos", "bitstart", "bitend", "first", "last", "search", NULL};
    if (!PyArg_ParseTupleAndKeywords(args, keywds, "OKKBBKKK", kwlist, &mfsrc, &blklen, &blkpos, &bitstart, &bitend, &first, &last, &search))
        return NULL;
    const unsigned char *src = (const unsigned char *)PyCapsule_GetPointer(mfsrc, "src");
    uint64_t h = find_first_sub_uint64_t(src, blklen, blkpos, bitstart, bitend, &first, &last, search);
    result = PyTuple_New(3);
    PyTuple_SetItem(result, 0, Py_BuildValue("K", h));
    PyTuple_SetItem(result, 1, Py_BuildValue("K", first));
    PyTuple_SetItem(result, 2, Py_BuildValue("K", last));
    return result;
}

// ----------

static PyObject* py_find_last_sub_uint8(PyObject *Py_UNUSED(ignored), PyObject *args, PyObject *keywds)
{
    PyObject *result;
    uint64_t blklen, blkpos, first, last;
    uint8_t bitstart, bitend;
    uint8_t search;
    PyObject* mfsrc = NULL;
    static char *kwlist[] = {"mfsrc", "blklen", "blkpos", "bitstart", "bitend", "first", "last", "search", NULL};
    if (!PyArg_ParseTupleAndKeywords(args, keywds, "OKKBBKKB", kwlist, &mfsrc, &blklen, &blkpos, &bitstart, &bitend, &first, &last, &search))
        return NULL;
    const unsigned char *src = (const unsigned char *)PyCapsule_GetPointer(mfsrc, "src");
    uint64_t h = find_last_sub_uint8_t(src, blklen, blkpos, bitstart, bitend, &first, &last, search);
    result = PyTuple_New(3);
    PyTuple_SetItem(result, 0, Py_BuildValue("K", h));
    PyTuple_SetItem(result, 1, Py_BuildValue("K", first));
    PyTuple_SetItem(result, 2, Py_BuildValue("K", last));
    return result;
}

static PyObject* py_find_last_sub_uint16(PyObject *Py_UNUSED(ignored), PyObject *args, PyObject *keywds)
{
    PyObject *result;
    uint64_t blklen, blkpos, first, last;
    uint8_t bitstart, bitend;
    uint16_t search;
    PyObject* mfsrc = NULL;
    static char *kwlist[] = {"mfsrc", "blklen", "blkpos", "bitstart", "bitend", "first", "last", "search", NULL};
    if (!PyArg_ParseTupleAndKeywords(args, keywds, "OKKBBKKH", kwlist, &mfsrc, &blklen, &blkpos, &bitstart, &bitend, &first, &last, &search))
        return NULL;
    const unsigned char *src = (const unsigned char *)PyCapsule_GetPointer(mfsrc, "src");
    uint64_t h = find_last_sub_uint16_t(src, blklen, blkpos, bitstart, bitend, &first, &last, search);
    result = PyTuple_New(3);
    PyTuple_SetItem(result, 0, Py_BuildValue("K", h));
    PyTuple_SetItem(result, 1, Py_BuildValue("K", first));
    PyTuple_SetItem(result, 2, Py_BuildValue("K", last));
    return result;
}

static PyObject* py_find_last_sub_uint32(PyObject *Py_UNUSED(ignored), PyObject *args, PyObject *keywds)
{
    PyObject *result;
    uint64_t blklen, blkpos, first, last;
    uint8_t bitstart, bitend;
    uint32_t search;
    PyObject* mfsrc = NULL;
    static char *kwlist[] = {"mfsrc", "blklen", "blkpos", "bitstart", "bitend", "first", "last", "search", NULL};
    if (!PyArg_ParseTupleAndKeywords(args, keywds, "OKKBBKKI", kwlist, &mfsrc, &blklen, &blkpos, &bitstart, &bitend, &first, &last, &search))
        return NULL;
    const unsigned char *src = (const unsigned char *)PyCapsule_GetPointer(mfsrc, "src");
    uint64_t h = find_last_sub_uint32_t(src, blklen, blkpos, bitstart, bitend, &first, &last, search);
    result = PyTuple_New(3);
    PyTuple_SetItem(result, 0, Py_BuildValue("K", h));
    PyTuple_SetItem(result, 1, Py_BuildValue("K", first));
    PyTuple_SetItem(result, 2, Py_BuildValue("K", last));
    return result;
}

static PyObject* py_find_last_sub_uint64(PyObject *Py_UNUSED(ignored), PyObject *args, PyObject *keywds)
{
    PyObject *result;
    uint64_t blklen, blkpos, first, last;
    uint8_t bitstart, bitend;
    uint64_t search;
    PyObject* mfsrc = NULL;
    static char *kwlist[] = {"mfsrc", "blklen", "blkpos", "bitstart", "bitend", "first", "last", "search", NULL};
    if (!PyArg_ParseTupleAndKeywords(args, keywds, "OKKBBKKK", kwlist, &mfsrc, &blklen, &blkpos, &bitstart, &bitend, &first, &last, &search))
        return NULL;
    const unsigned char *src = (const unsigned char *)PyCapsule_GetPointer(mfsrc, "src");
    uint64_t h = find_last_sub_uint64_t(src, blklen, blkpos, bitstart, bitend, &first, &last, search);
    result = PyTuple_New(3);
    PyTuple_SetItem(result, 0, Py_BuildValue("K", h));
    PyTuple_SetItem(result, 1, Py_BuildValue("K", first));
    PyTuple_SetItem(result, 2, Py_BuildValue("K", last));
    return result;
}

// ----------

static PyObject* py_has_next_uint8(PyObject *Py_UNUSED(ignored), PyObject *args, PyObject *keywds)
{
    PyObject *result;
    uint64_t blklen, blkpos, pos, last;
    uint8_t search;
    PyObject* mfsrc = NULL;
    static char *kwlist[] = {"mfsrc", "blklen", "blkpos", "pos", "last", "search", NULL};
    if (!PyArg_ParseTupleAndKeywords(args, keywds, "OKKKKB", kwlist, &mfsrc, &blklen, &blkpos, &pos, &last, &search))
        return NULL;
    const unsigned char *src = (const unsigned char *)PyCapsule_GetPointer(mfsrc, "src");
    bool h = has_next_uint8_t(src, blklen, blkpos, &pos, last, search);
    result = PyTuple_New(2);
    PyTuple_SetItem(result, 0, Py_BuildValue("i", h));
    PyTuple_SetItem(result, 1, Py_BuildValue("K", pos));
    return result;
}

static PyObject* py_has_next_uint16(PyObject *Py_UNUSED(ignored), PyObject *args, PyObject *keywds)
{
    PyObject *result;
    uint64_t blklen, blkpos, pos, last;
    uint16_t search;
    PyObject* mfsrc = NULL;
    static char *kwlist[] = {"mfsrc", "blklen", "blkpos", "pos", "last", "search", NULL};
    if (!PyArg_ParseTupleAndKeywords(args, keywds, "OKKKKH", kwlist, &mfsrc, &blklen, &blkpos, &pos, &last, &search))
        return NULL;
    const unsigned char *src = (const unsigned char *)PyCapsule_GetPointer(mfsrc, "src");
    bool h = has_next_uint16_t(src, blklen, blkpos, &pos, last, search);
    result = PyTuple_New(2);
    PyTuple_SetItem(result, 0, Py_BuildValue("i", h));
    PyTuple_SetItem(result, 1, Py_BuildValue("K", pos));
    return result;
}

static PyObject* py_has_next_uint32(PyObject *Py_UNUSED(ignored), PyObject *args, PyObject *keywds)
{
    PyObject *result;
    uint64_t blklen, blkpos, pos, last;
    uint32_t search;
    PyObject* mfsrc = NULL;
    static char *kwlist[] = {"mfsrc", "blklen", "blkpos", "pos", "last", "search", NULL};
    if (!PyArg_ParseTupleAndKeywords(args, keywds, "OKKKKI", kwlist, &mfsrc, &blklen, &blkpos, &pos, &last, &search))
        return NULL;
    const unsigned char *src = (const unsigned char *)PyCapsule_GetPointer(mfsrc, "src");
    bool h = has_next_uint32_t(src, blklen, blkpos, &pos, last, search);
    result = PyTuple_New(2);
    PyTuple_SetItem(result, 0, Py_BuildValue("i", h));
    PyTuple_SetItem(result, 1, Py_BuildValue("K", pos));
    return result;
}

static PyObject* py_has_next_uint64(PyObject *Py_UNUSED(ignored), PyObject *args, PyObject *keywds)
{
    PyObject *result;
    uint64_t blklen, blkpos, pos, last;
    uint64_t search;
    PyObject* mfsrc = NULL;
    static char *kwlist[] = {"mfsrc", "blklen", "blkpos", "pos", "last", "search", NULL};
    if (!PyArg_ParseTupleAndKeywords(args, keywds, "OKKKKK", kwlist, &mfsrc, &blklen, &blkpos, &pos, &last, &search))
        return NULL;
    const unsigned char *src = (const unsigned char *)PyCapsule_GetPointer(mfsrc, "src");
    bool h = has_next_uint64_t(src, blklen, blkpos, &pos, last, search);
    result = PyTuple_New(2);
    PyTuple_SetItem(result, 0, Py_BuildValue("i", h));
    PyTuple_SetItem(result, 1, Py_BuildValue("K", pos));
    return result;
}

// ----------

static PyObject* py_has_next_sub_uint8(PyObject *Py_UNUSED(ignored), PyObject *args, PyObject *keywds)
{
    PyObject *result;
    uint64_t blklen, blkpos, pos, last;
    uint8_t bitstart, bitend;
    uint8_t search;
    PyObject* mfsrc = NULL;
    static char *kwlist[] = {"mfsrc", "blklen", "blkpos", "bitstart", "bitend", "pos", "last", "search", NULL};
    if (!PyArg_ParseTupleAndKeywords(args, keywds, "OKKBBKKB", kwlist, &mfsrc, &blklen, &blkpos, &bitstart, &bitend, &pos, &last, &search))
        return NULL;
    const unsigned char *src = (const unsigned char *)PyCapsule_GetPointer(mfsrc, "src");
    bool h = has_next_sub_uint8_t(src, blklen, blkpos, bitstart, bitend, &pos, last, search);
    result = PyTuple_New(2);
    PyTuple_SetItem(result, 0, Py_BuildValue("i", h));
    PyTuple_SetItem(result, 1, Py_BuildValue("K", pos));
    return result;
}

static PyObject* py_has_next_sub_uint16(PyObject *Py_UNUSED(ignored), PyObject *args, PyObject *keywds)
{
    PyObject *result;
    uint64_t blklen, blkpos, pos, last;
    uint8_t bitstart, bitend;
    uint16_t search;
    PyObject* mfsrc = NULL;
    static char *kwlist[] = {"mfsrc", "blklen", "blkpos", "bitstart", "bitend", "pos", "last", "search", NULL};
    if (!PyArg_ParseTupleAndKeywords(args, keywds, "OKKBBKKH", kwlist, &mfsrc, &blklen, &blkpos, &bitstart, &bitend, &pos, &last, &search))
        return NULL;
    const unsigned char *src = (const unsigned char *)PyCapsule_GetPointer(mfsrc, "src");
    bool h = has_next_sub_uint16_t(src, blklen, blkpos, bitstart, bitend, &pos, last, search);
    result = PyTuple_New(2);
    PyTuple_SetItem(result, 0, Py_BuildValue("i", h));
    PyTuple_SetItem(result, 1, Py_BuildValue("K", pos));
    return result;
}

static PyObject* py_has_next_sub_uint32(PyObject *Py_UNUSED(ignored), PyObject *args, PyObject *keywds)
{
    PyObject *result;
    uint64_t blklen, blkpos, pos, last;
    uint8_t bitstart, bitend;
    uint32_t search;
    PyObject* mfsrc = NULL;
    static char *kwlist[] = {"mfsrc", "blklen", "blkpos", "bitstart", "bitend", "pos", "last", "search", NULL};
    if (!PyArg_ParseTupleAndKeywords(args, keywds, "OKKBBKKI", kwlist, &mfsrc, &blklen, &blkpos, &bitstart, &bitend, &pos, &last, &search))
        return NULL;
    const unsigned char *src = (const unsigned char *)PyCapsule_GetPointer(mfsrc, "src");
    bool h = has_next_sub_uint32_t(src, blklen, blkpos, bitstart, bitend, &pos, last, search);
    result = PyTuple_New(2);
    PyTuple_SetItem(result, 0, Py_BuildValue("i", h));
    PyTuple_SetItem(result, 1, Py_BuildValue("K", pos));
    return result;
}

static PyObject* py_has_next_sub_uint64(PyObject *Py_UNUSED(ignored), PyObject *args, PyObject *keywds)
{
    PyObject *result;
    uint64_t blklen, blkpos, pos, last;
    uint8_t bitstart, bitend;
    uint64_t search;
    PyObject* mfsrc = NULL;
    static char *kwlist[] = {"mfsrc", "blklen", "blkpos", "bitstart", "bitend", "pos", "last", "search", NULL};
    if (!PyArg_ParseTupleAndKeywords(args, keywds, "OKKBBKKK", kwlist, &mfsrc, &blklen, &blkpos, &bitstart, &bitend, &pos, &last, &search))
        return NULL;
    const unsigned char *src = (const unsigned char *)PyCapsule_GetPointer(mfsrc, "src");
    bool h = has_next_sub_uint64_t(src, blklen, blkpos, bitstart, bitend, &pos, last, search);
    result = PyTuple_New(2);
    PyTuple_SetItem(result, 0, Py_BuildValue("i", h));
    PyTuple_SetItem(result, 1, Py_BuildValue("K", pos));
    return result;
}

// ----------

static PyObject* py_has_prev_uint8(PyObject *Py_UNUSED(ignored), PyObject *args, PyObject *keywds)
{
    PyObject *result;
    uint64_t blklen, blkpos, first, pos;
    uint8_t search;
    PyObject* mfsrc = NULL;
    static char *kwlist[] = {"mfsrc", "blklen", "blkpos", "first", "pos", "search", NULL};
    if (!PyArg_ParseTupleAndKeywords(args, keywds, "OKKKKB", kwlist, &mfsrc, &blklen, &blkpos, &first, &pos, &search))
        return NULL;
    const unsigned char *src = (const unsigned char *)PyCapsule_GetPointer(mfsrc, "src");
    bool h = has_prev_uint8_t(src, blklen, blkpos, first, &pos, search);
    result = PyTuple_New(2);
    PyTuple_SetItem(result, 0, Py_BuildValue("i", h));
    PyTuple_SetItem(result, 1, Py_BuildValue("K", pos));
    return result;
}

static PyObject* py_has_prev_uint16(PyObject *Py_UNUSED(ignored), PyObject *args, PyObject *keywds)
{
    PyObject *result;
    uint64_t blklen, blkpos, first, pos;
    uint16_t search;
    PyObject* mfsrc = NULL;
    static char *kwlist[] = {"mfsrc", "blklen", "blkpos", "first", "pos", "search", NULL};
    if (!PyArg_ParseTupleAndKeywords(args, keywds, "OKKKKH", kwlist, &mfsrc, &blklen, &blkpos, &first, &pos, &search))
        return NULL;
    const unsigned char *src = (const unsigned char *)PyCapsule_GetPointer(mfsrc, "src");
    bool h = has_prev_uint16_t(src, blklen, blkpos, first, &pos, search);
    result = PyTuple_New(2);
    PyTuple_SetItem(result, 0, Py_BuildValue("i", h));
    PyTuple_SetItem(result, 1, Py_BuildValue("K", pos));
    return result;
}

static PyObject* py_has_prev_uint32(PyObject *Py_UNUSED(ignored), PyObject *args, PyObject *keywds)
{
    PyObject *result;
    uint64_t blklen, blkpos, first, pos;
    uint32_t search;
    PyObject* mfsrc = NULL;
    static char *kwlist[] = {"mfsrc", "blklen", "blkpos", "first", "pos", "search", NULL};
    if (!PyArg_ParseTupleAndKeywords(args, keywds, "OKKKKI", kwlist, &mfsrc, &blklen, &blkpos, &first, &pos, &search))
        return NULL;
    const unsigned char *src = (const unsigned char *)PyCapsule_GetPointer(mfsrc, "src");
    bool h = has_prev_uint32_t(src, blklen, blkpos, first, &pos, search);
    result = PyTuple_New(2);
    PyTuple_SetItem(result, 0, Py_BuildValue("i", h));
    PyTuple_SetItem(result, 1, Py_BuildValue("K", pos));
    return result;
}

static PyObject* py_has_prev_uint64(PyObject *Py_UNUSED(ignored), PyObject *args, PyObject *keywds)
{
    PyObject *result;
    uint64_t blklen, blkpos, first, pos;
    uint64_t search;
    PyObject* mfsrc = NULL;
    static char *kwlist[] = {"mfsrc", "blklen", "blkpos", "first", "pos", "search", NULL};
    if (!PyArg_ParseTupleAndKeywords(args, keywds, "OKKKKK", kwlist, &mfsrc, &blklen, &blkpos, &first, &pos, &search))
        return NULL;
    const unsigned char *src = (const unsigned char *)PyCapsule_GetPointer(mfsrc, "src");
    bool h = has_prev_uint64_t(src, blklen, blkpos, first, &pos, search);
    result = PyTuple_New(2);
    PyTuple_SetItem(result, 0, Py_BuildValue("i", h));
    PyTuple_SetItem(result, 1, Py_BuildValue("K", pos));
    return result;
}

// ----------

static PyObject* py_has_prev_sub_uint8(PyObject *Py_UNUSED(ignored), PyObject *args, PyObject *keywds)
{
    PyObject *result;
    uint64_t blklen, blkpos, first, pos;
    uint8_t bitstart, bitend;
    uint8_t search;
    PyObject* mfsrc = NULL;
    static char *kwlist[] = {"mfsrc", "blklen", "blkpos", "bitstart", "bitend", "first", "pos", "search", NULL};
    if (!PyArg_ParseTupleAndKeywords(args, keywds, "OKKBBKKB", kwlist, &mfsrc, &blklen, &blkpos, &bitstart, &bitend, &first, &pos, &search))
        return NULL;
    const unsigned char *src = (const unsigned char *)PyCapsule_GetPointer(mfsrc, "src");
    bool h = has_prev_sub_uint8_t(src, blklen, blkpos, bitstart, bitend, first, &pos, search);
    result = PyTuple_New(2);
    PyTuple_SetItem(result, 0, Py_BuildValue("i", h));
    PyTuple_SetItem(result, 1, Py_BuildValue("K", pos));
    return result;
}

static PyObject* py_has_prev_sub_uint16(PyObject *Py_UNUSED(ignored), PyObject *args, PyObject *keywds)
{
    PyObject *result;
    uint64_t blklen, blkpos, first, pos;
    uint8_t bitstart, bitend;
    uint16_t search;
    PyObject* mfsrc = NULL;
    static char *kwlist[] = {"mfsrc", "blklen", "blkpos", "bitstart", "bitend", "first", "pos", "search", NULL};
    if (!PyArg_ParseTupleAndKeywords(args, keywds, "OKKBBKKH", kwlist, &mfsrc, &blklen, &blkpos, &bitstart, &bitend, &first, &pos, &search))
        return NULL;
    const unsigned char *src = (const unsigned char *)PyCapsule_GetPointer(mfsrc, "src");
    bool h = has_prev_sub_uint16_t(src, blklen, blkpos, bitstart, bitend, first, &pos, search);
    result = PyTuple_New(2);
    PyTuple_SetItem(result, 0, Py_BuildValue("i", h));
    PyTuple_SetItem(result, 1, Py_BuildValue("K", pos));
    return result;
}

static PyObject* py_has_prev_sub_uint32(PyObject *Py_UNUSED(ignored), PyObject *args, PyObject *keywds)
{
    PyObject *result;
    uint64_t blklen, blkpos, first, pos;
    uint8_t bitstart, bitend;
    uint32_t search;
    PyObject* mfsrc = NULL;
    static char *kwlist[] = {"mfsrc", "blklen", "blkpos", "bitstart", "bitend", "first", "pos", "search", NULL};
    if (!PyArg_ParseTupleAndKeywords(args, keywds, "OKKBBKKI", kwlist, &mfsrc, &blklen, &blkpos, &bitstart, &bitend, &first, &pos, &search))
        return NULL;
    const unsigned char *src = (const unsigned char *)PyCapsule_GetPointer(mfsrc, "src");
    bool h = has_prev_sub_uint32_t(src, blklen, blkpos, bitstart, bitend, first, &pos, search);
    result = PyTuple_New(2);
    PyTuple_SetItem(result, 0, Py_BuildValue("i", h));
    PyTuple_SetItem(result, 1, Py_BuildValue("K", pos));
    return result;
}

static PyObject* py_has_prev_sub_uint64(PyObject *Py_UNUSED(ignored), PyObject *args, PyObject *keywds)
{
    PyObject *result;
    uint64_t blklen, blkpos, first, pos;
    uint8_t bitstart, bitend;
    uint64_t search;
    PyObject* mfsrc = NULL;
    static char *kwlist[] = {"mfsrc", "blklen", "blkpos", "bitstart", "bitend", "first", "pos", "search", NULL};
    if (!PyArg_ParseTupleAndKeywords(args, keywds, "OKKBBKKK", kwlist, &mfsrc, &blklen, &blkpos, &bitstart, &bitend, &first, &pos, &search))
        return NULL;
    const unsigned char *src = (const unsigned char *)PyCapsule_GetPointer(mfsrc, "src");
    bool h = has_prev_sub_uint64_t(src, blklen, blkpos, bitstart, bitend, first, &pos, search);
    result = PyTuple_New(2);
    PyTuple_SetItem(result, 0, Py_BuildValue("i", h));
    PyTuple_SetItem(result, 1, Py_BuildValue("K", pos));
    return result;
}

// ----------

// --- RSIDVAR ---

static PyObject* py_get_vr_rsid(PyObject *Py_UNUSED(ignored), PyObject *args, PyObject *keywds)
{
    uint64_t item;
    PyObject* mfsrc = NULL;
    static char *kwlist[] = {"mfsrc", "item", NULL};
    if (!PyArg_ParseTupleAndKeywords(args, keywds, "OK", kwlist, &mfsrc, &item))
        return NULL;
    const unsigned char *src = (const unsigned char *)PyCapsule_GetPointer(mfsrc, "src");
    uint32_t h = get_vr_rsid(src, item);
    return Py_BuildValue("I", h);
}

static PyObject* py_get_rv_variantkey(PyObject *Py_UNUSED(ignored), PyObject *args, PyObject *keywds)
{
    uint64_t item;
    PyObject* mfsrc = NULL;
    static char *kwlist[] = {"mfsrc", "item", NULL};
    if (!PyArg_ParseTupleAndKeywords(args, keywds, "OK", kwlist, &mfsrc, &item))
        return NULL;
    const unsigned char *src = (const unsigned char *)PyCapsule_GetPointer(mfsrc, "src");
    uint64_t h = get_rv_variantkey(src, item);
    return Py_BuildValue("K", h);
}

static PyObject* py_find_rv_variantkey_by_rsid(PyObject *Py_UNUSED(ignored), PyObject *args, PyObject *keywds)
{
    PyObject *result;
    uint64_t first, last;
    uint32_t rsid;
    PyObject* mfsrc = NULL;
    static char *kwlist[] = {"mfsrc", "first", "last", "rsid", NULL};
    if (!PyArg_ParseTupleAndKeywords(args, keywds, "OKKI", kwlist, &mfsrc, &first, &last, &rsid))
        return NULL;
    const unsigned char *src = (const unsigned char *)PyCapsule_GetPointer(mfsrc, "src");
    uint64_t h = find_rv_variantkey_by_rsid(src, &first, last, rsid);
    result = PyTuple_New(2);
    PyTuple_SetItem(result, 0, Py_BuildValue("K", h));
    PyTuple_SetItem(result, 1, Py_BuildValue("K", first));
    return result;
}

static PyObject* py_get_next_rv_variantkey_by_rsid(PyObject *Py_UNUSED(ignored), PyObject *args, PyObject *keywds)
{
    PyObject *result;
    uint64_t pos, last;
    uint32_t rsid;
    PyObject* mfsrc = NULL;
    static char *kwlist[] = {"mfsrc", "pos", "last", "rsid", NULL};
    if (!PyArg_ParseTupleAndKeywords(args, keywds, "OKKI", kwlist, &mfsrc, &pos, &last, &rsid))
        return NULL;
    const unsigned char *src = (const unsigned char *)PyCapsule_GetPointer(mfsrc, "src");
    uint64_t h = get_next_rv_variantkey_by_rsid(src, &pos, last, rsid);
    result = PyTuple_New(2);
    PyTuple_SetItem(result, 0, Py_BuildValue("K", h));
    PyTuple_SetItem(result, 1, Py_BuildValue("K", pos));
    return result;
}

static PyObject* py_find_all_rv_variantkey_by_rsid(PyObject *Py_UNUSED(ignored), PyObject *args, PyObject *keywds)
{
    PyObject* vks = PyList_New(0);
    uint64_t first, last;
    uint32_t rsid;
    PyObject* mfsrc = NULL;
    static char *kwlist[] = {"mfsrc", "first", "last", "rsid", NULL};
    if (!PyArg_ParseTupleAndKeywords(args, keywds, "OKKI", kwlist, &mfsrc, &first, &last, &rsid))
        return NULL;
    const unsigned char *src = (const unsigned char *)PyCapsule_GetPointer(mfsrc, "src");
    uint64_t h = find_rv_variantkey_by_rsid(src, &first, last, rsid);
    while (h > 0)
    {
        PyList_Append(vks, Py_BuildValue("K", h));
        h = get_next_rv_variantkey_by_rsid(src, &first, last, rsid);
    }
    return vks;
}

static PyObject* py_find_vr_rsid_by_variantkey(PyObject *Py_UNUSED(ignored), PyObject *args, PyObject *keywds)
{
    PyObject *result;
    uint64_t first, last, vk;
    PyObject* mfsrc = NULL;
    static char *kwlist[] = {"mfsrc", "first", "last", "vk", NULL};
    if (!PyArg_ParseTupleAndKeywords(args, keywds, "OKKK", kwlist, &mfsrc, &first, &last, &vk))
        return NULL;
    const unsigned char *src = (const unsigned char *)PyCapsule_GetPointer(mfsrc, "src");
    uint32_t h = find_vr_rsid_by_variantkey(src, &first, last, vk);
    result = PyTuple_New(2);
    PyTuple_SetItem(result, 0, Py_BuildValue("I", h));
    PyTuple_SetItem(result, 1, Py_BuildValue("K", first));
    return result;
}

static PyObject* py_find_vr_chrompos_range(PyObject *Py_UNUSED(ignored), PyObject *args, PyObject *keywds)
{
    PyObject *result;
    uint64_t first, last;
    uint8_t chrom;
    uint32_t pos_min, pos_max;
    PyObject* mfsrc = NULL;
    static char *kwlist[] = {"mfsrc", "first", "last", "chrom", "pos_min", "pos_max", NULL};
    if (!PyArg_ParseTupleAndKeywords(args, keywds, "OKKBII", kwlist, &mfsrc, &first, &last, &chrom, &pos_min, &pos_max))
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

static PyObject* py_find_ref_alt_by_variantkey(PyObject *Py_UNUSED(ignored), PyObject *args, PyObject *keywds)
{
    PyObject *result;
    uint64_t last, vk;
    PyObject* mfsrc = NULL;
    static char *kwlist[] = {"mfsrc", "last", "vk", NULL};
    if (!PyArg_ParseTupleAndKeywords(args, keywds, "OKK", kwlist, &mfsrc, &last, &vk))
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

static PyObject* py_reverse_variantkey(PyObject *Py_UNUSED(ignored), PyObject *args, PyObject *keywds)
{
    PyObject *result;
    uint64_t last, vk;
    PyObject* mfsrc = NULL;
    static char *kwlist[] = {"mfsrc", "last", "vk", NULL};
    if (!PyArg_ParseTupleAndKeywords(args, keywds, "OKK", kwlist, &mfsrc, &last, &vk))
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

static PyObject* py_get_ref_len_by_variantkey(PyObject *Py_UNUSED(ignored), PyObject *args, PyObject *keywds)
{
    uint64_t last, vk;
    PyObject* mfsrc = NULL;
    static char *kwlist[] = {"mfsrc", "last", "vk", NULL};
    if (!PyArg_ParseTupleAndKeywords(args, keywds, "OKK", kwlist, &mfsrc, &last, &vk))
        return NULL;
    const unsigned char *src = (const unsigned char *)PyCapsule_GetPointer(mfsrc, "src");
    size_t len = get_ref_len_by_variantkey(src, last, vk);
    return Py_BuildValue("K", len);
}

static PyObject* py_get_variantkey_endpos(PyObject *Py_UNUSED(ignored), PyObject *args, PyObject *keywds)
{
    uint64_t last, vk;
    PyObject* mfsrc = NULL;
    static char *kwlist[] = {"mfsrc", "last", "vk", NULL};
    if (!PyArg_ParseTupleAndKeywords(args, keywds, "OKK", kwlist, &mfsrc, &last, &vk))
        return NULL;
    const unsigned char *src = (const unsigned char *)PyCapsule_GetPointer(mfsrc, "src");
    uint32_t endpos = get_variantkey_endpos(src, last, vk);
    return Py_BuildValue("I", endpos);
}

static PyObject* py_get_variantkey_chrom_startpos(PyObject *Py_UNUSED(ignored), PyObject *args, PyObject *keywds)
{
    uint64_t vk;
    static char *kwlist[] = {"vk", NULL};
    if (!PyArg_ParseTupleAndKeywords(args, keywds, "K", kwlist, &vk))
        return NULL;
    uint64_t h = get_variantkey_chrom_startpos(vk);
    return Py_BuildValue("K", h);
}

static PyObject* py_get_variantkey_chrom_endpos(PyObject *Py_UNUSED(ignored), PyObject *args, PyObject *keywds)
{
    uint64_t last, vk;
    PyObject* mfsrc = NULL;
    static char *kwlist[] = {"mfsrc", "last", "vk", NULL};
    if (!PyArg_ParseTupleAndKeywords(args, keywds, "OKK", kwlist, &mfsrc, &last, &vk))
        return NULL;
    const unsigned char *src = (const unsigned char *)PyCapsule_GetPointer(mfsrc, "src");
    uint64_t h = get_variantkey_chrom_endpos(src, last, vk);
    return Py_BuildValue("K", h);
}

static PyObject* py_vknr_bin_to_tsv(PyObject *Py_UNUSED(ignored), PyObject *args, PyObject *keywds)
{
    uint64_t last;
    PyObject* mfsrc = NULL;
    const char *tsvfile;
    static char *kwlist[] = {"mfsrc", "last", "tsvfile", NULL};
    if (!PyArg_ParseTupleAndKeywords(args, keywds, "OKs", kwlist, &mfsrc, &last, &tsvfile))
        return NULL;
    const unsigned char *src = (const unsigned char *)PyCapsule_GetPointer(mfsrc, "src");
    size_t len = vknr_bin_to_tsv(src, last, tsvfile);
    return Py_BuildValue("K", len);
}

// --- GENOREF ---

static PyObject* py_load_genoref_index(PyObject *Py_UNUSED(ignored), PyObject *args, PyObject *keywds)
{
    PyObject* mfsrc = NULL;
    static char *kwlist[] = {"mfsrc", NULL};
    if (!PyArg_ParseTupleAndKeywords(args, keywds, "O", kwlist, &mfsrc))
        return NULL;
    const unsigned char *src = (const unsigned char *)PyCapsule_GetPointer(mfsrc, "src");
    uint32_t *idx = (uint32_t *)malloc(27 * sizeof(uint32_t));
    load_genoref_index(src, idx);
    return PyCapsule_New((void*)idx, "idx", NULL);
}

static PyObject* py_get_genoref_seq(PyObject *Py_UNUSED(ignored), PyObject *args, PyObject *keywds)
{
    PyObject* mfsrc = NULL;
    PyObject* mfidx = NULL;
    uint8_t chrom;
    uint32_t pos;
    static char *kwlist[] = {"mfsrc", "mfidx", "chrom", "pos", NULL};
    if (!PyArg_ParseTupleAndKeywords(args, keywds, "OOBI", kwlist, &mfsrc, &mfidx, &chrom, &pos))
        return NULL;
    const unsigned char *src = (const unsigned char *)PyCapsule_GetPointer(mfsrc, "src");
    uint32_t *idx = (uint32_t *)PyCapsule_GetPointer(mfidx, "idx");
    char ref = get_genoref_seq(src, idx, chrom, pos);
    return Py_BuildValue("c", ref);
}

static PyObject *py_check_reference(PyObject *Py_UNUSED(ignored), PyObject *args, PyObject *keywds)
{
    PyObject* mfsrc = NULL;
    PyObject* mfidx = NULL;
    uint8_t chrom;
    uint32_t pos;
    const char *ref;
    Py_ssize_t sizeref;
    static char *kwlist[] = {"mfsrc", "mfidx", "chrom", "pos", "ref", NULL};
    if (!PyArg_ParseTupleAndKeywords(args, keywds, "OOBIs#", kwlist, &mfsrc, &mfidx, &chrom, &pos, &ref, &sizeref))
        return NULL;
    const unsigned char *src = (const unsigned char *)PyCapsule_GetPointer(mfsrc, "src");
    uint32_t *idx = (uint32_t *)PyCapsule_GetPointer(mfidx, "idx");
    int ret = check_reference(src, idx, chrom, pos, ref, (size_t)sizeref);
    return Py_BuildValue("i", ret);
}

static PyObject *py_flip_allele(PyObject *Py_UNUSED(ignored), PyObject *args, PyObject *keywds)
{
    char *allele;
    Py_ssize_t size;
    static char *kwlist[] = {"allele", NULL};
    if (!PyArg_ParseTupleAndKeywords(args, keywds, "s#", kwlist, &allele, &size))
        return NULL;
    flip_allele(allele, (size_t)size);
    return Py_BuildValue("y", allele);
}

static PyObject *py_normalize_variant(PyObject *Py_UNUSED(ignored), PyObject *args, PyObject *keywds)
{
    PyObject *result;
    PyObject* mfsrc = NULL;
    PyObject* mfidx = NULL;
    uint8_t chrom;
    uint32_t pos;
    char ref[ALLELE_MAXSIZE] = "", alt[ALLELE_MAXSIZE] = "";
    Py_ssize_t sizeref, sizealt;
    static char *kwlist[] = {"mfsrc", "mfidx", "chrom", "pos", "ref", "alt", NULL};
    if (!PyArg_ParseTupleAndKeywords(args, keywds, "OOBIs#s#", kwlist, &mfsrc, &mfidx, &chrom, &pos, &ref, &sizeref, &alt, &sizealt))
        return NULL;
    const unsigned char *src = (const unsigned char *)PyCapsule_GetPointer(mfsrc, "src");
    uint32_t *idx = (uint32_t *)PyCapsule_GetPointer(mfidx, "idx");
    size_t stref = (size_t)sizeref;
    size_t stalt = (size_t)sizealt;
    int ret = normalize_variant(src, idx, chrom, &pos, ref, &stref, alt, &stalt);
    result = PyTuple_New(6);
    PyTuple_SetItem(result, 0, Py_BuildValue("i", ret));
    PyTuple_SetItem(result, 1, Py_BuildValue("I", pos));
    PyTuple_SetItem(result, 2, Py_BuildValue("y", ref));
    PyTuple_SetItem(result, 3, Py_BuildValue("y", alt));
    PyTuple_SetItem(result, 4, Py_BuildValue("K", stref));
    PyTuple_SetItem(result, 5, Py_BuildValue("K", stalt));
    return result;
}

// ---

static PyMethodDef PyVariantKeyMethods[] =
{
    // VARIANTKEY
    {"encode_chrom", (PyCFunction)py_encode_chrom, METH_VARARGS|METH_KEYWORDS, PYENCODECHROM_DOCSTRING},
    {"decode_chrom", (PyCFunction)py_decode_chrom, METH_VARARGS|METH_KEYWORDS, PYDECODECHROM_DOCSTRING},
    {"encode_refalt", (PyCFunction)py_encode_refalt, METH_VARARGS|METH_KEYWORDS, PYENCODEREFALT_DOCSTRING},
    {"decode_refalt", (PyCFunction)py_decode_refalt, METH_VARARGS|METH_KEYWORDS, PYDECODEREFALT_DOCSTRING},
    {"encode_variantkey", (PyCFunction)py_encode_variantkey, METH_VARARGS|METH_KEYWORDS, PYENCODEVARIANTKEY_DOCSTRING},
    {"extract_variantkey_chrom", (PyCFunction)py_extract_variantkey_chrom, METH_VARARGS|METH_KEYWORDS, PYEXTRACTVARIANTKEYCHROM_DOCSTRING},
    {"extract_variantkey_pos", (PyCFunction)py_extract_variantkey_pos, METH_VARARGS|METH_KEYWORDS, PYEXTRACTVARIANTKEYPOS_DOCSTRING},
    {"extract_variantkey_refalt", (PyCFunction)py_extract_variantkey_refalt, METH_VARARGS|METH_KEYWORDS, PYEXTRACTVARIANTKEYREFALT_DOCSTRING},
    {"decode_variantkey", (PyCFunction)py_decode_variantkey, METH_VARARGS|METH_KEYWORDS, PYDECODEVARIANTKEY_DOCSTRING},
    {"variantkey", (PyCFunction)py_variantkey, METH_VARARGS|METH_KEYWORDS, PYVARIANTKEY_DOCSTRING},
    {"variantkey_range", (PyCFunction)py_variantkey_range, METH_VARARGS|METH_KEYWORDS, PYVARIANTKEYRANGE_DOCSTRING},
    {"compare_variantkey_chrom", (PyCFunction)py_compare_variantkey_chrom, METH_VARARGS|METH_KEYWORDS, PYCOMPAREVARIANTKEYCHROM_DOCSTRING},
    {"compare_variantkey_chrom_pos", (PyCFunction)py_compare_variantkey_chrom_pos, METH_VARARGS|METH_KEYWORDS, PYCOMPAREVARIANTKEYCHROMPOS_DOCSTRING},
    {"variantkey_hex", (PyCFunction)py_variantkey_hex, METH_VARARGS|METH_KEYWORDS, PYVARIANTKEYHEX_DOCSTRING},
    {"parse_variantkey_hex", (PyCFunction)py_parse_variantkey_hex, METH_VARARGS|METH_KEYWORDS, PYPARSEVARIANTKEYSTRING_DOCSTRING},

    // BINSEARCH
    {"mmap_binfile", (PyCFunction)py_mmap_binfile, METH_VARARGS|METH_KEYWORDS, PYMMAPBINFILE_DOCSTRING},
    {"munmap_binfile", (PyCFunction)py_munmap_binfile, METH_VARARGS|METH_KEYWORDS, PYMUNMAPBINFILE_DOCSTRING},
    {"get_address", (PyCFunction)py_get_address, METH_VARARGS|METH_KEYWORDS, PYGETADDRESS_DOCSTRING},
    {"find_first_uint8", (PyCFunction)py_find_first_uint8, METH_VARARGS|METH_KEYWORDS, PYFINDFIRSTUINT8_DOCSTRING},
    {"find_first_uint16", (PyCFunction)py_find_first_uint16, METH_VARARGS|METH_KEYWORDS, PYFINDFIRSTUINT16_DOCSTRING},
    {"find_first_uint32", (PyCFunction)py_find_first_uint32, METH_VARARGS|METH_KEYWORDS, PYFINDFIRSTUINT32_DOCSTRING},
    {"find_first_uint64", (PyCFunction)py_find_first_uint64, METH_VARARGS|METH_KEYWORDS, PYFINDFIRSTUINT64_DOCSTRING},
    {"find_last_uint8", (PyCFunction)py_find_last_uint8, METH_VARARGS|METH_KEYWORDS, PYFINDLASTUINT8_DOCSTRING},
    {"find_last_uint16", (PyCFunction)py_find_last_uint16, METH_VARARGS|METH_KEYWORDS, PYFINDLASTUINT16_DOCSTRING},
    {"find_last_uint32", (PyCFunction)py_find_last_uint32, METH_VARARGS|METH_KEYWORDS, PYFINDLASTUINT32_DOCSTRING},
    {"find_last_uint64", (PyCFunction)py_find_last_uint64, METH_VARARGS|METH_KEYWORDS, PYFINDLASTUINT64_DOCSTRING},
    {"find_first_sub_uint8", (PyCFunction)py_find_first_sub_uint8, METH_VARARGS|METH_KEYWORDS, PYFINDFIRSTSUBUINT8_DOCSTRING},
    {"find_first_sub_uint16", (PyCFunction)py_find_first_sub_uint16, METH_VARARGS|METH_KEYWORDS, PYFINDFIRSTSUBUINT16_DOCSTRING},
    {"find_first_sub_uint32", (PyCFunction)py_find_first_sub_uint32, METH_VARARGS|METH_KEYWORDS, PYFINDFIRSTSUBUINT32_DOCSTRING},
    {"find_first_sub_uint64", (PyCFunction)py_find_first_sub_uint64, METH_VARARGS|METH_KEYWORDS, PYFINDFIRSTSUBUINT64_DOCSTRING},
    {"find_last_sub_uint8", (PyCFunction)py_find_last_sub_uint8, METH_VARARGS|METH_KEYWORDS, PYFINDLASTSUBUINT8_DOCSTRING},
    {"find_last_sub_uint16", (PyCFunction)py_find_last_sub_uint16, METH_VARARGS|METH_KEYWORDS, PYFINDLASTSUBUINT16_DOCSTRING},
    {"find_last_sub_uint32", (PyCFunction)py_find_last_sub_uint32, METH_VARARGS|METH_KEYWORDS, PYFINDLASTSUBUINT32_DOCSTRING},
    {"find_last_sub_uint64", (PyCFunction)py_find_last_sub_uint64, METH_VARARGS|METH_KEYWORDS, PYFINDLASTSUBUINT64_DOCSTRING},
    {"has_next_uint8", (PyCFunction)py_has_next_uint8, METH_VARARGS|METH_KEYWORDS, PYHASNEXTUINT8_DOCSTRING},
    {"has_next_uint16", (PyCFunction)py_has_next_uint16, METH_VARARGS|METH_KEYWORDS, PYHASNEXTUINT16_DOCSTRING},
    {"has_next_uint32", (PyCFunction)py_has_next_uint32, METH_VARARGS|METH_KEYWORDS, PYHASNEXTUINT32_DOCSTRING},
    {"has_next_uint64", (PyCFunction)py_has_next_uint64, METH_VARARGS|METH_KEYWORDS, PYHASNEXTUINT64_DOCSTRING},
    {"has_next_sub_uint8", (PyCFunction)py_has_next_sub_uint8, METH_VARARGS|METH_KEYWORDS, PYHASNEXTSUBUINT8_DOCSTRING},
    {"has_next_sub_uint16", (PyCFunction)py_has_next_sub_uint16, METH_VARARGS|METH_KEYWORDS, PYHASNEXTSUBUINT16_DOCSTRING},
    {"has_next_sub_uint32", (PyCFunction)py_has_next_sub_uint32, METH_VARARGS|METH_KEYWORDS, PYHASNEXTSUBUINT32_DOCSTRING},
    {"has_next_sub_uint64", (PyCFunction)py_has_next_sub_uint64, METH_VARARGS|METH_KEYWORDS, PYHASNEXTSUBUINT64_DOCSTRING},
    {"has_prev_uint8", (PyCFunction)py_has_prev_uint8, METH_VARARGS|METH_KEYWORDS, PYHASPREVUINT8_DOCSTRING},
    {"has_prev_uint16", (PyCFunction)py_has_prev_uint16, METH_VARARGS|METH_KEYWORDS, PYHASPREVUINT16_DOCSTRING},
    {"has_prev_uint32", (PyCFunction)py_has_prev_uint32, METH_VARARGS|METH_KEYWORDS, PYHASPREVUINT32_DOCSTRING},
    {"has_prev_uint64", (PyCFunction)py_has_prev_uint64, METH_VARARGS|METH_KEYWORDS, PYHASPREVUINT64_DOCSTRING},
    {"has_prev_sub_uint8", (PyCFunction)py_has_prev_sub_uint8, METH_VARARGS|METH_KEYWORDS, PYHASPREVSUBUINT8_DOCSTRING},
    {"has_prev_sub_uint16", (PyCFunction)py_has_prev_sub_uint16, METH_VARARGS|METH_KEYWORDS, PYHASPREVSUBUINT16_DOCSTRING},
    {"has_prev_sub_uint32", (PyCFunction)py_has_prev_sub_uint32, METH_VARARGS|METH_KEYWORDS, PYHASPREVSUBUINT32_DOCSTRING},
    {"has_prev_sub_uint64", (PyCFunction)py_has_prev_sub_uint64, METH_VARARGS|METH_KEYWORDS, PYHASPREVSUBUINT64_DOCSTRING},

    // RSIDVAR
    {"get_vr_rsid", (PyCFunction)py_get_vr_rsid, METH_VARARGS|METH_KEYWORDS, PYGETVRRSID_DOCSTRING},
    {"get_rv_variantkey", (PyCFunction)py_get_rv_variantkey, METH_VARARGS|METH_KEYWORDS, PYGETRVVARIANTKEY_DOCSTRING},
    {"find_rv_variantkey_by_rsid", (PyCFunction)py_find_rv_variantkey_by_rsid, METH_VARARGS|METH_KEYWORDS, PYFINDRVVARIANTKEYBYRSID_DOCSTRING},
    {"get_next_rv_variantkey_by_rsid", (PyCFunction)py_get_next_rv_variantkey_by_rsid, METH_VARARGS|METH_KEYWORDS, PYGETNEXTRVVARIANTKEYBYRSID_DOCSTRING},
    {"find_all_rv_variantkey_by_rsid", (PyCFunction)py_find_all_rv_variantkey_by_rsid, METH_VARARGS|METH_KEYWORDS, PYFINDALLRVVARIANTKEYBYRSID_DOCSTRING},
    {"find_vr_rsid_by_variantkey", (PyCFunction)py_find_vr_rsid_by_variantkey, METH_VARARGS|METH_KEYWORDS, PYFINDVRRSIDBYVARIANTKEY_DOCSTRING},
    {"find_vr_chrompos_range", (PyCFunction)py_find_vr_chrompos_range, METH_VARARGS|METH_KEYWORDS, PYFINDVRCHROMPOSRANGE_DOCSTRING},

    // NRVK
    {"find_ref_alt_by_variantkey", (PyCFunction)py_find_ref_alt_by_variantkey, METH_VARARGS|METH_KEYWORDS, PYFINDREFALTBYVARIANTKEY_DOCSTRING},
    {"reverse_variantkey", (PyCFunction)py_reverse_variantkey, METH_VARARGS|METH_KEYWORDS, PYREVERSEVARIANTKEY_DOCSTRING},
    {"get_ref_len_by_variantkey", (PyCFunction)py_get_ref_len_by_variantkey, METH_VARARGS|METH_KEYWORDS, PYGETREFLENGTHBYVARIANTKEY_DOCSTRING},
    {"get_variantkey_endpos", (PyCFunction)py_get_variantkey_endpos, METH_VARARGS|METH_KEYWORDS, PYGETVARIANTKEYENDPOS_DOCSTRING},
    {"get_variantkey_chrom_startpos", (PyCFunction)py_get_variantkey_chrom_startpos, METH_VARARGS|METH_KEYWORDS, PYGETVARIANTKEYCHROMSTARTPOS_DOCSTRING},
    {"get_variantkey_chrom_endpos", (PyCFunction)py_get_variantkey_chrom_endpos, METH_VARARGS|METH_KEYWORDS, PYGETVARIANTKEYCHROMENDPOS_DOCSTRING},
    {"vknr_bin_to_tsv", (PyCFunction)py_vknr_bin_to_tsv, METH_VARARGS|METH_KEYWORDS, PYVKNRBINTOTSV_DOCSTRING},

    // GENOREF
    {"load_genoref_index", (PyCFunction)py_load_genoref_index, METH_VARARGS|METH_KEYWORDS, PYLOADGENOREFINDEX},
    {"get_genoref_seq", (PyCFunction)py_get_genoref_seq, METH_VARARGS|METH_KEYWORDS, PYGETGENOREFSEQ},
    {"check_reference", (PyCFunction)py_check_reference, METH_VARARGS|METH_KEYWORDS, PYCHECKREFERENCE},
    {"flip_allele", (PyCFunction)py_flip_allele, METH_VARARGS|METH_KEYWORDS, PYFLIPALLELE},
    {"normalize_variant", (PyCFunction)py_normalize_variant, METH_VARARGS|METH_KEYWORDS, PYNORMALIZEVARIANT},

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
