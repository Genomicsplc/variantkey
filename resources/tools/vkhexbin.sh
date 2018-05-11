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


# generate VariantKey hex files
bcftools +variantkey-hex ${VCF_INPUT_FILE}

# --- VARIANTKEY -> RSID BINARY FILE ----------------------------------------------------------------------------------
# Hexadecimal format : [16 BYTE VARIANTKEY][1 BYTE TAB][8 BYTE RSID][1 BYTE NEWLINE]...
# Binary format      : [8 BYTE VARIANTKEY][4 BYTE RSID]...

# sort by VariantKey 
LC_ALL=C sort --parallel=${PARALLEL} --output=vkrs.hex vkrs.unsorted.hex

# remove temporary file
rm -f vkrs.unsorted.hex

# convert HEX file in binary format (the newlines are removed to get adjacent blocks of 160 bits)
xxd -r -p vkrs.hex vkrs.bin

# remove temporary file
rm -f vkrs.hex


# --- RSID -> VARIANTKEY BINARY FILE ----------------------------------------------------------------------------------
# Hexadecimal format : [8 BYTE RSID][1 BYTE TAB][16 BYTE VARIANTKEY][1 BYTE NEWLINE]
# Binary format      : [4 BYTE RSID][8 BYTE VARIANTKEY]

# sort by RSID
LC_ALL=C sort --parallel=${PARALLEL} --output=rsvk.hex rsvk.unsorted.hex

# remove temporary file
rm -f rsvk.unsorted.hex

# convert HEX file in binary format (the newlines are removed to get adjacent blocks of 160 bits)
xxd -r -p rsvk.hex rsvk.bin

# remove temporary file
rm -f rsvk.hex


# --- NON-REVERSIBLE VARIANTKEY BINARY FILE ---------------------------------------------------------------------------
# Hexadecimal format : [16 BYTE VARIANTKEY][1 BYTE TAB][16 BYTE VALUE ADDRESS][1 BYTE NEWLINE]...
#                      [2 BYTE REF LENGTH][1 BYTE TAB][2 BYTE ALT LENGTH][1 BYTE TAB][REF STRING HEX][1 BYTE TAB][ALT STRING HEX]...
#                      [8 BYTE NUM VARIANTS]
# Binary format      : [8 BYTE VARIANTKEY][8 BYTE VALUE ADDRESS]...
#                      [1 BYTE REF LENGTH][1 BYTE ALT LENGTH][REF STRING][ALT STRING]...
#                      [4 BYTE NUM VARIANTS]

# sort by VariantKey 
LC_ALL=C sort --parallel=${PARALLEL} --output=vknr.sorted.hex vknr.unsorted.hex

# extract variantkey column
cut -f1 vknr.sorted.hex > vknr.vk.hex

# extract positions column
cut -f2 vknr.sorted.hex > vknr.pos.hex

# get total number of variants
NVAR=$(($(stat -c%s vknr.vk.hex) / 17))

# address of the first value
OFFSET=$((${NVAR} * 16))
OFFSETHEX=$(printf "%016x" ${OFFSET})

# build offset file
echo -e "${OFFSETHEX}" > vknr.offset.hex
head -n -1 vknr.pos.hex >> vknr.offset.hex 

# calculate absolute offsets
awk '{printf "%016x\n", (total += strtonum("0x" $0))}' vknr.offset.hex > vknr.pos.hex

# remove temporary file
rm -f vknr.offset.hex

# put file together
paste vknr.vk.hex vknr.pos.hex > vknr.hex

# remove temporary file
rm -f vknr.vk.hex vknr.pos.hex

# append values
cut -f3,4,5,6 vknr.sorted.hex >> vknr.hex

# remove temporary file
rm -f vknr.sorted.hex

# add number of variants at the end of the file (32 bit)
printf "%08x\n" ${NVAR} >> vknr.hex

# convert HEX file in binary format
xxd -r -p vknr.hex vknr.bin

# remove temporary file
rm -f vknr.hex


# --- ADD VARIANTKEY TO THE VCF FILE ----------------------------------------------------------------------------------

# Add VariantKey fields in the VCF
bcftools +add-variantkey ${VCF_INPUT_FILE} > ${VCF_OUTPUT_NAME}.vcf

# Compress VCF file
bgzip ${VCF_OUTPUT_NAME}.vcf

# Index VCF file
vt index ${VCF_OUTPUT_NAME}.vcf.gz
