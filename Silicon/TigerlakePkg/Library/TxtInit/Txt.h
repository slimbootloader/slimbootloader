/** @file
  This file contains definitions required to use the TXT BIOS
  Authenticated Code Module Library.

@copyright
  INTEL CONFIDENTIAL
  Copyright 1999 - 2020 Intel Corporation.

  The source code contained or described herein and all documents related to the
  source code ("Material") are owned by Intel Corporation or its suppliers or
  licensors. Title to the Material remains with Intel Corporation or its suppliers
  and licensors. The Material may contain trade secrets and proprietary and
  confidential information of Intel Corporation and its suppliers and licensors,
  and is protected by worldwide copyright and trade secret laws and treaty
  provisions. No part of the Material may be used, copied, reproduced, modified,
  published, uploaded, posted, transmitted, distributed, or disclosed in any way
  without Intel's prior express written permission.

  No license under any patent, copyright, trade secret or other intellectual
  property right is granted to or conferred upon you by disclosure or delivery
  of the Materials, either expressly, by implication, inducement, estoppel or
  otherwise. Any license under such intellectual property rights must be
  express and approved by Intel in writing.

  Unless otherwise agreed by Intel in writing, you may not remove or alter
  this notice or any other notice embedded in Materials by Intel or
  Intel's suppliers or licensors in any way.

  This file contains an 'Intel Peripheral Driver' and is uniquely identified as
  "Intel Reference Module" and is licensed for Intel CPUs and chipsets under
  the terms of your license agreement with Intel or your vendor. This file may
  be modified by the user, subject to additional terms of the license agreement.

@par Specification Reference:
**/
#ifndef _TXT_BIOS_ACM_LIB_H_
#define _TXT_BIOS_ACM_LIB_H_

#define TPM_TIME_OUT  750  ///< TPM access polling timeout (~750ms)

//
// TxT BIOS Spec version
//
#define TXT_BIOS_SPEC_VER_MAJOR     2  ///< Major number of TXT BIOS spec version
#define TXT_BIOS_SPEC_VER_MINOR     1  ///< Minor number of TXT BIOS spec version
#define TXT_BIOS_SPEC_VER_REVISION  0  ///< Revision number of TXT BIOS spec version

//
// TXT HEAP extended data type
//
#define HEAP_EXTDATA_TYPE_END           0  ///< Element type identifier: terminator
#define HEAP_EXTDATA_TYPE_BIOS_SPEC_VER 1  ///< Element type identifier: BIOS spec version
#define HEAP_EXTDATA_TYPE_BIOSACM       2  ///< Element type identifier: BIOS ACM version
#define BIOS_OS_DATAREGION_VERSION      6  ///< Element type identifier: BIOS OS data region

//
// The following are values that are placed in the esi register when
// calling the BIOS ACM.  These constants are used as parameters to the
// TxtSetupandLaunchBiosAcm function.
//
#define TXT_LAUNCH_SCLEAN         0x00   ///< BIOS ACM function #: SCLEAN
#define TXT_RESET_EST_BIT         0x01   ///< BIOS ACM function #: Reset establishment bit
#define TXT_RESET_AUX             0x02   ///< BIOS ACM function #: Reset AUX index
#define TXT_LAUNCH_SCHECK         0x04   ///< BIOS ACM function #: SCHECK
#define TXT_LAUNCH_CLEAR_SECRETS  0x05   ///< BIOS ACM function #: CLEAR_SECRETS
#define TXT_LAUNCH_LOCK_CONFIG    0x06   ///< BIOS ACM function #: LOCK_CONFIG
#define TXT_LAUNCH_ACHECK         0x07   ///< BIOS ACM function #: ACHECK


#define TPM_STATUS_REG_ADDRESS    0xFED40000 ///< Address of TPM status register
#define TXT_PUBLIC_BASE           0xFED30000 ///< Base address of TXT public space
#define TXT_PRIVATE_BASE          0xFED20000 ///< Base address of TXT private space
#define TXT_CONFIG_SPACE_LENGTH   0x60000    ///< Size TXT address space

// TXT public address space
#define TXT_ERROR_STATUS_REG_OFF  0x8    ///< Offset of TXT error status register (ESTS)
#define TXT_CRASHCODE_REG_OFF     0x30   ///< Offset of TXT crash code register (CRASH)
#define TXT_SINIT_BASE_REG_OFF    0x270  ///< Offset of SINIT base address register (SINIT_MEMORY_BASE)
#define TXT_SINIT_SIZE_REG_OFF    0x278  ///< Offset of SINIT size register (SINIT_MEMORY_SIZE)
#define TXT_SINIT_SIZE_REG_OFF2   0x27C  ///< Offset of SINIT size register (SINIT_MEMORY_SIZE)
#define TXT_SVMM_JOIN_REG_OFF     0x290  ///< Offset of SVMM join register (SVMM_JOIN)
#define TXT_SVMM_JOIN_REG_OFF2    0x294  ///< Offset of SVMM join register (SVMM_JOIN)
#define TXT_HEAP_BASE_REG_OFF     0x300  ///< Offset of TXT heap base address register (HEAP_BASE)
#define TXT_HEAP_SIZE_REG_OFF     0x308  ///< Offset of TXT heap size register (HEAP_SIZE)
#define TXT_SCRATCHPAD            0x320  ///< Offset of scratchpad register (LT_SCRATCHPAD)
#define TXT_SCRATCHPAD2           0x324  ///< Offset of scratchpad register (LT_SCRATCHPAD2)
#define TXT_SCRATCHPAD3           0x328  ///< Offset of scratchpad register (LT_SCRATCHPAD3)
#define TXT_SCRATCHPAD4           0x32C  ///< Offset of scratchpad register (LT_SCRATCHPAD4)
#define TXT_DPR_SIZE_REG_OFF      0x330  ///< Offset of DMA protected range register (DPR)

