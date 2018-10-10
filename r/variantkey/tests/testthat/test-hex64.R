context("variantkey")
library(variantkey)

# test-hex64.R
# @category   Libraries
# @author     Nicola Asuni <nicola.asuni@genomicsplc.com>
# @copyright  2017-2018 GENOMICS plc
# @license    MIT (see LICENSE)
# @link       https://github.com/genomicsplc/variantkey

test_that("hex64Class", {
    res <- hex64(1)
    expect_that(class(res), equals("hex64"))
    expect_that(typeof(res), equals("character"))
})

test_that("asHex64", {
    res <- as.hex64("ffffffffffffffff")
    expect_that(class(res), equals("hex64"))
    expect_that(typeof(res), equals("character"))
    expect_that(as.character(res), equals("ffffffffffffffff"))
})

test_that("isHex64", {
    res <- is.hex64(as.hex64("ffffffffffffffff"))
    expect_that(res, equals(TRUE))
    res <- is.hex64(1)
    expect_that(res, equals(FALSE))
})

test_that("asHex64Uint64", {
    res <- as.hex64(as.uint64(as.hex64("ffffffffffffffff")))
    expect_that(as.character(res), equals("ffffffffffffffff"))
})
