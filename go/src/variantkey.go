// VariantKey
//
// Go wrapper for the variantkey C software library.
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
#include "../../c/src/binsearch.h"
#include "../../c/src/binsearch.c"
#include "../../c/src/esid.h"
#include "../../c/src/esid.c"
#include "../../c/src/genoref.h"
#include "../../c/src/genoref.c"
#include "../../c/src/hex.h"
#include "../../c/src/hex.c"
#include "../../c/src/nrvk.h"
#include "../../c/src/nrvk.c"
#include "../../c/src/regionkey.h"
#include "../../c/src/regionkey.c"
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

// FindFirstBEUint8 search for the first occurrence of a BE 8 bit unsigned integer on a memory mapped
// binary file containing adjacent blocks of sorted binary data.
// The 8 bit values in the file must encoded in big-endian format and sorted in ascending order.
// Return the item number if found or (last + 1) if not found, plus the first and last positions.
func (mf TMMFile) FindFirstBEUint8(offset, blklen, blkpos, first, last uint64, search uint8) (uint64, uint64, uint64) {
	cfirst := C.uint64_t(first)
	clast := C.uint64_t(last)
	//ret := uint64(C.find_first_be_uint8_t((*C.uchar)(unsafe.Pointer(uintptr(mf.Src)+uintptr(offset))), C.uint64_t(blklen), C.uint64_t(blkpos), &cfirst, &clast, C.uint8_t(search)))
	ret := uint64(C.find_first_be_uint8_t((*C.uchar)(unsafe.Pointer(uintptr(mf.Src)+uintptr(offset))), C.uint64_t(blklen), C.uint64_t(blkpos), &cfirst, &clast, C.uint8_t(search)))
	return ret, uint64(cfirst), uint64(clast)
}

// FindLastBEUint8 search for the last occurrence of a BE 8 bit unsigned integer on a memory mapped
// binary file containing adjacent blocks of sorted binary data.
// The 8 bit values in the file must encoded in big-endian format and sorted in ascending order.
// Return the item number if found or (last + 1) if not found, plus the first and last positions.
func (mf TMMFile) FindLastBEUint8(offset, blklen, blkpos, first, last uint64, search uint8) (uint64, uint64, uint64) {
	cfirst := C.uint64_t(first)
	clast := C.uint64_t(last)
	ret := uint64(C.find_last_be_uint8_t((*C.uchar)(unsafe.Pointer(uintptr(mf.Src)+uintptr(offset))), C.uint64_t(blklen), C.uint64_t(blkpos), &cfirst, &clast, C.uint8_t(search)))
	return ret, uint64(cfirst), uint64(clast)
}

// FindFirstBEUint16 search for the first occurrence of a BE 16 bit unsigned integer on a memory mapped
// binary file containing adjacent blocks of sorted binary data.
// The 16 bit values in the file must encoded in big-endian format and sorted in ascending order.
// Return the item number if found or (last + 1) if not found, plus the first and last positions.
func (mf TMMFile) FindFirstBEUint16(offset, blklen, blkpos, first, last uint64, search uint16) (uint64, uint64, uint64) {
	cfirst := C.uint64_t(first)
	clast := C.uint64_t(last)
	ret := uint64(C.find_first_be_uint16_t((*C.uchar)(unsafe.Pointer(uintptr(mf.Src)+uintptr(offset))), C.uint64_t(blklen), C.uint64_t(blkpos), &cfirst, &clast, C.uint16_t(search)))
	return ret, uint64(cfirst), uint64(clast)
}

// FindLastBEUint16 search for the last occurrence of a BE 16 bit unsigned integer on a memory mapped
// binary file containing adjacent blocks of sorted binary data.
// The 16 bit values in the file must encoded in big-endian format and sorted in ascending order.
// Return the item number if found or (last + 1) if not found, plus the first and last positions.
func (mf TMMFile) FindLastBEUint16(offset, blklen, blkpos, first, last uint64, search uint16) (uint64, uint64, uint64) {
	cfirst := C.uint64_t(first)
	clast := C.uint64_t(last)
	ret := uint64(C.find_last_be_uint16_t((*C.uchar)(unsafe.Pointer(uintptr(mf.Src)+uintptr(offset))), C.uint64_t(blklen), C.uint64_t(blkpos), &cfirst, &clast, C.uint16_t(search)))
	return ret, uint64(cfirst), uint64(clast)
}

// FindFirstBEUint32 search for the first occurrence of a BE 32 bit unsigned integer on a memory mapped
// binary file containing adjacent blocks of sorted binary data.
// The 32 bit values in the file must encoded in big-endian format and sorted in ascending order.
// Return the item number if found or (last + 1) if not found, plus the first and last positions.
func (mf TMMFile) FindFirstBEUint32(offset, blklen, blkpos, first, last uint64, search uint32) (uint64, uint64, uint64) {
	cfirst := C.uint64_t(first)
	clast := C.uint64_t(last)
	ret := uint64(C.find_first_be_uint32_t((*C.uchar)(unsafe.Pointer(uintptr(mf.Src)+uintptr(offset))), C.uint64_t(blklen), C.uint64_t(blkpos), &cfirst, &clast, C.uint32_t(search)))
	return ret, uint64(cfirst), uint64(clast)
}

// FindLastBEUint32 search for the last occurrence of a BE 32 bit unsigned integer on a memory mapped
// binary file containing adjacent blocks of sorted binary data.
// The 32 bit values in the file must encoded in big-endian format and sorted in ascending order.
// Return the item number if found or (last + 1) if not found, plus the first and last positions.
func (mf TMMFile) FindLastBEUint32(offset, blklen, blkpos, first, last uint64, search uint32) (uint64, uint64, uint64) {
	cfirst := C.uint64_t(first)
	clast := C.uint64_t(last)
	ret := uint64(C.find_last_be_uint32_t((*C.uchar)(unsafe.Pointer(uintptr(mf.Src)+uintptr(offset))), C.uint64_t(blklen), C.uint64_t(blkpos), &cfirst, &clast, C.uint32_t(search)))
	return ret, uint64(cfirst), uint64(clast)
}

// FindFirstBEUint64 search for the first occurrence of a BE 64 bit unsigned integer on a memory mapped
// binary file containing adjacent blocks of sorted binary data.
// The 64 bit values in the file must encoded in big-endian format and sorted in ascending order.
// Return the item number if found or (last + 1) if not found, plus the first and last positions.
func (mf TMMFile) FindFirstBEUint64(offset, blklen, blkpos, first, last uint64, search uint64) (uint64, uint64, uint64) {
	cfirst := C.uint64_t(first)
	clast := C.uint64_t(last)
	ret := uint64(C.find_first_be_uint64_t((*C.uchar)(unsafe.Pointer(uintptr(mf.Src)+uintptr(offset))), C.uint64_t(blklen), C.uint64_t(blkpos), &cfirst, &clast, C.uint64_t(search)))
	return ret, uint64(cfirst), uint64(clast)
}

// FindLastBEUint64 search for the last occurrence of a BE 64 bit unsigned integer on a memory mapped
// binary file containing adjacent blocks of sorted binary data.
// The 64 bit values in the file must encoded in big-endian format and sorted in ascending order.
// Return the item number if found or (last + 1) if not found, plus the first and last positions.
func (mf TMMFile) FindLastBEUint64(offset, blklen, blkpos, first, last uint64, search uint64) (uint64, uint64, uint64) {
	cfirst := C.uint64_t(first)
	clast := C.uint64_t(last)
	ret := uint64(C.find_last_be_uint64_t((*C.uchar)(unsafe.Pointer(uintptr(mf.Src)+uintptr(offset))), C.uint64_t(blklen), C.uint64_t(blkpos), &cfirst, &clast, C.uint64_t(search)))
	return ret, uint64(cfirst), uint64(clast)
}

// FindFirstSubBEUint8 search for the first occurrence of a BE bit set contained in a 8 bit unsigned integer on a memory mapped
// binary file containing adjacent blocks of sorted binary data.
// The 8 bit values in the file must encoded in big-endian format and sorted in ascending order.
// Return the item number if found or (last + 1) if not found, plus the first and last positions.
func (mf TMMFile) FindFirstSubBEUint8(offset, blklen, blkpos uint64, bitstart, bitend uint8, first, last uint64, search uint8) (uint64, uint64, uint64) {
	cfirst := C.uint64_t(first)
	clast := C.uint64_t(last)
	ret := uint64(C.find_first_sub_be_uint8_t((*C.uchar)(unsafe.Pointer(uintptr(mf.Src)+uintptr(offset))), C.uint64_t(blklen), C.uint64_t(blkpos), C.uint8_t(bitstart), C.uint8_t(bitend), &cfirst, &clast, C.uint8_t(search)))
	return ret, uint64(cfirst), uint64(clast)
}

// FindLastSubBEUint8 search for the last occurrence of a BE bit set contained in a 8 bit unsigned integer on a memory mapped
// binary file containing adjacent blocks of sorted binary data.
// The 8 bit values in the file must encoded in big-endian format and sorted in ascending order.
// Return the item number if found or (last + 1) if not found, plus the first and last positions.
func (mf TMMFile) FindLastSubBEUint8(offset, blklen, blkpos uint64, bitstart, bitend uint8, first, last uint64, search uint8) (uint64, uint64, uint64) {
	cfirst := C.uint64_t(first)
	clast := C.uint64_t(last)
	ret := uint64(C.find_last_sub_be_uint8_t((*C.uchar)(unsafe.Pointer(uintptr(mf.Src)+uintptr(offset))), C.uint64_t(blklen), C.uint64_t(blkpos), C.uint8_t(bitstart), C.uint8_t(bitend), &cfirst, &clast, C.uint8_t(search)))
	return ret, uint64(cfirst), uint64(clast)
}

// FindFirstSubBEUint16 search for the first occurrence of a BE bit set contained in a 16 bit unsigned integer on a memory mapped
// binary file containing adjacent blocks of sorted binary data.
// The 16 bit values in the file must encoded in big-endian format and sorted in ascending order.
// Return the item number if found or (last + 1) if not found, plus the first and last positions.
func (mf TMMFile) FindFirstSubBEUint16(offset, blklen, blkpos uint64, bitstart, bitend uint8, first, last uint64, search uint16) (uint64, uint64, uint64) {
	cfirst := C.uint64_t(first)
	clast := C.uint64_t(last)
	ret := uint64(C.find_first_sub_be_uint16_t((*C.uchar)(unsafe.Pointer(uintptr(mf.Src)+uintptr(offset))), C.uint64_t(blklen), C.uint64_t(blkpos), C.uint8_t(bitstart), C.uint8_t(bitend), &cfirst, &clast, C.uint16_t(search)))
	return ret, uint64(cfirst), uint64(clast)
}

// FindLastSubBEUint16 search for the last occurrence of a BE bit set contained in a 16 bit unsigned integer on a memory mapped
// binary file containing adjacent blocks of sorted binary data.
// The 16 bit values in the file must encoded in big-endian format and sorted in ascending order.
// Return the item number if found or (last + 1) if not found, plus the first and last positions.
func (mf TMMFile) FindLastSubBEUint16(offset, blklen, blkpos uint64, bitstart, bitend uint8, first, last uint64, search uint16) (uint64, uint64, uint64) {
	cfirst := C.uint64_t(first)
	clast := C.uint64_t(last)
	ret := uint64(C.find_last_sub_be_uint16_t((*C.uchar)(unsafe.Pointer(uintptr(mf.Src)+uintptr(offset))), C.uint64_t(blklen), C.uint64_t(blkpos), C.uint8_t(bitstart), C.uint8_t(bitend), &cfirst, &clast, C.uint16_t(search)))
	return ret, uint64(cfirst), uint64(clast)
}

