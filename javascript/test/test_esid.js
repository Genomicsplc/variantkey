/** VariantKey Javascript Library Test
 * 
 * test_esid.js
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
    encodeStringID,
    decodeStringID,
} = require(process.argv[2]);

var k_test_size = 36;
// 0     1      2      3     4     5     6
// size, start, esize, esid, hsid, estr, str
var test_data = [
    [36,  0, 10, {"hi": 0xa4114935, "lo": 0x15597619}, {"hi": 0xb3a5fdb8, "lo": 0x808cb7dc}, "0123456789", "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ"],
    [36,  1, 10, {"hi": 0xa4524d45, "lo": 0x565d8661}, {"hi": 0xb3a5fdb8, "lo": 0x808cb7dc}, "123456789A", "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ"],
    [36, 10, 10, {"hi": 0xa8628e49, "lo": 0x669e8a6a}, {"hi": 0xd93e3820, "lo": 0x10f46f32}, "ABCDEFGHIJ", "0223456789ABCDEFGHIJKLMNOPQRSTUVWXYZ"],
    [36, 25, 10, {"hi": 0xac31cb3d, "lo": 0x35db7e39}, {"hi": 0xbfc379f4, "lo": 0xa71cb3be}, "PQRSTUVWXY", "0133456789ABCDEFGHIJKLMNOPQRSTUVWXYZ"],
    [36, 26, 10, {"hi": 0xac72cf4d, "lo": 0x76df8e7a}, {"hi": 0xdba299d0, "lo": 0x6b54215d}, "QRSTUVWXYZ", "1123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ"],
    [36, 35,  1, {"hi": 0x1ec00000, "lo": 0x00000000}, {"hi": 0x996dcdc8, "lo": 0xef7c89c1}, "[",          "0123456789ABCDEFGHIJKLMNOPQRSTUVWXY["],
    [ 6,  0,  6, {"hi": 0x64114935, "lo": 0x15000000}, {"hi": 0xa8127939, "lo": 0x5d9c891e}, "012345",     "012345"                              ],
    [ 6,  1,  5, {"hi": 0x54524d45, "lo": 0x40000000}, {"hi": 0xa8127939, "lo": 0x5d9c891e}, "12345",      "012345"                              ],
    [ 6,  3,  3, {"hi": 0x34d45400, "lo": 0x00000000}, {"hi": 0xa8127939, "lo": 0x5d9c891e}, "345",        "012345"                              ],
    [ 6,  5,  1, {"hi": 0x15400000, "lo": 0x00000000}, {"hi": 0xa8127939, "lo": 0x5d9c891e}, "5",          "012345"                              ],
    [10,  0, 10, {"hi": 0xafc10831, "lo": 0x05187209}, {"hi": 0x838c1480, "lo": 0x28a06998}, "_!\"#$%&'()"," !\"#$%&'()"                         ],
    [10,  0, 10, {"hi": 0xa28b30d3, "lo": 0x8f411493}, {"hi": 0x96e42cf7, "lo": 0xaefeccf0}, "*+,-./0123", "*+,-./0123"                          ],
    [10,  0, 10, {"hi": 0xa5155976, "lo": 0x1969b71d}, {"hi": 0xb391c7f2, "lo": 0x88e446ee}, "456789:;<=", "456789:;<="                          ],
    [10,  0, 10, {"hi": 0xa79f8218, "lo": 0xa39259a7}, {"hi": 0xcc0de203, "lo": 0x81405e0a}, ">?@ABCDEFG", ">?@ABCDEFG"                          ],
    [10,  0, 10, {"hi": 0xaa29aabb, "lo": 0x2dbafc31}, {"hi": 0xbbfc630e, "lo": 0xacb6c552}, "HIJKLMNOPQ", "HIJKLMNOPQ"                          ],
    [10,  0, 10, {"hi": 0xacb3d35d, "lo": 0xb7e39ebb}, {"hi": 0x803eda0a, "lo": 0x1781a117}, "RSTUVWXYZ[", "RSTUVWXYZ["                          ],
    [10,  0, 10, {"hi": 0xaf3dfbf8, "lo": 0x218a3925}, {"hi": 0x878ef294, "lo": 0x7ee3cc2b}, "\\]^_@ABCDE","\\]^_`abcde"                         ],
    [10,  0, 10, {"hi": 0xa9a7a29a, "lo": 0xabb2dbaf}, {"hi": 0xbaadaef0, "lo": 0x7826969b}, "FGHIJKLMNO", "fghijklmno"                          ],
    [10,  0, 10, {"hi": 0xac31cb3d, "lo": 0x35db7e39}, {"hi": 0xd3da4d5e, "lo": 0x28be3590}, "PQRSTUVWXY", "pqrstuvwxy"                          ],
    [ 6,  0,  6, {"hi": 0x6ebbf3df, "lo": 0xbf000000}, {"hi": 0xeadc752a, "lo": 0x50c5b850}, "Z[\\]^_",    "z{|}~\t"                             ],
    [15,  0, 10, {"hi": 0xa4524d45, "lo": 0x565d8452}, {"hi": 0x9af6ee55, "lo": 0x3ba41827}, "1234567812", "123456781234567"                     ],
    [14,  0, 10, {"hi": 0xa4524d45, "lo": 0x565d8452}, {"hi": 0xc94e10e9, "lo": 0xfe153fd2}, "1234567812", "12345678123456"                      ],
    [13,  0, 10, {"hi": 0xa4524d45, "lo": 0x565d8452}, {"hi": 0x9b56411c, "lo": 0x7abdefe6}, "1234567812", "1234567812345"                       ],
    [12,  0, 10, {"hi": 0xa4524d45, "lo": 0x565d8452}, {"hi": 0xcbec3594, "lo": 0x4ff1c863}, "1234567812", "123456781234"                        ],
    [11,  0, 10, {"hi": 0xa4524d45, "lo": 0x565d8452}, {"hi": 0x92992029, "lo": 0x8cf56b9b}, "1234567812", "12345678123"                         ],
    [10,  0, 10, {"hi": 0xa4524d45, "lo": 0x565d8452}, {"hi": 0xbdf006d5, "lo": 0x0a33ad90}, "1234567812", "1234567812"                          ],
    [ 9,  0,  9, {"hi": 0x94524d45, "lo": 0x565d8440}, {"hi": 0xde094182, "lo": 0xe93557ae}, "123456781",  "123456781"                           ],
    [ 8,  0,  8, {"hi": 0x84524d45, "lo": 0x565d8000}, {"hi": 0xccbc926a, "lo": 0x73ece95c}, "12345678",   "12345678"                            ],
    [ 7,  0,  7, {"hi": 0x74524d45, "lo": 0x565c0000}, {"hi": 0xd18b960e, "lo": 0x2f99c279}, "1234567",    "1234567"                             ],
    [ 6,  0,  6, {"hi": 0x64524d45, "lo": 0x56000000}, {"hi": 0x811c9c02, "lo": 0xfcc22096}, "123456",     "123456"                              ],
    [ 5,  0,  5, {"hi": 0x54524d45, "lo": 0x40000000}, {"hi": 0xd863467d, "lo": 0xedd1cab1}, "12345",      "12345"                               ],
    [ 4,  0,  4, {"hi": 0x44524d40, "lo": 0x00000000}, {"hi": 0xbef404ec, "lo": 0xb71e4cd9}, "1234",       "1234"                                ],
    [ 3,  0,  3, {"hi": 0x34524c00, "lo": 0x00000000}, {"hi": 0xfad47064, "lo": 0x4116fa54}, "123",        "123"                                 ],
    [ 2,  0,  2, {"hi": 0x24520000, "lo": 0x00000000}, {"hi": 0xd94ae1e7, "lo": 0x173e781d}, "12",         "12"                                  ],
    [ 1,  0,  1, {"hi": 0x14400000, "lo": 0x00000000}, {"hi": 0xfc73100b, "lo": 0xaa96ad81}, "1",          "1"                                   ],
    [ 0,  0,  0, {"hi": 0x00000000, "lo": 0x00000000}, {"hi": 0x80000000, "lo": 0x00000000}, "",           ""                                    ],
];

function test_encodeStringID() {
    var errors = 0;
    var i;
    var rk;
    for (i = 0; i < k_test_size; i++) {
        h = encodeStringID(test_data[i][6], test_data[i][1]);
        if ((h.hi != test_data[i][3].hi) || (h.lo != test_data[i][3].lo)) {
            console.error("(", i, "): Unexpected ESID: expected ", test_data[i][3], ", got ", h);
            ++errors;
        }
    }
    return errors;
}

function test_decodeStringID() {
    var errors = 0;
    var i;
    var rk;
    for (i = 0; i < k_test_size; i++) {
        h = decodeStringID(test_data[i][3]);
        if (h != test_data[i][5]) {
            console.error("(", i, "): Unexpected decoded ESID: expected ", test_data[i][5], ", got ", h);
            ++errors;
        }
    }
    return errors;
}

var errors = 0;

errors += test_encodeStringID();
errors += test_decodeStringID();

if (errors > 0) {
    console.log("FAILED: " + errors);
    process.exit(1);
} else {
    console.log("OK");
}
