/** @file

  Copyright (c) 2020 Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent
**/

#include <PiPei.h>
#include <Library/BaseLib.h>
#include <Library/DebugLib.h>
#include <Library/HobLib.h>
#include <Guid/FusaInfoGuid.h>
#include <Library/BootloaderCoreLib.h>

/**
  Map TestResult to its matching string for FuSa test

  @param[in] TestResult

  @return matching string in related to the TestResult value
**/
STATIC
CHAR8*
TestResultString (
  UINT8 TestResult
  )
{
  STATIC CHAR8 ResultString[][15] = {
    "TEST_DEV_NA",
    "TEST_NOTRUN",
    "TEST_FAIL",
    "TEST_PASS",
    "Illegal state"
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
    default:
      ResultIndex = 4;
      break;
  }
  return &(ResultString[ResultIndex][0]);
}

/**
  Find FSP Fusa info HOB.
  Existence of this HOB means that the system entered Diagnostic mode.

  @return  EFI_SUCCESS           Success if Hob found.
  @return  EFI_NOT_FOUND      If Hob is NULL
**/
EFI_STATUS
FindFspFusaInfoHob (
  VOID
  )
{
  FUSA_INFO_HOB                 *FspFusaInfo;
  EFI_HOB_GUID_TYPE             *GuidHob;
  VOID                          *FspHobList;
  UINT32                         CheckIndex;
  UINT32                         TestIndex;

  GuidHob = NULL;
  FspFusaInfo = NULL;

  // Find Fusa Info Hob from FSP
  FspHobList = GetFspHobListPtr ();
  if (FspHobList != NULL) {
    GuidHob = GetNextGuidHob (&gSiFusaInfoGuid, FspHobList);
  }
  if (GuidHob == NULL) {
    return EFI_NOT_FOUND;
  }
  FspFusaInfo = GET_GUID_HOB_DATA (GuidHob);
  if (FspFusaInfo == NULL) {
    return EFI_NOT_FOUND;
  }
  DEBUG((DEBUG_INFO, "\nEntering Fusa Key-off flow\n"));
  DEBUG((DEBUG_INFO, "\nFspFusaInfo\n"));
  DEBUG((DEBUG_INFO, "  |-> Version : %d\n", FspFusaInfo->Version));
  for (TestIndex = 0; TestIndex < FusaTestNumTotal; TestIndex++) {
    DEBUG((DEBUG_INFO, "  |-> Test Id : %d\n", FspFusaInfo->FspDxCtcTestResult[TestIndex].TestNumber));
    if(CalculateCrc32(&(FspFusaInfo->FspDxCtcTestResult[TestIndex]), sizeof(FUSA_TEST_RESULT)) == 0) {
      DEBUG((DEBUG_INFO, "    |-> Total checks : %d\n", FspFusaInfo->FspDxCtcTestResult[TestIndex].TotalChecks));
      DEBUG((DEBUG_INFO, "    |-> Test result : %a\n", TestResultString (FspFusaInfo->FspDxCtcTestResult[TestIndex].TestResult)));
      for (CheckIndex = 0; CheckIndex < FspFusaInfo->FspDxCtcTestResult[TestIndex].TotalChecks; CheckIndex++){
         DEBUG((DEBUG_INFO, "      |-> Check Id %d result: %s\n", CheckIndex, TestResultString (FspFusaInfo->FspDxCtcTestResult[TestIndex].CheckResults[CheckIndex])));
      }
    } else {
      DEBUG((DEBUG_INFO, "    |-> CRC32 check on test result failed\n"));
    }
  }
  DEBUG((DEBUG_INFO, "\n"));
  return EFI_SUCCESS;
}

/**
  IOTG FuSa SKU Key-off processing
  Dump Fusa Info Hob and enter Cpu Dead loop if HOB is found
**/
VOID
FusaKeyOffProcessing (
  VOID
  )
{
  EFI_STATUS Status;

  //Sample code performs only result dumping; in production code, expect passing the result to the MCU when present
  // If EFI_SUCCESS, Fusa Hob is found which means that system is in diagnostic mode. Enter DeadLoop.
  Status = FindFspFusaInfoHob ();
  if(Status == EFI_SUCCESS) {
    CpuDeadLoop ();  //expecting the MCU to G3 reset the system
  }
}