// FindFirstSubBEUint32 search for the first occurrence of a BE bit set contained in a 32 bit unsigned integer on a memory mapped
// binary file containing adjacent blocks of sorted binary data.
// The 32 bit values in the file must encoded in big-endian format and sorted in ascending order.
// Return the item number if found or (last + 1) if not found, plus the first and last positions.
func (mf TMMFile) FindFirstSubBEUint32(offset, blklen, blkpos uint64, bitstart, bitend uint8, first, last uint64, search uint32) (uint64, uint64, uint64) {
	cfirst := C.uint64_t(first)
	clast := C.uint64_t(last)
	ret := uint64(C.find_first_sub_be_uint32_t((*C.uchar)(unsafe.Pointer(uintptr(mf.Src)+uintptr(offset))), C.uint64_t(blklen), C.uint64_t(blkpos), C.uint8_t(bitstart), C.uint8_t(bitend), &cfirst, &clast, C.uint32_t(search)))
	return ret, uint64(cfirst), uint64(clast)
}

// FindLastSubBEUint32 search for the last occurrence of a BE bit set contained in a 32 bit unsigned integer on a memory mapped
// binary file containing adjacent blocks of sorted binary data.
// The 32 bit values in the file must encoded in big-endian format and sorted in ascending order.
// Return the item number if found or (last + 1) if not found, plus the first and last positions.
func (mf TMMFile) FindLastSubBEUint32(offset, blklen, blkpos uint64, bitstart, bitend uint8, first, last uint64, search uint32) (uint64, uint64, uint64) {
	cfirst := C.uint64_t(first)
	clast := C.uint64_t(last)
	ret := uint64(C.find_last_sub_be_uint32_t((*C.uchar)(unsafe.Pointer(uintptr(mf.Src)+uintptr(offset))), C.uint64_t(blklen), C.uint64_t(blkpos), C.uint8_t(bitstart), C.uint8_t(bitend), &cfirst, &clast, C.uint32_t(search)))
	return ret, uint64(cfirst), uint64(clast)
}

// FindFirstSubBEUint64 search for the first occurrence of a BE bit set contained in a 64 bit unsigned integer on a memory mapped
// binary file containing adjacent blocks of sorted binary data.
// The 64 bit values in the file must encoded in big-endian format and sorted in ascending order.
// Return the item number if found or (last + 1) if not found, plus the first and last positions.
func (mf TMMFile) FindFirstSubBEUint64(offset, blklen, blkpos uint64, bitstart, bitend uint8, first, last uint64, search uint64) (uint64, uint64, uint64) {
	cfirst := C.uint64_t(first)
	clast := C.uint64_t(last)
	ret := uint64(C.find_first_sub_be_uint64_t((*C.uchar)(unsafe.Pointer(uintptr(mf.Src)+uintptr(offset))), C.uint64_t(blklen), C.uint64_t(blkpos), C.uint8_t(bitstart), C.uint8_t(bitend), &cfirst, &clast, C.uint64_t(search)))
	return ret, uint64(cfirst), uint64(clast)
}

// FindLastSubBEUint64 search for the last occurrence of a BE bit set contained in a 64 bit unsigned integer on a memory mapped
// binary file containing adjacent blocks of sorted binary data.
// The 64 bit values in the file must encoded in big-endian format and sorted in ascending order.
// Return the item number if found or (last + 1) if not found, plus the first and last positions.
func (mf TMMFile) FindLastSubBEUint64(offset, blklen, blkpos uint64, bitstart, bitend uint8, first, last uint64, search uint64) (uint64, uint64, uint64) {
	cfirst := C.uint64_t(first)
	clast := C.uint64_t(last)
	ret := uint64(C.find_last_sub_be_uint64_t((*C.uchar)(unsafe.Pointer(uintptr(mf.Src)+uintptr(offset))), C.uint64_t(blklen), C.uint64_t(blkpos), C.uint8_t(bitstart), C.uint8_t(bitend), &cfirst, &clast, C.uint64_t(search)))
	return ret, uint64(cfirst), uint64(clast)
}

// HasNextBEUint8 checks if the next occurrence of a BE unsigned integer on a memory mapped
// binary file containing adjacent blocks of sorted binary data still matches the search value.
// The values in the file must encoded in big-endian format and sorted in ascending order.
// This function can be used after FindFirstBEUint8 to get the next elements that still satisfy the search.
// The item returned by FindFirstBEUint8 should be set as the "pos" parameter in this function.
// Return true if the next item is valid, false otherwise, plus the position.
func (mf TMMFile) HasNextBEUint8(offset, blklen, blkpos, pos, last uint64, search uint8) (bool, uint64) {
	cpos := C.uint64_t(pos)
	ret := bool(C.has_next_be_uint8_t((*C.uchar)(unsafe.Pointer(uintptr(mf.Src)+uintptr(offset))), C.uint64_t(blklen), C.uint64_t(blkpos), &cpos, C.uint64_t(last), C.uint8_t(search)))
	return ret, uint64(cpos)
}

// HasNextBEUint16 checks if the next occurrence of a BE unsigned integer on a memory mapped
// binary file containing adjacent blocks of sorted binary data still matches the search value.
// The values in the file must encoded in big-endian format and sorted in ascending order.
// This function can be used after FindFirstBEUint16 to get the next elements that still satisfy the search.
// The item returned by FindFirstBEUint16 should be set as the "pos" parameter in this function.
// Return true if the next item is valid, false otherwise, plus the position.
func (mf TMMFile) HasNextBEUint16(offset, blklen, blkpos, pos, last uint64, search uint16) (bool, uint64) {
	cpos := C.uint64_t(pos)
	ret := bool(C.has_next_be_uint16_t((*C.uchar)(unsafe.Pointer(uintptr(mf.Src)+uintptr(offset))), C.uint64_t(blklen), C.uint64_t(blkpos), &cpos, C.uint64_t(last), C.uint16_t(search)))
	return ret, uint64(cpos)
}

// HasNextBEUint32 checks if the next occurrence of a BE unsigned integer on a memory mapped
// binary file containing adjacent blocks of sorted binary data still matches the search value.
// The values in the file must encoded in big-endian format and sorted in ascending order.
// This function can be used after FindFirstBEUint32 to get the next elements that still satisfy the search.
// The item returned by FindFirstBEUint32 should be set as the "pos" parameter in this function.
// Return true if the next item is valid, false otherwise, plus the position.
func (mf TMMFile) HasNextBEUint32(offset, blklen, blkpos, pos, last uint64, search uint32) (bool, uint64) {
	cpos := C.uint64_t(pos)
	ret := bool(C.has_next_be_uint32_t((*C.uchar)(unsafe.Pointer(uintptr(mf.Src)+uintptr(offset))), C.uint64_t(blklen), C.uint64_t(blkpos), &cpos, C.uint64_t(last), C.uint32_t(search)))
	return ret, uint64(cpos)
}

// HasNextBEUint64 checks if the next occurrence of a BE unsigned integer on a memory mapped
// binary file containing adjacent blocks of sorted binary data still matches the search value.
// The values in the file must encoded in big-endian format and sorted in ascending order.
// This function can be used after FindFirstBEUint64 to get the next elements that still satisfy the search.
// The item returned by FindFirstBEUint64 should be set as the "pos" parameter in this function.
// Return true if the next item is valid, false otherwise, plus the position.
func (mf TMMFile) HasNextBEUint64(offset, blklen, blkpos, pos, last uint64, search uint64) (bool, uint64) {
	cpos := C.uint64_t(pos)
	ret := bool(C.has_next_be_uint64_t((*C.uchar)(unsafe.Pointer(uintptr(mf.Src)+uintptr(offset))), C.uint64_t(blklen), C.uint64_t(blkpos), &cpos, C.uint64_t(last), C.uint64_t(search)))
	return ret, uint64(cpos)
}

// HasNextSubBEUint8 checks if the next occurrence of a BE unsigned integer on a memory mapped
// binary file containing adjacent blocks of sorted binary data still matches the search value.
// The values in the file must encoded in big-endian format and sorted in ascending order.
// This function can be used after FindFirstBEUint8 to get the next elements that still satisfy the search.
// The item returned by FindFirstBEUint8 should be set as the "pos" parameter in this function.
// Return true if the next item is valid, false otherwise, plus the position.
func (mf TMMFile) HasNextSubBEUint8(offset, blklen, blkpos uint64, bitstart, bitend uint8, pos, last uint64, search uint8) (bool, uint64) {
	cpos := C.uint64_t(pos)
	ret := bool(C.has_next_sub_be_uint8_t((*C.uchar)(unsafe.Pointer(uintptr(mf.Src)+uintptr(offset))), C.uint64_t(blklen), C.uint64_t(blkpos), C.uint8_t(bitstart), C.uint8_t(bitend), &cpos, C.uint64_t(last), C.uint8_t(search)))
	return ret, uint64(cpos)
}

// HasNextSubBEUint16 checks if the next occurrence of a BE unsigned integer on a memory mapped
// binary file containing adjacent blocks of sorted binary data still matches the search value.
// The values in the file must encoded in big-endian format and sorted in ascending order.
// This function can be used after FindFirstBEUint16 to get the next elements that still satisfy the search.
// The item returned by FindFirstBEUint16 should be set as the "pos" parameter in this function.
// Return true if the next item is valid, false otherwise, plus the position.
func (mf TMMFile) HasNextSubBEUint16(offset, blklen, blkpos uint64, bitstart, bitend uint8, pos, last uint64, search uint16) (bool, uint64) {
	cpos := C.uint64_t(pos)
	ret := bool(C.has_next_sub_be_uint16_t((*C.uchar)(unsafe.Pointer(uintptr(mf.Src)+uintptr(offset))), C.uint64_t(blklen), C.uint64_t(blkpos), C.uint8_t(bitstart), C.uint8_t(bitend), &cpos, C.uint64_t(last), C.uint16_t(search)))
	return ret, uint64(cpos)
}

// HasNextSubBEUint32 checks if the next occurrence of a BE unsigned integer on a memory mapped
// binary file containing adjacent blocks of sorted binary data still matches the search value.
// The values in the file must encoded in big-endian format and sorted in ascending order.
// This function can be used after FindFirstBEUint32 to get the next elements that still satisfy the search.
// The item returned by FindFirstBEUint32 should be set as the "pos" parameter in this function.
// Return true if the next item is valid, false otherwise, plus the position.
func (mf TMMFile) HasNextSubBEUint32(offset, blklen, blkpos uint64, bitstart, bitend uint8, pos, last uint64, search uint32) (bool, uint64) {
	cpos := C.uint64_t(pos)
	ret := bool(C.has_next_sub_be_uint32_t((*C.uchar)(unsafe.Pointer(uintptr(mf.Src)+uintptr(offset))), C.uint64_t(blklen), C.uint64_t(blkpos), C.uint8_t(bitstart), C.uint8_t(bitend), &cpos, C.uint64_t(last), C.uint32_t(search)))
	return ret, uint64(cpos)
}

// HasNextSubBEUint64 checks if the next occurrence of a BE unsigned integer on a memory mapped
// binary file containing adjacent blocks of sorted binary data still matches the search value.
// The values in the file must encoded in big-endian format and sorted in ascending order.
// This function can be used after FindFirstBEUint64 to get the next elements that still satisfy the search.
// The item returned by FindFirstBEUint64 should be set as the "pos" parameter in this function.
// Return true if the next item is valid, false otherwise, plus the position.
func (mf TMMFile) HasNextSubBEUint64(offset, blklen, blkpos uint64, bitstart, bitend uint8, pos, last uint64, search uint64) (bool, uint64) {
	cpos := C.uint64_t(pos)
	ret := bool(C.has_next_sub_be_uint64_t((*C.uchar)(unsafe.Pointer(uintptr(mf.Src)+uintptr(offset))), C.uint64_t(blklen), C.uint64_t(blkpos), C.uint8_t(bitstart), C.uint8_t(bitend), &cpos, C.uint64_t(last), C.uint64_t(search)))
	return ret, uint64(cpos)
}

