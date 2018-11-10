## @file
# Provides driver and definitions to build bootloader.
#
# Copyright (c) 2016 - 2017, Intel Corporation. All rights reserved.<BR>
# This program and the accompanying materials are licensed and made available under
# the terms and conditions of the BSD License that accompanies this distribution.
# The full text of the license may be found at
# http://opensource.org/licenses/bsd-license.php.
#
# THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
# WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.
#
##

################################################################################
#
# Defines Section - statements that will be processed to create a Makefile.
#
################################################################################
[Defines]
  PLATFORM_NAME                       = BootloaderCorePkg
  PLATFORM_GUID                       = 0E8F73FA-CF73-4AA2-A406-1610CCF49EF1
  PLATFORM_VERSION                    = 0.1
  DSC_SPECIFICATION                   = 0x00010005
  OUTPUT_DIRECTORY                    = Build/$(PLATFORM_NAME)
  SUPPORTED_ARCHITECTURES             = IA32|X64|ARC
  BUILD_TARGETS                       = DEBUG|RELEASE
  SKUID_IDENTIFIER                    = DEFAULT
  FLASH_DEFINITION                    = $(PLATFORM_NAME)/$(PLATFORM_NAME).fdf

  #
  # Set platform specific package/folder names, same as passed from PREBUILD script.
  # PLATFORM_PACKAGE would be the same as PLATFORM_NAME as well as package build folder
  # The CORE_PACKAGES value could be set to '.' if Core packages get moved to Workspace root
  #
  DEFINE PLATFORM_PACKAGE             = $(PLATFORM_NAME)

!include Platform.dsc

################################################################################
#
# SKU Identification section - list of all SKU IDs supported by this Platform.
#
################################################################################
[SkuIds]
  0|DEFAULT              # The entry: 0|DEFAULT is reserved and always required.

