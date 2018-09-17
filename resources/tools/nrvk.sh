#!/usr/bin/env bash
#
# nrvk.sh
#
# Process the non-reversible variantKey TSV file
# to generate a binary lookup table for
# REF and ALT: nrvk.bin
#
# Nicola Asuni
# ------------------------------------------------------------------------------

# NON-REVERSIBLE VARIANTKEY BINARY FILE

# INPUT TSV format
#    [16 BYTE VARIANTKEY HEX]\t[REF STRING]\t[ALT STRING]\n...

# OUTPUT binary format:
#    BINSRC1 00
#    03 08 08 01 00 00 00 00
#    [8 BYTE NUMBER OF ROWS]
#    [8 BYTE COLUMN OFFSET]+
#    [8 BYTE VARIANTKEY COLUMN]+
#    [8 BYTE VALUE ADDRESS]+
#    [1 BYTE REF LENGTH][1 BYTE ALT LENGTH][REF STRING][ALT STRING]+

# File examples:
#     c/test/data/nrvk.10.unsorted.tsv
#     c/test/data/nrvk.10.bin

set -e -u -x -o pipefail -o errtrace

: ${NRVK_INPUT_FILE:=nrvk.unsorted.tsv}
: ${NRVK_OUTPUT_FILE:=nrvk.bin}
: ${PARALLEL:=4}

# sort by VariantKey 
LC_ALL=C sort --parallel=${PARALLEL:=4} --output=nrvk.sorted.tsv ${NRVK_INPUT_FILE}

cut -f1 nrvk.sorted.tsv > nrvk.vk.hex

LANG=ASCII gawk '{print 2+length($2)+length($3)}' nrvk.sorted.tsv > nrvk.pos.tsv

# BINSRC1 header
echo "42494e5352433100" > nrvk.hex
# 3 columns
echo "0308080100000000" >> nrvk.hex

# number of rows
NK=$(($(stat -c%s nrvk.vk.hex) / 17))
printf "%016x\n" ${NK} > nrvk.head.hex
# vk col offsets
echo "0000000000000030" >> nrvk.head.hex
# offset of offset column
printf "%016x\n" $(((${NK} * 8) + 48)) >> nrvk.head.hex
# offset of data column
OFFSET=$(((${NK} * 16) + 48))
printf "%016x\n" ${OFFSET} >> nrvk.head.hex
# convert to Little-Endian
perl -nE 'say reverse /(..)/g' nrvk.head.hex >> nrvk.hex
rm -f nrvk.head.hex

# VK column
perl -nE 'say reverse /(..)/g' nrvk.vk.hex >> nrvk.hex
rm -f nrvk.vk.hex

# offsets column
echo -e "0" > nrvk.offset.tsv
head -n -1 nrvk.pos.tsv >> nrvk.offset.tsv
rm -f nrvk.pos.tsv
LANG=ASCII gawk '{printf "%016x\n", (total += $0)}' nrvk.offset.tsv > nrvk.pos.hex
rm -f nrvk.offset.tsv
perl -nE 'say reverse /(..)/g' nrvk.pos.hex >> nrvk.hex
rm -f nrvk.pos.hex

# convert to binary
xxd -r -p nrvk.hex ${NRVK_OUTPUT_FILE}
rm -f nrvk.hex

# data
LANG=ASCII gawk '{printf("%c%c%s%s",length($2),length($3),$2,$3)}' nrvk.sorted.tsv >> ${NRVK_OUTPUT_FILE}
rm -f nrvk.sorted.tsv
