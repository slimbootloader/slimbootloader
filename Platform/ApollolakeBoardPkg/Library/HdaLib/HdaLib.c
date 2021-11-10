/** @file

  Copyright (c) 2014 - 2019, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <Library/HdaLib.h>
#include <Library/DebugLib.h>
#include <Library/BaseMemoryLib.h>

#define EFI_ACPI_OEM_ID                   "INTEL\0"  // OEMID 6 bytes long
#define EFI_ACPI_OEM_TABLE_ID             SIGNATURE_64('N','H','L','T',' ',' ',' ',' ') // OEM table id 8 bytes long
#define EFI_ACPI_OEM_REVISION             0x00000002
#define EFI_ACPI_CREATOR_ID               SIGNATURE_32('I','N','T','L')
#define EFI_ACPI_CREATOR_REVISION         0x01000013

/**
  Returns pointer to Endpoint ENDPOINT_DESCRIPTOR structure.

  @param[in] NhltTable     Endpoint for which Format address is retrieved
  @param[in] FormatIndex   Index of Format to be retrieved

  @retval                  Pointer to ENDPOINT_DESCRIPTOR structure with given index

**/
ENDPOINT_DESCRIPTOR *
GetNhltEndpoint (
  IN CONST NHLT_ACPI_TABLE      *NhltTable,
  IN CONST UINT8                EndpointIndex
  )
{
  UINT8                i;
  ENDPOINT_DESCRIPTOR  *Endpoint;

  Endpoint = (ENDPOINT_DESCRIPTOR *) (NhltTable->EndpointDescriptors);

  if (EndpointIndex > NhltTable->EndpointCount) {
    return NULL;
  }

  for (i = 0; i < EndpointIndex; i++) {
    Endpoint = (ENDPOINT_DESCRIPTOR*) ((UINT8 *) (Endpoint) + Endpoint->EndpointDescriptorLength);
  }

  return Endpoint;
}

/**
  Returns pointer to Endpoint Specific Configuration SPECIFIC_CONFIG structure.

  @param[in]  Endpoint     Endpoint for which config address is retrieved

  @retval                  Pointer to SPECIFIC_CONFIG structure with endpoint's capabilities

**/
SPECIFIC_CONFIG *
GetNhltEndpointDeviceCapabilities (
  IN CONST ENDPOINT_DESCRIPTOR  *Endpoint
  )
{
  return (SPECIFIC_CONFIG *) (&Endpoint->EndpointConfig);
}

/**
  Returns pointer to all Formats Configuration FORMATS_CONFIG structure.

  @param[in]  Endpoint     Endpoint for which Formats address is retrieved

  @retval                  Pointer to FORMATS_CONFIG structure

**/
FORMATS_CONFIG *
GetNhltEndpointFormatsConfig (
  IN CONST ENDPOINT_DESCRIPTOR  *Endpoint
  )
{
  FORMATS_CONFIG *FormatsConfig;

  FormatsConfig = (FORMATS_CONFIG *) ((UINT8 *) (&Endpoint->EndpointConfig)
                + sizeof (Endpoint->EndpointConfig.CapabilitiesSize)
                + Endpoint->EndpointConfig.CapabilitiesSize);

  return FormatsConfig;
}

/**
  Returns pointer to Format Configuration FORMAT_CONFIG structure.

  @param[in] Endpoint      Endpoint for which Format address is retrieved
  @param[in] FormatIndex   Index of Format to be retrieved

  @retval                  Pointer to FORMAT_CONFIG structure with given index

**/
FORMAT_CONFIG *
GetNhltEndpointFormat (
  IN CONST ENDPOINT_DESCRIPTOR  *Endpoint,
  IN CONST UINT8                FormatIndex
  )
{
  UINT8           i;
  UINT32          Length;
  FORMATS_CONFIG  *FormatsConfig;
  FORMAT_CONFIG   *Format;

  Length = 0;
  FormatsConfig = GetNhltEndpointFormatsConfig (Endpoint);
  Format = FormatsConfig->FormatsConfiguration;

  if (FormatIndex > FormatsConfig->FormatsCount) {
    return NULL;
  }

  for (i = 0; i < FormatIndex; i++) {
    Length = sizeof (Format->Format) + Format->FormatConfiguration.CapabilitiesSize
           + sizeof (Format->FormatConfiguration.CapabilitiesSize);
    Format = (FORMAT_CONFIG *) ((UINT8 *) (Format) + Length);
  }

  return Format;
}

/**
  Returns pointer to OED Configuration SPECIFIC_CONFIG structure.

  @param[in]  NhltTable    NHLT table for which OED address is retrieved

  @retval                  Pointer to SPECIFIC_CONFIG structure with NHLT capabilities

**/
SPECIFIC_CONFIG *
GetNhltOedConfig (
  IN CONST NHLT_ACPI_TABLE      *NhltTable
  )
{
  ENDPOINT_DESCRIPTOR  *Endpoint;
  SPECIFIC_CONFIG      *OedConfig;

  Endpoint = GetNhltEndpoint (NhltTable, (NhltTable->EndpointCount));
  OedConfig = (SPECIFIC_CONFIG *) ((UINT8 *) (Endpoint));

  return OedConfig;
}

