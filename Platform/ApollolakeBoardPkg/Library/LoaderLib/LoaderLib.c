/** @file

  Copyright (c) 2015 - 2017, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

  Description:

  This sample code shows how to load the IBB with the asistance from the CSE.
  It has the following assumptions:
  - FSP TempRamInit() should have been called and the whole temporary memory range
    is from 0xFEF00000 to 0xFEF80000 as described in the FSP integration guide.
  - The full IBB size defined by IBB_CODE_SIZE(0x58000) includes both loader IBB
    and FSP-M component.
  - This IbbLoader code is loaded and executed from the temporary memory below
    IBB_CODE_BASE(0xFEF28000).
  - The PCIe MMIO configuration base address is set to PCIE_MMCFG_BASE(0xE0000000)

**/

#include <PiPei.h>
#include <Library/BaseLib.h>
#include <Library/ExtraBaseLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/BootloaderCoreLib.h>
#include <Library/DebugLib.h>
#include <Library/BpdtLib.h>
#include <Library/BootMediaLib.h>
#include <Library/PcdLib.h>
#include <Library/IoLib.h>
#include <Library/DecompressLib.h>
#include <CpuRegs.h>
#include <PlatformData.h>

#define  SRAM_SIZE              0x20000
#define  SRAM_REMAPPING         (0xFFFFFFFF - SRAM_SIZE + 1)
#define  HOST2CSE               0x70
#define  CSE2HOST               0x60
#define  PCIE_MMCFG_BASE        0xE0000000
#define  CSE_CFG_BASE           (PCIE_MMCFG_BASE + 0x78000)
#define  CSE_VIDDID             (*(volatile UINT32 *)(UINTN)CSE_CFG_BASE)
#define  CSE_HOST2CSE           (*(volatile UINT32 *)(UINTN)(CSE_CFG_BASE + HOST2CSE))
#define  CSE_CSE2HOST           (*(volatile UINT32 *)(UINTN)(CSE_CFG_BASE + CSE2HOST))

/**
  Load Stage1B raw image to destination address.

  The destination should be temporary memory. It utilizes CSME to
  load Stage1B (IBBM) from boot media to temporary memory.

  @param[in]  IbbDst    The destination address for image loading.
  @param[in]  IbbSrc    Not used. CSME knows where to load image from.
  @param[in]  IbbSize   The size for destination buffer.

  @retval EFI_DEVICE_ERROR
  @retval EFI_SUCCESS  Always return success.

**/
EFI_STATUS
LoadStage1B (
  IN UINT32   IbbDst,
  IN UINT32   IbbSrc,
  IN UINT32   IbbSize
  )
{
  UINT32     ChunkNum;
  UINT32     State;
  UINT32     Size;
  UINT8      RingIdx;
  UINT32     SramSize;
  UINT32     SramBase;
  UINT32     HostToCse;
  UINT32     CseToHost;
  UINT32     ChunkIndex;
  UINT32     ChunkSize;
  UINT32     NumberOfChunks;
  UINT32     IbbSizeLeft;
  UINT8     *Src;
  UINT8     *Dst;
  UINT8     *Ptr;
  PLATFORM_DATA   *PlatformData;

  if (CSE_VIDDID == 0xFFFFFFFF) {
    /* CSE does not exist */
    return EFI_DEVICE_ERROR;
  }

  /* Prepare paramters */
  SramSize      = SRAM_SIZE;
  SramBase      = SRAM_REMAPPING;

  /* Request data from CSE */
  HostToCse    = (IbbSize << 4) | 0xC0000000;
  ChunkIndex   = 0;
  CSE_HOST2CSE = HostToCse;

  /* Wait for response */
  do {
    CseToHost  = CSE_CSE2HOST;
    ChunkNum   = (CseToHost >> 28) & 3;
  } while (ChunkNum == 0);

  if (ChunkNum == 3) {
    ChunkNum = 4;
  }

  /* Retrieve related information */
  NumberOfChunks = ChunkNum;
  IbbSizeLeft    = (CseToHost & 0x0FFFC000) >> 4;
  IbbSize        = IbbSizeLeft;
  ChunkSize      = SramSize / NumberOfChunks;

  /* Loading IBBM */
  while (IbbSizeLeft > 0) {

    /* Check whether there is ring buffer ready to copy */
    State = (CseToHost ^ HostToCse) & 0x0F;
    while (State == 0) {
      CseToHost  = CSE_CSE2HOST;
      State = (CseToHost ^ HostToCse) & 0x0F;
    }

    /* Calculate ring index */
    RingIdx = (UINT8) (ChunkIndex % NumberOfChunks);
    if ((State & (1 << RingIdx)) != 0) {
      /* Calculate the source and destionation address in ring buffer */
      Src = (UINT8 *)(UINTN)(SramBase + ChunkSize * RingIdx);
      Dst = (UINT8 *)(UINTN)(IbbDst  + ChunkSize * ChunkIndex);

      if (IbbSizeLeft < ChunkSize) {
        Size = IbbSizeLeft;
      } else {
        Size = ChunkSize;
      }

      /* Move data from SRAM into temporary memory */
      if ((UINT32)(UINTN)Src >= SRAM_REMAPPING) {
        AsmFlushCacheRange (Src, Size);
      }
      CopyMem (Dst, Src, Size);
      IbbSizeLeft   -= Size;

      /* Send ACK to CSE */
      Ptr  = (UINT8 *)&HostToCse;
      *Ptr = ((*Ptr) & ~ (1 << RingIdx)) | (CseToHost & (1 << RingIdx));
      CSE_HOST2CSE = HostToCse;

      ChunkIndex++;
    }
  }

  do {
    CseToHost  = CSE_CSE2HOST;
  } while ((CseToHost & 0x00000400) == 0);

  if ((CseToHost & 0x00000200) != 0) {

    PlatformData = (PLATFORM_DATA *)GetPlatformDataPtr ();
    if (PlatformData != NULL) {
      if (PlatformData->BtGuardInfo.Bpm.Vb) {
        DEBUG ((DEBUG_INFO, "BtGuard IBBM verification succeeded !\n"));
      } else {
        DEBUG ((DEBUG_INFO, "No BtGuard verification !\n"));
      }
    }
  } else {
    CpuHalt ("BtGuard verification failed !!\n");
  }

  /* Clear MCA in BANK4 caused by the Clflush */
  AsmWriteMsr64 (IA32_MC4_STATUS, 0);

  return EFI_SUCCESS;
}

