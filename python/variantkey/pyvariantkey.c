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

#define MODULE_NAME "variantkey"
#define PY_SSIZE_T_CLEAN  //!< Make "s#" use Py_ssize_t rather than int.

#include <Python.h>
#include "../../c/src/variantkey/binsearch.h"
#include "../../c/src/variantkey/esid.h"
#include "../../c/src/variantkey/genoref.h"
#include "../../c/src/variantkey/hex.h"
#include "../../c/src/variantkey/nrvk.h"
#include "../../c/src/variantkey/regionkey.h"
#include "../../c/src/variantkey/rsidvar.h"
#include "../../c/src/variantkey/variantkey.h"
#include "pyvariantkey.h"

#ifndef Py_UNUSED // This is already defined for Python 3.4 onwards
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
    uint32_t code;
    static char *kwlist[] = {"code", NULL};
    if (!PyArg_ParseTupleAndKeywords(args, keywds, "I", kwlist, &code))
        return NULL;
    char ref[ALLELE_BUFFSIZE] = "", alt[ALLELE_BUFFSIZE] = "";
    size_t sizeref = 0, sizealt = 0;
    decode_refalt(code, ref, &sizeref, alt, &sizealt);
    PyObject *result = PyTuple_New(4);
    PyTuple_SetItem(result, 0, Py_BuildValue("y", ref));
    PyTuple_SetItem(result, 1, Py_BuildValue("y", alt));
    PyTuple_SetItem(result, 2, Py_BuildValue("B", (uint8_t)sizeref));
    PyTuple_SetItem(result, 3, Py_BuildValue("B", (uint8_t)sizealt));
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
    uint64_t vk;
    static char *kwlist[] = {"vk", NULL};
    if (!PyArg_ParseTupleAndKeywords(args, keywds, "K", kwlist, &vk))
        return NULL;
    variantkey_t h = {0};
    decode_variantkey(vk, &h);
    PyObject *result = PyTuple_New(3);
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
    uint8_t chrom;
    uint32_t pos_min, pos_max;
    static char *kwlist[] = {"chrom", "pos_min", "pos_max", NULL};
    if (!PyArg_ParseTupleAndKeywords(args, keywds, "BII", kwlist, &chrom, &pos_min, &pos_max))
        return NULL;
    vkrange_t r;
    variantkey_range(chrom, pos_min, pos_max, &r);
    PyObject *result = PyTuple_New(2);
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
    static char *kwlist[] = {"vk", NULL};
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

static const mmfile_t *py_get_mmfile_mf(PyObject *mf)
{
    if (mf == Py_None)
    {
        return (const mmfile_t *)PyMem_Malloc(sizeof(mmfile_t));
    }
    return (const mmfile_t *)PyCapsule_GetPointer(mf, "mf");
}

static void destroy_mf(PyObject *mf)
{
    const mmfile_t *cmf = py_get_mmfile_mf(mf);
    if (cmf == NULL)
    {
        return;
    }
    PyMem_Free((void *)cmf);
}

static PyObject* py_munmap_binfile(PyObject *Py_UNUSED(ignored), PyObject *args, PyObject *keywds)
{
    PyObject* mf = NULL;
    static char *kwlist[] = {"mf", NULL};
    if (!PyArg_ParseTupleAndKeywords(args, keywds, "O", kwlist, &mf))
        return NULL;
    const mmfile_t *cmf = py_get_mmfile_mf(mf);
    return Py_BuildValue("i", munmap_binfile(*cmf));
}

// ----------

// --- RSIDVAR ---

static const rsidvar_cols_t *py_get_rsidvar_mc(PyObject *mc)
{
    if (mc == Py_None)
    {
        return (const rsidvar_cols_t *)PyMem_Malloc(sizeof(rsidvar_cols_t));
    }
    return (const rsidvar_cols_t *)PyCapsule_GetPointer(mc, "mc");
}

static void destroy_rsidvar_mc(PyObject *mc)
{
    const rsidvar_cols_t *cmc = py_get_rsidvar_mc(mc);
    if (cmc == NULL)
    {
        return;
    }
    PyMem_Free((void *)cmc);
}

static PyObject* py_mmap_rsvk_file(PyObject *Py_UNUSED(ignored), PyObject *args, PyObject *keywds)
{
    const char *file;
    PyObject* ctbytes = NULL;
    static char *kwlist[] = {"file", "ctbytes", NULL};
    if (!PyArg_ParseTupleAndKeywords(args, keywds, "sO", kwlist, &file, &ctbytes))
        return NULL;
    mmfile_t *mf = (mmfile_t *)PyMem_Malloc(sizeof(mmfile_t));
    rsidvar_cols_t *mc = (rsidvar_cols_t *)PyMem_Malloc(sizeof(rsidvar_cols_t));
    ctbytes = PySequence_Fast(ctbytes, "argument must be iterable");
    if (!ctbytes)
    {
        return 0;
    }
    mf->ncols = (uint8_t)PySequence_Fast_GET_SIZE(ctbytes);
    int i;
    for(i = 0; i < mf->ncols; i++)
    {
        PyObject *fitem;
        PyObject *item = PySequence_Fast_GET_ITEM(ctbytes, i);
        if (!item)
        {
            Py_DECREF(ctbytes);
            return 0;
        }
        fitem = PyNumber_Long(item);
        if (!fitem)
        {
            Py_DECREF(ctbytes);
            return 0;
        }
        mf->ctbytes[i] = (uint8_t)PyLong_AsUnsignedLong(item);
        Py_DECREF(fitem);
    }
    Py_DECREF(ctbytes);
    mmap_rsvk_file(file, mf, mc);
    PyObject *result = PyTuple_New(3);
    PyTuple_SetItem(result, 0, PyCapsule_New(mf, "mf", destroy_mf));
    PyTuple_SetItem(result, 1, PyCapsule_New(mc, "mc", destroy_rsidvar_mc));
    PyTuple_SetItem(result, 2, Py_BuildValue("K", mc->nrows));
    return result;
}

