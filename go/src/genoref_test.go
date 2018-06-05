package variantkey

import "testing"

func TestGenorefIndex(t *testing.T) {
	exp := []uint32{0, 105, 132, 158, 183, 207, 230, 252, 273, 293, 312, 330, 347, 363, 378, 392, 405, 417, 428, 438, 447, 455, 462, 468, 473, 477, 480}
	if len(grefIdx) != len(exp) {
		t.Errorf("Expected size %d, got %d", len(exp), len(grefIdx))
	}
	for k, v := range exp {
		if v != grefIdx[k] {
			t.Errorf("(%d) Expected value %d, got %d", k, v, grefIdx[k])
		}
	}
}

func TestGetGenorefSeq(t *testing.T) {
	var chrom uint8
	var ref, exp byte
	for chrom = 1; chrom <= 25; chrom++ {
		ref = gref.GetGenorefSeq(grefIdx, chrom, 0) // first
		if ref != 'A' {
			t.Errorf("(first): Expected reference 'A', got '%c'", ref)
		}
		ref = gref.GetGenorefSeq(grefIdx, chrom, (26 - uint32(chrom))) // last
		exp = byte('Z' + 1 - chrom)
		if ref != exp {
			t.Errorf("(last): Expected reference '%c', got '%c'", exp, ref)
		}
		ref = gref.GetGenorefSeq(grefIdx, chrom, (27 - uint32(chrom))) // invalid
		if ref != 0 {
			t.Errorf("(invalid): Expected reference 0, got '%c'", ref)
		}
	}
}

func BenchmarkGetGenorefSeq(b *testing.B) {
	b.ResetTimer()
	for i := 0; i < b.N; i++ {
		gref.GetGenorefSeq(grefIdx, 13, 1)
	}
}