// HasPrevBEUint8 checks if the previous occurrence of a BE unsigned integer on a memory mapped
// binary file containing adjacent blocks of sorted binary data still matches the search value.
// The values in the file must encoded in big-endian format and sorted in ascending order.
// This function can be used after FindLastBEUint8 to get the previous elements that still satisfy the search.
// The item returned by FindLastBEUint8 should be set as the "pos" parameter in this function.
// Return true if the previous item is valid, false otherwise, plus the position.
func (mf TMMFile) HasPrevBEUint8(offset, blklen, blkpos, first, pos uint64, search uint8) (bool, uint64) {
	cpos := C.uint64_t(pos)
	ret := bool(C.has_prev_be_uint8_t((*C.uchar)(unsafe.Pointer(uintptr(mf.Src)+uintptr(offset))), C.uint64_t(blklen), C.uint64_t(blkpos), C.uint64_t(first), &cpos, C.uint8_t(search)))
	return ret, uint64(cpos)
}

// HasPrevBEUint16 checks if the previous occurrence of a BE unsigned integer on a memory mapped
// binary file containing adjacent blocks of sorted binary data still matches the search value.
// The values in the file must encoded in big-endian format and sorted in ascending order.
// This function can be used after FindLastBEUint16 to get the previous elements that still satisfy the search.
// The item returned by FindLastBEUint16 should be set as the "pos" parameter in this function.
// Return true if the previous item is valid, false otherwise, plus the position.
func (mf TMMFile) HasPrevBEUint16(offset, blklen, blkpos, first, pos uint64, search uint16) (bool, uint64) {
	cpos := C.uint64_t(pos)
	ret := bool(C.has_prev_be_uint16_t((*C.uchar)(unsafe.Pointer(uintptr(mf.Src)+uintptr(offset))), C.uint64_t(blklen), C.uint64_t(blkpos), C.uint64_t(first), &cpos, C.uint16_t(search)))
	return ret, uint64(cpos)
}

// HasPrevBEUint32 checks if the previous occurrence of a BE unsigned integer on a memory mapped
// binary file containing adjacent blocks of sorted binary data still matches the search value.
// The values in the file must encoded in big-endian format and sorted in ascending order.
// This function can be used after FindLastBEUint32 to get the previous elements that still satisfy the search.
// The item returned by FindLastBEUint32 should be set as the "pos" parameter in this function.
// Return true if the previous item is valid, false otherwise, plus the position.
func (mf TMMFile) HasPrevBEUint32(offset, blklen, blkpos, first, pos uint64, search uint32) (bool, uint64) {
	cpos := C.uint64_t(pos)
	ret := bool(C.has_prev_be_uint32_t((*C.uchar)(unsafe.Pointer(uintptr(mf.Src)+uintptr(offset))), C.uint64_t(blklen), C.uint64_t(blkpos), C.uint64_t(first), &cpos, C.uint32_t(search)))
	return ret, uint64(cpos)
}

// HasPrevBEUint64 checks if the previous occurrence of a BE unsigned integer on a memory mapped
// binary file containing adjacent blocks of sorted binary data still matches the search value.
// The values in the file must encoded in big-endian format and sorted in ascending order.
// This function can be used after FindLastBEUint64 to get the previous elements that still satisfy the search.
// The item returned by FindLastBEUint64 should be set as the "pos" parameter in this function.
// Return true if the previous item is valid, false otherwise, plus the position.
func (mf TMMFile) HasPrevBEUint64(offset, blklen, blkpos, first, pos uint64, search uint64) (bool, uint64) {
	cpos := C.uint64_t(pos)
	ret := bool(C.has_prev_be_uint64_t((*C.uchar)(unsafe.Pointer(uintptr(mf.Src)+uintptr(offset))), C.uint64_t(blklen), C.uint64_t(blkpos), C.uint64_t(first), &cpos, C.uint64_t(search)))
	return ret, uint64(cpos)
}

// HasPrevSubBEUint8 checks if the previous occurrence of a BE unsigned integer on a memory mapped
// binary file containing adjacent blocks of sorted binary data still matches the search value.
// The values in the file must encoded in big-endian format and sorted in ascending order.
// This function can be used after FindLastBEUint8 to get the previous elements that still satisfy the search.
// The item returned by FindLastBEUint8 should be set as the "pos" parameter in this function.
// Return true if the previous item is valid, false otherwise, plus the position.
func (mf TMMFile) HasPrevSubBEUint8(offset, blklen, blkpos uint64, bitstart, bitend uint8, first, pos uint64, search uint8) (bool, uint64) {
	cpos := C.uint64_t(pos)
	ret := bool(C.has_prev_sub_be_uint8_t((*C.uchar)(unsafe.Pointer(uintptr(mf.Src)+uintptr(offset))), C.uint64_t(blklen), C.uint64_t(blkpos), C.uint8_t(bitstart), C.uint8_t(bitend), C.uint64_t(first), &cpos, C.uint8_t(search)))
	return ret, uint64(cpos)
}

// HasPrevSubBEUint16 checks if the previous occurrence of a BE unsigned integer on a memory mapped
// binary file containing adjacent blocks of sorted binary data still matches the search value.
// The values in the file must encoded in big-endian format and sorted in ascending order.
// This function can be used after FindLastBEUint16 to get the previous elements that still satisfy the search.
// The item returned by FindLastBEUint16 should be set as the "pos" parameter in this function.
// Return true if the previous item is valid, false otherwise, plus the position.
func (mf TMMFile) HasPrevSubBEUint16(offset, blklen, blkpos uint64, bitstart, bitend uint8, first, pos uint64, search uint16) (bool, uint64) {
	cpos := C.uint64_t(pos)
	ret := bool(C.has_prev_sub_be_uint16_t((*C.uchar)(unsafe.Pointer(uintptr(mf.Src)+uintptr(offset))), C.uint64_t(blklen), C.uint64_t(blkpos), C.uint8_t(bitstart), C.uint8_t(bitend), C.uint64_t(first), &cpos, C.uint16_t(search)))
	return ret, uint64(cpos)
}

// HasPrevSubBEUint32 checks if the previous occurrence of a BE unsigned integer on a memory mapped
// binary file containing adjacent blocks of sorted binary data still matches the search value.
// The values in the file must encoded in big-endian format and sorted in ascending order.
// This function can be used after FindLastBEUint32 to get the previous elements that still satisfy the search.
// The item returned by FindLastBEUint32 should be set as the "pos" parameter in this function.
// Return true if the previous item is valid, false otherwise, plus the position.
func (mf TMMFile) HasPrevSubBEUint32(offset, blklen, blkpos uint64, bitstart, bitend uint8, first, pos uint64, search uint32) (bool, uint64) {
	cpos := C.uint64_t(pos)
	ret := bool(C.has_prev_sub_be_uint32_t((*C.uchar)(unsafe.Pointer(uintptr(mf.Src)+uintptr(offset))), C.uint64_t(blklen), C.uint64_t(blkpos), C.uint8_t(bitstart), C.uint8_t(bitend), C.uint64_t(first), &cpos, C.uint32_t(search)))
	return ret, uint64(cpos)
}

// HasPrevSubBEUint64 checks if the previous occurrence of a BE unsigned integer on a memory mapped
// binary file containing adjacent blocks of sorted binary data still matches the search value.
// The values in the file must encoded in big-endian format and sorted in ascending order.
// This function can be used after FindLastBEUint64 to get the previous elements that still satisfy the search.
// The item returned by FindLastBEUint64 should be set as the "pos" parameter in this function.
// Return true if the previous item is valid, false otherwise, plus the position.
func (mf TMMFile) HasPrevSubBEUint64(offset, blklen, blkpos uint64, bitstart, bitend uint8, first, pos uint64, search uint64) (bool, uint64) {
	cpos := C.uint64_t(pos)
	ret := bool(C.has_prev_sub_be_uint64_t((*C.uchar)(unsafe.Pointer(uintptr(mf.Src)+uintptr(offset))), C.uint64_t(blklen), C.uint64_t(blkpos), C.uint8_t(bitstart), C.uint8_t(bitend), C.uint64_t(first), &cpos, C.uint64_t(search)))
	return ret, uint64(cpos)
}

// FindFirstLEUint8 search for the first occurrence of a LE 8 bit unsigned integer on a memory mapped
// binary file containing adjacent blocks of sorted binary data.
// The 8 bit values in the file must encoded in big-endian format and sorted in ascending order.
// Return the item number if found or (last + 1) if not found, plus the first and last positions.
func (mf TMMFile) FindFirstLEUint8(offset, blklen, blkpos, first, last uint64, search uint8) (uint64, uint64, uint64) {
	cfirst := C.uint64_t(first)
	clast := C.uint64_t(last)
	ret := uint64(C.find_first_le_uint8_t((*C.uchar)(unsafe.Pointer(uintptr(mf.Src)+uintptr(offset))), C.uint64_t(blklen), C.uint64_t(blkpos), &cfirst, &clast, C.uint8_t(search)))
	return ret, uint64(cfirst), uint64(clast)
}

// FindLastLEUint8 search for the last occurrence of a LE 8 bit unsigned integer on a memory mapped
// binary file containing adjacent blocks of sorted binary data.
// The 8 bit values in the file must encoded in big-endian format and sorted in ascending order.
// Return the item number if found or (last + 1) if not found, plus the first and last positions.
func (mf TMMFile) FindLastLEUint8(offset, blklen, blkpos, first, last uint64, search uint8) (uint64, uint64, uint64) {
	cfirst := C.uint64_t(first)
	clast := C.uint64_t(last)
	ret := uint64(C.find_last_le_uint8_t((*C.uchar)(unsafe.Pointer(uintptr(mf.Src)+uintptr(offset))), C.uint64_t(blklen), C.uint64_t(blkpos), &cfirst, &clast, C.uint8_t(search)))
	return ret, uint64(cfirst), uint64(clast)
}

// FindFirstLEUint16 search for the first occurrence of a LE 16 bit unsigned integer on a memory mapped
// binary file containing adjacent blocks of sorted binary data.
// The 16 bit values in the file must encoded in big-endian format and sorted in ascending order.
// Return the item number if found or (last + 1) if not found, plus the first and last positions.
func (mf TMMFile) FindFirstLEUint16(offset, blklen, blkpos, first, last uint64, search uint16) (uint64, uint64, uint64) {
	cfirst := C.uint64_t(first)
	clast := C.uint64_t(last)
	ret := uint64(C.find_first_le_uint16_t((*C.uchar)(unsafe.Pointer(uintptr(mf.Src)+uintptr(offset))), C.uint64_t(blklen), C.uint64_t(blkpos), &cfirst, &clast, C.uint16_t(search)))
	return ret, uint64(cfirst), uint64(clast)
}

// FindLastLEUint16 search for the last occurrence of a LE 16 bit unsigned integer on a memory mapped
// binary file containing adjacent blocks of sorted binary data.
// The 16 bit values in the file must encoded in big-endian format and sorted in ascending order.
// Return the item number if found or (last + 1) if not found, plus the first and last positions.
func (mf TMMFile) FindLastLEUint16(offset, blklen, blkpos, first, last uint64, search uint16) (uint64, uint64, uint64) {
	cfirst := C.uint64_t(first)
	clast := C.uint64_t(last)
	ret := uint64(C.find_last_le_uint16_t((*C.uchar)(unsafe.Pointer(uintptr(mf.Src)+uintptr(offset))), C.uint64_t(blklen), C.uint64_t(blkpos), &cfirst, &clast, C.uint16_t(search)))
	return ret, uint64(cfirst), uint64(clast)
}

// FindFirstLEUint32 search for the first occurrence of a LE 32 bit unsigned integer on a memory mapped
// binary file containing adjacent blocks of sorted binary data.
// The 32 bit values in the file must encoded in big-endian format and sorted in ascending order.
// Return the item number if found or (last + 1) if not found, plus the first and last positions.
func (mf TMMFile) FindFirstLEUint32(offset, blklen, blkpos, first, last uint64, search uint32) (uint64, uint64, uint64) {
	cfirst := C.uint64_t(first)
	clast := C.uint64_t(last)
	ret := uint64(C.find_first_le_uint32_t((*C.uchar)(unsafe.Pointer(uintptr(mf.Src)+uintptr(offset))), C.uint64_t(blklen), C.uint64_t(blkpos), &cfirst, &clast, C.uint32_t(search)))
	return ret, uint64(cfirst), uint64(clast)
}

