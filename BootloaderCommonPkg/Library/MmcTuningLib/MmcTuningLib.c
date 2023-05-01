/** @file
  This file provides some helper functions which are specific for EMMC device.

  Copyright (c) 2015 - 2023, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <Library/BaseLib.h>
#include <Library/MmcTuningLib.h>
#include <Library/MmcAccessLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/BootloaderCommonLib.h>
#include <Library/HobLib.h>
#include <Library/IoLib.h>
#include <Library/TimerLib.h>
#include <Library/VariableLib.h>
#include <Library/DebugPrintErrorLevelLib.h>
#include "MmcTuningLibPrivate.h"

CONST UINT32 EmmcWorstCasePattern[] = {
  0x60FEFE00, 0xEDFF10FE, 0x60FEFE00, 0xEDFF10FE, 0xDE08216A, 0xE25F20DF, 0xDE08216A, 0xE25F20DF, // spt_a0_sandisk_50ohm
  0xFF00CFFF, 0x1014B5CF, 0xFF00CFFF, 0x1014B5CF, 0x225F25DF, 0x20DFDE61, 0x225F25DF, 0x20DFDE61, // spt_a0_sandisk_40ohm
  0xEFE770BD, 0x4AFF0000, 0xEFE770BD, 0x4AFF0000, 0x7B84FF5B, 0x5B655B0E, 0x7B84FF5B, 0x5B655B0E, // spt_a0_hynix_50ohm
  0x7B93D620, 0xDFFF00E8, 0x7B93D620, 0xDFFF00E8, 0x3DC2FF3D, 0x38EA7DF3, 0x3DC2FF3D, 0x38EA7DF3, // spt_a0_hynix_40ohm
  0x50127FAC, 0x4400FD00, 0x50127FAC, 0x4400FD00, 0x7887FF78, 0x109A394F, 0x7887FF78, 0x109A394F, // spt_a0_hynix_33ohm
  0x50F20DBF, 0x3832CDFD, 0x50F20DBF, 0x3832CDFD, 0xFB04D720, 0xE69DC704, 0xFB04D720, 0xE69DC704, // spt_a0_samsung19nm_33ohm
  0x40EFFF00, 0x40FFFF00, 0x40EFFF00, 0x40FFFF00, 0xE85F20DF, 0x20DF003F, 0xE85F20DF, 0x20DF003F, // spt_a0_toshiba_50ohm
  0xFF00FF00, 0xFF00FF00, 0xFF00FF00, 0xFF00FF00, 0x20DFA0FF, 0x58DB20DF, 0x20DFA0FF, 0x58DB20DF, // spt_a0_toshiba_40ohm
  0x50FF0008, 0x7E10FFFF, 0x50FF0008, 0x7E10FFFF, 0x3B8F7A85, 0xFF7A5AFF, 0x3B8F7A85, 0xFF7A5AFF, // spt_b0_hynix_40ohm
  0x8A7575FF, 0x9ACA7F54, 0x8A7575FF, 0x9ACA7F54, 0xDF649B92, 0x7B24AFA0, 0xDF649B92, 0x7B24AFA0, // spt_b0_samsung16nm_50ohm
  0xFF00FF00, 0xFF00FF00, 0xFF00FF00, 0xFF00FF00, 0xDAD6EF50, 0x6B1C6B25, 0xDAD6EF50, 0x6B1C6B25, // spt_b0_samsung16nm_40ohm
  0x7f52ff02, 0xff02ff02, 0x7f52ff02, 0xff02ff02, 0x72bc63a5, 0x679afa9d, 0x72bc63a5, 0x679afa9d, // spt_b0_samsung16nm_33ohm
  0xff00ff00, 0xff00ff00, 0xff00ff00, 0xff00ff00, 0xff00ff00, 0xff00ff00, 0xff00ff00, 0xff00ff00, // baseline_pattern_8
  0xff00ff00, 0xff00ff00, 0xff00ff00, 0xff00ff00, 0xff00ff00, 0xff00ff00, 0xff00ff00, 0xff00ff00,
  0xff00ff00, 0xff00ff00, 0xff00ff00, 0xff00ff00, 0xff00ff00, 0xff00ff00, 0xff00ff00, 0xff00ff00,
  0xff00ff00, 0xff00ff00, 0xff00ff00, 0xff00ff00, 0xff00ff00, 0xff00ff00, 0xff00ff00, 0xff00ff00
};

/**
  Populates the dll tuning register list library data.

  @param[in] RegList  DLL Tuning Register list

**/
VOID
EFIAPI
SetEmmcDllTuningLibData (
  IN EMMC_DLL_TUNING_REG_LIST *RegList
  )
{
  EFI_STATUS                  Status;
  EMMC_DLL_TUNING_REG_LIST    *EmmcLibData;

  Status = GetLibraryData (PcdGet8 (PcdMmcTuningLibId), (VOID **)&EmmcLibData);
  if (EFI_ERROR (Status)) {
    EmmcLibData = AllocatePool (sizeof (*EmmcLibData));
  }

  CopyMem (EmmcLibData, RegList, sizeof (*EmmcLibData));
  Status = SetLibraryData (PcdGet8 (PcdMmcTuningLibId), EmmcLibData, sizeof (*EmmcLibData));
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_WARN, "EmmcDllTuningLibData not generated!\n"));
  }
}

