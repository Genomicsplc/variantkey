context("variantkey")
library(variantkey)

# test-esid.R
# @category   Libraries
# @author     Nicola Asuni <nicola.asuni@genomicsplc.com>
# @copyright  2017-2018 GENOMICS plc
# @license    MIT (see LICENSE)
# @link       https://github.com/genomicsplc/variantkey

x <- rbind(
    list("0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ", 36,  0, "0411493515597619", "0123456789",  10, "b3a5fdb8808cb7dc"),
    list("0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ", 36,  1, "04524d45565d8661", "123456789A",  10, "b3a5fdb8808cb7dc"),
    list("0223456789ABCDEFGHIJKLMNOPQRSTUVWXYZ", 36, 10, "08628e49669e8a6a", "ABCDEFGHIJ",  10, "d93e382010f46f32"),
    list("0133456789ABCDEFGHIJKLMNOPQRSTUVWXYZ", 36, 25, "0c31cb3d35db7e39", "PQRSTUVWXY",  10, "bfc379f4a71cb3be"),
    list("1123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ", 36, 26, "0c72cf4d76df8e7a", "QRSTUVWXYZ",  10, "dba299d06b54215d"),
    list("0123456789ABCDEFGHIJKLMNOPQRSTUVWXY[", 36, 35, "0ec0000000000000", "[",            1, "996dcdc8ef7c89c1"),
    list("012345",                                6,  0, "0411493515000000", "012345",       6, "a81279395d9c891e"),
    list("012345",                                6,  1, "04524d4540000000", "12345",        5, "a81279395d9c891e"),
    list("012345",                                6,  3, "04d4540000000000", "345",          3, "a81279395d9c891e"),
    list("012345",                                6,  5, "0540000000000000", "5",            1, "a81279395d9c891e"),
    list(" !\"#$%&'()",                          10,  0, "0fc1083105187209", "_!\"#$%&'()", 10, "838c148028a06998"),
    list("*+,-./0123",                           10,  0, "028b30d38f411493", "*+,-./0123",  10, "96e42cf7aefeccf0"),
    list("456789:;<=",                           10,  0, "051559761969b71d", "456789:;<=",  10, "b391c7f288e446ee"),
    list(">?@ABCDEFG",                           10,  0, "079f8218a39259a7", ">?@ABCDEFG",  10, "cc0de20381405e0a"),
    list("HIJKLMNOPQ",                           10,  0, "0a29aabb2dbafc31", "HIJKLMNOPQ",  10, "bbfc630eacb6c552"),
    list("RSTUVWXYZ[",                           10,  0, "0cb3d35db7e39ebb", "RSTUVWXYZ[",  10, "803eda0a1781a117"),
    list("\\]^_`abcde",                          10,  0, "0f3dfbf8218a3925", "\\]^_@ABCDE", 10, "878ef2947ee3cc2b"),
    list("fghijklmno",                           10,  0, "09a7a29aabb2dbaf", "FGHIJKLMNO",  10, "baadaef07826969b"),
    list("pqrstuvwxy",                           10,  0, "0c31cb3d35db7e39", "PQRSTUVWXY",  10, "d3da4d5e28be3590"),
    list("z{|}~\t",                               6,  0, "0ebbf3dfbf000000", "Z[\\]^_",      6, "eadc752a50c5b850"),
    list("123456781234567",                      15,  0, "04524d45565d8452", "1234567812",  10, "9af6ee553ba41827"),
    list("12345678123456",                       14,  0, "04524d45565d8452", "1234567812",  10, "c94e10e9fe153fd2"),
    list("1234567812345",                        13,  0, "04524d45565d8452", "1234567812",  10, "9b56411c7abdefe6"),
    list("123456781234",                         12,  0, "04524d45565d8452", "1234567812",  10, "cbec35944ff1c863"),
    list("12345678123",                          11,  0, "04524d45565d8452", "1234567812",  10, "929920298cf56b9b"),
    list("1234567812",                           10,  0, "04524d45565d8452", "1234567812",  10, "bdf006d50a33ad90"),
    list("123456781",                             9,  0, "04524d45565d8440", "123456781",    9, "de094182e93557ae"),
    list("12345678",                              8,  0, "04524d45565d8000", "12345678",     8, "ccbc926a73ece95c"),
    list("1234567",                               7,  0, "04524d45565c0000", "1234567",      7, "d18b960e2f99c279"),
    list("123456",                                6,  0, "04524d4556000000", "123456",       6, "811c9c02fcc22096"),
    list("12345",                                 5,  0, "04524d4540000000", "12345",        5, "d863467dedd1cab1"),
    list("1234",                                  4,  0, "04524d4000000000", "1234",         4, "bef404ecb71e4cd9"),
    list("123",                                   3,  0, "04524c0000000000", "123",          3, "fad470644116fa54"),
    list("12",                                    2,  0, "0452000000000000", "12",           2, "d94ae1e7173e781d"),
    list("1",                                     1,  0, "0440000000000000", "1",            1, "fc73100baa96ad81"),
    list("",                                      0,  0, "0000000000000000", "",             0, "8000000000000000")
)
colnames(x) <- list("str", "size", "start", "esid", "estr", "esize", "hsid")

test_that("EncodeStringID", {
    res <- mapply(EncodeStringID, unlist(x[,"str"]), unlist(x[,"start"]), SIMPLIFY = TRUE, USE.NAMES = FALSE)
    expect_that(res, equals(unlist(x[,"esid"])))
})

test_that("DecodeStringID", {
    res <- mapply(DecodeStringID, unlist(x[,"esid"]), SIMPLIFY = TRUE, USE.NAMES = FALSE)
    expect_that(res, equals(unlist(x[,"estr"])))
})

test_that("DecodeStringIDHash", {
    res <- mapply(DecodeStringID, unlist(x[,"hsid"]), SIMPLIFY = TRUE, USE.NAMES = FALSE)
    expect_that(res, equals(rep("", 36)))
})

test_that("HashStringID", {
    res <- mapply(HashStringID, unlist(x[,"str"]), SIMPLIFY = TRUE, USE.NAMES = FALSE)
    expect_that(res, equals(unlist(x[,"hsid"])))
})
