# MAKEFILE
#
# @author      Nicola Asuni <nicola.asuni@genomicsplc.com>
# @link        https://github.com/genomicsplc/varianthash
# ------------------------------------------------------------------------------

# List special make targets that are not associated with files
.PHONY: help qa test build python pytest go cgo doc format clean

# Use bash as shell (Note: Ubuntu now uses dash which doesn't support PIPESTATUS).
SHELL=/bin/bash

# CVS path (path to the parent dir containing the project)
CVSPATH=github.com/genomicsplc

# Project owner
OWNER=GENOMICSplc

# Project vendor
VENDOR=genomicsplc

# Project name
PROJECT=varianthash

# Project version
VERSION=$(shell cat VERSION)

# Project release number (packaging build number)
RELEASE=$(shell cat RELEASE)

# Name of RPM or DEB package
PKGNAME=${VENDOR}-${PROJECT}

# Current directory
CURRENTDIR=$(shell pwd)

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
	@echo "    make build   : Build the library"
	@echo "    make python  : Build the python module"
	@echo "    make pytest  : Test the python module"
	@echo "    make go      : Test the native golang module"
	@echo "    make cgo     : Test the golang cgo module"
	@echo "    make doc     : Generate source code documentation"
	@echo "    make format  : Format the source code"
	@echo "    make clean   : Remove any build artifact"
	@echo ""

# Alias for help target
all: clean format test build doc cgo go python pytest

# BUikd and run the unit tests
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
	lcov --capture --initial --directory . --output-file coverage/varianthash && \
	env CTEST_OUTPUT_ON_FAILURE=1 make test | tee test.log ; test $${PIPESTATUS[0]} -eq 0 && \
	lcov --no-checksum --directory . --capture --output-file coverage/varianthash.info && \
	lcov --remove coverage/varianthash.info "/test_*" --output-file coverage/varianthash.info && \
	genhtml -o coverage -t "VariantHash Test Coverage" coverage/varianthash.info
ifeq ($(VH_BUILD_DOXYGEN),ON)
	cd target && \
	make doc | tee doc.log ; test $${PIPESTATUS[0]} -eq 0
endif

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

# Build the python module
python:
	cd python && \
	rm -rf ./build && \
	python3 setup.py build_ext --include-dirs=../src && \
	rm -f tests/*.so && \
	find build/ -iname '*.so' -exec cp {} tests/ \;

# Test python module
pytest:
	cd python && \
	python3 setup.py test

# Test golang module
go:
	cd go && \
	make qa

# Test golang cgo module
cgo:
	cd cgo && \
	make qa

# Generate source code documentation
doc:
	cd target/build && \
	make doc | tee doc.log ; test $${PIPESTATUS[0]} -eq 0

# Format the source code
format:
	astyle --style=allman --recursive --suffix=none 'src/*.h'
	astyle --style=allman --recursive --suffix=none 'src/*.c'
	astyle --style=allman --recursive --suffix=none 'test/*.c'
	astyle --style=allman --recursive --suffix=none 'python/src/*.h'
	astyle --style=allman --recursive --suffix=none 'python/src/*.c'
	#autopep8 --in-place --aggressive --aggressive ./python/tests/*.py
	cd cgo && make format
	cd go && make format

# Remove any build artifact
clean:
	rm -rf ./target
	rm -rf ./python/build
	rm -rf ./python/.cache
	rm -rf ./python/tests/*.so
	rm -rf ./python/tests/__pycache__
	rm -rf ./cgo/target
	rm -rf ./go/target
