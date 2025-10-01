/** @file
  Copyright (c) 2025, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent
**/

#include <Base.h>
#include <Uefi/UefiBaseType.h>
#include <Library/IoLib.h>
#include <Library/DebugLib.h>
#include <Library/BaseLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/MemoryAllocationLib.h>
#include <PiPei.h>
#include <Pi/PiBootMode.h>
#include <Pi/PiHob.h>
#include <Library/HobLib.h>
#include <Library/LocalApicLib.h>
#include <Library/BootloaderCoreLib.h>
#include <CpuRegs.h>
#include <Txt.h>
#include <Register/Intel/ArchitecturalMsr.h>
#include <Register/Intel/Cpuid.h>
#include "TxtCtx.h"
#include <Library/MpInitLib.h>

GLOBAL_REMOVE_IF_UNREFERENCED TXT_LIB_CONTEXT mTxtLibCtx;

/**
  Sets up the system and then launches the TXT BIOS ACM to run the function
  requested by AcmFunction.

  @param[in] AcmFunction   - Constant that represents the function from the BIOS ACM
                  that should be executed.
  @param[in] TxtLibCtx     - A pointer to an initialized TXT DXE Context data structure

  @retval EFI_SUCCESS            - BIOS ACM is set up.
  @retval EFI_INVALID_PARAMETER  - Wrong data in TxtInfoHob.
  @retval EFI_NOT_FOUND          - BIOS ACM is not found
**/
EFI_STATUS
TxtLibLaunchBiosAcm (
  IN TXT_LIB_CONTEXT *TxtLibCtx,
  IN UINT64              AcmFunction
  );

VOID
DumpFspHobList (
  IN CONST VOID             *HobStart
  )
{
  EFI_PEI_HOB_POINTERS  GuidHob;

  GuidHob.Raw = (UINT8 *) HobStart;
  while ((GuidHob.Raw = GetNextHob (EFI_HOB_TYPE_GUID_EXTENSION, GuidHob.Raw)) != NULL) {
    DEBUG ((DEBUG_INFO, "SBL -- FSP Extended    GUID HOB: {%g}\n", &GuidHob.Guid->Name));
    GuidHob.Raw = GET_NEXT_HOB (GuidHob);
  }
}


