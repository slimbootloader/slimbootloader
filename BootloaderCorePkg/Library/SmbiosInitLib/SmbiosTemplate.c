/** @file

  Copyright (c) 2022, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <PiPei.h>
#include "SmbiosTables.h"


SMBIOS_TABLE_TYPE0    mBiosInfo = {
  {                                         // Hdr
    SMBIOS_TYPE_BIOS_INFORMATION,           /// Hdr.Type
    sizeof (SMBIOS_TABLE_TYPE0),            /// Hdr.Length
    0                                       /// Hdr.Handle
  },
  SMBIOS_STRING_INDEX_1,                    // Vendor
  SMBIOS_STRING_INDEX_2,                    // BiosVersion
  0x0000,                                   // BiosSegment
  SMBIOS_STRING_INDEX_3,                    // BiosReleaseDate
  0,                                        // BiosSize
  {                                         // BiosCharacteristics
    0,                                      ///< Bit 0-1  Reserved1                         :2
    0,                                      ///< Bit 02 - Unknown                           :1
    0,                                      ///< Bit 03 - BiosCharacteristicsNotSupported   :1
    0,                                      ///< Bit 04 - IsaIsSupported                    :1
    0,                                      ///< Bit 05 - McaIsSupported                    :1
    0,                                      ///< Bit 06 - EisaIsSupported                   :1
    1,                                      ///< Bit 07 - PciIsSupported                    :1
    0,                                      ///< Bit 08 - PcmciaIsSupported                 :1
    0,                                      ///< Bit 09 - PlugAndPlayIsSupported            :1
    0,                                      ///< Bit 10 - ApmIsSupported                    :1
    1,                                      ///< Bit 11 - BiosIsUpgradable                  :1
    1,                                      ///< Bit 12 - BiosShadowingAllowed              :1
    0,                                      ///< Bit 13 - VLVesaIsSupported                 :1
    0,                                      ///< Bit 14 - EscdSupportIsAvailable            :1
    1,                                      ///< Bit 15 - BootFromCdIsSupported             :1
    1,                                      ///< Bit 16 - SelectableBootIsSupported         :1
    0,                                      ///< Bit 17 - RomBiosIsSocketed                 :1
    0,                                      ///< Bit 18 - BootFromPcmciaIsSupported         :1
    1,                                      ///< Bit 19 - EDDSpecificationIsSupported       :1
    0,                                      ///< Bit 20 - JapaneseNecFloppyIsSupported      :1
    0,                                      ///< Bit 21 - JapaneseToshibaFloppyIsSupported  :1
    0,                                      ///< Bit 22 - Floppy525_360IsSupported          :1
    0,                                      ///< Bit 23 - Floppy525_12IsSupported           :1
    0,                                      ///< Bit 24 - Floppy35_720IsSupported           :1
    0,                                      ///< Bit 25 - Floppy35_288IsSupported           :1
    1,                                      ///< Bit 26 - PrintScreenIsSupported            :1
    1,                                      ///< Bit 27 - Keyboard8042IsSupported           :1
    1,                                      ///< Bit 28 - SerialIsSupported                 :1
    1,                                      ///< Bit 29 - PrinterIsSupported                :1
    0,                                      ///< Bit 30 - CgaMonoIsSupported                :1
    0,                                      ///< Bit 31 - NecPc98                           :1
    0                                       ///< Bit 32-63 - Reserved for BIOS/System Vendor :32
  },
  {                                         // BiosCharacteristicsExtensionBytes
/**
  MISC_BIOS_CHARACTERISTICS_EXTENSION.BiosReserved
  1, Bit 0 - AcpiIsSupported                    :1
  1, Bit 1 - UsbLegacyIsSupported               :1
  0, Bit 2 - AgpIsSupported                     :1
  0, Bit 3 - I20BootIsSupported                 :1
  1, Bit 4 - Ls120BootIsSupported               :1
  1, Bit 5 - AtapiZipDriveBootIsSupported       :1
  0, Bit 6 - Boot1394IsSupported                :1
  0, Bit 7 - SmartBatteryIsSupported            :1
**/
   (UINT8)(BIT0 | BIT1 | BIT4 | BIT5),
/**
  MISC_BIOS_CHARACTERISTICS_EXTENSION.SystemReserved
  1, Bit 0 - BiosBootSpecIsSupported            :1
  1, Bit 1 - FunctionKeyNetworkBootIsSupported  :1
  1, Bit 2 - TargetContentDistributionEnabled   :1
  1, Bit 3 - UefiSpecificationSupported         :1
  0, Bit 4 - VirtualMachineSupported            :1
  0, Bit 5-7 - ExtensionByte2Reserved           :3
**/
    (UINT8)(BIT0 | BIT1 | BIT2 | BIT3)
  },
  0x0,                                      // System BIOS Major Release
  0x1,                                      // System BIOS Minor Release
  0xFF,                                     // Embedded controller firmware major Release
  0xFF                                      // Embedded controller firmware minor Release
};