/**
  Get the dll tuning register list.

  @param[out] RegList  DLL Tuning Register list

  @retval EFI_SUCCESS          Getting lib data successfully.
  @retval EFI_NOT_FOUND        The Lib data is not found.

**/
EFI_STATUS
EFIAPI
GetEmmcDllTuningLibData (
  OUT EMMC_DLL_TUNING_REG_LIST *RegList
  )
{
  EMMC_DLL_TUNING_REG_LIST   *MmcLibData;
  EFI_STATUS                  Status;

  Status = GetLibraryData (PcdGet8 (PcdMmcTuningLibId), (VOID **)&MmcLibData);
  if (EFI_ERROR (Status)) {
    return EFI_NOT_FOUND;
  }

  RegList->TxDataDllCntl1 = MmcLibData->TxDataDllCntl1;
  RegList->RxStrobeDllCntl = MmcLibData->RxStrobeDllCntl;
  return EFI_SUCCESS;
}

/**
  Set Tx Data Delay Control 1.

  @param[in] EmmcBaseAddress      Base address of Device MMIO registers
  @param[in] Value                Value (0 - 79)
  @param[in] RegList              EMMC DLL tuning relevant register List

**/
VOID
EmmcSetTxDllCtrl1 (
  IN UINTN                      EmmcBaseAddress,
  IN UINT8                      Value,
  IN EMMC_DLL_TUNING_REG_LIST  *RegList
  )
{
  MmioAndThenOr8 (EmmcBaseAddress + (RegList->TxDataDllCntl1 + 1), 0, Value);
}

/**
  Get Tx Data Delay Control 1.

  @param[in] EmmcBaseAddress      Base address of Device MMIO registers
  @param[in] RegList              EMMC DLL tuning relevant register List

  @retval UINT8                   DLL Value
**/
UINT8
EmmcGetTxDllCtrl1 (
  IN UINTN                      EmmcBaseAddress,
  IN EMMC_DLL_TUNING_REG_LIST  *RegList
  )
{
  UINT8 Value;

  Value = MmioRead8 (EmmcBaseAddress + (RegList->TxDataDllCntl1 + 1));

  return Value;
}

/**
  Set Rx Strobe Delay Control DLL1 for HS400.

  @param[in] EmmcBaseAddress      Base address of Device MMIO registers
  @param[in] RxDll                To program RxDll1 or RxDll2 register
  @param[in] Value                Value (0 - 39)
  @param[in] RegList              EMMC DLL tuning relevant register List

**/
VOID
EmmcSetRxDllCtrl (
  IN UINTN                      EmmcBaseAddress,
  IN UINT8                      RxDll,
  IN UINT8                      Value,
  IN EMMC_DLL_TUNING_REG_LIST  *RegList
  )
{
  if (RxDll == RxDll1) {
    MmioAndThenOr8 (EmmcBaseAddress + (RegList->RxStrobeDllCntl + 1), 0, Value);
  } else {
    MmioAndThenOr8 (EmmcBaseAddress + RegList->RxStrobeDllCntl, 0, Value);
  }
}

/**
  Get Rx Strobe Delay Control DLL1 for HS400.

  @param[in] EmmcBaseAddress      Base address of Device MMIO registers
  @param[in] RegList              EMMC DLL tuning relevant register List

  @retval UINT8                   DLL Value
**/
UINT8
EmmcGetRxDllCtrl (
  IN UINTN                      EmmcBaseAddress,
  IN EMMC_DLL_TUNING_REG_LIST  *RegList
  )
{
  UINT8 Value;

  Value = MmioRead8 (EmmcBaseAddress + RegList->RxStrobeDllCntl);

  return Value;
}

