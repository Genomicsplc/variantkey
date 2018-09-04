package variantkey

// main_test.go
// @category   Libraries
// @author     Nicola Asuni <nicola.asuni@genomicsplc.com>
// @copyright  2017-2018 GENOMICS plc
// @license    MIT (see LICENSE)
// @link       https://github.com/genomicsplc/variantkey

import "testing"
import "os"

var mf, cmf, gref, rvmf, rvmmf, vrmf, nrvkmf TMMFile
var rv, rvm, vr RSIDVARCols
var nrvk NRVKCols

var retCode int

func closeTMMFile(mmf TMMFile) {
	err := mmf.Close()
	if err != nil {
		retCode++
	}
}

func TestMain(m *testing.M) {
	var err error

	// memory map the input files

	mf, err = MmapBinFile("../../c/test/data/test_data.bin", []uint8{12})
	if err != nil {
		os.Exit(1)
	}
	defer closeTMMFile(mf)

	cmf, err = MmapBinFile("../../c/test/data/test_data_col.bin", []uint8{1, 2, 4, 8})
	if err != nil {
		os.Exit(1)
	}
	defer closeTMMFile(cmf)

	rvmf, rv, err = MmapRSVKFile("../../c/test/data/rsvk.10.bin", []uint8{})
	if err != nil {
		os.Exit(2)
	}
	defer closeTMMFile(rvmf)

	rvmmf, rvm, err = MmapRSVKFile("../../c/test/data/rsvk.m.10.bin", []uint8{})
	if err != nil {
		os.Exit(2)
	}
	defer closeTMMFile(rvmmf)

	vrmf, vr, err = MmapVKRSFile("../../c/test/data/vkrs.10.bin", []uint8{})
	if err != nil {
		os.Exit(3)
	}
	defer closeTMMFile(vrmf)

	nrvkmf, nrvk, err = MmapNRVKFile("../../c/test/data/nrvk.10.bin")
	if err != nil {
		os.Exit(4)
	}
	defer closeTMMFile(nrvkmf)

	gref, err = MmapGenorefFile("../../c/test/data/genoref.bin")
	if err != nil {
		os.Exit(5)
	}
	defer closeTMMFile(gref)

	retCode += m.Run()

	os.Exit(retCode)
}

func TestClose(t *testing.T) {
	lmf, err := MmapBinFile("../../c/test/data/test_data.bin", []uint8{12})
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
	_, err := MmapBinFile("error", []uint8{1})
	if err == nil {
		t.Errorf("An error was expected")
	}
}

func TestMmapRSVKFileError(t *testing.T) {
	_, _, err := MmapRSVKFile("error", []uint8{1})
	if err == nil {
		t.Errorf("An error was expected")
	}
}

func TestMmapVKRSFileError(t *testing.T) {
	_, _, err := MmapVKRSFile("error", []uint8{1})
	if err == nil {
		t.Errorf("An error was expected")
	}
}

func TestMmapNRVKFileError(t *testing.T) {
	_, _, err := MmapNRVKFile("error")
	if err == nil {
		t.Errorf("An error was expected")
	}
}

func TestMmapGenorefFileError(t *testing.T) {
	_, err := MmapGenorefFile("error")
	if err == nil {
		t.Errorf("An error was expected")
	}
}

func TestGetAddress(t *testing.T) {
	h := GetAddress(3, 5, 7)
	if h != 26 {
		t.Errorf("Expected 26, got %d", h)
	}
}

