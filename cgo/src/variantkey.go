package variantkey

/*
#include "../../src/uint128.h"
#include "../../src/farmhash64.h"
#include "../../src/farmhash64.c"
#include "../../src/variantkey.h"
#include "../../src/variantkey.c"
#include "../../src/binsearch.h"
#include "../../src/binsearch.c"
#include "../../src/rsidvar128.h"
#include "../../src/rsidvar128.c"
#include "../../src/rsidvar64.h"
#include "../../src/rsidvar64.c"
*/
import "C"
import "unsafe"
import "fmt"

// Uint128 contains 2 64bit unsigned integers
type Uint128 struct {
	Lo uint64
	Hi uint64
}

// castGoUint128 convert GO Uint128 to C uint128_t.
func castGoUint128(u Uint128) C.uint128_t {
	var c C.uint128_t
	c.lo = C.uint64_t(u.Lo)
	c.hi = C.uint64_t(u.Hi)
	return c
}

// TVariantKey128 contains a representation of a genetic variant key
type TVariantKey128 struct {
	Assembly uint32 `json:"assembly"`
	Chrom    uint32 `json:"chrom"`
	Pos      uint32 `json:"pos"`
	RefAlt   uint32 `json:"refalt"`
}

// TVariantKey64 contains a representation of a genetic variant key
type TVariantKey64 struct {
	Chrom  uint8  `json:"chrom"`
	Pos    uint32 `json:"pos"`
	RefAlt uint32 `json:"refalt"`
}

// castCVariantKey128 convert C variantkey128_t to GO TVariantKey128.
func castCVariantKey128(vh C.variantkey128_t) TVariantKey128 {
	return TVariantKey128{
		Assembly: uint32(vh.assembly),
		Chrom:    uint32(vh.chrom),
		Pos:      uint32(vh.pos),
		RefAlt:   uint32(vh.refalt),
	}
}

// castCVariantKey64 convert C variantkey64_t to GO TVariantKey64.
func castCVariantKey64(vh C.variantkey64_t) TVariantKey64 {
	return TVariantKey64{
		Chrom:  uint8(vh.chrom),
		Pos:    uint32(vh.pos),
		RefAlt: uint32(vh.refalt),
	}
}

// castGoVariantKey128 convert GO TVariantKey128 to C variantkey128_t.
func castGoVariantKey128(vh TVariantKey128) C.variantkey128_t {
	var cvh C.variantkey128_t
	cvh.assembly = C.uint32_t(vh.Assembly)
	cvh.chrom = C.uint32_t(vh.Chrom)
	cvh.pos = C.uint32_t(vh.Pos)
	cvh.refalt = C.uint32_t(vh.RefAlt)
	return cvh
}

// castGoVariantKey64 convert GO TVariantKey64 to C variantkey64_t.
func castGoVariantKey64(vh TVariantKey64) C.variantkey64_t {
	var cvh C.variantkey64_t
	cvh.chrom = C.uint8_t(vh.Chrom)
	cvh.pos = C.uint32_t(vh.Pos)
	cvh.refalt = C.uint32_t(vh.RefAlt)
	return cvh
}

// StringToNTBytes safely convert a string to byte array with an extra null terminator
// This is to ensure a correct CGO conversion to char*
func StringToNTBytes(s string) []byte {
	b := make([]byte, len(s)+1)
	copy(b[:], s)
	return b
}

// EncodeAssembly32bit returns 32-bit genome assembly encoding.
func EncodeAssembly32bit(assembly string) uint32 {
	bassembly := StringToNTBytes(assembly)
	var passembly unsafe.Pointer
	if len(bassembly) > 0 {
		passembly = unsafe.Pointer(&bassembly[0]) // #nosec
	}
	return uint32(C.encode_assembly_32bit((*C.char)(passembly)))
}

// EncodeChrom32bit returns 32-bit chromosome encoding.
func EncodeChrom32bit(chrom string) uint32 {
	bchrom := StringToNTBytes(chrom)
	var pchrom unsafe.Pointer
	if len(bchrom) > 0 {
		pchrom = unsafe.Pointer(&bchrom[0]) // #nosec
	}
	return uint32(C.encode_chrom_32bit((*C.char)(pchrom)))
}

