// Package variantkey is a Go wrapper for the variantkey C software library.
// 64 bit Encoding for Human Genetic Variants.
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
package variantkey

/*
#cgo CFLAGS: -O3 -pedantic -std=c99 -Wextra -Wno-strict-prototypes -Wcast-align -Wundef -Wformat-security -Wshadow
#include <stdlib.h>
#include <inttypes.h>
#include "../../c/src/variantkey/binsearch.h"
#include "../../c/src/variantkey/esid.h"
#include "../../c/src/variantkey/genoref.h"
#include "../../c/src/variantkey/hex.h"
#include "../../c/src/variantkey/nrvk.h"
#include "../../c/src/variantkey/regionkey.h"
#include "../../c/src/variantkey/rsidvar.h"
#include "../../c/src/variantkey/variantkey.h"
*/
import "C"
import "unsafe"
import "fmt"

// TVariantKey contains a representation of a genetic variant key
type TVariantKey struct {
	Chrom  uint8  `json:"chrom"`
	Pos    uint32 `json:"pos"`
	RefAlt uint32 `json:"refalt"`
}

// TVariantKeyRev contains a genetic variant components
type TVariantKeyRev struct {
	Chrom   string `json:"chrom"`
	Pos     uint32 `json:"pos"`
	Ref     string `json:"ref"`
	Alt     string `json:"alt"`
	SizeRef uint8  `json:"size_ref"`
	SizeAlt uint8  `json:"size_alt"`
}

// TVKRange contains min and max VariantKey values for range searches
type TVKRange struct {
	Min uint64 `json:"min"`
	Max uint64 `json:"max"`
}

// castCVariantKey convert C variantkey_t to GO TVariantKey.
func castCVariantKey(vk C.variantkey_t) TVariantKey {
	return TVariantKey{
		Chrom:  uint8(vk.chrom),
		Pos:    uint32(vk.pos),
		RefAlt: uint32(vk.refalt),
	}
}

// castCVariantKeyRev convert C variantkey_t to GO TVariantKey.
func castCVariantKeyRev(vk C.variantkey_rev_t) TVariantKeyRev {
	return TVariantKeyRev{
		Chrom:   C.GoString((*C.char)(unsafe.Pointer(&vk.chrom[0]))),
		Pos:     uint32(vk.pos),
		Ref:     C.GoString((*C.char)(unsafe.Pointer(&vk.ref[0]))),
		Alt:     C.GoString((*C.char)(unsafe.Pointer(&vk.alt[0]))),
		SizeRef: uint8(vk.sizeref),
		SizeAlt: uint8(vk.sizealt),
	}
}

// castCVKRrange convert C vkrange_t to GO TVKRange.
func castCVKRrange(vr C.vkrange_t) TVKRange {
	return TVKRange{
		Min: uint64(vr.min),
		Max: uint64(vr.max),
	}
}

// StringToNTBytes safely convert a string to byte array with an extra null terminator
// This is to ensure a correct CGO conversion to char*
func StringToNTBytes(s string) []byte {
	b := make([]byte, len(s)+1)
	copy(b[:], s)
	return b
}

// StringToNTBytesN convert a string to byte array allocating "size" bytes.
func StringToNTBytesN(s string, size uint32) []byte {
	b := make([]byte, size)
	copy(b[:], s)
	return b
}

// EncodeChrom returns chromosome encoding.
func EncodeChrom(chrom string) uint8 {
	bchrom := StringToNTBytes(chrom)
	sizechrom := len(chrom)
	pchrom := unsafe.Pointer(&bchrom[0]) // #nosec
	return uint8(C.encode_chrom((*C.char)(pchrom), C.size_t(sizechrom)))
}

// DecodeChrom decode chrom to string
func DecodeChrom(c uint8) string {
	cstr := C.malloc(3)
	defer C.free(unsafe.Pointer(cstr)) // #nosec
	len := C.decode_chrom(C.uint8_t(c), (*C.char)(cstr))
	return C.GoStringN((*C.char)(cstr), C.int(len))
}

// EncodeRefAlt returns reference+alternate code.
func EncodeRefAlt(ref string, alt string) uint32 {
	bref := StringToNTBytes(ref)
	balt := StringToNTBytes(alt)
	sizeref := len(ref)
	sizealt := len(alt)
	pref := unsafe.Pointer(&bref[0]) // #nosec
	palt := unsafe.Pointer(&balt[0]) // #nosec
	return uint32(C.encode_refalt((*C.char)(pref), C.size_t(sizeref), (*C.char)(palt), C.size_t(sizealt)))
}

