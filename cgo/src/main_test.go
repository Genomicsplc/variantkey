package variantkey

import "testing"
import "os"

var rv128, vr128, rv64, vr64 TMMFile

func TestMain(m *testing.M) {
	var err error

	// memory map the input files

	mf, err = MmapBinFile("../../test/data/test_data.bin")
	if err != nil {
		return
	}

	rv128, err = MmapBinFile("../../test/data/rsid_variantkey128.10.bin")
	if err != nil {
		return
	}

	vr128, err = MmapBinFile("../../test/data/variantkey128_rsid.10.bin")
	if err != nil {
		return
	}

	rv64, err = MmapBinFile("../../test/data/rsid_variantkey64.10.bin")
	if err != nil {
		return
	}

	vr64, err = MmapBinFile("../../test/data/variantkey64_rsid.10.bin")
	if err != nil {
		return
	}

	retCode := m.Run()

	// close the memory-mapped files

	err = mf.Close()
	if err != nil {
		retCode++
	}

	err = rv128.Close()
	if err != nil {
		retCode++
	}

	err = vr128.Close()
	if err != nil {
		retCode++
	}

	err = rv64.Close()
	if err != nil {
		retCode++
	}

	err = vr64.Close()
	if err != nil {
		retCode++
	}

	os.Exit(retCode)
}
