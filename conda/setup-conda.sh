#!/usr/bin/env bash
set -e -u -x -o pipefail

: ${MINICONDA_URL:=https://repo.continuum.io/miniconda}
: ${MINICONDA_FILE:=Miniconda3-4.4.10-Linux-x86_64.sh}
: ${MINICONDA_PIN_VERSION:=4.4.11}
: ${PYTHON_VERSION:=-}

if [ -x "$(command -v greadlink)" ]; then READLINK=greadlink; else READLINK=readlink; fi
PROJECT_CONDA="$(${READLINK} -f "$(dirname "${BASH_SOURCE[0]}")")"
PROJECT_HOME="$(dirname ${PROJECT_CONDA})"
PROJECT_ROOT="$(dirname ${PROJECT_HOME})"
PROJECT_ENV="${PROJECT_ROOT}/env-variantkey"

if [ ! -d "${PROJECT_ENV}" ]; then
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
"${PROJECT_ENV}/bin/conda" install -y conda-build pyyaml
