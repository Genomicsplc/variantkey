package main

// Usage example for VariantKey GO wrapper
// https://github.com/genomicsplc/variantkey

import (
	"fmt"
	"os"

	vk "github.com/genomicsplc/variantkey/go/src"
)

func main() {

	// BASIC VARIANTKEY FUNCTIONS
	// --------------------------

	fmt.Println(vk.EncodeChrom("X"))
	// 23

	fmt.Println(vk.DecodeChrom(23))
	// X

	fmt.Println(vk.EncodeRefAlt("AC", "GT"))
	// 286097408

	fmt.Println(vk.DecodeRefAlt(286097408))
	// AC GT 2 2 4

	fmt.Println(vk.EncodeVariantKey(23, 12345, 286097408))
	// 13258623813950472192

	fmt.Println(vk.ExtractVariantKeyChrom(13258623813950472192))
	// 23

	fmt.Println(vk.ExtractVariantKeyPos(13258623813950472192))
	// 12345

	fmt.Println(vk.ExtractVariantKeyRefAlt(13258623813950472192))
	// 286097408

	fmt.Println(vk.DecodeVariantKey(13258623813950472192))
	// {23, 12345, 286097408}

	fmt.Println(vk.VariantKey("X", 12345, "AC", "GT"))
	// 13258623813950472192

	fmt.Println(vk.Range(23, 1234, 5678))
	// {13258599952973561856, 13258609498538377215}

	fmt.Println(vk.CompareVariantKeyChrom(13258599952973561856, 13258609498538377215))
	// 0

	fmt.Println(vk.CompareVariantKeyChromPos(13258599952973561856, 13258609498538377215))
	// -1

	fmt.Println(vk.Hex(13258623813950472192))
	// b800181c910d8000

	fmt.Println(vk.ParseHex("b800181c910d8000"))
	// 13258623813950472192

	// /\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\

	// GENOREF FUNCTIONS
	// -----------------

	// Load the reference genome binary file.
	// The input reference binary files can be generated from a FASTA file using the resources/tools/fastabin.sh script.
	// This example uses the "c/test/data/genoref.bin".
	gref, err := vk.MmapBinFile("../../c/test/data/genoref.bin")
	if err != nil {
		fmt.Println(err)
		os.Exit(1)
	}
	defer gref.Close()
	gref.LoadGenorefIndex()

	fmt.Println(gref.GetGenorefSeq(23, 0))
	// 65

	fmt.Println(gref.CheckReference(23, 0, "A"))
	// 0

	fmt.Println(vk.FlipAllele("ATCGMKRYBVDHWSNatcgmkrybvdhwsn"))
	// TAGCKMYRVBHDWSNTAGCKMYRVBHDWSN

	// Normalize a variant - this function should be used before generating a new VariantKey
	fmt.Println(gref.NormalizeVariant(13, 2, "CDE", "CFE"))
	// 48 3 D F 1 1

	// /\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\

	// NRVK
	// ----

	// Load the lookup table for non-reversible variantkeys.
	// The input binary files can be generated from a normalized VCF file using the resources/tools/vkhexbin.sh script.
	// The VCF file can be normalized using the `resources/tools/vcfnorm.sh` script.
	// This example uses the "c/test/data/vknr.10.bin".
	vknr, err := vk.MmapBinFile("../../c/test/data/vknr.10.bin")
	if err != nil {
		fmt.Println(err)
		os.Exit(2)
	}
	defer vknr.Close()

	fmt.Println(vknr.FindRefAltByVariantkey(0x2000c3521f1c15ab))
	// ACGTACGT ACGT 8 4 12

	// Reverse all VariantKeys, including the ones that are not directly reversible by using a lookup table.
	fmt.Println(vknr.ReverseVariantkey(0x2000c3521f1c15ab))
	// {4 100004 ACGTACGT ACGT 8 4} 12

	fmt.Println(vknr.GetRefLenByVariantkey(0x2000c3521f1c15ab))
	// 8

	// /\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\

	// RSIDVAR
	// -------

	// Load the lookup table for rsID to VariantKey.
	// The input binary file can be generated using the resources/tools/vkhexbin.sh script.
	// This example uses the "c/test/data/rsvk.10.bin".
	rv, err := vk.MmapBinFile("../../c/test/data/rsvk.10.bin")
	if err != nil {
		fmt.Println(err)
		os.Exit(3)
	}
	defer rv.Close()

	fmt.Println(rv.GetRVVariantkey(3))
	// 9223656209074749440

	fmt.Println(rv.FindRVVariantkeyByRsid(0, 9, 0x00000061))
	// 9223656209074749440 3

	fmt.Println(rv.GetNextRVVariantkeyByRsid(2, 9, 0x00000061))
	// 9223656209074749440 3

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

	// Load the lookup table for rsID to VariantKey.
	// The input binary file can be generated using the resources/tools/vkhexbin.sh script.
	// This example uses the "c/test/data/rsvk.m.10.bin".
	rvm, err := vk.MmapBinFile("../../c/test/data/rsvk.m.10.bin")
	if err != nil {
		fmt.Println(err)
		os.Exit(4)
	}
	defer rvm.Close()

	fmt.Println(rvm.FindAllRVVariantkeyByRsid(0, 9, 0x00000003))
	// [9223656209074749440 9223656316446408704 9223656367992733696]

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

	// Load the lookup table for VariantKey ro rsID
	// The input binary file can be generated using the resources/tools/vkhexbin.sh script.
	// This example uses the "c/test/data/vkrs.10.bin".
	vr, err := vk.MmapBinFile("../../c/test/data/vkrs.10.bin")
	if err != nil {
		fmt.Println(err)
		os.Exit(5)
	}
	defer vr.Close()

	fmt.Println(vr.GetVRRsid(3))
	// 97

	fmt.Println(vr.FindVRRsidByVariantkey(0, 9, 0x80010274003A0000))
	// 97 3

	fmt.Println(vr.FindVRChromPosRange(0, 9, 0x14, 0x000256C5, 0x000256CB))
	// 9973 7 8
}