/**
  This routine initializes and collects all Protocols and data required
  by the routines in this file.

  @param[in] TxtLibCtx     - A pointer to a caller allocated data structure that contains
                             all of the Protocols and data required by the routines in this file.

  @retval EFI_SUCCESS       - Return EFI_SUCCESS if no error happen
  @retval EFI_NOT_FOUND     - If TxtInfoHob is not found
**/
EFI_STATUS
InitializeTxtLib (
  IN OUT TXT_LIB_CONTEXT *TxtLibCtx
  )
{
  TXT_INFO_HOB *TxtInfoHob;
  SYS_CPU_INFO  *SysCpuInfo;
  VOID          *FspHobList;

  TxtLibCtx->ImageHandle  = NULL;

  TxtLibCtx->CpuCount = 0;

  SysCpuInfo = MpGetInfo();
  ASSERT(SysCpuInfo);

  if (SysCpuInfo) {
    TxtLibCtx->CpuCount = SysCpuInfo->CpuCount;
  }

  ///
  /// Find TxtInfoHob
  ///
  FspHobList = GetFspHobListPtr ();
  if (FspHobList == NULL) {
    DEBUG ((DEBUG_INFO, "TxtLib: GetLoaderGlobalDataPointer FAIL\n"));
    return EFI_NOT_FOUND;
  }

  DumpFspHobList(FspHobList);
  TxtInfoHob = GetNextGuidHob (&gTxtInfoHobGuid, FspHobList);
  if (TxtInfoHob == NULL) {
    DEBUG ((DEBUG_INFO, "TxtLib: Unable to find gTxtInfoHobGuid.\n"));
    return EFI_NOT_FOUND;
  }

  TxtLibCtx->TxtInfoData  = &(TxtInfoHob->Data);

  ///
  /// Print out the TxtInfo HOB
  ///
  DEBUG ((DEBUG_INFO, "TxtLib: TxtInfoHob passed from platform as:\n"));
  DEBUG ((DEBUG_INFO, "TxtLib: ChipsetIsTxtCapable = %x\n", TxtLibCtx->TxtInfoData->ChipsetIsTxtCapable));
  DEBUG ((DEBUG_INFO, "TxtLib: TxtMode             = %x\n", TxtLibCtx->TxtInfoData->TxtMode));
  DEBUG ((DEBUG_INFO, "TxtLib: ResetAux            = %x\n", TxtLibCtx->TxtInfoData->ResetAux));
  DEBUG ((DEBUG_INFO, "TxtLib: AcpiBase            = %x\n", TxtLibCtx->TxtInfoData->AcpiBase));
  DEBUG ((DEBUG_INFO, "TxtLib: SinitMemorySize     = %x\n", TxtLibCtx->TxtInfoData->SinitMemorySize));
  DEBUG ((DEBUG_INFO, "TxtLib: TxtHeapMemorySize   = %x\n", TxtLibCtx->TxtInfoData->TxtHeapMemorySize));
  DEBUG ((DEBUG_INFO, "TxtLib: TxtDprMemoryBase    = %x\n", TxtLibCtx->TxtInfoData->TxtDprMemoryBase));
  DEBUG ((DEBUG_INFO, "TxtLib: TxtDprMemorySize    = %x\n", TxtLibCtx->TxtInfoData->TxtDprMemorySize));
  DEBUG ((DEBUG_INFO, "TxtLib: BiosAcmBase         = %x\n", TxtLibCtx->TxtInfoData->BiosAcmBase));
  DEBUG ((DEBUG_INFO, "TxtLib: BiosAcmSize         = %x\n", TxtLibCtx->TxtInfoData->BiosAcmSize));
  DEBUG ((DEBUG_INFO, "TxtLib: SinitAcmBase        = %x\n", TxtLibCtx->TxtInfoData->SinitAcmBase));
  DEBUG ((DEBUG_INFO, "TxtLib: SinitAcmSize        = %x\n", TxtLibCtx->TxtInfoData->SinitAcmSize));
  DEBUG ((DEBUG_INFO, "TxtLib: TgaSize             = %x\n", TxtLibCtx->TxtInfoData->TgaSize));
  DEBUG ((DEBUG_INFO, "TxtLib: TxtLcpPdBase        = %x\n", TxtLibCtx->TxtInfoData->TxtLcpPdBase));
  DEBUG ((DEBUG_INFO, "TxtLib: TxtLcpPdSize        = %x\n", TxtLibCtx->TxtInfoData->TxtLcpPdSize));
  DEBUG ((DEBUG_INFO, "TxtLib: Flags               = %x\n", TxtLibCtx->TxtInfoData->Flags));
  DEBUG ((DEBUG_INFO, "TxtLib: CpuCount            = %x\n", TxtLibCtx->CpuCount));

  return EFI_SUCCESS;
}

/**
  This routine is a dummy return function required when waking APs from WFS.
**/

VOID
EFIAPI
DummyWakupFunc (
  IN OUT VOID  *Buffer
  )
{
  return;
}

