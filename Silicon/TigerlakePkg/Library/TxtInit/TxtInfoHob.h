/** @file
  This file contains definitions required for creation of TXT Info HOB.

@copyright
  INTEL CONFIDENTIAL
  Copyright 1999 - 2017 Intel Corporation.

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
#ifndef _TXT_HOB_H_
#define _TXT_HOB_H_


#pragma pack(push, 1)
///
/// TXT Info data.
/// The data that will be passed to DXE via TXT_INFO_HOB.
///
typedef struct _TXT_INFO_DATA {
  UINT64               SinitMemorySize;         ///< Size of memory reserved for placement of SINIT module. This memory is used by MLE.
  UINT64               TxtHeapMemorySize;       ///< Size of memory reserved for TXT Heap. This memory is used by MLE.
  EFI_PHYSICAL_ADDRESS TxtDprMemoryBase;        ///< Base address of DPR protected memory reserved for Intel TXT component.
  UINT64               TxtDprMemorySize;        ///< Size of DPR protected memory reserved for TXT component.
  EFI_PHYSICAL_ADDRESS BiosAcmBase;             ///< Base address of BIOS ACM in system firmware.
  UINT64               BiosAcmSize;             ///< Size of BIOS ACM.
  EFI_PHYSICAL_ADDRESS SinitAcmBase;            ///< Base address of SINIT module if installed in system firmware. Zero otherwise.
  UINT64               SinitAcmSize;            ///< Size of SINIT module if installed in system firmware. Zero otherwise.
  UINT64               TgaSize;                 ///< Size of Trusted Graphics Aperture if supported by chipset.
  EFI_PHYSICAL_ADDRESS TxtLcpPdBase;            ///< Base address of Platform Default Launch Control Policy data if installed in system firmware. Zero otherwise.
  UINT64               TxtLcpPdSize;            ///< Size of Platform Default Launch Control Policy data if installed in system firmware. Zero otherwise.
  /**
  Flags passed from BIOS to OS or MRC.
  - BIT0: FLAGS0 for compatible definition.
  - BIT1: TXT_CPU_RESET_REQUIRED for MRC to issue reset if required.
  - BIT2: TPM_INIT_FAILED for indicate TPM initiate status. If the bit set, ResetEstablishmentBit is skipped in Dxe driver.
  **/
  UINT64 Flags;
  EFI_PHYSICAL_ADDRESS ApStartupBase;           ///< Base address of AP Startup code

  // Tseg and Mseg values only apply when PcdStmEnable is TRUE.
  EFI_PHYSICAL_ADDRESS TsegBase;            ///< Physical base address of TSEG region.
  UINT64               TsegSize;            ///< Size of the entity at TSEG region.
  EFI_PHYSICAL_ADDRESS MsegBase;            ///< Physical base address of MSEG region.
  UINT64               MsegSize;            ///< Size of the entity at MSEG region.

  UINT32               AcpiBase:16;             ///< Address of PM1a_CNT_BLK register block. It is used by TXT PEIM to clean Sleep Type field of PM1a_CNT_BLK.S4 register before running of SCLEAN.
  UINT32               ChipsetIsTxtCapable:1;   ///< Value is set to 1 if chipset is Intel TXT capable.
  UINT32               TxtMode:1;               ///< Value is set to 1 if Intel TXT mode is enabled in BIOS Setup.
  UINT32               ResetAux:1;              ///< Value is set to 1 if reset Aux is enabled in BIOS Setup.
  UINT32               TxtAcheckRequest:1;      ///< Value is set to 1 if TXT Acheck Request is enabled in BIOS Setup.
  UINT32               ProcessorIsTxtCapable:1; ///< Value is set to 1 if processor is Intel TXT capable.
  UINT32               Reserved:11;             ///< Reserved for future use

  UINT8                AcmMajorVersion;         ///< BIOSACM binary major version
  UINT8                AcmMinorVersion;         ///< BIOSACM binary minor version
  UINT8                AcmRevision;             ///< BIOSACM binary revision

  // Since the biggest element is UINT64, this structure should be aligned with 64 bits.
  UINT8                Rsvd[1];                 ///< Reserved for config block alignment.
} TXT_INFO_DATA;                                ///< TXT info

// Flags passed from BIOS to OS or MRC.
#define FLAGS0                  0x1  ///< BIT0: FLAGS0 for compatible definition.
#define TXT_CPU_RESET_REQUIRED  0x2  ///< BIT1: TXT_CPU_RESET_REQUIRED for MRC to issue reset if required.
#define TPM_INIT_FAILED         0x4  ///< BIT2: TPM_INIT_FAILED for indicate TPM initiate status.
                                     ///< If the bit set, ResetEstablishmentBit is skipped in Dxe driver.

//
// TXT:RestrictedBegin
//
#define TXT_SKIP_SCHECK_REQUIRED  0x100000000  ///< Test menu: If set, BIOS will skip SCHECK
#define TXT_FORCE_TXT_ENABLE      0x200000000  ///< Test menu: If set, BIOS will enable TXT unconditionally
#define TXT_FORCE_DPR_SET         0x400000000  ///< Test menu: If set, BIOS will report DPR memory size anyway
#define TXT_UNLOCK_DPR            0x800000000  ///< Test menu: If set, BIOS will skip locking DMA Protected Range
//
// TXT:RestrictedEnd
//

///
/// HOB to save TXT Info data.
/// TXT Info HOB is used for passing Policy settings and CPU/chipset information within TXT modules.
///
typedef struct _TXT_INFO_HOB {
  EFI_HOB_GUID_TYPE EfiHobGuidType; ///< EFI Hob Guid Type.
  TXT_INFO_DATA     Data;           ///< TXT Info Data.
} TXT_INFO_HOB;                     ///< TXT Info HOB
#pragma pack(pop)

#endif
