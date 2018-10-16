/** VariantKey Javascript Library
 * 
 * variantkey.js
 * 
 * @category   Tools
 * @author     Nicola Asuni <nicola.asuni@genomicsplc.com>
 * @copyright  2017-2018 GENOMICS plc
 * @license    MIT (see LICENSE)
 * @link       https://github.com/genomicsplc/variantkey
 */

// LICENSE
// 
// Copyright (c) 2017-2018 GENOMICS plc
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


function encodeChrom(chrom) {
    chrom = chrom.replace(/^chr/i, '');
    var clen = chrom.length;
    if (clen == 0) {
        return 0;
    }
    // X > 23 ; Y > 24 ; M > 25
    var onecharmap = [
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        /*                                     M                                 X   Y */
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 25, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 23, 24, 0, 0, 0, 0, 0, 0,
        /*                                     m                                 x   y */
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 25, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 23, 24, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    ];
    var c = chrom.charAt(0);
    if ((c <= '9') && (c >= '0')) {
        var i;
        var v = (c - '0');
        for (i = 1; i < clen; i++) {
            c = chrom.charAt(i);
            if ((c > '9') || (c < '0')) {
                return 0; // NA
            }
            v = ((v * 10) + (c - '0'));
        }
        return v;
    }
    if ((clen == 1) || ((clen == 2) && ((chrom.charAt(1) == 'T') || (chrom.charAt(1) == 't')))) {
        return onecharmap[chrom.charCodeAt(0)];
    }
    return 0; // NA
}

function decodeChrom(code) {
    if ((code < 1) || (code > 25)) {
        return 'NA';
    }
    if (code < 23) {
        return code.toString();
    }
    var map = ['X', 'Y', 'MT'];
    return map[(code - 23)];
}

function encodeBase(c) {
    /*
      Encode base:

      A > 0
      C > 1
      G > 2
      T > 3
    */
    var map = [
        4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,
        4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,
        /* A     C           G                                      T */
        4, 0, 4, 1, 4, 4, 4, 2, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 3, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,
        /* a     c           g                                      t */
        4, 0, 4, 1, 4, 4, 4, 2, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 3, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,
        4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,
        4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,
        4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,
        4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,
    ];
    return map[c.charCodeAt(0)];
}

function encodeAllele(h, bitpos, str, size) {
    var v;
    for (var i = 0; i < size; i++) {
        v = encodeBase(str.charAt(i));
        if (v > 3) {
            return -1;
        }
        bitpos -= 2;
        h |= (v << bitpos);
    }
    return h;
}

function encodeRefAltRev(ref, sizeref, alt, sizealt) {
    var h = 0 >>> 0;
    h |= (sizeref << 27); // RRRR: length of (REF - 1)
    h |= (sizealt << 23); // AAAA: length of (ALT - 1)
    h = encodeAllele(h, 23, ref, sizeref);
    if (h < 0) {
        return 0xffffffff;
    }
    h = encodeAllele(h, (23 - (2 * sizeref)), alt, sizealt);
    if (h < 0) {
        return 0xffffffff;
    }
    return h >>> 0;
}

// Mix two 32 bit hash numbers using the MurmurHash3 algorithm
function muxHash(k, h) {
    k = ((((k & 0xffff) * 0xcc9e2d51) + ((((k >>> 16) * 0xcc9e2d51) & 0xffff) << 16))) & 0xffffffff;
    k = ((k << 15) | (k >>> 17));
    k = ((((k & 0xffff) * 0x1b873593) + ((((k >>> 16) * 0x1b873593) & 0xffff) << 16))) & 0xffffffff;
    h ^= k;
    h = ((h << 13) | (h >>> 19));
    hb = ((((h & 0xffff) * 5) + ((((h >>> 16) * 5) & 0xffff) << 16))) & 0xffffffff;
    h = (((hb & 0xffff) + 0x6b64) + ((((hb >>> 16) + 0xe654) & 0xffff) << 16));
    return h >>> 0;
}

function encodePackChar(c) {
    if (c < 65) {
        return (27 >>> 0);
    }
    if (c >= 97) {
        return ((c - 96) >>> 0);
    }
    return ((c - 64) >>> 0);
}

