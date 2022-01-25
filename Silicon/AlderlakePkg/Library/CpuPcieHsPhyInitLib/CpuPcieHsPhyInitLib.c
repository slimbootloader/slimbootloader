/** @file

  Copyright (c) 2020 - 2021, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/
#include <Library/BaseLib.h>
#include <Library/DebugLib.h>
#include <Library/CpuPcieHsPhyInitLib.h>
#include <Library/IoLib.h>
#include <Library/PciLib.h>
#include <Library/HeciLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/BlMemoryAllocationLib.h>
#include <Library/SecureBootLib.h>
#include <BupMsgs.h>
#include <MkhiMsgsAdl.h>
#include <IndustryStandard/Pci.h>
#include <Register/PchBdfAssignment.h>
#include <PchAccess.h>

//
// Supported Hash Types
//
#define HASHALG_SHA1                      0x00000001
#define HASHALG_SHA256                    0x00000002
#define HASHALG_SHA384                    0x00000003
#define HASHALG_SHA512                    0x00000004

#define REG_BAR_BASE_ADDRESS              0xFB000000

/**
  Definition for REGBAR address
  The REGBAR address is used for the CPU IP's SB register access
**/
#define CPU_REGBAR_ADDRESS(Pid, Offset)    (REG_BAR_BASE_ADDRESS | ((UINT8)(Pid) << 16) | (UINT16)(Offset))

/**
  Push firmware contents into IP.

  @param[in] MemoryBuffer          Memory buffer of firmware data
  @param[in] MemorySize            Size of firmware data

  @retval EFI_SUCCESS              IP write success
  @retval Others                   Other failure of IP write
**/
EFI_STATUS
PushHsPhyFirmware (
  IN UINTN                MemoryBuffer,
  IN UINT32               MemorySize
)
{
  IP_PUSH_MODEL           *IpPushModel;
  UINT16                  Index;
  UINT32                  MemoryIndex;

  MemoryIndex = 0;
  IpPushModel = (IP_PUSH_MODEL *)(UINTN *)MemoryBuffer;

  while (MemoryIndex < MemorySize) {
    MemoryIndex += sizeof (IP_PUSH_MODEL);
    if ((IpPushModel->Mode.Type == 0) && (IpPushModel->Count == 0)) {
        break; // Check for EOF(End of File)
    }
    for (Index = 0; Index < IpPushModel->Count; Index++) {
      MmioWrite32 (CPU_REGBAR_ADDRESS (CPU_SB_PID_PCIE_PHYX16_BROADCAST, IpPushModel->Mode.Address), IpPushModel->DataBlob[Index]);
      MemoryIndex += sizeof (UINT32);
    }
    IpPushModel = (IP_PUSH_MODEL *)((UINT8 *)MemoryBuffer + MemoryIndex);
  }

  return EFI_SUCCESS;
}

