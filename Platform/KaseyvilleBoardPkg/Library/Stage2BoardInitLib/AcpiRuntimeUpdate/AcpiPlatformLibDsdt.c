/** @file
  ACPI DSDT runtime update

  Copyright (c) 2026, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

//
// Statements that include other files
//
#include "Stage2BoardInitLib.h"
#include "AcpiPlatformLibLocal.h"
#include <Acpi/AcpiMacro.h>
#include "SiSubsysInfoLib.h"
#include "IioSocketTopology.h"
#include "Acpi/EPRPPlatform_offset.h"

#define MAX_CORE      256
#define MAX_THREAD    2
#define MAX_CPU_NUM1  512
#define UBOX_STACK    MAX_LOGIC_IIO_STACK - 1 //use stack 13 for ubox

#define HexChar2int(c) (((c) > 'a') ? 10 + (c) - 'a' : ((c) > '9') ? 10 + (c) - 'A' : (c) - '0')

UINT32       mApicIdMap[MAX_SOCKET][MAX_CORE * MAX_THREAD];
UINT32       mSNCProximityDomainMap[MAX_SOCKET][MAX_CORE * MAX_THREAD];

#define DSDTDEBUG(...)  { DEBUG((DEBUG_VERBOSE, "[ACPI](DSDT) " __VA_ARGS__)); }
#define DSDTWARN(...)  { DEBUG((DEBUG_VERBOSE, "[ACPI](DSDT) WARNING: " __VA_ARGS__)); }
#define DSDTERROR(...)  { DEBUG((DEBUG_ERROR, "[ACPI](DSDT) ERROR: " __VA_ARGS__)); }

typedef union {
  struct {
    UINT8 Ignored         : 1;
    UINT8 DecodeType      : 1;
    UINT8 MinAddressFixed : 1;
    UINT8 MaxAddressFixed : 1;
    UINT8 Reserved        : 4;
  } Bits;
  UINT8 Data;
} ADDRESS_SPACE_DESCRIPTOR_GENERAL_FLAGS;
/**
 * Type of device object in ASL.
 */
typedef enum {
  AcpiIoatDev = 0,
  AcpiDevCpm,
  AcpiDevCpm1,
  AcpiDevHqm,
  AcpiDevHqm1,
  AcpiDevCxl1,
  AcpiDevCxl2,
  AcpiDevDino,
  AcpiDevPci,
  AcpiDevUncore,
  AcpiDevTip,
  AcpiDevTip1,
  AcpiDevMax
} ACPI_DEV_TYPE;

/**
 * Stack resources as provided by AcpiIsStachInCxlMode().
 */
typedef struct {
  UINT8  BusBase;
  UINT8  BusLimit;
  UINT16 IoBase;
  UINT16 IoLimit;
  UINT32 Mmio32Base;
  UINT32 Mmio32Limit;
  UINT64 Mmio64Base;
  UINT64 Mmio64Limit;
} ACPI_STACK_INFO;

extern AML_OFFSET_TABLE_ENTRY   DSDT_PLATEPRP_OffsetTable[];
AML_OFFSET_TABLE_ENTRY *mAmlOffsetTablePointer = NULL;

/**
    Get IOAT instance maped stack index

    @param[in]  Socket    - Socket index
    @param[out] IoatInst  - Ioat instance
    @param[out] Stack     - Stack index

    @retval EFI_SUCCESS     Returns Success if get stack index success
    @retval EFI_UNSUPPORTED Returns Unsupported if get stack index fail
**/
EFI_STATUS
GetIoatInstanceStackIndex (
  IN   UINT8    Socket,
  OUT  UINT8    IoatInst,
  OUT  UINT8    *Stack
  )
{
  SUBSYS_INFO_ENTRY       SubsysInfoEntry;

  if (!GetSubsysInfo (Socket, IoatInst, SUBSYS_TYPE_IOAT, &SubsysInfoEntry)) {
    DSDTERROR ("IOAT[%d.%d] Get subsystem information fail \n", Socket, IoatInst);
    *Stack = 0xFF;
    return EFI_UNSUPPORTED;
  }

  if (!SubsysInfoEntry.Header.Present) {
    DSDTWARN ("[%d.%d] IOAT is not present \n", Socket, IoatInst);
    *Stack = 0xFF;
    return EFI_UNSUPPORTED;
  }
  *Stack = SubsysInfoEntry.AddrMap.Stack;
  return EFI_SUCCESS;
}

/**
  Parse ASL Ioat device path to get socket, stack and Root index from the path.

  The PCI device path is typically:
  '_SB_.ATxy.FIXz' for IOAT stack object
  where x and y are hex digits, and 'a' is a letter like 'A', 'B',..,'H'.
  In the hex number 'xy' the 'x' digit is socket, 'y' is stack.
  Function returns ACPI device type as recognised in the name parsed.

  @param[in]  DevPathPtr - IOAT device path, e.g. '_SB_.AT00.FIX1'
  @param[out] SocketPtr  - Buffer for socket index.
  @param[out] StackPtr   - Buffer for stack index.
  @param[out] RootPtr    - Buffer for PCI root index.

  @return Type of parsed device name is returned.
**/
ACPI_DEV_TYPE
AcpiIoatDevPathParse (
  IN  CHAR8    *DevPathPtr,
  OUT UINT8    *SocketPtr,
  OUT UINT8    *StackPtr,
  OUT UINT8    *RootPtr
  )
{
  UINT8                   Socket = 0xFF;
  UINT8                   Stack = 0xFF;
  UINT8                   IoatInst = 0;
  UINT8                   Root = 0;
  BOOLEAN                 AccDevPresent = TRUE;
  EFI_STATUS              Status;

  switch (DevPathPtr[5] << 8 | DevPathPtr[6]) {
    case ('A' << 8 | 'T'): // 'ATxy' for IOAT stack
      Root = IOAT_ROOT;
      break;
    case ('A' << 8 | 'A'): // 'AAxy' for accelerator A
      Root = ACC_ROOT_A;
      break;
    case ('A' << 8 | 'B'): // 'ABxy' for accelerator B
      Root = ACC_ROOT_B;
      break;
    case ('A' << 8 | 'C'): // 'ACxy' for accelerator C
      Root = ACC_ROOT_C;
      break;
    case ('A' << 8 | 'D'): // 'ADxy' for accelerator D
      Root = ACC_ROOT_D;
      break;
    default:
      Root = 0;
      AccDevPresent = FALSE;
      break;
  }
  ASSERT (AccDevPresent == TRUE);
  if (!AccDevPresent) {
    *SocketPtr = 0xFF;
    *StackPtr = 0xFF;
    *RootPtr = 0;
    DSDTERROR ("Invalid IOAT device path: '%a' \n", DevPathPtr);
    return AcpiDevMax;
  }
  Socket = HexChar2int (DevPathPtr[7]);
  IoatInst = HexChar2int (DevPathPtr[8]);

  Status = GetIoatInstanceStackIndex (Socket, IoatInst, &Stack);
  if (EFI_ERROR (Status)) {
    *SocketPtr = 0xFF;
    *StackPtr = 0xFF;
    *RootPtr = 0;
    return AcpiDevMax;
  }

  *SocketPtr = Socket;
  *StackPtr = Stack;
  *RootPtr = Root;
  return AcpiIoatDev;
}

