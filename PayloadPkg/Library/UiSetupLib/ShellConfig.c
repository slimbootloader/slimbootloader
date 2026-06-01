/** @file
  Shell helpers for UiSetup-backed config access.

  Copyright (c) 2026, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent
**/

#include "Include/UiCommon.h"
#include "ConfigBridge.h"
#include "FormEngine.h"
#include <Library/ConsoleOutLib.h>
#include <Library/PcdLib.h>
#if defined(__has_include)
#if __has_include("UiDescData.h")
#include "UiDescData.h"
#else
STATIC CONST UINT8 mUiDescBin[] = { 0 };
#endif
#else
#include "UiDescData.h"
#endif

// UiCommon.h already pulls PrintLib; map ShellPrint to the same console sink.
#define ShellPrint      ConsolePrintUnicode


#define UI_SHELL_MAX_VALUE_CHARS  128

STATIC
EFI_STATUS
UiShellInit (
  VOID
  )
{
  EFI_STATUS  Status;

  if (!FeaturePcdGet (PcdUiSetupEnabled)) {
    return EFI_UNSUPPORTED;
  }

  Status = FormInit (mUiDescBin, sizeof (mUiDescBin));
  if (EFI_ERROR (Status)) {
    return Status;
  }

  return CfgLoad ();
}

STATIC
CHAR8 *
UnicodeToAsciiAlloc (
  IN CONST CHAR16  *String
  )
{
  CHAR8   *Ascii;
  UINTN    Size;

  if (String == NULL) {
    return NULL;
  }

  Size = StrLen (String) + 1;
  Ascii = AllocateZeroPool (Size);
  if (Ascii == NULL) {
    return NULL;
  }

  if (EFI_ERROR (UnicodeStrToAsciiStrS (String, Ascii, Size))) {
    FreePool (Ascii);
    return NULL;
  }

  return Ascii;
}

STATIC
CHAR8
AsciiToLowerChar (
  IN CHAR8  Value
  )
{
  if ((Value >= 'A') && (Value <= 'Z')) {
    return (CHAR8)(Value - 'A' + 'a');
  }
  return Value;
}

STATIC
BOOLEAN
AsciiCharEqualInsensitive (
  IN CHAR8  Left,
  IN CHAR8  Right
  )
{
  return (BOOLEAN)(AsciiToLowerChar (Left) == AsciiToLowerChar (Right));
}

STATIC
BOOLEAN
AsciiContainsInsensitive (
  IN CONST CHAR8  *String,
  IN CONST CHAR8  *Pattern
  )
{
  UINTN  Index;
  UINTN  Match;

  if ((String == NULL) || (Pattern == NULL)) {
    return FALSE;
  }

  if (*Pattern == '\0') {
    return TRUE;
  }

  for (Index = 0; String[Index] != '\0'; Index++) {
    for (Match = 0; Pattern[Match] != '\0'; Match++) {
      if ((String[Index + Match] == '\0') || !AsciiCharEqualInsensitive (String[Index + Match], Pattern[Match])) {
        break;
      }
    }
    if (Pattern[Match] == '\0') {
      return TRUE;
    }
  }

  return FALSE;
}

STATIC
EFI_STATUS
ParseAsciiUintn (
  IN  CONST CHAR8  *Text,
  OUT UINTN        *Value
  )
{
  UINTN          Result;
  CHAR8         *End;
  RETURN_STATUS  Ret;

  if ((Text == NULL) || (Value == NULL) || (*Text == '\0')) {
    return EFI_INVALID_PARAMETER;
  }

  if ((Text[0] == '0') && ((Text[1] == 'x') || (Text[1] == 'X'))) {
    Ret = AsciiStrHexToUintnS (Text, &End, &Result);
  } else {
    Ret = AsciiStrDecimalToUintnS (Text, &End, &Result);
  }

  if (RETURN_ERROR (Ret)) {
    return EFI_INVALID_PARAMETER;
  }

  if ((End == NULL) || (*End != '\0')) {
    return EFI_INVALID_PARAMETER;
  }

  *Value = Result;
  return EFI_SUCCESS;
}

