#!/usr/bin/env Rscript

args <- commandArgs(trailingOnly = F)
script.path <- normalizePath(dirname(sub("^--file=", "", args[grep("^--file=", args)])))
wrapper.variantkey <- paste(script.path, "/../src/variantkey.R", sep="")

source(wrapper.variantkey)

vkey <- VariantKey("X", 193330, "GCA", "G")
print(vkey)
# [1] "b801799918c90000"

var <- ReverseVariantKey(vkey)
print(var)
# $CHROM
# [1] "X"
#
# $POS
# [1] 193330
#
# $REF
# [1] "GCA"
#
# $ALT
# [1] "G"
#
# $SIZE_REF
# [1] 3
#
# $SIZE_ALT
# [1] 1

vkrange <- VariantKeyRange(15, 12002028, 12152133)
print(vkrange)
# $MIN
# [1] "785b917600000000"
#
# $MAX
# [1] "785cb6a2ffffffff"

cc <- EncodeChrom("MT")
print(cc)
# [1] 25

dc <- DecodeChrom(25)
print(dc)
# [1] "MT"

era <- EncodeRefAlt("A", "T")
print(era)
# [1] 144179200

dra <- DecodeRefAlt(144179200)
print(dra)
#$REF
#[1] "A"
#
#$ALT
#[1] "T"
#
#$REF_LEN
#[1] 1
#
#$ALT_LEN
#[1] 1
