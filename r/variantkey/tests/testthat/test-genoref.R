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
        list( 0, 1,   0,  1, "A"                         ),
        list( 0, 1,  25,  1, "Z"                         ),
        list( 0, 25,  0,  1, "A"                         ),
        list( 0, 25,  1,  1, "B"                         ),
        list( 0, 2,   0, 25, "ABCDEFGHIJKLmnopqrstuvwxy" ),
        list(-2, 1,  26,  4, "ZABC"                      ),
        list(-1, 1,   0, 26, "ABCDEFGHIJKLmnopqrstuvwxyJ"),
        list(-1, 14,  2,  3, "ZZZ"                       ),
        list( 1, 1,   0,  1, "N"                         ),
        list( 1, 10, 13,  1, "A"                         ),
        list( 1, 1,   3,  1, "B"                         ),
        list( 1, 1,   1,  1, "C"                         ),
        list( 1, 1,   0,  1, "D"                         ),
        list( 1, 1,   3,  1, "A"                         ),
        list( 1, 1,   0,  1, "H"                         ),
        list( 1, 1,   7,  1, "A"                         ),
        list( 1, 1,   0,  1, "V"                         ),
        list( 1, 1,  21,  1, "A"                         ),
        list( 1, 1,   0,  1, "W"                         ),
        list( 1, 1,  19,  1, "W"                         ),
        list( 1, 1,  22,  1, "A"                         ),
        list( 1, 1,  22,  1, "T"                         ),
        list( 1, 1,   2,  1, "S"                         ),
        list( 1, 1,   6,  1, "S"                         ),
        list( 1, 1,  18,  1, "C"                         ),
        list( 1, 1,  18,  1, "G"                         ),
        list( 1, 1,   0,  1, "M"                         ),
        list( 1, 1,   2,  1, "M"                         ),
        list( 1, 1,  12,  1, "A"                         ),
        list( 1, 1,  12,  1, "C"                         ),
        list( 1, 1,   6,  1, "K"                         ),
        list( 1, 1,  19,  1, "K"                         ),
        list( 1, 1,  10,  1, "G"                         ),
        list( 1, 1,  10,  1, "T"                         ),
        list( 1, 1,   0,  1, "R"                         ),
        list( 1, 1,   6,  1, "R"                         ),
        list( 1, 1,  17,  1, "A"                         ),
        list( 1, 1,  17,  1, "G"                         ),
        list( 1, 1,   2,  1, "Y"                         ),
        list( 1, 1,  19,  1, "Y"                         ),
        list( 1, 1,  24,  1, "C"                         ),
        list( 1, 1,  24,  1, "T"                         )
    )
    colnames(x) <- list("exp", "chrom", "pos", "sizeref", "ref")
    res <- mapply(CheckReference, chrom = unlist(x[,"chrom"]), pos = unlist(x[,"pos"]), ref = unlist(x[,"ref"]), MoreArgs = list(src = genoref$SRC, idx = idx), SIMPLIFY = TRUE, USE.NAMES = FALSE)
    expect_that(res, equals(unlist(x[,"exp"])))
})

test_that("FlipAllele", {
    res <- FlipAllele("ATCGMKRYBVDHWSNatcgmkrybvdhwsn")
    expect_that(res, equals("TAGCKMYRVBHDWSNTAGCKMYRVBHDWSN"))
})

test_that("NormalizeVariant", {
    x <- rbind(
        list(-2, 1, 26, 26, 1, 1, 1, 1, "A",  "C",  "A",      "C"     ),  # invalid position
        list(-1, 1,  0,  0, 1, 1, 1, 1, "J",  "C",  "J",      "C"     ),  # invalid reference
        list( 4, 1,  0,  0, 1, 1, 1, 1, "A",  "C",  "T",      "G"     ),  # flip
        list( 0, 1,  0,  0, 1, 1, 1, 1, "A",  "C",  "A",      "C"     ),  # OK
        list(32, 13, 2,  3, 3, 2, 2, 1, "DE", "D",  "CDE",    "CD"    ),  # left trim
        list(48, 13, 2,  3, 3, 3, 1, 1, "D",  "F",  "CDE",    "CFE"   ),  # left trim + right trim
        list(48, 1,  0,  2, 6, 6, 1, 1, "C",  "K",  "aBCDEF", "aBKDEF"),  # left trim + right trim
        list( 0, 1,  0,  0, 1, 0, 1, 0, "A",  "",   "A",      ""      ),  # OK
        list( 8, 1,  3,  2, 1, 0, 2, 1, "CD", "C",  "D",      ""      ),  # left extend
        list( 0, 1, 24, 24, 1, 2, 1, 2, "Y",  "CK", "Y",      "CK"    ),  # OK
        list( 2, 1,  0,  0, 1, 1, 1, 1, "A",  "G",  "G",      "A"     ),  # swap
        list( 6, 1,  0,  0, 1, 1, 1, 1, "A",  "C",  "G",      "T"     )   # swap + flip
    )
    colnames(x) <- list("ecode", "chrom", "pos", "epos", "sizeref", "sizealt", "esizeref", "esizealt", "eref", "ealt", "ref", "alt")
    res <- mapply(NormalizeVariant, chrom = unlist(x[,"chrom"]), pos = unlist(x[,"pos"]), ref = unlist(x[,"ref"]), alt = unlist(x[,"alt"]), MoreArgs = list(src = genoref$SRC, idx = idx), SIMPLIFY = TRUE, USE.NAMES = FALSE)
    expect_that(unlist(res[1,]), equals(unlist(x[,"ecode"])))
    expect_that(unlist(res[2,]), equals(unlist(x[,"epos"])))
    expect_that(unlist(res[3,]), equals(unlist(x[,"eref"])))
    expect_that(unlist(res[4,]), equals(unlist(x[,"ealt"])))
    expect_that(unlist(res[5,]), equals(unlist(x[,"esizeref"])))
    expect_that(unlist(res[6,]), equals(unlist(x[,"esizealt"])))
})

test_that("MunmapBinfile", {
    err <- MunmapBinfile(genoref$SRC, genoref$FD, genoref$SIZE)
    expect_that(err, equals(0))
})
