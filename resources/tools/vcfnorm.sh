#!/usr/bin/env bash
#
# vcfnorm.sh
#
# Normalize VCF files (allele decomposition + normalization)
#
# Requires:
#  - vt
#  - tabix
#
#  On Debian/Ubuntu:
#  sudo apt-get update && sudo apt-get install -y --allow-unauthenticated vt tabix coreutils vim-common
#
# Nicola Asuni
# ------------------------------------------------------------------------------
set -e -u -x -o pipefail

: ${VCF_INPUT_FILE:?}               # Input VCF file
: ${VCF_OUTPUT_NAME:?}              # Name to be used for the output VCF file (e.g. dbsnp)
: ${REFERENCE_GENOME_FASTA_FILE:?}  # Genome reference genome FASTA file


if [ -x "$(command -v greadlink)" ]; then READLINK=greadlink; else READLINK=readlink; fi
SCRIPT_DIR=$(${READLINK} -f $(dirname "$0"))
VERSION=$(cat ${SCRIPT_DIR}/../../VERSION)
DATE=$(date -u --iso-8601=d)

# Remove "chr" from chromosome names in the FASTA file
cat "${REFERENCE_GENOME_FASTA_FILE}" | sed 's/>chrM/>MT/g' | sed 's/>chr/>/g' > genome.fa

# Index the VCF file
vt index "${VCF_INPUT_FILE}"

# Decompose multiallelic variants into biallelic variants
vt decompose -s "${VCF_INPUT_FILE}" -o decomposed.vcf

# Normalize variants (https://academic.oup.com/bioinformatics/article/31/13/2202/196142)
vt normalize decomposed.vcf -m -r genome.fa -o "${VCF_OUTPUT_NAME}.vcf"

# Remove temporary file
rm -f decomposed.vcf

# Compress VCF file
bgzip "${VCF_OUTPUT_NAME}.vcf"

# Index VCF file
vt index "${VCF_OUTPUT_NAME}.vcf.gz"
