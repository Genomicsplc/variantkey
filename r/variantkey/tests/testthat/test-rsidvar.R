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

InitVariantKey(rsvk_file = "../../../../c/test/data/rsvk.10.bin", vkrs_file = "../../../../c/test/data/vkrs.10.bin")

test_that("FindRvVariantKeyByRsid", {
    res <- FindRvVariantKeyByRsid(rsid = unlist(x[,"rsid"]))
    expect_that(res$VK, equals(as.uint64(as.hex64(unlist(x[,"vk"])))))
    expect_that(res$FIRST, equals(unlist(x[,"item"])))
})

test_that("FindRvVariantKeyByRsidNotFound", {
    res <- FindRvVariantKeyByRsid(rsid=0xfffffff0)
    expect_that(res$VK, equals(as.uint64(0)))
    expect_that(res$FIRST, equals(9))
})

test_that("GetNextRvVariantKeyByRsid", {
    res <- GetNextRvVariantKeyByRsid(pos=2, rsid=0x00000061)
    expect_that(res$VK, equals(as.uint64(as.hex64("80010274003a0000"))))
    expect_that(res$POS, equals(3))
    res <- GetNextRvVariantKeyByRsid(rsid=0x00000061, pos=res$POS)
    expect_that(res$VK, equals(as.uint64(0)))
    expect_that(res$POS, equals(4))
})

test_that("FindVrRsidByVariantKey", {
    res <- FindVrRsidByVariantKey(vk = as.hex64(unlist(x[,"vk"])))
    expect_that(res$RSID, equals(unlist(x[,"rsid"])))
    expect_that(res$FIRST, equals(unlist(x[,"item"])))
})

test_that("FindVrRsidByVariantKeyNotFound", {
    res <- FindVrRsidByVariantKey(as.hex64("fffffffffffffff0"))
    expect_that(res$RSID, equals(0))
    expect_that(res$FIRST, equals(9))
})

test_that("FindVrChromposRange", {
    res <- FindVrChromposRange(0x14, 0x000256C5, 0x000256CB)
    expect_that(res$RSID, equals(0x000026F5))
    expect_that(res$FIRST, equals(7))
    expect_that(res$LAST, equals(9))
})

test_that("GetNextVrRsidByVariantKey", {
    res <- GetNextVrRsidByVariantKey(pos=2, vk="9223656209074749440")
    expect_that(res$RSID, equals(97))
    expect_that(res$POS, equals(3))
    res <- GetNextVrRsidByVariantKey(vk="9223656209074749440", pos=res$POS)
    expect_that(res$RSID, equals(0))
    expect_that(res$POS, equals(4))
})

test_that("FindAllVrRsidByVariantKey", {
    res <- FindAllVrRsidByVariantKey(vk="9223656209074749440")
    expect_that(length(res), equals(1))
    expect_that(unlist(res[1]), equals(97))
})

test_that("FindAllVrRsidByVariantKeyNotFound", {
    res <- FindAllVrRsidByVariantKey(vk="fffffffffffffff0")
    expect_that(length(res), equals(0))
})

test_that("FindVrChromposRangeNotFound", {
    res <- FindVrChromposRange(0xff, 0xffffff00, 0xfffffff0)
    expect_that(res$RSID, equals(0))
    expect_that(res$FIRST, equals(10))
    expect_that(res$LAST, equals(10))
})

test_that("MmapRSVKFile", {
    rsvkm <<- MmapRSVKFile("../../../../c/test/data/rsvk.m.10.bin", as.integer(c()))
    expect_that(rsvkm$NROWS, equals(10))
})

test_that("FindAllRvVariantKeyByRsid", {
    res <- FindAllRvVariantKeyByRsid(rsid=0x00000003, first=0, last=rsvkm$NROWS, mc=rsvkm$MC)
    expect_that(length(res), equals(3))
    expect_that(unlist(res[1]), equals(as.uint64(as.hex64("80010274003a0000"))))
    expect_that(unlist(res[2]), equals(as.uint64(as.hex64("8001028d00138000"))))
    expect_that(unlist(res[3]), equals(as.uint64(as.hex64("80010299007a0000"))))
})

test_that("FindAllRvVariantKeyByRsidNotFound", {
    res <- FindAllRvVariantKeyByRsid(rsid=0xfffffff0, first=0, last=rsvkm$NROWS, mc=rsvkm$MC)
    expect_that(length(res), equals(0))
})

test_that("MunmapBinfile", {
    err <- MunmapBinfile(rsvkm$MF)
    expect_that(err, equals(0))
})

CloseVariantKey()
