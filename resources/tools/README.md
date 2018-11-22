# Tools

Scripts to normalize VCF files and generate VariantKey information.


* **fastabin.sh**
  * Create a binary version of the input reference genome sequence FASTA file for quick lookup.
    It only extract the first 25 sequences for chromosomes 1 to 22, X, Y and MT.

* **vcfnorm.sh**
  * Normalize VCF files (allele decomposition + normalization)
  * *Requires*:
    * vt (https://github.com/atks/vt)
    * tabix

* **vkhexbin.sh**
  * Process the variantKey HEX file to generate the final binary counterparts:
    * <FILE>.vcf.gz     : decomposed and normalized VCF file with added VariantKey.
    * <FILE>.vcf.gz.tbi : VCF file index.
    * vkrs.bin          : VariantKey to rsID binary lookup table.
    * rsvk.bin          : rsID to VariantKey binary lookup table.
    * nrvk.bin          : Non-reversible VariantKey to REF+ALT lookup table.
  * *Requires*:
    * vt      (https://github.com/atks/vt)
    * bcftool (https://github.com/samtools/bcftools/tree/develop)
    * sort    (coreutils)
    * xxd     (vim-common)

## NOTE:

Prebuilt binary files can be downloaded from:
https://sourceforge.net/projects/variantkey/files/
