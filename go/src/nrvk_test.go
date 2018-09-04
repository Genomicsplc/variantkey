package variantkey

// nrvk_test.go
// @category   Libraries
// @author     Nicola Asuni <nicola.asuni@genomicsplc.com>
// @copyright  2017-2018 GENOMICS plc
// @license    MIT (see LICENSE)
// @link       https://github.com/genomicsplc/variantkey

import "testing"

var testNonRevVKData = []struct {
	vk            uint64
	chrom         string
	pos           uint32
	len           uint32
	sizeRef       uint8
	sizeAlt       uint8
	chromStartPos uint64
	chromEndPos   uint64
	ref           string
	alt           string
}{
	{0x0800c35093ace339, "1", 100001, 0x00000004, 0x01, 0x01, 0x00000000100186a1, 0x00000000100186a2, "N", "A"},
	{0x1000c3517f91cdb1, "2", 100002, 0x0000000e, 0x0b, 0x01, 0x00000000200186a2, 0x00000000200186ad, "AAGAAAGAAAG", "A"},
	{0x1800c351f61f65d3, "3", 100003, 0x0000000e, 0x01, 0x0b, 0x00000000300186a3, 0x00000000300186a4, "A", "AAGAAAGAAAG"},
	{0x2000c3521f1c15ab, "4", 100004, 0x0000000e, 0x08, 0x04, 0x00000000400186a4, 0x00000000400186ac, "ACGTACGT", "ACGT"},
	{0x2800c352d8f2d5b5, "5", 100005, 0x0000000e, 0x04, 0x08, 0x00000000500186a5, 0x00000000500186a9, "ACGT", "ACGTACGT"},
	{0x5000c3553bbf9c19, "10", 100010, 0x00000012, 0x08, 0x08, 0x00000000a00186aa, 0x00000000a00186b2, "ACGTACGT", "CGTACGTA"},
	{0xb000c35b64690b25, "22", 100022, 0x0000000b, 0x08, 0x01, 0x00000001600186b6, 0x00000001600186be, "ACGTACGT", "N"},
	{0xb800c35bbcece603, "X", 100023, 0x0000000e, 0x0a, 0x02, 0x00000001700186b7, 0x00000001700186c1, "AAAAAAAAGG", "AG"},
	{0xc000c35c63741ee7, "Y", 100024, 0x0000000e, 0x02, 0x0a, 0x00000001800186b8, 0x00000001800186ba, "AG", "AAAAAAAAGG"},
	{0xc800c35c96c18499, "MT", 100025, 0x00000012, 0x04, 0x0c, 0x00000001900186b9, 0x00000001900186bd, "ACGT", "AAACCCGGGTTT"},
}

func TestFindRefAltByVariantKey(t *testing.T) {
	for i, tt := range testNonRevVKData {
		i := i
		tt := tt
		t.Run("", func(t *testing.T) {
			t.Parallel()
			ref, alt, sizeRef, sizeAlt, len := nrvk.FindRefAltByVariantKey(tt.vk)
			if ref != tt.ref {
				t.Errorf("%d. Expected REF %s, got %s", i, tt.ref, ref)
			}
			if alt != tt.alt {
				t.Errorf("%d. Expected ALT %s, got %s", i, tt.alt, alt)
			}
			if sizeRef != tt.sizeRef {
				t.Errorf("%d. Expected REF size %d, got %d", i, tt.sizeRef, sizeRef)
			}
			if sizeAlt != tt.sizeAlt {
				t.Errorf("%d. Expected ALT size %d, got %d", i, tt.sizeAlt, sizeAlt)
			}
			if len != (tt.len - 2) {
				t.Errorf("%d. Expected len %d, got %d", i, (tt.len - 2), len)
			}
		})
	}
}

func BenchmarkFindRefAltByVariantKey(b *testing.B) {
	b.ResetTimer()
	for i := 0; i < b.N; i++ {
		nrvk.FindRefAltByVariantKey(0xb000c35b64690b25)
	}
}