function packCharsTail(str, size, offset) {
    var h = (0 >>> 0);
    offset += (size - 1);
    switch (size) {
        case 5:
            h ^= encodePackChar(str.charCodeAt(offset--)) << 6;
            // fall through
        case 4:
            h ^= encodePackChar(str.charCodeAt(offset--)) << 11;
            // fall through
        case 3:
            h ^= encodePackChar(str.charCodeAt(offset--)) << 16;
            // fall through
        case 2:
            h ^= encodePackChar(str.charCodeAt(offset--)) << 21;
            // fall through
        case 1:
            h ^= encodePackChar(str.charCodeAt(offset)) << 26;
    }
    return h;
}

function packChars(str, offset) {
    return ((encodePackChar(str.charCodeAt(offset + 5)) << 1) ^
        (encodePackChar(str.charCodeAt(offset + 4)) << 6) ^
        (encodePackChar(str.charCodeAt(offset + 3)) << 11) ^
        (encodePackChar(str.charCodeAt(offset + 2)) << 16) ^
        (encodePackChar(str.charCodeAt(offset + 1)) << 21) ^
        (encodePackChar(str.charCodeAt(offset)) << 26));
}

// Return a 32 bit hash of a nucleotide string
function hash32(str, size) {
    var h = 0;
    var len = 6;
    var offset = 0;
    while (size >= len) {
        h = muxHash(packChars(str, offset), h);
        size -= len;
        offset += len;
    }
    if (size > 0) {
        h = muxHash(packCharsTail(str, size, offset), h);
    }
    return h;
}

function encodeRefAltHash(ref, sizeref, alt, sizealt) {
    // 0x3 is the separator character between REF and ALT [00000000 00000000 00000000 00000011]
    var h = muxHash(hash32(alt, sizealt), muxHash(0x3, hash32(ref, sizeref)));
    // finalization mix - MurmurHash3 algorithm
    h ^= h >>> 16;
    h = (((h & 0xffff) * 0x85ebca6b) + ((((h >>> 16) * 0x85ebca6b) & 0xffff) << 16)) & 0xffffffff;
    h ^= h >>> 13;
    h = ((((h & 0xffff) * 0xc2b2ae35) + ((((h >>> 16) * 0xc2b2ae35) & 0xffff) << 16))) & 0xffffffff;
    h ^= h >>> 16;
    return ((h >>> 1) | 0x1); // 0x1 is the set bit to indicate HASH mode [00000000 00000000 00000000 00000001]
}

function encodeRefAlt(ref, alt) {
    var sizeref = ref.length >>> 0;
    var sizealt = alt.length >>> 0;
    if ((sizeref + sizealt) <= 11) {
        var h = encodeRefAltRev(ref, sizeref, alt, sizealt);
        if (h != 0xffffffff) {
            return h >>> 0;
        }
    }
    return encodeRefAltHash(ref, sizeref, alt, sizealt) >>> 0;
}

function decodeBase(code, bitpos) {
    base = ['A', 'C', 'G', 'T'];
    return base[((code >> bitpos) & 0x3)]; // 0x3 is the 2 bit mask [00000011]
}

function decodeRefAltRev(code) {
    code >>>= 0;
    var sizeref = ((code & 0x78000000) >>> 27); // [01111000 00000000 00000000 00000000]
    var sizealt = ((code & 0x07800000) >>> 23); // [00000111 10000000 00000000 00000000]
    var bitpos = 23;
    var ref = "";
    var alt = "";
    for (var i = 0; i < sizeref; i++) {
        bitpos -= 2;
        ref += decodeBase(code, bitpos);
    }
    for (var i = 0; i < sizealt; i++) {
        bitpos -= 2;
        alt += decodeBase(code, bitpos);
    }
    return {
        "ref": ref,
        "alt": alt
    };
}

function decodeRefAlt(code) {
    code >>>= 0;
    if (code & 0x1) // check last bit
    {
        return {
            "ref": "",
            "alt": ""
        }; // non-reversible encoding
    }
    return decodeRefAltRev(code);
}

function encodeVariantKey(chrom, pos, refalt) {
    return {
        "hi": ((((chrom >>> 0) << 27) | (pos >>> 1)) >>> 0),
        "lo": ((((pos >>> 0) << 31) | refalt) >>> 0)
    };
}

function extractVariantKeyChrom(vk) {
    return ((vk.hi & 0xF8000000) >>> 27);
}

function extractVariantKeyPos(vk) {
    return (((vk.hi & 0x07FFFFFF) << 1) | (vk.lo >>> 31)) >>> 0;
}

