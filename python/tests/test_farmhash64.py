"""Tests for libpyvarianthash module."""


import libpyvarianthash as vh
from unittest import TestCase


hashTestData = [
    (0xfe0061e9, 0x9ae16a3b2f90404f, ""),
    (0xd824662a, 0xb3454265b6df75e3, "a"),
    (0x15eb5ed6, 0xaa8d6e5242ada51e, "ab"),
    (0xcaf25fe2, 0x24a5b3a074e7f369, "abc"),
    (0xcf297808, 0x1a5502de4a1f8101, "abcd"),
    (0x5f8d48db, 0xc22f4663e54e04d4, "abcde"),
    (0x16b8a2fd, 0xc329379e6a03c2cd, "abcdef"),
    (0xcfc5f43d, 0x3c40c92b1ccb7355, "abcdefg"),
    (0x8d1b642, 0xfee9d22990c82909, "abcdefgh"),
    (0xb382832e, 0x332c8ed4dae5ba42, "abcdefghi"),
    (0x3f19a3cb, 0xad052244b781c4eb, "0123456789"),
    (0xee83c5c, 0x3ef4c03514208c77, "0123456789 "),
    (0x6fca023f, 0x496841e83a33cc91, "0123456789-0"),
    (0x6b2c02bd, 0xd81bcb9f3679ac0c, "0123456789~01"),
    (0xb8e8fba, 0x5da5a6a117c606f6, "0123456789#012"),
    (0xe6946835, 0x5361eae17c1ff6bc, "0123456789@0123"),
    (0xfa44df74, 0x4283d4ef43627f64, "0123456789'01234"),
    (0x2a1ed264, 0x46a7416ed4861e3b, "0123456789=012345"),
    (0xbcd3277f, 0xa4abb4e0da2c594c, "0123456789+0123456"),
    (0x26bf5a67, 0xcf1c7d3ad54f9215, "0123456789*01234567"),
    (0x8eedb634, 0x07adf50b2ac764fc, "0123456789&012345678"),
    (0xa329652e, 0xdebcba8e6f3eabd1, "0123456789^0123456789"),
    (0x4ba9b4ed, 0x4dbd128af51d77e8, "0123456789%0123456789£"),
    (0x1b9ea72f, 0xd78d5f852d522e6a, "0123456789$0123456789!0"),
    (0x819d77a5, 0x80d73b843ba57db8, "size:  a.out:  bad magic"),
    (0x8b72761e, 0x8eb3808d1ccfc779, "Nepal premier won't resign."),
    (0x5f21fe43, 0xb944f8a16261e414, "C is as portable as Stonehedge!!"),
    (0xa15ead04, 0xe8f89ab6df9bdd25, "Discard medicine more than two years old."),
    (0xe3763baf, 0xa9961670ce2a46d9, "I wouldn't marry him with a ten foot pole."),
    (0x50a48aaa, 0xbdd69b798d6ba37a, "If the enemy is within range, then so are you."),
    (0x517e346c, 0xc2f8db8624fefc0e, "The major problem is with sendmail.  -Mark Horton"),
    (0x8a4b0b6c, 0x5a0a6efd52e84e2a, "How can you write a big system without C++?  -Paul Glick"),
    (0xb360937b, 0x786d7e1987023ca9, "He who has a shady past knows that nice guys finish last."),
    (0x2e5713b3, 0x5d14f96c18fe3d5e, "Free! Free!/A trip/to Mars/for 900/empty jars/Burma Shave"),
    (0xec6d1e0e, 0xec8848fd3b266c10, "His money is twice tainted: 'taint yours and 'taint mine."),
    (0x7175f31d, 0x2a578b80bb82147c, "The days of the digital watch are numbered.  -Tom Stoppard"),
    (0xdf4c5297, 0x55182f8859eca4ce, "For every action there is an equal and opposite government program."),
    (0x62359aca, 0xabcdb319fcf2826c, "You remind me of a TV show, but that's all right: I watch it anyway."),
    (0x398c0b7c, 0x1d85702503ac7eb4, "It's well we cannot hear the screams/That we create in others' dreams."),
    (0x47f9c, 0xa2b8bf3032021993, "Give me a rock, paper and scissors and I will move the world.  CCFestoon"),
    (0xe56239a7, 0x38aa3175b37f305c, "It's a tiny change to the code and not completely disgusting. - Bob Manchek"),
    (0xb556f325, 0x7e85d7b050ed2967, "There is no reason for any individual to have a computer in their home. -Ken Olsen, 1977"),
    (0x75cc5362, 0x5a05644eb66e435e, "Even if I could be Shakespeare, I think I should still choose to be Faraday. - A. Huxley"),
    (0xc401a0bf, 0x98eff6958c5e91a, "The fugacity of a constituent in a mixture of gases at a given temperature is proportional to its mole fraction.  Lewis-Randall Rule"),
    (0x4e56b7e9, 0xc3f02c4ffd5d71e6, "Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor incididunt ut labore et dolore magna aliqua. Ut enim ad minim veniam, quis nostrud exercitation ullamco laboris nisi ut aliquip ex ea commodo consequat. Duis aute irure dolor in reprehenderit in voluptate velit esse cillum dolore eu fugiat nulla pariatur. Excepteur sint occaecat cupidatat non proident, sunt in culpa qui officia deserunt mollit anim id est laborum."),
]


class TestFunctions(TestCase):

    def test_farmhash64_strings(self):
        for expected32, expected64, test_input in hashTestData:
            h = vh.farmhash64(test_input)
            self.assertEqual(h, expected64)

    def test_farmhash32_strings(self):
        for expected32, expected64, test_input in hashTestData:
            h = vh.farmhash32(test_input)
            self.assertEqual(h, expected32)


class TestBenchmark(object):

    def test_farmhash64_benchmark(self, benchmark):
        benchmark.pedantic(
            vh.farmhash64,
            args=["2ZVSmMwBTILcCekZjgZ49Py5RoJUriQ7URkCgZPw"],
            iterations=10000,
            rounds=100)
