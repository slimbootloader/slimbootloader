/** @file

  Copyright (c) 2017-2018, Intel Corporation. All rights reserved.<BR>
  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#include <PiPei.h>

#include <Library/BaseLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/BootloaderCommonLib.h>
#include <Library/DebugLib.h>
#include <Library/HobLib.h>
#include <Library/IoLib.h>
#include <Library/PcdLib.h>
#include <Library/PciLib.h>
#include <Library/PciCf8Lib.h>
#include <Library/TimerLib.h>
#include <IndustryStandard/Pci22.h>
#include <Service/HeciService.h>
#include "CseMsg.h"
#include "HeciRegs.h"
#include "MkhiMsgs.h"
#include <Library/BootloaderCoreLib.h>

const HECI_SERVICE   mHeciService = {
  .Header.Signature  = HECI_SERVICE_SIGNATURE,
  .Header.Version    = HECI_SERVICE_VERSION,
  .SimpleHeciCommand = SimpleHeciCommand,
  .HeciUserCommand   = HeciSendUserCommand,
};

UINT32 mCseBus = 0;
UINT32 mCseDevice = 0;
UINT32 mHeciFunction = 0;

BOOLEAN mCseInfoIsSet = FALSE;


STATIC
EFI_STATUS
HeciSendWack (
  UINT8   *Message,
  UINT32  Length,
  UINT32  *RecLength,
  UINT8   HostAddress,
  UINT8   SeCAddress
);

STATIC
VOID
CheckCseResetAndIssueHeciReset (
  UINT32  HeciBAR,
  BOOLEAN *ResetStatus
);

/**
  Get MBP data pointer

  @return MBP_CMD_RESP_DATA     pointer to MBP data header.
  @return NULL                  NULL in case MBP not found.

**/
STATIC
MBP_CMD_RESP_DATA *
HeciMBP (
  VOID
)
{
  LOADER_GLOBAL_DATA       *LdrGlobal;
  MBP_CMD_RESP_DATA        *MBPHeader;

  LdrGlobal = (LOADER_GLOBAL_DATA *)GetLoaderGlobalDataPointer ();
  MBPHeader = GetGuidHobData(LdrGlobal->FspHobList, NULL, &gEfiHeciMbpDataHobGuid);
  ASSERT (MBPHeader != NULL);

  return MBPHeader;
}

/**
  Set the CSE device and HECI function Info

  @return EFI_SUCCESS       The device is supported for use
  @return EFI_NOT_FOUND     The platform device info HOB was not found
  @return EFI_UNSUPPORTED   The device is not supported for use

**/
STATIC
EFI_STATUS
SetCseInfo (
  VOID
)
{
  EFI_STATUS              Status;
  HECI_INSTANCE           *HeciInstance;

  mCseInfoIsSet = TRUE;

  Status = GetLibraryData (PcdGet8(PcdHeciLibId), (VOID **)&HeciInstance);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  mCseBus = HeciInstance->Bus;
  mCseDevice = HeciInstance->Device;
  mHeciFunction = HeciInstance->Function;

  return EFI_SUCCESS;
}

/**
  Waits for the CSE to report that it is ready for communication over the HECI
  interface.

  @param[in]  Arg  HECI Bar value

  @retval EFI_SUCCESS    The device is ready for use
  @retval EFI_NOT_READY  The device is not ready for use

**/
STATIC
EFI_STATUS
WaitForCseReady (
  VOID *Arg
)
{
  MicroSecondDelay (1);
  volatile CseControlRegister *CseControlReg = (volatile CseControlRegister*)(*((UINT32*)Arg) + SEC_CSR_HA);
  return (CseControlReg->r.SecRdyHra == 0 ? EFI_NOT_READY : EFI_SUCCESS);
}

/**
  Waits for the CSE to inidcate interrupt back to the host over the HECI
  interface.

  @param[in]  Arg  HECI Bar value

  @retval EFI_SUCCESS    The interrupt has been triggered
  @retval EFI_NOT_READY  The interrupt has not been triggered

**/
STATIC
EFI_STATUS
WaitForCseToHostInterrupt (
  VOID *Arg
)
{
  MicroSecondDelay (1);
  volatile HostControlRegister *HostControlReg = (volatile HostControlRegister*)(*((UINT32*)Arg) + H_CSR);
  return (HostControlReg->r.His == 0 ? EFI_NOT_READY : EFI_SUCCESS);
}

/**
  Waits for the CSE to inidcate the host side circular buffer is available for the host
  to read or write data to the buffer.

  @param[in]  Arg  HECI Bar value

  @retval EFI_SUCCESS    The buffer is ready for use
  @retval EFI_NOT_READY  The buffer is not ready for use

**/
STATIC
EFI_STATUS
WaitForCseRegCbrpCbwp (
  VOID *Arg
)
{
  MicroSecondDelay (1);
  volatile CseControlRegister *CseControlReg = (volatile CseControlRegister*)(*((UINT32*)Arg) + SEC_CSR_HA);
  return (CseControlReg->r.SecCbrpHra == CseControlReg->r.SecCbwpHra ? EFI_NOT_READY : EFI_SUCCESS);
}

/**
  Waits for the CSE to inidcate the HECI interface reset has been started.

  @param[in]  Arg  HECI Bar value

  @retval EFI_SUCCESS    The HECI interface reset has been started
  @retval EFI_NOT_READY  The HECI interface reset has not been started

**/
STATIC
EFI_STATUS
WaitForResetStarted (
  VOID *Arg
)
{
  MicroSecondDelay (1);
  volatile HostControlRegister *HostControlReg = (volatile HostControlRegister*)(*((UINT32*)Arg) + H_CSR);
  return (HostControlReg->r.Hrdy == 1 ? EFI_NOT_READY : EFI_SUCCESS);
}