/**
  This message is sent by the BIOS to tell CSME that Buffer has been allocated for payload to be copied.

  @param[in]      LoadRequest            IP information for which firmware has to be loaded
  @param[in]      BufferAddressLow       Lower DRAM address of the buffer
  @param[in]      BufferAddressHigh      Upper DRAM address of the buffer
  @param[in, out] BufferSize             On input, BufferSize is size of Buffer allocated by Bios.On output, actual PayloadSize copied by ME
  @param[out]     HashType               Hash algorithm used for message digest
  @param[out]     Hash                   Hash Value

  @retval EFI_SUCCESS                    Command succeeded
  @retval EFI_DEVICE_ERROR               HECI Device error, command aborts abnormally
  @retval EFI_TIMEOUT                    HECI does not return the buffer before timeout
  @retval EFI_UNSUPPORTED                Current ME mode doesn't support sending this message through this HECI or Input parameters are not valid.
  @retval EFI_BUFFER_TOO_SMALL           Message Buffer is too small for the Acknowledge
  @retval EFI_INVALID_PARAMETER          Parameter invalid
**/
EFI_STATUS
PeiHeciGetFwPayload (
  IN     UINT32        LoadRequest,
  IN     UINT32        BufferAddressLow,
  IN     UINT32        BufferAddressHigh,
  IN OUT UINT32        *BufferSize,
  OUT    UINT8         *HashType,
  OUT    UINT8         *Hash
  )
{
  EFI_STATUS                 Status;
  IP_LOADING_CMD_BUFFER      *IpLoad;
  UINT32                     ReqLength;
  UINT32                     RespLength;

  if (((BufferAddressLow == 0) && (BufferAddressHigh == 0)) ||
      (BufferSize == NULL) ||
      (HashType == NULL) ||
      (Hash == NULL)) {
    return EFI_INVALID_PARAMETER;
  }

  ReqLength = sizeof (IP_LOADING_CMD_REQ);
  RespLength = sizeof (IP_LOADING_CMD_RESP) + MAX_DIGEST_SIZE;
  IpLoad = (IP_LOADING_CMD_BUFFER*) AllocateTemporaryMemory (RespLength);
  if (IpLoad == NULL) {
    DEBUG ((DEBUG_ERROR, "HSPHY: Could not allocate Memory\n"));
    return EFI_OUT_OF_RESOURCES;
  }
  ZeroMem (IpLoad, RespLength);

  IpLoad->Request.MkhiHeader.Fields.GroupId = BUP_COMMON_GROUP_ID;
  IpLoad->Request.MkhiHeader.Fields.Command = GET_IP_FIRMWARE_CMD;
  IpLoad->Request.Version                   = IP_LOADING_CMD_VERSION;
  IpLoad->Request.Operation                 = LoadRequest;
  IpLoad->Request.DramBaseLow               = BufferAddressLow;
  IpLoad->Request.DramBaseHigh              = BufferAddressHigh;
  IpLoad->Request.MemorySize                = *BufferSize;

  //
  // Send Ip Load Request to ME
  //
  Status = HeciSendwAck (
                   HECI1_DEVICE,
                   (UINT32 *)IpLoad,
                   ReqLength,
                   &RespLength,
                   BIOS_FIXED_HOST_ADDR,
                   HECI_MKHI_MESSAGE_ADDR
                   );

  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "Heci Get IP Load Msg Failed %r\n", Status));
    return Status;
  }

  if ((IpLoad->Response.MkhiHeader.Fields.GroupId != BUP_COMMON_GROUP_ID) ||
      (IpLoad->Response.MkhiHeader.Fields.Command != GET_IP_FIRMWARE_CMD) ||
      (IpLoad->Response.MkhiHeader.Fields.IsResponse == 0) ||
      (IpLoad->Response.MkhiHeader.Fields.Result != MkhiStatusSuccess)) {
    DEBUG ((DEBUG_ERROR, "Heci Get IP Load Msg Failed\n"));
    return EFI_DEVICE_ERROR;
  }

  *BufferSize     = IpLoad->Response.PayloadSize;
  *HashType       = IpLoad->Response.HashType;

  DEBUG ((DEBUG_INFO, "Heci Ip Load Request Msg successful. Response:\n"));
  DEBUG ((DEBUG_INFO, "  PayloadSize = 0x%08x\n", *BufferSize));
  DEBUG ((DEBUG_INFO, "  Hash Type used for Signing Payload = 0x%08x\n", IpLoad->Response.HashType));
  CopyMem (Hash, IpLoad->Response.Hash, MAX_DIGEST_SIZE);

  return EFI_SUCCESS;
}

