context("variantkey")
library(variantkey)

# test-nrvk.R
# @category   Libraries
# @author     Nicola Asuni <nicola.asuni@genomicsplc.com>
# @copyright  2017-2018 GENOMICS plc
# @license    MIT (see LICENSE)
# @link       https://github.com/genomicsplc/variantkey

test_that("MmapBinfile", {
    vknr <<- MmapBinfile("../../../../c/test/data/vknr.10.bin")
    expect_that(vknr$SIZE, equals(299))
})

test_that("MunmapBinfile", {
    err <- MunmapBinfile(vknr$SRC,vknr$FD, vknr$SIZE)
    expect_that(err, equals(0))
})
