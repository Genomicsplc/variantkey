package varianthash

const (
	rvBinBlkLen = 20 // length of a binary block containing rsid + varhash

	rvBposRsid     = 0  // rv offset of rs id
	rvBposAssembly = 4  // rv offset of genome assembly hash
	rvBposChrom    = 8  // rv offset of chrom
	rvBposPos      = 12 // rv offset of pos
	rvBposRefAlt   = 16 // rv offset of ref_alt hash

	vrBposAssembly = 0 // rv offset of genome assembly hash
	vrBposChrom    = 4 // vr offset of chrom
	//vrBposPos   = 8  // vr offset of pos
	//vrBposRefAlt   = 12  // vr offset of ref_alt hash
	vrBposRsid = 16 // vr offset of rs id
)

// GetVRRsid returns the RSID at the specified position of the varhash_rsid.bin file.
func (mf TMMFile) GetVRRsid(item uint64) uint32 {
	return mf.BytesToUint32(int(GetAddress(rvBinBlkLen, vrBposRsid, item)))
}

// GetRVVarhash returns the VariantHash at the specified position of the rsid_varhash.bin file.
func (mf TMMFile) GetRVVarhash(item uint64) TVariantHash {
	i := GetAddress(rvBinBlkLen, 0, item)
	return TVariantHash{
		Assembly: mf.BytesToUint32(int(i + rvBposAssembly)),
		Chrom:    mf.BytesToUint32(int(i + rvBposChrom)),
		Pos:      mf.BytesToUint32(int(i + rvBposPos)),
		RefAlt:   mf.BytesToUint32(int(i + rvBposRefAlt)),
	}
}

// FindRVVarhashByRsid search for the specified RSID and returns the first occurrence of VariantHash, item position.
func (mf TMMFile) FindRVVarhashByRsid(first, last uint64, rsid uint32) (TVariantHash, uint64) {
	firstItem, _, _ := mf.FindFirstUint32(rvBinBlkLen, 0, first, last, rsid)
	if firstItem > last {
		return TVariantHash{}, firstItem
	}
	return mf.GetRVVarhash(firstItem), firstItem
}

// FindVRRsidByVarshash search for the specified VariantHash and returns the first occurrence of RSID, item position.
func (mf TMMFile) FindVRRsidByVarshash(first uint64, last uint64, vh TVariantHash) (uint32, uint64) {
	firstItem, _, _ := mf.FindFirstUint128(rvBinBlkLen, 0, first, last, Uint128{Hi: (uint64(vh.Assembly)<<32 | uint64(vh.Chrom)), Lo: (uint64(vh.Pos)<<32 | uint64(vh.RefAlt))})
	if firstItem > last {
		return 0, firstItem
	}
	return mf.GetVRRsid(firstItem), firstItem
}

// FindVRChromposRange search for the specified CHROM-POS range and returns the first occurrence of RSID, item position, last position.
func (mf TMMFile) FindVRChromposRange(first, last uint64, chrom, posStart, posEnd uint32) (uint32, uint64, uint64) {
	firstItem, f, _ := mf.FindFirstUint64(rvBinBlkLen, vrBposChrom, first, last, (uint64(chrom)<<32 | uint64(posStart)))
	if firstItem > last {
		firstItem = f
	}
	if f > last {
		return 0, firstItem, last
	}
	lastItem, _, _ := mf.FindLastUint64(rvBinBlkLen, vrBposChrom, firstItem, last, (uint64(chrom)<<32 | uint64(posEnd)))
	return mf.GetVRRsid(firstItem), firstItem, lastItem
}
