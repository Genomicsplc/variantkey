context("variantkey")
library(variantkey)

# test-uint64.R
# @category   Libraries
# @author     Nicola Asuni <nicola.asuni@genomicsplc.com>
# @copyright  2017-2018 GENOMICS plc
# @license    MIT (see LICENSE)
# @link       https://github.com/genomicsplc/variantkey

test_that("uint64Class", {
    res <- uint64(1)
    expect_that(class(res), equals("uint64"))
    expect_that(typeof(res), equals("double"))
})

test_that("asUint64Character", {
    res <- as.uint64("81985529216486895")
    expect_that(class(res), equals("uint64"))
    expect_that(typeof(res), equals("double"))
    expect_that(as.character(res), equals("81985529216486895"))
})

test_that("asUint64Double", {
    res <- as.uint64(as.double(123))
    expect_that(class(res), equals("uint64"))
    expect_that(typeof(res), equals("double"))
    expect_that(as.character(res), equals("123"))
})

test_that("asUint64Integer", {
    res <- as.uint64(as.integer(123))
    expect_that(class(res), equals("uint64"))
    expect_that(typeof(res), equals("double"))
    expect_that(as.character(res), equals("123"))
})

test_that("isUint64", {
    res <- is.uint64(as.uint64("81985529216486895"))
    expect_that(res, equals(TRUE))
    res <- is.uint64(1)
    expect_that(res, equals(FALSE))
})

test_that("identicalUint64", {
    res <- identical(as.uint64("81985529216486895"), as.uint64("81985529216486895"))
    expect_that(res, equals(TRUE))
})

test_that("repUint64", {
    res <- rep(as.uint64(123), 2)
    expect_length(res, 2)
    expect_that(as.character(res[1]), equals("123"))
    expect_that(as.character(res[2]), equals("123"))
})

test_that("lengthUint64", {
    res <- as.uint64(c(1,2,3))
    expect_length(res, 3)
})

test_that("cmpUint64", {
    a <- as.uint64(as.hex64("ffffffffffffffff"))
    b <- as.uint64(as.hex64("fffffffffffffff0"))
    expect_that((a == a), equals(TRUE))
    expect_that((a == b), equals(FALSE))
    expect_that((a > b), equals(TRUE))
    expect_that((a < b), equals(FALSE))
    expect_that((a >= b), equals(TRUE))
    expect_that((a <= b), equals(FALSE))
    expect_that((a >= a), equals(TRUE))
    expect_that((a <= a), equals(TRUE))
})

test_that("sortUint64", {
    u <- as.uint64(as.hex64(c("fffffffffffffff0", "ffffffff00000000", "ffffffffffffffff", "ffffffffff000000", "fffffffff0000000", "ffffffffffffff00", "fffffffffff00000", "0000000000000000", "ffffffffffff0000", "fffffffffffff000")))
    res <- sort(u)
    expect_that(as.character(res[1]), equals("0"))
    expect_that(as.character(res[2]), equals("18446744069414584320"))
    expect_that(as.character(res[3]), equals("18446744073441116160"))
    expect_that(as.character(res[4]), equals("18446744073692774400"))
    expect_that(as.character(res[5]), equals("18446744073708503040"))
    expect_that(as.character(res[6]), equals("18446744073709486080"))
    expect_that(as.character(res[7]), equals("18446744073709547520"))
    expect_that(as.character(res[8]), equals("18446744073709551360"))
    expect_that(as.character(res[9]), equals("18446744073709551600"))
    expect_that(as.character(res[10]), equals("18446744073709551615"))
})

test_that("revUint64", {
    u <- as.uint64(c("0", "18446744069414584320", "18446744073441116160", "18446744073692774400", "18446744073708503040", "18446744073709486080", "18446744073709547520", "18446744073709551360", "18446744073709551600", "18446744073709551615"))
    res <- rev(u)
    expect_that(as.character(res[1]), equals("18446744073709551615"))
    expect_that(as.character(res[2]), equals("18446744073709551600"))
    expect_that(as.character(res[3]), equals("18446744073709551360"))
    expect_that(as.character(res[4]), equals("18446744073709547520"))
    expect_that(as.character(res[5]), equals("18446744073709486080"))
    expect_that(as.character(res[6]), equals("18446744073708503040"))
    expect_that(as.character(res[7]), equals("18446744073692774400"))
    expect_that(as.character(res[8]), equals("18446744073441116160"))
    expect_that(as.character(res[9]), equals("18446744069414584320"))
    expect_that(as.character(res[10]), equals("0"))
})

test_that("orderUint64", {
    u <- as.uint64(as.hex64(c("fffffffffffffff0", "ffffffff00000000", "ffffffffffffffff", "ffffffffff000000", "fffffffff0000000", "ffffffffffffff00", "fffffffffff00000", "0000000000000000", "ffffffffffff0000", "fffffffffffff000")))
    res <- order.uint64(u)
    expect_that(res[1], equals(9))
    expect_that(res[2], equals(2))
    expect_that(res[3], equals(10))
    expect_that(res[4], equals(4))
    expect_that(res[5], equals(3))
    expect_that(res[6], equals(8))
    expect_that(res[7], equals(5))
    expect_that(res[8], equals(1))
    expect_that(res[9], equals(6))
    expect_that(res[10], equals(7))
})

test_that("uniqueUint64", {
    u <- as.uint64(c("1085102592571150095", "1085102592571150095"))
    res <- unique(u)
    expect_that(as.character(res), equals("1085102592571150095"))
})

test_that("intersectUint64", {
    a <- as.uint64(c("1085102592571150000", "1085102592571150010"))
    b <- as.uint64(c("1085102592571150010", "1085102592571150020"))
    res <- intersect.uint64(a, b)
    expect_that(as.character(res), equals("1085102592571150010"))
})

test_that("unionUint64", {
    a <- as.uint64(c("1085102592571150000", "1085102592571150010"))
    b <- as.uint64(c("1085102592571150010", "1085102592571150020"))
    res <- union.uint64(a, b)
    expect_length(res, 3)
    expect_that(as.character(res[1]), equals("1085102592571150000"))
    expect_that(as.character(res[2]), equals("1085102592571150010"))
    expect_that(as.character(res[3]), equals("1085102592571150020"))
})
