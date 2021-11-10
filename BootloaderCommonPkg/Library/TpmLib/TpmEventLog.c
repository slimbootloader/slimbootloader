/** @file
  TPM Event Log library routines to provide TCG Event Log support.
  For more details, consult 'Event Logging' chapter in TCG  PC Client
  Platform Firmware Profile  specification.

  Copyright (c) 2017, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/


#include <Library/DebugLib.h>
#include <Library/BaseLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/CryptoLib.h>
#include <Library/BootloaderCommonLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/TpmLib.h>
#include <Library/PcdLib.h>
#include <Pi/PiBootMode.h>
#include <IndustryStandard/Tpm2Acpi.h>
#include "Tpm2CommandLib.h"
#include "TpmLibInternal.h"
#include "TpmEventLog.h"


/**
  Get the TCG log area info from TPM Lib Private data.

  @param[out] Lasa  Pointer to the Log area start address.
  @param[out] Laml  Log area minimum length.

**/
VOID
GetTCGLasa (
  OUT  UINT32  *Lasa,
  OUT  UINT32  *Laml
  )
{
  TPM_LIB_PRIVATE_DATA *TpmLibData;

  TpmLibData = TpmLibGetPrivateData ();

  if (TpmLibData != NULL) {
    *Lasa = (UINT32) (TpmLibData->LogAreaStartAddress);
    *Laml = TpmLibData->LogAreaMinLength;
  } else {
    *Lasa = 0;
    *Laml = 0;
  }
}



/**
  Get the size of the compressed TCG 2.0 event.

  @param[in]  EventHdr    address of the event header.

  @retval size of the event..
**/
UINT32
GetCompressedTCGEventSize (
  IN CONST TCG_PCR_EVENT2_HDR *EventHdr
  )
{
  UINT8  *Loc;
  UINT32 Count;
  UINT32 EventSize;
  TPMI_ALG_HASH HashAlg;

  Loc = (UINT8 *)EventHdr;
  // Get the number of digests available in this event
  CopyMem (&Count, &EventHdr->Digests.count, sizeof (Count));

  if (Count != 0) {
    Loc = (UINT8 *) & (EventHdr->Digests.digests[0].hashAlg);
    while (Count != 0) {
      // Get the HashAlgorithm
      CopyMem (&HashAlg, Loc, sizeof (TPMI_ALG_HASH));
      Loc += sizeof (TPMI_ALG_HASH);
      Loc += GetHashSizeFromAlgo (HashAlg);
      --Count;
    }

    // Get the event description size
    CopyMem (&EventSize, Loc, sizeof (EventSize));
    Loc += sizeof (EventSize) + EventSize;
  }

  return (UINT32)(Loc - (UINT8 *)EventHdr);
}


/**
  Get the size of the uncompressed TCG 2.0 event.

  @param[in]  EventHdr    address of the event header.

  @retval size of the event..
**/
UINT32
GetUnCompressedTCGEventSize (
  IN CONST TCG_PCR_EVENT2_HDR *EventHdr
  )
{
  UINT32 Count;
  UINT32 EventSize;

  EventSize = sizeof(*EventHdr) - sizeof(TPML_DIGEST_VALUES);

  EventSize += sizeof(EventHdr->Digests.count);
  for (Count = 0 ; Count < EventHdr->Digests.count; Count++) {
    EventSize += sizeof (TPMI_ALG_HASH);
    EventSize += GetHashSizeFromAlgo (EventHdr->Digests.digests[Count].hashAlg);
  }
  EventSize += EventHdr->EventSize;

  return EventSize;
}