// DecodeRefAlt decode Ref+Alt code if reversible
func DecodeRefAlt(c uint32) (string, string, uint8, uint8, uint8) {
	cref := C.malloc(12)
	defer C.free(unsafe.Pointer(cref)) // #nosec
	calt := C.malloc(12)
	defer C.free(unsafe.Pointer(calt)) // #nosec
	csizeref := C.size_t(0)
	csizealt := C.size_t(0)
	len := C.decode_refalt(C.uint32_t(c), (*C.char)(cref), &csizeref, (*C.char)(calt), &csizealt)
	return C.GoStringN((*C.char)(cref), C.int(csizeref)), C.GoStringN((*C.char)(calt), C.int(csizealt)), uint8(csizeref), uint8(csizealt), uint8(len)
}

// EncodeVariantKey returns a Genetic Variant Key based on pre-encoded CHROM, POS (0-base), REF+ALT.
func EncodeVariantKey(chrom uint8, pos, refalt uint32) uint64 {
	return uint64(C.encode_variantkey(C.uint8_t(chrom), C.uint32_t(pos), C.uint32_t(refalt)))
}

// ExtractVariantKeyChrom extracts the CHROM code from VariantKey.
func ExtractVariantKeyChrom(v uint64) uint8 {
	return uint8(C.extract_variantkey_chrom(C.uint64_t(v)))
}

// ExtractVariantKeyPos extracts the POS code from VariantKey.
func ExtractVariantKeyPos(v uint64) uint32 {
	return uint32(C.extract_variantkey_pos(C.uint64_t(v)))
}

// ExtractVariantKeyRefAlt extracts the REF+ALT code from VariantKey.
func ExtractVariantKeyRefAlt(v uint64) uint32 {
	return uint32(C.extract_variantkey_refalt(C.uint64_t(v)))
}

// DecodeVariantKey parses a variant key string and returns the components as TVariantKey structure.
func DecodeVariantKey(v uint64) TVariantKey {
	var vk C.variantkey_t
	C.decode_variantkey(C.uint64_t(v), &vk)
	return castCVariantKey(vk)
}

// VariantKey returns a Genetic Variant Key based on CHROM, POS (0-base), REF, ALT.
func VariantKey(chrom string, pos uint32, ref, alt string) uint64 {
	bchrom := StringToNTBytes(chrom)
	bref := StringToNTBytes(ref)
	balt := StringToNTBytes(alt)
	sizeref := len(ref)
	sizealt := len(alt)
	pchrom := unsafe.Pointer(&bchrom[0]) // #nosec
	pref := unsafe.Pointer(&bref[0])     // #nosec
	palt := unsafe.Pointer(&balt[0])     // #nosec
	return uint64(C.variantkey((*C.char)(pchrom), C.size_t(len(chrom)), C.uint32_t(pos), (*C.char)(pref), C.size_t(sizeref), (*C.char)(palt), C.size_t(sizealt)))
}

// Range Returns minimum and maximum variant keys for range searches.
func Range(chrom uint8, posMin, posMax uint32) TVKRange {
	var r C.vkrange_t
	C.variantkey_range(C.uint8_t(chrom), C.uint32_t(posMin), C.uint32_t(posMax), &r)
	return castCVKRrange(r)
}

// CompareVariantKeyChrom compares two VariantKeys by chromosome only.
func CompareVariantKeyChrom(va, vb uint64) int {
	return int(C.compare_variantkey_chrom(C.uint64_t(va), C.uint64_t(vb)))
}

// CompareVariantKeyChromPos compares two VariantKeys by chromosome and position.
func CompareVariantKeyChromPos(va, vb uint64) int {
	return int(C.compare_variantkey_chrom_pos(C.uint64_t(va), C.uint64_t(vb)))
}

// Hex provides a 16 digits hexadecimal string representation of a 64bit unsigned number.
func Hex(v uint64) string {
	cstr := C.malloc(17)
	defer C.free(unsafe.Pointer(cstr)) // #nosec
	C.variantkey_hex(C.uint64_t(v), (*C.char)(cstr))
	return C.GoStringN((*C.char)(cstr), C.int(16))
}

