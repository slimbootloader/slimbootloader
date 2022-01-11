## @file
# Provides driver and definitions to build payload module.
#
# Copyright (c) 2020, Intel Corporation. All rights reserved.<BR>
# SPDX-License-Identifier: BSD-2-Clause-Patent
#
##

################################################################################
#
# Defines Section - statements that will be processed to create a Makefile.
#
################################################################################
[Defines]
  PLATFORM_NAME                       = PayloadPkg
  PLATFORM_GUID                       = 5BCB969A-9151-45E0-BE14-048954BD2E8E
  PLATFORM_VERSION                    = 0.1
  DSC_SPECIFICATION                   = 0x00010005
  OUTPUT_DIRECTORY                    = Build/PayloadPkg
  SUPPORTED_ARCHITECTURES             = IA32|X64
  BUILD_TARGETS                       = DEBUG|RELEASE|NOOPT
  SKUID_IDENTIFIER                    = DEFAULT

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
  BaseLib | MdePkg/Library/BaseLib/BaseLib.inf
  BaseMemoryLib | MdePkg/Library/BaseMemoryLibSse2/BaseMemoryLibSse2.inf
  IoLib | MdePkg/Library/BaseIoLibIntrinsic/BaseIoLibIntrinsic.inf
  PrintLib | MdePkg/Library/BasePrintLib/BasePrintLib.inf
  SynchronizationLib|MdePkg/Library/BaseSynchronizationLib/BaseSynchronizationLib.inf
  HobLib | BootloaderCommonPkg/Library/HobLib/HobLib.inf
  DebugLogBufferLib | BootloaderCommonPkg/Library/DebugLogBufferLib/DebugLogBufferLib.inf
  SerialPortLib | BootloaderCommonPkg/Library/SerialPortLib/SerialPortLib.inf
  DebugLib | BootloaderCommonPkg/Library/BootloaderDebugLib/BootloaderDebugLib.inf
  PcdLib | BootloaderCommonPkg/Library/PcdLib/PcdLib.inf
  RegisterFilterLib|MdePkg/Library/RegisterFilterLibNull/RegisterFilterLibNull.inf
  ConsoleInLib | BootloaderCommonPkg/Library/ConsoleInLib/ConsoleInLib.inf
  ConsoleOutLib | BootloaderCommonPkg/Library/ConsoleOutLib/ConsoleOutLib.inf
  GraphicsLib | BootloaderCommonPkg/Library/GraphicsLib/GraphicsLib.inf
  MemoryAllocationLib | BootloaderCommonPkg/Library/FullMemoryAllocationLib/FullMemoryAllocationLib.inf
  ModuleEntryLib | BootloaderCommonPkg/Library/ModuleEntryLib/ModuleEntryLib.inf
  TimeStampLib | BootloaderCommonPkg/Library/TimeStampLib/TimeStampLib.inf
  LoaderPerformanceLib | BootloaderCommonPkg/Library/LoaderPerformanceLib/LoaderPerformanceLib.inf
  BootloaderCommonLib | BootloaderCommonPkg/Library/BootloaderCommonLib/BootloaderCommonLib.inf
  UsbKbLib | BootloaderCommonPkg/Library/UsbKbLib/UsbKbLibNull.inf
  PayloadSupportLib | PayloadPkg/Library/PayloadSupportLib/PayloadSupportLib.inf
  DebugPrintErrorLevelLib | PayloadPkg/Library/DebugPrintErrorLevelLib/DebugPrintErrorLevelLib.inf
  BootloaderLib | PayloadPkg/Library/PayloadLib/PayloadLib.inf
  PayloadEntryLib | PayloadPkg/Library/PayloadEntryLib/PayloadEntryLib.inf
  PlatformHookLib | PayloadPkg/Library/PlatformHookLib/PlatformHookLib.inf
  PagingLib|BootloaderCommonPkg/Library/PagingLib/PagingLib.inf
  ThunkLib|BootloaderCommonPkg/Library/ThunkLib/ThunkLib.inf
  LitePeCoffLib|BootloaderCommonPkg/Library/LitePeCoffLib/LitePeCoffLib.inf
  DebugAgentLib|BootloaderCommonPkg/Library/DebugAgentLib/DebugAgentLibNull.inf
  TimerLib|BootloaderCommonPkg/Library/AcpiTimerLib/AcpiTimerLib.inf
  DebugPortLib|BootloaderCommonPkg/Library/DebugPortLib/DebugPortLibNull.inf

[PcdsPatchableInModule]
  gEfiMdePkgTokenSpaceGuid.PcdDebugPrintErrorLevel         | 0x8000004F
  # Following will be initialized at runtime
  gPayloadTokenSpaceGuid.PcdGlobalDataAddress              | 0
  gPayloadTokenSpaceGuid.PcdPayloadHobList                 | 0
  gEfiMdePkgTokenSpaceGuid.PcdPciExpressBaseAddress        | 0
  gPlatformCommonLibTokenSpaceGuid.PcdAcpiPmTimerBase      | 0
  gPlatformCommonLibTokenSpaceGuid.PcdSerialUseMmio        | 0
  gPlatformCommonLibTokenSpaceGuid.PcdSerialRegisterBase   | 0
  gPlatformCommonLibTokenSpaceGuid.PcdSerialBaudRate       | 0
  gPlatformCommonLibTokenSpaceGuid.PcdSerialRegisterStride | 0
  gPlatformCommonLibTokenSpaceGuid.PcdSerialClockRate      | 0

[PcdsFixedAtBuild]
!if $(TARGET) == RELEASE
  # Use this PCD to control the debug message that goes into binary image
  gEfiMdePkgTokenSpaceGuid.PcdFixedDebugPrintErrorLevel    | 0x80080003
  gEfiMdePkgTokenSpaceGuid.PcdDebugPropertyMask            | 0x23
!else
  gEfiMdePkgTokenSpaceGuid.PcdDebugPropertyMask            | 0x27
!endif

  gPayloadTokenSpaceGuid.PcdPayloadHeapSize                | 0x02000000
  gPayloadTokenSpaceGuid.PcdPayloadStackSize               | 0x00010000

[Components]
  PayloadPkg/HelloWorld/HelloWorld.inf

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

!if $(TARGET) == NOOPT
  # GCC: -O0 results in too big size. Override it to -O1 with lto
  *_GCC49_*_CC_FLAGS = -O1
  *_GCC49_*_DLINK_FLAGS = -O1
  *_GCC5_*_CC_FLAGS = -flto -O1
  *_GCC5_*_DLINK_FLAGS = -flto -O1
  # VS: Use default /Od for now
!endif

