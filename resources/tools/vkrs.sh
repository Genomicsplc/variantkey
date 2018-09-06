#!/usr/bin/env bash
#
# vkrs.sh
#
# Process the variantKey HEX file to generate the
# VariantKey to rsID binary lookup table: vkrs.bin
#
# Nicola Asuni
# ------------------------------------------------------------------------------

# VARIANTKEY -> RSID BINARY FILE

# INPUT hexadecimal format:
#    [16 BYTE VARIANTKEY][1 BYTE TAB][8 BYTE RSID][1 BYTE NEWLINE]...

# OUTPUT binary format:
#    BINSRC1 00
#    02 08 04 00 00 00 00 00
#    [8 BYTE NUMBER OF ROWS]
#    [8 BYTE COLUMN OFFSET]+
#    [8 BYTE VARIANTKEY COLUMN]+
#    [4 BYTE RSID COLUMN]+
#    [8-BYTE ALIGMNET PADDING]+

# File examples:
#    c/test/data/vkrs.10.bin
#    c/test/data/vkrs.10.hex
#    c/test/data/vkrs.sorted.10.hex
#    c/test/data/vkrs.unsorted.10.hex

set -e -u -x -o pipefail -o errtrace

: ${VKRS_INPUT_FILE:=vkrs.unsorted.hex}
: ${VKRS_OUTPUT_FILE:=vkrs.bin}
: ${PARALLEL:=4}

# sort by VariantKey 
LC_ALL=C sort --parallel=${PARALLEL:=4} --output=vkrs.hex ${VKRS_INPUT_FILE}

# extract first column (KEYS)
cut -f1 vkrs.hex > vkrs.col.hex

# get the number of rows (keys)
NK=$(($(stat -c%s vkrs.col.hex) / 17))

# extract and append second column (VALUES)
cut -f2 vkrs.hex >> vkrs.col.hex

# number of padding bytes for 8-byte alignment
PAD=$(((8 - ((${NK} << 2) & 7)) & 7))

# 0-padding
if [ ${PAD} -gt 1 ]; then
    printf "00%.0s" $(seq 1 ${PAD}) >> vkrs.col.hex
    echo '' >> vkrs.col.hex
fi

# BINSRC1 header
echo "42494e5352433100" > vkrs.hex
# 2 columns
echo "02" >> vkrs.hex
# first column is 8 bytes (uint64)
echo "08" >> vkrs.hex
# second column is 4 bytes (uint32)
echo "04" >> vkrs.hex
# padding to 8 bytes
echo "0000000000" >> vkrs.hex

# number of rows
printf "%016x\n" ${NK} > vkrs.head.hex
# vk col offsets
echo "0000000000000028" >> vkrs.head.hex
# rs col offsets
printf "%016x\n" $(((${NK} * 8) + 40)) >> vkrs.head.hex
# convert to Little-Endian
perl -nE 'say reverse /(..)/g' vkrs.head.hex >> vkrs.hex
rm -f  vkrs.head.hex

# convert data to Little-Endian
perl -nE 'say reverse /(..)/g' vkrs.col.hex >> vkrs.hex
rm -f  vkrs.col.hex

# convert HEX file in binary format
xxd -r -p vkrs.hex ${VKRS_OUTPUT_FILE}
rm -f  vkrs.hex
