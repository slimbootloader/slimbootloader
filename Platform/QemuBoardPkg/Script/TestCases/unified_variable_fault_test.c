#include <PiPei.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <malloc.h>
#include <setjmp.h>

EFI_GUID gEfiAuthenticatedVariableGuid = {0xaaf32c78, 0x947b, 0x439a, {0xa1, 0x80, 0x2e, 0x14, 0x4e, 0xc3, 0x77, 0x92}};
EFI_GUID gEfiVariableGuid = {0xddcf3616, 0x3275, 0x4164, {0x98, 0xb6, 0xfe, 0x85, 0x70, 0x7f, 0xfe, 0x7d}};
EFI_GUID gEfiSystemNvDataFvGuid = {0xfff12b8d, 0x7696, 0x4c8b, {0xa9, 0x85, 0x27, 0x47, 0x07, 0x5b, 0x4f, 0x50}};
EFI_GUID gEdkiiWorkingBlockSignatureGuid = {0x9e58292b, 0x7c68, 0x497d, {0xa0, 0xce, 0x65, 0x00, 0xfd, 0x9f, 0x1b, 0x95}};

#include "../../../../BootloaderCommonPkg/Library/FullVariableLib/FullVariableLib.c"

#define TEST_FLASH_SIZE  0x40000
#define TEST_ATTRIBUTES  (EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS)

STATIC UINT8              *mFlash;
STATIC VOID               *mLibraryData;
STATIC SPI_FLASH_SERVICE  mService;

typedef enum {
  FaultNone,
  ErrorBefore,
  ErrorAfter,
  ErrorPartial,
  CorruptReadback,
  ResetBefore,
  ResetAfter,
  ResetPartial,
  FaultModeMax
} FAULT_MODE;

typedef struct {
  CHAR8   Kind;
  UINT32  Address;
  UINT32  Count;
} IO_OPERATION;

STATIC IO_OPERATION  mTrace[8192];
STATIC UINTN         mIoCount;
STATIC UINTN         mFaultAt;
STATIC FAULT_MODE    mFaultMode;
STATIC BOOLEAN       mFaultHit;
STATIC BOOLEAN       mRecording;
STATIC BOOLEAN       mCommitted;
STATIC BOOLEAN       mTargetFlushEntered;
STATIC BOOLEAN       mCacheFlushedAtFault;
STATIC BOOLEAN       mSpiAvailable = TRUE;
STATIC BOOLEAN       mComponentLookupFail;
STATIC jmp_buf       mReset;
STATIC UINT8         mOldValue[] = {0x44, 0x33, 0x22, 0x11};
STATIC UINT8         mNewValue[] = {0x88, 0x77, 0x66, 0x55};
STATIC UINT8         mAnchor[] = {1, 3, 5, 7};
STATIC UINT8         mCrossBlock[4600];

#define CHECK(Expression) do { if (!(Expression)) { printf ("FAIL line %u: %s\n", __LINE__, #Expression); exit (1); } } while (0)

VOID *EFIAPI CopyMem (VOID *Destination, CONST VOID *Source, UINTN Length) { return memmove (Destination, Source, Length); }
VOID *EFIAPI SetMem (VOID *Buffer, UINTN Length, UINT8 Value) { return memset (Buffer, Value, Length); }
VOID *EFIAPI ZeroMem (VOID *Buffer, UINTN Length) { return memset (Buffer, 0, Length); }
INTN EFIAPI CompareMem (CONST VOID *Left, CONST VOID *Right, UINTN Length) { return memcmp (Left, Right, Length); }
BOOLEAN EFIAPI CompareGuid (CONST GUID *Left, CONST GUID *Right) { return (BOOLEAN)(memcmp (Left, Right, sizeof (*Left)) == 0); }
GUID *EFIAPI CopyGuid (GUID *Destination, CONST GUID *Source) { return memcpy (Destination, Source, sizeof (*Source)); }
UINTN EFIAPI StrSize (CONST CHAR16 *String) { UINTN Length = 0; while (String[Length] != 0) { Length++; } return (Length + 1) * sizeof (CHAR16); }
VOID *EFIAPI AllocateZeroPool (UINTN Size) { return calloc (1, Size); }
VOID EFIAPI FreePool (VOID *Buffer) { free (Buffer); }
VOID EFIAPI MemoryFence (VOID) { }
VOID EFIAPI AsmFlushCacheRange (VOID *Address, UINTN Length) { (VOID)Address; (VOID)Length; mCacheFlushedAtFault = TRUE; }
VOID EFIAPI DebugPrint (UINTN ErrorLevel, CONST CHAR8 *Format, ...) { }
BOOLEAN EFIAPI DebugPrintLevelEnabled (CONST UINTN ErrorLevel) { return FALSE; }
UINT16 EFIAPI CalculateSum16 (CONST UINT16 *Buffer, UINTN Length) { UINT16 Sum = 0; UINTN Index; for (Index = 0; Index < Length / 2; Index++) { Sum = (UINT16)(Sum + Buffer[Index]); } return Sum; }
UINT16 EFIAPI CalculateCheckSum16 (CONST UINT16 *Buffer, UINTN Length) { return (UINT16)(0 - CalculateSum16 (Buffer, Length)); }

UINT32 EFIAPI CalculateCrc32 (CONST VOID *Buffer, UINTN Length)
{
  UINT32 Crc = MAX_UINT32;
  UINTN Index;
  UINTN Bit;
  for (Index = 0; Index < Length; Index++) {
    Crc ^= ((CONST UINT8 *)Buffer)[Index];
    for (Bit = 0; Bit < 8; Bit++) { Crc = (Crc >> 1) ^ ((Crc & 1) ? 0xEDB88320 : 0); }
  }
  return ~Crc;
}

EFI_STATUS EFIAPI GetLibraryData (UINT32 LibraryId, VOID **Buffer)
{
  *Buffer = mLibraryData;
  return (mLibraryData == NULL) ? EFI_NOT_FOUND : EFI_SUCCESS;
}

EFI_STATUS EFIAPI SetLibraryData (UINT32 LibraryId, VOID *Buffer, UINT32 Size)
{
  mLibraryData = Buffer;
  return EFI_SUCCESS;
}

EFI_STATUS EFIAPI GetComponentInfo (UINT32 Signature, UINT32 *Base, UINT32 *Size)
{
  (VOID)Signature;
  if (mComponentLookupFail) {
    return EFI_NOT_FOUND;
  }
  *Base = (UINT32)(UINTN)mFlash;
  *Size = TEST_FLASH_SIZE;
  return EFI_SUCCESS;
}

VOID *EFIAPI GetServiceBySignature (UINT32 Signature)
{
  (VOID)Signature;
  return mSpiAvailable ? &mService : NULL;
}

STATIC BOOLEAN BeginIo (CHAR8 Kind, UINT32 Address, UINT32 Count)
{
  FULL_VARIABLE_INSTANCE *Instance = GetFullVariableInstance ();
  if ((Instance != NULL) && Instance->TargetInconsistent) {
    mTargetFlushEntered = TRUE;
  }
  mIoCount++;
  if (mRecording) {
    CHECK (mIoCount <= ARRAY_SIZE (mTrace));
    mTrace[mIoCount - 1].Kind = Kind;
    mTrace[mIoCount - 1].Address = Address;
    mTrace[mIoCount - 1].Count = Count;
  }
  if ((mFaultAt != 0) && (mIoCount == mFaultAt)) {
    mFaultHit = TRUE;
    mCacheFlushedAtFault = FALSE;
    if (mFaultMode == ResetBefore) { longjmp (mReset, 1); }
    if (mFaultMode == ErrorBefore) { return FALSE; }
  }
  return TRUE;
}

