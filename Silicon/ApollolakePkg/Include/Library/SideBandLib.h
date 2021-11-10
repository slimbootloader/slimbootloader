/** @file
  Prototype of the SideBandLib library.

  Copyright (c) 2017, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent
**/

#ifndef _SB_LIB_H_
#define _SB_LIB_H_

/**
  General SideBandLib Information

    Note: IOSF-SideBand prior to v2.0 is not supported.

    All future versions will be supported with backward support to v2.0. The latest
    version supported is specified in VERSION_IOSF_SIDEBAND_SUPPORT.

    Note: IOSF-SideBand access via MMIO is not valid prior to SBREG_BAR (P2SB BAR)
          being set. By calling the generic SideBandRead() and SideBandWrite() library
          functions, the appropriate method will be used based on SBREG_BAR availability.

    In the case SBREG_BAR is not set, the SBMI (SideBand Message Interface) functions
    will be used which read/write to SideBand using private PCI CFG registers in P2SB.

**/

#define VERSION_IOSF_SIDEBAND_SUPPORT 2.0  ///< Guaranteed IOSF-SideBand Supported Version

//
// Internal Library Defines
//
#define P2SB_SBI_ADDR           0xD0
#define P2SB_SBI_DATA           0xD4
#define P2SB_SBI_STAT           0xD8
#define P2SB_SBI_RID            0xDA
#define P2SB_SBI_EXTADDR        0xDC

#define B_ADDR_DEST_PORT_MASK  (BIT31 | BIT30 | BIT29 | BIT28 | BIT27 | BIT26 |  \
                                BIT25 | BIT24)
#define B_ADDR_OFFSET_MASK     (BIT15 | BIT14 | BIT13 | BIT12 | BIT11 | BIT10 |  \
                                BIT9 | BIT8 | BIT7 | BIT6 | BIT5 | BIT4 | BIT3 | \
                                BIT2 | BIT1 | BIT0)
#define B_ADDR_RSVD_MASK       (BIT23 | BIT22 | BIT21 | BIT20)
#define B_ADDR_ROOT_MASK       (BIT19 | BIT18 | BIT17 | BIT16)
#define B_STAT_INITIATE_MASK    BIT0
#define B_STAT_INITRDY_MASK     BIT0
#define B_STAT_OPCODE_MASK     (BIT15 | BIT14 | BIT13 | BIT12 | BIT11 | BIT10 |  \
                                BIT9 | BIT8)
#define B_STAT_POSTED_MASK      BIT7
#define B_STAT_RSVD_RESP       (BIT6 | BIT5 | BIT4 | BIT3 | BIT2 | BIT1)
#define V_INITIATE              0x01
#define V_INITRDY               0x01
#define V_OPCODE_CFGRD          0x04
#define V_OPCODE_CFGWR          0x05
#define V_OPCODE_CRRD           0x06
#define V_OPCODE_CRWR           0x07
#define V_OPCODE_IORD           0x02
#define V_OPCODE_IOWR           0x03
#define V_OPCODE_MRD            0x00
#define V_OPCODE_MWR            0x01
#define V_POSTED                0x01
#define V_RID_FBE               0xF000
#define V_RSVD_RESP             0x00
#define V_RID_BAR_DBC           0x05

//
// Main SideBand Interface Functions
//

/**
  Determines whether to call a SideBand read via MMIO or SBMI (MMCFG) based on whether
  SBREG_REG is set up.

  This is the standard Read function that should be used for SideBand transactions.

  For MMIO Access:
    In a single MMIO transaction, the "SBREG_BAR" is used
    to form a system address to access chipset registers.
    Transactions using this method will read using Private Control Register opcode CRRd(06h).
    All accesses using this method must be DWORD aligned DWORDs.

  For SBMI Access: Refer to SideBandSbmiRead32()

  @param[in]  TargetPortId    Target port to read.
  @param[in]  TargetRegister  Target register to read.

  @retval     UINT32          The value read from Target Register.

**/
UINT32
EFIAPI
SideBandRead32 (
  IN UINT8   TargetPortId,
  IN UINT16  TargetRegister
  );

/**
  Determines whether to call a SideBand write via MMIO or SBMI (MMCFG) based on whether
  SBREG_REG is set up.

  This is the standard Write function that should be used for SideBand transactions.

  For MMIO Access:
    In a single MMIO transaction, the "SBREG_BAR" is used
    to form a system address to access chipset registers.
    Transactions using this method will write using Private Control Register opcode CRWr(07h).
    All accesses using this method must be DWORD aligned DWORDs.
  For SBMI Access:  Refer to SideBandSbmiWrite32()

  @param[in]  TargetPortId    Target port to write.
  @param[in]  TargetRegister  Target register to write.
  @param[in]  Value           The value to write.

  @retval     None.

**/
VOID
EFIAPI
SideBandWrite32 (
  IN UINT8   TargetPortId,
  IN UINT16  TargetRegister,
  IN UINT32  Value
  );

