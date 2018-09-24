"""Vectorized VariantKey."""

import variantkey as pvk
import numpy as np


class VariantKey(object):
    """VariantKey numpy-vectorized functions."""

    genoref_mf = None
    genoref_size = 0
    nrvk_mf = None
    nrvk_mc = None
    nrvk_nrows = 0
    rsvk_mf = None
    rsvk_mc = None
    rsvk_nrows = 0
    vkrs_mf = None
    vkrs_mc = None
    vkrs_nrows = 0

    def __init__(self, genoref_file=None, nrvk_file=None, rsvk_file=None, vkrs_file=None):
        """Instantiate a new VariantKey object.
        Load the support files if specified.

        Parameters
        ----------
        genoref_file : string
            Name and path of the binary file containing the genome reference (fasta.bin).
            This file can be generated from a FASTA file using the resources/tools/fastabin.sh script.
        nrvk_file : string
            Name and path of the binary file containing the non-reversible-VariantKey mapping (nrvk.bin).
            This file can be generated from a normalized VCF file using the resources/tools/nrvk.sh script.
        rsvk_file : string
            Name and path of the binary file containing the rsID to VariantKey mapping (rsvk.bin).
            This file can be generated using the resources/tools/rsvk.sh script.
        vkrs_file : string
            Name and path of the binary file containing the VariantKey to rsID mapping (vkrs.bin).
            This file can be generated using the resources/tools/vkrs.sh script.
        """

        if genoref_file is not None:
            # Load the reference genome binary file.
            self.genoref_mf, self.genoref_size = pvk.mmap_genoref_file(genoref_file)
            if self.genoref_size <= 0:
                raise Exception('Unable to load the GENOREF file: {0}'.format(genoref_file))

        if nrvk_file is not None:
            # Load the lookup table for non-reversible variantkeys.
            self.nrvk_mf, self.nrvk_mc, self.nrvk_nrows = pvk.mmap_nrvk_file(nrvk_file)
            if self.nrvk_nrows <= 0:
                raise Exception('Unable to load the NRVK file: {0}'.format(nrvk_file))

        if rsvk_file is not None:
            # Load the lookup table for rsID to VariantKey.
            self.rsvk_mf, self.rsvk_mc, self.rsvk_nrows = pvk.mmap_rsvk_file(rsvk_file, [4, 8])
            if self.rsvk_nrows <= 0:
                raise Exception('Unable to load the RSVK file: {0}'.format(rsvk_file))

        if vkrs_file is not None:
            # Load the lookup table for VariantKey ro rsID
            self.vkrs_mf, self.vkrs_mc, self.vkrs_nrows = pvk.mmap_vkrs_file(vkrs_file, [8, 4])
            if self.vkrs_nrows <= 0:
                raise Exception('Unable to load the VKRS file: {0}'.format(vkrs_file))

    def __del__(self):
        """Unmap memory-mapped files"""
        if self.genoref_mf is not None:
            pvk.munmap_binfile(self.genoref_mf)
        if self.nrvk_mf is not None:
            pvk.munmap_binfile(self.nrvk_mf)
        if self.rsvk_mf is not None:
            pvk.munmap_binfile(self.rsvk_mf)
        if self.vkrs_mf is not None:
            pvk.munmap_binfile(self.vkrs_mf)

    # BASIC VARIANTKEY FUNCTIONS
    # --------------------------

    def encode_chrom(self, chrom):
        """Returns chromosome numerical encoding.

        Parameters
        ----------
        chrom : string
            Chromosome. An identifier from the reference genome, no white-space permitted.

        Returns
        -------
        uint8 :
            CHROM code
        """
        f = np.vectorize(pvk.encode_chrom, otypes=[np.uint8])
        return f(np.array(chrom).astype(np.string_))

    def decode_chrom(self, code):
        """Decode the chromosome numerical code.

        Parameters
        ----------
        code : uint8
            CHROM code.

        Returns
        -------
        '|S2' :
            Chromosome string
        """
        f = np.vectorize(pvk.decode_chrom, otypes=['|S2'])
        return f(np.array(code).astype(np.uint8))

    def encode_refalt(self, ref, alt):
        """Returns reference+alternate numerical encoding.

        Parameters
        ----------
        ref : string
            Reference allele.
            String containing a sequence of nucleotide letters.
            The value in the pos field refers to the position of the first nucleotide in the String.
            Characters must be A-Z, a-z or *
        alt : string
            Alternate non-reference allele string. Characters must be A-Z, a-z or *

        Returns
        -------
        uint32 :
            code
        """
        f = np.vectorize(pvk.encode_refalt, otypes=[np.uint32])
        return f(np.array(ref).astype(np.string_), np.array(alt).astype(np.string_))

    def decode_refalt(self, code):
        """Decode the 32 bit REF+ALT code if reversible
        (if it has 11 or less bases in total and only contains ACGT letters).

        Parameters
        ----------
        code : uint32
            REF+ALT code

        Returns
        -------
        tuple:
            - REF
            - ALT
            - REF length
            - ALT length
        """
        f = np.vectorize(pvk.decode_refalt, otypes=['|S256', '|S256', np.uint8, np.uint8])
        return f(np.array(code).astype(np.uint32))

    def encode_variantkey(self, chrom, pos, refalt):
        """Returns a 64 bit variant key based on the pre-encoded CHROM, POS (0-based) and REF+ALT.

        Parameters
        ----------
        chrom : uint8
            Encoded Chromosome (see encode_chrom).
        pos : uint32
            Position. The reference position, with the first base having position 0.
        refalt : uint32
            Encoded Reference + Alternate (see encode_refalt).

        Returns
        -------
        unit64:
            VariantKey 64 bit code.
        """
        f = np.vectorize(pvk.encode_variantkey, otypes=[np.uint64])
        return f(np.array(chrom).astype(np.uint8), np.array(pos).astype(np.uint32), np.array(refalt).astype(np.uint32))

    def extract_variantkey_chrom(self, vk):
        """Extract the CHROM code from VariantKey.

        Parameters
        ----------
        vk : uint64
            VariantKey code.

        Returns
        -------
        uint8 :
            CHROM code.
        """
        f = np.vectorize(pvk.extract_variantkey_chrom, otypes=[np.uint8])
        return f(np.array(vk).astype(np.uint64))

    def extract_variantkey_pos(self, vk):
        """Extract the POS code from VariantKey.

        Parameters
        ----------
        vk : uint64
            VariantKey code.

        Returns
        -------
        uint32 :
            Position.
        """
        f = np.vectorize(pvk.extract_variantkey_pos, otypes=[np.uint32])
        return f(np.array(vk).astype(np.uint64))

    def extract_variantkey_refalt(self, vk):
        """Extract the REF+ALT code from VariantKey.

        Parameters
        ----------
        vk : uint64
            VariantKey code.

        Returns
        -------
        uint32 :
            REF+ALT code.
        """
        f = np.vectorize(pvk.extract_variantkey_refalt, otypes=[np.uint32])
        return f(np.array(vk).astype(np.uint64))

    def decode_variantkey(self, vk):
        """Decode a VariantKey code and returns the components.

        Parameters
        ----------
        vk : uint64
            VariantKey code.

        Returns
        -------
        tuple :
            - CHROM code
            - POS
            - REF+ALT code
        """
        f = np.vectorize(pvk.decode_variantkey, otypes=[np.uint8, np.uint32, np.uint32])
        return f(np.array(vk).astype(np.uint64))

    def variantkey(self, chrom, pos, ref, alt):
        """Returns a 64 bit variant key based on CHROM, POS (0-based), REF, ALT.

        Parameters
        ----------
        chrom : string
            Chromosome. An identifier from the reference genome, no white-space or leading zeros permitted.
        pos : uint32
            Position. The reference position, with the first base having position 0.
        ref : string
            Reference allele. String containing a sequence of nucleotide letters.
            The value in the pos field refers to the position of the first nucleotide in the String.
            Characters must be A-Z, a-z or *
        alt : string
            Alternate non-reference allele string. Characters must be A-Z, a-z or *

        Returns
        -------
        uint64:
            VariantKey 64 bit code.
        """
        f = np.vectorize(pvk.variantkey, otypes=[np.uint64])
        return f(
            np.array(chrom).astype(np.string_),
            np.array(pos).astype(np.uint32),
            np.array(ref).astype(np.string_),
            np.array(alt).astype(np.string_))

    def variantkey_range(self, chrom, pos_min, pos_max):
        """Returns minimum and maximum VariantKeys for range searches.

        Parameters
        ----------
        chrom : uint8
            Chromosome encoded number.
        pos_min : uint32
            Start reference position, with the first base having position 0.
        pos_max : uint32
            End reference position, with the first base having position 0.

        Returns
        -------
        tuple : uint64
            - VariantKey min value
            - VariantKey max value
        """
        f = np.vectorize(pvk.variantkey_range, otypes=[np.uint32, np.uint32])
        return f(
            np.array(chrom).astype(np.uint8),
            np.array(pos_min).astype(np.uint32),
            np.array(pos_max).astype(np.uint32))

    def compare_variantkey_chrom(self, vka, vkb):
        """Compares two VariantKeys by chromosome only.

        Parameters
        ----------
        vka : uint64
            The first VariantKey to be compared.
        vkb : uint64
            The second VariantKey to be compared.

        Returns
        -------
        int_ :
            -1 if the first chromosome is smaller than the second,
            0 if they are equal and 1 if the first is greater than the second.
        0
        """
        f = np.vectorize(pvk.compare_variantkey_chrom, otypes=[np.int_])
        return f(np.array(vka).astype(np.uint64), np.array(vkb).astype(np.uint64))

    def compare_variantkey_chrom_pos(self, vka, vkb):
        """Compares two VariantKeys by chromosome and position.

        Parameters
        ----------
        vka : uint64
            The first VariantKey to be compared.
        vkb : uint64
            The second VariantKey to be compared.

        Returns
        -------
        int_ :
            -1 if the first CHROM+POS is smaller than the second,
            0 if they are equal and 1 if the first is greater than the second.
        """
        f = np.vectorize(pvk.compare_variantkey_chrom_pos, otypes=[np.int_])
        return f(np.array(vka).astype(np.uint64), np.array(vkb).astype(np.uint64))

    def variantkey_hex(self, vk):
        """Returns VariantKey hexadecimal string (16 characters).

        Parameters
        ----------
        vk : uint64
            VariantKey code.

        Returns
        -------
        '|S16':
            VariantKey hexadecimal string.
        """
        f = np.vectorize(pvk.variantkey_hex, otypes=['|S16'])
        return f(np.array(vk).astype(np.uint64))

    def parse_variantkey_hex(self, vs):
        """Parses a VariantKey hexadecimal string and returns the code.

        Parameters
        ----------
        vs : '|S16'
            VariantKey hexadecimal string (it must contain 16 hexadecimal characters).

        Returns
        -------
        uint64 :
            VariantKey 64 bit code.
        """
        f = np.vectorize(pvk.parse_variantkey_hex, otypes=[np.uint64])
        return f(np.array(vs).astype('|S16'))
