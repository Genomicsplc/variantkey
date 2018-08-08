package variantkey

// regionkey_test.go
// @category   Libraries
// @author     Nicola Asuni <nicola.asuni@genomicsplc.com>
// @copyright  2017-2018 GENOMICS plc
// @license    MIT (see LICENSE)
// @link       https://github.com/genomicsplc/variantkey

import "testing"

// TRegion contains test data
type TRegionData struct {
	chrom    string
	startpos uint32
	endpos   uint32
	strand   int8
	echrom   uint8
	estrand  uint8
	rk       uint64
	rs       string
	csp      uint64
	cep      uint64
}

var regionsTestData = []TRegionData{
	{"1", 1000, 1100, 0, 1, 0, 0x080001f400002260, "080001f400002260", 0x00000000100003e8, 0x000000001000044c},
	{"2", 1001, 1201, 1, 2, 1, 0x100001f48000258a, "100001f48000258a", 0x00000000200003e9, 0x00000000200004b1},
	{"3", 1002, 1302, -1, 3, 2, 0x180001f5000028b4, "180001f5000028b4", 0x00000000300003ea, 0x0000000030000516},
	{"4", 1003, 1403, 0, 4, 0, 0x200001f580002bd8, "200001f580002bd8", 0x00000000400003eb, 0x000000004000057b},
	{"5", 1004, 1504, 1, 5, 1, 0x280001f600002f02, "280001f600002f02", 0x00000000500003ec, 0x00000000500005e0},
	{"10", 1005, 1605, -1, 10, 2, 0x500001f68000322c, "500001f68000322c", 0x00000000a00003ed, 0x00000000a0000645},
	{"22", 1006, 1706, 0, 22, 0, 0xb00001f700003550, "b00001f700003550", 0x00000001600003ee, 0x00000001600006aa},
	{"X", 1007, 1807, 1, 23, 1, 0xb80001f78000387a, "b80001f78000387a", 0x00000001700003ef, 0x000000017000070f},
	{"Y", 1008, 1908, -1, 24, 2, 0xc00001f800003ba4, "c00001f800003ba4", 0x00000001800003f0, 0x0000000180000774},
	{"MT", 1009, 2009, 0, 25, 0, 0xc80001f880003ec8, "c80001f880003ec8", 0x00000001900003f1, 0x00000001900007d9},
}

func TestEncodeRegionStrand(t *testing.T) {
	for _, v := range regionsTestData {
		v := v
		t.Run("", func(t *testing.T) {
			t.Parallel()
			estrand := EncodeRegionStrand(v.strand)
			if estrand != v.estrand {
				t.Errorf("The code value is different, expected %#v got %#v", v.estrand, estrand)
			}
		})
	}
}

func BenchmarkEncodeRegionStrand(b *testing.B) {
	b.ResetTimer()
	for i := 0; i < b.N; i++ {
		EncodeRegionStrand(-1)
	}
}

func TestDecodeRegionStrand(t *testing.T) {
	for _, v := range regionsTestData {
		v := v
		t.Run("", func(t *testing.T) {
			t.Parallel()
			strand := DecodeRegionStrand(v.estrand)
			if strand != v.strand {
				t.Errorf("The code value is different, expected %#v got %#v", v.strand, strand)
			}
		})
	}
}

func BenchmarkDecodeRegionStrand(b *testing.B) {
	b.ResetTimer()
	for i := 0; i < b.N; i++ {
		DecodeRegionStrand(2)
	}
}

func TestEncodeRegionKey(t *testing.T) {
	for _, v := range regionsTestData {
		v := v
		t.Run("", func(t *testing.T) {
			t.Parallel()
			rk := EncodeRegionKey(v.echrom, v.startpos, v.endpos, v.estrand)
			if rk != v.rk {
				t.Errorf("The code value is different, expected %#v got %#v", v.rk, rk)
			}
		})
	}
}

func BenchmarkEncodeRegionKey(b *testing.B) {
	b.ResetTimer()
	for i := 0; i < b.N; i++ {
		EncodeRegionKey(25, 1000, 2000, 2)
	}
}

func TestExtractRegionKeyChrom(t *testing.T) {
	for _, v := range regionsTestData {
		v := v
		t.Run("", func(t *testing.T) {
			t.Parallel()
			chrom := ExtractRegionKeyChrom(v.rk)
			if chrom != v.echrom {
				t.Errorf("The chrom value is different, expected %#v got: %#v", v.echrom, chrom)
			}
		})
	}
}

func TestExtractRegionKeyStartPos(t *testing.T) {
	for _, v := range regionsTestData {
		v := v
		t.Run("", func(t *testing.T) {
			t.Parallel()
			startpos := ExtractRegionKeyStartPos(v.rk)
			if startpos != v.startpos {
				t.Errorf("The startpos value is different, expected %#v got: %#v", v.startpos, startpos)
			}
		})
	}
}

func TestExtractRegionKeyEndPos(t *testing.T) {
	for _, v := range regionsTestData {
		v := v
		t.Run("", func(t *testing.T) {
			t.Parallel()
			endpos := ExtractRegionKeyEndPos(v.rk)
			if endpos != v.endpos {
				t.Errorf("The startpos value is different, expected %#v got: %#v", v.endpos, endpos)
			}
		})
	}
}

func TestExtractRegionKeyStrand(t *testing.T) {
	for _, v := range regionsTestData {
		v := v
		t.Run("", func(t *testing.T) {
			t.Parallel()
			estrand := ExtractRegionKeyStrand(v.rk)
			if estrand != v.estrand {
				t.Errorf("The strand value is different, expected %#v got: %#v", v.estrand, estrand)
			}
		})
	}
}