/**
  Prints Format configuration.

  @param[in] *Format       Format to be printed

  @retval     None

**/
VOID
NhltFormatDump (
  IN CONST FORMAT_CONFIG        *Format
  )
{
  DEBUG_CODE_BEGIN ();

  UINT32  i;

  DEBUG ((DEBUG_VERBOSE, "------------------------------- FORMAT -------------------------------\n"));
  DEBUG ((DEBUG_VERBOSE, " Format->Format.Format.wFormatTag      = 0x%x\n", Format->Format.Format.wFormatTag));
  DEBUG ((DEBUG_VERBOSE, " Format->Format.Format.nChannels       = %d\n", Format->Format.Format.nChannels));
  DEBUG ((DEBUG_VERBOSE, " Format->Format.Format.nSamplesPerSec  = %d\n", Format->Format.Format.nSamplesPerSec));
  DEBUG ((DEBUG_VERBOSE, " Format->Format.Format.nAvgBytesPerSec = %d\n", Format->Format.Format.nAvgBytesPerSec));
  DEBUG ((DEBUG_VERBOSE, " Format->Format.Format.nBlockAlign     = %d\n", Format->Format.Format.nBlockAlign));
  DEBUG ((DEBUG_VERBOSE, " Format->Format.Format.wBitsPerSample  = %d\n", Format->Format.Format.wBitsPerSample));
  DEBUG ((DEBUG_VERBOSE, " Format->Format.Format.cbSize          = %d\n", Format->Format.Format.cbSize));
  DEBUG ((DEBUG_VERBOSE, " Format->Format.Samples                = %d\n", Format->Format.Samples));
  DEBUG ((DEBUG_VERBOSE, " Format->Format.dwChannelMask          = 0x%x\n", Format->Format.dwChannelMask));
  DEBUG ((DEBUG_VERBOSE, " Format->Format.SubFormat              = %g\n", Format->Format.SubFormat));

  DEBUG ((DEBUG_VERBOSE, " Format->FormatConfiguration.CapabilitiesSize = %d B\n", Format->FormatConfiguration.CapabilitiesSize));
  DEBUG ((DEBUG_VERBOSE, " Format->FormatConfiguration.Capabilities:"));
  for (i = 0; i < (  Format->FormatConfiguration.CapabilitiesSize ) ; i++) {
    if(i % 16 == 0) {
      DEBUG ((DEBUG_VERBOSE, "\n"));
    }
    DEBUG ((DEBUG_VERBOSE, "0x%02x, ", Format->FormatConfiguration.Capabilities[i]));
  }
  DEBUG ((DEBUG_VERBOSE, "\n"));

  DEBUG_CODE_END ();
}

/**
  Prints Endpoint configuration.

  @param[in]  Endpoint     Endpoint to be printed

  @retval     None

**/
VOID
NhltEndpointDump (
  IN CONST ENDPOINT_DESCRIPTOR  *Endpoint
  )
{
  DEBUG_CODE_BEGIN ();

  UINT8           i;
  FORMATS_CONFIG  *FormatsConfigs;
  FORMAT_CONFIG   *Format;

  DEBUG ((DEBUG_VERBOSE, "------------------------------ ENDPOINT ------------------------------\n"));
  DEBUG ((DEBUG_VERBOSE, " Endpoint->DeviceDescriptorLength = %d B\n", Endpoint->EndpointDescriptorLength));
  DEBUG ((DEBUG_VERBOSE, " Endpoint->LinkType               = 0x%x\n", Endpoint->LinkType));
  DEBUG ((DEBUG_VERBOSE, " Endpoint->InstanceId             = 0x%x\n", Endpoint->InstanceId));
  DEBUG ((DEBUG_VERBOSE, " Endpoint->HwVendorId             = 0x%x\n", Endpoint->HwVendorId));
  DEBUG ((DEBUG_VERBOSE, " Endpoint->HwDeviceId             = 0x%x\n", Endpoint->HwDeviceId));
  DEBUG ((DEBUG_VERBOSE, " Endpoint->HwRevisionId           = 0x%x\n", Endpoint->HwRevisionId));
  DEBUG ((DEBUG_VERBOSE, " Endpoint->HwSubsystemId          = 0x%x\n", Endpoint->HwSubsystemId));
  DEBUG ((DEBUG_VERBOSE, " Endpoint->DeviceType             = 0x%x\n", Endpoint->DeviceType));
  DEBUG ((DEBUG_VERBOSE, " Endpoint->Direction              = 0x%x\n", Endpoint->Direction));
  DEBUG ((DEBUG_VERBOSE, " Endpoint->VirtualBusId           = 0x%x\n", Endpoint->VirtualBusId));

  DEBUG ((DEBUG_VERBOSE, " Endpoint->EndpointConfig.CapabilitiesSize = %d B\n", Endpoint->EndpointConfig.CapabilitiesSize));
  DEBUG ((DEBUG_VERBOSE, " Endpoint->EndpointConfig.Capabilities:"));
  for (i = 0; i < (Endpoint->EndpointConfig.CapabilitiesSize ) ; i++) {
    if (i % 16 == 0) {
                DEBUG ((DEBUG_VERBOSE, "\n"));
        }
    DEBUG ((DEBUG_VERBOSE, "0x%02x, ", Endpoint->EndpointConfig.Capabilities[i]));
  }

  FormatsConfigs = GetNhltEndpointFormatsConfig(Endpoint);

  DEBUG ((DEBUG_VERBOSE, "\n"));
  DEBUG ((DEBUG_VERBOSE, " Endpoint->FormatsConfig.FormatsCount = %d\n", FormatsConfigs->FormatsCount));
  for (i = 0; i < FormatsConfigs->FormatsCount; i++) {
    Format = GetNhltEndpointFormat (Endpoint, i);
    NhltFormatDump (Format);
  }

  DEBUG ((DEBUG_VERBOSE, "\n"));

  DEBUG_CODE_END ();
}

