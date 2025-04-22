/** @file
  Intel ACPI Reference Code for Intel(R) Dynamic Tuning Technology

  Copyright (c) 2022, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent
**/

External (\_SB.PC00.LPCB.H_EC.UVTH, FieldUnitObj)

Scope(\_SB.PC00.TCPU) // SA Thermal Device
{
    // PFLG
    //   0 - Default, participant is on the main board
    //   1 - Participant device is on the docking station
    //   2 - Participant device is on the detachable base
    //   3 - Participant device is an external device (such as a USB device, Intel(R) Dynamic Tuning Technology does not support this type of device today)
    Name(PFLG, 0)

    // _STA (Status)
    //
    // This object returns the current status of a device.
    //
    // Arguments: (0)
    //   None
    // Return Value:
    //   An Integer containing a device status bitmap:
    //    Bit 0 - Set if the device is present.
    //    Bit 1 - Set if the device is enabled and decoding its resources.
    //    Bit 2 - Set if the device should be shown in the UI.
    //    Bit 3 - Set if the device is functioning properly (cleared if device failed its diagnostics).
    //    Bit 4 - Set if the battery is present.
    //    Bits 5-31 - Reserved (must be cleared).
    //
    Method(_STA)
    {
      If (LEqual(\SADE,1)){
        Return(0x0F)
      } Else {
        Return(0x00)
      }
    }

    //
    // Define an OpRegion for the Intel(R) Dynamic Tuning Technology MSR's accessed via MCHBAR+0x5000
    //
    OperationRegion (CPWR, SystemMemory, Add(ShiftLeft(\_SB.PC00.MC.MHBR,15),0x5000), 0x1000)
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
        Offset(0x978),
        PCTP,  8,       // Package Temperature.(MCHBAR+ 0x5978)
        Offset (0x998), // RP_STATE_CAP_0_0_0_MCHBAR_PCU (MCHBAR+0x5998)
        RP0C,  8,       // [7:0] RP0_CAP,  These fields indicate the maximum RPx base frequency capability for the Integrated GFX Engine (GT).
        RP1C,  8,       // [15:8] RP1_CAP,  Values are in units of 100 MHz.
        RPNC,  8,       // [23:16] RPN_CAP
        Offset (0xF3C), // CONFIG_TDP_NOMINAL (MCHBAR+0x5F3C)
        TRAT,  8,       // TDP Ratio [7:0]
        Offset (0xF40), // CONFIG_TDP_LEVEL1 (MCHBAR+0x5F40)
        PTD1, 15,       // Package TDP [14:0]
        ,      1,       // reserved [15]
        TRA1,  8,       // TDP Ratio [23:16]
        ,      8,       // reserved [31:24]
        PMX1, 15,       // Package MAX Power [46:32]
        ,      1,       // reserved [47]
        PMN1, 15,       // Package MIN Power [62:48]
        Offset (0xF48), // CONFIG_TDP_LEVEL2 (MCHBAR+0x5F48)
        PTD2, 15,       // Package TDP [14:0]
        ,      1,       // reserved [15]
        TRA2,  8,       // TDP Ratio [23:16]
        ,      8,       // reserved [31:24]
        PMX2, 15,       // Package MAX Power [46:32]
        ,      1,       // reserved [47]
        PMN2, 15,       // Package MIN Power [62:48]
        Offset (0xF50), // CONFIG_TDP_CONTROL (MCHBAR+0x5F50)
        CTCL,  2,       // TDP Level [1:0]
        ,     29,       // reserved [30:2]
        CLCK,  1,       // Config TDP Lock [31]
        Offset (0xF54), // TURBO_ACTIVATION_RATIO (MCHBAR+0x5F54)
        MNTR,  8,       // Max Non Turbo Ratio [7:0]
      }

    Name(XPCC,0) // semaphore to record when PPCC gets called for the first time

    // PPCC (Participant Power Control Capabilities)
    //
    // The PPCC object evaluates to a package of packages that indicates to Intel(R) Dynamic Tuning Technology processor
    // participant the power control capabilities.
    //
    // Arguments: (0)
    //   None
    // Return Value:
    //   PPCC package of packages
    //
    Method(PPCC,0,Serialized,,PkgObj)
    {
      If(LAnd(LEqual(XPCC,0),CondRefOf(\_SB.CBMI))){
        Switch(ToInteger(\_SB.CBMI)){ // use the boot index from PPM to choose the PL for PPCC
          case(0){
            If(LAnd(LGreaterEqual(\_SB.CLVL,1),LLessEqual(\_SB.CLVL,3))){
              CPL0() // copy PL0 values to PPCC
              Store(1,XPCC)
            }
          }
          case(1){
            If(LOr(LEqual(\_SB.CLVL,2),LEqual(\_SB.CLVL,3))){
              CPL1() // copy PL1 values to PPCC
              Store(1,XPCC)
            }
          }
          case(2){
            If(LEqual(\_SB.CLVL,3)){
              CPL2() // copy PL2 values to PPCC
              Store(1,XPCC)
            }
          }
        }
      }
      Return(NPCC)
    }

    // PPCC (Participant Power Control Capabilities)
    //
    // The PPCC object evaluates to a package of packages that indicates to Intel(R) Dynamic Tuning Technology processor
    // participant the power control capabilities.
    //
    // Arguments: (0)
    //   None
    // Return Value:
    //   PPCC package of packages
    //
    // PPCC will be initialized by the _INI method with power on default values from the PPM code.
    //
    Name (NPCC, Package()
    {                          // Field Name : Field Type
      2,                       // Revision : DWordConst

      Package ()               // Power Limit 1
      {
        0,                     // PowerLimitIndex : DWordConst = 0
        35000,                 // PowerLimitMinimum : DWordConst
        45000,                 // PowerLimitMaximum : DWordConst
        28000,                 // TimeWindowMinimum : DWordConst
        32000,                 // TimeWindowMaximum : DWordConst
        1000                   // StepSize : DWordConst
      },
      Package ()               // Power Limit 2
      {
        1,                     // PowerLimitIndex : DWordConst = 1
        56250,                 // PowerLimitMinimum : DWordConst
        56250,                 // PowerLimitMaximum : DWordConst
        0,                     // TimeWindowMinimum : DWordConst
        0,                     // TimeWindowMaximum : DWordConst
        1000                   // StepSize : DWordConst
      }
    }) // End of PPCC object

    // CPNU (Convert Power Number from MMIO register to correct Units)
    //
    // Arguments: (1)
    //   Arg0 = Number to be converted
    //   Arg1 = Units desired
    //     0 = Watts
    //     1 = MilliWatts
    // Return Value:
    //   Converted integer
    //
    Method(CPNU,2,Serialized,,IntObj)
    {
      Name(CNVT,0) // converted number
      Name(PPUU,0) // units
      Name(RMDR,0) // remainder

      if (LEqual(PWRU,0)) {  // use PACKAGE_POWER_SKU_UNIT - Power Units[3:0]
        Store(1,PPUU)
      } else {
        ShiftLeft(Decrement(PWRU),2,PPUU) // get units
      }
      Divide(Arg0,PPUU,RMDR,CNVT) // convert Arg0 to Watts

      if(LEqual(Arg1,0)){
        Return(CNVT)              // return in watts
      } else {
        Multiply(CNVT,1000,CNVT)  // convert to milliwatts
        Multiply(RMDR,1000,RMDR)  // convert remainder to a useful integer
        Divide(RMDR,PPUU,,RMDR)   // convert remainder to watts
        Add(CNVT,RMDR,CNVT)       // add the integer part and the fraction part together
        Return(CNVT)              // return in milliwatts
      }
    }

    // CPL0 (Copy PL0 power limits to PPCC)
    //
    // Arguments:
    //   None
    // Return Value:
    //   None
    //
    Method(CPL0,0)
    {
      Store (2,Index(\_SB.PC00.TCPU.NPCC,0))                                         // Revision
      Store (0,Index(DerefOf(Index(\_SB.PC00.TCPU.NPCC,1)),0))                       // PowerLimitIndex
      Store (125,Index(DerefOf(Index(\_SB.PC00.TCPU.NPCC,1)),1))                     // PowerLimitMinimum
      Store (CPNU(\_SB.PL10,1),Index(DerefOf(Index(\_SB.PC00.TCPU.NPCC,1)),2))       // PowerLimitMaximum
      Multiply (\_SB.PLW0,1000,Index(DerefOf(Index(\_SB.PC00.TCPU.NPCC,1)),3))       // TimeWindowMinimum
      Add (Multiply(\_SB.PLW0,1000),4000,Index(DerefOf(Index(\_SB.PC00.TCPU.NPCC,1)),4))// TimeWindowMaximum
      Store (PPSZ,Index(DerefOf(Index(\_SB.PC00.TCPU.NPCC,1)),5))                    // StepSize
      Store (1,Index(DerefOf(Index(\_SB.PC00.TCPU.NPCC,2)),0))                       // PowerLimitIndex
      Store (CPNU(\_SB.PL20,1),Index(DerefOf(Index(\_SB.PC00.TCPU.NPCC,2)),1))       // PowerLimitMinimum
      Store (CPNU(\_SB.PL20,1),Index(DerefOf(Index(\_SB.PC00.TCPU.NPCC,2)),2))       // PowerLimitMaximum
      Store (0,Index(DerefOf(Index(\_SB.PC00.TCPU.NPCC,2)),3))                       // TimeWindowMinimum
      Store (0,Index(DerefOf(Index(\_SB.PC00.TCPU.NPCC,2)),4))                       // TimeWindowMaximum
      Store (PPSZ,Index(DerefOf(Index(\_SB.PC00.TCPU.NPCC,2)),5))                    // StepSize
    }

    // CPL1 (Copy PL1 power limits to PPCC)
    //
    // Arguments:
    //   None
    // Return Value:
    //   None
    //
    Method(CPL1,0)
    {
      Store (2,Index(\_SB.PC00.TCPU.NPCC,0))                                         // Revision
      Store (0,Index(DerefOf(Index(\_SB.PC00.TCPU.NPCC,1)),0))                       // PowerLimitIndex
      Store (125,Index(DerefOf(Index(\_SB.PC00.TCPU.NPCC,1)),1))                     // PowerLimitMinimum
      Store (CPNU(\_SB.PL11,1),Index(DerefOf(Index(\_SB.PC00.TCPU.NPCC,1)),2))       // PowerLimitMaximum
      Multiply (\_SB.PLW1,1000,Index(DerefOf(Index(\_SB.PC00.TCPU.NPCC,1)),3))       // TimeWindowMinimum
      Add (Multiply(\_SB.PLW1,1000),4000,Index(DerefOf(Index(\_SB.PC00.TCPU.NPCC,1)),4))// TimeWindowMaximum
      Store (PPSZ,Index(DerefOf(Index(\_SB.PC00.TCPU.NPCC,1)),5))                    // StepSize
      Store (1,Index(DerefOf(Index(\_SB.PC00.TCPU.NPCC,2)),0))                       // PowerLimitIndex
      Store (CPNU(\_SB.PL21,1),Index(DerefOf(Index(\_SB.PC00.TCPU.NPCC,2)),1))       // PowerLimitMinimum
      Store (CPNU(\_SB.PL21,1),Index(DerefOf(Index(\_SB.PC00.TCPU.NPCC,2)),2))       // PowerLimitMaximum
      Store (0,Index(DerefOf(Index(\_SB.PC00.TCPU.NPCC,2)),3))                       // TimeWindowMinimum
      Store (0,Index(DerefOf(Index(\_SB.PC00.TCPU.NPCC,2)),4))                       // TimeWindowMaximum
      Store (PPSZ,Index(DerefOf(Index(\_SB.PC00.TCPU.NPCC,2)),5))                    // StepSize
    }

    // CPL2 (Copy PL2 power limits to PPCC)
    //
    // Arguments:
    //   None
    // Return Value:
    //   None
    //
    Method(CPL2,0)
    {
      Store (2,Index(\_SB.PC00.TCPU.NPCC,0))                                         // Revision
      Store (0,Index(DerefOf(Index(\_SB.PC00.TCPU.NPCC,1)),0))                       // PowerLimitIndex
      Store (125,Index(DerefOf(Index(\_SB.PC00.TCPU.NPCC,1)),1))                     // PowerLimitMinimum
      Store (CPNU(\_SB.PL12,1),Index(DerefOf(Index(\_SB.PC00.TCPU.NPCC,1)),2))       // PowerLimitMaximum
      Multiply (\_SB.PLW2,1000,Index(DerefOf(Index(\_SB.PC00.TCPU.NPCC,1)),3))       // TimeWindowMinimum
      Add (Multiply(\_SB.PLW2,1000),4000,Index(DerefOf(Index(\_SB.PC00.TCPU.NPCC,1)),4))// TimeWindowMaximum
      Store (PPSZ,Index(DerefOf(Index(\_SB.PC00.TCPU.NPCC,1)),5))                    // StepSize
      Store (1,Index(DerefOf(Index(\_SB.PC00.TCPU.NPCC,2)),0))                       // PowerLimitIndex
      Store (CPNU(\_SB.PL22,1),Index(DerefOf(Index(\_SB.PC00.TCPU.NPCC,2)),1))       // PowerLimitMinimum
      Store (CPNU(\_SB.PL22,1),Index(DerefOf(Index(\_SB.PC00.TCPU.NPCC,2)),2))       // PowerLimitMaximum
      Store (0,Index(DerefOf(Index(\_SB.PC00.TCPU.NPCC,2)),3))                       // TimeWindowMinimum
      Store (0,Index(DerefOf(Index(\_SB.PC00.TCPU.NPCC,2)),4))                       // TimeWindowMaximum
      Store (PPSZ,Index(DerefOf(Index(\_SB.PC00.TCPU.NPCC,2)),5))                    // StepSize
    }

    Name (LSTM,0)            // Last temperature reported

    // _PPC (Performance Present Capabilities)
    //
    // Arguments: (0)
    //   None
    // Return Value:
    //   An Integer containing the range of states supported
    //   0 - States 0 through nth state are available (all states available)
    //   1 - States 1 through nth state are available
    //   2 - States 2 through nth state are available
    //   ...
    //   n - State n is available only
    //
    Name(_PPC,0)

    // SPPC (Set Participant Performance Capability)
    //
    // SPPC is a control method object that takes one integer parameter that will indicate the maximum allowable
    // P-State for OSPM to use at any given time.
    //
    // Arguments: (1)
    //   Arg0 - integer
    // Return Value:
    //   None
    //
    Method(SPPC,1,Serialized)
    {
      If (CondRefOf(\_SB.CPPC)){
        Store(Arg0, \_SB.CPPC) // Note: \_SB.CPPC must be an Integer not a Method
      }

      If (LGreater(ToInteger(\TCNT),0)) {
        Notify(\_SB.PR00, 0x80)  // Tell PR00 driver to re-eval _PPC
      }
      If (LGreater(ToInteger(\TCNT),1)) {
        Notify(\_SB.PR01, 0x80)  // Tell PR01 driver to re-eval _PPC
      }
      If (LGreater(ToInteger(\TCNT),2)) {
        Notify(\_SB.PR02, 0x80)  // Tell PR02 driver to re-eval _PPC
      }
      If (LGreater(ToInteger(\TCNT),3)) {
        Notify(\_SB.PR03, 0x80)  // Tell PR03 driver to re-eval _PPC
      }
      If (LGreater(ToInteger(\TCNT),4)) {
        Notify(\_SB.PR04, 0x80)  // Tell PR04 driver to re-eval _PPC
      }
      If (LGreater(ToInteger(\TCNT),5)) {
        Notify(\_SB.PR05, 0x80)  // Tell PR05 driver to re-eval _PPC
      }
      If (LGreater(ToInteger(\TCNT),6)) {
        Notify(\_SB.PR06, 0x80)  // Tell PR06 driver to re-eval _PPC
      }
      If (LGreater(ToInteger(\TCNT),7)) {
        Notify(\_SB.PR07, 0x80)  // Tell PR07 driver to re-eval _PPC
      }
      If (LGreater(ToInteger(\TCNT),8)) {
        Notify(\_SB.PR08, 0x80)  // Tell PR08 driver to re-eval _PPC
      }
      If (LGreater(ToInteger(\TCNT),9)) {
        Notify(\_SB.PR09, 0x80)  // Tell PR09 driver to re-eval _PPC
      }
      If (LGreater(ToInteger(\TCNT),10)) {
        Notify(\_SB.PR10, 0x80)  // Tell PR10 driver to re-eval _PPC
      }
      If (LGreater(ToInteger(\TCNT),11)) {
        Notify(\_SB.PR11, 0x80)  // Tell PR11 driver to re-eval _PPC
      }
      If (LGreater(ToInteger(\TCNT),12)) {
        Notify(\_SB.PR12, 0x80)  // Tell PR12 driver to re-eval _PPC
      }
      If (LGreater(ToInteger(\TCNT),13)) {
        Notify(\_SB.PR13, 0x80)  // Tell PR13 driver to re-eval _PPC
      }
      If (LGreater(ToInteger(\TCNT),14)) {
        Notify(\_SB.PR14, 0x80)  // Tell PR14 driver to re-eval _PPC
      }
      If (LGreater(ToInteger(\TCNT),15)) {
        Notify(\_SB.PR15, 0x80)  // Tell PR15 driver to re-eval _PPC
      }
      If (LGreater(ToInteger(\TCNT),16)) {
        Notify(\_SB.PR16, 0x80)  // Tell PR16 driver to re-eval _PPC
      }
      If (LGreater(ToInteger(\TCNT),17)) {
        Notify(\_SB.PR17, 0x80)  // Tell PR17 driver to re-eval _PPC
      }
      If (LGreater(ToInteger(\TCNT),18)) {
        Notify(\_SB.PR18, 0x80)  // Tell PR18 driver to re-eval _PPC
      }
      If (LGreater(ToInteger(\TCNT),19)) {
        Notify(\_SB.PR19, 0x80)  // Tell PR19 driver to re-eval _PPC
      }
      If (LGreater(ToInteger(\TCNT),20)) {
        Notify(\_SB.PR20, 0x80)  // Tell PR20 driver to re-eval _PPC
      }
      If (LGreater(ToInteger(\TCNT),21)) {
        Notify(\_SB.PR21, 0x80)  // Tell PR21 driver to re-eval _PPC
      }
      If (LGreater(ToInteger(\TCNT),22)) {
        Notify(\_SB.PR22, 0x80)  // Tell PR22 driver to re-eval _PPC
      }
      If (LGreater(ToInteger(\TCNT),23)) {
        Notify(\_SB.PR23, 0x80)  // Tell PR23 driver to re-eval _PPC
      }
      If (LGreater(ToInteger(\TCNT),24)) {
        Notify(\_SB.PR24, 0x80)  // Tell PR24 driver to re-eval _PPC
      }
      If (LGreater(ToInteger(\TCNT),25)) {
        Notify(\_SB.PR25, 0x80)  // Tell PR25 driver to re-eval _PPC
      }
      If (LGreater(ToInteger(\TCNT),26)) {
        Notify(\_SB.PR26, 0x80)  // Tell PR26 driver to re-eval _PPC
      }
      If (LGreater(ToInteger(\TCNT),27)) {
        Notify(\_SB.PR27, 0x80)  // Tell PR27 driver to re-eval _PPC
      }
      If (LGreater(ToInteger(\TCNT),28)) {
        Notify(\_SB.PR28, 0x80)  // Tell PR28 driver to re-eval _PPC
      }
      If (LGreater(ToInteger(\TCNT),29)) {
        Notify(\_SB.PR29, 0x80)  // Tell PR29 driver to re-eval _PPC
      }
      If (LGreater(ToInteger(\TCNT),30)) {
        Notify(\_SB.PR30, 0x80)  // Tell PR30 driver to re-eval _PPC
      }
      If (LGreater(ToInteger(\TCNT),31)) {
        Notify(\_SB.PR31, 0x80)  // Tell PR31 driver to re-eval _PPC
      }
    }

    // SPUR (Set _PUR)
    //
    // This object takes an integer as argument that indicates the number of Logical processors that
    // must be idled by the OSPM. This object's implementation must modify the NumProcessors field
    // of the _PUR object implemented in the processor aggregator object definition and also must
    // notify the processor aggregator object with code 0x80 to allow the OSPM to re-evaluate _PUR.
    //
    // Arguments: (1)
    //   number of Logical processors that that must be idled by the OSPM
    // Return Value:
    //   None
    //
    Method(SPUR,1,,,IntObj)
    {
      If(LLessEqual(Arg0,\TCNT)){ // bounds check the argument
        If(LEqual(\_SB.PAGD._STA, 0x0F)){ // check if _PUR is enabled
          Store(Arg0, Index(\_SB.PAGD._PUR, 1))
          Notify (\_SB.PAGD, 0x80)
        }
      }
    }

    // PCCC (Participant Current Control Capabilities)
    //
    // The PCCC object evaluates to a package of packages that indicates to Intel(R) Dynamic Tuning Technology processor participant the Icc control capabilities.
    //
    // Arguments: (0)
    //   None
    //
    // Return Value:
    //   A package of packages as described below:
    //
    //    Package()
    //    {
    //      0x80000000,       // DWordConst, Revision
    //      Package ()
    //      {
    //        0x80000000,     // DWordConst, CurrentLimitMinimum in milli Amps
    //        0x80000000,     // DWordConst, CurrentLimitMaximum in milli Amps
    //      }
    //    }
    Method(PCCC,0,Serialized,,PkgObj)
    {
      Store (1,Index(PCCX,0))  // Revision
      Switch(ToInteger(CPNU(PTDP,0))){    // SKU check
        case(57){
          Store (43000,Index(DerefOf(Index(PCCX,1)),0))       // CurrentLimitMinimum
          Store (95000,Index(DerefOf(Index(PCCX,1)),1))       // CurrentLimitMaximum
        }
        case(47){
          Store (39000,Index(DerefOf(Index(PCCX,1)),0))       // CurrentLimitMinimum
          Store (85000,Index(DerefOf(Index(PCCX,1)),1))       // CurrentLimitMaximum
        }
        case(37){
          Store (29000,Index(DerefOf(Index(PCCX,1)),0))       // CurrentLimitMinimum
          Store (55000,Index(DerefOf(Index(PCCX,1)),1))       // CurrentLimitMaximum
        }
        case(25){
          Store (16000,Index(DerefOf(Index(PCCX,1)),0))       // CurrentLimitMinimum
          Store (32000,Index(DerefOf(Index(PCCX,1)),1))       // CurrentLimitMaximum
        }
        case(15){
          Store (14000,Index(DerefOf(Index(PCCX,1)),0))       // CurrentLimitMinimum
          Store (32000,Index(DerefOf(Index(PCCX,1)),1))       // CurrentLimitMaximum
        }
        case(11){
          Store (14000,Index(DerefOf(Index(PCCX,1)),0))       // CurrentLimitMinimum
          Store (25000,Index(DerefOf(Index(PCCX,1)),1))       // CurrentLimitMaximum
        }
        Default{ // UNKNOWN SKU
          Store (0xFF,Index(DerefOf(Index(PCCX,1)),0))       // CurrentLimitMinimum
          Store (0xFF,Index(DerefOf(Index(PCCX,1)),1))       // CurrentLimitMaximum
        }
      } // End of Switch(PTDP)

      Return(PCCX)
    } // End of PCCC object

    // PCCX (Participant Current Control Capabilities temp structure)
    //
    // This is used to pass data from the PCCC object to the PDRT object.
    //
    Name (PCCX, Package()
    {
      0x80000000,       // DWordConst, Revision

      Package ()
      {
        0x80000000,     // DWordConst, CurrentLimitMinimum
        0x80000000      // DWordConst, CurrentLimitMaximum
      }
    }) // End of PCCC object

    // KEFF (VR efficiency Table)
    //
    // This object evaluates to a package of packages that indicates the VR efficiency factor for various processor power.
    //
    // Arguments: (0)
    //   None
    //
    // Return Value:
    //   A package of packages.
    //
    Name(KEFF, Package()
    {
      // Processor PMAX, Efficiency
      Package () {444, 0},
      Package () {463, 39},
      Package () {481, 75},
      Package () {499, 108},
      Package () {518, 139},
      Package () {536, 168},
      Package () {554, 195},
      Package () {573, 221},
      Package () {591, 244},
      Package () {609, 267},
      Package () {628, 287},
      Package () {812, 445},
      Package () {983, 551},
      Package () {1163, 621},
      Package () {1342, 673},
      Package () {1527, 710},
      Package () {1704, 742},
      Package () {1885, 767},
      Package () {2072, 785},
      Package () {2255, 802},
      Package () {6044, 897},
      Package () {11740, 924},
      Package () {17576, 926},
      Package () {23605, 919},
      Package () {29821, 909},
      Package () {36223, 898},
      Package () {42856, 886},
      Package () {49723, 873},
      Package () {56870, 858},
      Package () {64380, 842}
    })

    Name (CEUP, Package()
    {
      0x80000000,
      0x80000000,
      0x80000000,
      0x80000000,
      0x80000000,
      0x80000000
    })

    // _TMP (Temperature)
    //
    // This control method returns the thermal zone's current operating temperature.
    //
    // Arguments: (0)
    //   None
    // Return Value:
    //   An Integer containing the current temperature of the thermal zone (in tenths of degrees Kelvin)
    //
    Method(_TMP,0,Serialized)
    {
      Return((\_SB.IETM.CTOK(PCTP)))
    }

    // _DTI (Device Temperature Indication)
    //
    // Conveys the temperature of a device's internal temperature sensor to the platform when a temperature trip point
    // is crossed or when a meaningful temperature change occurs.
    //
    // Arguments: (1)
    //   Arg0 - An Integer containing the current value of the temperature sensor (in tenths Kelvin)
    // Return Value:
    //   None
    //
    Method(_DTI, 1)
    {
      Store(Arg0,LSTM)
      Notify(\_SB.PC00.TCPU, 0x91) // notify the participant of a trip point change event
    }

    // _NTT (Notification Temperature Threshold)
    //
    // Returns the temperature change threshold for devices containing native temperature sensors to cause
    // evaluation of the _DTI object
    //
    // Arguments: (0)
    //   None
    // Return Value:
    //   An Integer containing the temperature threshold in tenths of degrees Kelvin.
    //
    Method(_NTT, 0)
    {
      Return(2782)  // 5 degree Celcius, this could be a platform policy with setup item
    }

    Name(PTYP, 0x00)

    // _PSS (Performance Supported States)
    //
    // This optional object indicates to OSPM the number of supported processor performance states that any given system can support.
    //
    // Arguments: (1)
    //   None
    // Return Value:
    //   A variable-length Package containing a list of Pstate sub-packages as described below
    //
    // Return Value Information
    //   Package {
    //   PState [0] // Package - Performance state 0
    //   ....
    //   PState [n] // Package - Performance state n
    //   }
    //
    //   Each Pstate sub-Package contains the elements described below:
    //   Package {
    //     CoreFrequency     // Integer (DWORD)
    //     Power             // Integer (DWORD)
    //     Latency           // Integer (DWORD)
    //     BusMasterLatency  // Integer (DWORD)
    //     Control           // Integer (DWORD)
    //     Status            // Integer (DWORD)
    //   }
    //
    // Stub for the Actual CPU _PSS method.
    //
    Method(_PSS,,,,PkgObj)
    {
      If(CondRefOf(\_SB.PR00._PSS))
      { // Ensure _PSS is present
        Return(\_SB.PR00._PSS())
      } Else {
        Return(Package()
          {
            Package(){0,0,0,0,0,0},
            Package(){0,0,0,0,0,0}
          }
        )
      }
    }

    // _TSS (Throttling Supported States)
    //
    // Arguments: (0)
    //   None
    // Return Value:
    //   A variable-length Package containing a list of Tstate sub-packages as described below
    //
    // Return Value Information
    //   Package {
    //   TState [0] // Package - Throttling state 0
    //   ....
    //   TState [n] // Package - Throttling state n
    //   }
    //
    //   Each Tstate sub-Package contains the elements described below:
    //   Package {
    //     Percent // Integer (DWORD)
    //     Power   // Integer (DWORD)
    //     Latency // Integer (DWORD)
    //     Control // Integer (DWORD)
    //     Status  // Integer (DWORD)
    //   }
    //
    Method(_TSS,)
    {
      If(CondRefOf(\_SB.PR00._TSS))
      { // Ensure _TSS is present
        Return(\_SB.PR00._TSS())
      } Else {
        Return(Package()
          {
            Package(){1,0,0,0,0}
          }
        )
      }
    }

    // _TPC (Throttling Present Capabilities)
    //
    // This optional object is a method that dynamically indicates to OSPM the number of throttling states currently supported by the platform.
    //
    // Arguments: (0)
    //   None
    // Return Value:
    //   An Integer containing the number of states supported:
    //   0 - states 0 .. nth state available (all states available)
    //   1 - state 1 .. nth state available
    //   2 - state 2 .. nth state available
    //   ...
    //   n - state n available only
    //
    Method(_TPC)
    {
      If(CondRefOf(\_SB.PR00._TPC))
      { // Ensure _TPC is present
        Return(\_SB.PR00._TPC)
      } Else {
        Return(0)
      }
    }

    // _PTC (Processor Throttling Control)
    //
    // _PTC is an optional object that defines a processor throttling control interface alternative to the I/O address spaced-based P_BLK throttling control register (P_CNT)
    //
    //  PF00[2] = ACPI object indicating if OSPM is capable of direct access to On Demand throttling MSR
    //
    // Arguments: (0)
    //   None
    // Return Value:
    //   A Package as described below
    //
    // Return Value Information
    //   Package {
    //     ControlRegister // Buffer (Resource Descriptor)
    //     StatusRegister // Buffer (Resource Descriptor)
    //   }
    //
    Method(_PTC,,,,PkgObj)
    {
      If(LAnd(CondRefOf(\PF00),LNotEqual(\PF00,0x80000000))) // is object present and initialized?
      {
        If(And(\PF00, 0x0004)) { // does OS support MSR interface?
          Return(Package() {
          ResourceTemplate(){Register(FFixedHW, 0, 0, 0)},
          ResourceTemplate(){Register(FFixedHW, 0, 0, 0)} }) // if OS supports MSR interface
        } Else {
          Return(Package() {
          ResourceTemplate(){Register(SystemIO, 5, 0, PCH_ACPI_PBLK)},
          ResourceTemplate(){Register(SystemIO, 5, 0, PCH_ACPI_PBLK)} }) // if OS support IO based interface
        }
      } Else {
        Return(Package() {
        ResourceTemplate(){Register(FFixedHW, 0, 0, 0)},
        ResourceTemplate(){Register(FFixedHW, 0, 0, 0)} }) // if object is not present or not initialized then return MSR interface
      }
    }

    // _TSD (T-State Dependency)
    //
    // This optional object provides T-state control cross logical processor dependency information to OSPM.
    //
    // Arguments: (0)
    //   None
    // Return Value:
    //   A variable-length Package containing a list of T-state dependency Packages as described below.
    //
    // Return Value Information
    //   Package {
    //     NumEntries    // Integer
    //     Revision      // Integer (BYTE)
    //     Domain        // Integer (DWORD)
    //     CoordType     // Integer (DWORD)
    //     NumProcessors // Integer (DWORD)
    //   }
    //
    Method(_TSD,)
    {
      If(CondRefOf(\_SB.PR00._TSD))
      { // Ensure _TSD is present
        Return(\_SB.PR00._TSD())
      } Else {
        Return(Package()
          {
            Package(){5,0,0,0xFC,0}
          }
        )
      }
    }

    // _TDL (T-state Depth Limit)
    //
    // This optional object evaluates to the _TSS entry number of the lowest power throttling state that OSPM may use.
    //
    // Arguments: (0)
    //   None
    // Return Value:
    //   An Integer containing the Throttling Depth Limit _TSS entry number:
    //   0 - throttling disabled.
    //   1 - state 1 is the lowest power T-state available.
    //   2 - state 2 is the lowest power T-state available.
    //   ...
    //   n - state n is the lowest power T-state available.
    //
    Method(_TDL)
    {
      If(LAnd(CondRefOf(\_SB.PR00._TSS),CondRefOf(\_SB.CFGD)))
      { // Ensure _TSS is present
        If(And(\_SB.CFGD, PPM_TSTATE_FINE_GRAINED))
        {
          Return(Subtract(SizeOf(\_SB.PR00.TSMF),1))
        } Else {
          Return(Subtract(SizeOf(\_SB.PR00.TSMC),1))
        }
      } Else {
        Return(0)
      }
    }

    // _PDL (P-state Depth Limit)
    //
    // This optional object evaluates to the _PSS entry number of the lowest performance P-state
    // that OSPM may use when performing passive thermal control.
    //
    // Arguments: (0)
    //   None
    // Return Value:
    //   An Integer containing the P-state Depth Limit _PSS entry number:
    //   Integer containing the P-state Depth Limit _PSS entry number:
    //   0 - P0 is the only P-state available for OSPM use
    //   1 - state 1 is the lowest power P-state available
    //   2 - state 2 is the lowest power P-state available
    //   ...
    //   n - state n is the lowest power P-state available
    //
    Method(_PDL)
    {
      If(CondRefOf(\_SB.PR00._PSS))
      { // Ensure _PSS is present
        //
        // \_SB.OSCP[10] = Platform-Wide OS Capable for no limit 16 P-states
        //
        If(And(\_SB.OSCP, 0x0400))
        {
          Return(Subtract(SizeOf(\_SB.PR00.TPSS),1))
        } Else {
          Return(Subtract(SizeOf(\_SB.PR00.LPSS),1))
        }
      } Else {
        Return(0)
      }
    }

    // Default values for Tjmax.
    Name (TJMX,110)  // Tjmax to calculate ACx trip point

    // _TSP (Thermal Sampling Period)
    //
    // Sets the polling interval in 10ths of seconds. A value of 0 tells the driver to use interrupts.
    //
    // Arguments: (0)
    //   None
    // Return Value:
    //   An Integer containing the polling rate in tenths of seconds.
    //   A value of 0 will specify using interrupts through the ACPI notifications.
    //
    //   The granularity of the sampling period is 0.1 seconds. For example, if the sampling period is 30.0
    //   seconds, then _TSP needs to report 300; if the sampling period is 0.5 seconds, then it will report 5.
    //
    Method(_TSP,0,Serialized)
    {
      // OxMs can change this to a non-zero value if the polling is desired
      Return(0)
    }

    // _ACx (Active Cooling)
    //
    // This ACPI method is optional.
    // OEM/ODM can directly set the value on Intel(R) Dynamic Tuning Technology UI for the participant
    // The value programmed here is the default value used on Intel RVP for the validation purpose.
    // OEM/ODM can set a different default value for a specific platform if necessary
    // x is a value between 0 and 9 that designates multiple active cooling levels of the thermal zone.
    //
    //  Arguments: (0)
    //    None
    //  Return Value:
    //    An Integer containing the active cooling temperature threshold in tenths of degrees Kelvin
    //
    Method(_AC0,0,Serialized)
    {
      Store(\_SB.IETM.CTOK(TJMX),Local1) // Tjmax
      Subtract(Local1, 10, Local1)       // AC0 = Tjmax-10

      If(LGreaterEqual(LSTM,Local1))
      {
        Return(Subtract(Local1,20)) // subtract 2 degrees which is the Hysteresis
      }
      Else
      {
        Return(Local1)
      }
    }

    // _ACx (Active Cooling)
    //
    // This optional object, if present under a thermal zone, returns the
    //  temperature trip point at which OSPM must start or stop Active cooling,
    //  where x is a value between 0 and 9 that designates multiple active cooling levels of the thermal zone.
    //
    //  Arguments: (0)
    //    None
    //  Return Value:
    //    An Integer containing the active cooling temperature threshold in tenths of degrees Kelvin
    //
    Method(_AC1,0,Serialized)
    {
      Store(\_SB.IETM.CTOK(TJMX),Local1) // Tjmax
      Subtract(Local1, 30, Local1)       // AC1 = AC0-20 or  Tjmax-30

      If(LGreaterEqual(LSTM,Local1))
      {
        Return(Subtract(Local1,20)) // subtract 2 degrees which is the Hysteresis
      }
      Else
      {
        Return(Local1)
      }
    }

    Method(_AC2,0,Serialized)
    {
      Store(\_SB.IETM.CTOK(TJMX),Local1) // Tjmax
      Subtract(Local1, 40, Local1)       // AC2 = AC0-30 or  Tjmax-40

      If(LGreaterEqual(LSTM,Local1))
      {
        Return(Subtract(Local1,20)) // subtract 2 degrees which is the Hysteresis
      }
      Else
      {
        Return(Local1)
      }
    }

    Method(_AC3,0,Serialized)
    {
      Store(\_SB.IETM.CTOK(TJMX),Local1) // Tjmax
      Subtract(Local1, 55, Local1)       // AC3 = AC0-45 or  Tjmax-55

      If(LGreaterEqual(LSTM,Local1))
      {
        Return(Subtract(Local1,20)) // subtract 2 degrees which is the Hysteresis
      }
      Else
      {
        Return(Local1)
      }
    }

    Method(_AC4,0,Serialized)
    {
      Store(\_SB.IETM.CTOK(TJMX),Local1) // Tjmax
      Subtract(Local1, 70, Local1)       // AC4 = AC0-60 or  Tjmax-70

      If(LGreaterEqual(LSTM,Local1))
      {
        Return(Subtract(Local1,20)) // subtract 2 degrees which is the Hysteresis
      }
      Else
      {
        Return(Local1)
      }
    }

    // _PSV (Passive)
    //
    // This ACPI method is optional.
    // OEM/ODM can directly set the value on Intel(R) Dynamic Tuning Technology UI for the participant
    // The value programmed here is the default value used on Intel RVP for the validation purpose.
    // OEM/ODM can set a different default value for a specific platform if necessary
    //
    //  Arguments: (0)
    //    None
    //  Return Value:
    //    An Integer containing the passive cooling temperature threshold in tenths of degrees Kelvin
    //
    Method(_PSV,0,Serialized)
    {
      Return(\_SB.IETM.CTOK(TJMX)) // passive Cooling Policy
    }

    // _CRT (Critical Temperature)
    //
    // This ACPI method is optional.
    // OEM/ODM can directly set the value on Intel(R) Dynamic Tuning Technology UI for the participant
    // The value programmed here is the default value used on Intel RVP for the validation purpose.
    // OEM/ODM can set a different default value for a specific platform if necessary
    //
    //  Arguments: (0)
    //    None
    //  Return Value:
    //    An Integer containing the critical temperature threshold in tenths of degrees Kelvin
    //
    Method(_CRT,0,Serialized)
    {
      Return(\_SB.IETM.CTOK(TJMX))
    }

    // _CR3 (Critical Temperature for S3/CS)
    //
    // This ACPI method is optional.
    // OEM/ODM can directly set the value on Intel(R) Dynamic Tuning Technology UI for the participant
    // The value programmed here is the default value used on Intel RVP for the validation purpose.
    // OEM/ODM can set a different default value for a specific platform if necessary
    //
    //  Arguments: (0)
    //    None
    //  Return Value:
    //    An Integer containing the critical temperature threshold in tenths of degrees Kelvin
    //
    Method(_CR3,0,Serialized)
    {
      Return(\_SB.IETM.CTOK(TJMX))
    }

    // _HOT (Hot Temperature)
    //
    // This ACPI method is optional.
    // OEM/ODM can directly set the value on Intel(R) Dynamic Tuning Technology UI for the participant
    // The value programmed here is the default value used on Intel RVP for the validation purpose.
    // OEM/ODM can set a different default value for a specific platform if necessary
    //
    //  Arguments: (0)
    //    None
    //  Return Value:
    //    The return value is an integer that represents the critical sleep threshold tenths of degrees Kelvin.
    //
    Method(_HOT,0,Serialized)
    {
      Return(\_SB.IETM.CTOK(TJMX))
    }

    // UVTH (Under Voltage Threshold.)
    // UVTH is a command which BIOS sends to EC. [15:0]
    //
    // Arguments: (1)
    //   Arg0 should be sent by Intel(R) Dynamic Tuning Technology driver in response to power delivery capability changes. The unit is 1mV.
    // Return Value:
    //   None
    //
    Method(UVTH,1,Serialized)
    {
      If (LAnd(LEqual(\ECON,1), LEqual(\_SB.PC00.LPCB.H_EC.ECAV,1))) // Check If EC opregion is available
      {
        \_SB.PC00.LPCB.H_EC.ECWT (Arg0, RefOf(\_SB.PC00.LPCB.H_EC.UVTH)) // power delivery capability changed
        \_SB.PC00.LPCB.H_EC.ECMD (0x17)
      }
    }

}// End Scope(\_SB.PC00.TCPU)
