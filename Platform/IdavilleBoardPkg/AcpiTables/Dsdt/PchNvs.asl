/** @file

  Copyright (c) 2020, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

//
// Define PCH NVS Area operatino region.
//
Name (PNVB, 0xFFFF0000)  // PCH NVS Base address
Name (PNVL, 0xAA55)      // PCH NVS Length
OperationRegion(PNVA,SystemMemory,PNVB,PNVL)
Field(PNVA,AnyAcc,Lock,Preserve)
{  Offset(0),      PCHS, 16, // Offset(0),     PCH Series
Offset(2),      PCHG, 16, // Offset(2),     PCH Generation
Offset(4),      PSTP, 16, // Offset(4),     PCH Stepping
Offset(6),      RPA1, 32, // Offset(6),     Root Port address 1
Offset(10),     RPA2, 32, // Offset(10),    Root Port address 2
Offset(14),     RPA3, 32, // Offset(14),    Root Port address 3
Offset(18),     RPA4, 32, // Offset(18),    Root Port address 4
Offset(22),     RPA5, 32, // Offset(22),    Root Port address 5
Offset(26),     RPA6, 32, // Offset(26),    Root Port address 6
Offset(30),     RPA7, 32, // Offset(30),    Root Port address 7
Offset(34),     RPA8, 32, // Offset(34),    Root Port address 8
Offset(38),     RPA9, 32, // Offset(38),    Root Port address 9
Offset(42),     RPAA, 32, // Offset(42),    Root Port address 10
Offset(46),     RPAB, 32, // Offset(46),    Root Port address 11
Offset(50),     RPAC, 32, // Offset(50),    Root Port address 12
Offset(54),     RPAD, 32, // Offset(54),    Root Port address 13
Offset(58),     RPAE, 32, // Offset(58),    Root Port address 14
Offset(62),     RPAF, 32, // Offset(62),    Root Port address 15
Offset(66),     RPAG, 32, // Offset(66),    Root Port address 16
Offset(70),     RPAH, 32, // Offset(70),    Root Port address 17
Offset(74),     RPAI, 32, // Offset(74),    Root Port address 18
Offset(78),     RPAJ, 32, // Offset(78),    Root Port address 19
Offset(82),     RPAK, 32, // Offset(82),    Root Port address 20
Offset(86),     RPAL, 32, // Offset(86),    Root Port address 21
Offset(90),     RPAM, 32, // Offset(90),    Root Port address 22
Offset(94),     RPAN, 32, // Offset(94),    Root Port address 23
Offset(98),     RPAO, 32, // Offset(98),    Root Port address 24
Offset(102),    NHLA, 64, // Offset(102),   HD-Audio NHLT ACPI address
Offset(110),    NHLL, 32, // Offset(110),   HD-Audio NHLT ACPI length
Offset(114),    ADFM, 32, // Offset(114),   HD-Audio DSP Feature Mask
Offset(118),    SWQ0, 8,  // Offset(118),   HD-Audio SoundWire Link #1 quirk mask
Offset(119),    SWQ1, 8,  // Offset(119),   HD-Audio SoundWire Link #2 quirk mask
Offset(120),    SWQ2, 8,  // Offset(120),   HD-Audio SoundWire Link #3 quirk mask
Offset(121),    SWQ3, 8,  // Offset(121),   HD-Audio SoundWire Link #4 quirk mask
Offset(122),    ACS0, 8,  // Offset(122),   HD-Audio SoundWire Link #1 Autonomous Clock Stop
Offset(123),    ACS1, 8,  // Offset(123),   HD-Audio SoundWire Link #2 Autonomous Clock Stop
Offset(124),    ACS2, 8,  // Offset(124),   HD-Audio SoundWire Link #3 Autonomous Clock Stop
Offset(125),    ACS3, 8,  // Offset(125),   HD-Audio SoundWire Link #4 Autonomous Clock Stop
Offset(126),    DAI0, 8,  // Offset(126),   HD-Audio SoundWire Link #1 Data On Active Interval Select
Offset(127),    DAI1, 8,  // Offset(127),   HD-Audio SoundWire Link #2 Data On Active Interval Select
Offset(128),    DAI2, 8,  // Offset(128),   HD-Audio SoundWire Link #3 Data On Active Interval Select
Offset(129),    DAI3, 8,  // Offset(129),   HD-Audio SoundWire Link #4 Data On Active Interval Select
Offset(130),    DOD0, 8,  // Offset(130),   HD-Audio SoundWire Link #1 Data On Delay Select
Offset(131),    DOD1, 8,  // Offset(131),   HD-Audio SoundWire Link #2 Data On Delay Select
Offset(132),    DOD2, 8,  // Offset(132),   HD-Audio SoundWire Link #3 Data On Delay Select
Offset(133),    DOD3, 8,  // Offset(133),   HD-Audio SoundWire Link #4 Data On Delay Select
Offset(134),    XTAL, 8,  // Offset(134),   XTAL frequency: 0: 24MHz; 1: 38.4MHz; 2: Unsupported
Offset(135),    DSPM, 32, // Offset(135),   HD-Audio DSP Stolen Memory Base Address (@todo: Remove after CNL-LP B0)
Offset(139),    SBRG, 32, // Offset(139),   SBREG_BAR
Offset(143),    GEI0, 8,  // Offset(143),   GPIO GroupIndex mapped to GPE_DW0
Offset(144),    GEI1, 8,  // Offset(144),   GPIO GroupIndex mapped to GPE_DW1
Offset(145),    GEI2, 8,  // Offset(145),   GPIO GroupIndex mapped to GPE_DW2
Offset(146),    GED0, 8,  // Offset(146),   GPIO DW part of group mapped to GPE_DW0
Offset(147),    GED1, 8,  // Offset(147),   GPIO DW part of group mapped to GPE_DW1
Offset(148),    GED2, 8,  // Offset(148),   GPIO DW part of group mapped to GPE_DW2
Offset(149),    PML1, 16, // Offset(149),   PCIE LTR max snoop Latency 1
Offset(151),    PML2, 16, // Offset(151),   PCIE LTR max snoop Latency 2
Offset(153),    PML3, 16, // Offset(153),   PCIE LTR max snoop Latency 3
Offset(155),    PML4, 16, // Offset(155),   PCIE LTR max snoop Latency 4
Offset(157),    PML5, 16, // Offset(157),   PCIE LTR max snoop Latency 5
Offset(159),    PML6, 16, // Offset(159),   PCIE LTR max snoop Latency 6
Offset(161),    PML7, 16, // Offset(161),   PCIE LTR max snoop Latency 7
Offset(163),    PML8, 16, // Offset(163),   PCIE LTR max snoop Latency 8
Offset(165),    PML9, 16, // Offset(165),   PCIE LTR max snoop Latency 9
Offset(167),    PMLA, 16, // Offset(167),   PCIE LTR max snoop Latency 10
Offset(169),    PMLB, 16, // Offset(169),   PCIE LTR max snoop Latency 11
Offset(171),    PMLC, 16, // Offset(171),   PCIE LTR max snoop Latency 12
Offset(173),    PMLD, 16, // Offset(173),   PCIE LTR max snoop Latency 13
Offset(175),    PMLE, 16, // Offset(175),   PCIE LTR max snoop Latency 14
Offset(177),    PMLF, 16, // Offset(177),   PCIE LTR max snoop Latency 15
Offset(179),    PMLG, 16, // Offset(179),   PCIE LTR max snoop Latency 16
Offset(181),    PMLH, 16, // Offset(181),   PCIE LTR max snoop Latency 17
Offset(183),    PMLI, 16, // Offset(183),   PCIE LTR max snoop Latency 18
Offset(185),    PMLJ, 16, // Offset(185),   PCIE LTR max snoop Latency 19
Offset(187),    PMLK, 16, // Offset(187),   PCIE LTR max snoop Latency 20
Offset(189),    PMLL, 16, // Offset(189),   PCIE LTR max snoop Latency 21
Offset(191),    PMLM, 16, // Offset(191),   PCIE LTR max snoop Latency 22
Offset(193),    PMLN, 16, // Offset(193),   PCIE LTR max snoop Latency 23
Offset(195),    PMLO, 16, // Offset(195),   PCIE LTR max snoop Latency 24
Offset(197),    PNL1, 16, // Offset(197),   PCIE LTR max no snoop Latency 1
Offset(199),    PNL2, 16, // Offset(199),   PCIE LTR max no snoop Latency 2
Offset(201),    PNL3, 16, // Offset(201),   PCIE LTR max no snoop Latency 3
Offset(203),    PNL4, 16, // Offset(203),   PCIE LTR max no snoop Latency 4
Offset(205),    PNL5, 16, // Offset(205),   PCIE LTR max no snoop Latency 5
Offset(207),    PNL6, 16, // Offset(207),   PCIE LTR max no snoop Latency 6
Offset(209),    PNL7, 16, // Offset(209),   PCIE LTR max no snoop Latency 7
Offset(211),    PNL8, 16, // Offset(211),   PCIE LTR max no snoop Latency 8
Offset(213),    PNL9, 16, // Offset(213),   PCIE LTR max no snoop Latency 9
Offset(215),    PNLA, 16, // Offset(215),   PCIE LTR max no snoop Latency 10
Offset(217),    PNLB, 16, // Offset(217),   PCIE LTR max no snoop Latency 11
Offset(219),    PNLC, 16, // Offset(219),   PCIE LTR max no snoop Latency 12
Offset(221),    PNLD, 16, // Offset(221),   PCIE LTR max no snoop Latency 13
Offset(223),    PNLE, 16, // Offset(223),   PCIE LTR max no snoop Latency 14
Offset(225),    PNLF, 16, // Offset(225),   PCIE LTR max no snoop Latency 15
Offset(227),    PNLG, 16, // Offset(227),   PCIE LTR max no snoop Latency 16
Offset(229),    PNLH, 16, // Offset(229),   PCIE LTR max no snoop Latency 17
Offset(231),    PNLI, 16, // Offset(231),   PCIE LTR max no snoop Latency 18
Offset(233),    PNLJ, 16, // Offset(233),   PCIE LTR max no snoop Latency 19
Offset(235),    PNLK, 16, // Offset(235),   PCIE LTR max no snoop Latency 20
Offset(237),    PNLL, 16, // Offset(237),   PCIE LTR max no snoop Latency 21
Offset(239),    PNLM, 16, // Offset(239),   PCIE LTR max no snoop Latency 22
Offset(241),    PNLN, 16, // Offset(241),   PCIE LTR max no snoop Latency 23
Offset(243),    PNLO, 16, // Offset(243),   PCIE LTR max no snoop Latency 24
Offset(245),    XHPC, 8,  // Offset(245),   Number of HighSpeed ports implemented in XHCI controller
Offset(246),    XRPC, 8,  // Offset(246),   Number of USBR ports implemented in XHCI controller
Offset(247),    XSPC, 8,  // Offset(247),   Number of SuperSpeed ports implemented in XHCI controller
Offset(248),    XSPA, 8,  // Offset(248),   Address of 1st SuperSpeed port
Offset(249),    HPTB, 32, // Offset(249),   HPET base address
Offset(253),    HPTE, 8,  // Offset(253),   HPET enable
//SerialIo block
Offset(254),    SM00, 8,  // Offset(254),   SerialIo SPI Controller 0 Mode
Offset(255),    SM01, 8,  // Offset(255),   SerialIo SPI Controller 1 Mode
Offset(256),    SM02, 8,  // Offset(256),   SerialIo SPI Controller 2 Mode
Offset(257),    SC00, 64, // Offset(257),   SerialIo SPI Controller 0 Pci Config
Offset(265),    SC01, 64, // Offset(265),   SerialIo SPI Controller 1 Pci Config
Offset(273),    SC02, 64, // Offset(273),   SerialIo SPI Controller 2 Pci Config
Offset(281),    IM00, 8,  // Offset(281),   SerialIo I2C Controller 0 Mode
Offset(282),    IM01, 8,  // Offset(282),   SerialIo I2C Controller 1 Mode
Offset(283),    IM02, 8,  // Offset(283),   SerialIo I2C Controller 2 Mode
Offset(284),    IM03, 8,  // Offset(284),   SerialIo I2C Controller 3 Mode
Offset(285),    IM04, 8,  // Offset(285),   SerialIo I2C Controller 4 Mode
Offset(286),    IM05, 8,  // Offset(286),   SerialIo I2C Controller 5 Mode
Offset(287),    IC00, 64, // Offset(287),   SerialIo I2C Controller 0 Pci Config
Offset(295),    IC01, 64, // Offset(295),   SerialIo I2C Controller 1 Pci Config
Offset(303),    IC02, 64, // Offset(303),   SerialIo I2C Controller 2 Pci Config
Offset(311),    IC03, 64, // Offset(311),   SerialIo I2C Controller 3 Pci Config
Offset(319),    IC04, 64, // Offset(319),   SerialIo I2C Controller 4 Pci Config
Offset(327),    IC05, 64, // Offset(327),   SerialIo I2C Controller 5 Pci Config
Offset(335),    UM00, 8,  // Offset(335),   SerialIo UART Controller 0 Mode
Offset(336),    UM01, 8,  // Offset(336),   SerialIo UART Controller 1 Mode
Offset(337),    UM02, 8,  // Offset(337),   SerialIo UART Controller 2 Mode
  Offset(338),    UP00, 16, // Offset(338),   SerialIo UART Controller 0 IO Port
  Offset(340),    UP01, 16, // Offset(340),   SerialIo UART Controller 1 IO Port
  Offset(342),    UP02, 16, // Offset(342),   SerialIo UART Controller 2 IO Port
  Offset(344),    UI00, 8,  // Offset(344),   SerialIo UART Controller 0 Int
  Offset(345),    UI01, 8,  // Offset(345),   SerialIo UART Controller 1 Int
  Offset(346),    UI02, 8,  // Offset(346),   SerialIo UART Controller 2 Int
//end of SerialIo block
  Offset(347),    SGIR, 8,  // Offset(347),   GPIO IRQ
  Offset(348),    GPHD, 8,  // Offset(348),   Hide GPIO ACPI device
  Offset(349),    NIT1, 8,  // Offset(349),   RST PCIe Storage Cycle Router#1 Interface Type
  Offset(350),    NIT2, 8,  // Offset(350),   RST PCIe Storage Cycle Router#2 Interface Type
  Offset(351),    NIT3, 8,  // Offset(351),   RST PCIe Storage Cycle Router#3 Interface Type
  Offset(352),    NPM1, 8,  // Offset(352),   RST PCIe Storage Cycle Router#1 Power Management Capability Pointer
  Offset(353),    NPM2, 8,  // Offset(353),   RST PCIe Storage Cycle Router#2 Power Management Capability Pointer
  Offset(354),    NPM3, 8,  // Offset(354),   RST PCIe Storage Cycle Router#3 Power Management Capability Pointer
  Offset(355),    NPC1, 8,  // Offset(355),   RST PCIe Storage Cycle Router#1 PCIe Capabilities Pointer
  Offset(356),    NPC2, 8,  // Offset(356),   RST PCIe Storage Cycle Router#2 PCIe Capabilities Pointer
  Offset(357),    NPC3, 8,  // Offset(357),   RST PCIe Storage Cycle Router#3 PCIe Capabilities Pointer
  Offset(358),    NL11, 16, // Offset(358),   RST PCIe Storage Cycle Router#1 L1SS Capability Pointer
  Offset(360),    NL12, 16, // Offset(360),   RST PCIe Storage Cycle Router#2 L1SS Capability Pointer
  Offset(362),    NL13, 16, // Offset(362),   RST PCIe Storage Cycle Router#3 L1SS Capability Pointer
  Offset(364),    ND21, 8,  // Offset(364),   RST PCIe Storage Cycle Router#1 Endpoint L1SS Control Data2
  Offset(365),    ND22, 8,  // Offset(365),   RST PCIe Storage Cycle Router#2 Endpoint L1SS Control Data2
  Offset(366),    ND23, 8,  // Offset(366),   RST PCIe Storage Cycle Router#3 Endpoint L1SS Control Data2
  Offset(367),    ND11, 32, // Offset(367),   RST PCIe Storage Cycle Router#1 Endpoint L1SS Control Data1
  Offset(371),    ND12, 32, // Offset(371),   RST PCIe Storage Cycle Router#2 Endpoint L1SS Control Data1
  Offset(375),    ND13, 32, // Offset(375),   RST PCIe Storage Cycle Router#3 Endpoint L1SS Control Data1
  Offset(379),    NLR1, 16, // Offset(379),   RST PCIe Storage Cycle Router#1 LTR Capability Pointer
  Offset(381),    NLR2, 16, // Offset(381),   RST PCIe Storage Cycle Router#2 LTR Capability Pointer
  Offset(383),    NLR3, 16, // Offset(383),   RST PCIe Storage Cycle Router#3 LTR Capability Pointer
  Offset(385),    NLD1, 32, // Offset(385),   RST PCIe Storage Cycle Router#1 Endpoint LTR Data
  Offset(389),    NLD2, 32, // Offset(389),   RST PCIe Storage Cycle Router#2 Endpoint LTR Data
  Offset(393),    NLD3, 32, // Offset(393),   RST PCIe Storage Cycle Router#3 Endpoint LTR Data
  Offset(397),    NEA1, 16, // Offset(397),   RST PCIe Storage Cycle Router#1 Endpoint LCTL Data
  Offset(399),    NEA2, 16, // Offset(399),   RST PCIe Storage Cycle Router#2 Endpoint LCTL Data
  Offset(401),    NEA3, 16, // Offset(401),   RST PCIe Storage Cycle Router#3 Endpoint LCTL Data
  Offset(403),    NEB1, 16, // Offset(403),   RST PCIe Storage Cycle Router#1 Endpoint DCTL Data
  Offset(405),    NEB2, 16, // Offset(405),   RST PCIe Storage Cycle Router#2 Endpoint DCTL Data
  Offset(407),    NEB3, 16, // Offset(407),   RST PCIe Storage Cycle Router#3 Endpoint DCTL Data
  Offset(409),    NEC1, 16, // Offset(409),   RST PCIe Storage Cycle Router#1 Endpoint DCTL2 Data
  Offset(411),    NEC2, 16, // Offset(411),   RST PCIe Storage Cycle Router#2 Endpoint DCTL2 Data
  Offset(413),    NEC3, 16, // Offset(413),   RST PCIe Storage Cycle Router#3 Endpoint DCTL2 Data
  Offset(415),    NRA1, 16, // Offset(415),   RST PCIe Storage Cycle Router#1 RootPort DCTL2 Data
  Offset(417),    NRA2, 16, // Offset(417),   RST PCIe Storage Cycle Router#2 RootPort DCTL2 Data
  Offset(419),    NRA3, 16, // Offset(419),   RST PCIe Storage Cycle Router#3 RootPort DCTL2 Data
  Offset(421),    NMB1, 32, // Offset(421),   RST PCIe Storage Cycle Router#1 Endpoint unique MSI-X Table BAR
  Offset(425),    NMB2, 32, // Offset(425),   RST PCIe Storage Cycle Router#2 Endpoint unique MSI-X Table BAR
  Offset(429),    NMB3, 32, // Offset(429),   RST PCIe Storage Cycle Router#3 Endpoint unique MSI-X Table BAR
  Offset(433),    NMV1, 32, // Offset(433),   RST PCIe Storage Cycle Router#1 Endpoint unique MSI-X Table BAR value
  Offset(437),    NMV2, 32, // Offset(437),   RST PCIe Storage Cycle Router#2 Endpoint unique MSI-X Table BAR value
  Offset(441),    NMV3, 32, // Offset(441),   RST PCIe Storage Cycle Router#3 Endpoint unique MSI-X Table BAR value
  Offset(445),    NPB1, 32, // Offset(445),   RST PCIe Storage Cycle Router#1 Endpoint unique MSI-X PBA BAR
  Offset(449),    NPB2, 32, // Offset(449),   RST PCIe Storage Cycle Router#2 Endpoint unique MSI-X PBA BAR
  Offset(453),    NPB3, 32, // Offset(453),   RST PCIe Storage Cycle Router#3 Endpoint unique MSI-X PBA BAR
  Offset(457),    NPV1, 32, // Offset(457),   RST PCIe Storage Cycle Router#1 Endpoint unique MSI-X PBA BAR value
  Offset(461),    NPV2, 32, // Offset(461),   RST PCIe Storage Cycle Router#2 Endpoint unique MSI-X PBA BAR value
  Offset(465),    NPV3, 32, // Offset(465),   RST PCIe Storage Cycle Router#3 Endpoint unique MSI-X PBA BAR value
  Offset(469),    NRP1, 32, // Offset(469),   RST PCIe Storage Cycle Router#1 Root Port number
  Offset(473),    NRP2, 32, // Offset(473),   RST PCIe Storage Cycle Router#2 Root Port number
  Offset(477),    NRP3, 32, // Offset(477),   RST PCIe Storage Cycle Router#3 Root Port number
  Offset(481),    EMH4, 8,  // Offset(481),   eMMC HS400 mode enabled
  Offset(482),    EMDS, 8,  // Offset(482),   eMMC Driver Strength
  Offset(483),    CSKU, 8,  // Offset(483),   CPU SKU
  Offset(484),    ITA0, 16, // Offset(484),
  Offset(486),    ITA1, 16, // Offset(486),
  Offset(488),    ITA2, 16, // Offset(488),
  Offset(490),    ITA3, 16, // Offset(490),
  Offset(492),    ITS0, 8,  // Offset(492),
  Offset(493),    ITS1, 8,  // Offset(493),
  Offset(494),    ITS2, 8,  // Offset(494),
  Offset(495),    ITS3, 8,  // Offset(495),
  Offset(496),    PMBS, 16, // Offset(496),   ACPI IO BASE address
  Offset(498),    PWRM, 32, // Offset(498),   PWRM MEM BASE address
// CNVi specific
  Offset(502),    CNVI, 8,  // Offset(502),   CNVi mode
  Offset(503),    CBTC, 8,  // Offset(503),   CNVi BT Core
  Offset(504),    CBTA, 8,  // Offset(504),   CNVi BT Audio Offload
//Voltage Margining
  Offset(505),    S0I3, 8,  // Offset(505),   Dynamic S0i3 Support
// PCH Trace Hub
  Offset(506),    PTHM, 8,  // Offset(506),   PCH Trace Hub Mode
// PCH PS_ON support
  Offset(507),    PSON, 8,  // Offset(507),   PCH PS_ON enable
//
// These are for PchApciTablesSelfTest use
//
  Offset(508),    LTR1, 8,  // Offset(508),   Latency Tolerance Reporting Enable
  Offset(509),    LTR2, 8,  // Offset(509),   Latency Tolerance Reporting Enable
  Offset(510),    LTR3, 8,  // Offset(510),   Latency Tolerance Reporting Enable
  Offset(511),    LTR4, 8,  // Offset(511),   Latency Tolerance Reporting Enable
  Offset(512),    LTR5, 8,  // Offset(512),   Latency Tolerance Reporting Enable
  Offset(513),    LTR6, 8,  // Offset(513),   Latency Tolerance Reporting Enable
  Offset(514),    LTR7, 8,  // Offset(514),   Latency Tolerance Reporting Enable
  Offset(515),    LTR8, 8,  // Offset(515),   Latency Tolerance Reporting Enable
  Offset(516),    LTR9, 8,  // Offset(516),   Latency Tolerance Reporting Enable
  Offset(517),    LTRA, 8,  // Offset(517),   Latency Tolerance Reporting Enable
  Offset(518),    LTRB, 8,  // Offset(518),   Latency Tolerance Reporting Enable
  Offset(519),    LTRC, 8,  // Offset(519),   Latency Tolerance Reporting Enable
  Offset(520),    LTRD, 8,  // Offset(520),   Latency Tolerance Reporting Enable
  Offset(521),    LTRE, 8,  // Offset(521),   Latency Tolerance Reporting Enable
  Offset(522),    LTRF, 8,  // Offset(522),   Latency Tolerance Reporting Enable
  Offset(523),    LTRG, 8,  // Offset(523),   Latency Tolerance Reporting Enable
  Offset(524),    LTRH, 8,  // Offset(524),   Latency Tolerance Reporting Enable
  Offset(525),    LTRI, 8,  // Offset(525),   Latency Tolerance Reporting Enable
  Offset(526),    LTRJ, 8,  // Offset(526),   Latency Tolerance Reporting Enable
  Offset(527),    LTRK, 8,  // Offset(527),   Latency Tolerance Reporting Enable
  Offset(528),    LTRL, 8,  // Offset(528),   Latency Tolerance Reporting Enable
  Offset(529),    LTRM, 8,  // Offset(529),   Latency Tolerance Reporting Enable
  Offset(530),    LTRN, 8,  // Offset(530),   Latency Tolerance Reporting Enable
  Offset(531),    LTRO, 8,  // Offset(531),   Latency Tolerance Reporting Enable
  Offset(532),    GBES, 8,  // Offset(532),   GbE Support
  Offset(533),    PPDS, 32, // Offset(533),   PCH xDCI Power Down Scale Value, DWC_USB3_GCTL_INIT[31:19]
  Offset(537),    SDPH, 8,  // Offset(537),   SD_PWREN# active high indication
  Offset(538),    EMCE, 8,  // Offset(538),   Set to indicate that eMMC is enabled
  Offset(539),    SDCE, 8,  // Offset(539),   Set to indicate that SD card is enabled
  Offset(540),    PIRA, 8,  // Offset(540),   PIRQA -> IRQx mapping
  Offset(541),    PIRB, 8,  // Offset(541),   PIRQB -> IRQx mapping
  Offset(542),    PIRC, 8,  // Offset(542),   PIRQC -> IRQx mapping
  Offset(543),    PIRD, 8,  // Offset(543),   PIRQD -> IRQx mapping
  Offset(544),    PIRE, 8,  // Offset(544),   PIRQE -> IRQx mapping
  Offset(545),    PIRF, 8,  // Offset(545),   PIRQF -> IRQx mapping
  Offset(546),    PIRG, 8,  // Offset(546),   PIRQG -> IRQx mapping
  Offset(547),    PIRH, 8,  // Offset(547),   PIRQH -> IRQx mapping
  Offset(548),    MCFG, 32, // Offset(548),   PCI Express BAR (PcieMmCfgBaseAddress)
}