/**
  Determines whether or not the platform has executed an TXT launch by
  examining the TPM Establishment bit.

  @param[in] TxtLibCtx      - A pointer to an initialized TXT DXE Context data structure

  @retval TRUE          - If the TPM establishment bit is asserted.
  @retval FALSE         - If the TPM establishment bit is unasserted.
**/
BOOLEAN
IsTxtEstablished (
  IN TXT_LIB_CONTEXT *TxtLibCtx
  )
{
  UINT8      AccessReg;
  UINT16     TimeOutCount;

  ///
  /// Set TPM.ACCESS polling timeout about 750ms
  ///
  TimeOutCount = TPM_TIME_OUT;
  do {
    ///
    /// Read TPM status register
    ///
    AccessReg = MmioRead8 ((UINT64) TPM_STATUS_REG_ADDRESS);
    ///
    /// if TPM.Access == 0xFF, TPM is not present
    ///
    if (AccessReg == 0xFF) {
      return FALSE;
    }
    ///
    /// Check tpmRegValidSts bit before checking establishment bit
    ///
    if ((AccessReg & 0x80) != 0x80) {
      ///
      /// Delay 1ms and read again
      ///
      MicroSecondDelay (1000);
    } else {
      ///
      /// tpmRegValidSts set, we can check establishment bit now.
      ///
      break;
    }

    TimeOutCount--;
  } while (TimeOutCount != 0);

  ///
  /// if tpmRegValidSts is not set, TPM is not usable
  ///
  if ((AccessReg & 0x80) != 0x80) {
    DEBUG ((DEBUG_ERROR, "TxtLib: TPM Valid Status is not set!! TPM.ACCESS=%x\n", AccessReg));
    ASSERT (FALSE);
    CpuDeadLoop ();
  }
  ///
  /// The bit we're interested in uses negative logic:
  /// If bit 0 == 1 then return False
  /// Else return True
  ///
  return (AccessReg & 0x1) ? FALSE : TRUE;
}

/**
  Determines whether or not POISON bit is set in status register

  @param[in] TxtLibCtx      - A pointer to an initialized TXT DXE Context data structure

  @retval TRUE          - If the TPM reset bit is asserted.
  @retval FALSE         - If the TPM reset bit is unasserted.
**/
BOOLEAN
IsTxtResetSet (
  IN TXT_LIB_CONTEXT *TxtLibCtx
  )
{
  UINT8      EstsReg;

  ///
  /// Read TPM status register
  ///
  EstsReg = MmioRead8 ((UINT64) TXT_PUBLIC_BASE + TXT_ERROR_STATUS_REG_OFF);

  return (EstsReg & 0x1) ? TRUE : FALSE;
}

/**
  Determines whether or not the platform requires initialization for TXT use.

  @param[in] TxtLibCtx      - A pointer to an initialized TXT DXE Context data structure

  @retval TRUE          - If the the platoform should be configured for TXT.
  @retval FALSE         - If TXT is not to be used.
**/
BOOLEAN
EFIAPI
IsTxtEnabled ()
{

  UINT64        Ia32FeatureControl;
  TXT_INFO_DATA *TxtInfoData;

  TxtInfoData = mTxtLibCtx.TxtInfoData;

  ///
  /// If TxtInfoHob reported TXT disabled, return FALSE to indicate TXT should not be used
  ///
  if (TxtInfoData->TxtMode == 0) {
    return FALSE;
  }

  Ia32FeatureControl = AsmReadMsr64 (MSR_IA32_FEATURE_CONTROL);
  DEBUG ((DEBUG_INFO, "TxtLib: MSR_IA32_FEATURE_CONTROL=%x\n", Ia32FeatureControl));

  return ((Ia32FeatureControl & TXT_OPT_IN_VMX_AND_SMX_MSR_VALUE) == TXT_OPT_IN_VMX_AND_SMX_MSR_VALUE) ? TRUE : FALSE;
}

/**
  Determines whether or not the current processor is TXT Capable.

  @retval TRUE          - If the current processor supports TXT
  @retval FALSE         - If the current processor does not support TXT
**/
BOOLEAN
IsTxtProcessor (
  VOID
  )
{
  CPUID_VERSION_INFO_ECX Ecx;

  AsmCpuid (CPUID_VERSION_INFO, NULL, NULL, &Ecx.Uint32, NULL);

  DEBUG ((DEBUG_INFO, "TxtLib: CPUID = %x\n", Ecx.Uint32));

  return (BOOLEAN) (Ecx.Bits.SMX == 1);
}