static PyObject* py_mmap_vkrs_file(PyObject *Py_UNUSED(ignored), PyObject *args, PyObject *keywds)
{
    const char *file;
    PyObject* ctbytes = NULL;
    static char *kwlist[] = {"file", "ctbytes", NULL};
    if (!PyArg_ParseTupleAndKeywords(args, keywds, "sO", kwlist, &file, &ctbytes))
        return NULL;
    mmfile_t *mf = (mmfile_t *)PyMem_Malloc(sizeof(mmfile_t));
    rsidvar_cols_t *mc = (rsidvar_cols_t *)PyMem_Malloc(sizeof(rsidvar_cols_t));
    ctbytes = PySequence_Fast(ctbytes, "argument must be iterable");
    if (!ctbytes)
    {
        return 0;
    }
    mf->ncols = (uint8_t)PySequence_Fast_GET_SIZE(ctbytes);
    int i;
    for(i = 0; i < mf->ncols; i++)
    {
        PyObject *fitem;
        PyObject *item = PySequence_Fast_GET_ITEM(ctbytes, i);
        if (!item)
        {
            Py_DECREF(ctbytes);
            return 0;
        }
        fitem = PyNumber_Long(item);
        if (!fitem)
        {
            Py_DECREF(ctbytes);
            return 0;
        }
        mf->ctbytes[i] = (uint8_t)PyLong_AsUnsignedLong(item);
        Py_DECREF(fitem);
    }
    Py_DECREF(ctbytes);
    mmap_vkrs_file(file, mf, mc);
    PyObject *result = PyTuple_New(3);
    PyTuple_SetItem(result, 0, PyCapsule_New(mf, "mf", destroy_mf));
    PyTuple_SetItem(result, 1, PyCapsule_New(mc, "mc", destroy_rsidvar_mc));
    PyTuple_SetItem(result, 2, Py_BuildValue("K", mc->nrows));
    return result;
}

static PyObject* py_find_rv_variantkey_by_rsid(PyObject *Py_UNUSED(ignored), PyObject *args, PyObject *keywds)
{
    uint64_t first, last;
    uint32_t rsid;
    PyObject* mc = NULL;
    static char *kwlist[] = {"mc", "first", "last", "rsid", NULL};
    if (!PyArg_ParseTupleAndKeywords(args, keywds, "OKKI", kwlist, &mc, &first, &last, &rsid))
        return NULL;
    const rsidvar_cols_t *cmc = py_get_rsidvar_mc(mc);
    uint64_t h = find_rv_variantkey_by_rsid(*cmc, &first, last, rsid);
    PyObject *result = PyTuple_New(2);
    PyTuple_SetItem(result, 0, Py_BuildValue("K", h));
    PyTuple_SetItem(result, 1, Py_BuildValue("K", first));
    return result;
}

static PyObject* py_get_next_rv_variantkey_by_rsid(PyObject *Py_UNUSED(ignored), PyObject *args, PyObject *keywds)
{
    uint64_t pos, last;
    uint32_t rsid;
    PyObject* mc = NULL;
    static char *kwlist[] = {"mc", "pos", "last", "rsid", NULL};
    if (!PyArg_ParseTupleAndKeywords(args, keywds, "OKKI", kwlist, &mc, &pos, &last, &rsid))
        return NULL;
    const rsidvar_cols_t *cmc = py_get_rsidvar_mc(mc);
    uint64_t h = get_next_rv_variantkey_by_rsid(*cmc, &pos, last, rsid);
    PyObject *result = PyTuple_New(2);
    PyTuple_SetItem(result, 0, Py_BuildValue("K", h));
    PyTuple_SetItem(result, 1, Py_BuildValue("K", pos));
    return result;
}

static PyObject* py_find_all_rv_variantkey_by_rsid(PyObject *Py_UNUSED(ignored), PyObject *args, PyObject *keywds)
{
    PyObject* vks = PyList_New(0);
    uint64_t first, last;
    uint32_t rsid;
    PyObject* mc = NULL;
    static char *kwlist[] = {"mc", "first", "last", "rsid", NULL};
    if (!PyArg_ParseTupleAndKeywords(args, keywds, "OKKI", kwlist, &mc, &first, &last, &rsid))
        return NULL;
    const rsidvar_cols_t *cmc = py_get_rsidvar_mc(mc);
    uint64_t h = find_rv_variantkey_by_rsid(*cmc, &first, last, rsid);
    while (h > 0)
    {
        PyList_Append(vks, Py_BuildValue("K", h));
        h = get_next_rv_variantkey_by_rsid(*cmc, &first, last, rsid);
    }
    return vks;
}

static PyObject* py_find_vr_rsid_by_variantkey(PyObject *Py_UNUSED(ignored), PyObject *args, PyObject *keywds)
{
    uint64_t first, last, vk;
    PyObject* mc = NULL;
    static char *kwlist[] = {"mc", "first", "last", "vk", NULL};
    if (!PyArg_ParseTupleAndKeywords(args, keywds, "OKKK", kwlist, &mc, &first, &last, &vk))
        return NULL;
    const rsidvar_cols_t *cmc = py_get_rsidvar_mc(mc);
    uint32_t h = find_vr_rsid_by_variantkey(*cmc, &first, last, vk);
    PyObject *result = PyTuple_New(2);
    PyTuple_SetItem(result, 0, Py_BuildValue("I", h));
    PyTuple_SetItem(result, 1, Py_BuildValue("K", first));
    return result;
}

