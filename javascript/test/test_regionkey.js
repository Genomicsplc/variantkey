/** VariantKey Javascript Library Test
 * 
 * test_regionkey.js
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

const {
    encodeRegionStrand,
    decodeRegionStrand,
    encodeRegionKey,
    extractRegionKeyChrom,
    extractRegionKeyStartPos,
    extractRegionKeyEndPos,
    extractRegionKeyStrand,
    decodeRegionKey,
    reverseRegionKey,
    regionKey,
    extendRegionKey,
    regionKeyString,
    getVariantKeyEndPos,
    areOverlappingRegions,
    areOverlappingRegionRegionKey,
    areOverlappingRegionKeys,
    areOverlappingVariantKeyRegionKey,
    variantKeyToRegionKey,
} = require(process.argv[2]);

var k_test_size = 10;
// 0      1         2       3       4       5        6   7   8               9
// chrom, startpos, endpos, strand, echrom, estrand, rk, rs, chrom_startpos, chrom_endpos
var test_data = [
    [ "1", 1000, 1100,  0,  1, 0, {"hi": 0x080001f4, "lo": 0x00002260}, "080001f400002260", {"hi": 0x00000000, "lo": 0x100003e8}, {"hi": 0x00000000, "lo": 0x1000044c}],
    [ "2", 1001, 1201,  1,  2, 1, {"hi": 0x100001f4, "lo": 0x8000258a}, "100001f48000258a", {"hi": 0x00000000, "lo": 0x200003e9}, {"hi": 0x00000000, "lo": 0x200004b1}],
    [ "3", 1002, 1302, -1,  3, 2, {"hi": 0x180001f5, "lo": 0x000028b4}, "180001f5000028b4", {"hi": 0x00000000, "lo": 0x300003ea}, {"hi": 0x00000000, "lo": 0x30000516}],
    [ "4", 1003, 1403,  0,  4, 0, {"hi": 0x200001f5, "lo": 0x80002bd8}, "200001f580002bd8", {"hi": 0x00000000, "lo": 0x400003eb}, {"hi": 0x00000000, "lo": 0x4000057b}],
    [ "5", 1004, 1504,  1,  5, 1, {"hi": 0x280001f6, "lo": 0x00002f02}, "280001f600002f02", {"hi": 0x00000000, "lo": 0x500003ec}, {"hi": 0x00000000, "lo": 0x500005e0}],
    ["10", 1005, 1605, -1, 10, 2, {"hi": 0x500001f6, "lo": 0x8000322c}, "500001f68000322c", {"hi": 0x00000000, "lo": 0xa00003ed}, {"hi": 0x00000000, "lo": 0xa0000645}],
    ["22", 1006, 1706,  0, 22, 0, {"hi": 0xb00001f7, "lo": 0x00003550}, "b00001f700003550", {"hi": 0x00000001, "lo": 0x600003ee}, {"hi": 0x00000001, "lo": 0x600006aa}],
    [ "X", 1007, 1807,  1, 23, 1, {"hi": 0xb80001f7, "lo": 0x8000387a}, "b80001f78000387a", {"hi": 0x00000001, "lo": 0x700003ef}, {"hi": 0x00000001, "lo": 0x7000070f}],
    [ "Y", 1008, 1908, -1, 24, 2, {"hi": 0xc00001f8, "lo": 0x00003ba4}, "c00001f800003ba4", {"hi": 0x00000001, "lo": 0x800003f0}, {"hi": 0x00000001, "lo": 0x80000774}],
    ["MT", 1009, 2009,  0, 25, 0, {"hi": 0xc80001f8, "lo": 0x80003ec8}, "c80001f880003ec8", {"hi": 0x00000001, "lo": 0x900003f1}, {"hi": 0x00000001, "lo": 0x900007d9}],
];

var k_test_overlap_size = 12;
// 0        1           2         3     4     5        6           7         8     9
// a_chrom, a_startpos, a_endpos, a_rk, a_vk, b_chrom, b_startpos, b_endpos, b_rk, res;
test_overlap = [
    [ 1, 5,  7, {"hi": 0x08000002, "lo": 0x80000038}, {"hi": 0x08000002, "lo": 0x90920000},  2, 5, 7, {"hi": 0x10000002, "lo": 0x80000038}, false], // different chromosome
    [ 1, 0,  2, {"hi": 0x08000000, "lo": 0x00000010}, {"hi": 0x08000000, "lo": 0x10920000},  1, 3, 7, {"hi": 0x08000001, "lo": 0x80000038}, false], // -[-]|---|----
    [ 2, 1,  3, {"hi": 0x10000000, "lo": 0x80000018}, {"hi": 0x10000000, "lo": 0x90920000},  2, 3, 7, {"hi": 0x10000001, "lo": 0x80000038}, false], // --[-]---|----
    [ 3, 2,  4, {"hi": 0x18000001, "lo": 0x00000020}, {"hi": 0x18000001, "lo": 0x10920000},  3, 3, 7, {"hi": 0x18000001, "lo": 0x80000038}, true],  // ---[|]--|----
    [ 4, 3,  5, {"hi": 0x20000001, "lo": 0x80000028}, {"hi": 0x20000001, "lo": 0x90920000},  4, 3, 7, {"hi": 0x20000001, "lo": 0x80000038}, true],  // ----[-]-|----
    [ 5, 4,  6, {"hi": 0x28000002, "lo": 0x00000030}, {"hi": 0x28000002, "lo": 0x10920000},  5, 3, 7, {"hi": 0x28000001, "lo": 0x80000038}, true],  // ----|[-]|----
    [ 6, 5,  7, {"hi": 0x30000002, "lo": 0x80000038}, {"hi": 0x30000002, "lo": 0x90920000},  6, 3, 7, {"hi": 0x30000001, "lo": 0x80000038}, true],  // ----|-[ ]----
    [10, 6,  8, {"hi": 0x50000003, "lo": 0x00000040}, {"hi": 0x50000003, "lo": 0x10920000}, 10, 3, 7, {"hi": 0x50000001, "lo": 0x80000038}, true],  // ----|--[|]---
    [22, 7,  9, {"hi": 0xb0000003, "lo": 0x80000048}, {"hi": 0xb0000003, "lo": 0x90920000}, 22, 3, 7, {"hi": 0xb0000001, "lo": 0x80000038}, false], // ----|---[-]--
    [23, 8, 10, {"hi": 0xb8000004, "lo": 0x00000050}, {"hi": 0xb8000004, "lo": 0x10920000}, 23, 3, 7, {"hi": 0xb8000001, "lo": 0x80000038}, false], // ----|---|[-]-
    [24, 2,  8, {"hi": 0xc0000001, "lo": 0x00000040}, {"hi": 0xc0000001, "lo": 0x30911200}, 24, 3, 7, {"hi": 0xc0000001, "lo": 0x80000038}, true],  // ---[|---|]---
    [25, 3,  7, {"hi": 0xc8000001, "lo": 0x80000038}, {"hi": 0xc8000001, "lo": 0xa0912000}, 25, 3, 7, {"hi": 0xc8000001, "lo": 0x80000038}, true],  // ----[---]----
];

function test_encodeRegionStrand() {
    var errors = 0;
    var i;
    var rk;
    for (i = 0; i < k_test_size; i++) {
        estrand = encodeRegionStrand(test_data[i][3]);
        if (estrand != test_data[i][5]) {
            console.error("(", i, "): Unexpected encoded strand: expected ", test_data[i][5], ", got ", estrand);
            ++errors;
        }
    }
    return errors;
}

function test_decodeRegionStrand() {
    var errors = 0;
    var i;
    var strand;
    for (i = 0; i < k_test_size; i++) {
        strand = decodeRegionStrand(test_data[i][5]);
        if (strand != test_data[i][3]) {
            console.error("(", i, "): Unexpected strand: expected ", test_data[i][3], ", got ", strand);
            ++errors;
        }
    }
    return errors;
}

function test_encodeRegionKey() {
    var errors = 0;
    var i;
    var rk;
    for (i = 0; i < k_test_size; i++) {
        rk = encodeRegionKey(test_data[i][4], test_data[i][1], test_data[i][2], test_data[i][5]);
        if ((rk.hi != test_data[i][6].hi) || (rk.lo != test_data[i][6].lo)) {
            console.error("(", i, "): Unexpected regionkey: expected ", test_data[i][6], ", got ", rk);
            ++errors;
        }
    }
    return errors;
}

function test_extractRegionKeyChrom() {
    var errors = 0;
    var i;
    var h;
    for (i = 0; i < k_test_size; i++) {
        h = extractRegionKeyChrom(test_data[i][6]);
        if (h != test_data[i][4]) {
            console.error("(", i, "): Unexpected chrom code: expected ", test_data[i][4], ", got ", h);
            ++errors;
        }
    }
    return errors;
}

function test_extractRegionKeyStartPos() {
    var errors = 0;
    var i;
    var h;
    for (i = 0; i < k_test_size; i++) {
        h = extractRegionKeyStartPos(test_data[i][6]);
        if (h != test_data[i][1]) {
            console.error("(", i, "): Unexpected START POS: expected ", test_data[i][1], ", got ", h);
            ++errors;
        }
    }
    return errors;
}

function test_extractRegionKeyEndPos() {
    var errors = 0;
    var i;
    var h;
    for (i = 0; i < k_test_size; i++) {
        h = extractRegionKeyEndPos(test_data[i][6]);
        if (h != test_data[i][2]) {
            console.error("(", i, "): Unexpected END POS: expected ", test_data[i][2], ", got ", h);
            ++errors;
        }
    }
    return errors;
}

function test_extractRegionKeyStrand() {
    var errors = 0;
    var i;
    var h;
    for (i = 0; i < k_test_size; i++) {
        h = extractRegionKeyStrand(test_data[i][6]);
        if (h != test_data[i][5]) {
            console.error("(", i, "): Unexpected STRAND code: expected ", test_data[i][5], ", got ", h);
            ++errors;
        }
    }
    return errors;
}


function test_decodeRegionKey() {
    var errors = 0;
    var i;
    var h;
    for (i = 0; i < k_test_size; i++) {
        h = decodeRegionKey(test_data[i][6]);
        if (h.chrom != test_data[i][4]) {
            console.error("(", i, "): Unexpected CHROM code: expected ", test_data[i][4], ", got ", h.chrom);
            ++errors;
        }
        if (h.startpos != test_data[i][1]) {
            console.error("(", i, "): Unexpected START POS: expected ", test_data[i][1], ", got ", h.startpos);
            ++errors;
        }
        if (h.endpos != test_data[i][2]) {
            console.error("(", i, "): Unexpected END POS: expected ", test_data[i][2], ", got ", h.endpos);
            ++errors;
        }
        if (h.strand != test_data[i][5]) {
            console.error("(", i, "): Unexpected STRAND code: expected ", test_data[i][5], ", got ", h.strand);
            ++errors;
        }
    }
    return errors;
}

function test_reverseRegionKey() {
    var errors = 0;
    var i;
    var h;
    for (i = 0; i < k_test_size; i++) {
        h = reverseRegionKey(test_data[i][6]);
        if (h.chrom != test_data[i][0]) {
            console.error("(", i, "): Unexpected CHROM: expected ", test_data[i][0], ", got ", h.chrom);
            ++errors;
        }
        if (h.startpos != test_data[i][1]) {
            console.error("(", i, "): Unexpected START POS: expected ", test_data[i][1], ", got ", h.startpos);
            ++errors;
        }
        if (h.endpos != test_data[i][2]) {
            console.error("(", i, "): Unexpected END POS: expected ", test_data[i][2], ", got ", h.endpos);
            ++errors;
        }
        if (h.strand != test_data[i][3]) {
            console.error("(", i, "): Unexpected STRAND: expected ", test_data[i][3], ", got ", h.strand);
            ++errors;
        }
    }
    return errors;
}

function test_regionKey() {
    var errors = 0;
    var i;
    var rk;
    for (i = 0; i < k_test_size; i++) {
        rk = regionKey(test_data[i][0], test_data[i][1], test_data[i][2], test_data[i][3]);
        if ((rk.hi != test_data[i][6].hi) || (rk.lo != test_data[i][6].lo)) {
            console.error("(", i, "): Unexpected regionkey: expected ", test_data[i][6], ", got ", rk);
            ++errors;
        }
    }
    return errors;
}

function test_extendRegionKey() {
    var errors = 0;
    var rk = regionKey("X", 10000, 20000, -1);
    var erk = extendRegionKey(rk, 1000);
    var drk = decodeRegionKey(erk);
    if ((drk.startpos != 9000) || (drk.endpos != 21000)) {
        console.error("Expected region 9000:21000, got ", drk.startpos, ":", drk.endpos);
        ++errors;
    }
    erk = extendRegionKey(rk, 300000000);
    drk = decodeRegionKey(erk);
    if ((drk.startpos != 0) || (drk.endpos != 268435455)) {
        console.error("Expected region 0:268435455, got ", drk.startpos, ":", drk.endpos);
        ++errors;
    }
    return errors;
}

function test_regionKeyString() {
    var errors = 0;
    var rs = "";
    var i;
    for (i = 0; i < k_test_size; i++) {
        rs = regionKeyString(test_data[i][6]);
        if (rs !== test_data[i][7]) {
            console.error("(", i, "): Unexpected regionkey: expected ", test_data[i][7], ", got ", rs);
            ++errors;
        }
    }
    return errors;
}

function test_getVariantKeyEndPos() {
    var errors = 0;
    var h;
    var i;
    for (i = 0; i < k_test_overlap_size; i++) {
        h = getVariantKeyEndPos(test_overlap[i][4]);
        if (h !== test_overlap[i][2]) {
            console.error("(", i, "): Expected ", test_overlap[i][2], ", got ", h);
            ++errors;
        }
    }
    return errors;
}

function test_areOverlappingRegions() {
    var errors = 0;
    var h;
    var i;
    for (i = 0; i < k_test_overlap_size; i++) {
        h = areOverlappingRegions(test_overlap[i][0], test_overlap[i][1], test_overlap[i][2], test_overlap[i][5], test_overlap[i][6], test_overlap[i][7]);
        if (h !== test_overlap[i][9]) {
            console.error("(", i, "): Expected ", test_overlap[i][9], ", got ", h);
            ++errors;
        }
    }
    return errors;
}

function test_areOverlappingRegionRegionKey() {
    var errors = 0;
    var h;
    var i;
    for (i = 0; i < k_test_overlap_size; i++) {
        h = areOverlappingRegionRegionKey(test_overlap[i][0], test_overlap[i][1], test_overlap[i][2], test_overlap[i][8]);
        if (h !== test_overlap[i][9]) {
            console.error("(", i, "): Expected ", test_overlap[i][9], ", got ", h);
            ++errors;
        }
    }
    return errors;
}

function test_areOverlappingRegionKeys() {
    var errors = 0;
    var h;
    var i;
    for (i = 0; i < k_test_overlap_size; i++) {
        h = areOverlappingRegionKeys(test_overlap[i][3], test_overlap[i][8]);
        if (h !== test_overlap[i][9]) {
            console.error("(", i, "): Expected ", test_overlap[i][9], ", got ", h);
            ++errors;
        }
    }
    return errors;
}

function test_areOverlappingVariantKeyRegionKey() {
    var errors = 0;
    var h;
    var i;
    for (i = 0; i < k_test_overlap_size; i++) {
        h = areOverlappingVariantKeyRegionKey(test_overlap[i][4], test_overlap[i][8]);
        if (h !== test_overlap[i][9]) {
            console.error("(", i, "): Expected ", test_overlap[i][9], ", got ", h);
            ++errors;
        }
    }
    return errors;
}

function test_variantKeyToRegionKey() {
    var errors = 0;
    var rk;
    var i;
    for (i = 0; i < k_test_overlap_size; i++) {
        rk = variantKeyToRegionKey(test_overlap[i][4]);
        if ((rk.hi != test_overlap[i][3].hi) || (rk.lo != test_overlap[i][3].lo)) {
            console.error("(", i, "): Unexpected regionkey: expected ", test_overlap[i][3], ", got ", rk);
            ++errors;
        }
    }
    return errors;
}

var errors = 0;

errors += test_encodeRegionStrand();
errors += test_decodeRegionStrand();
errors += test_encodeRegionKey();
errors += test_extractRegionKeyChrom();
errors += test_extractRegionKeyStartPos();
errors += test_extractRegionKeyEndPos();
errors += test_extractRegionKeyStrand();
errors += test_decodeRegionKey();
errors += test_reverseRegionKey();
errors += test_regionKey();
errors += test_extendRegionKey();
errors += test_regionKeyString();
errors += test_getVariantKeyEndPos();
errors += test_areOverlappingRegions();
errors += test_areOverlappingRegionRegionKey();
errors += test_areOverlappingRegionKeys();
errors += test_areOverlappingVariantKeyRegionKey();
errors += test_variantKeyToRegionKey();

if (errors > 0) {
    console.log("FAILED: " + errors);
    process.exit(1);
} else {
    console.log("OK");
}
