package variantkey

import "testing"

var testSize = 300
var dataSize = 1048576 // 1 << 20

var hashTestData = []struct {
	oh32 uint32
	oh64 uint64
	in   string
}{
	{0xfe0061e9, 0x9ae16a3b2f90404f, ""},
	{0xd824662a, 0xb3454265b6df75e3, "a"},
	{0x15eb5ed6, 0xaa8d6e5242ada51e, "ab"},
	{0xcaf25fe2, 0x24a5b3a074e7f369, "abc"},
	{0xcf297808, 0x1a5502de4a1f8101, "abcd"},
	{0x5f8d48db, 0xc22f4663e54e04d4, "abcde"},
	{0x16b8a2fd, 0xc329379e6a03c2cd, "abcdef"},
	{0xcfc5f43d, 0x3c40c92b1ccb7355, "abcdefg"},
	{0x8d1b642, 0xfee9d22990c82909, "abcdefgh"},
	{0xb382832e, 0x332c8ed4dae5ba42, "abcdefghi"},
	{0x3f19a3cb, 0xad052244b781c4eb, "0123456789"},
	{0xee83c5c, 0x3ef4c03514208c77, "0123456789 "},
	{0x6fca023f, 0x496841e83a33cc91, "0123456789-0"},
	{0x6b2c02bd, 0xd81bcb9f3679ac0c, "0123456789~01"},
	{0xb8e8fba, 0x5da5a6a117c606f6, "0123456789#012"},
	{0xe6946835, 0x5361eae17c1ff6bc, "0123456789@0123"},
	{0xfa44df74, 0x4283d4ef43627f64, "0123456789'01234"},
	{0x2a1ed264, 0x46a7416ed4861e3b, "0123456789=012345"},
	{0xbcd3277f, 0xa4abb4e0da2c594c, "0123456789+0123456"},
	{0x26bf5a67, 0xcf1c7d3ad54f9215, "0123456789*01234567"},
	{0x8eedb634, 0x07adf50b2ac764fc, "0123456789&012345678"},
	{0xa329652e, 0xdebcba8e6f3eabd1, "0123456789^0123456789"},
	{0x4ba9b4ed, 0x4dbd128af51d77e8, "0123456789%0123456789£"},
	{0x1b9ea72f, 0xd78d5f852d522e6a, "0123456789$0123456789!0"},
	{0x819d77a5, 0x80d73b843ba57db8, "size:  a.out:  bad magic"},
	{0x8b72761e, 0x8eb3808d1ccfc779, "Nepal premier won't resign."},
	{0x5f21fe43, 0xb944f8a16261e414, "C is as portable as Stonehedge!!"},
	{0xa15ead04, 0xe8f89ab6df9bdd25, "Discard medicine more than two years old."},
	{0xe3763baf, 0xa9961670ce2a46d9, "I wouldn't marry him with a ten foot pole."},
	{0x50a48aaa, 0xbdd69b798d6ba37a, "If the enemy is within range, then so are you."},
	{0x517e346c, 0xc2f8db8624fefc0e, "The major problem is with sendmail.  -Mark Horton"},
	{0x8a4b0b6c, 0x5a0a6efd52e84e2a, "How can you write a big system without C++?  -Paul Glick"},
	{0xb360937b, 0x786d7e1987023ca9, "He who has a shady past knows that nice guys finish last."},
	{0x2e5713b3, 0x5d14f96c18fe3d5e, "Free! Free!/A trip/to Mars/for 900/empty jars/Burma Shave"},
	{0xec6d1e0e, 0xec8848fd3b266c10, "His money is twice tainted: 'taint yours and 'taint mine."},
	{0x7175f31d, 0x2a578b80bb82147c, "The days of the digital watch are numbered.  -Tom Stoppard"},
	{0xdf4c5297, 0x55182f8859eca4ce, "For every action there is an equal and opposite government program."},
	{0x62359aca, 0xabcdb319fcf2826c, "You remind me of a TV show, but that's all right: I watch it anyway."},
	{0x398c0b7c, 0x1d85702503ac7eb4, "It's well we cannot hear the screams/That we create in others' dreams."},
	{0x47f9c, 0xa2b8bf3032021993, "Give me a rock, paper and scissors and I will move the world.  CCFestoon"},
	{0xe56239a7, 0x38aa3175b37f305c, "It's a tiny change to the code and not completely disgusting. - Bob Manchek"},
	{0xb556f325, 0x7e85d7b050ed2967, "There is no reason for any individual to have a computer in their home. -Ken Olsen, 1977"},
	{0x75cc5362, 0x5a05644eb66e435e, "Even if I could be Shakespeare, I think I should still choose to be Faraday. - A. Huxley"},
	{0xc401a0bf, 0x98eff6958c5e91a, "The fugacity of a constituent in a mixture of gases at a given temperature is proportional to its mole fraction.  Lewis-Randall Rule"},
	{0x4e56b7e9, 0xc3f02c4ffd5d71e6, "Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor incididunt ut labore et dolore magna aliqua. Ut enim ad minim veniam, quis nostrud exercitation ullamco laboris nisi ut aliquip ex ea commodo consequat. Duis aute irure dolor in reprehenderit in voluptate velit esse cillum dolore eu fugiat nulla pariatur. Excepteur sint occaecat cupidatat non proident, sunt in culpa qui officia deserunt mollit anim id est laborum."},
}

