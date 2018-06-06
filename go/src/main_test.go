package variantkey

import "testing"
import "os"

var mf, rv, vr, vknr, gref TMMFile
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

	mf, err = MmapBinFile("../../c/test/data/test_data.bin")
	if err != nil {
		os.Exit(1)
	}
	defer closeTMMFile(mf)

	rv, err = MmapBinFile("../../c/test/data/rsvk.10.bin")
	if err != nil {
		os.Exit(2)
	}
	defer closeTMMFile(rv)

	vr, err = MmapBinFile("../../c/test/data/vkrs.10.bin")
	if err != nil {
		os.Exit(3)
	}
	defer closeTMMFile(vr)

	vknr, err = MmapBinFile("../../c/test/data/vknr.10.bin")
	if err != nil {
		os.Exit(4)
	}
	defer closeTMMFile(vknr)

	gref, err = MmapBinFile("../../c/test/data/genoref.bin")
	if err != nil {
		os.Exit(5)
	}
	defer closeTMMFile(gref)
	gref.LoadGenorefIndex()

	retCode += m.Run()

	os.Exit(retCode)
}
