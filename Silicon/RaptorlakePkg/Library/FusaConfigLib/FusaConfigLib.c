/** @file
  This file contains the FuSa configuration support library template.

  Copyright (c) 2023, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/
#include <Base.h>
#include <Uefi.h>
#include <PiPei.h>
#include <Library/HobLib.h>
#include <Library/FusaConfigLib.h>
#include <FspsUpd.h>
#include <FspmUpd.h>
#include <Library/BlMemoryAllocationLib.h>
#include <Library/ContainerLib.h>
#include <ConfigDataDefs.h>
#include <Library/DebugLib.h>
#include <Library/SpiFlashLib.h>
#include <Library/ConfigDataLib.h>
#include <Library/BootloaderCommonLib.h>
#include <Library/BootloaderCoreLib.h>
#include <Library/PchGspiLib.h>
#include <Library/TpmLib.h>


/**
  FuSa Info HOB version
  Use this to compare to the HOB retrieved from the FSP for the
  exact match
**/
#define FUSA_INFO_VERSION 0x00000102

/** Constants used for FUSA_TEST_RESULT->CheckResults[] and
 *  FUSA_TEST_RESULT->TestResult */
/**@defgroup ResultConstant Check Result Constants*/
/**@{*/
#define FUSA_TEST_DEVICE_NOTAVAILABLE  0xFF    // device is not available
#define FUSA_TEST_NOTRUN               0x0U    // check is not run
#define FUSA_TEST_FAIL                 0xD2U   // check fail
#define FUSA_TEST_PASS                 0x2DU   // check pass
#define FUSA_TEST_NOT_SUPPORTED        0x55    // Test Not Supported
/**@}*/

/** Fusa test result structure
 **/
typedef struct
{
  UINT32 TestNumber;            // test number assigned to this test
  UINT32 TotalChecks;           // total number of checks in this test
  UINT8  TestResult;            // if all tests passed then this is FUSA_TEST_PASS.
                                // if at least one check fails, then this is TEST_FAIL
                                // if the device (eg. MC channel DIMM) is not available
                                // then this is FUSA_TEST_DEVICE_NOTAVAILABLE.
                                // if the test has not been run, then this is FUSA_TEST_NOTRUN
  UINT8  ReservedByte[3];       // reserved, as padding for 4 byte-alignment
  UINT8  CheckResults[32];      // test result for each check.
  UINT32 Crc32;                 // crc32 of the structure
} FUSA_TEST_RESULT;

#define FUSA_TOTAL_NUMBER_OF_TESTS    79

/**
  Fusa test result HOB structure
**/
typedef struct {
  UINT32 Version;
  FUSA_TEST_RESULT FspDxCtcTestResult[FUSA_TOTAL_NUMBER_OF_TESTS];
} FUSA_INFO_HOB;


/**
 * Map TestResult to its matching string for FuSa test
 *
 * @param[in] TestResult
 *
 * @return matching string in related to the TestResult value
 */
STATIC
CHAR16*
TestResultString (
  IN UINT8 TestResult
  )
{
  STATIC CHAR16 ResultString[][15] = {
    L"TEST_DEV_NA",
    L"TEST_NOTRUN",
    L"TEST_FAIL",
    L"TEST_PASS",
    L"TEST_NOT_SPT",
    L"Illegal state"
  } ;

  UINT32 ResultIndex;
  switch (TestResult) {
    case FUSA_TEST_DEVICE_NOTAVAILABLE:
      ResultIndex = 0;
      break;
    case FUSA_TEST_NOTRUN:
      ResultIndex = 1;
      break;
    case FUSA_TEST_FAIL:
      ResultIndex = 2;
      break;
    case FUSA_TEST_PASS:
      ResultIndex = 3;
      break;
    case FUSA_TEST_NOT_SUPPORTED:
      ResultIndex = 4;
      break;
    default:
      ResultIndex = 5;
      break;
  }
  return &(ResultString[ResultIndex][0]);
}


/**
  Dump Fusa test result for debug purpose. Does nothing in
  release build.

  @param[in] pFusaTestResult - pointer to the result buffer to
        be dumped
**/
VOID
DumpResults(
  IN FUSA_TEST_RESULT *pFusaTestResult
  )
{
  DEBUG ((DEBUG_INFO, "Test number = %d \n", pFusaTestResult->TestNumber));
  DEBUG ((DEBUG_INFO, "Test result = %s \n", TestResultString(pFusaTestResult->TestResult)));
  DEBUG ((DEBUG_INFO, "Total checks = %d \n", pFusaTestResult->TotalChecks));
  for (UINT32 i = 0; i < pFusaTestResult->TotalChecks; i++) {
    DEBUG ((DEBUG_INFO, "  Check#%d result = %s\n", i, TestResultString(pFusaTestResult->CheckResults[i])));
  }
}


