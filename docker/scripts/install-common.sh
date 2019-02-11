#!/bin/bash error-source-me.sh

function install_common_apt_update() {
    if [ -z ${SUPPRESS_install_common_apt_update+x} ]; then
        echo "install-common.sh: calling apt-get update"
        apt-get -qq update
    else
        echo "install-common.sh: suppressing apt update since SUPPRESS_install_common_apt_update is set"
    fi
}

function install_common_apt_cleanup() {
    if [ -z ${SUPPRESS_install_common_apt_cleanup+x} ]; then
        echo "install-common.sh: removing extraneous packages"
        apt-get -qq autoremove

        echo "install-common.sh: clearing apt cache"
        rm -rf /var/lib/apt/lists/*
    else
        echo "install-common.sh: suppressing apt cleanup since SUPPRESS_install_common_apt_cleanup is set"
    fi
}

function install_common_apt_install() {
    apt_packages=("${@}")

    echo "install-common.sh: installing apt packages: ("${apt_packages[@]}")"
    apt-get -qq install --no-install-recommends ${apt_packages[@]}
}

function install_common_run_with_build_deps() {
    run=$1
    shift 1
    apt_build_deps=("${@}")
    apt_build_deps_have=$(comm -12 <(for dep in "${apt_build_deps[@]}"; do echo "${dep}"; done | sort) <(dpkg-query -W -f='${db:Status-Abbrev} ${Package} ${Version}\n' | awk '$1=="ii" {package=$2; version=$3; print package; print package"="version; }' | sort))
    apt_build_deps_need=$(comm -23 <(for dep in "${apt_build_deps[@]}"; do echo "${dep}"; done | sort) <(dpkg-query -W -f='${db:Status-Abbrev} ${Package} ${Version}\n' | awk '$1=="ii" {package=$2; version=$3; print package; print package"="version; }' | sort))
    
    echo "install-common.sh: installing temporary build deps: ("${apt_build_deps_need[@]}") already have: ("${apt_build_deps_have[@]}")"
    install_common_apt_install ${apt_build_deps_need[@]}

    echo "install-common.sh: running ${run}"
    ${run}

    echo "install-common.sh: removing temporary build deps: ("${apt_build_deps_need[@]}")"
    apt-get -qq remove ${apt_build_deps_need[@]}
}

function install_common_apt_add_source_inner() {
    name=$1
    source=$2
    cat <<APT_LINE | tee /etc/apt/sources.list.d/${name}.list
deb ${source} $(lsb_release --codename --short) universe
deb-src ${source} $(lsb_release --codename --short) universe
APT_LINE
}

function install_common_apt_add_source() {
    name=$1
    source=$2
    install_common_apt_add_source_inner_deps=( apt-transport-https lsb-release )
    install_common_run_with_build_deps "install_common_apt_add_source_inner ${name} ${source}" "${install_common_apt_add_source_inner_deps[@]}"
}

function install_common_apt_add_key_inner() {
    name=$1
    key_url=$2
    wget -O - ${key_url} | apt-key add -
}

function install_common_apt_add_key() {
    name=$1
    key_url=$2
    install_common_apt_add_key_inner_deps=( wget gnupg ca-certificates )
    install_common_run_with_build_deps "install_common_apt_add_key_inner ${name} ${key_url}" "${install_common_apt_add_key_inner_deps[@]}"
}

