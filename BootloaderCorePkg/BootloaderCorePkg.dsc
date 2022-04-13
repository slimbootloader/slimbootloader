## @file
# Provides driver and definitions to build bootloader.
#
# Copyright (c) 2016 - 2022, Intel Corporation. All rights reserved.<BR>
# SPDX-License-Identifier: BSD-2-Clause-Patent
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
  OUTPUT_DIRECTORY                    = Build/BootloaderCorePkg
  SUPPORTED_ARCHITECTURES             = IA32|X64
  BUILD_TARGETS                       = DEBUG|RELEASE|NOOPT
  SKUID_IDENTIFIER                    = DEFAULT
  FLASH_DEFINITION                    = BootloaderCorePkg/BootloaderCorePkg.fdf

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
  RegisterFilterLib|MdePkg/Library/RegisterFilterLibNull/RegisterFilterLibNull.inf
  BaseLib|MdePkg/Library/BaseLib/BaseLib.inf
  IoLib|MdePkg/Library/BaseIoLibIntrinsic/BaseIoLibIntrinsic.inf
  PciLib|MdePkg/Library/BasePciLibPciExpress/BasePciLibPciExpress.inf
  PciCf8Lib|MdePkg/Library/BasePciCf8Lib/BasePciCf8Lib.inf
  PciExpressLib|MdePkg/Library/BasePciExpressLib/BasePciExpressLib.inf
  PciSegmentLib|MdePkg/Library/BasePciSegmentLibPci/BasePciSegmentLibPci.inf
  DebugPrintErrorLevelLib|BootloaderCorePkg/Library/DebugPrintErrorLevelLib/DebugPrintErrorLevelLib.inf
  PrintLib|MdePkg/Library/BasePrintLib/BasePrintLib.inf
  BaseMemoryLib|MdePkg/Library/BaseMemoryLibSse2/BaseMemoryLibSse2.inf
  SynchronizationLib|MdePkg/Library/BaseSynchronizationLib/BaseSynchronizationLib.inf
  TimeStampLib|BootloaderCommonPkg/Library/TimeStampLib/TimeStampLib.inf
  ExtraBaseLib|BootloaderCommonPkg/Library/ExtraBaseLib/ExtraBaseLib.inf
  ModuleEntryLib|BootloaderCommonPkg/Library/ModuleEntryLib/ModuleEntryLib.inf
  LzmaDecompressLib|BootloaderCommonPkg/Library/LzmaCustomDecompressLib/LzmaCustomDecompressLib.inf
  Lz4CompressLib|BootloaderCommonPkg/Library/Lz4CompressLib/Lz4CompressLib.inf
  DecompressLib|BootloaderCommonPkg/Library/DecompressLib/DecompressLib.inf
  RleCompressLib|BootloaderCommonPkg/Library/RleCompressLib/RleCompressLib.inf
  FspSupportLib|BootloaderCorePkg/Library/FspSupportLib/FspSupportLib.inf
  BootloaderLib|BootloaderCorePkg/Library/BootloaderLib/BootloaderLib.inf
  BootloaderCoreLib|BootloaderCorePkg/Library/BootloaderCoreLib/BootloaderCoreLib.inf
  PciEnumerationLib|BootloaderCorePkg/Library/PciEnumerationLib/PciEnumerationLib.inf
  AcpiInitLib|BootloaderCorePkg/Library/AcpiInitLib/AcpiInitLib.inf
  SmbiosInitLib|BootloaderCorePkg/Library/SmbiosInitLib/SmbiosInitLib.inf
  LitePeCoffLib|BootloaderCommonPkg/Library/LitePeCoffLib/LitePeCoffLib.inf
  HobLib|BootloaderCommonPkg/Library/HobLib/HobLib.inf
  HobBuildLib|BootloaderCorePkg/Library/HobBuildLib/HobBuildLib.inf
  LiteFvLib|BootloaderCommonPkg/Library/LiteFvLib/LiteFvLib.inf
  LoaderPerformanceLib|BootloaderCommonPkg/Library/LoaderPerformanceLib/LoaderPerformanceLib.inf
  MemoryAllocationLib|BootloaderCorePkg/Library/MemoryAllocationLib/MemoryAllocationLib.inf
  MpInitLib|BootloaderCorePkg/Library/MpInitLib/MpInitLib.inf
  StageLib|BootloaderCorePkg/Library/StageLib/StageLib.inf
  LocalApicLib|BootloaderCommonPkg/Library/BaseXApicX2ApicLib/BaseXApicX2ApicLib.inf
  SecureBootLib|BootloaderCommonPkg/Library/SecureBootLib/SecureBootLib.inf
  TpmLib|BootloaderCommonPkg/Library/TpmLib/TpmLib.inf
  BootloaderCommonLib|BootloaderCommonPkg/Library/BootloaderCommonLib/BootloaderCommonLib.inf
  ConfigDataLib|BootloaderCommonPkg/Library/ConfigDataLib/ConfigDataLib.inf
  MmcAccessLib|BootloaderCommonPkg/Library/MmcAccessLib/MmcAccessLib.inf
  GraphicsLib|BootloaderCommonPkg/Library/GraphicsLib/GraphicsLib.inf
  Crc32Lib|BootloaderCommonPkg/Library/Crc32Lib/Crc32Lib.inf
  VariableLib|BootloaderCommonPkg/Library/LiteVariableLib/LiteVariableLib.inf
  DebugDataLib|BootloaderCorePkg/Library/DebugDataLib/DebugDataLib.inf
  CpuExceptionLib|BootloaderCorePkg/Library/CpuExceptionLib/CpuExceptionLib.inf
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
  UsbInitLib|BootloaderCommonPkg/Library/UsbInitLib/UsbInitLib.inf
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
  ContainerLib|BootloaderCommonPkg/Library/ContainerLib/ContainerLib.inf
  LinuxLib|BootloaderCommonPkg/Library/LinuxLib/LinuxLib.inf
  UefiVariableLib|BootloaderCommonPkg/Library/UefiVariableLib/UefiVariableLib.inf
  SerialPortLib|BootloaderCommonPkg/Library/SerialPortLib/SerialPortLib.inf
  SortLib|BootloaderCommonPkg/Library/SortLib/SortLib.inf
  IoMmuLib|BootloaderCommonPkg/Library/IoMmuLib/IoMmuLib.inf
  MtrrLib|BootloaderCommonPkg/Library/MtrrLib/MtrrLib.inf
  StringSupportLib|BootloaderCommonPkg/Library/StringSupportLib/StringSupportLib.inf
  ThunkLib|BootloaderCommonPkg/Library/ThunkLib/ThunkLib.inf
  UniversalPayloadLib|BootloaderCommonPkg/Library/UniversalPayloadLib/UniversalPayloadLib.inf

