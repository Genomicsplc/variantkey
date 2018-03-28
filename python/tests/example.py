import variantkey as vk

vkey = vk.variantkey("X", 193330, "GCA", "G")
print(vkey)
# 13259012477095908224

s = vk.variantkey_string(vkey)
print(s)
# b'b801799941c61380'

v = vk.parse_variantkey_string(s)
print(v)
# 13259012477095908224

chrom, pos, refalt = vk.decode_variantkey(v)
print(chrom, pos, refalt)
# 23 193330 1103500160

c = vk.decode_chrom(chrom)
print(c)
# X

ref, alt, reflen, altlen = vk.decode_refalt(refalt)
print(ref, alt, reflen, altlen)
# GCA G 3 1
