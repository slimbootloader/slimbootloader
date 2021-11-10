/** @file
  ACPI DSDT table

  Copyright (c) 2018 - 2019, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

  // Define a Global region of ACPI NVS Region that may be used for any
  // type of implementation.  The starting offset and size will be fixed
  // up by the System BIOS during POST.  Note that the Size must be a word
  // in size to be fixed up correctly.




  OperationRegion(GNVS,SystemMemory,0xFFFF0000,0xAA55)
  Field(GNVS,AnyAcc,Lock,Preserve)
  {  //
  // Miscellaneous Dynamic Registers:
  //
  Offset(0),      OSYS, 16, // Offset(0),     Operating System
  Offset(2),      SMIF, 8,  // Offset(2),     SMI Function Call (ASL to SMI via I/O Trap)
  Offset(3),      PRM0, 8,  // Offset(3),     SMIF - Parameter 0
  Offset(4),      PRM1, 8,  // Offset(4),     SMIF - Parameter 1
  Offset(5),      SCIF, 8,  // Offset(5),     SCI Function Call (SMI to ASL via _L00)
  Offset(6),      PRM2, 8,  // Offset(6),     SCIF - Parameter 0
  Offset(7),      PRM3, 8,  // Offset(7),     SCIF - Parameter 1
  Offset(8),      LCKF, 8,  // Offset(8),     Global Lock Function Call (EC Communication)
  Offset(9),      PRM4, 8,  // Offset(9),     LCKF - Parameter 0
  Offset(10),     PRM5, 8,  // Offset(10),    LCKF - Parameter 1
  Offset(11),     PWRS, 8,  // Offset(11),    Power State (AC Mode = 1)
  Offset(12),     DBGS, 8,  // Offset(12),    Debug State
  //
  // Thermal Policy Registers:
  //
  Offset(13),     THOF, 8,  // Offset(13),    Enable Thermal Offset for KSC
  Offset(14),     ACT1, 8,  // Offset(14),    Active Trip Point 1
  Offset(15),     ACTT, 8,  // Offset(15),    Active Trip Point
  Offset(16),     PSVT, 8,  // Offset(16),    Passive Trip Point
  Offset(17),     TC1V, 8,  // Offset(17),    Passive Trip Point TC1 Value
  Offset(18),     TC2V, 8,  // Offset(18),    Passive Trip Point TC2 Value
  Offset(19),     TSPV, 8,  // Offset(19),    Passive Trip Point TSP Value
  Offset(20),     CRTT, 8,  // Offset(20),    Critical Trip Point
  Offset(21),     DTSE, 8,  // Offset(21),    Digital Thermal Sensor Enable
  Offset(22),     DTS1, 8,  // Offset(22),    Digital Thermal Sensor 1 Reading
  Offset(23),     DTS2, 8,  // Offset(23),    Digital Thermal Sensor 2 Reading
  Offset(24),     DTSF, 8,  // Offset(24),    DTS SMI Function Call
  //
  // Revision Field:
  //
  Offset(25),     REVN, 8,  // Offset(25),    Revison of GlobalNvsArea
  //
  // CPU Identification Registers:
  //
  Offset(26),     APIC, 8,  // Offset(26),    APIC Enabled by SBIOS (APIC Enabled = 1)
  Offset(27),     TCNT, 8,  // Offset(27),    Number of Enabled Threads
  Offset(28),     PCP0, 8,  // Offset(28),    PDC Settings, Processor 0
  Offset(29),     PCP1, 8,  // Offset(29),    PDC Settings, Processor 1
  Offset(30),     PPCM, 8,  // Offset(30),    Maximum PPC state
  Offset(31),     PPMF, 32, // Offset(31),    PPM Flags (Same as CFGD)
  Offset(35),     C67L, 8,  // Offset(35),    C6/C7 Entry/Exit latency
  //
  // SIO Configuration Registers:
  //
  Offset(36),     NATP, 8,  // Offset(36),    National SIO Present
  Offset(37),     CMAP, 8,  // Offset(37),    COM A Port
  Offset(38),     CMBP, 8,  // Offset(38),    COM B Port
  Offset(39),     LPTP, 8,  // Offset(39),    LPT Port
  Offset(40),     FDCP, 8,  // Offset(40),    FDC Port
  Offset(41),     W381, 8,  // Offset(41),    WPCN381U SIO Present
  //
  // Extended Mobile Access Values
  //
  Offset(42),     EMAE, 8,  // Offset(42),    EMA Enable
  Offset(43),     EMAP, 16, // Offset(43),    EMA Pointer
  Offset(45),     EMAL, 16, // Offset(45),    EMA Length
  //
  // MEF Registers:
  //
  Offset(47),     MEFE, 8,  // Offset(47),    MEF Enable
  //
  // PCIe Dock Status:
  //
  Offset(48),     DSTS, 8,  // Offset(48),    PCIe Dock Status
  //
  // TPM Registers
  //
  Offset(49),     MORD, 8,  // Offset(49),    Memory Overwrite Request Data
  Offset(50),     TCGP, 8,  // Offset(50),    Used for save the Mor and/or physical presence paramter
  Offset(51),     PPRP, 32, // Offset(51),    Physical Presence request operation response
  Offset(55),     PPRQ, 8,  // Offset(55),    Physical Presence request operation
  Offset(56),     LPPR, 8,  // Offset(56),    Last Physical Presence request operation
  //
  // SATA Registers:
  //
  Offset(57),     IDEM, 8,  // Offset(57),    IDE Mode (Compatible\Enhanced)
  //
  // Board Id
  //
  Offset(58),     PLID, 8,  // Offset(58),    Platform id
  Offset(59),     BTYP, 8,  // Offset(59),    Board Type
  //
  // PCIe Hot Plug
  //
  Offset(60),     OSCC, 8,  // Offset(60),    PCIE OSC Control
  Offset(61),     NEXP, 8,  // Offset(61),    Native PCIE Setup Value
  //
  // USB Sideband Deferring Support
  //
  Offset(62),     SBV1, 8,  // Offset(62),    USB Sideband Deferring GPE Vector (HOST_ALERT#1)
  Offset(63),     SBV2, 8,  // Offset(63),    USB Sideband Deferring GPE Vector (HOST_ALERT#2)
  //
  // Embedded Controller Availability Flag.
  //
  Offset(64),     ECON, 8,  // Offset(64),    Embedded Controller Availability Flag.
  //
  // Global Variables
  //
  Offset(65),     DSEN, 8,  // Offset(65),    _DOS Display Support Flag.
  Offset(66),     GPIC, 8,  // Offset(66),    Global IOAPIC/8259 Interrupt Mode Flag.
  Offset(67),     CTYP, 8,  // Offset(67),    Global Cooling Type Flag.
  Offset(68),     L01C, 8,  // Offset(68),    Global L01 Counter.
  Offset(69),     VFN0, 8,  // Offset(69),    Virtual Fan0 Status.
  Offset(70),     VFN1, 8,  // Offset(70),    Virtual Fan1 Status.
  Offset(71),     VFN2, 8,  // Offset(71),    Virtual Fan2 Status.
  Offset(72),     VFN3, 8,  // Offset(72),    Virtual Fan3 Status.
  Offset(73),     VFN4, 8,  // Offset(73),    Virtual Fan4 Status.
  Offset(74),     VFN5, 8,  // Offset(74),    Virtual Fan5 Status.
  Offset(75),     VFN6, 8,  // Offset(75),    Virtual Fan6 Status.
  Offset(76),     VFN7, 8,  // Offset(76),    Virtual Fan7 Status.
  Offset(77),     VFN8, 8,  // Offset(77),    Virtual Fan8 Status.
  Offset(78),     VFN9, 8,  // Offset(78),    Virtual Fan9 Status.
  //
  // Thermal
  //
  Offset(79),     ATMC, 8,  // Offset(79),    Active Trip Point for MCH
  Offset(80),     PTMC, 8,  // Offset(80),    Passive Trip Point for MCH
  Offset(81),     PNHM, 32, // Offset(81),    CPUID Feature Information [EAX]
  Offset(85),     TBAL, 32, // Offset(85),    Reserved for Thermal Base Low Address for BIOS
  Offset(89),     TBAH, 32, // Offset(89),    Reserved for Thermal Base High Address for BIOS
  Offset(93),     TSOD, 8,  // Offset(93),    TS-on-DIMM is chosen in SETUP and present on the DIMM
  //
  // Board info
  //
  Offset(94),     PFLV, 8,  // Offset(94),    Platform Flavor
  Offset(95),     BREV, 8,  // Offset(95),    Board Rev
  //
  // Package temperature
  //
  Offset(96),     PDTS, 8,  // Offset(96),    Package Temperature
  Offset(97),     PKGA, 8,  // Offset(97),    Package Temperature MSR available
  Offset(98),     PAMT, 8,  // Offset(98),    Peci Access Method
  Offset(99),     AC0F, 8,  // Offset(99),    _AC0 Fan Speed
  Offset(100),    AC1F, 8,  // Offset(100),   _AC1 Fan Speed
  Offset(101),    DTS3, 8,  // Offset(101),   Digital Thermal Sensor 3 Reading
  Offset(102),    DTS4, 8,  // Offset(102),   Digital Thermal Sensor 4 Reading
  //
  // XTU 3.0 Specification
  //
  Offset(103),    XTUB, 32, // Offset(103),   XTU Continous structure Base Address
  Offset(107),    XTUS, 32, // Offset(107),   XMP Size
  Offset(111),    XMPB, 32, // Offset(111),   XMP Base Address
  Offset(115),    DDRF, 8,  // Offset(115),   DDR Reference Frequency
  Offset(116),    RTD3, 8,  // Offset(116),   Runtime D3 support.
  Offset(117),    PEP0, 8,  // Offset(117),   User selctable Delay for Device D0 transition.
  //
  // Intel(R) Dynamic Tuning Devices and trip points
  //
  Offset(118),    DPTF, 8,  // Offset(118),   EnableDptf
  Offset(119),    DCFE, 16, // Offset(119),   EnableDCFG
  Offset(121),    SADE, 8,  // Offset(121),   EnableSaDevice
  Offset(122),    SACT, 8,  // Offset(122),   CriticalThermalTripPointSa
  Offset(123),    SAHT, 8,  // Offset(123),   HotThermalTripPointSa
  Offset(124),    CPUS, 8,  // Offset(124),   ThermalSamplingPeriodSA
  //
  // Intel(R) Dynamic Tuning Policies
  //
  Offset(125),    CTDP, 8,  // Offset(125),   EnableCtdpPolicy
  //
  // DPPM Devices and trip points
  //
  Offset(126),    FND1, 8,  // Offset(126),   EnableFan1Device
  Offset(127),    AMBD, 8,  // Offset(127),   EnableAmbientDevice
  Offset(128),    AMAT, 8,  // Offset(128),   ActiveThermalTripPointAmbient
  Offset(129),    AMPT, 8,  // Offset(129),   PassiveThermalTripPointAmbient
  Offset(130),    AMCT, 8,  // Offset(130),   CriticalThermalTripPointAmbient
  Offset(131),    AMHT, 8,  // Offset(131),   HotThermalTripPointAmbient
  Offset(132),    SKDE, 8,  // Offset(132),   EnableSkinDevice
  Offset(133),    SKAT, 8,  // Offset(133),   ActiveThermalTripPointSkin
  Offset(134),    SKPT, 8,  // Offset(134),   PassiveThermalTripPointSkin
  Offset(135),    SKCT, 8,  // Offset(135),   CriticalThermalTripPointSkin
  Offset(136),    SKHT, 8,  // Offset(136),   HotThermalTripPointSkin
  Offset(137),    EFDE, 8,  // Offset(137),   EnableExhaustFanDevice
  Offset(138),    EFAT, 8,  // Offset(138),   ActiveThermalTripPointExhaustFan
  Offset(139),    EFPT, 8,  // Offset(139),   PassiveThermalTripPointExhaustFan
  Offset(140),    EFCT, 8,  // Offset(140),   CriticalThermalTripPointExhaustFan
  Offset(141),    EFHT, 8,  // Offset(141),   HotThermalTripPointExhaustFan
  Offset(142),    VRDE, 8,  // Offset(142),   EnableVRDevice
  Offset(143),    VRAT, 8,  // Offset(143),   ActiveThermalTripPointVR
  Offset(144),    VRPT, 8,  // Offset(144),   PassiveThermalTripPointVR
  Offset(145),    VRCT, 8,  // Offset(145),   CriticalThermalTripPointVR
  Offset(146),    VRHT, 8,  // Offset(146),   HotThermalTripPointVR
  //
  // CLPO (Current Logical Processor Off lining Setting)
  //
  Offset(147),    LPOE, 8,  // Offset(147),   LPOEnable
  Offset(148),    LPOP, 8,  // Offset(148),   LPOStartPState
  Offset(149),    LPOS, 8,  // Offset(149),   LPOStepSize
  Offset(150),    LPOW, 8,  // Offset(150),   LPOPowerControlSetting
  Offset(151),    LPER, 8,  // Offset(151),   LPOPerformanceControlSetting
  //
  // Miscellaneous Intel(R) Dynamic Tuning
  //
  Offset(152),    PPSZ, 32, // Offset(152),   PPCC Step Size
  Offset(156),    DISE, 8,  // Offset(156),   EnableDisplayParticipant
  //
  // BIOS Guard
  //
  Offset(157),    BGMA, 64, // Offset(157),   BIOS Guard Memory Address for Tool Interface
  Offset(165),    BGMS, 8,  // Offset(165),   BIOS Guard Memory Size for Tool Interface
  Offset(166),    BGIA, 16, // Offset(166),   BIOS Guard IoTrap Address for Tool Interface
  //
  // Never Sleep Technology
  //
  Offset(168),    IRMC, 8,  // Offset(168),   Irmt Configuration
  //
  // Comms Hub
  //
  Offset(169),    CHEN, 8,  // Offset(169),   Comms Hub Enable/Disable
  Offset(170),    S0ID, 8,  // Offset(170),   Low Power S0 Idle Enable
  //
  // BIOS only version of Config TDP
  //
  Offset(171),    CTDB, 8,  // Offset(171),   enable/disable BIOS only version of Config TDP
  Offset(172),    DKSM, 8,  // Offset(172),   Dock SMI number
  //
  // LPC SIO configuration
  //
  Offset(500),              // Offset(173) : Offset(499), Reserved bytes
  Offset(500),    WRFD, 8,  // Offset(500),   EnableWrlsParticipant
  Offset(501),    WRAT, 8,  // Offset(501),   ActiveThermalTripPointWrls
  Offset(502),    WRPT, 8,  // Offset(502),   PassiveThermalTripPointWrls
  Offset(503),    WRCT, 8,  // Offset(503),   CriticalThermalTripPointWrls
  Offset(504),    WRHT, 8,  // Offset(504),   HotThermalTripPointWrls
  Offset(505),    PWRE, 8,  // Offset(505),   EnablePowerParticipant
  Offset(506),        , 16, // Offset(506),   DPTFRsvd0
  Offset(508),    PPPR, 16, // Offset(508),   PowerParticipantPollingRate
  Offset(515),              // Offset(510) : Offset(514), Reserved bytes
  Offset(515),    XHPR, 8,  // Offset(515),   RTD3 USB Power Resource config
  Offset(516),    GBSX, 8,  // Offset(516),   Virtual GPIO button Notify Sleep State Change
  Offset(517),    IUBE, 8,  // Offset(517),   IUER Button Enable
  Offset(518),    IUCE, 8,  // Offset(518),   IUER Convertible Enable
  Offset(519),    IUDE, 8,  // Offset(519),   IUER Dock Enable
  Offset(520),    ECNO, 8,  // Offset(520),   EC Notification of Low Power S0 Idle State
  Offset(521),    AUDD, 16, // Offset(521),   RTD3 Audio Codec device delay
  Offset(523),    IC0D, 16, // Offset(523),   RTD3 SensorHub delay time after applying power to device
  Offset(525),    IC1D, 16, // Offset(525),   RTD3 TouchPanel delay time after applying power to device
  Offset(527),    IC1S, 16, // Offset(527),   RTD3 TouchPad delay time after applying power to device
  Offset(529),    VRRD, 16, // Offset(529),   VR Ramp up delay
  Offset(531),    PSCP, 8,  // Offset(531),   P-state Capping
  Offset(532),    RCG0, 16, // Offset(532),   RTD3 Config Setting0(BIT0:ZPODD, BIT1:Reserved, BIT2:PCIe NVMe, Bit4:SKL SDS SIP I2C Touch, BIT6:Card Reader, BIT7:WWAN)
  Offset(534),    RCG1, 16, // Offset(534),   RTD3 Config Setting1(BIT0:Sata Port0, BIT1:Sata Port1, BIT2:Sata Port2, BIT3:Sata Port3)
  Offset(536),    ECDB, 8,  // Offset(536),   EC Debug Light (CAPS LOCK) for when in Low Power S0 Idle State
  Offset(537),    P2ME, 8,  // Offset(537),   Ps2 Mouse Enable
  Offset(538),    P2MK, 8,  // Offset(538),   Ps2 Keyboard and Mouse Enable
  Offset(539),    WIRC, 8,  // Offset(539),   Enable RTD3 Cold Support for Wifi
  Offset(540),    WWRC, 8,  // Offset(540),   Enable RTD3 Cold Support for WWAN
  Offset(541),    SSH0, 16, // Offset(541),   SSCN-HIGH for I2C0
  Offset(543),    SSL0, 16, // Offset(543),   SSCN-LOW  for I2C0
  Offset(545),    SSD0, 16, // Offset(545),   SSCN-HOLD for I2C0
  Offset(547),    FMH0, 16, // Offset(547),   FMCN-HIGH for I2C0
  Offset(549),    FML0, 16, // Offset(549),   FMCN-LOW  for I2C0
  Offset(551),    FMD0, 16, // Offset(551),   FMCN-HOLD for I2C0
  Offset(553),    FPH0, 16, // Offset(553),   FPCN-HIGH for I2C0
  Offset(555),    FPL0, 16, // Offset(555),   FPCN-LOW  for I2C0
  Offset(557),    FPD0, 16, // Offset(557),   FPCN-HOLD for I2C0
  Offset(559),    HMH0, 16, // Offset(559),   HMCN-HIGH for I2C0
  Offset(561),    HML0, 16, // Offset(561),   HMCN-LOW  for I2C0
  Offset(563),    HMD0, 16, // Offset(563),   HMCN-HOLD for I2C0
  Offset(573),              // Offset(565) : Offset(572), Reserved bytes
  Offset(573),    SSH1, 16, // Offset(573),   SSCN-HIGH for I2C1
  Offset(575),    SSL1, 16, // Offset(575),   SSCN-LOW  for I2C1
  Offset(577),    SSD1, 16, // Offset(577),   SSCN-HOLD for I2C1
  Offset(579),    FMH1, 16, // Offset(579),   FMCN-HIGH for I2C1
  Offset(581),    FML1, 16, // Offset(581),   FMCN-LOW  for I2C1
  Offset(583),    FMD1, 16, // Offset(583),   FMCN-HOLD for I2C1
  Offset(585),    FPH1, 16, // Offset(585),   FPCN-HIGH for I2C1
  Offset(587),    FPL1, 16, // Offset(587),   FPCN-LOW  for I2C1
  Offset(589),    FPD1, 16, // Offset(589),   FPCN-HOLD for I2C1
  Offset(591),    HMH1, 16, // Offset(591),   HMCN-HIGH for I2C1
  Offset(593),    HML1, 16, // Offset(593),   HMCN-LOW  for I2C1
  Offset(595),    HMD1, 16, // Offset(595),   HMCN-HOLD for I2C1
  Offset(598),              // Offset(597) : Offset(597), Reserved bytes
  Offset(598),    SSH2, 16, // Offset(598),   SSCN-HIGH for I2C2
  Offset(600),    SSL2, 16, // Offset(600),   SSCN-LOW  for I2C2
  Offset(602),    SSD2, 16, // Offset(602),   SSCN-HOLD for I2C2
  Offset(604),    FMH2, 16, // Offset(604),   FMCN-HIGH for I2C2
  Offset(606),    FML2, 16, // Offset(606),   FMCN-LOW  for I2C2
  Offset(608),    FMD2, 16, // Offset(608),   FMCN-HOLD for I2C2
  Offset(610),    FPH2, 16, // Offset(610),   FPCN-HIGH for I2C2
  Offset(612),    FPL2, 16, // Offset(612),   FPCN-LOW  for I2C2
  Offset(614),    FPD2, 16, // Offset(614),   FPCN-HOLD for I2C2
  Offset(616),    HMH2, 16, // Offset(616),   HMCN-HIGH for I2C2
  Offset(618),    HML2, 16, // Offset(618),   HMCN-LOW  for I2C2
  Offset(620),    HMD2, 16, // Offset(620),   HMCN-HOLD for I2C2
  Offset(623),              // Offset(622) : Offset(622), Reserved bytes
  Offset(623),    SSH3, 16, // Offset(623),   SSCN-HIGH for I2C3
  Offset(625),    SSL3, 16, // Offset(625),   SSCN-LOW  for I2C3
  Offset(627),    SSD3, 16, // Offset(627),   SSCN-HOLD for I2C3
  Offset(629),    FMH3, 16, // Offset(629),   FMCN-HIGH for I2C3
  Offset(631),    FML3, 16, // Offset(631),   FMCN-LOW  for I2C3
  Offset(633),    FMD3, 16, // Offset(633),   FMCN-HOLD for I2C3
  Offset(635),    FPH3, 16, // Offset(635),   FPCN-HIGH for I2C3
  Offset(637),    FPL3, 16, // Offset(637),   FPCN-LOW  for I2C3
  Offset(639),    FPD3, 16, // Offset(639),   FPCN-HOLD for I2C3
  Offset(641),    HMH3, 16, // Offset(641),   HMCN-HIGH for I2C3
  Offset(643),    HML3, 16, // Offset(643),   HMCN-LOW  for I2C3
  Offset(645),    HMD3, 16, // Offset(645),   HMCN-HOLD for I2C3
  Offset(648),              // Offset(647) : Offset(647), Reserved bytes
  Offset(648),    SSH4, 16, // Offset(648),   SSCN-HIGH for I2C4
  Offset(650),    SSL4, 16, // Offset(650),   SSCN-LOW  for I2C4
  Offset(652),    SSD4, 16, // Offset(652),   SSCN-HOLD for I2C4
  Offset(654),    FMH4, 16, // Offset(654),   FMCN-HIGH for I2C4
  Offset(656),    FML4, 16, // Offset(656),   FMCN-LOW  for I2C4
  Offset(658),    FMD4, 16, // Offset(658),   FMCN-HOLD for I2C4
  Offset(660),    FPH4, 16, // Offset(660),   FPCN-HIGH for I2C4
  Offset(662),    FPL4, 16, // Offset(662),   FPCN-LOW  for I2C4
  Offset(664),    FPD4, 16, // Offset(664),   FPCN-HOLD for I2C4
  Offset(666),    HMH4, 16, // Offset(666),   HMCN-HIGH for I2C4
  Offset(668),    HML4, 16, // Offset(668),   HMCN-LOW  for I2C4
  Offset(670),    HMD4, 16, // Offset(670),   HMCN-HOLD for I2C4
  Offset(673),              // Offset(672) : Offset(672), Reserved bytes
  Offset(673),    SSH5, 16, // Offset(673),   SSCN-HIGH for I2C5
  Offset(675),    SSL5, 16, // Offset(675),   SSCN-LOW  for I2C5
  Offset(677),    SSD5, 16, // Offset(677),   SSCN-HOLD for I2C5
  Offset(679),    FMH5, 16, // Offset(679),   FMCN-HIGH for I2C5
  Offset(681),    FML5, 16, // Offset(681),   FMCN-LOW  for I2C5
  Offset(683),    FMD5, 16, // Offset(683),   FMCN-HOLD for I2C5
  Offset(685),    FPH5, 16, // Offset(685),   FPCN-HIGH for I2C5
  Offset(687),    FPL5, 16, // Offset(687),   FPCN-LOW  for I2C5
  Offset(689),    FPD5, 16, // Offset(689),   FPCN-HOLD for I2C5
  Offset(691),    HMH5, 16, // Offset(691),   HMCN-HIGH for I2C5
  Offset(693),    HML5, 16, // Offset(693),   HMCN-LOW  for I2C5
  Offset(695),    HMD5, 16, // Offset(695),   HMCN-HOLD for I2C5
  Offset(698),              // Offset(697) : Offset(697), Reserved bytes
  Offset(698),    M0C0, 16, // Offset(698),   M0D3 for I2C0
  Offset(700),    M1C0, 16, // Offset(700),   M1D3 for I2C0
  Offset(702),    M0C1, 16, // Offset(702),   M0D3 for I2C1
  Offset(704),    M1C1, 16, // Offset(704),   M1D3 for I2C1
  Offset(706),    M0C2, 16, // Offset(706),   M0D3 for I2C2
  Offset(708),    M1C2, 16, // Offset(708),   M1D3 for I2C2
  Offset(710),    M0C3, 16, // Offset(710),   M0D3 for I2C3
  Offset(712),    M1C3, 16, // Offset(712),   M1D3 for I2C3
  Offset(714),    M0C4, 16, // Offset(714),   M0D3 for I2C4
  Offset(716),    M1C4, 16, // Offset(716),   M1D3 for I2C4
  Offset(718),    M0C5, 16, // Offset(718),   M0D3 for I2C5
  Offset(720),    M1C5, 16, // Offset(720),   M1D3 for I2C5
  Offset(722),    M0C6, 16, // Offset(722),   M0D3 for SPI0
  Offset(724),    M1C6, 16, // Offset(724),   M1D3 for SPI0
  Offset(726),    M0C7, 16, // Offset(726),   M0D3 for SPI1
  Offset(728),    M1C7, 16, // Offset(728),   M1D3 for SPI1
  Offset(730),    M0C8, 16, // Offset(730),   M0D3 for SPI2
  Offset(732),    M1C8, 16, // Offset(732),   M1D3 for SPI2
  Offset(735),              // Offset(734) : Offset(734), Reserved bytes
  Offset(735),    M0C9, 16, // Offset(735),   M0D3 for UART0
  Offset(737),    M1C9, 16, // Offset(737),   M1D3 for UART0
  Offset(739),    M0CA, 16, // Offset(739),   M0D3 for UART1
  Offset(741),    M1CA, 16, // Offset(741),   M1D3 for UART1
  Offset(743),    M0CB, 16, // Offset(743),   M0D3 for UART2
  Offset(745),    M1CB, 16, // Offset(745),   M1D3 for UART2
  Offset(748),              // Offset(747) : Offset(747), Reserved bytes
  //
  // Driver Mode
  //
  Offset(748),    GIRQ, 32, // Offset(748),   GPIO IRQ
  Offset(752),    DMTP, 8,  // Offset(752),   PIRQS 34,50(GPIO)
  Offset(753),    DMTD, 8,  // Offset(753),   PIRQX 39,55(GPIO)
  Offset(754),    DMSH, 8,  // Offset(754),   PIRQM 28,14(GPIO)
  Offset(755),    SHSB, 8,  // Offset(755),   Sensor Standby mode
  Offset(756),    PLCS, 8,  // Offset(756),   set PL1 limit when entering CS
  Offset(757),    PLVL, 16, // Offset(757),   PL1 limit value
  Offset(759),    WWSD, 8,  // Offset(759),   EnableWwanTempSensorDevice
  Offset(760),    CVSD, 8,  // Offset(760),   EnableCpuVrTempSensorDevice
  Offset(761),    SSDD, 8,  // Offset(761),   EnableSsdTempSensorDevice
  Offset(762),    INLD, 8,  // Offset(762),   EnableInletFanTempSensorDevice
  Offset(763),    IFAT, 8,  // Offset(763),   ActiveThermalTripPointInletFan
  Offset(764),    IFPT, 8,  // Offset(764),   PassiveThermalTripPointInletFan
  Offset(765),    IFCT, 8,  // Offset(765),   CriticalThermalTripPointInletFan
  Offset(766),    IFHT, 8,  // Offset(766),   HotThermalTripPointInletFan
  Offset(767),    USBH, 8,  // Offset(767),   Sensor Hub Type - (0)None, (1)USB, (2)I2C Intel, (3)I2C STM
  Offset(768),    BCV4, 8,  // Offset(768),   Broadcom's Bluetooth adapter's revision
  Offset(769),    WTV0, 8,  // Offset(769),   I2C0/WITT devices version
  Offset(770),    WTV1, 8,  // Offset(770),   I2C1/WITT devices version
  Offset(771),    APFU, 8,  // Offset(771),   Atmel panel FW update Enable/Disable
  Offset(778),              // Offset(772) : Offset(777), Reserved bytes
  Offset(778),    PEPC, 32, // Offset(778),   PEP Constraints
  // Bit[1:0] - Storage (0:None, 1:Storage Controller, 2:Raid)
  // Bit[2]   - En/Dis UART0
  // Bit[3]   - En/Dis UART1
  // Bit[4]   - Unused
  // Bit[5]   - En/Dis I2C0
  // Bit[6]   - En/Dis I2C1
  // Bit[7]   - En/Dis XHCI
  // Bit[8]   - En/Dis HD Audio (includes ADSP)
  // Bit[9]   - En/Dis Gfx
  // Bit[10]  - En/Dis CPU
  // Bit[11]  - En/Dis EMMC
  // Bit[12]  - En/Dis SDXC
  // Bit[13]  - En/Dis I2C2
  // Bit[14]  - En/Dis I2C3
  // Bit[15]  - En/Dis I2C4
  // Bit[16]  - En/Dis I2C5
  // Bit[17]  - En/Dis UART2
  // Bit[18]  - En/Dis SPI0
  // Bit[19]  - En/Dis SPI1
  // Bit[20]  - [CNL] En/Dis SPI2
  // Bit[21]  - En/Dis IPU0
  // Bit[22]  - En/Dis CSME
  // Bit[23]  - En/Dis LAN(GBE)
  // Bit[24]  - En/Dis PEG0
  // Bit[25]  - En/Dis TBT RP
  Offset(782),    PB1E, 8,  // Offset(782),   10sec Power button support
  // Bit0: 10 sec P-button Enable/Disable
  // Bit1: Internal Flag
  // Bit2: Rotation Lock flag, 0:unlock, 1:lock
  // Bit3: Slate/Laptop Mode Flag, 0: Slate, 1: Laptop
  // Bit4: Undock / Dock Flag, 0: Undock, 1: Dock
  // Bit5: VBDL Flag. 0: VBDL is not called, 1: VBDL is called, Virtual Button Driver is loaded.
  // Bit7-6: Reserved for future use.
  //
  // Generation Id(Tock/Tick)
  //
  Offset(783),    GNID, 8,  // Offset(783),   Generation Id(0=Shark bay, 1=Crescent Bay)
  //
  // Intel(R) Dynamic Tuning
  //
  Offset(784),    WAND, 8,  // Offset(784),   EnableWWANParticipant
  Offset(785),    WWAT, 8,  // Offset(785),   ActiveThermalTripPointWWAN
  Offset(786),    WWPT, 8,  // Offset(786),   PassiveThermalTripPointWWAN
  Offset(787),    WWCT, 8,  // Offset(787),   CriticalThermalTripPointWWAN
  Offset(788),    WWHT, 8,  // Offset(788),   HotThermalTripPointWWAN
  Offset(807),              // Offset(789) : Offset(806), Reserved bytes
  Offset(807),    MPL0, 16, // Offset(807),   Minimum Power Limit 0 for Intel(R) Dynamic Tuning use via PPCC Object
  Offset(809),    CHGE, 8,  // Offset(809),   EnableChargerParticipant
  Offset(810),    SAC3, 8,  // Offset(810),   CriticalThermalTripPointSaS3
  Offset(811),    AMC3, 8,  // Offset(811),   CriticalThermalTripPointAmbientS3
  Offset(812),    SKC3, 8,  // Offset(812),   CriticalThermalTripPointSkinS3
  Offset(813),    EFC3, 8,  // Offset(813),   CriticalThermalTripPointExhaustFanS3
  Offset(814),    VRC3, 8,  // Offset(814),   CriticalThermalTripPointVRS3
  Offset(815),    WLC3, 8,  // Offset(815),   CriticalThermalTripPointWrlsS3
  Offset(816),    IFC3, 8,  // Offset(816),   CriticalThermalTripPointInletFanS3
  Offset(817),    WWC3, 8,  // Offset(817),   CriticalThermalTripPointWWANS3
  Offset(818),    WGC3, 8,  // Offset(818),   CriticalThermalTripPointWGigS3
  //
  // Intel(R) Dynamic Tuning
  //
  Offset(819),    CA2D, 8,  // Offset(819),   Enable2DCameraParticipant
  Offset(820),    BATR, 8,  // Offset(820),   EnableBatteryParticipant
  Offset(821),    ECLP, 8,  // Offset(821),   EC Low Power Mode: 1 - Enabled, 0 - Disabled
  Offset(822),    SSP1, 8,  // Offset(822),   SensorSamplingPeriodSen1
  Offset(823),    SSP2, 8,  // Offset(823),   SensorSamplingPeriodSen2
  Offset(824),    SSP3, 8,  // Offset(824),   SensorSamplingPeriodSen3
  Offset(825),    SSP4, 8,  // Offset(825),   SensorSamplingPeriodSen4
  Offset(826),    SSP5, 8,  // Offset(826),   SensorSamplingPeriodSen5
  Offset(827),    SGE1, 8,  // Offset(827),   EnableStorageParticipantST1
  Offset(828),    SAT1, 8,  // Offset(828),   ActiveThermalTripPointST1
  Offset(829),    SPT1, 8,  // Offset(829),   PassiveThermalTripPointST1
  Offset(830),    SCT1, 8,  // Offset(830),   CriticalThermalTripPointST1
  Offset(831),    SC31, 8,  // Offset(831),   CriticalThermalTripPointS3ST1
  Offset(832),    SHT1, 8,  // Offset(832),   HotThermalTripPointST1
  Offset(833),    SGE2, 8,  // Offset(833),   EnableStorageParticipantST2
  Offset(834),    SAT2, 8,  // Offset(834),   ActiveThermalTripPointST2
  Offset(835),    SPT2, 8,  // Offset(835),   PassiveThermalTripPointST2
  Offset(836),    SCT2, 8,  // Offset(836),   CriticalThermalTripPointST2
  Offset(837),    SC32, 8,  // Offset(837),   CriticalThermalTripPointS3ST2
  Offset(838),    SHT2, 8,  // Offset(838),   HotThermalTripPointST2
  Offset(839),    VSP1, 8,  // Offset(839),   EnableVS1Participant
  Offset(840),    V1AT, 8,  // Offset(840),   ActiveThermalTripPointVS1
  Offset(841),    V1PV, 8,  // Offset(841),   PassiveThermalTripPointVS1
  Offset(842),    V1CR, 8,  // Offset(842),   CriticalThermalTripPointVS1
  Offset(843),    V1C3, 8,  // Offset(843),   CriticalThermalTripPointVS1S3
  Offset(844),    V1HT, 8,  // Offset(844),   HotThermalTripPointVS1
  Offset(845),    VSP2, 8,  // Offset(845),   EnableVS2Participant
  Offset(846),    V2AT, 8,  // Offset(846),   ActiveThermalTripPointVS2
  Offset(847),    V2PV, 8,  // Offset(847),   PassiveThermalTripPointVS2
  Offset(848),    V2CR, 8,  // Offset(848),   CriticalThermalTripPointVS2
  Offset(849),    V2C3, 8,  // Offset(849),   CriticalThermalTripPointVS2S3
  Offset(850),    V2HT, 8,  // Offset(850),   HotThermalTripPointVS2
  Offset(851),    S1DE, 8,  // Offset(851),   EnableSen1Participant
  Offset(852),    S1AT, 8,  // Offset(852),   ActiveThermalTripPointSen1
  Offset(853),    S1PT, 8,  // Offset(853),   PassiveThermalTripPointSen1
  Offset(854),    S1CT, 8,  // Offset(854),   CriticalThermalTripPointSen1
  Offset(855),    S1HT, 8,  // Offset(855),   HotThermalTripPointSen1
  Offset(856),    S2DE, 8,  // Offset(856),   EnableSen2Participant
  Offset(857),    S2AT, 8,  // Offset(857),   ActiveThermalTripPointSen2
  Offset(858),    S2PT, 8,  // Offset(858),   PassiveThermalTripPointSen2
  Offset(859),    S2CT, 8,  // Offset(859),   CriticalThermalTripPointSen2
  Offset(860),    S2HT, 8,  // Offset(860),   HotThermalTripPointSen2
  Offset(861),    S3DE, 8,  // Offset(861),   EnableSen3Participant
  Offset(862),    S3AT, 8,  // Offset(862),   ActiveThermalTripPointSen3
  Offset(863),    S3PT, 8,  // Offset(863),   PassiveThermalTripPointSen3
  Offset(864),    S3CT, 8,  // Offset(864),   CriticalThermalTripPointSen3
  Offset(865),    S3HT, 8,  // Offset(865),   HotThermalTripPointSen3
  Offset(866),    S4DE, 8,  // Offset(866),   EnableSen4Participant
  Offset(867),    S4AT, 8,  // Offset(867),   ActiveThermalTripPointSen4
  Offset(868),    S4PT, 8,  // Offset(868),   PassiveThermalTripPointSen4
  Offset(869),    S4CT, 8,  // Offset(869),   CriticalThermalTripPointSen4
  Offset(870),    S4HT, 8,  // Offset(870),   HotThermalTripPointSen4
  Offset(871),    S5DE, 8,  // Offset(871),   EnableSen5Participant
  Offset(872),    S5AT, 8,  // Offset(872),   ActiveThermalTripPointSen5
  Offset(873),    S5PT, 8,  // Offset(873),   PassiveThermalTripPointSen5
  Offset(874),    S5CT, 8,  // Offset(874),   CriticalThermalTripPointSen5
  Offset(875),    S5HT, 8,  // Offset(875),   HotThermalTripPointSen5
  Offset(876),    S1S3, 8,  // Offset(876),   CriticalThermalTripPointSen1S3
  Offset(877),    S2S3, 8,  // Offset(877),   CriticalThermalTripPointSen2S3
  Offset(878),    S3S3, 8,  // Offset(878),   CriticalThermalTripPointSen3S3
  Offset(879),    S4S3, 8,  // Offset(879),   CriticalThermalTripPointSen4S3
  Offset(880),    S5S3, 8,  // Offset(880),   CriticalThermalTripPointSen5S3
  Offset(888),              // Offset(881) : Offset(887), Reserved bytes
  Offset(888),    WIFE, 8,  // Offset(888),   WifiEnable
  Offset(889),    DOM1, 8,  // Offset(889),   WifiDomainType1
  Offset(890),    LIM1, 16, // Offset(890),   WifiPowerLimit1
  Offset(892),    TIM1, 32, // Offset(892),   WifiTimeWindow1
  Offset(896),    TRD0, 8,  // Offset(896),   TRxDelay0
  Offset(897),    TRL0, 8,  // Offset(897),   TRxCableLength0
  Offset(898),    TRD1, 8,  // Offset(898),   TRxDelay1
  Offset(899),    TRL1, 8,  // Offset(899),   TRxCableLength1
  Offset(900),    WDM1, 8,  // Offset(900),   WrddDomainType1
  Offset(901),    CID1, 16, // Offset(901),   WrddCountryIndentifier1
  Offset(1018),             // Offset(903) : Offset(1017), Reserved bytes
  Offset(1018),   MPL1, 16, // Offset(1018),  Minimum Power Limit 1 for Intel(R) Dynamic Tuning use via PPCC Object
  Offset(1020),   MPL2, 16, // Offset(1020),  Minimum Power Limit 2 for Intel(R) Dynamic Tuning use via PPCC Object
  //
  // Intel Serial(R) IO Sensor Device Selection
  //
  Offset(1022),   SDS0, 8,  // Offset(1022),  SerialIo Devices for controller0
  Offset(1023),   SDS1, 8,  // Offset(1023),  SerialIo Devices for controller1
  Offset(1024),   SDS2, 8,  // Offset(1024),  SerialIo Devices for controller2
  Offset(1025),   SDS3, 8,  // Offset(1025),  SerialIo Devices for controller3
  Offset(1026),   SDS4, 8,  // Offset(1026),  SerialIo Devices for controller4
  Offset(1027),   SDS5, 8,  // Offset(1027),  SerialIo Devices for controller5
  Offset(1028),   SDS6, 8,  // Offset(1028),  SerialIo Devices for controller6
  Offset(1029),   SDS7, 8,  // Offset(1029),  SerialIo Devices for controller7
  Offset(1030),   SDS8, 8,  // Offset(1030),  SerialIo Devices for controller8
  Offset(1031),   SDS9, 8,  // Offset(1031),  SerialIo Devices for controller9
  Offset(1032),   SDSA, 8,  // Offset(1032),  SerialIo Devices for controller10
  Offset(1033),   WTVX, 8,  // Offset(1033),  WITT test devices' version
  Offset(1034),   WITX, 8,  // Offset(1034),  WITT test devices' connection point
  Offset(1035),   GPTD, 8,  // Offset(1035),  GPIO test devices
  Offset(1036),   GDBT, 16, // Offset(1036),  GPIO test devices' debounce value,
  Offset(1038),   UTKX, 8,  // Offset(1038),  UTK test devices' connection point
  Offset(1039),   SPTD, 8,  // Offset(1039),  SerialIo additional test devices
  Offset(1065),             // Offset(1040) : Offset(1064), Reserved bytes
  Offset(1065),   ATLB, 32, // Offset(1065),  Buffer for runtime ACPI Table loading
  Offset(1069),   SDM0, 8,  // Offset(1069),  interrupt mode for controller0 devices
  Offset(1070),   SDM1, 8,  // Offset(1070),  interrupt mode for controller1 devices
  Offset(1071),   SDM2, 8,  // Offset(1071),  interrupt mode for controller2 devices
  Offset(1072),   SDM3, 8,  // Offset(1072),  interrupt mode for controller3 devices
  Offset(1073),   SDM4, 8,  // Offset(1073),  interrupt mode for controller4 devices
  Offset(1074),   SDM5, 8,  // Offset(1074),  interrupt mode for controller5 devices
  Offset(1075),   SDM6, 8,  // Offset(1075),  interrupt mode for controller6 devices
  Offset(1076),   SDM7, 8,  // Offset(1076),  interrupt mode for controller7 devices
  Offset(1077),   SDM8, 8,  // Offset(1077),  interrupt mode for controller8 devices
  Offset(1078),   SDM9, 8,  // Offset(1078),  interrupt mode for controller9 devices
  Offset(1079),   SDMA, 8,  // Offset(1079),  interrupt mode for controller10 devices
  Offset(1080),   SDMB, 8,  // Offset(1080),  interrupt mode for controller11 devices
  Offset(1083),             // Offset(1081) : Offset(1082), Reserved bytes
  Offset(1083),   USTP, 8,  // Offset(1083),  use SerialIo timing parameters
  Offset(1125),             // Offset(1084) : Offset(1124), Reserved bytes
  Offset(1125),   GFPS, 32, // Offset(1125),  Gpio for fingerprint sleep
  Offset(1129),   GFPI, 32, // Offset(1129),  Gpio for fingerprint irq
  Offset(1133),   GNSC, 8,  // Offset(1133),  GNSS module and its interface, 0=disabled, 1=CG2000 over SerialIo Uart, 2=CG2000 over ISH Uart
  Offset(1134),   GGNR, 32, // Offset(1134),  Gpio for GNSS reset
  Offset(1138),   GBTK, 32, // Offset(1138),  Gpio for Bluetooth RfKill
  Offset(1142),   GBTI, 32, // Offset(1142),  Gpio for Bluetooth interrupt
  Offset(1146),   GPDI, 32, // Offset(1146),  Gpio for touchPaD Interrupt
  Offset(1150),   GPLI, 32, // Offset(1150),  Gpio for touchPaneL Interrupt
  Offset(1154),   DBUS, 8,  // Offset(1154),  Switch to enable BT UART Support
  //
  // MipiCam specific
  //
  Offset(1155),   CL00, 8,  // Offset(1155),
  Offset(1156),   CL01, 8,  // Offset(1156),
  Offset(1157),   CL02, 8,  // Offset(1157),
  Offset(1158),   CL03, 8,  // Offset(1158),
  Offset(1159),   L0EN, 8,  // Offset(1159),
  Offset(1160),   L1EN, 8,  // Offset(1160),
  Offset(1161),   L2EN, 8,  // Offset(1161),
  Offset(1162),   L3EN, 8,  // Offset(1162),
  Offset(1163),   CDIV, 8,  // Offset(1163),
  // Control Logic 0 options
  Offset(1164),   C0VE, 8,  // Offset(1164),  Version of CLDB structure
  Offset(1165),   C0TP, 8,  // Offset(1165),  Type
  Offset(1166),   C0CV, 8,  // Offset(1166),  Version of CRD
  Offset(1167),   C0IC, 32, // Offset(1167),  Input Clock
  Offset(1171),   C0GP, 8,  // Offset(1171),  Number of GPIO Pins enabled
  Offset(1172),   C0IB, 8,  // Offset(1172),  I2C Serial Bus Number
  Offset(1173),   C0IA, 16, // Offset(1173),  I2C Address
  Offset(1175),   C0P0, 8,  // Offset(1175),  GPIO Group Pad Number
  Offset(1176),   C0P1, 8,  // Offset(1176),
  Offset(1177),   C0P2, 8,  // Offset(1177),
  Offset(1178),   C0P3, 8,  // Offset(1178),
  Offset(1179),   C0G0, 8,  // Offset(1179),  GPIO Group Number
  Offset(1180),   C0G1, 8,  // Offset(1180),
  Offset(1181),   C0G2, 8,  // Offset(1181),
  Offset(1182),   C0G3, 8,  // Offset(1182),
  Offset(1183),   C0F0, 8,  // Offset(1183),  GPIO Function
  Offset(1184),   C0F1, 8,  // Offset(1184),
  Offset(1185),   C0F2, 8,  // Offset(1185),
  Offset(1186),   C0F3, 8,  // Offset(1186),
  Offset(1187),   C0A0, 8,  // Offset(1187),  GPIO Active Value
  Offset(1188),   C0A1, 8,  // Offset(1188),
  Offset(1189),   C0A2, 8,  // Offset(1189),
  Offset(1190),   C0A3, 8,  // Offset(1190),
  Offset(1191),   C0I0, 8,  // Offset(1191),  GPIO Initial Value
  Offset(1192),   C0I1, 8,  // Offset(1192),
  Offset(1193),   C0I2, 8,  // Offset(1193),
  Offset(1194),   C0I3, 8,  // Offset(1194),
  Offset(1195),   C0PL, 8,  // Offset(1195),  Camera Position
  Offset(1196),   C0W0, 8,  // Offset(1196),  WLED1 Flash Max Current
  Offset(1197),   C0W1, 8,  // Offset(1197),  WLED1 Torch Max Current
  Offset(1198),   C0W2, 8,  // Offset(1198),  WLED2 Flash Max Current
  Offset(1199),   C0W3, 8,  // Offset(1199),  WLED2 Torch Max Current
  Offset(1200),   C0SP, 8,  // Offset(1200),  Sub Platform Id
  Offset(1201),   C0W4, 8,  // Offset(1201),  WLED1 Type
  Offset(1202),   C0W5, 8,  // Offset(1202),  WLED2 Type
  Offset(1203),   C0CS, 8,  // Offset(1203),  PCH Clock source
  // Control Logic 1 options
  Offset(1204),   C1VE, 8,  // Offset(1204),  Version of CLDB structure
  Offset(1205),   C1TP, 8,  // Offset(1205),  Type
  Offset(1206),   C1CV, 8,  // Offset(1206),  Version of CRD
  Offset(1207),   C1IC, 32, // Offset(1207),  Input Clock
  Offset(1211),   C1GP, 8,  // Offset(1211),  Number of GPIO Pins enabled
  Offset(1212),   C1IB, 8,  // Offset(1212),  I2C Serial Bus Number
  Offset(1213),   C1IA, 16, // Offset(1213),  I2C Address
  Offset(1215),   C1P0, 8,  // Offset(1215),  GPIO Group Pad Number
  Offset(1216),   C1P1, 8,  // Offset(1216),
  Offset(1217),   C1P2, 8,  // Offset(1217),
  Offset(1218),   C1P3, 8,  // Offset(1218),
  Offset(1219),   C1G0, 8,  // Offset(1219),  GPIO Group Number
  Offset(1220),   C1G1, 8,  // Offset(1220),
  Offset(1221),   C1G2, 8,  // Offset(1221),
  Offset(1222),   C1G3, 8,  // Offset(1222),
  Offset(1223),   C1F0, 8,  // Offset(1223),  GPIO Function
  Offset(1224),   C1F1, 8,  // Offset(1224),
  Offset(1225),   C1F2, 8,  // Offset(1225),
  Offset(1226),   C1F3, 8,  // Offset(1226),
  Offset(1227),   C1A0, 8,  // Offset(1227),  GPIO Active Value
  Offset(1228),   C1A1, 8,  // Offset(1228),
  Offset(1229),   C1A2, 8,  // Offset(1229),
  Offset(1230),   C1A3, 8,  // Offset(1230),
  Offset(1231),   C1I0, 8,  // Offset(1231),  GPIO Initial Value
  Offset(1232),   C1I1, 8,  // Offset(1232),
  Offset(1233),   C1I2, 8,  // Offset(1233),
  Offset(1234),   C1I3, 8,  // Offset(1234),
  Offset(1235),   C1PL, 8,  // Offset(1235),  Camera Position
  Offset(1236),   C1W0, 8,  // Offset(1236),  WLED1 Flash Max Current
  Offset(1237),   C1W1, 8,  // Offset(1237),  WLED1 Torch Max Current
  Offset(1238),   C1W2, 8,  // Offset(1238),  WLED2 Flash Max Current
  Offset(1239),   C1W3, 8,  // Offset(1239),  WLED2 Torch Max Current
  Offset(1240),   C1SP, 8,  // Offset(1240),  Sub Platform Id
  Offset(1241),   C1W4, 8,  // Offset(1241),  WLED1 Type
  Offset(1242),   C1W5, 8,  // Offset(1242),  WLED2 Type
  Offset(1243),   C1CS, 8,  // Offset(1243),  PCH Clock source
  // Control Logic 2 options
  Offset(1244),   C2VE, 8,  // Offset(1244),  Version of CLDB structure
  Offset(1245),   C2TP, 8,  // Offset(1245),  Type
  Offset(1246),   C2CV, 8,  // Offset(1246),  Version of CRD
  Offset(1247),   C2IC, 32, // Offset(1247),  Input Clock
  Offset(1251),   C2GP, 8,  // Offset(1251),  Number of GPIO Pins enabled
  Offset(1252),   C2IB, 8,  // Offset(1252),  I2C Serial Bus Number
  Offset(1253),   C2IA, 16, // Offset(1253),  I2C Address
  Offset(1255),   C2P0, 8,  // Offset(1255),  GPIO Group Pad Number
  Offset(1256),   C2P1, 8,  // Offset(1256),
  Offset(1257),   C2P2, 8,  // Offset(1257),
  Offset(1258),   C2P3, 8,  // Offset(1258),
  Offset(1259),   C2G0, 8,  // Offset(1259),  GPIO Group Number
  Offset(1260),   C2G1, 8,  // Offset(1260),
  Offset(1261),   C2G2, 8,  // Offset(1261),
  Offset(1262),   C2G3, 8,  // Offset(1262),
  Offset(1263),   C2F0, 8,  // Offset(1263),  GPIO Function
  Offset(1264),   C2F1, 8,  // Offset(1264),
  Offset(1265),   C2F2, 8,  // Offset(1265),
  Offset(1266),   C2F3, 8,  // Offset(1266),
  Offset(1267),   C2A0, 8,  // Offset(1267),  GPIO Active Value
  Offset(1268),   C2A1, 8,  // Offset(1268),
  Offset(1269),   C2A2, 8,  // Offset(1269),
  Offset(1270),   C2A3, 8,  // Offset(1270),
  Offset(1271),   C2I0, 8,  // Offset(1271),  GPIO Initial Value
  Offset(1272),   C2I1, 8,  // Offset(1272),
  Offset(1273),   C2I2, 8,  // Offset(1273),
  Offset(1274),   C2I3, 8,  // Offset(1274),
  Offset(1275),   C2PL, 8,  // Offset(1275),  Camera Position
  Offset(1276),   C2W0, 8,  // Offset(1276),  WLED1 Flash Max Current
  Offset(1277),   C2W1, 8,  // Offset(1277),  WLED1 Torch Max Current
  Offset(1278),   C2W2, 8,  // Offset(1278),  WLED2 Flash Max Current
  Offset(1279),   C2W3, 8,  // Offset(1279),  WLED2 Torch Max Current
  Offset(1280),   C2SP, 8,  // Offset(1280),  Sub Platform Id
  Offset(1281),   C2W4, 8,  // Offset(1281),  WLED1 Type
  Offset(1282),   C2W5, 8,  // Offset(1282),  WLED2 Type
  Offset(1283),   C2CS, 8,  // Offset(1283),  PCH Clock source
  // Control Logic 3 options
  Offset(1284),   C3VE, 8,  // Offset(1284),  Version of CLDB structure
  Offset(1285),   C3TP, 8,  // Offset(1285),  Type
  Offset(1286),   C3CV, 8,  // Offset(1286),  Version of CRD
  Offset(1287),   C3IC, 32, // Offset(1287),  Input Clock
  Offset(1291),   C3GP, 8,  // Offset(1291),  Number of GPIO Pins enabled
  Offset(1292),   C3IB, 8,  // Offset(1292),  I2C Serial Bus Number
  Offset(1293),   C3IA, 16, // Offset(1293),  I2C Address
  Offset(1295),   C3P0, 8,  // Offset(1295),  GPIO Group Pad Number
  Offset(1296),   C3P1, 8,  // Offset(1296),
  Offset(1297),   C3P2, 8,  // Offset(1297),
  Offset(1298),   C3P3, 8,  // Offset(1298),
  Offset(1299),   C3G0, 8,  // Offset(1299),  GPIO Group Number
  Offset(1300),   C3G1, 8,  // Offset(1300),
  Offset(1301),   C3G2, 8,  // Offset(1301),
  Offset(1302),   C3G3, 8,  // Offset(1302),
  Offset(1303),   C3F0, 8,  // Offset(1303),  GPIO Function
  Offset(1304),   C3F1, 8,  // Offset(1304),
  Offset(1305),   C3F2, 8,  // Offset(1305),
  Offset(1306),   C3F3, 8,  // Offset(1306),
  Offset(1307),   C3A0, 8,  // Offset(1307),  GPIO Active Value
  Offset(1308),   C3A1, 8,  // Offset(1308),
  Offset(1309),   C3A2, 8,  // Offset(1309),
  Offset(1310),   C3A3, 8,  // Offset(1310),
  Offset(1311),   C3I0, 8,  // Offset(1311),  GPIO Initial Value
  Offset(1312),   C3I1, 8,  // Offset(1312),
  Offset(1313),   C3I2, 8,  // Offset(1313),
  Offset(1314),   C3I3, 8,  // Offset(1314),
  Offset(1315),   C3PL, 8,  // Offset(1315),  Camera Position
  Offset(1316),   C3W0, 8,  // Offset(1316),  WLED1 Flash Max Current
  Offset(1317),   C3W1, 8,  // Offset(1317),  WLED1 Torch Max Current
  Offset(1318),   C3W2, 8,  // Offset(1318),  WLED2 Flash Max Current
  Offset(1319),   C3W3, 8,  // Offset(1319),  WLED2 Torch Max Current
  Offset(1320),   C3SP, 8,  // Offset(1320),  Sub Platform Id
  Offset(1321),   C3W4, 8,  // Offset(1321),  WLED1 Type
  Offset(1322),   C3W5, 8,  // Offset(1322),  WLED2 Type
  Offset(1323),   C3CS, 8,  // Offset(1323),  PCH Clock source
  // Mipi Cam Link0 options
  Offset(1324),   L0SM, 8,  // Offset(1324),  Sensor Model
  Offset(1325),   L0H0, 8,  // Offset(1325),  User defined HID ASCII character 0
  Offset(1326),   L0H1, 8,  // Offset(1326),
  Offset(1327),   L0H2, 8,  // Offset(1327),
  Offset(1328),   L0H3, 8,  // Offset(1328),
  Offset(1329),   L0H4, 8,  // Offset(1329),
  Offset(1330),   L0H5, 8,  // Offset(1330),
  Offset(1331),   L0H6, 8,  // Offset(1331),
  Offset(1332),   L0H7, 8,  // Offset(1332),
  Offset(1333),   L0H8, 8,  // Offset(1333),  User defined HID ASCII character 8
  Offset(1334),   L0PL, 8,  // Offset(1334),  Camera Position
  Offset(1335),   L0M0, 8,  // Offset(1335),  Camera Module Name ASCII character 0
  Offset(1336),   L0M1, 8,  // Offset(1336),
  Offset(1337),   L0M2, 8,  // Offset(1337),
  Offset(1338),   L0M3, 8,  // Offset(1338),
  Offset(1339),   L0M4, 8,  // Offset(1339),
  Offset(1340),   L0M5, 8,  // Offset(1340),
  Offset(1341),   L0M6, 8,  // Offset(1341),
  Offset(1342),   L0M7, 8,  // Offset(1342),
  Offset(1343),   L0M8, 8,  // Offset(1343),
  Offset(1344),   L0M9, 8,  // Offset(1344),
  Offset(1345),   L0MA, 8,  // Offset(1345),
  Offset(1346),   L0MB, 8,  // Offset(1346),
  Offset(1347),   L0MC, 8,  // Offset(1347),
  Offset(1348),   L0MD, 8,  // Offset(1348),
  Offset(1349),   L0ME, 8,  // Offset(1349),
  Offset(1350),   L0MF, 8,  // Offset(1350),  Camera Module Name ASCII character 15
  Offset(1351),   L0DI, 8,  // Offset(1351),  Number of I2C devices
  Offset(1352),   L0BS, 8,  // Offset(1352),  I2C Serial Bus number
  Offset(1353),   L0A0, 16, // Offset(1353),  Address of I2C Device0 on Link0
  Offset(1355),   L0A1, 16, // Offset(1355),  Address of I2C Device1 on Link0
  Offset(1357),   L0A2, 16, // Offset(1357),  Address of I2C Device2 on Link0
  Offset(1359),   L0A3, 16, // Offset(1359),  Address of I2C Device3 on Link0
  Offset(1361),   L0A4, 16, // Offset(1361),  Address of I2C Device4 on Link0
  Offset(1363),   L0A5, 16, // Offset(1363),  Address of I2C Device5 on Link0
  Offset(1365),   L0A6, 16, // Offset(1365),  Address of I2C Device6 on Link0
  Offset(1367),   L0A7, 16, // Offset(1367),  Address of I2C Device7 on Link0
  Offset(1369),   L0A8, 16, // Offset(1369),  Address of I2C Device8 on Link0
  Offset(1371),   L0A9, 16, // Offset(1371),  Address of I2C Device9 on Link0
  Offset(1373),   L0AA, 16, // Offset(1373),  Address of I2C Device10 on Link0
  Offset(1375),   L0AB, 16, // Offset(1375),  Address of I2C Device11 on Link0
  Offset(1377),   L0D0, 8,  // Offset(1377),  Type of I2C Device0 on Link0
  Offset(1378),   L0D1, 8,  // Offset(1378),  Type of I2C Device1 on Link0
  Offset(1379),   L0D2, 8,  // Offset(1379),  Type of I2C Device2 on Link0
  Offset(1380),   L0D3, 8,  // Offset(1380),  Type of I2C Device3 on Link0
  Offset(1381),   L0D4, 8,  // Offset(1381),  Type of I2C Device4 on Link0
  Offset(1382),   L0D5, 8,  // Offset(1382),  Type of I2C Device5 on Link0
  Offset(1383),   L0D6, 8,  // Offset(1383),  Type of I2C Device6 on Link0
  Offset(1384),   L0D7, 8,  // Offset(1384),  Type of I2C Device7 on Link0
  Offset(1385),   L0D8, 8,  // Offset(1385),  Type of I2C Device8 on Link0
  Offset(1386),   L0D9, 8,  // Offset(1386),  Type of I2C Device9 on Link0
  Offset(1387),   L0DA, 8,  // Offset(1387),  Type of I2C Device10 on Link0
  Offset(1388),   L0DB, 8,  // Offset(1388),  Type of I2C Device11 on Link0
  Offset(1389),   L0DV, 8,  // Offset(1389),  Version of SSDB structure
  Offset(1390),   L0CV, 8,  // Offset(1390),  Version of CRD
  Offset(1391),   L0LU, 8,  // Offset(1391),  CSI2 Link used
  Offset(1392),   L0NL, 8,  // Offset(1392),  MIPI-CSI2 Data Lane
  Offset(1393),   L0EE, 8,  // Offset(1393),  EEPROM Type
  Offset(1394),   L0VC, 8,  // Offset(1394),  VCM Type
  Offset(1395),   L0FS, 8,  // Offset(1395),  Flash Support
  Offset(1396),   L0LE, 8,  // Offset(1396),  Privacy LED
  Offset(1397),   L0DG, 8,  // Offset(1397),  Degree
  Offset(1398),   L0CK, 32, // Offset(1398),  MCLK
  Offset(1402),   L0CL, 8,  // Offset(1402),  Control Logic
  Offset(1403),   L0PP, 8,  // Offset(1403),  PMIC Position
  Offset(1404),   L0VR, 8,  // Offset(1404),  Voltage Rail
  // Mipi Cam Link1 options
  Offset(1405),   L1SM, 8,  // Offset(1405),  Sensor Model
  Offset(1406),   L1H0, 8,  // Offset(1406),  User defined HID ASCII character 0
  Offset(1407),   L1H1, 8,  // Offset(1407),
  Offset(1408),   L1H2, 8,  // Offset(1408),
  Offset(1409),   L1H3, 8,  // Offset(1409),
  Offset(1410),   L1H4, 8,  // Offset(1410),
  Offset(1411),   L1H5, 8,  // Offset(1411),
  Offset(1412),   L1H6, 8,  // Offset(1412),
  Offset(1413),   L1H7, 8,  // Offset(1413),
  Offset(1414),   L1H8, 8,  // Offset(1414),  User defined HID ASCII character 8
  Offset(1415),   L1PL, 8,  // Offset(1415),  Camera Position
  Offset(1416),   L1M0, 8,  // Offset(1416),  Camera Module Name ASCII character 0
  Offset(1417),   L1M1, 8,  // Offset(1417),
  Offset(1418),   L1M2, 8,  // Offset(1418),
  Offset(1419),   L1M3, 8,  // Offset(1419),
  Offset(1420),   L1M4, 8,  // Offset(1420),
  Offset(1421),   L1M5, 8,  // Offset(1421),
  Offset(1422),   L1M6, 8,  // Offset(1422),
  Offset(1423),   L1M7, 8,  // Offset(1423),
  Offset(1424),   L1M8, 8,  // Offset(1424),
  Offset(1425),   L1M9, 8,  // Offset(1425),
  Offset(1426),   L1MA, 8,  // Offset(1426),
  Offset(1427),   L1MB, 8,  // Offset(1427),
  Offset(1428),   L1MC, 8,  // Offset(1428),
  Offset(1429),   L1MD, 8,  // Offset(1429),
  Offset(1430),   L1ME, 8,  // Offset(1430),
  Offset(1431),   L1MF, 8,  // Offset(1431),  Camera Module Name ASCII character 15
  Offset(1432),   L1DI, 8,  // Offset(1432),  Number of I2C devices
  Offset(1433),   L1BS, 8,  // Offset(1433),  I2C Serial Bus number
  Offset(1434),   L1A0, 16, // Offset(1434),  Address of I2C Device0 on Link1
  Offset(1436),   L1A1, 16, // Offset(1436),  Address of I2C Device1 on Link1
  Offset(1438),   L1A2, 16, // Offset(1438),  Address of I2C Device2 on Link1
  Offset(1440),   L1A3, 16, // Offset(1440),  Address of I2C Device3 on Link1
  Offset(1442),   L1A4, 16, // Offset(1442),  Address of I2C Device4 on Link1
  Offset(1444),   L1A5, 16, // Offset(1444),  Address of I2C Device5 on Link1
  Offset(1446),   L1A6, 16, // Offset(1446),  Address of I2C Device6 on Link1
  Offset(1448),   L1A7, 16, // Offset(1448),  Address of I2C Device7 on Link1
  Offset(1450),   L1A8, 16, // Offset(1450),  Address of I2C Device8 on Link1
  Offset(1452),   L1A9, 16, // Offset(1452),  Address of I2C Device9 on Link1
  Offset(1454),   L1AA, 16, // Offset(1454),  Address of I2C Device10 on Link1
  Offset(1456),   L1AB, 16, // Offset(1456),  Address of I2C Device11 on Link1
  Offset(1458),   L1D0, 8,  // Offset(1458),  Type of I2C Device0 on Link1
  Offset(1459),   L1D1, 8,  // Offset(1459),  Type of I2C Device1 on Link1
  Offset(1460),   L1D2, 8,  // Offset(1460),  Type of I2C Device2 on Link1
  Offset(1461),   L1D3, 8,  // Offset(1461),  Type of I2C Device3 on Link1
  Offset(1462),   L1D4, 8,  // Offset(1462),  Type of I2C Device4 on Link1
  Offset(1463),   L1D5, 8,  // Offset(1463),  Type of I2C Device5 on Link1
  Offset(1464),   L1D6, 8,  // Offset(1464),  Type of I2C Device6 on Link1
  Offset(1465),   L1D7, 8,  // Offset(1465),  Type of I2C Device7 on Link1
  Offset(1466),   L1D8, 8,  // Offset(1466),  Type of I2C Device8 on Link1
  Offset(1467),   L1D9, 8,  // Offset(1467),  Type of I2C Device9 on Link1
  Offset(1468),   L1DA, 8,  // Offset(1468),  Type of I2C Device10 on Link1
  Offset(1469),   L1DB, 8,  // Offset(1469),  Type of I2C Device11 on Link1
  Offset(1470),   L1DV, 8,  // Offset(1470),  Version of SSDB structure
  Offset(1471),   L1CV, 8,  // Offset(1471),  Version of CRD
  Offset(1472),   L1LU, 8,  // Offset(1472),  CSI2 Link used
  Offset(1473),   L1NL, 8,  // Offset(1473),  MIPI-CSI2 Data Lane
  Offset(1474),   L1EE, 8,  // Offset(1474),  EEPROM Type
  Offset(1475),   L1VC, 8,  // Offset(1475),  VCM Type
  Offset(1476),   L1FS, 8,  // Offset(1476),  Flash Support
  Offset(1477),   L1LE, 8,  // Offset(1477),  Privacy LED
  Offset(1478),   L1DG, 8,  // Offset(1478),  Degree
  Offset(1479),   L1CK, 32, // Offset(1479),  MCLK
  Offset(1483),   L1CL, 8,  // Offset(1483),  Control Logic
  Offset(1484),   L1PP, 8,  // Offset(1484),  PMIC Position
  Offset(1485),   L1VR, 8,  // Offset(1485),  Voltage Rail
  // Mipi Cam Link2 options
  Offset(1486),   L2SM, 8,  // Offset(1486),  Sensor Model
  Offset(1487),   L2H0, 8,  // Offset(1487),  User defined HID ASCII character 0
  Offset(1488),   L2H1, 8,  // Offset(1488),
  Offset(1489),   L2H2, 8,  // Offset(1489),
  Offset(1490),   L2H3, 8,  // Offset(1490),
  Offset(1491),   L2H4, 8,  // Offset(1491),
  Offset(1492),   L2H5, 8,  // Offset(1492),
  Offset(1493),   L2H6, 8,  // Offset(1493),
  Offset(1494),   L2H7, 8,  // Offset(1494),
  Offset(1495),   L2H8, 8,  // Offset(1495),  User defined HID ASCII character 8
  Offset(1496),   L2PL, 8,  // Offset(1496),  Camera Position
  Offset(1497),   L2M0, 8,  // Offset(1497),  Camera Module Name ASCII character 0
  Offset(1498),   L2M1, 8,  // Offset(1498),
  Offset(1499),   L2M2, 8,  // Offset(1499),
  Offset(1500),   L2M3, 8,  // Offset(1500),
  Offset(1501),   L2M4, 8,  // Offset(1501),
  Offset(1502),   L2M5, 8,  // Offset(1502),
  Offset(1503),   L2M6, 8,  // Offset(1503),
  Offset(1504),   L2M7, 8,  // Offset(1504),
  Offset(1505),   L2M8, 8,  // Offset(1505),
  Offset(1506),   L2M9, 8,  // Offset(1506),
  Offset(1507),   L2MA, 8,  // Offset(1507),
  Offset(1508),   L2MB, 8,  // Offset(1508),
  Offset(1509),   L2MC, 8,  // Offset(1509),
  Offset(1510),   L2MD, 8,  // Offset(1510),
  Offset(1511),   L2ME, 8,  // Offset(1511),
  Offset(1512),   L2MF, 8,  // Offset(1512),  Camera Module Name ASCII character 15
  Offset(1513),   L2DI, 8,  // Offset(1513),  Number of I2C devices
  Offset(1514),   L2BS, 8,  // Offset(1514),  I2C Serial Bus number
  Offset(1515),   L2A0, 16, // Offset(1515),  Address of I2C Device0 on Link2
  Offset(1517),   L2A1, 16, // Offset(1517),  Address of I2C Device1 on Link2
  Offset(1519),   L2A2, 16, // Offset(1519),  Address of I2C Device2 on Link2
  Offset(1521),   L2A3, 16, // Offset(1521),  Address of I2C Device3 on Link2
  Offset(1523),   L2A4, 16, // Offset(1523),  Address of I2C Device4 on Link2
  Offset(1525),   L2A5, 16, // Offset(1525),  Address of I2C Device5 on Link2
  Offset(1527),   L2A6, 16, // Offset(1527),  Address of I2C Device6 on Link2
  Offset(1529),   L2A7, 16, // Offset(1529),  Address of I2C Device7 on Link2
  Offset(1531),   L2A8, 16, // Offset(1531),  Address of I2C Device8 on Link2
  Offset(1533),   L2A9, 16, // Offset(1533),  Address of I2C Device9 on Link2
  Offset(1535),   L2AA, 16, // Offset(1535),  Address of I2C Device10 on Link2
  Offset(1537),   L2AB, 16, // Offset(1537),  Address of I2C Device11 on Link2
  Offset(1539),   L2D0, 8,  // Offset(1539),  Type of I2C Device0 on Link2
  Offset(1540),   L2D1, 8,  // Offset(1540),  Type of I2C Device1 on Link2
  Offset(1541),   L2D2, 8,  // Offset(1541),  Type of I2C Device2 on Link2
  Offset(1542),   L2D3, 8,  // Offset(1542),  Type of I2C Device3 on Link2
  Offset(1543),   L2D4, 8,  // Offset(1543),  Type of I2C Device4 on Link2
  Offset(1544),   L2D5, 8,  // Offset(1544),  Type of I2C Device5 on Link2
  Offset(1545),   L2D6, 8,  // Offset(1545),  Type of I2C Device6 on Link2
  Offset(1546),   L2D7, 8,  // Offset(1546),  Type of I2C Device7 on Link2
  Offset(1547),   L2D8, 8,  // Offset(1547),  Type of I2C Device8 on Link2
  Offset(1548),   L2D9, 8,  // Offset(1548),  Type of I2C Device9 on Link2
  Offset(1549),   L2DA, 8,  // Offset(1549),  Type of I2C Device10 on Link2
  Offset(1550),   L2DB, 8,  // Offset(1550),  Type of I2C Device11 on Link2
  Offset(1551),   L2DV, 8,  // Offset(1551),  Version of SSDB structure
  Offset(1552),   L2CV, 8,  // Offset(1552),  Version of CRD
  Offset(1553),   L2LU, 8,  // Offset(1553),  CSI2 Link used
  Offset(1554),   L2NL, 8,  // Offset(1554),  MIPI-CSI2 Data Lane
  Offset(1555),   L2EE, 8,  // Offset(1555),  EEPROM Type
  Offset(1556),   L2VC, 8,  // Offset(1556),  VCM Type
  Offset(1557),   L2FS, 8,  // Offset(1557),  Flash Support
  Offset(1558),   L2LE, 8,  // Offset(1558),  Privacy LED
  Offset(1559),   L2DG, 8,  // Offset(1559),  Degree
  Offset(1560),   L2CK, 32, // Offset(1560),  MCLK
  Offset(1564),   L2CL, 8,  // Offset(1564),  Control Logic
  Offset(1565),   L2PP, 8,  // Offset(1565),  PMIC Position
  Offset(1566),   L2VR, 8,  // Offset(1566),  Voltage Rail
  // Mipi Cam Link3 options
  Offset(1567),   L3SM, 8,  // Offset(1567),  Sensor Model
  Offset(1568),   L3H0, 8,  // Offset(1568),  User defined HID ASCII character 0
  Offset(1569),   L3H1, 8,  // Offset(1569),
  Offset(1570),   L3H2, 8,  // Offset(1570),
  Offset(1571),   L3H3, 8,  // Offset(1571),
  Offset(1572),   L3H4, 8,  // Offset(1572),
  Offset(1573),   L3H5, 8,  // Offset(1573),
  Offset(1574),   L3H6, 8,  // Offset(1574),
  Offset(1575),   L3H7, 8,  // Offset(1575),
  Offset(1576),   L3H8, 8,  // Offset(1576),  User defined HID ASCII character 8
  Offset(1577),   L3PL, 8,  // Offset(1577),  Camera Position
  Offset(1578),   L3M0, 8,  // Offset(1578),  Camera Module Name ASCII character 0
  Offset(1579),   L3M1, 8,  // Offset(1579),
  Offset(1580),   L3M2, 8,  // Offset(1580),
  Offset(1581),   L3M3, 8,  // Offset(1581),
  Offset(1582),   L3M4, 8,  // Offset(1582),
  Offset(1583),   L3M5, 8,  // Offset(1583),
  Offset(1584),   L3M6, 8,  // Offset(1584),
  Offset(1585),   L3M7, 8,  // Offset(1585),
  Offset(1586),   L3M8, 8,  // Offset(1586),
  Offset(1587),   L3M9, 8,  // Offset(1587),
  Offset(1588),   L3MA, 8,  // Offset(1588),
  Offset(1589),   L3MB, 8,  // Offset(1589),
  Offset(1590),   L3MC, 8,  // Offset(1590),
  Offset(1591),   L3MD, 8,  // Offset(1591),
  Offset(1592),   L3ME, 8,  // Offset(1592),
  Offset(1593),   L3MF, 8,  // Offset(1593),  Camera Module Name ASCII character 15
  Offset(1594),   L3DI, 8,  // Offset(1594),  Number of I2C devices
  Offset(1595),   L3BS, 8,  // Offset(1595),  I2C Serial Bus number
  Offset(1596),   L3A0, 16, // Offset(1596),  Address of I2C Device0 on Link3
  Offset(1598),   L3A1, 16, // Offset(1598),  Address of I2C Device1 on Link3
  Offset(1600),   L3A2, 16, // Offset(1600),  Address of I2C Device2 on Link3
  Offset(1602),   L3A3, 16, // Offset(1602),  Address of I2C Device3 on Link3
  Offset(1604),   L3A4, 16, // Offset(1604),  Address of I2C Device4 on Link3
  Offset(1606),   L3A5, 16, // Offset(1606),  Address of I2C Device5 on Link3
  Offset(1608),   L3A6, 16, // Offset(1608),  Address of I2C Device6 on Link3
  Offset(1610),   L3A7, 16, // Offset(1610),  Address of I2C Device7 on Link3
  Offset(1612),   L3A8, 16, // Offset(1612),  Address of I2C Device8 on Link3
  Offset(1614),   L3A9, 16, // Offset(1614),  Address of I2C Device9 on Link3
  Offset(1616),   L3AA, 16, // Offset(1616),  Address of I2C Device10 on Link3
  Offset(1618),   L3AB, 16, // Offset(1618),  Address of I2C Device11 on Link3
  Offset(1620),   L3D0, 8,  // Offset(1620),  Type of I2C Device0 on Link3
  Offset(1621),   L3D1, 8,  // Offset(1621),  Type of I2C Device1 on Link3
  Offset(1622),   L3D2, 8,  // Offset(1622),  Type of I2C Device2 on Link3
  Offset(1623),   L3D3, 8,  // Offset(1623),  Type of I2C Device3 on Link3
  Offset(1624),   L3D4, 8,  // Offset(1624),  Type of I2C Device4 on Link3
  Offset(1625),   L3D5, 8,  // Offset(1625),  Type of I2C Device5 on Link3
  Offset(1626),   L3D6, 8,  // Offset(1626),  Type of I2C Device6 on Link3
  Offset(1627),   L3D7, 8,  // Offset(1627),  Type of I2C Device7 on Link3
  Offset(1628),   L3D8, 8,  // Offset(1628),  Type of I2C Device8 on Link3
  Offset(1629),   L3D9, 8,  // Offset(1629),  Type of I2C Device9 on Link3
  Offset(1630),   L3DA, 8,  // Offset(1630),  Type of I2C Device10 on Link3
  Offset(1631),   L3DB, 8,  // Offset(1631),  Type of I2C Device11 on Link3
  Offset(1632),   L3DV, 8,  // Offset(1632),  Version of SSDB structure
  Offset(1633),   L3CV, 8,  // Offset(1633),  Version of CRD
  Offset(1634),   L3LU, 8,  // Offset(1634),  CSI2 Link used
  Offset(1635),   L3NL, 8,  // Offset(1635),  MIPI-CSI2 Data Lane
  Offset(1636),   L3EE, 8,  // Offset(1636),  EEPROM Type
  Offset(1637),   L3VC, 8,  // Offset(1637),  VCM Type
  Offset(1638),   L3FS, 8,  // Offset(1638),  Flash Support
  Offset(1639),   L3LE, 8,  // Offset(1639),  Privacy LED
  Offset(1640),   L3DG, 8,  // Offset(1640),  Degree
  Offset(1641),   L3CK, 32, // Offset(1641),  MCLK
  Offset(1645),   L3CL, 8,  // Offset(1645),  Control Logic
  Offset(1646),   L3PP, 8,  // Offset(1646),  PMIC Position
  Offset(1647),   L3VR, 8,  // Offset(1647),  Voltage Rail
  Offset(1650),             // Offset(1648) : Offset(1649), Reserved bytes
  Offset(1650),   ECR1, 8,  // Offset(1650),
  Offset(1651),   I2SC, 8,  // Offset(1651),  HD Audio I2S Codec Selection
  Offset(1652),   I2SI, 32, // Offset(1652),  HD Audio I2S Codec Interrupt Pin
  Offset(1656),   I2SB, 8,  // Offset(1656),  HD Audio I2S Codec Connection to I2C bus controller instance (I2C[0-5])
  Offset(1657),   ODV0, 8,  // Offset(1657),  Intel(R) Dynamic Tuning Oem Design Variables
  Offset(1658),   ODV1, 8,  // Offset(1658),  Intel(R) Dynamic Tuning Oem Design Variables
  Offset(1659),   ODV2, 8,  // Offset(1659),  Intel(R) Dynamic Tuning Oem Design Variables
  Offset(1660),   ODV3, 8,  // Offset(1660),  Intel(R) Dynamic Tuning Oem Design Variables
  Offset(1661),   ODV4, 8,  // Offset(1661),  Intel(R) Dynamic Tuning Oem Design Variables
  Offset(1662),   ODV5, 8,  // Offset(1662),  Intel(R) Dynamic Tuning Oem Design Variables
  Offset(1663),   UBCB, 32, // Offset(1663),  USB Type C Opregion base address
  Offset(1672),             // Offset(1667) : Offset(1671), Reserved bytes
  Offset(1672),   WIFC, 8,  // Offset(1672),  WirelessCharging
  // RTD3 Settings
  Offset(1680),             // Offset(1673) : Offset(1679), Reserved bytes
  Offset(1680),   TPLT, 8,  // Offset(1680),  I2C SerialIo Devices Type of TouchPanel
  Offset(1681),   TPLM, 8,  // Offset(1681),  I2C SerialIo Devices Interrupt Mode for TouchPanel
  Offset(1682),   TPLB, 8,  // Offset(1682),  I2C Custom TouchPanel's BUS Address
  Offset(1683),   TPLH, 16, // Offset(1683),  I2C Custom TouchPanel's HID Address
  Offset(1685),   TPLS, 8,  // Offset(1685),  I2C Custom TouchPanel's BUS Speed
  Offset(1686),   TPDT, 8,  // Offset(1686),  I2C SerialIo Devices Type of TouchPad
  Offset(1687),   TPDM, 8,  // Offset(1687),  I2C SerialIo Devices Interrupt Mode for TouchPad
  Offset(1688),   TPDB, 8,  // Offset(1688),  I2C Custom TouchPad's BUS Address
  Offset(1689),   TPDH, 16, // Offset(1689),  I2C Custom TouchPad's HID Address
  Offset(1691),   TPDS, 8,  // Offset(1691),  I2C Custom TouchPad's BUS Speed
  Offset(1692),   FPTT, 8,  // Offset(1692),  SPI SerialIo Devices Type of FingerPrint
  Offset(1693),   FPTM, 8,  // Offset(1693),  SPI SerialIo Devices Interrupt Mode for FingerPrint
  Offset(1694),   ADPM, 32, // Offset(1694),  HD-Audio DSP Post-Processing Module Mask
  Offset(1698),   AG1L, 64, // Offset(1698),  HDA PP module custom GUID 1 - first 64bit  [0-63]
  Offset(1706),   AG1H, 64, // Offset(1706),  HDA PP module custom GUID 1 - second 64bit [64-127]
  Offset(1714),   AG2L, 64, // Offset(1714),  HDA PP module custom GUID 2 - first 64bit  [0-63]
  Offset(1722),   AG2H, 64, // Offset(1722),  HDA PP module custom GUID 2 - second 64bit [64-127]
  Offset(1730),   AG3L, 64, // Offset(1730),  HDA PP module custom GUID 3 - first 64bit  [0-63]
  Offset(1738),   AG3H, 64, // Offset(1738),  HDA PP module custom GUID 3 - second 64bit [64-127]
  Offset(1746),   HEFE, 8,  // Offset(1746),  HID Event Filter Driver enable
  Offset(1747),   XDCE, 8,  // Offset(1747),  XDCI Enable/Disable status
  Offset(1748),   STXE, 8,  // Offset(1748),  WrdsWiFiSarEnable
  Offset(1749),   ST10, 8,  // Offset(1749),  WrdsWiFiSarTxPowerSet1Limit1
  Offset(1750),   ST11, 8,  // Offset(1750),  WrdsWiFiSarTxPowerSet1Limit2
  Offset(1751),   ST12, 8,  // Offset(1751),  WrdsWiFiSarTxPowerSet1Limit3
  Offset(1752),   ST13, 8,  // Offset(1752),  WrdsWiFiSarTxPowerSet1Limit4
  Offset(1753),   ST14, 8,  // Offset(1753),  WrdsWiFiSarTxPowerSet1Limit5
  Offset(1754),   ST15, 8,  // Offset(1754),  WrdsWiFiSarTxPowerSet1Limit6
  Offset(1755),   ST16, 8,  // Offset(1755),  WrdsWiFiSarTxPowerSet1Limit7
  Offset(1756),   ST17, 8,  // Offset(1756),  WrdsWiFiSarTxPowerSet1Limit8
  Offset(1757),   ST18, 8,  // Offset(1757),  WrdsWiFiSarTxPowerSet1Limit9
  Offset(1758),   ST19, 8,  // Offset(1758),  WrdsWiFiSarTxPowerSet1Limit10
  Offset(1759),   ENVM, 8,  // Offset(1759),  Enable Voltage Margining
  Offset(1760),   DHSP, 16, // Offset(1760),  D-State for xHCI HS port(BIT0:USB HS Port0 ~ BIT15:USB HS Port15)
  Offset(1762),   DSSP, 16, // Offset(1762),  D-State for xHCI SS port(BIT0:USB SS Port0 ~ BIT15:USB SS Port15)
  Offset(1764),   DSTP, 8,  // Offset(1764),  D-State for SATA port(BIT0:SATA Port0 ~ BIT7:SATA Port7)
  Offset(1765),   STDE, 8,  // Offset(1765),  EwrdWiFiDynamicSarEnable
  Offset(1766),   STRS, 8,  // Offset(1766),  EwrdWiFiDynamicSarRangeSets
  Offset(1767),   ST20, 8,  // Offset(1767),  EwrdWiFiSarTxPowerSet2Limit1
  Offset(1768),   ST21, 8,  // Offset(1768),  EwrdWiFiSarTxPowerSet2Limit2
  Offset(1769),   ST22, 8,  // Offset(1769),  EwrdWiFiSarTxPowerSet2Limit3
  Offset(1770),   ST23, 8,  // Offset(1770),  EwrdWiFiSarTxPowerSet2Limit4
  Offset(1771),   ST24, 8,  // Offset(1771),  EwrdWiFiSarTxPowerSet2Limit5
  Offset(1772),   ST25, 8,  // Offset(1772),  EwrdWiFiSarTxPowerSet2Limit6
  Offset(1773),   ST26, 8,  // Offset(1773),  EwrdWiFiSarTxPowerSet2Limit7
  Offset(1774),   ST27, 8,  // Offset(1774),  EwrdWiFiSarTxPowerSet2Limit8
  Offset(1775),   ST28, 8,  // Offset(1775),  EwrdWiFiSarTxPowerSet2Limit9
  Offset(1776),   ST29, 8,  // Offset(1776),  EwrdWiFiSarTxPowerSet2Limit10
  Offset(1777),   ST30, 8,  // Offset(1777),  EwrdWiFiSarTxPowerSet3Limit1
  Offset(1778),   ST31, 8,  // Offset(1778),  EwrdWiFiSarTxPowerSet3Limit2
  Offset(1779),   ST32, 8,  // Offset(1779),  EwrdWiFiSarTxPowerSet3Limit3
  Offset(1780),   ST33, 8,  // Offset(1780),  EwrdWiFiSarTxPowerSet3Limit4
  Offset(1781),   ST34, 8,  // Offset(1781),  EwrdWiFiSarTxPowerSet3Limit5
  Offset(1782),   ST35, 8,  // Offset(1782),  EwrdWiFiSarTxPowerSet3Limit6
  Offset(1783),   ST36, 8,  // Offset(1783),  EwrdWiFiSarTxPowerSet3Limit7
  Offset(1784),   ST37, 8,  // Offset(1784),  EwrdWiFiSarTxPowerSet3Limit8
  Offset(1785),   ST38, 8,  // Offset(1785),  EwrdWiFiSarTxPowerSet3Limit9
  Offset(1786),   ST39, 8,  // Offset(1786),  EwrdWiFiSarTxPowerSet3Limit10
  Offset(1787),   ST40, 8,  // Offset(1787),  EwrdWiFiSarTxPowerSet4Limit1
  Offset(1788),   ST41, 8,  // Offset(1788),  EwrdWiFiSarTxPowerSet4Limit2
  Offset(1789),   ST42, 8,  // Offset(1789),  EwrdWiFiSarTxPowerSet4Limit3
  Offset(1790),   ST43, 8,  // Offset(1790),  EwrdWiFiSarTxPowerSet4Limit4
  Offset(1791),   ST44, 8,  // Offset(1791),  EwrdWiFiSarTxPowerSet4Limit5
  Offset(1792),   ST45, 8,  // Offset(1792),  EwrdWiFiSarTxPowerSet4Limit6
  Offset(1793),   ST46, 8,  // Offset(1793),  EwrdWiFiSarTxPowerSet4Limit7
  Offset(1794),   ST47, 8,  // Offset(1794),  EwrdWiFiSarTxPowerSet4Limit8
  Offset(1795),   ST48, 8,  // Offset(1795),  EwrdWiFiSarTxPowerSet4Limit9
  Offset(1796),   ST49, 8,  // Offset(1796),  EwrdWiFiSarTxPowerSet4Limit10
  Offset(1797),   SD11, 8,  // Offset(1797),  WgdsWiFiSarDeltaGroup1PowerMax1
  Offset(1798),   SD12, 8,  // Offset(1798),  WgdsWiFiSarDeltaGroup1PowerChainA1
  Offset(1799),   SD13, 8,  // Offset(1799),  WgdsWiFiSarDeltaGroup1PowerChainB1
  Offset(1800),   SD14, 8,  // Offset(1800),  WgdsWiFiSarDeltaGroup1PowerMax2
  Offset(1801),   SD15, 8,  // Offset(1801),  WgdsWiFiSarDeltaGroup1PowerChainA2
  Offset(1802),   SD16, 8,  // Offset(1802),  WgdsWiFiSarDeltaGroup1PowerChainB2
  Offset(1803),   SD21, 8,  // Offset(1803),  WgdsWiFiSarDeltaGroup2PowerMax1
  Offset(1804),   SD22, 8,  // Offset(1804),  WgdsWiFiSarDeltaGroup2PowerChainA1
  Offset(1805),   SD23, 8,  // Offset(1805),  WgdsWiFiSarDeltaGroup2PowerChainB1
  Offset(1806),   SD24, 8,  // Offset(1806),  WgdsWiFiSarDeltaGroup2PowerMax2
  Offset(1807),   SD25, 8,  // Offset(1807),  WgdsWiFiSarDeltaGroup2PowerChainA2
  Offset(1808),   SD26, 8,  // Offset(1808),  WgdsWiFiSarDeltaGroup2PowerChainB2
  Offset(1809),   SD31, 8,  // Offset(1809),  WgdsWiFiSarDeltaGroup3PowerMax1
  Offset(1810),   SD32, 8,  // Offset(1810),  WgdsWiFiSarDeltaGroup3PowerChainA1
  Offset(1811),   SD33, 8,  // Offset(1811),  WgdsWiFiSarDeltaGroup3PowerChainB1
  Offset(1812),   SD34, 8,  // Offset(1812),  WgdsWiFiSarDeltaGroup3PowerMax2
  Offset(1813),   SD35, 8,  // Offset(1813),  WgdsWiFiSarDeltaGroup3PowerChainA2
  Offset(1814),   SD36, 8,  // Offset(1814),  WgdsWiFiSarDeltaGroup3PowerChainB2
  Offset(1842),             // Offset(1815) : Offset(1841), Reserved bytes
  // Reserved for Groups 4 to 9, each needs 6 bytes and total 36 bytes reserved
  Offset(1842),   SDAA, 8,  // Offset(1842),  WiFiDynamicSarAntennaACurrentSet
  Offset(1843),   SDAB, 8,  // Offset(1843),  WiFiDynamicSarAntennaBCurrentSet
  Offset(1844),   BTSE, 8,  // Offset(1844),  BluetoothSar
  Offset(1845),   BTBR, 8,  // Offset(1845),  BluetoothSarBr
  Offset(1846),   BED2, 8,  // Offset(1846),  BluetoothSarEdr2
  Offset(1847),   BED3, 8,  // Offset(1847),  BluetoothSarEdr3
  Offset(1848),   BTLE, 8,  // Offset(1848),  BluetoothSarLe
  Offset(1853),             // Offset(1849) : Offset(1852), Reserved bytes
  // Reserved for Bluetooth Sar future use
  Offset(1853),   COEM, 8,  // Offset(1853),  CoExistenceManager
  Offset(1854),   RTVM, 8,  // Offset(1854),  RunTime VM Control
  //
  //Feature Specific Data Bits
  //
  Offset(1855),   USTC, 8,  // Offset(1855),  USB Type C Supported
  Offset(1856),   UTCE, 8,  // Offset(1856),  USB Type C EC Less
  Offset(1857),   HEB1, 32, // Offset(1857),  HebcValue
  Offset(1861),   BATP, 8,  // Offset(1861),  Battery Present - Bit0: Real Battery is supported on this platform. Bit1: Virtual Battery is supported on this platform.
  Offset(1862),   TSDB, 8,  // Offset(1862),  TS-on-DIMM temperature
  Offset(1865),             // Offset(1863) : Offset(1864), Reserved bytes
  Offset(1865),   RBY1, 8,  // Offset(1865),  Real Battery 1 Control
  Offset(1866),   RBY2, 8,  // Offset(1866),  Real Battery 2 Control
  Offset(1867),   SCSS, 8,  // Offset(1867),  Mipi Camera Sensor
  Offset(1868),   NCTC, 8,  // Offset(1868),  NCT6776F COM
  Offset(1869),   NCTI, 8,  // Offset(1869),  NCT6776F SIO
  Offset(1870),   NCTH, 8,  // Offset(1870),  NCT6776F HWMON
  Offset(1871),   HSIO, 8,  // Offset(1871),  H8S2113 SIO
  Offset(1872),   RGBC, 8,  // Offset(1872),  RGB Camera Address
  Offset(1873),   DPTC, 8,  // Offset(1873),  Depth Camera Addresy
  Offset(1874),   SRSP, 32, // Offset(1874),  SMC Runtime Sci Pin
  Offset(1878),   CEDS, 8,  // Offset(1878),  Convertable Dock Support
  Offset(1879),   EHK3, 8,  // Offset(1879),  Ec Hotkey F3 Support
  Offset(1880),   EHK4, 8,  // Offset(1880),  Ec Hotkey F4 Support
  Offset(1881),   EHK5, 8,  // Offset(1881),  Ec Hotkey F5 Support
  Offset(1882),   EHK6, 8,  // Offset(1882),  Ec Hotkey F6 Support
  Offset(1883),   EHK7, 8,  // Offset(1883),  Ec Hotkey F7 Support
  Offset(1884),   EHK8, 8,  // Offset(1884),  Ec Hotkey F8 Support
  Offset(1885),   VBVP, 8,  // Offset(1885),  Virtual Button Volume Up Support
  Offset(1886),   VBVD, 8,  // Offset(1886),  Virtual Button Volume Down Support
  Offset(1887),   VBHB, 8,  // Offset(1887),  Virtual Button Home Button Support
  Offset(1888),   VBRL, 8,  // Offset(1888),  Virtual Button Rotation Lock Support
  Offset(1889),   SMSS, 8,  // Offset(1889),  Slate Mode Switch Support
  Offset(1890),   ADAS, 8,  // Offset(1890),  Ac Dc Auto Switch Support
  Offset(1891),   PPBG, 32, // Offset(1891),  Pm Power Button Gpio Pin
  Offset(1895),   AEAB, 8,  // Offset(1895),  Acpi Enable All Button Support
  Offset(1896),   AHDB, 8,  // Offset(1896),  Acpi Hid Driver Button Support
  Offset(1897),   DPLL, 8,  // Offset(1897),  Intel(R) Dynamic Tuning Display Depth Lower Limit in percent
  Offset(1898),   DPHL, 8,  // Offset(1898),  Intel(R) Dynamic Tuning Display Depth Upper Limit in percent
  Offset(1899),   AUPL, 32, // Offset(1899),  Maximum aux power available for PCIe root ports
  Offset(1917),             // Offset(1903) : Offset(1916), Reserved bytes
  Offset(1917),   WTSP, 8,  // Offset(1917),  ThermalSamplingPeriodWrls
  Offset(1918),   ELPM, 32, // Offset(1918),  EcLowPowerModeGpioPin
  Offset(1922),   ELPS, 32, // Offset(1922),  EcSmiGpioPin
  //
  // UCMC setup option, GPIO Pad
  //
  Offset(1926),   UCSI, 8,  // Offset(1926),  Option to Enable/Disable UCSI/UCMC device
  Offset(1927),   UCG1, 32, // Offset(1927),  Gpio for UCMC Port 1 Interrupt
  Offset(1931),   UCG2, 32, // Offset(1931),  Gpio for UCMC Port 2 Interrupt
  Offset(1935),   UCG3, 32, // Offset(1935),  Gpio for UCMC Port 3 Interrupt
  Offset(1939),   UCG4, 32, // Offset(1939),  Gpio for UCMC Port 4 Interrupt
  Offset(1959),             // Offset(1943) : Offset(1958), Reserved bytes
  Offset(1959),   SRLD, 8,  // Offset(1959),  Serial Port ACPI debug
  Offset(1960),   TTUP, 8,  // Offset(1960),  Total Number of type C ports that are supported by platform
  Offset(1961),   TP1T, 8,  // Offset(1961),  Type C Connector 1  Port mapping within the controller the port exposed
  Offset(1962),   TP1P, 8,  // Offset(1962),  Type C Connector 1  Port mapping within the PCH controller (If Split mode supported)
  Offset(1963),   TP1D, 8,  // Offset(1963),  Type C Connector 1  Portperties Split Support/Controller(PCH/TBT/CPU)/Root port (vaild for TBT)
  Offset(1964),   TP2T, 8,  // Offset(1964),  Type C Connector 2  Port mapping within the controller the port exposed
  Offset(1965),   TP2P, 8,  // Offset(1965),  Type C Connector 2  Port mapping within the PCH controller (If Split mode supported)
  Offset(1966),   TP2D, 8,  // Offset(1966),  Type C Connector 2  Portperties Split Support/Controller(PCH/TBT/CPU)/Root port (vaild for TBT)
  Offset(1967),   TP3T, 8,  // Offset(1967),  Type C Connector 3  Port mapping within the controller the port exposed
  Offset(1968),   TP3P, 8,  // Offset(1968),  Type C Connector 3  Port mapping within the PCH controller (If Split mode supported)
  Offset(1969),   TP3D, 8,  // Offset(1969),  Type C Connector 3  Portperties Split Support/Controller(PCH/TBT/CPU)/Root port (vaild for TBT)
  Offset(1970),   TP4T, 8,  // Offset(1970),  Type C Connector 4  Port mapping within the controller the port exposed
  Offset(1971),   TP4P, 8,  // Offset(1971),  Type C Connector 4  Port mapping within the PCH controller (If Split mode supported)
  Offset(1972),   TP4D, 8,  // Offset(1972),  Type C Connector 4  Portperties Split Support/Controller(PCH/TBT/CPU)/Root port (vaild for TBT)
  Offset(1973),   TP5T, 8,  // Offset(1973),  Type C Connector 5  Port mapping within the controller the port exposed
  Offset(1974),   TP5P, 8,  // Offset(1974),  Type C Connector 5  Port mapping within the PCH controller (If Split mode supported)
  Offset(1975),   TP5D, 8,  // Offset(1975),  Type C Connector 5  Portperties Split Support/Controller(PCH/TBT/CPU)/Root port (vaild for TBT)
  Offset(1976),   TP6T, 8,  // Offset(1976),  Type C Connector 6  Port mapping within the controller the port exposed
  Offset(1977),   TP6P, 8,  // Offset(1977),  Type C Connector 6  Port mapping within the PCH controller (If Split mode supported)
  Offset(1978),   TP6D, 8,  // Offset(1978),  Type C Connector 6  Portperties Split Support/Controller(PCH/TBT/CPU)/Root port (vaild for TBT)
  Offset(1979),   UDGF, 8,  // Offset(1979),  Upstream Facing port or Downstream Facing port Global Flag from LPC EC
  Offset(1980),   UDUP, 8,  // Offset(1980),  Upstream Facing port or Downstream Facing port number from LPC EC
  Offset(1981),   DBGF, 8,  // Offset(1981),  Debug Mode Global Flag from LPC EC
  Offset(1982),   DBUP, 8,  // Offset(1982),  Debug Mode USB Port Number from LPC EC
  //
  // XTU SMI base address
  //
  Offset(1983),   XSMI, 32, // Offset(1983),  XTU SMI structure Base Address
  // SlpS0WithGBESupport Setup variable is main switch for support of Sleep S0 with GBE
  // and used in _DSM method supported by the Driver
  Offset(1987),   GSEN, 8,  // Offset(1987),  Enable/Disable the System to achieve SLP_S0 even with LAN(GBE) connected.
  Offset(1988),   ATDV, 8,  // Offset(1988),  AntennaDiversity
  Offset(1989),   BTL2, 8,  // Offset(1989),  BluetoothSarLe2Mhz
  Offset(1990),   BTLL, 8,  // Offset(1990),  BluetoothSarLeLr
  Offset(1991),   POME, 8,  // Offset(1991),  Enable PowerMeter
  Offset(1992),   WRTO, 8,  // Offset(1992),  WWAN RTD3 options
  Offset(1993),   PRST, 32, // Offset(1993),  WWAN PERST Gpio pin
  Offset(1997),   WPRP, 8,  // Offset(1997),  WWAN PERST Gpio polarity
  Offset(1998),   PUIS, 8,  // Offset(1998),  Power Up In Standby mode
  Offset(1999),   WDCT, 16, // Offset(1999),  WWAN Device Control register save area
  Offset(2001),   WLCT, 16, // Offset(2001),  WWAN Link Control register save area
  Offset(2003),   WDC2, 16, // Offset(2003),  WWAN Device Control register save area
  Offset(2005),   WMXS, 16, // Offset(2005),  WWAN LTR Max Snoop Latency save area
  Offset(2007),   WMNS, 16, // Offset(2007),  WWAN LTR Max No Snoop Latency save area
  Offset(2009),   ESPC, 8,  // Offset(2009),  Set to indicate to PEP that constraints at SATA ports should be enumerated
  Offset(2010),   UCRT, 8,  // Offset(2010),  UcsiRetryEC
  Offset(2011),   TJMX, 8,  // Offset(2011),  TjMax Temperature
  Offset(2012),   STAS, 8,  // Offset(2012),  Select source for System time and alarm
  Offset(2013),   SXI1, 8,  // Offset(2013),  SDEV xHCI Interface Number for device 1
  Offset(2014),   SXI2, 8,  // Offset(2014),  SDEV xHCI Interface Number for device 2
  Offset(2015),   SXP1, 8,  // Offset(2015),  SDEV xHCI Root Port Number for device 1
  Offset(2016),   SXP2, 8,  // Offset(2016),  SDEV xHCI Root Port Number for device 2
  Offset(2017),   WAGE, 8,  // Offset(2017),  Wifi Ant Gain Enable
  Offset(2018),   AGA1, 8,  // Offset(2018),  Ant Gain Table Chain A 2400
  Offset(2019),   AGA2, 8,  // Offset(2019),  Ant Gain Table Chain A 5150-5350
  Offset(2020),   AGA3, 8,  // Offset(2020),  Ant Gain Table Chain A 5350-5470
  Offset(2021),   AGA4, 8,  // Offset(2021),  Ant Gain Table Chain A 5470-5725
  Offset(2022),   AGA5, 8,  // Offset(2022),  Ant Gain Table Chain A 5725-5950
  Offset(2023),   AGB1, 8,  // Offset(2023),  Ant Gain Table Chain B 2400
  Offset(2024),   AGB2, 8,  // Offset(2024),  Ant Gain Table Chain B 5150-5350
  Offset(2025),   AGB3, 8,  // Offset(2025),  Ant Gain Table Chain B 5350-5470
  Offset(2026),   AGB4, 8,  // Offset(2026),  Ant Gain Table Chain B 5470-5725
  Offset(2027),   AGB5, 8,  // Offset(2027),  Ant Gain Table Chain B 5725-5950
  Offset(2028),   CECV, 32, // Offset(2028),  CNV external 32KHz Clock
  Offset(2032),   WLGP, 32, // Offset(2032),  WLAN PEWAKE Gpio pin
  Offset(2036),   WLRP, 8,  // Offset(2036),  WLAN Root Port Number
  }