// EncodeChrom8bit returns 8-bit chromosome encoding.
func EncodeChrom8bit(chrom string) uint8 {
	bchrom := StringToNTBytes(chrom)
	var pchrom unsafe.Pointer
	if len(bchrom) > 0 {
		pchrom = unsafe.Pointer(&bchrom[0]) // #nosec
	}
	return uint8(C.encode_chrom_8bit((*C.char)(pchrom)))
}

// DecodeChrom32bit decode 32 bit chrom to string
func DecodeChrom32bit(c uint32) string {
	var cstr *C.char = C.CString("0000000000")
	defer C.free(unsafe.Pointer(cstr)) // #nosec
	len := C.decode_chrom_32bit(C.uint32_t(c), cstr)
	return C.GoStringN(cstr, C.int(len))
}

// DecodeChrom8bit decode 8 bit chrom to string
func DecodeChrom8bit(c uint8) string {
	var cstr *C.char = C.CString("000")
	defer C.free(unsafe.Pointer(cstr)) // #nosec
	len := C.decode_chrom_8bit(C.uint8_t(c), cstr)
	return C.GoStringN(cstr, C.int(len))
}

// EncodeRefAlt32bit returns 32-bit reference+alternate code.
func EncodeRefAlt32bit(ref string, alt string) uint32 {
	bref := StringToNTBytes(ref)
	balt := StringToNTBytes(alt)
	var pref unsafe.Pointer
	var palt unsafe.Pointer
	if len(ref) > 0 {
		pref = unsafe.Pointer(&bref[0]) // #nosec
	}
	if len(alt) > 0 {
		palt = unsafe.Pointer(&balt[0]) // #nosec
	}
	return uint32(C.encode_refalt_32bit((*C.char)(pref), (*C.char)(palt)))
}

// EncodeRefAlt24bit returns 24-bit reference+alternate code.
func EncodeRefAlt24bit(ref string, alt string) uint32 {
	bref := StringToNTBytes(ref)
	balt := StringToNTBytes(alt)
	var pref unsafe.Pointer
	var palt unsafe.Pointer
	if len(ref) > 0 {
		pref = unsafe.Pointer(&bref[0]) // #nosec
	}
	if len(alt) > 0 {
		palt = unsafe.Pointer(&balt[0]) // #nosec
	}
	return uint32(C.encode_refalt_24bit((*C.char)(pref), (*C.char)(palt)))
}

// DecodeRefAlt32bit decode 32 bit Ref+Alt code to string
func DecodeRefAlt32bit(c uint32) (string, string) {
	var cref *C.char = C.CString("00000")
	defer C.free(unsafe.Pointer(cref)) // #nosec
	var calt *C.char = C.CString("00000")
	defer C.free(unsafe.Pointer(calt)) // #nosec
	len := C.decode_refalt_32bit(C.uint32_t(c), cref, calt)
	return C.GoStringN(cref, C.int(5-len)), C.GoStringN(calt, C.int(len))
}

// DecodeRefAlt24bit decode 24 bit Ref+Alt code to string
func DecodeRefAlt24bit(c uint32) (string, string) {
	var cref *C.char = C.CString("0000")
	defer C.free(unsafe.Pointer(cref)) // #nosec
	var calt *C.char = C.CString("0000")
	defer C.free(unsafe.Pointer(calt)) // #nosec
	len := C.decode_refalt_24bit(C.uint32_t(c), cref, calt)
	return C.GoStringN(cref, C.int(4-len)), C.GoStringN(calt, C.int(len))
}

// VariantKey128 returns a Genetic Variant Key based on ASSEMBLY, CHROM, POS (0-base), REF, ALT.
func VariantKey128(assembly, chrom string, pos uint32, ref, alt string) TVariantKey128 {
	bassembly := StringToNTBytes(assembly)
	bchrom := StringToNTBytes(chrom)
	bref := StringToNTBytes(ref)
	balt := StringToNTBytes(alt)
	var passembly unsafe.Pointer
	var pchrom unsafe.Pointer
	var pref unsafe.Pointer
	var palt unsafe.Pointer
	if len(assembly) > 0 {
		passembly = unsafe.Pointer(&bassembly[0]) // #nosec
	}
	if len(chrom) > 0 {
		pchrom = unsafe.Pointer(&bchrom[0]) // #nosec
	}
	if len(ref) > 0 {
		pref = unsafe.Pointer(&bref[0]) // #nosec
	}
	if len(alt) > 0 {
		palt = unsafe.Pointer(&balt[0]) // #nosec
	}
	return castCVariantKey128(C.variantkey128((*C.char)(passembly), (*C.char)(pchrom), C.uint32_t(pos), (*C.char)(pref), (*C.char)(palt)))
}

