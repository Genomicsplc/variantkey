"""Vectorized VariantKey."""

import variantkey as vk
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
            self.genoref_mf, self.genoref_size = vk.mmap_genoref_file(genoref_file)
            if self.genoref_size <= 0:
                raise Exception('Unable to load the GENOREF file: {0}'.format(genoref_file))

        if nrvk_file is not None:
            # Load the lookup table for non-reversible variantkeys.
            self.nrvk_mf, self.nrvk_mc, self.nrvk_nrows = vk.mmap_nrvk_file(nrvk_file)
            if self.nrvk_nrows <= 0:
                raise Exception('Unable to load the NRVK file: {0}'.format(nrvk_file))

        if rsvk_file is not None:
            # Load the lookup table for rsID to VariantKey.
            self.rsvk_mf, self.rsvk_mc, self.rsvk_nrows = vk.mmap_rsvk_file(rsvk_file, [4, 8])
            if self.rsvk_nrows <= 0:
                raise Exception('Unable to load the RSVK file: {0}'.format(rsvk_file))

        if vkrs_file is not None:
            # Load the lookup table for VariantKey ro rsID
            self.vkrs_mf, self.vkrs_mc, self.vkrs_nrows = vk.mmap_vkrs_file(vkrs_file, [8, 4])
            if self.vkrs_nrows <= 0:
                raise Exception('Unable to load the VKRS file: {0}'.format(vkrs_file))

    def __del__(self):
        """Unmap memory-mapped files"""
        if genoref_mf is not None:
            vk.munmap_binfile(genoref_mf)
        if nrvk_mf is not None:
            vk.munmap_binfile(nrvk_mf)
        if rsvk_mf is not None:
            vk.munmap_binfile(rsvk_mf)
        if vkrs_mf is not None:
            vk.munmap_binfile(vkrs_mf)

    # BASIC VARIANTKEY FUNCTIONS
    # --------------------------

    np_encode_chrom = np.vectorize(vk.encode_chrom, otypes=[np.uint8])
    np_decode_chrom = np.vectorize(vk.decode_chrom, otypes=['|S3'])
    np_encode_refalt = np.vectorize(vk.encode_refalt, otypes=[np.uint32])
    np_decode_refalt = np.vectorize(vk.decode_refalt, otypes=['|S256', '|S256', np.uint8, np.uint8])
    np_encode_variantkey = np.vectorize(vk.encode_variantkey, otypes=[np.uint64])
    np_extract_variantkey_chrom = np.vectorize(vk.extract_variantkey_chrom, otypes=[np.uint8])
    np_extract_variantkey_pos = np.vectorize(vk.extract_variantkey_pos, otypes=[np.uint32])
    np_extract_variantkey_refalt = np.vectorize(vk.extract_variantkey_refalt, otypes=[np.uint32])
    np_decode_variantkey = np.vectorize(vk.decode_variantkey, otypes=[np.uint8, np.uint32, np.uint32])
    np_variantkey = np.vectorize(vk.variantkey, otypes=[np.uint64])
    np_variantkey_range = np.vectorize(vk.variantkey_range, otypes=[np.uint32, np.uint32])
    np_compare_variantkey_chrom = np.vectorize(vk.compare_variantkey_chrom, otypes=[np.int_])
    np_compare_variantkey_chrom_pos = np.vectorize(vk.compare_variantkey_chrom_pos, otypes=[np.int_])
    np_variantkey_hex = np.vectorize(vk.variantkey_hex, otypes=['|S17'])
    np_parse_variantkey_hex = np.vectorize(vk.parse_variantkey_hex, otypes=[np.uint64])
