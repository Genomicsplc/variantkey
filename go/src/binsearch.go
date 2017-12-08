package varianthash

// Copyright (c) 2017 Nicola Asuni - Tecnick.com
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.

import "golang.org/x/exp/mmap"

// TMMFile contains the memory mapped file reader
type TMMFile struct {
	r *mmap.ReaderAt
}

// MmapBinFile maps the specified file in memory.
func MmapBinFile(file string) (TMMFile, error) {
	mf, err := mmap.Open(file)
	return TMMFile{r: mf}, err
}

// Close unmap and close the memory-mapped file.
func (mf TMMFile) Close() error {
	if mf.r != nil {
		return mf.r.Close()
	}
	return nil
}

// GetAddress returns the absolute file start position of the specified item (binary block).
func GetAddress(blklen, blkpos, item uint64) uint64 {
	return ((blklen * item) + blkpos)
}

// BytesToUint32be convert 4 bytes in big-endian format to uint32.
func (mf TMMFile) BytesToUint32be(i int) uint32 {
	return ((uint32(mf.r.At(i)) << 24) | (uint32(mf.r.At(i+1)) << 16) | (uint32(mf.r.At(i+2)) << 8) | uint32(mf.r.At(i+3)))
}

// BytesToUint64be convert 8 bytes in big-endian format to uint64.
func (mf TMMFile) BytesToUint64be(i int) uint64 {
	return ((uint64(mf.r.At(i)) << 56) | (uint64(mf.r.At(i+1)) << 48) | (uint64(mf.r.At(i+2)) << 40) | (uint64(mf.r.At(i+3)) << 32) | (uint64(mf.r.At(i+4)) << 24) | (uint64(mf.r.At(i+5)) << 16) | (uint64(mf.r.At(i+6)) << 8) | uint64(mf.r.At(i+7)))
}

// FindFirstUint32be search for the first occurrence of a 32 bit unsigned integer on a memory mapped
// binary file containing adjacent blocks of sorted binary data.
// The 32 bit values in the file must encoded in big-endian format and sorted in ascending order.
// Return the item number if found or (last + 1) if not found.
func (mf TMMFile) FindFirstUint32be(blklen, blkpos, first, last uint64, search uint32) uint64 {
	var i, middle uint64
	var x uint32
	found := (last + 1)
	for first <= last {
		middle = ((first + last) >> 1)
		i = GetAddress(blklen, blkpos, middle)
		x = mf.BytesToUint32be(int(i))
		if x == search {
			if middle == 0 {
				return middle
			}
			found = middle
			last = (middle - 1)
		} else {
			if x < search {
				first = (middle + 1)
			} else {
				if middle > 0 {
					last = (middle - 1)
				} else {
					return found
				}
			}
		}
	}
	return found
}

// FindLastUint32be search for the last occurrence of a 32 bit unsigned integer on a memory mapped
// binary file containing adjacent blocks of sorted binary data.
// The 32 bit values in the file must encoded in big-endian format and sorted in ascending order.
// Return the item number if found or (last + 1) if not found.
func (mf TMMFile) FindLastUint32be(blklen, blkpos, first, last uint64, search uint32) uint64 {
	var i, middle uint64
	var x uint32
	found := (last + 1)
	for first <= last {
		middle = ((first + last) >> 1)
		i = GetAddress(blklen, blkpos, middle)
		x = mf.BytesToUint32be(int(i))
		if x == search {
			found = middle
			first = (middle + 1)
		} else {
			if x < search {
				first = (middle + 1)
			} else {
				if middle > 0 {
					last = (middle - 1)
				} else {
					return found
				}
			}
		}
	}
	return found
}