/**
  Parse ASL device path to get socket, stack and optionally port index from the path.

  The PCI device path is typically:
  '_SB_.PCxy.FIXz' for PCIe stack object
  '_SB_.UBxy.FIXz' for UBOX stack object
  '_SB_.PCxy.BRPa' for PCIe bridge root port object
  where x and y are hex digits, and 'a' is a letter like 'A', 'B',..,'H'.
  In the hex number 'xy' the 'x' digit is socket, 'y' is stack.
  Function returns ACPI device type as recognised in the name parsed.

  @param[in]  DevPathPtr - PCI device path, e.g. '_SB_.PC00.FIX1'
  @param[out] SocketPtr  - Buffer for socket index.
  @param[out] StackPtr   - Buffer for stack index.
  @param[out] RootPtr    - Buffer for PCI root index.
  @param[out] PortPtr    - Buffer for port index.

  @return Type of parsed device name is returned.
**/
ACPI_DEV_TYPE
AcpiPciDevPathParse (
  IN  CHAR8    *DevPathPtr,
  OUT UINT8    *SocketPtr,
  OUT UINT8    *StackPtr,
  OUT UINT8    *RootPtr,
  OUT UINT8    *PortPtr
  )
{
  ACPI_DEV_TYPE DevType = AcpiDevMax;
  UINT8         Socket = 0xFF;
  UINT8         Stack = 0xFF;
  UINT8         Root = 0xFF;
  UINT8         Port = 0xFF;
  UINT8         StackOrPeOrIoatIndex;
  EFI_STATUS    Status;

  if (PortPtr != NULL) {
    //
    // Device path should contain bridge root port object.
    // Let's verify it is long enough and port name starts with 'RP'.
    //
    if (AsciiStrLen (DevPathPtr) < 3*4 + 2 ||
        DevPathPtr[10]  != 'B' || DevPathPtr[12]  != 'P' || DevPathPtr[13]  < 'A' || DevPathPtr[13]  > 'H') {
      goto ErrMsg;
    }
    Port = DevPathPtr[13] - 'A';
  }
  //
  // Device path should contain stack object.
  // Let's verify it is long enough and thre are digits in socket and stack positions.
  //
  if (AsciiStrLen (DevPathPtr) < 2*4 + 1 || DevPathPtr[7]  < '0' || DevPathPtr[8]  < '0') {
    goto ErrMsg;
  }
  Root = 0;
  if (DevPathPtr[5] == 'A') {
     DevType = AcpiIoatDevPathParse (DevPathPtr, SocketPtr, StackPtr, RootPtr);
    return DevType;
  }
  switch (DevPathPtr[5] << 8 | DevPathPtr[6]) {
    case ('C' << 8 | 'P'): // 'CPxy' for CPM
      DevType = AcpiDevCpm;
      Root = PCIROOT_CPM0;
      break;
    case ('B' << 8 | 'P'): // 'BPxy' for CPM1
      DevType = AcpiDevCpm1;
      Root = PCIROOT_CPM1;
      break;
    case ('L' << 8 | 'Q'): // 'LQxy' for HQM
      DevType = AcpiDevHqm;
      Root = PCIROOT_HQM;
      break;
    case ('H' << 8 | 'Q'): // 'HQxy' for HQM
      DevType = AcpiDevHqm;
      Root = PCIROOT_HQM0;
      break;
    case ('B' << 8 | 'Q'): // 'BQxy' for HQM1
      DevType = AcpiDevHqm1;
      Root = PCIROOT_HQM1;
      break;
    case ('C' << 8 | '1'): // 'C1xy' for CXL 1.1 device in stack
      DevType = AcpiDevCxl1;
      Root = PCIROOT_CXL1P1;
      break;
    case ('C' << 8 | '2'): // 'C2xy' for CXL 2.0 device in stack
      DevType = AcpiDevCxl2;
      break;
    case ('D' << 8 | 'I'): // 'DIxy' for DINO stack
      DevType = AcpiDevDino;
      break;
    case ('P' << 8 | 'C'): // 'PCxy' for PCI stack
      DevType = AcpiDevPci;
      break;
    case ('U' << 8 | 'B'): // 'UBxy' for UBOX stack
      DevType = AcpiDevUncore;
      break;
    case ('T' << 8 | 'I'): // 'TIxy' for HCT TIP
      DevType = AcpiDevTip;
      Root = PCIROOT_TIP;
      break;
    case ('T' << 8 | 'A'): // 'TAxy' for HCTa TIP
      DevType = AcpiDevTip1;
      Root = PCIROOT_TIP1;
      break;
    default:
    ErrMsg:
      DSDTERROR ("String '%a' is not valid PCI stack name, ", DevPathPtr);
      DSDTERROR ("expect _SB_.PCxy.FIXz, or _SB_.UCxv.FIXz, or _SB_.PCxy.RPya\n");
      goto GetOut;
  }
  if (DevPathPtr[7] <= '9') {

    Socket = DevPathPtr[7] - '0';

  } else if (DevPathPtr[7] <= 'F') {

    if (DevPathPtr[7] < 'A') {
      goto ErrMsg;
    }
    Socket = 10 + DevPathPtr[7] - 'A';

  } else if (DevPathPtr[7] <= 'f') {

    if (DevPathPtr[7] < 'a') {
      goto ErrMsg;
    }
    Socket = 10 + DevPathPtr[7] - 'a';

  } else {
    goto ErrMsg;
  }
  if (DevPathPtr[8] <= '9') {

    StackOrPeOrIoatIndex = DevPathPtr[8] - '0';

  } else if (DevPathPtr[8] <= 'F') {

    if (DevPathPtr[8] < 'A') {
      goto ErrMsg;
    }
    StackOrPeOrIoatIndex = 10 + DevPathPtr[8] - 'A';

  } else {
    goto ErrMsg;
  }
  switch (DevType) {
    case AcpiDevDino:
    case AcpiDevCpm:
    case AcpiDevCpm1:
    case AcpiDevHqm:
    case AcpiDevHqm1:
    case AcpiDevTip:
    case AcpiDevTip1:
      Status = GetIoatInstanceStackIndex (Socket, StackOrPeOrIoatIndex, &Stack);
      if (EFI_ERROR (Status)) {
        DSDTDEBUG ("Get stack index fail \n");
      }
      break;
    case AcpiDevPci:
    case AcpiDevCxl1:
    case AcpiDevCxl2:
      Stack = IioPe2StackIndex (Socket, StackOrPeOrIoatIndex);
      break;
    default:
      Stack = StackOrPeOrIoatIndex;
  }
  if (PortPtr != NULL) {
    DSDTDEBUG ("Object '%a' location is [%d.%d.%d] PCI root[%d]\n", DevPathPtr, Socket, Stack, Port, Root);
  } else {
    DSDTDEBUG ("Object '%a' location is [%d.%d] PCI root[%d]\n", DevPathPtr, Socket, Stack, Root);
  }

 GetOut:
  if (SocketPtr != NULL) {
    *SocketPtr = Socket;
  }
  if (StackPtr != NULL) {
    *StackPtr = Stack;
  }
  if (RootPtr != NULL) {
    *RootPtr = Root;
  }
  if (PortPtr != NULL) {
    *PortPtr = Port;
  }
  return DevType;
} // AcpiPciDevPathParse()

