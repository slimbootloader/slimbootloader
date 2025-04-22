/** @file

  Copyright (c) 2017 - 2020, Intel Corporation. All rights reserved.<BR>

  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <PiPei.h>
#include <Library/VtdPmrLib.h>
#include <Library/BaseLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/IoLib.h>
#include <Library/DebugLib.h>
#include <Library/BootloaderCoreLib.h>

/**
  Get protected low memory alignment.

  @param HostAddressWidth   The host address width.
  @param VtdUnitBaseAddress The base address of the VTd engine.

  @return protected low memory alignment.
**/
UINT32
STATIC
GetPlmrAlignment (
  IN UINT8         HostAddressWidth,
  IN UINTN         VtdUnitBaseAddress
  )
{
  UINT32        Data32;

  MmioWrite32 (VtdUnitBaseAddress + R_PMEN_LOW_BASE_REG, 0xFFFFFFFF);
  Data32 = MmioRead32 (VtdUnitBaseAddress + R_PMEN_LOW_BASE_REG);
  Data32 = ~Data32 + 1;

  return Data32;
}

/**
  Get protected high memory alignment.

  @param HostAddressWidth   The host address width.
  @param VtdUnitBaseAddress The base address of the VTd engine.

  @return protected high memory alignment.
**/
STATIC
UINT64
GetPhmrAlignment (
  IN UINT8         HostAddressWidth,
  IN UINTN         VtdUnitBaseAddress
  )
{
  UINT64        Data64;

  MmioWrite64 (VtdUnitBaseAddress + R_PMEN_HIGH_BASE_REG, 0xFFFFFFFFFFFFFFFF);
  Data64 = MmioRead64 (VtdUnitBaseAddress + R_PMEN_HIGH_BASE_REG);
  Data64 = ~Data64 + 1;
  Data64 = Data64 & (LShiftU64 (1, HostAddressWidth) - 1);

  return Data64;
}

/**
  Get protected low memory alignment.

  @param VTdInfo            The VTd engine context information.
  @param EngineMask         The mask of the VTd engine to be accessed.

  @return protected low memory alignment.
**/
UINT32
GetLowMemoryAlignment (
  IN VTD_INFO      *VTdInfo,
  IN UINT64        EngineMask
  )
{
  UINTN         Index;
  UINT32        Alignment;
  UINT32        FinalAlignment;

  FinalAlignment = 0;
  for (Index = 0; Index < VTdInfo->VTdEngineCount; Index++) {
    if ((EngineMask & LShiftU64(1, Index)) == 0) {
      continue;
    }
    Alignment = GetPlmrAlignment (VTdInfo->HostAddressWidth, (UINTN)VTdInfo->VTdEngineAddress[Index]);
    if (FinalAlignment < Alignment) {
      FinalAlignment = Alignment;
    }
  }
  return FinalAlignment;
}

/**
  Get protected high memory alignment.

  @param VTdInfo            The VTd engine context information.
  @param EngineMask         The mask of the VTd engine to be accessed.

  @return protected high memory alignment.
**/
UINT64
GetHighMemoryAlignment (
  IN VTD_INFO      *VTdInfo,
  IN UINT64        EngineMask
  )
{
  UINTN         Index;
  UINT64        Alignment;
  UINT64        FinalAlignment;

  FinalAlignment = 0;
  for (Index = 0; Index < VTdInfo->VTdEngineCount; Index++) {
    if ((EngineMask & LShiftU64(1, Index)) == 0) {
      continue;
    }
    Alignment = GetPhmrAlignment (VTdInfo->HostAddressWidth, (UINTN)VTdInfo->VTdEngineAddress[Index]);
    if (FinalAlignment < Alignment) {
      FinalAlignment = Alignment;
    }
  }
  return FinalAlignment;
}