func TestLoadArrow(t *testing.T) {
	ctbytes := []uint8{4, 8}
	amf, err := MmapBinFile("../../c/test/data/test_data_arrow.bin", ctbytes)
	if err != nil {
		t.Errorf("Unexpected error: %v", err)
	}
	if amf.Fd < 0 {
		t.Errorf("can't open test_data_arrow.bin for reading")
	}
	if amf.Size != 730 {
		t.Errorf("Size : Expecting 730 bytes, got instead: %d", amf.Size)
	}
	if amf.DOffset != 376 {
		t.Errorf("DOffset : Expecting 376 bytes, got instead: %d", amf.DOffset)
	}
	if amf.DLength != 136 {
		t.Errorf("DLength : Expecting 136 bytes, got instead: %d", amf.DLength)
	}
	if amf.NRows != 11 {
		t.Errorf("mf.NRows : Expecting 11 items, got instead: %d", amf.NRows)
	}
	if amf.NCols != 2 {
		t.Errorf("mf.NCols : Expecting 2 items, got instead: %d", amf.NCols)
	}
	if amf.Index[0] != 376 {
		t.Errorf("mf.Index[0] : Expecting 376 bytes, got instead: %d", amf.Index[0])
	}
	if amf.Index[1] != 424 {
		t.Errorf("mf.Index[1] : Expecting 424 bytes, got instead: %d", amf.Index[1])
	}
	err = amf.Close()
	if err != nil {
		t.Errorf("Unexpected error: %v", err)
	}
}

func TestLoadFeather(t *testing.T) {
	ctbytes := []uint8{4, 8}
	amf, err := MmapBinFile("../../c/test/data/test_data_feather.bin", ctbytes)
	if err != nil {
		t.Errorf("Unexpected error: %v", err)
	}
	if amf.Fd < 0 {
		t.Errorf("can't open test_data_arrow.bin for reading")
	}
	if amf.Size != 384 {
		t.Errorf("Size : Expecting 384 bytes, got instead: %d", amf.Size)
	}
	if amf.DOffset != 8 {
		t.Errorf("DOffset : Expecting 8 bytes, got instead: %d", amf.DOffset)
	}
	if amf.DLength != 136 {
		t.Errorf("DLength : Expecting 136 bytes, got instead: %d", amf.DLength)
	}
	if amf.NRows != 11 {
		t.Errorf("mf.NRows : Expecting 11 items, got instead: %d", amf.NRows)
	}
	if amf.NCols != 2 {
		t.Errorf("mf.NCols : Expecting 2 items, got instead: %d", amf.NCols)
	}
	if amf.Index[0] != 8 {
		t.Errorf("mf.Index[0] : Expecting 8 bytes, got instead: %d", amf.Index[0])
	}
	if amf.Index[1] != 56 {
		t.Errorf("mf.Index[1] : Expecting 56 bytes, got instead: %d", amf.Index[1])
	}
	err = amf.Close()
	if err != nil {
		t.Errorf("Unexpected error: %v", err)
	}
}

func TestLoadBinsrc(t *testing.T) {
	ctbytes := []uint8{4, 8}
	amf, err := MmapBinFile("../../c/test/data/test_data_binsrc.bin", ctbytes)
	if err != nil {
		t.Errorf("Unexpected error: %v", err)
	}
	if amf.Fd < 0 {
		t.Errorf("can't open test_data_arrow.bin for reading")
	}
	if amf.Size != 176 {
		t.Errorf("Size : Expecting 176 bytes, got instead: %d", amf.Size)
	}
	if amf.DOffset != 40 {
		t.Errorf("DOffset : Expecting 40 bytes, got instead: %d", amf.DOffset)
	}
	if amf.DLength != 136 {
		t.Errorf("DLength : Expecting 136 bytes, got instead: %d", amf.DLength)
	}
	if amf.NRows != 11 {
		t.Errorf("mf.NRows : Expecting 11 items, got instead: %d", amf.NRows)
	}
	if amf.NCols != 2 {
		t.Errorf("mf.NCols : Expecting 2 items, got instead: %d", amf.NCols)
	}
	if amf.Index[0] != 40 {
		t.Errorf("mf.Index[0] : Expecting 40 bytes, got instead: %d", amf.Index[0])
	}
	if amf.Index[1] != 88 {
		t.Errorf("mf.Index[1] : Expecting 88 bytes, got instead: %d", amf.Index[1])
	}
	err = amf.Close()
	if err != nil {
		t.Errorf("Unexpected error: %v", err)
	}
}
