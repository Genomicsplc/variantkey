package binsearch

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

// Uint128 contains 2 64bit unsigned integers
type Uint128 struct {
	Hi uint64
	Lo uint64
}

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

// BytesToUint8 convert 1 byte in big-endian format to uint8.
func (mf TMMFile) BytesToUint8(i int) uint8 {
	return (uint8(mf.r.At(i)))
}

// BytesToUint16 convert 2 bytes in big-endian format to uint16.
func (mf TMMFile) BytesToUint16(i int) uint16 {
	return ((uint16(mf.r.At(i)) << 8) | uint16(mf.r.At(i+1)))
}

// BytesToUint32 convert 4 bytes in big-endian format to uint32.
func (mf TMMFile) BytesToUint32(i int) uint32 {
	return ((uint32(mf.r.At(i)) << 24) | (uint32(mf.r.At(i+1)) << 16) | (uint32(mf.r.At(i+2)) << 8) | uint32(mf.r.At(i+3)))
}

// BytesToUint64 convert 8 bytes in big-endian format to uint64.
func (mf TMMFile) BytesToUint64(i int) uint64 {
	return ((uint64(mf.r.At(i)) << 56) | (uint64(mf.r.At(i+1)) << 48) | (uint64(mf.r.At(i+2)) << 40) | (uint64(mf.r.At(i+3)) << 32) | (uint64(mf.r.At(i+4)) << 24) | (uint64(mf.r.At(i+5)) << 16) | (uint64(mf.r.At(i+6)) << 8) | uint64(mf.r.At(i+7)))
}

// BytesToUint128 convert 16 bytes in big-endian format to Uint128.
func (mf TMMFile) BytesToUint128(i int) Uint128 {
	return Uint128{
		Hi: mf.BytesToUint64(i),
		Lo: mf.BytesToUint64(i + 8),
	}
}

// FindFirstUint8 search for the first occurrence of a 8 bit unsigned integer on a memory mapped
// binary file containing adjacent blocks of sorted binary data.
// The 8 bit values in the file must encoded in big-endian format and sorted in ascending order.
// Return the item number if found or (last + 1) if not found, plus the first and last positions.
func (mf TMMFile) FindFirstUint8(blklen, blkpos, first, last uint64, search uint8) (uint64, uint64, uint64) {
	var i, middle uint64
	var x uint8
	found := (last + 1)
	for first <= last {
		middle = ((first + last) >> 1)
		i = GetAddress(blklen, blkpos, middle)
		x = mf.BytesToUint8(int(i))
		if x == search {
			if middle == 0 {
				return middle, first, last
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
					return found, first, last
				}
			}
		}
	}
	return found, first, last
}

// FindLastUint8 search for the last occurrence of a 8 bit unsigned integer on a memory mapped
// binary file containing adjacent blocks of sorted binary data.
// The 8 bit values in the file must encoded in big-endian format and sorted in ascending order.
// Return the item number if found or (last + 1) if not found, plus the first and last positions.
func (mf TMMFile) FindLastUint8(blklen, blkpos, first, last uint64, search uint8) (uint64, uint64, uint64) {
	var i, middle uint64
	var x uint8
	found := (last + 1)
	for first <= last {
		middle = ((first + last) >> 1)
		i = GetAddress(blklen, blkpos, middle)
		x = mf.BytesToUint8(int(i))
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
					return found, first, last
				}
			}
		}
	}
	return found, first, last
}

// FindFirstUint16 search for the first occurrence of a 16 bit unsigned integer on a memory mapped
// binary file containing adjacent blocks of sorted binary data.
// The 16 bit values in the file must encoded in big-endian format and sorted in ascending order.
// Return the item number if found or (last + 1) if not found, plus the first and last positions.
func (mf TMMFile) FindFirstUint16(blklen, blkpos, first, last uint64, search uint16) (uint64, uint64, uint64) {
	var i, middle uint64
	var x uint16
	found := (last + 1)
	for first <= last {
		middle = ((first + last) >> 1)
		i = GetAddress(blklen, blkpos, middle)
		x = mf.BytesToUint16(int(i))
		if x == search {
			if middle == 0 {
				return middle, first, last
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
					return found, first, last
				}
			}
		}
	}
	return found, first, last
}

// FindLastUint16 search for the last occurrence of a 16 bit unsigned integer on a memory mapped
// binary file containing adjacent blocks of sorted binary data.
// The 16 bit values in the file must encoded in big-endian format and sorted in ascending order.
// Return the item number if found or (last + 1) if not found, plus the first and last positions.
func (mf TMMFile) FindLastUint16(blklen, blkpos, first, last uint64, search uint16) (uint64, uint64, uint64) {
	var i, middle uint64
	var x uint16
	found := (last + 1)
	for first <= last {
		middle = ((first + last) >> 1)
		i = GetAddress(blklen, blkpos, middle)
		x = mf.BytesToUint16(int(i))
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
					return found, first, last
				}
			}
		}
	}
	return found, first, last
}