var expectedFarmHash64 = []uint32{
	2598464059, 797982799, 1410420968, 2134990486, 255297188, 2992121793, 4019337850, 452431531, 299850021,
	2532580744, 2199864459, 3696623795, 1053458494, 1882212500, 458884671, 3033004529, 2700149065, 2699376854,
	4220361840, 1712034059, 594028478, 2921867846, 3280331829, 326029180, 3824583307, 1612122221, 2233466664,
	1432476832, 1628777059, 1499109081, 1145519619, 3190844552, 65721842, 489963606, 1790705123, 2128624475,
	155445229, 1672724608, 3610042449, 1911523866, 1099072299, 1389770549, 3603803785, 629449419, 1552847036,
	645684964, 3151491850, 3272648435, 916494250, 1230085527, 231181488, 851743255, 1142264800, 3667013118,
	732137533, 1909203251, 4072067757, 4165088768, 956300927, 914413116, 3074915312, 3117299654, 1438494951,
	507436733, 126024219, 146044391, 165589978, 1578546616, 249776086, 1207522198, 46987739, 1157614300,
	3614377032, 586863115, 1164298657, 4140791139, 3725511003, 232064808, 512845449, 3748861010, 22638523,
	648000590, 1024246061, 4027776454, 411505255, 1973395102, 3474970689, 1029055034, 589567754, 325737734,
	257578986, 3698087965, 2305332220, 191910725, 3315355162, 2135941665, 23075771, 3252374102, 663013031,
	3444053918, 2115441882, 4081398201, 1379288194, 4225182569, 3667516477, 1709989541, 2725013602, 3639843023,
	2470483982, 877580602, 3981838403, 3762572073, 1129162571, 732225574, 3232041815, 1652884780, 2227121257,
	1426140634, 1386256573, 24035717, 1598686658, 3146815575, 739944537, 579625482, 3903349120, 389846205,
	2834153464, 1481069623, 3740748788, 3388062747, 1020177209, 734239551, 2610427744, 49703572, 1416453607,
	2815915291, 937074653, 3035635454, 3711259084, 2627383582, 3669691805, 263366740, 3565059103, 1190977418,
	2747519432, 4129538640, 2271095827, 2993032712, 795918276, 1116991810, 937372240, 1343017609, 1166522068,
	1623631848, 2721658101, 1937681565, 114616703, 954762543, 1756889687, 2936126607, 2483004780, 1927385370,
	1672737098, 2148675559, 2636210123, 1338083267, 1335160250, 2084630531, 2746885618, 636616055, 2076016059,
	408721884, 2301682622, 2691859523, 2614088922, 1975527044, 3529473373, 1490330107, 4271796078, 1910401882,
	3738454258, 2554452696, 2237827073, 2803250686, 1996680960, 839529273, 3544595875, 3909443124, 3656063205,
	837475154, 438095290, 484603494, 308425103, 268427550, 4243643405, 2849988118, 2948254999, 2102063419,
	1735616066, 1539151988, 95237878, 2005032160, 1433635018, 116647396, 881378302, 2159170082, 336034862,
	2017579106, 944743644, 1694443528, 260177668, 505662155, 3722741628, 1511077569, 1103819072, 2089123665,
	2475035432, 1120017626, 2842141483, 4029205195, 3873078673, 136118734, 1699452298, 1403506686, 1805475756,
	2562064338, 4271866024, 3071338162, 459509140, 771592405, 185232757, 4032960199, 3512945009, 308584855,
	4250142168, 2565680167, 38924274, 3770488806, 3099963860, 1255084262, 2363435042, 54945052, 2534883189,
	2432427547, 2741583197, 1280920000, 1281043691, 1121403845, 2127558730, 713121337, 2108187161, 927011680,
	4134691985, 1958963937, 2567532373, 4075249328, 4104757832, 3026358429, 3573008472, 3615577014, 1541946015,
	3087190425, 857839960, 2515339233, 2809830736, 460237542, 1950698961, 2069753399, 1106466069, 356742959,
	3662626864, 1750561299, 992181339, 3384018814, 100741310, 451656820, 3650357479, 2390172694, 2088767754,
	164402616, 2751052984, 1767810825, 3441135892, 3323383489, 2756998822, 207428029, 2648427775, 2360400900,
	1396468647, 1377764574, 1435134775, 1099809675, 3374512975, 3542220540, 4081637863, 337070226, 644850146,
	1306761320, 1242645122, 4109252858, 3377483696, 1788337208, 1658628529, 2911512007, 367022558, 3071359622,
	4273132307, 3898950547, 1858986613, 2040551642, 4077477194, 3565689036, 265993036, 1864569342, 923017956,
	490608221, 3833372385, 3287246572, 2649450292, 500120236, 2810524030, 1561519055, 3224066062, 2774151984,
	2107011431, 96459446, 1235983679, 4237425634, 276949224, 4100839753, 427484362, 4246879223, 1858777639,
	3476334357, 358032121, 2511026735, 1535473864, 556796152, 1476438092, 2913077464, 3051522276, 4046477658,
	1802040304, 990407433, 4052924496, 2926590471, 4265214507, 82077489, 464407878, 4190838199, 733509243,
	1583801700, 1877837196, 3912423882, 8759461, 2540185277, 2019419351, 4051584612, 700836153, 1675560450,
	3130433948, 405251683, 2224044848, 4071581802, 2272418128, 803575837, 4019147902, 3841480082, 3424361375,
	779434428, 3057021940, 2285701422, 1783152480, 823305654, 3032187389, 4159715581, 3420960112, 3198900547,
	3006227299, 4194096960, 1775955687, 1719108984, 684087286, 531310503, 3105682208, 3382290593, 777173623,
	3241407531, 2649684057, 1397502982, 3193669211, 811750340, 3403136990, 2540585554, 784952939, 943914610,
	3985088434, 1911188923, 519948041, 3181425568, 1089679033, 240953857, 3017658263, 3828377737, 308018483,
	4262383425, 3188015819, 4051263539, 4074952232, 1683612329, 206775997, 2283918569, 2217060665, 350160869,
	140980, 1891558063, 422986366, 330624974, 918718096, 376390582, 3424344721, 3187805406, 3855037968,
	1928519266, 3059200728, 2108753646, 1343511943, 2247006571, 622521957, 917121602, 3299763344, 2864033668,
	2661022773, 2006922227, 1237256330, 3449066284, 3285899651, 786322314, 1244759631, 3263135197, 987586766,
	3206261120, 1827135136, 1781944746, 2482286699, 1109175923, 4190721328, 1129462471, 1623777358, 3389003793,
	1646071378, 1164309901, 989577914, 3626554867, 1516846461, 3656006011, 3698796465, 3155218919, 1237411891,
	1854985978, 3939149151, 878608872, 2437686324, 3163786257, 1235300371, 1256485167, 1883344352, 2083771672,
	3066325351, 2770847216, 601221482, 3992583643, 2557027816, 900741486, 90375300, 300318232, 3253901179,
	542270815, 1273768482, 1216399252, 325675502, 3652676161, 1097584090, 3262252593, 3704419305, 411263051,
	3460621305, 1967599860, 901109753, 2682611693, 797089608, 3286110054, 2219863904, 3623364733, 3061255808,
	1615375832, 2701956286, 4145497671, 449740816, 2686506989, 1235084019, 2151665147, 2091754612, 1178454681,
	3213794286, 2601416506, 4004834921, 238887261, 186020771, 2367569534, 1962659444, 3539886328, 2144472852,
	1390394371, 3597555910, 3188438773, 3371014971, 2058751609, 1169588594, 857915866, 923161569, 4068653043,
	3808667664, 581227317, 2077539039, 851579036, 2794103714, 2094375930, 3122317317, 2365436865, 2023960931,
	2312244996, 612094988, 1555465129, 3306195841, 1702313921, 1171351291, 2043136409, 3744621107, 1028502697,
	6114625, 3359104346, 1024572712, 1927582962, 3392622118, 1347167673, 2075035198, 4202817168, 701024148,
	1481965992, 1334816273, 2870251538, 1010064531, 713520765, 4089081247, 3231042924, 2452539325, 1343734533,
	587001593, 1917607088, 3498936874, 246692543, 2836854664, 2317249321, 774652981, 1285694082, 397012087,
	1717527689, 2904461070, 3893453420, 1565179401, 600903026, 1134342772, 3234226304, 345572299, 2274770442,
	1079209397, 2122849632, 1242840526, 3987000643, 3065138774, 3111336863, 1023721001, 3763083325, 2196937373,
	2643841788, 4201389782, 4223278891, 292733931, 1424229089, 2927147928, 1048291071, 2490333812, 4098360768,
	3948800722, 335456628, 540133451, 3313113759, 3430536378, 2514123129, 2418881542, 487365389, 1136054817,
	3004241477, 4109233936, 3679809321, 3527024461, 1147434678, 3308746763, 1875093248, 4217929592, 400784472,
	160353261, 2413172925, 1853298225, 3201741245, 3680311316, 4274382900, 1131020455, 194781179, 3440090658,
	2165746386, 3106421434, 880320527, 1429837716, 252230074, 3623657004, 3869801679, 2507199021, 1659221866,
	3121647246, 3884308578, 2610217849, 641564641, 329123979, 121860586, 947795261, 1992594155, 3050771207,
	2767035539, 627269409, 1806905031, 584050483, 4142579188, 3259749808, 644172091, 3053081915, 2840648309,
	2244943480, 4057483496, 873421687, 2447660175, 1233635843, 2163464207, 2515400215, 3100476924, 470325051,
	2598261204, 850667549, 3622479237, 2781907007, 943739431, 1901484772, 939810041, 3261383939, 2212130277,
	3349254805, 2796552902, 3372846298, 3835884044, 2764936304, 1338171648, 2525665319, 4196233786, 2290169528,
	1793910997, 1554419340, 1733094688, 1084699349, 3233936866, 1428704144, 3269904970, 3347011944, 1892898231,
	1072588531, 3547435717, 1593338562, 919414554, 3953006207, 877438080, 224271045, 2914958001, 2920583824,
	1251814062, 385182008, 640855184, 4263183176, 3041193150, 3505072908, 2830570613, 1949847968, 2999344380,
	1714496583, 15918244, 2605688266, 3253705097, 4152736859, 2097020806, 2122199776, 1069285218, 670591796,
	768977505, 379861934, 1557579480, 547346027, 388559045, 1495176194, 4093461535, 1911655402, 1053371241,
	3717104621, 1144474110, 4166253320, 2747410691,
}