STATIC EFI_STATUS EndIo (BOOLEAN Fault)
{
  if (Fault) {
    if ((mFaultMode == ResetAfter) || (mFaultMode == ResetPartial)) { longjmp (mReset, 1); }
    if ((mFaultMode == ErrorAfter) || (mFaultMode == ErrorPartial)) { return EFI_DEVICE_ERROR; }
  }
  return EFI_SUCCESS;
}

STATIC EFI_STATUS EFIAPI HostRead (FLASH_REGION_TYPE Region, UINT32 Address, UINT32 Count, UINT8 *Buffer)
{
  BOOLEAN Fault;
  CHECK (Address <= TEST_FLASH_SIZE && Count <= TEST_FLASH_SIZE - Address);
  if (!BeginIo ('R', Address, Count)) { return EFI_DEVICE_ERROR; }
  Fault = (BOOLEAN)((mFaultAt != 0) && (mIoCount == mFaultAt));
  memcpy (Buffer, mFlash + Address, Count);
  if (Fault && (mFaultMode == CorruptReadback)) { Buffer[0] ^= 1; }
  return EndIo (Fault);
}

STATIC EFI_STATUS EFIAPI HostWrite (FLASH_REGION_TYPE Region, UINT32 Address, UINT32 Count, UINT8 *Buffer)
{
  UINT32 Index;
  UINT32 Programmed;
  BOOLEAN Fault;
  CHECK (Address <= TEST_FLASH_SIZE && Count <= TEST_FLASH_SIZE - Address);
  if (!BeginIo ('W', Address, Count)) { return EFI_DEVICE_ERROR; }
  Fault = (BOOLEAN)((mFaultAt != 0) && (mIoCount == mFaultAt));
  Programmed = (Fault && ((mFaultMode == ErrorPartial) || (mFaultMode == ResetPartial))) ?
               MAX (1, Count / 2) : Count;
  for (Index = 0; Index < Programmed; Index++) {
    CHECK ((mFlash[Address + Index] & Buffer[Index]) == Buffer[Index]);
    mFlash[Address + Index] &= Buffer[Index];
  }
  if ((Address == 0x1E048) && (Count == 1) && (Buffer[0] == 0xFD)) { mCommitted = TRUE; }
  return EndIo (Fault);
}

STATIC EFI_STATUS EFIAPI HostErase (FLASH_REGION_TYPE Region, UINT32 Address, UINT32 Count)
{
  BOOLEAN Fault;
  UINT32 Erased;
  CHECK ((Address % SIZE_4KB) == 0 && (Count % SIZE_4KB) == 0);
  CHECK (Address <= TEST_FLASH_SIZE && Count <= TEST_FLASH_SIZE - Address);
  if (!BeginIo ('E', Address, Count)) { return EFI_DEVICE_ERROR; }
  Fault = (BOOLEAN)((mFaultAt != 0) && (mIoCount == mFaultAt));
  Erased = (Fault && ((mFaultMode == ErrorPartial) || (mFaultMode == ResetPartial))) ? Count / 2 : Count;
  memset (mFlash + Address, 0xFF, Erased);
  return EndIo (Fault);
}

STATIC EFI_STATUS EFIAPI HostRegion (FLASH_REGION_TYPE Region, UINT32 *Base, UINT32 *Size)
{
  if (Base != NULL) { *Base = 0; }
  if (Size != NULL) { *Size = 0 - (UINT32)(UINTN)mFlash; }
  return EFI_SUCCESS;
}

STATIC VOID Reboot (VOID)
{
  mFaultAt = 0;
  mFaultMode = FaultNone;
  mRecording = FALSE;
  mSpiAvailable = TRUE;
  mComponentLookupFail = FALSE;
  free (mLibraryData);
  mLibraryData = NULL;
  CHECK (VariableConstructor ((UINT32)(UINTN)mFlash, TEST_FLASH_SIZE) == EFI_SUCCESS);
}

STATIC VOID VerifyValue (CONST CHAR16 *Name, CONST UINT8 *Expected, UINTN ExpectedSize)
{
  UINT8 Buffer[5120];
  UINTN Size = sizeof (Buffer);
  UINT32 Attributes = 0;
  CHECK (ExpectedSize <= sizeof (Buffer));
  CHECK (GetVariable ((CHAR16 *)Name, &gEfiVariableGuid, &Attributes, &Size, Buffer) == EFI_SUCCESS);
  CHECK (Attributes == TEST_ATTRIBUTES && Size == ExpectedSize);
  CHECK (memcmp (Buffer, Expected, Size) == 0);
}

STATIC VOID PrepareFixture (BOOLEAN WorkspaceFull)
{
  FULL_VARIABLE_FTW_WRITE_HEADER Header;
  FULL_VARIABLE_FTW_WRITE_RECORD Record;
  UINTN Offset;
  mFaultAt = 0;
  mRecording = FALSE;
  memset (mFlash, 0xFF, TEST_FLASH_SIZE);
  Reboot ();
  CHECK (SetVariable (L"HostMarker", &gEfiVariableGuid, TEST_ATTRIBUTES, sizeof (mOldValue), mOldValue) == EFI_SUCCESS);
  CHECK (SetVariable (L"CrossBlock", &gEfiVariableGuid, TEST_ATTRIBUTES, sizeof (mCrossBlock), mCrossBlock) == EFI_SUCCESS);
  CHECK (SetVariable (L"Anchor", &gEfiVariableGuid, TEST_ATTRIBUTES, sizeof (mAnchor), mAnchor) == EFI_SUCCESS);
  mFlash[0x1DFFF] = 0;
  if (WorkspaceFull) {
    memset (&Header, 0xFF, sizeof (Header));
    Header.HeaderAllocated = 0;
    Header.WritesAllocated = 0;
    Header.Complete = 0;
    Header.NumberOfWrites = 1;
    Header.PrivateDataSize = 0;
    memset (&Record, 0xFF, sizeof (Record));
    Record.SpareComplete = 0;
    Record.DestinationComplete = 0;
    for (Offset = 32; Offset + sizeof (Header) + sizeof (Record) <= 0x2000;
         Offset += sizeof (Header) + sizeof (Record)) {
      memcpy (mFlash + 0x1E000 + Offset, &Header, sizeof (Header));
      memcpy (mFlash + 0x1E000 + Offset + sizeof (Header), &Record, sizeof (Record));
    }
  }
  Reboot ();
}

STATIC EFI_STATUS ChangeMarker (BOOLEAN Append)
{
  return SetVariable (L"HostMarker", &gEfiVariableGuid,
                      TEST_ATTRIBUTES | (Append ? EFI_VARIABLE_APPEND_WRITE : 0),
                      sizeof (mNewValue), mNewValue);
}

