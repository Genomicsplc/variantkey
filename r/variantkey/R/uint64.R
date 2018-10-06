UINT64 <- "uint64"

setOldClass(UINT64)

#' Create a new uint64 vector.
#' @param length vector length
#' @export
uint64 <- function(length=0) {
    ret <- double(length)
    oldClass(ret) <- UINT64
    return(ret)
}

#' Check if the object x is of uint64 class.
#' @param x object
#' @export
is.uint64 <- function(x) {
    return(inherits(x, UINT64))
}

#' Identity function for class uint64.
#' @export
identical.uint64 <- function(x, y, num.eq=FALSE, single.NA=FALSE, attrib.as.set=TRUE, ignore.bytecode=TRUE) {
    return(identical(x=x, y=y, num.eq=num.eq, single.NA=single.NA, attrib.as.set=attrib.as.set, ignore.bytecode=ignore.bytecode))
}

#' Coerce to uint64.
#' @param x vector
#' @export
as.uint64 <- function(x, ...) {
    UseMethod("as.uint64")
}

#' Coerce from factor to uint64.
#' @param x factor vector
#' @export
as.uint64.factor <- function(x, ...) {
    return(as.uint64(unclass(x), ...))
}

#' Coerce from NULL to uint64.
#' @param x NULL vector
#' @export
as.uint64.NULL <- function(x, ...) {
    return(uint64())
}

#' Coerce from uint64 to uint64.
#' @param x uint64 vector
#' @export
as.uint64.uint64 <- function(x, ...) {
    return(x)
}

#' Coerce double vector to uint64
#' @param x double vector
#' @useDynLib variantkey R_double_to_uint64
#' @export
as.uint64.double <- function(x, ...) {
    ret <- uint64(length(x))
    return(.Call("R_double_to_uint64", x, ret))
}

#' Coerce integer vector to uint64
#' @param x integer vector
#' @useDynLib variantkey R_integer_to_uint64
#' @export
as.uint64.integer <- function(x, ...) {
    ret <- uint64(length(x))
    return(.Call("R_integer_to_uint64", x, ret))
}

#' Coerce character vector to uint64
#' @param x character vector
#' @useDynLib variantkey R_decstr_to_uint64
#' @export
as.uint64.character <- function(x, ...) {
    ret <- uint64(length(x))
    return(.Call("R_decstr_to_uint64", x, ret))
}

setAs("character", UINT64, function(from)as.uint64.character(from))

#' Coerce uint64 vector to character
#' @param x uint64 vector
#' @useDynLib variantkey R_uint64_to_decstr
#' @export
as.character.uint64 <- function(x, ...) {
    ret <- character(length(x))
    return(.Call("R_uint64_to_decstr", x, ret))
}

setAs(UINT64, "character", function(from)as.character.uint64(from))

#' Convert hexadecimal character vector to uint64
#' @param x hexadecimal character vector (16 characters per item)
#' @useDynLib variantkey R_parse_hex_uint64_t
#' @export
hexToUint64 <- function(x, ...) {
    ret <- uint64(length(x))
    return(.Call("R_parse_hex_uint64_t", as.character(x), ret))
}

#' Convert uint64 vector to hexadecimal character.
#' @param x uint64 vector
#' @useDynLib variantkey R_hex_uint64_t
#' @export
uint64ToHex <- function(x, ...) {
    ret <- character(length(x))
    return(.Call("R_hex_uint64_t", as.uint64(x), ret))
}

#' Replicate elements of uint64 vectors.
#' @param x uint64 vector to be replicated
#' @export
"rep.uint64" <- function(x, ...) {
    cx <- oldClass(x)
    ret <- NextMethod()
    oldClass(ret) <- cx
    return(ret)
}

#' Set the length of uint64 vector.
#' @param x uint64 vector
#' @param value value to set the new length
#' @export
"length<-.uint64" <- function(x, value) {
    cx <- oldClass(x)
    n <- length(x)
    x <- NextMethod()
    oldClass(x) <- cx
    if (value > n) {
        x[(n + 1):value] <- as.uint64(0)
    }
    return(x)
}

#' Extract uint64 vector parts
#' @param x uint64 vector
#' @export
"[.uint64" <- function(x,...) {
    cx <- oldClass(x)
    ret <- NextMethod()
    oldClass(ret) <- cx
    return(ret)
}

#' Extract uint64 vector parts
#' @param x uint64 vector
#' @export
"[[.uint64" <- function(x,...) {
    cx <- oldClass(x)
    ret <- NextMethod()
    oldClass(ret) <- cx
    return(ret)
}

