/**@file

  Copyright (c) 2018 - 2019, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

  //
  // Define PCH NVS Area operatino region.
  //



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
  Offset(122),    DSPM, 32, // Offset(122),   HD-Audio DSP Stolen Memory Base Address (@todo: Remove after CNL-LP B0)
  Offset(126),    SBRG, 32, // Offset(126),   SBREG_BAR
  Offset(130),    GEI0, 8,  // Offset(130),   GPIO GroupIndex mapped to GPE_DW0
  Offset(131),    GEI1, 8,  // Offset(131),   GPIO GroupIndex mapped to GPE_DW1
  Offset(132),    GEI2, 8,  // Offset(132),   GPIO GroupIndex mapped to GPE_DW2
  Offset(133),    GED0, 8,  // Offset(133),   GPIO DW part of group mapped to GPE_DW0
  Offset(134),    GED1, 8,  // Offset(134),   GPIO DW part of group mapped to GPE_DW1
  Offset(135),    GED2, 8,  // Offset(135),   GPIO DW part of group mapped to GPE_DW2
  Offset(136),    PML1, 16, // Offset(136),   PCIE LTR max snoop Latency 1
  Offset(138),    PML2, 16, // Offset(138),   PCIE LTR max snoop Latency 2
  Offset(140),    PML3, 16, // Offset(140),   PCIE LTR max snoop Latency 3
  Offset(142),    PML4, 16, // Offset(142),   PCIE LTR max snoop Latency 4
  Offset(144),    PML5, 16, // Offset(144),   PCIE LTR max snoop Latency 5
  Offset(146),    PML6, 16, // Offset(146),   PCIE LTR max snoop Latency 6
  Offset(148),    PML7, 16, // Offset(148),   PCIE LTR max snoop Latency 7
  Offset(150),    PML8, 16, // Offset(150),   PCIE LTR max snoop Latency 8
  Offset(152),    PML9, 16, // Offset(152),   PCIE LTR max snoop Latency 9
  Offset(154),    PMLA, 16, // Offset(154),   PCIE LTR max snoop Latency 10
  Offset(156),    PMLB, 16, // Offset(156),   PCIE LTR max snoop Latency 11
  Offset(158),    PMLC, 16, // Offset(158),   PCIE LTR max snoop Latency 12
  Offset(160),    PMLD, 16, // Offset(160),   PCIE LTR max snoop Latency 13
  Offset(162),    PMLE, 16, // Offset(162),   PCIE LTR max snoop Latency 14
  Offset(164),    PMLF, 16, // Offset(164),   PCIE LTR max snoop Latency 15
  Offset(166),    PMLG, 16, // Offset(166),   PCIE LTR max snoop Latency 16
  Offset(168),    PMLH, 16, // Offset(168),   PCIE LTR max snoop Latency 17
  Offset(170),    PMLI, 16, // Offset(170),   PCIE LTR max snoop Latency 18
  Offset(172),    PMLJ, 16, // Offset(172),   PCIE LTR max snoop Latency 19
  Offset(174),    PMLK, 16, // Offset(174),   PCIE LTR max snoop Latency 20
  Offset(176),    PMLL, 16, // Offset(176),   PCIE LTR max snoop Latency 21
  Offset(178),    PMLM, 16, // Offset(178),   PCIE LTR max snoop Latency 22
  Offset(180),    PMLN, 16, // Offset(180),   PCIE LTR max snoop Latency 23
  Offset(182),    PMLO, 16, // Offset(182),   PCIE LTR max snoop Latency 24
  Offset(184),    PNL1, 16, // Offset(184),   PCIE LTR max no snoop Latency 1
  Offset(186),    PNL2, 16, // Offset(186),   PCIE LTR max no snoop Latency 2
  Offset(188),    PNL3, 16, // Offset(188),   PCIE LTR max no snoop Latency 3
  Offset(190),    PNL4, 16, // Offset(190),   PCIE LTR max no snoop Latency 4
  Offset(192),    PNL5, 16, // Offset(192),   PCIE LTR max no snoop Latency 5
  Offset(194),    PNL6, 16, // Offset(194),   PCIE LTR max no snoop Latency 6
  Offset(196),    PNL7, 16, // Offset(196),   PCIE LTR max no snoop Latency 7
  Offset(198),    PNL8, 16, // Offset(198),   PCIE LTR max no snoop Latency 8
  Offset(200),    PNL9, 16, // Offset(200),   PCIE LTR max no snoop Latency 9
  Offset(202),    PNLA, 16, // Offset(202),   PCIE LTR max no snoop Latency 10
  Offset(204),    PNLB, 16, // Offset(204),   PCIE LTR max no snoop Latency 11
  Offset(206),    PNLC, 16, // Offset(206),   PCIE LTR max no snoop Latency 12
  Offset(208),    PNLD, 16, // Offset(208),   PCIE LTR max no snoop Latency 13
  Offset(210),    PNLE, 16, // Offset(210),   PCIE LTR max no snoop Latency 14
  Offset(212),    PNLF, 16, // Offset(212),   PCIE LTR max no snoop Latency 15
  Offset(214),    PNLG, 16, // Offset(214),   PCIE LTR max no snoop Latency 16
  Offset(216),    PNLH, 16, // Offset(216),   PCIE LTR max no snoop Latency 17
  Offset(218),    PNLI, 16, // Offset(218),   PCIE LTR max no snoop Latency 18
  Offset(220),    PNLJ, 16, // Offset(220),   PCIE LTR max no snoop Latency 19
  Offset(222),    PNLK, 16, // Offset(222),   PCIE LTR max no snoop Latency 20
  Offset(224),    PNLL, 16, // Offset(224),   PCIE LTR max no snoop Latency 21
  Offset(226),    PNLM, 16, // Offset(226),   PCIE LTR max no snoop Latency 22
  Offset(228),    PNLN, 16, // Offset(228),   PCIE LTR max no snoop Latency 23
  Offset(230),    PNLO, 16, // Offset(230),   PCIE LTR max no snoop Latency 24
  Offset(232),    XHPC, 8,  // Offset(232),   Number of HighSpeed ports implemented in XHCI controller
  Offset(233),    XRPC, 8,  // Offset(233),   Number of USBR ports implemented in XHCI controller
  Offset(234),    XSPC, 8,  // Offset(234),   Number of SuperSpeed ports implemented in XHCI controller
  Offset(235),    XSPA, 8,  // Offset(235),   Address of 1st SuperSpeed port
  Offset(236),    HPTB, 32, // Offset(236),   HPET base address
  Offset(240),    HPTE, 8,  // Offset(240),   HPET enable
  //SerialIo block
  Offset(241),    SMD0, 8,  // Offset(241),   SerialIo controller 0 mode
  Offset(242),    SMD1, 8,  // Offset(242),   SerialIo controller 1 mode
  Offset(243),    SMD2, 8,  // Offset(243),   SerialIo controller 2 mode
  Offset(244),    SMD3, 8,  // Offset(244),   SerialIo controller 3 mode
  Offset(245),    SMD4, 8,  // Offset(245),   SerialIo controller 4 mode
  Offset(246),    SMD5, 8,  // Offset(246),   SerialIo controller 5 mode
  Offset(247),    SMD6, 8,  // Offset(247),   SerialIo controller 6 mode
  Offset(248),    SMD7, 8,  // Offset(248),   SerialIo controller 7 mode
  Offset(249),    SMD8, 8,  // Offset(249),   SerialIo controller 8 mode
  Offset(250),    SMD9, 8,  // Offset(250),   SerialIo controller 9 mode
  Offset(251),    SMDA, 8,  // Offset(251),   SerialIo controller A mode
  Offset(252),    SMDB, 8,  // Offset(252),   SerialIo controller B mode
  Offset(253),    SIR0, 8,  // Offset(253),   SerialIo controller 0 irq number
  Offset(254),    SIR1, 8,  // Offset(254),   SerialIo controller 1 irq number
  Offset(255),    SIR2, 8,  // Offset(255),   SerialIo controller 2 irq number
  Offset(256),    SIR3, 8,  // Offset(256),   SerialIo controller 3 irq number
  Offset(257),    SIR4, 8,  // Offset(257),   SerialIo controller 4 irq number
  Offset(258),    SIR5, 8,  // Offset(258),   SerialIo controller 5 irq number
  Offset(259),    SIR6, 8,  // Offset(259),   SerialIo controller 6 irq number
  Offset(260),    SIR7, 8,  // Offset(260),   SerialIo controller 7 irq number
  Offset(261),    SIR8, 8,  // Offset(261),   SerialIo controller 8 irq number
  Offset(262),    SIR9, 8,  // Offset(262),   SerialIo controller 9 irq number
  Offset(263),    SIRA, 8,  // Offset(263),   SerialIo controller A irq number
  Offset(264),    SIRB, 8,  // Offset(264),   SerialIo controller B irq number
  Offset(265),    SB00, 64, // Offset(265),   SerialIo controller 0 BAR0
  Offset(273),    SB01, 64, // Offset(273),   SerialIo controller 1 BAR0
  Offset(281),    SB02, 64, // Offset(281),   SerialIo controller 2 BAR0
  Offset(289),    SB03, 64, // Offset(289),   SerialIo controller 3 BAR0
  Offset(297),    SB04, 64, // Offset(297),   SerialIo controller 4 BAR0
  Offset(305),    SB05, 64, // Offset(305),   SerialIo controller 5 BAR0
  Offset(313),    SB06, 64, // Offset(313),   SerialIo controller 6 BAR0
  Offset(321),    SB07, 64, // Offset(321),   SerialIo controller 7 BAR0
  Offset(329),    SB08, 64, // Offset(329),   SerialIo controller 8 BAR0
  Offset(337),    SB09, 64, // Offset(337),   SerialIo controller 9 BAR0
  Offset(345),    SB0A, 64, // Offset(345),   SerialIo controller A BAR0
  Offset(353),    SB0B, 64, // Offset(353),   SerialIo controller B BAR0
  Offset(361),    SB10, 64, // Offset(361),   SerialIo controller 0 BAR1
  Offset(369),    SB11, 64, // Offset(369),   SerialIo controller 1 BAR1
  Offset(377),    SB12, 64, // Offset(377),   SerialIo controller 2 BAR1
  Offset(385),    SB13, 64, // Offset(385),   SerialIo controller 3 BAR1
  Offset(393),    SB14, 64, // Offset(393),   SerialIo controller 4 BAR1
  Offset(401),    SB15, 64, // Offset(401),   SerialIo controller 5 BAR1
  Offset(409),    SB16, 64, // Offset(409),   SerialIo controller 6 BAR1
  Offset(417),    SB17, 64, // Offset(417),   SerialIo controller 7 BAR1
  Offset(425),    SB18, 64, // Offset(425),   SerialIo controller 8 BAR1
  Offset(433),    SB19, 64, // Offset(433),   SerialIo controller 9 BAR1
  Offset(441),    SB1A, 64, // Offset(441),   SerialIo controller A BAR1
  Offset(449),    SB1B, 64, // Offset(449),   SerialIo controller B BAR1
  //end of SerialIo block
  Offset(457),    SGIR, 8,  // Offset(457),   GPIO IRQ
  Offset(458),    GPHD, 8,  // Offset(458),   Hide GPIO ACPI device
  Offset(459),    NIT1, 8,  // Offset(459),   RST PCIe Storage Cycle Router#1 Interface Type
  Offset(460),    NIT2, 8,  // Offset(460),   RST PCIe Storage Cycle Router#2 Interface Type
  Offset(461),    NIT3, 8,  // Offset(461),   RST PCIe Storage Cycle Router#3 Interface Type
  Offset(462),    NPM1, 8,  // Offset(462),   RST PCIe Storage Cycle Router#1 Power Management Capability Pointer
  Offset(463),    NPM2, 8,  // Offset(463),   RST PCIe Storage Cycle Router#2 Power Management Capability Pointer
  Offset(464),    NPM3, 8,  // Offset(464),   RST PCIe Storage Cycle Router#3 Power Management Capability Pointer
  Offset(465),    NPC1, 8,  // Offset(465),   RST PCIe Storage Cycle Router#1 PCIe Capabilities Pointer
  Offset(466),    NPC2, 8,  // Offset(466),   RST PCIe Storage Cycle Router#2 PCIe Capabilities Pointer
  Offset(467),    NPC3, 8,  // Offset(467),   RST PCIe Storage Cycle Router#3 PCIe Capabilities Pointer
  Offset(468),    NL11, 16, // Offset(468),   RST PCIe Storage Cycle Router#1 L1SS Capability Pointer
  Offset(470),    NL12, 16, // Offset(470),   RST PCIe Storage Cycle Router#2 L1SS Capability Pointer
  Offset(472),    NL13, 16, // Offset(472),   RST PCIe Storage Cycle Router#3 L1SS Capability Pointer
  Offset(474),    ND21, 8,  // Offset(474),   RST PCIe Storage Cycle Router#1 Endpoint L1SS Control Data2
  Offset(475),    ND22, 8,  // Offset(475),   RST PCIe Storage Cycle Router#2 Endpoint L1SS Control Data2
  Offset(476),    ND23, 8,  // Offset(476),   RST PCIe Storage Cycle Router#3 Endpoint L1SS Control Data2
  Offset(477),    ND11, 32, // Offset(477),   RST PCIe Storage Cycle Router#1 Endpoint L1SS Control Data1
  Offset(481),    ND12, 32, // Offset(481),   RST PCIe Storage Cycle Router#2 Endpoint L1SS Control Data1
  Offset(485),    ND13, 32, // Offset(485),   RST PCIe Storage Cycle Router#3 Endpoint L1SS Control Data1
  Offset(489),    NLR1, 16, // Offset(489),   RST PCIe Storage Cycle Router#1 LTR Capability Pointer
  Offset(491),    NLR2, 16, // Offset(491),   RST PCIe Storage Cycle Router#2 LTR Capability Pointer
  Offset(493),    NLR3, 16, // Offset(493),   RST PCIe Storage Cycle Router#3 LTR Capability Pointer
  Offset(495),    NLD1, 32, // Offset(495),   RST PCIe Storage Cycle Router#1 Endpoint LTR Data
  Offset(499),    NLD2, 32, // Offset(499),   RST PCIe Storage Cycle Router#2 Endpoint LTR Data
  Offset(503),    NLD3, 32, // Offset(503),   RST PCIe Storage Cycle Router#3 Endpoint LTR Data
  Offset(507),    NEA1, 16, // Offset(507),   RST PCIe Storage Cycle Router#1 Endpoint LCTL Data
  Offset(509),    NEA2, 16, // Offset(509),   RST PCIe Storage Cycle Router#2 Endpoint LCTL Data
  Offset(511),    NEA3, 16, // Offset(511),   RST PCIe Storage Cycle Router#3 Endpoint LCTL Data
  Offset(513),    NEB1, 16, // Offset(513),   RST PCIe Storage Cycle Router#1 Endpoint DCTL Data
  Offset(515),    NEB2, 16, // Offset(515),   RST PCIe Storage Cycle Router#2 Endpoint DCTL Data
  Offset(517),    NEB3, 16, // Offset(517),   RST PCIe Storage Cycle Router#3 Endpoint DCTL Data
  Offset(519),    NEC1, 16, // Offset(519),   RST PCIe Storage Cycle Router#1 Endpoint DCTL2 Data
  Offset(521),    NEC2, 16, // Offset(521),   RST PCIe Storage Cycle Router#2 Endpoint DCTL2 Data
  Offset(523),    NEC3, 16, // Offset(523),   RST PCIe Storage Cycle Router#3 Endpoint DCTL2 Data
  Offset(525),    NRA1, 16, // Offset(525),   RST PCIe Storage Cycle Router#1 RootPort DCTL2 Data
  Offset(527),    NRA2, 16, // Offset(527),   RST PCIe Storage Cycle Router#2 RootPort DCTL2 Data
  Offset(529),    NRA3, 16, // Offset(529),   RST PCIe Storage Cycle Router#3 RootPort DCTL2 Data
  Offset(531),    NMB1, 32, // Offset(531),   RST PCIe Storage Cycle Router#1 Endpoint unique MSI-X Table BAR
  Offset(535),    NMB2, 32, // Offset(535),   RST PCIe Storage Cycle Router#2 Endpoint unique MSI-X Table BAR
  Offset(539),    NMB3, 32, // Offset(539),   RST PCIe Storage Cycle Router#3 Endpoint unique MSI-X Table BAR
  Offset(543),    NMV1, 32, // Offset(543),   RST PCIe Storage Cycle Router#1 Endpoint unique MSI-X Table BAR value
  Offset(547),    NMV2, 32, // Offset(547),   RST PCIe Storage Cycle Router#2 Endpoint unique MSI-X Table BAR value
  Offset(551),    NMV3, 32, // Offset(551),   RST PCIe Storage Cycle Router#3 Endpoint unique MSI-X Table BAR value
  Offset(555),    NPB1, 32, // Offset(555),   RST PCIe Storage Cycle Router#1 Endpoint unique MSI-X PBA BAR
  Offset(559),    NPB2, 32, // Offset(559),   RST PCIe Storage Cycle Router#2 Endpoint unique MSI-X PBA BAR
  Offset(563),    NPB3, 32, // Offset(563),   RST PCIe Storage Cycle Router#3 Endpoint unique MSI-X PBA BAR
  Offset(567),    NPV1, 32, // Offset(567),   RST PCIe Storage Cycle Router#1 Endpoint unique MSI-X PBA BAR value
  Offset(571),    NPV2, 32, // Offset(571),   RST PCIe Storage Cycle Router#2 Endpoint unique MSI-X PBA BAR value
  Offset(575),    NPV3, 32, // Offset(575),   RST PCIe Storage Cycle Router#3 Endpoint unique MSI-X PBA BAR value
  Offset(579),    NRP1, 32, // Offset(579),   RST PCIe Storage Cycle Router#1 Root Port number
  Offset(583),    NRP2, 32, // Offset(583),   RST PCIe Storage Cycle Router#2 Root Port number
  Offset(587),    NRP3, 32, // Offset(587),   RST PCIe Storage Cycle Router#3 Root Port number
  Offset(591),    EMH4, 8,  // Offset(591),   eMMC HS400 mode enabled
  Offset(592),    EMDS, 8,  // Offset(592),   eMMC Driver Strength
  Offset(593),    CSKU, 8,  // Offset(593),   CPU SKU
  Offset(594),    ITA0, 16, // Offset(594),
  Offset(596),    ITA1, 16, // Offset(596),
  Offset(598),    ITA2, 16, // Offset(598),
  Offset(600),    ITA3, 16, // Offset(600),
  Offset(602),    ITS0, 8,  // Offset(602),
  Offset(603),    ITS1, 8,  // Offset(603),
  Offset(604),    ITS2, 8,  // Offset(604),
  Offset(605),    ITS3, 8,  // Offset(605),
  Offset(606),    PMBS, 16, // Offset(606),   ACPI IO BASE address
  Offset(608),    PWRM, 32, // Offset(608),   PWRM MEM BASE address
  // Cnvi specific
  Offset(612),    CNVI, 8,  // Offset(612),   CNVi mode
  Offset(613),    RMRC, 32, // Offset(613),   RMRR CSME Base Address
  //Voltage Margining
  Offset(617),    VMRC, 8,  // Offset(617),   SLP_S0 Voltage Margining Runtime Control
  Offset(618),    VM70, 8,  // Offset(618),   SLP_S0 0.70V Voltage Margining Support
  Offset(619),    VM75, 8,  // Offset(619),   SLP_S0 0.75V Voltage Margining Support
  // PCH Trace Hub
  Offset(620),    PTHM, 8,  // Offset(620),   PCH Trace Hub Mode
  // PCH PS_ON support
  Offset(621),    PSON, 8,  // Offset(621),   PCH PS_ON enable
  Offset(622),    TWMB, 32, // Offset(622),   Reserved memory base address for Temp MBAR
  //
  // These are for PchApciTablesSelfTest use
  //
  Offset(626),    LTR1, 8,  // Offset(626),   Latency Tolerance Reporting Enable
  Offset(627),    LTR2, 8,  // Offset(627),   Latency Tolerance Reporting Enable
  Offset(628),    LTR3, 8,  // Offset(628),   Latency Tolerance Reporting Enable
  Offset(629),    LTR4, 8,  // Offset(629),   Latency Tolerance Reporting Enable
  Offset(630),    LTR5, 8,  // Offset(630),   Latency Tolerance Reporting Enable
  Offset(631),    LTR6, 8,  // Offset(631),   Latency Tolerance Reporting Enable
  Offset(632),    LTR7, 8,  // Offset(632),   Latency Tolerance Reporting Enable
  Offset(633),    LTR8, 8,  // Offset(633),   Latency Tolerance Reporting Enable
  Offset(634),    LTR9, 8,  // Offset(634),   Latency Tolerance Reporting Enable
  Offset(635),    LTRA, 8,  // Offset(635),   Latency Tolerance Reporting Enable
  Offset(636),    LTRB, 8,  // Offset(636),   Latency Tolerance Reporting Enable
  Offset(637),    LTRC, 8,  // Offset(637),   Latency Tolerance Reporting Enable
  Offset(638),    LTRD, 8,  // Offset(638),   Latency Tolerance Reporting Enable
  Offset(639),    LTRE, 8,  // Offset(639),   Latency Tolerance Reporting Enable
  Offset(640),    LTRF, 8,  // Offset(640),   Latency Tolerance Reporting Enable
  Offset(641),    LTRG, 8,  // Offset(641),   Latency Tolerance Reporting Enable
  Offset(642),    LTRH, 8,  // Offset(642),   Latency Tolerance Reporting Enable
  Offset(643),    LTRI, 8,  // Offset(643),   Latency Tolerance Reporting Enable
  Offset(644),    LTRJ, 8,  // Offset(644),   Latency Tolerance Reporting Enable
  Offset(645),    LTRK, 8,  // Offset(645),   Latency Tolerance Reporting Enable
  Offset(646),    LTRL, 8,  // Offset(646),   Latency Tolerance Reporting Enable
  Offset(647),    LTRM, 8,  // Offset(647),   Latency Tolerance Reporting Enable
  Offset(648),    LTRN, 8,  // Offset(648),   Latency Tolerance Reporting Enable
  Offset(649),    LTRO, 8,  // Offset(649),   Latency Tolerance Reporting Enable
  Offset(650),    GBES, 8,  // Offset(650),   GbE Support
  Offset(651),    SPPR, 8,  // Offset(651),   Holds information from SATA PCS register about SATA ports which recieved COMINIT from connected devices.
  Offset(652),    SDPH, 8,  // Offset(652),   SD PWREN# active high indication
  Offset(653),    EMCE, 8,  // Offset(653),   Set to indicate that eMMC is enabled
  Offset(654),    SDCE, 8,  // Offset(654),   Set to indicate that SD card is enabled
  }