################################################################################
#
# Library Class section - list of all Library Classes needed by this Platform.
#
################################################################################
[LibraryClasses]
  PcdLib|BootloaderCommonPkg/Library/PcdLib/PcdLib.inf
  BaseLib|MdePkg/Library/BaseLib/BaseLib.inf
  IoLib|MdePkg/Library/BaseIoLibIntrinsic/BaseIoLibIntrinsic.inf
  PciLib|MdePkg/Library/BasePciLibPciExpress/BasePciLibPciExpress.inf
  PciCf8Lib|MdePkg/Library/BasePciCf8Lib/BasePciCf8Lib.inf
  PciExpressLib|MdePkg/Library/BasePciExpressLib/BasePciExpressLib.inf
  DebugPrintErrorLevelLib|$(PLATFORM_PACKAGE)/Library/DebugPrintErrorLevelLib/DebugPrintErrorLevelLib.inf
  PrintLib|MdePkg/Library/BasePrintLib/BasePrintLib.inf
  BaseMemoryLib|MdePkg/Library/BaseMemoryLibRepStr/BaseMemoryLibRepStr.inf
  SynchronizationLib|MdePkg/Library/BaseSynchronizationLib/BaseSynchronizationLib.inf
  TimeStampLib|BootloaderCommonPkg/Library/TimeStampLib/TimeStampLib.inf
  ExtraBaseLib|BootloaderCommonPkg/Library/ExtraBaseLib/ExtraBaseLib.inf
  ModuleEntryLib|BootloaderCommonPkg/Library/ModuleEntryLib/ModuleEntryLib.inf
  LzmaDecompressLib|BootloaderCommonPkg/Library/LzmaCustomDecompressLib/LzmaCustomDecompressLib.inf
  Lz4DecompressLib|BootloaderCommonPkg/Library/Lz4DecompressLib/Lz4DecompressLib.inf
  DecompressLib|BootloaderCommonPkg/Library/DecompressLib/DecompressLib.inf
  RleCompressLib|BootloaderCommonPkg/Library/RleCompressLib/RleCompressLib.inf
  FspSupportLib|$(PLATFORM_PACKAGE)/Library/FspSupportLib/FspSupportLib.inf
  BootloaderLib|$(PLATFORM_PACKAGE)/Library/BootloaderLib/BootloaderLib.inf
  BootloaderCoreLib|$(PLATFORM_PACKAGE)/Library/BootloaderCoreLib/BootloaderCoreLib.inf
  PciEnumerationLib|$(PLATFORM_PACKAGE)/Library/PciEnumerationLib/PciEnumerationLib.inf
  AcpiInitLib|$(PLATFORM_PACKAGE)/Library/AcpiInitLib/AcpiInitLib.inf
  LitePeCoffLib|BootloaderCommonPkg/Library/LitePeCoffLib/LitePeCoffLib.inf
  HobLib|BootloaderCommonPkg/Library/HobLib/HobLib.inf
  HobBuildLib|$(PLATFORM_PACKAGE)/Library/HobBuildLib/HobBuildLib.inf
  LiteFvLib|BootloaderCommonPkg/Library/LiteFvLib/LiteFvLib.inf
  LoaderPerformanceLib|BootloaderCommonPkg/Library/LoaderPerformanceLib/LoaderPerformanceLib.inf
  MemoryAllocationLib|$(PLATFORM_PACKAGE)/Library/MemoryAllocationLib/MemoryAllocationLib.inf
  MpInitLib|$(PLATFORM_PACKAGE)/Library/MpInitLib/MpInitLib.inf
  LocalApicLib|BootloaderCommonPkg/Library/BaseXApicLib/BaseXApicLib.inf
  SecureBootLib|BootloaderCommonPkg/Library/SecureBootLib/SecureBootLib.inf
  TpmLib|BootloaderCommonPkg/Library/TpmLib/TpmLib.inf
  BootloaderCommonLib|BootloaderCommonPkg/Library/BootloaderCommonLib/BootloaderCommonLib.inf
  ConfigDataLib|BootloaderCommonPkg/Library/ConfigDataLib/ConfigDataLib.inf
  MmcAccessLib|BootloaderCommonPkg/Library/MmcAccessLib/MmcAccessLib.inf
  GraphicsLib|BootloaderCommonPkg/Library/GraphicsLib/GraphicsLib.inf
  Crc32Lib|BootloaderCommonPkg/Library/Crc32Lib/Crc32Lib.inf
  VariableLib|BootloaderCommonPkg/Library/LiteVariableLib/LiteVariableLib.inf
  DebugDataLib|$(PLATFORM_PACKAGE)/Library/DebugDataLib/DebugDataLib.inf
  CpuExceptionLib|$(PLATFORM_PACKAGE)/Library/CpuExceptionLib/CpuExceptionLib.inf
  FileSystemLib|BootloaderCommonPkg/Library/FileSystemLib/FileSystemLib.inf
  FatLib|BootloaderCommonPkg/Library/FatLib/FatLib.inf
  PartitionLib|BootloaderCommonPkg/Library/PartitionLib/PartitionLib.inf
  Ext23Lib|BootloaderCommonPkg/Library/Ext23Lib/Ext23Lib.inf
  ShellLib|BootloaderCommonPkg/Library/ShellLib/ShellLib.inf
  SpiBlockIoLib|BootloaderCommonPkg/Library/SpiBlockIoLib/SpiBlockIoLib.inf
  UfsBlockIoLib|BootloaderCommonPkg/Library/UfsBlockIoLib/UfsBlockIoLib.inf
  MmcAccessLib|BootloaderCommonPkg/Library/MmcAccessLib/MmcAccessLib.inf
  MmcTuningLib|BootloaderCommonPkg/Library/MmcTuningLib/MmcTuningLib.inf
  RpmbLib|BootloaderCommonPkg/Library/RpmbLib/RpmbLib.inf
  NvmExpressLib|BootloaderCommonPkg/Library/NvmExpressLib/NvmExpressLib.inf
  MemoryDeviceBlockIoLib|BootloaderCommonPkg/Library/MemoryDeviceBlockIoLib/MemoryDeviceBlockIoLib.inf
  AhciLib|BootloaderCommonPkg/Library/AhciLib/AhciLib.inf
  UsbIoLib|BootloaderCommonPkg/Library/UsbIoLib/UsbIoLib.inf
  UsbKbLib|BootloaderCommonPkg/Library/UsbKbLib/UsbKbLib.inf
  UsbHostCtrlLib|BootloaderCommonPkg/Library/XhciLib/XhciLib.inf
  UsbBusLib|BootloaderCommonPkg/Library/UsbBusLib/UsbBusLib.inf
  UsbBlockIoLib|BootloaderCommonPkg/Library/UsbBlockIoLib/UsbBlockIoLib.inf
  IasImageLib|BootloaderCommonPkg/Library/IasImageLib/IasImageLib.inf
  MultibootLib|BootloaderCommonPkg/Library/MultibootLib/MultibootLib.inf
  MediaAccessLib|BootloaderCommonPkg/Library/MediaAccessLib/MediaAccessLib.inf
  ResetSystemLib|BootloaderCommonPkg/Library/ResetSystemLib/ResetSystemLib.inf
  BootOptionLib|BootloaderCommonPkg/Library/BootOptionLib/BootOptionLib.inf
  RngLib|MdePkg/Library/BaseRngLib/BaseRngLib.inf
  SeedListInfoLib|BootloaderCommonPkg/Library/SeedListInfoLib/SeedListInfoLib.inf
  CryptoLib|BootloaderCommonPkg/Library/IppCryptoLib/IppCryptoLib.inf
  ShellExtensionLib|BootloaderCommonPkg/Library/ShellExtensionLibNull/ShellExtensionLibNull.inf
  DebugLogBufferLib|BootloaderCommonPkg/Library/DebugLogBufferLib/DebugLogBufferLib.inf
  DebugLib|BootloaderCommonPkg/Library/BootloaderDebugLib/BootloaderDebugLib.inf
  ConsoleInLib|BootloaderCommonPkg/Library/ConsoleInLib/ConsoleInLib.inf
  ConsoleOutLib|BootloaderCommonPkg/Library/ConsoleOutLib/ConsoleOutLib.inf