/**
  Return SEC Operation Mode

  @param[in]  SecMode pointer for status report

  @retval  EFI_SUCCESS      The device returned its current mode
  @retval  EFI_UNSUPPORTED  The device is not supported

**/
STATIC
EFI_STATUS
HeciGetSecMode (
  UINTN *SecMode
)
{
  HECI_FWS_REGISTER SeCFirmwareStatus;
  EFI_STATUS Status;

  if (!mCseInfoIsSet) {
    Status = SetCseInfo ();
    if (EFI_ERROR(Status)) {
      return EFI_UNSUPPORTED;
    }
  }
  if (mCseDevice == 0) {
    return EFI_UNSUPPORTED;
  }

  SeCFirmwareStatus.ul = HeciPciRead32 (R_SEC_FW_STS0);

  switch (SeCFirmwareStatus.r.SeCOperationMode) {
  case SEC_OPERATION_MODE_NORMAL:
    *SecMode = SEC_MODE_NORMAL;
    break;

  case SEC_OPERATION_MODE_ALT_DISABLED:
    *SecMode = SEC_DEBUG_MODE_ALT_DIS; //debug Mode
    break;

  case SEC_OPERATION_MODE_SOFT_TEMP_DISABLE:
    *SecMode = SEC_MODE_TEMP_DISABLED;
    break;

  case SEC_OPERATION_MODE_SECOVR_JMPR:
  case SEC_OPERATION_MODE_SECOVR_HECI_MSG:
    *SecMode = SEC_MODE_SECOVER;
    break;

  default:
    *SecMode = SEC_MODE_FAILED;
  }

  DEBUG ((DEBUG_INFO, "HECI SecMode %X\n", SeCFirmwareStatus.r.SeCOperationMode));

  return EFI_SUCCESS;
}

/**
  This function gets HECI device PCI base address and checks for HECI device availability and provides HECI Device MBAR
  after enabling Device specific BME, MSE and SERR.

  @retval Corresponding HECI Device MBAR

**/
STATIC
UINT32
CheckAndFixHeciForAccess (
  VOID
)
{
  UINT32 HeciMBAR;
  UINT32 Value;
  BOOLEAN ResetStatus;
  EFI_STATUS Status;

  if (!mCseInfoIsSet) {
    Status = SetCseInfo ();
    if (EFI_ERROR(Status)) {
      return 0;
    }
  }
  if (mCseDevice == 0) {
    return 0;
  }

  //
  // Read HECI_MBAR in case it has changed
  //
  Value = HeciPciRead32 (R_HECIMBAR0) & 0xFFFFFFF0;

  DEBUG ((DEBUG_VERBOSE, "CheckAndFixHeciForAccess R_HECIMBAR0 %x\n", Value));

  HeciPciWrite32 (R_HECIMBAR0, 0xFFFFFFFF);
  HeciMBAR = HeciPciRead32 (R_HECIMBAR0) & 0xFFFFFFF0 ;
  DEBUG ((DEBUG_VERBOSE, "CheckAndFixHeciForAccess %x\n", HeciMBAR));
  HeciPciWrite32 (R_HECIMBAR0,Value);

  HeciMBAR = HeciPciRead32 (R_HECIMBAR0) & 0xFFFFFFF0 ;
  DEBUG ((DEBUG_VERBOSE, "CheckAndFixHeciForAccess HeciMBAR %x\n", HeciMBAR));
  //
  // Check if HECI_MBAR is disabled
  //
  if ((HeciPciRead8 (PCI_COMMAND_OFFSET) & (EFI_PCI_COMMAND_MEMORY_SPACE | EFI_PCI_COMMAND_BUS_MASTER)) !=
          (EFI_PCI_COMMAND_MEMORY_SPACE | EFI_PCI_COMMAND_BUS_MASTER))
  {
    //
    // If cmdl it on.
    //
    HeciPciOr16 (PCI_COMMAND_OFFSET, EFI_PCI_COMMAND_MEMORY_SPACE | EFI_PCI_COMMAND_BUS_MASTER | EFI_PCI_COMMAND_SERR);
  }

  CheckCseResetAndIssueHeciReset (HeciMBAR, &ResetStatus);

  return HeciMBAR;
}

