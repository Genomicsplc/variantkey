package varianthash

import (
	"fmt"
	"strconv"
	"strings"
)

// TVariantHash contains a representation of a genetic variant hash
type TVariantHash struct {
	Chrom  uint32 `json:"chrom"`
	Pos    uint32 `json:"pos"`
	RefAlt uint64 `json:"refalt"`
}

// EncodeChrom returns 32-bit chromosome encoding.
func EncodeChrom(chrom string) uint32 {
	cl := len(chrom)
	if cl == 0 {
		return 0
	}
	n, err := strconv.ParseUint(chrom, 10, 32)
	if err == nil {
		return uint32(n)
	}
	// HUMAN
	if (chrom[0] == 'X') || (chrom[0] == 'x') {
		if cl > 1 && ((chrom[1] == 'Y') || (chrom[1] == 'y')) {
			return 25
		}
		return 23
	}
	if (chrom[0] == 'Y') || (chrom[0] == 'y') {
		return 24
	}
	if (chrom[0] == 'M') || (chrom[0] == 'm') {
		return 26
	}
	return 0
}

// EncodeRefAlt returns 64-bit reference+alternate hash code.
func EncodeRefAlt(ref string, alt string) uint64 {
	lenref := len(ref)
	ra := make([]byte, lenref+1+len(alt))
	i := 0
	copy(ra[i:], strings.ToUpper(ref))
	i += lenref
	ra[i] = '_'
	i++
	copy(ra[i:], strings.ToUpper(alt))
	return FarmHash64(ra)
}

// VariantHash returns a Genetic Variant Hash based on CHROM, POS (1-base), REF, ALT.
func VariantHash(chrom string, pos uint32, ref, alt string) TVariantHash {
	return TVariantHash{
		EncodeChrom(chrom),
		pos,
		EncodeRefAlt(ref, alt),
	}
}

// String representation of the TVariantHash
func (v TVariantHash) String() string {
	return fmt.Sprintf("%08x%08x%016x", v.Chrom, v.Pos, v.RefAlt)

}

// DecodeVariantHash parses a VariantHash string and returns the components.
func DecodeVariantHashString(vh string) TVariantHash {
	ret := TVariantHash{}
	chrom, err := strconv.ParseUint(vh[0:8], 16, 32)
	if err == nil {
		ret.Chrom = uint32(chrom)
	}
	pos, err := strconv.ParseUint(vh[8:16], 16, 32)
	if err == nil {
		ret.Pos = uint32(pos)
	}
	refalt, err := strconv.ParseUint(vh[16:32], 16, 64)
	if err == nil {
		ret.RefAlt = refalt
	}
	return ret
}