/**
  Allocate and initialize TCG Event Log.

  @retval RETURN_SUCCESS     Operation completed successfully.
  @retval Others             Unable to create TCG event log.
**/
RETURN_STATUS
TpmTcgLogInit (
  VOID
  )
{
  UINT64                       Lasa;
  TPM_LIB_PRIVATE_DATA         *TpmLibData;


  Lasa = (UINT64) (UINTN) AllocatePages (EFI_SIZE_TO_PAGES (PcdGet32 (PcdTcgLogAreaMinLen)));
  if (Lasa != 0) {
    DEBUG ((DEBUG_INFO, "TCG Event Log created at 0x%8X \n", (UINT32)Lasa));

    // @todo UEFI BIOS fills the buffer with 0xFF. If we do this we will have to check for the
    // presence of 0xFFFFFFFF in EventSize to determine the location for new event while updating
    // TCG Event log
    ZeroMem ((VOID *)(UINTN)Lasa,  PcdGet32 (PcdTcgLogAreaMinLen));
    TpmLibSetEventLogData (Lasa, PcdGet32 (PcdTcgLogAreaMinLen));

    TpmLibData = TpmLibGetPrivateData ();

    if (TpmLibData != NULL) {
      TpmLogSpecIDEvent (TpmLibData->ActivePcrBanks);
    }
  } else {
    DEBUG ((DEBUG_WARN, "Unable to allocate memory for TCG Event log.\n"));
    return RETURN_OUT_OF_RESOURCES;
  }

  return RETURN_SUCCESS;
}



/**
  Add a new PCR event in TCG 2.0 log.

  @param[in]  Loc         memory address where event needs to be copied.
  @param[in]  EventHdr    header of the event.
  @param[in]  EventData   Description of the event.

**/
VOID
AddEventTCGLog (
  IN          UINT8              *Loc,
  IN  CONST   TCG_PCR_EVENT2_HDR *EventHdr,
  IN  CONST   UINT8              *EventData
  )
{
  TPMI_ALG_HASH HashAlg;
  UINT32 Idx;

  DEBUG ((DEBUG_INFO, "Adding event in TCG event log at : 0x%08X \n", Loc));

  // Copy the PCR index and event type
  CopyMem (Loc, EventHdr, sizeof (TCG_PCRINDEX) + sizeof (TCG_EVENTTYPE));
  Loc += sizeof (TCG_PCRINDEX) + sizeof (TCG_EVENTTYPE);

  // Copy the digest count
  CopyMem (Loc, & (EventHdr->Digests.count), sizeof (EventHdr->Digests.count));
  Loc += sizeof (EventHdr->Digests.count);

  for (Idx = 0 ; Idx < EventHdr->Digests.count; Idx++) {
    HashAlg =  EventHdr->Digests.digests[Idx].hashAlg;
    // Copy the HashAlgorithm
    CopyMem (Loc, &HashAlg, sizeof (TPMI_ALG_HASH));
    Loc += sizeof (TPMI_ALG_HASH);

    // Copy the hash
    CopyMem (Loc, & (EventHdr->Digests.digests[Idx].digest), GetHashSizeFromAlgo (HashAlg));
    Loc += GetHashSizeFromAlgo (HashAlg);
  }

  // Copy the event size and event description
  CopyMem (Loc, & (EventHdr->EventSize), sizeof (EventHdr->EventSize));
  Loc += sizeof (EventHdr->EventSize);

  CopyMem (Loc, EventData, EventHdr->EventSize);

  return;
}