static PyObject* py_find_vr_chrompos_range(PyObject *Py_UNUSED(ignored), PyObject *args, PyObject *keywds)
{
    uint64_t first, last;
    uint8_t chrom;
    uint32_t pos_min, pos_max;
    PyObject* mc = NULL;
    static char *kwlist[] = {"mc", "first", "last", "chrom", "pos_min", "pos_max", NULL};
    if (!PyArg_ParseTupleAndKeywords(args, keywds, "OKKBII", kwlist, &mc, &first, &last, &chrom, &pos_min, &pos_max))
        return NULL;
    const rsidvar_cols_t *cmc = py_get_rsidvar_mc(mc);
    uint32_t h = find_vr_chrompos_range(*cmc, &first, &last, chrom, pos_min, pos_max);
    PyObject *result = PyTuple_New(3);
    PyTuple_SetItem(result, 0, Py_BuildValue("I", h));
    PyTuple_SetItem(result, 1, Py_BuildValue("K", first));
    PyTuple_SetItem(result, 2, Py_BuildValue("K", last));
    return result;
}

// --- NRVK ---

static const nrvk_cols_t *py_get_nrvk_mc(PyObject *mc)
{
    if (mc == Py_None)
    {
        return (const nrvk_cols_t *)PyMem_Malloc(sizeof(nrvk_cols_t));
    }
    return (const nrvk_cols_t *)PyCapsule_GetPointer(mc, "mc");
}

static void destroy_nrvk_mc(PyObject *mc)
{
    const nrvk_cols_t *cmc = py_get_nrvk_mc(mc);
    if (cmc == NULL)
    {
        return;
    }
    PyMem_Free((void *)cmc);
}

static PyObject* py_mmap_nrvk_file(PyObject *Py_UNUSED(ignored), PyObject *args, PyObject *keywds)
{
    const char *file;
    static char *kwlist[] = {"file", NULL};
    if (!PyArg_ParseTupleAndKeywords(args, keywds, "s", kwlist, &file))
        return NULL;
    mmfile_t *mf = (mmfile_t *)PyMem_Malloc(sizeof(mmfile_t));
    nrvk_cols_t *mc = (nrvk_cols_t *)PyMem_Malloc(sizeof(nrvk_cols_t));
    mmap_nrvk_file(file, mf, mc);
    PyObject *result = PyTuple_New(3);
    PyTuple_SetItem(result, 0, PyCapsule_New(mf, "mf", destroy_mf));
    PyTuple_SetItem(result, 1, PyCapsule_New(mc, "mc", destroy_nrvk_mc));
    PyTuple_SetItem(result, 2, Py_BuildValue("K", mc->nrows));
    return result;
}

static PyObject* py_find_ref_alt_by_variantkey(PyObject *Py_UNUSED(ignored), PyObject *args, PyObject *keywds)
{
    uint64_t vk;
    PyObject* mc = NULL;
    static char *kwlist[] = {"mc", "vk", NULL};
    if (!PyArg_ParseTupleAndKeywords(args, keywds, "OK", kwlist, &mc, &vk))
        return NULL;
    char ref[ALLELE_MAXSIZE] = "", alt[ALLELE_MAXSIZE] = "";
    size_t sizeref = 0, sizealt = 0;
    const nrvk_cols_t *cmc = py_get_nrvk_mc(mc);
    size_t len = find_ref_alt_by_variantkey(*cmc, vk, ref, &sizeref, alt, &sizealt);
    PyObject *result = PyTuple_New(5);
    PyTuple_SetItem(result, 0, Py_BuildValue("y", ref));
    PyTuple_SetItem(result, 1, Py_BuildValue("y", alt));
    PyTuple_SetItem(result, 2, Py_BuildValue("B", (uint8_t)sizeref));
    PyTuple_SetItem(result, 3, Py_BuildValue("B", (uint8_t)sizealt));
    PyTuple_SetItem(result, 4, Py_BuildValue("H", (uint16_t)len));
    return result;
}

static PyObject* py_reverse_variantkey(PyObject *Py_UNUSED(ignored), PyObject *args, PyObject *keywds)
{
    uint64_t vk;
    PyObject* mc = NULL;
    static char *kwlist[] = {"mc", "vk", NULL};
    if (!PyArg_ParseTupleAndKeywords(args, keywds, "OK", kwlist, &mc, &vk))
        return NULL;
    variantkey_rev_t rev = {0};
    const nrvk_cols_t *cmc = py_get_nrvk_mc(mc);
    size_t len = reverse_variantkey(*cmc, vk, &rev);
    PyObject *result = PyTuple_New(7);
    PyTuple_SetItem(result, 0, Py_BuildValue("y", rev.chrom));
    PyTuple_SetItem(result, 1, Py_BuildValue("I", rev.pos));
    PyTuple_SetItem(result, 2, Py_BuildValue("y", rev.ref));
    PyTuple_SetItem(result, 3, Py_BuildValue("y", rev.alt));
    PyTuple_SetItem(result, 4, Py_BuildValue("B", (uint8_t)rev.sizeref));
    PyTuple_SetItem(result, 5, Py_BuildValue("B", (uint8_t)rev.sizealt));
    PyTuple_SetItem(result, 6, Py_BuildValue("H", (uint16_t)len));
    return result;
}

static PyObject* py_get_variantkey_ref_length(PyObject *Py_UNUSED(ignored), PyObject *args, PyObject *keywds)
{
    uint64_t vk;
    PyObject* mc = NULL;
    static char *kwlist[] = {"mc", "vk", NULL};
    if (!PyArg_ParseTupleAndKeywords(args, keywds, "OK", kwlist, &mc, &vk))
        return NULL;
    const nrvk_cols_t *cmc = py_get_nrvk_mc(mc);
    size_t len = get_variantkey_ref_length(*cmc, vk);
    return Py_BuildValue("B", (uint8_t)len);
}

