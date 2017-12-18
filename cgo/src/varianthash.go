package varianthash

/*
#include "../../src/uint128.h"
#include "../../src/farmhash64.h"
#include "../../src/farmhash64.c"
#include "../../src/varianthash.h"
#include "../../src/varianthash.c"
#include "../../src/binsearch.h"
#include "../../src/binsearch.c"
#include "../../src/rsidvar.h"
#include "../../src/rsidvar.c"
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

// TVariantHash contains a representation of a genetic variant hash
type TVariantHash struct {
	Assembly uint32 `json:"assembly"`
	Chrom    uint32 `json:"chrom"`
	Pos      uint32 `json:"pos"`
	RefAlt   uint32 `json:"refalt"`
}

// castCVariantHash convert C varianthash_t to GO TVariantHash.
func castCVariantHash(vh C.varhash_t) TVariantHash {
	return TVariantHash{
		Assembly: uint32(vh.assembly),
		Chrom:    uint32(vh.chrom),
		Pos:      uint32(vh.pos),
		RefAlt:   uint32(vh.refalt),
	}
}

// castGoVariantHash convert GO TVariantHash to C varianthash_t.
func castGoVariantHash(vh TVariantHash) C.varhash_t {
	var cvh C.varhash_t
	cvh.assembly = C.uint32_t(vh.Assembly)
	cvh.chrom = C.uint32_t(vh.Chrom)
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

// EncodeAssembly returns 32-bit genome assembly encoding.
func EncodeAssembly(assembly string) uint32 {
	bassembly := StringToNTBytes(assembly)
	var passembly unsafe.Pointer
	if len(bassembly) > 0 {
		passembly = unsafe.Pointer(&bassembly[0]) // #nosec
	}
	return uint32(C.encode_assembly((*C.char)(passembly)))
}

// EncodeChrom returns 32-bit chromosome encoding.
func EncodeChrom(chrom string) uint32 {
	bchrom := StringToNTBytes(chrom)
	var pchrom unsafe.Pointer
	if len(bchrom) > 0 {
		pchrom = unsafe.Pointer(&bchrom[0]) // #nosec
	}
	return uint32(C.encode_chrom((*C.char)(pchrom)))
}

// EncodeRefAlt returns 64-bit reference+alternate hash code.
func EncodeRefAlt(ref string, alt string) uint32 {
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
	return uint32(C.encode_ref_alt((*C.char)(pref), (*C.char)(palt)))
}

// VariantHash returns a Genetic Variant Hash based on CHROM, POS (0-base), REF, ALT.
func VariantHash(assembly, chrom string, pos uint32, ref, alt string) TVariantHash {
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
	return castCVariantHash(C.variant_hash((*C.char)(passembly), (*C.char)(pchrom), C.uint32_t(pos), (*C.char)(pref), (*C.char)(palt)))
}

// String representation of the TVariantHash
func (v TVariantHash) String() string {
	var cvh C.varhash_t
	cvh.assembly = C.uint32_t(v.Assembly)
	cvh.chrom = C.uint32_t(v.Chrom)
	cvh.pos = C.uint32_t(v.Pos)
	cvh.refalt = C.uint32_t(v.RefAlt)
	var cstr *C.char = C.CString("00000000000000000000000000000000")
	defer C.free(unsafe.Pointer(cstr)) // #nosec
	C.variant_hash_string(cstr, C.size_t(33), cvh)
	return C.GoStringN(cstr, C.int(32))
}

// DecodeVariantHashString parses a variant hash string and returns the components as TVariantHash structure.
func DecodeVariantHashString(s string) TVariantHash {
	b := StringToNTBytes(s)
	var p unsafe.Pointer
	if len(s) > 0 {
		p = unsafe.Pointer(&b[0]) // #nosec
	}
	return castCVariantHash(C.decode_variant_hash_string((*C.char)(p)))
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

// --- RSIDVAR ---

// GetVRRsid returns the RSID at the specified position of the varhash_rsid.bin file.
func (mf TMMFile) GetVRRsid(item uint64) uint32 {
	return uint32(C.get_vr_rsid((*C.uchar)(mf.Src), C.uint64_t(item)))
}

// GetRVVarhash returns the VariantHash at the specified position of the rsid_varhash.bin file.
func (mf TMMFile) GetRVVarhash(item uint64) TVariantHash {
	return castCVariantHash(C.get_rv_varhash((*C.uchar)(mf.Src), C.uint64_t(item)))
}

// FindRVVarhashByRsid search for the specified RSID and returns the first occurrence of VariantHash, item position.
func (mf TMMFile) FindRVVarhashByRsid(first, last uint64, rsid uint32) (TVariantHash, uint64) {
	cfirst := C.uint64_t(first)
	vh := castCVariantHash(C.find_rv_varhash_by_rsid((*C.uchar)(mf.Src), &cfirst, C.uint64_t(last), C.uint32_t(rsid)))
	return vh, uint64(cfirst)
}

// FindVRRsidByVarshash search for the specified VariantHash and returns the first occurrence of RSID, item position.
func (mf TMMFile) FindVRRsidByVarshash(first uint64, last uint64, vh TVariantHash) (uint32, uint64) {
	cfirst := C.uint64_t(first)
	rsid := uint32(C.find_vr_rsid_by_varhash((*C.uchar)(mf.Src), &cfirst, C.uint64_t(last), castGoVariantHash(vh)))
	return rsid, uint64(cfirst)
}

// FindVRChromposRange search for the specified CHROM-POS range and returns the first occurrence of RSID, item position, last position.
func (mf TMMFile) FindVRChromposRange(first, last uint64, chrom, posStart, posEnd uint32) (uint32, uint64, uint64) {
	cfirst := C.uint64_t(first)
	clast := C.uint64_t(last)
	rsid := uint32(C.find_vr_chrompos_range((*C.uchar)(mf.Src), &cfirst, &clast, C.uint32_t(chrom), C.uint32_t(posStart), C.uint32_t(posEnd)))
	return rsid, uint64(cfirst), uint64(clast)
}
