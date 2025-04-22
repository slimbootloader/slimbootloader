/** @file
  Intel Processor CTDP ACPI Code to enable CTDP without using a driver or EC.
  This is the BIOS only solution.

  Copyright (c) 1999 - 2020, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent
**/

DefinitionBlock (
  "CTDP.aml",
  "SSDT",
  0x02,
  "CtdpB",
  "CtdpB",
  0x1000
  )
{
External(\_SB.OSCP, IntObj)
External(\TCNT, FieldUnitObj)
External(\PNHM, FieldUnitObj)
External(\_SB.PC00, DeviceObj)
External(\_SB.PC00.GMHB, MethodObj)
External(\_SB.PR00, DeviceObj)
External(\_SB.PR01, DeviceObj)
External(\_SB.PR02, DeviceObj)
External(\_SB.PR03, DeviceObj)
External(\_SB.PR04, DeviceObj)
External(\_SB.PR05, DeviceObj)
External(\_SB.PR06, DeviceObj)
External(\_SB.PR07, DeviceObj)
External(\_SB.PR08, DeviceObj)
External(\_SB.PR09, DeviceObj)
External(\_SB.PR10, DeviceObj)
External(\_SB.PR11, DeviceObj)
External(\_SB.PR12, DeviceObj)
External(\_SB.PR13, DeviceObj)
External(\_SB.PR14, DeviceObj)
External(\_SB.PR15, DeviceObj)
External(\_SB.PR16, DeviceObj)
External(\_SB.PR17, DeviceObj)
External(\_SB.PR18, DeviceObj)
External(\_SB.PR19, DeviceObj)
External(\_SB.PR20, DeviceObj)
External(\_SB.PR21, DeviceObj)
External(\_SB.PR22, DeviceObj)
External(\_SB.PR23, DeviceObj)
External(\_SB.PR00._PPC, MethodObj)
External(\_SB.CPPC, IntObj)
External(\_SB.PR00._PSS, MethodObj)
External(\_SB.PR00.LPSS, PkgObj)
External(\_SB.PR00.TPSS, PkgObj)

Scope(\_SB.PC00)
{
  //
  // Memory window to the CTDP registers starting at MCHBAR+5000h.
  //
  OperationRegion (CPWR, SystemMemory, Add(\_SB.PC00.GMHB(),0x5000), 0x1000)
  Field (CPWR, ByteAcc, NoLock, Preserve)
  {
    Offset (0x930), // PACKAGE_POWER_SKU (MCHBAR+0x5930)
    PTDP, 15,       // TDP Package Power [14:0]
    ,      1,       // reserved [15]
    PMIN, 15,       // Minimal Package Power [30:16]
    ,      1,       // Reserved [31]
    PMAX, 15,       // Maximal Package Power [46:32]
    ,      1,       // Reserved [47]
    TMAX,  7,       // Maximal Time Window [54:48]
    Offset (0x938), // PACKAGE_POWER_SKU_UNIT (MCHBAR+0x5938)
    PWRU,  4,       // Power Units [3:0]
    ,      4,       // Reserved [7:4]
    EGYU,  5,       // Energy Units [12:8]
    ,      3,       // Reserved [15:13]
    TIMU,  4,       // Time Units [19:16]
    Offset (0x958), // PLATFORM_INFO (MCHBAR+0x5958)
    , 32,       // [31:0]
    LPMS,  1,       // LPM Support [32]
    CTNL,  2,       // CONFIG_TDP_NUM_LEVELS [34:33]
    Offset (0x9A0), // TURBO_POWER_LIMIT1 (MCHBAR+0x59A0)
    PPL1, 15,       // PKG_PWR_LIM_1 [14:0]
    PL1E,1,         // PKG_PWR_LIM1_EN [15]
    ,      1,       // reserved [16]
    PL1T, 7,        // PKG_PWR_LIM_1_TIME [23:17]
    Offset (0x9A4), // TURBO_POWER_LIMIT2 (MCHBAR+0x59A4)
    PPL2, 15,       // PKG_PWR_LIM_2 [14:0]
    PL2E,1,         // PKG_PWR_LIM2_EN [15]
    ,      1,       // reserved [16]
    PL2T, 7,        // PKG_PWR_LIM_2_TIME [23:17]
    Offset (0xF3C), // CONFIG_TDP_NOMINAL (MCHBAR+0x5F3C)
    TARN,  8,       // TDP Ratio [7:0]
    Offset (0xF40), // CONFIG_TDP_LEVEL1 (MCHBAR+0x5F40)
    PTD1, 15,       // Package TDP [14:0]
    ,      1,       // reserved [15]
    TAR1,  8,       // TDP Ratio [23:16]
    ,      8,       // reserved [31:24]
    PMX1, 15,       // Package MAX Power [46:32]
    ,      1,       // reserved [47]
    PMN1, 15,       // Package MIN Power [62:48]
    Offset (0xF48), // CONFIG_TDP_LEVEL2 (MCHBAR+0x5F48)
    PTD2, 15,       // Package TDP [14:0]
    ,      1,       // reserved [15]
    TAR2,  8,       // TDP Ratio [23:16]
    ,      8,       // reserved [31:24]
    PMX2, 15,       // Package MAX Power [46:32]
    ,      1,       // reserved [47]
    PMN2, 15,       // Package MIN Power [62:48]
    Offset (0xF50), // CONFIG_TDP_CONTROL (MCHBAR+0x5F50)
    CTCL,  2,       // TDP Level [1:0]
    ,     29,       // reserved [30:2]
    CLCK,  1,       // Config TDP Lock [31]
    Offset (0xF54), // TURBO_ACTIVATION_RATIO (MCHBAR+0x5F54)
    TAR_,  8,       // Max Non Turbo Ratio [7:0]
  }

  Name(TRAT,0)     // holder for the target ratio
  Name(PRAT,0)     // holder for the ratio in _PSS table
  Name(TMPI,0)     // index

  // CTCU (Config Tdp Control Up)
  //
  // Program the CTDP Up point.
  //
  // Arguments: (0)
  //   None
  // Return Value:
  //   None
  //
  Method(CTCU)
  {
    Store(PTD2,PPL1)       // Set PL1
    Store(1,PL1E)          // Set PL1 enable
    Store(1,PL2E)          // Set PL2 enable
    SPPC(1)                // Set _PPC
    Subtract(TAR2,1,TAR_)  // Set TAR
    Store(2,CTCL)          // Set CTC
  }

  // CTCN (Config Tdp Control Nominal)
  //
  // Program the CTDP Nominal point.
  //
  // Arguments: (0)
  //   None
  // Return Value:
  //   None
  //
  Method(CTCN)
  {
    If(LEqual(CTCL,1))       // algorithm for going to Nominal from Down
    {
      Store(PTDP,PPL1)       // Set PL1
      Store(1,PL1E)          // Set PL1 enable
      Store(1,PL2E)          // Set PL2 enable
      NPPC(TARN)             // Set _PPC
      Subtract(TARN,1,TAR_)  // Set TAR
      Store(0,CTCL)          // Set CTC
    }
    ElseIf(LEqual(CTCL,2))   // algorithm for going to Nominal from Up
    {
      Store(0,CTCL)          // Set CTC
      Subtract(TARN,1,TAR_)  // Set TAR
      NPPC(TARN)             // Set _PPC
      Store(1,PL2E)          // Set PL2 enable
      Store(PTDP,PPL1)       // Set PL1
      Store(1,PL1E)          // Set PL1 enable
    }
  }

  // CTCD (Config Tdp Control Down)
  //
  // Program the CTDP Down point.
  //
  // Arguments: (0)
  //   None
  // Return Value:
  //   None
  //
  Method(CTCD)
  {
    Store(1,CTCL)          // Set CTC
    Subtract(TAR1,1,TAR_)  // Set TAR
    NPPC(TAR1)             // Set _PPC
    Store(1,PL2E)          // Set PL2 enable
    Store(PTD1,PPL1)       // Set PL1
    Store(1,PL1E)          // Set PL1 enable
  }

  // NPPC (Notify _PPC object)
  //
  // Find the ratio or next highest ratio in the _PSS table and program _PPC with the index of that ratio.
  //
  // Arguments: (1)
  //   Arg0 - Turbo Activation Ratio
  // Return Value:
  //   None
  //
  Method(NPPC,1)
  {
    Store(Arg0,TRAT) // init target ratio from caller
    //
    // \_SB.OSCP[10] = Platform-Wide OS Capable for no limit 16 P-states
    //
    If(And(\_SB.OSCP, 0x0400)) {
      Store(SizeOf(\_SB.PR00.TPSS),TMPI) // init index from _PSS
    } Else {
      Store(SizeOf(\_SB.PR00.LPSS),TMPI) // init index from _PSS
    }
    While(LNotEqual(TMPI,0)) {
      Decrement(TMPI) // convert from 1 based count to 0 based count
      //
      // \_SB.OSCP[10] = Platform-Wide OS Capable for no limit 16 P-states
      //
      If(And(\_SB.OSCP, 0x0400)) {
        Store(DeRefOf(Index(DeRefOf(Index(\_SB.PR00.TPSS,TMPI)),4)),PRAT)
      } Else {
        Store(DeRefOf(Index(DeRefOf(Index(\_SB.PR00.LPSS,TMPI)),4)),PRAT)
      }
      ShiftRight(PRAT,8,PRAT)
      If(LGreaterEqual(PRAT,TRAT)){
        SPPC(TMPI)
        Break
      }
    }
  }

  // SPPC (Set Participant Performance Capability)
  //
  // Progam the _PPC object and notify the OSPM.
  //
  // Arguments: (1)
  //   Arg0 - integer
  // Return Value:
  //   None
  //
  Method(SPPC,1,Serialized)
  {
    Store(Arg0, \_SB.CPPC) // Note: CPPC is an Integer not a Method

    Store(ToInteger(TCNT), Local0)
    Notify(\_SB.PR00, 0x80)  // Tell PR00 driver to re-eval _PPC
    If(LGreater(Local0,  1))  {Notify(\_SB.PR01, 0x80)}  // Tell PR01 driver to re-eval _PPC
    If(LGreater(Local0,  2))  {Notify(\_SB.PR02, 0x80)}  // Tell PR02 driver to re-eval _PPC
    If(LGreater(Local0,  3))  {Notify(\_SB.PR03, 0x80)}  // Tell PR03 driver to re-eval _PPC
    If(LGreater(Local0,  4))  {Notify(\_SB.PR04, 0x80)}  // Tell PR04 driver to re-eval _PPC
    If(LGreater(Local0,  5))  {Notify(\_SB.PR05, 0x80)}  // Tell PR05 driver to re-eval _PPC
    If(LGreater(Local0,  6))  {Notify(\_SB.PR06, 0x80)}  // Tell PR06 driver to re-eval _PPC
    If(LGreater(Local0,  7))  {Notify(\_SB.PR07, 0x80)}  // Tell PR07 driver to re-eval _PPC
    If(LGreater(Local0,  8))  {Notify(\_SB.PR08, 0x80)}  // Tell PR08 driver to re-eval _PPC
    If(LGreater(Local0,  9))  {Notify(\_SB.PR09, 0x80)}  // Tell PR09 driver to re-eval _PPC
    If(LGreater(Local0,  10)) {Notify(\_SB.PR10, 0x80)}  // Tell PR10 driver to re-eval _PPC
    If(LGreater(Local0,  11)) {Notify(\_SB.PR11, 0x80)}  // Tell PR11 driver to re-eval _PPC
    If(LGreater(Local0,  12)) {Notify(\_SB.PR12, 0x80)}  // Tell PR12 driver to re-eval _PPC
    If(LGreater(Local0,  13)) {Notify(\_SB.PR13, 0x80)}  // Tell PR13 driver to re-eval _PPC
    If(LGreater(Local0,  14)) {Notify(\_SB.PR14, 0x80)}  // Tell PR14 driver to re-eval _PPC
    If(LGreater(Local0,  15)) {Notify(\_SB.PR15, 0x80)}  // Tell PR15 driver to re-eval _PPC
    If(LGreater(Local0,  16)) {Notify(\_SB.PR16, 0x80)}  // Tell PR16 driver to re-eval _PPC
    If(LGreater(Local0,  17)) {Notify(\_SB.PR17, 0x80)}  // Tell PR17 driver to re-eval _PPC
    If(LGreater(Local0,  18)) {Notify(\_SB.PR18, 0x80)}  // Tell PR18 driver to re-eval _PPC
    If(LGreater(Local0,  19)) {Notify(\_SB.PR19, 0x80)}  // Tell PR19 driver to re-eval _PPC
    If(LGreater(Local0,  20)) {Notify(\_SB.PR20, 0x80)}  // Tell PR20 driver to re-eval _PPC
    If(LGreater(Local0,  21)) {Notify(\_SB.PR21, 0x80)}  // Tell PR21 driver to re-eval _PPC
    If(LGreater(Local0,  22)) {Notify(\_SB.PR22, 0x80)}  // Tell PR22 driver to re-eval _PPC
    If(LGreater(Local0,  23)) {Notify(\_SB.PR23, 0x80)}  // Tell PR23 driver to re-eval _PPC
  }

} // end of scope(\_SB.PC00)
} // end of definition block