static PyObject* py_get_variantkey_endpos(PyObject *Py_UNUSED(ignored), PyObject *args, PyObject *keywds)
{
    uint64_t vk;
    PyObject* mc = NULL;
    static char *kwlist[] = {"mc", "vk", NULL};
    if (!PyArg_ParseTupleAndKeywords(args, keywds, "OK", kwlist, &mc, &vk))
        return NULL;
    const nrvk_cols_t *cmc = py_get_nrvk_mc(mc);
    uint32_t endpos = get_variantkey_endpos(*cmc, vk);
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
    uint64_t vk;
    PyObject* mc = NULL;
    static char *kwlist[] = {"mc", "vk", NULL};
    if (!PyArg_ParseTupleAndKeywords(args, keywds, "OK", kwlist, &mc, &vk))
        return NULL;
    const nrvk_cols_t *cmc = py_get_nrvk_mc(mc);
    uint64_t h = get_variantkey_chrom_endpos(*cmc, vk);
    return Py_BuildValue("K", h);
}

static PyObject* py_nrvk_bin_to_tsv(PyObject *Py_UNUSED(ignored), PyObject *args, PyObject *keywds)
{
    const char *tsvfile;
    PyObject* mc = NULL;
    static char *kwlist[] = {"mc", "tsvfile", NULL};
    if (!PyArg_ParseTupleAndKeywords(args, keywds, "Os", kwlist, &mc, &tsvfile))
        return NULL;
    const nrvk_cols_t *cmc = py_get_nrvk_mc(mc);
    size_t len = nrvk_bin_to_tsv(*cmc, tsvfile);
    return Py_BuildValue("K", len);
}

// --- GENOREF ---

static PyObject* py_mmap_genoref_file(PyObject *Py_UNUSED(ignored), PyObject *args, PyObject *keywds)
{
    const char *file;
    static char *kwlist[] = {"file", NULL};
    if (!PyArg_ParseTupleAndKeywords(args, keywds, "s", kwlist, &file))
        return NULL;
    mmfile_t *mf = (mmfile_t *)PyMem_Malloc(sizeof(mmfile_t));
    mmap_genoref_file(file, mf);
    PyObject *result = PyTuple_New(2);
    PyTuple_SetItem(result, 0, PyCapsule_New(mf, "mf", destroy_mf));
    PyTuple_SetItem(result, 1, Py_BuildValue("K", mf->size));
    return result;
}

static PyObject* py_get_genoref_seq(PyObject *Py_UNUSED(ignored), PyObject *args, PyObject *keywds)
{
    PyObject* mf = NULL;
    uint8_t chrom;
    uint32_t pos;
    static char *kwlist[] = {"mf", "chrom", "pos", NULL};
    if (!PyArg_ParseTupleAndKeywords(args, keywds, "OBI", kwlist, &mf, &chrom, &pos))
        return NULL;
    const mmfile_t *cmf = py_get_mmfile_mf(mf);
    char ref = get_genoref_seq(*cmf, chrom, pos);
    return Py_BuildValue("c", ref);
}

static PyObject *py_check_reference(PyObject *Py_UNUSED(ignored), PyObject *args, PyObject *keywds)
{
    PyObject* mf = NULL;
    uint8_t chrom;
    uint32_t pos;
    const char *ref;
    Py_ssize_t sizeref;
    static char *kwlist[] = {"mf", "chrom", "pos", "ref", NULL};
    if (!PyArg_ParseTupleAndKeywords(args, keywds, "OBIs#", kwlist, &mf, &chrom, &pos, &ref, &sizeref))
        return NULL;
    const mmfile_t *cmf = py_get_mmfile_mf(mf);
    int ret = check_reference(*cmf, chrom, pos, ref, (size_t)sizeref);
    return Py_BuildValue("i", ret);
}

static PyObject *py_flip_allele(PyObject *Py_UNUSED(ignored), PyObject *args, PyObject *keywds)
{
    const char *callele;
    Py_ssize_t size;
    static char *kwlist[] = {"allele", NULL};
    if (!PyArg_ParseTupleAndKeywords(args, keywds, "s#", kwlist, &callele, &size))
        return NULL;
    char allele[ALLELE_MAXSIZE] = "";
    strncpy(allele, callele, (size_t)size);
    allele[size] = 0;
    flip_allele(allele, (size_t)size);
    return Py_BuildValue("y", allele);
}

static PyObject *py_normalize_variant(PyObject *Py_UNUSED(ignored), PyObject *args, PyObject *keywds)
{
    PyObject* mf = NULL;
    uint8_t chrom;
    uint32_t pos;
    const char *cref;
    const char *calt;
    Py_ssize_t sizeref, sizealt;
    static char *kwlist[] = {"mf", "chrom", "pos", "ref", "alt", NULL};
    if (!PyArg_ParseTupleAndKeywords(args, keywds, "OBIs#s#", kwlist, &mf, &chrom, &pos, &cref, &sizeref, &calt, &sizealt))
        return NULL;
    size_t stref = (size_t)sizeref;
    size_t stalt = (size_t)sizealt;
    char ref[ALLELE_MAXSIZE] = "", alt[ALLELE_MAXSIZE] = "";
    strncpy(ref, cref, stref);
    ref[stref] = 0;
    strncpy(alt, calt, stalt);
    alt[stalt] = 0;
    const mmfile_t *cmf = py_get_mmfile_mf(mf);
    int ret = normalize_variant(*cmf, chrom, &pos, ref, &stref, alt, &stalt);
    PyObject *result = PyTuple_New(6);
    PyTuple_SetItem(result, 0, Py_BuildValue("i", ret));
    PyTuple_SetItem(result, 1, Py_BuildValue("I", pos));
    PyTuple_SetItem(result, 2, Py_BuildValue("y", ref));
    PyTuple_SetItem(result, 3, Py_BuildValue("y", alt));
    PyTuple_SetItem(result, 4, Py_BuildValue("B", (uint8_t)stref));
    PyTuple_SetItem(result, 5, Py_BuildValue("B", (uint8_t)stalt));
    return result;
}

