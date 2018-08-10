package variantkey

// esid_test.go
// @category   Libraries
// @author     Nicola Asuni <nicola.asuni@genomicsplc.com>
// @copyright  2017-2018 GENOMICS plc
// @license    MIT (see LICENSE)
// @link       https://github.com/genomicsplc/variantkey

import "testing"

type TEsidData struct {
	str   string
	size  uint32
	start uint32
	esid  uint64
	estr  string
	esize uint32
	hsid  uint64
}

var esidTestData = []TEsidData{
	{"0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ", 36, 0, 0x0411493515597619, "0123456789", 10, 0xb3a5fdb8808cb7dc},
	{"0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ", 36, 1, 0x04524d45565d8661, "123456789A", 10, 0xb3a5fdb8808cb7dc},
	{"0223456789ABCDEFGHIJKLMNOPQRSTUVWXYZ", 36, 10, 0x08628e49669e8a6a, "ABCDEFGHIJ", 10, 0xd93e382010f46f32},
	{"0133456789ABCDEFGHIJKLMNOPQRSTUVWXYZ", 36, 25, 0x0c31cb3d35db7e39, "PQRSTUVWXY", 10, 0xbfc379f4a71cb3be},
	{"1123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ", 36, 26, 0x0c72cf4d76df8e7a, "QRSTUVWXYZ", 10, 0xdba299d06b54215d},
	{"0123456789ABCDEFGHIJKLMNOPQRSTUVWXY[", 36, 35, 0x0ec0000000000000, "[", 1, 0x996dcdc8ef7c89c1},
	{"012345", 6, 0, 0x0411493515000000, "012345", 6, 0xa81279395d9c891e},
	{"012345", 6, 1, 0x04524d4540000000, "12345", 5, 0xa81279395d9c891e},
	{"012345", 6, 3, 0x04d4540000000000, "345", 3, 0xa81279395d9c891e},
	{"012345", 6, 5, 0x0540000000000000, "5", 1, 0xa81279395d9c891e},
	{" !\"#$%&'()", 10, 0, 0x0fc1083105187209, "_!\"#$%&'()", 10, 0x838c148028a06998},
	{"*+,-./0123", 10, 0, 0x028b30d38f411493, "*+,-./0123", 10, 0x96e42cf7aefeccf0},
	{"456789:;<=", 10, 0, 0x051559761969b71d, "456789:;<=", 10, 0xb391c7f288e446ee},
	{">?@ABCDEFG", 10, 0, 0x079f8218a39259a7, ">?@ABCDEFG", 10, 0xcc0de20381405e0a},
	{"HIJKLMNOPQ", 10, 0, 0x0a29aabb2dbafc31, "HIJKLMNOPQ", 10, 0xbbfc630eacb6c552},
	{"RSTUVWXYZ[", 10, 0, 0x0cb3d35db7e39ebb, "RSTUVWXYZ[", 10, 0x803eda0a1781a117},
	{"\\]^_`abcde", 10, 0, 0x0f3dfbf8218a3925, "\\]^_@ABCDE", 10, 0x878ef2947ee3cc2b},
	{"fghijklmno", 10, 0, 0x09a7a29aabb2dbaf, "FGHIJKLMNO", 10, 0xbaadaef07826969b},
	{"pqrstuvwxy", 10, 0, 0x0c31cb3d35db7e39, "PQRSTUVWXY", 10, 0xd3da4d5e28be3590},
	{"z{|}~\t", 6, 0, 0x0ebbf3dfbf000000, "Z[\\]^_", 6, 0xeadc752a50c5b850},
	{"123456781234567", 15, 0, 0x04524d45565d8452, "1234567812", 10, 0x9af6ee553ba41827},
	{"12345678123456", 14, 0, 0x04524d45565d8452, "1234567812", 10, 0xc94e10e9fe153fd2},
	{"1234567812345", 13, 0, 0x04524d45565d8452, "1234567812", 10, 0x9b56411c7abdefe6},
	{"123456781234", 12, 0, 0x04524d45565d8452, "1234567812", 10, 0xcbec35944ff1c863},
	{"12345678123", 11, 0, 0x04524d45565d8452, "1234567812", 10, 0x929920298cf56b9b},
	{"1234567812", 10, 0, 0x04524d45565d8452, "1234567812", 10, 0xbdf006d50a33ad90},
	{"123456781", 9, 0, 0x04524d45565d8440, "123456781", 9, 0xde094182e93557ae},
	{"12345678", 8, 0, 0x04524d45565d8000, "12345678", 8, 0xccbc926a73ece95c},
	{"1234567", 7, 0, 0x04524d45565c0000, "1234567", 7, 0xd18b960e2f99c279},
	{"123456", 6, 0, 0x04524d4556000000, "123456", 6, 0x811c9c02fcc22096},
	{"12345", 5, 0, 0x04524d4540000000, "12345", 5, 0xd863467dedd1cab1},
	{"1234", 4, 0, 0x04524d4000000000, "1234", 4, 0xbef404ecb71e4cd9},
	{"123", 3, 0, 0x04524c0000000000, "123", 3, 0xfad470644116fa54},
	{"12", 2, 0, 0x0452000000000000, "12", 2, 0xd94ae1e7173e781d},
	{"1", 1, 0, 0x0440000000000000, "1", 1, 0xfc73100baa96ad81},
	{"", 0, 0, 0x0000000000000000, "", 0, 0x8000000000000000},
}

func TestEncodeStringID(t *testing.T) {
	for _, v := range esidTestData {
		v := v
		t.Run("", func(t *testing.T) {
			t.Parallel()
			esid := EncodeStringID(v.str, v.start)
			if esid != v.esid {
				t.Errorf("The code value is different, expected %#v got %#v", v.esid, esid)
			}
		})
	}
}

func BenchmarkEncodeStringID(b *testing.B) {
	b.ResetTimer()
	for i := 0; i < b.N; i++ {
		EncodeStringID("ABC0123456", 0)
	}
}

func TestDecodeStringID(t *testing.T) {
	for _, v := range esidTestData {
		v := v
		t.Run("", func(t *testing.T) {
			t.Parallel()
			estr := DecodeStringID(v.esid)
			if estr != v.estr {
				t.Errorf("The code value is different, expected %#v got %#v", v.estr, estr)
			}
		})
	}
}

func BenchmarkDecodeStringID(b *testing.B) {
	b.ResetTimer()
	for i := 0; i < b.N; i++ {
		DecodeStringID(0x08628e49669e8a6a)
	}
}

func TestDecodeStringIDHash(t *testing.T) {
	for _, v := range esidTestData {
		v := v
		t.Run("", func(t *testing.T) {
			t.Parallel()
			estr := DecodeStringID(v.hsid)
			if estr != "" {
				t.Errorf("The code value is different, expected empty string got %#v", estr)
			}
		})
	}
}

func TestHashStringID(t *testing.T) {
	for _, v := range esidTestData {
		v := v
		t.Run("", func(t *testing.T) {
			t.Parallel()
			hsid := HashStringID(v.str)
			if hsid != v.hsid {
				t.Errorf("The code value is different, expected %#v got %#v", v.hsid, hsid)
			}
		})
	}
}

func BenchmarkHashStringID(b *testing.B) {
	b.ResetTimer()
	for i := 0; i < b.N; i++ {
		HashStringID("0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ")
	}
}