/**
  To perform HS400 Rx Data Path Training

  @param[in]  EmmcBaseAddress           Base address of device MMIO registers
  @param[in]  BlockLen                  Block length of device
  @param[in]  RegList                   EMMC DLL tuning relevant register List
  @param[in, out] EmmcTuningData        A pointer to EMMC_TUNING_DATA structure

  @retval EFI_SUCCESS                    HS400 Rx Data Path Training is successful.
  @retval EFI_OUT_OF_RESOURCES           The request could not be completed due to a lack of resources.
  @retval EFI_INVALID_PARAMETER          A parameter was incorrect.
  @retval EFI_DEVICE_ERROR               Hardware Error
  @retval EFI_NO_MEDIA                   No media
  @retval EFI_MEDIA_CHANGED              Media Change
  @retval EFI_BAD_BUFFER_SIZE            Buffer size is bad
  @retval EFI_CRC_ERROR                  Command or Data CRC Error
**/
EFI_STATUS
EmmcRxHs400Tuning (
  IN UINTN                        EmmcBaseAddress,
  IN UINT32                       BlockLen,
  IN EMMC_DLL_TUNING_REG_LIST     *RegList,
  IN OUT EMMC_TUNING_DATA         *EmmcTuningData
  )
{
  UINT8                     *Buffer;
  UINT8                     DllCount;
  UINT8                     Smin;
  UINT8                     Smax;
  UINT8                     Sopt;
  EFI_STATUS                Status;
  BLOCK_READ_WRITE_STATUS   FirstRead;
  UINT32                    TuningPatternSize;
  CONST UINT8               DllMax = RX_STROBE_DLL1_TAP_MAX_RANGE;
  CONST UINT8               DllMin = RX_STROBE_DLL1_TAP_MIN_RANGE;

  DEBUG ((DEBUG_VERBOSE, "EmmcRxHs400Tuning() Start\n"));

  Status = EFI_SUCCESS;
  Smin = RX_STROBE_DLL1_TAP_MIN_MEPT;
  Smax = RX_STROBE_DLL1_TAP_MAX_MEPT;

  TuningPatternSize = BlockLen * EMMC_HS400_TUNING_PATTERN_BLOCKS_NUMBER;

  Buffer = (VOID *) AllocatePool (TuningPatternSize);
  if (Buffer == NULL) {
    Status = EFI_OUT_OF_RESOURCES;
    goto Exit;
  }

  //
  // 1. Read Tuning Block
  //
  Status = MmcReadBlocks (
             0,
             PcdGet32 (PcdMmcTuningLba),
             TuningPatternSize,
             Buffer
             );
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "EmmcRxHs400Tuning: Tuning Blocks Read at HS200 Mode Failed!\n"));
    goto Exit;
  }

  //
  // 2. Move to HS400 Mode
  //
  Status = EmmcModeSelection (Hs400);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "EmmcRxHs400Tuning: eMMC HS400 Mode Selection Failed!\n"));
    goto Exit;
  }

  //
  // 3. Set Rx Strobe DLL1 to the Minimal Expected Passing Tap (Smin)
  //    Offset 830h: Rx Strobe Delay DLL 1(HS400 Mode), bits [14:8]
  //    Set Rx Data Strobe DLL2 to middle point 0x13
  //    Offset 830h: Rx Strobe Delay DLL 2(HS400 Mode), bits [6:0]
  DEBUG ((DEBUG_VERBOSE, "EmmcRxHs400Tuning: Set Rx Data DLL1 to the Minimal Expected Passing Tap (Smin = 0x%x)\n",
          Smin));
  DllCount = RX_STROBE_DLL1_TAP_MIN_MEPT;
  EmmcSetRxDllCtrl (EmmcBaseAddress, RxDll1, DllCount, RegList);
  EmmcSetRxDllCtrl (EmmcBaseAddress, RxDll2, DllCount, RegList);

  //
  // 4. Read the block that was stored
  // 5. If CRC fails on first read, increase the DLL Step and repeat block read until passed
  //    Else if CRC passed on first read, decrease the DLL Step and repeat block read until failed
  // 6. Store the Rx Path min DLL passing step number
  //
  FirstRead = NotAvailable;

  while ((DllCount <= DllMax) && (DllCount >= DllMin)) {
    DEBUG ((DEBUG_VERBOSE, "[ EmmcRxHs400Tuning: Rx Min DLL1 (DllCount) = 0x%x ]\n", DllCount));
    MicroSecondDelay (1 * 1000);
    Status = MmcReadBlocks (
               0,
               PcdGet32 (PcdMmcTuningLba),
               TuningPatternSize,
               Buffer
               );

    if (Status == EFI_SUCCESS) {
      if (FirstRead == NotAvailable) {
        FirstRead = Passed;
      } else if (FirstRead == Failed) {
        Smin = DllCount;
        break;
      }
      if (DllCount == RX_STROBE_DLL1_TAP_MIN_RANGE) {
        Smin = RX_STROBE_DLL1_TAP_MIN_RANGE;
        break;
      }
    } else if (Status == EFI_CRC_ERROR || Status == EFI_DEVICE_ERROR || Status == EFI_TIMEOUT) {
      if (FirstRead == NotAvailable) {
        FirstRead = Failed;
      } else if (FirstRead == Passed) {
        Smin = DllCount + 1;
        break;
      }
      if (DllCount == RX_STROBE_DLL1_TAP_MAX_RANGE) {
        Status = EFI_CRC_ERROR;
        DEBUG ((DEBUG_ERROR, "[Rx DLL Tuning Failed] DllCount == TAP_MAX_RANGE\n"));
        goto Exit;
      }
    } else {
      DEBUG ((DEBUG_ERROR, "[Rx DLL Tuning Failed] Smin - Tuning Blocks Read Error!\n"));
      goto Exit;
    }

    if (FirstRead == Failed) {
      DllCount++;
    } else {
      DllCount--;
    }
    EmmcSetRxDllCtrl (EmmcBaseAddress, RxDll1, DllCount, RegList);
    EmmcSetRxDllCtrl (EmmcBaseAddress, RxDll2, DllCount, RegList);
  }

  DEBUG ((DEBUG_VERBOSE, "[Rx DLL Tuning] Found Minimal Passing Tap = 0x%x\n", Smin));

  //
  // 7. Set the Rx Strobe DLL1 to the Maximal Expected Passing Tap (Smax)
  // Offset 830h: Rx Strobe Delay DLL 1(HS400 Mode), bits [14:8]
  // 8. Read the block that was stored
  // 9. If CRC fails on first read, decrease the DLL Step and repeat step 8 until pass
  //    Else if CRC passed on first read, increase the DLL Step and repeat step 8 until failed
  //
  DEBUG ((DEBUG_VERBOSE, "EmmcRxHs400Tuning: Set Rx Data DLL1 to the Maximal Expected Passing Tap (Smax = 0x%x)\n",
          Smax));
  DllCount = RX_STROBE_DLL1_TAP_MAX_MEPT;
  EmmcSetRxDllCtrl (EmmcBaseAddress, RxDll1, DllCount, RegList);
  EmmcSetRxDllCtrl (EmmcBaseAddress, RxDll2, DllCount, RegList);

  FirstRead = NotAvailable;
  while ((DllCount <= DllMax) && (DllCount >= DllMin)) {
    DEBUG ((DEBUG_VERBOSE, "[ EmmcRxHs400Tuning: Rx Max DLL1 (DllCount) = 0x%x ]\n", DllCount));
    MicroSecondDelay (1 * 1000);
    Status = MmcReadBlocks (
               0,
               PcdGet32 (PcdMmcTuningLba),
               TuningPatternSize,
               Buffer
               );

    if (Status == EFI_SUCCESS) {
      if (FirstRead == NotAvailable) {
        FirstRead = Passed;
      } else if (FirstRead == Failed) {
        Smax = DllCount;
        break;
      }
      if (DllCount == RX_STROBE_DLL1_TAP_MAX_RANGE) {
        Smax = DllCount;
        break;
      }
    } else if (Status == EFI_CRC_ERROR || Status == EFI_DEVICE_ERROR || Status == EFI_TIMEOUT) {
      if (FirstRead == NotAvailable) {
        FirstRead = Failed;
      } else if (FirstRead == Passed) {
        Smax = DllCount - 1;
        break;
      }
      if (DllCount == RX_STROBE_DLL1_TAP_MIN_RANGE) {
        Status = EFI_CRC_ERROR;
        goto Exit;
      }
    } else {
      DEBUG ((DEBUG_ERROR, "[Rx DLL Tuning Failed] Smax - Tuning Blocks Read Error!\n"));
      goto Exit;
    }
    if (FirstRead == Failed) {
      DllCount--;
    } else {
      DllCount++;
    }
    EmmcSetRxDllCtrl (EmmcBaseAddress, RxDll1, DllCount, RegList);
    EmmcSetRxDllCtrl (EmmcBaseAddress, RxDll2, DllCount, RegList);
  }

  //
  // 10. Store the Rx Path max DLL Passing Step number
  //

  //
  // 11. Compute the Rx DLL Optimal Point (Sopt) = (Smax - Smin)/2 + Smin
  //
  Sopt = (Smax - Smin) / 2 + Smin;
  DEBUG ((DEBUG_VERBOSE, "[Rx DLL Tuning] Optimal Point (Sopt = (Smax[0x%x] - Smin[0x%x]) / 2 + Smin[0x%x]) = 0x%x\n",
          Smax, Smin, Smin, Sopt));
  //
  // 12. Store the Rx DLL optimal value (Sopt)
  //
  EmmcSetRxDllCtrl (EmmcBaseAddress, RxDll1, Sopt, RegList);
  EmmcSetRxDllCtrl (EmmcBaseAddress, RxDll2, Sopt, RegList);

  Status = EFI_SUCCESS;
  EmmcTuningData->Hs400RxStrobe1Dll  = Sopt;

