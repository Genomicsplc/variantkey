// Usage example for VariantKey
// https://github.com/genomicsplc/variantkey

#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../src/hex.h"
#include "../src/binsearch.h"
#include "../src/esid.h"
#include "../src/genoref.h"
#include "../src/nrvk.h"
#include "../src/regionkey.h"
#include "../src/rsidvar.h"
#include "../src/variantkey.h"

int main()
{
    // BASIC VARIANTKEY FUNCTIONS
    // --------------------------

    uint8_t ec = encode_chrom("X", 1);
    fprintf(stdout, "%" PRIu8 "\n", ec);
    // 23

    char chrom[3];
    size_t clen = decode_chrom(23, chrom);
    fprintf(stdout, "%s %lu\n", chrom, clen);
    // X 1

    uint32_t era = encode_refalt("AC", 2, "GT", 2);
    fprintf(stdout, "%" PRIu32 "\n", era);
    // 286097408

    char ref[12], alt[12];
    size_t sizeref, sizealt;
    size_t ldra = decode_refalt(286097408, ref, &sizeref, alt, &sizealt);
    fprintf(stdout, "%s %s %lu %lu %lu\n", ref, alt, sizeref, sizealt, ldra);
    // AC GT 2 2 4

    uint64_t vk = encode_variantkey(23, 12345, 286097408);
    fprintf(stdout, "%" PRIx64 "\n", vk);
    // b800181c910d8000

    ec = extract_variantkey_chrom(0xB800181C910D8000);
    fprintf(stdout, "%" PRIu8 "\n", ec);
    // 23

    uint32_t pos = extract_variantkey_pos(0xB800181C910D8000);
    fprintf(stdout, "%" PRIu32 "\n", pos);
    // 12345

    era = extract_variantkey_refalt(0xB800181C910D8000);
    fprintf(stdout, "%" PRIu32 "\n", era);
    // 286097408

    variantkey_t dvk = {0,0,0};
    decode_variantkey(0xB800181C910D8000, &dvk);
    fprintf(stdout, "%" PRIu8 " %" PRIu32 " %" PRIu32 "\n", dvk.chrom, dvk.pos, dvk.refalt);
    // 23 12345 286097408

    vk = variantkey("X", 1, 12345, "AC", 2, "GT", 2);
    fprintf(stdout, "%" PRIx64 "\n", vk);
    // b800181c910d8000

    vkrange_t vrng;
    variantkey_range(23, 1234, 5678, &vrng);
    fprintf(stdout, "%" PRIu64 " %" PRIu64 "\n", vrng.min, vrng.max);
    // 13258599952973561856 13258609498538377215

    int cmp = compare_variantkey_chrom(0xB800026900000000, 0xB8000B177FFFFFFF);
    fprintf(stdout, "%d\n", cmp);
    // 0

    cmp = compare_variantkey_chrom_pos(0xB800026900000000, 0xB8000B177FFFFFFF);
    fprintf(stdout, "%d\n", cmp);
    // -1

    char vs[17] = "";
    variantkey_hex(0xB800181C910D8000, vs);
    fprintf(stdout, "%s\n", vs);
    // b800181c910d8000

    vk = parse_variantkey_hex("b800181c910d8000");
    fprintf(stdout, "%" PRIx64 "\n", vk);
    // b800181c910d8000


    // ============================================================================


    // GENOREF FUNCTIONS
    // -----------------

    // Load the reference genome binary file.
    // The input reference binary files can be generated from a FASTA file using the resources/tools/fastabin.sh script.
    // This example uses the "c/test/data/genoref.bin".
    mmfile_t genoref = {0};
    mmap_genoref_file("genoref.bin", &genoref);
    if (genoref.fd < 0)
    {
        fprintf(stderr, "Error loading genoref.bin\n");
        return 1;
    }

    char sref = get_genoref_seq(genoref, 23, 0);
    fprintf(stdout, "%c\n", sref);
    // A

    int ret = check_reference(genoref, 23, 0, "A", 1);
    fprintf(stdout, "%d\n", ret);
    // 0

    char allele[] = "ATCGMKRYBVDHWSNatcgmkrybvdhwsn";
    flip_allele(allele, 30);
    fprintf(stdout, "%s\n", allele);
    // TAGCKMYRVBHDWSNTAGCKMYRVBHDWSN

    // Normalize a variant - this function should be used before generating a new VariantKey
    pos = 2;
    char nref[256] = "CDE";
    char nalt[256] = "CFE";
    sizeref = 3;
    sizealt = 3;
    ret = normalize_variant(genoref, 13, &pos, nref, &sizeref, nalt, &sizealt);
    fprintf(stdout, "%d %" PRIu32 " %s %s %lu %lu\n", ret, pos, nref, nalt, sizeref, sizealt);
    // 48 3 D F 1 1

    int err = munmap_binfile(genoref);
    if (err != 0)
    {
        fprintf(stderr, "Got %d error while unmapping the genoref file\n", err);
        return 1;
    }


    // ============================================================================


    // NRVK
    // ----

    // Load the lookup table for non-reversible variantkeys.
    // The input binary files can be generated from a normalized VCF file using the resources/tools/vkhexbin.sh script.
    // The VCF file can be normalized using the `resources/tools/vcfnorm.sh` script.
    // This example uses the "c/test/data/nrvk.10.bin".
    mmfile_t nrvk = {0};
    nrvk_cols_t nvc = {0};
    mmap_nrvk_file("nrvk.10.bin", &nrvk, &nvc);
    if (nrvk.fd < 0)
    {
        fprintf(stderr, "Error loading nrvk.10.bin\n");
        return 1;
    }

    char lref[256], lalt[256];
    size_t len = find_ref_alt_by_variantkey(nvc, 0x2000c3521f1c15ab, lref, &sizeref, lalt, &sizealt);
    fprintf(stdout, "%s %s %lu %lu %lu\n", lref, lalt, sizeref, sizealt, len);
    // ACGTACGT ACGT 8 4 12

    // Reverse all VariantKeys, including the ones that are not directly reversible by using a lookup table.
    variantkey_rev_t rev = {0};
    len = reverse_variantkey(nvc, 0x2000c3521f1c15ab, &rev);
    fprintf(stdout, "%s %" PRIu32 " %s %s %lu %lu %lu\n", rev.chrom, rev.pos, rev.ref, rev.alt, rev.sizeref, rev.sizealt, len);
    // 4 100004 ACGTACGT ACGT 8 4 12

    len = get_variantkey_ref_length(nvc, 0x2000c3521f1c15ab);
    fprintf(stdout, "%lu\n", len);
    // 8

    uint32_t endpos = get_variantkey_endpos(nvc, 0x2000c3521f1c15ab);
    fprintf(stdout, "%" PRIu32 "\n", endpos);
    // 100012

    uint64_t csp = get_variantkey_chrom_startpos(0x2000c3521f1c15ab);
    fprintf(stdout, "%016" PRIx64 "\n", csp);
    // 00000000400186a4

    uint64_t cep = get_variantkey_chrom_endpos(nvc, 0x2000c3521f1c15ab);
    fprintf(stdout, "%016" PRIx64 "\n", cep);
    // 00000000400186ac

    err = munmap_binfile(nrvk);
    if (err != 0)
    {
        fprintf(stderr, "Got %d error while unmapping the nrvk file\n", err);
        return 1;
    }


    // ============================================================================


    // RSIDVAR
    // -------

    // Load the lookup table for rsID to VariantKey.
    // The input binary file can be generated using the resources/tools/vkhexbin.sh script.
    // This example uses the "c/test/data/rsvk.10.bin".
    mmfile_t rv = {0};
    rsidvar_cols_t crv = {0};
    mmap_rsvk_file("rsvk.10.bin", &rv, &crv);
    if (rv.fd < 0)
    {
        fprintf(stderr, "Error loading rsvk.10.bin\n");
        return 1;
    }

    uint64_t first = 0;
    vk = find_rv_variantkey_by_rsid(crv, &first, crv.nrows, 0x00000061);
    fprintf(stdout, "%" PRIu64 " %" PRIu64 "\n", vk, first);
    // 9223656209074749440 3

    uint64_t fpos = 2;
    vk = get_next_rv_variantkey_by_rsid(crv, &fpos, 9, 0x00000061);
    fprintf(stdout, "%" PRIu64 " %" PRIu64 "\n", vk, fpos);
    // 9223656209074749440 3

    err = munmap_binfile(rv);
    if (err != 0)
    {
        fprintf(stderr, "Got %d error while unmapping the rv file\n", err);
        return 1;
    }

    // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

    // Load the lookup table for rsID to VariantKey.
    // The input binary file can be generated using the resources/tools/vkhexbin.sh script.
    // This example uses the "c/test/data/rsvk.m.10.bin".
    mmfile_t rvm = {0};
    rsidvar_cols_t crvm = {0};
    mmap_rsvk_file("rsvk.m.10.bin", &rvm, &crvm);
    if (rvm.fd < 0)
    {
        fprintf(stderr, "Error loading rsvk.m.10.bin\n");
        return 1;
    }

    first = 0;
    vk = find_rv_variantkey_by_rsid(crvm, &first, crvm.nrows, 0x00000003);
    while (vk > 0)
    {
        fprintf(stdout, "%" PRIu64 "\n", vk);
        vk = get_next_rv_variantkey_by_rsid(crvm, &first, 9, 0x00000003);
    }
    // 9223656209074749440
    // 9223656316446408704
    // 9223656367992733696

    err = munmap_binfile(rvm);
    if (err != 0)
    {
        fprintf(stderr, "Got %d error while unmapping the rvm file\n", err);
        return 1;
    }

    // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

    // Load the lookup table for VariantKey ro rsID
    // The input binary file can be generated using the resources/tools/vkhexbin.sh script.
    // This example uses the "c/test/data/vkrs.10.bin".
    mmfile_t vr = {0};
    rsidvar_cols_t cvr = {0};
    mmap_vkrs_file("vkrs.10.bin", &vr, &cvr);
    if (vr.fd < 0)
    {
        fprintf(stderr, "Error loading vkrs.10.bin\n");
        return 1;
    }

    first = 0;
    uint32_t rsid = find_vr_rsid_by_variantkey(cvr, &first, cvr.nrows, 0x80010274003A0000);
    fprintf(stdout, "%" PRIu32 " %" PRIu64 "\n", rsid, first);
    // 97 3

    first = 0;
    uint64_t last = 9;
    rsid = find_vr_chrompos_range(cvr, &first, &last, 0x14, 0x000256C5, 0x000256CB);
    fprintf(stdout, "%" PRIu32 "%" PRIu64 "%" PRIu64 "\n", rsid, first, last);
    // 9973 7 8

    err = munmap_binfile(vr);
    if (err != 0)
    {
        fprintf(stderr, "Got %d error while unmapping the vr file\n", err);
        return 1;
    }


    // ============================================================================


    // REGIONKEY
    // ---------

    uint8_t estrand = encode_region_strand(-1);
    fprintf(stdout, "%" PRIu8 "\n", estrand);
    // 2

    int8_t strand = decode_region_strand(2);
    fprintf(stdout, "%" PRIi8 "\n", strand);
    // -1

    uint64_t rk = encode_regionkey(25, 1000, 2000, 2);
    fprintf(stdout, "%016" PRIx64 "\n", rk);
    // c80001f400003e84

    ec = extract_regionkey_chrom(0xc80001f400003e84);
    fprintf(stdout, "%" PRIu8 "\n", ec);
    // 25

    pos = extract_regionkey_startpos(0xc80001f400003e84);
    fprintf(stdout, "%" PRIu32 "\n", pos);
    // 1000

    pos = extract_regionkey_endpos(0xc80001f400003e84);
    fprintf(stdout, "%" PRIu32 "\n", pos);
    // 2000

    estrand = extract_regionkey_strand(0xc80001f400003e84);
    fprintf(stdout, "%" PRIu8 "\n", estrand);
    // 2

    regionkey_t drk = {0,0,0,0};
    decode_regionkey(0xc80001f400003e84, &drk);
    fprintf(stdout, "%" PRIu8 " %" PRIu32 " %" PRIu32 " %" PRIu8 "\n", drk.chrom, drk.startpos, drk.endpos, drk.strand);
    // 25 1000 2000 2

    regionkey_rev_t rrk = {0};
    reverse_regionkey(0xc80001f400003e84, &rrk);
    fprintf(stdout, "%s %" PRIu32 " %" PRIu32 " %" PRIi8 "\n", rrk.chrom, rrk.startpos, rrk.endpos, rrk.strand);
    // MT 1000 2000 -1

    rk = regionkey("MT", 2, 1000, 2000, -1);
    fprintf(stdout, "%016" PRIx64 "\n", rk);
    // c80001f400003e84

    char rs[17] = "";
    regionkey_hex(0xc80001f400003e84, rs);
    fprintf(stdout, "%s\n", rs);
    // c80001f400003e84

    rk = parse_regionkey_hex("c80001f400003e84");
    fprintf(stdout, "%016" PRIx64 "\n", rk);
    // c80001f400003e84

    uint64_t cp = get_regionkey_chrom_startpos(0xc80001f400003e84);
    fprintf(stdout, "%016" PRIx64 "\n", cp);
    // 00000001900003e8

    cp = get_regionkey_chrom_endpos(0xc80001f400003e84);
    fprintf(stdout, "%016" PRIx64 "\n", cp);
    // 00000001900007d0

    uint8_t ov = are_overlapping_regions(5, 4, 6, 5, 3, 7);
    fprintf(stdout, "%" PRIu8 "\n", ov);
    // 1

    ov = are_overlapping_region_regionkey(5, 4, 6, 0x2800000180000038);
    fprintf(stdout, "%" PRIu8 "\n", ov);
    // 1

    ov = are_overlapping_regionkeys(0x2800000200000030, 0x2800000180000038);
    fprintf(stdout, "%" PRIu8 "\n", ov);
    // 1

    nrvk_cols_t tnvc = {0};
    ov = are_overlapping_variantkey_regionkey(tnvc, 0x2800000210920000, 0x2800000180000038);
    fprintf(stdout, "%" PRIu8 "\n", ov);
    // 1

    rk = variantkey_to_regionkey(tnvc, 0x2800000210920000);
    fprintf(stdout, "%016" PRIx64 "\n", rk);
    // 2800000200000030


    // ============================================================================


    // ESID
    // -------

    uint64_t esid = encode_string_id("A0A022YWF9", 10, 0);
    fprintf(stdout, "%016" PRIx64 "\n", esid);
    // a850850492e77999

    char strid[30];
    size_t stridlen = decode_string_id(0xa850850492e77999, strid);
    fprintf(stdout, "%s %lu\n", strid, stridlen);
    // A0A022YWF9 10

    esid = encode_string_num_id("ABC:0000123456", 14, ':');
    fprintf(stdout, "%016" PRIx64 "\n", esid);
    //

    stridlen = decode_string_id(esid, strid);
    fprintf(stdout, "%s %lu\n", strid, stridlen);
    //

    esid = hash_string_id("0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ", 36);
    fprintf(stdout, "%016" PRIx64 "\n", esid);
    // b3a5fdb8808cb7dc

    // ============================================================================

    return 0;
}