// FindLastLEUint32 search for the last occurrence of a LE 32 bit unsigned integer on a memory mapped
// binary file containing adjacent blocks of sorted binary data.
// The 32 bit values in the file must encoded in big-endian format and sorted in ascending order.
// Return the item number if found or (last + 1) if not found, plus the first and last positions.
func (mf TMMFile) FindLastLEUint32(offset, blklen, blkpos, first, last uint64, search uint32) (uint64, uint64, uint64) {
	cfirst := C.uint64_t(first)
	clast := C.uint64_t(last)
	ret := uint64(C.find_last_le_uint32_t((*C.uchar)(unsafe.Pointer(uintptr(mf.Src)+uintptr(offset))), C.uint64_t(blklen), C.uint64_t(blkpos), &cfirst, &clast, C.uint32_t(search)))
	return ret, uint64(cfirst), uint64(clast)
}

// FindFirstLEUint64 search for the first occurrence of a LE 64 bit unsigned integer on a memory mapped
// binary file containing adjacent blocks of sorted binary data.
// The 64 bit values in the file must encoded in big-endian format and sorted in ascending order.
// Return the item number if found or (last + 1) if not found, plus the first and last positions.
func (mf TMMFile) FindFirstLEUint64(offset, blklen, blkpos, first, last uint64, search uint64) (uint64, uint64, uint64) {
	cfirst := C.uint64_t(first)
	clast := C.uint64_t(last)
	ret := uint64(C.find_first_le_uint64_t((*C.uchar)(unsafe.Pointer(uintptr(mf.Src)+uintptr(offset))), C.uint64_t(blklen), C.uint64_t(blkpos), &cfirst, &clast, C.uint64_t(search)))
	return ret, uint64(cfirst), uint64(clast)
}

// FindLastLEUint64 search for the last occurrence of a LE 64 bit unsigned integer on a memory mapped
// binary file containing adjacent blocks of sorted binary data.
// The 64 bit values in the file must encoded in big-endian format and sorted in ascending order.
// Return the item number if found or (last + 1) if not found, plus the first and last positions.
func (mf TMMFile) FindLastLEUint64(offset, blklen, blkpos, first, last uint64, search uint64) (uint64, uint64, uint64) {
	cfirst := C.uint64_t(first)
	clast := C.uint64_t(last)
	ret := uint64(C.find_last_le_uint64_t((*C.uchar)(unsafe.Pointer(uintptr(mf.Src)+uintptr(offset))), C.uint64_t(blklen), C.uint64_t(blkpos), &cfirst, &clast, C.uint64_t(search)))
	return ret, uint64(cfirst), uint64(clast)
}

// FindFirstSubLEUint8 search for the first occurrence of a LE bit set contained in a 8 bit unsigned integer on a memory mapped
// binary file containing adjacent blocks of sorted binary data.
// The 8 bit values in the file must encoded in big-endian format and sorted in ascending order.
// Return the item number if found or (last + 1) if not found, plus the first and last positions.
func (mf TMMFile) FindFirstSubLEUint8(offset, blklen, blkpos uint64, bitstart, bitend uint8, first, last uint64, search uint8) (uint64, uint64, uint64) {
	cfirst := C.uint64_t(first)
	clast := C.uint64_t(last)
	ret := uint64(C.find_first_sub_le_uint8_t((*C.uchar)(unsafe.Pointer(uintptr(mf.Src)+uintptr(offset))), C.uint64_t(blklen), C.uint64_t(blkpos), C.uint8_t(bitstart), C.uint8_t(bitend), &cfirst, &clast, C.uint8_t(search)))
	return ret, uint64(cfirst), uint64(clast)
}

// FindLastSubLEUint8 search for the last occurrence of a LE bit set contained in a 8 bit unsigned integer on a memory mapped
// binary file containing adjacent blocks of sorted binary data.
// The 8 bit values in the file must encoded in big-endian format and sorted in ascending order.
// Return the item number if found or (last + 1) if not found, plus the first and last positions.
func (mf TMMFile) FindLastSubLEUint8(offset, blklen, blkpos uint64, bitstart, bitend uint8, first, last uint64, search uint8) (uint64, uint64, uint64) {
	cfirst := C.uint64_t(first)
	clast := C.uint64_t(last)
	ret := uint64(C.find_last_sub_le_uint8_t((*C.uchar)(unsafe.Pointer(uintptr(mf.Src)+uintptr(offset))), C.uint64_t(blklen), C.uint64_t(blkpos), C.uint8_t(bitstart), C.uint8_t(bitend), &cfirst, &clast, C.uint8_t(search)))
	return ret, uint64(cfirst), uint64(clast)
}

// FindFirstSubLEUint16 search for the first occurrence of a LE bit set contained in a 16 bit unsigned integer on a memory mapped
// binary file containing adjacent blocks of sorted binary data.
// The 16 bit values in the file must encoded in big-endian format and sorted in ascending order.
// Return the item number if found or (last + 1) if not found, plus the first and last positions.
func (mf TMMFile) FindFirstSubLEUint16(offset, blklen, blkpos uint64, bitstart, bitend uint8, first, last uint64, search uint16) (uint64, uint64, uint64) {
	cfirst := C.uint64_t(first)
	clast := C.uint64_t(last)
	ret := uint64(C.find_first_sub_le_uint16_t((*C.uchar)(unsafe.Pointer(uintptr(mf.Src)+uintptr(offset))), C.uint64_t(blklen), C.uint64_t(blkpos), C.uint8_t(bitstart), C.uint8_t(bitend), &cfirst, &clast, C.uint16_t(search)))
	return ret, uint64(cfirst), uint64(clast)
}

// FindLastSubLEUint16 search for the last occurrence of a LE bit set contained in a 16 bit unsigned integer on a memory mapped
// binary file containing adjacent blocks of sorted binary data.
// The 16 bit values in the file must encoded in big-endian format and sorted in ascending order.
// Return the item number if found or (last + 1) if not found, plus the first and last positions.
func (mf TMMFile) FindLastSubLEUint16(offset, blklen, blkpos uint64, bitstart, bitend uint8, first, last uint64, search uint16) (uint64, uint64, uint64) {
	cfirst := C.uint64_t(first)
	clast := C.uint64_t(last)
	ret := uint64(C.find_last_sub_le_uint16_t((*C.uchar)(unsafe.Pointer(uintptr(mf.Src)+uintptr(offset))), C.uint64_t(blklen), C.uint64_t(blkpos), C.uint8_t(bitstart), C.uint8_t(bitend), &cfirst, &clast, C.uint16_t(search)))
	return ret, uint64(cfirst), uint64(clast)
}

// FindFirstSubLEUint32 search for the first occurrence of a LE bit set contained in a 32 bit unsigned integer on a memory mapped
// binary file containing adjacent blocks of sorted binary data.
// The 32 bit values in the file must encoded in big-endian format and sorted in ascending order.
// Return the item number if found or (last + 1) if not found, plus the first and last positions.
func (mf TMMFile) FindFirstSubLEUint32(offset, blklen, blkpos uint64, bitstart, bitend uint8, first, last uint64, search uint32) (uint64, uint64, uint64) {
	cfirst := C.uint64_t(first)
	clast := C.uint64_t(last)
	ret := uint64(C.find_first_sub_le_uint32_t((*C.uchar)(unsafe.Pointer(uintptr(mf.Src)+uintptr(offset))), C.uint64_t(blklen), C.uint64_t(blkpos), C.uint8_t(bitstart), C.uint8_t(bitend), &cfirst, &clast, C.uint32_t(search)))
	return ret, uint64(cfirst), uint64(clast)
}

// FindLastSubLEUint32 search for the last occurrence of a LE bit set contained in a 32 bit unsigned integer on a memory mapped
// binary file containing adjacent blocks of sorted binary data.
// The 32 bit values in the file must encoded in big-endian format and sorted in ascending order.
// Return the item number if found or (last + 1) if not found, plus the first and last positions.
func (mf TMMFile) FindLastSubLEUint32(offset, blklen, blkpos uint64, bitstart, bitend uint8, first, last uint64, search uint32) (uint64, uint64, uint64) {
	cfirst := C.uint64_t(first)
	clast := C.uint64_t(last)
	ret := uint64(C.find_last_sub_le_uint32_t((*C.uchar)(unsafe.Pointer(uintptr(mf.Src)+uintptr(offset))), C.uint64_t(blklen), C.uint64_t(blkpos), C.uint8_t(bitstart), C.uint8_t(bitend), &cfirst, &clast, C.uint32_t(search)))
	return ret, uint64(cfirst), uint64(clast)
}

// FindFirstSubLEUint64 search for the first occurrence of a LE bit set contained in a 64 bit unsigned integer on a memory mapped
// binary file containing adjacent blocks of sorted binary data.
// The 64 bit values in the file must encoded in big-endian format and sorted in ascending order.
// Return the item number if found or (last + 1) if not found, plus the first and last positions.
func (mf TMMFile) FindFirstSubLEUint64(offset, blklen, blkpos uint64, bitstart, bitend uint8, first, last uint64, search uint64) (uint64, uint64, uint64) {
	cfirst := C.uint64_t(first)
	clast := C.uint64_t(last)
	ret := uint64(C.find_first_sub_le_uint64_t((*C.uchar)(unsafe.Pointer(uintptr(mf.Src)+uintptr(offset))), C.uint64_t(blklen), C.uint64_t(blkpos), C.uint8_t(bitstart), C.uint8_t(bitend), &cfirst, &clast, C.uint64_t(search)))
	return ret, uint64(cfirst), uint64(clast)
}

// FindLastSubLEUint64 search for the last occurrence of a LE bit set contained in a 64 bit unsigned integer on a memory mapped
// binary file containing adjacent blocks of sorted binary data.
// The 64 bit values in the file must encoded in big-endian format and sorted in ascending order.
// Return the item number if found or (last + 1) if not found, plus the first and last positions.
func (mf TMMFile) FindLastSubLEUint64(offset, blklen, blkpos uint64, bitstart, bitend uint8, first, last uint64, search uint64) (uint64, uint64, uint64) {
	cfirst := C.uint64_t(first)
	clast := C.uint64_t(last)
	ret := uint64(C.find_last_sub_le_uint64_t((*C.uchar)(unsafe.Pointer(uintptr(mf.Src)+uintptr(offset))), C.uint64_t(blklen), C.uint64_t(blkpos), C.uint8_t(bitstart), C.uint8_t(bitend), &cfirst, &clast, C.uint64_t(search)))
	return ret, uint64(cfirst), uint64(clast)
}

// HasNextLEUint8 checks if the next occurrence of a LE unsigned integer on a memory mapped
// binary file containing adjacent blocks of sorted binary data still matches the search value.
// The values in the file must encoded in big-endian format and sorted in ascending order.
// This function can be used after FindFirstLEUint8 to get the next elements that still satisfy the search.
// The item returned by FindFirstLEUint8 should be set as the "pos" parameter in this function.
// Return true if the next item is valid, false otherwise, plus the position.
func (mf TMMFile) HasNextLEUint8(offset, blklen, blkpos, pos, last uint64, search uint8) (bool, uint64) {
	cpos := C.uint64_t(pos)
	ret := bool(C.has_next_le_uint8_t((*C.uchar)(unsafe.Pointer(uintptr(mf.Src)+uintptr(offset))), C.uint64_t(blklen), C.uint64_t(blkpos), &cpos, C.uint64_t(last), C.uint8_t(search)))
	return ret, uint64(cpos)
}

// HasNextLEUint16 checks if the next occurrence of a LE unsigned integer on a memory mapped
// binary file containing adjacent blocks of sorted binary data still matches the search value.
// The values in the file must encoded in big-endian format and sorted in ascending order.
// This function can be used after FindFirstLEUint16 to get the next elements that still satisfy the search.
// The item returned by FindFirstLEUint16 should be set as the "pos" parameter in this function.
// Return true if the next item is valid, false otherwise, plus the position.
func (mf TMMFile) HasNextLEUint16(offset, blklen, blkpos, pos, last uint64, search uint16) (bool, uint64) {
	cpos := C.uint64_t(pos)
	ret := bool(C.has_next_le_uint16_t((*C.uchar)(unsafe.Pointer(uintptr(mf.Src)+uintptr(offset))), C.uint64_t(blklen), C.uint64_t(blkpos), &cpos, C.uint64_t(last), C.uint16_t(search)))
	return ret, uint64(cpos)
}