/**
  Function sends one messsage (of any length) through the HECI circular buffer.

  @param[in]  HeciBar     - HECI Bar value
  @param[in]  Message     - Pointer to the message data to be sent.
  @param[in]  Length      - Length of the message in bytes.
  @param[in]  HostAddress - The address of the host processor.
  @param[in]  SeCAddress  - Address of the ME subsystem the message is being sent to.

  @retval  EFI_STATUS

**/
STATIC
EFI_STATUS
HeciSend (
  UINT32 HeciBar,
  UINT8 *Message,
  UINT32 Length,
  UINT8 HostAddress,
  UINT8 SeCAddress
)
{
  UINT32 LeftSize;
  UINT32 MaxBuffer;
  UINT32 WriteSize;
  UINT32 Size;
  UINT32 Index;
  UINT32 *MessageBody;
  UINTN  Timeout;
  HeciMessageHeader MessageHeader;
  volatile UINT32 *WriteBuffer;
  volatile HostControlRegister *HostControlReg;
  volatile CseControlRegister *CseControlReg;

  DEBUG ((DEBUG_VERBOSE, "Start HeciSend\n"));
  HostControlReg = (volatile HostControlRegister *) (HeciBar + H_CSR);
  CseControlReg = (volatile CseControlRegister *) (HeciBar + SEC_CSR_HA);
  WriteBuffer = (UINT32*) (HeciBar + H_CB_WW);
  MessageBody = (UINT32*) Message;

  MaxBuffer = HostControlReg->r.Hcbd;

  //The first DWORD used for send MessageHeader, so useable Buffer Size should be MaxBuffer -1;
  MaxBuffer -= 1;
  LeftSize = (Length + 3)/4;
  WriteSize = 0;
  HostControlReg->r.Hrdy = 1;
  while (LeftSize > 0) {
    DEBUG ((DEBUG_VERBOSE, "Wait for CSE ready, CseControlReg %x\n", CseControlReg->Data));

    Timeout = 50 * 1000;
    do {
      if (WaitForCseReady ((VOID*)&HeciBar) == EFI_SUCCESS) {
        break;
      }
      Timeout--;
    } while (Timeout > 0);
    if (Timeout == 0) {
      DEBUG ((DEBUG_ERROR, "Timeout waiting CSE ready\n"));
      return EFI_TIMEOUT;
    }

    DEBUG ((DEBUG_VERBOSE, "CSE ready CseControlReg %x\n", CseControlReg->Data));
    HostControlReg->r.Hrdy = 1;
    HostControlReg->r.Hie = 0;

    Size = (LeftSize > MaxBuffer) ? MaxBuffer : LeftSize;

    LeftSize -= Size;

    // Prepare message header
    MessageHeader.Data = 0;
    MessageHeader.Fields.SecAddress = SeCAddress;
    MessageHeader.Fields.HostAddress = HostAddress;
    MessageHeader.Fields.MessageComplete = (LeftSize > 0) ? 0 : 1;
    MessageHeader.Fields.Length = (UINT32)((LeftSize > 0) ? Size * sizeof (UINT32) : Length - WriteSize * sizeof (UINT32));
    DEBUG ((DEBUG_VERBOSE, "Heci Message Header: %08x\n", MessageHeader.Data));
    *WriteBuffer = MessageHeader.Data;
    for (Index = 0; Index < Size; Index ++) {
      DEBUG ((DEBUG_VERBOSE, "MessageBody[%x] = %08x\n", Index, MessageBody[Index + WriteSize]));
      *WriteBuffer = MessageBody[Index + WriteSize];
    }

    //Send the Interrupt;
    DEBUG ((DEBUG_VERBOSE, "Prepare Send Interrupt to CSE: %08x\n", HostControlReg->Data));
    HostControlReg->r.His = 1;
    HostControlReg->r.Hrdy = 1;
    HostControlReg->r.Hie = 0;
    HostControlReg->r.Hig = 1;
    DEBUG ((DEBUG_VERBOSE, "Send Interrupt to CSE: %08x\n", HostControlReg->Data));

    WriteSize += Size;
    if (LeftSize > 0) {
      DEBUG ((DEBUG_VERBOSE, "MoreData Need be sent, waiting CSE notify\n"));
      DEBUG ((DEBUG_VERBOSE, "HostControlReg %x\n", CseControlReg->Data));
      Timeout = 50 * 1000;
      do {
        if (WaitForCseToHostInterrupt ((VOID*)&HeciBar) == EFI_SUCCESS) {
          break;
        }
        Timeout--;
      } while (Timeout > 0);
      if (Timeout == 0) {
        DEBUG ((DEBUG_ERROR, "Timeout waiting interrupt\n"));
        return EFI_TIMEOUT;
      }
      DEBUG ((DEBUG_VERBOSE, "Get CSE notify, HostControlReg %x\n", CseControlReg->Data));
    }
  }
  DEBUG ((DEBUG_VERBOSE, "No MoreData Need be sent."));
  DEBUG ((DEBUG_VERBOSE, "End HeciSend \n"));
  return EFI_SUCCESS;
}

/**
  Reads a message from the SEC across HECI.

  @param[in]      HeciBar     - HECI Bar value
  @param[out]     MessageData - Pointer to a buffer used to receive a message.
  @param[in,out]  Length      - Pointer to the length of the buffer on input and the length
                                of the message on return. (in bytes)

  @retval  EFI_STATUS

**/
STATIC
EFI_STATUS
HeciReceive (
  UINT32 HeciBar,
  UINT8 *Message,
  UINT32 *Length
)
{
  UINT32 ReadSize = 0;
  UINT32 Index;
  UINT32 *MessageBody;
  UINTN  Timeout;
  HeciMessageHeader MessageHeader;
  volatile UINT32 *ReadBuffer;
  volatile HostControlRegister *HostControlReg;

  DEBUG ((DEBUG_VERBOSE, "Start HeciReceive\n"));

  HostControlReg = (volatile HostControlRegister*) (HeciBar + H_CSR);

  ReadBuffer = (UINT32*) (HeciBar + SEC_CB_RW);
  MessageBody = (UINT32*) Message;
  while (1) {
    DEBUG ((DEBUG_VERBOSE, "Waiting for CSE notify, HostControlReg: %08x\n", HostControlReg->Data));

    if (HostControlReg->Data == 0x800B0909) {
      DEBUG ((DEBUG_VERBOSE, "Need CSE to finish loading and release NVM control...\n"));
      DEBUG ((DEBUG_VERBOSE, "\nIf you're stuck here for more than 30sec, Please check that your eMMC has GPP4.\n"));
    }

    DEBUG ((DEBUG_VERBOSE, "Get CSE notify, HostControlReg: %08x\n", HostControlReg->Data));
    HostControlReg->r.Hrdy = 1;
    HostControlReg->r.Hie = 0;
    DEBUG ((DEBUG_VERBOSE, "Disable Interrupt, HostControlReg: %08x\n", HostControlReg->Data));

    Timeout = 500 * 1000;
    do {
      if (WaitForCseRegCbrpCbwp ((VOID*)&HeciBar) == EFI_SUCCESS) {
        break;
      }
      Timeout--;
    } while (Timeout > 0);
    if (Timeout == 0) {
      goto tx_failed;
    }

    MessageHeader.Data = *ReadBuffer;
    DEBUG ((DEBUG_VERBOSE, "Get Message Header: %08x\n", MessageHeader.Data));
    for(Index = 0; Index < (MessageHeader.Fields.Length + 3)/4; Index ++) {
      Timeout = 50 * 1000;
      do {
        if (WaitForCseRegCbrpCbwp ((VOID*)&HeciBar) == EFI_SUCCESS) {
          break;
        }
        Timeout--;
      } while (Timeout > 0);
      if (Timeout == 0) {
        goto tx_failed;
      }
      MessageBody[Index + ReadSize] = *ReadBuffer;
      DEBUG ((DEBUG_VERBOSE, "MessageBody[%x] = %08x\n", Index, MessageBody[Index + ReadSize]));
    }
    DEBUG ((DEBUG_VERBOSE, "Enable Host to get CSE interrupt: %08x\n", HostControlReg->Data));
    HostControlReg->r.His = 1;
    HostControlReg->r.Hrdy = 1;
    HostControlReg->r.Hie = 0;
    HostControlReg->r.Hig = 1;
    DEBUG ((DEBUG_VERBOSE, "get CSE interrupt Enabled: %08x\n", HostControlReg->Data));
    if (MessageHeader.Fields.MessageComplete == 1) {
      DEBUG ((DEBUG_VERBOSE, "Not moreData need be receive, end\n"));
      break;
    }
  }
  DEBUG ((DEBUG_VERBOSE, "End HeciReceive \n"));
  return EFI_SUCCESS;

tx_failed:
  DEBUG ((DEBUG_ERROR, "Timeout during Data tx\n"));
  return EFI_TIMEOUT;
}

