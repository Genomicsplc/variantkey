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
as.uint64 <- function (x, ...) {
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
as.uint64.NULL <- function (x, ...) {
    ret <- double()
    oldClass(ret) <- UINT64
    return(ret)
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
    ret <- double(length(x))
    ret <- .Call("R_double_to_uint64", x, ret)
    oldClass(ret) <- UINT64
    return(ret)
}

#' Coerce integer vector to uint64
#' @param x integer vector
#' @useDynLib variantkey R_integer_to_uint64
#' @export
as.uint64.integer <- function(x, ...) {
    ret <- double(length(x))
    ret <- .Call("R_integer_to_uint64", x, ret)
    oldClass(ret) <- UINT64
    return(ret)
}

#' Coerce logical vector to uint64
#' @param x logical vector
#' @useDynLib variantkey R_integer_to_uint64
#' @export
as.uint64.logical <- function(x, ...) {
    ret <- double(length(x))
    ret <- .Call("R_integer_to_uint64", x, ret)
    oldClass(ret) <- UINT64
    return(ret)
}

#' Coerce character vector to uint64
#' @param x character vector
#' @useDynLib variantkey R_decstr_to_uint64
#' @export
as.uint64.character <- function(x, ...) {
    ret <- double(length(x))
    ret <- .Call("R_decstr_to_uint64", x, ret)
    oldClass(ret) <- UINT64
    return(ret)
}

setAs("character", UINT64, function(from)as.uint64.character(from))

#' Coerce uint64 vector to character
#' @param x uint64 vector
#' @useDynLib variantkey R_uint64_to_decstr
#' @export
as.character.uint64 <- function(x, ...) {
    return(.Call("R_uint64_to_decstr", x, character(length(x))))
}

setAs(UINT64, "character", function(from)as.character.uint64(from))

#' Coerce uint64 vector to hexadecimal character vector.
#' @param x uint64 vector
#' @useDynLib variantkey R_hex_uint64_t
#' @export
as.uint64.hex <- function(x, ...) {
    ret <- double(length(x))
    ret <- .Call("R_hex_uint64_t", x, ret)
    oldClass(ret) <- UINT64
    return(ret)
}

#' Coerce hexadecimal character vector vector to uint64.
#' @param x hexadecimal character vector (16 characters per item)
#' @useDynLib variantkey R_parse_hex_uint64_t
#' @export
as.hex.uint64 <- function(x, ...) {
    return(.Call("R_parse_hex_uint64_t", x, character(length(x))))
}

#' Get or set the length of uint64 vector.
#' @param x uint64 vector
#' @param value value to set the new length
#' @export
"length<-.uint64" <- function(x, value) {
    n <- length(x)
    x <- NextMethod()
    oldClass(x) <- oldClass(x)
    if (value > n) {
        x[(n + 1):value] <- 0L
    }
    return(x)
}

#' Replicate elements of uint64 vectors.
#' @param x uint64 vector to be replicated
#' @export
"rep.uint64" <- function(x, ...) {
    ret <- NextMethod()
    oldClass(ret) <- oldClass(x)
    return(ret)
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

#' Format uint64 vector for pretty printing.
#' @export
format.uint64 <- function(x, justify="right", ...) {
    a <- attributes(x)
    ret <- format(as.character(x), justify=justify, ...)
    a$class <- remUint64Class(a$class)
    attributes(ret) <- a
    return(ret)
}

#' Prints uint64 argument and returns it invisibly.
#' @export
print.uint64 <- function(x, quote=FALSE, ...) {
    a <- attributes(x)
    ret <- as.character(x)
    a$class <- remUint64Class(a$class)
    attributes(ret) <- a
    print(ret, quote=quote, ...)
    invisible(x)
}

#' Compactly Display the Structure of an uint64 Object.
#' @export
str.uint64 <- function(object, vec.len=strO$vec.len, give.head=TRUE, give.length=give.head, ...) {
    strO <- strOptions()
    vec.len <- (2 * vec.len)
    n <- length(object)
    if (n > vec.len) {
        object <- object[seq_len(vec.len)]
    }
    cat(if (give.head)paste(" ", UINT64, " ", if (give.length && (n > 1)) paste("[1:",n,"] ",sep=""), sep=""), paste(as.character(object), collapse=" "), if (n > vec.len)" ...", " \n", sep="")
    invisible()
}

#' Extract uint64 vector parts
#' @param x uint64 vector
#' @export
"[.uint64" <- function(x,...) {
    ret <- NextMethod()
    oldClass(ret) <- oldClass(x)
    return(ret)
}

#' Extract uint64 vector parts
#' @param x uint64 vector
#' @export
"[[.uint64" <- function(x,...) {
    ret <- NextMethod()
    oldClass(ret) <- oldClass(x)
    return(ret)
}

#' Replace parts of uint64 vector
#' @param x uint64 vector
#' @param value uint64 replacement value
#' @export
"[<-.uint64" <- function(x,...,value) {
    value <- as.uint64(value)
    ret <- NextMethod()
    oldClass(ret) <- oldClass(x)
    return(ret)
}

#' Replace parts of uint64 vector 
#' @param x uint64 vector
#' @param value uint64 replacement value
#' @export
"[[<-.uint64" <- function(x,...,value) {
    value <- as.uint64(value)
    ret <- NextMethod()
    oldClass(ret) <- oldClass(x)
    return(ret)
}

#' Concatenate uint64 vectors.
#' @param Two or more vectors coerced to uint64
#' @export
c.uint64 <- function (..., recursive=FALSE) {
    l <- list(...)
    K <- length(l)
    for (k in 1:K) {
        if (recursive && is.list(l[[k]])) {
            l[[k]] <- do.call("c.uint64", c(l[[k]], list(recursive=TRUE)))
        } else {
            if (!is.uint64(l[[k]])) {
                s <- names(l[[k]])
                l[[k]] <- as.uint64(l[[k]])
                names(l[[k]]) <- s
            }
            oldClass(l[[k]]) <- NULL
        }
    }
    ret <- do.call("c", l)
    oldClass(ret) <- UINT64
    return(ret)
}

#' Combine uint64 vectors by columns.
#' @export
cbind.uint64 <- function(...) {
    l <- list(...)
    K <- length(l)
    for (k in 1:K) {
        if (!is.uint64(l[[k]])) {
            s <- names(l[[k]])
            l[[k]] <- as.uint64(l[[k]])
            names(l[[k]]) <- s
        }
        oldClass(l[[k]]) <- NULL
    }
    ret <- do.call("cbind", l)
    oldClass(ret) <- UINT64
    return(ret)
}

#' Combine uint64 vectors by rows.
#' @export
rbind.uint64 <- function(...) {
    l <- list(...)
    K <- length(l)
    for (k in 1:K) {
        if (!is.uint64(l[[k]])) {
            s <- names(l[[k]])
            l[[k]] <- as.uint64(l[[k]])
            names(l[[k]]) <- s
        }
        oldClass(l[[k]]) <- NULL
    }
    ret <- do.call("rbind", l)
    oldClass(ret) <- UINT64
    return(ret)
}

#' Coerce uint64 vector to data.frame.
#' @param x uint64 vector
#' @export
as.data.frame.uint64 <- function(x, ...) {
    c <- oldClass(x)
    on.exit(setattr(x, "class", c))
    setattr(x, "class", remUint64Class(c))
    ret <- as.data.frame(x, ...)
    k <- length(ret)
    for (i in 1:k) {
        setattr(ret[[i]], "class", c)
    }
    return(ret)
}

selattr <- function(x, y) {
    nx <- length(x)
    ny <- length(y)
    dx <- dim(x)
    dy <- dim(y)
    ldx <- length(dx)
    ldy <- length(dy)
    if (ldx) {
        if (ldy) {
            if (!identical(dx, dy)) {
                stop("non-conformable arrays")
            }
        } else {
            if (ny > nx) {
                stop("length(y) does not match dim(x)")
            }
            if (nx %% ny) {
                warning("length(x) not a multiple length(y)")
            }
        }
        return(attributes(x))
    }
    if (ldy) {
        if (nx > ny) {
            stop("length(x) does not match dim(y)")
        }
        if (ny %% nx) {
            warning("length(y) not a multiple length(x)")
        }
        return(attributes(y))
    }
    if (nx < ny) {
        if (ny %% nx) {
            warning("length(y) not a multiple length(x)")
        }
    } else {
        if (nx %% ny) {
            warning("length(x) not a multiple length(y)")
        }
    }
    return(attributes(x))
}

#' Returns true if x and y are equal.
#' @param x uint64 vector
#' @param y uint64 vector
#' @useDynLib variantkey R_EQ_uint64
#' @export
"==.uint64" <- function(x, y) {
    a <- selattr(x, y)
    x <- as.uint64(x)
    y <- as.uint64(y)
    ret <- logical(max(length(x), length(y)))
    ret <- .Call("R_EQ_uint64", x, y, ret)
    a$class <- remUint64Class(a$class)
    attributes(ret) <- a
    return(ret)
}

#' Returns true if x and y are different.
#' @param x uint64 vector
#' @param y uint64 vector
#' @useDynLib variantkey R_NE_uint64
#' @export
"!=.uint64" <- function(x, y) {
    a <- selattr(x, y)
    x <- as.uint64(x)
    y <- as.uint64(y)
    ret <- logical(max(length(x), length(y)))
    ret <- .Call("R_NE_uint64", x, y, ret)
    a$class <- remUint64Class(a$class)
    attributes(ret) <- a
    return(ret)
}

#' Returns true if x is less than y.
#' @param x uint64 vector
#' @param y uint64 vector
#' @useDynLib variantkey R_LT_uint64
#' @export
"<.uint64" <- function(x, y) {
    a <- selattr(x, y)
    x <- as.uint64(x)
    y <- as.uint64(y)
    ret <- logical(max(length(x), length(y)))
    ret <- .Call("R_LT_uint64", x, y, ret)
    a$class <- remUint64Class(a$class)
    attributes(ret) <- a
    return(ret)
}

#' Returns true if x is less or equal than y.
#' @param x uint64 vector
#' @param y uint64 vector
#' @useDynLib variantkey R_LE_uint64
#' @export
"<=.uint64" <- function(x, y) {
    a <- selattr(x, y)
    x <- as.uint64(x)
    y <- as.uint64(y)
    ret <- logical(max(length(x), length(y)))
    ret <- .Call("R_LE_uint64", x, y, ret)
    a$class <- remUint64Class(a$class)
    attributes(ret) <- a
    return(ret)
}

#' Returns true if x is greater than y.
#' @param x uint64 vector
#' @param y uint64 vector
#' @useDynLib variantkey R_GT_uint64
#' @export
">.uint64" <- function(x, y) {
    a <- selattr(x, y)
    x <- as.uint64(x)
    y <- as.uint64(y)
    ret <- logical(max(length(x), length(y)))
    ret <- .Call("R_GT_uint64", x, y, ret)
    a$class <- remUint64Class(a$class)
    attributes(ret) <- a
    return(ret)
}

#' Returns true if x is greater or equal than y.
#' @param x uint64 vector
#' @param y uint64 vector
#' @useDynLib variantkey R_GE_uint64
#' @export
">=.uint64" <- function(x, y) {
    a <- selattr(x, y)
    x <- as.uint64(x)
    y <- as.uint64(y)
    ret <- logical(max(length(x), length(y)))
    ret <- .Call("R_GE_uint64", x, y, ret)
    a$class <- remUint64Class(a$class)
    attributes(ret) <- a
    return(ret)
}
