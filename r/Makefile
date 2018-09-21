# MAKEFILE
#
# @author      Nicola Asuni <nicola.asuni@genomicsplc.com>
# @link        https://github.com/genomicsplc/variantkey
# ------------------------------------------------------------------------------

# List special make targets that are not associated with files
.PHONY: help version build test format clean devdoc

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

# Current directory
CURRENTDIR=$(dir $(realpath $(firstword $(MAKEFILE_LIST))))

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
	@echo "    make devdoc  : Execute devtools::document()"
	@echo ""

all: clean format build

# Set the version from VERSION file
version:
	sed -i "s/version:.*$$/version: $(VERSION)/" ../conda/r/meta.yaml
	sed -i "s/number:.*$$/number: $(RELEASE)/" ../conda/r/meta.yaml
	sed -i "s/Version:.*$$/Version: $(VERSION).$(RELEASE)/" variantkey/DESCRIPTION

# Build the library
build: version
	../conda/setup-conda.sh
	PROJECT_ROOT=${CURRENTDIR} ${CONDA_ENV}/bin/conda build --prefix-length 128 --no-anaconda-upload --override-channels $(ARTIFACTORY_CONDA_CHANNELS) ../conda/r

# Format the source code
format:
	astyle --style=allman --recursive --suffix=none 'variantkey/src/*.c'
	#astyle --style=google --recursive --suffix=none 'variantkey/R/*.R'
	#astyle --style=google --recursive --suffix=none 'variantkey/tests/*.R'
	#astyle --style=google --recursive --suffix=none 'example/*.R'

# Remove any build artifact
clean:
	rm -rf variantkey.Rcheck

devdoc:
	cd variantkey &&\
	R -e "devtools::document()"
