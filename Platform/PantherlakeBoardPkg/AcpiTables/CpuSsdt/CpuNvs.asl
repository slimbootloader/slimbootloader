/** @file

  Copyright (c) 2026, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/
  //
  // Define CPU NVS Area operation region.
  //


  OperationRegion(PNVS,SystemMemory,0xFFFF0000,0xAA55)
  Field(PNVS,AnyAcc,Lock,Preserve)
  {
  PGRV, 8,  // Offset(0),     CPU GlobalNvs Revision
  CFGD, 32, // Offset(1),     PPM Flags Values
  // Offset(5) : Offset(5), Reserved bytes
  Offset(6),
  ACRT, 8,  // Offset(6),     Auto Critical Trip Point
  APSV, 8,  // Offset(7),     Auto Passive Trip Point
  AAC0, 8,  // Offset(8),     Auto Active Trip Point
  CPID, 32, // Offset(9),     CPUID
  CPPC, 8,  // Offset(13),    Boot Mode vlues for _PPC
  CLVL, 8,  // Offset(14),    ConfigTdp Number Of Levels
  CBMI, 8,  // Offset(15),    CTDP Boot Mode Index
  PL1X, 16, // Offset(16),    CTDP selected Level Power Limit1
  PL2X, 16, // Offset(18),    CTDP selected Level Power Limit2
  PLWX, 8,  // Offset(20),    CTDP selected Level Power Limit1 Time Window
  // Offset(21) : Offset(20), Reserved bytes
  Offset(21),
  C6MW, 8,  // Offset(21),    Mwait Hint value for C6
  C7MW, 8,  // Offset(22),    Mwait Hint value for C7/C7s
  CDMW, 8,  // Offset(23),    Mwait Hint value for C7/C8/C9/C10
  // Offset(24) : Offset(25), Reserved bytes
  Offset(26),
  C6LT, 16, // Offset(26),    Latency Value for C6
  C7LT, 16, // Offset(28),    Latency Value for C7/C7S
  CDLT, 16, // Offset(30),    Latency Value for C8/C9/C10
  CDPW, 16, // Offset(32),    Power value for C8/C9/C10
  MPMF, 8,  // Offset(34),    MiscPowerManagementFlags
  HWPV, 8,  // Offset(35),    HWP Version
  // Offset(36) : Offset(66), Reserved bytes
  Offset(67),
  OBIN, 8,  // Offset(67),    Indicates bins of Oc support. MSR 194h FLEX_RATIO Bits (19:17)
  HWPE, 8,  // Offset(68),    HWP Enable
  HTRO, 8,  // Offset(69),    Indicates whether Hetero is supported by SoC
  DI00, 8,  // Offset(70),    Indicates the index of Domain which contains core 0
  DI01, 8,  // Offset(71),    Indicates the index of Domain which contains core 1
  DI02, 8,  // Offset(72),    Indicates the index of Domain which contains core 2
  DI03, 8,  // Offset(73),    Indicates the index of Domain which contains core 3
  DI04, 8,  // Offset(74),    Indicates the index of Domain which contains core 4
  DI05, 8,  // Offset(75),    Indicates the index of Domain which contains core 5
  DI06, 8,  // Offset(76),    Indicates the index of Domain which contains core 6
  DI07, 8,  // Offset(77),    Indicates the index of Domain which contains core 7
  DI08, 8,  // Offset(78),    Indicates the index of Domain which contains core 8
  DI09, 8,  // Offset(79),    Indicates the index of Domain which contains core 9
  DI10, 8,  // Offset(80),    Indicates the index of Domain which contains core 10
  DI11, 8,  // Offset(81),    Indicates the index of Domain which contains core 11
  DI12, 8,  // Offset(82),    Indicates the index of Domain which contains core 12
  DI13, 8,  // Offset(83),    Indicates the index of Domain which contains core 13
  DI14, 8,  // Offset(84),    Indicates the index of Domain which contains core 14
  DI15, 8,  // Offset(85),    Indicates the index of Domain which contains core 15
  DI16, 8,  // Offset(86),    Indicates the index of Domain which contains core 16
  DI17, 8,  // Offset(87),    Indicates the index of Domain which contains core 17
  DI18, 8,  // Offset(88),    Indicates the index of Domain which contains core 18
  DI19, 8,  // Offset(89),    Indicates the index of Domain which contains core 19
  DI20, 8,  // Offset(90),    Indicates the index of Domain which contains core 20
  DI21, 8,  // Offset(91),    Indicates the index of Domain which contains core 21
  DI22, 8,  // Offset(92),    Indicates the index of Domain which contains core 22
  DI23, 8,  // Offset(93),    Indicates the index of Domain which contains core 23
  DI24, 8,  // Offset(94),    Indicates the index of Domain which contains core 24
  DI25, 8,  // Offset(95),    Indicates the index of Domain which contains core 25
  DI26, 8,  // Offset(96),    Indicates the index of Domain which contains core 26
  DI27, 8,  // Offset(97),    Indicates the index of Domain which contains core 27
  DI28, 8,  // Offset(98),    Indicates the index of Domain which contains core 28
  DI29, 8,  // Offset(99),    Indicates the index of Domain which contains core 29
  DI30, 8,  // Offset(100),   Indicates the index of Domain which contains core 30
  DI31, 8,  // Offset(101),   Indicates the index of Domain which contains core 31
  DI32, 8,  // Offset(102),   Indicates the index of Domain which contains core 32
  DI33, 8,  // Offset(103),   Indicates the index of Domain which contains core 33
  DI34, 8,  // Offset(104),   Indicates the index of Domain which contains core 34
  DI35, 8,  // Offset(105),   Indicates the index of Domain which contains core 35
  DI36, 8,  // Offset(106),   Indicates the index of Domain which contains core 36
  DI37, 8,  // Offset(107),   Indicates the index of Domain which contains core 37
  DI38, 8,  // Offset(108),   Indicates the index of Domain which contains core 38
  DI39, 8,  // Offset(109),   Indicates the index of Domain which contains core 39
  DI40, 8,  // Offset(110),   Indicates the index of Domain which contains core 40
  DI41, 8,  // Offset(111),   Indicates the index of Domain which contains core 41
  DI42, 8,  // Offset(112),   Indicates the index of Domain which contains core 42
  DI43, 8,  // Offset(113),   Indicates the index of Domain which contains core 43
  DI44, 8,  // Offset(114),   Indicates the index of Domain which contains core 44
  DI45, 8,  // Offset(115),   Indicates the index of Domain which contains core 45
  DI46, 8,  // Offset(116),   Indicates the index of Domain which contains core 46
  DI47, 8,  // Offset(117),   Indicates the index of Domain which contains core 47
  DI48, 8,  // Offset(118),   Indicates the index of Domain which contains core 48
  DI49, 8,  // Offset(119),   Indicates the index of Domain which contains core 49
  DI50, 8,  // Offset(120),   Indicates the index of Domain which contains core 50
  DI51, 8,  // Offset(121),   Indicates the index of Domain which contains core 51
  DI52, 8,  // Offset(122),   Indicates the index of Domain which contains core 52
  DI53, 8,  // Offset(123),   Indicates the index of Domain which contains core 53
  DI54, 8,  // Offset(124),   Indicates the index of Domain which contains core 54
  DI55, 8,  // Offset(125),   Indicates the index of Domain which contains core 55
  DI56, 8,  // Offset(126),   Indicates the index of Domain which contains core 56
  DI57, 8,  // Offset(127),   Indicates the index of Domain which contains core 57
  DI58, 8,  // Offset(128),   Indicates the index of Domain which contains core 58
  DI59, 8,  // Offset(129),   Indicates the index of Domain which contains core 59
  DI60, 8,  // Offset(130),   Indicates the index of Domain which contains core 60
  DI61, 8,  // Offset(131),   Indicates the index of Domain which contains core 61
  DI62, 8,  // Offset(132),   Indicates the index of Domain which contains core 62
  DI63, 8,  // Offset(133),   Indicates the index of Domain which contains core 63
  CM00, 8,  // Offset(134),   Indicates the number of cores in the Domain which contains core 0
  CM01, 8,  // Offset(135),   Indicates the number of cores in the Domain which contains core 1
  CM02, 8,  // Offset(136),   Indicates the number of cores in the Domain which contains core 2
  CM03, 8,  // Offset(137),   Indicates the number of cores in the Domain which contains core 3
  CM04, 8,  // Offset(138),   Indicates the number of cores in the Domain which contains core 4
  CM05, 8,  // Offset(139),   Indicates the number of cores in the Domain which contains core 5
  CM06, 8,  // Offset(140),   Indicates the number of cores in the Domain which contains core 6
  CM07, 8,  // Offset(141),   Indicates the number of cores in the Domain which contains core 7
  CM08, 8,  // Offset(142),   Indicates the number of cores in the Domain which contains core 8
  CM09, 8,  // Offset(143),   Indicates the number of cores in the Domain which contains core 9
  CM10, 8,  // Offset(144),   Indicates the number of cores in the Domain which contains core 10
  CM11, 8,  // Offset(145),   Indicates the number of cores in the Domain which contains core 11
  CM12, 8,  // Offset(146),   Indicates the number of cores in the Domain which contains core 12
  CM13, 8,  // Offset(147),   Indicates the number of cores in the Domain which contains core 13
  CM14, 8,  // Offset(148),   Indicates the number of cores in the Domain which contains core 14
  CM15, 8,  // Offset(149),   Indicates the number of cores in the Domain which contains core 15
  CM16, 8,  // Offset(150),   Indicates the number of cores in the Domain which contains core 16
  CM17, 8,  // Offset(151),   Indicates the number of cores in the Domain which contains core 17
  CM18, 8,  // Offset(152),   Indicates the number of cores in the Domain which contains core 18
  CM19, 8,  // Offset(153),   Indicates the number of cores in the Domain which contains core 19
  CM20, 8,  // Offset(154),   Indicates the number of cores in the Domain which contains core 20
  CM21, 8,  // Offset(155),   Indicates the number of cores in the Domain which contains core 21
  CM22, 8,  // Offset(156),   Indicates the number of cores in the Domain which contains core 22
  CM23, 8,  // Offset(157),   Indicates the number of cores in the Domain which contains core 23
  CM24, 8,  // Offset(158),   Indicates the number of cores in the Domain which contains core 24
  CM25, 8,  // Offset(159),   Indicates the number of cores in the Domain which contains core 25
  CM26, 8,  // Offset(160),   Indicates the number of cores in the Domain which contains core 26
  CM27, 8,  // Offset(161),   Indicates the number of cores in the Domain which contains core 27
  CM28, 8,  // Offset(162),   Indicates the number of cores in the Domain which contains core 28
  CM29, 8,  // Offset(163),   Indicates the number of cores in the Domain which contains core 29
  CM30, 8,  // Offset(164),   Indicates the number of cores in the Domain which contains core 30
  CM31, 8,  // Offset(165),   Indicates the number of cores in the Domain which contains core 31
  CM32, 8,  // Offset(166),   Indicates the number of cores in the Domain which contains core 32
  CM33, 8,  // Offset(167),   Indicates the number of cores in the Domain which contains core 33
  CM34, 8,  // Offset(168),   Indicates the number of cores in the Domain which contains core 34
  CM35, 8,  // Offset(169),   Indicates the number of cores in the Domain which contains core 35
  CM36, 8,  // Offset(170),   Indicates the number of cores in the Domain which contains core 36
  CM37, 8,  // Offset(171),   Indicates the number of cores in the Domain which contains core 37
  CM38, 8,  // Offset(172),   Indicates the number of cores in the Domain which contains core 38
  CM39, 8,  // Offset(173),   Indicates the number of cores in the Domain which contains core 39
  CM40, 8,  // Offset(174),   Indicates the number of cores in the Domain which contains core 40
  CM41, 8,  // Offset(175),   Indicates the number of cores in the Domain which contains core 41
  CM42, 8,  // Offset(176),   Indicates the number of cores in the Domain which contains core 42
  CM43, 8,  // Offset(177),   Indicates the number of cores in the Domain which contains core 43
  CM44, 8,  // Offset(178),   Indicates the number of cores in the Domain which contains core 44
  CM45, 8,  // Offset(179),   Indicates the number of cores in the Domain which contains core 45
  CM46, 8,  // Offset(180),   Indicates the number of cores in the Domain which contains core 46
  CM47, 8,  // Offset(181),   Indicates the number of cores in the Domain which contains core 47
  CM48, 8,  // Offset(182),   Indicates the number of cores in the Domain which contains core 48
  CM49, 8,  // Offset(183),   Indicates the number of cores in the Domain which contains core 49
  CM50, 8,  // Offset(184),   Indicates the number of cores in the Domain which contains core 50
  CM51, 8,  // Offset(185),   Indicates the number of cores in the Domain which contains core 51
  CM52, 8,  // Offset(186),   Indicates the number of cores in the Domain which contains core 52
  CM53, 8,  // Offset(187),   Indicates the number of cores in the Domain which contains core 53
  CM54, 8,  // Offset(188),   Indicates the number of cores in the Domain which contains core 54
  CM55, 8,  // Offset(189),   Indicates the number of cores in the Domain which contains core 55
  CM56, 8,  // Offset(190),   Indicates the number of cores in the Domain which contains core 56
  CM57, 8,  // Offset(191),   Indicates the number of cores in the Domain which contains core 57
  CM58, 8,  // Offset(192),   Indicates the number of cores in the Domain which contains core 58
  CM59, 8,  // Offset(193),   Indicates the number of cores in the Domain which contains core 59
  CM60, 8,  // Offset(194),   Indicates the number of cores in the Domain which contains core 60
  CM61, 8,  // Offset(195),   Indicates the number of cores in the Domain which contains core 61
  CM62, 8,  // Offset(196),   Indicates the number of cores in the Domain which contains core 62
  CM63, 8,  // Offset(197),   Indicates the number of cores in the Domain which contains core 63
  P100, 8,  // Offset(198),   Indicate the P1 ratio of core 0
  P101, 8,  // Offset(199),   Indicate the P1 ratio of core 1
  P102, 8,  // Offset(200),   Indicate the P1 ratio of core 2
  P103, 8,  // Offset(201),   Indicate the P1 ratio of core 3
  P104, 8,  // Offset(202),   Indicate the P1 ratio of core 4
  P105, 8,  // Offset(203),   Indicate the P1 ratio of core 5
  P106, 8,  // Offset(204),   Indicate the P1 ratio of core 6
  P107, 8,  // Offset(205),   Indicate the P1 ratio of core 7
  P108, 8,  // Offset(206),   Indicate the P1 ratio of core 8
  P109, 8,  // Offset(207),   Indicate the P1 ratio of core 9
  P110, 8,  // Offset(208),   Indicate the P1 ratio of core 10
  P111, 8,  // Offset(209),   Indicate the P1 ratio of core 11
  P112, 8,  // Offset(210),   Indicate the P1 ratio of core 12
  P113, 8,  // Offset(211),   Indicate the P1 ratio of core 13
  P114, 8,  // Offset(212),   Indicate the P1 ratio of core 14
  P115, 8,  // Offset(213),   Indicate the P1 ratio of core 15
  P116, 8,  // Offset(214),   Indicate the P1 ratio of core 16
  P117, 8,  // Offset(215),   Indicate the P1 ratio of core 17
  P118, 8,  // Offset(216),   Indicate the P1 ratio of core 18
  P119, 8,  // Offset(217),   Indicate the P1 ratio of core 19
  P120, 8,  // Offset(218),   Indicate the P1 ratio of core 20
  P121, 8,  // Offset(219),   Indicate the P1 ratio of core 21
  P122, 8,  // Offset(220),   Indicate the P1 ratio of core 22
  P123, 8,  // Offset(221),   Indicate the P1 ratio of core 23
  P124, 8,  // Offset(222),   Indicate the P1 ratio of core 24
  P125, 8,  // Offset(223),   Indicate the P1 ratio of core 25
  P126, 8,  // Offset(224),   Indicate the P1 ratio of core 26
  P127, 8,  // Offset(225),   Indicate the P1 ratio of core 27
  P128, 8,  // Offset(226),   Indicate the P1 ratio of core 28
  P129, 8,  // Offset(227),   Indicate the P1 ratio of core 29
  P130, 8,  // Offset(228),   Indicate the P1 ratio of core 30
  P131, 8,  // Offset(229),   Indicate the P1 ratio of core 31
  P132, 8,  // Offset(230),   Indicate the P1 ratio of core 32
  P133, 8,  // Offset(231),   Indicate the P1 ratio of core 33
  P134, 8,  // Offset(232),   Indicate the P1 ratio of core 34
  P135, 8,  // Offset(233),   Indicate the P1 ratio of core 35
  P136, 8,  // Offset(234),   Indicate the P1 ratio of core 36
  P137, 8,  // Offset(235),   Indicate the P1 ratio of core 37
  P138, 8,  // Offset(236),   Indicate the P1 ratio of core 38
  P139, 8,  // Offset(237),   Indicate the P1 ratio of core 39
  P140, 8,  // Offset(238),   Indicate the P1 ratio of core 40
  P141, 8,  // Offset(239),   Indicate the P1 ratio of core 41
  P142, 8,  // Offset(240),   Indicate the P1 ratio of core 42
  P143, 8,  // Offset(241),   Indicate the P1 ratio of core 43
  P144, 8,  // Offset(242),   Indicate the P1 ratio of core 44
  P145, 8,  // Offset(243),   Indicate the P1 ratio of core 45
  P146, 8,  // Offset(244),   Indicate the P1 ratio of core 46
  P147, 8,  // Offset(245),   Indicate the P1 ratio of core 47
  P148, 8,  // Offset(246),   Indicate the P1 ratio of core 48
  P149, 8,  // Offset(247),   Indicate the P1 ratio of core 49
  P150, 8,  // Offset(248),   Indicate the P1 ratio of core 50
  P151, 8,  // Offset(249),   Indicate the P1 ratio of core 51
  P152, 8,  // Offset(250),   Indicate the P1 ratio of core 52
  P153, 8,  // Offset(251),   Indicate the P1 ratio of core 53
  P154, 8,  // Offset(252),   Indicate the P1 ratio of core 54
  P155, 8,  // Offset(253),   Indicate the P1 ratio of core 55
  P156, 8,  // Offset(254),   Indicate the P1 ratio of core 56
  P157, 8,  // Offset(255),   Indicate the P1 ratio of core 57
  P158, 8,  // Offset(256),   Indicate the P1 ratio of core 58
  P159, 8,  // Offset(257),   Indicate the P1 ratio of core 59
  P160, 8,  // Offset(258),   Indicate the P1 ratio of core 60
  P161, 8,  // Offset(259),   Indicate the P1 ratio of core 61
  P162, 8,  // Offset(260),   Indicate the P1 ratio of core 62
  P163, 8,  // Offset(261),   Indicate the P1 ratio of core 63
  IS00, 8,  // Offset(262),   Indicate the IPC scaling of core 0
  IS01, 8,  // Offset(263),   Indicate the IPC scaling of core 1
  IS02, 8,  // Offset(264),   Indicate the IPC scaling of core 2
  IS03, 8,  // Offset(265),   Indicate the IPC scaling of core 3
  IS04, 8,  // Offset(266),   Indicate the IPC scaling of core 4
  IS05, 8,  // Offset(267),   Indicate the IPC scaling of core 5
  IS06, 8,  // Offset(268),   Indicate the IPC scaling of core 6
  IS07, 8,  // Offset(269),   Indicate the IPC scaling of core 7
  IS08, 8,  // Offset(270),   Indicate the IPC scaling of core 8
  IS09, 8,  // Offset(271),   Indicate the IPC scaling of core 9
  IS10, 8,  // Offset(272),   Indicate the IPC scaling of core 10
  IS11, 8,  // Offset(273),   Indicate the IPC scaling of core 11
  IS12, 8,  // Offset(274),   Indicate the IPC scaling of core 12
  IS13, 8,  // Offset(275),   Indicate the IPC scaling of core 13
  IS14, 8,  // Offset(276),   Indicate the IPC scaling of core 14
  IS15, 8,  // Offset(277),   Indicate the IPC scaling of core 15
  IS16, 8,  // Offset(278),   Indicate the IPC scaling of core 16
  IS17, 8,  // Offset(279),   Indicate the IPC scaling of core 17
  IS18, 8,  // Offset(280),   Indicate the IPC scaling of core 18
  IS19, 8,  // Offset(281),   Indicate the IPC scaling of core 19
  IS20, 8,  // Offset(282),   Indicate the IPC scaling of core 20
  IS21, 8,  // Offset(283),   Indicate the IPC scaling of core 21
  IS22, 8,  // Offset(284),   Indicate the IPC scaling of core 22
  IS23, 8,  // Offset(285),   Indicate the IPC scaling of core 23
  IS24, 8,  // Offset(286),   Indicate the IPC scaling of core 24
  IS25, 8,  // Offset(287),   Indicate the IPC scaling of core 25
  IS26, 8,  // Offset(288),   Indicate the IPC scaling of core 26
  IS27, 8,  // Offset(289),   Indicate the IPC scaling of core 27
  IS28, 8,  // Offset(290),   Indicate the IPC scaling of core 28
  IS29, 8,  // Offset(291),   Indicate the IPC scaling of core 29
  IS30, 8,  // Offset(292),   Indicate the IPC scaling of core 30
  IS31, 8,  // Offset(293),   Indicate the IPC scaling of core 31
  IS32, 8,  // Offset(294),   Indicate the IPC scaling of core 32
  IS33, 8,  // Offset(295),   Indicate the IPC scaling of core 33
  IS34, 8,  // Offset(296),   Indicate the IPC scaling of core 34
  IS35, 8,  // Offset(297),   Indicate the IPC scaling of core 35
  IS36, 8,  // Offset(298),   Indicate the IPC scaling of core 36
  IS37, 8,  // Offset(299),   Indicate the IPC scaling of core 37
  IS38, 8,  // Offset(300),   Indicate the IPC scaling of core 38
  IS39, 8,  // Offset(301),   Indicate the IPC scaling of core 39
  IS40, 8,  // Offset(302),   Indicate the IPC scaling of core 40
  IS41, 8,  // Offset(303),   Indicate the IPC scaling of core 41
  IS42, 8,  // Offset(304),   Indicate the IPC scaling of core 42
  IS43, 8,  // Offset(305),   Indicate the IPC scaling of core 43
  IS44, 8,  // Offset(306),   Indicate the IPC scaling of core 44
  IS45, 8,  // Offset(307),   Indicate the IPC scaling of core 45
  IS46, 8,  // Offset(308),   Indicate the IPC scaling of core 46
  IS47, 8,  // Offset(309),   Indicate the IPC scaling of core 47
  IS48, 8,  // Offset(310),   Indicate the IPC scaling of core 48
  IS49, 8,  // Offset(311),   Indicate the IPC scaling of core 49
  IS50, 8,  // Offset(312),   Indicate the IPC scaling of core 50
  IS51, 8,  // Offset(313),   Indicate the IPC scaling of core 51
  IS52, 8,  // Offset(314),   Indicate the IPC scaling of core 52
  IS53, 8,  // Offset(315),   Indicate the IPC scaling of core 53
  IS54, 8,  // Offset(316),   Indicate the IPC scaling of core 54
  IS55, 8,  // Offset(317),   Indicate the IPC scaling of core 55
  IS56, 8,  // Offset(318),   Indicate the IPC scaling of core 56
  IS57, 8,  // Offset(319),   Indicate the IPC scaling of core 57
  IS58, 8,  // Offset(320),   Indicate the IPC scaling of core 58
  IS59, 8,  // Offset(321),   Indicate the IPC scaling of core 59
  IS60, 8,  // Offset(322),   Indicate the IPC scaling of core 60
  IS61, 8,  // Offset(323),   Indicate the IPC scaling of core 61
  IS62, 8,  // Offset(324),   Indicate the IPC scaling of core 62
  IS63, 8,  // Offset(325),   Indicate the IPC scaling of core 63
  MXP1, 8,  // Offset(326),   Indicate the max P1 ratio of all cores
  BSFQ, 16, // Offset(327),   Bus frequency
  RPEF, 8,  // Offset(329),   Enable/Disable Resource Priority Feature
  ISEN, 8,  // Offset(330),   Enable/Disable ThETA Ibatt Feature
  LL1E, 8,  // Offset(331),   Enable/Disable Isys Limit L1 Feature
  LL2E, 8,  // Offset(332),   Enable/Disable Isys Limit L2 Feature
  TBMD, 8,  // Offset(333),   Enable/Disable Turbo Mode
  }