#' Replace parts of uint64 vector
#' @param x uint64 vector
#' @param value uint64 replacement value
#' @export
"[<-.uint64" <- function(x,...,value) {
    cx <- oldClass(x)
    value <- as.uint64(value)
    ret <- NextMethod()
    oldClass(ret) <- cx
    return(ret)
}

#' Replace parts of uint64 vector 
#' @param x uint64 vector
#' @param value uint64 replacement value
#' @export
"[[<-.uint64" <- function(x,...,value) {
    cx <- oldClass(x)
    value <- as.uint64(value)
    ret <- NextMethod()
    oldClass(ret) <- cx
    return(ret)
}

uint64LogicCompare <- function(cfunc, x, y) {
    ret <- logical(max(length(x), length(y)))
    return(.Call(cfunc, as.uint64(x), as.uint64(y), ret))
}

#' Returns true if x and y are equal.
#' @param x uint64 vector
#' @param y uint64 vector
#' @useDynLib variantkey R_EQ_uint64
#' @export
"==.uint64" <- function(x, y) {
    return(uint64LogicCompare("R_EQ_uint64", x, y))
}

#' Returns true if x and y are different.
#' @param x uint64 vector
#' @param y uint64 vector
#' @useDynLib variantkey R_NE_uint64
#' @export
"!=.uint64" <- function(x, y) {
    return(uint64LogicCompare("R_NE_uint64", x, y))
}

#' Returns true if x is less than y.
#' @param x uint64 vector
#' @param y uint64 vector
#' @useDynLib variantkey R_LT_uint64
#' @export
"<.uint64" <- function(x, y) {
    return(uint64LogicCompare("R_LT_uint64", x, y))
}

#' Returns true if x is less or equal than y.
#' @param x uint64 vector
#' @param y uint64 vector
#' @useDynLib variantkey R_LE_uint64
#' @export
"<=.uint64" <- function(x, y) {
    return(uint64LogicCompare("R_LE_uint64", x, y))
}

#' Returns true if x is greater than y.
#' @param x uint64 vector
#' @param y uint64 vector
#' @useDynLib variantkey R_GT_uint64
#' @export
">.uint64" <- function(x, y) {
    return(uint64LogicCompare("R_GT_uint64", x, y))
}

#' Returns true if x is greater or equal than y.
#' @param x uint64 vector
#' @param y uint64 vector
#' @useDynLib variantkey R_GE_uint64
#' @export
">=.uint64" <- function(x, y) {
    return(uint64LogicCompare("R_GE_uint64", x, y))
}

#' Format uint64 vector for pretty printing.
#' @export
format.uint64 <- function(x, ...) {
    return(format(as.character(x), ...))
}

#' Prints uint64 argument and returns it invisibly.
#' @export
print.uint64 <- function(x, ...) {
    return(print(as.character(x), ...))
}

bindUint64 <- function(mode, recursive=FALSE, ...) {
    x <- list(...)
    n <- length(x)
    for (i in 1:n) {
        if (recursive && is.list(x[[i]])) {
            x[[i]] <- do.call("c.uint64", c(x[[i]], list(recursive=TRUE)))
        } else {
            if (!is.uint64(x[[i]])) {
                m <- names(x[[i]])
                x[[i]] <- as.uint64(x[[i]])
                names(x[[i]]) <- m
            }
            oldClass(x[[i]]) <- NULL
        }
    }
    ret <- do.call(mode, x)
    oldClass(ret) <- UINT64
    return(ret)
}

#' Concatenate uint64 vectors.
#' @param Two or more vectors coerced to uint64
#' @export
c.uint64 <- function(..., recursive=FALSE) {
    return(bindUint64(mode="c", recursive=recursive, ...))
}

#' Combine uint64 vectors by columns.
#' @export
cbind.uint64 <- function(...) {
    return(bindUint64(mode="cbind", recursive=FALSE, ...))
}

#' Combine uint64 vectors by rows.
#' @export
rbind.uint64 <- function(...) {
    return(bindUint64(mode="rbind", recursive=FALSE, ...))
}

remUint64Class <- function(x) {
    if (length(x)) {
        i <- (x == UINT64)
        if (any(i)) {
            return(x[!i])
        }
    }
    return(x)
}

#' Coerce uint64 vector to data.frame.
#' @param x uint64 vector
#' @export
as.data.frame.uint64 <- function(x, ...) {
    cx <- oldClass(x)
    on.exit(setattr(x, "class", cx))
    setattr(x, "class", remUint64Class(cx))
    ret <- as.data.frame(x, ...)
    n <- length(ret)
    for (i in 1:n) {
        setattr(ret[[i]], "class", cx)
    }
    return(ret)
}
