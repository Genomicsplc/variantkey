# MAKEFILE
#
# @author      Nicola Asuni <nicola.asuni@genomicsplc.com>
# @link        https://github.com/genomicsplc/variantkey
# ------------------------------------------------------------------------------

# List special make targets that are not associated with files
.PHONY: help testcpp test tidy build package_vk version doc format clean install uniinstall rpm deb

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

# Current directory
CURRENTDIR=$(dir $(realpath $(firstword $(MAKEFILE_LIST))))

# Name of RPM or DEB package
PKGNAME=${VENDOR}-${PROJECT}-headers

# Path for binary files (where the executable files will be installed)
LIBPATH=usr/include/$(VENDOR)/$(PROJECT)/

# Path path for documentation
DOCPATH=usr/share/doc/$(PKGNAME)/

# Installation path for the binary files
PATHINSTLIB=$(DESTDIR)/$(LIBPATH)

# Installation path for documentation
PATHINSTDOC=$(DESTDIR)/$(DOCPATH)

# RPM Packaging path (where RPMs will be stored)
PATHRPMPKG=$(CURRENTDIR)/target/RPM

# DEB Packaging path (where DEBs will be stored)
PATHDEBPKG=$(CURRENTDIR)/target/DEB

# --- MAKE TARGETS ---

# Display general help about this command
help:
	@echo ""
	@echo "$(PROJECT) Makefile."
	@echo "The following commands are available:"
	@echo ""
	@echo "    make qa        : Run all the tests and static analysis reports"
	@echo "    make test      : Run the unit tests"
	@echo "    make tidy      : Check the code using clang-tidy"
	@echo "    make build     : Build the library"
	@echo "    make version   : Set version from VERSION file"
	@echo "    make doc       : Generate source code documentation"
	@echo "    make format    : Format the source code"
	@echo "    make clean     : Remove any build artifact"
	@echo "    make install   : Install the library"
	@echo "    make uninstall : Uninstall the library"
	@echo "    make rpm       : Build an RPM package"
	@echo "    make deb       : Build a DEB package"
	@echo ""

all: clean version format test tidy build rpm deb package_vk doc

