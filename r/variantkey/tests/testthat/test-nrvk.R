context("variantkey")
library(variantkey)

# test-nrvk.R
# @category   Libraries
# @author     Nicola Asuni <nicola.asuni@genomicsplc.com>
# @copyright  2017-2018 GENOMICS plc
# @license    MIT (see LICENSE)
# @link       https://github.com/genomicsplc/variantkey

x <- rbind(
    list("0800c35093ace339",  "1", 100001, 0x00000004, 0x01, 0x01, "00000000100186a1", "00000000100186a2", "N", "A"),
    list("1000c3517f91cdb1",  "2", 100002, 0x0000000e, 0x0b, 0x01, "00000000200186a2", "00000000200186ad", "AAGAAAGAAAG", "A"),
    list("1800c351f61f65d3",  "3", 100003, 0x0000000e, 0x01, 0x0b, "00000000300186a3", "00000000300186a4", "A", "AAGAAAGAAAG"),
    list("2000c3521f1c15ab",  "4", 100004, 0x0000000e, 0x08, 0x04, "00000000400186a4", "00000000400186ac", "ACGTACGT", "ACGT"),
    list("2800c352d8f2d5b5",  "5", 100005, 0x0000000e, 0x04, 0x08, "00000000500186a5", "00000000500186a9", "ACGT", "ACGTACGT"),
    list("5000c3553bbf9c19", "10", 100010, 0x00000012, 0x08, 0x08, "00000000a00186aa", "00000000a00186b2", "ACGTACGT", "CGTACGTA"),
    list("b000c35b64690b25", "22", 100022, 0x0000000b, 0x08, 0x01, "00000001600186b6", "00000001600186be", "ACGTACGT", "N"),
    list("b800c35bbcece603",  "X", 100023, 0x0000000e, 0x0a, 0x02, "00000001700186b7", "00000001700186c1", "AAAAAAAAGG", "AG"),
    list("c000c35c63741ee7",  "Y", 100024, 0x0000000e, 0x02, 0x0a, "00000001800186b8", "00000001800186ba", "AG", "AAAAAAAAGG"),
    list("c800c35c96c18499", "MT", 100025, 0x00000012, 0x04, 0x0c, "00000001900186b9", "00000001900186bd", "ACGT", "AAACCCGGGTTT")
)
colnames(x) <- list("vk", "chrom", "pos", "ralen", "sizeref", "sizealt", "csp", "cep", "ref", "alt")

test_that("MmapBinfile", {
    nrvk <<- MmapNRVKFile("../../../../c/test/data/nrvk.10.bin")
    expect_that(nrvk$NROWS, equals(10))
})

test_that("FindRefAltByVariantKey", {
    res <- mapply(FindRefAltByVariantKey, vk = unlist(x[,"vk"]), MoreArgs = list(mc = nrvk$MC), SIMPLIFY = TRUE, USE.NAMES = FALSE)
    expect_that(unlist(res[1,]), equals(unlist(x[,"ref"])))
    expect_that(unlist(res[2,]), equals(unlist(x[,"alt"])))
    expect_that(unlist(res[3,]), equals(unlist(x[,"sizeref"])))
    expect_that(unlist(res[4,]), equals(unlist(x[,"sizealt"])))
    expect_that(unlist(res[5,]), equals(unlist(x[,"ralen"]) - 2))
})

test_that("ReverseVariantKey", {
    res <- mapply(ReverseVariantKey, vk = unlist(x[,"vk"]), MoreArgs = list(mc = nrvk$MC), SIMPLIFY = TRUE, USE.NAMES = FALSE)
    expect_that(unlist(res[1,]), equals(unlist(x[,"chrom"])))
    expect_that(unlist(res[2,]), equals(unlist(x[,"pos"])))
    expect_that(unlist(res[3,]), equals(unlist(x[,"ref"])))
    expect_that(unlist(res[4,]), equals(unlist(x[,"alt"])))
    expect_that(unlist(res[5,]), equals(unlist(x[,"sizeref"])))
    expect_that(unlist(res[6,]), equals(unlist(x[,"sizealt"])))
    expect_that(unlist(res[7,]), equals(unlist(x[,"ralen"]) - 2))
})

test_that("GetVariantKeyRefLength", {
    res <- mapply(GetVariantKeyRefLength, vk = unlist(x[,"vk"]), MoreArgs = list(mc = nrvk$MC), SIMPLIFY = TRUE, USE.NAMES = FALSE)
    expect_that(unlist(res), equals(unlist(x[,"sizeref"])))
})

test_that("GetVariantKeyRefLengthReversible", {
    res <- GetVariantKeyRefLength(nrvk$MC, "1800925199160000")
    expect_that(res, equals(3))
})

test_that("GetVariantKeyRefLengthNotFound", {
    res <- GetVariantKeyRefLength(nrvk$MC, "ffffffffffffffff")
    expect_that(res, equals(0))
})

test_that("GetVariantKeyEndPos", {
    res <- mapply(GetVariantKeyEndPos, vk = unlist(x[,"vk"]), MoreArgs = list(mc = nrvk$MC), SIMPLIFY = TRUE, USE.NAMES = FALSE)
    expect_that(unlist(res), equals(unlist(x[,"pos"]) + unlist(x[,"sizeref"])))
})

test_that("GetVariantKeyChromStartPos", {
    res <- mapply(GetVariantKeyChromStartPos, vk = unlist(x[,"vk"]), SIMPLIFY = TRUE, USE.NAMES = FALSE)
    expect_that(unlist(res), equals(unlist(x[,"csp"])))
})

test_that("GetVariantKeyChromEndPos", {
    res <- mapply(GetVariantKeyChromEndPos, vk = unlist(x[,"vk"]), MoreArgs = list(mc = nrvk$MC), SIMPLIFY = TRUE, USE.NAMES = FALSE)
    expect_that(unlist(res), equals(unlist(x[,"cep"])))
})

test_that("VknrBinToTsv", {
    size <- VknrBinToTsv(nrvk$MC, "nrvk.test")
    expect_that(size, equals(305))
})

test_that("MunmapBinfile", {
    err <- MunmapBinfile(nrvk$MF)
    expect_that(err, equals(0))
})
