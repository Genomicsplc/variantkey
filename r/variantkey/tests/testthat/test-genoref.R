context("variantkey")
library(variantkey)

# test-genoref.R
# @category   Libraries
# @author     Nicola Asuni <nicola.asuni@genomicsplc.com>
# @copyright  2017-2018 GENOMICS plc
# @license    MIT (see LICENSE)
# @link       https://github.com/genomicsplc/variantkey

test_that("MmapBinfile", {
    genoref <<- MmapBinfile("../../../../c/test/data/genoref.bin")
    expect_that(genoref$SIZE, equals(480))
})

test_that("MunmapBinfile", {
    err <- MunmapBinfile(genoref$SRC,genoref$FD, genoref$SIZE)
    expect_that(err, equals(0))
})
