/** @file

  Copyright (c) 2017 - 2018, Intel Corporation. All rights reserved.<BR>
  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#include <PiPei.h>
#include <Library/DebugLib.h>
#include <Library/BootloaderCommonLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/PrintLib.h>
#include <Library/IoLib.h>
#include <Library/HobLib.h>
#include <Library/PayloadLib.h>
#include <Guid/OsBootOptionGuid.h>
#include <Guid/OsConfigDataHobGuid.h>
#include <Guid/LoaderPlatformInfoGuid.h>
#include <Library/TrustyBootLib.h>
#include <Library/SeedListInfoLib.h>
#include <Library/PcdLib.h>

#define VMM_OS_HEAP_SIZE               0x10000
#define VMM_OS_RUNTIME_MEM_SIZE        0x400000

typedef enum {
  TrustyNativeAndroid,
  TrustyNativeLinux,
  TrustyAplCwpAaaG,
  TrustyAplCwpLaaG,
  TrustyIclCwpAaaG,
  TrustyIclCwpLaaG,
  TrustyMaxType
} TRUSTY_BOOT_TYPE;

#pragma pack (1)

typedef struct {
  UINT32 Eip;
  UINT32 Eax;
  UINT32 Ebx;
  UINT32 Esi;
  UINT32 Edi;
  UINT32 Ecx;
} CPU_BOOT_STATE;

typedef struct {
  // use size as version. In future, we can only add members in the structure
  UINT32         SizeOfThisStruct;
  UINT32         Version;
  UINT32         HeapAddr;          // 64KB under 4G, SBL should reserve it in e820
  UINT32         SipiPage;          // 4KB  under 1M, SBL should reserve it in e820
  UINT32         VmmRuntimeAddr;    // 4MB  under 4G, SBL should reserve it in e820
  UINT32         TrustyRuntimeAddr; // 16MB under 4G, For Linux only, SBL should should reserve it in e820. For Android, ignore it and set to 0
  CPU_BOOT_STATE CpuState;
} VMM_BOOT_PARAM;

// SBL should put all parameters into a big structure,
// and put the address of the big structure in cmd line,
// like: "ImageBootParamsAddr=0xXXXXXXXX"
typedef struct {
  UINT32 SizeOfThisStruct;
  UINT32 Version;
  UINT64 SeedListInfoAddr;
  UINT64 PlatformInfoAddr;
  UINT64 VmmBootParamAddr;
} IMAGE_BOOT_PARAM;

#pragma pack ()

