package variantkey

// nrvk_test.go
// @category   Libraries
// @author     Nicola Asuni <nicola.asuni@genomicsplc.com>
// @copyright  2017-2018 GENOMICS plc
// @license    MIT (see LICENSE)
// @link       https://github.com/genomicsplc/variantkey

import "testing"

var testNonRevVKData = []struct {
	vk      uint64
	chrom   string
	pos     uint32
	len     uint32
	sizeref uint8
	sizealt uint8
	ref     string
	alt     string
}{
	{0x0800c35093ace339, "1", 100001, 0x00000004, 0x01, 0x01, "N", "A"},
	{0x1000c3517f91cdb1, "2", 100002, 0x0000000e, 0x0b, 0x01, "AAGAAAGAAAG", "A"},
	{0x1800c351f61f65d3, "3", 100003, 0x0000000e, 0x01, 0x0b, "A", "AAGAAAGAAAG"},
	{0x2000c3521f1c15ab, "4", 100004, 0x0000000e, 0x08, 0x04, "ACGTACGT", "ACGT"},
	{0x2800c352d8f2d5b5, "5", 100005, 0x0000000e, 0x04, 0x08, "ACGT", "ACGTACGT"},
	{0x5000c3553bbf9c19, "10", 100010, 0x00000012, 0x08, 0x08, "ACGTACGT", "CGTACGTA"},
	{0xb000c35b64690b25, "22", 100022, 0x0000000b, 0x08, 0x01, "ACGTACGT", "N"},
	{0xb800c35bbcece603, "X", 100023, 0x0000000e, 0x0a, 0x02, "AAAAAAAAGG", "AG"},
	{0xc000c35c63741ee7, "Y", 100024, 0x0000000e, 0x02, 0x0a, "AG", "AAAAAAAAGG"},
	{0xc800c35c96c18499, "MT", 100025, 0x00000012, 0x04, 0x0c, "ACGT", "AAACCCGGGTTT"},
}

func TestFindRefAltByVariantkey(t *testing.T) {
	for i, tt := range testNonRevVKData {
		i := i
		tt := tt
		t.Run("", func(t *testing.T) {
			t.Parallel()
			ref, alt, sizeref, sizealt, len := vknr.FindRefAltByVariantkey(tt.vk)
			if ref != tt.ref {
				t.Errorf("%d. Expected ref %s, got %s", i, tt.ref, ref)
			}
			if alt != tt.alt {
				t.Errorf("%d. Expected alt %s, got %s", i, tt.alt, alt)
			}
			if sizeref != tt.sizeref {
				t.Errorf("%d. Expected ref size %d, got %d", i, tt.sizeref, sizeref)
			}
			if sizealt != tt.sizealt {
				t.Errorf("%d. Expected alt size %d, got %d", i, tt.sizealt, sizealt)
			}
			if len != (tt.len - 2) {
				t.Errorf("%d. Expected len %d, got %d", i, (tt.len - 2), len)
			}
		})
	}
}

func BenchmarkFindRefAltByVariantkey(b *testing.B) {
	b.ResetTimer()
	for i := 0; i < b.N; i++ {
		vknr.FindRefAltByVariantkey(0xb000c35b64690b25)
	}
}

func TestReverseVariantkey(t *testing.T) {
	for i, tt := range testNonRevVKData {
		i := i
		tt := tt
		t.Run("", func(t *testing.T) {
			t.Parallel()
			rev, len := vknr.ReverseVariantkey(tt.vk)
			if rev.Chrom != tt.chrom {
				t.Errorf("%d. Expected chrom %s, got %s", i, tt.chrom, rev.Chrom)
			}
			if rev.Pos != tt.pos {
				t.Errorf("%d. Expected pos size %d, got %d", i, tt.pos, rev.Pos)
			}
			if rev.Ref != tt.ref {
				t.Errorf("%d. Expected ref %s, got %s", i, tt.ref, rev.Ref)
			}
			if rev.Alt != tt.alt {
				t.Errorf("%d. Expected alt %s, got %s", i, tt.alt, rev.Alt)
			}
			if rev.SizeRef != tt.sizeref {
				t.Errorf("%d. Expected ref size %d, got %d", i, tt.sizeref, rev.SizeRef)
			}
			if rev.SizeAlt != tt.sizealt {
				t.Errorf("%d. Expected alt size %d, got %d", i, tt.sizealt, rev.SizeAlt)
			}
			if len != (tt.len - 2) {
				t.Errorf("%d. Expected len %d, got %d", i, (tt.len - 2), len)
			}
		})
	}
}

func BenchmarkReverseVariantkey(b *testing.B) {
	b.ResetTimer()
	for i := 0; i < b.N; i++ {
		vknr.ReverseVariantkey(0xb000c35b64690b25)
	}
}

func TestVknrBinToTSV(t *testing.T) {
	len := vknr.VknrBinToTSV("vknr.test")
	if len != 305 {
		t.Errorf("Expected file length 305, got %d", len)
	}
}
