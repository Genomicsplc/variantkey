#!/usr/bin/env bash
#
# vkhexbin.sh
#
# Process the variantKey HEX file to generate the final binary counterparts:
# - vkrs.bin : VariantKey to rsID
# - rsvk.bin : rsID to VariantKey
# - vknr.bin : Non-reversible VariantKey REF+ALT map
#
# Requires:
#  - vt
#  - bcftoolsvh (Genomics PLC version of bcftools)
#  - sort (coreutils)
#  - xxd (vim-common)
#
#  On Debian/Ubuntu:
#  sudo apt-get update && sudo apt-get install -y --allow-unauthenticated vt bcftoolsvh tabix coreutils vim-common
#
# Nicola Asuni
# ------------------------------------------------------------------------------
set -e -u -x -o pipefail

: ${VCF_INPUT_FILE:?}   # Input VCF file
: ${VCF_OUTPUT_NAME:?}  # Name to be used for the output VCF file (e.g. dbsnp)
: ${PARALLEL:=4}        # Number of parallel processes used by "sort"

if [ -x "$(command -v greadlink)" ]; then READLINK=greadlink; else READLINK=readlink; fi
SCRIPT_DIR=$(${READLINK} -f $(dirname "$0"))

# generate VariantKey hex files
bcftools +variantkey-hex "${VCF_INPUT_FILE}"

# --- VARIANTKEY -> RSID BINARY FILE
source "${SCRIPT_DIR}/vkrs.sh"

# --- RSID -> VARIANTKEY BINARY FILE
source "${SCRIPT_DIR}/rsvk.sh"

# --- NON-REVERSIBLE VARIANTKEY BINARY FILE
source "${SCRIPT_DIR}/nrvk.sh"

# --- ADD VARIANTKEY TO THE VCF FILE

# Add VariantKey fields in the VCF
bcftools +add-variantkey "${VCF_INPUT_FILE}" > "${VCF_OUTPUT_NAME}.vcf"

# Compress VCF file
bgzip "${VCF_OUTPUT_NAME}.vcf"

# Index VCF file
vt index "${VCF_OUTPUT_NAME}.vcf.gz"
