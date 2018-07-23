package variantkey

/*
#include <stdlib.h>
#include <inttypes.h>
#include "../../c/src/astring.h"
#include "../../c/src/astring.c"
#include "../../c/src/binsearch.h"
#include "../../c/src/binsearch.c"
#include "../../c/src/genoref.h"
#include "../../c/src/genoref.c"
#include "../../c/src/nrvk.h"
#include "../../c/src/nrvk.c"
#include "../../c/src/rsidvar.h"
#include "../../c/src/rsidvar.c"
#include "../../c/src/variantkey.h"
#include "../../c/src/variantkey.c"
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

// Hex provides a string representation of the VariantKey 64bit
func Hex(v uint64) string {
	cstr := C.malloc(17)
	defer C.free(unsafe.Pointer(cstr)) // #nosec
	C.variantkey_hex(C.uint64_t(v), (*C.char)(cstr))
	return C.GoStringN((*C.char)(cstr), C.int(16))
}

// ParseHex parses a variant key string and returns the code.
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
	Src   unsafe.Pointer
	Fd    int
	Size  uint64
	Last  uint64
	Index []uint32
}

// MmapBinFile maps the specified file in memory.
func MmapBinFile(file string) (TMMFile, error) {
	bfile := StringToNTBytes(file)
	p := unsafe.Pointer(&bfile[0]) // #nosec
	var mf C.mmfile_t
	C.mmap_binfile((*C.char)(p), &mf)
	if mf.fd < 0 || mf.size == 0 || mf.src == nil {
		return TMMFile{}, fmt.Errorf("unable to map the file: %s", file)
	}
	return TMMFile{unsafe.Pointer(mf.src), int(mf.fd), uint64(mf.size), uint64(mf.last), []uint32{}}, nil // #nosec
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

// FindFirstUint8 search for the first occurrence of a 8 bit unsigned integer on a memory mapped
// binary file containing adjacent blocks of sorted binary data.
// The 8 bit values in the file must encoded in big-endian format and sorted in ascending order.
// Return the item number if found or (last + 1) if not found, plus the first and last positions.
func (mf TMMFile) FindFirstUint8(blklen, blkpos, first, last uint64, search uint8) (uint64, uint64, uint64) {
	cfirst := C.uint64_t(first)
	clast := C.uint64_t(last)
	ret := uint64(C.find_first_uint8_t((*C.uchar)(mf.Src), C.uint64_t(blklen), C.uint64_t(blkpos), &cfirst, &clast, C.uint8_t(search)))
	return ret, uint64(cfirst), uint64(clast)
}

// FindLastUint8 search for the last occurrence of a 8 bit unsigned integer on a memory mapped
// binary file containing adjacent blocks of sorted binary data.
// The 8 bit values in the file must encoded in big-endian format and sorted in ascending order.
// Return the item number if found or (last + 1) if not found, plus the first and last positions.
func (mf TMMFile) FindLastUint8(blklen, blkpos, first, last uint64, search uint8) (uint64, uint64, uint64) {
	cfirst := C.uint64_t(first)
	clast := C.uint64_t(last)
	ret := uint64(C.find_last_uint8_t((*C.uchar)(mf.Src), C.uint64_t(blklen), C.uint64_t(blkpos), &cfirst, &clast, C.uint8_t(search)))
	return ret, uint64(cfirst), uint64(clast)
}

// FindFirstUint16 search for the first occurrence of a 16 bit unsigned integer on a memory mapped
// binary file containing adjacent blocks of sorted binary data.
// The 16 bit values in the file must encoded in big-endian format and sorted in ascending order.
// Return the item number if found or (last + 1) if not found, plus the first and last positions.
func (mf TMMFile) FindFirstUint16(blklen, blkpos, first, last uint64, search uint16) (uint64, uint64, uint64) {
	cfirst := C.uint64_t(first)
	clast := C.uint64_t(last)
	ret := uint64(C.find_first_uint16_t((*C.uchar)(mf.Src), C.uint64_t(blklen), C.uint64_t(blkpos), &cfirst, &clast, C.uint16_t(search)))
	return ret, uint64(cfirst), uint64(clast)
}

// FindLastUint16 search for the last occurrence of a 16 bit unsigned integer on a memory mapped
// binary file containing adjacent blocks of sorted binary data.
// The 16 bit values in the file must encoded in big-endian format and sorted in ascending order.
// Return the item number if found or (last + 1) if not found, plus the first and last positions.
func (mf TMMFile) FindLastUint16(blklen, blkpos, first, last uint64, search uint16) (uint64, uint64, uint64) {
	cfirst := C.uint64_t(first)
	clast := C.uint64_t(last)
	ret := uint64(C.find_last_uint16_t((*C.uchar)(mf.Src), C.uint64_t(blklen), C.uint64_t(blkpos), &cfirst, &clast, C.uint16_t(search)))
	return ret, uint64(cfirst), uint64(clast)
}

// FindFirstUint32 search for the first occurrence of a 32 bit unsigned integer on a memory mapped
// binary file containing adjacent blocks of sorted binary data.
// The 32 bit values in the file must encoded in big-endian format and sorted in ascending order.
// Return the item number if found or (last + 1) if not found, plus the first and last positions.
func (mf TMMFile) FindFirstUint32(blklen, blkpos, first, last uint64, search uint32) (uint64, uint64, uint64) {
	cfirst := C.uint64_t(first)
	clast := C.uint64_t(last)
	ret := uint64(C.find_first_uint32_t((*C.uchar)(mf.Src), C.uint64_t(blklen), C.uint64_t(blkpos), &cfirst, &clast, C.uint32_t(search)))
	return ret, uint64(cfirst), uint64(clast)
}

// FindLastUint32 search for the last occurrence of a 32 bit unsigned integer on a memory mapped
// binary file containing adjacent blocks of sorted binary data.
// The 32 bit values in the file must encoded in big-endian format and sorted in ascending order.
// Return the item number if found or (last + 1) if not found, plus the first and last positions.
func (mf TMMFile) FindLastUint32(blklen, blkpos, first, last uint64, search uint32) (uint64, uint64, uint64) {
	cfirst := C.uint64_t(first)
	clast := C.uint64_t(last)
	ret := uint64(C.find_last_uint32_t((*C.uchar)(mf.Src), C.uint64_t(blklen), C.uint64_t(blkpos), &cfirst, &clast, C.uint32_t(search)))
	return ret, uint64(cfirst), uint64(clast)
}

// FindFirstUint64 search for the first occurrence of a 64 bit unsigned integer on a memory mapped
// binary file containing adjacent blocks of sorted binary data.
// The 64 bit values in the file must encoded in big-endian format and sorted in ascending order.
// Return the item number if found or (last + 1) if not found, plus the first and last positions.
func (mf TMMFile) FindFirstUint64(blklen, blkpos, first, last uint64, search uint64) (uint64, uint64, uint64) {
	cfirst := C.uint64_t(first)
	clast := C.uint64_t(last)
	ret := uint64(C.find_first_uint64_t((*C.uchar)(mf.Src), C.uint64_t(blklen), C.uint64_t(blkpos), &cfirst, &clast, C.uint64_t(search)))
	return ret, uint64(cfirst), uint64(clast)
}

// FindLastUint64 search for the last occurrence of a 64 bit unsigned integer on a memory mapped
// binary file containing adjacent blocks of sorted binary data.
// The 64 bit values in the file must encoded in big-endian format and sorted in ascending order.
// Return the item number if found or (last + 1) if not found, plus the first and last positions.
func (mf TMMFile) FindLastUint64(blklen, blkpos, first, last uint64, search uint64) (uint64, uint64, uint64) {
	cfirst := C.uint64_t(first)
	clast := C.uint64_t(last)
	ret := uint64(C.find_last_uint64_t((*C.uchar)(mf.Src), C.uint64_t(blklen), C.uint64_t(blkpos), &cfirst, &clast, C.uint64_t(search)))
	return ret, uint64(cfirst), uint64(clast)
}

// FindFirstSubUint8 search for the first occurrence of a bit set contained in a 8 bit unsigned integer on a memory mapped
// binary file containing adjacent blocks of sorted binary data.
// The 8 bit values in the file must encoded in big-endian format and sorted in ascending order.
// Return the item number if found or (last + 1) if not found, plus the first and last positions.
func (mf TMMFile) FindFirstSubUint8(blklen, blkpos uint64, bitstart, bitend uint8, first, last uint64, search uint8) (uint64, uint64, uint64) {
	cfirst := C.uint64_t(first)
	clast := C.uint64_t(last)
	ret := uint64(C.find_first_sub_uint8_t((*C.uchar)(mf.Src), C.uint64_t(blklen), C.uint64_t(blkpos), C.uint8_t(bitstart), C.uint8_t(bitend), &cfirst, &clast, C.uint8_t(search)))
	return ret, uint64(cfirst), uint64(clast)
}

// FindLastSubUint8 search for the last occurrence of a bit set contained in a 8 bit unsigned integer on a memory mapped
// binary file containing adjacent blocks of sorted binary data.
// The 8 bit values in the file must encoded in big-endian format and sorted in ascending order.
// Return the item number if found or (last + 1) if not found, plus the first and last positions.
func (mf TMMFile) FindLastSubUint8(blklen, blkpos uint64, bitstart, bitend uint8, first, last uint64, search uint8) (uint64, uint64, uint64) {
	cfirst := C.uint64_t(first)
	clast := C.uint64_t(last)
	ret := uint64(C.find_last_sub_uint8_t((*C.uchar)(mf.Src), C.uint64_t(blklen), C.uint64_t(blkpos), C.uint8_t(bitstart), C.uint8_t(bitend), &cfirst, &clast, C.uint8_t(search)))
	return ret, uint64(cfirst), uint64(clast)
}

// FindFirstSubUint16 search for the first occurrence of a bit set contained in a 16 bit unsigned integer on a memory mapped
// binary file containing adjacent blocks of sorted binary data.
// The 16 bit values in the file must encoded in big-endian format and sorted in ascending order.
// Return the item number if found or (last + 1) if not found, plus the first and last positions.
func (mf TMMFile) FindFirstSubUint16(blklen, blkpos uint64, bitstart, bitend uint8, first, last uint64, search uint16) (uint64, uint64, uint64) {
	cfirst := C.uint64_t(first)
	clast := C.uint64_t(last)
	ret := uint64(C.find_first_sub_uint16_t((*C.uchar)(mf.Src), C.uint64_t(blklen), C.uint64_t(blkpos), C.uint8_t(bitstart), C.uint8_t(bitend), &cfirst, &clast, C.uint16_t(search)))
	return ret, uint64(cfirst), uint64(clast)
}

// FindLastSubUint16 search for the last occurrence of a bit set contained in a 16 bit unsigned integer on a memory mapped
// binary file containing adjacent blocks of sorted binary data.
// The 16 bit values in the file must encoded in big-endian format and sorted in ascending order.
// Return the item number if found or (last + 1) if not found, plus the first and last positions.
func (mf TMMFile) FindLastSubUint16(blklen, blkpos uint64, bitstart, bitend uint8, first, last uint64, search uint16) (uint64, uint64, uint64) {
	cfirst := C.uint64_t(first)
	clast := C.uint64_t(last)
	ret := uint64(C.find_last_sub_uint16_t((*C.uchar)(mf.Src), C.uint64_t(blklen), C.uint64_t(blkpos), C.uint8_t(bitstart), C.uint8_t(bitend), &cfirst, &clast, C.uint16_t(search)))
	return ret, uint64(cfirst), uint64(clast)
}

// FindFirstSubUint32 search for the first occurrence of a bit set contained in a 32 bit unsigned integer on a memory mapped
// binary file containing adjacent blocks of sorted binary data.
// The 32 bit values in the file must encoded in big-endian format and sorted in ascending order.
// Return the item number if found or (last + 1) if not found, plus the first and last positions.
func (mf TMMFile) FindFirstSubUint32(blklen, blkpos uint64, bitstart, bitend uint8, first, last uint64, search uint32) (uint64, uint64, uint64) {
	cfirst := C.uint64_t(first)
	clast := C.uint64_t(last)
	ret := uint64(C.find_first_sub_uint32_t((*C.uchar)(mf.Src), C.uint64_t(blklen), C.uint64_t(blkpos), C.uint8_t(bitstart), C.uint8_t(bitend), &cfirst, &clast, C.uint32_t(search)))
	return ret, uint64(cfirst), uint64(clast)
}

// FindLastSubUint32 search for the last occurrence of a bit set contained in a 32 bit unsigned integer on a memory mapped
// binary file containing adjacent blocks of sorted binary data.
// The 32 bit values in the file must encoded in big-endian format and sorted in ascending order.
// Return the item number if found or (last + 1) if not found, plus the first and last positions.
func (mf TMMFile) FindLastSubUint32(blklen, blkpos uint64, bitstart, bitend uint8, first, last uint64, search uint32) (uint64, uint64, uint64) {
	cfirst := C.uint64_t(first)
	clast := C.uint64_t(last)
	ret := uint64(C.find_last_sub_uint32_t((*C.uchar)(mf.Src), C.uint64_t(blklen), C.uint64_t(blkpos), C.uint8_t(bitstart), C.uint8_t(bitend), &cfirst, &clast, C.uint32_t(search)))
	return ret, uint64(cfirst), uint64(clast)
}

// FindFirstSubUint64 search for the first occurrence of a bit set contained in a 64 bit unsigned integer on a memory mapped
// binary file containing adjacent blocks of sorted binary data.
// The 64 bit values in the file must encoded in big-endian format and sorted in ascending order.
// Return the item number if found or (last + 1) if not found, plus the first and last positions.
func (mf TMMFile) FindFirstSubUint64(blklen, blkpos uint64, bitstart, bitend uint8, first, last uint64, search uint64) (uint64, uint64, uint64) {
	cfirst := C.uint64_t(first)
	clast := C.uint64_t(last)
	ret := uint64(C.find_first_sub_uint64_t((*C.uchar)(mf.Src), C.uint64_t(blklen), C.uint64_t(blkpos), C.uint8_t(bitstart), C.uint8_t(bitend), &cfirst, &clast, C.uint64_t(search)))
	return ret, uint64(cfirst), uint64(clast)
}

// FindLastSubUint64 search for the last occurrence of a bit set contained in a 64 bit unsigned integer on a memory mapped
// binary file containing adjacent blocks of sorted binary data.
// The 64 bit values in the file must encoded in big-endian format and sorted in ascending order.
// Return the item number if found or (last + 1) if not found, plus the first and last positions.
func (mf TMMFile) FindLastSubUint64(blklen, blkpos uint64, bitstart, bitend uint8, first, last uint64, search uint64) (uint64, uint64, uint64) {
	cfirst := C.uint64_t(first)
	clast := C.uint64_t(last)
	ret := uint64(C.find_last_sub_uint64_t((*C.uchar)(mf.Src), C.uint64_t(blklen), C.uint64_t(blkpos), C.uint8_t(bitstart), C.uint8_t(bitend), &cfirst, &clast, C.uint64_t(search)))
	return ret, uint64(cfirst), uint64(clast)
}

// HasNextUint8 checks if the next occurrence of an unsigned integer on a memory mapped
// binary file containing adjacent blocks of sorted binary data still matches the search value.
// The values in the file must encoded in big-endian format and sorted in ascending order.
// This function can be used after FindFirstUint8 to get the next elements that still satisfy the search.
// The item returned by FindFirstUint8 should be set as the "pos" parameter in this function.
// Return true if the next item is valid, false otherwise, plus the position.
func (mf TMMFile) HasNextUint8(blklen, blkpos, pos, last uint64, search uint8) (bool, uint64) {
	cpos := C.uint64_t(pos)
	ret := bool(C.has_next_uint8_t((*C.uchar)(mf.Src), C.uint64_t(blklen), C.uint64_t(blkpos), &cpos, C.uint64_t(last), C.uint8_t(search)))
	return ret, uint64(cpos)
}

// HasNextUint16 checks if the next occurrence of an unsigned integer on a memory mapped
// binary file containing adjacent blocks of sorted binary data still matches the search value.
// The values in the file must encoded in big-endian format and sorted in ascending order.
// This function can be used after FindFirstUint16 to get the next elements that still satisfy the search.
// The item returned by FindFirstUint16 should be set as the "pos" parameter in this function.
// Return true if the next item is valid, false otherwise, plus the position.
func (mf TMMFile) HasNextUint16(blklen, blkpos, pos, last uint64, search uint16) (bool, uint64) {
	cpos := C.uint64_t(pos)
	ret := bool(C.has_next_uint16_t((*C.uchar)(mf.Src), C.uint64_t(blklen), C.uint64_t(blkpos), &cpos, C.uint64_t(last), C.uint16_t(search)))
	return ret, uint64(cpos)
}

// HasNextUint32 checks if the next occurrence of an unsigned integer on a memory mapped
// binary file containing adjacent blocks of sorted binary data still matches the search value.
// The values in the file must encoded in big-endian format and sorted in ascending order.
// This function can be used after FindFirstUint32 to get the next elements that still satisfy the search.
// The item returned by FindFirstUint32 should be set as the "pos" parameter in this function.
// Return true if the next item is valid, false otherwise, plus the position.
func (mf TMMFile) HasNextUint32(blklen, blkpos, pos, last uint64, search uint32) (bool, uint64) {
	cpos := C.uint64_t(pos)
	ret := bool(C.has_next_uint32_t((*C.uchar)(mf.Src), C.uint64_t(blklen), C.uint64_t(blkpos), &cpos, C.uint64_t(last), C.uint32_t(search)))
	return ret, uint64(cpos)
}

// HasNextUint64 checks if the next occurrence of an unsigned integer on a memory mapped
// binary file containing adjacent blocks of sorted binary data still matches the search value.
// The values in the file must encoded in big-endian format and sorted in ascending order.
// This function can be used after FindFirstUint64 to get the next elements that still satisfy the search.
// The item returned by FindFirstUint64 should be set as the "pos" parameter in this function.
// Return true if the next item is valid, false otherwise, plus the position.
func (mf TMMFile) HasNextUint64(blklen, blkpos, pos, last uint64, search uint64) (bool, uint64) {
	cpos := C.uint64_t(pos)
	ret := bool(C.has_next_uint64_t((*C.uchar)(mf.Src), C.uint64_t(blklen), C.uint64_t(blkpos), &cpos, C.uint64_t(last), C.uint64_t(search)))
	return ret, uint64(cpos)
}

// HasNextSubUint8 checks if the next occurrence of an unsigned integer on a memory mapped
// binary file containing adjacent blocks of sorted binary data still matches the search value.
// The values in the file must encoded in big-endian format and sorted in ascending order.
// This function can be used after FindFirstUint8 to get the next elements that still satisfy the search.
// The item returned by FindFirstUint8 should be set as the "pos" parameter in this function.
// Return true if the next item is valid, false otherwise, plus the position.
func (mf TMMFile) HasNextSubUint8(blklen, blkpos uint64, bitstart, bitend uint8, pos, last uint64, search uint8) (bool, uint64) {
	cpos := C.uint64_t(pos)
	ret := bool(C.has_next_sub_uint8_t((*C.uchar)(mf.Src), C.uint64_t(blklen), C.uint64_t(blkpos), C.uint8_t(bitstart), C.uint8_t(bitend), &cpos, C.uint64_t(last), C.uint8_t(search)))
	return ret, uint64(cpos)
}

// HasNextSubUint16 checks if the next occurrence of an unsigned integer on a memory mapped
// binary file containing adjacent blocks of sorted binary data still matches the search value.
// The values in the file must encoded in big-endian format and sorted in ascending order.
// This function can be used after FindFirstUint16 to get the next elements that still satisfy the search.
// The item returned by FindFirstUint16 should be set as the "pos" parameter in this function.
// Return true if the next item is valid, false otherwise, plus the position.
func (mf TMMFile) HasNextSubUint16(blklen, blkpos uint64, bitstart, bitend uint8, pos, last uint64, search uint16) (bool, uint64) {
	cpos := C.uint64_t(pos)
	ret := bool(C.has_next_sub_uint16_t((*C.uchar)(mf.Src), C.uint64_t(blklen), C.uint64_t(blkpos), C.uint8_t(bitstart), C.uint8_t(bitend), &cpos, C.uint64_t(last), C.uint16_t(search)))
	return ret, uint64(cpos)
}

// HasNextSubUint32 checks if the next occurrence of an unsigned integer on a memory mapped
// binary file containing adjacent blocks of sorted binary data still matches the search value.
// The values in the file must encoded in big-endian format and sorted in ascending order.
// This function can be used after FindFirstUint32 to get the next elements that still satisfy the search.
// The item returned by FindFirstUint32 should be set as the "pos" parameter in this function.
// Return true if the next item is valid, false otherwise, plus the position.
func (mf TMMFile) HasNextSubUint32(blklen, blkpos uint64, bitstart, bitend uint8, pos, last uint64, search uint32) (bool, uint64) {
	cpos := C.uint64_t(pos)
	ret := bool(C.has_next_sub_uint32_t((*C.uchar)(mf.Src), C.uint64_t(blklen), C.uint64_t(blkpos), C.uint8_t(bitstart), C.uint8_t(bitend), &cpos, C.uint64_t(last), C.uint32_t(search)))
	return ret, uint64(cpos)
}

// HasNextSubUint64 checks if the next occurrence of an unsigned integer on a memory mapped
// binary file containing adjacent blocks of sorted binary data still matches the search value.
// The values in the file must encoded in big-endian format and sorted in ascending order.
// This function can be used after FindFirstUint64 to get the next elements that still satisfy the search.
// The item returned by FindFirstUint64 should be set as the "pos" parameter in this function.
// Return true if the next item is valid, false otherwise, plus the position.
func (mf TMMFile) HasNextSubUint64(blklen, blkpos uint64, bitstart, bitend uint8, pos, last uint64, search uint64) (bool, uint64) {
	cpos := C.uint64_t(pos)
	ret := bool(C.has_next_sub_uint64_t((*C.uchar)(mf.Src), C.uint64_t(blklen), C.uint64_t(blkpos), C.uint8_t(bitstart), C.uint8_t(bitend), &cpos, C.uint64_t(last), C.uint64_t(search)))
	return ret, uint64(cpos)
}

// HasPrevUint8 checks if the previous occurrence of an unsigned integer on a memory mapped
// binary file containing adjacent blocks of sorted binary data still matches the search value.
// The values in the file must encoded in big-endian format and sorted in ascending order.
// This function can be used after FindLastUint8 to get the previous elements that still satisfy the search.
// The item returned by FindLastUint8 should be set as the "pos" parameter in this function.
// Return true if the previous item is valid, false otherwise, plus the position.
func (mf TMMFile) HasPrevUint8(blklen, blkpos, first, pos uint64, search uint8) (bool, uint64) {
	cpos := C.uint64_t(pos)
	ret := bool(C.has_prev_uint8_t((*C.uchar)(mf.Src), C.uint64_t(blklen), C.uint64_t(blkpos), C.uint64_t(first), &cpos, C.uint8_t(search)))
	return ret, uint64(cpos)
}

// HasPrevUint16 checks if the previous occurrence of an unsigned integer on a memory mapped
// binary file containing adjacent blocks of sorted binary data still matches the search value.
// The values in the file must encoded in big-endian format and sorted in ascending order.
// This function can be used after FindLastUint16 to get the previous elements that still satisfy the search.
// The item returned by FindLastUint16 should be set as the "pos" parameter in this function.
// Return true if the previous item is valid, false otherwise, plus the position.
func (mf TMMFile) HasPrevUint16(blklen, blkpos, first, pos uint64, search uint16) (bool, uint64) {
	cpos := C.uint64_t(pos)
	ret := bool(C.has_prev_uint16_t((*C.uchar)(mf.Src), C.uint64_t(blklen), C.uint64_t(blkpos), C.uint64_t(first), &cpos, C.uint16_t(search)))
	return ret, uint64(cpos)
}

// HasPrevUint32 checks if the previous occurrence of an unsigned integer on a memory mapped
// binary file containing adjacent blocks of sorted binary data still matches the search value.
// The values in the file must encoded in big-endian format and sorted in ascending order.
// This function can be used after FindLastUint32 to get the previous elements that still satisfy the search.
// The item returned by FindLastUint32 should be set as the "pos" parameter in this function.
// Return true if the previous item is valid, false otherwise, plus the position.
func (mf TMMFile) HasPrevUint32(blklen, blkpos, first, pos uint64, search uint32) (bool, uint64) {
	cpos := C.uint64_t(pos)
	ret := bool(C.has_prev_uint32_t((*C.uchar)(mf.Src), C.uint64_t(blklen), C.uint64_t(blkpos), C.uint64_t(first), &cpos, C.uint32_t(search)))
	return ret, uint64(cpos)
}

// HasPrevUint64 checks if the previous occurrence of an unsigned integer on a memory mapped
// binary file containing adjacent blocks of sorted binary data still matches the search value.
// The values in the file must encoded in big-endian format and sorted in ascending order.
// This function can be used after FindLastUint64 to get the previous elements that still satisfy the search.
// The item returned by FindLastUint64 should be set as the "pos" parameter in this function.
// Return true if the previous item is valid, false otherwise, plus the position.
func (mf TMMFile) HasPrevUint64(blklen, blkpos, first, pos uint64, search uint64) (bool, uint64) {
	cpos := C.uint64_t(pos)
	ret := bool(C.has_prev_uint64_t((*C.uchar)(mf.Src), C.uint64_t(blklen), C.uint64_t(blkpos), C.uint64_t(first), &cpos, C.uint64_t(search)))
	return ret, uint64(cpos)
}

// HasPrevSubUint8 checks if the previous occurrence of an unsigned integer on a memory mapped
// binary file containing adjacent blocks of sorted binary data still matches the search value.
// The values in the file must encoded in big-endian format and sorted in ascending order.
// This function can be used after FindLastUint8 to get the previous elements that still satisfy the search.
// The item returned by FindLastUint8 should be set as the "pos" parameter in this function.
// Return true if the previous item is valid, false otherwise, plus the position.
func (mf TMMFile) HasPrevSubUint8(blklen, blkpos uint64, bitstart, bitend uint8, first, pos uint64, search uint8) (bool, uint64) {
	cpos := C.uint64_t(pos)
	ret := bool(C.has_prev_sub_uint8_t((*C.uchar)(mf.Src), C.uint64_t(blklen), C.uint64_t(blkpos), C.uint8_t(bitstart), C.uint8_t(bitend), C.uint64_t(first), &cpos, C.uint8_t(search)))
	return ret, uint64(cpos)
}

// HasPrevSubUint16 checks if the previous occurrence of an unsigned integer on a memory mapped
// binary file containing adjacent blocks of sorted binary data still matches the search value.
// The values in the file must encoded in big-endian format and sorted in ascending order.
// This function can be used after FindLastUint16 to get the previous elements that still satisfy the search.
// The item returned by FindLastUint16 should be set as the "pos" parameter in this function.
// Return true if the previous item is valid, false otherwise, plus the position.
func (mf TMMFile) HasPrevSubUint16(blklen, blkpos uint64, bitstart, bitend uint8, first, pos uint64, search uint16) (bool, uint64) {
	cpos := C.uint64_t(pos)
	ret := bool(C.has_prev_sub_uint16_t((*C.uchar)(mf.Src), C.uint64_t(blklen), C.uint64_t(blkpos), C.uint8_t(bitstart), C.uint8_t(bitend), C.uint64_t(first), &cpos, C.uint16_t(search)))
	return ret, uint64(cpos)
}

// HasPrevSubUint32 checks if the previous occurrence of an unsigned integer on a memory mapped
// binary file containing adjacent blocks of sorted binary data still matches the search value.
// The values in the file must encoded in big-endian format and sorted in ascending order.
// This function can be used after FindLastUint32 to get the previous elements that still satisfy the search.
// The item returned by FindLastUint32 should be set as the "pos" parameter in this function.
// Return true if the previous item is valid, false otherwise, plus the position.
func (mf TMMFile) HasPrevSubUint32(blklen, blkpos uint64, bitstart, bitend uint8, first, pos uint64, search uint32) (bool, uint64) {
	cpos := C.uint64_t(pos)
	ret := bool(C.has_prev_sub_uint32_t((*C.uchar)(mf.Src), C.uint64_t(blklen), C.uint64_t(blkpos), C.uint8_t(bitstart), C.uint8_t(bitend), C.uint64_t(first), &cpos, C.uint32_t(search)))
	return ret, uint64(cpos)
}

// HasPrevSubUint64 checks if the previous occurrence of an unsigned integer on a memory mapped
// binary file containing adjacent blocks of sorted binary data still matches the search value.
// The values in the file must encoded in big-endian format and sorted in ascending order.
// This function can be used after FindLastUint64 to get the previous elements that still satisfy the search.
// The item returned by FindLastUint64 should be set as the "pos" parameter in this function.
// Return true if the previous item is valid, false otherwise, plus the position.
func (mf TMMFile) HasPrevSubUint64(blklen, blkpos uint64, bitstart, bitend uint8, first, pos uint64, search uint64) (bool, uint64) {
	cpos := C.uint64_t(pos)
	ret := bool(C.has_prev_sub_uint64_t((*C.uchar)(mf.Src), C.uint64_t(blklen), C.uint64_t(blkpos), C.uint8_t(bitstart), C.uint8_t(bitend), C.uint64_t(first), &cpos, C.uint64_t(search)))
	return ret, uint64(cpos)
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
	vk := uint64(C.find_rv_variantkey_by_rsid((*C.uchar)(mf.Src), &cfirst, C.uint64_t(last), C.uint32_t(rsid)))
	return vk, uint64(cfirst)
}

// GetNextRVVariantkeyByRsid get the next VariantKey for the specified rsID in the RV file.
// Returns the VariantKey or 0, and the position
func (mf TMMFile) GetNextRVVariantkeyByRsid(pos, last uint64, rsid uint32) (uint64, uint64) {
	cpos := C.uint64_t(pos)
	vk := uint64(C.get_next_rv_variantkey_by_rsid((*C.uchar)(mf.Src), &cpos, C.uint64_t(last), C.uint32_t(rsid)))
	return vk, uint64(cpos)
}

// FindAllRVVariantkeyByRsid get all VariantKeys for the specified rsID in the RV file.
// Returns a list of VariantKeys
func (mf TMMFile) FindAllRVVariantkeyByRsid(first, last uint64, rsid uint32) (vks []uint64) {
	csrc := (*C.uchar)(mf.Src)
	cfirst := C.uint64_t(first)
	clast := C.uint64_t(last)
	crsid := C.uint32_t(rsid)
	vk := uint64(C.find_rv_variantkey_by_rsid(csrc, &cfirst, clast, crsid))
	for vk > 0 {
		vks = append(vks, vk)
		vk = uint64(C.get_next_rv_variantkey_by_rsid(csrc, &cfirst, clast, crsid))
	}
	return
}

// FindVRRsidByVariantkey search for the specified VariantKey and returns the first occurrence of RSID in the VR file.
func (mf TMMFile) FindVRRsidByVariantkey(first uint64, last uint64, vk uint64) (uint32, uint64) {
	cfirst := C.uint64_t(first)
	rsid := uint32(C.find_vr_rsid_by_variantkey((*C.uchar)(mf.Src), &cfirst, C.uint64_t(last), C.uint64_t(vk)))
	return rsid, uint64(cfirst)
}

// FindVRChromPosRange search for the specified CHROM-POS range and returns the first occurrence of RSID in the VR file.
func (mf TMMFile) FindVRChromPosRange(first, last uint64, chrom uint8, posMin, posMax uint32) (uint32, uint64, uint64) {
	cfirst := C.uint64_t(first)
	clast := C.uint64_t(last)
	rsid := uint32(C.find_vr_chrompos_range((*C.uchar)(mf.Src), &cfirst, &clast, C.uint8_t(chrom), C.uint32_t(posMin), C.uint32_t(posMax)))
	return rsid, uint64(cfirst), uint64(clast)
}

// --- NRVK ---

// FindRefAltByVariantkey retrieve the REF and ALT strings for the specified VariantKey.
func (mf TMMFile) FindRefAltByVariantkey(vk uint64) (string, string, uint8, uint8, uint32) {
	cref := C.malloc(256)
	defer C.free(unsafe.Pointer(cref)) // #nosec
	calt := C.malloc(256)
	defer C.free(unsafe.Pointer(calt)) // #nosec
	csizeref := C.size_t(0)
	csizealt := C.size_t(0)
	len := C.find_ref_alt_by_variantkey((*C.uchar)(mf.Src), C.uint64_t(mf.Last), C.uint64_t(vk), (*C.char)(cref), &csizeref, (*C.char)(calt), &csizealt)
	return C.GoStringN((*C.char)(cref), C.int(csizeref)), C.GoStringN((*C.char)(calt), C.int(csizealt)), uint8(csizeref), uint8(csizealt), uint32(len)
}

// ReverseVariantkey reverse a VariantKey code and returns the normalized components.
func (mf TMMFile) ReverseVariantkey(vk uint64) (TVariantKeyRev, uint32) {
	var rev C.variantkey_rev_t
	len := C.reverse_variantkey((*C.uchar)(mf.Src), C.uint64_t(mf.Last), C.uint64_t(vk), &rev)
	return castCVariantKeyRev(rev), uint32(len)
}

// VknrBinToTSV converts a vrnr.bin file to a simple TSV. For the reverse operation see the resources/tools/vknr.sh script.
func (mf TMMFile) VknrBinToTSV(tsvfile string) uint64 {
	file := StringToNTBytes(tsvfile)
	pfile := unsafe.Pointer(&file[0]) // #nosec
	return uint64(C.vknr_bin_to_tsv((*C.uchar)(mf.Src), C.uint64_t(mf.Last), (*C.char)(pfile)))
}

// --- GENOREF ---

// FlipAllele flips allele nucleotides.
func FlipAllele(allele string) string {
	ballele := StringToNTBytes(allele)
	size := len(allele)
	pallele := unsafe.Pointer(&ballele[0]) // #nosec
	C.flip_allele((*C.char)(pallele), C.size_t(size))
	return C.GoString((*C.char)(pallele))
}

// LoadGenorefIndex returns the index from the genome reference.
func (mf *TMMFile) LoadGenorefIndex() {
	mf.Index = make([]uint32, 27)
	C.load_genoref_index((*C.uchar)(mf.Src), (*C.uint32_t)(unsafe.Pointer(&mf.Index[0]))) // #nosec
}

// GetGenorefSeq returns the nucleotide at the specified chromosome and position.
func (mf TMMFile) GetGenorefSeq(chrom uint8, pos uint32) byte {
	return byte(C.get_genoref_seq((*C.uchar)(mf.Src), (*C.uint32_t)(unsafe.Pointer(&mf.Index[0])), C.uint8_t(chrom), C.uint32_t(pos))) // #nosec
}

// CheckReference checks if the reference allele matches the reference genome data.
func (mf TMMFile) CheckReference(chrom uint8, pos uint32, ref string) int {
	bref := StringToNTBytes(ref)
	sizeref := len(ref)
	pref := unsafe.Pointer(&bref[0])                                                                                                                                      // #nosec
	return int(C.check_reference((*C.uchar)(mf.Src), (*C.uint32_t)(unsafe.Pointer(&mf.Index[0])), C.uint8_t(chrom), C.uint32_t(pos), (*C.char)(pref), C.size_t(sizeref))) // #nosec
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
	code = int(C.normalize_variant((*C.uchar)(mf.Src), (*C.uint32_t)(unsafe.Pointer(&mf.Index[0])), C.uint8_t(chrom), &cpos, (*C.char)(pref), &csizeref, (*C.char)(palt), &csizealt)) // #nosec
	npos = uint32(cpos)
	nref = C.GoString((*C.char)(pref))
	nalt = C.GoString((*C.char)(palt))
	nsizeref = uint8(csizeref)
	nsizealt = uint8(csizealt)
	return
}