function extractVariantKeyRefAlt(vk) {
    return (vk.lo & 0x7FFFFFFF) >>> 0;
}

function decodeVariantKey(vk) {
    return {
        "chrom": extractVariantKeyChrom(vk),
        "pos": extractVariantKeyPos(vk),
        "refalt": extractVariantKeyRefAlt(vk)
    };
}

function reverseVariantKey(vk) {
    var ra = decodeRefAlt(extractVariantKeyRefAlt(vk));
    return {
        "chrom": decodeChrom(extractVariantKeyChrom(vk)),
        "pos": extractVariantKeyPos(vk),
        "ref": ra.ref,
        "alt": ra.alt
    }
}

function variantKey(chrom, pos, ref, alt) {
    return encodeVariantKey(encodeChrom(chrom), pos, encodeRefAlt(ref, alt));
}

function variantKeyRange(chrom, pos_min, pos_max) {
    return {
        "min": {
            "hi": ((((chrom >>> 0) << 27) | (pos_min >>> 1)) >>> 0),
            "lo": (((pos_min >>> 0) << 31) >>> 0)
        },
        "max": {
            "hi": ((((chrom >>> 0) << 27) | (pos_max >>> 1)) >>> 0),
            "lo": ((((pos_max >>> 0) << 31) | 0x7FFFFFFF) >>> 0)
        }
    };
}

function compare(a, b) {
    return ((a < b) ? -1 : ((a > b) ? 1 : 0));
}

function compareVariantKeyChrom(vka, vkb) {
    return compare((vka.hi >>> 27), (vkb.hi >>> 27));
}

function compareVariantKeyChromPos(vka, vkb) {
    var cmp = compare(vka.hi, vkb.hi);
    if (cmp == 0) {
        return compare((vka.lo >>> 31), (vkb.lo >>> 31));
    }
    return cmp;
}

function padL08(s) {
    return ("00000000" + s).slice(-8);
}

function variantKeyString(vk) {
    return padL08(vk.hi.toString(16)) + padL08(vk.lo.toString(16));
}

function parseHex(vs) {
    return {
        "hi": parseInt(vs.substring(0, 8), 16) >>> 0,
        "lo": parseInt(vs.substring(8, 16), 16) >>> 0,
    };
}

function encodeRegionStrand(strand) {
    var map = [2, 0, 1, 0];
    return map[((++strand) & 3)];
}

function decodeRegionStrand(strand) {
    var map = [0, 1, -1, 0];
    return map[(strand & 3)];
}

function encodeRegionKey(chrom, startpos, endpos, strand) {
    return {
        "hi": ((((chrom >>> 0) << 27) | (startpos >>> 1)) >>> 0),
        "lo": ((((startpos >>> 0) << 31) | ((endpos >>> 0) << 3) | ((strand >>> 0) << 1)) >>> 0)
    };
}

function extractRegionKeyChrom(rk) {
    return ((rk.hi & 0xF8000000) >>> 27);
}

function extractRegionKeyStartPos(rk) {
    return (((rk.hi & 0x07FFFFFF) << 1) | (rk.lo >>> 31)) >>> 0;
}

function extractRegionKeyEndPos(rk) {
    return (rk.lo & 0x7FFFFFF8) >>> 3
}

function extractRegionKeyStrand(rk) {
    return (rk.lo & 0x00000006) >>> 1;
}

function decodeRegionKey(rk) {
    return {
        "chrom": extractRegionKeyChrom(rk),
        "startpos": extractRegionKeyStartPos(rk),
        "endpos": extractRegionKeyEndPos(rk),
        "strand": extractRegionKeyStrand(rk)
    };
}

function reverseRegionKey(rk) {
    return {
        "chrom": decodeChrom(extractRegionKeyChrom(rk)),
        "startpos": extractRegionKeyStartPos(rk),
        "endpos": extractRegionKeyEndPos(rk),
        "strand": decodeRegionStrand(extractRegionKeyStrand(rk))
    };
}

function regionKey(chrom, startpos, endpos, strand) {
    return encodeRegionKey(encodeChrom(chrom), startpos, endpos, encodeRegionStrand(strand));
}

function extendRegionKey(rk, size) {
    var drk = decodeRegionKey(rk);
    drk.startpos = ((size >= drk.startpos) ? 0 : (drk.startpos - size));
    drk.endpos = (((0x0FFFFFFF - drk.endpos) <= size) ? 0x0FFFFFFF : (drk.endpos + size));
    return encodeRegionKey(drk.chrom, drk.startpos, drk.endpos, drk.strand);
}

