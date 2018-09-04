package variantkey

// rsidvar_test.go
// @category   Libraries
// @author     Nicola Asuni <nicola.asuni@genomicsplc.com>
// @copyright  2017-2018 GENOMICS plc
// @license    MIT (see LICENSE)
// @link       https://github.com/genomicsplc/variantkey

import "testing"

var testData = []struct {
	chrom  uint8
	pos    uint32
	refalt uint32
	rsid   uint32
	vk     uint64
}{
	{0x01, 0x0004F44B, 0x00338000, 0x00000001, 0x08027A2580338000},
	{0x09, 0x000143FC, 0x439E3918, 0x00000007, 0x4800A1FE439E3918},
	{0x09, 0x000143FC, 0x7555EB16, 0x0000000B, 0x4800A1FE7555EB16},
	{0x10, 0x000204E8, 0x003A0000, 0x00000061, 0x80010274003A0000},
	{0x10, 0x0002051A, 0x00138000, 0x00000065, 0x8001028D00138000},
	{0x10, 0x00020532, 0x007A0000, 0x000003E5, 0x80010299007A0000},
	{0x14, 0x000256C4, 0x003A0000, 0x000003F1, 0xA0012B62003A0000},
	{0x14, 0x000256C5, 0x00708000, 0x000026F5, 0xA0012B6280708000},
	{0x14, 0x000256CB, 0x63256692, 0x000186A3, 0xA0012B65E3256692},
	{0x14, 0x000256CF, 0x55439803, 0x00019919, 0xA0012B67D5439803},
}

func TestFindRVVariantKeyByRsid(t *testing.T) {
	for i, tt := range testData {
		i := i
		tt := tt
		t.Run("", func(t *testing.T) {
			t.Parallel()
			vk, first := rv.FindRVVariantKeyByRsid(0, rv.NRows, tt.rsid)
			if first != uint64(i) {
				t.Errorf("Expected first %d, got %d", i, first)
			}
			if vk != tt.vk {
				t.Errorf("Expected VariantKey %x, got %x", tt.vk, vk)
			}
		})
	}
}

func TestFindRVVariantKeyByRsidNotFound(t *testing.T) {
	vk, first := rv.FindRVVariantKeyByRsid(0, rv.NRows, 0xfffffff0)
	if first != 10 {
		t.Errorf("Expected first 10, got %d", first)
	}
	if vk != 0 {
		t.Errorf("ExpectedVariantKey 0, got %x", vk)
	}
}

func BenchmarkFindRVVariantKeyByRsid(b *testing.B) {
	b.ResetTimer()
	for i := 0; i < b.N; i++ {
		rv.FindRVVariantKeyByRsid(0, 9, 0x2F81F575)
	}
}

func TestGetNextRVVariantKeyByRsid(t *testing.T) {
	var vk uint64
	pos := uint64(2)
	vk, pos = rv.GetNextRVVariantKeyByRsid(pos, rv.NRows, 0x00000061)
	if pos != 3 {
		t.Errorf("(1) Expected pos 3, got %d", pos)
	}
	if vk != 0x80010274003A0000 {
		t.Errorf("(1) Expected VariantKey 0x80010274003A0000, got %x", vk)
	}
	vk, pos = rv.GetNextRVVariantKeyByRsid(pos, rv.NRows, 0x00000061)
	if pos != 4 {
		t.Errorf("(2) Expected pos 4, got %d", pos)
	}
	if vk != 0 {
		t.Errorf("(2) Expected VariantKey 0, got %x", vk)
	}
}

func TestFindAllRVVariantKeyByRsid(t *testing.T) {
	vks := rvm.FindAllRVVariantKeyByRsid(0, rvm.NRows, 0x00000003)
	if len(vks) != 3 {
		t.Errorf("Expected len 3, got %d", len(vks))
	}
	if vks[0] != 0x80010274003A0000 {
		t.Errorf("Expected VariantKey 0x80010274003A0000, got %x", vks[0])
	}
	if vks[1] != 0x8001028D00138000 {
		t.Errorf("Expected VariantKey 0x8001028D00138000, got %x", vks[1])
	}
	if vks[2] != 0x80010299007A0000 {
		t.Errorf("Expected VariantKey 0x80010299007A0000, got %x", vks[2])
	}
}

func TestFindAllRVVariantKeyByRsidNotFound(t *testing.T) {
	vks := rvm.FindAllRVVariantKeyByRsid(0, rvm.NRows, 0x12345678)
	if len(vks) != 0 {
		t.Errorf("Expected len 0, got %d", len(vks))
	}
}

func TestFindVRRsidByVariantKey(t *testing.T) {
	for i, tt := range testData {
		i := i
		tt := tt
		t.Run("", func(t *testing.T) {
			t.Parallel()
			rsid, first := vr.FindVRRsidByVariantKey(0, vr.NRows, tt.vk)
			if rsid != tt.rsid {
				t.Errorf("%d. Expected rsid %x, got %x", i, tt.rsid, rsid)
			}
			if first != uint64(i) {
				t.Errorf("%d. Expected first %d, got %d", i, i, first)
			}
		})
	}
}

func TestFindVRRsidByVariantKeyNotFound(t *testing.T) {
	rsid, first := vr.FindVRRsidByVariantKey(0, vr.NRows, 0xfffffffffffffff0)
	if rsid != 0 {
		t.Errorf("Expected rsid 0, got %x", rsid)
	}
	if first != 10 {
		t.Errorf("Expected first 10, got %d", first)
	}
}

func BenchmarkFindVRRsidByVariantKey(b *testing.B) {
	b.ResetTimer()
	for i := 0; i < b.N; i++ {
		vr.FindVRRsidByVariantKey(0, vr.NRows, 0x160017CCA313D0E0)
	}
}

func TestFindVRChromPosRange(t *testing.T) {
	rsid, first, last := vr.FindVRChromPosRange(0, 9, testData[6].chrom, testData[7].pos, testData[8].pos)
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

func TestFindVRChromPosRangeNotFound(t *testing.T) {
	rsid, first, last := vr.FindVRChromPosRange(0, vr.NRows, 0xff, 0xffffff00, 0xfffffff0)
	if rsid != 0 {
		t.Errorf("Expected rsid 0, got %x", rsid)
	}
	if first != 10 {
		t.Errorf("Expected first 10, got %d", first)
	}
	if last != 10 {
		t.Errorf("Expected last 9, got %d", last)
	}
}

func BenchmarkFindVRChromPosRange(b *testing.B) {
	b.ResetTimer()
	for i := 0; i < b.N; i++ {
		vr.FindVRChromPosRange(0, vr.NRows, 0x19, 0x001AF8FD, 0x001C8F2A)
	}
}