STATIC VOID VerifyRecovered (BOOLEAN Append, BOOLEAN Committed)
{
  UINT8 Combined[sizeof (mOldValue) + sizeof (mNewValue)];
  memcpy (Combined, mOldValue, sizeof (mOldValue));
  memcpy (Combined + sizeof (mOldValue), mNewValue, sizeof (mNewValue));
  VerifyValue (L"Anchor", mAnchor, sizeof (mAnchor));
  VerifyValue (L"CrossBlock", mCrossBlock, sizeof (mCrossBlock));
  if (!Committed) {
    VerifyValue (L"HostMarker", mOldValue, sizeof (mOldValue));
  } else if (Append) {
    VerifyValue (L"HostMarker", Combined, sizeof (Combined));
  } else {
    VerifyValue (L"HostMarker", mNewValue, sizeof (mNewValue));
  }
}

STATIC VOID VerifyInBootAfterReclaimError (BOOLEAN Append)
{
  UINT8 ReadBuf[5120];
  CHAR16 NameBuf[64];
  EFI_GUID Guid;
  UINTN DataSize;
  UINTN NameSize;
  UINT64 MaxStorage;
  UINT64 RemStorage;
  UINT64 MaxVar;
  FULL_VARIABLE_INSTANCE *Instance = GetFullVariableInstance ();

  CHECK (Instance != NULL && Instance->WriteFailed);
  if (Instance->TargetInconsistent) {
    DataSize = sizeof (ReadBuf);
    CHECK (GetVariable (L"HostMarker", &gEfiVariableGuid, NULL, &DataSize, ReadBuf) == EFI_DEVICE_ERROR);
    DataSize = sizeof (ReadBuf);
    CHECK (GetVariable (L"CrossBlock", &gEfiVariableGuid, NULL, &DataSize, ReadBuf) == EFI_DEVICE_ERROR);
    NameSize = sizeof (NameBuf);
    NameBuf[0] = L'\0';
    CHECK (GetNextVariableName (&NameSize, NameBuf, &Guid) == EFI_DEVICE_ERROR);
    CHECK (QueryVariableInfo (TEST_ATTRIBUTES, &MaxStorage, &RemStorage, &MaxVar) == EFI_DEVICE_ERROR);
    CHECK (VariableConstructor (0, 0) == EFI_DEVICE_ERROR);
    CHECK (Instance->WriteFailed && Instance->TargetInconsistent);
  } else {
    VerifyRecovered (Append, mTargetFlushEntered);
    NameSize = sizeof (NameBuf);
    NameBuf[0] = L'\0';
    CHECK (GetNextVariableName (&NameSize, NameBuf, &Guid) == EFI_SUCCESS);
    CHECK (QueryVariableInfo (TEST_ATTRIBUTES, &MaxStorage, &RemStorage, &MaxVar) == EFI_SUCCESS);
  }
}

STATIC VOID SweepFaults (BOOLEAN Append, BOOLEAN WorkspaceFull)
{
  UINT8 *Snapshot = malloc (TEST_FLASH_SIZE);
  UINTN OperationCount;
  UINTN Operation;
  UINTN Cases = 0;
  UINTN BeforeRetry;
  FAULT_MODE Mode;
  EFI_STATUS Status;
  BOOLEAN Committed;
  CHECK (Snapshot != NULL);
  PrepareFixture (WorkspaceFull);
  memcpy (Snapshot, mFlash, TEST_FLASH_SIZE);
  mRecording = TRUE;
  mIoCount = 0;
  mCommitted = FALSE;
  mTargetFlushEntered = FALSE;
  CHECK (ChangeMarker (Append) == EFI_SUCCESS);
  CHECK (mCommitted);
  OperationCount = mIoCount;
  mRecording = FALSE;
  VerifyRecovered (Append, TRUE);
  for (Operation = 1; Operation <= OperationCount; Operation++) {
    for (Mode = ErrorBefore; Mode < FaultModeMax; Mode++) {
      if ((Mode == CorruptReadback) && (mTrace[Operation - 1].Kind != 'R')) { continue; }
      if (((Mode == ErrorPartial) || (Mode == ResetPartial)) &&
          (mTrace[Operation - 1].Kind == 'R')) { continue; }
      memcpy (mFlash, Snapshot, TEST_FLASH_SIZE);
      Reboot ();
      mIoCount = 0;
      mCommitted = FALSE;
      mTargetFlushEntered = FALSE;
      mFaultHit = FALSE;
      mFaultMode = Mode;
      mFaultAt = Operation;
      if (setjmp (mReset) == 0) {
        Status = ChangeMarker (Append);
        CHECK (Mode < ResetBefore);
        CHECK (Status == EFI_DEVICE_ERROR);
        CHECK (mCacheFlushedAtFault);
        CHECK (GetFullVariableInstance ()->WriteFailed);
        mFaultAt = 0;
        mFaultMode = FaultNone;
        BeforeRetry = mIoCount;
        CHECK (EFI_ERROR (ChangeMarker (Append)));
        VerifyInBootAfterReclaimError (Append);
        CHECK (mIoCount == BeforeRetry);
      } else {
        CHECK (Mode >= ResetBefore);
      }
      CHECK (mFaultHit);
      Committed = mCommitted;
      Reboot ();
      VerifyRecovered (Append, Committed);
      Reboot ();
      VerifyRecovered (Append, Committed);
      CHECK (ChangeMarker (Append) == EFI_SUCCESS);
      Cases++;
    }
  }
  printf ("Native SPI fault sweep PASSED: %s%s, %u operations, %u faults\n",
          WorkspaceFull ? "workspace + " : "", Append ? "append" : "update",
          (unsigned)OperationCount, (unsigned)Cases);
  free (Snapshot);
}

STATIC VOID FillLiveStore (UINTN FreeBytes)
{
  UEFI_VARIABLE_STORE_INFO StoreInfo;
  UEFI_VARIABLE_STORE_HEADER *Store;
  UEFI_VARIABLE_HEADER *Variable;
  UEFI_VARIABLE_HEADER *Header;
  UEFI_AUTHENTICATED_VARIABLE_HEADER Padding;
  CHAR16 Name[] = L"Padding0";
  UINTN Remaining;
  UINTN EntrySize;
  Store = GetVariableStore (VariableStoreTypeNv, &StoreInfo);
  Variable = GetStartPointer (Store);
  while (GetVariableHeader (&StoreInfo, Variable, &Header)) {
    Variable = GetNextVariablePtr (&StoreInfo, Variable, Header);
  }
  Remaining = (UINTN)GetEndPointer (Store) - (UINTN)Variable - FreeBytes;
  while (Remaining != 0) {
    EntrySize = MIN (SIZE_64KB, Remaining);
    CHECK (EntrySize > sizeof (Padding) + StrSize (Name));
    memset (&Padding, 0, sizeof (Padding));
    Padding.StartId = UEFI_VARIABLE_DATA;
    Padding.State = UEFI_VAR_ADDED;
    Padding.Attributes = TEST_ATTRIBUTES;
    Padding.NameSize = (UINT32)StrSize (Name);
    Padding.DataSize = (UINT32)(EntrySize - sizeof (Padding) - Padding.NameSize);
    Padding.VendorGuid = gEfiVariableGuid;
    memcpy (Variable, &Padding, sizeof (Padding));
    memcpy ((UINT8 *)Variable + sizeof (Padding), Name, Padding.NameSize);
    memset ((UINT8 *)Variable + sizeof (Padding) + Padding.NameSize, 0x5A, Padding.DataSize);
    Variable = (VOID *)((UINT8 *)Variable + EntrySize);
    Remaining -= EntrySize;
    Name[7]++;
  }
}