// VariantKey64 returns a Genetic Variant Key based on CHROM, POS (0-base), REF, ALT.
func VariantKey64(chrom string, pos uint32, ref, alt string) uint64 {
	bchrom := StringToNTBytes(chrom)
	bref := StringToNTBytes(ref)
	balt := StringToNTBytes(alt)
	var pchrom unsafe.Pointer
	var pref unsafe.Pointer
	var palt unsafe.Pointer
	if len(chrom) > 0 {
		pchrom = unsafe.Pointer(&bchrom[0]) // #nosec
	}
	if len(ref) > 0 {
		pref = unsafe.Pointer(&bref[0]) // #nosec
	}
	if len(alt) > 0 {
		palt = unsafe.Pointer(&balt[0]) // #nosec
	}
	return uint64(C.variantkey64((*C.char)(pchrom), C.uint32_t(pos), (*C.char)(pref), (*C.char)(palt)))
}

// String representation of the TVariantKey128
func (v TVariantKey128) String() string {
	var cvh C.variantkey128_t
	cvh.assembly = C.uint32_t(v.Assembly)
	cvh.chrom = C.uint32_t(v.Chrom)
	cvh.pos = C.uint32_t(v.Pos)
	cvh.refalt = C.uint32_t(v.RefAlt)
	var cstr *C.char = C.CString("00000000000000000000000000000000")
	defer C.free(unsafe.Pointer(cstr)) // #nosec
	C.variantkey128_string(cstr, C.size_t(33), cvh)
	return C.GoStringN(cstr, C.int(32))
}

// VariantKey64String provides a string representation of the VariantKey 64bit
func VariantKey64String(v uint64) string {
	var cstr *C.char = C.CString("0000000000000000")
	defer C.free(unsafe.Pointer(cstr)) // #nosec
	C.variantkey64_string(cstr, C.size_t(17), C.uint64_t(v))
	return C.GoStringN(cstr, C.int(16))
}

// ParseVariantKey128String parses a variant key string and returns the components as TVariantKey128 structure.
func ParseVariantKey128String(s string) TVariantKey128 {
	b := StringToNTBytes(s)
	var p unsafe.Pointer
	if len(s) > 0 {
		p = unsafe.Pointer(&b[0]) // #nosec
	}
	return castCVariantKey128(C.parse_variantkey128_string((*C.char)(p)))
}

// ParseVariantKey64String parses a variant key string and returns the code.
func ParseVariantKey64String(s string) uint64 {
	b := StringToNTBytes(s)
	var p unsafe.Pointer
	if len(s) > 0 {
		p = unsafe.Pointer(&b[0]) // #nosec
	}
	return uint64(C.parse_variantkey64_string((*C.char)(p)))
}

// SplitVariantKey128String parses a variant key string and returns the components as TVariantKey128 structure.
func SplitVariantKey128String(v uint64) TVariantKey64 {
	return castCVariantKey64(C.split_variantkey64(C.uint64_t(v)))
}

// --- FARMHASH64 ---

// FarmHash64 returns a 64-bit fingerprint hash for a string.
func FarmHash64(s []byte) uint64 {
	slen := len(s)
	var p unsafe.Pointer
	if slen > 0 {
		p = unsafe.Pointer(&s[0]) /* #nosec */
	}
	return uint64(C.farmhash64((*C.char)(p), C.size_t(slen)))
}

