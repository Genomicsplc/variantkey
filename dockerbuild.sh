#!/bin/sh
#
# dockerbuild.sh
#
# Build the software inside a Docker container
#
# @author      Nicola Asuni <info@tecnick.com>
# ------------------------------------------------------------------------------

# NOTES:
# This script requires Docker

# EXAMPLE USAGE:
# CVSPATH=project VENDOR=vendorname PROJECT=projectname MAKETARGET=all ./dockerbuild.sh

# Get vendor and project name
: ${CVSPATH:=project}
: ${VENDOR:=vendor}
: ${PROJECT:=project}

# make target to execute
: ${MAKETARGET:=all}

# Name of the base development Docker image
DOCKERDEV=${VENDOR}/dev_${PROJECT}

# Build the base environment and keep it cached locally
docker build --pull --tag ${DOCKERDEV} --file ./resources/Docker/Dockerfile.dev ./resources/Docker/

# Define the project root path
PRJPATH=/root/src/${CVSPATH}/${PROJECT}

# Generate a temporary Dockerfile to build and test the project
# NOTE: The exit status of the RUN command is stored to be returned later,
#       so in case of error we can continue without interrupting this script.
cat > Dockerfile.test <<- EOM
FROM ${DOCKERDEV}
RUN mkdir -p ${PRJPATH}
ADD ./ ${PRJPATH}
WORKDIR ${PRJPATH}
RUN make ${MAKETARGET} || (echo \$? > target/make.exit)
EOM

# Define the temporary Docker image name
DOCKER_IMAGE_NAME=${VENDOR}/build_${PROJECT}

# Build the Docker image
docker build --no-cache --tag ${DOCKER_IMAGE_NAME} --file Dockerfile.test .

# Start a container using the newly created Docker image
CONTAINER_ID=$(docker run -d ${DOCKER_IMAGE_NAME})

# Copy all build/test artifacts back to the host
docker cp ${CONTAINER_ID}:"${PRJPATH}/target" ./

# Remove the temporary container and image
docker rm -f ${CONTAINER_ID} || true
docker rmi -f ${DOCKER_IMAGE_NAME} || true
