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
	gref, err := vk.MmapGenorefFile("../c/test/data/genoref.bin")
	if err != nil {
		fmt.Println(err)
		os.Exit(1)
	}
	defer gref.Close()

	fmt.Println(gref.GetGenorefSeq(23, 0))
	// 65

	fmt.Println(gref.CheckReference(23, 0, "A"))
	// 0

	fmt.Println(vk.FlipAllele("ATCGMKRYBVDHWSNatcgmkrybvdhwsn"))
	// TAGCKMYRVBHDWSNTAGCKMYRVBHDWSN

	// Normalize a variant - this function should be used before generating a new VariantKey
	fmt.Println(gref.NormalizeVariant(13, 2, "CDE", "CFE"))
	// 48 3 D F 1 1

	// Create a normalized variantkey
	fmt.Println(gref.NormalizedVariantKey("13", 2, 0, "CDE", "CFE"))
	// 7493989787586955617 48

	// /\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\

	// NRVK
	// ----

	// Load the lookup table for non-reversible variantkeys.
	// The input binary files can be generated from a normalized VCF file using the resources/tools/nrvk.sh script.
	// The VCF file can be normalized using the `resources/tools/vcfnorm.sh` script.
	// This example uses the "c/test/data/nrvk.10.bin".
	nrvkmf, nrvk, err := vk.MmapNRVKFile("../c/test/data/nrvk.10.bin")
	if err != nil {
		fmt.Println(err)
		os.Exit(2)
	}
	defer nrvkmf.Close()

	fmt.Println(nrvk.FindRefAltByVariantKey(0x2000c3521f1c15ab))
	// ACGTACGT ACGT 8 4 12

	// Reverse all VariantKeys, including the ones that are not directly reversible by using a lookup table.
	fmt.Println(nrvk.ReverseVariantKey(0x2000c3521f1c15ab))
	// {4 100004 ACGTACGT ACGT 8 4} 12

	fmt.Println(nrvk.GetVariantKeyRefLength(0x2000c3521f1c15ab))
	// 8

	fmt.Println(nrvk.GetVariantKeyEndPos(0x2000c3521f1c15ab))
	// 100012

	fmt.Println(vk.GetVariantKeyChromStartPos(0x2000c3521f1c15ab))
	// 1073841828

	fmt.Println(nrvk.GetVariantKeyChromEndPos(0x2000c3521f1c15ab))
	// 1073841836

	// /\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\

	// RSIDVAR
	// -------

	// Load the lookup table for rsID to VariantKey.
	// The input binary file can be generated using the resources/tools/rsvk.sh script.
	// This example uses the "c/test/data/rsvk.10.bin".
	rvmf, rv, err := vk.MmapRSVKFile("../c/test/data/rsvk.10.bin", []uint8{})
	if err != nil {
		fmt.Println(err)
		os.Exit(3)
	}
	defer rvmf.Close()

	fmt.Println(rv.FindRVVariantKeyByRsid(0, 9, 0x00000061))
	// 9223656209074749440 3

	fmt.Println(rv.GetNextRVVariantKeyByRsid(2, 9, 0x00000061))
	// 9223656209074749440 3

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

	// Load the lookup table for rsID to VariantKey.
	// The input binary file can be generated using the resources/tools/rsvk.sh script.
	// This example uses the "c/test/data/rsvk.m.10.bin".
	rvmmf, rvm, err := vk.MmapRSVKFile("../c/test/data/rsvk.m.10.bin", []uint8{})
	if err != nil {
		fmt.Println(err)
		os.Exit(4)
	}
	defer rvmmf.Close()

	fmt.Println(rvm.FindAllRVVariantKeyByRsid(0, 9, 0x00000003))
	// [9223656209074749440 9223656316446408704 9223656367992733696]

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

	// Load the lookup table for VariantKey ro rsID
	// The input binary file can be generated using the resources/tools/vkrs.sh script.
	// This example uses the "c/test/data/vkrs.10.bin".
	vrmf, vr, err := vk.MmapVKRSFile("../c/test/data/vkrs.10.bin", []uint8{})
	if err != nil {
		fmt.Println(err)
		os.Exit(5)
	}
	defer vrmf.Close()

	fmt.Println(vr.FindVRRsidByVariantKey(0, 9, 0x80010274003A0000))
	// 97 3

	fmt.Println(vr.FindVRChromPosRange(0, 9, 0x14, 0x000256C5, 0x000256CB))
	// 9973 7 8

	// /\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\

	// REGIONKEY
	// ---------

	fmt.Println(vk.EncodeRegionStrand(-1))
	// 2

	fmt.Println(vk.DecodeRegionStrand(2))
	// -1

	fmt.Println(vk.EncodeRegionKey(25, 1000, 2000, 2))
	// 14411520955069251204

	fmt.Println(vk.ExtractRegionKeyChrom(0xc80001f400003e84))
	// 25

	fmt.Println(vk.ExtractRegionKeyStartPos(0xc80001f400003e84))
	// 1000

	fmt.Println(vk.ExtractRegionKeyEndPos(0xc80001f400003e84))
	// 2000

	fmt.Println(vk.ExtractRegionKeyStrand(0xc80001f400003e84))
	// 2

	fmt.Println(vk.DecodeRegionKey(0xc80001f400003e84))
	// {25 1000 2000 2}

	fmt.Println(vk.ReverseRegionKey(0xc80001f400003e84))
	// {MT 1000 2000 -1}

	fmt.Println(vk.RegionKey("MT", 1000, 2000, -1))
	// 14411520955069251204

	fmt.Println(vk.ExtendRegionKey(14411520955069251204, 100))
	// 14411520740320887204

	fmt.Println(vk.GetRegionKeyChromStartPos(0xc80001f400003e84))
	// 6710887400

	fmt.Println(vk.GetRegionKeyChromEndPos(0xc80001f400003e84))
	// 6710888400

	fmt.Println(vk.AreOverlappingRegions(5, 4, 6, 5, 3, 7))
	// true

	fmt.Println(vk.AreOverlappingRegionRegionKey(5, 4, 6, 0x2800000180000038))
	// true

	fmt.Println(vk.AreOverlappingRegionKeys(0x2800000200000030, 0x2800000180000038))
	// true

	fmt.Println(nrvk.AreOverlappingVariantKeyRegionKey(0x2800000210920000, 0x2800000180000038))
	// true

	fmt.Println(nrvk.VariantToRegionkey(0x2800000210920000))
	// 2882303770107052080

	// /\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\

	// ESID
	// ---------

	fmt.Println(vk.EncodeStringID("A0A022YWF9", 0))
	// 12128340051199752601

	fmt.Println(vk.DecodeStringID(0xa850850492e77999))
	// A0A022YWF9

	fmt.Println(vk.EncodeStringNumID("ABC:0000123456", ':'))
	// 15592178792074961472

	fmt.Println(vk.DecodeStringID(0xd8628c002001e240))
	// ABC:0000123456

	fmt.Println(vk.HashStringID("0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ"))
	// 12945031672818874332

}