/**
  Add extened elements to BiosOsData

  @param[in] TxtLibCtx      - A pointer to an initialized TXT DXE Context data structure
  @param[in] Type        - The element's type
  @param[in] Buffer      - A pointer to buffer which need to append the element
  @param[in] Size        - return the size of the appened element.
**/
VOID
AppendElement (
  IN TXT_LIB_CONTEXT *TxtLibCtx,
  IN UINT32              Type,
  IN VOID                *Buffer,
  OUT UINT32             *Size
  )
{
  VOID   *Element;
  UINT32 NumberOfAcm;
  UINT64 *AcmBase;

  NumberOfAcm = 1;
  AcmBase     = NULL;
  Element     = NULL;
  *Size       = 0;

  switch (Type) {
    case HEAP_EXTDATA_TYPE_BIOS_SPEC_VER:
  ///
  /// Fill BIOS spec ver element
  ///
      Element = AllocatePool (sizeof (HEAP_BIOS_SPEC_VER_ELEMENT));

      if (Element != NULL) {
        *Size = sizeof (HEAP_BIOS_SPEC_VER_ELEMENT);
        ((HEAP_BIOS_SPEC_VER_ELEMENT *) Element)->Header.Type     = HEAP_EXTDATA_TYPE_BIOS_SPEC_VER;
        ((HEAP_BIOS_SPEC_VER_ELEMENT *) Element)->Header.Size     = sizeof (HEAP_BIOS_SPEC_VER_ELEMENT);
        ((HEAP_BIOS_SPEC_VER_ELEMENT *) Element)->SpecVerMajor    = TXT_BIOS_SPEC_VER_MAJOR;
        ((HEAP_BIOS_SPEC_VER_ELEMENT *) Element)->SpecVerMinor    = TXT_BIOS_SPEC_VER_MINOR;
        ((HEAP_BIOS_SPEC_VER_ELEMENT *) Element)->SpecVerRevision = TXT_BIOS_SPEC_VER_REVISION;
      }
      break;

    case HEAP_EXTDATA_TYPE_BIOSACM:
  ///
  /// Fill BIOS ACM element
  ///
      Element = AllocatePool (sizeof (HEAP_BIOSACM_ELEMENT) + NumberOfAcm * sizeof (UINT64));

      if (Element != NULL) {
        *Size = sizeof (HEAP_BIOSACM_ELEMENT) + sizeof (UINT64) * NumberOfAcm;
        ((HEAP_BIOSACM_ELEMENT *) Element)->Header.Type = HEAP_EXTDATA_TYPE_BIOSACM;
        ((HEAP_BIOSACM_ELEMENT *) Element)->Header.Size = sizeof (HEAP_BIOSACM_ELEMENT) + NumberOfAcm * sizeof (UINT64);
        ((HEAP_BIOSACM_ELEMENT *) Element)->NumAcms     = NumberOfAcm;
        AcmBase = (UINT64 *) ((UINTN) Element + sizeof (HEAP_BIOSACM_ELEMENT));
        *AcmBase = TxtLibCtx->TxtInfoData->BiosAcmBase;
      }
      break;

    case HEAP_EXTDATA_TYPE_END:
  ///
  /// Fill end type element
  ///
      Element = AllocatePool (sizeof (HEAP_EXT_DATA_ELEMENT));

      if (Element != NULL) {
        *Size = sizeof (HEAP_EXT_DATA_ELEMENT);
        ((HEAP_EXT_DATA_ELEMENT *) Element)->Type = HEAP_EXTDATA_TYPE_END;
        ((HEAP_EXT_DATA_ELEMENT *) Element)->Size = sizeof (HEAP_EXT_DATA_ELEMENT);
      }
      break;

    default:
      break;
  }

  if (Element != NULL) {
    CopyMem (Buffer, Element, *Size);
    FreePool (Element);
  }
}