/**
  Perform any Pre-memory FuSa Configuration required, including updating FSP-M UPDs

  @param  FspmUpdPtr    The pointer to the FSP-M UPD to be updated.

  @retval EFI_STATUS
**/
EFI_STATUS
EFIAPI
FusaConfigPreMem (
  VOID *FspmUpdPtr
  )
{
  FSPM_UPD                    *FspmUpd;
  FSP_M_CONFIG                *FspmConfig;
  FUSA_CFG_DATA               *FusaCfgData;
  SILICON_CFG_DATA            *SiliconCfgData;

  FspmUpd              = (FSPM_UPD *) FspmUpdPtr;
  FspmConfig           = &FspmUpd->FspmConfig;

  FusaCfgData =(FUSA_CFG_DATA *) FindConfigDataByTag (CDATA_FUSA_TAG);

  if (FusaCfgData != NULL) {
    FspmConfig->McParity             = FusaCfgData->McParity;
    FspmConfig->IbeccParity          = FusaCfgData->IbeccParity;
  }

  SiliconCfgData = (SILICON_CFG_DATA*) FindConfigDataByTag(CDATA_SILICON_TAG);

  if (SiliconCfgData != NULL) {
    FspmConfig->CpuCrashLogEnable = SiliconCfgData->CpuCrashLogEnable;
  }

  return EFI_SUCCESS;
}