function regionKeyString(rk) {
    return padL08(rk.hi.toString(16)) + padL08(rk.lo.toString(16));
}

function getVariantKeyEndPos(vk) {
    return extractVariantKeyPos(vk) + ((vk.lo & 0x78000000) >>> 27);
}

function areOverlappingRegions(a_chrom, a_startpos, a_endpos, b_chrom, b_startpos, b_endpos) {
    return ((a_chrom == b_chrom) && (a_startpos < b_endpos) && (a_endpos > b_startpos));
}

function areOverlappingRegionRegionKey(chrom, startpos, endpos, rk) {
    return ((chrom == extractRegionKeyChrom(rk)) && (startpos < extractRegionKeyEndPos(rk)) && (endpos > extractRegionKeyStartPos(rk)));
}

function areOverlappingRegionKeys(rka, rkb) {
    return ((extractRegionKeyChrom(rka) == extractRegionKeyChrom(rkb)) && (extractRegionKeyStartPos(rka) < extractRegionKeyEndPos(rkb)) && (extractRegionKeyEndPos(rka) > extractRegionKeyStartPos(rkb)));
}

function areOverlappingVariantKeyRegionKey(vk, rk) {
    return ((extractVariantKeyChrom(vk) == extractRegionKeyChrom(rk)) && (extractVariantKeyPos(vk) < extractRegionKeyEndPos(rk)) && (getVariantKeyEndPos(vk) > extractRegionKeyStartPos(rk)));
}

function variantKeyToRegionKey(vk) {
    return {
        "hi": vk.hi,
        "lo": ((vk.lo & 0x80000000) | ((getVariantKeyEndPos(vk) << 3) >>> 0)) >>> 0
    };
}

function esidEncodeChar(c) {
    if (c < 33) {
        return (63 >>> 0);
    }
    if (c > 95) {
        return ((c - 64) >>> 0);
    }
    return ((c - 32) >>> 0);
}

function esidDecodeChar(esid, pos) {
    return String.fromCharCode(((esid >>> pos) & 63) + 32); // 63 dec = 00111111 bin
}

function encodeStringID(str, start) {
    var size = str.length - start;
    if (size > 10) {
        size = 10;
    }
    var hi = (0 >>> 0);
    var lo = (0 >>> 0);
    var offset = (size + start - 1);
    switch (size) {
        case 10:
            lo ^= esidEncodeChar(str.charCodeAt(offset--));
            // fall through
        case 9:
            lo ^= esidEncodeChar(str.charCodeAt(offset--)) << 6;
            // fall through
        case 8:
            lo ^= esidEncodeChar(str.charCodeAt(offset--)) << 12;
            // fall through
        case 7:
            lo ^= esidEncodeChar(str.charCodeAt(offset--)) << 18;
            // fall through
        case 6:
            lo ^= esidEncodeChar(str.charCodeAt(offset--)) << 24;
            // fall through
        case 5:
            hi ^= esidEncodeChar(str.charCodeAt(offset--));
            // fall through
        case 4:
            hi ^= esidEncodeChar(str.charCodeAt(offset--)) << 6;
            // fall through
        case 3:
            hi ^= esidEncodeChar(str.charCodeAt(offset--)) << 12;
            // fall through
        case 2:
            hi ^= esidEncodeChar(str.charCodeAt(offset--)) << 18;
            // fall through
        case 1:
            hi ^= esidEncodeChar(str.charCodeAt(offset)) << 24;
    }
    return {
        "hi": ((hi >>> 2) | ((size >>> 0) << 28)) >>> 0,
        "lo": (lo | ((hi & 3) << 30)) >>> 0,
    };
}

function encodeStringNumID(str, sep) {
    var size = str.length;
    if (size <= 10) {
        return encodeStringID(str, 0);
    }
    var hi = (0 >>> 0);
    var lo = (0 >>> 0);
    var num = (0 >>> 0);
    var nchr = (0 >>> 0);
    var npad = (0 >>> 0);
    var bitpos = 30;
    var c;
    var i = 0;
    while (size--) {
        c = str.charCodeAt(i++);
        if (c == sep) {
            break;
        }
        if (nchr < 5) {
            bitpos -= 6;
            hi |= (esidEncodeChar(c) << bitpos);
            nchr++;
        }
    }
    lo |= ((hi & 3) << 30) >>> 0;
    hi = ((hi >>> 2) | (((nchr + 10) >>> 0) << 28)) >>> 0;
    while (((c = str.charCodeAt(i++)) == 48) && (npad < 7) && (size--)) {
        npad++;
    }
    lo |= (npad << 27) >>> 0;
    while ((c >= 48) && (c <= 57) && (size--)) {
        num = ((num * 10) + (c - 48)) >>> 0;
        c = str.charCodeAt(i++);
    }
    lo |= (num & 0x7FFFFFF);
    return {
        "hi": hi,
        "lo": lo,
    };
}

