
/**@file

  Copyright (c) 2024, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

  //
  // Define MTP-S NVS Area operatino region.
  //
#ifndef _MTL_PCH_NVS_AREA_DEF_H_
#define _MTL_PCH_NVS_AREA_DEF_H_

#pragma pack (push,1)
typedef struct {
  UINT32   PWRM;                                    ///< Offset 0       PWRM MEM BASE address
  UINT16   AcpiBase;                                ///< Offset 4       PMC ACPI base address
  UINT64   SBRG;                                    ///< Offset 6       Sideband base address
  UINT8    PU2C;                                    ///< Offset 14      Number of USB2 ports in PCH
  UINT8    PU3C;                                    ///< Offset 15      Number of USB3 ports in PCH
  UINT8    GBES;                                    ///< Offset 16      GbE Support
  UINT64   NHLA;                                    ///< Offset 17      HD-Audio NHLT ACPI address
  UINT32   NHLL;                                    ///< Offset 25      HD-Audio NHLT ACPI length
  UINT32   ADFM;                                    ///< Offset 29      HD-Audio DSP Feature Mask
  UINT8    SWQ0;                                    ///< Offset 33      HD-Audio SoundWire Link #1 quirk mask
  UINT8    SWQ1;                                    ///< Offset 34      HD-Audio SoundWire Link #2 quirk mask
  UINT8    SWQ2;                                    ///< Offset 35      HD-Audio SoundWire Link #3 quirk mask
  UINT8    SWQ3;                                    ///< Offset 36      HD-Audio SoundWire Link #4 quirk mask
  UINT8    ACS0;                                    ///< Offset 37      HD-Audio SoundWire Link #1 Autonomous Clock Stop
  UINT8    ACS1;                                    ///< Offset 38      HD-Audio SoundWire Link #2 Autonomous Clock Stop
  UINT8    ACS2;                                    ///< Offset 39      HD-Audio SoundWire Link #3 Autonomous Clock Stop
  UINT8    ACS3;                                    ///< Offset 40      HD-Audio SoundWire Link #4 Autonomous Clock Stop
  UINT8    DAI0;                                    ///< Offset 41      HD-Audio SoundWire Link #1 Data On Active Interval Select
  UINT8    DAI1;                                    ///< Offset 42      HD-Audio SoundWire Link #2 Data On Active Interval Select
  UINT8    DAI2;                                    ///< Offset 43      HD-Audio SoundWire Link #3 Data On Active Interval Select
  UINT8    DAI3;                                    ///< Offset 44      HD-Audio SoundWire Link #4 Data On Active Interval Select
  UINT8    DOD0;                                    ///< Offset 45      HD-Audio SoundWire Link #1 Data On Delay Select
  UINT8    DOD1;                                    ///< Offset 46      HD-Audio SoundWire Link #2 Data On Delay Select
  UINT8    DOD2;                                    ///< Offset 47      HD-Audio SoundWire Link #3 Data On Delay Select
  UINT8    DOD3;                                    ///< Offset 48      HD-Audio SoundWire Link #4 Data On Delay Select
  UINT16   HDBO;                                    ///< Offset 49      HD-Audio Discrete BT Offload
  UINT8    SWMC;                                    ///< Offset 51      HD-Audio SoundWire Host Count
  UINT32   XTAL;                                    ///< Offset 52      XTAL frequency: 0: 24MHz; 1: 38.4MHz; 2: Unsupported
  UINT32   RpAddress[24];                           ///< Offset 56      Root Port address 1
                                                    ///< Offset 60      Root Port address 2
                                                    ///< Offset 64      Root Port address 3
                                                    ///< Offset 68      Root Port address 4
                                                    ///< Offset 72      Root Port address 5
                                                    ///< Offset 76      Root Port address 6
                                                    ///< Offset 80      Root Port address 7
                                                    ///< Offset 84      Root Port address 8
                                                    ///< Offset 88      Root Port address 9
                                                    ///< Offset 92      Root Port address 10
                                                    ///< Offset 96      Root Port address 11
                                                    ///< Offset 100     Root Port address 12
                                                    ///< Offset 104     Root Port address 13
                                                    ///< Offset 108     Root Port address 14
                                                    ///< Offset 112     Root Port address 15
                                                    ///< Offset 116     Root Port address 16
                                                    ///< Offset 120     Root Port address 17
                                                    ///< Offset 124     Root Port address 18
                                                    ///< Offset 128     Root Port address 19
                                                    ///< Offset 132     Root Port address 20
                                                    ///< Offset 136     Root Port address 21
                                                    ///< Offset 140     Root Port address 22
                                                    ///< Offset 144     Root Port address 23
                                                    ///< Offset 148     Root Port address 24
  UINT16   PcieLtrMaxSnoopLatency[24];              ///< Offset 152     PCIE LTR max snoop Latency 1
                                                    ///< Offset 154     PCIE LTR max snoop Latency 2
                                                    ///< Offset 156     PCIE LTR max snoop Latency 3
                                                    ///< Offset 158     PCIE LTR max snoop Latency 4
                                                    ///< Offset 160     PCIE LTR max snoop Latency 5
                                                    ///< Offset 162     PCIE LTR max snoop Latency 6
                                                    ///< Offset 164     PCIE LTR max snoop Latency 7
                                                    ///< Offset 166     PCIE LTR max snoop Latency 8
                                                    ///< Offset 168     PCIE LTR max snoop Latency 9
                                                    ///< Offset 170     PCIE LTR max snoop Latency 10
                                                    ///< Offset 172     PCIE LTR max snoop Latency 11
                                                    ///< Offset 174     PCIE LTR max snoop Latency 12
                                                    ///< Offset 176     PCIE LTR max snoop Latency 13
                                                    ///< Offset 178     PCIE LTR max snoop Latency 14
                                                    ///< Offset 180     PCIE LTR max snoop Latency 15
                                                    ///< Offset 182     PCIE LTR max snoop Latency 16
                                                    ///< Offset 184     PCIE LTR max snoop Latency 17
                                                    ///< Offset 186     PCIE LTR max snoop Latency 18
                                                    ///< Offset 188     PCIE LTR max snoop Latency 19
                                                    ///< Offset 190     PCIE LTR max snoop Latency 20
                                                    ///< Offset 192     PCIE LTR max snoop Latency 21
                                                    ///< Offset 194     PCIE LTR max snoop Latency 22
                                                    ///< Offset 196     PCIE LTR max snoop Latency 23
                                                    ///< Offset 198     PCIE LTR max snoop Latency 24
  UINT16   PcieLtrMaxNoSnoopLatency[24];            ///< Offset 200     PCIE LTR max no snoop Latency 1
                                                    ///< Offset 202     PCIE LTR max no snoop Latency 2
                                                    ///< Offset 204     PCIE LTR max no snoop Latency 3
                                                    ///< Offset 206     PCIE LTR max no snoop Latency 4
                                                    ///< Offset 208     PCIE LTR max no snoop Latency 5
                                                    ///< Offset 210     PCIE LTR max no snoop Latency 6
                                                    ///< Offset 212     PCIE LTR max no snoop Latency 7
                                                    ///< Offset 214     PCIE LTR max no snoop Latency 8
                                                    ///< Offset 216     PCIE LTR max no snoop Latency 9
                                                    ///< Offset 218     PCIE LTR max no snoop Latency 10
                                                    ///< Offset 220     PCIE LTR max no snoop Latency 11
                                                    ///< Offset 222     PCIE LTR max no snoop Latency 12
                                                    ///< Offset 224     PCIE LTR max no snoop Latency 13
                                                    ///< Offset 226     PCIE LTR max no snoop Latency 14
                                                    ///< Offset 228     PCIE LTR max no snoop Latency 15
                                                    ///< Offset 230     PCIE LTR max no snoop Latency 16
                                                    ///< Offset 232     PCIE LTR max no snoop Latency 17
                                                    ///< Offset 234     PCIE LTR max no snoop Latency 18
                                                    ///< Offset 236     PCIE LTR max no snoop Latency 19
                                                    ///< Offset 238     PCIE LTR max no snoop Latency 20
                                                    ///< Offset 240     PCIE LTR max no snoop Latency 21
                                                    ///< Offset 242     PCIE LTR max no snoop Latency 22
                                                    ///< Offset 244     PCIE LTR max no snoop Latency 23
                                                    ///< Offset 246     PCIE LTR max no snoop Latency 24
  UINT32   ThcWakOnTouchInterrupt[2];               ///< Offset 248     Touch Host Controller Wake On Touch Interrupt Number - when 0 wake is disabled
  UINT16   ThcMode[2];                              ///< Offset 256     Touch Host Controller Mode THC or HID over SPI
  UINT8    Ufs0Enabled;                             ///< Offset 260     Is UFS0 Enabled
  UINT8    Ufs1Enabled;                             ///< Offset 261     Is UFS1 Enabled
  UINT8    UaolEnabled;                             ///< Offset 262     Is USB Audio Offload enabled
  UINT32   ThcResetPad[2];                          ///< Offset 263     Touch Host Controller HID over SPI Reset Pad
  UINT8    ThcResetPadTrigger[2];                   ///< Offset 271     Touch Host Controller HID over SPI Reset Pad Trigger
  UINT32   ThcHidSpiConnectionSpeed[2];             ///< Offset 273     Touch Host Controller HID over SPI Connection Speed
  UINT32   ThcHidSpiInputReportHeaderAddress[2];    ///< Offset 281     Touch Host Controller HID over SPI Input Report Header Address
  UINT32   ThcHidSpiInputReportBodyAddress[2];      ///< Offset 289     Touch Host Controller HID over SPI Input Report Body Address
  UINT32   ThcHidSpiOutputReportAddress[2];         ///< Offset 297     Touch Host Controller HID over SPI Output Report Address
  UINT32   ThcHidSpiReadOpcode[2];                  ///< Offset 305     Touch Host Controller HID over SPI Read Opcode
  UINT32   ThcHidSpiWriteOpcode[2];                 ///< Offset 313     Touch Host Controller HID over SPI Write Opcode
  UINT32   ThcHidSpiFlags[2];                       ///< Offset 321     Touch Host Controller HID over SPI Flags
  UINT32   ThcActiveLtr[2];                         ///< Offset 329     Touch Host Controller Active Ltr
  UINT32   ThcIdleLtr[2];                           ///< Offset 337     Touch Host Controller Idle Ltr
  UINT32   ThcHidSpiLimitPacketSize[2];             ///< Offset 345     Touch Host Controller Limit Packet Size
  UINT32   ThcPerformanceLimitation[2];             ///< Offset 353     Touch Host Controller Performance Limitation
  UINT32   ThcDisplayFrameSyncPeriod[2];            ///< Offset 361     Touch Host Controller Display Frame Sync Period
  UINT32   ThcResetSequencingDelay[2];              ///< Offset 369     Touch Host Controller Reset Sequencing Delay
  //SerialIo block
  UINT8    SM0[4];                                  ///< Offset 377     SerialIo SPI Controller 0 Mode
                                                    ///< Offset 378     SerialIo SPI Controller 1 Mode
                                                    ///< Offset 379     SerialIo SPI Controller 2 Mode
                                                    ///< Offset 380     SerialIo SPI Controller 3 Mode
  UINT64   SC0[4];                                  ///< Offset 381     SerialIo SPI Controller 0 Pci Config
                                                    ///< Offset 389     SerialIo SPI Controller 1 Pci Config
                                                    ///< Offset 397     SerialIo SPI Controller 2 Pci Config
                                                    ///< Offset 405     SerialIo SPI Controller 3 Pci Config
  UINT8    IM0[6];                                  ///< Offset 413     SerialIo I2C Controller 0 Mode
                                                    ///< Offset 414     SerialIo I2C Controller 1 Mode
                                                    ///< Offset 415     SerialIo I2C Controller 2 Mode
                                                    ///< Offset 416     SerialIo I2C Controller 3 Mode
                                                    ///< Offset 417     SerialIo I2C Controller 4 Mode
                                                    ///< Offset 418     SerialIo I2C Controller 5 Mode
  UINT64   IC0[6];                                  ///< Offset 419     SerialIo I2C Controller 0 Pci Config
                                                    ///< Offset 427     SerialIo I2C Controller 1 Pci Config
                                                    ///< Offset 435     SerialIo I2C Controller 2 Pci Config
                                                    ///< Offset 443     SerialIo I2C Controller 3 Pci Config
                                                    ///< Offset 451     SerialIo I2C Controller 4 Pci Config
                                                    ///< Offset 459     SerialIo I2C Controller 5 Pci Config
  UINT8    UM0[4];                                  ///< Offset 467     SerialIo UART Controller 0 Mode
                                                    ///< Offset 468     SerialIo UART Controller 1 Mode
                                                    ///< Offset 469     SerialIo UART Controller 2 Mode
                                                    ///< Offset 470     SerialIo UART Controller 3 Mode
  UINT64   UC0[4];                                  ///< Offset 471     SerialIo UART Controller 0 Pci Config
                                                    ///< Offset 479     SerialIo UART Controller 1 Pci Config
                                                    ///< Offset 487     SerialIo UART Controller 2 Pci Config
                                                    ///< Offset 495     SerialIo UART Controller 3 Pci Config
  UINT8    UD0[4];                                  ///< Offset 503     SerialIo UART Controller 0 DmaEnable
                                                    ///< Offset 504     SerialIo UART Controller 1 DmaEnable
                                                    ///< Offset 505     SerialIo UART Controller 2 DmaEnable
                                                    ///< Offset 506     SerialIo UART Controller 3 DmaEnable
  UINT8    UP0[4];                                  ///< Offset 507     SerialIo UART Controller 0 Power Gating
                                                    ///< Offset 508     SerialIo UART Controller 1 Power Gating
                                                    ///< Offset 509     SerialIo UART Controller 2 Power Gating
                                                    ///< Offset 510     SerialIo UART Controller 3 Power Gating
  UINT8    UI0[4];                                  ///< Offset 511     SerialIo UART Controller 0 Irq
                                                    ///< Offset 512     SerialIo UART Controller 1 Irq
                                                    ///< Offset 513     SerialIo UART Controller 2 Irq
                                                    ///< Offset 514     SerialIo UART Controller 3 Irq
  //end of SerialIo block
  UINT16   CnviPortId;                              ///< Offset 515     CNVi Sideband Port ID
  //Gpio groups mapped to GPE
  UINT8    GEI0;                                    ///< Offset 517     GPIO GroupIndex mapped to GPE_DW0
  UINT8    GEI1;                                    ///< Offset 518     GPIO GroupIndex mapped to GPE_DW1
  UINT8    GEI2;                                    ///< Offset 519     GPIO GroupIndex mapped to GPE_DW2
  UINT8    GED0;                                    ///< Offset 520     GPIO DW part of group mapped to GPE_DW0
  UINT8    GED1;                                    ///< Offset 521     GPIO DW part of group mapped to GPE_DW1
  UINT8    GED2;                                    ///< Offset 522     GPIO DW part of group mapped to GPE_DW2
} MTL_PCH_NVS_AREA;

#pragma pack(pop)
#endif