STATIC
EFI_STATUS
ParseFieldPathIndex (
  IN  CONST CHAR16  *FieldArg,
  OUT CHAR8        **BasePath,
  OUT BOOLEAN       *HasIndex,
  OUT UINTN         *Index
  )
{
  CHAR8  *Path;
  CHAR8  *Lbr;
  CHAR8  *Rbr;

  if ((FieldArg == NULL) || (BasePath == NULL) || (HasIndex == NULL) || (Index == NULL)) {
    return EFI_INVALID_PARAMETER;
  }

  Path = UnicodeToAsciiAlloc (FieldArg);
  if (Path == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  *HasIndex = FALSE;
  *Index = 0;

  Lbr = AsciiStrStr (Path, "[");
  if (Lbr == NULL) {
    *BasePath = Path;
    return EFI_SUCCESS;
  }

  Rbr = AsciiStrStr (Lbr, "]");
  if ((Rbr == NULL) || (Rbr[1] != '\0')) {
    FreePool (Path);
    return EFI_INVALID_PARAMETER;
  }

  *Lbr = '\0';
  *Rbr = '\0';
  if (*(Lbr + 1) == '\0') {
    FreePool (Path);
    return EFI_INVALID_PARAMETER;
  }

  if (EFI_ERROR (ParseAsciiUintn (Lbr + 1, Index))) {
    FreePool (Path);
    return EFI_INVALID_PARAMETER;
  }

  *HasIndex = TRUE;
  *BasePath = Path;
  return EFI_SUCCESS;
}

STATIC
EFI_STATUS
BuildTableElementField (
  IN  UI_FIELD_ENTRY   *Field,
  IN  BOOLEAN           HasIndex,
  IN  UINTN             Index,
  OUT UI_FIELD_ENTRY   *ElementField,
  OUT BOOLEAN          *UseElementField
  )
{
  UINT16  ElemBits;
  UINT16  ElemCount;

  if ((Field == NULL) || (ElementField == NULL) || (UseElementField == NULL)) {
    return EFI_INVALID_PARAMETER;
  }

  *UseElementField = FALSE;
  *ElementField = *Field;

  if (Field->FieldType != UI_FIELD_TYPE_TABLE) {
    if (HasIndex) {
      return EFI_INVALID_PARAMETER;
    }
    return EFI_SUCCESS;
  }

  ElemBits  = Field->OptionStartIdx;
  ElemCount = Field->OptionCount;
  if ((ElemBits == 0) || (ElemCount == 0)) {
    return EFI_UNSUPPORTED;
  }

  if (!HasIndex) {
    return EFI_SUCCESS;
  }

  if (Index >= ElemCount) {
    return EFI_NOT_FOUND;
  }

  ElementField->BitLength = ElemBits;
  ElementField->BitOffset = (UINT16)(Field->BitOffset + (UINT16)(Index * ElemBits));
  ElementField->FieldType = UI_FIELD_TYPE_EDITNUM;
  *UseElementField = TRUE;
  return EFI_SUCCESS;
}

STATIC
EFI_STATUS
ParseNumericValue (
  IN  CONST CHAR16  *ValueString,
  OUT UINT32        *Value
  )
{
  EFI_STATUS    Status;
  UINTN         Parsed;
  CHAR16       *End;
  CONST CHAR16 *Walker;

  if ((ValueString == NULL) || (Value == NULL) || (*ValueString == L'\0')) {
    return EFI_INVALID_PARAMETER;
  }

  Walker = ValueString;
  if ((Walker[0] == L'0') && ((Walker[1] == L'x') || (Walker[1] == L'X'))) {
    Status = StrHexToUintnS (Walker, &End, &Parsed);
  } else {
    Status = StrDecimalToUintnS (Walker, &End, &Parsed);
  }

  if (EFI_ERROR (Status) || (*End != L'\0') || (Parsed > MAX_UINT32)) {
    return EFI_INVALID_PARAMETER;
  }

  *Value = (UINT32)Parsed;
  return EFI_SUCCESS;
}

STATIC
EFI_STATUS
ParseNumericValue64 (
  IN  CONST CHAR16  *ValueString,
  OUT UINT64        *Value
  )
{
  EFI_STATUS    Status;
  CHAR16       *End;
  CONST CHAR16 *Walker;

  if ((ValueString == NULL) || (Value == NULL) || (*ValueString == L'\0')) {
    return EFI_INVALID_PARAMETER;
  }

  Walker = ValueString;
  if ((Walker[0] == L'0') && ((Walker[1] == L'x') || (Walker[1] == L'X'))) {
    Status = StrHexToUint64S (Walker, &End, Value);
  } else {
    Status = StrDecimalToUint64S (Walker, &End, Value);
  }

  if (EFI_ERROR (Status) || (*End != L'\0')) {
    return EFI_INVALID_PARAMETER;
  }

  return EFI_SUCCESS;
}

STATIC
UINT64
ReadBitField64 (
  IN CONST UINT8  *Data,
  IN UINT16        BitOffset,
  IN UINT16        BitLength
  )
{
  UINT64  Value;
  UINT32  ByteOff;
  UINT32  BitOff;
  UINT32  Idx;

  if ((Data == NULL) || (BitLength == 0) || (BitLength > 64)) {
    return 0;
  }

  Value = 0;
  for (Idx = 0; Idx < BitLength; Idx++) {
    ByteOff = (BitOffset + Idx) / 8;
    BitOff  = (BitOffset + Idx) % 8;
    if (Data[ByteOff] & (1U << BitOff)) {
      Value |= LShiftU64 (1ULL, Idx);
    }
  }

  return Value;
}

STATIC
VOID
WriteBitField64 (
  IN UINT8   *Data,
  IN UINT16   BitOffset,
  IN UINT16   BitLength,
  IN UINT64   Value
  )
{
  UINT32  ByteOff;
  UINT32  BitOff;
  UINT32  Idx;

  if ((Data == NULL) || (BitLength == 0) || (BitLength > 64)) {
    return;
  }

  for (Idx = 0; Idx < BitLength; Idx++) {
    ByteOff = (BitOffset + Idx) / 8;
    BitOff  = (BitOffset + Idx) % 8;
    if (Value & LShiftU64 (1ULL, Idx)) {
      Data[ByteOff] |= (UINT8)(1U << BitOff);
    } else {
      Data[ByteOff] &= (UINT8)~(1U << BitOff);
    }
  }
}

STATIC
BOOLEAN
IsFieldByteAligned (
  IN UI_FIELD_ENTRY  *Field
  )
{
  return (BOOLEAN)((Field->BitOffset % 8 == 0) && (Field->BitLength % 8 == 0));
}

STATIC
UINT32
PackBytesToUint32 (
  IN CONST UINT8  *Bytes,
  IN UINTN         ByteCount
  )
{
  UINT32  Value;
  UINTN   Index;

  Value = 0;
  for (Index = 0; Index < ByteCount && Index < sizeof (UINT32); Index++) {
    Value |= ((UINT32)Bytes[Index]) << (Index * 8);
  }

  return Value;
}

STATIC
EFI_STATUS
ApplyTextValue (
  IN  UI_FIELD_ENTRY  *Field,
  IN  UINT8           *TagData,
  IN  CONST CHAR16    *ValueString,
  OUT UINT64          *PackedValue
  )
{
  CHAR8  *AsciiValue;
  UINT8  *FieldBytes;
  UINTN   ByteCount;
  UINTN   Length;

  if ((Field == NULL) || (TagData == NULL) || (ValueString == NULL) || (PackedValue == NULL)) {
    return EFI_INVALID_PARAMETER;
  }

  if ((Field->BitLength > 32) || !IsFieldByteAligned (Field)) {
    return EFI_UNSUPPORTED;
  }

  ByteCount = Field->BitLength / 8;
  AsciiValue = UnicodeToAsciiAlloc (ValueString);
  if (AsciiValue == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  Length = AsciiStrLen (AsciiValue);
  if (Length > ByteCount) {
    FreePool (AsciiValue);
    return EFI_BAD_BUFFER_SIZE;
  }

  FieldBytes = TagData + (Field->BitOffset / 8);
  ZeroMem (FieldBytes, ByteCount);
  if (Length > 0) {
    CopyMem (FieldBytes, AsciiValue, Length);
  }

  *PackedValue = (UINT64)PackBytesToUint32 (FieldBytes, ByteCount);
  FreePool (AsciiValue);
  return EFI_SUCCESS;
}

STATIC
EFI_STATUS
ResolveFieldValue (
  IN  UI_FIELD_ENTRY  *Field,
  IN  CONST CHAR16    *ValueString,
  IN  UINT8           *TagData,
  OUT UINT64          *Value,
  OUT BOOLEAN         *UsedDirectWrite
  )
{
  EFI_STATUS       Status;
  CHAR8           *AsciiValue;
  UI_OPTION_ENTRY *Options;
  UINT16           Count;
  UINT16           Index;
  UINT64           MaxValue;
  UINT32           Value32;

  if ((Field == NULL) || (ValueString == NULL) || (TagData == NULL) ||
      (Value == NULL) || (UsedDirectWrite == NULL)) {
    return EFI_INVALID_PARAMETER;
  }

  *UsedDirectWrite = FALSE;

  if (Field->FieldType == UI_FIELD_TYPE_EDITTEXT) {
    *UsedDirectWrite = TRUE;
    return ApplyTextValue (Field, TagData, ValueString, Value);
  }

  AsciiValue = NULL;

  Value32 = 0;
  Status = ParseNumericValue (ValueString, &Value32);
  if (!EFI_ERROR (Status)) {
    *Value = Value32;
  }
  if ((Field->FieldType == UI_FIELD_TYPE_COMBO) && EFI_ERROR (Status)) {
    AsciiValue = UnicodeToAsciiAlloc (ValueString);
    if (AsciiValue == NULL) {
      return EFI_OUT_OF_RESOURCES;
    }
    Options = FormGetOptions (Field, &Count);
    for (Index = 0; Index < Count; Index++) {
      if (AsciiStriCmp (FormGetString (Options[Index].LabelStrOffset), AsciiValue) == 0) {
        *Value = Options[Index].Value;
        FreePool (AsciiValue);
        return EFI_SUCCESS;
      }
    }
    FreePool (AsciiValue);
    return EFI_INVALID_PARAMETER;
  }

  if (EFI_ERROR (Status)) {
    Status = ParseNumericValue64 (ValueString, Value);
    if (EFI_ERROR (Status)) {
      return Status;
    }
  }

  if ((Field->FieldType == UI_FIELD_TYPE_COMBO) && (Field->OptionCount > 0)) {
    Options = FormGetOptions (Field, &Count);
    for (Index = 0; Index < Count; Index++) {
      if (Options[Index].Value == *Value) {
        return EFI_SUCCESS;
      }
    }
    return EFI_INVALID_PARAMETER;
  }

  if (Field->BitLength >= 64) {
    MaxValue = MAX_UINT64;
  } else {
    MaxValue = LShiftU64 (1ULL, Field->BitLength) - 1ULL;
  }

  if (*Value > MaxValue) {
    return EFI_INVALID_PARAMETER;
  }

  return EFI_SUCCESS;
}

STATIC
VOID
PrintFieldValue (
  IN UI_FIELD_ENTRY  *Field,
  IN UINT8           *TagData
  )
{
  UI_OPTION_ENTRY *Options;
  UINT16           Count;
  UINT16           Index;
  UINT32           Value;
  UINT64           Value64;
  UINT8           *FieldBytes;
  UINTN            ByteCount;
  CHAR8            ValueBuffer[UI_SHELL_MAX_VALUE_CHARS];
  UINTN            CopyLength;

  if ((Field == NULL) || (TagData == NULL)) {
    return;
  }

  if (Field->FieldType == UI_FIELD_TYPE_EDITTEXT) {
    if (!IsFieldByteAligned (Field)) {
      ShellPrint (L"<unsupported text layout>");
      return;
    }

    ByteCount = Field->BitLength / 8;
    if (ByteCount >= sizeof (ValueBuffer)) {
      ByteCount = sizeof (ValueBuffer) - 1;
    }
    FieldBytes = TagData + (Field->BitOffset / 8);
    ZeroMem (ValueBuffer, sizeof (ValueBuffer));
    CopyLength = ByteCount;
    CopyMem (ValueBuffer, FieldBytes, CopyLength);
    ValueBuffer[sizeof (ValueBuffer) - 1] = '\0';
    ShellPrint (L"\"%a\"", ValueBuffer);
    return;
  }

  if (Field->FieldType == UI_FIELD_TYPE_TABLE) {
    ShellPrint (L"table[%u] elem_bits=%u", Field->OptionCount, Field->OptionStartIdx);
    return;
  }

  if ((Field->FieldType != UI_FIELD_TYPE_COMBO) && (Field->BitLength > 32) && (Field->BitLength <= 64)) {
    Value64 = ReadBitField64 (TagData, Field->BitOffset, Field->BitLength);
    ShellPrint (L"%llu (0x%llx)", Value64, Value64);
    return;
  }

  Value = FormGetFieldValue (Field, TagData);
  if (Field->FieldType == UI_FIELD_TYPE_COMBO) {
    Options = FormGetOptions (Field, &Count);
    for (Index = 0; Index < Count; Index++) {
      if (Options[Index].Value == Value) {
        ShellPrint (L"%u (0x%x, %a)", Value, Value, FormGetString (Options[Index].LabelStrOffset));
        return;
      }
    }
  }

  ShellPrint (L"%u (0x%x)", Value, Value);
}

STATIC
VOID
RecordFieldChange64 (
  IN UI_FIELD_ENTRY  *Field,
  IN UINT64           Value
  )
{
  UI_FIELD_ENTRY  PartField;
  UINT16          RemainingBits;
  UINT16          ChunkBits;
  UINT16          OffsetBits;
  UINT8           Shift;
  UINT64          Mask;
  UINT32          ChunkValue;

  if (Field == NULL) {
    return;
  }

  if (Field->BitLength <= 32) {
    CfgRecordFieldChange (Field, (UINT32)Value);
    return;
  }

  PartField = *Field;
  RemainingBits = Field->BitLength;
  OffsetBits = 0;
  Shift = 0;
  while (RemainingBits > 0) {
    ChunkBits = (RemainingBits > 32) ? 32 : RemainingBits;
    PartField.BitOffset = (UINT16)(Field->BitOffset + OffsetBits);
    PartField.BitLength = ChunkBits;

    if (ChunkBits == 32) {
      Mask = 0xFFFFFFFFULL;
    } else {
      Mask = LShiftU64 (1ULL, ChunkBits) - 1ULL;
    }

    ChunkValue = (UINT32)RShiftU64 (Value, Shift);
    ChunkValue &= (UINT32)Mask;
    CfgRecordFieldChange (&PartField, ChunkValue);

    OffsetBits = (UINT16)(OffsetBits + ChunkBits);
    RemainingBits = (UINT16)(RemainingBits - ChunkBits);
    Shift = (UINT8)(Shift + ChunkBits);
  }
}

STATIC
VOID
PrintUsageSet (
  IN CONST CHAR16  *Command
  )
{
  ShellPrint (L"Usage: %s <field.path> <value>\n", Command);
  ShellPrint (L"       Value accepts decimal, hex, combo labels, or quoted text for short EditText fields\n");
}

STATIC
VOID
PrintUsageGet (
  IN CONST CHAR16  *Command
  )
{
  ShellPrint (L"Usage: %s <field.path> [--dump [start count]]\n", Command);
  ShellPrint (L"       --dump applies to UINT8 table fields and prints a hex/ASCII view\n");
}

STATIC
EFI_STATUS
ParseCfgGetOptions (
  IN  UINTN     Argc,
  IN  CHAR16   *Argv[],
  OUT BOOLEAN  *DumpMode,
  OUT UINTN    *DumpStart,
  OUT UINTN    *DumpCount
  )
{
  UINT32  Value32;

  if ((DumpMode == NULL) || (DumpStart == NULL) || (DumpCount == NULL)) {
    return EFI_INVALID_PARAMETER;
  }

  *DumpMode  = FALSE;
  *DumpStart = 0;
  *DumpCount = 0;

  if (Argc == 2) {
    return EFI_SUCCESS;
  }

  if ((Argc != 3) && (Argc != 5)) {
    return EFI_INVALID_PARAMETER;
  }

  if (StrCmp (Argv[2], L"--dump") != 0) {
    return EFI_INVALID_PARAMETER;
  }

  *DumpMode = TRUE;
  if (Argc == 3) {
    return EFI_SUCCESS;
  }

  if (EFI_ERROR (ParseNumericValue (Argv[3], &Value32))) {
    return EFI_INVALID_PARAMETER;
  }
  *DumpStart = Value32;

  if (EFI_ERROR (ParseNumericValue (Argv[4], &Value32)) || (Value32 == 0)) {
    return EFI_INVALID_PARAMETER;
  }
  *DumpCount = Value32;

  return EFI_SUCCESS;
}

STATIC
VOID
PrintUint8TableDump (
  IN UI_FIELD_ENTRY  *Field,
  IN UINT8           *TagData,
  IN CONST CHAR16    *Path,
  IN UINTN            Start,
  IN UINTN            Count
  )
{
  UI_FIELD_ENTRY  ElemField;
  UINTN           ElemCount;
  UINTN           End;
  UINTN           RowStart;
  UINTN           Col;
  UINTN           RowCount;
  UINT64          ElemValue;
  CHAR8           HexBuf[(16 * 3) + 1];
  CHAR8           AsciiBuf[17];

  if ((Field == NULL) || (TagData == NULL) || (Path == NULL)) {
    return;
  }

  ElemCount = Field->OptionCount;
  if (Start >= ElemCount) {
    ShellPrint (L"Error: dump start (%u) out of range, max index is %u\n", (UINT32)Start, (UINT32)(ElemCount - 1));
    return;
  }

  if (Count == 0) {
    Count = ElemCount - Start;
  }

  End = Start + Count;
  if ((End < Start) || (End > ElemCount)) {
    End = ElemCount;
  }

  ShellPrint (L"%s dump [%u..%u]\n", Path, (UINT32)Start, (UINT32)(End - 1));

  ElemField = *Field;
  ElemField.FieldType = UI_FIELD_TYPE_EDITNUM;
  ElemField.BitLength = 8;
  for (RowStart = Start; RowStart < End; RowStart += 16) {
    RowCount = End - RowStart;
    if (RowCount > 16) {
      RowCount = 16;
    }

    SetMem (HexBuf, sizeof (HexBuf), ' ');
    HexBuf[sizeof (HexBuf) - 1] = '\0';
    SetMem (AsciiBuf, sizeof (AsciiBuf), ' ');
    AsciiBuf[16] = '\0';

    for (Col = 0; Col < RowCount; Col++) {
      ElemField.BitOffset = (UINT16)(Field->BitOffset + (UINT16)((RowStart + Col) * 8));
      ElemValue = ReadBitField64 (TagData, ElemField.BitOffset, ElemField.BitLength);

      AsciiSPrint (&HexBuf[Col * 3], 4, "%02X ", (UINT8)ElemValue);
      AsciiBuf[Col] = ((ElemValue >= 0x20) && (ElemValue <= 0x7E)) ? (CHAR8)ElemValue : '.';
    }

    ShellPrint (L"%04x: %a|%a|\n", (UINT32)RowStart, HexBuf, AsciiBuf);
  }
}

STATIC
VOID
PrintUsageFind (
  IN CONST CHAR16  *Command
  )
{
  ShellPrint (L"Usage: %s [pattern] [--verbose]\n", Command);
  ShellPrint (L"       --verbose or -v: show field display names in brackets\n");
}

EFI_STATUS
EFIAPI
UiCfgShellCfgSetCommand (
  IN UINTN    Argc,
  IN CHAR16  *Argv[]
  )
{
  EFI_STATUS      Status;
  CHAR8          *FieldPath;
  BOOLEAN         HasIndex;
  UINTN           Index;
  UI_FIELD_ENTRY *Field;
  UI_FIELD_ENTRY  ElementField;
  UI_FIELD_ENTRY *TargetField;
  BOOLEAN         UseElementField;
  UINT8          *TagData;
  UINT64          Value;
  BOOLEAN         UsedDirectWrite;

  if (Argc != 3) {
    PrintUsageSet (Argv[0]);
    return EFI_INVALID_PARAMETER;
  }

  Status = UiShellInit ();
  if (EFI_ERROR (Status)) {
    ShellPrint (L"Error: failed to initialize config access: %r\n", Status);
    return Status;
  }

  Status = ParseFieldPathIndex (Argv[1], &FieldPath, &HasIndex, &Index);
  if (EFI_ERROR (Status)) {
    ShellPrint (L"Error: invalid field path '%s'\n", Argv[1]);
    return Status;
  }

  Field = FormFindFieldByPath (FieldPath);
  if (Field == NULL) {
    ShellPrint (L"Error: field '%s' not found\n", Argv[1]);
    FreePool (FieldPath);
    return EFI_NOT_FOUND;
  }

  Status = BuildTableElementField (Field, HasIndex, Index, &ElementField, &UseElementField);
  if (EFI_ERROR (Status)) {
    if ((Field->FieldType == UI_FIELD_TYPE_TABLE) && !HasIndex) {
      ShellPrint (L"Error: table field '%s' requires index syntax, e.g. %s[0]\n", Argv[1], Argv[1]);
    } else {
      ShellPrint (L"Error: invalid table index for '%s'\n", Argv[1]);
    }
    FreePool (FieldPath);
    return Status;
  }

  TargetField = UseElementField ? &ElementField : Field;

  TagData = CfgGetTagData (Field->TagId);
  if (TagData == NULL) {
    ShellPrint (L"Error: tag 0x%x data not found for '%s'\n", Field->TagId, Argv[1]);
    FreePool (FieldPath);
    return EFI_NOT_FOUND;
  }

  Status = ResolveFieldValue (TargetField, Argv[2], TagData, &Value, &UsedDirectWrite);
  if (EFI_ERROR (Status)) {
    if (Status == EFI_BAD_BUFFER_SIZE) {
      ShellPrint (L"Error: text value is too long for '%s'\n", Argv[1]);
    } else if (Status == EFI_UNSUPPORTED) {
      ShellPrint (L"Error: '%s' uses a field format this shell path does not support yet\n", Argv[1]);
    } else {
      ShellPrint (L"Error: invalid value '%s' for '%s'\n", Argv[2], Argv[1]);
    }
    FreePool (FieldPath);
    return Status;
  }

  if (!UsedDirectWrite) {
    if (TargetField->BitLength <= 32) {
      FormSetFieldValue (TargetField, TagData, (UINT32)Value);
    } else if (TargetField->BitLength <= 64) {
      WriteBitField64 (TagData, TargetField->BitOffset, TargetField->BitLength, Value);
    } else {
      ShellPrint (L"Error: '%s' field width (%u bits) is not supported by cfgset\n", Argv[1], TargetField->BitLength);
      FreePool (FieldPath);
      return EFI_UNSUPPORTED;
    }
  }
  RecordFieldChange64 (TargetField, Value);
  CfgSetDirty ();
  Status = CfgSave ();
  if (EFI_ERROR (Status)) {
    ShellPrint (L"Error: failed to save CfgDelta: %r\n", Status);
    FreePool (FieldPath);
    return Status;
  }

  ShellPrint (L"%s = ", Argv[1]);
  PrintFieldValue (TargetField, TagData);
  ShellPrint (L"\n");

  FreePool (FieldPath);
  return EFI_SUCCESS;
}

EFI_STATUS
EFIAPI
UiCfgShellCfgGetCommand (
  IN UINTN    Argc,
  IN CHAR16  *Argv[]
  )
{
  EFI_STATUS      Status;
  CHAR8          *FieldPath;
  BOOLEAN         HasIndex;
  UINTN           Index;
  UI_FIELD_ENTRY *Field;
  UI_FIELD_ENTRY  ElementField;
  BOOLEAN         UseElementField;
  UI_FIELD_ENTRY *TargetField;
  UINT8          *TagData;
  UINT16          ElemIdx;
  BOOLEAN         DumpMode;
  UINTN           DumpStart;
  UINTN           DumpCount;

  if ((Argc < 2) || (Argc > 5)) {
    PrintUsageGet (Argv[0]);
    return EFI_INVALID_PARAMETER;
  }

  Status = ParseCfgGetOptions (Argc, Argv, &DumpMode, &DumpStart, &DumpCount);
  if (EFI_ERROR (Status)) {
    PrintUsageGet (Argv[0]);
    return Status;
  }

  Status = UiShellInit ();
  if (EFI_ERROR (Status)) {
    ShellPrint (L"Error: failed to initialize config access: %r\n", Status);
    return Status;
  }

  Status = ParseFieldPathIndex (Argv[1], &FieldPath, &HasIndex, &Index);
  if (EFI_ERROR (Status)) {
    ShellPrint (L"Error: invalid field path '%s'\n", Argv[1]);
    return Status;
  }

  Field = FormFindFieldByPath (FieldPath);
  if (Field == NULL) {
    ShellPrint (L"Error: field '%s' not found\n", Argv[1]);
    FreePool (FieldPath);
    return EFI_NOT_FOUND;
  }

  Status = BuildTableElementField (Field, HasIndex, Index, &ElementField, &UseElementField);
  if (EFI_ERROR (Status)) {
    ShellPrint (L"Error: invalid table index for '%s'\n", Argv[1]);
    FreePool (FieldPath);
    return Status;
  }

  TargetField = UseElementField ? &ElementField : Field;

  TagData = CfgGetTagData (Field->TagId);
  if (TagData == NULL) {
    ShellPrint (L"Error: tag 0x%x data not found for '%s'\n", Field->TagId, Argv[1]);
    FreePool (FieldPath);
    return EFI_NOT_FOUND;
  }

  if (DumpMode) {
    if (HasIndex || (Field->FieldType != UI_FIELD_TYPE_TABLE) || (Field->OptionStartIdx != 8)) {
      ShellPrint (L"Error: --dump is supported only for non-indexed UINT8 table fields\n");
      FreePool (FieldPath);
      return EFI_UNSUPPORTED;
    }

    PrintUint8TableDump (Field, TagData, Argv[1], DumpStart, DumpCount);
  } else if ((Field->FieldType == UI_FIELD_TYPE_TABLE) && !HasIndex) {
    for (ElemIdx = 0; ElemIdx < Field->OptionCount; ElemIdx++) {
      ElementField = *Field;
      ElementField.BitLength = Field->OptionStartIdx;
      ElementField.BitOffset = (UINT16)(Field->BitOffset + ElemIdx * Field->OptionStartIdx);
      ElementField.FieldType = UI_FIELD_TYPE_EDITNUM;
      ShellPrint (L"%s[%u] = ", Argv[1], ElemIdx);
      PrintFieldValue (&ElementField, TagData);
      ShellPrint (L"\n");
    }
  } else {
    ShellPrint (L"%s = ", Argv[1]);
    PrintFieldValue (TargetField, TagData);
    ShellPrint (L"\n");
  }

  FreePool (FieldPath);
  return EFI_SUCCESS;
}

EFI_STATUS
EFIAPI
UiCfgShellCfgFindCommand (
  IN UINTN    Argc,
  IN CHAR16  *Argv[]
  )
{
  EFI_STATUS      Status;
  CHAR8          *Pattern;
  UINT16          FieldCount;
  UINT16          FieldIdx;
  UINTN           Matches;
  UI_FIELD_ENTRY *Field;
  UINT8          *TagData;
  CHAR8          *Name;
  CHAR8          *Path;
  BOOLEAN         Verbose;
  UINTN           ArgIdx;

  if (Argc > 3) {
    PrintUsageFind (Argv[0]);
    return EFI_INVALID_PARAMETER;
  }

  Pattern = NULL;
  Verbose = FALSE;

  // Parse arguments: pattern and/or --verbose flag
  for (ArgIdx = 1; ArgIdx < Argc; ArgIdx++) {
    if ((StrCmp (Argv[ArgIdx], L"--verbose") == 0) ||
        (StrCmp (Argv[ArgIdx], L"-v") == 0)) {
      Verbose = TRUE;
    } else if (Pattern == NULL) {
      Pattern = UnicodeToAsciiAlloc (Argv[ArgIdx]);
      if (Pattern == NULL) {
        return EFI_OUT_OF_RESOURCES;
      }
    }
  }

  Status = UiShellInit ();
  if (EFI_ERROR (Status)) {
    ShellPrint (L"Error: failed to initialize config access: %r\n", Status);
    if (Pattern != NULL) {
      FreePool (Pattern);
    }
    return Status;
  }

  Matches = 0;
  FieldCount = FormGetFieldCount ();
  for (FieldIdx = 0; FieldIdx < FieldCount; FieldIdx++) {
    Field = FormGetField (FieldIdx);
    if (Field == NULL) {
      continue;
    }

    Name = FormGetString (Field->NameStrOffset);
    Path = FormGetFieldPath (Field);
    if ((Pattern != NULL) &&
        !AsciiContainsInsensitive (Path, Pattern) &&
        !AsciiContainsInsensitive (Name, Pattern)) {
      continue;
    }

    TagData = CfgGetTagData (Field->TagId);
    if (TagData == NULL) {
      continue;
    }

    ShellPrint (L"%a = ", Path);
    PrintFieldValue (Field, TagData);
    if (Verbose) {
      ShellPrint (L"  [%a]\n", Name);
    } else {
      ShellPrint (L"\n");
    }
    Matches++;
  }

  ShellPrint (L"%u match(es)\n", (UINT32)Matches);

  if (Pattern != NULL) {
    FreePool (Pattern);
  }

  return EFI_SUCCESS;
}

