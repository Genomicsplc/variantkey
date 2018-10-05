context("variantkey")
library(variantkey)

# test-genoref.R
# @category   Libraries
# @author     Nicola Asuni <nicola.asuni@genomicsplc.com>
# @copyright  2017-2018 GENOMICS plc
# @license    MIT (see LICENSE)
# @link       https://github.com/genomicsplc/variantkey

InitVariantKey(genoref_file = "../../../../c/test/data/genoref.bin")

test_that("GetGenorefSeq", {
    chrom = seq(1, 25)
    res <- GetGenorefSeq(chrom = chrom, pos = rep(as.integer(0), 25)) # first
    expect_that(res, equals(rep("A", 25))) # always A
    res <- GetGenorefSeq(chrom = chrom, pos = seq(25, 1)) # last
    expect_that(res, equals(intToUtf8(seq(utf8ToInt("Z"), utf8ToInt("B")), multiple = TRUE))) # Z to A
    res <- GetGenorefSeq(chrom = chrom, pos = seq(26, 2)) # invalid
    expect_that(res, equals(intToUtf8(rep(0, 25), multiple = TRUE))) # always 0
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
    res <- CheckReference(chrom = unlist(x[,"chrom"]), pos = unlist(x[,"pos"]), ref = unlist(x[,"ref"]))
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
    res <- NormalizeVariant(chrom = unlist(x[,"chrom"]), pos = unlist(x[,"pos"]), ref = unlist(x[,"ref"]), alt = unlist(x[,"alt"]))
    expect_that(res$RET, equals(unlist(x[,"ecode"])))
    expect_that(res$POS, equals(unlist(x[,"epos"])))
    expect_that(res$REF, equals(unlist(x[,"eref"])))
    expect_that(res$ALT, equals(unlist(x[,"ealt"])))
})

CloseVariantKey()