/**
  Prints OED (Offload Engine Driver) configuration.

  @param[in]  OedConfig   OED to be printed

  @retval     None

**/
VOID
NhltOedConfigDump (
  IN CONST SPECIFIC_CONFIG      *OedConfig
  )
{
  DEBUG_CODE_BEGIN ();

  UINT8  i;

  DEBUG ((DEBUG_VERBOSE, "-------------------------- OED CONFIGURATION -------------------------\n"));
  DEBUG ((DEBUG_VERBOSE, " OedConfig->CapabilitiesSize = %d B\n", OedConfig->CapabilitiesSize));
  DEBUG ((DEBUG_VERBOSE, " OedConfig->Capabilities:"));
  for (i = 0; i < (OedConfig->CapabilitiesSize) ; i++) {
    if(i % 16 == 0) {
      DEBUG ((DEBUG_VERBOSE, "\n"));
    }
    DEBUG ((DEBUG_VERBOSE, "0x%02x, ", OedConfig->Capabilities[i]));
  }

  DEBUG ((DEBUG_VERBOSE, "\n"));

  DEBUG_CODE_END ();
}

/**
  Prints NHLT (Non HDA-Link Table) to be exposed via ACPI (aka. OED (Offload Engine Driver) Configuration Table).

  @param[in]  NhltTable    The NHLT table to print

  @retval     None

**/
VOID
NhltAcpiTableDump (
  IN NHLT_ACPI_TABLE            *NhltTable
  )
{
  DEBUG_CODE_BEGIN ();

  UINT8 i;

  DEBUG ((DEBUG_VERBOSE, "\n"));
  DEBUG ((DEBUG_VERBOSE, "--- NHLT ACPI Table Dump [OED (Offload Engine Driver) Configuration] ---\n"));

  DEBUG ((DEBUG_VERBOSE, "sizeof NHLT_ACPI_TABLE = %d B\n", sizeof (NHLT_ACPI_TABLE)));
  DEBUG ((DEBUG_VERBOSE, "sizeof EFI_ACPI_DESCRIPTION_HEADER = %d B\n", sizeof (EFI_ACPI_DESCRIPTION_HEADER)));
  DEBUG ((DEBUG_VERBOSE, "sizeof ENDPOINT_DESCRIPTOR = %d B\n", sizeof (ENDPOINT_DESCRIPTOR)));
  DEBUG ((DEBUG_VERBOSE, "sizeof SPECIFIC_CONFIG = %d B\n", sizeof (SPECIFIC_CONFIG)));
  DEBUG ((DEBUG_VERBOSE, "sizeof FORMATS_CONFIG = %d B\n", sizeof (FORMATS_CONFIG)));
  DEBUG ((DEBUG_VERBOSE, "sizeof FORMAT_CONFIG = %d B\n", sizeof (FORMAT_CONFIG)));
  DEBUG ((DEBUG_VERBOSE, "sizeof WAVEFORMATEXTENSIBLE = %d B\n", sizeof (WAVEFORMATEXTENSIBLE)));

  DEBUG ((DEBUG_VERBOSE, " NHLT_ACPI_TABLE Header.Signature       = 0x%08x\n", NhltTable->Header.Signature));
  DEBUG ((DEBUG_VERBOSE, " NHLT_ACPI_TABLE Header.Length          = 0x%08x\n", NhltTable->Header.Length));
  DEBUG ((DEBUG_VERBOSE, " NHLT_ACPI_TABLE Header.Revision        = 0x%02x\n", NhltTable->Header.Revision));
  DEBUG ((DEBUG_VERBOSE, " NHLT_ACPI_TABLE Header.Checksum        = 0x%02x\n", NhltTable->Header.Checksum));
  DEBUG ((DEBUG_VERBOSE, " NHLT_ACPI_TABLE Header.OemId           = %a\n",     NhltTable->Header.OemId));
  DEBUG ((DEBUG_VERBOSE, " NHLT_ACPI_TABLE Header.OemTableId      = 0x%lx\n",  NhltTable->Header.OemTableId));
  DEBUG ((DEBUG_VERBOSE, " NHLT_ACPI_TABLE Header.OemRevision     = 0x%08x\n", NhltTable->Header.OemRevision));
  DEBUG ((DEBUG_VERBOSE, " NHLT_ACPI_TABLE Header.CreatorId       = 0x%08x\n", NhltTable->Header.CreatorId));
  DEBUG ((DEBUG_VERBOSE, " NHLT_ACPI_TABLE Header.CreatorRevision = 0x%08x\n", NhltTable->Header.CreatorRevision));
  DEBUG ((DEBUG_VERBOSE, "\n"));

  DEBUG ((DEBUG_VERBOSE, " NHLT_ACPI_TABLE EndpointCount = %d\n", NhltTable->EndpointCount));
  for (i = 0; i < NhltTable->EndpointCount; i++) {
    NhltEndpointDump (GetNhltEndpoint (NhltTable, i));
  }

  NhltOedConfigDump (GetNhltOedConfig (NhltTable));
  DEBUG ((DEBUG_VERBOSE, "----------------------------------------------------------------------\n"));

  DEBUG_CODE_END();
}

