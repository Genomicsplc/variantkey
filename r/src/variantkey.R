# R VariantKey Wrapper
#
# variantkey.R
#
# @category   Tools
# @author     Nicola Asuni <nicola.asuni@genomicsplc.com>
# @copyright  2017-2018 GENOMICS plc
# @license    MIT (see LICENSE)
# @link       https://github.com/genomicsplc/variantkey
#
# LICENSE
#
# Copyright (c) 2017-2018 GENOMICS plc
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in
# all copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
# THE SOFTWARE.
#
# VariantKey by Nicola Asuni

args <- commandArgs(trailingOnly = F)  
script.path <- normalizePath(dirname(sub("^--file=", "", args[grep("^--file=", args)])))
lib.variantkey <- paste(script.path, "/rvariantkey.so", sep="")

dyn.load(lib.variantkey)

EncodeChrom <- function(chrom) {
    return(.Call("EncodeChrom", chrom))
}

DecodeChrom <- function(code) {
    return(.Call("DecodeChrom", as.integer(code)))
}

EncodeRefAlt <- function(ref, alt) {
    return(.Call("EncodeRefAlt", ref, alt))
}

DecodeRefAlt <- function(code) {
    return(.Call("DecodeRefAlt", as.integer(code)))
}

VariantKey <- function(chrom, pos, ref, alt) {
    return(.Call("VariantKey", chrom, as.integer(pos), ref, alt))
}

VariantKeyRange <- function(chrom, pos_min, pos_max) {
    return(.Call("VariantKeyRange", as.integer(chrom), as.integer(pos_min), as.integer(pos_max)))
}

ReverseVariantKey <- function(hexcode) {
    return(.Call("ReverseVariantKey", hexcode))
}
