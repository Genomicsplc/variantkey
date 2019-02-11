#!/bin/bash

set -euf -o pipefail

script_dir="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"

source "${script_dir}/install-common.sh"

install_python_variantkey_build_deps=( python3-setuptools build-essential python3-dev )
function install_python_variantkey() {
    echo "Installing python variantkey"
    (cd "${script_dir}/../../python" && python3 setup.py install)
}

install_python_variantkey_dependencies_deps=( )
function install_python_variantkey_dependencies() {
    echo "install-python-variantkey.sh: installing apt dependencies"
    install_python_variantkey_deps=( python3 )
    install_common_apt_install "${install_python_variantkey_deps[@]}"
}

echo "install-python-variantkey.sh: updating apt cache"
install_common_apt_update

echo "install-python-variantkey.sh: installing python variantkey dependencies"
install_common_run_with_build_deps install_python_variantkey_dependencies "${install_python_variantkey_dependencies_deps[@]}"

echo "install-python-variantkey.sh: building python variantkey"
install_common_run_with_build_deps install_python_variantkey "${install_python_variantkey_build_deps[@]}"

echo "install-python-variantkey.sh: cleaning up"
install_common_apt_cleanup

echo "install-python-variantkey.sh: done."