!if $(HAVE_FSP_BIN)
  FspApiLib|$(PLATFORM_PACKAGE)/Library/FspApiLib/FspApiLib.inf
!endif
!if $(ENABLE_SOURCE_DEBUG)
  DebugAgentLib|BootloaderCommonPkg/Library/DebugAgentLib/DebugAgentLib.inf
!else
  DebugAgentLib|BootloaderCommonPkg/Library/DebugAgentLib/DebugAgentLibNull.inf
!endif
  ElfLib|BootloaderCommonPkg/Library/ElfLib/ElfLib.inf

[LibraryClasses.IA32]
  PagingLib|$(PLATFORM_PACKAGE)/Library/PagingLib/PagingLib.inf
  TimerLib|BootloaderCommonPkg/Library/AcpiTimerLib/AcpiTimerLib.inf

################################################################################
#
# Pcd Section - list of all EDK II PCD Entries defined by this Platform
#
################################################################################
[PcdsFixedAtBuild]
!if $(TARGET) == RELEASE
  # Use this PCD to control the debug message that goes into binary image
  gEfiMdePkgTokenSpaceGuid.PcdFixedDebugPrintErrorLevel     | 0x80080003
  gEfiMdePkgTokenSpaceGuid.PcdDebugPropertyMask            | 0x23
!else
  gEfiMdePkgTokenSpaceGuid.PcdDebugPropertyMask            | 0x27