/**
  Constructs FORMATS_CONFIGS structure based on given formats list.

  @param[in, out] Endpoint      Endpoint for which format structures are created
  @param[in]      FormatBitmask Bitmask of formats supported for given endpoint

  @retval                       Size of created FORMATS_CONFIGS structure

**/
UINT32
NhltFormatsConstructor (
  IN OUT ENDPOINT_DESCRIPTOR    *Endpoint,
  IN CONST UINT32               FormatsBitmask,
  IN NHLT_ENDPOINT              EndpointType
  )
{
  FORMATS_CONFIG *FormatsConfig;
  FORMAT_CONFIG  *Format;
  UINT8          FormatIndex;
  UINT32         FormatsConfigLength;

  DEBUG ((DEBUG_VERBOSE, "NhltFormatsConstructor() Start, FormatsBitmask = 0x%08x\n", FormatsBitmask));

  FormatsConfig = NULL;
  FormatIndex = 0;
  FormatsConfigLength = 0;

  if (!FormatsBitmask) {
    DEBUG ((DEBUG_WARN, "No supported format found!\n"));
    return 0;
  }

  FormatsConfig = GetNhltEndpointFormatsConfig (Endpoint);
  FormatsConfig->FormatsCount = 0;

  if (FormatsBitmask & B_HDA_2CH_48KHZ_16BIT_FORMAT) {
    DEBUG ((DEBUG_VERBOSE, "Format: B_HDA_2CH_48KHZ_16BIT_FORMAT\n"));

    Format = GetNhltEndpointFormat (Endpoint, FormatIndex++);
    if (Format != NULL){
      CopyMem (&(Format->Format), &Ch2_48kHz16bitFormat, sizeof (WAVEFORMATEXTENSIBLE));

      Format->FormatConfiguration.CapabilitiesSize = DmicFormatConfigSize + DmicCommonFormatConfigSize;
      CopyMem (Format->FormatConfiguration.Capabilities, DmicStereo16BitFormatConfig, DmicFormatConfigSize);
      CopyMem ((Format->FormatConfiguration.Capabilities + DmicFormatConfigSize), DmicCommonFormatConfig, DmicCommonFormatConfigSize);

      FormatsConfigLength += sizeof (*Format)
                           - sizeof (Format->FormatConfiguration.Capabilities)
                           + Format->FormatConfiguration.CapabilitiesSize;
      FormatsConfig->FormatsCount++;
    }
  }

  if (FormatsBitmask & B_HDA_2CH_48KHZ_24BIT_FORMAT) {
    DEBUG ((DEBUG_VERBOSE, "Format: B_HDA_2CH_48KHZ_24BIT_FORMAT\n"));

    Format = GetNhltEndpointFormat (Endpoint, FormatIndex++);
    if (Format != NULL){
      CopyMem (&(Format->Format), &Ch2_48kHz24bitFormat, sizeof (WAVEFORMATEXTENSIBLE));
      if ((EndpointType == HdaI2sRenderHP) || (EndpointType == HdaI2sRenderSKP)){
        Format->FormatConfiguration.CapabilitiesSize = I2sFormatConfigRenderSize;
        CopyMem(Format->FormatConfiguration.Capabilities, I2sFormatConfigRender, I2sFormatConfigRenderSize);
      } else if ((EndpointType == HdaI2sCaptureHP) || (EndpointType == HdaI2sCaptureSKP) ){
        Format->FormatConfiguration.CapabilitiesSize = I2sFormatConfigCaptureSize;
        CopyMem(Format->FormatConfiguration.Capabilities, I2sFormatConfigCapture, I2sFormatConfigCaptureSize);
      }
      FormatsConfigLength += sizeof (*Format)
                           - sizeof (Format->FormatConfiguration.Capabilities)
                           + Format->FormatConfiguration.CapabilitiesSize;
      FormatsConfig->FormatsCount++;
    }
  }

  if (FormatsBitmask & B_HDA_2CH_48KHZ_32BIT_FORMAT) {
    DEBUG ((DEBUG_VERBOSE, "Format: B_HDA_2CH_48KHZ_32BIT_FORMAT\n"));

    Format = GetNhltEndpointFormat (Endpoint, FormatIndex++);
    if (Format != NULL) {
      CopyMem (&(Format->Format), &Ch2_48kHz32bitFormat, sizeof (WAVEFORMATEXTENSIBLE));

      Format->FormatConfiguration.CapabilitiesSize = DmicFormatConfigSize + DmicCommonFormatConfigSize;
      CopyMem (Format->FormatConfiguration.Capabilities, DmicStereo32BitFormatConfig, DmicFormatConfigSize);
      CopyMem ((Format->FormatConfiguration.Capabilities + DmicFormatConfigSize), DmicCommonFormatConfig, DmicCommonFormatConfigSize);

      FormatsConfigLength += sizeof (*Format)
                           - sizeof (Format->FormatConfiguration.Capabilities)
                           + Format->FormatConfiguration.CapabilitiesSize;
      FormatsConfig->FormatsCount++;
    }
  }

  if (FormatsBitmask & B_HDA_4CH_48KHZ_16BIT_FORMAT) {
    DEBUG ((DEBUG_VERBOSE, "Format: B_HDA_4CH_48KHZ_16BIT_FORMAT\n"));

    Format = GetNhltEndpointFormat (Endpoint, FormatIndex++);
    if (Format != NULL){
      CopyMem (&(Format->Format), &Ch4_48kHz16bitFormat, sizeof (WAVEFORMATEXTENSIBLE));

      Format->FormatConfiguration.CapabilitiesSize = DmicFormatConfigSize + DmicCommonFormatConfigSize;
      CopyMem (Format->FormatConfiguration.Capabilities, DmicQuad16BitFormatConfig, DmicFormatConfigSize);
      CopyMem ((Format->FormatConfiguration.Capabilities + DmicFormatConfigSize), DmicCommonFormatConfig, DmicCommonFormatConfigSize);

      FormatsConfigLength += sizeof (*Format)
                           - sizeof (Format->FormatConfiguration.Capabilities)
                           + Format->FormatConfiguration.CapabilitiesSize;
      FormatsConfig->FormatsCount++;
    }
  }

  if (FormatsBitmask & B_HDA_4CH_48KHZ_32BIT_FORMAT) {
    DEBUG ((DEBUG_VERBOSE, "Format: B_HDA_4CH_48KHZ_32BIT_FORMAT\n"));

    Format = GetNhltEndpointFormat (Endpoint, FormatIndex++);
    if (Format != NULL){
      CopyMem (&(Format->Format), &Ch4_48kHz32bitFormat, sizeof (WAVEFORMATEXTENSIBLE));

      Format->FormatConfiguration.CapabilitiesSize = DmicFormatConfigSize + DmicCommonFormatConfigSize;
      CopyMem (Format->FormatConfiguration.Capabilities, DmicQuad32BitFormatConfig, DmicFormatConfigSize);
      CopyMem ((Format->FormatConfiguration.Capabilities + DmicFormatConfigSize), DmicCommonFormatConfig, DmicCommonFormatConfigSize);

      FormatsConfigLength += sizeof (*Format)
                           - sizeof (Format->FormatConfiguration.Capabilities)
                           + Format->FormatConfiguration.CapabilitiesSize;
      FormatsConfig->FormatsCount++;
    }
  }

  if (FormatsBitmask & B_HDA_NARROWBAND_FORMAT) {
    DEBUG ((DEBUG_VERBOSE, "Format: B_HDA_NARROWBAND_FORMAT\n"));

    Format = GetNhltEndpointFormat (Endpoint, FormatIndex++);
    if (Format != NULL) {
      CopyMem (&(Format->Format), &NarrowbandFormat, sizeof (WAVEFORMATEXTENSIBLE));

      Format->FormatConfiguration.CapabilitiesSize = BtFormatConfigSize;
      CopyMem(Format->FormatConfiguration.Capabilities, BtFormatConfig, BtFormatConfigSize);

      FormatsConfigLength += sizeof (*Format)
                           - sizeof (Format->FormatConfiguration.Capabilities)
                           + Format->FormatConfiguration.CapabilitiesSize;
      FormatsConfig->FormatsCount++;
    }
  }

  if (FormatsBitmask & B_HDA_WIDEBAND_FORMAT) {
    DEBUG ((DEBUG_VERBOSE, "Format: B_HDA_WIDEBAND_FORMAT\n"));

    Format = GetNhltEndpointFormat (Endpoint, FormatIndex++);
    if(Format != NULL){
      CopyMem (&(Format->Format), &WidebandFormat, sizeof (WAVEFORMATEXTENSIBLE));

      Format->FormatConfiguration.CapabilitiesSize = BtFormatConfigSize;
      CopyMem(Format->FormatConfiguration.Capabilities, BtFormatConfig, BtFormatConfigSize);

      FormatsConfigLength += sizeof (*Format)
                           - sizeof (Format->FormatConfiguration.Capabilities)
                           + Format->FormatConfiguration.CapabilitiesSize;
      FormatsConfig->FormatsCount++;
    }
  }

  if (FormatsBitmask & B_HDA_A2DP_FORMAT) {
    DEBUG ((DEBUG_VERBOSE, "Format: B_HDA_A2DP_FORMAT\n"));

    Format = GetNhltEndpointFormat (Endpoint, FormatIndex++);
    if (Format != NULL){
      CopyMem (&(Format->Format), &A2dpFormat, sizeof (WAVEFORMATEXTENSIBLE));

      Format->FormatConfiguration.CapabilitiesSize = BtFormatConfigSize;
      CopyMem(Format->FormatConfiguration.Capabilities, BtFormatConfig, BtFormatConfigSize);

      FormatsConfigLength += sizeof (*Format)
                           - sizeof (Format->FormatConfiguration.Capabilities)
                           + Format->FormatConfiguration.CapabilitiesSize;
      FormatsConfig->FormatsCount++;
    }
  }

  DEBUG ((DEBUG_VERBOSE, "NhltFormatsConstructor() End, FormatsCount = %d, FormatsConfigLength = %d B\n", FormatsConfig->FormatsCount, FormatsConfigLength));
  return FormatsConfigLength;
}