/**
  HeciSendwACK

  @param[in,out]  Message
  @param[in]      Length
  @param[in,out]  RecLength
  @param[in]      HostAddress
  @param[in]      SeCAddress

  @return EFI_STATUS

**/
STATIC
EFI_STATUS
HeciSendWack (
  OUT UINT8  *Message,
  IN  UINT32 Length,
  OUT UINT32 *RecLength,
  IN  UINT8  HostAddress,
  IN  UINT8  SeCAddress
)
{
  EFI_STATUS Status;
  UINT32   HeciBar;

  HeciBar = CheckAndFixHeciForAccess ();
  if (HeciBar == 0) {
    return EFI_NOT_FOUND;
  }
  DEBUG ((DEBUG_VERBOSE, "HeciSendWack HeciBar %08x\n", HeciBar));
  //
  // Send the message
  //
  DEBUG ((DEBUG_VERBOSE, "HeciSendWack() - Start\n"));
  Status = HeciSend (HeciBar, Message, Length, HostAddress, SeCAddress);
  if (EFI_ERROR(Status)) {
    return Status;
  }

  ///
  /// Wait for ACK message
  ///
  Status = HeciReceive (HeciBar, Message, RecLength);
  return Status;
}

/**
  Function forces a reinit of the HECI interface by following the reset HECI interface via host algorithm
  in HPS 0.90 doc 4-17-06 njy

  @retval  EFI_STATUS

**/
EFI_STATUS
EFIAPI
ResetHeciInterface (
  VOID
)
{
  volatile HostControlRegister HostControlReg;
  UINT32 HeciBar;
  UINTN  Timeout;

  HeciBar = CheckAndFixHeciForAccess ();
  if (HeciBar == 0) {
    return EFI_NOT_FOUND;
  }
  DEBUG ((DEBUG_INFO, "Starting reset of HECI interface\n"));

  // Enable Reset
  HostControlReg.Data = MmioRead32 (HeciBar + H_CSR);

  DEBUG ((DEBUG_INFO, "Assert reset\n"));
  HostControlReg.r.Hig = 1;
  HostControlReg.r.His = 1;
  HostControlReg.r.Hrst = 1;
  MmioWrite32 (HeciBar + H_CSR, HostControlReg.Data);

  // Make sure that the reset started
  DEBUG ((DEBUG_INFO, "Wait for reset started\n"));
  Timeout = 8000;
  do {
    if (WaitForResetStarted ((VOID*)&HeciBar) == EFI_SUCCESS) {
      break;
    }
    Timeout--;
  } while (Timeout > 0);
  if (Timeout == 0) {
    DEBUG ((DEBUG_INFO, "Timeout waiting reset\n"));
    return EFI_TIMEOUT;
  }

  // Wait for SEC to perform reset
  DEBUG ((DEBUG_INFO, "Wait for CSE to get RDY again\n"));
  Timeout = 8000;
  do {
    if (WaitForCseReady ((VOID*)&HeciBar) == EFI_SUCCESS) {
      break;
    }
    Timeout--;
  } while (Timeout > 0);
  if (Timeout == 0) {
    DEBUG ((DEBUG_INFO, "Timeout waiting CSE ready\n"));
    return EFI_TIMEOUT;
  }

  //
  // Make sure IS has been signaled on the HOST side
  DEBUG ((DEBUG_INFO, "Wait for CSE to host interrupt enable\n"));
  Timeout = 8000;
  do {
    if (WaitForCseToHostInterrupt ((VOID*)&HeciBar) == EFI_SUCCESS) {
      break;
    }
    Timeout--;
  } while (Timeout > 0);
  if (Timeout == 0) {
    DEBUG ((DEBUG_INFO, "Timeout waiting interrupt\n"));
    return EFI_TIMEOUT;
  }

  HostControlReg.Data = MmioRead32 (HeciBar + H_CSR);

  // Enable host side interface
  HostControlReg.r.Hrst = 0;
  HostControlReg.r.Hig = 1;
  HostControlReg.r.Hrdy = 1;
  MmioWrite32(HeciBar + H_CSR, HostControlReg.Data);

  DEBUG ((DEBUG_INFO, "Reset HECI interface done\n"));

  return EFI_SUCCESS;
}

/**
  Wait for CSE reset bit set and then Reset HECI interface.

  @param[in]  HeciBar         HECI Bar value
  @param[out] Reset_Status    Reset Status value

**/
STATIC
VOID
CheckCseResetAndIssueHeciReset (
  IN  UINT32 HeciBar,
  OUT BOOLEAN *ResetStatus
)
{
  EFI_STATUS Status;
  volatile CseControlRegister *SecControlReg;

  *ResetStatus = FALSE;
  SecControlReg = (volatile CseControlRegister *) (HeciBar + SEC_CSR_HA);
  if (SecControlReg->r.SecRstHra == 1) {
    DEBUG ((DEBUG_INFO, "CSE IS IN RESET - executing HECI interface reset procedure\n"));

    *ResetStatus = TRUE;
    // Reset HECI Interface
    Status = ResetHeciInterface ();
    if (EFI_ERROR(Status)) {
      DEBUG ((DEBUG_ERROR, "Reset HECI failed: %d\n",Status));
      *ResetStatus = FALSE;
    } else {
      DEBUG ((DEBUG_VERBOSE, "HECI Interface Reset success\n"));
    }
  } else {
    DEBUG ((DEBUG_VERBOSE, "CSE NOT IN RESET - doing nothing\n"));
  }
}

