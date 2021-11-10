/** @file

  Copyright (c) 2017 - 2020, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include "OsLoader.h"

/**
  Load a file into memory from path.

  @param[in]      Path     File path to load.
  @param[in]      Buffer   Buffer pointer to store the loaded file.
  @param[in]      Length   Buffer pointer to store the loaded file size.

  @retval  EFI_SUCCESS            File was loaded successfully.
  @retval  EFI_UNSUPPORTED        Unsupported file path.
  @retval  EFI_INVALID_PARAMETER  File path is NULL.
**/
EFI_STATUS
EFIAPI
LoadFile (
  IN     CHAR8    *Path,
  IN OUT VOID    **Buffer,
  IN OUT UINT32   *Length
  )
{
  EFI_STATUS  Status;

  if (Path == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  Status = EFI_UNSUPPORTED;

  // Handle special path  !XXXX/YYYY
  // Load file from a container
  if ((AsciiStrLen(Path) == 10) && (Path[0] == '!') && (Path[5] == '/')) {
    Status = LoadComponent (SIGNATURE_32 (Path[1], Path[2], Path[3], Path[4]),
                            SIGNATURE_32 (Path[6], Path[7], Path[8], Path[9]),
                            (VOID **)Buffer, Length);
  }

  return Status;
}

/**
  Save SETUP config data back into flash.

  @param[in]      Buffer    CFGDATA buffer pointer.
  @param[in]      DataLen   CFGDATA buffer length.

  @retval  EFI_SUCCESS            Config data was saved successfully.
  @retval  EFI_BUFFER_TOO_SMALL   Flash space allocated for CFGDATA is too small.
  @retval  EFI_NOT_FOUND          Could not find CFGB component.
  @retval  EFI_INVALID_PARAMETER  Buffer is NULL.
**/
EFI_STATUS
EFIAPI
SaveConfigData (
  UINT8   *Buffer,
  UINT32   DataLen
)
{
  EFI_STATUS                      Status;
  UINT8                          *CfgAddr;
  UINT32                          CfgLen;
  UINT32                          RgnBase;
  UINT32                          RgnSize;
  UINT32                          Offset;
  UINT32                          DataOff;
  LOADER_COMPRESSED_HEADER        LzHeader;
  CDATA_BLOB                     *CdataBlob;
  SPI_FLASH_SERVICE              *SpiService;

  if (Buffer == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  CdataBlob = (CDATA_BLOB *)Buffer;
  if (CdataBlob->Signature != CFG_DATA_SIGNATURE) {
    return EFI_UNSUPPORTED;
  }

  if (CdataBlob->HeaderLength != sizeof(CDATA_BLOB)) {
    return EFI_UNSUPPORTED;
  }

  if (CdataBlob->UsedLength != DataLen) {
    return EFI_UNSUPPORTED;
  }

  SpiService = (SPI_FLASH_SERVICE *)GetServiceBySignature (SPI_FLASH_SERVICE_SIGNATURE);
  if (SpiService == NULL) {
    return EFI_UNSUPPORTED;
  }

  CfgAddr = NULL;
  CfgLen  = 0;
  Status = LocateComponent (CONTAINER_SETUP_SIGNATURE,  CFG_DATA_SIGNATURE, (VOID **)&CfgAddr, &CfgLen);
  if (EFI_ERROR (Status)) {
    return EFI_NOT_FOUND;
  }

  if (DataLen > CfgLen) {
    DEBUG ((DEBUG_INFO, "Insufficant space for SETP/CFGD !\n"));
    return EFI_BUFFER_TOO_SMALL;
  }

  // Update check sum
  CdataBlob->ExtraInfo.InternalDataOffset = 0;
  CdataBlob->ExtraInfo.InternalDataOffset = CalculateCheckSum16 ((UINT16 *)CdataBlob, CdataBlob->UsedLength);

  // Do not update signature yet
  CdataBlob->Signature = 0xFFFFFFFF;
  Status = SpiService->SpiInit ();
  if (!EFI_ERROR (Status)) {
    Status = SpiService->SpiGetRegion (FlashRegionBios, &RgnBase, &RgnSize);
    if (!EFI_ERROR (Status)) {
      DataOff = sizeof(LzHeader);
      CopyMem (&LzHeader, CfgAddr, DataOff);
      LzHeader.CompressedSize = DataLen;
      LzHeader.Size = DataLen;
      Offset = (UINT32)(UINTN)CfgAddr + RgnSize;
      Status = SpiService->SpiErase (FlashRegionBios, Offset, ALIGN_UP(DataLen, SIZE_4KB));
      if (!EFI_ERROR (Status)) {
        Status = SpiService->SpiWrite (FlashRegionBios, Offset, DataOff, (UINT8 *)&LzHeader);
      }
      if (!EFI_ERROR (Status)) {
        Status = SpiService->SpiWrite (FlashRegionBios, Offset + DataOff, DataLen, Buffer);
      }
      if (!EFI_ERROR (Status)) {
        CdataBlob->Signature = CFG_DATA_SIGNATURE;
        Status = SpiService->SpiWrite (FlashRegionBios, Offset + DataOff, 4, (UINT8 *)&CdataBlob->Signature);
      }
      if (EFI_ERROR (Status)) {
        DEBUG ((DEBUG_INFO, "Write flash failed, please check if FLASH is locked - %r !\n", Status));
      }
    }
  }

  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_INFO, "Failed to save SETP/CFGD - %r !\n", Status));
  } else {
    DEBUG ((DEBUG_INFO, "SETP/CFGD was saved successfully !\n"));
  }

  return Status;
}

