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
#    [4 BYTE RSID][8 BYTE VARIANTKEY]...

# sort by RSID
LC_ALL=C sort --parallel=${PARALLEL:=4} --output=rsvk.hex rsvk.unsorted.hex

# remove temporary file
rm -f rsvk.unsorted.hex

# convert HEX file in binary format (newlines are removed to get adjacent blocks of 96 bits)
xxd -r -p rsvk.hex rsvk.bin

# remove temporary file
rm -f rsvk.hex