!if $(ENABLE_SOURCE_DEBUG)
  DebugAgentLib|BootloaderCommonPkg/Library/DebugAgentLib/DebugAgentLib.inf
!else
  DebugAgentLib|BootloaderCommonPkg/Library/DebugAgentLib/DebugAgentLibNull.inf
!endif
  ElfLib|BootloaderCommonPkg/Library/ElfLib/ElfLib.inf
  S3SaveRestoreLib|BootloaderCorePkg/Library/S3SaveRestoreLib/S3SaveRestoreLib.inf
  BoardSupportLib|Platform/CommonBoardPkg/Library/BoardSupportLib/BoardSupportLib.inf
  PagingLib|BootloaderCommonPkg/Library/PagingLib/PagingLib.inf
  TimerLib|BootloaderCommonPkg/Library/AcpiTimerLib/AcpiTimerLib.inf
  DebugPortLib|BootloaderCommonPkg/Library/DebugPortLib/DebugPortLibNull.inf

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

  # Exclude platform console devices BIT7 as debug output for SBL stage 1 and 2.
  # Once in payload, more debug devices can be enabled, such as frame buffer.
  gPlatformCommonLibTokenSpaceGuid.PcdDebugOutputDeviceMask  | $(DEBUG_OUTPUT_DEVICE_MASK) & 0xFFFFFF7F

  gPlatformCommonLibTokenSpaceGuid.PcdDebugPortNumber      | $(DEBUG_PORT_NUMBER)

  gPlatformModuleTokenSpaceGuid.PcdPciMmcfgBase           | $(PCI_EXPRESS_BASE)
  gPlatformModuleTokenSpaceGuid.PcdPciEnumPolicyInfo      | $(PCI_ENUM_POLICY_INFO)

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
  gPlatformModuleTokenSpaceGuid.PcdStage1StackBaseOffset  | $(STAGE1_STACK_BASE_OFFSET)
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
  gPlatformModuleTokenSpaceGuid.PcdFSPMStackTop           | $(FSP_M_STACK_TOP)

  gPlatformModuleTokenSpaceGuid.PcdMemoryMapEntryNumber   | $(MAX_MEMORY_MAP_ENTRY_NUM)

  gPlatformModuleTokenSpaceGuid.PcdTopSwapRegionSize      | $(TOP_SWAP_SIZE)
  gPlatformModuleTokenSpaceGuid.PcdRedundantRegionSize    | $(REDUNDANT_SIZE)
  gPlatformModuleTokenSpaceGuid.PcdCfgDataLoadSource      | $(CFGDATA_REGION_TYPE)
  gPlatformModuleTokenSpaceGuid.PcdCfgDatabaseSize        | $(CFG_DATABASE_SIZE)

  gPlatformModuleTokenSpaceGuid.PcdHashStoreSize          | $(HASH_STORE_SIZE)

  gPlatformModuleTokenSpaceGuid.PcdAcpiProcessorIdBase    | $(ACPI_PROCESSOR_ID_BASE)
  gPlatformModuleTokenSpaceGuid.PcdCpuMaxLogicalProcessorNumber | $(CPU_MAX_LOGICAL_PROCESSOR_NUMBER)
  gPlatformModuleTokenSpaceGuid.PcdCpuSortMethod          | $(CPU_SORT_METHOD)

  gPlatformCommonLibTokenSpaceGuid.PcdConsoleInDeviceMask  | $(CONSOLE_IN_DEVICE_MASK)
  gPlatformCommonLibTokenSpaceGuid.PcdConsoleOutDeviceMask | $(CONSOLE_OUT_DEVICE_MASK)

  gPlatformModuleTokenSpaceGuid.PcdVerifiedBootStage1B   | $(VERIFIED_BOOT_STAGE_1B)
  gPlatformCommonLibTokenSpaceGuid.PcdCryptoShaOptMask    | $(ENABLE_CRYPTO_SHA_OPT)

  gPlatformCommonLibTokenSpaceGuid.PcdSpiIasImageRegionType    | $(SPI_IAS_REGION_TYPE)
  gPlatformCommonLibTokenSpaceGuid.PcdSpiIasImage1RegionBase   | $(SPI_IAS1_BASE)
  gPlatformCommonLibTokenSpaceGuid.PcdSpiIasImage1RegionSize   | $(SPI_IAS1_SIZE)
  gPlatformCommonLibTokenSpaceGuid.PcdSpiIasImage2RegionBase   | $(SPI_IAS2_BASE)
  gPlatformCommonLibTokenSpaceGuid.PcdSpiIasImage2RegionSize   | $(SPI_IAS2_SIZE)

  gPlatformCommonLibTokenSpaceGuid.PcdSupportedMediaTypeMask   | $(BOOT_MEDIA_SUPPORT_MASK)
  gPlatformCommonLibTokenSpaceGuid.PcdSupportedFileSystemMask  | $(FILE_SYSTEM_SUPPORT_MASK)

  gPlatformCommonLibTokenSpaceGuid.PcdSeedListEnabled     | $(HAVE_SEED_LIST)
  gPlatformCommonLibTokenSpaceGuid.PcdUsbKeyboardPollingTimeout | $(USB_KB_POLLING_TIMEOUT)
  gPlatformCommonLibTokenSpaceGuid.PcdUsbCmdTimeout             | $(USB_CMD_TIMEOUT)
  gPlatformCommonLibTokenSpaceGuid.PcdLowestSupportedFwVer      | $(LOWEST_SUPPORTED_FW_VER)

  gPlatformCommonLibTokenSpaceGuid.PcdIppHashLibSupportedMask    | $(IPP_HASH_LIB_SUPPORTED_MASK)

  gPlatformCommonLibTokenSpaceGuid.PcdCompSignHashAlg             | $(SIGN_HASH_TYPE)
  gPlatformModuleTokenSpaceGuid.PcdFastBootEnabled                | $(ENABLE_FAST_BOOT)

  gPayloadTokenSpaceGuid.PcdRtcmRsvdSize                        | $(RTCM_RSVD_SIZE)