/**
  Allocates 1 MB of 1MB-aligned memory for use as TXT Device Memory.  Records
  the location of TXT Device Memory in TXT Chipset registers and then adds
  programming instructions for these registers into BootScript.

  @param[in] TxtLibCtx      - A pointer to an initialized TXT DXE Context data structure

  @retval EFI_SUCCESS     - TXT Device memory has been successfully initialized.
  @exception EFI_UNSUPPORTED - TXT Device memory not available.
**/
EFI_STATUS
SetupTxtDeviceMemory (
  IN TXT_LIB_CONTEXT *TxtLibCtx
  )
{
  EFI_PHYSICAL_ADDRESS TopAddr;
  UINT64               *Ptr64;
  UINT64               Value64;
  UINT32               Value32;
  UINT64               TxtHeapMemoryBase;
  UINT64               TxtSinitMemoryBase;
  BOOLEAN              Locked;
  BIOS_OS_DATA_REGION  *BiosOsDataRegion;
  TXT_INFO_DATA        *TxtInfoData;
  UINT8                *Ptr8;

  TxtHeapMemoryBase   = 0;
  TxtSinitMemoryBase  = 0;
  Locked              = FALSE;
  Ptr8                = NULL;
  Value32             = 0;

  TxtInfoData         = TxtLibCtx->TxtInfoData;

  if ((TxtInfoData == 0) ||
      (TxtInfoData->TxtDprMemoryBase == 0) ||
      (TxtInfoData->TxtDprMemorySize == 0) ||
      (TxtInfoData->TxtHeapMemorySize == 0) ||
      (TxtInfoData->SinitMemorySize == 0)
      ) {
    return EFI_UNSUPPORTED;
  } else {
  ///
  /// Use address passed from TxtInfoHob
  ///
    TopAddr             = TxtInfoData->TxtDprMemoryBase + TxtInfoData->TxtDprMemorySize;

    TxtHeapMemoryBase   = (UINT64) (TopAddr - TxtInfoData->TxtHeapMemorySize);

    TxtSinitMemoryBase  = TxtHeapMemoryBase - TxtInfoData->SinitMemorySize;
  }
  ///
  /// Program TXT Device Memory Chipset Registers and record them in
  /// BootScript so they will be saved and restored on S3
  ///
  ASSERT ((TopAddr & 0x0FFFFF) == 0);

  ///
  /// DPR registers
  ///
  Ptr64   = (UINT64 *) (UINTN) (TXT_PUBLIC_BASE + TXT_DPR_SIZE_REG_OFF);
  Value64 = RShiftU64 (TxtInfoData->TxtDprMemorySize, 16) | 1;
  ///
  ///
  *Ptr64 = Value64 | TopAddr;
  ///
  /// Assert error if programmed value is different from requested. This
  /// means error is requested size.
  ///
  Value64 = *Ptr64;
  ASSERT ((LShiftU64 ((Value64 & 0xFFE), 16)) == TxtInfoData->TxtDprMemorySize);
  ///
  /// HEAP Registers
  /// Program size register first
  ///
  Ptr64 = (UINT64 *) (UINTN) (TXT_PUBLIC_BASE + TXT_HEAP_SIZE_REG_OFF);

  ///
  /// Test register locked status. If locked, skip programming since
  /// this will be done by BIOS ACM
  ///
  *Ptr64  = TEST_PATTERN;
  Value64 = *Ptr64;
  if (Value64 != TEST_PATTERN) {
    Locked = TRUE;
  } else {
  ///
  /// To be safe invert pattern and try again
  ///
    *Ptr64  = (UINT64) ~TEST_PATTERN;
    Value64 = *Ptr64;
    if (Value64 != (UINT64) ~TEST_PATTERN) {
      Locked = TRUE;
    }
  }

  if (!Locked) {

    *Ptr64 = TxtInfoData->TxtHeapMemorySize;
  ///
  /// Assert error if programmed value is different from requested. This
  /// means error is requested size.
  ///
    Value64 = *Ptr64;
    ASSERT (Value64 == TxtInfoData->TxtHeapMemorySize);
  ///
  /// Program base register.
  ///
    Ptr64   = (UINT64 *) (UINTN) (TXT_PUBLIC_BASE + TXT_HEAP_BASE_REG_OFF);
    *Ptr64  = TxtHeapMemoryBase;

  ///
  /// Assert error if programmed value is different from requested. This
  /// means error is requested size.
  ///
    Value64 = *Ptr64;
    ASSERT (Value64 == TxtHeapMemoryBase);
  }
  ///
  /// SINIT Registers
  /// Program size register first
  ///
  Ptr64   = (UINT64 *) (UINTN) (TXT_PUBLIC_BASE + TXT_SINIT_SIZE_REG_OFF);
  *Ptr64  = TxtInfoData->SinitMemorySize;
  ///
  /// Assert error if programmed value is different from requested. This
  /// means error is requested size.
  ///
  Value64 = *Ptr64;
  ASSERT (Value64 == TxtInfoData->SinitMemorySize);
  ///
  /// Program base register
  ///
  Ptr64   = (UINT64 *) (UINTN) (TXT_PUBLIC_BASE + TXT_SINIT_BASE_REG_OFF);
  *Ptr64  = TxtSinitMemoryBase;
  ///
  /// Assert error if programmed value is different from requested. This
  /// means error is requested size.
  ///
  Value64 = *Ptr64;
  ASSERT (Value64 == TxtSinitMemoryBase);
  ///
  /// Make sure TXT Device Memory has been zeroed
  ///
  ZeroMem (
    (VOID *) ((UINTN) TxtSinitMemoryBase),
    (UINTN) (TopAddr - TxtSinitMemoryBase)
    );

  if (TxtInfoData->TgaSize) {
    ;
  ///
  /// Placeholder for Trusted graphics support
  ///
  }

  Ptr64   = (UINT64 *) (UINTN) (TxtHeapMemoryBase);

  *Ptr64  = sizeof (BIOS_OS_DATA_REGION);
  ///
  /// BiosOsDataSize plus sizew of data size feld itself
  ///
  BiosOsDataRegion                          = (BIOS_OS_DATA_REGION *) (Ptr64 + 1);
  BiosOsDataRegion->Version                 = BIOS_OS_DATAREGION_VERSION;
  BiosOsDataRegion->BiosSinitSize           = 0;
  BiosOsDataRegion->LcpPdBase               = TxtInfoData->TxtLcpPdBase;
  BiosOsDataRegion->LcpPdSize               = TxtInfoData->TxtLcpPdSize;
  BiosOsDataRegion->NumOfLogicalProcessors  = (UINT32) (TxtLibCtx->CpuCount);
  BiosOsDataRegion->Flags = 0;
  if (BiosOsDataRegion->Version >= 6) { /// From CBNT, version = 6
    /// Note: BiosOsDataRegion->Flags is 64 bit : MLE flags (32:63) | SINIT flags (0:31)
    /// offset 28: SINTIFlags - currently none defined (0)
    /// offset 32: Version >=5 with updates in Version 6
    /// MLE flags Field:
    /// Bit [0] TXT/VT-x/VT-d ACPI PPI specification
    /// Bit [2:1]:
    /// 00: legacy state / platform undefined
    /// 01: client platform, client SINIT is required
    /// 10: server platform, server SINIT is required
    /// 11: Reserved/illegal, must be ignored
    /// MSR_BOOT_GUARD_SACM_INFO 0x13A [Bit 34 - LT_SX_FUSE] : 1 = server, 0 = client
    if ((AsmReadMsr64 (MSR_BOOT_GUARD_SACM_INFO) & BIT34) == 0) {
      BiosOsDataRegion->Flags = ((UINT64)BIT1 << 32);
    } else {
      BiosOsDataRegion->Flags = ((UINT64)BIT2 << 32);
    }
  }
  Ptr8 = (UINT8 *) (UINTN) &(BiosOsDataRegion->ExtData);
  AppendElement (TxtLibCtx, HEAP_EXTDATA_TYPE_BIOS_SPEC_VER, Ptr8, &Value32);
  Ptr8 += Value32;
  AppendElement (TxtLibCtx, HEAP_EXTDATA_TYPE_BIOSACM, Ptr8, &Value32);
  Ptr8 += Value32;
  AppendElement (TxtLibCtx, HEAP_EXTDATA_TYPE_END, Ptr8, &Value32);
  Value64 = (UINT64) (UINTN) Ptr8 - TxtHeapMemoryBase + Value32;
  *Ptr64  = Value64;

  return EFI_SUCCESS;
}

