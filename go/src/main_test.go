package varianthash

import "testing"
import "os"

func TestMain(m *testing.M) {
	var err error

	// memory map the input files

	mf, err = MmapBinFile("../../test/test_data.bin")
	if err != nil {
		return
	}

	rv, err = MmapBinFile("../../test/rsid_varhash.10.bin")
	if err != nil {
		return
	}

	vr, err = MmapBinFile("../../test/varhash_rsid.10.bin")
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
