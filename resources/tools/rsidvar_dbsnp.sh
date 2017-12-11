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

# split multialleic variants in dbSNP
bcftools norm --multiallelics -any --output ${OUTPUT_DIR}/dbSNP_split.vcf ${INPUT_FILE}

cd ${OUTPUT_DIR}

# create RSID to Variant map
bcftools query -f '%RSID_HEX%VARIANT_HASH_HEX\n' dbSNP_split.vcf > rsid_varhash.txt
# sort the map 
LC_ALL=C sort --parallel=${PARALLEL} --output=rsid_varhash.sorted.txt rsid_varhash.txt
# remove unsorted file
rm -f rsid_varhash.txt
# convert the map to binary format
xxd -r -p rsid_varhash.sorted.txt rsid_varhash.bin
# remove sorted text format
rm -f rsid_varhash.sorted.txt

# create Variant to RSID map
bcftools query -f '%VARIANT_HASH_HEX%RSID_HEX\n' dbSNP_split.vcf > varhash_rsid.txt
# sort the map
LC_ALL=C sort --parallel=${PARALLEL} --output=varhash_rsid.sorted.txt varhash_rsid.txt
# remove unsorted file
rm -f varhash_rsid.txt
# convert the map to binary format
xxd -r -p varhash_rsid.sorted.txt varhash_rsid.bin
# remove sorted text format
rm -f varhash_rsid.sorted.txt