/**
  Determines whether to call SideBand operations via MMIO or SBMI (MMCFG) based on whether
  SBREG_REG is set up.

  This is the standard AndThenOr function that should be used for SideBand transactions.

  Reads the value in the Target Register of the Target Port Id, performs a bitwise AND followed
  by a bitwise OR between the read result and the value specified by AndData, and writes the
  32-bit value to the configuration register specified by Target Register.

  @param[in]  TargetPortId    Target port to read/write.
  @param[in]  TargetRegister  Target register to read/write.
  @param[in]  AndData         The value to AND with Target Register data.
  @param[in]  OrData          The value to OR with Target Register data.

  @retval     None.

**/
VOID
EFIAPI
SideBandAndThenOr32 (
  IN UINT8   TargetPortId,
  IN UINT16  TargetRegister,
  IN UINT32  AndData,
  IN UINT32  OrData
  );

//
// Special Case SideBand Interface Functions
//

/**
  This function is reserved for special case sideband access:
  Non-private control register opcode

  In multiple MMIO transactions, this access method uses a mechanism provided in PCI
  Configuration space to send IOSF-SB messages. This method should be used prior to
  full PCI enumeration. Since multiple messages are sent this method is not thread-safe
  but this is not an issue for single-threaded IAFW.

  Transactions using this method will read using the OpCode parameter.

  All accesses using this method must be DWORD aligned DWORDs.

  @param[in]  TargetPortId    Target port to read.
  @param[in]  TargetRegister  Target register to read.
  @param[in]  OpCode          Opcode to use.
  @param[in]  Bar             Target register access BAR.

  @retval     The value read from Target Register.

**/
UINT32
EFIAPI
SideBandReadReserved32 (
  IN UINT8   TargetPortId,
  IN UINT16  TargetRegister,
  IN UINT8   OpCode,
  IN UINT8   Bar
  );

/**
  This function is reserved for special case sideband access:
  Non-private control register opcode

  In multiple MMIO transactions, this access method uses a mechanism provided in PCI
  Configuration space to send IOSF-SB messages. This method should be used prior to
  full PCI enumeration. Since multiple messages are sent this method is not thread-safe
  but this is not an issue for single-threaded IAFW.

  Transactions using this method will write using the OpCode parameter.

  All accesses using this method must be DWORD aligned DWORDs.

  @param[in]  TargetPortId    Target port to write.
  @param[in]  TargetRegister  Target register to write.
  @param[in]  OpCode          Opcode to use.
  @param[in]  Bar             Target register access BAR.
  @param[in]  Value           The value to write.

  @retval     None.

**/
VOID
EFIAPI
SideBandWriteReserved32 (
  IN UINT8   TargetPortId,
  IN UINT16  TargetRegister,
  IN UINT8   OpCode,
  IN UINT8   Bar,
  IN UINT32  Value
  );

/**
  This function is reserved for special case sideband access:
  Non-private control register opcode

  Reads the value in the Target Register of the Target Port Id, performs a bitwise AND followed
  by a bitwise OR between the read result and the value specified by AndData, and writes the
  32-bit value to the configuration register specified by Target Register.

  @param[in]  TargetPortId    Target port to read/write.
  @param[in]  TargetRegister  Target register to read/write.
  @param[in]  ReadOpCode      Opcode to use for read.
  @param[in]  WriteOpCode     Opcode to use for write.
  @param[in]  Bar             Target register access BAR.
  @param[in]  AndData         The value to AND with Target Register data.
  @param[in]  OrData          The value to OR with Target Register data.

  @retval     None.

**/
VOID
EFIAPI
SideBandAndThenOrReserved32 (
  IN UINT8   TargetPortId,
  IN UINT16  TargetRegister,
  IN UINT8   ReadOpCode,
  IN UINT8   WriteOpCode,
  IN UINT8   Bar,
  IN UINT32  AndData,
  IN UINT32  OrData
  );

//
// Helper Functions
//
/**
  Returns if SideBand access using MMIO is valid.

  @param  None.

  @retval BOOLEAN    Returns true if SideBand access via P2SB MMIO is valid else returns false

**/
BOOLEAN
EFIAPI
SideBandMmioValid (
  VOID
  );

/**
  Library utility function.

  Returns the MMIO Address used to access the register over P2SB.

  This function doesn't check if SB MMIO is enabled or not. Caller has to make sure
  SB MMIO is enabled, this is exposed to meet timing constraints of SDQ protocol in less than 10us

  @param[in]  TargetPortId    Target port.
  @param[in]  TargetRegister  Target register

  @retval     UINT32          The MMIO address to access TargetRegister in TargetPortId with P2SB.

**/
UINT32
EFIAPI
GetSideBandMmioAddress (
  IN UINT8   TargetPortId,
  IN UINT16  TargetRegister
  );

#endif // _SB_LIB_H_
