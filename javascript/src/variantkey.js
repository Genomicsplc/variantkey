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

// VariantKey by Nicola Asuni


function encodeChrom(chrom) {
    chrom = chrom.replace(/^chr/i, '');
    var c = chrom.charAt(0);
    if ((c >= '0') && (c <= '9')) {
        return parseInt(chrom, 10);
    }
    if ((c == 'X') || (c == 'x')) {
        return 23;
    }
    if ((c == 'Y') || (c == 'y')) {
        return 24;
    }
    if ((c == 'M') || (c == 'm')) {
        return 25;
    }
    return 0;
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
    if ((c == 'A') || (c == 'a')) {
        return 0;
    }
    if ((c == 'C') || (c == 'c')) {
        return 1;
    }
    if ((c == 'G') || (c == 'g')) {
        return 2;
    }
    if ((c == 'T') || (c == 't')) {
        return 3;
    }
    return 4;
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
        return 0;
    }
    h = encodeAllele(h, (23 - (2 * sizeref)), alt, sizealt);
    if (h < 0) {
        return 0;
    }
    return h >>> 0;
}

function azToUpper(c) {
    if ((c >= 97) && (c <= 122)) { // 'a' = 97; 'z' = 122;
        return (c ^ 32); // ('a' - 'A') = 32
    }
    return c;
}

function packChars(str, size, offset) {
    var h = 0 >>> 0;
    var bitpos = (31 >>> 0);
    var c;
    for (var i = offset; i < (size + offset); i++) {
        c = azToUpper(str.charCodeAt(i)) >>> 0;
        if (c == 42) { // '*' = 42
            c = 91; // ('Z' + 1) = '[' = 91
        }
        bitpos -= 5;
        h |= (((c - 64) << bitpos) >>> 0); // A will be coded as 1 ('A' - 1 = 64)
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

// Return a 32 bit hash of a nucleotide string
function hash32(str, size) {
    var h = 0;
    var len = 6;
    var offset = 0;
    while (size > 0) {
        if (size < len) {
            len = size;
        }
        //[01111122 22233333 44444555 55666660]
        // pack blocks of 6 characters in 32 bit (6 x 5 bit + 2 spare bit)
        h = muxHash(packChars(str, len, offset), h);
        size -= len;
        offset += len;
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
        if (h != 0) {
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

function variantKey(chrom, pos, ref, alt) {
    return {
        "hi": ((((encodeChrom(chrom) >>> 0) << 27) | (pos >>> 1)) >>> 0),
        "lo": ((((pos >>> 0) << 31) | encodeRefAlt(ref, alt)) >>> 0)
    };
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

function decodeVariantKey(vk) {
    return {
        "chrom": ((vk.hi & 0xF8000000) >>> 27),
        "pos": (((vk.hi & 0x7FFFFFF) << 1) | (vk.lo >>> 31)) >>> 0,
        "refalt": (vk.lo & 0x7FFFFFFF) >>> 0
    }
}

function reverseVariantKey(dvk) {
    var ra = decodeRefAlt(dvk.refalt);
    return {
        "chrom": decodeChrom(dvk.chrom),
        "pos": dvk.pos,
        "ref": ra.ref,
        "alt": ra.alt
    }
}

if (typeof(module) !== 'undefined') {
    module.exports = {
        encodeChrom: encodeChrom,
        decodeChrom: decodeChrom,
        reverseVariantKey: reverseVariantKey,
        decodeVariantKey: decodeVariantKey,
        parseHex: parseHex,
        azToUpper: azToUpper,
        encodeRefAlt: encodeRefAlt,
        decodeRefAlt: decodeRefAlt,
        variantKey: variantKey,
        variantKeyRange: variantKeyRange,
        variantKeyString: variantKeyString,
    }
}