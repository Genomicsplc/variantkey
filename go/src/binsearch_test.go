package varianthash

import "testing"

var mf TMMFile

var testData32 = []struct {
	blkpos      uint64
	first       uint64
	last        uint64
	search      uint32
	foundFirst  uint64
	foundFFirst uint64
	foundFLast  uint64
	foundLast   uint64
	foundLFirst uint64
	foundLLast  uint64
}{
	{4, 0, 99, 0x00002722, 0, 0, 1, 0, 1, 0},
	{4, 0, 99, 0x000033f5, 99, 99, 98, 99, 100, 99},
	{0, 0, 99, 0x00000001, 0, 0, 1, 99, 100, 99},
	{4, 0, 99, 0xf00027f3, 100, 100, 99, 100, 100, 99},
	{4, 0, 99, 0x000027f3, 13, 13, 12, 14, 15, 14},
	{4, 13, 99, 0x000027f3, 13, 13, 12, 14, 15, 14},
	{4, 14, 99, 0x000027f3, 14, 14, 13, 14, 15, 14},
	{4, 0, 0, 0x00000001, 1, 0, 0, 1, 0, 0},
	{4, 0, 0, 0xfffffff0, 1, 1, 0, 1, 1, 0},
	{4, 99, 99, 0x00000001, 100, 99, 98, 100, 99, 98},
	{4, 99, 99, 0xfffffff0, 100, 100, 99, 100, 100, 99},
	{4, 0, 99, 0x00002805, 100, 19, 18, 100, 19, 18},
	{4, 0, 99, 0x000028ff, 100, 45, 44, 100, 45, 44},
}

var testData64 = []struct {
	blkpos      uint64
	first       uint64
	last        uint64
	search      uint64
	foundFirst  uint64
	foundFFirst uint64
	foundFLast  uint64
	foundLast   uint64
	foundLFirst uint64
	foundLLast  uint64
}{
	{4, 0, 99, 0x000027225fb6e591, 0, 0, 1, 0, 1, 0},
	{4, 0, 99, 0x000033f522a78fd9, 99, 99, 98, 99, 100, 99},
	{0, 0, 99, 0x0000000100002722, 0, 0, 1, 0, 1, 0},
	{4, 0, 99, 0xf0000001f00027f3, 100, 100, 99, 100, 100, 99},
	{4, 0, 99, 0x000027f35fb6e591, 13, 13, 12, 13, 14, 13},
	{0, 13, 99, 0x00000001000027f3, 13, 13, 12, 14, 15, 14},
	{0, 14, 99, 0x00000001000027f3, 14, 14, 13, 14, 15, 14},
	{4, 0, 0, 0x0000000000000001, 1, 0, 0, 1, 0, 0},
	{4, 0, 0, 0xfffffffffffffff0, 1, 1, 0, 1, 1, 0},
	{4, 99, 99, 0x0000000000000001, 100, 99, 98, 100, 99, 98},
	{4, 99, 99, 0xfffffffffffffff0, 100, 100, 99, 100, 100, 99},
	{4, 0, 99, 0x000028060981ef0a, 100, 19, 18, 100, 19, 18},
	{4, 0, 99, 0x000028fca24c9149, 100, 45, 44, 100, 45, 44},
}

