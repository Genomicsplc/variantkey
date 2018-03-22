package variantkey

const (
	BinBlkLen = 12 // Length in bytes of a binary block containing RSID + VARIANTKEY
	rvPosRSID = 0  // RSIDVAR byte offset of RSID
	rvPosVK   = 4  // RSIDVAR byte offset of VARIANTKEY
	vrPosVK   = 0  // VARRSID byte offset of VARIANTKEY
	vrPosRSID = 8  // VARRSID byte offset of RSID
)

// GetVRRsid returns the RSID at the specified position in the VR file.
func (mf TMMFile) GetVRRsid(item uint64) uint32 {
	return mf.BytesToUint32(int(GetAddress(BinBlkLen, vrPosRSID, item)), 0, 31)
}

// GetRVVariantkey returns the VariantKey at the specified position in the RV file.
func (mf TMMFile) GetRVVariantkey(item uint64) uint64 {
	i := GetAddress(BinBlkLen, 0, item)
	return mf.BytesToUint64(int(i+rvPosVK), 0, 63)
}

// FindRVVariantkeyByRsid search for the specified RSID and returns the first occurrence of VariantKey in the RV file.
func (mf TMMFile) FindRVVariantkeyByRsid(first, last uint64, rsid uint32) (uint64, uint64) {
	firstItem, _, _ := mf.FindFirstUint32(BinBlkLen, rvPosRSID, 0, 31, first, last, rsid)
	if firstItem > last {
		return 0, firstItem
	}
	return mf.GetRVVariantkey(firstItem), firstItem
}

// FindVRRsidByVarshash search for the specified VariantKey and returns the first occurrence of RSID in the VR file.
func (mf TMMFile) FindVRRsidByVariantkey(first uint64, last uint64, vk uint64) (uint32, uint64) {
	firstItem, _, _ := mf.FindFirstUint64(BinBlkLen, vrPosVK, 0, 63, first, last, vk)
	if firstItem > last {
		return 0, firstItem
	}
	return mf.GetVRRsid(firstItem), firstItem
}

// FindVRChromPosRange search for the specified CHROM-POS range and returns the first occurrence of RSID in the VR file.
func (mf TMMFile) FindVRChromPosRange(first, last uint64, chrom uint8, posStart, posEnd uint32) (uint32, uint64, uint64) {
	ckey := (uint64(chrom) << 59)
	firstItem, f, _ := mf.FindFirstUint64(BinBlkLen, vrPosVK, 0, 32, first, last, (ckey|(uint64(posStart)<<31))>>31)
	if firstItem > last {
		firstItem = f
	}
	if f > last {
		return 0, firstItem, last
	}
	lastItem, _, _ := mf.FindLastUint64(BinBlkLen, vrPosVK, 0, 32, firstItem, last, (ckey|(uint64(posEnd)<<31))>>31)
	return mf.GetVRRsid(firstItem), firstItem, lastItem
}
