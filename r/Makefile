# MAKEFILE
#
# @author      Nicola Asuni <nicola.asuni@genomicsplc.com>
# @link        https://github.com/genomicsplc/variantkey
# ------------------------------------------------------------------------------

# List special make targets that are not associated with files
.PHONY: help build test format clean

# --- MAKE TARGETS ---

# Display general help about this command
help:
	@echo ""
	@echo "VariantKey R Makefile."
	@echo "Requires: node-js and uglify-js."
	@echo "The following commands are available:"
	@echo ""
	@echo "    make build   : Build the library"
	@echo "    make test    : Test the library"
	@echo "    make format  : Format the source code"
	@echo "    make clean   : Remove any build artifact"
	@echo ""

all: clean format build test

# Build a minified version of the library
build:
	cd src && \
	R CMD SHLIB rvariantkey.c

# Test the library
test:
	cd test && \
	./test_rvariantkey.R

# Format the source code
format:
	astyle --style=allman --recursive --suffix=none 'src/*.c'
	astyle --style=google --recursive --suffix=none 'src/*.R'
	astyle --style=google --recursive --suffix=none 'test/*.R'
	astyle --style=google --recursive --suffix=none 'example/*.R'

# Remove any build artifact
clean:
	rm -f src/*.o src/*.so
