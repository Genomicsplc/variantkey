context("variantkey")
library(variantkey)

# test-esid.R
# @category   Libraries
# @author     Nicola Asuni <nicola.asuni@genomicsplc.com>
# @copyright  2017-2018 GENOMICS plc
# @license    MIT (see LICENSE)
# @link       https://github.com/genomicsplc/variantkey

x <- rbind(
    list(36,  0, 10, "a411493515597619", "b3a5fdb8808cb7dc", "0123456789", "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ"),
    list(36,  1, 10, "a4524d45565d8661", "b3a5fdb8808cb7dc", "123456789A", "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ"),
    list(36, 10, 10, "a8628e49669e8a6a", "d93e382010f46f32", "ABCDEFGHIJ", "0223456789ABCDEFGHIJKLMNOPQRSTUVWXYZ"),
    list(36, 25, 10, "ac31cb3d35db7e39", "bfc379f4a71cb3be", "PQRSTUVWXY", "0133456789ABCDEFGHIJKLMNOPQRSTUVWXYZ"),
    list(36, 26, 10, "ac72cf4d76df8e7a", "dba299d06b54215d", "QRSTUVWXYZ", "1123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ"),
    list(36, 35,  1, "1ec0000000000000", "996dcdc8ef7c89c1", "[",          "0123456789ABCDEFGHIJKLMNOPQRSTUVWXY["),
    list( 6,  0,  6, "6411493515000000", "a81279395d9c891e", "012345",     "012345"                             ),
    list( 6,  1,  5, "54524d4540000000", "a81279395d9c891e", "12345",      "012345"                             ),
    list( 6,  3,  3, "34d4540000000000", "a81279395d9c891e", "345",        "012345"                             ),
    list( 6,  5,  1, "1540000000000000", "a81279395d9c891e", "5",          "012345"                             ),
    list(10,  0, 10, "afc1083105187209", "838c148028a06998", "_!\"#$%&'()"," !\"#$%&'()"                        ),
    list(10,  0, 10, "a28b30d38f411493", "96e42cf7aefeccf0", "*+,-./0123", "*+,-./0123"                         ),
    list(10,  0, 10, "a51559761969b71d", "b391c7f288e446ee", "456789:;<=", "456789:;<="                         ),
    list(10,  0, 10, "a79f8218a39259a7", "cc0de20381405e0a", ">?@ABCDEFG", ">?@ABCDEFG"                         ),
    list(10,  0, 10, "aa29aabb2dbafc31", "bbfc630eacb6c552", "HIJKLMNOPQ", "HIJKLMNOPQ"                         ),
    list(10,  0, 10, "acb3d35db7e39ebb", "803eda0a1781a117", "RSTUVWXYZ[", "RSTUVWXYZ["                         ),
    list(10,  0, 10, "af3dfbf8218a3925", "878ef2947ee3cc2b", "\\]^_@ABCDE","\\]^_`abcde"                        ),
    list(10,  0, 10, "a9a7a29aabb2dbaf", "baadaef07826969b", "FGHIJKLMNO", "fghijklmno"                         ),
    list(10,  0, 10, "ac31cb3d35db7e39", "d3da4d5e28be3590", "PQRSTUVWXY", "pqrstuvwxy"                         ),
    list( 6,  0,  6, "6ebbf3dfbf000000", "eadc752a50c5b850", "Z[\\]^_",    "z{|}~\t"                            ),
    list(15,  0, 10, "a4524d45565d8452", "9af6ee553ba41827", "1234567812", "123456781234567"                    ),
    list(14,  0, 10, "a4524d45565d8452", "c94e10e9fe153fd2", "1234567812", "12345678123456"                     ),
    list(13,  0, 10, "a4524d45565d8452", "9b56411c7abdefe6", "1234567812", "1234567812345"                      ),
    list(12,  0, 10, "a4524d45565d8452", "cbec35944ff1c863", "1234567812", "123456781234"                       ),
    list(11,  0, 10, "a4524d45565d8452", "929920298cf56b9b", "1234567812", "12345678123"                        ),
    list(10,  0, 10, "a4524d45565d8452", "bdf006d50a33ad90", "1234567812", "1234567812"                         ),
    list( 9,  0,  9, "94524d45565d8440", "de094182e93557ae", "123456781",  "123456781"                          ),
    list( 8,  0,  8, "84524d45565d8000", "ccbc926a73ece95c", "12345678",   "12345678"                           ),
    list( 7,  0,  7, "74524d45565c0000", "d18b960e2f99c279", "1234567",    "1234567"                            ),
    list( 6,  0,  6, "64524d4556000000", "811c9c02fcc22096", "123456",     "123456"                             ),
    list( 5,  0,  5, "54524d4540000000", "d863467dedd1cab1", "12345",      "12345"                              ),
    list( 4,  0,  4, "44524d4000000000", "bef404ecb71e4cd9", "1234",       "1234"                               ),
    list( 3,  0,  3, "34524c0000000000", "fad470644116fa54", "123",        "123"                                ),
    list( 2,  0,  2, "2452000000000000", "d94ae1e7173e781d", "12",         "12"                                 ),
    list( 1,  0,  1, "1440000000000000", "fc73100baa96ad81", "1",          "1"                                  ),
    list( 0,  0,  0, "0000000000000000", "8000000000000000", "",           ""                                   )
)
colnames(x) <- list("size", "start", "esize", "esid", "hsid", "estr", "str")

test_that("EncodeStringID", {
    res <- mapply(EncodeStringID, unlist(x[,"str"]), unlist(x[,"start"]), SIMPLIFY = TRUE, USE.NAMES = FALSE)
    expect_that(res, equals(unlist(x[,"esid"])))
})

test_that("DecodeStringID", {
    res <- mapply(DecodeStringID, unlist(x[,"esid"]), SIMPLIFY = TRUE, USE.NAMES = FALSE)
    expect_that(res, equals(unlist(x[,"estr"])))
})

test_that("DecodeStringIDError", {
    res <- DecodeStringID("ffffffffffffffff")
    expect_that(res, equals(""))
})

test_that("HashStringID", {
    res <- mapply(HashStringID, unlist(x[,"str"]), SIMPLIFY = TRUE, USE.NAMES = FALSE)
    expect_that(res, equals(unlist(x[,"hsid"])))
})