STATIC VOID VerifyPadding (VOID)
{
  UEFI_VARIABLE_STORE_INFO StoreInfo;
  UEFI_VARIABLE_STORE_HEADER *Store;
  UEFI_VARIABLE_HEADER *Variable;
  UEFI_VARIABLE_HEADER *Header;
  UINTN Index;
  UINTN Count = 0;
  UINT8 *Data;
  Store = GetVariableStore (VariableStoreTypeNv, &StoreInfo);
  Variable = GetStartPointer (Store);
  while (GetVariableHeader (&StoreInfo, Variable, &Header)) {
    if ((Header->State == UEFI_VAR_ADDED) && (GetVariableNamePtr (Variable, TRUE)[0] == L'P')) {
      CHECK (Header->Attributes == TEST_ATTRIBUTES);
      Data = GetVariableDataPtr (Variable, Header, TRUE);
      for (Index = 0; Index < DataSizeOfVariable (Header, TRUE); Index++) { CHECK (Data[Index] == 0x5A); }
      Count++;
    }
    Variable = GetNextVariablePtr (&StoreInfo, Variable, Header);
  }
  CHECK (Count == 2);
  VerifyValue (L"Anchor", mAnchor, sizeof (mAnchor));
}

STATIC VOID NearFullTests (VOID)
{
  UINT8 Initial[260];
  UINT8 Added[260];
  UINT8 Combined[520];
  UINT8 TooLarge[524];
  UINT8 *Snapshot = malloc (TEST_FLASH_SIZE);
  UINTN Operation;
  UINTN DestinationErase = 0;
  UINT64 Maximum;
  UINT64 Remaining;
  UINT64 MaximumVariable;
  CHECK (Snapshot != NULL);
  PrepareFixture (FALSE);
  memset (mFlash + 100, 0xFF, 0x1E000 - 100);
  memset (Initial, 0x55, sizeof (Initial));
  memset (Added, 0xAA, sizeof (Added));
  memcpy (Combined, Initial, sizeof (Initial));
  memcpy (Combined + sizeof (Initial), Added, sizeof (Added));
  memset (TooLarge, 0x33, sizeof (TooLarge));
  CHECK (SetVariable (L"HostMarker", &gEfiVariableGuid, TEST_ATTRIBUTES, sizeof (Initial), Initial) == EFI_SUCCESS);
  CHECK (SetVariable (L"Anchor", &gEfiVariableGuid, TEST_ATTRIBUTES, sizeof (mAnchor), mAnchor) == EFI_SUCCESS);
  FillLiveStore (sizeof (Added));
  CHECK (QueryVariableInfo (TEST_ATTRIBUTES, &Maximum, &Remaining, &MaximumVariable) == EFI_SUCCESS);
  CHECK (Remaining == sizeof (Added));
  memcpy (Snapshot, mFlash, TEST_FLASH_SIZE);
  mIoCount = 0;
  CHECK (SetVariable (L"HostMarker", &gEfiVariableGuid, TEST_ATTRIBUTES, sizeof (TooLarge), TooLarge) == EFI_OUT_OF_RESOURCES);
  CHECK (mIoCount == 0 && memcmp (Snapshot, mFlash, TEST_FLASH_SIZE) == 0);
  VerifyValue (L"HostMarker", Initial, sizeof (Initial));
  VerifyPadding ();
  puts ("Near-full growing-update rejection leaves flash unchanged PASSED");
  mRecording = TRUE;
  mIoCount = 0;
  mCommitted = FALSE;
  CHECK (SetVariable (L"HostMarker", &gEfiVariableGuid, TEST_ATTRIBUTES | EFI_VARIABLE_APPEND_WRITE,
                      sizeof (Added), Added) == EFI_SUCCESS);
  mRecording = FALSE;
  CHECK (mCommitted);
  for (Operation = 0; Operation < mIoCount; Operation++) {
    if ((mTrace[Operation].Kind == 'E') && (mTrace[Operation].Address == 0)) {
      DestinationErase = Operation + 1;
      break;
    }
  }
  CHECK (DestinationErase != 0);
  VerifyValue (L"HostMarker", Combined, sizeof (Combined));
  VerifyPadding ();
  puts ("Near-full append using only final-live-set capacity PASSED");
  memcpy (mFlash, Snapshot, TEST_FLASH_SIZE);
  Reboot ();
  mIoCount = 0;
  mCommitted = FALSE;
  mFaultHit = FALSE;
  mFaultMode = ResetBefore;
  mFaultAt = DestinationErase;
  if (setjmp (mReset) == 0) {
    SetVariable (L"HostMarker", &gEfiVariableGuid, TEST_ATTRIBUTES | EFI_VARIABLE_APPEND_WRITE,
                 sizeof (Added), Added);
    CHECK (FALSE);
  }
  CHECK (mFaultHit && mCommitted);
  Reboot ();
  VerifyValue (L"HostMarker", Combined, sizeof (Combined));
  VerifyPadding ();
  Reboot ();
  VerifyValue (L"HostMarker", Combined, sizeof (Combined));
  VerifyPadding ();
  puts ("Near-full appended prefix/suffix recovery and attributes PASSED");
  free (Snapshot);
}

