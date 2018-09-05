#!/usr/bin/env bash
#
# fastabin.sh
#
# Create a binary version of the input reference genome sequence FASTA file for quick lookup.
# It only extract the first 25 sequences for chromosomes 1 to 22, X, Y and MT.
#
# Nicola Asuni
# ------------------------------------------------------------------------------

# OUTPUT binary format:
#    BINSRC1 00
#    02 08 04 00 00 00 00 00
#    19
#    01010101010101 0101010101010101 0101 000000000000
#    1000000000000000
#    [8-BYTE COLUMN OFFSET]+
#    [DATA]

set -e -u -x -o pipefail

: ${REFERENCE_GENOME_FASTA_FILE:?}  # Input genome reference genome FASTA file
: ${FASTA_BINARY_FILE:=fasta.bin}   # Name of the output binary FASTA file

# linearize the fasta file : one line per sequence
gawk '/^>/ {printf("%s",(N>0?"\n":""));N++;next;} {printf("%s",$0);} END {printf("\n");}' ${REFERENCE_GENOME_FASTA_FILE} > fasta_full_linear.tmp

# add BINSRC1 header
echo "42494e5352433100" > fasta.header.hex
# 25 columns = 1 byte per column
echo "1901010101010101" >> fasta.header.hex
# all columns are 1 byte (uint8) + padding
echo "0101010101010101" >> fasta.header.hex
echo "0101010101010101" >> fasta.header.hex
echo "0101000000000000" >> fasta.header.hex
# number of rows
echo "0100000000000000" >> fasta.header.hex

# initialize file with padding for header ((6 + 25) * 8)
printf "00000000%.0s" $(seq 1 31) > fasta_linear.tmp
echo "" >> fasta_linear.tmp
# only keep the first 25 sequences
head --quiet --lines 25 fasta_full_linear.tmp >> fasta_linear.tmp

# create the index in hexadecimal format
gawk '{printf "%016x\n", (total += length($0));}' fasta_linear.tmp > fasta_index.tmp
# convert to Little-Endian
perl -nE 'say reverse /(..)/g' fasta_index.tmp > fasta_index.hex
# append offsets
tail -n 26 fasta_index.hex | head -n 25 >> fasta.header.hex

# convert HEX file in binary format
xxd -r -p fasta.header.hex > ${FASTA_BINARY_FILE}

# merge files
tail -n +2 fasta_linear.tmp | tr -d "\n\r" >> ${FASTA_BINARY_FILE}

# remove temporary file
rm -f fasta_index.tmp fasta_index.hex fasta_full_linear.tmp fasta_index.tmp fasta_linear.tmp
