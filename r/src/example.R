#!/usr/bin/env Rscript

args <- commandArgs(trailingOnly = F)  
script.path <- normalizePath(dirname(sub("^--file=", "", args[grep("^--file=", args)])))
lib.variantkey <- paste(script.path, "/rvariantkey.so", sep="")

dyn.load(lib.variantkey)

vkey <- .Call("VariantKey", "X", 193330, "GCA", "G")
print(vkey)
# [1] "b801799918c90000"

var <- .Call("ReverseVariantKey", vkey)
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