// HasNextLEUint32 checks if the next occurrence of a LE unsigned integer on a memory mapped
// binary file containing adjacent blocks of sorted binary data still matches the search value.
// The values in the file must encoded in big-endian format and sorted in ascending order.
// This function can be used after FindFirstLEUint32 to get the next elements that still satisfy the search.
// The item returned by FindFirstLEUint32 should be set as the "pos" parameter in this function.
// Return true if the next item is valid, false otherwise, plus the position.
func (mf TMMFile) HasNextLEUint32(offset, blklen, blkpos, pos, last uint64, search uint32) (bool, uint64) {
	cpos := C.uint64_t(pos)
	ret := bool(C.has_next_le_uint32_t((*C.uchar)(unsafe.Pointer(uintptr(mf.Src)+uintptr(offset))), C.uint64_t(blklen), C.uint64_t(blkpos), &cpos, C.uint64_t(last), C.uint32_t(search)))
	return ret, uint64(cpos)
}

// HasNextLEUint64 checks if the next occurrence of a LE unsigned integer on a memory mapped
// binary file containing adjacent blocks of sorted binary data still matches the search value.
// The values in the file must encoded in big-endian format and sorted in ascending order.
// This function can be used after FindFirstLEUint64 to get the next elements that still satisfy the search.
// The item returned by FindFirstLEUint64 should be set as the "pos" parameter in this function.
// Return true if the next item is valid, false otherwise, plus the position.
func (mf TMMFile) HasNextLEUint64(offset, blklen, blkpos, pos, last uint64, search uint64) (bool, uint64) {
	cpos := C.uint64_t(pos)
	ret := bool(C.has_next_le_uint64_t((*C.uchar)(unsafe.Pointer(uintptr(mf.Src)+uintptr(offset))), C.uint64_t(blklen), C.uint64_t(blkpos), &cpos, C.uint64_t(last), C.uint64_t(search)))
	return ret, uint64(cpos)
}

// HasNextSubLEUint8 checks if the next occurrence of a LE unsigned integer on a memory mapped
// binary file containing adjacent blocks of sorted binary data still matches the search value.
// The values in the file must encoded in big-endian format and sorted in ascending order.
// This function can be used after FindFirstLEUint8 to get the next elements that still satisfy the search.
// The item returned by FindFirstLEUint8 should be set as the "pos" parameter in this function.
// Return true if the next item is valid, false otherwise, plus the position.
func (mf TMMFile) HasNextSubLEUint8(offset, blklen, blkpos uint64, bitstart, bitend uint8, pos, last uint64, search uint8) (bool, uint64) {
	cpos := C.uint64_t(pos)
	ret := bool(C.has_next_sub_le_uint8_t((*C.uchar)(unsafe.Pointer(uintptr(mf.Src)+uintptr(offset))), C.uint64_t(blklen), C.uint64_t(blkpos), C.uint8_t(bitstart), C.uint8_t(bitend), &cpos, C.uint64_t(last), C.uint8_t(search)))
	return ret, uint64(cpos)
}

// HasNextSubLEUint16 checks if the next occurrence of a LE unsigned integer on a memory mapped
// binary file containing adjacent blocks of sorted binary data still matches the search value.
// The values in the file must encoded in big-endian format and sorted in ascending order.
// This function can be used after FindFirstLEUint16 to get the next elements that still satisfy the search.
// The item returned by FindFirstLEUint16 should be set as the "pos" parameter in this function.
// Return true if the next item is valid, false otherwise, plus the position.
func (mf TMMFile) HasNextSubLEUint16(offset, blklen, blkpos uint64, bitstart, bitend uint8, pos, last uint64, search uint16) (bool, uint64) {
	cpos := C.uint64_t(pos)
	ret := bool(C.has_next_sub_le_uint16_t((*C.uchar)(unsafe.Pointer(uintptr(mf.Src)+uintptr(offset))), C.uint64_t(blklen), C.uint64_t(blkpos), C.uint8_t(bitstart), C.uint8_t(bitend), &cpos, C.uint64_t(last), C.uint16_t(search)))
	return ret, uint64(cpos)
}

// HasNextSubLEUint32 checks if the next occurrence of a LE unsigned integer on a memory mapped
// binary file containing adjacent blocks of sorted binary data still matches the search value.
// The values in the file must encoded in big-endian format and sorted in ascending order.
// This function can be used after FindFirstLEUint32 to get the next elements that still satisfy the search.
// The item returned by FindFirstLEUint32 should be set as the "pos" parameter in this function.
// Return true if the next item is valid, false otherwise, plus the position.
func (mf TMMFile) HasNextSubLEUint32(offset, blklen, blkpos uint64, bitstart, bitend uint8, pos, last uint64, search uint32) (bool, uint64) {
	cpos := C.uint64_t(pos)
	ret := bool(C.has_next_sub_le_uint32_t((*C.uchar)(unsafe.Pointer(uintptr(mf.Src)+uintptr(offset))), C.uint64_t(blklen), C.uint64_t(blkpos), C.uint8_t(bitstart), C.uint8_t(bitend), &cpos, C.uint64_t(last), C.uint32_t(search)))
	return ret, uint64(cpos)
}

// HasNextSubLEUint64 checks if the next occurrence of a LE unsigned integer on a memory mapped
// binary file containing adjacent blocks of sorted binary data still matches the search value.
// The values in the file must encoded in big-endian format and sorted in ascending order.
// This function can be used after FindFirstLEUint64 to get the next elements that still satisfy the search.
// The item returned by FindFirstLEUint64 should be set as the "pos" parameter in this function.
// Return true if the next item is valid, false otherwise, plus the position.
func (mf TMMFile) HasNextSubLEUint64(offset, blklen, blkpos uint64, bitstart, bitend uint8, pos, last uint64, search uint64) (bool, uint64) {
	cpos := C.uint64_t(pos)
	ret := bool(C.has_next_sub_le_uint64_t((*C.uchar)(unsafe.Pointer(uintptr(mf.Src)+uintptr(offset))), C.uint64_t(blklen), C.uint64_t(blkpos), C.uint8_t(bitstart), C.uint8_t(bitend), &cpos, C.uint64_t(last), C.uint64_t(search)))
	return ret, uint64(cpos)
}

// HasPrevLEUint8 checks if the previous occurrence of a LE unsigned integer on a memory mapped
// binary file containing adjacent blocks of sorted binary data still matches the search value.
// The values in the file must encoded in big-endian format and sorted in ascending order.
// This function can be used after FindLastLEUint8 to get the previous elements that still satisfy the search.
// The item returned by FindLastLEUint8 should be set as the "pos" parameter in this function.
// Return true if the previous item is valid, false otherwise, plus the position.
func (mf TMMFile) HasPrevLEUint8(offset, blklen, blkpos, first, pos uint64, search uint8) (bool, uint64) {
	cpos := C.uint64_t(pos)
	ret := bool(C.has_prev_le_uint8_t((*C.uchar)(unsafe.Pointer(uintptr(mf.Src)+uintptr(offset))), C.uint64_t(blklen), C.uint64_t(blkpos), C.uint64_t(first), &cpos, C.uint8_t(search)))
	return ret, uint64(cpos)
}

// HasPrevLEUint16 checks if the previous occurrence of a LE unsigned integer on a memory mapped
// binary file containing adjacent blocks of sorted binary data still matches the search value.
// The values in the file must encoded in big-endian format and sorted in ascending order.
// This function can be used after FindLastLEUint16 to get the previous elements that still satisfy the search.
// The item returned by FindLastLEUint16 should be set as the "pos" parameter in this function.
// Return true if the previous item is valid, false otherwise, plus the position.
func (mf TMMFile) HasPrevLEUint16(offset, blklen, blkpos, first, pos uint64, search uint16) (bool, uint64) {
	cpos := C.uint64_t(pos)
	ret := bool(C.has_prev_le_uint16_t((*C.uchar)(unsafe.Pointer(uintptr(mf.Src)+uintptr(offset))), C.uint64_t(blklen), C.uint64_t(blkpos), C.uint64_t(first), &cpos, C.uint16_t(search)))
	return ret, uint64(cpos)
}

// HasPrevLEUint32 checks if the previous occurrence of a LE unsigned integer on a memory mapped
// binary file containing adjacent blocks of sorted binary data still matches the search value.
// The values in the file must encoded in big-endian format and sorted in ascending order.
// This function can be used after FindLastLEUint32 to get the previous elements that still satisfy the search.
// The item returned by FindLastLEUint32 should be set as the "pos" parameter in this function.
// Return true if the previous item is valid, false otherwise, plus the position.
func (mf TMMFile) HasPrevLEUint32(offset, blklen, blkpos, first, pos uint64, search uint32) (bool, uint64) {
	cpos := C.uint64_t(pos)
	ret := bool(C.has_prev_le_uint32_t((*C.uchar)(unsafe.Pointer(uintptr(mf.Src)+uintptr(offset))), C.uint64_t(blklen), C.uint64_t(blkpos), C.uint64_t(first), &cpos, C.uint32_t(search)))
	return ret, uint64(cpos)
}

// HasPrevLEUint64 checks if the previous occurrence of a LE unsigned integer on a memory mapped
// binary file containing adjacent blocks of sorted binary data still matches the search value.
// The values in the file must encoded in big-endian format and sorted in ascending order.
// This function can be used after FindLastLEUint64 to get the previous elements that still satisfy the search.
// The item returned by FindLastLEUint64 should be set as the "pos" parameter in this function.
// Return true if the previous item is valid, false otherwise, plus the position.
func (mf TMMFile) HasPrevLEUint64(offset, blklen, blkpos, first, pos uint64, search uint64) (bool, uint64) {
	cpos := C.uint64_t(pos)
	ret := bool(C.has_prev_le_uint64_t((*C.uchar)(unsafe.Pointer(uintptr(mf.Src)+uintptr(offset))), C.uint64_t(blklen), C.uint64_t(blkpos), C.uint64_t(first), &cpos, C.uint64_t(search)))
	return ret, uint64(cpos)
}

// HasPrevSubLEUint8 checks if the previous occurrence of a LE unsigned integer on a memory mapped
// binary file containing adjacent blocks of sorted binary data still matches the search value.
// The values in the file must encoded in big-endian format and sorted in ascending order.
// This function can be used after FindLastLEUint8 to get the previous elements that still satisfy the search.
// The item returned by FindLastLEUint8 should be set as the "pos" parameter in this function.
// Return true if the previous item is valid, false otherwise, plus the position.
func (mf TMMFile) HasPrevSubLEUint8(offset, blklen, blkpos uint64, bitstart, bitend uint8, first, pos uint64, search uint8) (bool, uint64) {
	cpos := C.uint64_t(pos)
	ret := bool(C.has_prev_sub_le_uint8_t((*C.uchar)(unsafe.Pointer(uintptr(mf.Src)+uintptr(offset))), C.uint64_t(blklen), C.uint64_t(blkpos), C.uint8_t(bitstart), C.uint8_t(bitend), C.uint64_t(first), &cpos, C.uint8_t(search)))
	return ret, uint64(cpos)
}

// HasPrevSubLEUint16 checks if the previous occurrence of a LE unsigned integer on a memory mapped
// binary file containing adjacent blocks of sorted binary data still matches the search value.
// The values in the file must encoded in big-endian format and sorted in ascending order.
// This function can be used after FindLastLEUint16 to get the previous elements that still satisfy the search.
// The item returned by FindLastLEUint16 should be set as the "pos" parameter in this function.
// Return true if the previous item is valid, false otherwise, plus the position.
func (mf TMMFile) HasPrevSubLEUint16(offset, blklen, blkpos uint64, bitstart, bitend uint8, first, pos uint64, search uint16) (bool, uint64) {
	cpos := C.uint64_t(pos)
	ret := bool(C.has_prev_sub_le_uint16_t((*C.uchar)(unsafe.Pointer(uintptr(mf.Src)+uintptr(offset))), C.uint64_t(blklen), C.uint64_t(blkpos), C.uint8_t(bitstart), C.uint8_t(bitend), C.uint64_t(first), &cpos, C.uint16_t(search)))
	return ret, uint64(cpos)
}

