package variantkey

// genoref_test.go
// @category   Libraries
// @author     Nicola Asuni <nicola.asuni@genomicsplc.com>
// @copyright  2017-2018 GENOMICS plc
// @license    MIT (see LICENSE)
// @link       https://github.com/genomicsplc/variantkey

import "testing"

func TestGenorefIndex(t *testing.T) {
	exp := []uint64{248, 248, 274, 299, 323, 346, 368, 389, 409, 428, 446, 463, 479, 494, 508, 521, 533, 544, 554, 563, 571, 578, 584, 589, 593, 596, 598}
	if len(gref.Index) != len(exp) {
		t.Errorf("Expected size %d, got %d", len(exp), len(gref.Index))
	}
	for k, v := range exp {
		if v != gref.Index[k] {
			t.Errorf("(%d) Expected value %d, got %d", k, v, gref.Index[k])
		}
	}
}

func TestGetGenorefSeq(t *testing.T) {
	var chrom uint8
	var ref, exp byte
	for chrom = 1; chrom <= 25; chrom++ {
		ref = gref.GetGenorefSeq(chrom, 0) // first
		if ref != 'A' {
			t.Errorf("%d (first): Expected reference 'A', got '%c'", chrom, ref)
		}
		ref = gref.GetGenorefSeq(chrom, (26 - uint32(chrom))) // last
		exp = byte('Z' + 1 - chrom)
		if ref != exp {
			t.Errorf("%d (last): Expected reference '%c', got '%c'", chrom, exp, ref)
		}
		ref = gref.GetGenorefSeq(chrom, (27 - uint32(chrom))) // invalid
		if ref != 0 {
			t.Errorf("%d (invalid): Expected reference 0, got '%c'", chrom, ref)
		}
	}
}

func BenchmarkGetGenorefSeq(b *testing.B) {
	b.ResetTimer()
	for i := 0; i < b.N; i++ {
		gref.GetGenorefSeq(13, 1)
	}
}

func TestCheckReference(t *testing.T) {
	type TCheckRefData struct {
		exp     int
		chrom   uint8
		pos     uint32
		sizeref uint8
		ref     string
	}
	var tdata = []TCheckRefData{
		{0, 1, 0, 1, "A"},
		{0, 1, 25, 1, "Z"},
		{0, 25, 0, 1, "A"},
		{0, 25, 1, 1, "B"},
		{0, 2, 0, 25, "ABCDEFGHIJKLmnopqrstuvwxy"},
		{-2, 1, 26, 4, "ZABC"},
		{-1, 1, 0, 26, "ABCDEFGHIJKLmnopqrstuvwxyJ"},
		{-1, 14, 2, 3, "ZZZ"},
		{1, 1, 0, 1, "N"},
		{1, 10, 13, 1, "A"},
		{1, 1, 3, 1, "B"},
		{1, 1, 1, 1, "C"},
		{1, 1, 0, 1, "D"},
		{1, 1, 3, 1, "A"},
		{1, 1, 0, 1, "H"},
		{1, 1, 7, 1, "A"},
		{1, 1, 0, 1, "V"},
		{1, 1, 21, 1, "A"},
		{1, 1, 0, 1, "W"},
		{1, 1, 19, 1, "W"},
		{1, 1, 22, 1, "A"},
		{1, 1, 22, 1, "T"},
		{1, 1, 2, 1, "S"},
		{1, 1, 6, 1, "S"},
		{1, 1, 18, 1, "C"},
		{1, 1, 18, 1, "G"},
		{1, 1, 0, 1, "M"},
		{1, 1, 2, 1, "M"},
		{1, 1, 12, 1, "A"},
		{1, 1, 12, 1, "C"},
		{1, 1, 6, 1, "K"},
		{1, 1, 19, 1, "K"},
		{1, 1, 10, 1, "G"},
		{1, 1, 10, 1, "T"},
		{1, 1, 0, 1, "R"},
		{1, 1, 6, 1, "R"},
		{1, 1, 17, 1, "A"},
		{1, 1, 17, 1, "G"},
		{1, 1, 2, 1, "Y"},
		{1, 1, 19, 1, "Y"},
		{1, 1, 24, 1, "C"},
		{1, 1, 24, 1, "T"},
	}
	for _, v := range tdata {
		v := v
		t.Run("", func(t *testing.T) {
			t.Parallel()
			h := gref.CheckReference(v.chrom, v.pos, v.ref)
			if h != v.exp {
				t.Errorf("The return code is different, got: %#v expected %#v", h, v.exp)
			}
		})
	}
}

