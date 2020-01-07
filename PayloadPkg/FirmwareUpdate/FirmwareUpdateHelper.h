/** @file
  The header file for internal firmware update definitions.

  Copyright (c) 2020, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef __INTERNAL_FIRMWARE_UPDATE_LIB_H__
#define __INTERNAL_FIRMWARE_UPDATE_LIB_H__

/**
  Update a region block.

  This is the acture function to update boot meia. It will erase boot device,
  write new data to boot device, and verify the written data.

  @param[in] Address          The boot media address to be update.
  @param[in] Buffer           The source buffer to write to the boot media.
  @param[in] Length           The length of data to write to boot media.

  @retval  EFI_SUCCESS        Update successfully.
  @retval  others             Error happening when updating.
**/
EFI_STATUS
EFIAPI
UpdateRegionLessthan4k (
  IN  UINT64    Address,
  IN  VOID      *Buffer,
  IN  UINT32    Length
  );

/**
  Update a region block.

  This is the acture function to update boot meia. It will erase boot device,
  write new data to boot device, and verify the written data.

  @param[in] Address          The boot media address to be update.
  @param[in] Buffer           The source buffer to write to the boot media.
  @param[in] Length           The length of data to write to boot media.

  @retval  EFI_SUCCESS        Update successfully.
  @retval  others             Error happening when updating.
**/
EFI_STATUS
EFIAPI
UpdateRegionBlock (
  IN  UINT64    Address,
  IN  VOID      *Buffer,
  IN  UINT32    Length
  );

/**
  Update a boot region.

  This function also output the update process info.

  @param[in] UpdateRegion     The detail information for this region to update.
  @param[in] WrittenSize      The data size has been written before this region.
  @param[in] TotalSize        The total size need to write for the partition.

  @retval  EFI_SUCCESS        Update this boot region successfully.
  @retval  others             Error happening when updating boot region.
**/
EFI_STATUS
UpdateBootRegion (
  IN  FIRMWARE_UPDATE_REGION     *UpdateRegion,
  IN  UINT32                     WrittenSize,
  IN  UINT32                     TotalSize
  );

/**
  Update all the regions in this boot partition.

  @param[in] UpdatePartition  The detail information for this update.

  @retval  EFI_SUCCESS        Update this boot partition successfully.
  @retval  others             Error happening when updating boot partition.
**/
EFI_STATUS
UpdateBootPartition (
  IN FIRMWARE_UPDATE_PARTITION   *UpdatePartition
  );

/**
  Verify the firmware version to make sure it is no less than current firmware version.

  @param[in] ImageHdr     Pointer to the fw mgmt capsule image header
  @param[in] FwPolicy     Firmware update policy.

  @retval  EFI_SUCCESS    The operation completed successfully.
  @retval  others         There is error happening.
**/
EFI_STATUS
VerifyFwVersion (
  IN  EFI_FW_MGMT_CAP_IMAGE_HEADER  *ImageHdr,
  IN  FIRMWARE_UPDATE_POLICY  FwPolicy
  );

/**
  Set state machine flag in flash.

  This function will set state machine flag in the bootloader reserved region
  First byte in the booloader reserved region is state machine flag.

  @param[in] StateMachine     State machine flag byte.

  @retval  EFI_SUCCESS        State machine flag set.
  @retval  others             Error while setting state machine flag.
**/
EFI_STATUS
SetStateMachineFlag (
  IN UINT8    StateMachine
);

/**
  This function will enforce firmware update policy.

  Firmware update policy

  ----------------------------------------------------------
  |  SM   |   TS   |             Operation                 |
  ----------------------------------------------------------
  |  FF   |    0   | Set SM to FE, Set TS and reboot       |
  |  FF   |    1   | Set SM to FD, clear TS and reboot     |
  |  FE   |    0   | Set TS and reboot                     |
  |  FE   |    1   | Set SM to FC, clear TS and reboot     |
  |  FD   |    0   | Set SM to FC, reboot                  |
  |  FD   |    1   | clear TS and reboot                   |
  |  FC   |    0   | Clear IBB signal,Set SM to FF, reboot |
  |  FC   |    1   | Clear IBB signal,Set SM to FF, reboot |
  ----------------------------------------------------------
  @param[in][out] FwPolicy    Pointer to Firmware update policy.

  @retval  EFI_SUCCESS        The operation completed successfully.
  @retval  others             There is error happening.
**/
EFI_STATUS
EnforceFwUpdatePolicy (
  IN FIRMWARE_UPDATE_POLICY   *FwPolicy
 );

/**
  This function will enforce firmware update policy after
  partition update is successful.

  After update firmware update policy

  @param[in] FwPolicy         Firmware update policy.

  @retval  EFI_SUCCESS        The operation completed successfully.
  @retval  others             There is error happening.
**/
EFI_STATUS
AfterUpdateEnforceFwUpdatePolicy (
  IN FIRMWARE_UPDATE_POLICY   FwPolicy
 );

/**
  Perform system Firmware update.

  This function will update SBL or Configuration data alone.

  @param[in] ImageHdr       Pointer to fw mgmt capsule Image header

  @retval  EFI_SUCCESS      Update successful.
  @retval  other            error occurred during firmware update
**/
EFI_STATUS
UpdateSystemFirmware (
  IN EFI_FW_MGMT_CAP_IMAGE_HEADER  *ImageHdr
  );
#endif
