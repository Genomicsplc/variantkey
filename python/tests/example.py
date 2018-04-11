#!/usr/bin/env python3

import variantkey as vk

vkey = vk.variantkey(b"X", 193330, b"GCA", b"G")
print(vkey)  # 13259012476408233984

vkrange = vk.variantkey_range(15, 12002028, 12152133)
print(vkrange)  # (8672685443424190464, 8673007793604657151)

s = vk.variantkey_hex(vkey)
print(s)  # b'b801799918c90000'

v = vk.parse_variantkey_hex(s)
print(v)  # 13259012476408233984

chrom, pos, ref, alt, sizeref, sizealt = vk.reverse_variantkey(v)
print(chrom, pos, ref, alt, sizeref, sizealt)  # b'X' 193330 b'GCA' b'G' 3 1

chrom, pos, refalt = vk.decode_variantkey(v)
print(chrom, pos, refalt)  # 23 193330 415825920

c = vk.decode_chrom(chrom)
print(c)  # b'X'

ref, alt, reflen, altlen = vk.decode_refalt(refalt)
print(ref, alt, reflen, altlen)  # b'GCA' b'G' 3 1
