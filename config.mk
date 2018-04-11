# This file sets the default build of the library and is run by typing "make"

# Build shared library ON/OFF
VH_BUILD_SHARED_LIB:=ON

# Architecture
CMAKE_C_FLAGS=

# Tool chain 
# options: ../../resources/cmake/mingw64-cross.cmake
#          ../../resources/cmake/mingw32-cross.cmake
CMAKE_TOOLCHAIN_FILE=

# Conda channels
ARTIFACTORY_CONDA_CHANNELS=-c bioconda -c conda-forge -c https://repo.continuum.io/pkgs/main -c https://repo.continuum.io/pkgs/free
