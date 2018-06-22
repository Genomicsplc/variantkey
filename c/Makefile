# MAKEFILE
#
# @author      Nicola Asuni <nicola.asuni@genomicsplc.com>
# @link        https://github.com/genomicsplc/variantkey
# ------------------------------------------------------------------------------

# List special make targets that are not associated with files
.PHONY: help testcpp test tidy build package_vk version doc format clean

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
	@echo "$(PROJECT) Makefile."
	@echo "The following commands are available:"
	@echo ""
	@echo "    make qa         : Run all the tests and static analysis reports"
	@echo "    make test       : Run the unit tests"
	@echo "    make tidy       : Check the code using clang-tidy"
	@echo "    make build      : Build the library"
	@echo "    make version    : Set version from VERSION file"
	@echo "    make doc        : Generate source code documentation"
	@echo "    make format     : Format the source code"
	@echo "    make clean      : Remove any build artifact"
	@echo ""

all: clean version format test tidy build doc

# Test C code compatibility with C++
testcpp:
	gcc -c -x c++ -o /dev/null src/astring.c
	gcc -c -x c++ -o /dev/null src/binsearch.c
	gcc -c -x c++ -o /dev/null src/genoref.c
	gcc -c -x c++ -o /dev/null src/nrvk.c
	gcc -c -x c++ -o /dev/null src/rsidvar.c
	gcc -c -x c++ -o /dev/null src/variantkey.c

# Build and run the unit tests
test: testcpp
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
	clang-tidy -checks='*,-llvm-header-guard,-llvm-include-order,-android-cloexec-open' -header-filter=.* -p . src/*.c vk/*.c

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
	env CTEST_OUTPUT_ON_FAILURE=1 make test | tee build.log ; test $${PIPESTATUS[0]} -eq 0
	make package_vk

package_vk: version
	cd target/build/vk && make -j package
	../conda/setup-conda.sh
	${CONDA_ENV}/bin/conda build --prefix-length 128 --no-anaconda-upload --override-channels $(ARTIFACTORY_CONDA_CHANNELS) ../conda/c.vk

# Set the version from VERSION file
version:
	sed -i "s/version:.*$$/version: $(VERSION)/" ../conda/c.vk/meta.yaml
	sed -i "s/number:.*$$/number: $(RELEASE)/" ../conda/c.vk/meta.yaml

# Generate source code documentation
doc:
	cd target/build && \
	make doc | tee doc.log ; test $${PIPESTATUS[0]} -eq 0

# Format the source code
format:
	astyle --style=allman --recursive --suffix=none 'src/*.h'
	astyle --style=allman --recursive --suffix=none 'src/*.c'
	astyle --style=allman --recursive --suffix=none 'test/*.c'
	astyle --style=allman --recursive --suffix=none 'vk/*.c'

# Remove any build artifact
clean:
	rm -rf target