/**
  Reset the platform.

  This function does not return.

  @param[in]      Type      Requested console row.

**/
VOID
EFIAPI
PlatformReset (
  UINT32    Type
)
{
  PLATFORM_SERVICE   *PlatformService;

  // Reset platform
  PlatformService = (PLATFORM_SERVICE *) GetServiceBySignature (PLATFORM_SERVICE_SIGNATURE);
  if ((PlatformService != NULL) && (PlatformService->ResetSystem != NULL)) {
    DEBUG ((DEBUG_INFO, "Reboot system\n"));
    PlatformService->ResetSystem (EfiResetCold);
  }
  CpuHalt (NULL);
}

/**
  Intialize console.

  @param[in]      Row      Requested console row.
  @param[in]      Col      Requested console col.

  @retval  EFI_SUCCESS     Console was initialized successfully.
  @retval  Others          Console initialization failed.
**/
EFI_STATUS
EFIAPI
ConsoleInit (
  IN UINT32  Row,
  IN UINT32  Col
)
{
  return LocalConsoleInit (TRUE);
}

// Provide paylaod module API service function table
VOID  *mSerivceMapTbl[] = {
  "DebugPrint",         (VOID *)((UINTN)&DebugPrint),
  "ConsoleInit",        (VOID *)((UINTN)&ConsoleInit),
  "ConsolePoll",        (VOID *)((UINTN)&ConsolePoll),
  "ConsoleRead",        (VOID *)((UINTN)&ConsoleRead),
  "ConsoleWrite",       (VOID *)((UINTN)&ConsoleWrite),
  "MicroSecondDelay",   (VOID *)((UINTN)&MicroSecondDelay),
  "AllocatePages",      (VOID *)((UINTN)&AllocatePages),
  "FreePages",          (VOID *)((UINTN)&FreePages),
  "SerialPortWrite",    (VOID *)((UINTN)&SerialPortWrite),
  "LoadFile",           (VOID *)((UINTN)&LoadFile),
  "PlatformReset",      (VOID *)((UINTN)&PlatformReset),
  "DrawFrameBuffer",    NULL,
  "BuildConfigData",    NULL,
  "SaveConfigData",     NULL,
};

/**
  Get a function pointer from the function name.

  @param[in]      FuncName    Function name

  @retval   Function pointer for the given funciton name.
  @retval   NULL   Failed to get the function address.

**/
VOID *
EFIAPI
GetProcAddress (
  IN CHAR8     *FuncName
  )
{
  UINT32   Index;

  if (FuncName != NULL) {
    for (Index = 0; Index < sizeof(mSerivceMapTbl) / sizeof(mSerivceMapTbl[0]); Index += 2) {
      if ((AsciiStrCmp (FuncName, mSerivceMapTbl[Index]) == 0) && \
        (mSerivceMapTbl[Index + 1] != NULL)) {
        return (VOID *)mSerivceMapTbl[Index + 1];
      }
    }
    DEBUG ((DEBUG_ERROR, "Service '%a' not found !\n", FuncName));
  }

  return NULL;
}

/**
  Update a function pointer iin the API service table.

  @param[in]      FuncName       Function name
  @param[in]      FuncPointer    Function pointer

  @retval  EFI_SUCCESS           API has been updated successfully.
  @retval  EFI_NOT_FOUND         Not found the entry to update.

**/
EFI_STATUS
EFIAPI
UpdateProcAddress (
  IN CHAR8     *FuncName,
  IN VOID      *FuncPointer
  )
{
  UINT32   Index;

  for (Index = 0; Index < sizeof(mSerivceMapTbl) / sizeof(mSerivceMapTbl[0]); Index += 2) {
    if (AsciiStrCmp (FuncName, mSerivceMapTbl[Index]) == 0) {
      mSerivceMapTbl[Index + 1] = FuncPointer;
      return EFI_SUCCESS;
    }
  }

  return EFI_NOT_FOUND;
}

/**
  Initialize payload module serivce table and paramters.

  @param[in]      PldModParam   Payload module parameter pointer
  @param[in]      ModuleName    Payload module name

**/
VOID
EFIAPI
PayloadModuleInit (
  IN  PLD_MOD_PARAM       *PldModParam,
  IN  CHAR8               *ModuleName
  )
{
  CHAR8  *ModCmdLineBuf;

  if (PldModParam == NULL) {
    return;
  }

  if (ModuleName != NULL) {
    if (AsciiStrnCmp ((CONST CHAR8 *)ModuleName, "!SETP/MPYM", 10) == 0) {
      // Setup, enable additional services
      UpdateProcAddress ("BuildConfigData",    (VOID *)((UINTN)&BuildConfigData));
      UpdateProcAddress ("SaveConfigData",     (VOID *)((UINTN)&SaveConfigData));
      UpdateProcAddress ("DrawFrameBuffer",    (VOID *)((UINTN)&DrawFrameBuffer));
    }

    ModCmdLineBuf = PldModParam->CmdLineBuf;
    if (ModCmdLineBuf == NULL) {
      return;
    }

    if (PldModParam->CmdLineLen >= 11) {
      if ((ModuleName[0] == '!') && ((ModuleName[5] == '/')) && (ModuleName[10] == ':')) {
        // !SETP/MPYM:????, here ???? is the script component name
        // So build a path like !SETP/???? for command line.
        CopyMem (ModCmdLineBuf, ModuleName, 6);
        CopyMem (ModCmdLineBuf + 6, ModuleName + 11, 4);
        ModCmdLineBuf[10] = 0;
      } else {
        ModCmdLineBuf[0] = 0;
      }
    }
  }

}
