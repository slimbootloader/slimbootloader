/** @file

  Copyright (c) 2026, Intel Corporation. All rights reserved.<BR>
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
  Offset(102),    RPAP, 32, // Offset(102),   Root Port address 25
  Offset(106),    RPAQ, 32, // Offset(106),   Root Port address 26
  Offset(110),    RPAR, 32, // Offset(110),   Root Port address 27
  Offset(114),    RPAS, 32, // Offset(114),   Root Port address 28
  Offset(118),    NHAA, 64, // Offset(118),   HD-Audio NHLT ACPI address 0
  Offset(126),    NHA1, 64, // Offset(126),   HD-Audio NHLT ACPI address 1
  Offset(134),    NHA2, 64, // Offset(134),   HD-Audio NHLT ACPI address 2
  Offset(142),    NHA3, 64, // Offset(142),   HD-Audio NHLT ACPI address 3
  Offset(150),    NHA4, 64, // Offset(150),   HD-Audio NHLT ACPI address 4
  Offset(158),    NHA5, 64, // Offset(158),   HD-Audio NHLT ACPI address 5
  Offset(166),    NHA6, 64, // Offset(166),   HD-Audio NHLT ACPI address 6
  Offset(174),    NHA7, 64, // Offset(174),   HD-Audio NHLT ACPI address 7
  Offset(182),    NHLL, 32, // Offset(182),   HD-Audio NHLT ACPI length  0
  Offset(186),    NHL1, 32, // Offset(186),   HD-Audio NHLT ACPI length  1
  Offset(190),    NHL2, 32, // Offset(190),   HD-Audio NHLT ACPI length  2
  Offset(194),    NHL3, 32, // Offset(194),   HD-Audio NHLT ACPI length  3
  Offset(198),    NHL4, 32, // Offset(198),   HD-Audio NHLT ACPI length  4
  Offset(202),    NHL5, 32, // Offset(202),   HD-Audio NHLT ACPI length  5
  Offset(206),    NHL6, 32, // Offset(206),   HD-Audio NHLT ACPI length  6
  Offset(210),    NHL7, 32, // Offset(210),   HD-Audio NHLT ACPI length  7
  Offset(214),    ADFM, 32, // Offset(214),   HD-Audio DSP Feature Mask
  Offset(218),    SWQ0, 8,  // Offset(218),   HD-Audio SoundWire Link #1 quirk mask
  Offset(219),    SWQ1, 8,  // Offset(219),   HD-Audio SoundWire Link #2 quirk mask
  Offset(220),    SWQ2, 8,  // Offset(220),   HD-Audio SoundWire Link #3 quirk mask
  Offset(221),    SWQ3, 8,  // Offset(221),   HD-Audio SoundWire Link #4 quirk mask
  Offset(222),    ACS0, 8,  // Offset(222),   HD-Audio SoundWire Link #1 Autonomous Clock Stop
  Offset(223),    ACS1, 8,  // Offset(223),   HD-Audio SoundWire Link #2 Autonomous Clock Stop
  Offset(224),    ACS2, 8,  // Offset(224),   HD-Audio SoundWire Link #3 Autonomous Clock Stop
  Offset(225),    ACS3, 8,  // Offset(225),   HD-Audio SoundWire Link #4 Autonomous Clock Stop
  Offset(226),    DAI0, 8,  // Offset(226),   HD-Audio SoundWire Link #1 Data On Active Interval Select
  Offset(227),    DAI1, 8,  // Offset(227),   HD-Audio SoundWire Link #2 Data On Active Interval Select
  Offset(228),    DAI2, 8,  // Offset(228),   HD-Audio SoundWire Link #3 Data On Active Interval Select
  Offset(229),    DAI3, 8,  // Offset(229),   HD-Audio SoundWire Link #4 Data On Active Interval Select
  Offset(230),    DOD0, 8,  // Offset(230),   HD-Audio SoundWire Link #1 Data On Delay Select
  Offset(231),    DOD1, 8,  // Offset(231),   HD-Audio SoundWire Link #2 Data On Delay Select
  Offset(232),    DOD2, 8,  // Offset(232),   HD-Audio SoundWire Link #3 Data On Delay Select
  Offset(233),    DOD3, 8,  // Offset(233),   HD-Audio SoundWire Link #4 Data On Delay Select
  Offset(234),    HDBO, 16, // Offset(234),   HD-Audio Discrete BT Offload
  Offset(236),    SWMC, 8,  // Offset(236),   HD-Audio SoundWire Host Count
  Offset(237),    XTAL, 32, // Offset(237),   XTAL frequency: 0: 24MHz; 1: 38.4MHz; 2: Unsupported
  Offset(241),    AFDF, 8,  // Offset(241),   Audio FPGA Number of Device and Function. 0: Audio FPGA not detected
  Offset(242),    SBRG, 64, // Offset(242),   SBREG_BAR
  Offset(250),    GEI0, 8,  // Offset(250),   GPIO GroupIndex mapped to GPE_DW0
  Offset(251),    GEI1, 8,  // Offset(251),   GPIO GroupIndex mapped to GPE_DW1
  Offset(252),    GEI2, 8,  // Offset(252),   GPIO GroupIndex mapped to GPE_DW2
  Offset(253),    GED0, 8,  // Offset(253),   GPIO DW part of group mapped to GPE_DW0
  Offset(254),    GED1, 8,  // Offset(254),   GPIO DW part of group mapped to GPE_DW1
  Offset(255),    GED2, 8,  // Offset(255),   GPIO DW part of group mapped to GPE_DW2
  Offset(256),    PML1, 16, // Offset(256),   PCIE LTR max snoop Latency 1
  Offset(258),    PML2, 16, // Offset(258),   PCIE LTR max snoop Latency 2
  Offset(260),    PML3, 16, // Offset(260),   PCIE LTR max snoop Latency 3
  Offset(262),    PML4, 16, // Offset(262),   PCIE LTR max snoop Latency 4
  Offset(264),    PML5, 16, // Offset(264),   PCIE LTR max snoop Latency 5
  Offset(266),    PML6, 16, // Offset(266),   PCIE LTR max snoop Latency 6
  Offset(268),    PML7, 16, // Offset(268),   PCIE LTR max snoop Latency 7
  Offset(270),    PML8, 16, // Offset(270),   PCIE LTR max snoop Latency 8
  Offset(272),    PML9, 16, // Offset(272),   PCIE LTR max snoop Latency 9
  Offset(274),    PMLA, 16, // Offset(274),   PCIE LTR max snoop Latency 10
  Offset(276),    PMLB, 16, // Offset(276),   PCIE LTR max snoop Latency 11
  Offset(278),    PMLC, 16, // Offset(278),   PCIE LTR max snoop Latency 12
  Offset(280),    PMLD, 16, // Offset(280),   PCIE LTR max snoop Latency 13
  Offset(282),    PMLE, 16, // Offset(282),   PCIE LTR max snoop Latency 14
  Offset(284),    PMLF, 16, // Offset(284),   PCIE LTR max snoop Latency 15
  Offset(286),    PMLG, 16, // Offset(286),   PCIE LTR max snoop Latency 16
  Offset(288),    PMLH, 16, // Offset(288),   PCIE LTR max snoop Latency 17
  Offset(290),    PMLI, 16, // Offset(290),   PCIE LTR max snoop Latency 18
  Offset(292),    PMLJ, 16, // Offset(292),   PCIE LTR max snoop Latency 19
  Offset(294),    PMLK, 16, // Offset(294),   PCIE LTR max snoop Latency 20
  Offset(296),    PMLL, 16, // Offset(296),   PCIE LTR max snoop Latency 21
  Offset(298),    PMLM, 16, // Offset(298),   PCIE LTR max snoop Latency 22
  Offset(300),    PMLN, 16, // Offset(300),   PCIE LTR max snoop Latency 23
  Offset(302),    PMLO, 16, // Offset(302),   PCIE LTR max snoop Latency 24
  Offset(304),    PMLP, 16, // Offset(304),   PCIE LTR max snoop Latency 25
  Offset(306),    PMLQ, 16, // Offset(306),   PCIE LTR max snoop Latency 26
  Offset(308),    PMLR, 16, // Offset(308),   PCIE LTR max snoop Latency 27
  Offset(310),    PMLS, 16, // Offset(310),   PCIE LTR max snoop Latency 28
  Offset(312),    PNL1, 16, // Offset(312),   PCIE LTR max no snoop Latency 1
  Offset(314),    PNL2, 16, // Offset(314),   PCIE LTR max no snoop Latency 2
  Offset(316),    PNL3, 16, // Offset(316),   PCIE LTR max no snoop Latency 3
  Offset(318),    PNL4, 16, // Offset(318),   PCIE LTR max no snoop Latency 4
  Offset(320),    PNL5, 16, // Offset(320),   PCIE LTR max no snoop Latency 5
  Offset(322),    PNL6, 16, // Offset(322),   PCIE LTR max no snoop Latency 6
  Offset(324),    PNL7, 16, // Offset(324),   PCIE LTR max no snoop Latency 7
  Offset(326),    PNL8, 16, // Offset(326),   PCIE LTR max no snoop Latency 8
  Offset(328),    PNL9, 16, // Offset(328),   PCIE LTR max no snoop Latency 9
  Offset(330),    PNLA, 16, // Offset(330),   PCIE LTR max no snoop Latency 10
  Offset(332),    PNLB, 16, // Offset(332),   PCIE LTR max no snoop Latency 11
  Offset(334),    PNLC, 16, // Offset(334),   PCIE LTR max no snoop Latency 12
  Offset(336),    PNLD, 16, // Offset(336),   PCIE LTR max no snoop Latency 13
  Offset(338),    PNLE, 16, // Offset(338),   PCIE LTR max no snoop Latency 14
  Offset(340),    PNLF, 16, // Offset(340),   PCIE LTR max no snoop Latency 15
  Offset(342),    PNLG, 16, // Offset(342),   PCIE LTR max no snoop Latency 16
  Offset(344),    PNLH, 16, // Offset(344),   PCIE LTR max no snoop Latency 17
  Offset(346),    PNLI, 16, // Offset(346),   PCIE LTR max no snoop Latency 18
  Offset(348),    PNLJ, 16, // Offset(348),   PCIE LTR max no snoop Latency 19
  Offset(350),    PNLK, 16, // Offset(350),   PCIE LTR max no snoop Latency 20
  Offset(352),    PNLL, 16, // Offset(352),   PCIE LTR max no snoop Latency 21
  Offset(354),    PNLM, 16, // Offset(354),   PCIE LTR max no snoop Latency 22
  Offset(356),    PNLN, 16, // Offset(356),   PCIE LTR max no snoop Latency 23
  Offset(358),    PNLO, 16, // Offset(358),   PCIE LTR max no snoop Latency 24
  Offset(360),    PNLP, 16, // Offset(360),   PCIE LTR max no snoop Latency 25
  Offset(362),    PNLQ, 16, // Offset(362),   PCIE LTR max no snoop Latency 26
  Offset(364),    PNLR, 16, // Offset(364),   PCIE LTR max no snoop Latency 27
  Offset(366),    PNLS, 16, // Offset(366),   PCIE LTR max no snoop Latency 28
  Offset(368),    XHPC, 8,  // Offset(368),   Number of HighSpeed ports implemented in XHCI controller
  Offset(369),    XRPC, 8,  // Offset(369),   Number of USBR ports implemented in XHCI controller
  Offset(370),    XSPC, 8,  // Offset(370),   Number of SuperSpeed ports implemented in XHCI controller
  Offset(371),    XSPA, 8,  // Offset(371),   Address of 1st SuperSpeed port
  Offset(372),    HPTB, 32, // Offset(372),   HPET base address
  Offset(376),    HPTE, 8,  // Offset(376),   HPET enable
  //SerialIo block
  Offset(377),    SM00, 8,  // Offset(377),   SerialIo SPI Controller 0 Mode
  Offset(378),    SM01, 8,  // Offset(378),   SerialIo SPI Controller 1 Mode
  Offset(379),    SM02, 8,  // Offset(379),   SerialIo SPI Controller 2 Mode
  Offset(380),    SM03, 8,  // Offset(380),   SerialIo SPI Controller 3 Mode
  Offset(381),    SM04, 8,  // Offset(381),   SerialIo SPI Controller 4 Mode
  Offset(382),    SM05, 8,  // Offset(382),   SerialIo SPI Controller 5 Mode
  Offset(383),    SM06, 8,  // Offset(383),   SerialIo SPI Controller 6 Mode
  Offset(384),    SC00, 64, // Offset(384),   SerialIo SPI Controller 0 Pci Config
  Offset(392),    SC01, 64, // Offset(392),   SerialIo SPI Controller 1 Pci Config
  Offset(400),    SC02, 64, // Offset(400),   SerialIo SPI Controller 2 Pci Config
  Offset(408),    SC03, 64, // Offset(408),   SerialIo SPI Controller 3 Pci Config
  Offset(416),    SC04, 64, // Offset(416),   SerialIo SPI Controller 4 Pci Config
  Offset(424),    SC05, 64, // Offset(424),   SerialIo SPI Controller 5 Pci Config
  Offset(432),    SC06, 64, // Offset(432),   SerialIo SPI Controller 6 Pci Config
  Offset(440),    IM00, 8,  // Offset(440),   SerialIo I2C Controller 0 Mode
  Offset(441),    IM01, 8,  // Offset(441),   SerialIo I2C Controller 1 Mode
  Offset(442),    IM02, 8,  // Offset(442),   SerialIo I2C Controller 2 Mode
  Offset(443),    IM03, 8,  // Offset(443),   SerialIo I2C Controller 3 Mode
  Offset(444),    IM04, 8,  // Offset(444),   SerialIo I2C Controller 4 Mode
  Offset(445),    IM05, 8,  // Offset(445),   SerialIo I2C Controller 5 Mode
  Offset(446),    IM06, 8,  // Offset(446),   SerialIo I2C Controller 6 Mode
  Offset(447),    IM07, 8,  // Offset(447),   SerialIo I2C Controller 7 Mode
  Offset(448),    IC00, 64, // Offset(448),   SerialIo I2C Controller 0 Pci Config
  Offset(456),    IC01, 64, // Offset(456),   SerialIo I2C Controller 1 Pci Config
  Offset(464),    IC02, 64, // Offset(464),   SerialIo I2C Controller 2 Pci Config
  Offset(472),    IC03, 64, // Offset(472),   SerialIo I2C Controller 3 Pci Config
  Offset(480),    IC04, 64, // Offset(480),   SerialIo I2C Controller 4 Pci Config
  Offset(488),    IC05, 64, // Offset(488),   SerialIo I2C Controller 5 Pci Config
  Offset(496),    IC06, 64, // Offset(496),   SerialIo I2C Controller 6 Pci Config
  Offset(504),    IC07, 64, // Offset(504),   SerialIo I2C Controller 7 Pci Config
  Offset(512),    UM00, 8,  // Offset(512),   SerialIo UART Controller 0 Mode
  Offset(513),    UM01, 8,  // Offset(513),   SerialIo UART Controller 1 Mode
  Offset(514),    UM02, 8,  // Offset(514),   SerialIo UART Controller 2 Mode
  Offset(515),    UM03, 8,  // Offset(515),   SerialIo UART Controller 3 Mode
  Offset(516),    UM04, 8,  // Offset(516),   SerialIo UART Controller 4 Mode
  Offset(517),    UM05, 8,  // Offset(517),   SerialIo UART Controller 5 Mode
  Offset(518),    UM06, 8,  // Offset(518),   SerialIo UART Controller 6 Mode
  Offset(519),    UC00, 64, // Offset(519),   SerialIo UART Controller 0 Pci Config
  Offset(527),    UC01, 64, // Offset(527),   SerialIo UART Controller 1 Pci Config
  Offset(535),    UC02, 64, // Offset(535),   SerialIo UART Controller 2 Pci Config
  Offset(543),    UC03, 64, // Offset(543),   SerialIo UART Controller 3 Pci Config
  Offset(551),    UC04, 64, // Offset(551),   SerialIo UART Controller 4 Pci Config
  Offset(559),    UC05, 64, // Offset(559),   SerialIo UART Controller 5 Pci Config
  Offset(567),    UC06, 64, // Offset(567),   SerialIo UART Controller 6 Pci Config
  Offset(575),    UD00, 8,  // Offset(575),   SerialIo UART Controller 0 DmaEnable
  Offset(576),    UD01, 8,  // Offset(576),   SerialIo UART Controller 1 DmaEnable
  Offset(577),    UD02, 8,  // Offset(577),   SerialIo UART Controller 2 DmaEnable
  Offset(578),    UD03, 8,  // Offset(578),   SerialIo UART Controller 3 DmaEnable
  Offset(579),    UD04, 8,  // Offset(579),   SerialIo UART Controller 4 DmaEnable
  Offset(580),    UD05, 8,  // Offset(580),   SerialIo UART Controller 5 DmaEnable
  Offset(581),    UD06, 8,  // Offset(581),   SerialIo UART Controller 6 DmaEnable
  Offset(582),    UP00, 8,  // Offset(582),   SerialIo UART Controller 0 Power Gating
  Offset(583),    UP01, 8,  // Offset(583),   SerialIo UART Controller 1 Power Gating
  Offset(584),    UP02, 8,  // Offset(584),   SerialIo UART Controller 2 Power Gating
  Offset(585),    UP03, 8,  // Offset(585),   SerialIo UART Controller 3 Power Gating
  Offset(586),    UP04, 8,  // Offset(586),   SerialIo UART Controller 4 Power Gating
  Offset(587),    UP05, 8,  // Offset(587),   SerialIo UART Controller 5 Power Gating
  Offset(588),    UP06, 8,  // Offset(588),   SerialIo UART Controller 6 Power Gating
  Offset(589),    UI00, 8,  // Offset(589),   SerialIo UART Controller 0 Irq
  Offset(590),    UI01, 8,  // Offset(590),   SerialIo UART Controller 1 Irq
  Offset(591),    UI02, 8,  // Offset(591),   SerialIo UART Controller 2 Irq
  Offset(592),    UI03, 8,  // Offset(592),   SerialIo UART Controller 3 Irq
  Offset(593),    UI04, 8,  // Offset(593),   SerialIo UART Controller 4 Irq
  Offset(594),    UI05, 8,  // Offset(594),   SerialIo UART Controller 5 Irq
  Offset(595),    UI06, 8,  // Offset(595),   SerialIo UART Controller 6 Irq
  //end of SerialIo block
  Offset(596),    SGIR, 8,  // Offset(596),   GPIO IRQ
  Offset(597),    GPHD, 8,  // Offset(597),   Hide GPIO ACPI device
  Offset(598),    NIT1, 8,  // Offset(598),   RST PCIe Storage Cycle Router#1 Interface Type
  Offset(599),    NIT2, 8,  // Offset(599),   RST PCIe Storage Cycle Router#2 Interface Type
  Offset(600),    NIT3, 8,  // Offset(600),   RST PCIe Storage Cycle Router#3 Interface Type
  Offset(601),    NPM1, 8,  // Offset(601),   RST PCIe Storage Cycle Router#1 Power Management Capability Pointer
  Offset(602),    NPM2, 8,  // Offset(602),   RST PCIe Storage Cycle Router#2 Power Management Capability Pointer
  Offset(603),    NPM3, 8,  // Offset(603),   RST PCIe Storage Cycle Router#3 Power Management Capability Pointer
  Offset(604),    NPC1, 8,  // Offset(604),   RST PCIe Storage Cycle Router#1 PCIe Capabilities Pointer
  Offset(605),    NPC2, 8,  // Offset(605),   RST PCIe Storage Cycle Router#2 PCIe Capabilities Pointer
  Offset(606),    NPC3, 8,  // Offset(606),   RST PCIe Storage Cycle Router#3 PCIe Capabilities Pointer
  Offset(607),    NL11, 16, // Offset(607),   RST PCIe Storage Cycle Router#1 L1SS Capability Pointer
  Offset(609),    NL12, 16, // Offset(609),   RST PCIe Storage Cycle Router#2 L1SS Capability Pointer
  Offset(611),    NL13, 16, // Offset(611),   RST PCIe Storage Cycle Router#3 L1SS Capability Pointer
  Offset(613),    ND21, 8,  // Offset(613),   RST PCIe Storage Cycle Router#1 Endpoint L1SS Control Data2
  Offset(614),    ND22, 8,  // Offset(614),   RST PCIe Storage Cycle Router#2 Endpoint L1SS Control Data2
  Offset(615),    ND23, 8,  // Offset(615),   RST PCIe Storage Cycle Router#3 Endpoint L1SS Control Data2
  Offset(616),    ND11, 32, // Offset(616),   RST PCIe Storage Cycle Router#1 Endpoint L1SS Control Data1
  Offset(620),    ND12, 32, // Offset(620),   RST PCIe Storage Cycle Router#2 Endpoint L1SS Control Data1
  Offset(624),    ND13, 32, // Offset(624),   RST PCIe Storage Cycle Router#3 Endpoint L1SS Control Data1
  Offset(628),    NLR1, 16, // Offset(628),   RST PCIe Storage Cycle Router#1 LTR Capability Pointer
  Offset(630),    NLR2, 16, // Offset(630),   RST PCIe Storage Cycle Router#2 LTR Capability Pointer
  Offset(632),    NLR3, 16, // Offset(632),   RST PCIe Storage Cycle Router#3 LTR Capability Pointer
  Offset(634),    NLD1, 32, // Offset(634),   RST PCIe Storage Cycle Router#1 Endpoint LTR Data
  Offset(638),    NLD2, 32, // Offset(638),   RST PCIe Storage Cycle Router#2 Endpoint LTR Data
  Offset(642),    NLD3, 32, // Offset(642),   RST PCIe Storage Cycle Router#3 Endpoint LTR Data
  Offset(646),    NEA1, 16, // Offset(646),   RST PCIe Storage Cycle Router#1 Endpoint LCTL Data
  Offset(648),    NEA2, 16, // Offset(648),   RST PCIe Storage Cycle Router#2 Endpoint LCTL Data
  Offset(650),    NEA3, 16, // Offset(650),   RST PCIe Storage Cycle Router#3 Endpoint LCTL Data
  Offset(652),    NEB1, 16, // Offset(652),   RST PCIe Storage Cycle Router#1 Endpoint DCTL Data
  Offset(654),    NEB2, 16, // Offset(654),   RST PCIe Storage Cycle Router#2 Endpoint DCTL Data
  Offset(656),    NEB3, 16, // Offset(656),   RST PCIe Storage Cycle Router#3 Endpoint DCTL Data
  Offset(658),    NEC1, 16, // Offset(658),   RST PCIe Storage Cycle Router#1 Endpoint DCTL2 Data
  Offset(660),    NEC2, 16, // Offset(660),   RST PCIe Storage Cycle Router#2 Endpoint DCTL2 Data
  Offset(662),    NEC3, 16, // Offset(662),   RST PCIe Storage Cycle Router#3 Endpoint DCTL2 Data
  Offset(664),    NRA1, 16, // Offset(664),   RST PCIe Storage Cycle Router#1 RootPort DCTL2 Data
  Offset(666),    NRA2, 16, // Offset(666),   RST PCIe Storage Cycle Router#2 RootPort DCTL2 Data
  Offset(668),    NRA3, 16, // Offset(668),   RST PCIe Storage Cycle Router#3 RootPort DCTL2 Data
  Offset(670),    NMB1, 32, // Offset(670),   RST PCIe Storage Cycle Router#1 Endpoint unique MSI-X Table BAR
  Offset(674),    NMB2, 32, // Offset(674),   RST PCIe Storage Cycle Router#2 Endpoint unique MSI-X Table BAR
  Offset(678),    NMB3, 32, // Offset(678),   RST PCIe Storage Cycle Router#3 Endpoint unique MSI-X Table BAR
  Offset(682),    NMV1, 32, // Offset(682),   RST PCIe Storage Cycle Router#1 Endpoint unique MSI-X Table BAR value
  Offset(686),    NMV2, 32, // Offset(686),   RST PCIe Storage Cycle Router#2 Endpoint unique MSI-X Table BAR value
  Offset(690),    NMV3, 32, // Offset(690),   RST PCIe Storage Cycle Router#3 Endpoint unique MSI-X Table BAR value
  Offset(694),    NPB1, 32, // Offset(694),   RST PCIe Storage Cycle Router#1 Endpoint unique MSI-X PBA BAR
  Offset(698),    NPB2, 32, // Offset(698),   RST PCIe Storage Cycle Router#2 Endpoint unique MSI-X PBA BAR
  Offset(702),    NPB3, 32, // Offset(702),   RST PCIe Storage Cycle Router#3 Endpoint unique MSI-X PBA BAR
  Offset(706),    NPV1, 32, // Offset(706),   RST PCIe Storage Cycle Router#1 Endpoint unique MSI-X PBA BAR value
  Offset(710),    NPV2, 32, // Offset(710),   RST PCIe Storage Cycle Router#2 Endpoint unique MSI-X PBA BAR value
  Offset(714),    NPV3, 32, // Offset(714),   RST PCIe Storage Cycle Router#3 Endpoint unique MSI-X PBA BAR value
  Offset(718),    NRP1, 32, // Offset(718),   RST PCIe Storage Cycle Router#1 Root Port number
  Offset(722),    NRP2, 32, // Offset(722),   RST PCIe Storage Cycle Router#2 Root Port number
  Offset(726),    NRP3, 32, // Offset(726),   RST PCIe Storage Cycle Router#3 Root Port number
  Offset(730),    EMH4, 8,  // Offset(730),   eMMC HS400 mode enabled
  Offset(731),    EMDS, 8,  // Offset(731),   eMMC Driver Strength
  Offset(732),    CSKU, 8,  // Offset(732),   CPU SKU
  Offset(733),    ITA0, 16, // Offset(733),
  Offset(735),    ITA1, 16, // Offset(735),
  Offset(737),    ITA2, 16, // Offset(737),
  Offset(739),    ITA3, 16, // Offset(739),
  Offset(741),    ITS0, 8,  // Offset(741),
  Offset(742),    ITS1, 8,  // Offset(742),
  Offset(743),    ITS2, 8,  // Offset(743),
  Offset(744),    ITS3, 8,  // Offset(744),
  Offset(745),    PMBS, 16, // Offset(745),   ACPI IO BASE address
  Offset(747),    PWRM, 64, // Offset(747),   PWRM MEM BASE address
  // CNVi specific
  Offset(755),    CRFP, 8,  // Offset(755),   CNVi CRF Present
  Offset(756),    PCNV, 16, // Offset(756),   CNVi Sideband Port ID
  Offset(758),    CWFC, 8,  // Offset(758),   CNVi WiFi Core
  Offset(759),    CBTC, 8,  // Offset(759),   CNVi BT Core
  Offset(760),    CBTI, 8,  // Offset(760),   CNVi BT Interface
  Offset(761),    CBTA, 8,  // Offset(761),   CNVi BT Audio Offload
  Offset(762),    GBTP, 32, // Offset(762),   CNVi BT Enable (CNV_BTEN) pin
  Offset(766),    CVPR, 8,  // Offset(766),   CNVi vPro
  Offset(767),    CRFI, 8,  // Offset(767),   CNVi DDR RFI Mitigation
  // TraceHub
  Offset(768),    TVBS, 8,  // Offset(768),   BIOS trace verbosity
  Offset(769),    TFWB, 32, // Offset(769),   BIOS Trace Hub FW base address
  // PCH PS_ON support
  Offset(773),    PSON, 8,  // Offset(773),   PCH PS_ON enable
  //
  // These are for PchApciTablesSelfTest use
  //
  Offset(774),    LTR1, 8,  // Offset(774),   Latency Tolerance Reporting Enable
  Offset(775),    LTR2, 8,  // Offset(775),   Latency Tolerance Reporting Enable
  Offset(776),    LTR3, 8,  // Offset(776),   Latency Tolerance Reporting Enable
  Offset(777),    LTR4, 8,  // Offset(777),   Latency Tolerance Reporting Enable
  Offset(778),    LTR5, 8,  // Offset(778),   Latency Tolerance Reporting Enable
  Offset(779),    LTR6, 8,  // Offset(779),   Latency Tolerance Reporting Enable
  Offset(780),    LTR7, 8,  // Offset(780),   Latency Tolerance Reporting Enable
  Offset(781),    LTR8, 8,  // Offset(781),   Latency Tolerance Reporting Enable
  Offset(782),    LTR9, 8,  // Offset(782),   Latency Tolerance Reporting Enable
  Offset(783),    LTRA, 8,  // Offset(783),   Latency Tolerance Reporting Enable
  Offset(784),    LTRB, 8,  // Offset(784),   Latency Tolerance Reporting Enable
  Offset(785),    LTRC, 8,  // Offset(785),   Latency Tolerance Reporting Enable
  Offset(786),    LTRD, 8,  // Offset(786),   Latency Tolerance Reporting Enable
  Offset(787),    LTRE, 8,  // Offset(787),   Latency Tolerance Reporting Enable
  Offset(788),    LTRF, 8,  // Offset(788),   Latency Tolerance Reporting Enable
  Offset(789),    LTRG, 8,  // Offset(789),   Latency Tolerance Reporting Enable
  Offset(790),    LTRH, 8,  // Offset(790),   Latency Tolerance Reporting Enable
  Offset(791),    LTRI, 8,  // Offset(791),   Latency Tolerance Reporting Enable
  Offset(792),    LTRJ, 8,  // Offset(792),   Latency Tolerance Reporting Enable
  Offset(793),    LTRK, 8,  // Offset(793),   Latency Tolerance Reporting Enable
  Offset(794),    LTRL, 8,  // Offset(794),   Latency Tolerance Reporting Enable
  Offset(795),    LTRM, 8,  // Offset(795),   Latency Tolerance Reporting Enable
  Offset(796),    LTRN, 8,  // Offset(796),   Latency Tolerance Reporting Enable
  Offset(797),    LTRO, 8,  // Offset(797),   Latency Tolerance Reporting Enable
  Offset(798),    LTRP, 8,  // Offset(798),   Latency Tolerance Reporting Enable
  Offset(799),    LTRQ, 8,  // Offset(799),   Latency Tolerance Reporting Enable
  Offset(800),    LTRR, 8,  // Offset(800),   Latency Tolerance Reporting Enable
  Offset(801),    LTRS, 8,  // Offset(801),   Latency Tolerance Reporting Enable
  Offset(802),    GBES, 8,  // Offset(802),   GbE Support
  Offset(803),    PPDS, 32, // Offset(803),   PCH xDCI Power Down Scale Value, DWC_USB3_GCTL_INIT[31:19]
  Offset(807),    EMCE, 8,  // Offset(807),   Set to indicate that eMMC is enabled
  Offset(808),    SDCE, 8,  // Offset(808),   Set to indicate that SD card is enabled
  Offset(809),    TGEA, 8,  // Offset(809),   Set to indicate that Timed GPIO 0 is enabled
  Offset(810),    TGEB, 8,  // Offset(810),   Set to indicate that Timed GPIO 1 is enabled
  Offset(811),    CR00, 8,  // Offset(811),   CLOCK index to root port index map. Used during PCIe D3Cold flows
  Offset(812),    CR01, 8,  // Offset(812),
  Offset(813),    CR02, 8,  // Offset(813),
  Offset(814),    CR03, 8,  // Offset(814),
  Offset(815),    CR04, 8,  // Offset(815),
  Offset(816),    CR05, 8,  // Offset(816),
  Offset(817),    CR06, 8,  // Offset(817),
  Offset(818),    CR07, 8,  // Offset(818),
  Offset(819),    CR08, 8,  // Offset(819),
  Offset(820),    CR09, 8,  // Offset(820),
  Offset(821),    CR10, 8,  // Offset(821),
  Offset(822),    CR11, 8,  // Offset(822),
  Offset(823),    CR12, 8,  // Offset(823),
  Offset(824),    CR13, 8,  // Offset(824),
  Offset(825),    CR14, 8,  // Offset(825),
  Offset(826),    CR15, 8,  // Offset(826),
  Offset(827),    CR16, 8,  // Offset(827),
  Offset(828),    CR17, 8,  // Offset(828),
  Offset(829),    TCOB, 16, // Offset(829),   TCO base address
  Offset(831),    ICKP, 16, // Offset(831),   Iclk PID number
  Offset(833),    PU2C, 8,  // Offset(833),   Number of USB2 ports in PCH
  Offset(834),    PU3C, 8,  // Offset(834),   Number of USB3 ports in PCH
  Offset(835),    SPPR, 8,  // Offset(835),   Holds information from SATA PCS register about SATA ports which recieved COMINIT from connected devices.
  Offset(836),    IPCC, 8,  // Offset(836),   PCIE CLKREQ IPC command support
  Offset(837),    CQ00, 8,  // Offset(837),   CLOCK Source index to ClkReq Number. Used during PCIe D3Cold flows
  Offset(838),    CQ01, 8,  // Offset(838),
  Offset(839),    CQ02, 8,  // Offset(839),
  Offset(840),    CQ03, 8,  // Offset(840),
  Offset(841),    CQ04, 8,  // Offset(841),
  Offset(842),    CQ05, 8,  // Offset(842),
  Offset(843),    CQ06, 8,  // Offset(843),
  Offset(844),    CQ07, 8,  // Offset(844),
  Offset(845),    CQ08, 8,  // Offset(845),
  Offset(846),    CQ09, 8,  // Offset(846),
  Offset(847),    CQ10, 8,  // Offset(847),
  Offset(848),    CQ11, 8,  // Offset(848),
  Offset(849),    CQ12, 8,  // Offset(849),
  Offset(850),    CQ13, 8,  // Offset(850),
  Offset(851),    CQ14, 8,  // Offset(851),
  Offset(852),    CQ15, 8,  // Offset(852),
  Offset(853),    CQ16, 8,  // Offset(853),
  Offset(854),    CQ17, 8,  // Offset(854),
  Offset(855),    TIN0, 32, // Offset(855),   Touch Host Controller Wake On Touch Interrupt Number - when 0 wake is disabled
  Offset(859),    TIN1, 32, // Offset(859),
  Offset(863),    TMD0, 16, // Offset(863),   Touch Host Controller Mode THC or HID over SPI
  Offset(865),    TMD1, 16, // Offset(865),
  Offset(867),    UF0E, 8,  // Offset(867),   Is UFS0 Enabled
  Offset(868),    UF1E, 8,  // Offset(868),   Is UFS1 Enabled
  Offset(869),    UAOE, 8,  // Offset(869),   Is USB Audio Offload enabled
  Offset(870),    T010, 32, // Offset(870),   Touch Host Controller Reset Pad
  Offset(874),    T011, 32, // Offset(874),
  Offset(878),    T020, 8,  // Offset(878),   Touch Host Controller Reset Pad Trigger
  Offset(879),    T021, 8,  // Offset(879),
  Offset(880),    T030, 32, // Offset(880),   Touch Host Controller HID over SPI Connection Speed
  Offset(884),    T031, 32, // Offset(884),
  Offset(888),    T040, 32, // Offset(888),   Touch Host Controller HID over SPI Input Report Header Address
  Offset(892),    T041, 32, // Offset(892),
  Offset(896),    T050, 32, // Offset(896),   Touch Host Controller HID over SPI Input Report Body Address
  Offset(900),    T051, 32, // Offset(900),
  Offset(904),    T060, 32, // Offset(904),   Touch Host Controller HID over SPI Output Report Address
  Offset(908),    T061, 32, // Offset(908),
  Offset(912),    T070, 32, // Offset(912),   Touch Host Controller HID over SPI Read Opcode
  Offset(916),    T071, 32, // Offset(916),
  Offset(920),    T080, 32, // Offset(920),   Touch Host Controller HID over SPI Write Opcode
  Offset(924),    T081, 32, // Offset(924),
  Offset(928),    T090, 32, // Offset(928),   Touch Host Controller HID over SPI Flags
  Offset(932),    T091, 32, // Offset(932),
  Offset(936),    T0A0, 32, // Offset(936),   Touch Host Controller Active Ltr
  Offset(940),    T0A1, 32, // Offset(940),
  Offset(944),    T0B0, 32, // Offset(944),   Touch Host Controller Idle Ltr
  Offset(948),    T0B1, 32, // Offset(948),
  Offset(952),    T0C0, 32, // Offset(952),   Touch Host Controller Limit Packet Size
  Offset(956),    T0C1, 32, // Offset(956),
  Offset(960),    T0D0, 32, // Offset(960),   Touch Host Controller Performance Limitation
  Offset(964),    T0D1, 32, // Offset(964),
  Offset(968),    T0E0, 32, // Offset(968),   Touch Host Controller Display Frame Sync Period
  Offset(972),    T0E1, 32, // Offset(972),
  Offset(976),    T0F0, 32, // Offset(976),   Touch Host Controller Reset Sequencing Delay
  Offset(980),    T0F1, 32, // Offset(980),
  Offset(984),    T0G0, 16, // Offset(984),   Touch Host Controller HID over I2C Descriptor Address
  Offset(986),    T0G1, 16, // Offset(986),
  Offset(988),    T0H0, 32, // Offset(988),   Touch Host Controller HID over I2C Connection Speed
  Offset(992),    T0H1, 32, // Offset(992),
  Offset(996),    T0I0, 8,  // Offset(996),   Touch Host Controller HID over I2C Addressing Mode
  Offset(997),    T0I1, 8,  // Offset(997),
  Offset(998),    T0J0, 64, // Offset(998),   Touch Host Controller HID over I2C Serial Clock Line High Period
  Offset(1006),   T0J1, 64, // Offset(1006),
  Offset(1014),   T0K0, 64, // Offset(1014),  Touch Host Controller HID over I2C Standard Mode Serial Clock Line Low Period
  Offset(1022),   T0K1, 64, // Offset(1022),
  Offset(1030),   T0L0, 64, // Offset(1030),  Touch Host Controller HID over I2C Standard Mode Serial Data Line Transmit Hold Period
  Offset(1038),   T0L1, 64, // Offset(1038),
  Offset(1046),   T0M0, 64, // Offset(1046),  Touch Host Controller HID over I2C Standard Mode Serial Data Receive Hold Period
  Offset(1054),   T0M1, 64, // Offset(1054),
  Offset(1062),   T0N0, 64, // Offset(1062),  Touch Host Controller HID over I2C Fast Mode Serial Clock Line High Period
  Offset(1070),   T0N1, 64, // Offset(1070),
  Offset(1078),   T0O0, 64, // Offset(1078),  Touch Host Controller HID over I2C Fast Mode Serial Clock Line Low Period
  Offset(1086),   T0O1, 64, // Offset(1086),
  Offset(1094),   T0P0, 64, // Offset(1094),  Touch Host Controller HID over I2C Fast Mode Serial Data Line Transmit Hold Period
  Offset(1102),   T0P1, 64, // Offset(1102),
  Offset(1110),   T0Q0, 64, // Offset(1110),  Touch Host Controller HID over I2C Fast Mode Serial Data Line Receive Hold Period
  Offset(1118),   T0Q1, 64, // Offset(1118),
  Offset(1126),   T0R0, 64, // Offset(1126),  Touch Host Controller HID over I2C Maximum Length Of Suppressed Spikes In Std Mode Fast Mode And Fast Mode Plus
  Offset(1134),   T0R1, 64, // Offset(1134),
  Offset(1142),   T0S0, 64, // Offset(1142),  Touch Host Controller HID over I2C Fast Mode Plus Serial Clock Line High Period
  Offset(1150),   T0S1, 64, // Offset(1150),
  Offset(1158),   T0T0, 64, // Offset(1158),  Touch Host Controller HID over I2C Fast Mode Plus Serial Clock Line Low Period
  Offset(1166),   T0T1, 64, // Offset(1166),
  Offset(1174),   T0U0, 64, // Offset(1174),  Touch Host Controller HID over I2C Fast Mode Plus Serial Data Line Transmit Hold Period
  Offset(1182),   T0U1, 64, // Offset(1182),
  Offset(1190),   T0V0, 64, // Offset(1190),  Touch Host Controller HID over I2C Fast Mode Plus Serial Data Line Receive Hold Period
  Offset(1198),   T0V1, 64, // Offset(1198),
  Offset(1206),   T0W0, 64, // Offset(1206),  Touch Host Controller HID over I2C High Speed Mode Plus Serial Clock Line High Period
  Offset(1214),   T0W1, 64, // Offset(1214),
  Offset(1222),   T0X0, 64, // Offset(1222),  Touch Host Controller HID over I2C High Speed Mode Plus Serial Clock Line Low Period
  Offset(1230),   T0X1, 64, // Offset(1230),
  Offset(1238),   T0Y0, 64, // Offset(1238),  Touch Host Controller HID over I2C High Speed Mode Plus Serial Data Line Transmit Hold Period
  Offset(1246),   T0Y1, 64, // Offset(1246),
  Offset(1254),   T0Z0, 64, // Offset(1254),  Touch Host Controller HID over I2C High Speed Mode Plus Serial Data Line Receive Hold Period
  Offset(1262),   T0Z1, 64, // Offset(1262),
  Offset(1270),   T000, 64, // Offset(1270),  Touch Host Controller HID over I2C Maximum Length Of Suppressed Spikes In High Speed Mode
  Offset(1278),   T001, 64, // Offset(1278),
  Offset(1286),   T002, 16, // Offset(1286),  Touch Host Controller HID over I2C Device Descriptor Address
  Offset(1288),   T003, 16, // Offset(1288),
  Offset(1290),   CPPE, 8,  // Offset(1290),  CPPM Forced Alignment Enable
  Offset(1291),   PCHA, 8,  // Offset(1291),  PCH Attached
  }
