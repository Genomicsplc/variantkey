context("variantkey")
library(variantkey)

# test-binsearch.R
# @category   Libraries
# @author     Nicola Asuni <info@tecnick.com>
# @copyright  2017-2018 Nicola Asuni - Tecnick.com
# @license    MIT (see LICENSE)
# @link       https://github.com/tecnickcom/binsearch

test_that("MmapBinfile", {
    mm <<- MmapBinfile("../../../../c/test/data/test_data.bin")
    expect_that(mm$SIZE, equals(4016))
})

test_that("MunmapBinfile", {
    err <- MunmapBinfile(mm$SRC, mm$FD, mm$SIZE)
    expect_that(err, equals(0))
})
