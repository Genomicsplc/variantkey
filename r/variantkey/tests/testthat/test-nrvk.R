context("variantkey")
library(variantkey)

# test-nrvk.R
# @category   Libraries
# @author     Nicola Asuni <nicola.asuni@genomicsplc.com>
# @copyright  2017-2018 GENOMICS plc
# @license    MIT (see LICENSE)
# @link       https://github.com/genomicsplc/variantkey

x <- rbind(
    list("0800c35093ace339",  "1", 100001, 0x00000004, 0x01, 0x01, "N", "A"),
    list("1000c3517f91cdb1",  "2", 100002, 0x0000000e, 0x0b, 0x01, "AAGAAAGAAAG", "A"),
    list("1800c351f61f65d3",  "3", 100003, 0x0000000e, 0x01, 0x0b, "A", "AAGAAAGAAAG"),
    list("2000c3521f1c15ab",  "4", 100004, 0x0000000e, 0x08, 0x04, "ACGTACGT", "ACGT"),
    list("2800c352d8f2d5b5",  "5", 100005, 0x0000000e, 0x04, 0x08, "ACGT", "ACGTACGT"),
    list("5000c3553bbf9c19", "10", 100010, 0x00000012, 0x08, 0x08, "ACGTACGT", "CGTACGTA"),
    list("b000c35b64690b25", "22", 100022, 0x0000000b, 0x08, 0x01, "ACGTACGT", "N"),
    list("b800c35bbcece603",  "X", 100023, 0x0000000e, 0x0a, 0x02, "AAAAAAAAGG", "AG"),
    list("c000c35c63741ee7",  "Y", 100024, 0x0000000e, 0x02, 0x0a, "AG", "AAAAAAAAGG"),
    list("c800c35c96c18499", "MT", 100025, 0x00000012, 0x04, 0x0c, "ACGT", "AAACCCGGGTTT")
)
colnames(x) <- list("vk", "chrom", "pos", "ralen", "sizeref", "sizealt", "ref", "alt")

test_that("MmapBinfile", {
    vknr <<- MmapBinfile("../../../../c/test/data/vknr.10.bin")
    expect_that(vknr$SIZE, equals(299))
})

test_that("FindRefAltByVariantkey", {
    res <- mapply(FindRefAltByVariantkey, vk = unlist(x[,"vk"]), MoreArgs = list(src = vknr$SRC, last = vknr$LAST), SIMPLIFY = TRUE, USE.NAMES = FALSE)
    expect_that(unlist(res[1,]), equals(unlist(x[,"ref"])))
    expect_that(unlist(res[2,]), equals(unlist(x[,"alt"])))
    expect_that(unlist(res[3,]), equals(unlist(x[,"sizeref"])))
    expect_that(unlist(res[4,]), equals(unlist(x[,"sizealt"])))
    expect_that(unlist(res[5,]), equals(unlist(x[,"ralen"]) - 2))
})

test_that("ReverseVariantkey", {
    res <- mapply(ReverseVariantkey, vk = unlist(x[,"vk"]), MoreArgs = list(src = vknr$SRC, last = vknr$LAST), SIMPLIFY = TRUE, USE.NAMES = FALSE)
    expect_that(unlist(res[1,]), equals(unlist(x[,"chrom"])))
    expect_that(unlist(res[2,]), equals(unlist(x[,"pos"])))
    expect_that(unlist(res[3,]), equals(unlist(x[,"ref"])))
    expect_that(unlist(res[4,]), equals(unlist(x[,"alt"])))
    expect_that(unlist(res[5,]), equals(unlist(x[,"sizeref"])))
    expect_that(unlist(res[6,]), equals(unlist(x[,"sizealt"])))
    expect_that(unlist(res[7,]), equals(unlist(x[,"ralen"]) - 2))
})

test_that("GetRefLenByVariantkey", {
    res <- mapply(GetRefLenByVariantkey, vk = unlist(x[,"vk"]), MoreArgs = list(src = vknr$SRC, last = vknr$LAST), SIMPLIFY = TRUE, USE.NAMES = FALSE)
    expect_that(unlist(res), equals(unlist(x[,"sizeref"])))
})

test_that("GetRefLenByVariantkeyReversible", {
    res <- GetRefLenByVariantkey(vknr$SRC, vknr$LAST, "1800925199160000")
    expect_that(res, equals(3))
})

test_that("GetRefLenByVariantkeyNotFound", {
    res <- GetRefLenByVariantkey(vknr$SRC, vknr$LAST, "ffffffffffffffff")
    expect_that(res, equals(0))
})

test_that("GetVariantkeyEndPos", {
    res <- mapply(GetVariantkeyEndPos, vk = unlist(x[,"vk"]), MoreArgs = list(src = vknr$SRC, last = vknr$LAST), SIMPLIFY = TRUE, USE.NAMES = FALSE)
    expect_that(unlist(res), equals(unlist(x[,"pos"]) + unlist(x[,"sizeref"])))
})

test_that("VknrBinToTsv", {
    size <- VknrBinToTsv(vknr$SRC, vknr$LAST, "vknr.test")
    expect_that(size, equals(305))
})

test_that("MunmapBinfile", {
    err <- MunmapBinfile(vknr$SRC, vknr$FD, vknr$SIZE)
    expect_that(err, equals(0))
})
