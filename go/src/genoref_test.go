package variantkey

import "testing"

func TestGenorefIndex(t *testing.T) {
	exp := []uint32{0, 105, 132, 158, 183, 207, 230, 252, 273, 293, 312, 330, 347, 363, 378, 392, 405, 417, 428, 438, 447, 455, 462, 468, 473, 477, 480}
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
			t.Errorf("(first): Expected reference 'A', got '%c'", ref)
		}
		ref = gref.GetGenorefSeq(chrom, (26 - uint32(chrom))) // last
		exp = byte('Z' + 1 - chrom)
		if ref != exp {
			t.Errorf("(last): Expected reference '%c', got '%c'", exp, ref)
		}
		ref = gref.GetGenorefSeq(chrom, (27 - uint32(chrom))) // invalid
		if ref != 0 {
			t.Errorf("(invalid): Expected reference 0, got '%c'", ref)
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
		chrom   uint8
		pos     uint32
		ref     string
		sizeref uint8
		exp     int
	}
	var tdata = []TCheckRefData{
		{1, 0, "A", 1, 0},
		{1, 25, "Z", 1, 0},
		{25, 0, "A", 1, 0},
		{25, 1, "B", 1, 0},
		{2, 0, "ABCDEFGHIJKLmnopqrstuvwxy", 25, 0},
		{1, 26, "ZABC", 4, -2},
		{1, 0, "ABCDEFGHIJKLmnopqrstuvwxyJ", 26, -1},
		{14, 2, "ZZZ", 3, -1},
		{1, 0, "N", 1, 1},
		{10, 13, "A", 1, 1},
		{1, 3, "B", 1, 1},
		{1, 1, "C", 1, 1},
		{1, 0, "D", 1, 1},
		{1, 3, "A", 1, 1},
		{1, 0, "H", 1, 1},
		{1, 7, "A", 1, 1},
		{1, 0, "V", 1, 1},
		{1, 21, "A", 1, 1},
		{1, 0, "W", 1, 1},
		{1, 19, "W", 1, 1},
		{1, 22, "A", 1, 1},
		{1, 22, "T", 1, 1},
		{1, 2, "S", 1, 1},
		{1, 6, "S", 1, 1},
		{1, 18, "C", 1, 1},
		{1, 18, "G", 1, 1},
		{1, 0, "M", 1, 1},
		{1, 2, "M", 1, 1},
		{1, 12, "A", 1, 1},
		{1, 12, "C", 1, 1},
		{1, 6, "K", 1, 1},
		{1, 19, "K", 1, 1},
		{1, 10, "G", 1, 1},
		{1, 10, "T", 1, 1},
		{1, 0, "R", 1, 1},
		{1, 6, "R", 1, 1},
		{1, 17, "A", 1, 1},
		{1, 17, "G", 1, 1},
		{1, 2, "Y", 1, 1},
		{1, 19, "Y", 1, 1},
		{1, 24, "C", 1, 1},
		{1, 24, "T", 1, 1},
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
		chrom    uint8
		pos      uint32
		ref      string
		sizeref  uint8
		alt      string
		sizealt  uint8
		epos     uint32
		eref     string
		esizeref uint8
		ealt     string
		esizealt uint8
		ecode    int
	}
	var ndata = []TNormData{
		{1, 26, "A", 1, "C", 1, 26, "A", 1, "C", 1, -2},         // invalid position
		{1, 0, "J", 1, "C", 1, 0, "J", 1, "C", 1, -1},           // invalid reference
		{1, 0, "T", 1, "G", 1, 0, "A", 1, "C", 1, 2},            // flip
		{1, 0, "A", 1, "C", 1, 0, "A", 1, "C", 1, 0},            // OK
		{13, 2, "CDE", 3, "CD", 2, 3, "DE", 2, "D", 1, 32},      // left trim
		{13, 2, "CDE", 3, "CFE", 3, 3, "D", 1, "F", 1, 48},      // left trim + right trim
		{1, 0, "aBCDEF", 6, "aBKDEF", 6, 2, "C", 1, "K", 1, 48}, // left trim + right trim
		{1, 0, "A", 1, "", 0, 0, "A", 1, "", 0, 0},              // OK
		{1, 3, "D", 1, "", 0, 2, "CD", 2, "C", 1, 8},            // left extend
		{1, 24, "Y", 1, "CK", 2, 24, "Y", 1, "CK", 2, 0},        // OK
		{1, 0, "G", 1, "A", 1, 0, "A", 1, "G", 1, 4},            // swap
		{1, 0, "G", 1, "T", 1, 0, "A", 1, "C", 1, 6},            // swap + flip
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