/**
  Constructs NHLT_ENDPOINT structure based on given endpoint type.

  @param[in, out] NhltTable     NHLT table for which endpoint is created
  @param[in]      EndpointType  Type of endpoint to be created
  @param[in]      EndpointIndex Endpoint index in NHLT table

  @retval                       Size of created NHLT_ENDPOINT structure

**/
UINT32
NhltEndpointConstructor (
  IN OUT NHLT_ACPI_TABLE        *NhltTable,
  IN     NHLT_ENDPOINT           EndpointType,
  IN     UINT8                   EndpointIndex
  )
{
  ENDPOINT_DESCRIPTOR *Endpoint;
  SPECIFIC_CONFIG     *EndpointConfig;
  CONST UINT8         *EndpointConfigBuffer;
  UINT32              EndpointConfigBufferSize;
  UINT32              EndpointFormatsBitmask;
  UINT32              EndpointDescriptorLength;

  DEBUG ((DEBUG_VERBOSE, "NhltEndpointConstructor() Start, EndpointIndex = %d\n", EndpointIndex));

  EndpointDescriptorLength = 0;
  Endpoint = GetNhltEndpoint (NhltTable, EndpointIndex);
  EndpointDescriptorLength = sizeof (ENDPOINT_DESCRIPTOR)
                           - sizeof (SPECIFIC_CONFIG)
                           - sizeof (FORMAT_CONFIG);

  switch (EndpointType) {
    case HdaDmicX2:
      DEBUG ((DEBUG_VERBOSE, "Endpoint: HdaDmicX2\n"));
      CopyMem (Endpoint, &HdaEndpointDmicX2, sizeof (ENDPOINT_DESCRIPTOR));
      EndpointConfigBuffer = DmicX2Config;
      EndpointConfigBufferSize = DmicX2ConfigSize;
      EndpointFormatsBitmask = B_HDA_2CH_48KHZ_16BIT_FORMAT | B_HDA_2CH_48KHZ_32BIT_FORMAT;
      break;
    case HdaDmicX4:
      DEBUG ((DEBUG_VERBOSE, "Endpoint: HdaDmicX4\n"));
      CopyMem (Endpoint, &HdaEndpointDmicX4, sizeof (ENDPOINT_DESCRIPTOR));
      EndpointConfigBuffer = DmicX4Config;
      EndpointConfigBufferSize = DmicX4ConfigSize;
      EndpointFormatsBitmask = B_HDA_4CH_48KHZ_16BIT_FORMAT | B_HDA_4CH_48KHZ_32BIT_FORMAT;
      break;
    case HdaBtRender:
      DEBUG ((DEBUG_VERBOSE, "Endpoint: HdaBtRender\n"));
      CopyMem (Endpoint, &HdaEndpointBtRender, sizeof (ENDPOINT_DESCRIPTOR));
      EndpointConfigBuffer = BtConfig;
      EndpointConfigBufferSize = BtConfigSize;
      EndpointFormatsBitmask = B_HDA_NARROWBAND_FORMAT | B_HDA_WIDEBAND_FORMAT | B_HDA_A2DP_FORMAT;
      break;
    case HdaBtCapture:
      DEBUG ((DEBUG_VERBOSE, "Endpoint: HdaBtCapture\n"));
      CopyMem (Endpoint, &HdaEndpointBtCapture, sizeof (ENDPOINT_DESCRIPTOR));
      EndpointConfigBuffer = BtConfig;
      EndpointConfigBufferSize = BtConfigSize;
      EndpointFormatsBitmask = B_HDA_NARROWBAND_FORMAT | B_HDA_WIDEBAND_FORMAT;
      break;
    case HdaI2sRenderSKP:
      DEBUG ((DEBUG_VERBOSE, "Endpoint: HdaI2sRenderSKP\n"));
      CopyMem (Endpoint, &HdaEndpointI2sRenderSKP, sizeof (ENDPOINT_DESCRIPTOR));
      EndpointConfigBuffer = I2sConfigSKP;
      EndpointConfigBufferSize = I2sConfigSKPSize;
      EndpointFormatsBitmask = B_HDA_2CH_48KHZ_24BIT_FORMAT;
      break;
    case HdaI2sCaptureSKP:
      DEBUG ((DEBUG_VERBOSE, "Endpoint: HdaI2sCaptureSKP\n"));
      CopyMem (Endpoint, &HdaEndpointI2sCapture, sizeof (ENDPOINT_DESCRIPTOR));
      EndpointConfigBuffer = I2sConfig;
      EndpointConfigBufferSize = I2sConfigSize;
      EndpointFormatsBitmask = B_HDA_2CH_48KHZ_24BIT_FORMAT;
      break;
    case HdaI2sRenderHP:
      DEBUG ((DEBUG_VERBOSE, "Endpoint: HdaI2sRenderHP\n"));
      CopyMem (Endpoint, &HdaEndpointI2sRenderHP, sizeof (ENDPOINT_DESCRIPTOR));
      EndpointConfigBuffer = I2sConfig;
      EndpointConfigBufferSize = I2sConfigSize;
      EndpointFormatsBitmask = B_HDA_2CH_48KHZ_24BIT_FORMAT;
      break;

    case HdaI2sCaptureHP:
      DEBUG ((DEBUG_VERBOSE, "Endpoint: HdaI2sCaptureHP\n"));
      CopyMem (Endpoint, &HdaEndpointI2sCapture, sizeof (ENDPOINT_DESCRIPTOR));
      EndpointConfigBuffer = I2sConfig;
      EndpointConfigBufferSize = I2sConfigSize;
      EndpointFormatsBitmask = B_HDA_2CH_48KHZ_24BIT_FORMAT;
      break;
    case HdaModem1Render:
      DEBUG ((DEBUG_VERBOSE, "Endpoint: HdaModem1Render\n"));
      CopyMem (Endpoint, &HdaEndpointModem1Render, sizeof (ENDPOINT_DESCRIPTOR));
      EndpointConfigBuffer = Modem1Config;
      EndpointConfigBufferSize = Modem1ConfigSize;
      EndpointFormatsBitmask = B_HDA_2CH_48KHZ_24BIT_FORMAT;
      break;
    case HdaModem1Capture:
      DEBUG ((DEBUG_VERBOSE, "Endpoint: HdaModem1Capture\n"));
      CopyMem (Endpoint, &HdaEndpointModem1Capture, sizeof (ENDPOINT_DESCRIPTOR));
      EndpointConfigBuffer = Modem1Config;
      EndpointConfigBufferSize = Modem1ConfigSize;
      EndpointFormatsBitmask = B_HDA_2CH_48KHZ_24BIT_FORMAT;
      break;
    case HdaModem2Render:
      DEBUG ((DEBUG_VERBOSE, "Endpoint: HdaModem2Render\n"));
      CopyMem (Endpoint, &HdaEndpointModem2Render, sizeof (ENDPOINT_DESCRIPTOR));
      EndpointConfigBuffer = Modem2Config;
      EndpointConfigBufferSize = Modem2ConfigSize;
      EndpointFormatsBitmask = B_HDA_2CH_48KHZ_24BIT_FORMAT;
      break;
    case HdaModem2Capture:
      DEBUG ((DEBUG_VERBOSE, "Endpoint: HdaModem2Capture\n"));
      CopyMem (Endpoint, &HdaEndpointModem2Capture, sizeof (ENDPOINT_DESCRIPTOR));
      EndpointConfigBuffer = Modem2Config;
      EndpointConfigBufferSize = Modem2ConfigSize;
      EndpointFormatsBitmask = B_HDA_2CH_48KHZ_24BIT_FORMAT;
      break;
    default:
      DEBUG ((DEBUG_WARN, "Unknown endpoint!\n"));
      return 0;
  }

  EndpointConfig = GetNhltEndpointDeviceCapabilities (Endpoint);
  EndpointConfig->CapabilitiesSize = EndpointConfigBufferSize;
  CopyMem (EndpointConfig->Capabilities, EndpointConfigBuffer, EndpointConfig->CapabilitiesSize);
  EndpointDescriptorLength += sizeof (*EndpointConfig)
                            - sizeof (EndpointConfig->Capabilities)
                            + EndpointConfig->CapabilitiesSize;

  EndpointDescriptorLength += NhltFormatsConstructor (Endpoint,EndpointFormatsBitmask,EndpointType);
  Endpoint->EndpointDescriptorLength = EndpointDescriptorLength;

  DEBUG ((DEBUG_VERBOSE, "NhltEndpointConstructor() End, EndpointDescriptorLength = %d B\n", Endpoint->EndpointDescriptorLength));
  return Endpoint->EndpointDescriptorLength;
}