/**
 Initializes boot device and loads stage2 firmware to
 destination address provided.

 @param [in] Dst Destination address to load the Firmware.
 @param [in] Src Source address to get firmware.
 @param [in] Size size of the firmware to load.
 @retval     EFI_SUCCESS if load is success.
 **/
EFI_STATUS
EFIAPI
LoadStage2 (
  UINT32                         Dst,
  UINT32                         Src,
  UINT32                         Size
  )
{
  UINT32                         ImageSize;
  LOADER_COMPRESSED_HEADER       *Hdr;

  if (PcdGetBool (PcdLoadImageUseFsp)) {
    return EFI_SUCCESS;
  }

  ImageSize = Size;
  Hdr = (LOADER_COMPRESSED_HEADER *)(UINTN)Src;
  if ((Hdr == NULL) || (ImageSize == 0)) {
    return EFI_NOT_FOUND;
  }

  if (IS_COMPRESSED (Hdr)) {
    ImageSize = sizeof(LOADER_COMPRESSED_HEADER) + Hdr->CompressedSize;
  }

  if (Src != Dst) {
    CopyMem ((VOID *)(UINTN)Dst, (VOID *)(UINTN)Src, ImageSize);
  }

  return EFI_SUCCESS;
}

/**
  Based on the boot mode loads the payload image
  from flash or from the source address provided
  and loads on to the destination address.

  @param [in] Dst Destination address to load the payload.
  @param [in] Src Source address to get the payload.
  @param [in] Size size of the payload.
  @retval   EFI_SUCCESS           Load the firmware successfully.
  @retval   EFI_BUFFER_TOO_SMALL  Dstination buffer is too small.
  @retval   Others                Failed to load firmware.
 **/
EFI_STATUS
EFIAPI
LoadPayload (
  UINT32                         Dst,
  UINT32                         Src,
  UINT32                         Size
  )
{
  UINT32                          ActualSize;
  LOADER_COMPRESSED_HEADER       *Hdr;

  Hdr = (LOADER_COMPRESSED_HEADER *)(UINTN)Src;
  if (IS_COMPRESSED (Hdr)) {
    ActualSize = sizeof(LOADER_COMPRESSED_HEADER) + Hdr->CompressedSize;
  } else {
    ActualSize = Size;
  }

  if (Dst != Src) {
    if (Size < ActualSize) {
      return EFI_BUFFER_TOO_SMALL;
    }
    CopyMem ((VOID *)(UINTN)Dst, (VOID *)(UINTN)Src, ActualSize);
  }

  return EFI_SUCCESS;
}
