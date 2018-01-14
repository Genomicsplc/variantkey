package varianthash

import "testing"

var testDataRV = []struct {
	rsid     uint32
	assembly uint32
	chrom    uint32
	pos      uint32
	refalt   uint32
}{
	{0x00000001, 0x8b29d2c7, 0x00000005, 0x00006edf, 0x387351cb},
	{0x00000002, 0x8b29d2c7, 0x00000007, 0x0022e9c0, 0xd0b64ba3},
	{0x00000003, 0x8b29d2c7, 0x00000004, 0x0000ec56, 0xcef1000f},
	{0x00000004, 0x8b29d2c7, 0x00000008, 0x00028f54, 0x8a59f7ae},
	{0x00000005, 0x8b29d2c7, 0x00000003, 0x000124a3, 0x4c61400f},
	{0x00000006, 0x8b29d2c7, 0x00000009, 0x000143fc, 0x6bddcdbc},
	{0x00000007, 0x8b29d2c7, 0x00000002, 0x00006d6d, 0x181d293a},
	{0x00000008, 0x8b29d2c7, 0x0000000a, 0x00019015, 0x387351cb},
	{0x00000009, 0x8b29d2c7, 0x00000001, 0x0004f442, 0x387351cb},
	{0x0000000a, 0x8b29d2c7, 0x0000000b, 0x00032edc, 0x145dc65c},
}

var testDataVR = []struct {
	assembly uint32
	chrom    uint32
	pos      uint32
	refalt   uint32
	rsid     uint32
}{
	{0x8b29d2c7, 0x00000001, 0x0004f442, 0x387351cb, 0x00000009},
	{0x8b29d2c7, 0x00000002, 0x00006d6d, 0x181d293a, 0x00000007},
	{0x8b29d2c7, 0x00000003, 0x000124a3, 0x4c61400f, 0x00000005},
	{0x8b29d2c7, 0x00000004, 0x0000ec56, 0xcef1000f, 0x00000003},
	{0x8b29d2c7, 0x00000005, 0x00006edf, 0x387351cb, 0x00000001},
	{0x8b29d2c7, 0x00000007, 0x0022e9c0, 0xd0b64ba3, 0x00000002},
	{0x8b29d2c7, 0x00000008, 0x00028f54, 0x8a59f7ae, 0x00000004},
	{0x8b29d2c7, 0x00000009, 0x000143fc, 0x6bddcdbc, 0x00000006},
	{0x8b29d2c7, 0x0000000a, 0x00019015, 0x387351cb, 0x00000008},
	{0x8b29d2c7, 0x0000000b, 0x00032edc, 0x145dc65c, 0x0000000a},
}

func TestGetVRRsid(t *testing.T) {
	for i, tt := range testDataVR {
		i := i
		tt := tt
		t.Run("", func(t *testing.T) {
			t.Parallel()
			rsid := vr.GetVRRsid(uint64(i))
			if rsid != tt.rsid {
				t.Errorf("Expected %x, got %x", tt.rsid, rsid)
			}
		})
	}
}

func BenchmarkGetVRRsid(b *testing.B) {
	b.ResetTimer()
	for i := 0; i < b.N; i++ {
		vr.GetVRRsid(3)
	}
}

func TestGetRVVarhash(t *testing.T) {
	for i, tt := range testDataRV {
		i := i
		tt := tt
		t.Run("", func(t *testing.T) {
			t.Parallel()
			vh := rv.GetRVVarhash(uint64(i))
			if vh.Assembly != tt.assembly {
				t.Errorf("Expected assembly %x, got %x", tt.assembly, vh.Assembly)
			}
			if vh.Chrom != tt.chrom {
				t.Errorf("Expected chrom %x, got %x", tt.chrom, vh.Chrom)
			}
			if vh.Pos != tt.pos {
				t.Errorf("Expected pos %x, got %x", tt.pos, vh.Pos)
			}
			if vh.RefAlt != tt.refalt {
				t.Errorf("Expected refalt %x, got %x", tt.refalt, vh.RefAlt)
			}
		})
	}
}

func BenchmarkGetRVVarhash(b *testing.B) {
	b.ResetTimer()
	for i := 0; i < b.N; i++ {
		rv.GetRVVarhash(3)
	}
}

