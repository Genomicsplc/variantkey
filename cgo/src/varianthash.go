package varianthash

/*
#include "../../src/farmhash64.h"
#include "../../src/farmhash64.c"
#include "../../src/varianthash.h"
#include "../../src/varianthash.c"
*/
import "C"
import "unsafe"

// TVariantHash contains a representation of a genetic variant hash
type TVariantHash struct {
	Chrom  uint32 `json:"chrom"`
	Pos    uint32 `json:"pos"`
	RefAlt uint64 `json:"refalt"`
}

// StringToNTBytes safely convert a string to byte array with an extra null terminator
// This is to ensure a correct CGO conversion to char*
func StringToNTBytes(s string) []byte {
	b := make([]byte, len(s)+1)
	copy(b[:], s)
	return b
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
func EncodeRefAlt(ref string, alt string) uint64 {
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
	return uint64(C.encode_ref_alt((*C.char)(pref), (*C.char)(palt)))
}

// VariantHash returns a Genetic Variant Hash based on CHROM, POS (0-base), REF, ALT.
func VariantHash(chrom string, pos uint32, ref, alt string) TVariantHash {
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
	h := C.variant_hash((*C.char)(pchrom), C.uint32_t(pos), (*C.char)(pref), (*C.char)(palt))
	return TVariantHash{
		Chrom:  uint32(h.chrom),
		Pos:    uint32(h.pos),
		RefAlt: uint64(h.refalt),
	}
}

// String representation of the TVariantHash
func (v TVariantHash) String() string {
	var cvh C.varhash_t
	cvh.chrom = C.uint32_t(v.Chrom)
	cvh.pos = C.uint32_t(v.Pos)
	cvh.refalt = C.uint64_t(v.RefAlt)
	var cstr *C.char = C.CString("00000000000000000000000000000000")
	defer C.free(unsafe.Pointer(cstr)) // #nosec
	C.variant_hash_string(cstr, C.size_t(33), cvh)
	return C.GoStringN(cstr, C.int(32)) // #nosec
}

// DecodeVariantHashString parses a variant hash string and returns the components as TVariantHash structure.
func DecodeVariantHashString(s string) TVariantHash {
	b := StringToNTBytes(s)
	var p unsafe.Pointer
	if len(s) > 0 {
		p = unsafe.Pointer(&b[0]) // #nosec
	}
	h := C.decode_variant_hash_string((*C.char)(p))
	return TVariantHash{
		Chrom:  uint32(h.chrom),
		Pos:    uint32(h.pos),
		RefAlt: uint64(h.refalt),
	}
}

// FarmHash64 returns a 64-bit fingerprint hash for a string.
func FarmHash64(s []byte) uint64 {
	slen := len(s)
	var p unsafe.Pointer
	if slen > 0 {
		p = unsafe.Pointer(&s[0]) // #nosec
	}
	return uint64(C.farmhash64((*C.char)(p), C.size_t(slen)))
}