// ParseHex parses a 16 digit HEX string and returns the 64 bit unsigned number.
func ParseHex(s string) uint64 {
	b := StringToNTBytes(s)
	p := unsafe.Pointer(&b[0]) // #nosec
	return uint64(C.parse_variantkey_hex((*C.char)(p)))
}

// ReverseVariantKey parses a variant key string and returns the components.
func ReverseVariantKey(v uint64) (chrom string, pos uint32, ref string, alt string, sizeref uint8, sizealt uint8) {
	vk := DecodeVariantKey(v)
	chrom = DecodeChrom(vk.Chrom)
	pos = vk.Pos
	if (vk.RefAlt & 0x1) == 0 {
		ref, alt, sizeref, sizealt, _ = DecodeRefAlt(vk.RefAlt)
	}
	return
}

// --- BINSEARCH ---

// TMMFile contains the memory mapped file info
type TMMFile struct {
	Src     unsafe.Pointer // Pointer to the memory map.
	Fd      int            // File descriptor.
	Size    uint64         // File size in bytes.
	DOffset uint64         // Offset to the beginning of the data block (address of the first byte of the first item in the first column).
	DLength uint64         // Length in bytes of the data block.
	NRows   uint64         // Number of rows.
	NCols   uint8          // Number of columns.
	CTBytes []uint8        // Number of bytes per column type (i.e. 1 for uint8_t, 2 for uint16_t, 4 for uint32_t, 8 for uint64_t)
	Index   []uint64       // Index of the offsets to the beginning of each column.
}

// castCTMMFileToGo convert C.mmfile_t to GO TMMFile.
func castCTMMFileToGo(mf C.mmfile_t) TMMFile {
	ncols := uint8(mf.ncols)
	ctbytes := make([]uint8, ncols)
	index := make([]uint64, ncols)
	var i uint8
	for i = 0; i < ncols; i++ {
		ctbytes[i] = uint8(mf.ctbytes[i])
		index[i] = uint64(mf.index[i])
	}
	return TMMFile{
		Src:     unsafe.Pointer(mf.src), // #nosec
		Fd:      int(mf.fd),
		Size:    uint64(mf.size),
		DOffset: uint64(mf.doffset),
		DLength: uint64(mf.dlength),
		NRows:   uint64(mf.nrows),
		NCols:   ncols,
		CTBytes: ctbytes,
		Index:   index,
	}
}

// castGoTMMFileToC convert GO TMMFile to C.mmfile_t.
func castGoTMMFileToC(mf TMMFile) C.mmfile_t {
	var cmf C.mmfile_t
	cmf.src = (*C.uint8_t)(mf.Src)
	cmf.fd = C.int(mf.Fd)
	cmf.size = C.uint64_t(mf.Size)
	cmf.doffset = C.uint64_t(mf.DOffset)
	cmf.dlength = C.uint64_t(mf.DLength)
	cmf.nrows = C.uint64_t(mf.NRows)
	cmf.ncols = C.uint8_t(mf.NCols)
	if len(mf.CTBytes) > 0 {
		cmf.ctbytes = *(*[256]C.uint8_t)(unsafe.Pointer(&mf.CTBytes[0]))
		cmf.index = *(*[256]C.uint64_t)(unsafe.Pointer(&mf.Index[0]))
	}
	return cmf
}

// Close Unmap and close the memory-mapped file.
func (mf TMMFile) Close() error {
	e := int(C.munmap_binfile(castGoTMMFileToC(mf)))
	if e != 0 {
		return fmt.Errorf("got %d error while unmapping the file", e)
	}
	return nil
}

// --- RSIDVAR ---

// RSIDVARCols contains the RSVK or VKRS memory mapped file column info.
type RSIDVARCols struct {
	Vk    unsafe.Pointer // Pointer to the VariantKey column.
	Rs    unsafe.Pointer // Pointer to the rsID column.
	NRows uint64         // Number of rows.
}

// castCRSIDVARColsToGo convert C.rsidvar_cols_t to GO RSIDVARCols.
func castCRSIDVARColsToGo(crv C.rsidvar_cols_t) RSIDVARCols {
	return RSIDVARCols{
		Vk:    unsafe.Pointer(crv.vk), // #nosec
		Rs:    unsafe.Pointer(crv.rs), // #nosec
		NRows: uint64(crv.nrows),
	}
}

