# MAKEFILE
#
# @author      Nicola Asuni <nicola.asuni@genomicsplc.com>
# @link        https://github.com/genomicsplc/variantkey
# ------------------------------------------------------------------------------

# List special make targets that are not associated with files
.PHONY: help qa test tidy build python version conda go r doc format clean

# Use bash as shell (Note: Ubuntu now uses dash which doesn't support PIPESTATUS).
SHELL=/bin/bash

# CVS path (path to the parent dir containing the project)
CVSPATH=github.com/genomicsplc

# Project owner
OWNER=GENOMICSplc

# Project vendor
VENDOR=genomicsplc

# Project name
PROJECT=variantkey

# Project version
VERSION=$(shell cat VERSION)

# Project release number (packaging build number)
RELEASE=$(shell cat RELEASE)

# Name of RPM or DEB package
PKGNAME=${VENDOR}-${PROJECT}

# Current directory
CURRENTDIR=$(shell pwd)

# Conda environment
CONDA_ENV=${CURRENTDIR}/../env-${PROJECT}

# Include default build configuration
include $(CURRENTDIR)/config.mk

# --- MAKE TARGETS ---

# Display general help about this command
help:
	@echo ""
	@echo "$(PROJECT) Makefile."
	@echo "GOPATH=$(GOPATH)"
	@echo "The following commands are available:"
	@echo ""
	@echo "    make qa      : Run all the tests and static analysis reports"
	@echo "    make test    : Run the unit tests"
	@echo "    make tidy    : Check the code using clang-tidy"
	@echo "    make build   : Build the library"
	@echo "    make python  : Build and test the python module"
	@echo "    make version : Set version from VERSION file"
	@echo "    make conda   : Build a conda package for the python wrapper"
	@echo "    make go      : Test the golang cgo module"
	@echo "    make r       : Build the R module"
	@echo "    make doc     : Generate source code documentation"
	@echo "    make format  : Format the source code"
	@echo "    make clean   : Remove any build artifact"
	@echo ""

all: clean format qa build doc go python pytest r

# Alias for test
qa: test tidy

# Build and run the unit tests
test:
	@mkdir -p target/test/test
	@echo -e "\n\n*** BUILD TEST - see config.mk ***\n"
	rm -rf target/test/*
	mkdir -p target/test/coverage
	cd target/test && \
	cmake -DCMAKE_C_FLAGS=$(CMAKE_C_FLAGS) \
	-DCMAKE_TOOLCHAIN_FILE=$(CMAKE_TOOLCHAIN_FILE) \
	-DCMAKE_BUILD_TYPE=Coverage \
	-DCMAKE_INSTALL_PREFIX=$(CMAKE_INSTALL_PATH) \
	-DBUILD_SHARED_LIB=$(VH_BUILD_SHARED_LIB) \
	-DBUILD_DOXYGEN=$(VH_BUILD_DOXYGEN) \
	../.. | tee cmake.log ; test $${PIPESTATUS[0]} -eq 0 && \
	export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:./ && \
	make | tee make.log ; test $${PIPESTATUS[0]} -eq 0 && \
	lcov --zerocounters --directory . && \
	lcov --capture --initial --directory . --output-file coverage/variantkey && \
	env CTEST_OUTPUT_ON_FAILURE=1 make test | tee test.log ; test $${PIPESTATUS[0]} -eq 0 && \
	lcov --no-checksum --directory . --capture --output-file coverage/variantkey.info && \
	lcov --remove coverage/variantkey.info "/test_*" --output-file coverage/variantkey.info && \
	genhtml -o coverage -t "VariantKey Test Coverage" coverage/variantkey.info
ifeq ($(VH_BUILD_DOXYGEN),ON)
	cd target && \
	make doc | tee doc.log ; test $${PIPESTATUS[0]} -eq 0
endif

# use clang-tidy
tidy:
	clang-tidy -checks='*,-llvm-header-guard,-llvm-include-order,-android-cloexec-open' -header-filter=.* -p . src/*.c

# Build the library
build:
	@mkdir -p target/build
	@echo -e "\n\n*** BUILD RELEASE - see config.mk ***\n"
	rm -rf target/build/*
	cd target/build && \
	cmake -DCMAKE_C_FLAGS=$(CMAKE_C_FLAGS) \
	-DCMAKE_TOOLCHAIN_FILE=$(CMAKE_TOOLCHAIN_FILE) \
	-DCMAKE_BUILD_TYPE=Release \
	-DCMAKE_INSTALL_PREFIX=$(CMAKE_INSTALL_PATH) \
	-DBUILD_SHARED_LIB=$(VH_BUILD_SHARED_LIB) \
	-DBUILD_DOXYGEN=$(VH_BUILD_DOXYGEN) \
	../.. | tee cmake.log ; test $${PIPESTATUS[0]} -eq 0 && \
	export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:./ && \
	make | tee make.log ; test $${PIPESTATUS[0]} -eq 0
	cd target/build && \
	export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:./ && \
	env CTEST_OUTPUT_ON_FAILURE=1 make test | tee test.log ; test $${PIPESTATUS[0]} -eq 0

# Set the version from VERSION file
version:
	sed -i "s/version:.*$$/version: $(VERSION).$(RELEASE)/" conda/meta.yaml
	sed -i "s/version=.*,$$/version='$(VERSION)',/" python/setup.py

# Build the python module
python: version
	cd python && \
	rm -rf ./build && \
	python3 setup.py build_ext --include-dirs=../src && \
	rm -f tests/*.so && \
	find build/ -iname '*.so' -exec cp {} tests/ \; && \
	python3 setup.py test

# Build a conda package
conda: version
	@mkdir -p target
	./conda/setup-conda.sh && \
	${CONDA_ENV}/bin/conda build --prefix-length 160 --no-anaconda-upload --no-remove-work-dir --override-channels $(ARTIFACTORY_CONDA_CHANNELS) conda

# Test golang go module
go:
	cd go && \
	make deps qa

# Build R module
r:
	cd r/src && \
	R CMD SHLIB rvariantkey.c && \
	./test_rvariantkey.R

# Generate source code documentation
doc:
	cd target/build && \
	make doc | tee doc.log ; test $${PIPESTATUS[0]} -eq 0

# Format the source code
format:
	astyle --style=allman --recursive --suffix=none 'src/*.h'
	astyle --style=allman --recursive --suffix=none 'src/*.c'
	astyle --style=allman --recursive --suffix=none 'test/*.c'
	astyle --style=allman --recursive --suffix=none 'python/variantkey/*.h'
	astyle --style=allman --recursive --suffix=none 'python/variantkey/*.c'
	astyle --style=allman --recursive --suffix=none 'r/src/*.c'
	autopep8 --in-place --max-line-length=255 ./python/tests/*.py
	cd go && make format

# Remove any build artifact
clean:
	rm -rf target
	rm -rf ./go/target ./go/src.test
	rm -f ./r/src/*.o ./r/src/*.so
	rm -rf ./python/htmlcov ./python/build ./python/dist ./python/.cache ./python/.benchmarks ./python/tests/*.so ./python/tests/__pycache__ ./python/variantkey/__pycache__ ./python/variantkey.egg-info
	find . -type f -name '*.pyc' -exec rm -f {} \;