/**
  Dump the CSE device status.

  @retval EFI_SUCCESS         Dumping the status succeeded
  @retval EFI_NOT_FOUND       The CSE device is not found

**/
EFI_STATUS
EFIAPI
DumpCseStatus (
  VOID
)
{
  UINT32 HeciBar;
  UINT32 HECI1_FWSTS1;
  volatile HostControlRegister HostControlReg;
  volatile CseControlRegister SecControlReg;

  HeciBar = CheckAndFixHeciForAccess ();
  if (HeciBar == 0) {
    return EFI_NOT_FOUND;
  }

  HECI1_FWSTS1 = HeciPciRead32 (0x40);

  HostControlReg.Data = MmioRead32 (HeciBar + H_CSR);
  SecControlReg.Data = MmioRead32 (HeciBar + SEC_CSR_HA);

  DEBUG ((DEBUG_INFO, "HECI1 FW STATUS: 0x%08x\n\n", HECI1_FWSTS1));

  DEBUG ((DEBUG_INFO, "SEC control reg 0x%08x:\n", SecControlReg.Data));
  DEBUG ((DEBUG_INFO, "---------------------------------------\n"));
  DEBUG ((DEBUG_INFO, "SecIeHra: %u, ", SecControlReg.r.SecIeHra));
  DEBUG ((DEBUG_INFO, "SecIsHra: %u, ", SecControlReg.r.SecIsHra));
  DEBUG ((DEBUG_INFO, "SecIgHra: %u, ", SecControlReg.r.SecIgHra));
  DEBUG ((DEBUG_INFO, "SecRdyHra: %u, ", SecControlReg.r.SecRdyHra));
  DEBUG ((DEBUG_INFO, "SecRstHra: %u\n", SecControlReg.r.SecRstHra));
  DEBUG ((DEBUG_INFO, "Reserved:\n"));
  DEBUG ((DEBUG_INFO, "SecCbrpHra: 0x%04x, ", SecControlReg.r.SecCbrpHra));
  DEBUG ((DEBUG_INFO, "SecCbwpHra: 0x%04x, ", SecControlReg.r.SecCbwpHra));
  DEBUG ((DEBUG_INFO, "SecCbdHra: 0x%04x\n\n", SecControlReg.r.SecCbdHra));

  DEBUG ((DEBUG_INFO, "Host control reg 0x%08x:\n", HostControlReg.Data));
  DEBUG ((DEBUG_INFO, "---------------------------------------\n"));
  DEBUG ((DEBUG_INFO, "Hie: %u, ", HostControlReg.r.Hie));
  DEBUG ((DEBUG_INFO, "His: %u, ", HostControlReg.r.His));
  DEBUG ((DEBUG_INFO, "Hig: %u, ", HostControlReg.r.Hig));
  DEBUG ((DEBUG_INFO, "Hrdy: %u, ", HostControlReg.r.Hrdy));
  DEBUG ((DEBUG_INFO, "Hrst: %u\n", HostControlReg.r.Hrst));
  DEBUG ((DEBUG_INFO, "Reserved:\n"));
  DEBUG ((DEBUG_INFO, "Hcbrp: 0x%04x, ", HostControlReg.r.Hcbrp));
  DEBUG ((DEBUG_INFO, "Hcbwp: 0x%04x, ", HostControlReg.r.Hcbwp));
  DEBUG ((DEBUG_INFO, "Hcbd: 0x%04x\n", HostControlReg.r.Hcbd));

  return EFI_SUCCESS;
}

/**
  Set HECI to DNX mode.

  @return EFI_SUCCESS   Enter DNX mode success.
  @return Others        Enter DNX mode failed.

**/
EFI_STATUS
EFIAPI
EnterDnxMode (
  VOID
)
{
  UINT8  DataBuffer[sizeof (HECI_REQ_CSE_DNX_REQ)];
  UINT32 HeciSendLength;
  UINT32 HeciRecvLength;
  UINT32 Status;
  HECI_REQ_CSE_DNX_REQ *Request;
  HECI_RES_CSE_DNX_REQ *Response;

  ZeroMem (DataBuffer, sizeof (DataBuffer));

  Request = (HECI_REQ_CSE_DNX_REQ*)DataBuffer;
  Request->MkhiHeader.Fields.GroupId = 0x0D;
  Request->MkhiHeader.Fields.Command = 0x01;

  HeciSendLength = sizeof (HECI_REQ_CSE_DNX_REQ);
  HeciRecvLength = sizeof (HECI_RES_CSE_DNX_REQ);

  Status = HeciSendWack (
               DataBuffer,
               HeciSendLength,
               &HeciRecvLength,
               BIOS_FIXED_HOST_ADDR,
               0x7
            );

  if (EFI_ERROR(Status)) {
    return Status;
  }

  Response = (HECI_RES_CSE_DNX_REQ*)DataBuffer;

  if (Response->MkhiHeader.Fields.Result == 0 && Response->ReqBiosAction == 0x1) {
    // Cold reset
    IoWrite8 (0xcf9, 0xE);
    CpuDeadLoop ();
  }

  return Status;
}

