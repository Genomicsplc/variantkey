context("variantkey")
library(variantkey)

# test-rsidvar.R
# @category   Libraries
# @author     Nicola Asuni <nicola.asuni@genomicsplc.com>
# @copyright  2017-2018 GENOMICS plc
# @license    MIT (see LICENSE)
# @link       https://github.com/genomicsplc/variantkey

test_that("MmapBinfile", {
    rsvk <<- MmapBinfile("../../../../c/test/data/rsvk.10.bin")
    expect_that(rsvk$SIZE, equals(120))
    rsvkm <<- MmapBinfile("../../../../c/test/data/rsvk.m.10.bin")
    expect_that(rsvkm$SIZE, equals(120))
    vkrs <<- MmapBinfile("../../../../c/test/data/vkrs.10.bin")
    expect_that(rsvk$SIZE, equals(120))
})

test_that("MunmapBinfile", {
    err <- MunmapBinfile(rsvk$SRC,rsvk$FD, rsvk$SIZE)
    expect_that(err, equals(0))
    err <- MunmapBinfile(rsvkm$SRC,rsvkm$FD, rsvkm$SIZE)
    expect_that(err, equals(0))
    err <- MunmapBinfile(vkrs$SRC,vkrs$FD, vkrs$SIZE)
    expect_that(err, equals(0))
})
