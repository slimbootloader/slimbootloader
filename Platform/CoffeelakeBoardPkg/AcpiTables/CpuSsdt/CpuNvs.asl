/**@file

  Copyright (c) 2018 - 2019, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

  //
  // Define CPU NVS Area operation region.
  //



  OperationRegion(PNVS,SystemMemory,0xFFFF0000,0xAA55)
  Field(PNVS,AnyAcc,Lock,Preserve)
  {  Offset(0),      PGRV, 8,  // Offset(0),     CPU GlobalNvs Revision
  Offset(1),      CFGD, 32, // Offset(1),     PPM Flags Values
  Offset(6),                // Offset(5) : Offset(5), Reserved bytes
  Offset(6),      ACRT, 8,  // Offset(6),     Auto Critical Trip Point
  Offset(7),      APSV, 8,  // Offset(7),     Auto Passive Trip Point
  Offset(8),      AAC0, 8,  // Offset(8),     Auto Active Trip Point
  Offset(9),      CPID, 32, // Offset(9),     CPUID
  Offset(13),     CPPC, 8,  // Offset(13),    Boot Mode vlues for _PPC
  Offset(14),     CLVL, 8,  // Offset(14),    ConfigTdp Number Of Levels
  Offset(15),     CBMI, 8,  // Offset(15),    CTDP Boot Mode Index
  Offset(16),     PL10, 16, // Offset(16),    CTDP Level 0 Power Limit1
  Offset(18),     PL11, 16, // Offset(18),    CTDP Level 1 Power Limit1
  Offset(20),     PL12, 16, // Offset(20),    CTDP Level 2 Power Limit1
  Offset(22),     PL20, 16, // Offset(22),    CTDP Level 0 Power Limit2
  Offset(24),     PL21, 16, // Offset(24),    CTDP Level 1 Power Limit2
  Offset(26),     PL22, 16, // Offset(26),    CTDP Level 2 Power Limit2
  Offset(28),     PLW0, 8,  // Offset(28),    CTDP Level 0 Power Limit1 Time Window
  Offset(29),     PLW1, 8,  // Offset(29),    CTDP Level 1 Power Limit1 Time Window
  Offset(30),     PLW2, 8,  // Offset(30),    CTDP Level 2 Power Limit1 Time Window
  Offset(31),     CTC0, 8,  // Offset(31),    CTDP Level 0 CTC
  Offset(32),     CTC1, 8,  // Offset(32),    CTDP Level 1 CTC
  Offset(33),     CTC2, 8,  // Offset(33),    CTDP Level 2 CTC
  Offset(34),     TAR0, 8,  // Offset(34),    CTDP Level 0 TAR
  Offset(35),     TAR1, 8,  // Offset(35),    CTDP Level 1 TAR
  Offset(36),     TAR2, 8,  // Offset(36),    CTDP Level 2 TAR
  Offset(37),     PPC0, 8,  // Offset(37),    CTDP Level 0 PPC
  Offset(38),     PPC1, 8,  // Offset(38),    CTDP Level 1 PPC
  Offset(39),     PPC2, 8,  // Offset(39),    CTDP Level 2 PPC
  Offset(41),               // Offset(40) : Offset(40), Reserved bytes
  Offset(41),     C6MW, 8,  // Offset(41),    Mwait Hint value for C6
  Offset(42),     C7MW, 8,  // Offset(42),    Mwait Hint value for C7/C7s
  Offset(43),     CDMW, 8,  // Offset(43),    Mwait Hint value for C7/C8/C9/C10
  Offset(46),               // Offset(44) : Offset(45), Reserved bytes
  Offset(46),     C6LT, 16, // Offset(46),    Latency Value for C6
  Offset(48),     C7LT, 16, // Offset(48),    Latency Value for C7/C7S
  Offset(50),     CDLT, 16, // Offset(50),    Latency Value for C8/C9/C10
  Offset(52),     CDLV, 16, // Offset(52),    IO LVL value for C8/C9/C10
  Offset(54),     CDPW, 16, // Offset(54),    Power value for C8/C9/C10
  Offset(56),     MPMF, 8,  // Offset(56),    MiscPowerManagementFlags
  Offset(57),     DTSE, 8,  // Offset(57),    Digital Thermal Sensor Enable
  Offset(58),     DTS1, 8,  // Offset(58),    Digital Thermal Sensor 1 Readingn for BSP
  Offset(59),     DTS2, 8,  // Offset(59),    Digital Thermal Sensor 2 Reading for AP1
  Offset(60),     DTSF, 8,  // Offset(60),    DTS SMI Function Call via DTS IO Trap
  Offset(61),     PDTS, 8,  // Offset(61),    Package Temperature
  Offset(62),     PKGA, 8,  // Offset(62),    Package Temperature MSR available
  Offset(63),     DTS3, 8,  // Offset(63),    Digital Thermal Sensor 3 Reading for AP2
  Offset(64),     DTS4, 8,  // Offset(64),    Digital Thermal Sensor 4 Reading for AP3
  Offset(65),     BGMA, 64, // Offset(65),    BIOS Guard Memory Address for Tool Interface
  Offset(73),     BGMS, 8,  // Offset(73),    BIOS Guard Memory Size for Tool Interface
  Offset(74),     BGIA, 16, // Offset(74),    BIOS Guard IoTrap Address for Tool Interface
  Offset(76),     BGIL, 16, // Offset(76),    BIOS Guard IoTrap Length for Tool Interface
  Offset(78),     DSIA, 16, // Offset(78),    DTS IO trap Address
  Offset(80),     DSIL, 8,  // Offset(80),    DTS IO trap Length
  Offset(81),     DSAE, 8,  // Offset(81),    DTS is in ACPI Mode Enabled
  Offset(82),     EPCS, 8,  // Offset(82),    SGX Status
  Offset(83),     EMNA, 64, // Offset(83),    EPC Base Address
  Offset(91),     ELNG, 64, // Offset(91),    EPC Length
  Offset(99),     HWPV, 8,  // Offset(99),    HWP Version
  Offset(100),    HWPI, 8,  // Offset(100),   HWP Interrupt Status
  Offset(101),    DTSI, 8,  // Offset(101),   DTS Interrupt Status
  Offset(102),    HWPS, 8,  // Offset(102),   SMI to setup HWP LVT tables
  Offset(103),    LMPS, 8,  // Offset(103),   Max ratio of the slowest core.
  Offset(104),    ITBM, 8,  // Offset(104),   Enable/Disable Intel Turbo Boost Max Technology 3.0.
  Offset(105),    ITBD, 8,  // Offset(105),   Enable/Disable Intel Turbo Boost Max Technology 3.0 Driver.
  Offset(106),    ITBI, 8,  // Offset(106),   Intel Turbo Boost Max Technology 3.0 interrupt status.
  Offset(107),    ITBS, 8,  // Offset(107),   SMI to resume periodic SMM for Intel Turbo Boost Max Technology 3.0.
  Offset(108),    OBIN, 8,  // Offset(108),   Indicates bins of Oc support. MSR 194h FLEX_RATIO Bits (19:17)
  #ifdef CPU_CFL
  Offset(109),    C3MW, 8,  // Offset(109),   Mwait Hint value for C3 (CFL only)
  Offset(110),    C3LT, 16, // Offset(110),   Latency Value for C3 (CFL only)
  #endif
  }
