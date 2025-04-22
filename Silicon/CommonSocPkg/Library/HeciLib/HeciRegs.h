/** @file

  Copyright (c) 2018 - 2020, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef _HECI_REGS_H_
#define _HECI_REGS_H_

//
// HECIMBAR register definition
//
#define H_CB_WW                             0x00
#define H_CSR                               0x04
#define ME_CB_RW                            0x08
#define ME_CSR_HA                           0x0C

#define R_ME_HFS                            0x40
#define R_ME_HIDM                           0xA0

//
// Management Engine Current Operation Mode
//
#define ME_OPERATION_MODE_NORMAL            0x00
#define ME_OPERATION_MODE_DEBUG             0x02
#define ME_OPERATION_MODE_SOFT_TEMP_DISABLE 0x03
#define ME_OPERATION_MODE_SECOVR_JMPR       0x04
#define ME_OPERATION_MODE_SECOVR_HECI_MSG   0x05
#define ME_OPERATION_MODE_SPS               0x0F

#pragma pack(1)

///
/// H_CSR     - Host Control Status Register, offset 04h
/// ME_CSR_HA - ME Control Status Register (Host Access), offset 0Ch
///
typedef union {
  UINT32 Data;
  struct {
    UINT32  IntEnable      : 1;  ///< 0 - Host/ME Interrupt Enable
    UINT32  IntStatus      : 1;  ///< 1 - Host/ME Interrupt Status
    UINT32  IntGenerate    : 1;  ///< 2 - Host/ME Interrupt Generate
    UINT32  Ready          : 1;  ///< 3 - Host/ME Ready
    UINT32  Reset          : 1;  ///< 4 - Host/ME Reset
    UINT32  Reserved       : 3;  ///< 7:5 - Reserved
    UINT32  CBReadPointer  : 8;  ///< 15:8 - Host/ME Circular Buffer Read Pointer
    UINT32  CBWritePointer : 8;  ///< 23:16 - Host/ME Circular Buffer Write Pointer
    UINT32  CBDepth        : 8;  ///< 31:24 - Host/ME Circular Buffer Depth
  } Fields;
} HECI_CONTROL_STATUS_REGISTER;

///
/// HFSTS1, offset 40h
///
typedef union {
  UINT32  ul;
  struct {
    UINT32  CurrentState         : 4;  ///< 0:3 - Current State
    UINT32  SpiProtectionMode    : 1;  ///< 4 - SPI Protection Mode
    UINT32  FptBad               : 1;  ///< 5 - FPT(Flash Partition Table ) Bad
    UINT32  MeOperationState     : 3;  ///< 6:8 - ME Operation State
    UINT32  FwInitComplete       : 1;  ///< 9 - This bit is set when firmware has fully entered a stable state
    UINT32  FtBupLdFlr           : 1;  ///< 10 - This bit is set when firmware is not able to load BRINGUP from the fault tolerant (FT) code
    UINT32  FwUpdateInprogress   : 1;  ///< 11 - This bit is set when data migration is required during ME FW update process
    UINT32  ErrorCode            : 4;  ///< 12:15 - Error Code
    UINT32  MeOperationMode      : 4;  ///< 16:19 - Management Engine Current Operation Mode
    UINT32  MeRstCnt             : 4;  ///< 20:23 - Number of ME Resets
    UINT32  MeBootOptionsPresent : 1;  ///< 24 - If this bit is set, an Boot Options is present
    UINT32  Reserved             : 1;  ///< 25 - Reserved
    UINT32  BistTestState        : 1;  ///< 26 - This bit informs if BIST is starting or BIST is finished
    UINT32  BistResetRequest     : 1;  ///< 27 - This bit informs if BIST Reset Request
    UINT32  CurrentPowerSource   : 2;  ///< 28:29 - Current power source selection mode
    UINT32  D3SupportValid       : 1;  ///< 30 - D3 support
    UINT32  D0i3SupportValid     : 1;  ///< 31 - D0i3 support
  } r;
} HECI_FWS_REGISTER;

///
/// HFSTS6, offset 6Ch
///
typedef union {
  UINT32  ul;
  struct {
    UINT32 ForceBootPolicy    : 1;  ///< 0 - Force Boot Guard ACM Boot Policy
    UINT32 CpuDebugDisabled   : 1;  ///< 1 - CPU Debug Disabled
    UINT32 BspInitDisabled    : 1;  ///< 2 - BSP Initialization Disabled
    UINT32 PBE                : 1;  ///< 3 - Protect BIOS Environment Policy
    UINT32 Reserved           : 2;  ///< 5:4 - Reserved
    UINT32 ENF                : 2;  ///< 7:6 - Error Enforcement Policy
    UINT32 MeasuredBootPolicy : 1;  ///< 8 - Measured Boot Policy, 0 - Disabled; 1 - Enabled
    UINT32 VerifiedBootPolicy : 1;  ///< 9 - Verified Boot Policy
    UINT32 AcmSvn             : 4;  ///< 13:10 - Boot Guard ACM Security Version Number
    UINT32 KvmSvn             : 4;  ///< 17:14 - Key Manifest Security Version Number
    UINT32 BpmSvn             : 4;  ///< 21:18 - Boot Policy Manifest Security Version Number
    UINT32 KmId               : 4;  ///< 25:22 - Key Manifest ID
    UINT32 ExeStatus          : 1;  ///< 26 - BSP Boot Policy Manifest Execution Status
    UINT32 Error              : 1;  ///< 27 - CPU encountered an unexpected error and is asking FW to start the enforcement logic.
    UINT32 BootGuardDisable   : 1;  ///< 28 - Boot Guard Disable
    UINT32 FpfDisable         : 1;  ///< 29 - Field Programmable Fuses (FPF) Disable
    UINT32 FpfSocConfigLock   : 1;  ///< 30 - Field Programmable Fuses (FPF) SOC Configuration Lock
    UINT32 TxtSupport         : 1;  ///< 31 - TXT Support
  } r;
} HECI_FW_STS6_REGISTER;

///
/// HFSTS2, offset 48h
///
typedef union {
  UINT32  ul;
  struct {
    UINT32  NftpLoadFailure  : 1;  ///< 0 - NFTP Load Failure Detection Field
    UINT32  IccProgSts       : 2;  ///< 1:2 - ICC Prog STS
    UINT32  InvokeMEBx       : 1;  ///< 3 - Invoke MEBX
    UINT32  CpuReplacedSts   : 1;  ///< 4 - CPU Replaced STS
    UINT32  Reserved         : 1;  ///< 5 - Reserved
    UINT32  MfsFailure       : 1;  ///< 6 - MFS Failure
    UINT32  WarmRstReqForDF  : 1;  ///< 7 - Warm Reset Required for Dynamic Fusing
    UINT32  CpuReplacedValid : 1;  ///< 8 - CPU Replaced Valid
    UINT32  LowPowerState    : 1;  ///< 9 - Low Power State
    UINT32  MePowerGating    : 1;  ///< 10 - ME Power Gating Indicator
    UINT32  FwUpdIpu         : 1;  ///< 11 - FW Update IPU Needed
    UINT32  FwUpdForcedSB    : 1;  ///< 12 - FW Update Forced Safe Boot
    UINT32  Reserved2        : 1;  ///< 13 - Reserved
    UINT32  IfuFaultTol      : 1;  ///< 14 - IFU Fault Tolerance Test State
    UINT32  HeciListCh       : 1;  ///< 15 - HECI Listner Change
    UINT32  ExtStatCode1     : 8;  ///< 16:23 - EXT Status Code 1
    UINT32  ExtStatCode2     : 4;  ///< 24:27 - EXT Status Code 2
    UINT32  InfPhaseCode     : 4;  ///< 31:28 - Infra. Phase code
  } r;
} HECI_GS_SHDW_REGISTER;

#pragma pack()

///
/// Header for all HECI message message placed in an Intel MEI circular buffer
///
typedef union {
  UINT32  Data;
  struct {
    /**
      This is the logical address of the Intel ME client of the message. This address is assigned
      during ME firmware initialization.
    **/
    UINT32  MeAddress       : 8;
    /**
      This is the logical address of the Host client of the message. This address is assigned
      when the host client registers itself with the Host MEI driver.
    **/
    UINT32  HostAddress     : 8;
    /**
      This is the message length in bytes, not including the MEI_MESSAGE_HEADER. A value of 0
      indicates no additional bytes are part of the message.
    **/
    UINT32  Length          : 9;
    UINT32  Reserved        : 6;
    UINT32  MessageComplete : 1; ///< Indicates the last message of the list
  } Fields;
} HECI_MESSAGE_HEADER;

#endif // _HECI_REGS_H_
