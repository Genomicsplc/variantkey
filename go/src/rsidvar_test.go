package varianthash

import "testing"

var rv, vr TMMFile

var testDataRV = []struct {
	rsid   uint32
	chrom  uint32
	pos    uint32
	refalt uint64
}{
	{0x2F81F572, 0x00000017, 0x0509C4A7, 0xCCD3C1D35DF0CE04},
	{0x2F81F573, 0x00000017, 0x05098728, 0x4EECA287F5C90712},
	{0x2F81F574, 0x0000000B, 0x054CAF51, 0xBB9EE46DF5C4C2E0},
	{0x2F81F575, 0x00000002, 0x095E7925, 0x90C4DEFF765F63B8},
	{0x2F81F576, 0x0000000F, 0x060C1468, 0x0981EF0F500126C2},
	{0x2F81F577, 0x00000011, 0x0199B8D6, 0x22A78FD91ACC7B43},
	{0x2F81F578, 0x0000000A, 0x07D8C4BE, 0x90C4DEFF765F63B8},
	{0x2F81F579, 0x00000001, 0x0DE63E19, 0x453EA229CAA9A652},
	{0x2F81F57A, 0x00000011, 0x026140E3, 0xCCD3C1D35DF0CE04},
	{0x2F81F57B, 0x00000011, 0x02610DFC, 0x26CAA8F5488B8717},
}

var testDataVR = []struct {
	chrom  uint32
	pos    uint32
	refalt uint64
	rsid   uint32
}{
	{0x0000001A, 0x00004005, 0x26CAA8F5488B8717, 0x0277648B},
	{0x0000001A, 0x00004006, 0x3D9C29BE7B4387A2, 0x020B67DE},
	{0x0000001A, 0x00004006, 0xCCD3C1D35DF0CE04, 0x020B67DE},
	{0x0000001A, 0x0000400E, 0x4EECA287F5C90712, 0x0849000D},
	{0x0000001A, 0x0000403C, 0x0981EF0F500126C2, 0x162BA962},
	{0x0000001A, 0x00004072, 0x4EECA287F5C90712, 0x167638AD},
	{0x0000001A, 0x0000407F, 0x0981EF0F500126C2, 0x1649ED55},
	{0x0000001A, 0x00004086, 0x0981EF0F500126C2, 0x003C1309},
	{0x0000001A, 0x0000408F, 0x22A78FD91ACC7B43, 0x170E8C03},
	{0x0000001A, 0x00004090, 0x0981EF0F500126C2, 0x161885A7},
}

func TestGetVRRsid(t *testing.T) {
	for i, tt := range testDataVR {
		rsid := vr.GetVRRsid(uint64(i))
		if rsid != tt.rsid {
			t.Errorf("%d. Expected %#016x, got %#016x", i, tt.rsid, rsid)
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
		if vh.Chrom != tt.chrom {
			t.Errorf("%d. Expected chrom %#08x, got %#08x", i, tt.chrom, vh.Chrom)
		}
		if vh.Pos != tt.pos {
			t.Errorf("%d. Expected pos %#08x, got %#08x", i, tt.pos, vh.Pos)
		}
		if vh.RefAlt != tt.refalt {
			t.Errorf("%d. Expected refalt %#016x, got %#016x", i, tt.refalt, vh.RefAlt)
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
		if vh.Chrom != tt.chrom {
			t.Errorf("%d. Expected chrom %#08x, got %#08x", i, tt.chrom, vh.Chrom)
		}
		if vh.Pos != tt.pos {
			t.Errorf("%d. Expected pos %#08x, got %#08x", i, tt.pos, vh.Pos)
		}
		if vh.RefAlt != tt.refalt {
			t.Errorf("%d. Expected refalt %#016x, got %#016x", i, tt.refalt, vh.RefAlt)
		}
	}
}

func TestFindRVVarhashByRsidNotFound(t *testing.T) {
	vh, first := rv.FindRVVarhashByRsid(0, 9, 0xfffffff0)
	if first != 10 {
		t.Errorf("Expected first 10, got %d", first)
	}
	if vh.Chrom != 0 {
		t.Errorf("Expected chrom 0, got %#08x", vh.Chrom)
	}
	if vh.Pos != 0 {
		t.Errorf("Expected pos0, got %#08x", vh.Pos)
	}
	if vh.RefAlt != 0 {
		t.Errorf("Expected refalt 0, got %#016x", vh.RefAlt)
	}
}

func BenchmarkFindRVVarhashByRsid(b *testing.B) {
	b.ResetTimer()
	for i := 0; i < b.N; i++ {
		rv.FindRVVarhashByRsid(0, 9, 0x2F81F575)
	}
}

func TestFindVRChromposRange(t *testing.T) {
	rsid, first, last := vr.FindVRChromposRange(0, 9, testDataVR[0].chrom, testDataVR[3].pos, testDataVR[7].pos)
	if rsid != testDataVR[3].rsid {
		t.Errorf("Expected rsid %#016x, got %#016x", testDataVR[3].rsid, rsid)
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
		t.Errorf("Expected rsid 0, got %#016x", rsid)
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
		vr.FindVRChromposRange(0, 9, 0x0000001A, 0x0000400E, 0x00004086)
	}
}

func TestFindVRRsidByVarshash(t *testing.T) {
	vh := TVariantHash{}
	for i, tt := range testDataVR {
		vh.Chrom = testDataVR[i].chrom
		vh.Pos = testDataVR[i].pos
		vh.RefAlt = testDataVR[i].refalt
		rsid, first := vr.FindVRRsidByVarshash(0, 9, vh)
		if rsid != tt.rsid {
			t.Errorf("%d. Expected %#016x, got %#016x", i, tt.rsid, rsid)
		}
		if first != uint64(i) {
			t.Errorf("%d. Expected first %d, got %d", i, i, first)
		}
	}
}

func TestFindVRRsidByVarshashNotFound(t *testing.T) {
	vh := TVariantHash{}
	vh.Chrom = 0xfffffff0
	vh.Pos = 0xfffffff0
	vh.RefAlt = 0xfffffffffffffff0
	rsid, first := vr.FindVRRsidByVarshash(0, 9, vh)
	if rsid != 0 {
		t.Errorf("Expected rsid 0, got %#016x", rsid)
	}
	if first != 10 {
		t.Errorf("Expected first 10, got %d", first)
	}
}

func BenchmarkFindVRRsidByVarshash(b *testing.B) {
	vh := TVariantHash{
		Chrom:  0x0000001A,
		Pos:    0x0000400E,
		RefAlt: 0x4EECA287F5C90712,
	}
	b.ResetTimer()
	for i := 0; i < b.N; i++ {
		vr.FindVRRsidByVarshash(0, 9, vh)
	}
}
