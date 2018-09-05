#!/usr/bin/env bash
#
# rsvk.sh
#
# Process the variantKey HEX file to generate the
# rsID to VariantKey binary lookup table: rsvk.bin
#
# Nicola Asuni
# ------------------------------------------------------------------------------

# RSID -> VARIANTKEY BINARY FILE

# INPUT hexadecimal format:
#    [8 BYTE RSID][1 BYTE TAB][16 BYTE VARIANTKEY][1 BYTE NEWLINE]...

# OUTPUT binary format:
#    BINSRC1 00
#    02 04 08 00 00 00 00 00
#    [8 BYTE NUMBER OF ROWS]
#    [8 BYTE COLUMN OFFSET]+
#    [4 BYTE RSID COLUMN]+
#    [8-BYTE ALIGMNET PADDING]+
#    [8 BYTE VARIANTKEY COLUMN]+

# File examples:
#    c/test/data/rsvk.10.bin
#    c/test/data/rsvk.10.hex
#    c/test/data/rsvk.sorted.10.hex
#    c/test/data/rsvk.unsorted.10.hex

: ${RSVK_INPUT_FILE:=rsvk.unsorted.hex}
: ${RSVK_OUTPUT_FILE:=rsvk.bin}

# sort by rsID
LC_ALL=C sort --parallel=${PARALLEL:=4} --output=rsvk.hex ${RSVK_INPUT_FILE}

# extract first column (KEYS)
cut -f1 rsvk.hex > rsvk.col.hex

# get the number of rows (keys)
NK=$(($(stat -c%s rsvk.col.hex) / 9))

# number of padding bytes for 8-byte alignment
PAD=$(((8 - ((${NK} << 2) & 7)) & 7))

# 0-padding
if [ ${PAD} -gt 1 ]; then
    printf "00%.0s" $(seq 1 ${PAD}) >> rsvk.col.hex
    echo '' >> rsvk.col.hex
fi

# extract and append second column (VALUES)
cut -f2 rsvk.hex >> rsvk.col.hex

# BINSRC1 header
echo "42494e5352433100" > rsvk.hex
# 2 columns
echo "02" >> rsvk.hex
# first column is 4 bytes (uint32)
echo "04" >> rsvk.hex
# second column is 8 bytes (uint64)
echo "08" >> rsvk.hex
# padding to 8 bytes
echo "0000000000" >> rsvk.hex

# number of rows
printf "%016x\n" ${NK} > rsvk.head.hex
# rs col offsets
echo "0000000000000028" >> rsvk.head.hex
# vk col offsets
printf "%016x\n" $(((${NK} * 4) + ${PAD} + 40)) >> rsvk.head.hex
# convert to Little-Endian
perl -nE 'say reverse /(..)/g' rsvk.head.hex >> rsvk.hex

# convert data to Little-Endian
perl -nE 'say reverse /(..)/g' rsvk.col.hex >> rsvk.hex

# convert HEX file in binary format
xxd -r -p rsvk.hex ${RSVK_OUTPUT_FILE}

# remove temporary files
rm -f  rsvk.head.hex rsvk.col.hex rsvk.hex
