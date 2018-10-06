HEX64 <- "hex64"

setOldClass(HEX64)

#' Create a new hex64 vector.
#' @param length vector length
#' @export
hex64 <- function(length=0) {
    ret <- character(length)
    oldClass(ret) <- HEX64
    return(ret)
}

#' Check if the object x is of hex64 class.
#' @param x object
#' @export
is.hex64 <- function(x) {
    return(inherits(x, HEX64))
}

#' Identity function for class hex64.
#' @export
identical.hex64 <- function(x, y, num.eq=FALSE, single.NA=FALSE, attrib.as.set=TRUE, ignore.bytecode=TRUE) {
    return(identical(x=x, y=y, num.eq=num.eq, single.NA=single.NA, attrib.as.set=attrib.as.set, ignore.bytecode=ignore.bytecode))
}

#' Coerce to hex64.
#' @param x vector
#' @export
as.hex64 <- function(x, ...) {
    UseMethod("as.hex64")
}

#' Coerce from factor to hex64.
#' @param x factor vector
#' @export
as.hex64.factor <- function(x, ...) {
    return(as.hex64(unclass(x), ...))
}

#' Coerce from NULL to hex64.
#' @param x NULL vector
#' @export
as.hex64.NULL <- function(x, ...) {
    return(hex64())
}

#' Coerce from hex64 to hex64.
#' @param x hex64 vector
#' @export
as.hex64.hex64 <- function(x, ...) {
    return(x)
}

#' Convert uint64 vector to hex64.
#' @param x uint64 vector
#' @useDynLib variantkey R_hex_uint64_t
#' @export
as.hex64.uint64 <- function(x, ...) {
    ret <- hex64(length(x))
    return(.Call("R_hex_uint64_t", x, ret))
}

#' Coerce character vector to hex64
#' @param x character vector
#' @export
as.hex64.character <- function(x, ...) {
    oldClass(x) <- HEX64
    return(x)
}

setAs("character", HEX64, function(from)as.hex64.character(from))

#' Coerce hex64 vector to decimal character
#' @param x hex64 vector
#' @export
as.character.hex64 <- function(x, ...) {
    oldClass(x) <- "character"
    return(x)
}

setAs(HEX64, "character", function(from)as.character.hex64(from))
