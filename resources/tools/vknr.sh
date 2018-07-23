#!/usr/bin/env bash
#
# vknr.sh
#
# Process the non-reversible variantKey TSV file
# to generate a binary lookup table for
# REF and ALT: vknr.bin
#
# Nicola Asuni
# ------------------------------------------------------------------------------

# NON-REVERSIBLE VARIANTKEY BINARY FILE

# INPUT TSV format
#    [16 BYTE VARIANTKEY HEX]\t[REF STRING]\t[ALT STRING]\n...

# OUTPUT binary format:
#    [8 BYTE VARIANTKEY][8 BYTE VALUE ADDRESS]...
#    [1 BYTE REF LENGTH][1 BYTE ALT LENGTH][REF STRING][ALT STRING]...
#    [4 BYTE NUM VARIANTS]

# File examples:
#     c/test/data/vknr.10.unsorted.tsv
#     c/test/data/vknr.10.hex
#     c/test/data/vknr.10.bin

# sort by VariantKey 
LC_ALL=C sort --parallel=${PARALLEL:=4} --output=vknr.sorted.tsv vknr.unsorted.tsv

cut -f1 vknr.sorted.tsv > vknr.vk.hex
gawk '{print 2+length($2)+length($3)}' vknr.sorted.tsv > vknr.pos.tsv

NVAR=$(($(stat -c%s vknr.vk.hex) / 17))
OFFSET=$((${NVAR} * 16))
OFFSETHEX=$(printf "%016x" ${OFFSET})

echo -e "${OFFSET}" > vknr.offset.tsv
head -n -1 vknr.pos.tsv >> vknr.offset.tsv
gawk '{printf "%016x\n", (total += $0)}' vknr.offset.tsv > vknr.pos.hex
rm -f vknr.pos.tsv vknr.offset.tsv

paste vknr.vk.hex vknr.pos.hex > vknr.hex
rm -f vknr.bin
xxd -r -p vknr.hex vknr.bin
rm -f vknr.pos.hex vknr.hex vknr.vk.hex

gawk '{printf("%c%c%s%s",length($2),length($3),$2,$3)}' vknr.sorted.tsv >> vknr.bin
rm -f vknr.sorted.tsv

# add number of variants at the end of the file (32 bit)
xxd -r -p <<< $(printf "%08x\n" ${NVAR}) >> vknr.bin