[PcdsPatchableInModule]
  gEfiMdePkgTokenSpaceGuid.PcdDebugPrintErrorLevel   | 0x8000004F
  gEfiMdePkgTokenSpaceGuid.PcdPciExpressBaseAddress  | $(PCI_EXPRESS_BASE)
  gPlatformModuleTokenSpaceGuid.PcdAcpiTablesRsdp    | 0xFF000000
  gPlatformModuleTokenSpaceGuid.PcdAcpiTablesAddress | 0xFF000000
  gPlatformModuleTokenSpaceGuid.PcdAcpiGnvsAddress   | 0xFF000000
  gPlatformModuleTokenSpaceGuid.PcdGraphicsVbtAddress| 0xFF000000
  gPlatformModuleTokenSpaceGuid.PcdIgdOpRegionAddress| 0xFF000000
  gPlatformModuleTokenSpaceGuid.PcdDeviceTreeBase    | 0xFF000000
  gPlatformCommonLibTokenSpaceGuid.PcdAcpiPmTimerBase   | $(ACPI_PM_TIMER_BASE)
  gPlatformModuleTokenSpaceGuid.PcdFSPSBase          | $(FSP_S_BASE)
  gPlatformModuleTokenSpaceGuid.PcdHashStoreBase     | 0xFF000000
  gPlatformModuleTokenSpaceGuid.PcdVerInfoBase       | 0xFF000000
  gPlatformModuleTokenSpaceGuid.PcdFileDataBase      | 0xFF000000
  gPlatformModuleTokenSpaceGuid.PcdSplashLogoAddress | 0xFF000000
  gPlatformModuleTokenSpaceGuid.PcdCfgDataIntBase    | 0xFF000000
  gPlatformCommonLibTokenSpaceGuid.PcdEmmcMaxRwBlockNumber     | 0xFFFF
  gPlatformModuleTokenSpaceGuid.PcdPayloadReservedMemSize | $(PLD_RSVD_MEM_SIZE)
  gPlatformModuleTokenSpaceGuid.PcdSmramTsegBase     | 0xFF000000
  gPlatformModuleTokenSpaceGuid.PcdSmramTsegSize     | 0x00000000

  gPayloadTokenSpaceGuid.PcdPayloadHobList           | 0x00000000
  gPayloadTokenSpaceGuid.PcdPayloadHeapSize          | $(PLD_HEAP_SIZE)
  gPayloadTokenSpaceGuid.PcdPayloadStackSize         | $(PLD_STACK_SIZE)
  gPayloadTokenSpaceGuid.PcdGlobalDataAddress        | 0x00000000

  gPlatformModuleTokenSpaceGuid.PcdSmmRebaseMode     | $(ENABLE_SMM_REBASE)

  gPlatformModuleTokenSpaceGuid.PcdSmbiosTablesBase  | 0x00000000
  gPlatformModuleTokenSpaceGuid.PcdSmbiosStringsPtr  | 0x00000000
  gPlatformModuleTokenSpaceGuid.PcdSmbiosStringsCnt  | 0x40
  gPlatformModuleTokenSpaceGuid.PcdFuncCpuInitHook   | 0x00000000
  gPlatformModuleTokenSpaceGuid.PcdFspsUpdPtr        | 0x00000000
  gPlatformModuleTokenSpaceGuid.PcdAcpiTableTemplatePtr | 0
  gPlatformModuleTokenSpaceGuid.PcdPciEnumHookProc   | 0x00000000

  gPlatformModuleTokenSpaceGuid.PcdPciResAllocTableBase    | 0x00000000

  gPlatformCommonLibTokenSpaceGuid.PcdSerialUseMmio        | 0
  gPlatformCommonLibTokenSpaceGuid.PcdSerialRegisterBase   | 0
  gPlatformCommonLibTokenSpaceGuid.PcdSerialBaudRate       | 0
  gPlatformCommonLibTokenSpaceGuid.PcdSerialRegisterStride | 0
  gPlatformCommonLibTokenSpaceGuid.PcdSerialClockRate      | 0

  gPlatformModuleTokenSpaceGuid.PcdPciResourceIoBase      | $(PCI_IO_BASE)
  gPlatformModuleTokenSpaceGuid.PcdPciResourceMem32Base   | $(PCI_MEM32_BASE)
  gPlatformModuleTokenSpaceGuid.PcdPciResourceMem64Base   | $(PCI_MEM64_BASE)

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
  gPlatformCommonLibTokenSpaceGuid.PcdTccEnabled          | $(ENABLE_TCC)
  gPlatformModuleTokenSpaceGuid.PcdPsdBiosEnabled         | $(HAVE_PSD_TABLE)
  gPayloadTokenSpaceGuid.PcdGrubBootCfgEnabled            | $(ENABLE_GRUB_CONFIG)
  gPlatformModuleTokenSpaceGuid.PcdSmbiosEnabled          | $(ENABLE_SMBIOS)
  gPlatformModuleTokenSpaceGuid.PcdLinuxPayloadEnabled    | $(ENABLE_LINUX_PAYLOAD)
  gPlatformCommonLibTokenSpaceGuid.PcdContainerBootEnabled| $(ENABLE_CONTAINER_BOOT)
  gPayloadTokenSpaceGuid.PcdCsmeUpdateEnabled             | $(ENABLE_CSME_UPDATE)
  gPlatformModuleTokenSpaceGuid.PcdLegacyEfSegmentEnabled | $(ENABLE_LEGACY_EF_SEG)
  gPlatformCommonLibTokenSpaceGuid.PcdEmmcHs400SupportEnabled | $(ENABLE_EMMC_HS400)
  gPlatformCommonLibTokenSpaceGuid.PcdDmaProtectionEnabled | $(ENABLE_DMA_PROTECTION)
  gPlatformCommonLibTokenSpaceGuid.PcdMultiUsbBootDeviceEnabled |  $(ENABLE_MULTI_USB_BOOT_DEV)
  gPlatformCommonLibTokenSpaceGuid.PcdCpuX2ApicEnabled    | $(SUPPORT_X2APIC)
  gPlatformModuleTokenSpaceGuid.PcdAriSupport             | $(SUPPORT_ARI)
  gPlatformModuleTokenSpaceGuid.PcdSrIovSupport           | $(SUPPORT_SR_IOV)
  gPlatformModuleTokenSpaceGuid.PcdEnableSetup            | $(ENABLE_SBL_SETUP)
  gPayloadTokenSpaceGuid.PcdPayloadModuleEnabled          | $(ENABLE_PAYLOD_MODULE)
  gPlatformModuleTokenSpaceGuid.PcdEnableDts              | $(ENABLE_DTS)
  gPlatformModuleTokenSpaceGuid.PcdEnablePciePm           | $(ENABLE_PCIE_PM)