// HasPrevSubLEUint32 checks if the previous occurrence of a LE unsigned integer on a memory mapped
// binary file containing adjacent blocks of sorted binary data still matches the search value.
// The values in the file must encoded in big-endian format and sorted in ascending order.
// This function can be used after FindLastLEUint32 to get the previous elements that still satisfy the search.
// The item returned by FindLastLEUint32 should be set as the "pos" parameter in this function.
// Return true if the previous item is valid, false otherwise, plus the position.
func (mf TMMFile) HasPrevSubLEUint32(offset, blklen, blkpos uint64, bitstart, bitend uint8, first, pos uint64, search uint32) (bool, uint64) {
	cpos := C.uint64_t(pos)
	ret := bool(C.has_prev_sub_le_uint32_t((*C.uchar)(unsafe.Pointer(uintptr(mf.Src)+uintptr(offset))), C.uint64_t(blklen), C.uint64_t(blkpos), C.uint8_t(bitstart), C.uint8_t(bitend), C.uint64_t(first), &cpos, C.uint32_t(search)))
	return ret, uint64(cpos)
}

// HasPrevSubLEUint64 checks if the previous occurrence of a LE unsigned integer on a memory mapped
// binary file containing adjacent blocks of sorted binary data still matches the search value.
// The values in the file must encoded in big-endian format and sorted in ascending order.
// This function can be used after FindLastLEUint64 to get the previous elements that still satisfy the search.
// The item returned by FindLastLEUint64 should be set as the "pos" parameter in this function.
// Return true if the previous item is valid, false otherwise, plus the position.
func (mf TMMFile) HasPrevSubLEUint64(offset, blklen, blkpos uint64, bitstart, bitend uint8, first, pos uint64, search uint64) (bool, uint64) {
	cpos := C.uint64_t(pos)
	ret := bool(C.has_prev_sub_le_uint64_t((*C.uchar)(unsafe.Pointer(uintptr(mf.Src)+uintptr(offset))), C.uint64_t(blklen), C.uint64_t(blkpos), C.uint8_t(bitstart), C.uint8_t(bitend), C.uint64_t(first), &cpos, C.uint64_t(search)))
	return ret, uint64(cpos)
}

// --- COLUMN ---

// ColFindFirstUint8 search for the first occurrence of a LE 8 bit unsigned integer
// on a memory buffer containing contiguos blocks of unsigned integers of the same type.
// The 8 bit values in the file must encoded in big-endian format and sorted in ascending order.
// Return the item number if found or (last + 1) if not found, plus the first and last positions.
func (mf TMMFile) ColFindFirstUint8(offset, first, last uint64, search uint8) (uint64, uint64, uint64) {
	cfirst := C.uint64_t(first)
	clast := C.uint64_t(last)
	ret := uint64(C.col_find_first_uint8_t((*C.uint8_t)(unsafe.Pointer(uintptr(mf.Src)+uintptr(offset))), &cfirst, &clast, C.uint8_t(search)))
	return ret, uint64(cfirst), uint64(clast)
}

// ColFindLastUint8 search for the last occurrence of a LE 8 bit unsigned integer
// on a memory buffer containing contiguos blocks of unsigned integers of the same type.
// The 8 bit values in the file must encoded in big-endian format and sorted in ascending order.
// Return the item number if found or (last + 1) if not found, plus the first and last positions.
func (mf TMMFile) ColFindLastUint8(offset, first, last uint64, search uint8) (uint64, uint64, uint64) {
	cfirst := C.uint64_t(first)
	clast := C.uint64_t(last)
	ret := uint64(C.col_find_last_uint8_t((*C.uint8_t)(unsafe.Pointer(uintptr(mf.Src)+uintptr(offset))), &cfirst, &clast, C.uint8_t(search)))
	return ret, uint64(cfirst), uint64(clast)
}

// ColFindFirstUint16 search for the first occurrence of a LE 16 bit unsigned integer
// on a memory buffer containing contiguos blocks of unsigned integers of the same type.
// The 16 bit values in the file must encoded in big-endian format and sorted in ascending order.
// Return the item number if found or (last + 1) if not found, plus the first and last positions.
func (mf TMMFile) ColFindFirstUint16(offset, first, last uint64, search uint16) (uint64, uint64, uint64) {
	cfirst := C.uint64_t(first)
	clast := C.uint64_t(last)
	ret := uint64(C.col_find_first_uint16_t((*C.uint16_t)(unsafe.Pointer(uintptr(mf.Src)+uintptr(offset))), &cfirst, &clast, C.uint16_t(search)))
	return ret, uint64(cfirst), uint64(clast)
}

// ColFindLastUint16 search for the last occurrence of a LE 16 bit unsigned integer
// on a memory buffer containing contiguos blocks of unsigned integers of the same type.
// The 16 bit values in the file must encoded in big-endian format and sorted in ascending order.
// Return the item number if found or (last + 1) if not found, plus the first and last positions.
func (mf TMMFile) ColFindLastUint16(offset, first, last uint64, search uint16) (uint64, uint64, uint64) {
	cfirst := C.uint64_t(first)
	clast := C.uint64_t(last)
	ret := uint64(C.col_find_last_uint16_t((*C.uint16_t)(unsafe.Pointer(uintptr(mf.Src)+uintptr(offset))), &cfirst, &clast, C.uint16_t(search)))
	return ret, uint64(cfirst), uint64(clast)
}

// ColFindFirstUint32 search for the first occurrence of a LE 32 bit unsigned integer
// on a memory buffer containing contiguos blocks of unsigned integers of the same type.
// The 32 bit values in the file must encoded in big-endian format and sorted in ascending order.
// Return the item number if found or (last + 1) if not found, plus the first and last positions.
func (mf TMMFile) ColFindFirstUint32(offset, first, last uint64, search uint32) (uint64, uint64, uint64) {
	cfirst := C.uint64_t(first)
	clast := C.uint64_t(last)
	ret := uint64(C.col_find_first_uint32_t((*C.uint32_t)(unsafe.Pointer(uintptr(mf.Src)+uintptr(offset))), &cfirst, &clast, C.uint32_t(search)))
	return ret, uint64(cfirst), uint64(clast)
}

// ColFindLastUint32 search for the last occurrence of a LE 32 bit unsigned integer
// on a memory buffer containing contiguos blocks of unsigned integers of the same type.
// The 32 bit values in the file must encoded in big-endian format and sorted in ascending order.
// Return the item number if found or (last + 1) if not found, plus the first and last positions.
func (mf TMMFile) ColFindLastUint32(offset, first, last uint64, search uint32) (uint64, uint64, uint64) {
	cfirst := C.uint64_t(first)
	clast := C.uint64_t(last)
	ret := uint64(C.col_find_last_uint32_t((*C.uint32_t)(unsafe.Pointer(uintptr(mf.Src)+uintptr(offset))), &cfirst, &clast, C.uint32_t(search)))
	return ret, uint64(cfirst), uint64(clast)
}

// ColFindFirstUint64 search for the first occurrence of a LE 64 bit unsigned integer
// on a memory buffer containing contiguos blocks of unsigned integers of the same type.
// The 64 bit values in the file must encoded in big-endian format and sorted in ascending order.
// Return the item number if found or (last + 1) if not found, plus the first and last positions.
func (mf TMMFile) ColFindFirstUint64(offset, first, last uint64, search uint64) (uint64, uint64, uint64) {
	cfirst := C.uint64_t(first)
	clast := C.uint64_t(last)
	ret := uint64(C.col_find_first_uint64_t((*C.uint64_t)(unsafe.Pointer(uintptr(mf.Src)+uintptr(offset))), &cfirst, &clast, C.uint64_t(search)))
	return ret, uint64(cfirst), uint64(clast)
}

// ColFindLastUint64 search for the last occurrence of a LE 64 bit unsigned integer
// on a memory buffer containing contiguos blocks of unsigned integers of the same type.
// The 64 bit values in the file must encoded in big-endian format and sorted in ascending order.
// Return the item number if found or (last + 1) if not found, plus the first and last positions.
func (mf TMMFile) ColFindLastUint64(offset, first, last uint64, search uint64) (uint64, uint64, uint64) {
	cfirst := C.uint64_t(first)
	clast := C.uint64_t(last)
	ret := uint64(C.col_find_last_uint64_t((*C.uint64_t)(unsafe.Pointer(uintptr(mf.Src)+uintptr(offset))), &cfirst, &clast, C.uint64_t(search)))
	return ret, uint64(cfirst), uint64(clast)
}

// ColFindFirstSubUint8 search for the first occurrence of a LE bit set contained in a 8 bit unsigned integer
// on a memory buffer containing contiguos blocks of unsigned integers of the same type.
// The 8 bit values in the file must encoded in big-endian format and sorted in ascending order.
// Return the item number if found or (last + 1) if not found, plus the first and last positions.
func (mf TMMFile) ColFindFirstSubUint8(offset uint64, bitstart, bitend uint8, first, last uint64, search uint8) (uint64, uint64, uint64) {
	cfirst := C.uint64_t(first)
	clast := C.uint64_t(last)
	ret := uint64(C.col_find_first_sub_uint8_t((*C.uint8_t)(unsafe.Pointer(uintptr(mf.Src)+uintptr(offset))), C.uint8_t(bitstart), C.uint8_t(bitend), &cfirst, &clast, C.uint8_t(search)))
	return ret, uint64(cfirst), uint64(clast)
}

// ColFindLastSubUint8 search for the last occurrence of a LE bit set contained in a 8 bit unsigned integer
// on a memory buffer containing contiguos blocks of unsigned integers of the same type.
// The 8 bit values in the file must encoded in big-endian format and sorted in ascending order.
// Return the item number if found or (last + 1) if not found, plus the first and last positions.
func (mf TMMFile) ColFindLastSubUint8(offset uint64, bitstart, bitend uint8, first, last uint64, search uint8) (uint64, uint64, uint64) {
	cfirst := C.uint64_t(first)
	clast := C.uint64_t(last)
	ret := uint64(C.col_find_last_sub_uint8_t((*C.uint8_t)(unsafe.Pointer(uintptr(mf.Src)+uintptr(offset))), C.uint8_t(bitstart), C.uint8_t(bitend), &cfirst, &clast, C.uint8_t(search)))
	return ret, uint64(cfirst), uint64(clast)
}

// ColFindFirstSubUint16 search for the first occurrence of a LE bit set contained in a 16 bit unsigned integer
// on a memory buffer containing contiguos blocks of unsigned integers of the same type.
// The 16 bit values in the file must encoded in big-endian format and sorted in ascending order.
// Return the item number if found or (last + 1) if not found, plus the first and last positions.
func (mf TMMFile) ColFindFirstSubUint16(offset uint64, bitstart, bitend uint8, first, last uint64, search uint16) (uint64, uint64, uint64) {
	cfirst := C.uint64_t(first)
	clast := C.uint64_t(last)
	ret := uint64(C.col_find_first_sub_uint16_t((*C.uint16_t)(unsafe.Pointer(uintptr(mf.Src)+uintptr(offset))), C.uint8_t(bitstart), C.uint8_t(bitend), &cfirst, &clast, C.uint16_t(search)))
	return ret, uint64(cfirst), uint64(clast)
}

