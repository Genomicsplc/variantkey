import variantkey as vh

vk = vh.variantkey("X", 193330, "TCA", "TGTCG")
print(vk)

s = vh.variantkey_string(vk)
print(s)

v = vh.parse_variantkey_string(s)
print(v)

chrom, pos, refalt = vh.decode_variantkey(v)
print(chrom, pos, refalt)