// --- REGIONKEY ---

static PyObject* py_encode_region_strand(PyObject *Py_UNUSED(ignored), PyObject *args, PyObject *keywds)
{
    short int strand;
    static char *kwlist[] = {"strand", NULL};
    if (!PyArg_ParseTupleAndKeywords(args, keywds, "h", kwlist, &strand))
        return NULL;
    uint8_t h = encode_region_strand((int8_t)strand);
    return Py_BuildValue("B", h);
}

static PyObject* py_decode_region_strand(PyObject *Py_UNUSED(ignored), PyObject *args, PyObject *keywds)
{
    uint8_t strand;
    static char *kwlist[] = {"strand", NULL};
    if (!PyArg_ParseTupleAndKeywords(args, keywds, "B", kwlist, &strand))
        return NULL;
    int8_t h = decode_region_strand(strand);
    return Py_BuildValue("h", (short int)h);
}

static PyObject* py_encode_regionkey(PyObject *Py_UNUSED(ignored), PyObject *args, PyObject *keywds)
{
    uint8_t chrom, strand;
    uint32_t startpos, endpos;
    static char *kwlist[] = {"chrom", "startpos", "endpos", "strand", NULL};
    if (!PyArg_ParseTupleAndKeywords(args, keywds, "BIIB", kwlist, &chrom, &startpos, &endpos, &strand))
        return NULL;
    uint64_t h = encode_regionkey(chrom, startpos, endpos, strand);
    return Py_BuildValue("K", h);
}

static PyObject* py_extract_regionkey_chrom(PyObject *Py_UNUSED(ignored), PyObject *args, PyObject *keywds)
{
    uint64_t rk;
    static char *kwlist[] = {"rk", NULL};
    if (!PyArg_ParseTupleAndKeywords(args, keywds, "K", kwlist, &rk))
        return NULL;
    uint8_t h = extract_regionkey_chrom(rk);
    return Py_BuildValue("B", h);
}

static PyObject* py_extract_regionkey_startpos(PyObject *Py_UNUSED(ignored), PyObject *args, PyObject *keywds)
{
    uint64_t rk;
    static char *kwlist[] = {"rk", NULL};
    if (!PyArg_ParseTupleAndKeywords(args, keywds, "K", kwlist, &rk))
        return NULL;
    uint32_t h = extract_regionkey_startpos(rk);
    return Py_BuildValue("I", h);
}

static PyObject* py_extract_regionkey_endpos(PyObject *Py_UNUSED(ignored), PyObject *args, PyObject *keywds)
{
    uint64_t rk;
    static char *kwlist[] = {"rk", NULL};
    if (!PyArg_ParseTupleAndKeywords(args, keywds, "K", kwlist, &rk))
        return NULL;
    uint32_t h = extract_regionkey_endpos(rk);
    return Py_BuildValue("I", h);
}

static PyObject* py_extract_regionkey_strand(PyObject *Py_UNUSED(ignored), PyObject *args, PyObject *keywds)
{
    uint64_t rk;
    static char *kwlist[] = {"rk", NULL};
    if (!PyArg_ParseTupleAndKeywords(args, keywds, "K", kwlist, &rk))
        return NULL;
    uint32_t h = extract_regionkey_strand(rk);
    return Py_BuildValue("B", h);
}

static PyObject* py_decode_regionkey(PyObject *Py_UNUSED(ignored), PyObject *args, PyObject *keywds)
{
    PyObject *result;
    uint64_t rk;
    static char *kwlist[] = {"rk", NULL};
    if (!PyArg_ParseTupleAndKeywords(args, keywds, "K", kwlist, &rk))
        return NULL;
    regionkey_t h = {0};
    decode_regionkey(rk, &h);
    result = PyTuple_New(4);
    PyTuple_SetItem(result, 0, Py_BuildValue("B", h.chrom));
    PyTuple_SetItem(result, 1, Py_BuildValue("I", h.startpos));
    PyTuple_SetItem(result, 2, Py_BuildValue("I", h.endpos));
    PyTuple_SetItem(result, 3, Py_BuildValue("B", h.strand));
    return result;
}

static PyObject* py_reverse_regionkey(PyObject *Py_UNUSED(ignored), PyObject *args, PyObject *keywds)
{
    PyObject *result;
    uint64_t rk;
    static char *kwlist[] = {"rk", NULL};
    if (!PyArg_ParseTupleAndKeywords(args, keywds, "K", kwlist, &rk))
        return NULL;
    regionkey_rev_t rev = {0};
    reverse_regionkey(rk, &rev);
    result = PyTuple_New(4);
    PyTuple_SetItem(result, 0, Py_BuildValue("y", rev.chrom));
    PyTuple_SetItem(result, 1, Py_BuildValue("I", rev.startpos));
    PyTuple_SetItem(result, 2, Py_BuildValue("I", rev.endpos));
    PyTuple_SetItem(result, 3, Py_BuildValue("h", rev.strand));
    return result;
}

static PyObject* py_regionkey(PyObject *Py_UNUSED(ignored), PyObject *args, PyObject *keywds)
{
    const char *chrom;
    Py_ssize_t sizechrom;
    uint32_t startpos, endpos;
    short int strand;
    static char *kwlist[] = {"chrom", "startpos", "endpos", "strand", NULL};
    if (!PyArg_ParseTupleAndKeywords(args, keywds, "s#IIh", kwlist, &chrom, &sizechrom, &startpos, &endpos, &strand))
        return NULL;
    uint64_t h = regionkey(chrom, (size_t)sizechrom, startpos, endpos, (int8_t)strand);
    return Py_BuildValue("K", h);
}

static PyObject* py_regionkey_hex(PyObject *Py_UNUSED(ignored), PyObject *args, PyObject *keywds)
{
    uint64_t code;
    static char *kwlist[] = {"code", NULL};
    if (!PyArg_ParseTupleAndKeywords(args, keywds, "K", kwlist, &code))
        return NULL;
    char str[17];
    regionkey_hex(code, str);
    return PyBytes_FromString(str);
}