!ifdef $(S3_DEBUG)
  gPlatformModuleTokenSpaceGuid.PcdS3DebugEnabled         | $(S3_DEBUG)
!endif
  gPlatformCommonLibTokenSpaceGuid.PcdSourceDebugEnabled  | $(ENABLE_SOURCE_DEBUG)

[PcdsDynamicDefault]
  gEfiMdePkgTokenSpaceGuid.PcdPlatformBootTimeOut         | 2
  gPlatformCommonLibTokenSpaceGuid.PcdMeasuredBootHashMask | 0x00000002

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
  BootloaderCorePkg/PcdData/PcdData.inf

  BootloaderCorePkg/Stage1A/Stage1A.inf {
    <PcdsFeatureFlag>
      gPlatformCommonLibTokenSpaceGuid.PcdMinDecompression | TRUE
      gPlatformCommonLibTokenSpaceGuid.PcdForceToInitSerialPort | TRUE
    <LibraryClasses>
      FspApiLib    | BootloaderCorePkg/Library/FspApiLib/FsptApiLib.inf
      BaseMemoryLib| MdePkg/Library/BaseMemoryLibRepStr/BaseMemoryLibRepStr.inf
      SocInitLib   | $(SOC_INIT_STAGE1A_LIB_INF_FILE)
      BoardInitLib | $(BRD_INIT_STAGE1A_LIB_INF_FILE)
!if $(SKIP_STAGE1A_SOURCE_DEBUG)
      DebugAgentLib| BootloaderCommonPkg/Library/DebugAgentLib/DebugAgentLibNull.inf
!endif
  }

  BootloaderCorePkg/Stage1B/Stage1B.inf {
    <LibraryClasses>
      FspApiLib    | BootloaderCorePkg/Library/FspApiLib/FspmApiLib.inf
      BaseMemoryLib| MdePkg/Library/BaseMemoryLibRepStr/BaseMemoryLibRepStr.inf
      SocInitLib   | $(SOC_INIT_STAGE1B_LIB_INF_FILE)
      BoardInitLib | $(BRD_INIT_STAGE1B_LIB_INF_FILE)
  }

  BootloaderCorePkg/Stage2/Stage2.inf {
    <LibraryClasses>
      FspApiLib    | BootloaderCorePkg/Library/FspApiLib/FspsApiLib.inf
      SocInitLib   | $(SOC_INIT_STAGE2_LIB_INF_FILE)
      BoardInitLib | $(BRD_INIT_STAGE2_LIB_INF_FILE)
  }

  PayloadPkg/OsLoader/OsLoader.inf {
    <PcdsFixedAtBuild>
      gPlatformCommonLibTokenSpaceGuid.PcdDebugOutputDeviceMask  | $(DEBUG_OUTPUT_DEVICE_MASK)
    <LibraryClasses>
      MemoryAllocationLib | BootloaderCommonPkg/Library/FullMemoryAllocationLib/FullMemoryAllocationLib.inf
      PayloadEntryLib     | PayloadPkg/Library/PayloadEntryLib/PayloadEntryLib.inf
      PayloadSupportLib   | PayloadPkg/Library/PayloadSupportLib/PayloadSupportLib.inf
      BootloaderLib       | PayloadPkg/Library/PayloadLib/PayloadLib.inf
      PlatformHookLib     | PayloadPkg/Library/PlatformHookLib/PlatformHookLib.inf
      AbSupportLib        | PayloadPkg/Library/AbSupportLib/AbSupportLib.inf
      SblParameterLib     | PayloadPkg/Library/SblParameterLib/SblParameterLib.inf
      TrustyBootLib       | PayloadPkg/Library/TrustyBootLib/TrustyBootLib.inf
      MpServiceLib        | PayloadPkg/Library/MpServiceLib/MpServiceLib.inf
  }