/**
  Enable PMR in the VTd engine.

  @param VtdUnitBaseAddress The base address of the VTd engine.

  @retval EFI_SUCCESS      The PMR is enabled.
  @retval EFI_UNSUPPORTED  The PMR is not supported.
**/
STATIC
EFI_STATUS
EnablePmr (
  IN UINTN         VtdUnitBaseAddress
  )
{
  UINT32        Reg32;
  VTD_CAP_REG   CapReg;

  CapReg.Uint64 = MmioRead64 (VtdUnitBaseAddress + R_CAP_REG);
  if (CapReg.Bits.PLMR == 0 || CapReg.Bits.PHMR == 0) {
    return EFI_UNSUPPORTED;
  }

  Reg32 = MmioRead32 (VtdUnitBaseAddress + R_PMEN_ENABLE_REG);
  if (Reg32 == 0xFFFFFFFF) {
    DEBUG ((DEBUG_ERROR, "R_PMEN_ENABLE_REG - 0x%x\n", Reg32));
    ASSERT(FALSE);
  }

  if ((Reg32 & BIT0) == 0) {
    MmioWrite32 (VtdUnitBaseAddress + R_PMEN_ENABLE_REG, BIT31);
    do {
      Reg32 = MmioRead32 (VtdUnitBaseAddress + R_PMEN_ENABLE_REG);
    } while((Reg32 & BIT0) == 0);
  }

  return EFI_SUCCESS;
}

/**
  Disable PMR in the VTd engine.

  @param VtdUnitBaseAddress The base address of the VTd engine.

  @retval EFI_SUCCESS      The PMR is disabled.
  @retval EFI_UNSUPPORTED  The PMR is not supported.
**/
STATIC
EFI_STATUS
DisablePmr (
  IN UINTN         VtdUnitBaseAddress
  )
{
  UINT32        Reg32;
  VTD_CAP_REG   CapReg;

  CapReg.Uint64 = MmioRead64 (VtdUnitBaseAddress + R_CAP_REG);
  if (CapReg.Bits.PLMR == 0 || CapReg.Bits.PHMR == 0) {
    return EFI_UNSUPPORTED;
  }

  Reg32 = MmioRead32 (VtdUnitBaseAddress + R_PMEN_ENABLE_REG);
  if (Reg32 == 0xFFFFFFFF) {
    DEBUG ((DEBUG_ERROR, "R_PMEN_ENABLE_REG - 0x%x\n", Reg32));
    ASSERT(FALSE);
  }

  if ((Reg32 & BIT0) != 0) {
    MmioWrite32 (VtdUnitBaseAddress + R_PMEN_ENABLE_REG, 0x0);
    do {
      Reg32 = MmioRead32 (VtdUnitBaseAddress + R_PMEN_ENABLE_REG);
    } while((Reg32 & BIT0) != 0);
  }

  return EFI_SUCCESS;
}

/**
  Set PMR region in the VTd engine.

  @param HostAddressWidth   The host address width.
  @param VtdUnitBaseAddress The base address of the VTd engine.
  @param LowMemoryBase      The protected low memory region base.
  @param LowMemoryLength    The protected low memory region length.
  @param HighMemoryBase     The protected high memory region base.
  @param HighMemoryLength   The protected high memory region length.

  @retval EFI_SUCCESS      The PMR is set to protected region.
  @retval EFI_UNSUPPORTED  The PMR is not supported.
**/
STATIC
EFI_STATUS
SetPmrRegion (
  IN UINT8         HostAddressWidth,
  IN UINTN         VtdUnitBaseAddress,
  IN UINT32        LowMemoryBase,
  IN UINT32        LowMemoryLength,
  IN UINT64        HighMemoryBase,
  IN UINT64        HighMemoryLength
  )
{
  VTD_CAP_REG   CapReg;
  UINT32        PlmrAlignment;
  UINT64        PhmrAlignment;

  DEBUG ((DEBUG_INFO, "VtdUnitBaseAddress - 0x%x\n", VtdUnitBaseAddress));

  CapReg.Uint64 = MmioRead64 (VtdUnitBaseAddress + R_CAP_REG);
  if (CapReg.Bits.PLMR == 0 || CapReg.Bits.PHMR == 0) {
    DEBUG ((DEBUG_ERROR, "PLMR/PHMR unsupported\n"));
    return EFI_UNSUPPORTED;
  }

  PlmrAlignment = GetPlmrAlignment (HostAddressWidth, VtdUnitBaseAddress);
  PhmrAlignment = GetPhmrAlignment (HostAddressWidth, VtdUnitBaseAddress);
  if ((LowMemoryBase    != ALIGN_VALUE(LowMemoryBase, PlmrAlignment)) ||
      (LowMemoryLength  != ALIGN_VALUE(LowMemoryLength, PlmrAlignment)) ||
      (HighMemoryBase   != ALIGN_VALUE(HighMemoryBase, PhmrAlignment)) ||
      (HighMemoryLength != ALIGN_VALUE(HighMemoryLength, PhmrAlignment))) {
    DEBUG ((DEBUG_ERROR, "PLMR/PHMR alignment issue, require 0x%08X\n", PlmrAlignment));
    return EFI_UNSUPPORTED;
  }

  if ((PcdGet32 (PcdDmaBufferAlignment) < PlmrAlignment) ||
      (PcdGet32 (PcdDmaBufferAlignment) < PhmrAlignment)) {
    DEBUG ((DEBUG_ERROR, "PcdDmaBufferAlignment does not match VT-d PMR\n"));
    return EFI_UNSUPPORTED;
  }

  if (LowMemoryBase == 0 && LowMemoryLength == 0) {
    LowMemoryBase = 0xFFFFFFFF;
  }
  if (HighMemoryBase == 0 && HighMemoryLength == 0) {
    HighMemoryBase = 0xFFFFFFFFFFFFFFFF;
  }

  MmioWrite32 (VtdUnitBaseAddress + R_PMEN_LOW_BASE_REG,    LowMemoryBase);
  MmioWrite32 (VtdUnitBaseAddress + R_PMEN_LOW_LIMITE_REG,  LowMemoryBase + LowMemoryLength - 1);
  MmioWrite64 (VtdUnitBaseAddress + R_PMEN_HIGH_BASE_REG,   HighMemoryBase);
  MmioWrite64 (VtdUnitBaseAddress + R_PMEN_HIGH_LIMITE_REG, HighMemoryBase + HighMemoryLength - 1);

  return EFI_SUCCESS;
}

