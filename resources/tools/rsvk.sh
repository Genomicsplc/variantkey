#!/usr/bin/env bash
#
# rsvk.sh
#
# Normalize VCF files (allele decomposition + normalization)
# Generate RSID<->VariantKey binary files
#
# Requires:
#  - vt
#  - bcftoolsvh (Genomics PLC version of bcftools)
#  - tabix
#  - sort (coreutils)
#  - xxd (vim-common)
#
#  On Debian/Ubuntu:
#  sudo apt-get update && sudo apt-get install -y --allow-unauthenticated vt bcftoolsvh tabix coreutils vim-common
#
# Nicola Asuni
# ------------------------------------------------------------------------------
set -e -u -x -o pipefail

: ${VCF_INPUT_FILE:?}                 # Input VCF file
: ${VCF_OUTPUT_NAME:?}                # Name to be used for the output VCF file (e.g. dbsnp)
: ${REFERENCE_GENOME_FASTA_FILE:?}    # Genome reference genome FASTA file
: ${GENOME_ASSEMBLY:?}                # Genome Assembly code (e.g. GRCh37.p13.b150)
: ${PARALLEL:=4}                      # Number of parallel processes used by "sort"


if [ -x "$(command -v greadlink)" ]; then READLINK=greadlink; else READLINK=readlink; fi
SCRIPT_DIR=$(${READLINK} -f $(dirname "$0"))
VERSION=$(cat ${SCRIPT_DIR}/../../VERSION)
DATE=$(date -u --iso-8601=d)

# Add VariantKey fields in the VCF (this generates the final normalized VCF file)
bcftools +add-variantkey ${VCF_INPUT_FILE} > ${VCF_OUTPUT_NAME}.vcf


# --- RSID -> VARIANTKEY BINARY FILE ---

# Create a RSID to VariantKey HEX file
bcftools query -f '%RSX%VKX\n' ${VCF_OUTPUT_NAME}.vcf > rsvk.hex

#  Sort the RSID to VariantKey HEX file
LC_ALL=C sort --parallel=${PARALLEL} --output=rsvk.sorted.hex rsvk.hex

# Remove temporary file
rm -f rsvk.hex

# Convert HEX file in binary format (the newlines are removed to get adjacent blocks of 160 bits)
xxd -r -p rsvk.sorted.hex rsvk.bin

# Remove temporary file
rm -f rsvk.sorted.hex


# --- VARIANTKEY -> RSID BINARY FILE ---

# Create VariantKey to RSID HEX file
bcftools query -f '%VKX%RSX\n' ${VCF_OUTPUT_NAME}.vcf > vkrs.hex

#  Sort the RSID to VariantKey HEX file
LC_ALL=C sort --parallel=${PARALLEL} --output=vkrs.sorted.hex vkrs.hex

# Remove temporary file
rm -f vkrs.hex

# Convert HEX file in binary format (the newlines are removed to get adjacent blocks of 160 bits)
xxd -r -p vkrs.sorted.hex vkrs.bin

# Remove temporary file
rm -f vkrs.sorted.hex


# ---

# Compress VCF file
bgzip ${VCF_OUTPUT_NAME}.vcf

# Index VCF file
vt index ${VCF_OUTPUT_NAME}.vcf.gz
