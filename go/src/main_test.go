package variantkey

// main_test.go
// @category   Libraries
// @author     Nicola Asuni <nicola.asuni@genomicsplc.com>
// @copyright  2017-2018 GENOMICS plc
// @license    MIT (see LICENSE)
// @link       https://github.com/genomicsplc/variantkey

import "testing"
import "os"

var gref, rvmf, rvmmf, vrmf, nrvkmf TMMFile
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
	lmf, err := MmapGenorefFile("../../c/test/data/test_data.bin")
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
