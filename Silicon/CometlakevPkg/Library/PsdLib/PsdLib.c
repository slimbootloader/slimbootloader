/** @file

  Copyright (c) 2018 - 2019, Intel Corporation. All rights reserved.<BR>
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
#include <Library/HeciLib/CseMsg.h>
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
#include <Library/SgxLib.h>
#include <RegAccess.h>
#include <Library/ConfigDataLib.h>
#include <ConfigDataStruct.h>


#define PSD_VERSION_MAJOR                               0x0000
#define PSD_VERSION_MINOR                               0x0003
#define CPUID_STRUCTURED_EXTENDED_FEATURE_FLAGS         0x7
#define CPUID_SGX_ENABLED                               0x12
#define PSD_HROT_NONE                                   0
#define PSD_HROT_ROM                                    1
#define PSD_HROT_TXE                                    2
#define PSD_HROT_CSE                                    3
#define PSD_HROT_ACM                                    4
#define PSD_HROT_TXT                                    5




/**
Get SGX Capabilities of IA core.
@param[in] SgxCapabilies Pointer to Sec Caps.
@retval EFI_SUCCESS Success get all FW hash value
@retval EFI_ERROR Unable to get cababilites.

 **/
EFI_STATUS
GetSgxCapabilities(
    UINT16    *SgxCapabilities
    )
{

  EFI_CPUID_REGISTER            Cpuid = { 0, 0, 0, 0 };
  SGX_CFG_DATA                  *SgxCfgData;
  PSD_SGX_CAPS                  SgxCaps;
  UINT64                        Msr = 0x0;
  SgxCaps.Sgx16                  = 0;
  ///
  /// Verify processor supports SGX Flexible Launch Control
  ///
  AsmCpuidEx (
      CPUID_STRUCTURED_EXTENDED_FEATURE_FLAGS,
      0,
      &Cpuid.RegEax,
      &Cpuid.RegEbx,
      &Cpuid.RegEcx,
      &Cpuid.RegEdx
      );

  ///
  /// Processor support SGX feature by reading CPUID.(EAX=7,ECX=0):EBX[2]
  ///

  ///
  /// SGX feature is supported only on SKL and later,
  /// with CPUID.(EAX=7,ECX=0):EBX[2]=1
  /// PRMRR configuration enabled, MSR IA32_MTRRCAP (FEh) [12] == 1
  ///
  if ((Cpuid.RegEbx & BIT2) && (AsmReadMsr64 (MSR_IA32_MTRRCAP) & BIT12)) {
    DEBUG ((DEBUG_INFO, "PSD_DEBUG SGX bit set in PSD=0x%x\n",(Cpuid.RegEbx & BIT2) && (AsmReadMsr64 (MSR_IA32_MTRRCAP) & BIT12) ));
    SgxCaps.Bits.Enabled = 1;
  }

  //Check  Flexible launch control.
  if (Cpuid.RegEcx & BIT30) {
    DEBUG ((DEBUG_INFO, "PSD_DEBUG SGX Flexible Launch Control is supported.\n"));
    if(AsmReadMsr64 (MSR_IA32_FEATURE_CONTROL) & BIT17) {
      DEBUG ((DEBUG_INFO, "PSD_DEBUG SGX Flexible Launch Control is enabled.\n"));
      SgxCaps.Bits.FlcEnabled = 1;
    }
  }
  else {
    SgxCaps.Bits.FlcEnabled = 0;
    DEBUG ((DEBUG_INFO, "PSD_DEBUG SGX Flexible Launch Control is not supported.\n"));
  }

  SgxCfgData = (SGX_CFG_DATA *)FindConfigDataByTag (CDATA_SGX_TAG);
  if (SgxCfgData != NULL) {
    DEBUG ((DEBUG_INFO, " PSD_DEBUG SgxCfgData->EnableSgx=0x%x\n",SgxCfgData->EnableSgx));
    if(SgxCfgData->EnableSgx == 2) {
      DEBUG ((DEBUG_INFO, " PSD_DEBUG Sgx SwCrtl enabled\n"));
      SgxCaps.Bits.SwCrtl = 1;
    }
    DEBUG ((DEBUG_INFO, "SgxCfgData->PrmrrSize=0x%x\n",SgxCfgData->PrmrrSize));
    if ((AsmReadMsr64 (MSR_PRMRR_PHYS_BASE) != 0) && ((AsmReadMsr64 (MSR_PRMRR_PHYS_MASK) & BIT10) != 0)) {
      Msr = AsmReadMsr64(MSR_PRMRR_PHYS_MASK);
      //DEBUG ((DEBUG_INFO, "Sgx 64bit MSR_PRMRR_PHYS_MASK= 0x%Lx\n",Msr));
      DEBUG ((DEBUG_INFO, "Sgx size64 MSR_PRMRR_PHYS_MASK= 0x%Lx\n",(Msr)&0x3FFFFFFFF000));
      //Following code is only to debug SGX size decoding logic.
      //DEBUG ((DEBUG_INFO, "Sgx PRMRR MASK calculated from Size = 0x%x\n", (0x0000007FFFFFFFFF &(~((UINT64)(SgxCfgData->PrmrrSize - 1))))));
    }
    //read bits [45:12] this will give the mask and [39:12] will be used for size.
    switch((UINT64)(Msr&0x3FFFFFFFF000))
    {
      case 0x7FFFF00000: //1M
        SgxCaps.Bits.PRMRR =0x3;
        break;
      case 0x7FFFE00000: //2M
        SgxCaps.Bits.PRMRR =0x4;
        break;
      case 0x7FFE000000: //32M
        SgxCaps.Bits.PRMRR =0x5;
        break;
      case 0x7FFC000000: //64M
        SgxCaps.Bits.PRMRR =0x6;
        break;
      case 0X7FF8000000: //128M
        SgxCaps.Bits.PRMRR =0x7;
        break;
      case 0x7FF0000000: //256M
        SgxCaps.Bits.PRMRR =0x8;
        break;
      case 0x7FE0000000: //512M
        SgxCaps.Bits.PRMRR =0x9;
        break;
      default:
        SgxCaps.Bits.PRMRR =0x0;
        break;
    }
  }
  else {
    //SGX config is null. setting PRMRR into SGX caps to default = 0.
    SgxCaps.Bits.PRMRR =0x0;
  }

  *SgxCapabilities =  SgxCaps.Sgx16;
  DEBUG ((DEBUG_INFO, "PSD_DEBUG SGX Fields in PSD=0x%x\n",SgxCaps.Sgx16));

  return EFI_SUCCESS;
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
  UINT32                FwSts;

  if(EomState == NULL) {
    DEBUG ((DEBUG_ERROR, "EomState is not valid pointer\n"));
    return EFI_INVALID_PARAMETER;
  }
  Status = ReadHeciFwStatus( &FwSts);
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
  GEN_GET_FW_VER_ACK    MsgGenGetFwVersionAckData;
  DEBUG ((DEBUG_ERROR, "GetSecFwVersion called\n"));

  if(SecVersion == NULL) {
    DEBUG ((DEBUG_ERROR, "GetSecFwVersion Failed Status=0x%x\n",EFI_INVALID_PARAMETER));
    return EFI_INVALID_PARAMETER;
  }

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
  EFI_STATUS               Status;
  GEN_GET_FW_CAPSKU        MsgGenGetFwCapsSku;
  GEN_GET_FW_CAPS_SKU_ACK  MsgGenGetFwCapsSkuAck;
  if(SecCapability == NULL) {
    DEBUG ((DEBUG_ERROR, "GetSecCapability Failed Status=0x%x\n",EFI_INVALID_PARAMETER));
    return EFI_INVALID_PARAMETER;
  }


  Status = HeciGetFwCapsSkuMsg ( (UINT8 *)&MsgGenGetFwCapsSku, (UINT8 *)&MsgGenGetFwCapsSkuAck);
  if (EFI_ERROR(Status)) {
    return Status;
  }
  *SecCapability = MsgGenGetFwCapsSkuAck.Data.FWCap.Data;
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
  EFI_STATUS                      Status;

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
  CopyMem(&mPsdt->Header.OemId, PSDS_EFI_ACPI_OEM_ID, 6);
  mPsdt->Header.OemTableId              = PSDS_EFI_ACPI_OEM_TABLE_ID;
  mPsdt->Header.OemRevision             = PSDS_EFI_ACPI_OEM_REVISION;
  mPsdt->Header.CreatorId               = PSDS_EFI_ACPI_CREATOR_ID;
  mPsdt->Header.CreatorRevision         = PSDS_EFI_ACPI_CREATOR_REVISION;
  ZeroMem( mPsdt +  sizeof (EFI_ACPI_DESCRIPTION_HEADER) , \
     sizeof(EFI_ACPI_PSD_TABLE) - sizeof(EFI_ACPI_DESCRIPTION_HEADER) );
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
    DEBUG((DEBUG_ERROR, " GetEomState failed =%x\n",Status));
  }
  DEBUG( (DEBUG_INFO, "PSD Values:  EomState =%x\n", mPsdt->EomState ));

  //Sec Capabilities,
  Status = GetSecCapability( &(mPsdt->CsmeSecCapabilities) );
  if (EFI_ERROR(Status)) {
    DEBUG((DEBUG_ERROR, " GetSecCapability failed =%x\n",Status));
  }
  DEBUG((DEBUG_INFO, "PSD Values: CsmeSecCapabilities=%x\n", mPsdt->CsmeSecCapabilities));

  //SGX Capabilities
  GetSgxCapabilities(&(mPsdt->SgxCapabilities));

  //FW version,
  Status = GetSecFwVersion( &(mPsdt->FwVer) );
  if (EFI_ERROR(Status)) {
    DEBUG((DEBUG_ERROR, " GetSecCFwVersion failed =%x\n",Status));
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
    DEBUG(( DEBUG_ERROR, "PSD Values:  GetPlatformDataPtr Failed\n" ));
    return  EFI_UNSUPPORTED;
  }

  //000 - Secure boot is Disabled; 010 - Boot Guard Enabled; 100 - Bootloader Verified boot Enabled.
  mPsdt->SecureBoot = (UINT8)(((PlatformData->BtGuardInfo.VerifiedBoot) << 1)| (FeaturePcdGet (PcdVerifiedBootEnabled)) << 2);
  //Measured boot enabled.
  mPsdt->MeasuredBoot = (UINT8)((PlatformData->BtGuardInfo.MeasuredBoot));

  //0 - No HWRoT; 1 - ROM based RoT; 2 - TXE; 3 - CSE; 4 - ACM; 5 - TXT
  mPsdt->HwrotType                      = PSD_HROT_ACM;
  DEBUG((DEBUG_INFO, "PSD Values:  SecureBootEnabled=%x\n", mPsdt->SecureBoot));
  DEBUG((DEBUG_INFO, "PSD Values:  MeasuredBootEnabled=%x\n", mPsdt->MeasuredBoot));
  DEBUG((DEBUG_INFO, "PSD Values:  HwrotType=%x\n", mPsdt->HwrotType));
  DumpHex (2, 0, sizeof(EFI_ACPI_PSD_TABLE), (VOID *)Table);
  DEBUG( (DEBUG_INFO, "UpdateAcpiPsdTable() end\n") );

  return  EFI_SUCCESS;
}