Exit:
  if (Buffer != NULL) {
    FreePool (Buffer);
  }
  DEBUG ((DEBUG_VERBOSE, "EmmcRxHs400Tuning() End, Status = %r\n", Status));
  return Status;
}

/**
  To perform HS400 Tx Data Path Training

  @param[in] EmmcBaseAddress            Base address of device MMIO registers
  @param[in] BlockLen                   Block length of device
  @param[in] RegList                    EMMC DLL tuning relevant register List
  @param[in, out] EmmcTuningData        A pointer to EMMC_TUNING_DATA structure

  @retval EFI_SUCCESS                    HS400 Rx Data Path Training is successful.
  @retval EFI_OUT_OF_RESOURCES           The request could not be completed due to a lack of resources.
  @retval EFI_INVALID_PARAMETER          A parameter was incorrect.
  @retval EFI_DEVICE_ERROR               Hardware Error
  @retval EFI_NO_MEDIA                   No media
  @retval EFI_MEDIA_CHANGED              Media Change
  @retval EFI_BAD_BUFFER_SIZE            Buffer size is bad
  @retval EFI_CRC_ERROR                  Command or Data CRC Error
**/
EFI_STATUS
EmmcTxHs400Tuning (
  IN UINTN                        EmmcBaseAddress,
  IN UINT32                       BlockLen,
  IN EMMC_DLL_TUNING_REG_LIST    *RegList,
  IN OUT EMMC_TUNING_DATA        *EmmcTuningData
  )
{
  UINT8                     *Buffer;
  UINT8                     DllCount;
  UINT8                     Smin;
  UINT8                     Smax;
  UINT8                     Sopt;
  UINT8                     Num;
  EFI_STATUS                Status;
  BLOCK_READ_WRITE_STATUS   FirstWrite;
  CONST UINT8               DllMax = TX_DATA_DLL_TAP_MAX_RANGE;
  CONST UINT8               DllMin = TX_DATA_DLL_TAP_MIN_RANGE;

  DEBUG ((DEBUG_VERBOSE, "EmmcTxHs400Tuning() Start\n"));

  Status = EFI_SUCCESS;
  Smin = TX_DATA_DLL_TAP_MIN_MEPT;
  Smax = TX_DATA_DLL_TAP_MAX_MEPT;
  Num = 0;

  Buffer = (VOID *) AllocatePool (BlockLen);
  if (Buffer == NULL) {
    Status = EFI_OUT_OF_RESOURCES;
    goto Exit;
  }
  //
  // 1. Read Tuning Block that used at Rx HS400 Tuning
  //
  Status = MmcReadBlocks (
             0,
             PcdGet32 (PcdMmcTuningLba),
             BlockLen,
             Buffer
             );
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "EmmcTxHs400Tuning: Tuning Block Read at HS400 Mode Failed!\n"));
    goto Exit;
  }

  //
  // 2. Set Tx Data DLL1 to the Minimal Expected Passing Tap (Smin)
  // Offset 824h: Tx Data Delay Control 1
  // Tx Data Delay (HS400 Mode), BIT[14:8]
  //
  DEBUG ((DEBUG_VERBOSE, "EmmcTxHs400Tuning: Set Tx Data DLL to the Minimal Expected Passing Tap (Smin = 0x%x)\n", Smin));

  DllCount = TX_DATA_DLL_TAP_MIN_MEPT;
  EmmcSetTxDllCtrl1 (EmmcBaseAddress, DllCount, RegList);

  //
  // 2. Write Single Block
  //
  FirstWrite = NotAvailable;

  while ((DllCount <= DllMax) && (DllCount >= DllMin)) {
    DEBUG ((DEBUG_VERBOSE, "[ EmmcTxHs400Tuning: Tx Min DLL (DllCount) = 0x%x ]\n", DllCount));
    MicroSecondDelay (1 * 1000);
    Status = MmcWriteBlocks (
               0,
               PcdGet32 (PcdMmcTuningLba),
               BlockLen,
               Buffer
               );

    if (Status == EFI_SUCCESS) {
      if (FirstWrite == NotAvailable) {
        FirstWrite = Passed;
      } else if (FirstWrite == Failed) {
        Smin = DllCount;
        break;
      }
      if (DllCount == TX_DATA_DLL_TAP_MIN_RANGE) {
        Smin = TX_DATA_DLL_TAP_MIN_RANGE;
        break;
      }
      //
      // 3. If CRC fails increment DLL Step and repeat step 2
      //
    } else if (Status == EFI_CRC_ERROR || Status == EFI_DEVICE_ERROR || Status == EFI_TIMEOUT) {
      if (FirstWrite == NotAvailable) {
        FirstWrite = Failed;
      } else if (FirstWrite == Passed) {
        Smin = DllCount + 1;
        break;
      }
      if (DllCount == TX_DATA_DLL_TAP_MAX_RANGE) {
        DEBUG ((DEBUG_ERROR, "[Tx DLL Tuning Failed] DllCount == TAP_MAX_RANGE\n"));
        goto Exit;
      }
    } else {
      DEBUG ((DEBUG_ERROR, "[Tx DLL Tuning Failed] Smin - Tuning Block Write Error!\n"));
      goto Exit;
    }
    if (FirstWrite == Failed) {
      DllCount++;
    } else {
      DllCount--;
    }
    EmmcSetTxDllCtrl1 (EmmcBaseAddress, DllCount, RegList);
  }
  //
  // 4. Store the Tx Path min DLL passing step number
  //
  DEBUG ((DEBUG_VERBOSE, "[Tx DLL Tuning] Found Minimal Passing Tap = 0x%x\n", Smin));

  //
  // 5. Set the DLL to max expected passing step (Smax)
  //
  DEBUG ((DEBUG_VERBOSE, "EmmcTxHs400Tuning: Set Tx Data DLL to the Maximal Expected Passing Tap (Smax = 0x%x)\n", Smax));
  DllCount = TX_DATA_DLL_TAP_MAX_MEPT;
  EmmcSetTxDllCtrl1 (EmmcBaseAddress, DllCount, RegList);
  //
  // 6. Write Single Block
  //
  FirstWrite = NotAvailable;

  while ((DllCount <= DllMax) && (DllCount >= DllMin)) {
    DEBUG ((DEBUG_VERBOSE, "[ EmmcTxHs400Tuning: Tx Max DLL1 (DllCount) = 0x%x ]\n", DllCount));
    MicroSecondDelay (1 * 1000);
    Status = MmcWriteBlocks (
               0,
               PcdGet32 (PcdMmcTuningLba),
               BlockLen,
               Buffer
               );

    if (Status == EFI_SUCCESS) {
      if (FirstWrite == NotAvailable) {
        FirstWrite = Passed;
      } else if (FirstWrite == Failed) {
        Smax = DllCount;
        break;
      }
      if (DllCount == TX_DATA_DLL_TAP_MAX_RANGE) {
        Smax = TX_DATA_DLL_TAP_MAX_RANGE;
        break;
      }
      //
      // 7. If CRC fails decrement DLL Step and repeat step 6
      //
    } else if (Status == EFI_CRC_ERROR || Status == EFI_DEVICE_ERROR || Status == EFI_TIMEOUT) {
      if (FirstWrite == NotAvailable) {
        FirstWrite = Failed;
      } else if (FirstWrite == Passed) {
        Smax = DllCount - 1;
        break;
      }
      if (DllCount == TX_DATA_DLL_TAP_MIN_RANGE) {
        DEBUG ((DEBUG_ERROR, "[Tx DLL Tuning Failed] DllCount == TAP_MIN_RANGE\n"));
        goto Exit;
      }
    } else {
      DEBUG ((DEBUG_ERROR, "[Tx DLL Tuning Failed] Smax - Tuning Block Write Error!\n"));
      goto Exit;
    }
    if (FirstWrite == Failed) {
      DllCount--;
    } else {
      DllCount++;
    }
    EmmcSetTxDllCtrl1 (EmmcBaseAddress, DllCount, RegList);
  }
  //
  // 8. Store the DLL passing step number (Smax)
  //
  DEBUG ((DEBUG_VERBOSE, "[Tx DLL Tuning] Found Maximal Passing Tap = 0x%x\n", Smax));

  Num = Smax - Smin;
  if (Num <= 0) {
    Status = EFI_DEVICE_ERROR;
    goto Exit;
  }
  //
  // 9. Compute the Tx DLL Optimal point (Sopt) = (Smax - Smin) / 2 + Smin
  //
  Sopt = (Smax - Smin) / 2 + Smin;
  DEBUG ((DEBUG_VERBOSE, "[Tx DLL Tuning] Optimal Point (Sopt = (Smax[0x%x] - Smin[0x%x]) / 2 + Smin[0x%x]) = 0x%x\n",
          Smax, Smin, Smin, Sopt));

  //
  // 10. Store the Tx Strobe DLL Optimal point value
  //
  EmmcSetTxDllCtrl1 (EmmcBaseAddress, Sopt, RegList);

  Status = EFI_SUCCESS;
  EmmcTuningData->Hs400TxDataDll  = Sopt;

