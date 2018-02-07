#!/usr/bin/env bash
#
# rsidvar_dbsnp.sh
#
# Process the dbsnp file and generate index files
# to resolve RS ID to VarinatHash and vice-versa.
#
# REQUIRES the following linux tools:
# * bcftools with Genomics plc Varianthash module
# * vt (https://github.com/atks/vt)
# * GNU sort
# * xxd
# ------------------------------------------------------------------------------
set -e -u -x -o pipefail

SCRIPT_DIR=$(readlink -f $(dirname "$0"))  # directory where this script is
PROJECT_DIR=$(dirname "${SCRIPT_DIR}")     # this project root directory

: ${INPUT_FILE:?} # input dbSNP file
: ${REF_FASTA:?}  # reference genome FASTA file
: ${OUTPUT_DIR:?}
: ${PARALLEL:=4}  # number of parallel processes for sorting


vt index ${INPUT_FILE}

vt decompose ${INPUT_FILE} -o ${OUTPUT_DIR}/dbsnp.decomposed.vcf

cd ${OUTPUT_DIR}

vt normalize dbsnp.decomposed.vcf -r ${REF_FASTA} -o dbsnp_norm.vcf

rm -f dbsnp.decomposed.vcf

# create RSID to Variant map
bcftools query -f '%RSID_HEX%VARIANT_HASH_HEX\n' dbsnp_norm.vcf > rsid_varhash.hex
# sort the map 
LC_ALL=C sort --parallel=${PARALLEL} --output=rsid_varhash.sorted.hex rsid_varhash.hex
# remove unsorted file
rm -f rsid_varhash.hex
# convert the map to binary format
xxd -r -p rsid_varhash.sorted.hex rsid_varhash.bin
# remove sorted text format
rm -f rsid_varhash.sorted.hex

# create Variant to RSID map
bcftools query -f '%VARIANT_HASH_HEX%RSID_HEX\n' dbsnp_norm.vcf > varhash_rsid.hex
# sort the map
LC_ALL=C sort --parallel=${PARALLEL} --output=varhash_rsid.sorted.hex varhash_rsid.hex
# remove unsorted file
rm -f varhash_rsid.hex
# convert the map to binary format
xxd -r -p varhash_rsid.sorted.hex varhash_rsid.bin
# remove sorted text format
rm -f varhash_rsid.sorted.hex