/**
  Invokes TxtLibLaunchBiosAcm to execute the SCHECK function.

  @param[in] TxtLibCtx      - A pointer to an initialized TXT DXE Context data structure

  @retval EFI_SUCCESS   - Always.
**/
EFI_STATUS
DoScheck (
  IN TXT_LIB_CONTEXT *TxtLibCtx
  )
{
  return TxtLibLaunchBiosAcm (TxtLibCtx, TXT_LAUNCH_SCHECK);
}

/**
  Invokes TxtLibLaunchBiosAcm to reset the TPM's establishment bit.

  @param[in] TxtLibCtx      - A pointer to an initialized TXT DXE Context data structure

  @retval EFI_SUCCESS   - Always.
**/
EFI_STATUS
ResetTpmEstBit (
  IN TXT_LIB_CONTEXT *TxtLibCtx
  )
{
  return TxtLibLaunchBiosAcm (TxtLibCtx, TXT_RESET_EST_BIT);
}

/**
  Sets up the system and then launches the TXT BIOS ACM to run the function
  requested by AcmFunction.

  @param[in] TxtLibCtx     - A pointer to an initialized TXT DXE Context data structure
  @param[in] AcmFunction   - Constant that represents the function from the BIOS ACM
                  that should be executed.

  @retval EFI_SUCCESS            - BIOS ACM is set up.
  @retval EFI_INVALID_PARAMETER  - Wrong data in TxtInfoHob.
  @retval EFI_NOT_FOUND          - BIOS ACM is not found
**/
EFI_STATUS
TxtLibLaunchBiosAcm (
  IN TXT_LIB_CONTEXT *TxtLibCtx,
  IN UINT64              AcmFunction
  )
{
  EFI_PHYSICAL_ADDRESS        AlignedAddr = 0;

  if (TxtLibCtx->TxtInfoData == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  if ((TxtLibCtx->TxtInfoData->BiosAcmBase == 0) || (TxtLibCtx->TxtInfoData->BiosAcmSize == 0)) {
  /*
   * If no information about placement of TXT BIOS ACM has been
   * passed from PEI - assert.
   */
    DEBUG ((DEBUG_ERROR, "TxtLib: BiosAcmBase and BiosAcmSize = 0 from HOB, can not load\n"));
    ASSERT (FALSE);
    return EFI_NOT_FOUND;
  } else {
  ///
  /// Use address passed from PEI
  ///
    AlignedAddr = TxtLibCtx->TxtInfoData->BiosAcmBase;
  }

  SendInitIpiAllExcludingSelf();

  /// Give the APs time to enter wait-for-SIPI state
  MicroSecondDelay (10 * STALL_ONE_MILLI_SECOND);

  ///
  /// Launch the BIOS ACM to run the requested function
  ///
  DEBUG ((DEBUG_INFO, "TxtLib::Running of LaunchBiosAcm\n"));
#if TXT_ARCH_IA32
  _asm {
    mov     ax,  0xAAAA
    out     0x80, ax
  }
#endif

  LaunchBiosAcm (AlignedAddr, AcmFunction);

  return EFI_SUCCESS;
}

/**
  Read policy protocol to reset AUX content

  @param[in] TxtLibCtx - A pointer to an initialized TXT DXE Context data structure

  @retval EFI_SUCCESS   - No error happend
  @retval EFI_NOT_FOUND - TxtPolicyProtocol is not found
**/
EFI_STATUS
ResetTpmAux (
  IN TXT_LIB_CONTEXT *TxtLibCtx
  )
{
  EFI_STATUS Status;

  Status = EFI_SUCCESS;

  if (TxtLibCtx->TxtInfoData->ResetAux == 1) {
    DEBUG ((DEBUG_INFO, "TxtLib: Reset AUX content\n"));
    Status = TxtLibLaunchBiosAcm (TxtLibCtx, TXT_RESET_AUX);
  }

  return Status;
}

/*
  Initializes Intel TXT
*/
EFI_STATUS
EFIAPI
InitTxt(
  )
{
  EFI_STATUS Status;

  ///
  /// Initialize the platform specific code
  ///
  Status = InitializeTxtLib (&mTxtLibCtx);
  ///
  /// If failure - assume TXT is not enabled
  ///
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "TxtLib::InitializeTxtLib failed.... Unloading\n"));
    return EFI_UNSUPPORTED;
  }
  ///
  /// If ESTS.TXTRESET bit is set, skip all other functions since
  /// attempt to execute GETSEC will hang system. Skipping allows to
  /// boot to OS and let MLE assess situation.
  ///
  if (IsTxtResetSet (&mTxtLibCtx)) {
    DEBUG ((DEBUG_ERROR, "TxtLib::TXT_RESET bit is set.... Unloading\n"));
    return EFI_UNSUPPORTED;
  }
  ///
  /// If TXT is enabled, configure platform appropriately.
  /// Code assumes that if TXT is enabled by CPU driver than all checks
  /// are passed, i.e. TPM is present, CPU and CS are TXT capable.
  /// Add to check CPU TXT capable in case CPU drivers do not check additional requirements
  ///
  if ((mTxtLibCtx.TxtInfoData->ChipsetIsTxtCapable) && (IsTxtProcessor ()) && (IsTxtEnabled (&mTxtLibCtx))) {
    DEBUG ((DEBUG_INFO, "TxtLib::TXT Enabled\n"));

  ///
  /// Allocate and Initialize TXT Device Memory
  ///
    Status = SetupTxtDeviceMemory (&mTxtLibCtx);
    if (EFI_ERROR (Status)) {
      DEBUG ((DEBUG_ERROR, "TxtLib::SetupTxtDeviceMemory failed.... Unloading\n"));
      return EFI_UNSUPPORTED;
    }

    DEBUG ((DEBUG_INFO, "TxtLib::Running of DoScheck\n"));
    DoScheck (&mTxtLibCtx);
  } else {
  ///
  /// TXT is not enabled, so make sure TPM Establishment
  /// bit is de-asserted
  ///
    DEBUG ((DEBUG_INFO, "TxtLib::TXT Disabled\n"));

    if (IsTxtEstablished (&mTxtLibCtx)) {
  ///
  /// We can invoke BIOS ACM function only if CS and CPU are TXT
  /// capable
  ///
      if ((mTxtLibCtx.TxtInfoData->ChipsetIsTxtCapable) &&
          (IsTxtProcessor ()) &&
          !(mTxtLibCtx.TxtInfoData->Flags & TPM_INIT_FAILED)
          ) {
        DEBUG ((DEBUG_INFO, "TxtLib::Resetting TPM Establishment bit\n"));
        ResetTpmEstBit (&mTxtLibCtx);
      }
    }
  ///
  /// Reset AUX
  ///
    Status = ResetTpmAux (&mTxtLibCtx);
    ASSERT_EFI_ERROR (Status);
  }

  return EFI_SUCCESS;
}