Exit:
  if (Buffer != NULL) {
    FreePool (Buffer);
  }
  DEBUG ((DEBUG_VERBOSE, "EmmcTxHs400Tuning() End, Status = %r\n", Status));
  return Status;
}

/**
  Writes worst case pattern used for HS400 taining

  @param[in]  BlockLen                   Block length of device
  @param[in]  TuningPattern              A pointer to EMMC_TUNING_DATA structure

  @retval EFI_SUCCESS                    HS400 Rx Data Path Training is successful.
  @retval EFI_OUT_OF_RESOURCES           The request could not be completed due to a lack of resources.
  @retval EFI_INVALID_PARAMETER          A parameter was incorrect.
  @retval EFI_DEVICE_ERROR               Hardware Error
  @retval EFI_NO_MEDIA                   No media
  @retval EFI_MEDIA_CHANGED              Media Change
  @retval EFI_BAD_BUFFER_SIZE            Buffer size is bad
**/
EFI_STATUS
EmmcWriteHs400TuningPattern (
  IN UINT32                       BlockLen,
  IN UINT8                        *TuningPattern
  )
{
  UINT8                     *Buffer;
  UINT32                    Index;
  UINT32                    TuningPatternSize;
  EFI_STATUS                Status;

  Status = EFI_SUCCESS;
  TuningPatternSize = BlockLen * EMMC_HS400_TUNING_PATTERN_BLOCKS_NUMBER;

  DEBUG ((DEBUG_VERBOSE, "EmmcWriteHs400TuningPattern() Start\n"));

  Buffer = (VOID *) AllocatePool (TuningPatternSize);
  if (Buffer == NULL) {
    Status = EFI_OUT_OF_RESOURCES;
    goto Exit;
  }

  for (Index = 0; Index < EMMC_HS400_TUNING_PATTERN_BLOCKS_NUMBER; Index++) {
    CopyMem (Buffer + (Index * BlockLen), TuningPattern, BlockLen);
  }

  Status = MmcWriteBlocks (
             0,
             PcdGet32 (PcdMmcTuningLba),
             TuningPatternSize,
             Buffer
             );

  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "EmmcWriteHs400TuningPattern: Multiple Blocks Write at HS200 Mode Failed!\n"));
    goto Exit;
  }