// ColFindLastSubUint16 search for the last occurrence of a LE bit set contained in a 16 bit unsigned integer
// on a memory buffer containing contiguos blocks of unsigned integers of the same type.
// The 16 bit values in the file must encoded in big-endian format and sorted in ascending order.
// Return the item number if found or (last + 1) if not found, plus the first and last positions.
func (mf TMMFile) ColFindLastSubUint16(offset uint64, bitstart, bitend uint8, first, last uint64, search uint16) (uint64, uint64, uint64) {
	cfirst := C.uint64_t(first)
	clast := C.uint64_t(last)
	ret := uint64(C.col_find_last_sub_uint16_t((*C.uint16_t)(unsafe.Pointer(uintptr(mf.Src)+uintptr(offset))), C.uint8_t(bitstart), C.uint8_t(bitend), &cfirst, &clast, C.uint16_t(search)))
	return ret, uint64(cfirst), uint64(clast)
}

// ColFindFirstSubUint32 search for the first occurrence of a LE bit set contained in a 32 bit unsigned integer
// on a memory buffer containing contiguos blocks of unsigned integers of the same type.
// The 32 bit values in the file must encoded in big-endian format and sorted in ascending order.
// Return the item number if found or (last + 1) if not found, plus the first and last positions.
func (mf TMMFile) ColFindFirstSubUint32(offset uint64, bitstart, bitend uint8, first, last uint64, search uint32) (uint64, uint64, uint64) {
	cfirst := C.uint64_t(first)
	clast := C.uint64_t(last)
	ret := uint64(C.col_find_first_sub_uint32_t((*C.uint32_t)(unsafe.Pointer(uintptr(mf.Src)+uintptr(offset))), C.uint8_t(bitstart), C.uint8_t(bitend), &cfirst, &clast, C.uint32_t(search)))
	return ret, uint64(cfirst), uint64(clast)
}

// ColFindLastSubUint32 search for the last occurrence of a LE bit set contained in a 32 bit unsigned integer
// on a memory buffer containing contiguos blocks of unsigned integers of the same type.
// The 32 bit values in the file must encoded in big-endian format and sorted in ascending order.
// Return the item number if found or (last + 1) if not found, plus the first and last positions.
func (mf TMMFile) ColFindLastSubUint32(offset uint64, bitstart, bitend uint8, first, last uint64, search uint32) (uint64, uint64, uint64) {
	cfirst := C.uint64_t(first)
	clast := C.uint64_t(last)
	ret := uint64(C.col_find_last_sub_uint32_t((*C.uint32_t)(unsafe.Pointer(uintptr(mf.Src)+uintptr(offset))), C.uint8_t(bitstart), C.uint8_t(bitend), &cfirst, &clast, C.uint32_t(search)))
	return ret, uint64(cfirst), uint64(clast)
}

// ColFindFirstSubUint64 search for the first occurrence of a LE bit set contained in a 64 bit unsigned integer
// on a memory buffer containing contiguos blocks of unsigned integers of the same type.
// The 64 bit values in the file must encoded in big-endian format and sorted in ascending order.
// Return the item number if found or (last + 1) if not found, plus the first and last positions.
func (mf TMMFile) ColFindFirstSubUint64(offset uint64, bitstart, bitend uint8, first, last uint64, search uint64) (uint64, uint64, uint64) {
	cfirst := C.uint64_t(first)
	clast := C.uint64_t(last)
	ret := uint64(C.col_find_first_sub_uint64_t((*C.uint64_t)(unsafe.Pointer(uintptr(mf.Src)+uintptr(offset))), C.uint8_t(bitstart), C.uint8_t(bitend), &cfirst, &clast, C.uint64_t(search)))
	return ret, uint64(cfirst), uint64(clast)
}

// ColFindLastSubUint64 search for the last occurrence of a LE bit set contained in a 64 bit unsigned integer
// on a memory buffer containing contiguos blocks of unsigned integers of the same type.
// The 64 bit values in the file must encoded in big-endian format and sorted in ascending order.
// Return the item number if found or (last + 1) if not found, plus the first and last positions.
func (mf TMMFile) ColFindLastSubUint64(offset uint64, bitstart, bitend uint8, first, last uint64, search uint64) (uint64, uint64, uint64) {
	cfirst := C.uint64_t(first)
	clast := C.uint64_t(last)
	ret := uint64(C.col_find_last_sub_uint64_t((*C.uint64_t)(unsafe.Pointer(uintptr(mf.Src)+uintptr(offset))), C.uint8_t(bitstart), C.uint8_t(bitend), &cfirst, &clast, C.uint64_t(search)))
	return ret, uint64(cfirst), uint64(clast)
}

// ColHasNextUint8 checks if the next occurrence of a LE unsigned integer
// on a memory buffer containing contiguos blocks of unsigned integers of the same type.
// The values in the file must encoded in big-endian format and sorted in ascending order.
// This function can be used after ColFindFirstUint8 to get the next elements that still satisfy the search.
// The item returned by ColFindFirstUint8 should be set as the "pos" parameter in this function.
// Return true if the next item is valid, false otherwise, plus the position.
func (mf TMMFile) ColHasNextUint8(offset, pos, last uint64, search uint8) (bool, uint64) {
	cpos := C.uint64_t(pos)
	ret := bool(C.col_has_next_uint8_t((*C.uint8_t)(unsafe.Pointer(uintptr(mf.Src)+uintptr(offset))), &cpos, C.uint64_t(last), C.uint8_t(search)))
	return ret, uint64(cpos)
}

// ColHasNextUint16 checks if the next occurrence of a LE unsigned integer
// on a memory buffer containing contiguos blocks of unsigned integers of the same type.
// The values in the file must encoded in big-endian format and sorted in ascending order.
// This function can be used after ColFindFirstUint16 to get the next elements that still satisfy the search.
// The item returned by ColFindFirstUint16 should be set as the "pos" parameter in this function.
// Return true if the next item is valid, false otherwise, plus the position.
func (mf TMMFile) ColHasNextUint16(offset, pos, last uint64, search uint16) (bool, uint64) {
	cpos := C.uint64_t(pos)
	ret := bool(C.col_has_next_uint16_t((*C.uint16_t)(unsafe.Pointer(uintptr(mf.Src)+uintptr(offset))), &cpos, C.uint64_t(last), C.uint16_t(search)))
	return ret, uint64(cpos)
}

// ColHasNextUint32 checks if the next occurrence of a LE unsigned integer
// on a memory buffer containing contiguos blocks of unsigned integers of the same type.
// The values in the file must encoded in big-endian format and sorted in ascending order.
// This function can be used after ColFindFirstUint32 to get the next elements that still satisfy the search.
// The item returned by ColFindFirstUint32 should be set as the "pos" parameter in this function.
// Return true if the next item is valid, false otherwise, plus the position.
func (mf TMMFile) ColHasNextUint32(offset, pos, last uint64, search uint32) (bool, uint64) {
	cpos := C.uint64_t(pos)
	ret := bool(C.col_has_next_uint32_t((*C.uint32_t)(unsafe.Pointer(uintptr(mf.Src)+uintptr(offset))), &cpos, C.uint64_t(last), C.uint32_t(search)))
	return ret, uint64(cpos)
}

// ColHasNextUint64 checks if the next occurrence of a LE unsigned integer
// on a memory buffer containing contiguos blocks of unsigned integers of the same type.
// The values in the file must encoded in big-endian format and sorted in ascending order.
// This function can be used after ColFindFirstUint64 to get the next elements that still satisfy the search.
// The item returned by ColFindFirstUint64 should be set as the "pos" parameter in this function.
// Return true if the next item is valid, false otherwise, plus the position.
func (mf TMMFile) ColHasNextUint64(offset, pos, last uint64, search uint64) (bool, uint64) {
	cpos := C.uint64_t(pos)
	ret := bool(C.col_has_next_uint64_t((*C.uint64_t)(unsafe.Pointer(uintptr(mf.Src)+uintptr(offset))), &cpos, C.uint64_t(last), C.uint64_t(search)))
	return ret, uint64(cpos)
}

// ColHasNextSubUint8 checks if the next occurrence of a LE unsigned integer
// on a memory buffer containing contiguos blocks of unsigned integers of the same type.
// The values in the file must encoded in big-endian format and sorted in ascending order.
// This function can be used after ColFindFirstUint8 to get the next elements that still satisfy the search.
// The item returned by ColFindFirstUint8 should be set as the "pos" parameter in this function.
// Return true if the next item is valid, false otherwise, plus the position.
func (mf TMMFile) ColHasNextSubUint8(offset uint64, bitstart, bitend uint8, pos, last uint64, search uint8) (bool, uint64) {
	cpos := C.uint64_t(pos)
	ret := bool(C.col_has_next_sub_uint8_t((*C.uint8_t)(unsafe.Pointer(uintptr(mf.Src)+uintptr(offset))), C.uint8_t(bitstart), C.uint8_t(bitend), &cpos, C.uint64_t(last), C.uint8_t(search)))
	return ret, uint64(cpos)
}

// ColHasNextSubUint16 checks if the next occurrence of a LE unsigned integer
// on a memory buffer containing contiguos blocks of unsigned integers of the same type.
// The values in the file must encoded in big-endian format and sorted in ascending order.
// This function can be used after ColFindFirstUint16 to get the next elements that still satisfy the search.
// The item returned by ColFindFirstUint16 should be set as the "pos" parameter in this function.
// Return true if the next item is valid, false otherwise, plus the position.
func (mf TMMFile) ColHasNextSubUint16(offset uint64, bitstart, bitend uint8, pos, last uint64, search uint16) (bool, uint64) {
	cpos := C.uint64_t(pos)
	ret := bool(C.col_has_next_sub_uint16_t((*C.uint16_t)(unsafe.Pointer(uintptr(mf.Src)+uintptr(offset))), C.uint8_t(bitstart), C.uint8_t(bitend), &cpos, C.uint64_t(last), C.uint16_t(search)))
	return ret, uint64(cpos)
}

// ColHasNextSubUint32 checks if the next occurrence of a LE unsigned integer
// on a memory buffer containing contiguos blocks of unsigned integers of the same type.
// The values in the file must encoded in big-endian format and sorted in ascending order.
// This function can be used after ColFindFirstUint32 to get the next elements that still satisfy the search.
// The item returned by ColFindFirstUint32 should be set as the "pos" parameter in this function.
// Return true if the next item is valid, false otherwise, plus the position.
func (mf TMMFile) ColHasNextSubUint32(offset uint64, bitstart, bitend uint8, pos, last uint64, search uint32) (bool, uint64) {
	cpos := C.uint64_t(pos)
	ret := bool(C.col_has_next_sub_uint32_t((*C.uint32_t)(unsafe.Pointer(uintptr(mf.Src)+uintptr(offset))), C.uint8_t(bitstart), C.uint8_t(bitend), &cpos, C.uint64_t(last), C.uint32_t(search)))
	return ret, uint64(cpos)
}

// ColHasNextSubUint64 checks if the next occurrence of a LE unsigned integer
// on a memory buffer containing contiguos blocks of unsigned integers of the same type.
// The values in the file must encoded in big-endian format and sorted in ascending order.
// This function can be used after ColFindFirstUint64 to get the next elements that still satisfy the search.
// The item returned by ColFindFirstUint64 should be set as the "pos" parameter in this function.
// Return true if the next item is valid, false otherwise, plus the position.
func (mf TMMFile) ColHasNextSubUint64(offset uint64, bitstart, bitend uint8, pos, last uint64, search uint64) (bool, uint64) {
	cpos := C.uint64_t(pos)
	ret := bool(C.col_has_next_sub_uint64_t((*C.uint64_t)(unsafe.Pointer(uintptr(mf.Src)+uintptr(offset))), C.uint8_t(bitstart), C.uint8_t(bitend), &cpos, C.uint64_t(last), C.uint64_t(search)))
	return ret, uint64(cpos)
}

// ColHasPrevUint8 checks if the previous occurrence of a LE unsigned integer
// on a memory buffer containing contiguos blocks of unsigned integers of the same type.
// The values in the file must encoded in big-endian format and sorted in ascending order.
// This function can be used after ColFindLastUint8 to get the previous elements that still satisfy the search.
// The item returned by ColFindLastUint8 should be set as the "pos" parameter in this function.
// Return true if the previous item is valid, false otherwise, plus the position.
func (mf TMMFile) ColHasPrevUint8(offset, first, pos uint64, search uint8) (bool, uint64) {
	cpos := C.uint64_t(pos)
	ret := bool(C.col_has_prev_uint8_t((*C.uint8_t)(unsafe.Pointer(uintptr(mf.Src)+uintptr(offset))), C.uint64_t(first), &cpos, C.uint8_t(search)))
	return ret, uint64(cpos)
}