/**
  Constructs SPECIFIC_CONFIG structure for OED configuration.

  @param[in, out]  NhltTable    NHLT table for which OED config is created

  @retval                       Size of created SPECIFIC_CONFIG structure

**/
UINT32
NhltOedConfigConstructor (
  IN OUT NHLT_ACPI_TABLE        *NhltTable
  )
{
  SPECIFIC_CONFIG  *OedConfig;
  UINT32           OedConfigLength;

  OedConfigLength = 0;
  OedConfig = GetNhltOedConfig (NhltTable);

  OedConfig->CapabilitiesSize = NhltConfigurationSize;
  CopyMem (OedConfig->Capabilities, (UINT8 *) NhltConfiguration, NhltConfigurationSize);

  OedConfigLength = sizeof (*OedConfig)
                  - sizeof (OedConfig->Capabilities)
                  + OedConfig->CapabilitiesSize;

  return OedConfigLength;
}

/**
  Constructs NHLT_ACPI_TABLE structure based on given Endpoints list.

  @param[in]       EndpointTable          List of endpoints for NHLT
  @param[in, out]  NhltTable              NHLT table to be created
  @param[in, out]  NhltTableSize          Size of created NHLT table

  @retval          EFI_SUCCESS            NHLT created successfully
  @retval          EFI_BAD_BUFFER_SIZE    Not enough resources to allocate NHLT

**/
EFI_STATUS
NhltConstructor (
  IN     HDA_NHLT_ENDPOINTS      *EndpointTable,
  IN OUT NHLT_ACPI_TABLE         *NhltTable,
  IN OUT UINT32                  *NhltTableSize
  )
{
  EFI_STATUS       Status;
  UINT8            Index;
  UINT32           TableSize;
  UINT32           EndpointDescriptorsLength;
  UINT32           OedConfigLength;

  Status = EFI_SUCCESS;
  EndpointDescriptorsLength = 0;
  OedConfigLength = 0;

  if (NhltTable == NULL) {
    return EFI_BAD_BUFFER_SIZE;
  }

  NhltTable->EndpointCount = 0;

  for (Index = 0; Index < HdaEndpointMax; Index++) {
    if (EndpointTable[Index].Enable == TRUE) {
     EndpointDescriptorsLength += NhltEndpointConstructor (NhltTable, EndpointTable[Index].EndpointType, NhltTable->EndpointCount++);
    }
  }
  DEBUG ((DEBUG_VERBOSE, "NhltConstructor: EndpointCount = %d, All EndpointDescriptorsLength = %d B\n", NhltTable->EndpointCount, EndpointDescriptorsLength));

  OedConfigLength = NhltOedConfigConstructor (NhltTable);
  DEBUG ((DEBUG_VERBOSE, "NhltConstructor: OedConfigLength = %d B\n", OedConfigLength));

  TableSize = EndpointDescriptorsLength + OedConfigLength;

  *NhltTableSize = TableSize;

  return Status;
}

