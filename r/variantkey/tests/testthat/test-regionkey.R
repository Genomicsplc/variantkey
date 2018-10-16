context("variantkey")
library(variantkey)

# test-regionkey.R
# @category   Libraries
# @author     Nicola Asuni <nicola.asuni@genomicsplc.com>
# @copyright  2017-2018 GENOMICS plc
# @license    MIT (see LICENSE)
# @link       https://github.com/genomicsplc/variantkey

x <- rbind(
    list("1", 1000, 1100,  0,  1, 0,  "080001f400002260", "00000000100003e8", "000000001000044c"),
    list("2", 1001, 1201,  1,  2, 1,  "100001f48000258a", "00000000200003e9", "00000000200004b1"),
    list("3", 1002, 1302, -1,  3, 2,  "180001f5000028b4", "00000000300003ea", "0000000030000516"),
    list("4", 1003, 1403,  0,  4, 0,  "200001f580002bd8", "00000000400003eb", "000000004000057b"),
    list("5", 1004, 1504,  1,  5, 1,  "280001f600002f02", "00000000500003ec", "00000000500005e0"),
    list("10", 1005, 1605, -1, 10, 2, "500001f68000322c", "00000000a00003ed", "00000000a0000645"),
    list("22", 1006, 1706,  0, 22, 0, "b00001f700003550", "00000001600003ee", "00000001600006aa"),
    list("X", 1007, 1807,  1, 23, 1,  "b80001f78000387a", "00000001700003ef", "000000017000070f"),
    list("Y", 1008, 1908, -1, 24, 2,  "c00001f800003ba4", "00000001800003f0", "0000000180000774"),
    list("MT", 1009, 2009,  0, 25, 0, "c80001f880003ec8", "00000001900003f1", "00000001900007d9")
)
colnames(x) <- list("chrom", "startpos", "endpos", "strand", "echrom", "estrand", "rk", "csp", "cep")

t <- rbind(
    list(0,  1,  2, 5, 5,  7, 7, "0800000280000038", "0800000290920000", "1000000280000038"), # different chromosome
    list(0,  1,  1, 0, 3,  2, 7, "0800000000000010", "0800000010920000", "0800000180000038"), # -[-]|---|----
    list(0,  2,  2, 1, 3,  3, 7, "1000000080000018", "1000000090920000", "1000000180000038"), # --[-]---|----
    list(1,  3,  3, 2, 3,  4, 7, "1800000100000020", "1800000110920000", "1800000180000038"), # ---[|]--|----
    list(1,  4,  4, 3, 3,  5, 7, "2000000180000028", "2000000190920000", "2000000180000038"), # ----[-]-|----
    list(1,  5,  5, 4, 3,  6, 7, "2800000200000030", "2800000210920000", "2800000180000038"), # ----|[-]|----
    list(1,  6,  6, 5, 3,  7, 7, "3000000280000038", "3000000290920000", "3000000180000038"), # ----|-[ ]----
    list(1, 10, 10, 6, 3,  8, 7, "5000000300000040", "5000000310920000", "5000000180000038"), # ----|--[|]---
    list(0, 22, 22, 7, 3,  9, 7, "b000000380000048", "b000000390920000", "b000000180000038"), # ----|---[-]--
    list(0, 23, 23, 8, 3, 10, 7, "b800000400000050", "b800000410920000", "b800000180000038"), # ----|---|[-]-
    list(1, 24, 24, 2, 3,  8, 7, "c000000100000040", "c000000130911200", "c000000180000038"), # ---[|---|]---
    list(1, 25, 25, 3, 3,  7, 7, "c800000180000038", "c8000001a0912000", "c800000180000038")  # ----[---]----
)
colnames(t) <- list("exp", "a_chrom", "b_chrom", "a_startpos", "b_startpos", "a_endpos", "b_endpos", "a_rk", "a_vk", "b_rk")

test_that("EncodeRegionStrand", {
    res <- EncodeRegionStrand(unlist(x[,"strand"]))
    expect_that(res, equals(unlist(x[,"estrand"])))
})

test_that("DecodeRegionStrand", {
    res <- DecodeRegionStrand(unlist(x[,"estrand"]))
    expect_that(res, equals(unlist(x[,"strand"])))
})

test_that("EncodeRegionKey", {
    res <- EncodeRegionKey(unlist(x[,"echrom"]), unlist(x[,"startpos"]), unlist(x[,"endpos"]), unlist(x[,"estrand"]))
    expect_identical(res, as.uint64(as.hex64(unlist(x[,"rk"]))))
})