/**
  Get acpi pci root resource.

  @param[in]  DevPathPtr      - PCI device path, e.g. '_SB_.PC00.FIX1'
  @param[in]  IioUdsPtr       - Pointer to IioUds.
  @param[out] PciRootResPtr   - Pointer to UDS_PCIROOT_RES.

  @return Standard EFI_STATUS is returned.
**/
EFI_STATUS
GetAcpiPciRootResource (
  IN  CHAR8               *DevPathPtr,
  IN  IIO_UDS             *IioUdsPtr,
  OUT UDS_PCIROOT_RES     *PciRootResPtr
  )
{
  UINT8                        Socket;
  UINT8                        Stack;
  UINT8                        Root;
  ACPI_DEV_TYPE                DevType = AcpiDevMax;

  if (PciRootResPtr == NULL) {
    return EFI_UNSUPPORTED;
  }
  ZeroMem (PciRootResPtr, sizeof (*PciRootResPtr));
  DevType = AcpiPciDevPathParse (DevPathPtr, &Socket, &Stack, &Root, NULL);
  DSDTDEBUG ("Device type is %d \n", DevType);

  if (Socket >= MAX_SOCKET || !(IioUdsPtr->PlatformData.CpuQpiInfo[Socket].Valid)) {
    DSDTDEBUG ("Socket:%d is not present or out of MAX_SOCKET\n", Socket);
    return EFI_UNSUPPORTED;
  }
  if (Stack >= MAX_IIO_STACKS_PER_SOCKET || !(IioUdsPtr->PlatformData.CpuQpiInfo[Socket].StackPresentBitmap & (1 << Stack))) {
    DSDTDEBUG ("Stack:%d is not present or out of MAX_IIO_STACKS_PER_SOCKET\n", Stack);
    return EFI_UNSUPPORTED;
  }
  if (Root >= IioUdsPtr->PlatformData.IIO_resource[Socket].StackRes[Stack].PciRootBridgeNum) {
    DSDTDEBUG ("Root:%d is out of PciRootBridgeNum: %d\n", Root, IioUdsPtr->PlatformData.IIO_resource[Socket].StackRes[Stack].PciRootBridgeNum);
    return EFI_UNSUPPORTED;
  }
  CopyMem (PciRootResPtr, &(IioUdsPtr->PlatformData.IIO_resource[Socket].StackRes[Stack].PciRoot[Root]), sizeof(*PciRootResPtr));
  return EFI_SUCCESS;
}
/**
  Set the Address Space Descriptor Ranges to Fixed

  @param [in] *GeneralFlags

  @return VOID
**/
VOID
SetAddressSpaceRangesFixed (
  IN ADDRESS_SPACE_DESCRIPTOR_GENERAL_FLAGS *GeneralFlags
  )
{
  GeneralFlags->Bits.MinAddressFixed = 1;
  GeneralFlags->Bits.MaxAddressFixed = 1;
}


