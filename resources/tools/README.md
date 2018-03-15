# VCFNORM

Script and tools to normalize VCF files.

## Normalization process:

* Replace the Genome reference with the one manually set (e.g. GRCh37.p13.b146)

* [Decompose](https://genome.sph.umich.edu/wiki/Vt#Decompose) multiallelic variants in a VCF file. If the VCF file has genotype fields GT,PL, GL or DP, they are modified to reflect the change in alleles. All other genotype fields are removed.

* [Normalize](https://genome.sph.umich.edu/wiki/Vt#Normalization) variants as in [Tan et al. 2015](https://academic.oup.com/bioinformatics/article/31/13/2202/196142).

* Extend the VCF file to include [VariantHash](https://github.com/Genomicsplc/varianthash) fields.

* Compress and index the VCF file.

* Generate RSID to VariantHash binary files.
