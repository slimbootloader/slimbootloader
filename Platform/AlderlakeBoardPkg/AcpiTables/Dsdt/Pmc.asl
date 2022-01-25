/** @file

  Copyright (c) 2021, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <PchLimits.h>
#include <Register/PmcRegs.h>
#include "Register/PchRegsClk.h"
#include <Register/TcoRegs.h>

Scope(\) {

  Name (TMOV, 10) // Timeout Value, default = 10ms.
  //
  // Timeout Value Adjust Method
  // Arg0: Timeout Customize Value
  //
  Method (TMVA, 1, Serialized) {
    Store (10, TMOV) // Setting default value first to avoid the TMOV have been changed before.
    If (LGreater (Arg0, 10)) {
      Store (Arg0, TMOV) // Set customize Timeout Value if customize TMOV greater than 10ms.
    }
  }
  //
  // IPC Methods
  //
  // Send PMC IPC 1 Read/Write command
  // Input
  //   Arg0: Command           Command to be issued to PMC IPC 1 interface
  //   Arg1: SubCmdId          SUB_CMD_ID for provided Command
  //   Arg2: CmdSize           Total size in byte to be sent via PMC IPC 1 interface
  //   Arg3: WriteBuf0         1st DWORD Value to be issued to PMC IPC 1 interface
  //   Arg4: WriteBuf1         2nd DWORD Value to be issued to PMC IPC 1 interface
  //   Arg5: WriteBuf2         3rd DWORD Value to be issued to PMC IPC 1 interface
  //   Arg6: WriteBuf3         4th DWORD Value to be issued to PMC IPC 1 interface
  // Output
  //   Package of data in below order:
  //     Status                Status of the transaction {0 indicates success, non-zero indicates failure}
  //                           Error Code            Description
  //                           ----------            ---------------------
  //                           1                     Command Size Exceeds
  //                           2                     Device failure
  //                           3                     Timeout Occured
  //                           ----------            ---------------------
  //     ReadBuf               Structure of 4 DWORDs to be filled by PMC IPC 1 interface
  //
  Method(IPCS, 7, Serialized){
    Name(RPKG, Package (5) {
      Zero, // Status
      Zero, // ReadBuf0
      Zero, // ReadBuf1
      Zero, // ReadBuf2
      Zero  // ReadBuf3
    })


    if (LGreater(Arg2, 16))    // (CmdSize > 16)
    {
      Store(1, Index(RPKG, 0)) // Store Errorcode of Command Size Exceeds to Status
      Return(RPKG)
    }

    //
    // Program the Write Buffers with the Data that needs to be written to PMC
    //
    Store(Arg3,IWB0)
    Store(Arg4,IWB1)
    Store(Arg5,IWB2)
    Store(Arg6,IWB3)

    //
    // Program the command register with command and size
    //
    Store(0, Local0)
    Add(Local0, ShiftLeft(Arg0,N_PMC_PWRM_IPC_CMD_COMMAND), Local0)
    Add(Local0, ShiftLeft(Arg1,N_PMC_PWRM_IPC_CMD_CMD_ID), Local0)
    Add(Local0, ShiftLeft(Arg2,N_PMC_PWRM_IPC_CMD_SIZE), Local0)
    Store(Local0,CMDR)

    ///
    /// Read the IPC_STS to get BUSY or Error status
    /// Break on 10ms/Customize timeout or error
    ///
    Store (0, Local0)
    Store (TMOV, Local1) // Store IPC1 Command Timeout Value in Local Variable to Prevent Issue Corruption of Global Variable.
    While (1) {
      If (LOr (LNot (IBSY), LEqual (IERR, 1))){
        Break
      }
      If (Lgreater (Local0, Local1)) { // Default Timeout Value Set to 10ms.
        Store (3, Index (RPKG, 0)) // Store Errorcode of Timeout Occured to Status
        Return (RPKG)
      }
      Sleep (1)
      Increment (Local0)
    }
    If(LEqual(IERR,1)) {
      Store(2, Index(RPKG, 0)) // Store Errorcode of Device Failure to Status
      Return(RPKG)
    }

    //
    // Fill the  ReadBuffer contents with the Data that needs to be read from PMC
    //
    Store(IRB0, Index(RPKG, 1)) // ReadBuf0
    Store(IRB1, Index(RPKG, 2)) // ReadBuf1
    Store(IRB2, Index(RPKG, 3)) // ReadBuf2
    Store(IRB3, Index(RPKG, 4)) // ReadBuf3

    Return(RPKG)
  }

  //
  // Program IGN_UFSX2 (Bit 18) of PMC MMR register LTR_IGN (Offset 0x1B0C)
  // arg0: Value to be programmed
  //
  Method (ULTR, 1, Serialized) {
    Store(Arg0, LTRU)
  }

  Method (TCBS, 0) {
    Return (TCOB)
  }

  //
  // Define PCH ACPIBASE I/O as an ACPI operating region.  The base address
  // can be found in Device 31, Function 2, Offset 40h.
  //
  OperationRegion(PMIO, SystemIo, PMBS, 0x80)
  Field(PMIO, ByteAcc, NoLock, Preserve) {
          ,  8,
      PBSS,  1,       // Power Button Status
    Offset(R_ACPI_IO_GPE_CTRL),            // 0x40, General Purpose Event Control
          ,  17,
      GPEC,  1,       // Software GPE Control
    Offset(R_ACPI_IO_GPE0_STS_127_96),     // 0x6C, General Purpose Event 0 Status [127:96]
          ,  19,
      CPWS,  1,       // CPU WAKE STATUS
    Offset(R_ACPI_IO_GPE0_EN_127_96),      // 0x7C, General Purpose Event 0 Enable [127:96]
          ,  18,
      WAEN,  1,       // WADT EN
      CPWE,  1        // CPU WAKE EN
  }

  //
  // Define PCH TCOBASE I/O
  //
  OperationRegion(TCBA, SystemIo, TCBS, 0x10)
  Field(TCBA, ByteAcc, NoLock, Preserve) {
    Offset(R_TCO_IO_TCO1_STS), // 0x04
        , 9,
    CPSC, 1,        // CPUSCI_STS
  }

  //
  // PWRM register definitions
  //
  OperationRegion(PWMR, SystemMemory, PWRM, 0x1E30)
  Field(PWMR, DWordAcc, NoLock, Preserve) {
    //
    // IPC definitions
    //
    Offset(R_PMC_PWRM_IPC_CMD), // IPC Command (IPC_CMD)
    CMDR, 32,         // Command register. Includes: [7:0] Command, [8] MSI, [15:12] CMD_ID, [23:16] Size
    Offset(R_PMC_PWRM_IPC_STS), //  IPC Status (IPC_STS)
    IBSY, 1,          // [0]     Ready/Busy (BUSY)
    IERR, 1,          // [1]     Error (ERROR)
        , 1,          // [2]     IRQ (IRQ)
        , 1,          // [3]     Reserved
        , 4,          // [7:4]   Command ID (CMD_ID)
        , 8,          // [15:8]  Initiator ID (INIT_ID)
    IERC, 8,          // [23:16] Error Code (ERROR CODE)
        , 8,          // [31:24] Reserved
    Offset(R_PMC_PWRM_IPC_WBUF0), // IPC Write Buffer (IPC_WBUF0)
    IWB0, 32,         // [31:0] Write Buffer (WBUF)
    Offset(R_PMC_PWRM_IPC_WBUF1), // IPC Write Buffer (IPC_WBUF1)
    IWB1, 32,         // [31:0] Write Buffer (WBUF)
    Offset(R_PMC_PWRM_IPC_WBUF2), // IPC Write Buffer (IPC_WBUF2)
    IWB2, 32,         // [31:0] Write Buffer (WBUF)
    Offset(R_PMC_PWRM_IPC_WBUF3), // IPC Write Buffer (IPC_WBUF3)
    IWB3, 32,         // [31:0] Write Buffer (WBUF)
    Offset(R_PMC_PWRM_IPC_RBUF0), // IPC Read Buffer (IPC_RBUF0)
    IRB0, 32,         // [31:0] Read Buffer (RBUF)
    Offset(R_PMC_PWRM_IPC_RBUF1), // IPC Read Buffer (IPC_RBUF1)
    IRB1, 32,         // [31:0] Read Buffer (RBUF)
    Offset(R_PMC_PWRM_IPC_RBUF2), // IPC Read Buffer (IPC_RBUF2)
    IRB2, 32,         // [31:0] Read Buffer (RBUF)
    Offset(R_PMC_PWRM_IPC_RBUF3), // IPC Read Buffer (IPC_RBUF3)
    IRB3, 32,         // [31:0] Read Buffer (RBUF)
    Offset(R_PMC_PWRM_MODPHY_PM_CFG5),
    RAA0, 1,          // BIT0  : PCIE A, Function 0
    RAA1, 1,          // BIT1  : PCIE A, Function 1
    RAA2, 1,          // BIT2  : PCIE A, Function 2
    RAA3, 1,          // BIT3  : PCIE A, Function 3
    RPB0, 1,          // BIT4  : PCIE B, Function 0
    RPB1, 1,          // BIT5  : PCIE B, Function 1
    RPB2, 1,          // BIT6  : PCIE B, Function 2
    RPB3, 1,          // BIT7  : PCIE B, Function 3
    RPC0, 1,          // BIT8  : PCIE C, Function 0
    RPC1, 1,          // BIT9  : PCIE C, Function 1
    RPC2, 1,          // BIT10 : PCIE C, Function 2
    RPC3, 1,          // BIT11 : PCIE C, Function 3
    RSAT, 1,          // BIT12 : SATA
    RGBE, 1,          // BIT13 : GBE
    RXHC, 1,          // BIT14 : XHCI
    RXDC, 1,          // BIT15 : XDCI
    RUFS, 1,          // BIT16 : UFS
    RPD0, 1,          // BIT17 : PCIE D, Function 0
    RPD1, 1,          // BIT18 : PCIE D, Function 1
    RPD2, 1,          // BIT18 : PCIE D, Function 2
    RPD3, 1,          // BIT20 : PCIE D, Function 3
    RPE0, 1,          // BIT21 : PCIE E, Function 0
    RPE1, 1,          // BIT22 : PCIE E, Function 1
    RPE2, 1,          // BIT23 : PCIE E, Function 2
    RPE3, 1,          // BIT24 : PCIE E, Function 3
    Offset(R_PMC_PWRM_MODPHY_PM_CFG6),
    APA0, 1,          // BIT0  : PCIE A, Function 0
    APA1, 1,          // BIT1  : PCIE A, Function 1
    APA2, 1,          // BIT2  : PCIE A, Function 2
    APA3, 1,          // BIT3  : PCIE A, Function 3
    APB0, 1,          // BIT4  : PCIE B, Function 0
    APB1, 1,          // BIT5  : PCIE B, Function 1
    APB2, 1,          // BIT6  : PCIE B, Function 2
    APB3, 1,          // BIT7  : PCIE B, Function 3
    APC0, 1,          // BIT8  : PCIE C, Function 0
    APC1, 1,          // BIT9  : PCIE C, Function 1
    APC2, 1,          // BIT10 : PCIE C, Function 2
    APC3, 1,          // BIT11 : PCIE C, Function 3
    ASAT, 1,          // BIT12 : SATA
    AGBE, 1,          // BIT13 : GBE
    AXHC, 1,          // BIT14 : XHCI
    AXDC, 1,          // BIT15 : XDCI
    AUFS, 1,          // BIT16 : UFS
    APD0, 1,          // BIT17 : PCIE D, Function 0
    APD1, 1,          // BIT18 : PCIE D, Function 1
    APD2, 1,          // BIT18 : PCIE D, Function 2
    APD3, 1,          // BIT20 : PCIE D, Function 3
    APE0, 1,          // BIT21 : PCIE E, Function 0
    APE1, 1,          // BIT22 : PCIE E, Function 1
    APE2, 1,          // BIT23 : PCIE E, Function 2
    APE3, 1,          // BIT24 : PCIE E, Function 3
    Offset(R_PMC_PWRM_WADT_AC),
    //
    // AdvancedFeaturesBegin
    //
    ACWA, 32,         // Wake Alarm Device Timer: AC
    DCWA, 32,         // Wake Alarm Device Timer: DC
    ACET, 32,         // Wake Alarm Device Expired Timer: AC
    DCET, 32,         // Wake Alarm Device Expired Timer: DC
    //
    // AdvancedFeaturesEnd
    //
    Offset(R_PMC_PWRM_PM_CFG4),
        , 31,
    CECE,  1,         // PS_ON# de-assertion during SleepS0 enable
    Offset(R_PMC_PWRM_LTR_IGN),
        , 2,
    ISAT, 1,          // Ignore Sata LTR values
        , 18,
    LTRU,  1,         // Bit 18, IGN_UFSX2
    Offset(R_PMC_PWRM_CPPMVRIC),
        , 22,
    XSQD,  1,         // 24MHz Crystal Shutdown Qualification Disable (XTALSDQDIS)
    Offset(R_PMC_PWRM_CPPMFALIGNCTL_1),
        , 31,
    CPPM,  1,          // BIT31 : 1 = PMC allows stalling of the backbone, 0 = PMC does not allow stalling of the backbone
    Offset(R_PMC_PWRM_ST_PG_FDIS_PMC_1),
    GBED,  1,         // GBE_FDIS_PMC
    Offset(0x1E28),
        , 22,
    SCFD,  1,         // ST_FDIS_PMC (SATA)
  }

  //
  // PS_ON control. Enable/Disable PS_ON# de-assertion during SleepS0.
  // Arg0 - 0: Disable PS_ON# de-assertion, 1: Enable PS_ON# de-assertion
  //
  Method(PSOC, 1, Serialized) {
    If (PSON) {
      If(Arg0) {
        Store(1, CECE)
      } Else {
        Store(0, CECE)
      }
    }
  }

  //
  // PS_ON status.
  //
  // @retval 0 PS_ON de-assertion during SleepS0 disabled
  // @retval 1 PS_ON de-assertion during SleepS0 enabled
  //
  Method(PSOS) {
    If(CECE) {
      Return(1)
    } Else {
      Return(0)
    }
  }

  //
  // CPPM Forced Alignment Enable (CPPMFAEN):
  // 1 = PMC allows stalling of the backbone, enabling the CWB and blocking the DMI transmit arbiter and preventing CPU wakes (via PM_SYNC messages) when PCH idle (FAIDLEWAIT and FACSMEPGQDIS) and CPU is in the memory closed state.  It ceases the stall when the maximum wake time point is met.
  // 0 = PMC does not allow stalling of the backbone or enabling the CWB and blocking the DMI transmit arbiter.
  //
  Method(CFAE, 1, NotSerialized) {
    If(DerefOf(Index(Arg0, 0))) {
      ADBG("CPPM Forced Alignment Disable")
      Store(0, CPPM)
      Return(Buffer() {0})
    } Else {
      ADBG("CPPM Forced Alignment Enable")
      Store(1, CPPM)
      Return(Buffer() {0})
    }
  }

  //
  // Is MODPHY power gating supported
  // Checks if MODPHY power gating is supported on given SKU
  //
  // @retval 0 MODPHY power gating is not supported
  // @retval 1 MODPHY power gating is supported
  //
  Method (IMPS) {
    If (LEqual (PCHS, PCH_S)) {
      Return(0)
    } Else {
      Return(1)
    }
  }
}
