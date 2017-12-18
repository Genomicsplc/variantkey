package varianthash

import "testing"

var testDataRV = []struct {
	rsid     uint32
	assembly uint32
	chrom    uint32
	pos      uint32
	refalt   uint32
}{
	{0x00000001, 0x145dc65c, 0x8b29d2c7, 0x00000008, 0x00028f81},
	{0x00000002, 0x387351cb, 0x8b29d2c7, 0x00000001, 0x0004f442},
	{0x00000003, 0x387351cb, 0x8b29d2c7, 0x00000003, 0x000124ce},
	{0x00000004, 0x8ffb1a03, 0x8b29d2c7, 0x00000003, 0x000124a3},
	{0x00000005, 0x9f8e11a5, 0x8b29d2c7, 0x00000005, 0x00006f88},
	{0x00000006, 0xcef1000f, 0x8b29d2c7, 0x00000005, 0x00006e9a},
	{0x00000007, 0xf4d7e424, 0x8b29d2c7, 0x00000005, 0x00006ed7},
	{0x00000008, 0xf4d7e424, 0x8b29d2c7, 0x00000007, 0x0022ea0f},
	{0x00000009, 0xf8d91d98, 0x8b29d2c7, 0x00000002, 0x00006d98},
	{0x0000000a, 0xf8d91d98, 0x8b29d2c7, 0x00000006, 0x0002f2d6},
}

var testDataVR = []struct {
	assembly uint32
	chrom    uint32
	pos      uint32
	refalt   uint32
	rsid     uint32
}{
	{0x145dc65c, 0x8b29d2c7, 0x00000008, 0x00028f81, 0x00000001},
	{0x387351cb, 0x8b29d2c7, 0x00000001, 0x0004f442, 0x00000002},
	{0x387351cb, 0x8b29d2c7, 0x00000003, 0x000124ce, 0x00000003},
	{0x8ffb1a03, 0x8b29d2c7, 0x00000003, 0x000124a3, 0x00000004},
	{0x9f8e11a5, 0x8b29d2c7, 0x00000005, 0x00006f88, 0x00000005},
	{0xcef1000f, 0x8b29d2c7, 0x00000005, 0x00006e9a, 0x00000006},
	{0xf4d7e424, 0x8b29d2c7, 0x00000005, 0x00006ed7, 0x00000007},
	{0xf4d7e424, 0x8b29d2c7, 0x00000007, 0x0022ea0f, 0x00000008},
	{0xf8d91d98, 0x8b29d2c7, 0x00000002, 0x00006d98, 0x00000009},
	{0xf8d91d98, 0x8b29d2c7, 0x00000006, 0x0002f2d6, 0x0000000a},
}

func TestGetVRRsid(t *testing.T) {
	for i, tt := range testDataVR {
		rsid := vr.GetVRRsid(uint64(i))
		if rsid != tt.rsid {
			t.Errorf("%d. Expected %x, got %x", i, tt.rsid, rsid)
		}
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
		vh := rv.GetRVVarhash(uint64(i))
		if vh.Assembly != tt.assembly {
			t.Errorf("%d. Expected assembly %x, got %x", i, tt.assembly, vh.Assembly)
		}
		if vh.Chrom != tt.chrom {
			t.Errorf("%d. Expected chrom %x, got %x", i, tt.chrom, vh.Chrom)
		}
		if vh.Pos != tt.pos {
			t.Errorf("%d. Expected pos %x, got %x", i, tt.pos, vh.Pos)
		}
		if vh.RefAlt != tt.refalt {
			t.Errorf("%d. Expected refalt %x, got %x", i, tt.refalt, vh.RefAlt)
		}
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
		vh, first := rv.FindRVVarhashByRsid(0, 9, tt.rsid)
		if first != uint64(i) {
			t.Errorf("%d. Expected first %d, got %d", i, i, first)
		}
		if vh.Assembly != tt.assembly {
			t.Errorf("%d. Expected assembly %x, got %x", i, tt.assembly, vh.Assembly)
		}
		if vh.Chrom != tt.chrom {
			t.Errorf("%d. Expected chrom %x, got %x", i, tt.chrom, vh.Chrom)
		}
		if vh.Pos != tt.pos {
			t.Errorf("%d. Expected pos %x, got %x", i, tt.pos, vh.Pos)
		}
		if vh.RefAlt != tt.refalt {
			t.Errorf("%d. Expected refalt %x, got %x", i, tt.refalt, vh.RefAlt)
		}
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

func TestFindVRChromposRange(t *testing.T) {
	rsid, first, last := vr.FindVRChromposRange(0, 9, testDataVR[0].chrom, testDataVR[3].pos, testDataVR[7].pos)
	if rsid != testDataVR[3].rsid {
		t.Errorf("Expected rsid %x, got %x", testDataVR[3].rsid, rsid)
	}
	if first != 3 {
		t.Errorf("Expected first 3, got %d", first)
	}
	if last != 7 {
		t.Errorf("Expected last 7, got %d", last)
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

func TestFindVRRsidByVarshash(t *testing.T) {
	vh := TVariantHash{}
	for i, tt := range testDataVR {
		vh.Assembly = testDataVR[i].assembly
		vh.Chrom = testDataVR[i].chrom
		vh.Pos = testDataVR[i].pos
		vh.RefAlt = testDataVR[i].refalt
		rsid, first := vr.FindVRRsidByVarshash(0, 9, vh)
		if rsid != tt.rsid {
			t.Errorf("%d. Expected %x, got %x", i, tt.rsid, rsid)
		}
		if first != uint64(i) {
			t.Errorf("%d. Expected first %d, got %d", i, i, first)
		}
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