/**
  Update the DSDT table
  iASL generates an offset table .h file which contains offsets from the start of DSDT AML
  in the following format:
  Name, OperationRegion, WORDBusNumber, DWORDMemory, QWORDMemory, WORDIO
  The last 4 items must populate the last field (DescriptorName) for iASL to put them in
  the offset table. This function will patch these objects in the DSDT AML.

  @param[in,out] *Table - Pointer to the DSDT table starting with ACPI header.

  @return Standard EFI_STATUS is returned.
**/
EFI_STATUS
PatchDsdtTableAML (
  IN OUT EFI_ACPI_DESCRIPTION_HEADER   *Table,
  IN OUT GLOBAL_NVS_AREA *Gnvs
  )
{
  ACPI_DEV_TYPE                           AcpiDevType;
  UDS_PCIROOT_RES                         PciRootRes;
  EFI_STATUS                              Status;
  UINT8                                   *DsdtPointer;
  UINT32                                  *Signature;
  UINT16                                  Counter16;
  UINT16                                  i;  // DSDT_PLATEXRP_OffsetTable LUT entries extends beyond 256!
  UINT8                                   BusBase, BusLimit;
  UINT16                                  IoBase, IoLimit;
  UINT32                                  Mmio32Base, Mmio32Limit;
  UINT64                                  Mmio64Base, Mmio64Limit;
  EFI_ACPI_WORD_ADDRESS_SPACE_DESCRIPTOR  *AmlResourceAddress16Pointer;
  EFI_ACPI_DWORD_ADDRESS_SPACE_DESCRIPTOR *AmlResourceAddress32Pointer;
  EFI_ACPI_QWORD_ADDRESS_SPACE_DESCRIPTOR *AmlResourceAddress64Pointer;
  EFI_ACPI_DESCRIPTION_HEADER             *TableHeader;
  EFI_ACPI_NAMEPACK_DWORD                 *NamePtr;
  IIO_UDS                                 *IioUdsPtr;
  UINT8                                   *CurrPtr;
  UINT8                                   *EndPtr;
  UINT8                                   Socket;
  UINT8                                   Stack;
  UINT8                                   Root;
  UINT8                                   Index;
  UINT8                                   LegacyVgaStackSoc;
  UINT8                                   LegacyVgaStackFwInst;
  UINT8                                   IoatInst;
  UINT8                                   MaxIoat;
  EFI_HOB_GUID_TYPE                       *GuidHob;
  VOID                                    *FspHobList;
  UINT32                                  SocketPresentBitMap;
  BIOS_ACPI_PARAM                         *AcpiParameter;
  SYS_CPU_INFO                            *CpuInfo;

  CpuInfo  = MpGetInfo ();
  AcpiParameter = &Gnvs->BiosAcpiParam;

  mAmlOffsetTablePointer = DSDT_PLATEPRP_OffsetTable;

  // init global mApicIdMap variable
  SetMem32 ((VOID *)mApicIdMap, sizeof(mApicIdMap), 0xFFFFFFFF);
  for (Index = 0; Index < CpuInfo->CpuCount; ++Index)
  {
    mApicIdMap[0][Index] = CpuInfo->CpuInfo[Index].ApicId;
  }

  SocketPresentBitMap = 0x1;
  LegacyVgaStackFwInst = 0xA;
  LegacyVgaStackSoc = 0;

  // Get IIO universal HOB data
  GuidHob = NULL;
  FspHobList = GetFspHobListPtr ();
  if (FspHobList != NULL) {
    GuidHob = GetNextGuidHob (&gIioUdsHobGuid, FspHobList);
  }
  if (GuidHob == NULL) {
    DSDTERROR ("Iio Data Hob not found!\n");
    ASSERT (FALSE);
    return EFI_D_ERROR;
  }

  IioUdsPtr = GET_GUID_HOB_DATA (GuidHob);

  TableHeader = (EFI_ACPI_DESCRIPTION_HEADER *)Table;

  AcpiParameter->SocketBitMask = SocketPresentBitMap;
  DSDTDEBUG (" Sockets bitmap: 0x%02X\n", AcpiParameter->SocketBitMask);
  DSDTDEBUG (" MAX_SOCKET: %d MAX_CORE: %d MAX_THREAD: %d MAX_CPU_NUM: %d\n", MAX_SOCKET, MAX_CORE, MAX_THREAD, MAX_CPU_NUM1);

  for (Socket = 0; Socket < MAX_SOCKET; Socket++) {

    if (!IioUdsPtr->PlatformData.CpuQpiInfo[Socket].Valid) {
      continue;
    }
    for (Stack = 0; Stack < MAX_IIO_STACKS_PER_SOCKET; Stack++) {

      if (IioUdsPtr->PlatformData.CpuQpiInfo[Socket].StackPresentBitmap & (1 << Stack)) {

        if ((Index = IioStack2PeIndex (Socket, Stack)) < MAX_IIO_PCIE_PER_SOCKET) {
          AcpiParameter->IioPcieRootBitmap[Socket][Index] = (UINT8)-1 >> (8 - IioUdsPtr->PlatformData.IIO_resource[Socket].StackRes[Stack].PciRootBridgeNum);
          if (IioUdsPtr->PlatformData.CpuQpiInfo[Socket].CxlCapableBitmap & (1 << Stack)) {
            AcpiParameter->IioCxlCapableBitmap[Socket] |= 1 << Index;
          }
        }
      }
    }

  MaxIoat = GetMaxSubsys (Socket, SUBSYS_TYPE_IOAT);
  for (IoatInst = 0; IoatInst < MaxIoat; IoatInst++) {
    Status = GetIoatInstanceStackIndex (Socket, IoatInst, &Stack);
    if (EFI_ERROR (Status)) {
      continue;
    }

    AcpiParameter->IioIoatRootBitmap[Socket][IoatInst] = (UINT8)-1 >> (8 - IioUdsPtr->PlatformData.IIO_resource[Socket].StackRes[Stack].PciRootBridgeNum);
    AcpiParameter->IioIoatSegment[Socket][IoatInst] = IioUdsPtr->PlatformData.IIO_resource[Socket].StackRes[Stack].Segment;
  }
    DSDTDEBUG ("[%d] CXL capable PCIe bitmap: 0x%04X\n", Socket, AcpiParameter->IioCxlCapableBitmap[Socket]);
    DSDTDEBUG ("[%d] PCIe root bridge bitmap: %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X\n", Socket,
               AcpiParameter->IioPcieRootBitmap[Socket][0], AcpiParameter->IioPcieRootBitmap[Socket][1],
               AcpiParameter->IioPcieRootBitmap[Socket][2], AcpiParameter->IioPcieRootBitmap[Socket][3],
               AcpiParameter->IioPcieRootBitmap[Socket][4], AcpiParameter->IioPcieRootBitmap[Socket][5],
               AcpiParameter->IioPcieRootBitmap[Socket][6], AcpiParameter->IioPcieRootBitmap[Socket][7],
               AcpiParameter->IioPcieRootBitmap[Socket][8], AcpiParameter->IioPcieRootBitmap[Socket][9]);
    DSDTDEBUG ("[%d] IOAT root bridge bitmap: %02X %02X %02X %02X %02X %02X %02X %02X\n", Socket,
               AcpiParameter->IioIoatRootBitmap[Socket][0], AcpiParameter->IioIoatRootBitmap[Socket][1],
               AcpiParameter->IioIoatRootBitmap[Socket][2], AcpiParameter->IioIoatRootBitmap[Socket][3],
               AcpiParameter->IioIoatRootBitmap[Socket][4], AcpiParameter->IioIoatRootBitmap[Socket][5],
               AcpiParameter->IioIoatRootBitmap[Socket][6], AcpiParameter->IioIoatRootBitmap[Socket][7]);
    DSDTDEBUG ("[%d] IOAT Segment: %02X %02X %02X %02X %02X %02X %02X %02X\n", Socket,
               AcpiParameter->IioIoatSegment[Socket][0], AcpiParameter->IioIoatSegment[Socket][1],
               AcpiParameter->IioIoatSegment[Socket][2], AcpiParameter->IioIoatSegment[Socket][3],
               AcpiParameter->IioIoatSegment[Socket][4], AcpiParameter->IioIoatSegment[Socket][5],
               AcpiParameter->IioIoatSegment[Socket][6], AcpiParameter->IioIoatSegment[Socket][7]);
  } // for (Socket...)
  //
  // Update IIO PCIe Root Port PCIe Capability offset.
  // For 10nm and 7nm process CPU with PCIe GEN4/GEN5 controller, capability offset is 0x40.
  //
  AcpiParameter->IioPcieRpCapOffset = 0x40;
  //
  // Loop through the AML looking for values that we must fix up.
  //
  for (i = 0; mAmlOffsetTablePointer[i].Pathname != 0; i++) {
    //
    // Point to offset in DSDT for current item in AmlOffsetTable.
    //
    DsdtPointer = (UINT8 *) (TableHeader) + mAmlOffsetTablePointer[i].Offset;

    DSDTDEBUG ("Patching '%a' OpCode 0x%02X Offset %X\n", mAmlOffsetTablePointer[i].Pathname, mAmlOffsetTablePointer[i].Opcode,mAmlOffsetTablePointer[i].Offset);

    if (mAmlOffsetTablePointer[i].Opcode == AML_DWORD_PREFIX) { //BIOS PARAMETERS
      //
      // If Opcode is 0x0C, then operator is Name() or OperationRegion().
      // (TableHeader + AmlOffsetTable.Offset) is at offset for value to change.
      //
      // The assert below confirms that AML structure matches the offsets table.
      // If not then patching the AML would just corrupt it and result in OS failure.
      // If you encounter this assert something went wrong in *.offset.h files
      // generation. Remove the files and rebuild.
      //
      ASSERT(DsdtPointer[-1] == mAmlOffsetTablePointer[i].Opcode);
      //
      // AmlOffsetTable.Value has FIX tag, so check that to decide what to modify.
      //
      Signature = (UINT32 *) (&mAmlOffsetTablePointer[i].Value);
      switch (*Signature) {
        case (SIGNATURE_32 ('F', 'I', 'X', '0')):
          //
          // "FIX0" OperationRegion() in Acpi\AcpiTables\Dsdt\CommonPlatform.asi
          //
          *(UINT32*)DsdtPointer = (UINT32)(UINTN)AcpiParameter;
          DSDTDEBUG ("BIOS_PARAM pointer : 0x%X\n", AcpiParameter);
          break;
        default:
          DSDTDEBUG ("Object '%a' with opcode 0x%02X not patched\n",
                     mAmlOffsetTablePointer[i].Pathname, mAmlOffsetTablePointer[i].Opcode);
          break;
      }
    } else if (mAmlOffsetTablePointer[i].Opcode == AML_INDEX_OP) { //IioRootBridge(Cxl11/Cxl20).asi
      //
      // If Opcode is 0x88, then operator is WORDBusNumber() or WORDIO().
      // (TableHeader + AmlOffsetTable.Offset) must be cast to AML_RESOURCE_ADDRESS16 to change values.
      //
      AmlResourceAddress16Pointer = (EFI_ACPI_WORD_ADDRESS_SPACE_DESCRIPTOR *) (DsdtPointer);
      //
      // The assert below confirms that AML structure matches the offsets table.
      // If not then patching the AML would just corrupt it and result in OS failure.
      // If you encounter this assert something went wrong in *.offset.h files
      // generation. Remove the files and rebuild.
      //
      ASSERT(AmlResourceAddress16Pointer->Header.Header.Byte == mAmlOffsetTablePointer[i].Opcode);

      //
      // Last 4 chars of AmlOffsetTable.Pathname has FIX tag.
      //
      Signature = (UINT32 *) (mAmlOffsetTablePointer[i].Pathname + AsciiStrLen(mAmlOffsetTablePointer[i].Pathname) - 4);
      switch (*Signature) {
        case (SIGNATURE_32 ('F', 'I', 'X', '1')):
          //
          // "FIX1" is PCI bus resource for PCI root bridge object in Acpi/AcpiTables/Dsdt/IioRootBridge.asi
          // and also IioRootBridgeCxl11.asi (should we define new FIX<n> for this one to simplify things?)
          //
          Status = GetAcpiPciRootResource (mAmlOffsetTablePointer[i].Pathname, IioUdsPtr, &PciRootRes);
          if (EFI_ERROR (Status)) {
            DSDTDEBUG ("FIX1: Can not get '%a' root resource- skip\n", mAmlOffsetTablePointer[i].Pathname);
            break;
          }
          BusBase = PciRootRes.BusBase;
          BusLimit = PciRootRes.BusLimit;
          if (BusLimit < BusBase) {

            DSDTDEBUG ("FIX1: '%a' No bus resource - skip\n", mAmlOffsetTablePointer[i].Pathname);
            break;
          }
          DSDTDEBUG ("FIX1: '%a' BusBase %02X->%02X, BusLimit %02X->%02X\n", mAmlOffsetTablePointer[i].Pathname,
                     AmlResourceAddress16Pointer->AddrRangeMin, BusBase, AmlResourceAddress16Pointer->AddrRangeMax, BusLimit);
                     AmlResourceAddress16Pointer->AddrSpaceGranularity = 0;
                     AmlResourceAddress16Pointer->AddrRangeMin = BusBase;
                     AmlResourceAddress16Pointer->AddrRangeMax = BusLimit;
                     AmlResourceAddress16Pointer->AddrLen = BusLimit - BusBase + 1;
                     SetAddressSpaceRangesFixed ((ADDRESS_SPACE_DESCRIPTOR_GENERAL_FLAGS *) &AmlResourceAddress16Pointer->GenFlag);
          break;

        case (SIGNATURE_32 ('F', 'I', 'X', '2')):
          //
          // "FIX2" is IO resource for PCI Root object in Acpi/AcpiTables/Dsdt/IioRootBridge.asi
          //
          Status = GetAcpiPciRootResource (mAmlOffsetTablePointer[i].Pathname, IioUdsPtr, &PciRootRes);
          if (EFI_ERROR (Status)) {
            DSDTDEBUG ("FIX2: Can not get '%a' root resource- skip\n", mAmlOffsetTablePointer[i].Pathname);
            break;
          }
          IoBase = PciRootRes.IoBase;
          IoLimit = PciRootRes.IoLimit;
          if (IoLimit < IoBase) {
            DSDTDEBUG ("FIX2: '%a' No I/O resource - skip\n", mAmlOffsetTablePointer[i].Pathname);
            break;
          }
          DSDTDEBUG ("FIX2: '%a' IoBase %04X->%04X, IoLimit %04X->%04X\n", mAmlOffsetTablePointer[i].Pathname,
                     AmlResourceAddress16Pointer->AddrRangeMin, IoBase, AmlResourceAddress16Pointer->AddrRangeMax, IoLimit);
                     AmlResourceAddress16Pointer->AddrSpaceGranularity = 0;
                     AmlResourceAddress16Pointer->AddrRangeMin = IoBase;
                     AmlResourceAddress16Pointer->AddrRangeMax = IoLimit;
                     AmlResourceAddress16Pointer->AddrLen = IoLimit - IoBase + 1;
                     SetAddressSpaceRangesFixed ((ADDRESS_SPACE_DESCRIPTOR_GENERAL_FLAGS *) &AmlResourceAddress16Pointer->GenFlag);
          break;

        case (SIGNATURE_32('F', 'I', 'X', '9')) :
          //
          // "FIX9" PCI bus resource for UNRS in Acpi\AcpiTables\Dsdt\Uncore.asi
          //
          AcpiDevType = AcpiPciDevPathParse (mAmlOffsetTablePointer[i].Pathname, &Socket, &Stack, &Root, NULL);
          if (Socket >= MAX_SOCKET || !(AcpiParameter->SocketBitMask & 1 << Socket)) {
            DSDTDEBUG ("FIX9: Socket [%d] not present - skip '%a'\n", Socket, mAmlOffsetTablePointer[i].Pathname);
            break;
          }
          BusBase = IioUdsPtr->PlatformData.IIO_resource[Socket].StackRes[UBOX_STACK].BusBase;
          BusLimit = IioUdsPtr->PlatformData.IIO_resource[Socket].StackRes[UBOX_STACK].BusLimit;
          if (AcpiDevType != AcpiDevUncore ||
              IioUdsPtr->PlatformData.IIO_resource[Socket].StackRes[UBOX_STACK].Personality != TYPE_UBOX) {
            DSDTDEBUG ("Invalid uncore stack [%d.%d] type %d  - skip '%a'\n",
                       Socket, UBOX_STACK,
                       IioUdsPtr->PlatformData.IIO_resource[Socket].StackRes[UBOX_STACK].Personality,
                       mAmlOffsetTablePointer[i].Pathname);
            break;
          }
          if (BusLimit < BusBase || Stack > BusLimit - BusBase) {
            DSDTDEBUG ("FIX9: Not enougth bus resource in stack [%d.%d] - skip '%a'\n", Socket, UBOX_STACK,  mAmlOffsetTablePointer[i].Pathname);
            break;
          }
          DSDTDEBUG ("FIX9: '%a' IoBase %04X->%04X, IoLimit %04X->%04X\n", mAmlOffsetTablePointer[i].Pathname,
                     AmlResourceAddress16Pointer->AddrRangeMin, BusBase + Stack,
                     AmlResourceAddress16Pointer->AddrRangeMax, BusBase + Stack);
                     AmlResourceAddress16Pointer->AddrSpaceGranularity = 0;
                     AmlResourceAddress16Pointer->AddrRangeMin = BusBase + Stack;
                     AmlResourceAddress16Pointer->AddrRangeMax = BusBase + Stack;
                     AmlResourceAddress16Pointer->AddrLen = 1;
                     AcpiParameter->UncorePresentBitmap[Socket] |= 1 << Stack;
                     SetAddressSpaceRangesFixed ((ADDRESS_SPACE_DESCRIPTOR_GENERAL_FLAGS *) &AmlResourceAddress16Pointer->GenFlag);
          break;

        case (SIGNATURE_32 ('F', 'I', 'X', '6')):
          //
          // "FIX6" IO resource for video in PCXX in IioRootBridge.asi
          //
          AcpiDevType = AcpiPciDevPathParse (mAmlOffsetTablePointer[i].Pathname, &Socket, &Stack, &Root, NULL);
          if (Socket >= MAX_SOCKET || Stack >= MAX_IIO_STACKS_PER_SOCKET ||
              !(IioUdsPtr->PlatformData.CpuQpiInfo[Socket].StackPresentBitmap & 1 << Stack)) {
            DSDTDEBUG ("FIX6: Stack [%d.%d] not present - skip '%a'\n", Socket, Stack, mAmlOffsetTablePointer[i].Pathname);
            break;
          }
          if ((AcpiDevType == AcpiDevPci  || AcpiDevType == AcpiDevCxl2) &&
              LegacyVgaStackSoc == Socket && LegacyVgaStackFwInst == Stack) {
            DSDTDEBUG ("FIX6: IoBase %04X->%04X, BusLimit %04X->%04X\n",
                       AmlResourceAddress16Pointer->AddrRangeMin, 0x03B0, AmlResourceAddress16Pointer->AddrRangeMax, 0x03BB);
                       AmlResourceAddress16Pointer->AddrSpaceGranularity = 0;
                       AmlResourceAddress16Pointer->AddrRangeMin = (UINT16) 0x03B0;
                       AmlResourceAddress16Pointer->AddrRangeMax = (UINT16) 0x03BB;
                       AmlResourceAddress16Pointer->AddrLen = (UINT16) 0x000C;
                       SetAddressSpaceRangesFixed ((ADDRESS_SPACE_DESCRIPTOR_GENERAL_FLAGS *) &AmlResourceAddress16Pointer->GenFlag);

          } else {
            DSDTDEBUG ("FIX6: No legacy VGA in stack [%d.%d] - skip '%a'\n", Socket, Stack, mAmlOffsetTablePointer[i].Pathname);
          }
          break;

        case (SIGNATURE_32 ('F', 'I', 'X', '7')):
          //
          // "FIX7" IO resource for video PCXX in Acpi\AcpiTables\Dsdt\PCXX.asi
          //
          AcpiDevType = AcpiPciDevPathParse (mAmlOffsetTablePointer[i].Pathname, &Socket, &Stack, &Root, NULL);
          if (Socket >= MAX_SOCKET || Stack >= MAX_IIO_STACKS_PER_SOCKET ||
              !(IioUdsPtr->PlatformData.CpuQpiInfo[Socket].StackPresentBitmap & 1 << Stack)) {
            DSDTDEBUG ("FIX7: Stack [%d.%d] not present - skip '%a'\n", Socket, Stack, mAmlOffsetTablePointer[i].Pathname);
            break;
          }
          if ((AcpiDevType == AcpiDevPci  || AcpiDevType == AcpiDevCxl2) &&
              LegacyVgaStackSoc == Socket && LegacyVgaStackFwInst == Stack) {

            DSDTDEBUG ("FIX7: IoBase %04X->%04X, BusLimit %04X->%04X\n",
                       AmlResourceAddress16Pointer->AddrRangeMin, 0x03C0, AmlResourceAddress16Pointer->AddrRangeMax, 0x03DF);
                       AmlResourceAddress16Pointer->AddrSpaceGranularity = 0;
                       AmlResourceAddress16Pointer->AddrRangeMin = (UINT16) 0x03C0;
                       AmlResourceAddress16Pointer->AddrRangeMax = (UINT16) 0x03DF;
                       AmlResourceAddress16Pointer->AddrLen = (UINT16) 0x0020;
                       SetAddressSpaceRangesFixed ((ADDRESS_SPACE_DESCRIPTOR_GENERAL_FLAGS *) &AmlResourceAddress16Pointer->GenFlag);

          } else {
            DSDTDEBUG ("FIX7: No legacy VGA in stack [%d.%d] - skip '%a'\n", Socket, Stack, mAmlOffsetTablePointer[i].Pathname);
          }
          break;

        default:
          DSDTDEBUG ("Object '%a' with opcode 0x%02X not patched\n", mAmlOffsetTablePointer[i].Pathname, mAmlOffsetTablePointer[i].Opcode);
          break;
      }
    } else if (mAmlOffsetTablePointer[i].Opcode == AML_SIZE_OF_OP) {
      //
      // If Opcode is 0x87, then operator is DWORDMemory().
      // (TableHeader + AmlOffsetTable.Offset) must be cast to AML_RESOURCE_ADDRESS32 to change values.
      //
      AmlResourceAddress32Pointer = (EFI_ACPI_DWORD_ADDRESS_SPACE_DESCRIPTOR *) (DsdtPointer);
      //
      // The assert below confirms that AML structure matches the offsets table.
      // If not then patching the AML would just corrupt it and result in OS failure.
      // If you encounter this assert something went wrong in *.offset.h files
      // generation. Remove the files and rebuild.
      //
     ASSERT(AmlResourceAddress32Pointer->Header.Header.Byte == mAmlOffsetTablePointer[i].Opcode);
      //
      // Last 4 chars of AmlOffsetTable.Pathname has FIX tag.
      //
      Signature = (UINT32 *) (mAmlOffsetTablePointer[i].Pathname + AsciiStrLen(mAmlOffsetTablePointer[i].Pathname) - 4);
      switch (*Signature) {
        //
        // "FIX3" MMIO32 resource for PCXX in Acpi\AcpiTables\Dsdt\SysBus.asi and PCXX.asi
        //
        case (SIGNATURE_32 ('F', 'I', 'X', '3')):
          Status = GetAcpiPciRootResource (mAmlOffsetTablePointer[i].Pathname, IioUdsPtr, &PciRootRes);
          if (EFI_ERROR (Status)) {
            DSDTDEBUG ("FIX3: Can not get '%a' root resource- skip\n", mAmlOffsetTablePointer[i].Pathname);
            break;
          }
          Mmio32Base = PciRootRes.Mmio32Base;
          Mmio32Limit = PciRootRes.Mmio32Limit;

          if (Mmio32Limit <= Mmio32Base) {
            DSDTDEBUG ("FIX3: '%a' No MMIO32 resource - skip\n", mAmlOffsetTablePointer[i].Pathname);
            break;
          }

          DSDTDEBUG ("FIX3: '%a' Mmio32Base %08X->%08X, Mmio32Limit %08X->%08X\n", mAmlOffsetTablePointer[i].Pathname,
                     AmlResourceAddress32Pointer->AddrRangeMin, Mmio32Base, AmlResourceAddress32Pointer->AddrRangeMax, Mmio32Limit);
                     AmlResourceAddress32Pointer->AddrSpaceGranularity = 0;
                     AmlResourceAddress32Pointer->AddrRangeMin = Mmio32Base;
                     AmlResourceAddress32Pointer->AddrRangeMax = Mmio32Limit;
                     AmlResourceAddress32Pointer->AddrLen = Mmio32Limit - Mmio32Base + 1;
                     SetAddressSpaceRangesFixed ((ADDRESS_SPACE_DESCRIPTOR_GENERAL_FLAGS *) &AmlResourceAddress32Pointer->GenFlag);
          break;

        case (SIGNATURE_32 ('F', 'I', 'X', '5')):
          //
          // "FIX5" video RAM on video card for PCXX in Acpi\AcpiTables\Dsdt\PCXX.asi
          //
          AcpiDevType = AcpiPciDevPathParse (mAmlOffsetTablePointer[i].Pathname, &Socket, &Stack, &Root, NULL);
          if (Socket >= MAX_SOCKET || Stack >= MAX_IIO_STACKS_PER_SOCKET ||
              !(IioUdsPtr->PlatformData.CpuQpiInfo[Socket].StackPresentBitmap & 1 << Stack)) {
            DSDTDEBUG ("FIX5: Stack [%d.%d] not present - skip '%a'\n", Socket, Stack, mAmlOffsetTablePointer[i].Pathname);
            break;
          }
          AmlResourceAddress32Pointer->AddrSpaceGranularity = 0;
          if ((AcpiDevType == AcpiDevPci  || AcpiDevType == AcpiDevCxl2) &&
              LegacyVgaStackSoc == Socket && LegacyVgaStackFwInst == Stack) {
              DSDTDEBUG ("FIX3: Mmio32Base %08X->%08X, Mmio32Limit %08X->%08X\n",
                         AmlResourceAddress32Pointer->AddrRangeMin, 0x000A0000, AmlResourceAddress32Pointer->AddrRangeMax, 0x000BFFFF);
                         AmlResourceAddress32Pointer->AddrRangeMin = 0x000A0000;
                         AmlResourceAddress32Pointer->AddrRangeMax = 0x000BFFFF;
                         AmlResourceAddress32Pointer->AddrLen = 0x00020000;
                         SetAddressSpaceRangesFixed ((ADDRESS_SPACE_DESCRIPTOR_GENERAL_FLAGS *) &AmlResourceAddress32Pointer->GenFlag);

          } else {
            DSDTDEBUG ("FIX5: No legacy VGA in stack [%d.%d] - skip '%a'\n", Socket, Stack, mAmlOffsetTablePointer[i].Pathname);
          }
          break;

        default:
          DSDTDEBUG ("Object '%a' with opcode 0x%02X not patched\n", mAmlOffsetTablePointer[i].Pathname, mAmlOffsetTablePointer[i].Opcode);
          break;
      }
    } else if (mAmlOffsetTablePointer[i].Opcode == AML_CREATE_DWORD_FIELD_OP) {
      //
      // If Opcode is 0x8A, then operator is QWORDMemory().
      // (TableHeader + AmlOffsetTable.Offset) must be cast to AML_RESOURCE_ADDRESS64 to change values.
      //
      AmlResourceAddress64Pointer = (EFI_ACPI_QWORD_ADDRESS_SPACE_DESCRIPTOR *) (DsdtPointer);
      //
      // The assert below confirms that AML structure matches the offsets table.
      // If not then patching the AML would just corrupt it and result in OS failure.
      // If you encounter this assert something went wrong in *.offset.h files
      // generation. Remove the files and rebuild.
      //
      ASSERT(AmlResourceAddress64Pointer->Header.Header.Byte == mAmlOffsetTablePointer[i].Opcode);
      //
      // Last 4 chars of AmlOffsetTable.Pathname has FIX tag.
      //
      Signature = (UINT32 *) (mAmlOffsetTablePointer[i].Pathname + AsciiStrLen(mAmlOffsetTablePointer[i].Pathname) - 4);
      switch (*Signature) {
        //
        // "FIX4" PCI64 resource for PCXX in Acpi\AcpiTables\Dsdt\SysBus.asi and PCXX.asi
        //
        case (SIGNATURE_32 ('F', 'I', 'X', '4')):
          Status = GetAcpiPciRootResource (mAmlOffsetTablePointer[i].Pathname, IioUdsPtr, &PciRootRes);
          if (EFI_ERROR (Status)) {
            DSDTDEBUG ("FIX4: Can not get '%a' root resource- skip\n", mAmlOffsetTablePointer[i].Pathname);
            break;
          }
          Mmio64Base = PciRootRes.Mmio64Base;
          Mmio64Limit = PciRootRes.Mmio64Limit;

          if (Mmio64Limit <= Mmio64Base) {
            DSDTDEBUG ("FIX4: '%a' No MMIO64 resource - skip\n", mAmlOffsetTablePointer[i].Pathname);
            break;
          }
          DSDTDEBUG ("FIX4: '%a' Mmio64Base %08llX->%08llX, Mmio64Limit %08llX->%08llX\n", mAmlOffsetTablePointer[i].Pathname,
                     AmlResourceAddress64Pointer->AddrRangeMin, Mmio64Base, AmlResourceAddress64Pointer->AddrRangeMax, Mmio64Limit);
                     AmlResourceAddress64Pointer->AddrSpaceGranularity = 0;
                     AmlResourceAddress64Pointer->AddrRangeMin = Mmio64Base;
                     AmlResourceAddress64Pointer->AddrRangeMax = Mmio64Limit;
                     AmlResourceAddress64Pointer->AddrLen = Mmio64Limit - Mmio64Base + 1;
                     SetAddressSpaceRangesFixed ((ADDRESS_SPACE_DESCRIPTOR_GENERAL_FLAGS *) &AmlResourceAddress64Pointer->GenFlag);
          break;

        default:
          DSDTDEBUG ("WARNING: Object '%a' with opcode 0x%02X not patched\n", mAmlOffsetTablePointer[i].Pathname, mAmlOffsetTablePointer[i].Opcode);
          break;
      }
    } else {
      DSDTDEBUG ("WARNING: Object '%a' with opcode 0x%02X not patched\n", mAmlOffsetTablePointer[i].Pathname, mAmlOffsetTablePointer[i].Opcode);
    }
  }

  // CurrPtr = beginning of table
  //
  CurrPtr = (UINT8 *) TableHeader;

  // EndPtr = beginning of table + length of table
  //
  EndPtr = (CurrPtr + ((EFI_ACPI_COMMON_HEADER *) CurrPtr)->Length);

  // Subtract from End Ptr the largest data item we read from table
  //  so we don't try to access data beyond end of table
  //
  EndPtr -= 9;

  for (DsdtPointer = CurrPtr; DsdtPointer <= EndPtr; DsdtPointer++) {

    for (Socket = 0; Socket < MAX_SOCKET; Socket++) {
      //
      // Find APT##socket name
      //
      if ((DsdtPointer[0] == 'A') && (DsdtPointer[1] == 'P') && (DsdtPointer[2] == 'T') && (DsdtPointer[3] == '0' + Socket)) {
        NamePtr = ACPI_NAME_COMMAND_FROM_NAMEPACK_STR (DsdtPointer);
        if (NamePtr->StartByte != AML_NAME_OP) {
          continue;
        }

        Counter16 = *(UINT16 *) &DsdtPointer[8];
        ASSERT ((Counter16 / sizeof (UINT32)) >= (UINT32) (MAX_THREAD * MAX_CORE));
        DSDTDEBUG ("Found 'APT%x'...Counter16 = 0x%x\n", Socket, Counter16);
        for (i = 0; i < (MAX_THREAD * MAX_CORE); i++) {
          *(UINT32 *)&DsdtPointer[i*4+10] = mApicIdMap[Socket][i];
        }
      }
      //
      // Find SPD##socket name
      //
      if ((DsdtPointer[0] == 'S') && (DsdtPointer[1] == 'P') && (DsdtPointer[2] == 'D') && (DsdtPointer[3] == '0' + Socket)) {
        NamePtr = ACPI_NAME_COMMAND_FROM_NAMEPACK_STR (DsdtPointer);
        if (NamePtr->StartByte != AML_NAME_OP) {
          continue;
        }

        Counter16 = *(UINT16 *) &DsdtPointer[8];
        DSDTDEBUG ("Found 'SPD%x'...Counter16 = 0x%x\n", Socket, Counter16);
        ASSERT (Counter16 >= (UINT32) (MAX_THREAD * MAX_CORE));
        for (i = 0; i < (MAX_THREAD * MAX_CORE); i++) {
          DsdtPointer[i+10] = (UINT8) mSNCProximityDomainMap[Socket][i];
        }
      }
    }
    //
    // update PEBS
    //
    if ((DsdtPointer[0] == 'P') && (DsdtPointer[1] == 'E') && (DsdtPointer[2] == 'B') && (DsdtPointer[3] == 'S')) {
      NamePtr = ACPI_NAME_COMMAND_FROM_NAMEPACK_STR (DsdtPointer);
      if (NamePtr->StartByte != AML_NAME_OP) {
        DSDTDEBUG ("...Found 'PEBS', but it is not NAME_OP, continue...\n");
        continue;
      } else {
        NamePtr->Value = (UINT32)(UINTN)PcdGet64 (PcdPciExpressBaseAddress);
      }
    }

    //
    // Fix up _S3
    //
    if ((DsdtPointer[0] == '_') && (DsdtPointer[1] == 'S') && (DsdtPointer[2] == '3')) {
      NamePtr = ACPI_NAME_COMMAND_FROM_NAMEPACK_STR (DsdtPointer);
      if (NamePtr->StartByte != AML_NAME_OP) {
        continue;
      }
      //
      // S3 disabled
      //
      DSDTDEBUG ("[ACPI] S3 disabled\n");
      DsdtPointer[0] = 'D';
    }
    //
    // Fix up _S4
    //
    if ((DsdtPointer[0] == '_') && (DsdtPointer[1] == 'S') && (DsdtPointer[2] == '4')) {
      NamePtr = ACPI_NAME_COMMAND_FROM_NAMEPACK_STR (DsdtPointer);
      if (NamePtr->StartByte != AML_NAME_OP) {
        continue;
      }
      //
      // S4 disabled
      //
      DsdtPointer[0] = 'D';
    }

  }
  return EFI_SUCCESS;
}