function esdiDecodeStringID(size, esid) {
    var hi = ((esid.hi << 2) | (esid.lo >>> 30)) >>> 0;
    var str = ['', '', '', '', '', '', '', '', '', ''];
    switch (size) {
        case 10:
            str[9] = esidDecodeChar(esid.lo, 0);
            // fall through
        case 9:
            str[8] = esidDecodeChar(esid.lo, 6);
            // fall through
        case 8:
            str[7] = esidDecodeChar(esid.lo, 12);
            // fall through
        case 7:
            str[6] = esidDecodeChar(esid.lo, 18);
            // fall through
        case 6:
            str[5] = esidDecodeChar(esid.lo, 24);
            // fall through
        case 5:
            str[4] = esidDecodeChar(hi, 0);
            // fall through
        case 4:
            str[3] = esidDecodeChar(hi, 6);
            // fall through
        case 3:
            str[2] = esidDecodeChar(hi, 12);
            // fall through
        case 2:
            str[1] = esidDecodeChar(hi, 18);
            // fall through
        case 1:
            str[0] = esidDecodeChar(hi, 24);
    }
    return str.join('');
}

function decodeStringNumID(size, esid) {
    var str = esdiDecodeStringID(size, esid);
    var npad = (esid.lo >>> 27) & 7;
    var numstr = '';
    var num = (esid.lo & 0x7FFFFFF);
    if (num > 0) {
        numstr = num.toString();
    }
    return str + ':' + '0'.repeat(npad) + numstr;
}

function decodeStringID(esid) {
    var size = (esid.hi >>> 28);
    if (size > 10) {
        return decodeStringNumID((size - 10), esid);
    }
    return esdiDecodeStringID(size, esid);
}

if (typeof(module) !== 'undefined') {
    module.exports = {
        encodeChrom: encodeChrom,
        decodeChrom: decodeChrom,
        parseHex: parseHex,
        encodeRefAlt: encodeRefAlt,
        decodeRefAlt: decodeRefAlt,
        encodeVariantKey: encodeVariantKey,
        extractVariantKeyChrom: extractVariantKeyChrom,
        extractVariantKeyPos: extractVariantKeyPos,
        extractVariantKeyRefAlt: extractVariantKeyRefAlt,
        decodeVariantKey: decodeVariantKey,
        variantKey: variantKey,
        variantKeyRange: variantKeyRange,
        compareVariantKeyChrom: compareVariantKeyChrom,
        compareVariantKeyChromPos: compareVariantKeyChromPos,
        variantKeyString: variantKeyString,
        reverseVariantKey: reverseVariantKey,
        encodeRegionStrand: encodeRegionStrand,
        decodeRegionStrand: decodeRegionStrand,
        encodeRegionKey: encodeRegionKey,
        extractRegionKeyChrom: extractRegionKeyChrom,
        extractRegionKeyStartPos: extractRegionKeyStartPos,
        extractRegionKeyEndPos: extractRegionKeyEndPos,
        extractRegionKeyStrand: extractRegionKeyStrand,
        decodeRegionKey: decodeRegionKey,
        reverseRegionKey: reverseRegionKey,
        regionKey: regionKey,
        extendRegionKey: extendRegionKey,
        regionKeyString: regionKeyString,
        getVariantKeyEndPos: getVariantKeyEndPos,
        areOverlappingRegions: areOverlappingRegions,
        areOverlappingRegionRegionKey: areOverlappingRegionRegionKey,
        areOverlappingRegionKeys: areOverlappingRegionKeys,
        areOverlappingVariantKeyRegionKey: areOverlappingVariantKeyRegionKey,
        variantKeyToRegionKey: variantKeyToRegionKey,
        encodeStringID: encodeStringID,
        encodeStringNumID: encodeStringNumID,
        decodeStringID: decodeStringID,
    }
}