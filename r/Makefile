# MAKEFILE
#
# @author      Nicola Asuni <nicola.asuni@genomicsplc.com>
# @link        https://github.com/genomicsplc/variantkey
# ------------------------------------------------------------------------------

# List special make targets that are not associated with files
.PHONY: help version conda build test format clean

# Project name
PROJECT=variantkey

# Project version
VERSION=$(shell cat ../VERSION)

# Project release number (packaging build number)
RELEASE=$(shell cat ../RELEASE)

# Include conda channels
include ../conda/channels.mk

# Conda environment
CONDA_ENV=../../env-${PROJECT}

# --- MAKE TARGETS ---

# Display general help about this command
help:
	@echo ""
	@echo "VariantKey R Makefile."
	@echo "The following commands are available:"
	@echo ""
	@echo "    make build   : Build the library"
	@echo "    make test    : Test the library"
	@echo "    make format  : Format the source code"
	@echo "    make clean   : Remove any build artifact"
	@echo ""

all: clean format build test

# Set the version from VERSION file
version:
	sed -i "s/version:.*$$/version: $(VERSION)/" ../conda/r/meta.yaml
	sed -i "s/number:.*$$/number: $(RELEASE)/" ../conda/r/meta.yaml

# Build the library
conda:
	../conda/setup-conda.sh
	${CONDA_ENV}/bin/conda build --prefix-length 128 --no-anaconda-upload --no-remove-work-dir --override-channels $(ARTIFACTORY_CONDA_CHANNELS) ../conda/r

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
