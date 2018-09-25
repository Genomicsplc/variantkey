"""Vectorized VariantKey."""

import variantkey as pvk
import numpy as np


class VariantKey(object):
    """VariantKey numpy-vectorized functions."""

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

        self.genoref_mf = None
        self.genoref_size = 0
        self.nrvk_mf = None
        self.nrvk_mc = None
        self.nrvk_nrows = 0
        self.rsvk_mf = None
        self.rsvk_mc = None
        self.rsvk_nrows = 0
        self.vkrs_mf = None
        self.vkrs_mc = None
        self.vkrs_nrows = 0

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
        if pvk is None:
            return
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
        return f(np.array(chrom).astype(np.uint8),
                 np.array(pos).astype(np.uint32),
                 np.array(refalt).astype(np.uint32))

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
        f = np.vectorize(pvk.variantkey_range, otypes=[np.uint64, np.uint64])
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

    # RSIDVAR
    # -------

    def find_rv_variantkey_by_rsid(self, rsid):
        """Search for the specified rsID and returns the first occurrence of VariantKey in the RV file.

        Parameters
        ----------
        rsid : uint32
            rsID to search.

        Returns
        -------
        tuple :
            - VariantKey or 0 in case not found.
            - Item position in the file.
        """
        f = np.vectorize(pvk.find_rv_variantkey_by_rsid,
                         excluded=['mc', 'first', 'last'],
                         otypes=[np.uint64, np.uint64])
        return f(self.rsvk_mc,
                 0,
                 self.rsvk_nrows,
                 np.array(rsid).astype(np.uint32))

    def get_next_rv_variantkey_by_rsid(self, pos, rsid):
        """Get the next VariantKey for the specified rsID in the RV file."\
        " This function should be used after find_rv_variantkey_by_rsid."\
        " This function can be called in a loop to get all VariantKeys that are associated with the same rsID (if any).

        Parameters
        ----------
        pos : uint64
            Current item position.
        rsid : uint32
            rsID to search.

        Returns
        -------
        tuple :
            - VariantKey or 0 in case not found.
            - Item position in the file.
        """
        f = np.vectorize(pvk.get_next_rv_variantkey_by_rsid, excluded=['mc', 'last'], otypes=[np.uint64, np.uint64])
        return f(self.rsvk_mc,
                 np.array(pos).astype(np.uint64),
                 self.rsvk_nrows,
                 np.array(rsid).astype(np.uint32))

    def find_all_rv_variantkey_by_rsid(self, rsid):
        """Search for the specified rsID and returns all associated VariantKeys.

        Parameters
        ----------
        rsid : uint32
            rsID to search.

        Returns
        -------
        tuple : int
            - VariantKey(s).
        """
        vk = []
        rsid_arr = np.array(rsid).astype(np.uint32)
        for x in np.nditer(rsid_arr):
            vk = vk + pvk.find_all_rv_variantkey_by_rsid(self.rsvk_mc, 0, self.rsvk_nrows, x)
        return np.array(vk).astype(np.uint64)

    def find_vr_rsid_by_variantkey(self, vk):
        """Search for the specified VariantKey and returns the first occurrence of rsID in the VR file.

        Parameters
        ----------
        vk : uint64
            VariantKey.

        Returns
        -------
        tuple :
            - rsID or 0 in case not found.
            - Item position in the file.
        """
        f = np.vectorize(pvk.find_vr_rsid_by_variantkey,
                         excluded=['mc', 'first', 'last'],
                         otypes=[np.uint32, np.uint64])
        return f(self.vkrs_mc,
                 0,
                 self.vkrs_nrows,
                 np.array(vk).astype(np.uint64))

    def find_vr_chrompos_range(self, chrom, pos_min, pos_max):
        """Search for the specified CHROM-POS range and returns the first occurrence of rsID in the VR file.

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
        tuple :
            - rsID or 0 in case not found
            - Position of the first item.
            - Position of the last item.
        """
        f = np.vectorize(pvk.find_vr_chrompos_range,
                         excluded=['mc', 'first', 'last'],
                         otypes=[np.uint32, np.uint64, np.uint64])
        return f(self.vkrs_mc,
                 0,
                 self.vkrs_nrows,
                 np.array(chrom).astype(np.uint8),
                 np.array(pos_min).astype(np.uint32),
                 np.array(pos_max).astype(np.uint32))

    # NRVK
    # ----

    def find_ref_alt_by_variantkey(self, vk):
        """Retrieve the REF and ALT strings for the specified VariantKey.

        Parameters
        ----------
        vk : uint64
            VariantKey to search.

        Returns
        -------
        tuple :
            - REF string.
            - ALT string.
            - REF length.
            - ALT length.
            - REF+ALT length.
        """
        f = np.vectorize(
            pvk.find_ref_alt_by_variantkey,
            excluded=['mc'],
            otypes=['|S256', '|S256', np.uint8, np.uint8, np.uint16])
        return f(self.nrvk_mc, np.array(vk).astype(np.uint64))

    def reverse_variantkey(self, vk):
        """Reverse a VariantKey code and returns the normalized components.

        Parameters
        ----------
        vk : uint64
            VariantKey code.

        Returns
        -------
        tuple :
            - CHROM string.
            - POS.
            - REF string.
            - ALT string.
            - REF length.
            - ALT length.
            - REF+ALT length.
        """
        f = np.vectorize(pvk.reverse_variantkey,
                         excluded=['mc'],
                         otypes=['|S2', np.uint32, '|S256', '|S256', np.uint8, np.uint8, np.uint16])
        return f(self.nrvk_mc, np.array(vk).astype(np.uint64))

    def get_variantkey_ref_length(self, vk):
        """Retrieve the REF length for the specified VariantKey.

        Parameters
        ----------
        vk : uint64
            VariantKey

        Returns
        -------
        uint8 :
            REF length or 0 if the VariantKey is not reversible and not found.
        """
        f = np.vectorize(pvk.get_variantkey_ref_length, excluded=['mc'], otypes=[np.uint8])
        return f(self.nrvk_mc, np.array(vk).astype(np.uint64))

    def get_variantkey_endpos(self, vk):
        """Get the VariantKey end position (POS + REF length).

        Parameters
        ----------
        vk : uint64
            VariantKey.

        Returns
        -------
        uint32 :
            Variant end position.
        """
        f = np.vectorize(pvk.get_variantkey_endpos, excluded=['mc'], otypes=[np.uint32])
        return f(self.nrvk_mc, np.array(vk).astype(np.uint64))

    def get_variantkey_chrom_startpos(self, vk):
        """Get the CHROM + START POS encoding from VariantKey.

        Parameters
        ----------
        vk : uint64
            VariantKey.

        Returns
        -------
        uint64 :
            CHROM + START POS encoding.
        """
        f = np.vectorize(pvk.get_variantkey_chrom_startpos, otypes=[np.uint64])
        return f(np.array(vk).astype(np.uint64))

    def get_variantkey_chrom_endpos(self, vk):
        """Get the CHROM + END POS encoding from VariantKey.

        Parameters
        ----------
        vk : uint64
            VariantKey.

        Returns
        -------
        uint64 :
            CHROM + END POS encoding.
        """
        f = np.vectorize(pvk.get_variantkey_chrom_endpos, excluded=['mc'], otypes=[np.uint64])
        return f(self.nrvk_mc, np.array(vk).astype(np.uint64))

    def nrvk_bin_to_tsv(self, tsvfile):
        """Convert a vrnr.bin file to a simple TSV.
        For the reverse operation see the resources/tools/nrvk.sh script.

        Parameters
        ----------
        tsvfile : int
            Output file name.

        Returns
        -------
        uint64 :
            Number of bytes written or 0 in case of error.
        """
        return pvk.nrvk_bin_to_tsv(self.nrvk_mc, tsvfile)

    # GENOREF
    # ----------

    def get_genoref_seq(self, chrom, pos):
        """Returns the genome reference nucleotide at the specified chromosome and position.

        Parameters
        ----------
        chrom : uint8
            Encoded Chromosome number (see encode_chrom).
        pos : uint32
            Position. The reference position, with the first base having position 0.

        Returns
        -------
        string :
            Nucleotide letter or 0 (NULL char) in case of invalid position.
        """
        f = np.vectorize(pvk.get_genoref_seq, excluded=['mf'], otypes=['|S1'])
        return f(self.genoref_mf, np.array(chrom).astype(np.uint8), np.array(pos).astype(np.uint32))

    def check_reference(self, chrom, pos, ref):
        """Check if the reference allele matches the reference genome data.

        Parameters
        ----------
        chrom : uint8
            Encoded Chromosome number (see encode_chrom).
        pos : uint32
            Position. The reference position, with the first base having position 0.
        ref : string
            Reference allele. String containing a sequence of nucleotide letters.

        Returns
        -------
        int :
            positive number in case of success, negative in case of error:
            0 the reference allele match the reference genome;
            1 the reference allele is inconsistent with the genome reference
            (i.e. when contains nucleotide letters other than A, C, G and T);
               -1 the reference allele don't match the reference genome;
               -2 the reference allele is longer than the genome reference sequence.
        """
        f = np.vectorize(pvk.check_reference, excluded=['mf'], otypes=[np.int_])
        return f(self.genoref_mf,
                 np.array(chrom).astype(np.uint8),
                 np.array(pos).astype(np.uint32),
                 np.array(ref).astype(np.string_))

    def flip_allele(self, allele):
        """Flip the allele nucleotides (replaces each letter with its complement).
        The resulting string is always in uppercase.
        Supports extended nucleotide letters.

        Parameters
        ----------
        allele : string
            String containing a sequence of nucleotide letters.

        Returns
        -------
        string :
            Flipped allele.
        """
        f = np.vectorize(pvk.flip_allele, otypes=['|S256'])
        return f(allele)

    def normalize_variant(self, chrom, pos, ref, alt):
        """Normalize a variant."\
        " Flip alleles if required and apply the normalization algorithm described at:"\
        " https://genome.sph.umich.edu/wiki/Variant_Normalization

        Parameters
        ----------
        chrom : uint8
            Chromosome encoded number.
        pos : uint32
            Position. The reference position, with the first base having position 0.
        ref : string
            Reference allele. String containing a sequence of nucleotide letters.
        alt : string
            Alternate non-reference allele string.

        Returns
        -------
        tuple :
            - Bitmask number in case of success, negative number in case of error.
              When positive, each bit has a different meaning when set:
             - bit 0 : The reference allele is inconsistent with the genome reference
                       (i.e. when contains nucleotide letters other than A, C, G and T).
             - bit 1 : The alleles have been swapped.
             - bit 2 : The alleles nucleotides have been flipped
                       (each nucleotide have been replaced with its complement).
             - bit 3 : Alleles have been left extended.
             - bit 4 : Alleles have been right trimmed.
             - bit 5 : Alleles have been left trimmed.
            - POS.
            - REF string.
            - ALT string.
            - REF length.
            - ALT length.
        """
        f = np.vectorize(pvk.normalize_variant,
                         excluded=['mf'],
                         otypes=[np.int_, np.uint32, '|S256', '|S256', np.uint8, np.uint8])
        return f(self.genoref_mf,
                 np.array(chrom).astype(np.uint8),
                 np.array(pos).astype(np.uint32),
                 np.array(ref).astype(np.string_),
                 np.array(alt).astype(np.string_))

    # REGIONKEY
    # ---------

    def encode_region_strand(self, strand):
        """Encode the strand direction (-1 > 2, 0 > 0, +1 > 1).

        Parameters
        ----------
        strand : int16
            Strand direction (-1, 0, +1).

        Returns
        -------
        uint8 :
            Strand code.
        """
        f = np.vectorize(pvk.encode_region_strand, otypes=[np.uint8])
        return f(np.array(strand).astype(np.int16))

    def decode_region_strand(self, strand):
        """Decode the strand direction code (0 > 0, 1 > +1, 2 > -1).

        Parameters
        ----------
        strand : uint8
            Strand code.

        Returns
        -------
        int16 :
            Strand direction.
        """
        f = np.vectorize(pvk.decode_region_strand, otypes=[np.int16])
        return f(np.array(strand).astype(np.uint8))

    def encode_regionkey(self, chrom, startpos, endpos, strand):
        """Returns a 64 bit regionkey

        Parameters
        ----------
        chrom : uint8
            Encoded Chromosome (see encode_chrom).
        startpos : uint32
            Start position (zero based).
        endpos : uint32
            End position (startpos + region_length).
        strand : uint8
            Encoded Strand direction (-1 > 2, 0 > 0, +1 > 1)

        Returns
        -------
        uint64 :
            RegionKey 64 bit code.
        """
        f = np.vectorize(pvk.encode_regionkey, otypes=[np.uint64])
        return f(np.array(chrom).astype(np.uint8),
                 np.array(startpos).astype(np.uint32),
                 np.array(endpos).astype(np.uint32),
                 np.array(strand).astype(np.uint8))

    def extract_regionkey_chrom(self, rk):
        """Extract the CHROM code from RegionKey.

        Parameters
        ----------
        rk : uint64
            RegionKey code.

        Returns
        -------
        uint8 :
            CHROM code.
        """
        f = np.vectorize(pvk.extract_regionkey_chrom, otypes=[np.uint8])
        return f(np.array(rk).astype(np.uint64))

    def extract_regionkey_startpos(self, rk):
        """Extract the START POS code from RegionKey.

        Parameters
        ----------
        rk : uint64
            RegionKey code.

        Returns
        -------
        uint32 :
            START POS.
        """
        f = np.vectorize(pvk.extract_regionkey_startpos, otypes=[np.uint32])
        return f(np.array(rk).astype(np.uint64))

    def extract_regionkey_endpos(self, rk):
        """Extract the END POS code from RegionKey.

        Parameters
        ----------
        rk : uint64
            RegionKey code.

        Returns
        -------
        uint32 :
            END POS.
        """
        f = np.vectorize(pvk.extract_regionkey_endpos, otypes=[np.uint32])
        return f(np.array(rk).astype(np.uint64))

    def extract_regionkey_strand(self, rk):
        """Extract the STRAND from RegionKey.

        Parameters
        ----------
        rk : uint64
            RegionKey code.

        Returns
        -------
        uint8 :
            STRAND.
        """
        f = np.vectorize(pvk.extract_regionkey_strand, otypes=[np.uint8])
        return f(np.array(rk).astype(np.uint64))

    def decode_regionkey(self, rk):
        """Decode a RegionKey code and returns the components as regionkey_t structure.

        Parameters
        ----------
        rk : uint64
            RegionKey code.

        Returns
        -------
        tuple:
            - encoded chromosome
            - start position
            - end position
            - encoded strand
        """
        f = np.vectorize(pvk.decode_regionkey, otypes=[np.uint8, np.uint32, np.uint32, np.uint8])
        return f(np.array(rk).astype(np.uint64))

    def reverse_regionkey(self, rk):
        """Reverse a RegionKey code and returns the normalized components as regionkey_rev_t structure.

        Parameters
        ----------
        rk : uint64
            RegionKey code.

        Returns
        -------
        tuple:
            - chromosome
            - start position
            - end position
            - strand
        """
        f = np.vectorize(pvk.reverse_regionkey, otypes=['|S2', np.uint32, np.uint32, np.int16])
        return f(np.array(rk).astype(np.uint64))

    def regionkey(self, chrom, startpos, endpos, strand):
        """Returns a 64 bit regionkey based on CHROM, START POS (0-based), END POS and STRAND.

        Parameters
        ----------
        chrom : string
            Chromosome. An identifier from the reference genome, no white-space or leading zeros permitted.
        startpos : uint32
            Start position (zero based).
        endpos : uint32
            End position (startpos + region_length).
        strand : int16
            Strand direction (-1, 0, +1)

        Returns
        -------
        uint64 :
            RegionKey 64 bit code.
        """
        f = np.vectorize(pvk.regionkey, otypes=[np.uint64])
        return f(np.array(chrom).astype(np.string_),
                 np.array(startpos).astype(np.uint32),
                 np.array(endpos).astype(np.uint32),
                 np.array(strand).astype(np.int16))

    def regionkey_hex(self, rk):
        """Returns RegionKey hexadecimal string (16 characters).

        Parameters
        ----------
        rk : uint64
            RegionKey code.

        Returns
        -------
        '|S16' :
            RegionKey hexadecimal string.
        """
        f = np.vectorize(pvk.regionkey_hex, otypes=['|S16'])
        return f(np.array(rk).astype(np.uint64))

    def parse_regionkey_hex(self, rs):
        """Parses a RegionKey hexadecimal string and returns the code.

        Parameters
        ----------
        rs : string
            RegionKey hexadecimal string (it must contain 16 hexadecimal characters).

        Returns
        -------
        uint64 :
            A RegionKey code.
        """
        f = np.vectorize(pvk.parse_regionkey_hex, otypes=[np.uint64])
        return f(np.array(rs).astype(np.string_))

    def get_regionkey_chrom_startpos(self, rk):
        """Get the CHROM + START POS encoding from RegionKey.

        Parameters
        ----------
        rk : uint64
            RegionKey code.

        Returns
        -------
        uint64 :
            CHROM + START POS encoding.
        """
        f = np.vectorize(pvk.get_regionkey_chrom_startpos, otypes=[np.uint64])
        return f(np.array(rk).astype(np.uint64))

    def get_regionkey_chrom_endpos(self, rk):
        """Get the CHROM + END POS encoding from RegionKey.

        Parameters
        ----------
        rk : uint64
            RegionKey code.

        Returns
        -------
        uint64 :
            CHROM + END POS encoding.
        """
        f = np.vectorize(pvk.get_regionkey_chrom_endpos, otypes=[np.uint64])
        return f(np.array(rk).astype(np.uint64))

    def are_overlapping_regions(self, a_chrom, a_startpos, a_endpos, b_chrom, b_startpos, b_endpos):
        """Check if two regions are overlapping.

        Parameters
        ----------
        a_chrom : uint8
            Region A chromosome code.
        a_startpos : uint32
            Region A start position.
        a_endpos : uint32
            Region A end position (startpos + region length).
        b_chrom : uint8
            Region B chromosome code.
        b_startpos : uint32
            Region B start position.
        b_endpos : uint32
            Region B end position (startpos + region length).

        Returns
        -------
        uint8 :
            1 if the regions overlap, 0 otherwise.
        """
        f = np.vectorize(pvk.are_overlapping_regions, otypes=[np.uint8])
        return f(np.array(a_chrom).astype(np.uint8),
                 np.array(a_startpos).astype(np.uint32),
                 np.array(a_endpos).astype(np.uint32),
                 np.array(b_chrom).astype(np.uint8),
                 np.array(b_startpos).astype(np.uint32),
                 np.array(b_endpos).astype(np.uint32))

    def are_overlapping_region_regionkey(self, chrom, startpos, endpos, rk):
        """Check if a region and a regionkey are overlapping.

        Parameters
        ----------
        chrom : uint8
            Region A chromosome code.
        startpos : uint32
            Region A start position.
        endpos : uint32
            Region A end position (startpos + region length).
        rk : uint64
            RegionKey B.

        Returns
        -------
        uint8 :
            1 if the regions overlap, 0 otherwise.
        """
        f = np.vectorize(pvk.are_overlapping_region_regionkey, otypes=[np.uint8])
        return f(np.array(chrom).astype(np.uint8),
                 np.array(startpos).astype(np.uint32),
                 np.array(endpos).astype(np.uint32),
                 np.array(rk).astype(np.uint64))

    def are_overlapping_regionkeys(self, rka, rkb):
        """Check if two regionkeys are overlapping.

        Parameters
        ----------
        rka : uint64
            RegionKey A.
        rkb : uint64
            RegionKey B.

        Returns
        -------
        uint8 :
            1 if the regions overlap, 0 otherwise.
        """
        f = np.vectorize(pvk.are_overlapping_regionkeys, otypes=[np.uint8])
        return f(np.array(rka).astype(np.uint64), np.array(rkb).astype(np.uint64))

    def are_overlapping_variantkey_regionkey(self, vk, rk):
        """Check if variantkey and regionkey are overlapping.

        Parameters
        ----------
        vk : uint64
            VariantKey.
        rk : uint64
            RegionKey.

        Returns
        -------
        uint8 :
            1 if the regions overlap, 0 otherwise.
        """
        f = np.vectorize(pvk.are_overlapping_variantkey_regionkey, excluded=['mc'], otypes=[np.uint8])
        return f(self.nrvk_mc, np.array(vk).astype(np.uint64), np.array(rk).astype(np.uint64))

    def variantkey_to_regionkey(self, vk):
        """Get RegionKey from VariantKey.

        Parameters
        ----------
        vk : uint64
            VariantKey.

        Returns
        -------
        uint64 :
            A RegionKey code.
        """
        f = np.vectorize(pvk.variantkey_to_regionkey, excluded=['mc'], otypes=[np.uint64])
        return f(self.nrvk_mc, np.array(vk).astype(np.uint64))

    # ESID
    # ----

    def encode_string_id(self, strid, start):
        """Encode maximum 10 characters of a string into a 64 bit unsigned integer.
        This function can be used to convert generic string IDs to numeric IDs.
        Parameters
        ----------
        strid : string
            The string to encode. It must be maximum 10 characters long and support ASCII characters from '!' to 'z'.
        start : uint32
            First character to encode, starting from 0. To encode the last 10 characters, set this value at (size - 10).

        Returns
        -------
        uint64 :
            Encoded string ID.
        """
        f = np.vectorize(pvk.encode_string_id, otypes=[np.uint64])
        return f(np.array(strid).astype(np.string_), np.array(start).astype(np.uint32))

    def encode_string_num_id(self, strid, sep):
        """Encode a string composed by a character section followed by a separator
        character and a numerical section into a 64 bit unsigned integer. For example: ABCDE:0001234.
        Encodes up to 5 characters in uppercase, a number up to 2^27, and up to 7 zero padding digits.
        If the string is 10 character or less, then the encode_string_id() is used.
        Parameters
        ----------
        strid : string
            The string to encode. It must be maximum 10 characters long and support ASCII characters from '!' to 'z'.
        sep : char
            Separator character between string and number.

        Returns
        -------
        uint64 :
            Encoded string ID.
        """
        f = np.vectorize(pvk.encode_string_num_id, otypes=[np.uint64])
        return f(np.array(strid).astype(np.string_), np.array(sep).astype('|S1'))

    def decode_string_id(self, esid):
        """Decode the encoded string ID.
        This function is the reverse of encode_string_id.
        The string is always returned in uppercase mode.
        Parameters
        ----------
        esid : uint64
            Encoded string ID code.

        Returns
        -------
        tuple:
            - STRING
            - STRING length
        """
        f = np.vectorize(pvk.decode_string_id, otypes=['|S23', np.uint8])
        return f(np.array(esid).astype(np.uint64))

    def hash_string_id(self, strid):
        """Hash the input string into a 64 bit unsigned integer.
        This function can be used to convert long string IDs to numeric IDs.
        Parameters
        ----------
        strid : strint
            The string to encode.

        Returns
        -------
        uint64 :
            Hash string ID.
        """
        f = np.vectorize(pvk.hash_string_id, otypes=[np.uint64])
        return f(np.array(strid).astype(np.string_))