test_that("ExtractRegionKeyChrom", {
    res <- ExtractRegionKeyChrom(as.hex64(unlist(x[,"rk"])))
    expect_that(res, equals(unlist(x[,"echrom"])))
})

test_that("ExtractRegionKeyStartPos", {
    res <- ExtractRegionKeyStartPos(as.hex64(unlist(x[,"rk"])))
    expect_that(res, equals(unlist(x[,"startpos"])))
})

test_that("ExtractRegionKeyEndPos", {
    res <- ExtractRegionKeyEndPos(as.hex64(unlist(x[,"rk"])))
    expect_that(res, equals(unlist(x[,"endpos"])))
})

test_that("ExtractRegionKeyStrand", {
    res <- ExtractRegionKeyStrand(as.hex64(unlist(x[,"rk"])))
    expect_that(res, equals(unlist(x[,"estrand"])))
})

test_that("DecodeRegionKey", {
    res <- DecodeRegionKey(as.hex64(unlist(x[,"rk"])))
    expect_that(res$CHROM, equals(unlist(x[,"echrom"])))
    expect_that(res$STARTPOS, equals(unlist(x[,"startpos"])))
    expect_that(res$ENDPOS, equals(unlist(x[,"endpos"])))
    expect_that(res$STRAND, equals(unlist(x[,"estrand"])))
})

test_that("ReverseRegionKey", {
    res <- ReverseRegionKey(as.hex64(unlist(x[,"rk"])))
    expect_that(res$CHROM, equals(unlist(x[,"chrom"])))
    expect_that(res$STARTPOS, equals(unlist(x[,"startpos"])))
    expect_that(res$ENDPOS, equals(unlist(x[,"endpos"])))
    expect_that(res$STRAND, equals(unlist(x[,"strand"])))
})

test_that("RegionKey", {
    res <- RegionKey(unlist(x[,"chrom"]), unlist(x[,"startpos"]), unlist(x[,"endpos"]), unlist(x[,"strand"]))
    expect_identical(res, as.uint64(as.hex64(unlist(x[,"rk"]))))
})

test_that("ExtendRegionKey", {
    rk <- RegionKey("X", 10000, 20000, -1)
    res <- ExtendRegionKey(rk, 1000)
    expect_identical(res, as.uint64("13258616630331740228"))
    res <- ExtendRegionKey(rk, 300000000)
    expect_identical(res, as.uint64("13258597305126223868"))
})

test_that("GetRegionKeyChromStartPos", {
    res <- GetRegionKeyChromStartPos(as.hex64(unlist(x[,"rk"])))
    expect_that(res, equals(as.uint64(as.hex64(unlist(x[,"csp"])))))
})

test_that("GetRegionKeyChromEndPos", {
    res <- GetRegionKeyChromEndPos(as.hex64(unlist(x[,"rk"])))
    expect_identical(res, as.uint64(as.hex64(unlist(x[,"cep"]))))
})

test_that("AreOverlappingRegions", {
    res <- AreOverlappingRegions(unlist(t[,"a_chrom"]), unlist(t[,"a_startpos"]), unlist(t[,"a_endpos"]), unlist(t[,"b_chrom"]), unlist(t[,"b_startpos"]), unlist(t[,"b_endpos"]))
    expect_that(res, equals(unlist(t[,"exp"])))
})

test_that("AreOverlappingRegionRegionKey", {
    res <- AreOverlappingRegionRegionKey(unlist(t[,"a_chrom"]), unlist(t[,"a_startpos"]), unlist(t[,"a_endpos"]), as.hex64(unlist(t[,"b_rk"])))
    expect_that(res, equals(unlist(t[,"exp"])))
})

test_that("AreOverlappingRegionKeys", {
    res <- AreOverlappingRegionKeys(as.hex64(unlist(t[,"a_rk"])), as.hex64(unlist(t[,"b_rk"])))
    expect_that(res, equals(unlist(t[,"exp"])))
})

test_that("AreOverlappingVariantKeyRegionKey", {
    res <- AreOverlappingVariantKeyRegionKey(mc = NULL, as.hex64(unlist(t[,"a_vk"])), as.hex64(unlist(t[,"b_rk"])))
    expect_that(res, equals(unlist(t[,"exp"])))
})

test_that("VariantToRegionkey", {
    res <- VariantToRegionkey(mc = NULL, as.hex64(unlist(t[,"a_vk"])))
    expect_identical(res, as.uint64(as.hex64(unlist(t[,"a_rk"]))))
})