/**
  Send EOP message through HECI.

  @return EFI_SUCCESS   Send EOP message success.
  @return Others              Send EOP message failed.

**/
EFI_STATUS
EFIAPI
HeciEndOfPost (
  VOID
)
{
  EFI_STATUS Status;
  UINT32 HeciSendLength;
  UINT32 HeciRecvLength;
  GEN_END_OF_POST *SendEop;
  GEN_END_OF_POST_ACK *EopResp;
  UINT32 SeCMode;
  UINT8 DataBuffer[sizeof (GEN_END_OF_POST_ACK)];

  DEBUG ((DEBUG_INFO, "Start Send HECI Message: EndOfPost\n"));

  Status = HeciGetSecMode (&SeCMode);
  if (EFI_ERROR(Status) || (SeCMode != SEC_OPERATION_MODE_NORMAL)) {
    return EFI_DEVICE_ERROR;
  }
  DEBUG ((DEBUG_INFO, "GetSeCMode successful\n"));

  ZeroMem (DataBuffer, sizeof (DataBuffer));

  SendEop = (GEN_END_OF_POST*)DataBuffer;
  SendEop->MkhiHeader.Fields.GroupId = EOP_GROUP_ID;
  SendEop->MkhiHeader.Fields.Command = EOP_CMD_ID;

  DEBUG ((DEBUG_INFO, "GEN_END_OF_POST size is %x\n", sizeof (GEN_END_OF_POST)));
  HeciSendLength = sizeof (GEN_END_OF_POST);
  HeciRecvLength = sizeof (DataBuffer);

  Status = HeciSendWack (
               DataBuffer,
               HeciSendLength,
               &HeciRecvLength,
               BIOS_FIXED_HOST_ADDR,
               0x7
            );

  if (EFI_ERROR(Status)) {
    DEBUG ((DEBUG_ERROR, "HeciEndOfPost failed!\n"));
    return Status;
  }

  EopResp = (GEN_END_OF_POST_ACK*)DataBuffer;

  DEBUG ((DEBUG_INFO, "Group    =%08x\n", EopResp->Header.Fields.GroupId));
  DEBUG ((DEBUG_INFO, "Command  =%08x\n", EopResp->Header.Fields.Command));
  DEBUG ((DEBUG_INFO, "IsRespone=%08x\n", EopResp->Header.Fields.IsResponse));
  DEBUG ((DEBUG_INFO, "Result   =%08x\n", EopResp->Header.Fields.Result));
  DEBUG ((DEBUG_INFO, "RequestedActions   =%08x\n", EopResp->Data.RequestedActions));

  return Status;
}

/**
  HeciSendUserCommand

  @param[in,out]  Data
  @param[in]      Length
  @param[in]      Force

  @return EFI_STATUS

**/
EFI_STATUS
EFIAPI
HeciSendUserCommand (
  IN OUT UINT8 *Data,
  IN     UINT8 Length,
  IN     UINT8 Force
)
{
  UINTN  Status;
  UINT32 HeciSendLength;
  UINT32 HeciRecvLength;
  UINT32 SeCMode;
  UINT8  DataBuffer[sizeof (HECI_USER_CMD_REQUEST)];
  HECI_USER_CMD_REQUEST *SendCommand;
  HECI_USER_CMD_RESPONSE *CommandResp;

  if (Length == 0 && (!Force)) {
    DEBUG ((DEBUG_INFO, "No need Sending HeciSendUserCommandClear.\n"));
    return EFI_SUCCESS;
  }
  Status = HeciGetSecMode (&SeCMode);
  if (EFI_ERROR(Status) || (SeCMode != SEC_OPERATION_MODE_NORMAL)) {
    return EFI_DEVICE_ERROR;
  }

  ZeroMem (DataBuffer, sizeof (DataBuffer));

  SendCommand = (HECI_USER_CMD_REQUEST*) DataBuffer;
  SendCommand->MkhiHeader.Fields.GroupId = MKHI_IFWI_UPD_SIG;
  SendCommand->MkhiHeader.Fields.Command = MKHI_IFWI_UPD_SIG_CMD;
  SendCommand->SubCommand = (Length == 0) ? SEND_CLEAR_SUB_COMMAND :
                             SEND_USR_CMD_SUB_COMMAND;

  //sanity check
  if (Length > 0) {
    if (Length > CSE_USRCMD_SIZE) {
      Length = CSE_USRCMD_SIZE;
    }

    CopyMem (SendCommand->Data, Data, Length);
  }

  HeciSendLength = sizeof (HECI_USER_CMD_REQUEST);
  HeciRecvLength = sizeof (HECI_USER_CMD_RESPONSE);

  Status = HeciSendWack (
               DataBuffer,
               HeciSendLength,
               &HeciRecvLength,
               BIOS_FIXED_HOST_ADDR,
               0x7
            );

  CommandResp = (HECI_USER_CMD_RESPONSE*) DataBuffer;

  DEBUG ((DEBUG_INFO,  "Group    =%08x\n", CommandResp->Header.Fields.GroupId));
  DEBUG ((DEBUG_INFO,  "Command  =%08x\n", CommandResp->Header.Fields.Command));
  DEBUG ((DEBUG_INFO,  "IsRespone=%08x\n", CommandResp->Header.Fields.IsResponse));
  DEBUG ((DEBUG_INFO,  "Result   =%08x\n", CommandResp->Header.Fields.Result));

  if (CommandResp->Header.Fields.Result != 0) {
    Status = CommandResp->Header.Fields.Result;
    DEBUG ((DEBUG_ERROR, "Heci send user command fail!\n"));
  }

  return Status;
}

/**
  Extend the PCR for measured boot support

  @param[in]  Pcr

  @return EFI_STATUS

**/
EFI_STATUS
EFIAPI
PcrExtend (
  IN UINT8 *Pcr
)
{
  EFI_STATUS Status;
  UINT32 HeciSendLength;
  UINT32 HeciRecvLength;
  MKHI_BOOTLOADER_PCR_EXTEND_RESP_DATA *PCRResp;
  MKHI_BOOTLOADER_PCR_EXTEND_REQ_DATA *SendPCR;
  UINT8 DataBuffer[sizeof (MKHI_BOOTLOADER_PCR_EXTEND_REQ_DATA)];

  DEBUG ((DEBUG_INFO, "Start Send HECI Message: \n"));

  SendPCR = (MKHI_BOOTLOADER_PCR_EXTEND_REQ_DATA*)DataBuffer;
  SendPCR->MkhiHeader.Fields.GroupId = MKHI_BOOTLOADER_PCR_EXTEND_GRP_ID;
  SendPCR->MkhiHeader.Fields.Command = MKHI_BOOTLOADER_PCR_EXTEND_CMD_REQ;
  if (Pcr) {
    CopyMem ((VOID*)SendPCR->Pcr, (VOID*)Pcr, MKHI_BOOTLOADER_PCR_LEN);
  }

  HeciSendLength = sizeof (MKHI_BOOTLOADER_PCR_EXTEND_REQ_DATA);
  HeciRecvLength = sizeof (DataBuffer);

  Status = HeciSendWack (
               DataBuffer,
               HeciSendLength,
               &HeciRecvLength,
               BIOS_FIXED_HOST_ADDR,
               0x7
            );

  PCRResp = (MKHI_BOOTLOADER_PCR_EXTEND_RESP_DATA*)DataBuffer;

  DEBUG ((DEBUG_INFO, "Group    =%08x\n", PCRResp->MkhiHeader.Fields.GroupId));
  DEBUG ((DEBUG_INFO, "Command  =%08x\n", PCRResp->MkhiHeader.Fields.Command));
  DEBUG ((DEBUG_INFO, "IsRespone=%08x\n", PCRResp->MkhiHeader.Fields.IsResponse));
  DEBUG ((DEBUG_INFO, "Result   =%08x\n", PCRResp->MkhiHeader.Fields.Result));

  return Status;
}

