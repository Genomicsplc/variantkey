#!/usr/bin/env bash
#
# fastabin.sh
#
# Create a binary version of the input genome sequence FASTA file for quick lookup.
# It only extract sequences for chromosomes 1 to 22, X, Y and MT.
#
# BINARY FORMAT:
#
# The first line of the binary fasta file contains the index.
# The index is composed by 26 blocks of 32 bit numbers,
# one for each of the 25 chromosomes (1 to 22, X, Y and MT),
# plus one to indicate the end of the file.
# Each index number represents the file byte offset of the corresponding sequence.
# The index is followed by 25 lines, one for each chromosome sequence.
#
#
# Nicola Asuni
# ------------------------------------------------------------------------------
set -e -u -x -o pipefail

: ${REFERENCE_GENOME_FASTA_FILE:=hs38DH.fa}  # Input genome reference genome FASTA file
: ${FASTA_BINARY_FILE:=fasta.bin}            # Name of the output binary FASTA file

# add padding for the index section (26 x 4 bytes)
echo $(printf '0000%.0s' {1..26}) > fasta.linear

# linearize the fasta file : one line per sequence for the first 25 chromosomes
gawk '/^>/ {printf("%s",(N>0?"\n":""));N++;next;} {printf("%s",$0);} END {printf("\n");}' ${REFERENCE_GENOME_FASTA_FILE} | head -n 25 >> fasta.linear || true

# create the index in hexadecimal format
gawk '{printf "%08x\n", (total += length($0)+1);}' fasta.linear > fasta.index.hex

# convert HEX file in binary format
xxd -r -p fasta.index.hex > ${FASTA_BINARY_FILE}

# append newline
echo "" >> ${FASTA_BINARY_FILE}

# merge files
tail -n +2 fasta.linear >> ${FASTA_BINARY_FILE}

# remove temporary files
rm -f fasta.index.hex fasta.linear
