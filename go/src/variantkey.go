package variantkey

/*
#include <stdlib.h>
#include <inttypes.h>
#include "../../src/variantkey.h"
#include "../../src/variantkey.c"
#include "../../src/binsearch.h"
#include "../../src/binsearch.c"
#include "../../src/rsidvar.h"
#include "../../src/rsidvar.c"
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

// castCVariantKey convert C variantkey_t to GO TVariantKey.
func castCVariantKey(vh C.variantkey_t) TVariantKey {
	return TVariantKey{
		Chrom:  uint8(vh.chrom),
		Pos:    uint32(vh.pos),
		RefAlt: uint32(vh.refalt),
	}
}

// StringToNTBytes safely convert a string to byte array with an extra null terminator
// This is to ensure a correct CGO conversion to char*
func StringToNTBytes(s string) []byte {
	b := make([]byte, len(s)+1)
	copy(b[:], s)
	return b
}

// EncodeChrom returns chromosome encoding.
func EncodeChrom(chrom string) uint8 {
	bchrom := StringToNTBytes(chrom)
	sizechrom := len(chrom)
	var pchrom unsafe.Pointer
	if sizechrom > 0 {
		pchrom = unsafe.Pointer(&bchrom[0]) // #nosec
	}
	return uint8(C.encode_chrom((*C.char)(pchrom), C.size_t(sizechrom)))
}

// DecodeChrom decode chrom to string
func DecodeChrom(c uint8) string {
	var cstr *C.char = C.CString("000")
	defer C.free(unsafe.Pointer(cstr)) // #nosec
	len := C.decode_chrom(C.uint8_t(c), cstr)
	return C.GoStringN(cstr, C.int(len))
}

// EncodeRefAlt returns reference+alternate code.
func EncodeRefAlt(ref string, alt string) uint32 {
	bref := StringToNTBytes(ref)
	balt := StringToNTBytes(alt)
	sizeref := len(ref)
	sizealt := len(alt)
	var pref unsafe.Pointer
	var palt unsafe.Pointer
	if sizeref > 0 {
		pref = unsafe.Pointer(&bref[0]) // #nosec
	}
	if sizealt > 0 {
		palt = unsafe.Pointer(&balt[0]) // #nosec
	}
	return uint32(C.encode_refalt((*C.char)(pref), C.size_t(sizeref), (*C.char)(palt), C.size_t(sizealt)))
}

// DecodeRefAlt decode Ref+Alt code if reversible
func DecodeRefAlt(c uint32) (string, string, uint8, uint8, uint8) {
	var cref *C.char = C.CString("000000")
	defer C.free(unsafe.Pointer(cref)) // #nosec
	var calt *C.char = C.CString("000000")
	defer C.free(unsafe.Pointer(calt)) // #nosec
	csizeref := C.size_t(0)
	csizealt := C.size_t(0)
	len := C.decode_refalt(C.uint32_t(c), cref, &csizeref, calt, &csizealt)
	return C.GoStringN(cref, C.int(csizeref)), C.GoStringN(calt, C.int(csizealt)), uint8(csizeref), uint8(csizealt), uint8(len)
}

// VariantKey returns a Genetic Variant Key based on CHROM, POS (0-base), REF, ALT.
func VariantKey(chrom string, pos uint32, ref, alt string) uint64 {
	bchrom := StringToNTBytes(chrom)
	bref := StringToNTBytes(ref)
	balt := StringToNTBytes(alt)
	sizechrom := len(chrom)
	sizeref := len(ref)
	sizealt := len(alt)
	var pchrom unsafe.Pointer
	var pref unsafe.Pointer
	var palt unsafe.Pointer
	if sizechrom > 0 {
		pchrom = unsafe.Pointer(&bchrom[0]) // #nosec
	}
	if sizeref > 0 {
		pref = unsafe.Pointer(&bref[0]) // #nosec
	}
	if sizealt > 0 {
		palt = unsafe.Pointer(&balt[0]) // #nosec
	}
	return uint64(C.variantkey((*C.char)(pchrom), C.size_t(len(chrom)), C.uint32_t(pos), (*C.char)(pref), C.size_t(len(ref)), (*C.char)(palt), C.size_t(len(alt))))
}

// VariantKeyString provides a string representation of the VariantKey 64bit
func VariantKeyString(v uint64) string {
	var cstr *C.char = C.CString("00000000000000000")
	defer C.free(unsafe.Pointer(cstr)) // #nosec
	C.variantkey_string(C.uint64_t(v), cstr)
	return C.GoStringN(cstr, C.int(16))
}

// ParseVariantKeyString parses a variant key string and returns the code.
func ParseVariantKeyString(s string) uint64 {
	b := StringToNTBytes(s)
	size := len(s)
	var p unsafe.Pointer
	if size > 0 {
		p = unsafe.Pointer(&b[0]) // #nosec
	}
	return uint64(C.parse_variantkey_string((*C.char)(p)))
}

// DecodeVariantKeyg parses a variant key string and returns the components as TVariantKey128 structure.
func DecodeVariantKey(v uint64) TVariantKey {
	return castCVariantKey(C.decode_variantkey(C.uint64_t(v)))
}

// --- BINSEARCH ---

// TMMFile contains the memory mapped file info
type TMMFile struct {
	Src  unsafe.Pointer
	Fd   int
	Size uint64
}

// MmapBinFile maps the specified file in memory.
func MmapBinFile(file string) (TMMFile, error) {
	bfile := StringToNTBytes(file)
	flen := len(bfile)
	var p unsafe.Pointer
	if flen > 0 {
		p = unsafe.Pointer(&bfile[0]) // #nosec
	}
	mf := (C.mmap_binfile((*C.char)(p)))
	if mf.fd < 0 || mf.size == 0 || mf.src == nil {
		return TMMFile{}, fmt.Errorf("unable to map the file: %s", file)
	}
	return TMMFile{unsafe.Pointer(mf.src), int(mf.fd), uint64(mf.size)}, nil // #nosec
}

// Close Unmap and close the memory-mapped file.
func (mf TMMFile) Close() error {
	var tmf C.mmfile_t
	tmf.src = (*C.uchar)(mf.Src)
	tmf.fd = C.int(mf.Fd)
	tmf.size = C.uint64_t(mf.Size)
	e := int(C.munmap_binfile(tmf))
	if e != 0 {
		return fmt.Errorf("got %d error while unmapping the file", e)
	}
	return nil
}

// GetAddress returns the absolute file start position of the specified item (binary block).
func GetAddress(blklen, blkpos, item uint64) uint64 {
	return ((blklen * item) + blkpos)
}

// FindFirstUint32 search for the first occurrence of a 32 bit unsigned integer on a memory mapped
// binary file containing adjacent blocks of sorted binary data.
// The 32 bit values in the file must encoded in big-endian format and sorted in ascending order.
// Return the item number if found or (last + 1) if not found, plus the first and last positions.
func (mf TMMFile) FindFirstUint32(blklen, blkpos uint64, bitstart, bitend uint8, first, last uint64, search uint32) (uint64, uint64, uint64) {
	cfirst := C.uint64_t(first)
	clast := C.uint64_t(last)
	ret := uint64(C.find_first_uint32_t((*C.uchar)(mf.Src), C.uint64_t(blklen), C.uint64_t(blkpos), C.uint8_t(bitstart), C.uint8_t(bitend), &cfirst, &clast, C.uint32_t(search)))
	return ret, uint64(cfirst), uint64(clast)
}

// FindLastUint32 search for the last occurrence of a 32 bit unsigned integer on a memory mapped
// binary file containing adjacent blocks of sorted binary data.
// The 32 bit values in the file must encoded in big-endian format and sorted in ascending order.
// Return the item number if found or (last + 1) if not found, plus the first and last positions.
func (mf TMMFile) FindLastUint32(blklen, blkpos uint64, bitstart, bitend uint8, first, last uint64, search uint32) (uint64, uint64, uint64) {
	cfirst := C.uint64_t(first)
	clast := C.uint64_t(last)
	ret := uint64(C.find_last_uint32_t((*C.uchar)(mf.Src), C.uint64_t(blklen), C.uint64_t(blkpos), C.uint8_t(bitstart), C.uint8_t(bitend), &cfirst, &clast, C.uint32_t(search)))
	return ret, uint64(cfirst), uint64(clast)
}

// FindFirstUint64 search for the first occurrence of a 64 bit unsigned integer on a memory mapped
// binary file containing adjacent blocks of sorted binary data.
// The 64 bit values in the file must encoded in big-endian format and sorted in ascending order.
// Return the item number if found or (last + 1) if not found, plus the first and last positions.
func (mf TMMFile) FindFirstUint64(blklen, blkpos uint64, bitstart, bitend uint8, first, last uint64, search uint64) (uint64, uint64, uint64) {
	cfirst := C.uint64_t(first)
	clast := C.uint64_t(last)
	ret := uint64(C.find_first_uint64_t((*C.uchar)(mf.Src), C.uint64_t(blklen), C.uint64_t(blkpos), C.uint8_t(bitstart), C.uint8_t(bitend), &cfirst, &clast, C.uint64_t(search)))
	return ret, uint64(cfirst), uint64(clast)
}

// FindLastUint64 search for the last occurrence of a 64 bit unsigned integer on a memory mapped
// binary file containing adjacent blocks of sorted binary data.
// The 64 bit values in the file must encoded in big-endian format and sorted in ascending order.
// Return the item number if found or (last + 1) if not found, plus the first and last positions.
func (mf TMMFile) FindLastUint64(blklen, blkpos uint64, bitstart, bitend uint8, first, last uint64, search uint64) (uint64, uint64, uint64) {
	cfirst := C.uint64_t(first)
	clast := C.uint64_t(last)
	ret := uint64(C.find_last_uint64_t((*C.uchar)(mf.Src), C.uint64_t(blklen), C.uint64_t(blkpos), C.uint8_t(bitstart), C.uint8_t(bitend), &cfirst, &clast, C.uint64_t(search)))
	return ret, uint64(cfirst), uint64(clast)
}

// --- RSIDVAR ---

// GetVRRsid returns the RSID at the specified position in the VR file.
func (mf TMMFile) GetVRRsid(item uint64) uint32 {
	return uint32(C.get_vr_rsid((*C.uchar)(mf.Src), C.uint64_t(item)))
}

// GetRVVariantkey returns the VariantKey at the specified position in the RV file.
func (mf TMMFile) GetRVVariantkey(item uint64) uint64 {
	return uint64(C.get_rv_variantkey((*C.uchar)(mf.Src), C.uint64_t(item)))
}

// FindRVVariantkeyByRsid search for the specified RSID and returns the first occurrence of VariantKey in the RV file.
func (mf TMMFile) FindRVVariantkeyByRsid(first, last uint64, rsid uint32) (uint64, uint64) {
	cfirst := C.uint64_t(first)
	vh := uint64(C.find_rv_variantkey_by_rsid((*C.uchar)(mf.Src), &cfirst, C.uint64_t(last), C.uint32_t(rsid)))
	return vh, uint64(cfirst)
}

// FindVRRsidByVarshash search for the specified VariantKey and returns the first occurrence of RSID in the VR file.
func (mf TMMFile) FindVRRsidByVariantkey(first uint64, last uint64, vk uint64) (uint32, uint64) {
	cfirst := C.uint64_t(first)
	rsid := uint32(C.find_vr_rsid_by_variantkey((*C.uchar)(mf.Src), &cfirst, C.uint64_t(last), C.uint64_t(vk)))
	return rsid, uint64(cfirst)
}

// FindVRChromPosRange search for the specified CHROM-POS range and returns the first occurrence of RSID in the VR file.
func (mf TMMFile) FindVRChromPosRange(first, last uint64, chrom uint8, posStart, posEnd uint32) (uint32, uint64, uint64) {
	cfirst := C.uint64_t(first)
	clast := C.uint64_t(last)
	rsid := uint32(C.find_vr_chrompos_range((*C.uchar)(mf.Src), &cfirst, &clast, C.uint8_t(chrom), C.uint32_t(posStart), C.uint32_t(posEnd)))
	return rsid, uint64(cfirst), uint64(clast)
}
