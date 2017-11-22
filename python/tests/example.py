import libpyvarianthash as vh

h = vh.variant_hash("6", 193330, "TCA", "TGTCG")
print(h[0], h[1], h[2])

s = vh.variant_hash_string(h[0], h[1], h[2])
print(s)

d = vh.decode_variant_hash_string("000000060002f332e683c0ceef5adee0")
print(d[0], d[1], d[2])
