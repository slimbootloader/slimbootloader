/** @file

  Copyright (c) 2018, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <PiPei.h>
#include <Library/PcdLib.h>
#include <Library/DebugLib.h>
#include <Library/BaseLib.h>
#include <Library/SocInitLib.h>
#include <IndustryStandard/Acpi.h>
#include <Library/BaseMemoryLib.h>
#include <Library/MemoryAllocationLib.h>
#include <IndustryStandard/Acpi.h>
#include <IndustryStandard/Tpm20.h>
#include <Library/BpdtLib.h>
#include <Library/HeciLib.h>
#include <Library/PciLib.h>
#include <Library/BootGuardLib20.h>
#include <Library/SecureBootLib.h>
#include <Guid/PcdDataBaseSignatureGuid.h>
#include <Library/ConfigDataLib.h>
#include <Library/PcdLib.h>
#include <Library/BootloaderCoreLib.h>
#include <Library/BootloaderCommonLib.h>
#include <Guid/FlashMapInfoGuid.h>
#include <PlatformData.h>
#include <PsdLib.h>

#define EFI_ACPI_OEM_ID                   "INTEL "  // OEMID 6 bytes long
#define EFI_ACPI_OEM_TABLE_ID             SIGNATURE_64('E','D','K','2',' ',' ',' ',' ') // OEM table id 8 bytes long
#define EFI_ACPI_OEM_REVISION             0x00000005
#define EFI_ACPI_CREATOR_ID               SIGNATURE_32('I','N','T','L')
#define EFI_ACPI_CREATOR_REVISION         0x0100000D
#define PSD_VERSION_MAJOR                 0x0000
#define PSD_VERSION_MINOR                 0x0001

/**
  Wrapper function to Get EOM status from CSE Status Register.

  @param[in,out] EomState         Pointer to The EOM state value.

  @retval EFI_SUCCESS         The EOM Get Status successfully.
  @retval Others              The EOM Get from CSE doesn't get status.

 **/
EFI_STATUS
GetEomState (
  IN OUT UINT8 *EomState
  )
{
  EFI_STATUS            Status;
  UINT32                FwSts;

  if(EomState == NULL) {
    DEBUG ((DEBUG_ERROR, "EomState is not valid pointer\n"));
    return EFI_INVALID_PARAMETER;
  }
  Status = HeciReadFwStatus (&FwSts);
  if (EFI_ERROR(Status)) {
    return Status;
  }
  *EomState = (UINT8)((FwSts & BIT4) >> 4);
  return EFI_SUCCESS;
}

/**
Get Sec FW Version.
@param[in]: SecVersion Pointer to  Sec FW Veersion structure.
@return EFI_SUCCESS.
@return EFI_ERROR.

**/
EFI_STATUS
GetSecFwVersion (
    SEC_VERSION_INFO *SecVersion
    )
{
  EFI_STATUS            Status;
  GEN_GET_FW_VER_ACK    MsgGenGetFwVersionAckData;

  if (SecVersion == NULL) {
    DEBUG ((DEBUG_ERROR, "GetSecFwVersion Failed Status=0x%x\n",EFI_INVALID_PARAMETER));
    return EFI_INVALID_PARAMETER;
  }

  Status = MeGetFwVersionFromMbp (&MsgGenGetFwVersionAckData.Data);
  if (EFI_ERROR (Status)) {
    Status = HeciGetFwVersionMsg( (UINT8 *)&MsgGenGetFwVersionAckData);
  }
  if (!EFI_ERROR (Status)) {
    SecVersion->CodeMajor   = MsgGenGetFwVersionAckData.Data.CodeMajor;
    SecVersion->CodeMinor   = MsgGenGetFwVersionAckData.Data.CodeMinor;
    SecVersion->CodeHotFix  = MsgGenGetFwVersionAckData.Data.CodeHotFix;
    SecVersion->CodeBuildNo = MsgGenGetFwVersionAckData.Data.CodeBuildNo;
  }
  return Status;
}

/**
Get Sec Capabilities of CSE.
@param[in] SecCapability Pointer to Sec Caps.
@retval EFI_SUCCESS Success get all FW hash value
@retval EFI_ERROR Unable to get hash value

**/
EFI_STATUS
GetSecCapability (
  UINT32    *SecCapability
  )
{
  EFI_STATUS               Status;
  GEN_GET_FW_CAPS_SKU_ACK  MsgGenGetFwCapsSkuAck;
  if(SecCapability == NULL) {
    DEBUG ((DEBUG_ERROR, "GetSecCapability Failed Status=0x%x\n",EFI_INVALID_PARAMETER));
    return EFI_INVALID_PARAMETER;
  }


  Status = HeciGetFwCapsSkuMsg ((UINT8 *)&MsgGenGetFwCapsSkuAck);
  if (EFI_ERROR(Status)) {
    return Status;
  }
  *SecCapability = MsgGenGetFwCapsSkuAck.Data.FWCap.Data;
  return EFI_SUCCESS;
}


