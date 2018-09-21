# MAKEFILE
#
# @author      Nicola Asuni <nicola.asuni@genomicsplc.com>
# @link        https://github.com/genomicsplc/variantkey
# ------------------------------------------------------------------------------

# List special make targets that are not associated with files
.PHONY: help c go javascript python python-class r format clean

# CVS path (path to the parent dir containing the project)
CVSPATH=github.com/genomicsplc

# Project vendor
VENDOR=genomicsplc

# Project name
PROJECT=variantkey

# --- MAKE TARGETS ---

# Display general help about this command
help:
	@echo ""
	@echo "VariantKey Makefile."
	@echo "The following commands are available:"
	@echo ""
	@echo "    make c            : Build and test the C version"
	@echo "    make go           : Build and test the GO version"
	@echo "    make javascript   : Build and test the Javascript version"
	@echo "    make python       : Build and test the Python version"
	@echo "    make python-class : Build and test the Python wrapper class"
	@echo "    make r            : Build and test the R version"
	@echo "    make clean        : Remove any build artifact"
	@echo "    make dbuild       : Build everything inside a Docker container"
	@echo ""

all: clean c go javascript python python-class r

# Build and test the C version
c:
	cd c && make all

# Build and test the GO version
go:
	cd go && make all

# Build and test the Javascript version
javascript:
	cd javascript && make all

# Build and test the Python version
python:
	cd python && make all

# Build and test the Python wrapper class
python-class:
	cd python-class && make all

# Build and test the R version
r:
	cd r && make all

# Remove any build artifact
clean:
	rm -rf target
	cd c && make clean
	cd go && make clean
	cd javascript && make clean
	cd python && make clean
	cd r && make clean

# Build everything inside a Docker container
dbuild:
	@mkdir -p target
	@rm -rf target/*
	@echo 0 > target/make.exit
	CVSPATH=$(CVSPATH) VENDOR=$(VENDOR) PROJECT=$(PROJECT) MAKETARGET='$(MAKETARGET)' ./dockerbuild.sh
	@exit `cat target/make.exit`
