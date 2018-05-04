# MAKEFILE
#
# @author      Nicola Asuni <nicola.asuni@genomicsplc.com>
# @link        https://github.com/genomicsplc/variantkey
# ------------------------------------------------------------------------------

# List special make targets that are not associated with files
.PHONY: help c go javascript python r format clean

# Conda environment
CONDA_ENV=${CURRENTDIR}/../env-${PROJECT}

# --- MAKE TARGETS ---

# Display general help about this command
help:
	@echo ""
	@echo "$(PROJECT) Makefile."
	@echo "The following commands are available:"
	@echo ""
	@echo "    make c          : Build and test the C version"
	@echo "    make go         : Build and test the GO version"
	@echo "    make javascript : Build and test the Javascript version"
	@echo "    make python     : Build and test the Python version"
	@echo "    make r          : Build and test the R version"
	@echo "    make clean      : Remove any build artifact"
	@echo ""

all: clean c go javascript python r

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

# Build a conda package
conda:
	./conda/setup-conda.sh
	${CONDA_ENV}/bin/conda build --prefix-length 128 --no-anaconda-upload --no-remove-work-dir --override-channels $(ARTIFACTORY_CONDA_CHANNELS) conda/python
	${CONDA_ENV}/bin/conda build --prefix-length 128 --no-anaconda-upload --override-channels $(ARTIFACTORY_CONDA_CHANNELS) conda/c.vk

