context("variantkey")
library(variantkey)

# test-binsearch.R
# @category   Libraries
# @author     Nicola Asuni <info@tecnick.com>
# @copyright  2017-2018 Nicola Asuni - Tecnick.com
# @license    MIT (see LICENSE)
# @link       https://github.com/tecnickcom/binsearch

test_that("MmapGenorefFile", {
    gref <<- MmapGenorefFile("../../../../c/test/data/genoref.bin")
    expect_that(gref$SIZE, equals(598))
})

test_that("MunmapBinfile", {
    err <- MunmapBinfile(gref$MF)
    expect_that(err, equals(0))
})