!endif

  gPlatformModuleTokenSpaceGuid.PcdDebugInterfaceFlags    | $(DEBUG_INTERFACE_FLAGS)

  gPlatformModuleTokenSpaceGuid.PcdPciMmcfgBase           | $(PCI_EXPRESS_BASE)
  gPlatformModuleTokenSpaceGuid.PcdPciResourceIoBase      | $(PCI_IO_BASE)
  gPlatformModuleTokenSpaceGuid.PcdPciResourceMem32Base   | $(PCI_MEM32_BASE)

  gPlatformModuleTokenSpaceGuid.PcdLoaderReservedMemSize  | $(LOADER_RSVD_MEM_SIZE)
  gPlatformModuleTokenSpaceGuid.PcdLoaderAcpiNvsSize      | $(LOADER_ACPI_NVS_MEM_SIZE)
  gPlatformModuleTokenSpaceGuid.PcdLoaderAcpiReclaimSize  | $(LOADER_ACPI_RECLAIM_MEM_SIZE)

  gPlatformModuleTokenSpaceGuid.PcdFlashBaseAddress       | $(FLASH_BASE)
  gPlatformModuleTokenSpaceGuid.PcdFlashSize              | $(FLASH_SIZE)

  gPlatformModuleTokenSpaceGuid.PcdStage1ABase            | $(STAGE1A_BASE)
  gPlatformModuleTokenSpaceGuid.PcdStage1ASize            | $(STAGE1A_SIZE)
  gPlatformModuleTokenSpaceGuid.PcdStage1ALoadBase        | $(STAGE1A_LOAD_BASE)
  gPlatformModuleTokenSpaceGuid.PcdStage1AFdBase          | $(STAGE1A_FD_BASE)
  gPlatformModuleTokenSpaceGuid.PcdStage1AFdSize          | $(STAGE1A_FD_SIZE)
  gPlatformModuleTokenSpaceGuid.PcdStage1AFvSize          | $(STAGE1A_FV_SIZE)

  gPlatformModuleTokenSpaceGuid.PcdStage1BBase            | $(STAGE1B_BASE)
  gPlatformModuleTokenSpaceGuid.PcdStage1BSize            | $(STAGE1B_SIZE)
  gPlatformModuleTokenSpaceGuid.PcdStage1BLoadBase        | $(STAGE1B_LOAD_BASE)
  gPlatformModuleTokenSpaceGuid.PcdStage1BFdBase          | $(STAGE1B_FD_BASE)
  gPlatformModuleTokenSpaceGuid.PcdStage1BFdSize          | $(STAGE1B_FD_SIZE)
  gPlatformModuleTokenSpaceGuid.PcdStage1BFvSize          | $(STAGE1B_FV_SIZE)

  gPlatformModuleTokenSpaceGuid.PcdStage2Base             | $(STAGE2_BASE)
  gPlatformModuleTokenSpaceGuid.PcdStage2Size             | $(STAGE2_SIZE)
  gPlatformModuleTokenSpaceGuid.PcdStage2LoadBase         | $(STAGE2_LOAD_BASE)
  gPlatformModuleTokenSpaceGuid.PcdStage2FdBase           | $(STAGE2_FD_BASE)
  gPlatformModuleTokenSpaceGuid.PcdStage2FdSize           | $(STAGE2_FD_SIZE)
  gPlatformModuleTokenSpaceGuid.PcdStage2FvSize           | $(STAGE2_FV_SIZE)

  gPlatformModuleTokenSpaceGuid.PcdStage1StackSize        | $(STAGE1_STACK_SIZE)
  gPlatformModuleTokenSpaceGuid.PcdStage1DataSize         | $(STAGE1_DATA_SIZE)
  gPlatformModuleTokenSpaceGuid.PcdStage2LoadHigh         | $(STAGE2_LOAD_HIGH)

  gPlatformModuleTokenSpaceGuid.PcdPayloadLoadHigh        | $(PAYLOAD_LOAD_HIGH)
  gPlatformModuleTokenSpaceGuid.PcdPayloadExeBase         | $(PAYLOAD_EXE_BASE)
  gPlatformModuleTokenSpaceGuid.PcdPayloadLoadBase        | $(PAYLOAD_LOAD_BASE)
  gPlatformModuleTokenSpaceGuid.PcdPayloadBase            | $(PAYLOAD_BASE)
  gPlatformModuleTokenSpaceGuid.PcdPayloadSize            | $(PAYLOAD_SIZE)
  gPlatformModuleTokenSpaceGuid.PcdFwuPayloadSize         | $(FWUPDATE_SIZE)
  gPlatformModuleTokenSpaceGuid.PcdFwuPayloadBase         | $(FWUPDATE_BASE)
  gPlatformModuleTokenSpaceGuid.PcdFwuPayloadLoadBase     | $(FWUPDATE_LOAD_BASE)
  gPlatformModuleTokenSpaceGuid.PcdCfgDataBase            | $(CFGDATA_BASE)
  gPlatformModuleTokenSpaceGuid.PcdCfgDataSize            | $(CFGDATA_SIZE)
  gPlatformModuleTokenSpaceGuid.PcdMrcDataBase            | $(MRCDATA_BASE)
  gPlatformModuleTokenSpaceGuid.PcdMrcDataSize            | $(MRCDATA_SIZE)
  gPlatformModuleTokenSpaceGuid.PcdUcodeBase              | $(UCODE_BASE)
  gPlatformModuleTokenSpaceGuid.PcdUcodeSize              | $(UCODE_SIZE)
  gPlatformModuleTokenSpaceGuid.PcdVariableRegionBase     | $(VARIABLE_BASE)
  gPlatformModuleTokenSpaceGuid.PcdVariableRegionSize     | $(VARIABLE_SIZE)

  gPlatformModuleTokenSpaceGuid.PcdFSPTBase               | $(FSP_T_BASE)
  gPlatformModuleTokenSpaceGuid.PcdFSPTSize               | $(FSP_T_SIZE)
  gPlatformModuleTokenSpaceGuid.PcdFSPTUpdSize            | $(FSP_T_UPD_SIZE)
  gPlatformModuleTokenSpaceGuid.PcdFSPMBase               | $(FSP_M_BASE)
  gPlatformModuleTokenSpaceGuid.PcdFSPMSize               | $(FSP_M_SIZE)
  gPlatformModuleTokenSpaceGuid.PcdFSPMUpdSize            | $(FSP_M_UPD_SIZE)
  gPlatformModuleTokenSpaceGuid.PcdFSPSSize               | $(FSP_S_SIZE)
  gPlatformModuleTokenSpaceGuid.PcdFSPSUpdSize            | $(FSP_S_UPD_SIZE)

  gPlatformModuleTokenSpaceGuid.PcdTopSwapRegionSize      | $(TOP_SWAP_SIZE)
  gPlatformModuleTokenSpaceGuid.PcdRedundantRegionSize    | $(REDUNDANT_SIZE)
  gPlatformModuleTokenSpaceGuid.PcdCfgDataLoadSource      | $(CFGDATA_REGION_TYPE)
  gPlatformModuleTokenSpaceGuid.PcdCfgDatabaseSize        | $(CFG_DATABASE_SIZE)

  gPlatformCommonLibTokenSpaceGuid.PcdDebugOutputDeviceMask  | $(DEBUG_OUTPUT_DEVICE_MASK)

  gPlatformCommonLibTokenSpaceGuid.PcdConsoleInDeviceMask  | $(CONSOLE_IN_DEVICE_MASK)
  gPlatformCommonLibTokenSpaceGuid.PcdConsoleOutDeviceMask | $(CONSOLE_OUT_DEVICE_MASK)

  gPlatformModuleTokenSpaceGuid.PcdVerifiedBootHashMask   | $(VERIFIED_BOOT_HASH_MASK)
  gPlatformCommonLibTokenSpaceGuid.PcdCryptoShaNiEnabled  | $(ENABLE_CRYPTO_SHA_NI)

  gPlatformCommonLibTokenSpaceGuid.PcdSpiIasImageRegionType    | $(SPI_IAS_REGION_TYPE)
  gPlatformCommonLibTokenSpaceGuid.PcdSpiIasImage1RegionBase   | $(SPI_IAS1_BASE)
  gPlatformCommonLibTokenSpaceGuid.PcdSpiIasImage1RegionSize   | $(SPI_IAS1_SIZE)
  gPlatformCommonLibTokenSpaceGuid.PcdSpiIasImage2RegionBase   | $(SPI_IAS2_BASE)
  gPlatformCommonLibTokenSpaceGuid.PcdSpiIasImage2RegionSize   | $(SPI_IAS2_SIZE)

  gPlatformCommonLibTokenSpaceGuid.PcdSupportedMediaTypeMask | $(BOOT_MEDIA_SUPPORT_MASK)

  gPlatformCommonLibTokenSpaceGuid.PcdSeedListEnabled     | $(HAVE_SEED_LIST)
  gPlatformCommonLibTokenSpaceGuid.PcdUsbKeyboardPollingTimeout | $(USB_KB_POLLING_TIMEOUT)

