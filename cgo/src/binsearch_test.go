package varianthash

import "testing"

var mf TMMFile

var testData32 = []struct {
	blkpos     uint64
	first      uint64
	last       uint64
	search     uint32
	foundFirst uint64
	foundLast  uint64
}{
	{4, 0, 99, 0x00002722, 0, 0},
	{4, 0, 99, 0x000033f5, 99, 99},
	{0, 0, 99, 0x00000001, 0, 99},
	{4, 0, 99, 0xf00027f3, 100, 100},
	{4, 0, 99, 0x000027f3, 13, 14},
	{4, 13, 99, 0x000027f3, 13, 14},
	{4, 14, 99, 0x000027f3, 14, 14},
	{4, 0, 0, 0x00000001, 1, 1},
	{4, 0, 0, 0xfffffff0, 1, 1},
	{4, 99, 99, 0x00000001, 100, 100},
	{4, 99, 99, 0xfffffff0, 100, 100},
}

var testData64 = []struct {
	blkpos     uint64
	first      uint64
	last       uint64
	search     uint64
	foundFirst uint64
	foundLast  uint64
}{
	{4, 0, 99, 0x000027225FB6E591, 0, 0},
	{4, 0, 99, 0x000033F522A78FD9, 99, 99},
	{0, 0, 99, 0x0000000100002722, 0, 0},
	{4, 0, 99, 0xf0000001f00027f3, 100, 100},
	{4, 0, 99, 0x000027F35FB6E591, 13, 13},
	{0, 13, 99, 0x00000001000027f3, 13, 14},
	{0, 14, 99, 0x00000001000027f3, 14, 14},
	{4, 0, 0, 0x0000000000000001, 1, 1},
	{4, 0, 0, 0xfffffffffffffff0, 1, 1},
	{4, 99, 99, 0x0000000000000001, 100, 100},
	{4, 99, 99, 0xfffffffffffffff0, 100, 100},
}

var testData128 = []struct {
	blkpos     uint64
	first      uint64
	last       uint64
	searchHi   uint64
	searchLo   uint64
	foundFirst uint64
	foundLast  uint64
}{
	{4, 0, 99, 0x000027225FB6E591, 0x6EB7ABD92E3DEB1D, 0, 0},
	{4, 0, 99, 0x000027C07B9621EC, 0x01F886390C06811D, 10, 10},
	{4, 0, 0, 0x000027225FB6E591, 0x6EB7ABD92E3DEB1A, 1, 1},
	{4, 0, 99, 0x000027C30981EF0F, 0x500126C20C059EB5, 12, 12},
	{4, 0, 99, 0x000027F35FB6E591, 0x6EB7ABD90889E85E, 13, 13},
	{4, 0, 99, 0x000027F3D41A0CE2, 0xDF116BBC0BF2CF80, 100, 100},
	{4, 0, 99, 0x000027F53B9E3036, 0x5103B7A62E3FBBCC, 100, 100},
	{4, 0, 99, 0x000027F690C4DEFF, 0x765F63B80BF00517, 16, 16},
	{4, 0, 99, 0x000033F522A78FD9, 0x1ACC7B430AC5CA22, 99, 99},
	{4, 0, 0, 0x0000000000000001, 0x0000000000000001, 1, 1},
	{4, 0, 0, 0xfffffffffffffff0, 0xfffffffffffffff0, 1, 1},
}

func TestGetAddress(t *testing.T) {
	h := GetAddress(3, 5, 7)
	if h != 26 {
		t.Errorf("Expected 26, got %d", h)
	}
}

func TestFindFirstUint32be(t *testing.T) {
	for i, tt := range testData32 {
		h := mf.FindFirstUint32be(20, tt.blkpos, tt.first, tt.last, tt.search)
		if h != tt.foundFirst {
			t.Errorf("%d. Expected %#016x, got %#016x", i, tt.foundFirst, h)
		}
	}
}

func BenchmarkFindFirstUint32be(b *testing.B) {
	b.ResetTimer()
	for i := 0; i < b.N; i++ {
		mf.FindFirstUint32be(20, 4, 0, 99, 0x000027f3)
	}
}

func TestFindLastUint32be(t *testing.T) {
	for i, tt := range testData32 {
		h := mf.FindLastUint32be(20, tt.blkpos, tt.first, tt.last, tt.search)
		if h != tt.foundLast {
			t.Errorf("%d. Expected %d, got %d", i, tt.foundLast, h)
		}
	}
}

func BenchmarkFindLastUint32be(b *testing.B) {
	b.ResetTimer()
	for i := 0; i < b.N; i++ {
		mf.FindLastUint32be(20, 4, 0, 99, 0x000027f3)
	}
}

func TestFindFirstUint64be(t *testing.T) {
	for i, tt := range testData64 {
		h := mf.FindFirstUint64be(20, tt.blkpos, tt.first, tt.last, tt.search)
		if h != tt.foundFirst {
			t.Errorf("%d. Expected %#016x, got %#016x", i, tt.foundFirst, h)
		}
	}
}

func BenchmarkFindFirstUint64be(b *testing.B) {
	b.ResetTimer()
	for i := 0; i < b.N; i++ {
		mf.FindFirstUint64be(20, 4, 0, 99, 0x000027F35FB6E591)
	}
}

func TestFindLastUint64be(t *testing.T) {
	for i, tt := range testData64 {
		h := mf.FindLastUint64be(20, tt.blkpos, tt.first, tt.last, tt.search)
		if h != tt.foundLast {
			t.Errorf("%d. Expected %d, got %d", i, tt.foundLast, h)
		}
	}
}

func BenchmarkFindLastUint64be(b *testing.B) {
	b.ResetTimer()
	for i := 0; i < b.N; i++ {
		mf.FindLastUint64be(20, 4, 0, 99, 0x000027F35FB6E591)
	}
}

func TestFindFirstUint128be(t *testing.T) {
	for i, tt := range testData128 {
		h := mf.FindFirstUint128be(20, tt.blkpos, tt.first, tt.last, tt.searchHi, tt.searchLo)
		if h != tt.foundFirst {
			t.Errorf("%d. Expected %#016x, got %#016x", i, tt.foundFirst, h)
		}
	}
}

func BenchmarkFindFirstUint128be(b *testing.B) {
	b.ResetTimer()
	for i := 0; i < b.N; i++ {
		mf.FindFirstUint128be(20, 4, 0, 99, 0x000027C30981EF0F, 0x500126C20C059EB5)
	}
}

func TestFindLastUint128be(t *testing.T) {
	for i, tt := range testData128 {
		h := mf.FindLastUint128be(20, tt.blkpos, tt.first, tt.last, tt.searchHi, tt.searchLo)
		if h != tt.foundLast {
			t.Errorf("%d. Expected %d, got %d", i, tt.foundLast, h)
		}
	}
}

func BenchmarkFindLastUint128be(b *testing.B) {
	b.ResetTimer()
	for i := 0; i < b.N; i++ {
		mf.FindLastUint128be(20, 4, 0, 99, 0x000027C30981EF0F, 0x500126C20C059EB5)
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
	if err == nil {
		t.Errorf("An error was expected")
	}
}

func TestMmapBinFileError(t *testing.T) {
	_, err := MmapBinFile("error")
	if err == nil {
		t.Errorf("An error was expected")
	}
}