// castGoRSIDVARColsToC convert GO RSIDVARCols to C.rsidvar_cols_t.
func castGoRSIDVARColsToC(rc RSIDVARCols) C.rsidvar_cols_t {
	var rvc C.rsidvar_cols_t
	rvc.vk = (*C.uint64_t)(rc.Vk)
	rvc.rs = (*C.uint32_t)(rc.Rs)
	rvc.nrows = C.uint64_t(rc.NRows)
	return rvc
}

// MmapVKRSFile memory map the VKRS binary file.
func MmapVKRSFile(file string, ctbytes []uint8) (TMMFile, RSIDVARCols, error) {
	bfile := StringToNTBytes(file)
	flen := len(bfile)
	var p unsafe.Pointer
	if flen > 0 {
		p = unsafe.Pointer(&bfile[0]) // #nosec
	}
	var mf C.mmfile_t
	mf.ncols = C.uint8_t(len(ctbytes))
	for k, v := range ctbytes {
		mf.ctbytes[k] = C.uint8_t(v)
	}
	var rc C.rsidvar_cols_t
	C.mmap_vkrs_file((*C.char)(p), &mf, &rc)
	if mf.fd < 0 || mf.size == 0 || mf.src == nil {
		return TMMFile{}, RSIDVARCols{}, fmt.Errorf("unable to map the file: %s", file)
	}
	return castCTMMFileToGo(mf), castCRSIDVARColsToGo(rc), nil
}

// MmapRSVKFile memory map the RSVK binary file.
func MmapRSVKFile(file string, ctbytes []uint8) (TMMFile, RSIDVARCols, error) {
	bfile := StringToNTBytes(file)
	flen := len(bfile)
	var p unsafe.Pointer
	if flen > 0 {
		p = unsafe.Pointer(&bfile[0]) // #nosec
	}
	var mf C.mmfile_t
	mf.ncols = C.uint8_t(len(ctbytes))
	for k, v := range ctbytes {
		mf.ctbytes[k] = C.uint8_t(v)
	}
	var rc C.rsidvar_cols_t
	C.mmap_rsvk_file((*C.char)(p), &mf, &rc)
	if mf.fd < 0 || mf.size == 0 || mf.src == nil {
		return TMMFile{}, RSIDVARCols{}, fmt.Errorf("unable to map the file: %s", file)
	}
	return castCTMMFileToGo(mf), castCRSIDVARColsToGo(rc), nil
}

// FindRVVariantKeyByRsid search for the specified RSID and returns the first occurrence of VariantKey in the RV file.
func (crv RSIDVARCols) FindRVVariantKeyByRsid(first, last uint64, rsid uint32) (uint64, uint64) {
	cfirst := C.uint64_t(first)
	vk := uint64(C.find_rv_variantkey_by_rsid(castGoRSIDVARColsToC(crv), &cfirst, C.uint64_t(last), C.uint32_t(rsid)))
	return vk, uint64(cfirst)
}

// GetNextRVVariantKeyByRsid get the next VariantKey for the specified rsID in the RV file.
// Returns the VariantKey or 0, and the position
func (crv RSIDVARCols) GetNextRVVariantKeyByRsid(pos, last uint64, rsid uint32) (uint64, uint64) {
	cpos := C.uint64_t(pos)
	vk := uint64(C.get_next_rv_variantkey_by_rsid(castGoRSIDVARColsToC(crv), &cpos, C.uint64_t(last), C.uint32_t(rsid)))
	return vk, uint64(cpos)
}

// FindAllRVVariantKeyByRsid get all VariantKeys for the specified rsID in the RV file.
// Returns a list of VariantKeys
func (crv RSIDVARCols) FindAllRVVariantKeyByRsid(first, last uint64, rsid uint32) (vks []uint64) {
	ccr := castGoRSIDVARColsToC(crv)
	cfirst := C.uint64_t(first)
	clast := C.uint64_t(last)
	crsid := C.uint32_t(rsid)
	vk := uint64(C.find_rv_variantkey_by_rsid(ccr, &cfirst, clast, crsid))
	for vk > 0 {
		vks = append(vks, vk)
		vk = uint64(C.get_next_rv_variantkey_by_rsid(ccr, &cfirst, clast, crsid))
	}
	return
}