/**
  Set DMA protected region.

  @param VTdInfo            The VTd engine context information.
  @param EngineMask         The mask of the VTd engine to be accessed.
  @param LowMemoryBase      The protected low memory region base.
  @param LowMemoryLength    The protected low memory region length.
  @param HighMemoryBase     The protected high memory region base.
  @param HighMemoryLength   The protected high memory region length.

  @retval EFI_SUCCESS      The DMA protection is set.
  @retval EFI_UNSUPPORTED  The DMA protection is not set.
**/
STATIC
EFI_STATUS
SetDmaProtectedRange (
  IN VTD_INFO      *VTdInfo,
  IN UINT64        EngineMask,
  IN UINT32        LowMemoryBase,
  IN UINT32        LowMemoryLength,
  IN UINT64        HighMemoryBase,
  IN UINT64        HighMemoryLength
  )
{
  UINTN       Index;
  EFI_STATUS  Status;

  if ((VTdInfo == NULL) || (VTdInfo->VTdEngineCount == 0)) {
    return EFI_UNSUPPORTED;
  }

  DEBUG ((DEBUG_INFO, "SetDmaProtectedRange:\n  [0x%x, 0x%x]\n  [0x%lx, 0x%lx]\n",
          LowMemoryBase, LowMemoryBase + LowMemoryLength,
          HighMemoryBase, HighMemoryBase + HighMemoryLength));

  for (Index = 0; Index < VTdInfo->VTdEngineCount; Index++) {
    if ((EngineMask & LShiftU64(1, Index)) == 0) {
      continue;
    }
    DisablePmr ((UINTN)VTdInfo->VTdEngineAddress[Index]);
    Status = SetPmrRegion (
               VTdInfo->HostAddressWidth,
               (UINTN)VTdInfo->VTdEngineAddress[Index],
               LowMemoryBase,
               LowMemoryLength,
               HighMemoryBase,
               HighMemoryLength
               );
    if (EFI_ERROR(Status)) {
      return Status;
    }
    Status = EnablePmr ((UINTN)VTdInfo->VTdEngineAddress[Index]);
    if (EFI_ERROR(Status)) {
      return Status;
    }
  }

  return EFI_SUCCESS;
}