/**
  Prepare CSE for firmware update

  @return EFI_STATUS
**/
EFI_STATUS
PrepareCseForFirmwareUpdate (
  VOID
  )
{
  UINT8   DataBuffer[sizeof (HECI_RES_IFWI_PREPARE_FOR_UPDATE)];
  UINT32  HeciSendLength;
  UINT32  HeciRecvLength;
  UINT32  Status;
  HECI_REQ_IFWI_PREPARE_FOR_UPDATE *Request;
  HECI_RES_IFWI_PREPARE_FOR_UPDATE *Response;

  ZeroMem(DataBuffer, sizeof(DataBuffer));

  Request = (HECI_REQ_IFWI_PREPARE_FOR_UPDATE*)DataBuffer;
  Request->MkhiHeader.Fields.GroupId = 0x20;
  Request->MkhiHeader.Fields.Command = 0x01;

  Request->ResetType = 1;

  HeciSendLength = sizeof (HECI_REQ_IFWI_PREPARE_FOR_UPDATE) - 3;
  HeciRecvLength = sizeof (HECI_RES_IFWI_PREPARE_FOR_UPDATE);

  Status = HeciSendWack (
               DataBuffer,
               HeciSendLength,
               &HeciRecvLength,
               BIOS_FIXED_HOST_ADDR,
               0x7
            );

  if (EFI_ERROR(Status)) {
    return Status;
  }

  Response = (HECI_RES_IFWI_PREPARE_FOR_UPDATE*)DataBuffer;

  if (Response->MkhiHeader.Fields.Result != 0x0) {
    DEBUG ((EFI_D_ERROR, "Rejected request IFWI prepare update \n"));
    return EFI_ACCESS_DENIED;
  }

  if (Response->Flag == 0x02) {
    DEBUG ((EFI_D_ERROR, "HECI/CSE prepare for update failed \n"));
    return EFI_DEVICE_ERROR;
  }

  DEBUG ((EFI_D_ERROR, "HECI/CSE ready for update \n"));
  return EFI_SUCCESS;
}

/**
  Send Get Firmware Version Request to Sec

  @param[out] MsgGenGetFwVersionAck   Return FW version from CSE.

  @exception EFI_UNSUPPORTED      Current Sec mode doesn't support this function
  @retval EFI_SUCCESS             Command succeeded
  @retval EFI_DEVICE_ERROR        HECI Device error, command aborts abnormally
  @retval EFI_TIMEOUT             HECI does not return the buffer before timeout
  @retval EFI_BUFFER_TOO_SMALL    Message Buffer is too smallfor the Acknowledge
 **/
EFI_STATUS
EFIAPI
HeciGetFwVersionMsg (
  OUT UINT8     *MsgAck
)
{
  EFI_STATUS                Status;
  UINT32                    Length;
  UINT32                    HeciBar;
  GEN_GET_FW_VER            MsgGenGetFwCapsSku;
  GEN_GET_FW_VER_ACK        *MsgGenGetFwVersionAckData;
  MBP_CMD_RESP_DATA         *MBPHeader;
  MBP_ITEM_HEADER           *MBPItem;
  MBP_VERSION               *MBPVersion;

  if ( MsgAck == NULL ) {
    return EFI_INVALID_PARAMETER;
  }

  MsgGenGetFwVersionAckData = (GEN_GET_FW_VER_ACK *)MsgAck;

  MBPHeader = HeciMBP ();
  if ( MBPHeader != NULL) {
    MBPItem = (MBP_ITEM_HEADER *)(MBPHeader + 1);
    while ( (UINT32 *)MBPItem < (UINT32 *)MBPHeader + MBPHeader->Length) {
      if ( (MBPItem->AppID == MBP_APP_ID_KERNEL) && (MBPItem->ItemID == MBP_ITEM_ID_FW_VER_NAME)) {
        MBPVersion = (MBP_VERSION *)(MBPItem + 1);
        MsgGenGetFwVersionAckData->Data.CodeMinor   = MBPVersion->MinorVersion;
        MsgGenGetFwVersionAckData->Data.CodeMajor   = MBPVersion->MajorVersion;
        MsgGenGetFwVersionAckData->Data.CodeBuildNo = MBPVersion->BuildVersion;
        MsgGenGetFwVersionAckData->Data.CodeHotFix  = MBPVersion->HotfixVersion;
        return EFI_SUCCESS;
      }
      MBPItem = (MBP_ITEM_HEADER *)((UINT32 *)MBPItem + MBPItem->Length);

      //avoid infinite loop in case of corrupt data.
      if (MBPItem->Length == 0) {
        break;
      }
    }
  }

  //
  // Allocate MsgGenGetFwVersion data structure
  //
  MsgGenGetFwCapsSku.MKHIHeader.Data               = 0;
  MsgGenGetFwCapsSku.MKHIHeader.Fields.GroupId     = 0xFF;
  MsgGenGetFwCapsSku.MKHIHeader.Fields.Command     = 0x02;
  MsgGenGetFwCapsSku.MKHIHeader.Fields.IsResponse  = 0;

  Length = sizeof (GEN_GET_FW_VER);

  HeciBar = CheckAndFixHeciForAccess ();
  if (HeciBar == 0) {
    return EFI_NOT_FOUND;
  }

  //
  // Send Get FW SKU Request to SEC
  //
  Length = sizeof (GEN_GET_FW_VER);
  Status = HeciSend (
      HeciBar,
      (UINT8 *)&MsgGenGetFwCapsSku,
      Length,
      BIOS_FIXED_HOST_ADDR,
      0x7
      );

  if (EFI_ERROR (Status)) {
    return Status;
  }
  Length = sizeof (GEN_GET_FW_VER_ACK);
  Status = HeciReceive (
      HeciBar,
      (UINT8 *)MsgGenGetFwVersionAckData,
      &Length
      );

  if (EFI_ERROR (Status)) {
    return Status;
  }

  return EFI_SUCCESS;

}

