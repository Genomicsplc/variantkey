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

test_that("CheckReference", {
    x <- rbind(
        list(1, 0, "A", 1, 0),
        list(1, 25, "Z", 1, 0),
        list(25, 0, "A", 1, 0),
        list(25, 1, "B", 1, 0),
        list(2, 0, "ABCDEFGHIJKLmnopqrstuvwxy", 25, 0),
        list(1, 26, "ZABC", 4, -2),
        list(1, 0, "ABCDEFGHIJKLmnopqrstuvwxyJ", 26, -1),
        list(14, 2, "ZZZ", 3, -1),
        list(1, 0, "N", 1, 1),
        list(10, 13, "A", 1, 1),
        list(1, 3, "B", 1, 1),
        list(1, 1, "C", 1, 1),
        list(1, 0, "D", 1, 1),
        list(1, 3, "A", 1, 1),
        list(1, 0, "H", 1, 1),
        list(1, 7, "A", 1, 1),
        list(1, 0, "V", 1, 1),
        list(1, 21, "A", 1, 1),
        list(1, 0, "W", 1, 1),
        list(1, 19, "W", 1, 1),
        list(1, 22, "A", 1, 1),
        list(1, 22, "T", 1, 1),
        list(1, 2, "S", 1, 1),
        list(1, 6, "S", 1, 1),
        list(1, 18, "C", 1, 1),
        list(1, 18, "G", 1, 1),
        list(1, 0, "M", 1, 1),
        list(1, 2, "M", 1, 1),
        list(1, 12, "A", 1, 1),
        list(1, 12, "C", 1, 1),
        list(1, 6, "K", 1, 1),
        list(1, 19, "K", 1, 1),
        list(1, 10, "G", 1, 1),
        list(1, 10, "T", 1, 1),
        list(1, 0, "R", 1, 1),
        list(1, 6, "R", 1, 1),
        list(1, 17, "A", 1, 1),
        list(1, 17, "G", 1, 1),
        list(1, 2, "Y", 1, 1),
        list(1, 19, "Y", 1, 1),
        list(1, 24, "C", 1, 1),
        list(1, 24, "T", 1, 1)
    )
    colnames(x) <- list("chrom", "pos", "ref", "sizeref", "code")
    res <- mapply(CheckReference, chrom = unlist(x[,"chrom"]), pos = unlist(x[,"pos"]), ref = unlist(x[,"ref"]), MoreArgs = list(src = genoref$SRC, idx = idx), SIMPLIFY = TRUE, USE.NAMES = FALSE)
    expect_that(res, equals(unlist(x[,"code"])))
})

test_that("FlipAllele", {
    res <- FlipAllele("ATCGMKRYBVDHWSNatcgmkrybvdhwsn")
    expect_that(res, equals("TAGCKMYRVBHDWSNTAGCKMYRVBHDWSN"))
})

test_that("MunmapBinfile", {
    err <- MunmapBinfile(genoref$SRC, genoref$FD, genoref$SIZE)
    expect_that(err, equals(0))
})
