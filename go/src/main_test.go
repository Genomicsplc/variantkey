package variantkey

import "testing"
import "os"

var mf, rv, vr, vknr TMMFile

func TestMain(m *testing.M) {
	var err error

	// memory map the input files

	mf, err = MmapBinFile("../../c/test/data/test_data.bin")
	if err != nil {
		os.Exit(1)
	}

	rv, err = MmapBinFile("../../c/test/data/rsvk.10.bin")
	if err != nil {
		os.Exit(2)
	}

	vr, err = MmapBinFile("../../c/test/data/vkrs.10.bin")
	if err != nil {
		os.Exit(3)
	}

	vknr, err = MmapBinFile("../../c/test/data/vknr.10.bin")
	if err != nil {
		os.Exit(4)
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

	err = vknr.Close()
	if err != nil {
		retCode++
	}

	os.Exit(retCode)
}
