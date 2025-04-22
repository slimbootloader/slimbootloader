/** @file

  Copyright (c) 2021, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent
**/

#include <PiPei.h>
#include <Library/DebugLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/IoLib.h>
#include <Library/SmbiosInitLib.h>
#include <IndustryStandard/SmBios.h>
#include <CpuRegs.h>
#include <Register/PmcRegs.h>
#include <CpuDataStruct.h>
#include <PchReservedResources.h>
#include "Dts.h"

typedef enum {
  TemperatureProbeSiteOther = 0x1,
  TemperatureProbeSiteUnknown,
  TemperatureProbeSiteProcessor,
  TemperatureProbeSiteDisk,
  TemperatureProbeSitePeripheralBay,
  TemperatureProbeSiteSystemManagementModule,
  TemperatureProbeSiteMotherboard,
  TemperatureProbeSiteMemoryModule,
  TemperatureProbeSiteProcessorModule,
  TemperatureProbeSitePowerUnit = 0x10,
  TemperatureProbeSiteAddinCard,
  TemperatureProbeSiteFrontPanelBoard,
  TemperatureProbeSiteBackPanelBoard,
  TemperatureProbeSitePowerSystemBoard,
  TemperatureProbeSiteDriveBackPlane
} TEMPERATURE_PROBE_SITE;

typedef enum {
  TemperatureProbeStatusOther = 0x1,
  TemperatureProbeStatusUnknown,
  TemperatureProbeStatusOk,
  TemperatureProbeStatusNoncritical,
  TemperatureProbeStatusCritical,
  TemperatureProbeStatusNonrecoverable
} TEMPERATURE_PROBE_STATUS;

typedef enum {
  DtsPch = 0x0,
  DtsCpu
} INDEX_DTS;

STATIC CHAR8 *mDtsSite[] = {
  "Tjunction of PCH at Boot",
  "Tjunction of CPU at Boot"
};

STATIC SMBIOS_TABLE_TYPE28  mDtsInfo[] = {
  {                                        // Tjunction of PCH
    {                                       // Hdr
      SMBIOS_TYPE_TEMPERATURE_PROBE,        /// Hdr.Type
      sizeof (SMBIOS_TABLE_TYPE28),         /// Hdr.Size
      0                                     /// Hdr.Handle
    },
    1,                                      // Description: string index 1
    {                                       // LocationAndStatus
      TemperatureProbeSiteOther,            /// Site: Other
      TemperatureProbeStatusUnknown         /// Status: init as unknown, updated in runtime
    },
    1000,                                   // Maximum value: 100.0 deg C. Unit in 1/10 deg C
    0xFE70,                                 // Maximum value: -40.0 deg C. Unit in 1/10 deg C
    1000,                                   // Resolution: 1.000 deg C. Unit in 1/1000 deg C
    100,                                    // Tolerance: 10.0 deg C. Unit in 1/10 degC
    0x8000,                                 // Accuracy: unknow
    0,                                      // OEMDefined
    0x8000                                  // NominalValue: init as unknown, updated in runtime
  },
  {                                        // Tjunction of CPU
    {                                       // Hdr
      SMBIOS_TYPE_TEMPERATURE_PROBE,        /// Hdr.Type
      sizeof (SMBIOS_TABLE_TYPE28),         /// Hdr.Size
      0                                     /// Hdr.Handle
    },
    1,                                      // Description: string index 1
    {                                       // LocationAndStatus
      TemperatureProbeSiteProcessor,        /// Site: Processor
      TemperatureProbeStatusUnknown         /// Status: init as unknown, updated in runtime
    },
    1000,                                   // Maximum value: 100.0 deg C. Unit in 1/10 deg C
    0xFE70,                                 // Maximum value: -40.0 deg C. Unit in 1/10 deg C
    1000,                                   // Resolution: 1.000 deg C. Unit in 1/1000 deg C
    100,                                    // Tolerance: 10.0 deg C. Unit in 1/10 degC
    0x8000,                                 // Accuracy: unknow
    0,                                      // OEMDefined
    0x8000                                  // NominalValue: init as unknown, updated in runtime
  }
};

/**
  Read Pch DTS

**/
VOID
EFIAPI
ReadPchDts (
  VOID
  )
{
  UINT16          Dts;

  // According to EDS, the register is in 2s complement, where
  //   0x1: 1 deg C, 0x0: 0 deg C, 0x1FF: -1 deg C, and 0x1D8: -40 deg C
  Dts = MmioRead16 (PCH_PWRM_BASE_ADDRESS + R_PMC_PWRM_THERMAL_TSS0) & B_PMC_PWRM_THERMAL_TSS0_TSR_MASK;

  // As silicon limitation, no out-of-range check (Tj beyond -40 or 100)
  if (Dts <= 0xFF) {
    mDtsInfo[DtsPch].NominalValue = Dts * 10;
  } else {
    // Prepend 0xFE00 because TSR bit fields are 9-bit
    mDtsInfo[DtsPch].NominalValue = (Dts | 0xFE00) * 10;
  }
  mDtsInfo[DtsPch].LocationAndStatus.TemperatureProbeStatus = TemperatureProbeStatusOk;

  DEBUG ((DEBUG_INFO, "Boot-time Pch Dts: 0x%x\n", mDtsInfo[DtsPch].NominalValue));
}

/**
  Read CPU Dts

**/
VOID
EFIAPI
ReadCpuDts (
  VOID
  )
{
  MSR_REGISTER    ThermStatus;
  MSR_REGISTER    TemperatureTarget;
  INT16           Dts;

  ThermStatus.Qword = AsmReadMsr64(MSR_IA32_THERM_STATUS);

  if ((ThermStatus.Dwords.Low & B_IA32_THERM_STATUS_VALID) == 0) {
    return;
  }

  TemperatureTarget.Qword = AsmReadMsr64(MSR_TEMPERATURE_TARGET);

  // As silicon limitation, no out-of-range check (Tj beyond -40 or 100)
  Dts = (INT16)TemperatureTarget.Bytes.ThirdByte - (INT16)ThermStatus.Bytes.ThirdByte;
  mDtsInfo[DtsCpu].NominalValue = (UINT16)(Dts * 10);
  mDtsInfo[DtsCpu].LocationAndStatus.TemperatureProbeStatus = TemperatureProbeStatusOk;

  DEBUG ((DEBUG_INFO, "Boot-time Cpu DTS: 0x%x\n", mDtsInfo[DtsCpu].NominalValue));
}

/**
  Append Boot DTS to Smbios

  @retval EFI_SUCCESS             Initialized necessary information successfully
  @retval EFI_OUT_OF_RESOURCES    Failed to allocate memory
  @retval other                   other errors

**/
EFI_STATUS
EFIAPI
AppendSmbiosBootDts (
  VOID
  )
{
  EFI_STATUS        Status;
  UINT16            Index;

  for (Index = 0; Index < ARRAY_SIZE (mDtsInfo); Index++) {
    Status = AddSmbiosType (&mDtsInfo[Index]);
    Status = AddSmbiosString (SMBIOS_TYPE_TEMPERATURE_PROBE, mDtsSite[Index]);
  }

  return Status;
}
