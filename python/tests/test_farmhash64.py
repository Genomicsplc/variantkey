"""Tests for libpyfarmhash64 module."""


import libpyvarianthash as vh
from unittest import TestCase


hashTestData = [
    (0x9ae16a3b2f90404f, ""),
    (0xb3454265b6df75e3, "a"),
    (0xaa8d6e5242ada51e, "ab"),
    (0x24a5b3a074e7f369, "abc"),
    (0x1a5502de4a1f8101, "abcd"),
    (0xc22f4663e54e04d4, "abcde"),
    (0xc329379e6a03c2cd, "abcdef"),
    (0x3c40c92b1ccb7355, "abcdefg"),
    (0xfee9d22990c82909, "abcdefgh"),
    (0x332c8ed4dae5ba42, "abcdefghi"),
    (0xad052244b781c4eb, "0123456789"),
    (0x3ef4c03514208c77, "0123456789 "),
    (0x496841e83a33cc91, "0123456789-0"),
    (0xd81bcb9f3679ac0c, "0123456789~01"),
    (0x5da5a6a117c606f6, "0123456789#012"),
    (0x5361eae17c1ff6bc, "0123456789@0123"),
    (0x4283d4ef43627f64, "0123456789'01234"),
    (0x46a7416ed4861e3b, "0123456789=012345"),
    (0xa4abb4e0da2c594c, "0123456789+0123456"),
    (0xcf1c7d3ad54f9215, "0123456789*01234567"),
    (0x07adf50b2ac764fc, "0123456789&012345678"),
    (0xdebcba8e6f3eabd1, "0123456789^0123456789"),
    (0x4dbd128af51d77e8, "0123456789%0123456789£"),
    (0xd78d5f852d522e6a, "0123456789$0123456789!0"),
    (0x80d73b843ba57db8, "size:  a.out:  bad magic"),
    (0x8eb3808d1ccfc779, "Nepal premier won't resign."),
    (0xb944f8a16261e414, "C is as portable as Stonehedge!!"),
    (0xe8f89ab6df9bdd25, "Discard medicine more than two years old."),
    (0xa9961670ce2a46d9, "I wouldn't marry him with a ten foot pole."),
    (0xbdd69b798d6ba37a, "If the enemy is within range, then so are you."),
    (0xc2f8db8624fefc0e, "The major problem is with sendmail.  -Mark Horton"),
    (0x5a0a6efd52e84e2a, "How can you write a big system without C++?  -Paul Glick"),
    (0x786d7e1987023ca9, "He who has a shady past knows that nice guys finish last."),
    (0x5d14f96c18fe3d5e, "Free! Free!/A trip/to Mars/for 900/empty jars/Burma Shave"),
    (0xec8848fd3b266c10, "His money is twice tainted: 'taint yours and 'taint mine."),
    (0x2a578b80bb82147c, "The days of the digital watch are numbered.  -Tom Stoppard"),
    (0x55182f8859eca4ce, "For every action there is an equal and opposite government program."),
    (0xabcdb319fcf2826c, "You remind me of a TV show, but that's all right: I watch it anyway."),
    (0x1d85702503ac7eb4, "It's well we cannot hear the screams/That we create in others' dreams."),
    (0xa2b8bf3032021993, "Give me a rock, paper and scissors and I will move the world.  CCFestoon"),
    (0x38aa3175b37f305c, "It's a tiny change to the code and not completely disgusting. - Bob Manchek"),
    (0x7e85d7b050ed2967, "There is no reason for any individual to have a computer in their home. -Ken Olsen, 1977"),
    (0x5a05644eb66e435e, "Even if I could be Shakespeare, I think I should still choose to be Faraday. - A. Huxley"),
    (0x98eff6958c5e91a, "The fugacity of a constituent in a mixture of gases at a given temperature is proportional to its mole fraction.  Lewis-Randall Rule"),
    (0xc3f02c4ffd5d71e6, "Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor incididunt ut labore et dolore magna aliqua. Ut enim ad minim veniam, quis nostrud exercitation ullamco laboris nisi ut aliquip ex ea commodo consequat. Duis aute irure dolor in reprehenderit in voluptate velit esse cillum dolore eu fugiat nulla pariatur. Excepteur sint occaecat cupidatat non proident, sunt in culpa qui officia deserunt mollit anim id est laborum."),
]


class TestFunctions(TestCase):

    def test_farmhash64_strings(self):
        for expected, test_input in hashTestData:
            h = vh.farmhash64(test_input)
            self.assertEqual(h, expected)


class TestBenchmark(object):

    def test_farmhash64_benchmark(self, benchmark):
        benchmark.pedantic(
            vh.farmhash64,
            args=["2ZVSmMwBTILcCekZjgZ49Py5RoJUriQ7URkCgZPw"],
            iterations=10000,
            rounds=100)