// FindVRRsidByVariantKey search for the specified VariantKey and returns the first occurrence of RSID in the VR file.
func (crv RSIDVARCols) FindVRRsidByVariantKey(first uint64, last uint64, vk uint64) (uint32, uint64) {
	cfirst := C.uint64_t(first)
	rsid := uint32(C.find_vr_rsid_by_variantkey(castGoRSIDVARColsToC(crv), &cfirst, C.uint64_t(last), C.uint64_t(vk)))
	return rsid, uint64(cfirst)
}

// FindVRChromPosRange search for the specified CHROM-POS range and returns the first occurrence of RSID in the VR file.
func (crv RSIDVARCols) FindVRChromPosRange(first, last uint64, chrom uint8, posMin, posMax uint32) (uint32, uint64, uint64) {
	cfirst := C.uint64_t(first)
	clast := C.uint64_t(last)
	rsid := uint32(C.find_vr_chrompos_range(castGoRSIDVARColsToC(crv), &cfirst, &clast, C.uint8_t(chrom), C.uint32_t(posMin), C.uint32_t(posMax)))
	return rsid, uint64(cfirst), uint64(clast)
}

// --- NRVK ---

// NRVKCols contains the NRVK memory mapped file column info.
type NRVKCols struct {
	Vk     unsafe.Pointer // Pointer to the VariantKey column.
	Offset unsafe.Pointer // Pointer to the Offset column.
	Data   unsafe.Pointer // Pointer to the Data column.
	NRows  uint64         // Number of rows.
}

// castCNRVKColsToGo convert C.nrvk_cols_t to GO NRVKCols.
func castCNRVKColsToGo(nr C.nrvk_cols_t) NRVKCols {
	return NRVKCols{
		Vk:     unsafe.Pointer(nr.vk),     // #nosec
		Offset: unsafe.Pointer(nr.offset), // #nosec
		Data:   unsafe.Pointer(nr.data),   // #nosec
		NRows:  uint64(nr.nrows),
	}
}

// castGoNRVKColsToC convert GO NRVKCols to C.nrvk_cols_t.
func castGoNRVKColsToC(nr NRVKCols) C.nrvk_cols_t {
	var cnr C.nrvk_cols_t
	cnr.vk = (*C.uint64_t)(nr.Vk)
	cnr.offset = (*C.uint64_t)(nr.Offset)
	cnr.data = (*C.uint8_t)(nr.Data)
	cnr.nrows = C.uint64_t(nr.NRows)
	return cnr
}

// MmapNRVKFile memory map the NRVK binary file.
func MmapNRVKFile(file string) (TMMFile, NRVKCols, error) {
	bfile := StringToNTBytes(file)
	flen := len(bfile)
	var p unsafe.Pointer
	if flen > 0 {
		p = unsafe.Pointer(&bfile[0]) // #nosec
	}
	var mf C.mmfile_t
	var rc C.nrvk_cols_t
	C.mmap_nrvk_file((*C.char)(p), &mf, &rc)
	if mf.fd < 0 || mf.size == 0 || mf.src == nil {
		return TMMFile{}, NRVKCols{}, fmt.Errorf("unable to map the file: %s", file)
	}
	return castCTMMFileToGo(mf), castCNRVKColsToGo(rc), nil
}

// FindRefAltByVariantKey retrieve the REF and ALT strings for the specified VariantKey.
func (nr NRVKCols) FindRefAltByVariantKey(vk uint64) (string, string, uint8, uint8, uint32) {
	cref := C.malloc(256)
	defer C.free(unsafe.Pointer(cref)) // #nosec
	calt := C.malloc(256)
	defer C.free(unsafe.Pointer(calt)) // #nosec
	csizeref := C.size_t(0)
	csizealt := C.size_t(0)
	len := C.find_ref_alt_by_variantkey(castGoNRVKColsToC(nr), C.uint64_t(vk), (*C.char)(cref), &csizeref, (*C.char)(calt), &csizealt)
	return C.GoStringN((*C.char)(cref), C.int(csizeref)), C.GoStringN((*C.char)(calt), C.int(csizealt)), uint8(csizeref), uint8(csizealt), uint32(len)
}

// ReverseVariantKey reverse a VariantKey code and returns the normalized components.
func (nr NRVKCols) ReverseVariantKey(vk uint64) (TVariantKeyRev, uint32) {
	var rev C.variantkey_rev_t
	len := C.reverse_variantkey(castGoNRVKColsToC(nr), C.uint64_t(vk), &rev)
	return castCVariantKeyRev(rev), uint32(len)
}

