package varianthash

const (
	rvBinBlkLen = 20 // length of a binary block containing rsid + varhash

	rvBposRsid  = 0  // rv offset of rs id
	rvBposChrom = 4  // rv offset of chrom
	rvBposPos   = 8  // rv offset of pos
	rvBposRah   = 12 // rv offset of ref_alt_hash

	vrBposChrom = 0 // vr offset of chrom
	//vrBposPos   = 4  // vr offset of pos
	//vrBposRah   = 8  // vr offset of ref_alt_hash
	vrBposRsid = 16 // vr offset of rs id
)

// GetVRRsid returns the RSID at the specified position of the varhash_rsid.bin file.
func (mf TMMFile) GetVRRsid(item uint64) uint32 {
	return mf.BytesToUint32be(int(GetAddress(rvBinBlkLen, vrBposRsid, item)))
}

// GetRVVarhash returns the VariantHash at the specified position of the rsid_varhash.bin file.
func (mf TMMFile) GetRVVarhash(item uint64) TVariantHash {
	i := GetAddress(rvBinBlkLen, 0, item)
	return TVariantHash{
		Chrom:  mf.BytesToUint32be(int(i + rvBposChrom)),
		Pos:    mf.BytesToUint32be(int(i + rvBposPos)),
		RefAlt: mf.BytesToUint64be(int(i + rvBposRah)),
	}
}

// FindRVVarhashByRsid search for the specified RSID and returns the first occurrence of VariantHash, item position.
func (mf TMMFile) FindRVVarhashByRsid(first, last uint64, rsid uint32) (TVariantHash, uint64) {
	firstItem := mf.FindFirstUint32be(rvBinBlkLen, rvBposRsid, first, last, rsid)
	if firstItem > last {
		return TVariantHash{}, firstItem
	}
	return mf.GetRVVarhash(firstItem), firstItem
}

// FindVRChromposRange search for the specified CHROM-POS range and returns the first occurrence of RSID, item position, last position.
func (mf TMMFile) FindVRChromposRange(first, last uint64, chrom, posStart, posEnd uint32) (uint32, uint64, uint64) {
	firstItem := mf.FindFirstUint64be(rvBinBlkLen, vrBposChrom, first, last, (uint64(chrom)<<32 | uint64(posStart)))
	if firstItem > last {
		return 0, firstItem, last
	}
	lastItem := mf.FindLastUint64be(rvBinBlkLen, vrBposChrom, first, last, (uint64(chrom)<<32 | uint64(posEnd)))
	return mf.GetVRRsid(firstItem), firstItem, lastItem
}

// FindVRRsidByVarshash search for the specified VariantHash and returns the first occurrence of RSID, item position.
func (mf TMMFile) FindVRRsidByVarshash(first uint64, last uint64, vh TVariantHash) (uint32, uint64) {
	firstItem := mf.FindFirstUint128be(rvBinBlkLen, vrBposChrom, first, last, (uint64(vh.Chrom)<<32 | uint64(vh.Pos)), vh.RefAlt)
	if firstItem > last {
		return 0, firstItem
	}
	return mf.GetVRRsid(firstItem), firstItem
}
