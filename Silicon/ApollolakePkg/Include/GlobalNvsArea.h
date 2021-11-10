/** @file

  Copyright (c) 2017, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef _GLOBAL_NVS_AREA_H_
#define _GLOBAL_NVS_AREA_H_

//
// Includes
//
#define GLOBAL_NVS_DEVICE_ENABLE 1
#define GLOBAL_NVS_DEVICE_DISABLE 0

//
// Revision id - Added TPM related fields
//
#define GLOBAL_NVS_AREA_RIVISION_1       1

#define PPM_EIST                0x1
#define PPM_C1                  0x2
#define PPM_C1E                 0x4
#define PPM_TURBO               0x100
#define PPM_CMP                 0x200

///
/// Global NVS Area definition
///
#pragma pack (push,1)

typedef struct {
/**
  Miscellaneous Dynamic Values, the definitions below need to be matched
  GNVS definitions in Platform.ASL
**/
  UINT8       LogicalProcessorCount;                  ///<   Number of Logical Processors if MP Enabled != 0
  UINT8       BoardId;                                ///<   Platform board id
  UINT8       Reserved1;                              ///<   Reserved for DWORD alignment
  UINT8       Reserved2;                              ///<   Reserved for DWORD alignment
  UINT32      IgdOpRegionAddress;                     ///<   IGD OpRegion base address
  UINT32      HdaNhltAcpiAddr;                        ///<   HD-Audio NHLT ACPI address
  UINT32      HdaNhltAcpiLength;                      ///<   HD-Audio NHLT ACPI length
  UINT32      GPIO0Addr;                              ///<   GPIO0 BAR
  UINT32      GPIO0Len;                               ///<   GPIO0 BAR Length
  UINT32      GPIO1Addr;                              ///<   GPIO1 BAR
  UINT32      GPIO1Len;                               ///<   GPIO1 BAR Length
  UINT32      GPIO2Addr;                              ///<   GPIO2 BAR
  UINT32      GPIO2Len;                               ///<   GPIO2 BAR Length
  UINT32      GPIO3Addr;                              ///<   GPIO3 BAR
  UINT32      GPIO3Len;                               ///<   GPIO3 BAR Length
  UINT32      IPCBar0Address;                         ///<   IPC Bar0 base address
  UINT32      IPCBar0Length;                          ///<   IPC Bar0 Length
  UINT32      SSRAMBar0Address;                       ///<   IPC Bar0 base address
  UINT32      SSRAMBar0Length;                        ///<   IPC Bar0 Length
  UINT32      IPCBIOSMailBoxData;                     ///<   IPC BIOS mail box data
  UINT32      IPCBIOSMailBoxInterface;                ///<   IPC BIOS mail box interface
  UINT32      P2SBBaseAddress;                        ///<   P2SB Base Address
  UINT8       Ipc1Enable;                             ///<   IPC1Enable
  UINT8       BxtStepping;                            ///<   BXT Stepping
  UINT8       Reserved4[2];                           ///<   Reserved for DWORD alignment
  UINT32      RpAddress[6];                           ///<   Root Port address 1
  UINT32      Mmio32Base;                             ///<   PCIE MMIO resource base
  UINT32      Mmio32Length;                           ///<   PCIE MMIO resource length
  UINT32      HdaDspFeatureMask;                      ///<   HD-Audio DSP Feature Mask
  UINT32      HdaDspModMask;                          ///<   Hd-Audio DSP Post-Processing Module Mask
  UINT8       PcieRpGpeWakeBit[6];                    ///<   PCIE RP wake GPE bit offset
  UINT8       Reserved5[2];
} PLATFORM_NVS_AREA;

typedef struct {
  UINT32      PpmFlags;
  UINT8       LogicalProcessorCount;
  UINT8       MonitorMwaitEnable;
} CPU_NVS_AREA;

typedef struct {
  PLATFORM_NVS_AREA     PlatformNvs;
  CPU_NVS_AREA          CpuNvs;
} GLOBAL_NVS_AREA;

#pragma pack(pop)

#endif
