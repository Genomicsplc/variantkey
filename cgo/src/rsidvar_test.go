package variantkey

import "testing"

var testData = []struct {
	rsid   uint32
	vk     uint64
	chrom  uint8
	pos    uint32
	refalt uint32
}{
	{0X00000001, 0X08027A2580338000, 0X01, 0X0004F44B, 0X00338000},
	{0X00000007, 0X4800A1FE439E3918, 0X09, 0X000143FC, 0X439E3918},
	{0X0000000B, 0X4800A1FE7555EB16, 0X09, 0X000143FC, 0X7555EB16},
	{0X00000061, 0X80010274003A0000, 0X10, 0X000204E8, 0X003A0000},
	{0X00000065, 0X8001028D00138000, 0X10, 0X0002051A, 0X00138000},
	{0X000003E5, 0X80010299007A0000, 0X10, 0X00020532, 0X007A0000},
	{0X000003F1, 0XA0012B62003A0000, 0X14, 0X000256C4, 0X003A0000},
	{0X000026F5, 0XA0012B6280708000, 0X14, 0X000256C5, 0X00708000},
	{0X000186A3, 0XA0012B65E3256692, 0X14, 0X000256CB, 0X63256692},
	{0X00019919, 0XA0012B67D5439803, 0X14, 0X000256CF, 0X55439803},
}

func TestGetVRRsid(t *testing.T) {
	for i, tt := range testData {
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

func TestGetRVVariantkey(t *testing.T) {
	for i, tt := range testData {
		i := i
		tt := tt
		t.Run("", func(t *testing.T) {
			t.Parallel()
			vk := rv.GetRVVariantkey(uint64(i))
			if vk != tt.vk {
				t.Errorf("Expected VariantKey %x, got %x", tt.vk, vk)
			}
		})
	}
}

func BenchmarkGetRVVariantkey(b *testing.B) {
	b.ResetTimer()
	for i := 0; i < b.N; i++ {
		rv.GetRVVariantkey(3)
	}
}

func TestFindRVVariantkeyByRsid(t *testing.T) {
	for i, tt := range testData {
		i := i
		tt := tt
		t.Run("", func(t *testing.T) {
			t.Parallel()
			vk, first := rv.FindRVVariantkeyByRsid(0, 9, tt.rsid)
			if first != uint64(i) {
				t.Errorf("Expected first %d, got %d", i, first)
			}
			if vk != tt.vk {
				t.Errorf("Expected VariantKey %x, got %x", tt.vk, vk)
			}
		})
	}
}

func TestFindRVVariantkeyByRsidNotFound(t *testing.T) {
	vk, first := rv.FindRVVariantkeyByRsid(0, 9, 0xfffffff0)
	if first != 10 {
		t.Errorf("Expected first 10, got %d", first)
	}
	if vk != 0 {
		t.Errorf("ExpectedVariantKey 0, got %x", vk)
	}
}

func BenchmarkFindRVVariantkeyByRsid(b *testing.B) {
	b.ResetTimer()
	for i := 0; i < b.N; i++ {
		rv.FindRVVariantkeyByRsid(0, 9, 0x2F81F575)
	}
}

func TestFindVRRsidByVariantkey(t *testing.T) {
	for i, tt := range testData {
		i := i
		tt := tt
		t.Run("", func(t *testing.T) {
			t.Parallel()
			rsid, first := vr.FindVRRsidByVariantkey(0, 9, tt.vk)
			if rsid != tt.rsid {
				t.Errorf("%d. Expected rsid %x, got %x", i, tt.rsid, rsid)
			}
			if first != uint64(i) {
				t.Errorf("%d. Expected first %d, got %d", i, i, first)
			}
		})
	}
}

func TestFindVRRsidByVariantkeyNotFound(t *testing.T) {
	rsid, first := vr.FindVRRsidByVariantkey(0, 9, 0xfffffffffffffff0)
	if rsid != 0 {
		t.Errorf("Expected rsid 0, got %x", rsid)
	}
	if first != 10 {
		t.Errorf("Expected first 10, got %d", first)
	}
}

func BenchmarkFindVRRsidByVariantkey(b *testing.B) {
	b.ResetTimer()
	for i := 0; i < b.N; i++ {
		vr.FindVRRsidByVariantkey(0, 9, 0X160017CCA313D0E0)
	}
}

func TestFindVRChromposRange(t *testing.T) {
	rsid, first, last := vr.FindVRChromposRange(0, 9, testData[6].chrom, testData[7].pos, testData[8].pos)
	if rsid != testData[7].rsid {
		t.Errorf("Expected rsid %x, got %x", testData[7].rsid, rsid)
	}
	if first != 7 {
		t.Errorf("Expected first 4, got %d", first)
	}
	if last != 8 {
		t.Errorf("Expected last 4, got %d", last)
	}
}

func TestFindVRChromposRangeNotFound(t *testing.T) {
	rsid, first, last := vr.FindVRChromposRange(0, 9, 0xff, 0xffffff00, 0xfffffff0)
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
		vr.FindVRChromposRange(0, 9, 0x19, 0x001AF8FD, 0x001C8F2A)
	}
}