/**
  Log the Specification ID Version event in TCG 2.0 Event Log.
  This event is not extended in PCR's.

  @param[in] ActivePcr  Currently Active PCR banks in TPM.

  @retval RETURN_SUCCESS     Operation completed successfully.
  @retval Others             Unable to log Spec ID event in TCG Event log.
**/
RETURN_STATUS
TpmLogSpecIDEvent (
  IN UINT32 ActivePcr
  )
{
  UINT32                                 Lasa;            //LogAreaStartAddress
  UINT32                                 Laml;            //LogAreaMinimumLength
  TCG_PCR_EVENT_HDR                      *FirstPcrEvent;
  TCG_EfiSpecIDEventStruct               *SpecIdEvent;
  TCG_EfiSpecIdEventAlgorithmSize        *DigestSize;
  UINT32                                 NumberOfAlgorithms;
  UINT8                                  *VendorInfoSize;

  GetTCGLasa (&Lasa, &Laml);

  if (Lasa == 0 || Laml == 0 ) {
    DEBUG ((DEBUG_WARN, "Unable to get log area for TCG 2.0 format events !!\n"));
    return RETURN_BUFFER_TOO_SMALL;
  }

  // Create the event in TPM 1.2 format
  FirstPcrEvent = (TCG_PCR_EVENT_HDR *)(UINTN)Lasa;
  FirstPcrEvent->PCRIndex = 0;
  FirstPcrEvent->EventType = EV_NO_ACTION;
  ZeroMem (FirstPcrEvent->Digest.digest, sizeof (FirstPcrEvent->Digest));

  SpecIdEvent = (TCG_EfiSpecIDEventStruct *) (Lasa + sizeof (*FirstPcrEvent));
  CopyMem (SpecIdEvent->signature, (VOID *)TCG_EfiSpecIDEventStruct_SIGNATURE_03, sizeof (SpecIdEvent->signature));
  SpecIdEvent->platformClass = 0;
  SpecIdEvent->specVersionMajor = TCG_EfiSpecIDEventStruct_SPEC_VERSION_MAJOR_TPM2;
  SpecIdEvent->specVersionMinor = TCG_EfiSpecIDEventStruct_SPEC_VERSION_MINOR_TPM2;
  SpecIdEvent->specErrata = TCG_EfiSpecIDEventStruct_SPEC_ERRATA_TPM2;
  SpecIdEvent->uintnSize = sizeof (UINTN) / sizeof (UINT32);
  NumberOfAlgorithms = 0;

  DigestSize = (TCG_EfiSpecIdEventAlgorithmSize *) ((UINT8 *)SpecIdEvent + sizeof (*SpecIdEvent) + sizeof (
                 NumberOfAlgorithms));

  if ((ActivePcr & HASH_ALG_SHA256) != 0) {
    DigestSize[NumberOfAlgorithms].algorithmId = TPM_ALG_SHA256;
    DigestSize[NumberOfAlgorithms].digestSize = SHA256_DIGEST_SIZE;
    NumberOfAlgorithms++;
  }

  if ((ActivePcr & HASH_ALG_SHA384) != 0) {
    DigestSize[NumberOfAlgorithms].algorithmId = TPM_ALG_SHA384;
    DigestSize[NumberOfAlgorithms].digestSize = SHA384_DIGEST_SIZE;
    NumberOfAlgorithms++;
  }

  if ((ActivePcr & HASH_ALG_SHA512) != 0) {
    DigestSize[NumberOfAlgorithms].algorithmId = TPM_ALG_SHA512;
    DigestSize[NumberOfAlgorithms].digestSize = SHA512_DIGEST_SIZE;
    NumberOfAlgorithms++;
  }

  if ((ActivePcr & HASH_ALG_SM3_256) != 0) {
    DigestSize[NumberOfAlgorithms].algorithmId = TPM_ALG_SM3_256;
    DigestSize[NumberOfAlgorithms].digestSize = SM3_DIGEST_SIZE;
    NumberOfAlgorithms++;
  }

  DigestSize = &DigestSize[NumberOfAlgorithms];

  CopyMem (SpecIdEvent + 1, &NumberOfAlgorithms, sizeof (NumberOfAlgorithms));

  VendorInfoSize = (UINT8 *)DigestSize;
  *VendorInfoSize = 0;

  FirstPcrEvent->EventSize = sizeof (*SpecIdEvent)
                             + sizeof (NumberOfAlgorithms)
                             + (NumberOfAlgorithms * sizeof (TCG_EfiSpecIdEventAlgorithmSize))
                             + sizeof (UINT8)
                             + *VendorInfoSize;

  return RETURN_SUCCESS;
}