// GetVariantKeyRefLength retrieve the REF length for the specified VariantKey.
func (nr NRVKCols) GetVariantKeyRefLength(vk uint64) uint8 {
	return uint8(C.get_variantkey_ref_length(castGoNRVKColsToC(nr), C.uint64_t(vk)))
}

// GetVariantKeyEndPos get the VariantKey end position (POS + REF length).
func (nr NRVKCols) GetVariantKeyEndPos(vk uint64) uint32 {
	return uint32(C.get_variantkey_endpos(castGoNRVKColsToC(nr), C.uint64_t(vk)))
}

// GetVariantKeyChromStartPos get the CHROM + START POS encoding from VariantKey.
func GetVariantKeyChromStartPos(vk uint64) uint64 {
	return uint64(C.get_variantkey_chrom_startpos(C.uint64_t(vk)))
}

// GetVariantKeyChromEndPos get the CHROM + END POS encoding from VariantKey.
func (nr NRVKCols) GetVariantKeyChromEndPos(vk uint64) uint64 {
	return uint64(C.get_variantkey_chrom_endpos(castGoNRVKColsToC(nr), C.uint64_t(vk)))
}

// VknrBinToTSV converts a vrnr.bin file to a simple TSV. For the reverse operation see the resources/tools/nrvk.sh script.
func (nr NRVKCols) VknrBinToTSV(tsvfile string) uint64 {
	file := StringToNTBytes(tsvfile)
	pfile := unsafe.Pointer(&file[0]) // #nosec
	return uint64(C.nrvk_bin_to_tsv(castGoNRVKColsToC(nr), (*C.char)(pfile)))
}

// --- GENOREF ---

// MmapGenorefFile maps the specified fasta file in memory.
func MmapGenorefFile(file string) (TMMFile, error) {
	bfile := StringToNTBytes(file)
	flen := len(bfile)
	var p unsafe.Pointer
	if flen > 0 {
		p = unsafe.Pointer(&bfile[0]) // #nosec
	}
	var mf C.mmfile_t
	C.mmap_genoref_file((*C.char)(p), &mf)
	if mf.fd < 0 || mf.size == 0 || mf.src == nil {
		return TMMFile{}, fmt.Errorf("unable to map the file: %s", file)
	}
	return castCTMMFileToGo(mf), nil
}

// FlipAllele flips allele nucleotides.
func FlipAllele(allele string) string {
	ballele := StringToNTBytes(allele)
	size := len(allele)
	pallele := unsafe.Pointer(&ballele[0]) // #nosec
	C.flip_allele((*C.char)(pallele), C.size_t(size))
	return C.GoString((*C.char)(pallele))
}

// GetGenorefSeq returns the nucleotide at the specified chromosome and position.
func (mf TMMFile) GetGenorefSeq(chrom uint8, pos uint32) byte {
	return byte(C.get_genoref_seq(castGoTMMFileToC(mf), C.uint8_t(chrom), C.uint32_t(pos))) // #nosec
}

// CheckReference checks if the reference allele matches the reference genome data.
func (mf TMMFile) CheckReference(chrom uint8, pos uint32, ref string) int {
	bref := StringToNTBytes(ref)
	pref := unsafe.Pointer(&bref[0])                                                                                            // #nosec
	return int(C.check_reference(castGoTMMFileToC(mf), C.uint8_t(chrom), C.uint32_t(pos), (*C.char)(pref), C.size_t(len(ref)))) // #nosec
}

// NormalizeVariant flips alleles if required and apply the normalization algorithm described at: https://genome.sph.umich.edu/wiki/Variant_Normalization
func (mf TMMFile) NormalizeVariant(chrom uint8, pos uint32, ref string, alt string) (code int, npos uint32, nref, nalt string, nsizeref, nsizealt uint8) {
	bref := StringToNTBytesN(ref, 256)
	balt := StringToNTBytesN(alt, 256)
	sizeref := len(ref)
	sizealt := len(alt)
	pref := unsafe.Pointer(&bref[0]) // #nosec
	palt := unsafe.Pointer(&balt[0]) // #nosec
	cpos := C.uint32_t(pos)
	csizeref := C.size_t(sizeref)
	csizealt := C.size_t(sizealt)
	code = int(C.normalize_variant(castGoTMMFileToC(mf), C.uint8_t(chrom), &cpos, (*C.char)(pref), &csizeref, (*C.char)(palt), &csizealt)) // #nosec
	npos = uint32(cpos)
	nref = C.GoString((*C.char)(pref))
	nalt = C.GoString((*C.char)(palt))
	nsizeref = uint8(csizeref)
	nsizealt = uint8(csizealt)
	return
}

