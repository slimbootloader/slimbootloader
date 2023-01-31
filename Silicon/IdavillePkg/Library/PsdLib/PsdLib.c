/** @file

  Copyright (c) 2020 - 2021, Intel Corporation. All rights reserved.<BR>
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
#include <Library/HeciLib.h>
#include <Library/PciLib.h>
#include <Library/BootGuardLib.h>
#include <Library/SecureBootLib.h>
#include <Guid/PcdDataBaseSignatureGuid.h>
#include <Library/ConfigDataLib.h>
#include <Library/PcdLib.h>
#include <Library/BootloaderCoreLib.h>
#include <Library/BootloaderCommonLib.h>
#include <Guid/FlashMapInfoGuid.h>
#include <PlatformData.h>
#include <Library/BootGuardLib.h>
#include <PsdLib.h>
#include <RegAccess.h>
#include <Library/ConfigDataLib.h>
#include <ConfigDataStruct.h>
#include <MeBiosPayloadData.h>
#include <CpuDataStruct.h>
#include <Register/CpuRegs.h>
#include <MeChipset.h>

#define CPUID_STRUCTURED_EXTENDED_FEATURE_FLAGS 0x7
#define PSD_VERSION_MAJOR                               0x0000
#define PSD_VERSION_MINOR                               0x0004
#define PSD_HROT_NONE                                   0
#define PSD_HROT_ROM                                    1
#define PSD_HROT_TXE                                    2
#define PSD_HROT_CSE                                    3
#define PSD_HROT_ACM                                    4
#define PSD_HROT_TXT                                    5
#define TME_MT_MAX_KEY_MASK                             0x0007FFF000000000
#define TME_MT_MAX_KEY_IDX                              36

/**
  Checks if Me FW is SPS Ignition type

  @retval FALSE           No SPS Ignition on board
  @retval TRUE            SPS Ignition detected
**/
BOOLEAN
MeTypeIsSpsIgnition (
  VOID
  )
{
  HECI_FWS_REGISTER     MeFirmwareStatus;
  UINT8                 MeOpMode;

  MeFirmwareStatus.ul = HeciPciRead32 (R_ME_HFS);
  if (MeFirmwareStatus.ul == 0xFFFFFFFF) {
    DEBUG ((DEBUG_ERROR, "ERROR: HECI interface is disabled\n"));
    return TRUE;
  }

  MeOpMode = (UINT8) MeFirmwareStatus.r.MeOperationMode;
  DEBUG ((DEBUG_INFO, "[SPS] OpMode=0x%x, ErrCode=0x%x\n", MeFirmwareStatus.r.MeOperationMode, MeFirmwareStatus.r.ErrorCode));

  if (MeOpMode == ME_OPERATION_MODE_SPS_IGNITION) {
    return TRUE;
  }

  return FALSE;
}

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
  VOID                  *FspHobListPtr;
  ME_BIOS_PAYLOAD       *MbpDataHob;
  UINT32                MbpDataHobLen;
  UINT8                 *DataPtr;
  UINT32                FwSts;

  if(EomState == NULL) {
    DEBUG ((DEBUG_ERROR, "EomState is not valid pointer\n"));
    return EFI_INVALID_PARAMETER;
  }

  //
  // Fill the FW state data from MBP if it exists
  //
  FspHobListPtr = GetFspHobListPtr();
  if (FspHobListPtr != NULL) {
    DataPtr = (UINT8 *)GetGuidHobData (FspHobListPtr, &MbpDataHobLen, &gMeBiosPayloadHobGuid);
    if ((DataPtr != NULL) && (MbpDataHobLen > 0)) {
      MbpDataHob = (ME_BIOS_PAYLOAD *)(DataPtr + 4);
      if (MbpDataHob->FwFeaturesState.Available == 1 ) {
        *EomState = (UINT8)((MbpDataHob->FwFeaturesState.FwFeatures.Data & BIT4) >> 4);
        return EFI_SUCCESS;
      }
    }
  }

  //
  // Fill the FW state data if MBP doesn't exist
  //
  Status = HeciReadFwStatus( &FwSts);
  if (Status == EFI_SUCCESS ) {
    *EomState = (UINT8)((FwSts & BIT4) >> 4);
   }
  return Status;
}

