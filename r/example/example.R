#!/usr/bin/env Rscript

# Usage example for VariantKey R wrapper
# https://github.com/genomicsplc/variantkey

library(variantkey)

# BASIC VARIANTKEY FUNCTIONS
# --------------------------

x <- EncodeChrom("X")
print(x)
# [1] 23

x <- DecodeChrom(23)
print(x)
# [1] "X"

x <- EncodeRefAlt("AC", "GT")
print(x)
# [1] 286097408

x <- DecodeRefAlt(286097408)
print(x)
# $REF
# [1] "AC"
#
# $ALT
# [1] "GT"
#
# $REF_LEN
# [1] 2
#
# $ALT_LEN
# [1] 2

x <- EncodeVariantKey(23, 12345, 286097408)
print(x)
# [1] "b800181c910d8000"

x <- ExtractVariantkeyChrom("b800181c910d8000")
print(x)
# [1] 23

x <- ExtractVariantkeyPos("b800181c910d8000")
print(x)
# [1] 12345

x <- ExtractVariantkeyRefAlt("b800181c910d8000")
print(x)
# 286097408

x <- DecodeVariantkey("b800181c910d8000")
print(x)
# $CHROM
# [1] 23
#
# $POS
# [1] 12345
#
# $REFALT
# [1] 286097408

x <- VariantKey("X", 12345, "AC", "GT")
print(x)
# [1] "b800181c910d8000"

x <- VariantKeyRange(23, 1234, 5678)
print(x)
# $MIN
# [1] "b800026900000000"
#
# $MAX
# [1] "b8000b177fffffff"

x <- CompareVariantkeyChrom("B800026900000000", "B8000B177FFFFFFF")
print(x)
# [1] 0

x <- CompareVariantkeyChromPos("B800026900000000", "B8000B177FFFFFFF")
print(x)
# [1] -1


# /\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\


# GENOREF FUNCTIONS
# -----------------


# /\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\


# NRVK
# ----


# /\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\


# RSIDVAR
# -------


# - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

# - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