STATIC VOID UncommittedWorkspaceTests (VOID)
{
  FULL_VARIABLE_FTW_WORKING_BLOCK_HEADER Header;
  UINT8 *Snapshot = malloc (TEST_FLASH_SIZE);
  UINTN Fixture;
  UINTN Operation;
  UINTN CommitOperation;
  UINTN BadCase;
  UINTN BeforeReboot;

  CHECK (Snapshot != NULL);
  for (Fixture = 0; Fixture < 2; Fixture++) {
    if (Fixture == 0) {
      memset (mFlash, 0xFF, TEST_FLASH_SIZE);
    } else {
      PrepareFixture (FALSE);
      memset (mFlash + 0x1E000, 0xFF, 0x2000);
    }
    memcpy (Snapshot, mFlash, TEST_FLASH_SIZE);
    free (mLibraryData);
    mLibraryData = NULL;
    mRecording = TRUE;
    mIoCount = 0;
    CHECK (VariableConstructor ((UINT32)(UINTN)mFlash, TEST_FLASH_SIZE) == EFI_SUCCESS);
    mRecording = FALSE;
    CommitOperation = 0;
    for (Operation = 0; Operation < mIoCount; Operation++) {
      if ((mTrace[Operation].Kind == 'W') && (mTrace[Operation].Address == 0x1E014) &&
          (mTrace[Operation].Count == 1)) {
        CommitOperation = Operation + 1;
        break;
      }
    }
    CHECK (CommitOperation != 0);

    memcpy (mFlash, Snapshot, TEST_FLASH_SIZE);
    free (mLibraryData);
    mLibraryData = NULL;
    mIoCount = 0;
    mFaultHit = FALSE;
    mFaultMode = ResetBefore;
    mFaultAt = CommitOperation;
    if (setjmp (mReset) == 0) {
      VariableConstructor ((UINT32)(UINTN)mFlash, TEST_FLASH_SIZE);
      CHECK (FALSE);
    }
    CHECK (mFaultHit);
    BuildWorkspace (&Header);
    CHECK (memcmp (mFlash + 0x1E000, &Header, sizeof (Header)) == 0);
    CHECK (IsErased (mFlash + 0x1E000 + sizeof (Header), 0x2000 - sizeof (Header)));
    memcpy (Snapshot, mFlash, TEST_FLASH_SIZE);
    Snapshot[0x1E014] = 0xFE;
    mIoCount = 0;
    Reboot ();
    CHECK (mIoCount == 2);
    CHECK (memcmp (Snapshot, mFlash, TEST_FLASH_SIZE) == 0);
    CHECK (ValidWorkspace ((VOID *)(mFlash + 0x1E000)));
    if (Fixture != 0) {
      VerifyRecovered (FALSE, FALSE);
    }
    BeforeReboot = mIoCount;
    Reboot ();
    CHECK (mIoCount == BeforeReboot);
  }

  for (BadCase = 0; BadCase < 7; BadCase++) {
    PrepareFixture (FALSE);
    BuildWorkspace (&Header);
    memset (mFlash + 0x1E000, 0xFF, 0x2000);
    memcpy (mFlash + 0x1E000, &Header, sizeof (Header));
    switch (BadCase) {
      case 0: mFlash[0x1E010] ^= 1; break;
      case 1: mFlash[0x1E000] ^= 1; break;
      case 2: mFlash[0x1E018] ^= 1; break;
      case 3: mFlash[0x1E015] = 0; break;
      case 4: mFlash[0x1E020] = 0; break;
      case 5: mFlash[0x1FFFF] = 0; break;
      case 6: mFlash[0x1E014] = 0xFC; break;
    }
    memcpy (Snapshot, mFlash, TEST_FLASH_SIZE);
    free (mLibraryData);
    mLibraryData = NULL;
    mIoCount = 0;
    CHECK (VariableConstructor ((UINT32)(UINTN)mFlash, TEST_FLASH_SIZE) == EFI_VOLUME_CORRUPTED);
    CHECK (mIoCount == 0 && memcmp (Snapshot, mFlash, TEST_FLASH_SIZE) == 0);
    CHECK (VariableConstructor (0, 0) == EFI_VOLUME_CORRUPTED);
    CHECK (mIoCount == 0);
  }
  free (Snapshot);
  puts ("Interrupted workspace valid-bit commit and corrupt-header rejection PASSED");
}

STATIC VOID ConstructorFaultTests (VOID)
{
  UINT8 *Snapshot = malloc (TEST_FLASH_SIZE);
  UINT8 ReadBuf[64];
  CHAR16 NameBuf[64];
  EFI_GUID Guid;
  UINTN Size;
  UINTN NameSize;
  UINT64 MaxStorage;
  UINT64 RemStorage;
  UINT64 MaxVar;
  UINTN Operation;
  UINTN OperationCount;
  UINTN TargetEraseOp;
  UINTN BeforeRetry;
  UINTN Cases = 0;
  UINTN Which;
  FAULT_MODE Mode;
  EFI_STATUS Status;

  CHECK (Snapshot != NULL);

  memset (mFlash, 0xFF, TEST_FLASH_SIZE);
  free (mLibraryData);
  mLibraryData = NULL;
  mSpiAvailable = FALSE;
  mComponentLookupFail = TRUE;
  CHECK (VariableConstructor ((UINT32)(UINTN)mFlash, TEST_FLASH_SIZE) == EFI_NOT_AVAILABLE_YET);
  CHECK (!GetFullVariableInstance ()->WriteFailed && !GetFullVariableInstance ()->Initialized &&
         !GetFullVariableInstance ()->TargetInconsistent);
  Size = sizeof (ReadBuf);
  CHECK (GetVariable (L"HostMarker", &gEfiVariableGuid, NULL, &Size, ReadBuf) == EFI_NOT_AVAILABLE_YET);
  mSpiAvailable = TRUE;
  CHECK (SetVariable (L"HostMarker", &gEfiVariableGuid, TEST_ATTRIBUTES, sizeof (mOldValue), mOldValue) == EFI_SUCCESS);
  VerifyValue (L"HostMarker", mOldValue, sizeof (mOldValue));
  CHECK (QueryVariableInfo (0, &MaxStorage, &RemStorage, &MaxVar) == EFI_INVALID_PARAMETER);
  CHECK (QueryVariableInfo (EFI_VARIABLE_NON_VOLATILE, &MaxStorage, &RemStorage, &MaxVar) == EFI_INVALID_PARAMETER);
  CHECK (QueryVariableInfo (EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_HARDWARE_ERROR_RECORD, &MaxStorage, &RemStorage, &MaxVar) == EFI_INVALID_PARAMETER);
  CHECK (QueryVariableInfo (EFI_VARIABLE_BOOTSERVICE_ACCESS, &MaxStorage, &RemStorage, &MaxVar) == EFI_UNSUPPORTED);
  mComponentLookupFail = FALSE;

  mFlash[0] ^= 0xFF;
  free (mLibraryData);
  mLibraryData = NULL;
  CHECK (VariableConstructor ((UINT32)(UINTN)mFlash, TEST_FLASH_SIZE) == EFI_VOLUME_CORRUPTED);
  BeforeRetry = mIoCount;
  Size = sizeof (ReadBuf);
  CHECK (GetVariable (L"HostMarker", &gEfiVariableGuid, NULL, &Size, ReadBuf) == EFI_VOLUME_CORRUPTED);
  CHECK (SetVariable (L"HostMarker", &gEfiVariableGuid, TEST_ATTRIBUTES, sizeof (mOldValue), mOldValue) == EFI_VOLUME_CORRUPTED);
  NameSize = sizeof (NameBuf);
  NameBuf[0] = L'\0';
  CHECK (GetNextVariableName (&NameSize, NameBuf, &Guid) == EFI_DEVICE_ERROR);
  CHECK (QueryVariableInfo (TEST_ATTRIBUTES, &MaxStorage, &RemStorage, &MaxVar) == EFI_DEVICE_ERROR);
  CHECK (VariableConstructor (0, 0) == EFI_VOLUME_CORRUPTED);
  CHECK (VariableConstructor ((UINT32)(UINTN)mFlash, TEST_FLASH_SIZE) == EFI_VOLUME_CORRUPTED);
  CHECK (mIoCount == BeforeRetry);

  for (Which = 0; Which < 2; Which++) {
    PrepareFixture (FALSE);
    if (Which == 0) {
      memcpy (Snapshot, mFlash, TEST_FLASH_SIZE);
      mRecording = TRUE;
      mIoCount = 0;
      CHECK (ChangeMarker (FALSE) == EFI_SUCCESS);
      mRecording = FALSE;
      TargetEraseOp = 0;
      for (Operation = 0; Operation < mIoCount; Operation++) {
        if ((mTrace[Operation].Kind == 'E') && (mTrace[Operation].Address == SIZE_4KB)) {
          TargetEraseOp = Operation + 1;
          break;
        }
      }
      CHECK (TargetEraseOp != 0);
      memcpy (mFlash, Snapshot, TEST_FLASH_SIZE);
      Reboot ();
      mIoCount = 0;
      mFaultHit = FALSE;
      mFaultMode = ResetBefore;
      mFaultAt = TargetEraseOp;
      if (setjmp (mReset) == 0) {
        ChangeMarker (FALSE);
        CHECK (FALSE);
      }
      CHECK (mFaultHit);
    } else {
      memcpy (mFlash + 0x20000, mFlash + 0x1E000, 0x2000);
      mFlash[0x1E000 + 20] &= 0xFD;
    }
    memcpy (Snapshot, mFlash, TEST_FLASH_SIZE);
    mFaultAt = 0;
    mFaultMode = FaultNone;
    free (mLibraryData);
    mLibraryData = NULL;
    mSpiAvailable = FALSE;
    mComponentLookupFail = TRUE;
    CHECK (VariableConstructor ((UINT32)(UINTN)mFlash, TEST_FLASH_SIZE) == EFI_NOT_AVAILABLE_YET);
    CHECK (!GetFullVariableInstance ()->WriteFailed && !GetFullVariableInstance ()->Initialized &&
           !GetFullVariableInstance ()->TargetInconsistent);
    Size = sizeof (ReadBuf);
    CHECK (GetVariable (L"HostMarker", &gEfiVariableGuid, NULL, &Size, ReadBuf) == EFI_NOT_AVAILABLE_YET);
    CHECK (!GetFullVariableInstance ()->TargetInconsistent);
    mSpiAvailable = TRUE;
    VerifyRecovered (FALSE, Which == 0);
    mComponentLookupFail = FALSE;

    memcpy (mFlash, Snapshot, TEST_FLASH_SIZE);
    free (mLibraryData);
    mLibraryData = NULL;
    mRecording = TRUE;
    mIoCount = 0;
    CHECK (VariableConstructor ((UINT32)(UINTN)mFlash, TEST_FLASH_SIZE) == EFI_SUCCESS);
    OperationCount = mIoCount;
    mRecording = FALSE;
    CHECK (OperationCount > 0);
    VerifyRecovered (FALSE, Which == 0);

    for (Operation = 1; Operation <= OperationCount; Operation++) {
      for (Mode = ErrorBefore; Mode < FaultModeMax; Mode++) {
        if ((Mode == CorruptReadback) && (mTrace[Operation - 1].Kind != 'R')) { continue; }
        if (((Mode == ErrorPartial) || (Mode == ResetPartial)) &&
            (mTrace[Operation - 1].Kind == 'R')) { continue; }
        memcpy (mFlash, Snapshot, TEST_FLASH_SIZE);
        free (mLibraryData);
        mLibraryData = NULL;
        mIoCount = 0;
        mFaultHit = FALSE;
        mFaultMode = Mode;
        mFaultAt = Operation;
        if (setjmp (mReset) == 0) {
          Status = VariableConstructor ((UINT32)(UINTN)mFlash, TEST_FLASH_SIZE);
          CHECK (Mode < ResetBefore);
          CHECK (Status == EFI_DEVICE_ERROR);
          CHECK (mCacheFlushedAtFault);
          CHECK (GetFullVariableInstance ()->WriteFailed && !GetFullVariableInstance ()->Initialized);
          mFaultAt = 0;
          mFaultMode = FaultNone;
          BeforeRetry = mIoCount;
          Size = sizeof (ReadBuf);
          CHECK (GetVariable (L"HostMarker", &gEfiVariableGuid, NULL, &Size, ReadBuf) == EFI_DEVICE_ERROR);
          CHECK (SetVariable (L"HostMarker", &gEfiVariableGuid, TEST_ATTRIBUTES, sizeof (mNewValue), mNewValue) == EFI_DEVICE_ERROR);
          NameSize = sizeof (NameBuf);
          NameBuf[0] = L'\0';
          CHECK (GetNextVariableName (&NameSize, NameBuf, &Guid) == EFI_DEVICE_ERROR);
          CHECK (QueryVariableInfo (TEST_ATTRIBUTES, &MaxStorage, &RemStorage, &MaxVar) == EFI_DEVICE_ERROR);
          CHECK (VariableConstructor (0, 0) == EFI_DEVICE_ERROR);
          CHECK (VariableConstructor ((UINT32)(UINTN)mFlash, TEST_FLASH_SIZE) == EFI_DEVICE_ERROR);
          CHECK (GetFullVariableInstance ()->WriteFailed && !GetFullVariableInstance ()->Initialized);
          CHECK (mIoCount == BeforeRetry);
        } else {
          CHECK (Mode >= ResetBefore);
        }
        CHECK (mFaultHit);
        Reboot ();
        VerifyRecovered (FALSE, Which == 0);
        Cases++;
      }
    }
  }
  printf ("Constructor latching and recovery SPI fault sweep PASSED: %u faults\n", (unsigned)Cases);
  free (Snapshot);
}