/**
Get hash value for all Firmware.
@param[in] FwHashData Pointer of FW hash value stored
@retval EFI_SUCCESS Success get all FW hash value
@retval EFI_ERROR Unable to get hash value

**/
EFI_STATUS
GetIbbHashDataFromBpm (
  IN UINT8     *FwHashData
  )
{
  EFI_STATUS                     Status;
  BPMDATA                        *BpmData;
  UINT32                         BpmSize;
  UINT32                         BpmBase;

  if(FwHashData == NULL) {
    DEBUG ((DEBUG_ERROR, "GetIbbHashDataFromBpm, Status=0x%x\n",EFI_INVALID_PARAMETER));
    return EFI_INVALID_PARAMETER;
  }
  DEBUG ((DEBUG_ERROR, "FwHashData Addr=0x%x\n",FwHashData));

  Status = GetComponentInfo (FLASH_MAP_SIG_BPM, &BpmBase, &BpmSize);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "GetSubPartitionDataByName, Status=0x%x\n", Status));
    return Status;
  }
  DEBUG ((DEBUG_INFO, "SubPartitoinPayload BpmBase=0x%p, size=0x%x\n", BpmBase, BpmSize));

  BpmData = (BPMDATA*)(UINTN)BpmBase;
  //IBBL Hash
  if(BpmData->IbblHash == NULL) {
    return RETURN_BUFFER_TOO_SMALL;
  }
  CopyMem(FwHashData, BpmData->IbblHash, BpmData->IbblHashSize);
  DEBUG ((DEBUG_INFO, "BpmData->IbblHash IBBL Hash: 0x%08x\n", BpmData->IbblHash));
  DumpHex (2, 0, BpmData->IbblHashSize, (VOID *)BpmData->IbblHash);

  //IBBM Hash
  if(BpmData->IbbmHash == NULL) {
    return RETURN_BUFFER_TOO_SMALL;
  }
  CopyMem((FwHashData + BpmData->IbblHashSize), BpmData->IbbmHash, BpmData->IbbmHashSize);
  DEBUG ((DEBUG_INFO, "BpmData->IbbmHash IBB Hash: 0x%08x\n", BpmData->IbbmHash));
  DumpHex (2, 0, BpmData->IbbmHashSize, (VOID *)BpmData->IbbmHash);

  //OBB Hash, we will calculate Stage-2 HASH from hash store ( OBB = Stage2 + padding ),

  return EFI_SUCCESS;
}