var testData128 = []struct {
	blkpos      uint64
	first       uint64
	last        uint64
	search      Uint128
	foundFirst  uint64
	foundFFirst uint64
	foundFLast  uint64
	foundLast   uint64
	foundLFirst uint64
	foundLLast  uint64
}{
	{4, 0, 99, Uint128{0x000027225fb6e591, 0x6eb7abd92e3deb1d}, 0, 0, 1, 0, 1, 0},
	{4, 0, 99, Uint128{0x000027c07b9621ec, 0x01f886390c06811d}, 10, 10, 9, 10, 11, 10},
	{4, 0, 0, Uint128{0x000027225fb6e591, 0x6eb7abd92e3deb1a}, 1, 0, 0, 1, 0, 0},
	{4, 0, 99, Uint128{0x000027c30981ef0f, 0x500126c20c059eb5}, 12, 12, 11, 12, 13, 12},
	{4, 0, 99, Uint128{0x000027f35fb6e591, 0x6eb7abd90889e85e}, 13, 13, 12, 13, 14, 13},
	{4, 0, 99, Uint128{0x000027f3d41a0ce2, 0xdf116bbc0bf2cf80}, 100, 14, 13, 100, 14, 13},
	{4, 0, 99, Uint128{0x000027f53b9e3036, 0x5103b7a62e3fbbcc}, 100, 16, 15, 100, 16, 15},
	{4, 0, 99, Uint128{0x000027f690c4deff, 0x765f63b80bf00517}, 16, 16, 15, 16, 17, 16},
	{4, 0, 99, Uint128{0x000033f522a78fd9, 0x1acc7b430ac5ca22}, 99, 99, 98, 99, 100, 99},
	{4, 0, 0, Uint128{0x0000000000000001, 0x0000000000000001}, 1, 0, 0, 1, 0, 0},
	{4, 0, 0, Uint128{0xfffffffffffffff0, 0xfffffffffffffff0}, 1, 1, 0, 1, 1, 0},
	{4, 0, 99, Uint128{0x000028060981ef0f, 0x500126c22f813253}, 100, 19, 18, 100, 19, 18},
	{4, 0, 99, Uint128{0x000028fca24c9148, 0x830a986a0be5c095}, 100, 45, 44, 100, 45, 44},
}

func TestGetAddress(t *testing.T) {
	h := GetAddress(3, 5, 7)
	if h != 26 {
		t.Errorf("Expected 26, got %d", h)
	}
}

func TestFindFirstUint32(t *testing.T) {
	for _, tt := range testData32 {
		h, f, l := mf.FindFirstUint32(20, tt.blkpos, tt.first, tt.last, tt.search)
		if h != tt.foundFirst {
			t.Errorf("Expected found 0x%x, got 0x%x", tt.foundFirst, h)
		}
		if f != tt.foundFFirst {
			t.Errorf("Expected first 0x%x, got 0x%x", tt.foundFFirst, f)
		}
		if l != tt.foundFLast {
			t.Errorf("Expected last 0x%x, got 0x%x", tt.foundFLast, l)
		}
	}
}

func BenchmarkFindFirstUint32(b *testing.B) {
	b.ResetTimer()
	for i := 0; i < b.N; i++ {
		mf.FindFirstUint32(20, testData32[4].blkpos, 0, 99, testData32[4].search)
	}
}

func TestFindLastUint32(t *testing.T) {
	for _, tt := range testData32 {
		tt := tt
		t.Run("", func(t *testing.T) {
			t.Parallel()
			h, f, l := mf.FindLastUint32(20, tt.blkpos, tt.first, tt.last, tt.search)
			if h != tt.foundLast {
				t.Errorf("Expected found 0x%x, got 0x%x", tt.foundLast, h)
			}
			if f != tt.foundLFirst {
				t.Errorf("Expected first 0x%x, got 0x%x", tt.foundLFirst, f)
			}
			if l != tt.foundLLast {
				t.Errorf("Expected last 0x%x, got 0x%x", tt.foundLLast, l)
			}
		})
	}
}

func BenchmarkFindLastUint32(b *testing.B) {
	b.ResetTimer()
	for i := 0; i < b.N; i++ {
		mf.FindLastUint32(20, testData32[4].blkpos, 0, 99, testData32[4].search)
	}
}

func TestFindFirstUint64(t *testing.T) {
	for _, tt := range testData64 {
		tt := tt
		t.Run("", func(t *testing.T) {
			t.Parallel()
			h, f, l := mf.FindFirstUint64(20, tt.blkpos, tt.first, tt.last, tt.search)
			if h != tt.foundFirst {
				t.Errorf("Expected found 0x%x, got 0x%x", tt.foundFirst, h)
			}
			if f != tt.foundFFirst {
				t.Errorf("Expected first 0x%x, got 0x%x", tt.foundFFirst, f)
			}
			if l != tt.foundFLast {
				t.Errorf("Expected last 0x%x, got 0x%x", tt.foundFLast, l)
			}
		})
	}
}

