#!/usr/bin/env bash
#
# rsidvar_dbsnp.sh
#
# Process the dbsnp file and generate index files
# to resolve RS ID to VarinatHash and vice-versa.
#
# REQUIRES the following linux tools:
# * bcftools with Genomics plc Varianthash module
# * GNU sort
# * xxd
# ------------------------------------------------------------------------------
set -e -u -x -o pipefail

SCRIPT_DIR=$(readlink -f $(dirname "$0"))  # directory where this script is
PROJECT_DIR=$(dirname "${SCRIPT_DIR}")     # this project root directory

: ${INPUT_FILE:?}
: ${OUTPUT_DIR:?}
: ${PARALLEL:=4}  # number of parallel processes for sorting

# index source file
bcftools index ${INPUT_FILE}

# split multialleic variants in dbSNP
bcftools norm --multiallelics -any --output ${OUTPUT_DIR}/dbSNP_split.vcf ${INPUT_FILE}

cd ${OUTPUT_DIR}

# create RSID to Variant map
bcftools query -f '%RSID_HEX%VARIANT_HASH_HEX\n' dbSNP_split.vcf > rsid_varhash.hex
# sort the map 
LC_ALL=C sort --parallel=${PARALLEL} --output=rsid_varhash.sorted.hex rsid_varhash.hex
# remove unsorted file
rm -f rsid_varhash.hex
# convert the map to binary format
xxd -r -p rsid_varhash.sorted.hex rsid_varhash.bin
# remove sorted text format
rm -f rsid_varhash.sorted.hex

# create Variant to RSID map
bcftools query -f '%VARIANT_HASH_HEX%RSID_HEX\n' dbSNP_split.vcf > varhash_rsid.hex
# sort the map
LC_ALL=C sort --parallel=${PARALLEL} --output=varhash_rsid.sorted.hex varhash_rsid.hex
# remove unsorted file
rm -f varhash_rsid.hex
# convert the map to binary format
xxd -r -p varhash_rsid.sorted.hex varhash_rsid.bin
# remove sorted text format
rm -f varhash_rsid.sorted.hex
