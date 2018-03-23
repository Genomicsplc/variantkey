package variantkey

import "testing"
import "os"

var rv, vr TMMFile

func TestMain(m *testing.M) {
	var err error

	// memory map the input files

	mf, err = MmapBinFile("../../test/data/test_data.bin")
	if err != nil {
		return
	}

	rv, err = MmapBinFile("../../test/data/rsid_variantkey.10.bin")
	if err != nil {
		return
	}

	vr, err = MmapBinFile("../../test/data/variantkey_rsid.10.bin")
	if err != nil {
		return
	}

	retCode := m.Run()

	// close the memory-mapped files

	err = mf.Close()
	if err != nil {
		retCode++
	}

	err = rv.Close()
	if err != nil {
		retCode++
	}

	err = vr.Close()
	if err != nil {
		retCode++
	}

	os.Exit(retCode)
}