/**

  Update Platform Service Discovery Table.
  @param[in] Table Pointer of ACPI Table Data.
  @retval EFI_SUCCESS Installed PSD ACPI table successfully.
  @retval EFI_ ERROR.

**/
EFI_STATUS
EFIAPI
UpdateAcpiPsdTable (
  IN VOID* Table
  )
{
  EFI_ACPI_PSD_TABLE             *mPsdt;
  PLATFORM_DATA                   *PlatformData;
  CONST UINT8                     *PubKeyHash;
  EFI_STATUS                      Status;
  UINT8                           HashIndex;
  UINT8                           HashType;

  DEBUG((DEBUG_INFO, "UpdateAcpiPsdTable start\n"));
  if ( Table == NULL) {
    DEBUG((DEBUG_WARN, "EFI_ACPI_PSD_TABLE IS NULL\n"));
    return EFI_BUFFER_TOO_SMALL;
  }
  mPsdt = (EFI_ACPI_PSD_TABLE*)Table;
  // Populate Platfrom security capabilities in table structure
  mPsdt->Header.Signature               = EFI_ACPI_PSD_SIGNATURE;
  mPsdt->Header.Checksum                = 0;

  if( &(mPsdt->Header.OemId) == NULL) {
    return RETURN_BUFFER_TOO_SMALL;
  }
  CopyMem(&mPsdt->Header.OemId, EFI_ACPI_OEM_ID, 6);
  mPsdt->Header.OemTableId              = EFI_ACPI_OEM_TABLE_ID;
  mPsdt->Header.OemRevision             = EFI_ACPI_OEM_REVISION;
  mPsdt->Header.CreatorId               = EFI_ACPI_CREATOR_ID;
  mPsdt->Header.CreatorRevision         = EFI_ACPI_CREATOR_REVISION;

  DEBUG( (DEBUG_INFO, "Address of PSD_TABLE=%x\n", mPsdt));
  DEBUG( (DEBUG_INFO, "PSD Values: Signature=%x\n", mPsdt->Header.Signature) );
  DEBUG( (DEBUG_INFO, "PSD Values: Length=%x\n", mPsdt->Header.Length ));
  DEBUG( (DEBUG_INFO, "PSD Values: Revision=%x\n", mPsdt->Header.Revision ));
  DEBUG( (DEBUG_INFO, "PSD Values: Checksum=%x\n", mPsdt->Header.Checksum ));
  DEBUG( (DEBUG_INFO, "PSD Values: OemId=%x\n", mPsdt->Header.OemId ));
  DEBUG( (DEBUG_INFO, "PSD Values: OemTableId=%x\n", mPsdt->Header.OemTableId ));

  mPsdt->PsdVersion.PsdVerMajor = PSD_VERSION_MAJOR;
  mPsdt->PsdVersion.PsdVerMinor = PSD_VERSION_MINOR;
  DEBUG( (DEBUG_INFO, "PSD Values:  PsdVerMajor=%x\n", mPsdt->PsdVersion.PsdVerMajor ));
  DEBUG( (DEBUG_INFO, "PSD Values:  PsdVerMinor=%x\n", mPsdt->PsdVersion.PsdVerMinor ));

  //Eom State,
  Status = GetEomState(&mPsdt->EomState);
  if (EFI_ERROR(Status)) {
    DEBUG((DEBUG_INFO, " GetEomState failed =%x\n",Status));
  }
  DEBUG( (DEBUG_INFO, "PSD Values:  EomState =%x\n", mPsdt->EomState ));

  //Sec Capabilities,
  Status = GetSecCapability( &(mPsdt->CsmeSecCapabilities) );
  if (EFI_ERROR(Status)) {
    DEBUG((DEBUG_INFO, " GetSecCapability failed =%x\n",Status));
  }
  DEBUG((DEBUG_INFO, "PSD Values: CsmeSecCapabilities=%x\n", mPsdt->CsmeSecCapabilities));

  //FW version,
  Status = GetSecFwVersion( &(mPsdt->FwVer) );
  if (EFI_ERROR(Status)) {
    DEBUG((DEBUG_INFO, " GetSecCFwVersion failed =%x\n",Status));
  }
  DEBUG( (DEBUG_INFO, "PSD Values:  CodeMajor=%x\n", mPsdt->FwVer.CodeMajor ));
  DEBUG( (DEBUG_INFO, "PSD Values:  CodeMinor=%x\n", mPsdt->FwVer.CodeMinor ));
  DEBUG( (DEBUG_INFO, "PSD Values:  CodeHotFix=%x\n", mPsdt->FwVer.CodeHotFix ));
  DEBUG( (DEBUG_INFO, "PSD Values:  CodeBuildNo=%x \n", mPsdt->FwVer.CodeBuildNo ));
  if( &(mPsdt->FwVendor) == NULL) {
    return RETURN_BUFFER_TOO_SMALL;
  }
  CopyMem(&mPsdt->FwVendor, EFI_ACPI_PSD_FW_VENDOR, EFI_ACPI_PSD_FW_VENDOR_SIZE);
  PlatformData = (PLATFORM_DATA *)GetPlatformDataPtr();
  if (PlatformData == NULL) {
    DEBUG(( DEBUG_INFO, "PSD Values:  GetPlatformDataPtr Failed\n" ));
    return  EFI_UNSUPPORTED;
  }

  //00 - Secure boot is Disabled; 01 - Verified boot is enabled by ACM; 11 - Verified Boot in enabled for IBBL IBB and OBB,
  mPsdt->SecureBoot = (UINT8)(((PlatformData->BtGuardInfo.Bpm.Vb) << 1)| FeaturePcdGet (PcdVerifiedBootEnabled));
  mPsdt->MeasuredBoot = (UINT8)((PlatformData->BtGuardInfo.Bpm.Mb));

  //0 - No HWRoT; 1 - ROM based RoT; 2 - TXE; 3 - CSE; 4 - ACM; 5 - TXT
  mPsdt->PsdtType                      = 3;
  mPsdt->FwHashIndex                    = PsdCseHash;
  mPsdt->FwHashDataLen                  = EFI_ACPI_PSD_FW_HASH_DATA_LEN;
  DEBUG((DEBUG_INFO, "PSD Values:  SecureBootEnabled=%x\n", mPsdt->SecureBoot));
  DEBUG((DEBUG_INFO, "PSD Values:  MeasuredBootEnabled=%x\n", mPsdt->MeasuredBoot));
  DEBUG((DEBUG_INFO, "PSD Values:  PsdtType=%x\n", mPsdt->PsdtType));

  if ( mPsdt->SecureBoot ) {
    // When measured boot is enable, hashes will be extended to FIT table by CSE,
    if ( mPsdt->MeasuredBoot ) {

      //TXE HASH
      if( (mPsdt->FwHashData == NULL) && (PlatformData->BtGuardInfo.TxeHash == NULL) ) {
            return RETURN_BUFFER_TOO_SMALL;
      }
      CopyMem ((VOID*)(mPsdt->FwHashData), (VOID*)(PlatformData->BtGuardInfo.TxeHash) , mPsdt->FwHashDataLen);
      DumpHex (2, 0, mPsdt->FwHashDataLen, (VOID *)mPsdt->FwHashData);
      DEBUG ((DEBUG_INFO, "TXE Hash: 0x%08x\n", PlatformData->BtGuardInfo.TxeHash));
      DumpHex (2, 0, SHA256_DIGEST_SIZE, (VOID *)PlatformData->BtGuardInfo.TxeHash);

      //IBBL Hash
      if( (((mPsdt->FwHashData) + PsdIbblHash*(mPsdt->FwHashDataLen)) == NULL) && (PlatformData->BtGuardInfo.IbblHash == NULL) ) {
        return RETURN_BUFFER_TOO_SMALL;
      }
      CopyMem ((VOID*)((mPsdt->FwHashData) + PsdIbblHash*(mPsdt->FwHashDataLen)), (VOID*)(PlatformData->BtGuardInfo.IbblHash), mPsdt->FwHashDataLen);
      DEBUG ((DEBUG_INFO, "IBBL Hash: 0x%08x\n", PlatformData->BtGuardInfo.IbblHash));
      DumpHex (2, 0, mPsdt->FwHashDataLen, (VOID*)((mPsdt->FwHashData) + mPsdt->FwHashDataLen));

      //IBB Hash
      if( (((mPsdt->FwHashData) + PsdIbbHash*(mPsdt->FwHashDataLen)) == NULL) && (PlatformData->BtGuardInfo.IbblHash == NULL) ) {
        return RETURN_BUFFER_TOO_SMALL;
      }
      CopyMem ((VOID*)((mPsdt->FwHashData) + PsdIbbHash*(mPsdt->FwHashDataLen)), (VOID*)(PlatformData->BtGuardInfo.IbbHash.Hash), mPsdt->FwHashDataLen);
      DEBUG ((DEBUG_INFO, "IBB Hash: 0x%08x\n", PlatformData->BtGuardInfo.IbbHash.Hash));
      DumpHex (2, 0, mPsdt->FwHashDataLen, (VOID*)((mPsdt->FwHashData) + PsdIbbHash*mPsdt->FwHashDataLen));
      HashIndex = (UINT8)PsdObbHash;
      mPsdt->FwHashIndex = PsdFwHashMax;
    }
    //Verified boot is enabled (MeasureBoot is NOT enabled),
    if ( !(mPsdt->MeasuredBoot) && (PlatformData->BtGuardInfo.Bpm.Vb) ) {
      GetIbbHashDataFromBpm(mPsdt->FwHashData);
      HashIndex = (UINT8)PsdIbbHash;
      mPsdt->FwHashIndex = PsdObbHash;
    }
    //Stage-2 HASH,
    if ( (PlatformData->BtGuardInfo.Bpm.Vb) || (mPsdt->MeasuredBoot) ) {
      // Get public key hash of Stage-2
      Status = GetComponentHash (COMP_TYPE_STAGE_2, &PubKeyHash, &HashType);
      if (RETURN_ERROR (Status)) {
        DEBUG ((DEBUG_ERROR, "Warning: Component (%d) verification is bypassed.\n", COMP_TYPE_STAGE_2));
      } else {
        //Debug Prints
        if( (((mPsdt->FwHashData) + HashIndex*(mPsdt->FwHashDataLen)) == NULL) && (PubKeyHash == NULL) ) {
          return RETURN_BUFFER_TOO_SMALL;
        }
        CopyMem ((VOID*)((mPsdt->FwHashData) + HashIndex*(mPsdt->FwHashDataLen)), PubKeyHash, mPsdt->FwHashDataLen);
        DEBUG ((DEBUG_INFO, "OBB Hash: 0x%08x\n", PubKeyHash));
        DumpHex (2, 0, mPsdt->FwHashDataLen, (VOID*)((mPsdt->FwHashData) + 2*mPsdt->FwHashDataLen));
      }
    }
  }
  DEBUG( (DEBUG_INFO, "PSD Values:  FwHashIndex=%x\n", mPsdt->FwHashIndex ));
  DEBUG( (DEBUG_INFO, "PSD Values:  FwHashDataLen=%x\n", mPsdt->FwHashDataLen ));
  DEBUG( (DEBUG_INFO, "PSD Values:  FwHashDataSize=%x\n", sizeof(mPsdt->FwHashData) ));
  DumpHex (2, 0, sizeof(EFI_ACPI_PSD_TABLE), (VOID *)Table);
  DEBUG( (DEBUG_INFO, "UpdateAcpiPsdTable() end\n") );

  return  EFI_SUCCESS;
}