func TestFindRVVarhashByRsid(t *testing.T) {
	for i, tt := range testDataRV {
		i := i
		tt := tt
		t.Run("", func(t *testing.T) {
			t.Parallel()
			vh, first := rv.FindRVVarhashByRsid(0, 9, tt.rsid)
			if first != uint64(i) {
				t.Errorf("Expected first %d, got %d", i, first)
			}
			if vh.Assembly != tt.assembly {
				t.Errorf("Expected assembly %x, got %x", tt.assembly, vh.Assembly)
			}
			if vh.Chrom != tt.chrom {
				t.Errorf("Expected chrom %x, got %x", tt.chrom, vh.Chrom)
			}
			if vh.Pos != tt.pos {
				t.Errorf("Expected pos %x, got %x", tt.pos, vh.Pos)
			}
			if vh.RefAlt != tt.refalt {
				t.Errorf("Expected refalt %x, got %x", tt.refalt, vh.RefAlt)
			}
		})
	}
}

func TestFindRVVarhashByRsidNotFound(t *testing.T) {
	vh, first := rv.FindRVVarhashByRsid(0, 9, 0xfffffff0)
	if first != 10 {
		t.Errorf("Expected first 10, got %d", first)
	}
	if vh.Assembly != 0 {
		t.Errorf("Expected assembly 0, got %x", vh.Assembly)
	}
	if vh.Chrom != 0 {
		t.Errorf("Expected chrom 0, got %x", vh.Chrom)
	}
	if vh.Pos != 0 {
		t.Errorf("Expected pos0, got %x", vh.Pos)
	}
	if vh.RefAlt != 0 {
		t.Errorf("Expected refalt 0, got %x", vh.RefAlt)
	}
}

func BenchmarkFindRVVarhashByRsid(b *testing.B) {
	b.ResetTimer()
	for i := 0; i < b.N; i++ {
		rv.FindRVVarhashByRsid(0, 9, 0x387351cb)
	}
}

func TestFindVRRsidByVarshash(t *testing.T) {
	for i, tt := range testDataVR {
		i := i
		tt := tt
		t.Run("", func(t *testing.T) {
			t.Parallel()
			vh := TVariantHash{
				Assembly: testDataVR[i].assembly,
				Chrom:    testDataVR[i].chrom,
				Pos:      testDataVR[i].pos,
				RefAlt:   testDataVR[i].refalt,
			}
			rsid, first := vr.FindVRRsidByVarshash(0, 9, vh)
			if rsid != tt.rsid {
				t.Errorf("%d. Expected %x, got %x", i, tt.rsid, rsid)
			}
			if first != uint64(i) {
				t.Errorf("%d. Expected first %d, got %d", i, i, first)
			}
		})
	}
}

func TestFindVRRsidByVarshashNotFound(t *testing.T) {
	vh := TVariantHash{}
	vh.Assembly = 0xfffffff0
	vh.Chrom = 0xfffffff0
	vh.Pos = 0xfffffff0
	vh.RefAlt = 0xfffffff0
	rsid, first := vr.FindVRRsidByVarshash(0, 9, vh)
	if rsid != 0 {
		t.Errorf("Expected rsid 0, got %x", rsid)
	}
	if first != 10 {
		t.Errorf("Expected first 10, got %d", first)
	}
}

func BenchmarkFindVRRsidByVarshash(b *testing.B) {
	vh := TVariantHash{
		Assembly: 0x8b29d2c7,
		Chrom:    0x00000003,
		Pos:      0x000124a3,
		RefAlt:   0x8ffb1a03,
	}
	b.ResetTimer()
	for i := 0; i < b.N; i++ {
		vr.FindVRRsidByVarshash(0, 9, vh)
	}
}

func TestFindVRChromposRange(t *testing.T) {
	rsid, first, last := vr.FindVRChromposRange(0, 9, testDataVR[4].chrom, testDataVR[4].pos, testDataVR[4].pos)
	if rsid != testDataVR[4].rsid {
		t.Errorf("Expected rsid %x, got %x", testDataVR[4].rsid, rsid)
	}
	if first != 4 {
		t.Errorf("Expected first 4, got %d", first)
	}
	if last != 4 {
		t.Errorf("Expected last 4, got %d", last)
	}
}

func TestFindVRChromposRangeNotFound(t *testing.T) {
	rsid, first, last := vr.FindVRChromposRange(0, 9, 0xfffffff0, 0xffffff00, 0xfffffff0)
	if rsid != 0 {
		t.Errorf("Expected rsid 0, got %x", rsid)
	}
	if first != 10 {
		t.Errorf("Expected first 10, got %d", first)
	}
	if last != 9 {
		t.Errorf("Expected last 9, got %d", last)
	}
}

func BenchmarkFindVRChromposRange(b *testing.B) {
	b.ResetTimer()
	for i := 0; i < b.N; i++ {
		vr.FindVRChromposRange(0, 9, 0x00000005, 0x00006f88, 0x00006ed7)
	}
}