STATIC VOID AppendRawAddedRecord (CONST CHAR16 *Name, CONST UINT8 *Data, UINT32 DataSize)
{
  UEFI_VARIABLE_STORE_INFO StoreInfo;
  UEFI_VARIABLE_STORE_HEADER *Store;
  UEFI_VARIABLE_HEADER *Variable;
  UEFI_VARIABLE_HEADER *Header;
  UEFI_AUTHENTICATED_VARIABLE_HEADER Raw;
  UINT32 NameSize = (UINT32)StrSize (Name);
  UINTN EntrySize = HEADER_ALIGN (sizeof (Raw) + NameSize + DataSize);

  Store = GetVariableStore (VariableStoreTypeNv, &StoreInfo);
  CHECK (Store != NULL);
  Variable = GetStartPointer (Store);
  while (GetVariableHeader (&StoreInfo, Variable, &Header)) {
    Variable = GetNextVariablePtr (&StoreInfo, Variable, Header);
  }
  CHECK ((UINTN)Variable + EntrySize <= (UINTN)GetEndPointer (Store));
  memset (&Raw, 0, sizeof (Raw));
  Raw.StartId = UEFI_VARIABLE_DATA;
  Raw.State = UEFI_VAR_ADDED;
  Raw.Attributes = TEST_ATTRIBUTES;
  Raw.NameSize = NameSize;
  Raw.DataSize = DataSize;
  Raw.VendorGuid = gEfiVariableGuid;
  memcpy (Variable, &Raw, sizeof (Raw));
  memcpy ((UINT8 *)Variable + sizeof (Raw), Name, NameSize);
  memcpy ((UINT8 *)Variable + sizeof (Raw) + NameSize, Data, DataSize);
}

