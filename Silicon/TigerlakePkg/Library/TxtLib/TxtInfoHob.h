/** @file
  This code provides a initialization of Intel TXT.

  Copyright (c) 2025, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent
**/
#ifndef _TXT_HOB_H_
#define _TXT_HOB_H_


#pragma pack(push, 1)
///
/// TXT Info data.
/// The data that will be passed to TXT lib via TXT_INFO_HOB for txt initialization in Stage 2.
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
  ///
  /// Flags passed from BIOS to OS or MRC.
  /// - BIT0: FLAGS0 for compatible definition.
  /// - BIT1: TXT_CPU_RESET_REQUIRED for MRC to issue reset if required.
  /// - BIT2: TPM_INIT_FAILED for indicate TPM initiate status. If the bit set, ResetEstablishmentBit is skipped in Dxe driver.
  ///
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