/**
  This function loads HsPhy Firmware via CSME HECI calls,
  authenticates the buffer after the loading completes
  and pushes HsPhy Firmware to Phy Controller

  @retval EFI_SUCCESS                On successfully pushing IP Firmware to Hsphy controller
  @retval Others                     Failed to load the HS PHY FW
**/
EFI_STATUS
HsPhyLoadAndInit (
  VOID
  )
{
  EFI_STATUS    Status;
  UINT32        *Buffer;
  UINT32        BufferSize;
  UINT8         HashAlg;
  UINT8         *Hash;
  UINT32        *BufferAddressHigh;
  UINT32        HeciDev;
  UINT32        *CurrBottom;
  UINT16        Data16;

  Status = EFI_SUCCESS;

  CurrBottom = AllocateTemporaryMemory (0);
  if (CurrBottom == NULL) {
    DEBUG ((DEBUG_ERROR, "CurrBottom allocation in HsPhy firmware failed\n"));
    return EFI_OUT_OF_RESOURCES;
  }

  BufferSize = HSPHY_PAYLOAD_SIZE;
  Buffer = AllocateTemporaryMemory (BufferSize);
  if (Buffer == NULL) {
    DEBUG ((DEBUG_ERROR, "Buffer allocation for HsPhy firmware failed\n"));
    Status = EFI_OUT_OF_RESOURCES;
    goto HsPhyEnd;
  }
  ZeroMem (Buffer, BufferSize);

  Hash = AllocateTemporaryMemory (MAX_DIGEST_SIZE);
  if (Hash == NULL) {
    DEBUG ((DEBUG_ERROR, "Could not allocate Memory for HsPhy Hash Buffer\n"));
    Status = EFI_OUT_OF_RESOURCES;
    goto HsPhyEnd;
  }
  ZeroMem (Hash, MAX_DIGEST_SIZE);

  //
  // Set BME and MSE to allow HECI1 copy massive data to Buffer
  //
  Data16 = 0;
  HeciDev = PCI_LIB_ADDRESS (
          DEFAULT_PCI_BUS_NUMBER_PCH,
          PCI_DEVICE_NUMBER_PCH_HECI1,
          PCI_FUNCTION_NUMBER_PCH_HECI1,
          0
          );

  if (PciRead16 (HeciDev + PCI_VENDOR_ID_OFFSET) != 0xFFFF) {
    Data16 = PciRead16 (HeciDev + PCI_COMMAND_OFFSET);
    Data16 |= (EFI_PCI_COMMAND_MEMORY_SPACE | EFI_PCI_COMMAND_BUS_MASTER);
    PciWrite16 (HeciDev + PCI_COMMAND_OFFSET, Data16);
  } else {
    DEBUG ((DEBUG_ERROR, "Failed to set BME, MSE on Heci1 config space\n"));
    Status = EFI_DEVICE_ERROR;
    goto HsPhyEnd;
  }

  //
  // Send Heci for Load PCIe GEN5 request
  //
  BufferAddressHigh = 0;
  HashAlg = 0;
  Status = PeiHeciGetFwPayload (HsPhyFwLoad, (UINT32)(UINTN)Buffer, (UINT32)(UINTN)BufferAddressHigh, &BufferSize, &HashAlg, Hash);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "PeiHeciGetFwPayload failed %r\n", Status));
    goto HsPhyEnd;
  }

  //
  // Need to translate the algorithm type from CSME to SBL types
  //
  switch (HashAlg) {
    case HASHALG_SHA256:
        HashAlg = HASH_TYPE_SHA256;
        break;
    case HASHALG_SHA384:
        HashAlg = HASH_TYPE_SHA384;
        break;
    case HASHALG_SHA512:
        HashAlg = HASH_TYPE_SHA512;
        break;
    case HASHALG_SHA1:
    default:
        DEBUG ((DEBUG_WARN, "HashAlg %d not supported, try SHA384\n"));
        HashAlg = HASH_TYPE_SHA384;
        break;
  }

  Status = DoHashVerify ((UINT8 *)Buffer, BufferSize, 0, HashAlg, (UINT8 *) Hash);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "DoHashVerify Failed %r\n", Status));
    goto HsPhyEnd;
  }

  DEBUG ((DEBUG_INFO, "Push HsPhy firmware\n"));
  PushHsPhyFirmware ((UINTN)Buffer, BufferSize);

  //
  // Clear BME and MSE of Heci1
  //
  Data16 = 0;
  if (PciRead16 (HeciDev + PCI_VENDOR_ID_OFFSET) != 0xFFFF) {
    Data16 = PciRead16 (HeciDev + PCI_COMMAND_OFFSET);
    Data16 &= ~(EFI_PCI_COMMAND_MEMORY_SPACE | EFI_PCI_COMMAND_BUS_MASTER);
    PciWrite16 (HeciDev + PCI_COMMAND_OFFSET, Data16);
  } else {
    DEBUG ((DEBUG_ERROR, "Failed to clear BME, MSE on Heci1 config space\n"));
    Status = EFI_DEVICE_ERROR;
    goto HsPhyEnd;
  }

HsPhyEnd:
  FreeTemporaryMemory (CurrBottom);
  return Status;
}