/**
  Perform any Post-memory FuSa Configuration required, including updating FSP-S UPDs

  @param  FspsUpdPtr    The pointer to the FSP-S UPD to be updated.

  @retval EFI_STATUS
**/
EFI_STATUS
EFIAPI
FusaConfigPostMem (
  VOID *FspsUpdPtr
  )
{
  EFI_STATUS                  Status;
  FSPS_UPD                    *FspsUpd;
  FSP_S_CONFIG                *FspsConfig;
  FUSA_CFG_DATA               *FusaCfgData;
  UINT32                       FusaComp;
  VOID                        *FusaCompInMem;
  UINT32                      FusaCompLength;
  UINT8                       *FusaStartupBistPattern;
  UINT8                       *FusaPeriodicBistPattern;
  EFI_PLATFORM_FIRMWARE_BLOB  FusaStartupBistPatBlob;
  EFI_PLATFORM_FIRMWARE_BLOB  FusaPeriodicBistPatBlob;
  VOID                        *FspHobList;
  EFI_HOB_GUID_TYPE           *RawHob;

  FspHobList = GetFspHobListPtr();
  if (FspHobList != NULL) {
    RawHob = GetNextGuidHob (&gSiFusaInfoGuid, FspHobList);
    if (RawHob != NULL) {
      DEBUG((DEBUG_INFO, "Found FuSa Hob with GUID \"%g\", booting in FuSa diagnostic mode\n", &gSiFusaInfoGuid));
      Status = PcdSetBoolS(PcdFusaDiagnosticBoot,TRUE);
      ASSERT_EFI_ERROR(Status);
    }
  }
  if (FspHobList == NULL || RawHob == NULL) {
    DEBUG((DEBUG_INFO, "FuSa Hob not found, abort FuSa diagnostic boot flow.\n"));
  }

  FspsUpd              = (FSPS_UPD *) FspsUpdPtr;
  FspsConfig           = &FspsUpd->FspsConfig;

  FusaCfgData =(FUSA_CFG_DATA *) FindConfigDataByTag (CDATA_FUSA_TAG);

  if (FusaCfgData != NULL) {
    FspsConfig->FusaConfigEnable = 1;
    FspsConfig->DisplayFusaConfigEnable = FusaCfgData->DisplayFusaConfigEnable;
    FspsConfig->GraphicFusaConfigEnable = FusaCfgData->GraphicFusaConfigEnable;
    FspsConfig->OpioFusaConfigEnable = FusaCfgData->OpioFusaConfigEnable;
    FspsConfig->IopFusaConfigEnable = FusaCfgData->IopFusaConfigEnable;
    FspsConfig->PsfFusaConfigEnable = FusaCfgData->PsfFusaConfigEnable;
    FspsConfig->FusaRunPeriodicArrayBistMod0 = FusaCfgData->FusaPeriodicArrayBist0;
    FspsConfig->FusaRunPeriodicArrayBistMod1 = FusaCfgData->FusaPeriodicArrayBist1;
    FspsConfig->FusaRunPeriodicScanBistMod0 = FusaCfgData->FusaPeriodicScanBist0;
    FspsConfig->FusaRunPeriodicScanBistMod1 = FusaCfgData->FusaPeriodicScanBist1;
    FspsConfig->Module0Lockstep = FusaCfgData->Module0Lockstep;
    FspsConfig->Module1Lockstep = FusaCfgData->Module1Lockstep;
    // Enable GSPI 0 for CRB
    FspsConfig->SerialIoSpiMode[0] = 0x1;

    // Load FuSa binary ingredients
    Status = GetComponentInfo(
                SIGNATURE_32 ('F', 'U', 'F', 'W'),
                &FusaComp,
                &FusaCompLength
                );
    DEBUG((DEBUG_VERBOSE, "Find Fusa FW component Status: %r, Base 0x%X, Size 0x%X\n", Status, FusaComp, FusaCompLength));

    if (!EFI_ERROR(Status)) {
      FusaComp -= PcdGet32(PcdFlashBaseAddress);
      FusaCompInMem = AllocateTemporaryMemory(FusaCompLength);
      if (FusaCompInMem != NULL) {
        DEBUG((DEBUG_VERBOSE, "Allocate successful. Reading from SPI\n"));
        Status = SpiFlashRead(
            FlashRegionBios,
            FusaComp,
            FusaCompLength,
            FusaCompInMem
            );
        DEBUG((DEBUG_VERBOSE, "Copy FuSa FW Container to Mem Status: %r\n", Status));
        DEBUG((DEBUG_VERBOSE, "Signature: 0x%X\n",((CONTAINER_HDR*)FusaCompInMem)->Signature));
      }
      Status = RegisterContainer((UINT32)FusaCompInMem, NULL);
      DEBUG((DEBUG_VERBOSE, "Register Fusa FW Container Status: %r\n", Status));

      if (!EFI_ERROR(Status)) {
        FusaStartupBistPattern = NULL;
        Status = LoadComponent (SIGNATURE_32 ('F', 'U', 'F', 'W'), SIGNATURE_32 ('F', 'S', 'B', 'P'), (VOID**)&FusaStartupBistPattern, &FusaCompLength);
        DEBUG((DEBUG_VERBOSE, "LoadComponent FSBP Status: %r\n", Status));
        if (Status == EFI_SUCCESS) {
          DEBUG((DEBUG_VERBOSE, "Reading FuSa Startup BIST pattern from address 0x%X, Length 0x%X\n", FusaStartupBistPattern, FusaCompLength));
          DEBUG((DEBUG_VERBOSE, "Signature: 0x%X\n", ((LOADER_COMPRESSED_HEADER*)FusaStartupBistPattern)->Signature));
          FspsConfig->FusaStartupPatternAddr = (UINT32)FusaStartupBistPattern;
          // Extend Startup BIST pattern FW into PCR 0
          if (MEASURED_BOOT_ENABLED() && (GetBootMode() != BOOT_ON_S3_RESUME)) {
            FusaStartupBistPatBlob.BlobBase = (UINT64)(UINTN)FusaStartupBistPattern;
            FusaStartupBistPatBlob.BlobLength = FusaCompLength;
            TpmHashAndExtendPcrEventLog (0, FusaStartupBistPattern, FusaCompLength, EV_PLATFORM_CONFIG_FLAGS, sizeof(FusaStartupBistPatBlob), (UINT8 *)&FusaStartupBistPatBlob);
          }
        }

        FusaPeriodicBistPattern = NULL;
        Status = LoadComponent (SIGNATURE_32 ('F', 'U', 'F', 'W'), SIGNATURE_32 ('F', 'P', 'B', 'P'), (VOID**)&FusaPeriodicBistPattern, &FusaCompLength);
        DEBUG((DEBUG_VERBOSE, "LoadComponent FPBP Status: %r\n", Status));
        if (Status == EFI_SUCCESS) {
          DEBUG((DEBUG_VERBOSE, "Reading FuSa Periodic BIST pattern from address 0x%X, Length 0x%X\n", FusaPeriodicBistPattern, FusaCompLength));
          DEBUG((DEBUG_VERBOSE, "Signature: 0x%X\n", ((LOADER_COMPRESSED_HEADER*)FusaPeriodicBistPattern)->Signature));
          if (FspsConfig->FusaRunPeriodicScanBistMod0 || FspsConfig->FusaRunPeriodicScanBistMod1) {
            FspsConfig->FusaPeriodicPatternAddr = (UINT32)FusaPeriodicBistPattern;
          }
          // Extend Periodic BIST pattern FW into PCR 0
          if (MEASURED_BOOT_ENABLED() && (GetBootMode() != BOOT_ON_S3_RESUME)) {
            FusaPeriodicBistPatBlob.BlobBase = (UINT64)(UINTN)FusaPeriodicBistPattern;
            FusaPeriodicBistPatBlob.BlobLength = FusaCompLength;
            TpmHashAndExtendPcrEventLog (0, FusaPeriodicBistPattern, FusaCompLength, EV_PLATFORM_CONFIG_FLAGS, sizeof(FusaPeriodicBistPatBlob), (UINT8 *)&FusaPeriodicBistPatBlob);
          }
        }
      }
    }
  }

  return EFI_SUCCESS;
}