# Test C code compatibility with C++
testcpp:
	find ./src/variantkey -type f -name '*.h' -exec gcc -c -pedantic -Werror -Wall -Wextra -Wcast-align -Wundef -Wformat-security -std=c++14 -x c++ -o /dev/null {} \;

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
	lcov --capture --initial --directory . --output-file coverage/variantkey.cov && \
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
	clang-tidy -checks='*,-llvm-header-guard,-llvm-include-order,-android-cloexec-open,-hicpp-no-assembler,-hicpp-signed-bitwise,-clang-analyzer-alpha.*' -header-filter=.* -p . src/variantkey/*.h vk/*.c test/*.c test/rsidvar_bench/*.c

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

package_vk: version
	cd target/build/vk && make -j package
	../conda/setup-conda.sh
	PROJECT_ROOT=${CURRENTDIR} ${CONDA_ENV}/bin/conda build --prefix-length 128 --no-anaconda-upload --override-channels $(ARTIFACTORY_CONDA_CHANNELS) ../conda/c.src
	PROJECT_ROOT=${CURRENTDIR} ${CONDA_ENV}/bin/conda build --prefix-length 128 --no-anaconda-upload --override-channels $(ARTIFACTORY_CONDA_CHANNELS) ../conda/c.vk

# Set the version from VERSION file
version:
	sed -i "s/version:.*$$/version: $(VERSION)/" ../conda/c.src/meta.yaml
	sed -i "s/number:.*$$/number: $(RELEASE)/" ../conda/c.src/meta.yaml
	sed -i "s/version:.*$$/version: $(VERSION)/" ../conda/c.vk/meta.yaml
	sed -i "s/number:.*$$/number: $(RELEASE)/" ../conda/c.vk/meta.yaml

# Generate source code documentation
doc:
	cd target/build && \
	make doc | tee doc.log ; test $${PIPESTATUS[0]} -eq 0

# Format the source code
format:
	astyle --style=allman --recursive --suffix=none 'src/variantkey/*.h'
	astyle --style=allman --recursive --suffix=none 'test/*.c'
	astyle --style=allman --recursive --suffix=none 'vk/*.c'

# Remove any build artifact
clean:
	rm -rf target

# Install this application
install: uninstall
	mkdir -p $(PATHINSTLIB)
	cp src/variantkey/*.h $(PATHINSTLIB)
	find $(PATHINSTLIB) -type f -exec chmod 644 {} \;
	mkdir -p $(PATHINSTDOC)
	cp -f ../LICENSE $(PATHINSTDOC)
	cp -f ../README.md $(PATHINSTDOC)
	cp -f ../VERSION $(PATHINSTDOC)
	cp -f ../RELEASE $(PATHINSTDOC)
	chmod -R 644 $(PATHINSTDOC)*

# Remove all installed files (excluding configuration files)
uninstall:
	rm -rf $(PATHINSTLIB)
	rm -rf $(PATHINSTDOC)

# Build the RPM package for RedHat-like Linux distributions
rpm:
	rm -rf $(PATHRPMPKG)
	rpmbuild \
	--define "_topdir $(PATHRPMPKG)" \
	--define "_vendor $(VENDOR)" \
	--define "_owner $(OWNER)" \
	--define "_project $(PROJECT)" \
	--define "_package $(PKGNAME)" \
	--define "_version $(VERSION)" \
	--define "_release $(RELEASE)" \
	--define "_current_directory $(CURRENTDIR)" \
	--define "_libpath /$(LIBPATH)" \
	--define "_docpath /$(DOCPATH)" \
	-bb resources/rpm/rpm.spec

# Build the DEB package for Debian-like Linux distributions
deb:
	rm -rf $(PATHDEBPKG)
	make install DESTDIR=$(PATHDEBPKG)/$(PKGNAME)-$(VERSION)
	rm -f $(PATHDEBPKG)/$(PKGNAME)-$(VERSION)/$(DOCPATH)LICENSE
	tar -zcvf $(PATHDEBPKG)/$(PKGNAME)_$(VERSION).orig.tar.gz -C $(PATHDEBPKG)/ $(PKGNAME)-$(VERSION)
	cp -rf ./resources/debian $(PATHDEBPKG)/$(PKGNAME)-$(VERSION)/debian
	find $(PATHDEBPKG)/$(PKGNAME)-$(VERSION)/debian/ -type f -exec sed -i "s/~#DATE#~/`date -R`/" {} \;
	find $(PATHDEBPKG)/$(PKGNAME)-$(VERSION)/debian/ -type f -exec sed -i "s/~#PKGNAME#~/$(PKGNAME)/" {} \;
	find $(PATHDEBPKG)/$(PKGNAME)-$(VERSION)/debian/ -type f -exec sed -i "s/~#VERSION#~/$(VERSION)/" {} \;
	find $(PATHDEBPKG)/$(PKGNAME)-$(VERSION)/debian/ -type f -exec sed -i "s/~#RELEASE#~/$(RELEASE)/" {} \;
	echo $(LIBPATH) > $(PATHDEBPKG)/$(PKGNAME)-$(VERSION)/debian/$(PKGNAME).dirs
	echo "$(LIBPATH)* $(LIBPATH)" > $(PATHDEBPKG)/$(PKGNAME)-$(VERSION)/debian/install
	echo $(DOCPATH) >> $(PATHDEBPKG)/$(PKGNAME)-$(VERSION)/debian/$(PKGNAME).dirs
	echo "$(DOCPATH)* $(DOCPATH)" >> $(PATHDEBPKG)/$(PKGNAME)-$(VERSION)/debian/install
	echo "new-package-should-close-itp-bug" >> $(PATHDEBPKG)/$(PKGNAME)-$(VERSION)/debian/$(PKGNAME).lintian-overrides
	cd $(PATHDEBPKG)/$(PKGNAME)-$(VERSION) && debuild -us -uc