STATIC VOID VerifyDuplicateConsistency (BOOLEAN Append, CONST UINT8 *FirstDup)
{
  UINT8 Combined[8];
  UINT8 ReadBuf[64];
  UINTN ReadSize = sizeof (ReadBuf);
  UINT32 Attributes = 0;
  UEFI_VARIABLE_STORE_INFO StoreInfo;
  UEFI_VARIABLE_STORE_HEADER *Store;
  UEFI_VARIABLE_HEADER *Variable;
  UEFI_VARIABLE_HEADER *Header;
  UEFI_VARIABLE_HEADER *Edk2Selected = NULL;
  UEFI_VARIABLE_HEADER *Edk2Fallback = NULL;
  UINTN Edk2Size;
  UINT8 *Edk2Data;

  memcpy (Combined, FirstDup, 4);
  memcpy (Combined + 4, mNewValue, 4);

  CHECK (GetVariable (L"HostMarker", &gEfiVariableGuid, &Attributes, &ReadSize, ReadBuf) == EFI_SUCCESS);
  CHECK (Attributes == TEST_ATTRIBUTES);
  if (Append) {
    CHECK ((ReadSize == 4 && memcmp (ReadBuf, FirstDup, 4) == 0) ||
           (ReadSize == 8 && memcmp (ReadBuf, Combined, 8) == 0));
  } else {
    CHECK (ReadSize == 4 &&
           (memcmp (ReadBuf, FirstDup, 4) == 0 || memcmp (ReadBuf, mNewValue, 4) == 0));
  }

  Store = (UEFI_VARIABLE_STORE_HEADER *)(mFlash + FULL_VARIABLE_FV_HEADER_SIZE);
  StoreInfo.VariableStoreHeader = Store;
  StoreInfo.AuthFlag = TRUE;
  Variable = GetStartPointer (Store);
  while (GetVariableHeader (&StoreInfo, Variable, &Header)) {
    if ((Header->State == UEFI_VAR_ADDED) ||
        (Header->State == (UEFI_VAR_IN_DELETED_TRANSITION & UEFI_VAR_ADDED))) {
      if ((NameSizeOfVariable (Header, TRUE) == StrSize (L"HostMarker")) &&
          CompareGuid (GetVendorGuidPtr (Header, TRUE), &gEfiVariableGuid) &&
          (memcmp (GetVariableNamePtr (Variable, TRUE), L"HostMarker", StrSize (L"HostMarker")) == 0)) {
        if (Header->State == UEFI_VAR_ADDED) {
          Edk2Selected = Variable;
          break;
        }
        Edk2Fallback = Variable;
      }
    }
    Variable = GetNextVariablePtr (&StoreInfo, Variable, Header);
  }
  if (Edk2Selected == NULL) {
    Edk2Selected = Edk2Fallback;
  }
  CHECK (Edk2Selected != NULL);
  Edk2Size = DataSizeOfVariable (Edk2Selected, TRUE);
  Edk2Data = GetVariableDataPtr (Edk2Selected, Edk2Selected, TRUE);
  CHECK (Edk2Size == ReadSize && memcmp (Edk2Data, ReadBuf, ReadSize) == 0);
  VerifyValue (L"Anchor", mAnchor, sizeof (mAnchor));
}

STATIC VOID DuplicateNonReclaimTests (VOID)
{
  CONST UINT8 FirstDup[4] = {0xDD, 0xCC, 0xBB, 0xAA};
  CONST UINT8 StaleDup[4] = {0x88, 0x77, 0x66, 0x55};
  UINT8 *Snapshot = malloc (TEST_FLASH_SIZE);
  UINTN AppendPass;
  UINTN Operation;
  UINTN OperationCount;
  UINTN Cases = 0;
  FAULT_MODE Mode;
  EFI_STATUS Status;

  CHECK (Snapshot != NULL);
  for (AppendPass = 0; AppendPass < 2; AppendPass++) {
    BOOLEAN Append = (BOOLEAN)(AppendPass != 0);
    memset (mFlash, 0xFF, TEST_FLASH_SIZE);
    Reboot ();
    CHECK (SetVariable (L"Anchor", &gEfiVariableGuid, TEST_ATTRIBUTES, sizeof (mAnchor), mAnchor) == EFI_SUCCESS);
    AppendRawAddedRecord (L"HostMarker", FirstDup, sizeof (FirstDup));
    AppendRawAddedRecord (L"HostMarker", StaleDup, sizeof (StaleDup));
    Reboot ();
    VerifyValue (L"HostMarker", FirstDup, sizeof (FirstDup));
    memcpy (Snapshot, mFlash, TEST_FLASH_SIZE);

    mRecording = TRUE;
    mIoCount = 0;
    mCommitted = FALSE;
    CHECK (ChangeMarker (Append) == EFI_SUCCESS);
    CHECK (!mCommitted);
    OperationCount = mIoCount;
    mRecording = FALSE;
    VerifyDuplicateConsistency (Append, FirstDup);

    for (Operation = 1; Operation <= OperationCount; Operation++) {
      for (Mode = ErrorBefore; Mode < FaultModeMax; Mode++) {
        if ((Mode == CorruptReadback) && (mTrace[Operation - 1].Kind != 'R')) { continue; }
        if (((Mode == ErrorPartial) || (Mode == ResetPartial)) &&
            (mTrace[Operation - 1].Kind == 'R')) { continue; }
        memcpy (mFlash, Snapshot, TEST_FLASH_SIZE);
        Reboot ();
        mIoCount = 0;
        mFaultHit = FALSE;
        mFaultMode = Mode;
        mFaultAt = Operation;
        if (setjmp (mReset) == 0) {
          Status = ChangeMarker (Append);
          CHECK (Mode < ResetBefore);
          CHECK (Status == EFI_DEVICE_ERROR);
          CHECK (mCacheFlushedAtFault);
          mFaultAt = 0;
          mFaultMode = FaultNone;
          VerifyDuplicateConsistency (Append, FirstDup);
        } else {
          CHECK (Mode >= ResetBefore);
        }
        CHECK (mFaultHit);
        Reboot ();
        VerifyDuplicateConsistency (Append, FirstDup);
        Cases++;
      }
    }
  }
  printf ("Duplicate-ADDED non-reclaim fault sweep PASSED: %u faults\n", (unsigned)Cases);
  free (Snapshot);
}

STATIC VOID WriteMalformedTailRecord (CONST VOID *NameBytes, UINT32 NameSize, UINT32 DataSize)
{
  UEFI_VARIABLE_STORE_INFO StoreInfo;
  UEFI_VARIABLE_STORE_HEADER *Store;
  UEFI_VARIABLE_HEADER *Variable;
  UEFI_VARIABLE_HEADER *Header;
  UEFI_AUTHENTICATED_VARIABLE_HEADER Raw;
  UINTN EntrySize = HEADER_ALIGN (sizeof (Raw) + NameSize + DataSize);

  Store = GetVariableStore (VariableStoreTypeNv, &StoreInfo);
  CHECK (Store != NULL);
  Variable = GetStartPointer (Store);
  while (GetVariableHeader (&StoreInfo, Variable, &Header)) {
    Variable = GetNextVariablePtr (&StoreInfo, Variable, Header);
  }
  CHECK ((UINTN)Variable + EntrySize <= (UINTN)GetEndPointer (Store));
  memset (&Raw, 0, sizeof (Raw));
  Raw.StartId = UEFI_VARIABLE_DATA;
  Raw.State = UEFI_VAR_ADDED;
  Raw.Attributes = TEST_ATTRIBUTES;
  Raw.NameSize = NameSize;
  Raw.DataSize = DataSize;
  Raw.VendorGuid = gEfiVariableGuid;
  memcpy (Variable, &Raw, sizeof (Raw));
  if (NameSize > 0) {
    memcpy ((UINT8 *)Variable + sizeof (Raw), NameBytes, NameSize);
  }
  if (DataSize > 0) {
    memset ((UINT8 *)Variable + sizeof (Raw) + NameSize, 0xA5, DataSize);
  }
}

