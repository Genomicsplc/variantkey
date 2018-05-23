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
#    [8 BYTE VARIANTKEY][4 BYTE RSID]...

# sort by VariantKey 
LC_ALL=C sort --parallel=${PARALLEL:=4} --output=vkrs.hex vkrs.unsorted.hex

# remove temporary file
rm -f vkrs.unsorted.hex

# convert HEX file in binary format (newlines are removed to get adjacent blocks of 96 bits)
xxd -r -p vkrs.hex vkrs.bin

# remove temporary file
rm -f vkrs.hex