/**
  Send Get Firmware SKU Request to Sec

  @param[in] MsgGenGetFwCapsSku      Send  message for Get Firmware Capability SKU
  @param[out] MsgGenGetFwCapsSkuAck  Return message for Get Firmware Capability SKU ACK

  @exception EFI_UNSUPPORTED        Current Sec mode doesn't support this function
  @retval EFI_SUCCESS               Command succeeded
  @retval EFI_DEVICE_ERROR          HECI Device error, command aborts abnormally
  @retval EFI_TIMEOUT               HECI does not return the buffer before timeout
  @retval EFI_BUFFER_TOO_SMALL      Message Buffer is too smallfor the Acknowledge
 **/
EFI_STATUS
EFIAPI
HeciGetFwCapsSkuMsg (
  IN UINT8  *MsgGetFwCaps,
  OUT UINT8  *MsgGetFwCapsAck
)
{
  EFI_STATUS                Status;
  UINT32                    Length;
  UINT32                    HeciBar;
  GEN_GET_FW_CAPSKU         *MsgGenGetFwCapsSku;
  GEN_GET_FW_CAPS_SKU_ACK   *MsgGenGetFwCapsSkuAck;
  MBP_CMD_RESP_DATA         *MBPHeader;
  MBP_ITEM_HEADER           *MBPItem;

  if ( (MsgGetFwCaps == NULL) && (MsgGetFwCapsAck == NULL) ) {
    return EFI_INVALID_PARAMETER;
  }
  MsgGenGetFwCapsSku = (GEN_GET_FW_CAPSKU *)MsgGetFwCaps;
  MsgGenGetFwCapsSkuAck = (GEN_GET_FW_CAPS_SKU_ACK *)MsgGetFwCapsAck;

  MBPHeader = HeciMBP ();
  if (MBPHeader != NULL) {
    MBPItem = (MBP_ITEM_HEADER *)(MBPHeader + 1);
    while ((UINT32 *)MBPItem < (UINT32 *)MBPHeader + MBPHeader->Length) {
      if ((MBPItem->AppID == MBP_APP_ID_KERNEL) && (MBPItem->ItemID == 2)) {
        MsgGenGetFwCapsSkuAck->Data.FWCap.Data = *(UINT32 *)(MBPItem + 1);
        DEBUG ((DEBUG_INFO, "MBP FWCAPS: 0x%08x\n\n", MsgGenGetFwCapsSkuAck->Data.FWCap.Data));
        return EFI_SUCCESS;
      }
      MBPItem = (MBP_ITEM_HEADER *)((UINT32 *)MBPItem + MBPItem->Length);

      //avoid infinite loop in case of corrupt data.
      if (MBPItem->Length == 0) {
        break;
      }
    }
  }
  //
  // Allocate MsgGenGetFwVersion data structure
  //
  MsgGenGetFwCapsSku->MKHIHeader.Data               = 0;
  MsgGenGetFwCapsSku->MKHIHeader.Fields.GroupId     = 0x03;
  MsgGenGetFwCapsSku->MKHIHeader.Fields.Command     = 0x02;
  MsgGenGetFwCapsSku->MKHIHeader.Fields.IsResponse  = 0x0;
  MsgGenGetFwCapsSku->Data.RuleId                   = 0x0;
  Length = sizeof (GEN_GET_FW_CAPSKU);

  HeciBar = CheckAndFixHeciForAccess ();
  if (HeciBar == 0) {
    return EFI_NOT_FOUND;
  }

  //
  // Send Get FW SKU Request to SEC
  //
  Status = HeciSend (
      HeciBar,
      (UINT8 *)MsgGenGetFwCapsSku,
      Length,
      BIOS_FIXED_HOST_ADDR,
      0x7
      );

  if (EFI_ERROR (Status)) {
    return Status;
  }

  Length = sizeof (GEN_GET_FW_CAPS_SKU_ACK);
  Status = HeciReceive (
      HeciBar,
      (UINT8 *)MsgGenGetFwCapsSkuAck,
      &Length
      );

  if (EFI_ERROR (Status)) {
    return Status;
  }
  return EFI_SUCCESS;
}


EFI_STATUS
EFIAPI
ReadHeciFwStatus (
  UINT32 *FwSts
)
{
  UINT32 HeciBar;

  HeciBar = CheckAndFixHeciForAccess ();
  if (HeciBar == 0) {
    return EFI_NOT_FOUND;
  }

  *FwSts = HeciPciRead32 (0x40);

  DEBUG ((DEBUG_VERBOSE, "HECI1 FW STATUS: 0x%08x\n\n", *FwSts));
  return EFI_SUCCESS;
}


/**
  Wrapper function to for the HECI command without parameter.

  @param[in] SubCmd           The HECI sub-command

  @retval EFI_SUCCESS         The sub command runs success
  @retval Others              The sub command doesn't run success

**/
EFI_STATUS
EFIAPI
SimpleHeciCommand (
  IN UINT32     SubCmd
)
{
  EFI_STATUS    Status;

  Status = EFI_UNSUPPORTED;

  switch (SubCmd) {
    case SUBCMD_ENTER_DNX_MODE:
      Status = EnterDnxMode ();
      break;
    case SUBCMD_DUMP_CSE_STATUS:
      Status = DumpCseStatus ();
      break;
    default:
      break;
  }

  return Status;
}

/**
  Register HECI service

  @retval EFI_SUCCESS             HECI service is registered successfully.
  @retval Others                  There is failure when register HECI service
**/
EFI_STATUS
EFIAPI
RegisterHeciService (
  VOID
  )
{
  EFI_STATUS      Status;

  Status = RegisterService ((VOID *)&mHeciService);

  return Status;
}