[PcdsPatchableInModule]
  gEfiMdePkgTokenSpaceGuid.PcdDebugPrintErrorLevel   | 0x8000004F
  gEfiMdePkgTokenSpaceGuid.PcdPciExpressBaseAddress  | $(PCI_EXPRESS_BASE)
  gPlatformModuleTokenSpaceGuid.PcdAcpiTablesRsdp    | 0xFF000000
  gPlatformModuleTokenSpaceGuid.PcdAcpiTablesAddress | 0xFF000000
  gPlatformModuleTokenSpaceGuid.PcdAcpiGnvsAddress   | 0xFF000000
  gPlatformModuleTokenSpaceGuid.PcdGraphicsVbtAddress| 0xFF000000
  gPlatformModuleTokenSpaceGuid.PcdDeviceTreeBase    | 0xFF000000
  gPlatformCommonLibTokenSpaceGuid.PcdAcpiPmTimerBase   | $(ACPI_PM_TIMER_BASE)
  gPlatformModuleTokenSpaceGuid.PcdHashStoreBase     | 0xFF000000
  gPlatformModuleTokenSpaceGuid.PcdVerInfoBase       | 0xFF000000
  gPlatformModuleTokenSpaceGuid.PcdFileDataBase      | 0xFF000000
  gPlatformModuleTokenSpaceGuid.PcdFSPSBase          | $(FSP_S_BASE)
  gPlatformModuleTokenSpaceGuid.PcdSplashLogoAddress | 0xFF000000
  gPlatformCommonLibTokenSpaceGuid.PcdEmmcMaxRwBlockNumber     | 0xFFFF
  gPlatformModuleTokenSpaceGuid.PcdPayloadReservedMemSize | $(PLD_RSVD_MEM_SIZE)
  gPlatformModuleTokenSpaceGuid.PcdSmramTsegBase     | 0xFF000000
  gPlatformModuleTokenSpaceGuid.PcdSmramTsegSize     | 0x00000000

  gPayloadTokenSpaceGuid.PcdPayloadHobList           | 0x00000000
  gPayloadTokenSpaceGuid.PcdPayloadHeapSize          | $(PLD_HEAP_SIZE)
  gPayloadTokenSpaceGuid.PcdPayloadStackSize         | $(PLD_STACK_SIZE)
  gPayloadTokenSpaceGuid.PcdGlobalDataAddress        | 0x00000000