#define TXT_PTT_FTIF_OFF          0x800  ///< Offset of Interface Version Number register (VER_FTIF)
#define TXT_PTT_PRESENT           0x70000    ///< Bitfield indicates location of TPM
                                             ///< 111: fTPM is present and active

#define TXT_E2STS_REG_OFF         0x8F0  ///< Offset of Extended Error Status register (E2STS)
#define TXT_BLOCK_MEM_STS         BIT2   ///< 1: MCH should block memory accesses
#define TXT_SECRETS_STS           BIT1   ///< 1: there are any potential secrets in memory
#define TXT_SLP_ENTRY_ERROR_STS   BIT0   ///< 1: there has been an improper attempt to go to sleeping state

#define BIOACM_ADDR               TXT_SINIT_SIZE_REG_OFF2 ///< Offset of register containing BIOS ACM base address
#define APINIT_ADDR               TXT_SVMM_JOIN_REG_OFF   ///< Offset of register containing AP init base address
#define SEMAPHORE                 TXT_SVMM_JOIN_REG_OFF2  ///< Offset of register containing semaphore address
//
// CPU:RestrictedBegin
//
//
// CPU definition
//
#define SMX_FEATUREFLAG         0x00000040  ///< Bit 6 in the ECX feature flags
//
// CPU:RestrictedEnd
//


#pragma pack(push, 1)
///
/// TXT HEAP Memory: Extended data elements
///
/// As defined in Intel(R) TXT Software Development Guide /
///   Measured Launch Environment Developer's Guide
///
typedef struct _HEAP_EXT_DATA_ELEMENT {
  UINT32 Type;                           ///< Type of HEAP extended data element
  UINT32 Size;                           ///< Size of HEAP extended data element
} HEAP_EXT_DATA_ELEMENT;                 ///< TXT HEAP Extended data elements

///
/// TXT HEAP Memory: BIOS spec version element
///
/// As defined in Intel(R) TXT Software Development Guide /
///   Measured Launch Environment Developer's Guide
///
typedef struct _HEAP_BIOS_SPEC_VER_ELEMENT {
  HEAP_EXT_DATA_ELEMENT Header;          ///< Header of HEAP extended data. Header.Type = <b>1</b>.
  UINT16                SpecVerMajor;    ///< Major number of TXT BIOS spec version
  UINT16                SpecVerMinor;    ///< Minor number of TXT BIOS spec version
  UINT16                SpecVerRevision; ///< Revision number of TXT BIOS spec version
} HEAP_BIOS_SPEC_VER_ELEMENT;            ///< TXT HEAP BIOS spec version element

///
/// TXT HEAP Memory: BIOS ACM element
///
/// As defined in Intel(R) TXT Software Development Guide /
///   Measured Launch Environment Developer's Guide
///
typedef struct _HEAP_BIOSACM_ELEMENT {
  HEAP_EXT_DATA_ELEMENT Header;          ///< Header of HEAP extended data. Header.Type = <b>2</b>.
  UINT32                NumAcms;         ///< Number of BIOS ACMs carried by BIOS
} HEAP_BIOSACM_ELEMENT;                  ///< TXT HEAP BIOS ACM element

///
/// TXT HEAP Memory: BIOS OS Data region definitions
///
/// As defined in Intel(R) TXT Software Development Guide /
///   Measured Launch Environment Developer's Guide
///
typedef struct _BIOS_OS_DATA_REGION {
  UINT32                Version;                 ///< Version number of the BIOS to OS data
  /**
  This field indicates the size of the SINIT AC module stored in system firmware.
  A value of 0 indicates the system firmware is not providing a SINIT module for OS use.
  **/
  UINT32                BiosSinitSize;
  /**
  Physical base address of the Platform Default Launch Control Policy structure.
  If the Platform Default Policy does not require additional data this field should
  be set to 0x00 and will be ignored.
  **/
  EFI_PHYSICAL_ADDRESS  LcpPdBase;
  /**
  Size of the Launch Control Policy Platform Default Policy Data.
  If the Platform Default Policy does not require additional data this field should
  be set to 0x00 and will be ignored.
  **/
  UINT64                LcpPdSize;
  UINT32                NumOfLogicalProcessors;  ///< The total number of logical processors in the system. The minimum value in this register must be at least <b>0x01</b>.
  UINT64                Flags;                   ///< BIOS provided info for SINIT consumption
  HEAP_EXT_DATA_ELEMENT ExtData;                 ///< Array/list of extended data element structures
} BIOS_OS_DATA_REGION;                           ///< TXT HEAP BIOS OS Data region definitions

#pragma pack(pop)
#endif