static PyObject* py_parse_regionkey_hex(PyObject *Py_UNUSED(ignored), PyObject *args, PyObject *keywds)
{
    const char *rs;
    Py_ssize_t sizers;
    static char *kwlist[] = {"rs", NULL};
    if (!PyArg_ParseTupleAndKeywords(args, keywds, "s#", kwlist, &rs, &sizers))
        return NULL;
    uint64_t h = 0;
    if (sizers == 16)
    {
        h = parse_regionkey_hex(rs);
    }
    return Py_BuildValue("K", h);
}

static PyObject* py_get_regionkey_chrom_startpos(PyObject *Py_UNUSED(ignored), PyObject *args, PyObject *keywds)
{
    uint64_t rk;
    static char *kwlist[] = {"rk", NULL};
    if (!PyArg_ParseTupleAndKeywords(args, keywds, "K", kwlist, &rk))
        return NULL;
    uint64_t h = get_regionkey_chrom_startpos(rk);
    return Py_BuildValue("K", h);
}

static PyObject* py_get_regionkey_chrom_endpos(PyObject *Py_UNUSED(ignored), PyObject *args, PyObject *keywds)
{
    uint64_t rk;
    static char *kwlist[] = {"rk", NULL};
    if (!PyArg_ParseTupleAndKeywords(args, keywds, "K", kwlist, &rk))
        return NULL;
    uint64_t h = get_regionkey_chrom_endpos(rk);
    return Py_BuildValue("K", h);
}

static PyObject* py_are_overlapping_regions(PyObject *Py_UNUSED(ignored), PyObject *args, PyObject *keywds)
{
    uint8_t a_chrom, b_chrom;
    uint32_t a_startpos, b_startpos, a_endpos, b_endpos;
    static char *kwlist[] = {"a_chrom", "a_startpos", "a_endpos", "b_chrom", "b_startpos", "b_endpos", NULL};
    if (!PyArg_ParseTupleAndKeywords(args, keywds, "BIIBII", kwlist, &a_chrom, &a_startpos, &a_endpos, &b_chrom, &b_startpos, &b_endpos))
        return NULL;
    uint8_t h = are_overlapping_regions(a_chrom, a_startpos, a_endpos, b_chrom, b_startpos, b_endpos);
    return Py_BuildValue("B", h);
}

static PyObject* py_are_overlapping_region_regionkey(PyObject *Py_UNUSED(ignored), PyObject *args, PyObject *keywds)
{
    uint8_t chrom;
    uint32_t startpos, endpos;
    uint64_t rk;
    static char *kwlist[] = {"chrom", "startpos", "endpos", "rk", NULL};
    if (!PyArg_ParseTupleAndKeywords(args, keywds, "BIIK", kwlist, &chrom, &startpos, &endpos, &rk))
        return NULL;
    uint8_t h = are_overlapping_region_regionkey(chrom, startpos, endpos, rk);
    return Py_BuildValue("B", h);
}

static PyObject* py_are_overlapping_regionkeys(PyObject *Py_UNUSED(ignored), PyObject *args, PyObject *keywds)
{
    uint64_t rka, rkb;
    static char *kwlist[] = {"rka", "rkb", NULL};
    if (!PyArg_ParseTupleAndKeywords(args, keywds, "KK", kwlist, &rka, &rkb))
        return NULL;
    uint8_t h = are_overlapping_regionkeys(rka, rkb);
    return Py_BuildValue("B", h);
}

static PyObject* py_are_overlapping_variantkey_regionkey(PyObject *Py_UNUSED(ignored), PyObject *args, PyObject *keywds)
{
    uint64_t vk, rk;
    PyObject* mc = NULL;
    static char *kwlist[] = {"mc", "vk", "rk", NULL};
    if (!PyArg_ParseTupleAndKeywords(args, keywds, "OKK", kwlist, &mc, &vk, &rk))
        return NULL;
    const nrvk_cols_t *cmc = py_get_nrvk_mc(mc);
    uint8_t h = are_overlapping_variantkey_regionkey(*cmc, vk, rk);
    return Py_BuildValue("B", h);
}

static PyObject* py_variantkey_to_regionkey(PyObject *Py_UNUSED(ignored), PyObject *args, PyObject *keywds)
{
    uint64_t vk;
    PyObject* mc = NULL;
    static char *kwlist[] = {"mc", "vk", NULL};
    if (!PyArg_ParseTupleAndKeywords(args, keywds, "OK", kwlist, &mc, &vk))
        return NULL;
    const nrvk_cols_t *cmc = py_get_nrvk_mc(mc);
    uint64_t h = variantkey_to_regionkey(*cmc, vk);
    return Py_BuildValue("K", h);
}

// --- ESID ---

static PyObject* py_encode_string_id(PyObject *Py_UNUSED(ignored), PyObject *args, PyObject *keywds)
{
    const char *str;
    Py_ssize_t size;
    uint32_t start;
    static char *kwlist[] = {"strid", "start", NULL};
    if (!PyArg_ParseTupleAndKeywords(args, keywds, "s#I", kwlist, &str, &size, &start))
        return NULL;
    uint64_t h = encode_string_id(str, (size_t)size, (size_t)start);
    return Py_BuildValue("K", h);
}

static PyObject* py_encode_string_num_id(PyObject *Py_UNUSED(ignored), PyObject *args, PyObject *keywds)
{
    const char *str;
    Py_ssize_t size;
    char sep;
    static char *kwlist[] = {"strid", "sep", NULL};
    if (!PyArg_ParseTupleAndKeywords(args, keywds, "s#c", kwlist, &str, &size, &sep))
        return NULL;
    uint64_t h = encode_string_num_id(str, (size_t)size, sep);
    return Py_BuildValue("K", h);
}