[PcdsFeatureFlag]
  gPlatformCommonLibTokenSpaceGuid.PcdMinDecompression    | FALSE
  gPlatformCommonLibTokenSpaceGuid.PcdMeasuredBootEnabled | $(HAVE_MEASURED_BOOT)
  gPlatformCommonLibTokenSpaceGuid.PcdVerifiedBootEnabled | $(HAVE_VERIFIED_BOOT)
  gPlatformModuleTokenSpaceGuid.PcdIntelGfxEnabled        | $(HAVE_VBT_BIN)
  gPlatformModuleTokenSpaceGuid.PcdAcpiEnabled            | $(HAVE_ACPI_TABLE)
  gPlatformModuleTokenSpaceGuid.PcdSmpEnabled             | $(ENABLE_SMP_INIT)
  gPlatformModuleTokenSpaceGuid.PcdPciEnumEnabled         | $(ENABLE_PCI_ENUM)
  gPlatformModuleTokenSpaceGuid.PcdStage1AXip             | $(STAGE1A_XIP)
  gPlatformModuleTokenSpaceGuid.PcdStage1BXip             | $(STAGE1B_XIP)
  gPlatformModuleTokenSpaceGuid.PcdLoadImageUseFsp        | $(ENABLE_FSP_LOAD_IMAGE)
  gPlatformModuleTokenSpaceGuid.PcdSplashEnabled          | $(ENABLE_SPLASH)
  gPlatformModuleTokenSpaceGuid.PcdFramebufferInitEnabled | $(ENABLE_FRAMEBUFFER_INIT)
  gPlatformModuleTokenSpaceGuid.PcdVtdEnabled             | $(ENABLE_VTD)
  gPlatformModuleTokenSpaceGuid.PcdFlashMapEnabled        | $(HAVE_FLASH_MAP)
  gPlatformModuleTokenSpaceGuid.PcdPsdBiosEnabled         | $(HAVE_PSD_TABLE)
  gPlatformModuleTokenSpaceGuid.PcdSmmRebaseEnabled       | $(ENABLE_SMM_REBASE)

!ifdef $(S3_DEBUG)
  gPlatformModuleTokenSpaceGuid.PcdS3DebugEnabled         | $(S3_DEBUG)
!endif
  gPlatformCommonLibTokenSpaceGuid.PcdSourceDebugEnabled  | $(ENABLE_SOURCE_DEBUG)

[PcdsDynamicDefault]
  gEfiMdePkgTokenSpaceGuid.PcdPlatformBootTimeOut         | 2

###############################################################################
##
## Because of some odd behavior with current EDKII build tools,
## putting the !if around the [Section Header] does not work quite right.
## For example, the below will only work when the !if evaluates to false.
##   !if $(X64_CONFIG) == TRUE
##     [Components.X64]
##   !else
##     [Components.IA32]
##   !endif
##       !include Path/to/file.dsc
##
##  This is needed since Components sections must specify and Arch (ie no Common)
###############################################################################