Exit:
  if (Buffer != NULL) {
    FreePool (Buffer);
  }
  DEBUG ((DEBUG_VERBOSE, "EmmcWriteHs400TuningPattern() End, Status = %r\n", Status));
  return Status;
}

/**
  Configure eMMC DLLs to HS400 Mode

  @param[in]  EmmcHcPciBase               Base address of device MMIO registers
  @param[in]  BlockLen                    Block length of device
  @param[in]  RegList                     EMMC DLL tuning relevant register List
  @param[out] EmmcTuningData              A pointer to EMMC_TUNING_DATA structure

  @retval EFI_SUCCESS                     The function completed successfully
  @retval EFI_NOT_FOUND                   The item was not found
  @retval EFI_OUT_OF_RESOURCES            The request could not be completed due to a lack of resources.
  @retval EFI_INVALID_PARAMETER           A parameter was incorrect.
  @retval EFI_DEVICE_ERROR                Hardware Error
  @retval EFI_NO_MEDIA                    No media
  @retval EFI_MEDIA_CHANGED               Media Change
  @retval EFI_BAD_BUFFER_SIZE             Buffer size is bad
  @retval EFI_CRC_ERROR                   Command or Data CRC Error
**/
EFI_STATUS
EFIAPI
ConfigureEmmcHs400Mode (
  IN UINTN                              EmmcHcPciBase,
  IN UINT32                             BlockLen,
  IN EMMC_DLL_TUNING_REG_LIST          *RegList,
  OUT EMMC_TUNING_DATA                 *EmmcTuningData
  )
{
  EFI_STATUS                    Status;
  EFI_STATUS                    ModeStatus;
  UINTN                         EmmcBaseAddress;

  Status      =  EFI_SUCCESS;
  ModeStatus  =  EFI_SUCCESS;

  DEBUG ((DEBUG_VERBOSE, "ConfigureEmmcHs400Mode() Start\n"));
  EmmcTuningData->Hs400DataValid = FALSE;
  EmmcBaseAddress  = MmioRead32 (EmmcHcPciBase + PCI_BASE_ADDRESSREG_OFFSET) & 0xFFFFF000;

  DEBUG ((DEBUG_VERBOSE, "Initial HS400 DLL values :\n"));
  DEBUG ((DEBUG_VERBOSE, "Tx Data Delay Control 1 = 0x%08x\n", MmioRead32 (EmmcBaseAddress + RegList->TxDataDllCntl1)));
  DEBUG ((DEBUG_VERBOSE, "Rx Strobe Delay Control = 0x%08x\n", MmioRead32 (EmmcBaseAddress + RegList->RxStrobeDllCntl)));

  Status = EmmcWriteHs400TuningPattern (BlockLen, (UINT8 *)EmmcWorstCasePattern);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "ConfigureEmmcHs400Mode: EmmcWriteHs400TuningPattern Failed!\n"));
  }

  //
  // Rx HS400 Auto Tuning
  //
  Status = EmmcRxHs400Tuning (EmmcBaseAddress, BlockLen, RegList, EmmcTuningData);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "ConfigureEmmcHs400Mode: Rx HS400 Auto Tuning Failed!\n"));
    ModeStatus = EmmcModeSelection (Hs200);
    if (EFI_ERROR (ModeStatus)) {
      DEBUG ((DEBUG_ERROR, "EmmcRxHs400Tuning: eMMC HS200 Mode Selection Failed!\n"));
    }
    EmmcTuningData->Hs400DataValid = FALSE;
    return Status;
  }

  //
  // Tx HS400 Auto Tuning
  //
  Status = EmmcTxHs400Tuning (EmmcBaseAddress, BlockLen, RegList, EmmcTuningData);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "ConfigureEmmcHs400Mode: Tx HS400 Auto Tuning Failed!\n"));
    ModeStatus = EmmcModeSelection (Hs200);
    if (EFI_ERROR (ModeStatus)) {
      DEBUG ((DEBUG_ERROR, "EmmcTxHs400Tuning: eMMC HS200 Mode Selection Failed!\n"));
    }
    EmmcTuningData->Hs400DataValid = FALSE;
    return Status;
  }

  DEBUG ((DEBUG_VERBOSE, "ConfigureEmmcHs400Mode () End\n"));
  EmmcTuningData->Hs400DataValid = TRUE;
  return Status;
}


