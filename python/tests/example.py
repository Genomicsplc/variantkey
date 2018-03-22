import variantkey as vh

h = vh.variantkey("6", 193330, "TCA", "TGTCG")
print(h[0], h[1], h[2])

s = vh.variantkey_string(h)
print(s)

v = vh.parse_variantkey_string("08027a2a00338000")

d = vh.decode_variantkey(v)
print(d[0], d[1], d[2])