static PyObject* py_decode_string_id(PyObject *Py_UNUSED(ignored), PyObject *args, PyObject *keywds)
{
    uint64_t esid;
    static char *kwlist[] = {"esid", NULL};
    if (!PyArg_ParseTupleAndKeywords(args, keywds, "K", kwlist, &esid))
        return NULL;
    char str[23] = "";
    size_t len = decode_string_id(esid, str);
    PyObject *result;
    result = PyTuple_New(2);
    PyTuple_SetItem(result, 0, Py_BuildValue("y", str));
    PyTuple_SetItem(result, 1, Py_BuildValue("B", (uint8_t)len));
    return result;
}

static PyObject* py_hash_string_id(PyObject *Py_UNUSED(ignored), PyObject *args, PyObject *keywds)
{
    const char *str;
    Py_ssize_t size;
    static char *kwlist[] = {"str", NULL};
    if (!PyArg_ParseTupleAndKeywords(args, keywds, "s#", kwlist, &str, &size))
        return NULL;
    uint64_t h = hash_string_id(str, (size_t)size);
    return Py_BuildValue("K", h);
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
    {"munmap_binfile", (PyCFunction)py_munmap_binfile, METH_VARARGS|METH_KEYWORDS, PYMUNMAPBINFILE_DOCSTRING},

    // RSIDVAR
    {"mmap_rsvk_file", (PyCFunction)py_mmap_rsvk_file, METH_VARARGS|METH_KEYWORDS, PYMMAPRSVKFILE_DOCSTRING},
    {"mmap_vkrs_file", (PyCFunction)py_mmap_vkrs_file, METH_VARARGS|METH_KEYWORDS, PYMMAPVKRSFILE_DOCSTRING},
    {"find_rv_variantkey_by_rsid", (PyCFunction)py_find_rv_variantkey_by_rsid, METH_VARARGS|METH_KEYWORDS, PYFINDRVVARIANTKEYBYRSID_DOCSTRING},
    {"get_next_rv_variantkey_by_rsid", (PyCFunction)py_get_next_rv_variantkey_by_rsid, METH_VARARGS|METH_KEYWORDS, PYGETNEXTRVVARIANTKEYBYRSID_DOCSTRING},
    {"find_all_rv_variantkey_by_rsid", (PyCFunction)py_find_all_rv_variantkey_by_rsid, METH_VARARGS|METH_KEYWORDS, PYFINDALLRVVARIANTKEYBYRSID_DOCSTRING},
    {"find_vr_rsid_by_variantkey", (PyCFunction)py_find_vr_rsid_by_variantkey, METH_VARARGS|METH_KEYWORDS, PYFINDVRRSIDBYVARIANTKEY_DOCSTRING},
    {"find_vr_chrompos_range", (PyCFunction)py_find_vr_chrompos_range, METH_VARARGS|METH_KEYWORDS, PYFINDVRCHROMPOSRANGE_DOCSTRING},

    // NRVK
    {"mmap_nrvk_file", (PyCFunction)py_mmap_nrvk_file, METH_VARARGS|METH_KEYWORDS, PYMMAPNRVKFILE_DOCSTRING},
    {"find_ref_alt_by_variantkey", (PyCFunction)py_find_ref_alt_by_variantkey, METH_VARARGS|METH_KEYWORDS, PYFINDREFALTBYVARIANTKEY_DOCSTRING},
    {"reverse_variantkey", (PyCFunction)py_reverse_variantkey, METH_VARARGS|METH_KEYWORDS, PYREVERSEVARIANTKEY_DOCSTRING},
    {"get_variantkey_ref_length", (PyCFunction)py_get_variantkey_ref_length, METH_VARARGS|METH_KEYWORDS, PYGETREFLENGTHBYVARIANTKEY_DOCSTRING},
    {"get_variantkey_endpos", (PyCFunction)py_get_variantkey_endpos, METH_VARARGS|METH_KEYWORDS, PYGETVARIANTKEYENDPOS_DOCSTRING},
    {"get_variantkey_chrom_startpos", (PyCFunction)py_get_variantkey_chrom_startpos, METH_VARARGS|METH_KEYWORDS, PYGETVARIANTKEYCHROMSTARTPOS_DOCSTRING},
    {"get_variantkey_chrom_endpos", (PyCFunction)py_get_variantkey_chrom_endpos, METH_VARARGS|METH_KEYWORDS, PYGETVARIANTKEYCHROMENDPOS_DOCSTRING},
    {"nrvk_bin_to_tsv", (PyCFunction)py_nrvk_bin_to_tsv, METH_VARARGS|METH_KEYWORDS, PYNRVKBINTOTSV_DOCSTRING},

    // GENOREF
    {"mmap_genoref_file", (PyCFunction)py_mmap_genoref_file, METH_VARARGS|METH_KEYWORDS, PYMMAPGENOREFFILE_DOCSTRING},
    {"get_genoref_seq", (PyCFunction)py_get_genoref_seq, METH_VARARGS|METH_KEYWORDS, PYGETGENOREFSEQ_DOCSTRING},
    {"check_reference", (PyCFunction)py_check_reference, METH_VARARGS|METH_KEYWORDS, PYCHECKREFERENCE_DOCSTRING},
    {"flip_allele", (PyCFunction)py_flip_allele, METH_VARARGS|METH_KEYWORDS, PYFLIPALLELE_DOCSTRING},
    {"normalize_variant", (PyCFunction)py_normalize_variant, METH_VARARGS|METH_KEYWORDS, PYNORMALIZEVARIANT_DOCSTRING},

    // REGIONKEY
    {"encode_region_strand", (PyCFunction)py_encode_region_strand, METH_VARARGS|METH_KEYWORDS, PYENCODEREGIONSTRAND_DOCSTRING},
    {"decode_region_strand", (PyCFunction)py_decode_region_strand, METH_VARARGS|METH_KEYWORDS, PYDECODEREGIONSTRAND_DOCSTRING},
    {"encode_regionkey", (PyCFunction)py_encode_regionkey, METH_VARARGS|METH_KEYWORDS, PYENCODEREGIONKEY_DOCSTRING},
    {"extract_regionkey_chrom", (PyCFunction)py_extract_regionkey_chrom, METH_VARARGS|METH_KEYWORDS, PYEXTRACTREGIONKEYCHROM_DOCSTRING},
    {"extract_regionkey_startpos", (PyCFunction)py_extract_regionkey_startpos, METH_VARARGS|METH_KEYWORDS, PYEXTRACTREGIONKEYSTARTPOS_DOCSTRING},
    {"extract_regionkey_endpos", (PyCFunction)py_extract_regionkey_endpos, METH_VARARGS|METH_KEYWORDS, PYEXTRACTREGIONKEYENDPOS_DOCSTRING},
    {"extract_regionkey_strand", (PyCFunction)py_extract_regionkey_strand, METH_VARARGS|METH_KEYWORDS, PYEXTRACTREGIONKEYSTRAND_DOCSTRING},
    {"decode_regionkey", (PyCFunction)py_decode_regionkey, METH_VARARGS|METH_KEYWORDS, PYDECODEREGIONKEY_DOCSTRING},
    {"reverse_regionkey", (PyCFunction)py_reverse_regionkey, METH_VARARGS|METH_KEYWORDS, PYREVERSEREGIONKEY_DOCSTRING},
    {"regionkey", (PyCFunction)py_regionkey, METH_VARARGS|METH_KEYWORDS, PYREGIONKEY_DOCSTRING},
    {"regionkey_hex", (PyCFunction)py_regionkey_hex, METH_VARARGS|METH_KEYWORDS, PYREGIONKEYHEX_DOCSTRING},
    {"parse_regionkey_hex", (PyCFunction)py_parse_regionkey_hex, METH_VARARGS|METH_KEYWORDS, PYPARSEREGIONKEYHEX_DOCSTRING},
    {"get_regionkey_chrom_startpos", (PyCFunction)py_get_regionkey_chrom_startpos, METH_VARARGS|METH_KEYWORDS, PYGETREGIONKEYCHROMSTARTPOS_DOCSTRING},
    {"get_regionkey_chrom_endpos", (PyCFunction)py_get_regionkey_chrom_endpos, METH_VARARGS|METH_KEYWORDS, PYGETREGIONKEYCHROMENDPOS_DOCSTRING},
    {"are_overlapping_regions", (PyCFunction)py_are_overlapping_regions, METH_VARARGS|METH_KEYWORDS, PYAREOVERLAPPINGREGIONS_DOCSTRING},
    {"are_overlapping_region_regionkey", (PyCFunction)py_are_overlapping_region_regionkey, METH_VARARGS|METH_KEYWORDS, AREOVERLAPPINGREGIONREGIONKEY_DOCSTRING},
    {"are_overlapping_regionkeys", (PyCFunction)py_are_overlapping_regionkeys, METH_VARARGS|METH_KEYWORDS, AREOVERLAPPINGREGIONKEYS_DOCSTRING},
    {"are_overlapping_variantkey_regionkey", (PyCFunction)py_are_overlapping_variantkey_regionkey, METH_VARARGS|METH_KEYWORDS, AREOVERLAPPINGVARIANTKEYREGIONKEY_DOCSTRING},
    {"variantkey_to_regionkey", (PyCFunction)py_variantkey_to_regionkey, METH_VARARGS|METH_KEYWORDS, VARIANTKEYTOREGIONKEY_DOCSTRING},

    // ESID
    {"encode_string_id", (PyCFunction)py_encode_string_id, METH_VARARGS|METH_KEYWORDS, ENCODESTRINGID_DOCSTRING},
    {"encode_string_num_id", (PyCFunction)py_encode_string_num_id, METH_VARARGS|METH_KEYWORDS, ENCODESTRINGNUMID_DOCSTRING},
    {"decode_string_id", (PyCFunction)py_decode_string_id, METH_VARARGS|METH_KEYWORDS, DECODESTRINGID_DOCSTRING},
    {"hash_string_id", (PyCFunction)py_hash_string_id, METH_VARARGS|METH_KEYWORDS, HASHSTRINGID_DOCSTRING},

    {NULL, NULL, 0, NULL}
};