func TestFlipAlllele(t *testing.T) {
	allele := "ATCGMKRYBVDHWSNatcgmkrybvdhwsn"
	expected := "TAGCKMYRVBHDWSNTAGCKMYRVBHDWSN"
	res := FlipAllele(allele)
	if res != expected {
		t.Errorf("The returned value is incorrect, got: %#v expected %#v", res, expected)
	}
}

func TestNormalizeVariant(t *testing.T) {
	type TNormData struct {
		ecode    int
		chrom    uint8
		pos      uint32
		epos     uint32
		sizeref  uint8
		sizealt  uint8
		esizeref uint8
		esizealt uint8
		eref     string
		ealt     string
		ref      string
		alt      string
	}
	var ndata = []TNormData{
		{-2, 1, 26, 26, 1, 1, 1, 1, "A", "C", "A", "C"},         // invalid position
		{-1, 1, 0, 0, 1, 1, 1, 1, "J", "C", "J", "C"},           // invalid reference
		{4, 1, 0, 0, 1, 1, 1, 1, "A", "C", "T", "G"},            // flip
		{0, 1, 0, 0, 1, 1, 1, 1, "A", "C", "A", "C"},            // OK
		{32, 13, 2, 3, 3, 2, 2, 1, "DE", "D", "CDE", "CD"},      // left trim
		{48, 13, 2, 3, 3, 3, 1, 1, "D", "F", "CDE", "CFE"},      // left trim + right trim
		{48, 1, 0, 2, 6, 6, 1, 1, "C", "K", "aBCDEF", "aBKDEF"}, // left trim + right trim
		{0, 1, 0, 0, 1, 0, 1, 0, "A", "", "A", ""},              // OK
		{8, 1, 3, 2, 1, 0, 2, 1, "CD", "C", "D", ""},            // left extend
		{0, 1, 24, 24, 1, 2, 1, 2, "Y", "CK", "Y", "CK"},        // OK
		{2, 1, 0, 0, 1, 1, 1, 1, "A", "G", "G", "A"},            // swap
		{6, 1, 0, 0, 1, 1, 1, 1, "A", "C", "G", "T"},            // swap + flip
	}
	for _, v := range ndata {
		v := v
		t.Run("", func(t *testing.T) {
			t.Parallel()
			code, npos, nref, nalt, nsizeref, nsizealt := gref.NormalizeVariant(v.chrom, v.pos, v.ref, v.alt)
			if code != v.ecode {
				t.Errorf("The return code is different, got: %#v expected %#v", code, v.ecode)
			}
			if npos != v.epos {
				t.Errorf("The POS value is different, got: %#v expected %#v", npos, v.epos)
			}
			if nsizeref != v.esizeref {
				t.Errorf("The REF size is different, got: %#v expected %#v", nsizeref, v.esizeref)
			}
			if nsizealt != v.esizealt {
				t.Errorf("The ALT size is different, got: %#v expected %#v", nsizealt, v.esizealt)
			}
			if nref != v.eref {
				t.Errorf("The REF is different, got: %#v expected %#v", nref, v.eref)
			}
			if nalt != v.ealt {
				t.Errorf("The ALT is different, got: %#v expected %#v", nalt, v.ealt)
			}
		})
	}
}