/**
  Log a PCR event in TCG 2.0 format.

  @param[in] EventHdr      New event which needs to be logged.
  @param[in] EventData     New event data or description.

  @retval RETURN_SUCCESS   Operation completed successfully.
  @retval Others           Unable to log event in TCG Event log.
**/
RETURN_STATUS
TpmLogEvent (
  IN  CONST  TCG_PCR_EVENT2_HDR      *EventHdr,
  IN  CONST  UINT8                   *EventData
  )
{
  UINT32                Lasa;            //LogAreaStartAddress
  UINT32                Laml;            //LogAreaMinimumLength
  UINT32                EventSize;

  TCG_PCR_EVENT2_HDR *EmptySlot;
  TCG_PCR_EVENT_HDR *FirstEvent;

  GetTCGLasa (&Lasa, &Laml);

  if (Lasa == 0 || Laml == 0 ) {
    DEBUG ((DEBUG_WARN, "Unable to get log area for TCG 2.0 format events !!\n"));
    return RETURN_BUFFER_TOO_SMALL;
  }

  // Navigate log area to Locate the empty space for new event log
  // Note : First Event is of type TPM 1.2 (TCG_PCR_EVENT_HDR)

  FirstEvent = (TCG_PCR_EVENT_HDR *)(UINTN)Lasa;
  EmptySlot  = (TCG_PCR_EVENT2_HDR *)
               ((UINT8 *)FirstEvent + sizeof (TCG_PCR_EVENT_HDR) + FirstEvent->EventSize);

  while (EmptySlot < (TCG_PCR_EVENT2_HDR *)(UINTN)(Lasa + Laml - 1)) {
    EventSize = GetCompressedTCGEventSize (EmptySlot);
    if (EventSize == 0) {
      break;
    } else {
      EmptySlot = (TCG_PCR_EVENT2_HDR *) ((UINT8 *)EmptySlot + EventSize);
    }
  }

  // Before adding new event, check if there is enough space available.
  EventSize = GetUnCompressedTCGEventSize (EventHdr);

  if (((UINT8 *)EmptySlot + EventSize) <= (UINT8 *)(UINTN)(Lasa + Laml - 1)) {
    // Add the new event in the TCG 2.0 log area
    AddEventTCGLog ((UINT8 *)EmptySlot, EventHdr, EventData);
  } else {
    DEBUG ((DEBUG_WARN, "Insufficient space : Event not logged in TCG Event log!!\n"));
  }

  return RETURN_SUCCESS;
}

/**
  Log Startup Locality event in TCG event log.

  @param[in] StartupLocality  Locality from which TPM_Startup was performed.
**/
VOID
TpmLogLocalityEvent (
  IN UINT8 StartupLocality,
  IN UINT32 ActivePcrBanks
  )
{
  TCG_EfiStartupLocalityEvent     StartupLocalityEvent;
  TCG_PCR_EVENT2_HDR              PcrEventHdr;
  TPML_DIGEST_VALUES              *Digests;
  TPM_LIB_PRIVATE_DATA            *TpmLibData;

  CopyMem (StartupLocalityEvent.Signature, TCG_EfiStartupLocalityEvent_SIGNATURE,
           sizeof (StartupLocalityEvent.Signature));

  StartupLocalityEvent.StartupLocality = StartupLocality;

  PcrEventHdr.PCRIndex = 0;
  PcrEventHdr.EventType = EV_NO_ACTION;
  PcrEventHdr.EventSize = sizeof (StartupLocalityEvent);

  Digests = &PcrEventHdr.Digests;
  Digests->count = 0;

  TpmLibData = TpmLibGetPrivateData ();
  if (TpmLibData != NULL) {
    ActivePcrBanks = TpmLibData->ActivePcrBanks;
  }
  if ((ActivePcrBanks & HASH_ALG_SHA256) != 0) {
    Digests->digests[Digests->count].hashAlg = TPM_ALG_SHA256;
    ZeroMem (& (Digests->digests[Digests->count].digest), GetHashSizeFromAlgo (TPM_ALG_SHA256));
    Digests->count = Digests->count + 1;
  }
  if ((ActivePcrBanks & HASH_ALG_SHA384) != 0) {
    Digests->digests[Digests->count].hashAlg = TPM_ALG_SHA384;
    ZeroMem (& (Digests->digests[Digests->count].digest), GetHashSizeFromAlgo (TPM_ALG_SHA384));
    Digests->count = Digests->count + 1;
  }
  if ((ActivePcrBanks & HASH_ALG_SHA512) != 0) {
    Digests->digests[Digests->count].hashAlg = TPM_ALG_SHA512;
    ZeroMem (& (Digests->digests[Digests->count].digest), GetHashSizeFromAlgo (TPM_ALG_SHA512));
    Digests->count = Digests->count + 1;
  }
  if ((ActivePcrBanks & HASH_ALG_SM3_256) != 0) {
    Digests->digests[Digests->count].hashAlg = TPM_ALG_SM3_256;
    ZeroMem (& (Digests->digests[Digests->count].digest), GetHashSizeFromAlgo (TPM_ALG_SM3_256));
    Digests->count = Digests->count + 1;
  }

  TpmLogEvent ( &PcrEventHdr, (const UINT8 *)&StartupLocalityEvent);
}
