#!/bin/bash

set -euf -o pipefail

script_dir="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"

source "${script_dir}/install-common.sh"

install_python_pyvariantkey_build_deps=( python3-setuptools build-essential python3-dev )
function install_python_pyvariantkey() {
    echo "Installing python pyvariantkey"
    (cd "${script_dir}/../../python-class" && python3 setup.py install)
}

install_python_pyvariantkey_dependencies_deps=( )
function install_python_pyvariantkey_dependencies() {
    echo "install-python-pyvariantkey.sh: installing apt dependencies"
    install_python_pyvariantkey_deps=( python3 )
    install_common_apt_install "${install_python_pyvariantkey_deps[@]}"
}

echo "install-python-pyvariantkey.sh: updating apt cache"
install_common_apt_update

echo "install-python-pyvariantkey.sh: installing python pyvariantkey dependencies"
install_common_run_with_build_deps install_python_pyvariantkey_dependencies "${install_python_pyvariantkey_dependencies_deps[@]}"

echo "install-python-pyvariantkey.sh: building python pyvariantkey"
install_common_run_with_build_deps install_python_pyvariantkey "${install_python_pyvariantkey_build_deps[@]}"

echo "install-python-pyvariantkey.sh: cleaning up"
install_common_apt_cleanup

echo "install-python-pyvariantkey.sh: done."