/**
Get Sec FW Version.
@param[in]: SecVersion Pointer to  Sec FW Version structure.
@return EFI_SUCCESS.
@return EFI_ERROR.

**/
EFI_STATUS
GetSecFwVersion (
    SEC_VERSION_INFO *SecVersion
    )
{
  EFI_STATUS            Status;
  VOID                  *FspHobListPtr;
  ME_BIOS_PAYLOAD       *MbpDataHob;
  UINT32                MbpDataHobLen;
  UINT8                 *DataPtr;
  GEN_GET_FW_VER_ACK    MsgGenGetFwVersionAckData;

  if(SecVersion == NULL) {
    DEBUG ((DEBUG_ERROR, "GetSecFwVersion Failed Status=0x%x\n",EFI_INVALID_PARAMETER));
    return EFI_INVALID_PARAMETER;
  }

  //
  // Fill the FW version data from MBP if it exists
  //
  FspHobListPtr = GetFspHobListPtr();
  if (FspHobListPtr != NULL) {
    DataPtr = (UINT8 *)GetGuidHobData (FspHobListPtr, &MbpDataHobLen, &gMeBiosPayloadHobGuid);
    if ((DataPtr != NULL) && (MbpDataHobLen > 0)) {
      MbpDataHob = (ME_BIOS_PAYLOAD *)(DataPtr + 4);
      SecVersion->CodeMajor = MbpDataHob->FwVersionName.MajorVersion;
      SecVersion->CodeMinor = MbpDataHob->FwVersionName.MinorVersion;
      SecVersion->CodeHotFix = MbpDataHob->FwVersionName.HotfixVersion;
      SecVersion->CodeBuildNo = MbpDataHob->FwVersionName.BuildVersion;
      return EFI_SUCCESS;
    }
  }

  //
  // Fill the FW version data if MBP doesn't exist
  //
  Status = HeciGetFwVersionMsg( (UINT8 *)&MsgGenGetFwVersionAckData);
  if (Status == EFI_SUCCESS) {
    SecVersion->CodeMajor = MsgGenGetFwVersionAckData.Data.CodeMajor;
    SecVersion->CodeMinor = MsgGenGetFwVersionAckData.Data.CodeMinor;
    SecVersion->CodeHotFix = MsgGenGetFwVersionAckData.Data.CodeHotFix;
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
  EFI_STATUS                      Status;
  GEN_GET_FW_CAPS_SKU_ACK         MsgGenGetFwCapsSkuAck;
  VOID                           *FspHobListPtr;
  ME_BIOS_PAYLOAD                *MbpDataHob;
  UINT32                          MbpDataHobLen;
  UINT8                          *DataPtr;

  if(SecCapability == NULL) {
    DEBUG ((DEBUG_ERROR, "GetSecCapability(): Failed Status=0x%x\n", EFI_INVALID_PARAMETER));
    return EFI_INVALID_PARAMETER;
  }

  //
  // Fill the FW capapbility data from MBP if it exists
  // FIXME: Need to move MBP read to a different place.
  // It cannot be moved to HeciLib (SblOpen) as MBP is
  // specific to platform.
  //
  FspHobListPtr = GetFspHobListPtr();
  if (FspHobListPtr != NULL) {
    DataPtr = (UINT8 *)GetGuidHobData (FspHobListPtr, &MbpDataHobLen, &gMeBiosPayloadHobGuid);
    if ((DataPtr != NULL) && (MbpDataHobLen > 0)) {
      MbpDataHob = (ME_BIOS_PAYLOAD *)(DataPtr + 4);
      if (MbpDataHob->FwCapsSku.Available == 1 ) {
        *SecCapability = MbpDataHob->FwCapsSku.FwCapabilities.Data;
        return EFI_SUCCESS;
      }
    }
  }

  //
  // Fill the FW capability data if MBP doesn't exist
  //
  Status = HeciGetFwCapsSkuMsg ((UINT8 *)&MsgGenGetFwCapsSkuAck);
  if (EFI_ERROR(Status)) {
    DEBUG ((DEBUG_ERROR, "GetSecCapability(): HeciGetFwCapsSkyMsg(): Failed Status=0x%x\n", EFI_NO_RESPONSE));
    return Status;
  }
  *SecCapability = MsgGenGetFwCapsSkuAck.Data.FWCap.Data;

  return EFI_SUCCESS;
}


/**
Get TME status.
@param[in] Tme status pointer.
@retval EFI_SUCCESS Success get all FW hash value
@retval EFI_ERROR Unable to get hash value

**/
EFI_STATUS
GetTmeStatus (
  UINT32    *TmeStatus
  )
{

  EFI_CPUID_REGISTER            Cpuid = { 0, 0, 0, 0 };
  UINT64                        MsrTmeAct = 0x0;
  UINT64                        MsrTmeCap = 0x0;
  PSD_TME_CAPS                  TmeCaps;

  ///
  /// Processor TME Capability by reading CPUID.(EAX=7,ECX=0):ECX[13]
  ///
  AsmCpuidEx (
      CPUID_STRUCTURED_EXTENDED_FEATURE_FLAGS,
      0,
      &Cpuid.RegEax,
      &Cpuid.RegEbx,
      &Cpuid.RegEcx,
      &Cpuid.RegEdx
      );

  /// CPUID.(EAX=7,ECX=0):ECX[13]=1 will indicate presence of
  /// TME_CAPABILITY MSR (981H)
  /// TME_ACTIVATE MSR (982H)
  if (!(Cpuid.RegEcx & BIT13)) {
    DEBUG ((DEBUG_INFO, "PSD_DEBUG TME is not supported.\n"));
    *TmeStatus = 0;
    return EFI_UNSUPPORTED;
  }

  MsrTmeAct = AsmReadMsr64 (MSR_TME_ACTIVATE);
  MsrTmeCap = AsmReadMsr64 (MSR_TME_CAPABILITIES);
  DEBUG ((DEBUG_INFO, "PSD_DEBUG TME is supported.\n"));

  // TmeStatus Bit[0] TME Enabled = MSR_TME_ACTIVATE (982H):Bit1 = 1 and
  // MSR_TME_ACTIVATE (982H):Bit31 = 0 (TME Encryption Bypass Enable)
  if ((!(MsrTmeAct & BIT1)) || (MsrTmeAct & BIT31)) {
    DEBUG ((DEBUG_INFO, "PSD_DEBUG TME is not enabled.\n"));
    *TmeStatus = 0;
    return EFI_SUCCESS;
  }

  TmeCaps.Bits.Enabled = 1;

  // TmeStatus Bit[1] TME-MT Enabled = MSR_TME_ACTIVATE (982H):Bit1 = 1 and
  // MSR_TME_CAPABILITIES : MKTME_MAX_KEYID_BITS (BIT[35:32] != 0)
  if ((MsrTmeCap & BIT32)|| (MsrTmeCap & BIT33)||
    (MsrTmeCap & BIT34)|| (MsrTmeCap & BIT35)) {
    DEBUG ((DEBUG_INFO, "PSD_DEBUG TME MT is enabled.\n"));
    TmeCaps.Bits.TmeMtEnabled = 1;
  }

  // TmeStatus Bit[2:17] TME-MT Max Keys
  // MSR_TME_CAPABILITIES : MKTME_MAX_KEYS (BIT[50:36])
  TmeCaps.Bits.TmeMtMaxKeys = (MsrTmeCap & TME_MT_MAX_KEY_MASK) >> TME_MT_MAX_KEY_IDX;
  DEBUG ((DEBUG_INFO, "PSD_DEBUG TME MT MAX Keys = %x\n", TmeCaps.Bits.TmeMtMaxKeys));

  // TmeStatus Bit[18:25] Encryption Algorithm
  // TmeStatus Bit[18] AES-XTS 128bit
  // MSR_TME_CAPABILITIES : BIT0 AES-XTS 128-bit encryption algorithm support.
  // MSR_TME_ACTIVATE : MKTME_MAX_KEYS (BIT[7:4] = 0000)
  if ((MsrTmeCap & BIT0) && (!(MsrTmeAct & BIT7)) && (!(MsrTmeAct & BIT6)) &&
    (!(MsrTmeAct & BIT5)) && (!(MsrTmeAct & BIT4))) {
    DEBUG ((DEBUG_INFO, "PSD_DEBUG TME AES-XTS 128bit is enabled.\n"));
    TmeCaps.Tme32 = TmeCaps.Tme32 | BIT18;
  }

  // TmeStatus Bit[19] AES-XTS 256bit
  // MSR_TME_CAPABILITIES : BIT2 AES-XTS 256-bit encryption algorithm support.
  // MSR_TME_ACTIVATE : MKTME_MAX_KEYS (BIT[7:4] = 0010)
  if ((MsrTmeCap & BIT2) && (!(MsrTmeAct & BIT7)) && (!(MsrTmeAct & BIT6)) &&
    (MsrTmeAct & BIT5) && (!(MsrTmeAct & BIT4))) {
    DEBUG ((DEBUG_INFO, "PSD_DEBUG TME AES-XTS 128bit is enabled.\n"));
    TmeCaps.Tme32 = TmeCaps.Tme32 | BIT19;
  }

  // TmeStatus Bit[26:31] Reserved
  TmeCaps.Bits.Reserved = 0;

  *TmeStatus =  TmeCaps.Tme32;
  DEBUG ((DEBUG_INFO, "PSD_DEBUG Tme Status in PSD=0x%x\n",TmeCaps.Tme32));

  return EFI_SUCCESS;
}

/**
Get SGX Capabilities
@param[in] SgxCapabilies Pointer to Sec Caps.
@retval EFI_SUCCESS Success get all FW hash value
@retval EFI_ERROR Unable to get cababilites.

 **/
EFI_STATUS
GetSgxCapabilities (
  UINT32    *SgxCapabilities
  )
{

  EFI_CPUID_REGISTER      Cpuid = { 0, 0, 0, 0 };
  SGX_CFG_DATA            *SgxCfgData;
  PSD_SGX_CAPS            SgxCaps;

  SgxCaps.Sgx32 = 0;
  ///
  /// Processor support SGX feature by reading CPUID.(EAX=7,ECX=0):EBX[2]
  /// with CPUID.(EAX=7,ECX=0):EBX[2]=1
  /// PRMRR configuration enabled, MSR IA32_MTRRCAP (FEh) [12] == 1
  ///
  AsmCpuidEx (
    CPUID_STRUCTURED_EXTENDED_FEATURE_FLAGS,
    0,
    &Cpuid.RegEax,
    &Cpuid.RegEbx,
    &Cpuid.RegEcx,
    &Cpuid.RegEdx
  );

  SgxCfgData = (SGX_CFG_DATA *)FindConfigDataByTag (CDATA_SGX_TAG);
  if (SgxCfgData == NULL) {
    *SgxCapabilities = 0;
    return EFI_SUCCESS;
  }

  if ((Cpuid.RegEbx & BIT2) && SgxCfgData->EnableSgx ) {
    DEBUG ((DEBUG_INFO, " PSD_DEBUG SGX is enabled and supported\n"));
    SgxCaps.Bits.Enabled = 1;
    DEBUG ((DEBUG_INFO, "SgxCfgData->PrmrrSize=0x%x\n",SgxCfgData->PrmrrSize));

    switch((UINT64)(SgxCfgData->PrmrrSize))
    {
      case 0x80000: //512K
        SgxCaps.Bits.PRMRR =0x01;
        break;
      case 0x100000: //1M
        SgxCaps.Bits.PRMRR =0x02;
        break;
      case 0x200000: //2M
        SgxCaps.Bits.PRMRR =0x03;
        break;
      case 0x2000000: //32M
        SgxCaps.Bits.PRMRR =0x04;
        break;
      case 0x4000000: //64M
        SgxCaps.Bits.PRMRR =0x05;
        break;
      case 0x8000000: //128M
        SgxCaps.Bits.PRMRR =0x06;
        break;
      case 0x10000000: //256M
        SgxCaps.Bits.PRMRR =0x07;
        break;
      case 0x20000000: //512M
        SgxCaps.Bits.PRMRR =0x08;
        break;
      case 0x40000000: //1G
        SgxCaps.Bits.PRMRR =0x09;
        break;
      case 0x80000000: //2G
        SgxCaps.Bits.PRMRR =0x0a;
        break;
      case 0x100000000: //4G
        SgxCaps.Bits.PRMRR =0x0b;
        break;
      case 0x200000000: //8G
        SgxCaps.Bits.PRMRR =0x0c;
        break;
      case 0x800000000: //64G
        SgxCaps.Bits.PRMRR =0x0d;
        break;
      case 0x1000000000: //128G
        SgxCaps.Bits.PRMRR =0x0e;
        break;
      case 0x2000000000: //256G
        SgxCaps.Bits.PRMRR =0x0f;
        break;
      case 0x4000000000: //512G
        SgxCaps.Bits.PRMRR =0x10;
      default:
        SgxCaps.Bits.PRMRR =0x00;
        break;
    }
    SgxCaps.Bits.PRMRR = SgxCaps.Bits.PRMRR << 1;
    SgxCaps.Bits.SgxQos = SgxCfgData->SgxQos;
    SgxCaps.Bits.SgxAutoMpRegistration = SgxCfgData->SgxAutoRegistration;
    SgxCaps.Bits.SgxDebugMode = SgxCfgData->SgxDebugMode;
  } else {
    //SGX config is null. setting PRMRR into SGX caps to default = 0.
    *SgxCapabilities = 0;
    return EFI_SUCCESS;
  }

  //Check  Flexible launch control.
  if ((Cpuid.RegEcx & BIT30) && (AsmReadMsr64 (MSR_IA32_FEATURE_CONTROL) & BIT17)) {
    DEBUG ((DEBUG_INFO, "PSD_DEBUG SGX Flexible Launch Control is supported and enabled.\n"));
    SgxCaps.Bits.FlcEnabled = 1;
  } else {
    SgxCaps.Bits.FlcEnabled = 0;
    DEBUG ((DEBUG_INFO, "PSD_DEBUG SGX Flexible Launch Control is not supported or disabled.\n"));
  }

  *SgxCapabilities =  SgxCaps.Sgx32;
  DEBUG ((DEBUG_INFO, "PSD_DEBUG SGX Fields in PSD=0x%x\n",SgxCaps.Sgx32));

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
  EFI_ACPI_PSD_TABLE             *Psdt;
  PLATFORM_DATA                   *PlatformData;
  EFI_STATUS                      Status;
  BOOLEAN                         IsIgnitionFw;

  DEBUG((DEBUG_VERBOSE, "UpdateAcpiPsdTable start\n"));
  if ( Table == NULL) {
    DEBUG((DEBUG_WARN, "EFI_ACPI_PSD_TABLE IS NULL\n"));
    return EFI_INVALID_PARAMETER;
  }
  Psdt = (EFI_ACPI_PSD_TABLE*)Table;
  // Populate Platfrom security capabilities in table structure
  Psdt->Header.Signature               = EFI_ACPI_PSD_SIGNATURE;
  Psdt->Header.Checksum                = 0;

  CopyMem(&Psdt->Header.OemId, PSDS_EFI_ACPI_OEM_ID, sizeof(Psdt->Header.OemId));
  Psdt->Header.OemTableId              = PSDS_EFI_ACPI_OEM_TABLE_ID;
  Psdt->Header.OemRevision             = PSDS_EFI_ACPI_OEM_REVISION;
  Psdt->Header.CreatorId               = PSDS_EFI_ACPI_CREATOR_ID;
  Psdt->Header.CreatorRevision         = PSDS_EFI_ACPI_CREATOR_REVISION;

  Psdt->PsdVersion.PsdVerMajor = PSD_VERSION_MAJOR;
  Psdt->PsdVersion.PsdVerMinor = PSD_VERSION_MINOR;

  IsIgnitionFw = MeTypeIsSpsIgnition();

  //Eom State,
  Psdt->EomState = 0xFF;
  if (IsIgnitionFw == FALSE) {
    Status = GetEomState(&Psdt->EomState);
    if (EFI_ERROR(Status)) {
      DEBUG((DEBUG_ERROR, " GetEomState failed =%x\n", Status));
    }
  }

  //Sec Capabilities,
  Psdt->CsmeSecCapabilities = 0xFFFFFFFF;
  if (IsIgnitionFw == FALSE) {
    Status = GetSecCapability( &(Psdt->CsmeSecCapabilities) );
    if (EFI_ERROR(Status)) {
      DEBUG((DEBUG_ERROR, " GetSecCapability failed =%x\n",Status));
    }
  }

  //SGX Capabilities
  Status = GetSgxCapabilities( &(Psdt->SgxCapabilities) );
  if (EFI_ERROR(Status)) {
    DEBUG((DEBUG_ERROR, " GetSgxCapabilities failed =%x\n",Status));
  }

  //FW version,
  if (IsIgnitionFw == TRUE)
  {
    Psdt->FwVer.CodeBuildNo = 0xFFFFFFFF;
    Psdt->FwVer.CodeHotFix = 0xFFFFFFFF;
    Psdt->FwVer.CodeMajor = 0xFFFFFFFF;
    Psdt->FwVer.CodeMinor = 0xFFFFFFFF;
  } else {
    Status = GetSecFwVersion( &(Psdt->FwVer) );
    if (EFI_ERROR(Status)) {
      DEBUG((DEBUG_ERROR, " GetSecCFwVersion failed =%x\n",Status));
    }
  }

  CopyMem(&Psdt->FwVendor, EFI_ACPI_PSD_FW_VENDOR, EFI_ACPI_PSD_FW_VENDOR_SIZE);
  PlatformData = (PLATFORM_DATA *)GetPlatformDataPtr();
  if (PlatformData == NULL) {
    DEBUG(( DEBUG_ERROR, "PSD Values:  GetPlatformDataPtr Failed\n" ));
    return  EFI_UNSUPPORTED;
  }

  //BIT0: UEFI Secure boot is enabled, BIT1: Boot Guard is Enabled, BIT2: Bootloader Verified boot is Enabled, BIT0-2 can be combined.
  Psdt->SecureBoot = (UINT8)(((PlatformData->BtGuardInfo.VerifiedBoot) << 1)| (FeaturePcdGet (PcdVerifiedBootEnabled)) << 2);
  //Measured boot enabled.
  Psdt->MeasuredBoot = (UINT8)((PlatformData->BtGuardInfo.MeasuredBoot));

  //0 - No HWRoT; 1 - ROM based RoT; 2 - TXE; 3 - CSE; 4 - ACM; 5 - TXT
  Psdt->HwrotType                      = PSD_HROT_ACM;
  DumpHex (2, 0, sizeof(EFI_ACPI_PSD_TABLE), (VOID *)Table);
  DEBUG( (DEBUG_VERBOSE, "UpdateAcpiPsdTable() end\n") );

  //TBD : TME Status
  Status = GetTmeStatus(&(Psdt->TmeStatus));
  if (EFI_ERROR(Status)) {
    DEBUG((DEBUG_ERROR, " GetTmeStatus failed =%x\n",Status));
  }

  return  EFI_SUCCESS;
}