func BenchmarkFindFirstUint64(b *testing.B) {
	b.ResetTimer()
	for i := 0; i < b.N; i++ {
		mf.FindFirstUint64(20, testData64[4].blkpos, 0, 99, testData64[4].search)
	}
}

func TestFindLastUint64(t *testing.T) {
	for _, tt := range testData64 {
		tt := tt
		t.Run("", func(t *testing.T) {
			t.Parallel()
			h, f, l := mf.FindLastUint64(20, tt.blkpos, tt.first, tt.last, tt.search)
			if h != tt.foundLast {
				t.Errorf("Expected found 0x%x, got 0x%x", tt.foundLast, h)
			}
			if f != tt.foundLFirst {
				t.Errorf("Expected first 0x%x, got 0x%x", tt.foundLFirst, f)
			}
			if l != tt.foundLLast {
				t.Errorf("Expected last 0x%x, got 0x%x", tt.foundLLast, l)
			}
		})
	}
}

func BenchmarkFindLastUint64(b *testing.B) {
	b.ResetTimer()
	for i := 0; i < b.N; i++ {
		mf.FindLastUint64(20, testData64[4].blkpos, 0, 99, testData64[4].search)
	}
}

func TestFindFirstUint128(t *testing.T) {
	for _, tt := range testData128 {
		tt := tt
		t.Run("", func(t *testing.T) {
			t.Parallel()
			h, f, l := mf.FindFirstUint128(20, tt.blkpos, tt.first, tt.last, tt.search)
			if h != tt.foundFirst {
				t.Errorf("Expected found 0x%x, got 0x%x", tt.foundFirst, h)
			}
			if f != tt.foundFFirst {
				t.Errorf("Expected first 0x%x, got 0x%x", tt.foundFFirst, f)
			}
			if l != tt.foundFLast {
				t.Errorf("Expected last 0x%x, got 0x%x", tt.foundFLast, l)
			}
		})
	}
}

func BenchmarkFindFirstUint128(b *testing.B) {
	b.ResetTimer()
	for i := 0; i < b.N; i++ {
		mf.FindFirstUint128(20, testData128[4].blkpos, 0, 99, testData128[4].search)
	}
}

func TestFindLastUint128(t *testing.T) {
	for _, tt := range testData128 {
		tt := tt
		t.Run("", func(t *testing.T) {
			t.Parallel()
			h, f, l := mf.FindLastUint128(20, tt.blkpos, tt.first, tt.last, tt.search)
			if h != tt.foundLast {
				t.Errorf("Expected found 0x%x, got 0x%x", tt.foundLast, h)
			}
			if f != tt.foundLFirst {
				t.Errorf("Expected first 0x%x, got 0x%x", tt.foundLFirst, f)
			}
			if l != tt.foundLLast {
				t.Errorf("Expected last 0x%x, got 0x%x", tt.foundLLast, l)
			}
		})
	}
}

func BenchmarkFindLastUint128(b *testing.B) {
	b.ResetTimer()
	for i := 0; i < b.N; i++ {
		mf.FindLastUint128(20, testData128[4].blkpos, 0, 99, testData128[4].search)
	}
}

func TestClose(t *testing.T) {
	lmf, err := MmapBinFile("../../test/test_data_b.bin")
	if err != nil {
		t.Errorf("Unexpected error: %v", err)
	}
	err = lmf.Close()
	if err != nil {
		t.Errorf("Unexpected error: %v", err)
	}
}

func TestCloseError(t *testing.T) {
	lmf := TMMFile{}
	err := lmf.Close()
	if err != nil {
		t.Errorf("An error was expected")
	}
}

func TestMmapBinFileError(t *testing.T) {
	_, err := MmapBinFile("error")
	if err == nil {
		t.Errorf("An error was expected")
	}
}
