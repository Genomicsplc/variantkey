import variantkey as vh

h = vh.variantkey128("6", 193330, "TCA", "TGTCG")
print(h[0], h[1], h[2])

s = vh.variantkey128_string(h[0], h[1], h[2])
print(s)

d = vh.decode_variantkey128_string("000000060002f332e683c0ceef5adee0")
print(d[0], d[1], d[2])