// FindFirstUint32 search for the first occurrence of a 32 bit unsigned integer on a memory mapped
// binary file containing adjacent blocks of sorted binary data.
// The 32 bit values in the file must encoded in big-endian format and sorted in ascending order.
// Return the item number if found or (last + 1) if not found, plus the first and last positions.
func (mf TMMFile) FindFirstUint32(blklen, blkpos, first, last uint64, search uint32) (uint64, uint64, uint64) {
	var i, middle uint64
	var x uint32
	found := (last + 1)
	for first <= last {
		middle = ((first + last) >> 1)
		i = GetAddress(blklen, blkpos, middle)
		x = mf.BytesToUint32(int(i))
		if x == search {
			if middle == 0 {
				return middle, first, last
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
					return found, first, last
				}
			}
		}
	}
	return found, first, last
}

// FindLastUint32 search for the last occurrence of a 32 bit unsigned integer on a memory mapped
// binary file containing adjacent blocks of sorted binary data.
// The 32 bit values in the file must encoded in big-endian format and sorted in ascending order.
// Return the item number if found or (last + 1) if not found, plus the first and last positions.
func (mf TMMFile) FindLastUint32(blklen, blkpos, first, last uint64, search uint32) (uint64, uint64, uint64) {
	var i, middle uint64
	var x uint32
	found := (last + 1)
	for first <= last {
		middle = ((first + last) >> 1)
		i = GetAddress(blklen, blkpos, middle)
		x = mf.BytesToUint32(int(i))
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
					return found, first, last
				}
			}
		}
	}
	return found, first, last
}

// FindFirstUint64 search for the first occurrence of a 64 bit unsigned integer on a memory mapped
// binary file containing adjacent blocks of sorted binary data.
// The 64 bit values in the file must encoded in big-endian format and sorted in ascending order.
// Return the item number if found or (last + 1) if not found, plus the first and last positions.
func (mf TMMFile) FindFirstUint64(blklen, blkpos, first, last uint64, search uint64) (uint64, uint64, uint64) {
	var i, middle uint64
	var x uint64
	found := (last + 1)
	for first <= last {
		middle = ((first + last) >> 1)
		i = GetAddress(blklen, blkpos, middle)
		x = mf.BytesToUint64(int(i))
		if x == search {
			if middle == 0 {
				return middle, first, last
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
					return found, first, last
				}
			}
		}
	}
	return found, first, last
}

// FindLastUint64 search for the last occurrence of a 64 bit unsigned integer on a memory mapped
// binary file containing adjacent blocks of sorted binary data.
// The 64 bit values in the file must encoded in big-endian format and sorted in ascending order.
// Return the item number if found or (last + 1) if not found, plus the first and last positions.
func (mf TMMFile) FindLastUint64(blklen, blkpos, first, last uint64, search uint64) (uint64, uint64, uint64) {
	var i, middle uint64
	var x uint64
	found := (last + 1)
	for first <= last {
		middle = ((first + last) >> 1)
		i = GetAddress(blklen, blkpos, middle)
		x = mf.BytesToUint64(int(i))
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
					return found, first, last
				}
			}
		}
	}
	return found, first, last
}

// compareUint128 compare two 128bit numbers
func compareUint128(a Uint128, b Uint128) int {
	if a.Hi < b.Hi {
		return -1
	}
	if a.Hi > b.Hi {
		return 1
	}
	if a.Lo < b.Lo {
		return -1
	}
	if a.Lo > b.Lo {
		return 1
	}
	return 0
}

// FindFirstUint128 search for the first occurrence of a 128 bit unsigned integer on a memory mapped
// binary file containing adjacent blocks of sorted binary data.
// The 128 bit values in the file must encoded in big-endian format and sorted in ascending order.
// Return the item number if found or (last + 1) if not found, plus the first and last positions.
func (mf TMMFile) FindFirstUint128(blklen, blkpos, first, last uint64, search Uint128) (uint64, uint64, uint64) {
	var i, middle uint64
	var x Uint128
	var cmp int
	found := (last + 1)
	for first <= last {
		middle = ((first + last) >> 1)
		i = GetAddress(blklen, blkpos, middle)
		x = mf.BytesToUint128(int(i))
		cmp = compareUint128(x, search)
		if cmp == 0 {
			if middle == 0 {
				return middle, first, last
			}
			found = middle
			last = (middle - 1)
		} else {
			if cmp < 0 {
				first = (middle + 1)
			} else {
				if middle > 0 {
					last = (middle - 1)
				} else {
					return found, first, last
				}
			}
		}
	}
	return found, first, last
}

// FindLastUint128 search for the last occurrence of a 128 bit unsigned integer on a memory mapped
// binary file containing adjacent blocks of sorted binary data.
// The 128 bit values in the file must encoded in big-endian format and sorted in ascending order.
// Return the item number if found or (last + 1) if not found, plus the first and last positions.
func (mf TMMFile) FindLastUint128(blklen, blkpos, first, last uint64, search Uint128) (uint64, uint64, uint64) {
	var i, middle uint64
	var x Uint128
	var cmp int
	found := (last + 1)
	for first <= last {
		middle = ((first + last) >> 1)
		i = GetAddress(blklen, blkpos, middle)
		x = mf.BytesToUint128(int(i))
		cmp = compareUint128(x, search)
		if cmp == 0 {
			found = middle
			first = (middle + 1)
		} else {
			if cmp < 0 {
				first = (middle + 1)
			} else {
				if middle > 0 {
					last = (middle - 1)
				} else {
					return found, first, last
				}
			}
		}
	}
	return found, first, last
}
