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
    idx <<- LoadGenorefIndex(genoref$SRC)
})

test_that("GetGenorefSeq", {
    chrom = seq(1, 25)
    res <- mapply(GetGenorefSeq, chrom = chrom, MoreArgs = list(src = genoref$SRC, idx = idx, pos = 0), SIMPLIFY = TRUE, USE.NAMES = FALSE) # first
    expect_that(res, equals(rep(utf8ToInt("A"), 25))) # always A
    res <- mapply(GetGenorefSeq, chrom = chrom, pos = (26 - chrom), MoreArgs = list(src = genoref$SRC, idx = idx), SIMPLIFY = TRUE, USE.NAMES = FALSE) # last
    expect_that(res, equals(seq(utf8ToInt("Z"), utf8ToInt("B")))) # Z to A
    res <- mapply(GetGenorefSeq, chrom = chrom, pos = (27 - chrom), MoreArgs = list(src = genoref$SRC, idx = idx), SIMPLIFY = TRUE, USE.NAMES = FALSE) # invalid
    expect_that(res, equals(rep(0, 25))) # always 0
})

test_that("MunmapBinfile", {
    err <- MunmapBinfile(genoref$SRC, genoref$FD, genoref$SIZE)
    expect_that(err, equals(0))
})