// --- REGIONKEY ---

// TRegionKey contains a representation of a genetic region key
type TRegionKey struct {
	Chrom    uint8  `json:"chrom"`
	StartPos uint32 `json:"startpos"`
	EndPos   uint32 `json:"endpos"`
	Strand   uint8  `json:"strand"`
}

// TRegionKeyRev contains a genetic region components
type TRegionKeyRev struct {
	Chrom    string `json:"chrom"`
	StartPos uint32 `json:"startpos"`
	EndPos   uint32 `json:"endpos"`
	Strand   int8   `json:"strand"`
}

// castCRegionKey convert C regionkey_t to GO TRegionKey.
func castCRegionKey(rk C.regionkey_t) TRegionKey {
	return TRegionKey{
		Chrom:    uint8(rk.chrom),
		StartPos: uint32(rk.startpos),
		EndPos:   uint32(rk.endpos),
		Strand:   uint8(rk.strand),
	}
}

// castCRegionKeyRev convert C regionkey_t to GO TRegionKey.
func castCRegionKeyRev(rk C.regionkey_rev_t) TRegionKeyRev {
	return TRegionKeyRev{
		Chrom:    C.GoString((*C.char)(unsafe.Pointer(&rk.chrom[0]))),
		StartPos: uint32(rk.startpos),
		EndPos:   uint32(rk.endpos),
		Strand:   int8(rk.strand),
	}
}

// EncodeRegionStrand encode the strand direction (-1 > 2, 0 > 0, +1 > 1).
func EncodeRegionStrand(strand int8) uint8 {
	return uint8(C.encode_region_strand(C.int8_t(strand)))
}

// DecodeRegionStrand decode the strand direction code (0 > 0, 1 > +1, 2 > -1).
func DecodeRegionStrand(strand uint8) int8 {
	return int8(C.decode_region_strand(C.uint8_t(strand)))
}

// EncodeRegionKey returns  a 64 bit regionkey
func EncodeRegionKey(chrom uint8, startpos, endpos uint32, strand uint8) uint64 {
	return uint64(C.encode_regionkey(C.uint8_t(chrom), C.uint32_t(startpos), C.uint32_t(endpos), C.uint8_t(strand)))
}

// ExtractRegionKeyChrom extracts the CHROM code from RegionKey.
func ExtractRegionKeyChrom(rk uint64) uint8 {
	return uint8(C.extract_regionkey_chrom(C.uint64_t(rk)))
}

// ExtractRegionKeyStartPos extracts the START POS code from RegionKey.
func ExtractRegionKeyStartPos(rk uint64) uint32 {
	return uint32(C.extract_regionkey_startpos(C.uint64_t(rk)))
}

// ExtractRegionKeyEndPos extracts the END POS code from RegionKey.
func ExtractRegionKeyEndPos(rk uint64) uint32 {
	return uint32(C.extract_regionkey_endpos(C.uint64_t(rk)))
}

// ExtractRegionKeyStrand extract the STRAND from RegionKey.
func ExtractRegionKeyStrand(rk uint64) uint8 {
	return uint8(C.extract_regionkey_strand(C.uint64_t(rk)))
}

// DecodeRegionKey parses a regionkey string and returns the components as TRegionKey structure.
func DecodeRegionKey(rk uint64) TRegionKey {
	var drk C.regionkey_t
	C.decode_regionkey(C.uint64_t(rk), &drk)
	return castCRegionKey(drk)
}

// ReverseRegionKey parses a regionkey string and returns the components.
func ReverseRegionKey(rk uint64) TRegionKeyRev {
	var rrk C.regionkey_rev_t
	C.reverse_regionkey(C.uint64_t(rk), &rrk)
	return castCRegionKeyRev(rrk)
}