[Components]
  #
  # SEC
  #
  $(PLATFORM_PACKAGE)/PcdData/PcdData.inf

  $(PLATFORM_PACKAGE)/Stage1A/Stage1A.inf {
    <PcdsFeatureFlag>
      gPlatformCommonLibTokenSpaceGuid.PcdMinDecompression | TRUE
    <LibraryClasses>
      SocInitLib   | Silicon/$(SILICON_PKG_NAME)/Library/Stage1ASocInitLib/Stage1ASocInitLib.inf
      BoardInitLib | Platform/$(BOARD_PKG_NAME)/Library/Stage1ABoardInitLib/Stage1ABoardInitLib.inf
!if $(SKIP_STAGE1A_SOURCE_DEBUG)
      DebugAgentLib| BootloaderCommonPkg/Library/DebugAgentLib/DebugAgentLibNull.inf
!endif
  }

  $(PLATFORM_PACKAGE)/Stage1B/Stage1B.inf {
    <LibraryClasses>
      SocInitLib   | Silicon/$(SILICON_PKG_NAME)/Library/Stage1BSocInitLib/Stage1BSocInitLib.inf
      BoardInitLib | Platform/$(BOARD_PKG_NAME)/Library/Stage1BBoardInitLib/Stage1BBoardInitLib.inf
  }

  $(PLATFORM_PACKAGE)/Stage2/Stage2.inf {
    <LibraryClasses>
      SocInitLib   | Silicon/$(SILICON_PKG_NAME)/Library/Stage2SocInitLib/Stage2SocInitLib.inf
      BoardInitLib | Platform/$(BOARD_PKG_NAME)/Library/Stage2BoardInitLib/Stage2BoardInitLib.inf
  }

  PayloadPkg/OsLoader/OsLoader.inf {
    <LibraryClasses>
      MemoryAllocationLib | BootloaderCommonPkg/Library/FullMemoryAllocationLib/FullMemoryAllocationLib.inf
      PayloadEntryLib     | PayloadPkg/Library/PayloadEntryLib/PayloadEntryLib.inf
      PayloadSupportLib   | PayloadPkg/Library/PayloadSupportLib/PayloadSupportLib.inf
      BootloaderLib       | PayloadPkg/Library/PayloadLib/PayloadLib.inf
      AbSupportLib        | PayloadPkg/Library/AbSupportLib/AbSupportLib.inf
      SblParameterLib     | PayloadPkg/Library/SblParameterLib/SblParameterLib.inf
      TrustyBootLib       | PayloadPkg/Library/TrustyBootLib/TrustyBootLib.inf
  }

!if $(ENABLE_FWU)
  PayloadPkg/FirmwareUpdate/FirmwareUpdate.inf {
    <LibraryClasses>
      MemoryAllocationLib     | BootloaderCommonPkg/Library/FullMemoryAllocationLib/FullMemoryAllocationLib.inf
      PayloadEntryLib         | PayloadPkg/Library/PayloadEntryLib/PayloadEntryLib.inf
      PayloadSupportLib       | PayloadPkg/Library/PayloadSupportLib/PayloadSupportLib.inf
      BootloaderLib           | PayloadPkg/Library/PayloadLib/PayloadLib.inf
      FirmwareUpdateLib       | Silicon/$(SILICON_PKG_NAME)/Library/FirmwareUpdateLib/FirmwareUpdateLib.inf
  }
!endif

!if $(HAVE_ACPI_TABLE)
  Platform/$(BOARD_PKG_NAME)/AcpiTables/AcpiTables.inf
!endif

!if $(UCODE_SIZE) > 0
  Silicon/$(SILICON_PKG_NAME)/Microcode/Microcode.inf
!endif

[BuildOptions.Common.EDKII]
  # Enable link-time optimization when building with GCC49
  *_GCC49_IA32_CC_FLAGS = -flto
  *_GCC49_IA32_DLINK_FLAGS = -flto
  *_GCC5_IA32_CC_FLAGS = -fno-pic
  *_GCC5_IA32_DLINK_FLAGS = -no-pie
  *_GCC5_IA32_ASLCC_FLAGS = -fno-pic
  *_GCC5_IA32_ASLDLINK_FLAGS = -no-pie
  # Force synchronous PDB writes for parallel builds
  *_VS2015x86_IA32_CC_FLAGS = /FS
!if $(TARGET) == RELEASE
  *_*_*_CC_FLAGS = -DLITE_PRINT
!endif