// FarmHash32 returns a 32-bit fingerprint hash for a string.
func FarmHash32(s []byte) uint32 {
	slen := len(s)
	var p unsafe.Pointer
	if slen > 0 {
		p = unsafe.Pointer(&s[0]) /* #nosec */
	}
	return uint32(C.farmhash32((*C.char)(p), C.size_t(slen)))
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
		p = unsafe.Pointer(&bfile[0]) /* #nosec */
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

// FindFirstUint128 search for the first occurrence of a 128 bit unsigned integer on a memory mapped
// binary file containing adjacent blocks of sorted binary data.
// The 128 bit values in the file must encoded in big-endian format and sorted in ascending order.
// Return the item number if found or (last + 1) if not found, plus the first and last positions.
func (mf TMMFile) FindFirstUint128(blklen, blkpos, first, last uint64, search Uint128) (uint64, uint64, uint64) {
	cfirst := C.uint64_t(first)
	clast := C.uint64_t(last)
	ret := uint64(C.find_first_uint128_t((*C.uchar)(mf.Src), C.uint64_t(blklen), C.uint64_t(blkpos), &cfirst, &clast, castGoUint128(search)))
	return ret, uint64(cfirst), uint64(clast)
}

// FindLastUint128 search for the last occurrence of a 128 bit unsigned integer on a memory mapped
// binary file containing adjacent blocks of sorted binary data.
// The 128 bit values in the file must encoded in big-endian format and sorted in ascending order.
// Return the item number if found or (last + 1) if not found, plus the first and last positions.
func (mf TMMFile) FindLastUint128(blklen, blkpos, first, last uint64, search Uint128) (uint64, uint64, uint64) {
	cfirst := C.uint64_t(first)
	clast := C.uint64_t(last)
	ret := uint64(C.find_last_uint128_t((*C.uchar)(mf.Src), C.uint64_t(blklen), C.uint64_t(blkpos), &cfirst, &clast, castGoUint128(search)))
	return ret, uint64(cfirst), uint64(clast)
}

// --- RSIDVAR128 ---

// GetVR128Rsid returns the RSID at the specified position of the variantkey_rsid.bin file.
func (mf TMMFile) GetVR128Rsid(item uint64) uint32 {
	return uint32(C.get_vr128_rsid((*C.uchar)(mf.Src), C.uint64_t(item)))
}

// GetRV128Variantkey returns the VariantKey at the specified position of the rsid_variantkey.bin file.
func (mf TMMFile) GetRV128Variantkey(item uint64) TVariantKey128 {
	return castCVariantKey128(C.get_rv128_variantkey((*C.uchar)(mf.Src), C.uint64_t(item)))
}

// FindRV128VariantkeyByRsid search for the specified RSID and returns the first occurrence of VariantKey, item position.
func (mf TMMFile) FindRV128VariantkeyByRsid(first, last uint64, rsid uint32) (TVariantKey128, uint64) {
	cfirst := C.uint64_t(first)
	vh := castCVariantKey128(C.find_rv128_variantkey_by_rsid((*C.uchar)(mf.Src), &cfirst, C.uint64_t(last), C.uint32_t(rsid)))
	return vh, uint64(cfirst)
}

// FindVR128RsidByVariantkey search for the specified VariantKey and returns the first occurrence of RSID, item position.
func (mf TMMFile) FindVR128RsidByVariantkey(first uint64, last uint64, vh TVariantKey128) (uint32, uint64) {
	cfirst := C.uint64_t(first)
	rsid := uint32(C.find_vr128_rsid_by_variantkey((*C.uchar)(mf.Src), &cfirst, C.uint64_t(last), castGoVariantKey128(vh)))
	return rsid, uint64(cfirst)
}

// FindVR128ChromRange search for the specified CHROM range and returns the first occurrence of RSID, item position, last position.
func (mf TMMFile) FindVR128ChromRange(first, last uint64, chrom uint32) (uint32, uint64, uint64) {
	cfirst := C.uint64_t(first)
	clast := C.uint64_t(last)
	rsid := uint32(C.find_vr128_chrom_range((*C.uchar)(mf.Src), &cfirst, &clast, C.uint32_t(chrom)))
	return rsid, uint64(cfirst), uint64(clast)
}

// FindVR128PosRange search for the specified POS range and returns the first occurrence of RSID, item position, last position.
func (mf TMMFile) FindVR128PosRange(first, last uint64, posStart, posEnd uint32) (uint32, uint64, uint64) {
	cfirst := C.uint64_t(first)
	clast := C.uint64_t(last)
	rsid := uint32(C.find_vr128_pos_range((*C.uchar)(mf.Src), &cfirst, &clast, C.uint32_t(posStart), C.uint32_t(posEnd)))
	return rsid, uint64(cfirst), uint64(clast)
}

// FindVR128ChromposRange search for the specified CHROM-POS range and returns the first occurrence of RSID, item position, last position.
func (mf TMMFile) FindVR128ChromposRange(first, last uint64, chrom, posStart, posEnd uint32) (uint32, uint64, uint64) {
	cfirst := C.uint64_t(first)
	clast := C.uint64_t(last)
	rsid := uint32(C.find_vr128_chrompos_range((*C.uchar)(mf.Src), &cfirst, &clast, C.uint32_t(chrom), C.uint32_t(posStart), C.uint32_t(posEnd)))
	return rsid, uint64(cfirst), uint64(clast)
}

// --- RSIDVAR64 ---

// GetVR64Rsid returns the RSID at the specified position of the variantkey_rsid.bin file.
func (mf TMMFile) GetVR64Rsid(item uint64) uint32 {
	return uint32(C.get_vr64_rsid((*C.uchar)(mf.Src), C.uint64_t(item)))
}

// GetRV64Variantkey returns the VariantKey at the specified position of the rsid_variantkey.bin file.
func (mf TMMFile) GetRV64Variantkey(item uint64) uint64 {
	return uint64(C.get_rv64_variantkey((*C.uchar)(mf.Src), C.uint64_t(item)))
}

// FindRV64VariantkeyByRsid search for the specified RSID and returns the first occurrence of VariantKey, item position.
func (mf TMMFile) FindRV64VariantkeyByRsid(first, last uint64, rsid uint32) (uint64, uint64) {
	cfirst := C.uint64_t(first)
	vh := uint64(C.find_rv64_variantkey_by_rsid((*C.uchar)(mf.Src), &cfirst, C.uint64_t(last), C.uint32_t(rsid)))
	return vh, uint64(cfirst)
}

// FindVR64RsidByVariantkey search for the specified VariantKey and returns the first occurrence of RSID, item position.
func (mf TMMFile) FindVR64RsidByVariantkey(first uint64, last uint64, vh uint64) (uint32, uint64) {
	cfirst := C.uint64_t(first)
	rsid := uint32(C.find_vr64_rsid_by_variantkey((*C.uchar)(mf.Src), &cfirst, C.uint64_t(last), C.uint64_t(vh)))
	return rsid, uint64(cfirst)
}

// FindVR64ChromRange search for the specified CHROM range and returns the first occurrence of RSID, item position, last position.
func (mf TMMFile) FindVR64ChromRange(first, last uint64, chrom uint8) (uint32, uint64, uint64) {
	cfirst := C.uint64_t(first)
	clast := C.uint64_t(last)
	rsid := uint32(C.find_vr64_chrom_range((*C.uchar)(mf.Src), &cfirst, &clast, C.uint8_t(chrom)))
	return rsid, uint64(cfirst), uint64(clast)
}

// FindVR64PosRange search for the specified POS range and returns the first occurrence of RSID, item position, last position.
func (mf TMMFile) FindVR64PosRange(first, last uint64, posStart, posEnd uint32) (uint32, uint64, uint64) {
	cfirst := C.uint64_t(first)
	clast := C.uint64_t(last)
	rsid := uint32(C.find_vr64_pos_range((*C.uchar)(mf.Src), &cfirst, &clast, C.uint32_t(posStart), C.uint32_t(posEnd)))
	return rsid, uint64(cfirst), uint64(clast)
}

// FindVR64ChromposRange search for the specified CHROM-POS range and returns the first occurrence of RSID, item position, last position.
func (mf TMMFile) FindVR64ChromposRange(first, last uint64, chrom uint8, posStart, posEnd uint32) (uint32, uint64, uint64) {
	cfirst := C.uint64_t(first)
	clast := C.uint64_t(last)
	rsid := uint32(C.find_vr64_chrompos_range((*C.uchar)(mf.Src), &cfirst, &clast, C.uint8_t(chrom), C.uint32_t(posStart), C.uint32_t(posEnd)))
	return rsid, uint64(cfirst), uint64(clast)
}