/**
  Configure eMMC DLLs with given Tuning Data.

  @param[in]  EmmcHcPciBase               Base address of device MMIO registers
  @param[in]  RegList                     EMMC DLL tuning relevant register List
  @param[in]  EmmcTuningData              A pointer to EMMC_TUNING_DATA structure

**/
VOID
EFIAPI
MmcSetDlls (
  IN UINTN                              EmmcHcPciBase,
  IN EMMC_DLL_TUNING_REG_LIST          *RegList,
  IN EMMC_TUNING_DATA                  *EmmcTuningData
  )
{
  UINTN   EmmcBaseAddress;

  if ((EmmcTuningData != NULL) && (EmmcTuningData->Hs400DataValid)) {
    EmmcBaseAddress   = MmioRead32 (EmmcHcPciBase + PCI_BASE_ADDRESSREG_OFFSET) & 0xFFFFF000;

    EmmcSetRxDllCtrl (EmmcBaseAddress, RxDll1, EmmcTuningData->Hs400RxStrobe1Dll, RegList);
    EmmcSetRxDllCtrl (EmmcBaseAddress, RxDll2, EmmcTuningData->Hs400RxStrobe1Dll, RegList);
    EmmcSetTxDllCtrl1 (EmmcBaseAddress, EmmcTuningData->Hs400TxDataDll, RegList);
  }
}


/**
  Populate Tuning Data with existing programmed eMMC DLLs.

  @param[in]  EmmcHcPciBase               Base address of device MMIO registers
  @param[in]  RegList                     EMMC DLL tuning relevant register List
  @param[out] EmmcTuningData              A pointer to EMMC_TUNING_DATA structure

**/
VOID
EFIAPI
MmcGetDlls (
  IN UINTN                              EmmcHcPciBase,
  IN EMMC_DLL_TUNING_REG_LIST          *RegList,
  OUT EMMC_TUNING_DATA                 *EmmcTuningData
  )
{
  UINTN   EmmcBaseAddress;

  EmmcBaseAddress                   = MmioRead32 (EmmcHcPciBase + PCI_BASE_ADDRESSREG_OFFSET) & 0xFFFFF000;
  EmmcTuningData->Hs400DataValid    = TRUE;
  EmmcTuningData->Hs400RxStrobe1Dll = EmmcGetRxDllCtrl (EmmcBaseAddress, RegList);
  EmmcTuningData->Hs400TxDataDll    = EmmcGetTxDllCtrl1 (EmmcBaseAddress, RegList);
}


/**
  This function tunes EMMC device to HS400

  @param[in] EmmcHcPciBase              Base address of device MMIO registers
  @param[in] BlockLen                   Block length of device
  @param[in] RegList                    EMMC DLL tuning relevant register List
  @param[out] EmmcTuningData            EMMC DLL tuning data

  @retval EFI_SUCCESS                     The function completed successfully
  @retval EFI_NOT_FOUND                   The item was not found
  @retval EFI_OUT_OF_RESOURCES            The request could not be completed due to a lack of resources.
  @retval EFI_INVALID_PARAMETER           A parameter was incorrect.
  @retval EFI_DEVICE_ERROR                Hardware Error
  @retval EFI_NO_MEDIA                    No media
  @retval EFI_MEDIA_CHANGED               Media Change
  @retval EFI_BAD_BUFFER_SIZE             Buffer size is bad
  @retval EFI_CRC_ERROR                   Command or Data CRC Error
**/
EFI_STATUS
EFIAPI
MmcTuneDllsToHs400 (
  IN  UINTN                       EmmcHcPciBase,
  IN  UINT32                      BlockLen,
  IN  EMMC_DLL_TUNING_REG_LIST   *RegList,
  OUT EMMC_TUNING_DATA           *EmmcTuningData
  )
{
  EFI_STATUS                    Status;

  DEBUG ((DEBUG_INFO, "Mmc Dll Tuning for HS400 ...\n"));
  Status = ConfigureEmmcHs400Mode (EmmcHcPciBase, BlockLen, RegList, EmmcTuningData);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "MmcTuneDllsToHs400: Tuning Failed!\n"));
    goto Done;
  }
  DEBUG ((DEBUG_INFO, "Mmc Dll Tuning for HS400 Successful\n"));

Done:
  return Status;
}



