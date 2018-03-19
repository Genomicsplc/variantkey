#!/usr/bin/env bash
#
# vcfnorm.sh
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
: ${VCF_OUTPUT_NAME:?}                  # Name to be used for the output VCF file (e.g. dbsnp)
: ${REFERENCE_GENOME_FASTA_FILE:?}    # Genome reference genome FASTA file
: ${GENOME_ASSEMBLY:?}                # Genome Assembly code (e.g. GRCh37.p13.b150)
: ${PARALLEL:=4}                      # Number of parallel processes used by "sort"


if [ -x "$(command -v greadlink)" ]; then READLINK=greadlink; else READLINK=readlink; fi
SCRIPT_DIR=$(${READLINK} -f $(dirname "$0"))
VERSION=$(cat ${SCRIPT_DIR}/../../VERSION)
DATE=$(date -u --iso-8601=d)

# Remove "chr" from chromosome names in the FASTA file
cat ${REFERENCE_GENOME_FASTA_FILE} | sed 's/>chrM/>MT/g' | sed 's/>chr/>/g' > genome.fa

# Index the VCF file
vt index ${VCF_INPUT_FILE}

# Decompose multiallelic variants into biallelic variants
vt decompose ${VCF_INPUT_FILE} -o decomposed.vcf

# Normalize variants (https://academic.oup.com/bioinformatics/article/31/13/2202/196142)
vt normalize decomposed.vcf -m -r genome.fa -o normalized.vcf

# Remove temporary file
rm -f decomposed.vcf

# Replace the reference in the VCF file with the genome assembly string
sed -i "0,\|^##reference=.*$|{s|^##reference=.*$|##reference=${GENOME_ASSEMBLY}|}" normalized.vcf

# Add VariantKey fields in the VCF (this generates the final normalized VCF file)
bcftools +add-variantkey normalized.vcf > ${VCF_OUTPUT_NAME}.vcf

# Remove temporary file
rm -f normalized.vcf

# ---

# --- RSID->VARIANTKEY BINARY FILE ---

# Create a RSID to VariantKey HEX file
bcftools query -f '%RSID_HEX%VARIANTKEY_HEX\n' ${VCF_OUTPUT_NAME}.vcf > rsid_variantkey.hex

#  Sort the RSID to VariantKey HEX file
LC_ALL=C sort --parallel=${PARALLEL} --output=rsid_variantkey.sorted.hex rsid_variantkey.hex

# Remove temporary file
rm -f rsid_variantkey.hex

# Convert HEX file in binary format (the newlines are removed to get adjacent blocks of 160 bits)
xxd -r -p rsid_variantkey.sorted.hex rsid_variantkey.bin

# Remove temporary file
rm -f rsid_variantkey.sorted.hex

# ---


# --- VARIANTKEY->RSID BINARY FILE ---

# Create VariantKey to RSID HEX file
bcftools query -f '%VARIANTKEY_HEX%RSID_HEX\n' ${VCF_OUTPUT_NAME}.vcf > variantkey_rsid.hex

#  Sort the RSID to VariantKey HEX file
LC_ALL=C sort --parallel=${PARALLEL} --output=variantkey_rsid.sorted.hex variantkey_rsid.hex

# Remove temporary file
rm -f variantkey_rsid.hex

# Convert HEX file in binary format (the newlines are removed to get adjacent blocks of 160 bits)
xxd -r -p variantkey_rsid.sorted.hex variantkey_rsid.bin

# Remove temporary file
rm -f variantkey_rsid.sorted.hex

# ---


# Compress VCF file
bgzip ${VCF_OUTPUT_NAME}.vcf

# Index VCF file
vt index ${VCF_OUTPUT_NAME}.vcf.gz

# Create a JSON file with basic info
echo -e '{\n "description":"decomposed and normalized VCF files + RSID<->VariantKey binary files",\n "source_file":"'${VCF_INPUT_FILE}'",\n "reference_genome_fasta_file":"'${REFERENCE_GENOME_FASTA_FILE}'",\n "genome_assembly":"'${GENOME_ASSEMBLY}'",\n "date":"'${DATE}'"\n "version":"'${VERSION}'"\n}' > info.json
