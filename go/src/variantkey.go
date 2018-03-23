package variantkey

import (
	"fmt"
	"strconv"
	//"strings"
)

// TVariantKey contains a representation of a genetic variant key
type TVariantKey struct {
	Chrom  uint8  `json:"chrom"`
	Pos    uint32 `json:"pos"`
	RefAlt uint32 `json:"refalt"`
}

// EncodeChrom returns chromosome encoding.
func EncodeChrom(chrom string) uint8 {
	size := len(chrom)
	// remove "chr" prefix
	if (size > 3) &&
		((chrom[0] == 'C') || (chrom[0] == 'c')) &&
		((chrom[1] == 'H') || (chrom[1] == 'h')) &&
		((chrom[2] == 'R') || (chrom[2] == 'r')) {
		chrom = chrom[3:]
		size -= 3
	}
	if (chrom[0] == 'X') || (chrom[0] == 'x') {
		return 23
	}
	if (chrom[0] == 'Y') || (chrom[0] == 'y') {
		return 24
	}
	if (chrom[0] == 'M') || (chrom[0] == 'm') { // MT
		return 25
	}
	if (chrom[0] < '0') || (chrom[0] > '9') { // NA
		return 0
	}
	// convert digits
	var v uint8
	for i := 0; i < size; i++ {
		v = ((v * 10) + (chrom[i] - '0'))
	}
	return v
}

// DecodeChrom decode chrom to string
func DecodeChrom(c uint8) string {
	if (c < 1) || (c > 25) {
		return "NA"
	}
	if c < 23 {
		return strconv.FormatUint(uint64(c), 10)
	}
	chrmap := []string{"X", "Y", "MT"}
	return chrmap[c-23]
}

// EncodeRefAlt returns reference+alternate code.
func EncodeRefAlt(ref string, alt string) uint32 {
	return 0
}

// DecodeRefAlt decode Ref+Alt code to string
func DecodeRefAlt(c uint32) (string, string, uint8, uint8, uint8) {
	return "", "", 0, 0, 0
}

// VariantKey returns a Genetic Variant Key based on CHROM, POS (0-base), REF, ALT.
func VariantKey(chrom string, pos uint32, ref, alt string) uint64 {
	return ((uint64(EncodeChrom(chrom)) << 59) | (uint64(pos) << 31) | uint64(EncodeRefAlt(ref, alt)))
}

// VariantKeyString provides a string representation of the VariantKey 64bit
func VariantKeyString(v uint64) string {
	return fmt.Sprintf("%016x", v)
}

// ParseVariantKeyString parses a variant key string and returns the code.
func ParseVariantKeyString(s string) uint64 {
	v, err := strconv.ParseUint(s, 16, 64)
	if err != nil {
		return 0
	}
	return v
}

// DecodeVariantKeyg parses a variant key string and returns the components as TVariantKey128 structure.
func DecodeVariantKey(v uint64) TVariantKey {
	ret := TVariantKey{}
	// CHROM:   11111000 00000000 00000000 00000000 00000000 00000000 00000000 00000000
	ret.Chrom = uint8((v & 0xF800000000000000) >> 59)
	// POS:     00000111 11111111 11111111 11111111 10000000 00000000 00000000 00000000
	ret.Pos = uint32((v & 0x07FFFFFF80000000) >> 31)
	// REF+ALT: 00000000 00000000 00000000 00000000 01111111 11111111 11111111 11111111
	ret.RefAlt = uint32(v & 0x000000007FFFFFFF)
	return ret
}
