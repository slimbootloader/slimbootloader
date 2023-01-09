/** @file

  Copyright (c) 2022, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <PiPei.h>
#include "SmbiosTables.h"
#include <IndustryStandard/SmBios.h>


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


//
// Physical Memory Array (Type 16)
//

SMBIOS_TABLE_TYPE16  mPhysicalMemArray  = {
  {                                            // Hdr
    SMBIOS_TYPE_PHYSICAL_MEMORY_ARRAY,         // Hdr.Type
    sizeof (SMBIOS_TABLE_TYPE16),              // Hdr.Length
    0                                          // Hdr.Handle
  },

  MemoryArrayLocationSystemBoard,              // Location;                       // The enumeration value from MEMORY_ARRAY_LOCATION.
  MemoryArrayUseSystemMemory,                  // Use;                            // The enumeration value from MEMORY_ARRAY_USE.
  MemoryErrorCorrectionUnknown,                // MemoryErrorCorrection;          // The enumeration value from MEMORY_ERROR_CORRECTION.
  0x80000000,                                  // MaximumCapacity;
  0xFFFE,                                      // MemoryErrorInformationHandle;
  SMBIOS_STRING_INDEX_1,                       // NumberOfMemoryDevices;
  0x3fffffffffffffffULL                        // ExtendedMaximumCapacity;
 };

//
// Memory Device (Type 17)
//

SMBIOS_TABLE_TYPE17  mMemoryDevice  = {
  {                                             // Hdr
    SMBIOS_TYPE_MEMORY_DEVICE,                  ///< Hdr.Type
    sizeof (SMBIOS_TABLE_TYPE17),               ///< Hdr.Length
    0                                           ///< Hdr.Handle
  },
  0,                                            // MemoryArrayHandle;
  0xFFFE,                                       // MemoryErrorInformationHandle;
  0xFFFF,                                       // TotalWidth;
  0xFFFF,                                       // DataWidth;
  0xFFFF,                                       // Size
  MemoryFormFactorRowOfChips,                   // FormFactor;              // The enumeration value from MEMORY_FORM_FACTOR.
  0xff,                                         // DeviceSet;
  SMBIOS_STRING_INDEX_1 ,                       // DeviceLocator String
  SMBIOS_STRING_INDEX_2,                        // BankLocator String
  MemoryTypeLpddr4,                             // MemoryType;              // The enumeration value from MEMORY_DEVICE_TYPE.
  {                                             // TypeDetail;
    0,                                          // Reserved        :1;
    0,                                          // Other           :1;
    1,                                          // Unknown         :1;
    0,                                          // FastPaged       :1;
    0,                                          // StaticColumn    :1;
    0,                                          // PseudoStatic    :1;
    0,                                          // Rambus          :1;
    0,                                          // Synchronous     :1;
    0,                                          // Cmos            :1;
    0,                                          // Edo             :1;
    0,                                          // WindowDram      :1;
    0,                                          // CacheDram       :1;
    0,                                          // Nonvolatile     :1;
    0,                                          // Registered      :1;
    0,                                          // Unbuffered      :1;
    0,                                          // LrDimm          :1;
  },
  0,                                            // Speed;
  SMBIOS_STRING_INDEX_3,                        // Manufacturer String
  SMBIOS_STRING_INDEX_4,                        // SerialNumber String
  SMBIOS_STRING_INDEX_5,                        // AssetTag String
  SMBIOS_STRING_INDEX_6,                        // PartNumber String
  0,                                            // Attributes;
  0,                                            // ExtendedSize;
  0,                                            // ConfiguredMemoryClockSpeed;
  0,                                            // MinimumVoltage;
  0,                                            // MaximumVoltage;
  0,                                            // ConfiguredVoltage;
  SMBIOS_STRING_INDEX_7,                        // MemoryTechnology;               // The enumeration value from MEMORY_DEVICE_TECHNOLOGY
  {
    sizeof (MEMORY_DEVICE_OPERATING_MODE_CAPABILITY),
    (UINT16)(BIT1)
  },                                            // MemoryOperatingModeCapability;
  0,                                            // FirwareVersion;
  0,                                            // ModuleManufacturerID;
  0,                                            // ModuleProductID;
  0,                                            // MemorySubsystemControllerManufacturerID;
  0,                                            // MemorySubsystemControllerProductID;
  0,                                            // NonVolatileSize;
  0,                                            // VolatileSize;
  0,                                            // CacheSize;
  0                                             // LogicalSize;
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

