#!/usr/bin/env bash
set -e -u -x -o pipefail

: ${MINICONDA_URL:=https://repo.continuum.io/miniconda}
: ${MINICONDA_PIN_VERSION:=-}
: ${PYTHON_VERSION:=-}
: ${ENV_NAME:=env-variantkey}

if [[ "$OSTYPE" == "linux"* ]]; then
    READLINK=readlink
    MINICONDA_FILE="Miniconda3-latest-Linux-x86_64.sh"
elif [[ "$OSTYPE" == "darwin"* ]]; then
    READLINK=greadlink # to install greadlink: brew install coreutils
    MINICONDA_FILE="Miniconda3-latest-MacOSX-x86_64.sh"
fi

PROJECT_CONDA="$(${READLINK} -f "$(dirname "${BASH_SOURCE[0]}")")"
PROJECT_HOME="$(dirname ${PROJECT_CONDA})"
PROJECT_ROOT="$(dirname ${PROJECT_HOME})"
PROJECT_ENV="${PROJECT_ROOT}/${ENV_NAME}"

if [ ! -d "${PROJECT_ENV}" ]; then
    mkdir -p "${PROJECT_HOME}/target"
    curl "${MINICONDA_URL}/${MINICONDA_FILE}" > "${PROJECT_HOME}/target/${MINICONDA_FILE}"
    bash "${PROJECT_HOME}/target/${MINICONDA_FILE}" -b -p "${PROJECT_ENV}"
    if [ "${MINICONDA_PIN_VERSION}" != "-" ]; then
        # pin the exact conda version
        echo "conda ==${MINICONDA_PIN_VERSION}" >> "${PROJECT_ENV}/conda-meta/pinned"
    fi
    if [ "${PYTHON_VERSION}" != "-" ]; then
         # pin the exact python version
        echo "python ==${PYTHON_VERSION}" >> "${PROJECT_ENV}/conda-meta/pinned"
    fi
fi

set +u
"${PROJECT_ENV}/bin/conda" install -y conda-build conda-verify pyyaml