// FindFirstUint64be search for the first occurrence of a 64 bit unsigned integer on a memory mapped
// binary file containing adjacent blocks of sorted binary data.
// The 64 bit values in the file must encoded in big-endian format and sorted in ascending order.
// Return the item number if found or (last + 1) if not found.
func (mf TMMFile) FindFirstUint64be(blklen, blkpos, first, last uint64, search uint64) uint64 {
	var i, middle uint64
	var x uint64
	found := (last + 1)
	for first <= last {
		middle = ((first + last) >> 1)
		i = GetAddress(blklen, blkpos, middle)
		x = mf.BytesToUint64be(int(i))
		if x == search {
			if middle == 0 {
				return middle
			}
			found = middle
			last = (middle - 1)
		} else {
			if x < search {
				first = (middle + 1)
			} else {
				if middle > 0 {
					last = (middle - 1)
				} else {
					return found
				}
			}
		}
	}
	return found
}

// FindLastUint64be search for the last occurrence of a 64 bit unsigned integer on a memory mapped
// binary file containing adjacent blocks of sorted binary data.
// The 64 bit values in the file must encoded in big-endian format and sorted in ascending order.
// Return the item number if found or (last + 1) if not found.
func (mf TMMFile) FindLastUint64be(blklen, blkpos, first, last uint64, search uint64) uint64 {
	var i, middle uint64
	var x uint64
	found := (last + 1)
	for first <= last {
		middle = ((first + last) >> 1)
		i = GetAddress(blklen, blkpos, middle)
		x = mf.BytesToUint64be(int(i))
		if x == search {
			found = middle
			first = (middle + 1)
		} else {
			if x < search {
				first = (middle + 1)
			} else {
				if middle > 0 {
					last = (middle - 1)
				} else {
					return found
				}
			}
		}
	}
	return found
}

// FindFirstUint128be search for the first occurrence of a 128 bit unsigned integer on a memory mapped
// binary file containing adjacent blocks of sorted binary data.
// The 128 bit values in the file must encoded in big-endian format and sorted in ascending order.
// Return the item number if found or (last + 1) if not found.
func (mf TMMFile) FindFirstUint128be(blklen, blkpos, first, last uint64, searchHi, searchLo uint64) uint64 {
	var i, middle uint64
	var xHi, xLo uint64
	found := (last + 1)
	for first <= last {
		middle = ((first + last) >> 1)
		i = GetAddress(blklen, blkpos, middle)
		xHi = mf.BytesToUint64be(int(i))
		if xHi == searchHi {
			xLo = mf.BytesToUint64be(int(i + 8))
			if xLo == searchLo {
				if middle == 0 {
					return middle
				}
				found = middle
				last = (middle - 1)
			} else {
				if xLo < searchLo {
					first = (middle + 1)
				} else {
					if middle > 0 {
						last = (middle - 1)
					} else {
						return found
					}
				}
			}
		} else {
			if xHi < searchHi {
				first = (middle + 1)
			} else {
				if middle > 0 {
					last = (middle - 1)
				} else {
					return found
				}
			}
		}
	}
	return found
}

// FindLastUint128be search for the last occurrence of a 128 bit unsigned integer on a memory mapped
// binary file containing adjacent blocks of sorted binary data.
// The 128 bit values in the file must encoded in big-endian format and sorted in ascending order.
// Return the item number if found or (last + 1) if not found.
func (mf TMMFile) FindLastUint128be(blklen, blkpos, first, last uint64, searchHi, searchLo uint64) uint64 {
	var i, middle uint64
	var xHi, xLo uint64
	found := (last + 1)
	for first <= last {
		middle = ((first + last) >> 1)
		i = GetAddress(blklen, blkpos, middle)
		xHi = mf.BytesToUint64be(int(i))
		if xHi == searchHi {
			xLo = mf.BytesToUint64be(int(i + 8))
			if xLo == searchLo {
				found = middle
				first = (middle + 1)
			} else {
				if xLo < searchLo {
					first = (middle + 1)
				} else {
					if middle > 0 {
						last = (middle - 1)
					} else {
						return found
					}
				}
			}
		} else {
			if xHi < searchHi {
				first = (middle + 1)
			} else {
				if middle > 0 {
					last = (middle - 1)
				} else {
					return found
				}
			}
		}
	}
	return found
}