STATIC VOID VerifyEnumerationOrder (VOID)
{
  CONST CHAR16 *ExpectedNames[3] = {L"Alpha", L"BetaLongName", L"Gamma"};
  CHAR16 NameBuf[32];
  CHAR16 TinyBuf[4];
  EFI_GUID Guid;
  UINTN NameSize;
  UINTN Index;

  memset (NameBuf, 0, sizeof (NameBuf));
  for (Index = 0; Index < 3; Index++) {
    if (Index == 1) {
      memcpy (TinyBuf, NameBuf, sizeof (TinyBuf));
      NameSize = sizeof (TinyBuf);
      CHECK (GetNextVariableName (&NameSize, TinyBuf, &Guid) == EFI_INVALID_PARAMETER);
      NameSize = StrSize (NameBuf);
      CHECK (GetNextVariableName (&NameSize, NameBuf, &Guid) == EFI_BUFFER_TOO_SMALL);
      CHECK (NameSize == StrSize (ExpectedNames[Index]));
    }
    NameSize = sizeof (NameBuf);
    CHECK (GetNextVariableName (&NameSize, NameBuf, &Guid) == EFI_SUCCESS);
    CHECK (NameSize == StrSize (ExpectedNames[Index]));
    CHECK (CompareGuid (&Guid, &gEfiVariableGuid));
    CHECK (memcmp (NameBuf, ExpectedNames[Index], NameSize) == 0);
  }

  NameSize = sizeof (NameBuf);
  CHECK (GetNextVariableName (&NameSize, NameBuf, &Guid) == EFI_NOT_FOUND);

  memcpy (NameBuf, L"Missing", sizeof (L"Missing"));
  NameSize = sizeof (NameBuf);
  CHECK (GetNextVariableName (&NameSize, NameBuf, &Guid) == EFI_INVALID_PARAMETER);
}

STATIC VOID EnumerationAndMalformedRecordTests (VOID)
{
  CONST CHAR16 EmptyName[] = {L'\0'};
  CONST CHAR16 EmbeddedNulName[] = {L'B', L'a', L'\0', L'd', L'\0'};
  CONST CHAR16 ValidName[] = L"ZeroData";
  UINT8 ValA[4] = {1, 2, 3, 4};
  UINT8 ValB[4] = {5, 6, 7, 8};
  UINT8 ValC[4] = {9, 10, 11, 12};
  UINTN BadCase;

  for (BadCase = 0; BadCase < 3; BadCase++) {
    memset (mFlash, 0xFF, TEST_FLASH_SIZE);
    Reboot ();
    CHECK (SetVariable (L"Alpha", &gEfiVariableGuid, TEST_ATTRIBUTES, sizeof (ValA), ValA) == EFI_SUCCESS);
    CHECK (SetVariable (L"Obsolete", &gEfiVariableGuid, TEST_ATTRIBUTES, sizeof (ValB), ValB) == EFI_SUCCESS);
    CHECK (SetVariable (L"BetaLongName", &gEfiVariableGuid, TEST_ATTRIBUTES, sizeof (ValA), ValA) == EFI_SUCCESS);
    CHECK (SetVariable (L"BetaLongName", &gEfiVariableGuid, TEST_ATTRIBUTES, sizeof (ValB), ValB) == EFI_SUCCESS);
    CHECK (SetVariable (L"Obsolete", &gEfiVariableGuid, 0, 0, NULL) == EFI_SUCCESS);
    CHECK (SetVariable (L"Gamma", &gEfiVariableGuid, TEST_ATTRIBUTES, sizeof (ValC), ValC) == EFI_SUCCESS);
    AppendRawAddedRecord (L"BetaLongName", ValA, sizeof (ValA));

    VerifyEnumerationOrder ();

    if (BadCase == 0) {
      WriteMalformedTailRecord (EmptyName, sizeof (EmptyName), sizeof (ValA));
    } else if (BadCase == 1) {
      WriteMalformedTailRecord (EmbeddedNulName, sizeof (EmbeddedNulName), sizeof (ValA));
    } else {
      WriteMalformedTailRecord (ValidName, sizeof (ValidName), 0);
    }

    VerifyEnumerationOrder ();
    mCommitted = FALSE;
    CHECK (SetVariable (L"Alpha", &gEfiVariableGuid, TEST_ATTRIBUTES, sizeof (ValC), ValC) == EFI_SUCCESS);
    CHECK (mCommitted);
    {
      CONST CHAR16 *ReclaimedOrder[3] = {L"BetaLongName", L"Gamma", L"Alpha"};
      CHAR16 NameBuf[32];
      EFI_GUID Guid;
      UINTN NameSize;
      UINTN Index;
      memset (NameBuf, 0, sizeof (NameBuf));
      for (Index = 0; Index < 3; Index++) {
        NameSize = sizeof (NameBuf);
        CHECK (GetNextVariableName (&NameSize, NameBuf, &Guid) == EFI_SUCCESS);
        CHECK (NameSize == StrSize (ReclaimedOrder[Index]));
        CHECK (memcmp (NameBuf, ReclaimedOrder[Index], NameSize) == 0);
      }
      NameSize = sizeof (NameBuf);
      CHECK (GetNextVariableName (&NameSize, NameBuf, &Guid) == EFI_NOT_FOUND);
    }
    VerifyValue (L"Alpha", ValC, sizeof (ValC));
    VerifyValue (L"BetaLongName", ValB, sizeof (ValB));
    VerifyValue (L"Gamma", ValC, sizeof (ValC));
  }
  puts ("Enumeration walk and malformed-record rejection PASSED");
}

int main (void)
{
  UINTN Index;
  UINT32 Value = 0x11223344;
  UINT32 ReadValue = 0;
  UINTN Size = sizeof (ReadValue);
  for (Index = 0; Index < sizeof (mCrossBlock); Index++) {
    mCrossBlock[Index] = (UINT8)((Index * 37 + 13) & 0xFF);
  }
  mFlash = _aligned_malloc (TEST_FLASH_SIZE, SIZE_4KB);
  CHECK (mFlash != NULL);
  memset (mFlash, 0xFF, TEST_FLASH_SIZE);
  mService.SpiRead = HostRead;
  mService.SpiWrite = HostWrite;
  mService.SpiErase = HostErase;
  mService.SpiGetRegion = HostRegion;
  CHECK (VariableConstructor ((UINT32)(UINTN)mFlash, TEST_FLASH_SIZE) == EFI_SUCCESS);
  CHECK (SetVariable (L"HostMarker", &gEfiVariableGuid, TEST_ATTRIBUTES, sizeof (Value), &Value) == EFI_SUCCESS);
  CHECK (GetVariable (L"HostMarker", &gEfiVariableGuid, NULL, &Size, &ReadValue) == EFI_SUCCESS);
  CHECK (ReadValue == Value && Size == sizeof (Value));
  EnumerationAndMalformedRecordTests ();
  NearFullTests ();
  UncommittedWorkspaceTests ();
  ConstructorFaultTests ();
  DuplicateNonReclaimTests ();
  SweepFaults (FALSE, FALSE);
  SweepFaults (TRUE, FALSE);
  SweepFaults (FALSE, TRUE);
  free (mLibraryData);
  _aligned_free (mFlash);
  puts ("Unified variable native SPI failure and capacity tests PASSED");
  return 0;
}