func TestDecodeRegionKey(t *testing.T) {
	for _, v := range regionsTestData {
		v := v
		t.Run("", func(t *testing.T) {
			t.Parallel()
			rk := DecodeRegionKey(v.rk)
			if rk.Chrom != v.echrom {
				t.Errorf("The chrom hash value is different, expected %#v got: %#v", v.echrom, rk.Chrom)
			}
			if rk.StartPos != v.startpos {
				t.Errorf("The startpos value is different, expected %#v got: %#v", v.startpos, rk.StartPos)
			}
			if rk.EndPos != v.endpos {
				t.Errorf("The endpos value is different, expected %#v got: %#v", v.endpos, rk.EndPos)
			}
			if rk.Strand != v.estrand {
				t.Errorf("The strand value is different, expected %#v got: %#v", v.estrand, rk.Strand)
			}
		})
	}
}

func BenchmarkDecodeRegionKey(b *testing.B) {
	b.ResetTimer()
	for i := 0; i < b.N; i++ {
		DecodeRegionKey(0x080001f400002260)
	}
}

func TestReverseRegionKey(t *testing.T) {
	for _, v := range regionsTestData {
		v := v
		t.Run("", func(t *testing.T) {
			t.Parallel()
			rev := ReverseRegionKey(v.rk)
			if rev.Chrom != v.chrom {
				t.Errorf("The chrom is different, expected %#v got: %#v", v.chrom, rev.Chrom)
			}
			if rev.StartPos != v.startpos {
				t.Errorf("The start pos is different, expected %#v got: %#v", v.startpos, rev.StartPos)
			}
			if rev.EndPos != v.endpos {
				t.Errorf("The end pos is different, expected %#v got: %#v", v.endpos, rev.EndPos)
			}
			if rev.Strand != v.strand {
				t.Errorf("The strand is different, expected %#v got: %#v", v.strand, rev.Strand)
			}
		})
	}
}

func BenchmarkReverseRegionKey(b *testing.B) {
	b.ResetTimer()
	for i := 0; i < b.N; i++ {
		ReverseRegionKey(0x080001f400002260)
	}
}

func TestRegionKey(t *testing.T) {
	for _, v := range regionsTestData {
		v := v
		t.Run("", func(t *testing.T) {
			t.Parallel()
			rk := RegionKey(v.chrom, v.startpos, v.endpos, v.strand)
			if rk != v.rk {
				t.Errorf("The code value is different, expected %#v got %#v", v.rk, rk)
			}
		})
	}
}

func BenchmarkRegionKey(b *testing.B) {
	b.ResetTimer()
	for i := 0; i < b.N; i++ {
		RegionKey("MT", 1000, 2000, 1)
	}
}

func TestAreOverlappingRegions(t *testing.T) {
	type TOverlapData struct {
		chromA    uint8
		startposA uint32
		endposA   uint32
		chromB    uint8
		startposB uint32
		endposB   uint32
		res       bool
	}
	var overlapData = []TOverlapData{
		{1, 5, 7, 2, 5, 7, false},    // different chromosome
		{1, 0, 2, 1, 3, 7, false},    // [ ]|   |
		{2, 1, 3, 2, 3, 7, false},    //  [ ]   |
		{3, 2, 4, 3, 3, 7, true},     //   [|]  |
		{4, 3, 5, 4, 3, 7, true},     //    [ ] |
		{5, 4, 6, 5, 3, 7, true},     //    |[ ]|
		{6, 5, 7, 6, 3, 7, true},     //    | [ ]
		{10, 6, 8, 10, 3, 7, true},   //    |  [|]
		{22, 7, 9, 22, 3, 7, false},  //    |   [ ]
		{23, 8, 10, 23, 3, 7, false}, //    |   |[ ]
		{24, 2, 8, 24, 3, 7, true},   //   [|   |]
		{25, 3, 7, 25, 3, 7, true},   //    [   ]
	}
	for _, v := range overlapData {
		v := v
		t.Run("", func(t *testing.T) {
			t.Parallel()
			res := AreOverlappingRegions(v.chromA, v.startposA, v.endposA, v.chromB, v.startposB, v.endposB)
			if res != v.res {
				t.Errorf("The result value is different, expected %#v got: %#v", v.res, res)
			}
		})
	}
}

func BenchmarkAreOverlappingRegions(b *testing.B) {
	b.ResetTimer()
	for i := 0; i < b.N; i++ {
		AreOverlappingRegions(25, 1000, 2000, 25, 1500, 2500)
	}
}

func TestGetRegionkeyChromStartPos(t *testing.T) {
	for _, v := range regionsTestData {
		v := v
		t.Run("", func(t *testing.T) {
			t.Parallel()
			csp := GetRegionkeyChromStartPos(v.rk)
			if csp != v.csp {
				t.Errorf("The chrom-startpos value is different, expected %#v got %#v", v.csp, csp)
			}
		})
	}
}

func BenchmarkGetRegionkeyChromStartPos(b *testing.B) {
	b.ResetTimer()
	for i := 0; i < b.N; i++ {
		GetRegionkeyChromStartPos(0x080001f400002260)
	}
}

func TestGetRegionkeyChromEndPos(t *testing.T) {
	for _, v := range regionsTestData {
		v := v
		t.Run("", func(t *testing.T) {
			t.Parallel()
			cep := GetRegionkeyChromEndPos(v.rk)
			if cep != v.cep {
				t.Errorf("The chrom-endpos value is different, expected %#v got %#v", v.cep, cep)
			}
		})
	}
}

func BenchmarkGetRegionkeyChromEndPos(b *testing.B) {
	b.ResetTimer()
	for i := 0; i < b.N; i++ {
		GetRegionkeyChromEndPos(0x080001f400002260)
	}
}