// ColHasPrevUint16 checks if the previous occurrence of a LE unsigned integer
// on a memory buffer containing contiguos blocks of unsigned integers of the same type.
// The values in the file must encoded in big-endian format and sorted in ascending order.
// This function can be used after ColFindLastUint16 to get the previous elements that still satisfy the search.
// The item returned by ColFindLastUint16 should be set as the "pos" parameter in this function.
// Return true if the previous item is valid, false otherwise, plus the position.
func (mf TMMFile) ColHasPrevUint16(offset, first, pos uint64, search uint16) (bool, uint64) {
	cpos := C.uint64_t(pos)
	ret := bool(C.col_has_prev_uint16_t((*C.uint16_t)(unsafe.Pointer(uintptr(mf.Src)+uintptr(offset))), C.uint64_t(first), &cpos, C.uint16_t(search)))
	return ret, uint64(cpos)
}

// ColHasPrevUint32 checks if the previous occurrence of a LE unsigned integer
// on a memory buffer containing contiguos blocks of unsigned integers of the same type.
// The values in the file must encoded in big-endian format and sorted in ascending order.
// This function can be used after ColFindLastUint32 to get the previous elements that still satisfy the search.
// The item returned by ColFindLastUint32 should be set as the "pos" parameter in this function.
// Return true if the previous item is valid, false otherwise, plus the position.
func (mf TMMFile) ColHasPrevUint32(offset, first, pos uint64, search uint32) (bool, uint64) {
	cpos := C.uint64_t(pos)
	ret := bool(C.col_has_prev_uint32_t((*C.uint32_t)(unsafe.Pointer(uintptr(mf.Src)+uintptr(offset))), C.uint64_t(first), &cpos, C.uint32_t(search)))
	return ret, uint64(cpos)
}

// ColHasPrevUint64 checks if the previous occurrence of a LE unsigned integer
// on a memory buffer containing contiguos blocks of unsigned integers of the same type.
// The values in the file must encoded in big-endian format and sorted in ascending order.
// This function can be used after ColFindLastUint64 to get the previous elements that still satisfy the search.
// The item returned by ColFindLastUint64 should be set as the "pos" parameter in this function.
// Return true if the previous item is valid, false otherwise, plus the position.
func (mf TMMFile) ColHasPrevUint64(offset, first, pos uint64, search uint64) (bool, uint64) {
	cpos := C.uint64_t(pos)
	ret := bool(C.col_has_prev_uint64_t((*C.uint64_t)(unsafe.Pointer(uintptr(mf.Src)+uintptr(offset))), C.uint64_t(first), &cpos, C.uint64_t(search)))
	return ret, uint64(cpos)
}

// ColHasPrevSubUint8 checks if the previous occurrence of a LE unsigned integer
// on a memory buffer containing contiguos blocks of unsigned integers of the same type.
// The values in the file must encoded in big-endian format and sorted in ascending order.
// This function can be used after ColFindLastUint8 to get the previous elements that still satisfy the search.
// The item returned by ColFindLastUint8 should be set as the "pos" parameter in this function.
// Return true if the previous item is valid, false otherwise, plus the position.
func (mf TMMFile) ColHasPrevSubUint8(offset uint64, bitstart, bitend uint8, first, pos uint64, search uint8) (bool, uint64) {
	cpos := C.uint64_t(pos)
	ret := bool(C.col_has_prev_sub_uint8_t((*C.uint8_t)(unsafe.Pointer(uintptr(mf.Src)+uintptr(offset))), C.uint8_t(bitstart), C.uint8_t(bitend), C.uint64_t(first), &cpos, C.uint8_t(search)))
	return ret, uint64(cpos)
}

// ColHasPrevSubUint16 checks if the previous occurrence of a LE unsigned integer
// on a memory buffer containing contiguos blocks of unsigned integers of the same type.
// The values in the file must encoded in big-endian format and sorted in ascending order.
// This function can be used after ColFindLastUint16 to get the previous elements that still satisfy the search.
// The item returned by ColFindLastUint16 should be set as the "pos" parameter in this function.
// Return true if the previous item is valid, false otherwise, plus the position.
func (mf TMMFile) ColHasPrevSubUint16(offset uint64, bitstart, bitend uint8, first, pos uint64, search uint16) (bool, uint64) {
	cpos := C.uint64_t(pos)
	ret := bool(C.col_has_prev_sub_uint16_t((*C.uint16_t)(unsafe.Pointer(uintptr(mf.Src)+uintptr(offset))), C.uint8_t(bitstart), C.uint8_t(bitend), C.uint64_t(first), &cpos, C.uint16_t(search)))
	return ret, uint64(cpos)
}

// ColHasPrevSubUint32 checks if the previous occurrence of a LE unsigned integer
// on a memory buffer containing contiguos blocks of unsigned integers of the same type.
// The values in the file must encoded in big-endian format and sorted in ascending order.
// This function can be used after ColFindLastUint32 to get the previous elements that still satisfy the search.
// The item returned by ColFindLastUint32 should be set as the "pos" parameter in this function.
// Return true if the previous item is valid, false otherwise, plus the position.
func (mf TMMFile) ColHasPrevSubUint32(offset uint64, bitstart, bitend uint8, first, pos uint64, search uint32) (bool, uint64) {
	cpos := C.uint64_t(pos)
	ret := bool(C.col_has_prev_sub_uint32_t((*C.uint32_t)(unsafe.Pointer(uintptr(mf.Src)+uintptr(offset))), C.uint8_t(bitstart), C.uint8_t(bitend), C.uint64_t(first), &cpos, C.uint32_t(search)))
	return ret, uint64(cpos)
}

// ColHasPrevSubUint64 checks if the previous occurrence of a LE unsigned integer
// on a memory buffer containing contiguos blocks of unsigned integers of the same type.
// The values in the file must encoded in big-endian format and sorted in ascending order.
// This function can be used after ColFindLastUint64 to get the previous elements that still satisfy the search.
// The item returned by ColFindLastUint64 should be set as the "pos" parameter in this function.
// Return true if the previous item is valid, false otherwise, plus the position.
func (mf TMMFile) ColHasPrevSubUint64(offset uint64, bitstart, bitend uint8, first, pos uint64, search uint64) (bool, uint64) {
	cpos := C.uint64_t(pos)
	ret := bool(C.col_has_prev_sub_uint64_t((*C.uint64_t)(unsafe.Pointer(uintptr(mf.Src)+uintptr(offset))), C.uint8_t(bitstart), C.uint8_t(bitend), C.uint64_t(first), &cpos, C.uint64_t(search)))
	return ret, uint64(cpos)
}

// --- RSIDVAR ---

// GetVRRsid returns the RSID at the specified position in the VR file.
func (mf TMMFile) GetVRRsid(item uint64) uint32 {
	return uint32(C.get_vr_rsid((*C.uchar)(mf.Src), C.uint64_t(item)))
}

// GetRVVariantKey returns the VariantKey at the specified position in the RV file.
func (mf TMMFile) GetRVVariantKey(item uint64) uint64 {
	return uint64(C.get_rv_variantkey((*C.uchar)(mf.Src), C.uint64_t(item)))
}

// FindRVVariantKeyByRsid search for the specified RSID and returns the first occurrence of VariantKey in the RV file.
func (mf TMMFile) FindRVVariantKeyByRsid(first, last uint64, rsid uint32) (uint64, uint64) {
	cfirst := C.uint64_t(first)
	vk := uint64(C.find_rv_variantkey_by_rsid((*C.uchar)(mf.Src), &cfirst, C.uint64_t(last), C.uint32_t(rsid)))
	return vk, uint64(cfirst)
}

// GetNextRVVariantKeyByRsid get the next VariantKey for the specified rsID in the RV file.
// Returns the VariantKey or 0, and the position
func (mf TMMFile) GetNextRVVariantKeyByRsid(pos, last uint64, rsid uint32) (uint64, uint64) {
	cpos := C.uint64_t(pos)
	vk := uint64(C.get_next_rv_variantkey_by_rsid((*C.uchar)(mf.Src), &cpos, C.uint64_t(last), C.uint32_t(rsid)))
	return vk, uint64(cpos)
}

// FindAllRVVariantKeyByRsid get all VariantKeys for the specified rsID in the RV file.
// Returns a list of VariantKeys
func (mf TMMFile) FindAllRVVariantKeyByRsid(first, last uint64, rsid uint32) (vks []uint64) {
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

// FindVRRsidByVariantKey search for the specified VariantKey and returns the first occurrence of RSID in the VR file.
func (mf TMMFile) FindVRRsidByVariantKey(first uint64, last uint64, vk uint64) (uint32, uint64) {
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

// FindRefAltByVariantKey retrieve the REF and ALT strings for the specified VariantKey.
func (mf TMMFile) FindRefAltByVariantKey(vk uint64) (string, string, uint8, uint8, uint32) {
	cref := C.malloc(256)
	defer C.free(unsafe.Pointer(cref)) // #nosec
	calt := C.malloc(256)
	defer C.free(unsafe.Pointer(calt)) // #nosec
	csizeref := C.size_t(0)
	csizealt := C.size_t(0)
	len := C.find_ref_alt_by_variantkey((*C.uchar)(mf.Src), C.uint64_t(mf.Last), C.uint64_t(vk), (*C.char)(cref), &csizeref, (*C.char)(calt), &csizealt)
	return C.GoStringN((*C.char)(cref), C.int(csizeref)), C.GoStringN((*C.char)(calt), C.int(csizealt)), uint8(csizeref), uint8(csizealt), uint32(len)
}

// ReverseVariantKey reverse a VariantKey code and returns the normalized components.
func (mf TMMFile) ReverseVariantKey(vk uint64) (TVariantKeyRev, uint32) {
	var rev C.variantkey_rev_t
	len := C.reverse_variantkey((*C.uchar)(mf.Src), C.uint64_t(mf.Last), C.uint64_t(vk), &rev)
	return castCVariantKeyRev(rev), uint32(len)
}

// GetVariantKeyRefLength retrieve the REF length for the specified VariantKey.
func (mf TMMFile) GetVariantKeyRefLength(vk uint64) uint8 {
	return uint8(C.get_variantkey_ref_length((*C.uchar)(mf.Src), C.uint64_t(mf.Last), C.uint64_t(vk)))
}

// GetVariantKeyEndPos get the VariantKey end position (POS + REF length).
func (mf TMMFile) GetVariantKeyEndPos(vk uint64) uint32 {
	return uint32(C.get_variantkey_endpos((*C.uchar)(mf.Src), C.uint64_t(mf.Last), C.uint64_t(vk)))
}

// GetVariantKeyChromStartPos get the CHROM + START POS encoding from VariantKey.
func GetVariantKeyChromStartPos(vk uint64) uint64 {
	return uint64(C.get_variantkey_chrom_startpos(C.uint64_t(vk)))
}

// GetVariantKeyChromEndPos get the CHROM + END POS encoding from VariantKey.
func (mf TMMFile) GetVariantKeyChromEndPos(vk uint64) uint64 {
	return uint64(C.get_variantkey_chrom_endpos((*C.uchar)(mf.Src), C.uint64_t(mf.Last), C.uint64_t(vk)))
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
func (mf TMMFile) AreOverlappingVariantKeyRegionKey(vk, rk uint64) bool {
	return (uint8(C.are_overlapping_variantkey_regionkey((*C.uchar)(mf.Src), C.uint64_t(mf.Last), C.uint64_t(vk), C.uint64_t(rk))) != 0)
}

// VariantToRegionkey get RegionKey from VariantKey.
func (mf TMMFile) VariantToRegionkey(vk uint64) uint64 {
	return uint64(C.variantkey_to_regionkey((*C.uchar)(mf.Src), C.uint64_t(mf.Last), C.uint64_t(vk)))
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