// RegionKey returns a 64 bit regionkey based on CHROM, START POS (0-based), END POS and STRAND.
func RegionKey(chrom string, startpos, endpos uint32, strand int8) uint64 {
	bchrom := StringToNTBytes(chrom)
	pchrom := unsafe.Pointer(&bchrom[0]) // #nosec
	return uint64(C.regionkey((*C.char)(pchrom), C.size_t(len(chrom)), C.uint32_t(startpos), C.uint32_t(endpos), C.int8_t(strand)))
}

// GetRegionKeyChromStartPos get the CHROM + START POS encoding from RegionKey.
func GetRegionKeyChromStartPos(rk uint64) uint64 {
	return uint64(C.get_regionkey_chrom_startpos(C.uint64_t(rk)))
}

// GetRegionKeyChromEndPos get the CHROM + END POS encoding from RegionKey.
func GetRegionKeyChromEndPos(rk uint64) uint64 {
	return uint64(C.get_regionkey_chrom_endpos(C.uint64_t(rk)))
}

// AreOverlappingRegions check if two regions are overlapping.
func AreOverlappingRegions(chromA uint8, startposA, endposA uint32, chromB uint8, startposB, endposB uint32) bool {
	return (uint8(C.are_overlapping_regions(C.uint8_t(chromA), C.uint32_t(startposA), C.uint32_t(endposA), C.uint8_t(chromB), C.uint32_t(startposB), C.uint32_t(endposB))) != 0)
}

// AreOverlappingRegionRegionKey check if a region and a regionkey are overlapping.
func AreOverlappingRegionRegionKey(chrom uint8, startpos, endpos uint32, rk uint64) bool {
	return (uint8(C.are_overlapping_region_regionkey(C.uint8_t(chrom), C.uint32_t(startpos), C.uint32_t(endpos), C.uint64_t(rk))) != 0)
}

// AreOverlappingRegionKeys check if two regionkeys are overlapping.
func AreOverlappingRegionKeys(rka, rkb uint64) bool {
	return (uint8(C.are_overlapping_regionkeys(C.uint64_t(rka), C.uint64_t(rkb))) != 0)
}

// AreOverlappingVariantKeyRegionKey check if variantkey and regionkey are overlapping.
func (nr NRVKCols) AreOverlappingVariantKeyRegionKey(vk, rk uint64) bool {
	return (uint8(C.are_overlapping_variantkey_regionkey(castGoNRVKColsToC(nr), C.uint64_t(vk), C.uint64_t(rk))) != 0)
}

// VariantToRegionkey get RegionKey from VariantKey.
func (nr NRVKCols) VariantToRegionkey(vk uint64) uint64 {
	return uint64(C.variantkey_to_regionkey(castGoNRVKColsToC(nr), C.uint64_t(vk)))
}

// --- ESID ---

// EncodeStringID encode maximum 10 characters of a string into a 64 bit unsigned integer. The argument "start" indicate the first character to encode.
func EncodeStringID(s string, start uint32) uint64 {
	bs := StringToNTBytes(s)
	ps := unsafe.Pointer(&bs[0]) // #nosec
	return uint64(C.encode_string_id((*C.char)(ps), C.size_t(len(s)), C.size_t(start)))
}

// EncodeStringNumID cncode a string composed by a character section followed by a separator character and a
// numerical section into a 64 bit unsigned integer. For example: "ABCDE:0001234".
// Encodes up to 5 characters in uppercase, a number up to 2^27, and up to 7 zero padding digits.
// If the string is 10 character or less, then the encode_string_id() is used.
func EncodeStringNumID(s string, sep byte) uint64 {
	bs := StringToNTBytes(s)
	ps := unsafe.Pointer(&bs[0]) // #nosec
	return uint64(C.encode_string_num_id((*C.char)(ps), C.size_t(len(s)), C.char(sep)))
}

// DecodeStringID decode the encoded string ID.
func DecodeStringID(esid uint64) string {
	cstr := C.malloc(23)
	defer C.free(unsafe.Pointer(cstr)) // #nosec
	len := C.decode_string_id(C.uint64_t(esid), (*C.char)(cstr))
	return C.GoStringN((*C.char)(cstr), C.int(len))
}

// HashStringID hash the input string into a 64 bit unsigned integer.
func HashStringID(s string) uint64 {
	bs := StringToNTBytes(s)
	ps := unsafe.Pointer(&bs[0]) // #nosec
	return uint64(C.hash_string_id((*C.char)(ps), C.size_t(len(s))))
}
