#!/usr/bin/env bash
#
# fastabin.sh
#
# Create a binary version of the input reference genome sequence FASTA file for quick lookup.
# It only extract the first 25 sequences for chromosomes 1 to 22, X, Y and MT.
#
# OUTPUT BINARY FORMAT:
#
# The first line of the binary fasta file contains the index composed by 26 blocks
# of 32 bit numbers, one for each of the 25 chromosomes (1 to 22, X, Y and MT),
# plus one to indicate the end of the file.
# Each index number represents the file byte offset of the corresponding chromosome sequence.
# The index is followed by 25 lines, one for each chromosome sequence.
#
#
# Nicola Asuni
# ------------------------------------------------------------------------------
set -e -u -x -o pipefail

: ${REFERENCE_GENOME_FASTA_FILE:?}  # Input genome reference genome FASTA file
: ${FASTA_BINARY_FILE:=fasta.bin}   # Name of the output binary FASTA file

# linearize the fasta file : one line per sequence
gawk '/^>/ {printf("%s",(N>0?"\n":""));N++;next;} {printf("%s",$0);} END {printf("\n");}' ${REFERENCE_GENOME_FASTA_FILE} > fasta_full_linear.tmp

# initialize file with padding for the index section (26 x 4 bytes)
echo $(printf '0000%.0s' {1..26}) > fasta_linear.tmp

# only keep the first 25 sequences
head --quiet --lines 25 fasta_full_linear.tmp >> fasta_linear.tmp

# remove temporary file
rm -f fasta_full_linear.tmp

# create the index in hexadecimal format
gawk '{printf "%08x\n", (total += length($0)+1);}' fasta_linear.tmp > fasta_index.tmp

# convert HEX file in binary format
xxd -r -p fasta_index.tmp > ${FASTA_BINARY_FILE}

# remove temporary file
rm -f fasta_index.tmp

# append newline
echo "" >> ${FASTA_BINARY_FILE}

# merge files
tail -n +2 fasta_linear.tmp >> ${FASTA_BINARY_FILE}

# remove temporary file
rm -f fasta_linear.tmp