func TestNRReverseVariantKey(t *testing.T) {
	for i, tt := range testNonRevVKData {
		i := i
		tt := tt
		t.Run("", func(t *testing.T) {
			t.Parallel()
			rev, len := nrvk.ReverseVariantKey(tt.vk)
			if rev.Chrom != tt.chrom {
				t.Errorf("%d. Expected CHROM %s, got %s", i, tt.chrom, rev.Chrom)
			}
			if rev.Pos != tt.pos {
				t.Errorf("%d. Expected POS size %d, got %d", i, tt.pos, rev.Pos)
			}
			if rev.Ref != tt.ref {
				t.Errorf("%d. Expected REF %s, got %s", i, tt.ref, rev.Ref)
			}
			if rev.Alt != tt.alt {
				t.Errorf("%d. Expected ALT %s, got %s", i, tt.alt, rev.Alt)
			}
			if rev.SizeRef != tt.sizeRef {
				t.Errorf("%d. Expected REF size %d, got %d", i, tt.sizeRef, rev.SizeRef)
			}
			if rev.SizeAlt != tt.sizeAlt {
				t.Errorf("%d. Expected ALT size %d, got %d", i, tt.sizeAlt, rev.SizeAlt)
			}
			if len != (tt.len - 2) {
				t.Errorf("%d. Expected len %d, got %d", i, (tt.len - 2), len)
			}
		})
	}
}

func BenchmarkNRReverseVariantKey(b *testing.B) {
	b.ResetTimer()
	for i := 0; i < b.N; i++ {
		nrvk.ReverseVariantKey(0xb000c35b64690b25)
	}
}

func TestGetVariantKeyRefLength(t *testing.T) {
	for i, tt := range testNonRevVKData {
		i := i
		tt := tt
		t.Run("", func(t *testing.T) {
			t.Parallel()
			sizeRef := nrvk.GetVariantKeyRefLength(tt.vk)
			if sizeRef != tt.sizeRef {
				t.Errorf("%d. Expected REF size %d, got %d", i, tt.sizeRef, sizeRef)
			}
		})
	}
}

func TestGetVariantKeyRefLengthReversible(t *testing.T) {
	sizeRef := nrvk.GetVariantKeyRefLength(0x1800925199160000)
	if sizeRef != 3 {
		t.Errorf("Expected REF size 3, got %d", sizeRef)
	}
}

func TestGetVariantKeyRefLengthNotFound(t *testing.T) {
	sizeRef := nrvk.GetVariantKeyRefLength(0xffffffffffffffff)
	if sizeRef != 0 {
		t.Errorf("Expected REF size 0, got %d", sizeRef)
	}
}

func TestGetVariantKeyEndPos(t *testing.T) {
	for i, tt := range testNonRevVKData {
		i := i
		tt := tt
		t.Run("", func(t *testing.T) {
			t.Parallel()
			endpos := nrvk.GetVariantKeyEndPos(tt.vk)
			exp := tt.pos + uint32(tt.sizeRef)
			if endpos != exp {
				t.Errorf("%d. Expected END POS %d, got %d", i, exp, endpos)
			}
		})
	}
}

func TestGetVariantKeyChromStartPos(t *testing.T) {
	for i, tt := range testNonRevVKData {
		i := i
		tt := tt
		t.Run("", func(t *testing.T) {
			t.Parallel()
			res := GetVariantKeyChromStartPos(tt.vk)
			if res != tt.chromStartPos {
				t.Errorf("%d. Expected CHROM + START POS %d, got %d", i, tt.chromStartPos, res)
			}
		})
	}
}

func TestGetVariantKeyChromEndPos(t *testing.T) {
	for i, tt := range testNonRevVKData {
		i := i
		tt := tt
		t.Run("", func(t *testing.T) {
			t.Parallel()
			res := nrvk.GetVariantKeyChromEndPos(tt.vk)
			if res != tt.chromEndPos {
				t.Errorf("%d. Expected CHROM + END POS %d, got %d", i, tt.chromEndPos, res)
			}
		})
	}
}

func TestVknrBinToTSV(t *testing.T) {
	len := nrvk.VknrBinToTSV("nrvk.test")
	if len != 305 {
		t.Errorf("Expected file length 305, got %d", len)
	}
}