static const char modulename[] = MODULE_NAME;

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
static int variantkey_traverse(PyObject *m, visitproc visit, void *arg)
{
    Py_VISIT(GETSTATE(m)->error);
    return 0;
}

static int variantkey_clear(PyObject *m)
{
    Py_CLEAR(GETSTATE(m)->error);
    return 0;
}

static struct PyModuleDef moduledef =
{
    PyModuleDef_HEAD_INIT,
    modulename,
    NULL,
    sizeof(struct module_state),
    PyVariantKeyMethods,
    NULL,
    variantkey_traverse,
    variantkey_clear,
    NULL
};

#define INITERROR return NULL

PyObject* PyInit_variantkey(void)
#else
#define INITERROR return

void initvariantkey(void)
#endif
{
#if PY_MAJOR_VERSION >= 3
    PyObject *module = PyModule_Create(&moduledef);
#else
    PyObject *module = Py_InitModule(modulename, PyVariantKeyMethods);
#endif
    struct module_state *st = NULL;
    if (module == NULL)
    {
        INITERROR;
    }
    st = GETSTATE(module);
    st->error = PyErr_NewException(MODULE_NAME ".Error", NULL, NULL);
    if (st->error == NULL)
    {
        Py_DECREF(module);
        INITERROR;
    }
#if PY_MAJOR_VERSION >= 3
    return module;
#endif
}
