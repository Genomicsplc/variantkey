FROM ubuntu:19.04

COPY docker/scripts /tmp/variantkey/docker/scripts
COPY python /tmp/variantkey/python
COPY README.md /tmp/variantkey/README.md
COPY c/src /tmp/variantkey/c/src
RUN /tmp/variantkey/docker/scripts/install-python-variantkey.sh && rm -rf /tmp/variantkey