!if $(ENABLE_FWU)
  PayloadPkg/FirmwareUpdate/FirmwareUpdate.inf {
    <PcdsFixedAtBuild>
      gPlatformCommonLibTokenSpaceGuid.PcdDebugOutputDeviceMask  | $(DEBUG_OUTPUT_DEVICE_MASK)
      gPlatformCommonLibTokenSpaceGuid.PcdConsoleOutDeviceMask   | ($(CONSOLE_OUT_DEVICE_MASK) + 0x02)
    <LibraryClasses>
      MemoryAllocationLib     | BootloaderCommonPkg/Library/FullMemoryAllocationLib/FullMemoryAllocationLib.inf
      PayloadEntryLib         | PayloadPkg/Library/PayloadEntryLib/PayloadEntryLib.inf
      PayloadSupportLib       | PayloadPkg/Library/PayloadSupportLib/PayloadSupportLib.inf
      BootloaderLib           | PayloadPkg/Library/PayloadLib/PayloadLib.inf
      PlatformHookLib         | PayloadPkg/Library/PlatformHookLib/PlatformHookLib.inf
      FirmwareUpdateLib       | $(SOC_FWU_LIB_INF_FILE)
  }
!endif

!if $(BUILD_CSME_UPDATE_DRIVER)
  PayloadPkg/CsmeUpdateDriver/CsmeUpdateDriver.inf
!endif

!if $(HAVE_ACPI_TABLE)
  $(ACPI_TABLE_INF_FILE)
!endif

!if $(UCODE_SIZE) > 0
  $(MICROCODE_INF_FILE)
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
  *_XCODE5_IA32_CC_FLAGS = -flto

  *_*_*_CC_FLAGS = -DDISABLE_NEW_DEPRECATED_INTERFACES
!if $(TARGET) == RELEASE
  *_*_*_CC_FLAGS = -DLITE_PRINT
!endif

!if $(TARGET) == NOOPT
  # GCC: -O0 results in too big size. Override it to -O1 with lto
  *_GCC49_*_CC_FLAGS = -O1
  *_GCC49_*_DLINK_FLAGS = -O1
  *_GCC5_*_CC_FLAGS = -flto -O1
  *_GCC5_*_DLINK_FLAGS = -flto -O1
  # VS: Use default /Od for now
!endif

