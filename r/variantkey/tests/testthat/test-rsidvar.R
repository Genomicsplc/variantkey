context("variantkey")
library(variantkey)

# test-rsidvar.R
# @category   Libraries
# @author     Nicola Asuni <nicola.asuni@genomicsplc.com>
# @copyright  2017-2018 GENOMICS plc
# @license    MIT (see LICENSE)
# @link       https://github.com/genomicsplc/variantkey

x <- rbind(
    list(0, 0x01, 0x0004F44B, 0x00338000, 0x00000001, "08027a2580338000"),
    list(1, 0x09, 0x000143FC, 0x439E3918, 0x00000007, "4800a1fe439e3918"),
    list(2, 0x09, 0x000143FC, 0x7555EB16, 0x0000000B, "4800a1fe7555eb16"),
    list(3, 0x10, 0x000204E8, 0x003A0000, 0x00000061, "80010274003a0000"),
    list(4, 0x10, 0x0002051A, 0x00138000, 0x00000065, "8001028d00138000"),
    list(5, 0x10, 0x00020532, 0x007A0000, 0x000003E5, "80010299007a0000"),
    list(6, 0x14, 0x000256C4, 0x003A0000, 0x000003F1, "a0012b62003a0000"),
    list(7, 0x14, 0x000256C5, 0x00708000, 0x000026F5, "a0012b6280708000"),
    list(8, 0x14, 0x000256CB, 0x63256692, 0x000186A3, "a0012b65e3256692"),
    list(9, 0x14, 0x000256CF, 0x55439803, 0x00019919, "a0012b67d5439803")
)
colnames(x) <- list("item", "chrom", "pos", "refalt", "rsid", "vk")

test_that("MmapRSVKFile", {
    rsvk <<- MmapRSVKFile("../../../../c/test/data/rsvk.10.bin", as.integer(c(4, 8)))
    expect_that(rsvk$NROWS, equals(10))
    rsvkm <<- MmapRSVKFile("../../../../c/test/data/rsvk.m.10.bin", as.integer(c()))
    expect_that(rsvkm$NROWS, equals(10))
    vkrs <<- MmapVKRSFile("../../../../c/test/data/vkrs.10.bin", as.integer(c(8, 4)))
    expect_that(vkrs$NROWS, equals(10))
})

test_that("FindRvVariantKeyByRsid", {
    res <- FindRvVariantKeyByRsid(mc = rsvk$MC, first = 0, last = rsvk$NROWS, rsid = unlist(x[,"rsid"]))
    expect_that(res$VK, equals(hexToUint64(unlist(x[,"vk"]))))
    expect_that(res$FIRST, equals(unlist(x[,"item"])))
})

test_that("FindRvVariantKeyByRsidNotFound", {
    res <- FindRvVariantKeyByRsid(rsvk$MC, 0, rsvk$NROWS, 0xfffffff0)
    expect_that(res$VK, equals(as.uint64(0)))
    expect_that(res$FIRST, equals(9))
})

test_that("GetNextRvVariantKeyByRsid", {
    res <- GetNextRvVariantKeyByRsid(rsvk$MC, 2, rsvk$NROWS, 0x00000061)
    expect_that(res$VK, equals(hexToUint64("80010274003a0000")))
    expect_that(res$POS, equals(3))
    res <- GetNextRvVariantKeyByRsid(rsvk$MC, res$POS, rsvk$NROWS, 0x00000061)
    expect_that(res$VK, equals(as.uint64(0)))
    expect_that(res$POS, equals(4))
})

test_that("FindVrRsidByVariantKey", {
    res <- FindVrRsidByVariantKey(mc = vkrs$MC, first = 0, last = vkrs$NROWS, vk = hexToUint64(unlist(x[,"vk"])))
    expect_that(res$RSID, equals(unlist(x[,"rsid"])))
    expect_that(res$FIRST, equals(unlist(x[,"item"])))
})

test_that("FindVrRsidByVariantKeyNotFound", {
    res <- FindVrRsidByVariantKey(vkrs$MC, 0, vkrs$NROWS, hexToUint64("fffffffffffffff0"))
    expect_that(res$RSID, equals(0))
    expect_that(res$FIRST, equals(9))
})

test_that("FindVrChromposRange", {
    res <- FindVrChromposRange(vkrs$MC, 0, vkrs$NROWS, 0x14, 0x000256C5, 0x000256CB)
    expect_that(res$RSID, equals(0x000026F5))
    expect_that(res$FIRST, equals(7))
    expect_that(res$LAST, equals(9))
})

test_that("FindVrChromposRangeNotFound", {
    res <- FindVrChromposRange(vkrs$MC, 0, vkrs$NROWS, 0xff, 0xffffff00, 0xfffffff0)
    expect_that(res$RSID, equals(0))
    expect_that(res$FIRST, equals(10))
    expect_that(res$LAST, equals(10))
})

test_that("FindAllRvVariantKeyByRsid", {
    res <- FindAllRvVariantKeyByRsid(rsvkm$MC, 0, rsvkm$NROWS, 0x00000003)
    expect_that(length(res), equals(3))
    expect_that(unlist(res[1]), equals(hexToUint64("80010274003a0000")))
    expect_that(unlist(res[2]), equals(hexToUint64("8001028d00138000")))
    expect_that(unlist(res[3]), equals(hexToUint64("80010299007a0000")))
})

test_that("FindAllRvVariantKeyByRsidNotFound", {
    res <- FindAllRvVariantKeyByRsid(rsvkm$MC, 0, rsvkm$NROWS, 0xfffffff0)
    expect_that(length(res), equals(0))
})

test_that("MunmapBinfile", {
    err <- MunmapBinfile(rsvk$MF)
    expect_that(err, equals(0))
    err <- MunmapBinfile(rsvkm$MF)
    expect_that(err, equals(0))
    err <- MunmapBinfile(vkrs$MF)
    expect_that(err, equals(0))
})