//
// Static (possibly build generated) System Manufacturer data.
//
SMBIOS_TABLE_TYPE1    mSystemInfo = {
  {                                             // Hdr
    SMBIOS_TYPE_SYSTEM_INFORMATION,             /// Hdr.Type
    sizeof (SMBIOS_TABLE_TYPE1),                /// Hdr.Size
    0                                           /// Hdr.Handle
  },
  SMBIOS_STRING_INDEX_1,                        // SystemManufactrurer
  SMBIOS_STRING_INDEX_2,                        // SystemProductName
  SMBIOS_STRING_INDEX_3,                        // SystemVersion
  SMBIOS_STRING_INDEX_4,                        // SystemSerialNumber
  {                                             // SystemUuid
    0x00000000, 0x0000, 0x0000, { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }
  },
  SystemWakeupTypeOther,                        // SystemWakeupType
  SMBIOS_STRING_INDEX_5,                        // SystemSKUNumber
  SMBIOS_STRING_INDEX_6                         // SystemFamily
};


//
// Static (possibly build generated) Base Board Manufacturer data.
//
SMBIOS_TABLE_TYPE2    mBaseBoardInfo = {
  {                                             // Hdr
    SMBIOS_TYPE_BASEBOARD_INFORMATION,          ///< Hdr.Type
    sizeof (SMBIOS_TABLE_TYPE2),                ///< Hdr.Length
    0                                           ///< Hdr.Handle
  },
  SMBIOS_STRING_INDEX_1,                        ///< Manufacturer
  SMBIOS_STRING_INDEX_2,                        ///< ProductName
  SMBIOS_STRING_INDEX_3,                        ///< Version
  SMBIOS_STRING_INDEX_4,                        ///< SerialNumber
  SMBIOS_STRING_INDEX_5,                        ///< AssetTag
  {                                             // FeatureFlags
    1,                                          ///< Bit0 Motherboard
    0,                                          ///< Bit1 RequiresDaughterCard
    0,                                          ///< Bit2 Removable
    0,                                          ///< Bit3 Replaceable,
    0,                                          ///< Bit4 HotSwappable
    0,                                          ///< Bit5-7 Reserved
  },
  SMBIOS_STRING_INDEX_6,                        // LocationInChassis
  0,                                            // ChassisHandle
  BaseBoardTypeMotherBoard,                     // BoardType
  0,                                            // NumberOfContainedObjectHandles
  { 0 }                                         // ContainedObjectHandles
};

//
// Default Chassis Info for SMBIOS Type 3
//
SMBIOS_TABLE_TYPE3  mDefaultChasisInfo = {
  {                                             // Hdr
    SMBIOS_TYPE_SYSTEM_ENCLOSURE,               ///< Hdr.Type
    sizeof(SMBIOS_TABLE_TYPE3),                 ///< Hdr.Length
    0                                           ///< Hdr.Handle
  },
  SMBIOS_STRING_INDEX_1,                         ///< Manufactrurer
  0,                                            ///< Type
  SMBIOS_STRING_INDEX_2,                         ///< Version
  SMBIOS_STRING_INDEX_3,                         ///< SerialNumber
  SMBIOS_STRING_INDEX_4,                         ///< AssetTag
  ChassisStateSafe,                             ///< BootupState
  ChassisStateSafe,                             ///< PowerSupplyState
  ChassisStateOther,                            ///< ThermalState
  ChassisSecurityStatusOther,                   ///< SecurityStatus
  {                                             // OemDefined[4]
    0,
    0,
    0,
    0
  },
  0,                                            ///< Height
  1,                                            ///< NumberofPowerCords
  0,                                            ///< ContainedElementCount
  0,                                            ///< ContainedElementRecordLength
  {                                             // ContainedElements
    {
      0,                                          ///< ContainedElementType
      0,                                          ///< ContainedElementMinimum
      0                                           ///< ContainedElementMaximum
    }
  }
};

SMBIOS_TABLE_TYPE19  mMemArrayMappedAddr = {
  {                                             // Hdr
    SMBIOS_TYPE_MEMORY_ARRAY_MAPPED_ADDRESS,    ///< Hdr.Type
    sizeof (SMBIOS_TABLE_TYPE19),               ///< Hdr.Length
    0                                           ///< Hdr.Handle
  },
  0xFFFFFFFF,                                   // StartingAddress
  0xFFFFFFFF,                                   // EndingAddress
  SMBIOS_HANDLE_PI_RESERVED,                    // MemoryArrayHandle
  1,                                            // PartitionWidth
  0,                                            // ExtendedStartingAddress
  0                                             // ExtendedEndingAddress
};