/**
  Diable DMA protection.

  @param VTdInfo            The VTd engine context information.
  @param EngineMask         The mask of the VTd engine to be accessed.

  @retval EFI_SUCCESS DMA protection is disabled.
**/
STATIC
EFI_STATUS
DisableDmaProtection (
  IN VTD_INFO      *VTdInfo,
  IN UINT64        EngineMask
  )
{
  UINTN       Index;
  EFI_STATUS  Status;

  DEBUG ((DEBUG_INFO, "DisableDmaProtection\n"));

  for (Index = 0; Index < VTdInfo->VTdEngineCount; Index++) {
    if ((EngineMask & LShiftU64(1, Index)) == 0) {
      continue;
    }
    Status = DisablePmr ((UINTN)VTdInfo->VTdEngineAddress[Index]);
    if (EFI_ERROR(Status)) {
      return Status;
    }
  }

  return EFI_SUCCESS;
}

/**
  Return if the PMR is enabled.

  @param VtdUnitBaseAddress The base address of the VTd engine.

  @retval TRUE  PMR is enabled.
  @retval FALSE PMR is disabled or unsupported.
**/
STATIC
BOOLEAN
IsPmrEnabled (
  IN UINTN         VtdUnitBaseAddress
  )
{
  UINT32        Reg32;
  VTD_CAP_REG   CapReg;

  CapReg.Uint64 = MmioRead64 (VtdUnitBaseAddress + R_CAP_REG);
  if (CapReg.Bits.PLMR == 0 || CapReg.Bits.PHMR == 0) {
    return FALSE;
  }

  Reg32 = MmioRead32 (VtdUnitBaseAddress + R_PMEN_ENABLE_REG);
  if ((Reg32 & BIT0) == 0) {
    return FALSE;
  }

  return TRUE;
}

/**
  Return the mask of the VTd engine which is enabled.

  @param VTdInfo            The VTd engine context information.
  @param EngineMask         The mask of the VTd engine to be accessed.

  @return the mask of the VTd engine which is enabled.
**/
UINT64
GetDmaProtectionEnabledEngineMask (
  IN VTD_INFO      *VTdInfo,
  IN UINT64        EngineMask
  )
{
  UINTN       Index;
  BOOLEAN     Result;
  UINT64      EnabledEngineMask;

  DEBUG ((DEBUG_INFO, "GetDmaProtectionEnabledEngineMask - 0x%lx\n", EngineMask));

  EnabledEngineMask = 0;
  for (Index = 0; Index < VTdInfo->VTdEngineCount; Index++) {
    if ((EngineMask & LShiftU64(1, Index)) == 0) {
      continue;
    }
    Result = IsPmrEnabled ((UINTN)VTdInfo->VTdEngineAddress[Index]);
    if (Result) {
      EnabledEngineMask |= LShiftU64(1, Index);
    }
  }

  DEBUG ((DEBUG_INFO, "EnabledEngineMask - 0x%lx\n", EnabledEngineMask));
  return EnabledEngineMask;
}


/**
  Enable or disable DMA protection using VTD PMR.

  This function will enable/disable DMA protection.

  @param[in] VtdInfo            VT-d info structure pointer.
  @param[in] Enable             Enable/Disable DMA protection.

  @retval     EFI_UNSUPPORTED   DMA protection is not supported.
  @retval     EFI_SUCCESS       DMA protection is enabled or disabled successfully.

**/
EFI_STATUS
EFIAPI
SetDmaProtection (
  IN  VTD_INFO     *VtdInfo,
  IN  BOOLEAN       Enable
)
{
  UINT32        MemTop;
  UINT32        DmaStart;
  UINT32        DmaEnd;
  EFI_STATUS    Status;

  if (Enable) {
    MemTop   = (UINT32) GetMemoryInfo (EnumMemInfoTodplm);
    MemTop   = ALIGN_UP (MemTop, PcdGet32 (PcdDmaBufferAlignment));
    DmaStart = (UINT32)(UINTN)GetDmaBufferPtr ();
    DmaEnd   = DmaStart + ALIGN_UP (PcdGet32 (PcdDmaBufferSize), PcdGet32 (PcdDmaBufferAlignment));
    Status = SetDmaProtectedRange (VtdInfo, VTD_ENGINE_ALL, 0, DmaStart, DmaEnd, MemTop - DmaEnd);
  } else {
    Status = DisableDmaProtection (VtdInfo, VTD_ENGINE_ALL);
  }

  DEBUG ((DEBUG_INFO, "DMA protection %aabling: %r\n", Enable ? "en" : "dis", Status));
  return Status;
}