/**
  Perform any Post-Silicon FuSa Configuration required.

  @retval EFI_STATUS
 */
EFI_STATUS
EFIAPI
FusaConfigPostSi (
  VOID
  )
{
  VOID                *FspHobList;
  EFI_HOB_GUID_TYPE   *RawHob;
  EFI_STATUS          Status;
  UINT8               GspiController;
  FUSA_CFG_DATA       *FusaCfgData;
  UINTN               TestIndex;
  FUSA_INFO_HOB       *FusaHob;
  UINTN               ByteIndex;

  if (!PcdGetBool(PcdFusaDiagnosticBoot)) {
    DEBUG((DEBUG_INFO, "Not Booting in FuSa diagnostic mode, continue normal boot flow!\n"));
    return EFI_NOT_FOUND;
  }

  // Diagnostics results HOB present after FSP-M indicates FuSa diagnostics boot.
  // Control will never return from here.

  FspHobList = GetFspHobListPtr();
  if (FspHobList == NULL) {
    CpuHalt("Booting in FuSa diagnostic mode, but results HOB not  found, system will go into halt!");
  }

  RawHob = GetNextGuidHob (&gSiFusaInfoGuid, FspHobList);
  if (RawHob == NULL) {
    CpuHalt("Booting in FuSa diagnostic mode, but results HOB not  found, system will go into halt!");
  }

  DEBUG((DEBUG_INFO, "Raw Diagnostics HOB length: 0x%X data:\n", RawHob->Header.HobLength));

  for (ByteIndex = 0 ; ByteIndex < RawHob->Header.HobLength ; ByteIndex++) {
    DEBUG((DEBUG_INFO, "%02X ", ((UINT8*)RawHob)[ByteIndex]));
    if (ByteIndex % 16 == 15) {
      DEBUG((DEBUG_INFO, "\n"));
    }
  }

  FusaHob = (FUSA_INFO_HOB*)(RawHob+1);

  for (TestIndex = 0 ; TestIndex < FUSA_TOTAL_NUMBER_OF_TESTS ; TestIndex++) {
    DumpResults(&FusaHob->FspDxCtcTestResult[TestIndex]);
  }

  GspiController = 0;
  FusaCfgData = (FUSA_CFG_DATA *) FindConfigDataByTag (CDATA_FUSA_TAG);
  if (FusaCfgData != NULL) {
    GspiController = FusaCfgData->DiagGspiCtrlr;
  }

  Status = PchGspiInit(GspiController);
  if (EFI_ERROR(Status)) {
    DEBUG((DEBUG_INFO, "Initialize GSPI controller %r\n", Status));
    CpuHalt("Booting in FuSa diagnostic mode, but GSPI controller experienced error! Halting!");
  }

  DEBUG((DEBUG_INFO, "Sending Diagnostics HOB over GSPI #%d\n", GspiController));
  Status = PchGspiWrite(GspiController, 0, (UINT8*)RawHob, RawHob->Header.HobLength);
  DEBUG((DEBUG_INFO, "Status %r. Deadlooping until external reset.\n", Status));

  // Deadloop. External Safety Island will trigger a reboot.
  CpuHalt("Booting in FuSa diagnostic mode. Sending diagnostic results and halting!");

  // Code should never reach here.
  return EFI_SUCCESS;
}