func TestFarmHash64Strings(t *testing.T) {
	for _, tt := range hashTestData {
		tt := tt
		t.Run("", func(t *testing.T) {
			t.Parallel()
			h := FarmHash64([]byte(tt.in))
			if h != tt.oh64 {
				t.Errorf("FarmHash64(%q)=%#08x (len=%d), want %#08x", tt.in, h, len(tt.in), tt.oh64)
			}
		})
	}
}

func BenchmarkFarmHash64(b *testing.B) {
	var buf = []byte("2ZVSmMwBTILcCekZjgZ49Py5RoJUriQ7URkCgZPw")
	b.ResetTimer()
	for i := 0; i < b.N; i++ {
		FarmHash64(buf)
	}
}

func dataSetup() []byte {
	const k0 uint64 = 0xc3a5c85c97cb3127
	data := make([]byte, dataSize)
	var a uint64 = 9
	var b uint64 = 777
	var u byte
	for i := 0; i < dataSize; i++ {
		a += b
		b += a
		a = (a ^ (a >> 41)) * k0
		b = (b^(b>>41))*k0 + uint64(i)
		u = byte(b >> 37)
		data[i] = u
	}
	return data
}

func testDataItemFarmHash64(t *testing.T, data []byte, offset int, len int, index int) {
	h := FarmHash64(data[offset : offset+len])
	a := (uint32)(h >> 32)
	if a != expectedFarmHash64[index] {
		t.Errorf("Expected %d got %d", expectedFarmHash64[index], a)
	}
	a = (uint32)((h << 32) >> 32)
	if a != expectedFarmHash64[index+1] {
		t.Errorf("Expected %d got %d", expectedFarmHash64[index+1], a)
	}
}

func TestFarmHash64(t *testing.T) {
	data := dataSetup()
	index := 0
	i := 0
	for ; i < testSize-1; i++ {
		testDataItemFarmHash64(t, data, i*i, i, index)
		index += 2
	}
	for ; i < dataSize; i += i / 7 {
		testDataItemFarmHash64(t, data, 0, i, index)
		index += 2
	}
	testDataItemFarmHash64(t, data, 0, dataSize, index)
}

func TestFarmHash32Strings(t *testing.T) {
	for _, tt := range hashTestData {
		tt := tt
		t.Run("", func(t *testing.T) {
			t.Parallel()
			h := FarmHash32([]byte(tt.in))
			if h != tt.oh32 {
				t.Errorf("Hash32(%q)=%#08x (len=%d), want %#08x", tt.in, h, len(tt.in), tt.oh32)
			}
		})
	}
}