/**
  Constructs EFI_ACPI_DESCRIPTION_HEADER structure for NHLT table.

  @param[in, out] NhltTable             NHLT table for which header will be created
  @param[in]      NhltTableSize         Size of NHLT table

  @retval         None

**/
VOID
NhltAcpiHeaderConstructor (
  IN OUT NHLT_ACPI_TABLE        *NhltTable,
  IN     UINT32                  NhltTableSize
  )
{
  DEBUG ((DEBUG_VERBOSE, "NhltAcpiHeaderConstructor() Start\n"));

  //
  // Header
  //
  NhltTable->Header.Signature = NHLT_ACPI_TABLE_SIGNATURE;
  NhltTable->Header.Length = (UINT32) (NhltTableSize + sizeof (NHLT_ACPI_TABLE) - sizeof (ENDPOINT_DESCRIPTOR) - sizeof (SPECIFIC_CONFIG));
  NhltTable->Header.Revision = 0x0;
  NhltTable->Header.Checksum = 0x0;

  CopyMem (NhltTable->Header.OemId, EFI_ACPI_OEM_ID, sizeof (NhltTable->Header.OemId));
  NhltTable->Header.OemTableId      = EFI_ACPI_OEM_TABLE_ID;
  NhltTable->Header.OemRevision     = EFI_ACPI_OEM_REVISION;
  NhltTable->Header.CreatorId       = EFI_ACPI_CREATOR_ID;
  NhltTable->Header.CreatorRevision = EFI_ACPI_CREATOR_REVISION;

  NhltTable->Header.Checksum = CalculateCheckSum8 ((UINT8 *)NhltTable, NhltTable->Header.Length);

  DEBUG ((DEBUG_VERBOSE, "NhltAcpiHeaderConstructor(), NhltAcpiTable->Header.Length = %d B\n", NhltTable->Header.Length));
}
