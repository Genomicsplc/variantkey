#!/usr/bin/env bash
#
# vkhexbin.sh
#
# Process the variantKey HEX file to generate a binary lookup table for
# non-reversible VariantKey REF+ALT map: vknr.bin
#
# Nicola Asuni
# ------------------------------------------------------------------------------

# NON-REVERSIBLE VARIANTKEY BINARY FILE

# INPUT hexadecimal format
#    [16 BYTE VARIANTKEY][1 BYTE TAB][8 BYTE LENGTH of REF+ALT+2][1 BYTE TAB][2 BYTE REF LENGTH][1 BYTE TAB][2 BYTE ALT LENGTH][1 BYTE TAB][REF STRING HEX][1 BYTE TAB][ALT STRING HEX]...

# Intermediate hexadecimal format:
#    [16 BYTE VARIANTKEY][1 BYTE TAB][16 BYTE VALUE ADDRESS][1 BYTE NEWLINE]...
#    [2 BYTE REF LENGTH][1 BYTE TAB][2 BYTE ALT LENGTH][1 BYTE TAB][REF STRING HEX][1 BYTE TAB][ALT STRING HEX]...
#    [8 BYTE NUM VARIANTS]

# OUTPUT binary format:
#    [8 BYTE VARIANTKEY][8 BYTE VALUE ADDRESS]...
#    [1 BYTE REF LENGTH][1 BYTE ALT LENGTH][REF STRING][ALT STRING]...
#    [4 BYTE NUM VARIANTS]

# sort by VariantKey 
LC_ALL=C sort --parallel=${PARALLEL:=4} --output=vknr.sorted.hex vknr.unsorted.hex

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
gawk '{printf "%016x\n", (total += strtonum("0x" $0))}' vknr.offset.hex > vknr.pos.hex

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