/**
  This function tunes EMMC device

  @param[in]  EmmcHcPciBase               Base address of device MMIO registers
  @param[out] EmmcTuningData              EMMC DLL tuning data

  @retval EFI_SUCCESS                     The function completed successfully
  @retval EFI_ABORTED                     The function aborted in the middle
  @retval EFI_NOT_FOUND                   The item was not found
  @retval EFI_OUT_OF_RESOURCES            The request could not be completed due to a lack of resources.
  @retval EFI_INVALID_PARAMETER           A parameter was incorrect.
  @retval EFI_DEVICE_ERROR                Hardware Error
  @retval EFI_NO_MEDIA                    No media
  @retval EFI_MEDIA_CHANGED               Media Change
  @retval EFI_BAD_BUFFER_SIZE             Buffer size is bad
  @retval EFI_CRC_ERROR                   Command or Data CRC Error
**/
EFI_STATUS
DoMmcTuneDlls (
  IN   UINTN                       EmmcHcPciBase,
  OUT  EMMC_TUNING_DATA           *EmmcTuningData
  )
{
  EFI_STATUS                Status;
  EMMC_DLL_TUNING_REG_LIST  EmmcDllTuningRegList;
  EMMC_TUNING_DATA         *EmmcTuningDataCurrent;
  EMMC_TUNING_DATA          EmmcTuningDataInitial;
  DEVICE_BLOCK_INFO         DevBlockInfo;

  Status                = EFI_SUCCESS;
  EmmcTuningDataCurrent = EmmcTuningData;

  GetEmmcDllTuningLibData (&EmmcDllTuningRegList);
  MmcGetDlls (EmmcHcPciBase, &EmmcDllTuningRegList, &EmmcTuningDataInitial);

  MmcGetMediaInfo (0, &DevBlockInfo);
  Status = MmcTuneDllsToHs400 (EmmcHcPciBase, DevBlockInfo.BlockSize, &EmmcDllTuningRegList, EmmcTuningData);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "MMC DLL tuning failed, status = %r, restoring initial dlls\n", Status));
    EmmcTuningDataCurrent = &EmmcTuningDataInitial;
  }

  MmcSetDlls (EmmcHcPciBase, &EmmcDllTuningRegList, EmmcTuningDataCurrent);
  DEBUG ((DEBUG_INFO, "MMC DLL data (initial) - TX 0x%02x | RX 0x%02x\n", EmmcTuningDataInitial.Hs400TxDataDll,
          EmmcTuningDataInitial.Hs400RxStrobe1Dll));
  DEBUG ((DEBUG_INFO, "MMC DLL data (current) - TX 0x%02x | RX 0x%02x\n", EmmcTuningDataCurrent->Hs400TxDataDll,
          EmmcTuningDataCurrent->Hs400RxStrobe1Dll));

  return Status;
}


/**
  This function tuning the device.

  @param[in]  DevHcPciBase     Device Host Controller's PCI ConfigSpace Base address


  @retval EFI_SUCCESS          The request is executed successfully.
  @retval Others               The request could not be executed successfully.

**/
EFI_STATUS
EFIAPI
MmcTuning (
  IN  UINTN                 MmcHcPciBase
  )
{
  EFI_STATUS                Status;
  EMMC_TUNING_DATA          EmmcTuningData;
  UINTN                     VariableLen;
  UINT32                    PrintLevel;
  UINT32                    FeatureCfg;
  EMMC_DLL_TUNING_REG_LIST  EmmcDllTuningRegList;

  FeatureCfg = GetFeatureCfg ();


  if ((FeatureCfg & FEATURE_MMC_TUNING) == 0) {
    DEBUG ((DEBUG_WARN, "eMMC Tuning feature is disabled!\n"));
    return EFI_SUCCESS;
  }

  //
  // Get emmc tuning regesters.
  //
  Status = GetEmmcDllTuningLibData (&EmmcDllTuningRegList);
  if (EFI_ERROR (Status)) {
    EmmcDllTuningRegList.TxDataDllCntl1  = 0x824;
    EmmcDllTuningRegList.RxStrobeDllCntl = 0x830;
    SetEmmcDllTuningLibData (&EmmcDllTuningRegList);
  }

  //
  // Check if eMMC tuning is required.
  //
  VariableLen = sizeof (EmmcTuningData);
  ZeroMem (&EmmcTuningData, VariableLen);
  Status = GetVariable (L"MMCDLL", NULL, NULL, &VariableLen, &EmmcTuningData);
  if (!EFI_ERROR (Status)) {
    if ((FeatureCfg & FEATURE_MMC_FORCE_TUNING) == 0) {
      DEBUG ((DEBUG_INFO, "Found eMMC tunning data, re-tuning is not required.\n"));
      MmcSetDlls (MmcHcPciBase, &EmmcDllTuningRegList, &EmmcTuningData);
      return EFI_SUCCESS;
    }
  }

  //
  // Now need do eMMC tunning.
  // Mask of all output since errors are expected during tuning
  //
  if (FeaturePcdGet (PcdEmmcHs400SupportEnabled)) {
    // Only do eMMC tunning when HS400 is enabled.
    DEBUG ((DEBUG_INFO, "Execute eMMC tunning\n"));
    PrintLevel = GetDebugPrintErrorLevel ();
    SetDebugPrintErrorLevel (0);
    Status = DoMmcTuneDlls (MmcHcPciBase, &EmmcTuningData);
    SetDebugPrintErrorLevel (PrintLevel);
    if (EFI_ERROR (Status)) {
      DEBUG ((DEBUG_ERROR, "MMC MmcTuneDlls error, status = %r\n", Status));
      return Status;
    }

    DEBUG ((DEBUG_INFO, "eMMC tunning done, RxDll=%02X TxDll=%02X\n",
      EmmcTuningData.Hs400RxStrobe1Dll, EmmcTuningData.Hs400TxDataDll));
  }

  // Todo: Serial number should be removed from this function later.
  Status = EmmcGetSerialNumber (EmmcTuningData.SerialNumber, sizeof(EmmcTuningData.SerialNumber));
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "ERRORL: MMC serial number invalid, status = %r\n", Status));
  }

  Status = SetVariable (L"MMCDLL", NULL, 0, sizeof(EMMC_TUNING_DATA), &EmmcTuningData);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "MMC DLL data save error, status = %r\n", Status));
  }

  return Status;
}

