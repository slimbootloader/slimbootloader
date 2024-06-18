

/** @file
  ACPI Platform NVS Data

  Copyright (c) 2024, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/


#ifndef _PLATFORM_NVS_AREA_DEF_H_
#define _PLATFORM_NVS_AREA_DEF_H_

#pragma pack (push,1)
typedef struct {
  //
  // Miscellaneous Dynamic Registers:
  //
  UINT16   OperatingSystem;                         ///< Offset 0       Operating System
  UINT8    SmiFunction;                             ///< Offset 2       SMI Function Call (ASL to SMI via I/O Trap)
  UINT8    SmiParameter0;                           ///< Offset 3       SMIF - Parameter 0
  UINT8    SmiParameter1;                           ///< Offset 4       SMIF - Parameter 1
  UINT8    SciFunction;                             ///< Offset 5       SCI Function Call (SMI to ASL via _L00)
  UINT8    SciParameter0;                           ///< Offset 6       SCIF - Parameter 0
  UINT8    SciParameter1;                           ///< Offset 7       SCIF - Parameter 1
  UINT8    GlobalLock;                              ///< Offset 8       Global Lock Function Call (EC Communication)
  UINT8    LockParameter0;                          ///< Offset 9       LCKF - Parameter 0
  UINT8    LockParameter1;                          ///< Offset 10      LCKF - Parameter 1
  UINT8    PowerState;                              ///< Offset 11      Power State (AC Mode = 1)
  UINT8    DebugState;                              ///< Offset 12      Debug State
  //
  // Thermal Policy Registers:
  //
  UINT8    EnableThermalKSC;                        ///< Offset 13      Enable Thermal Offset for KSC
  UINT8    Ac1TripPoint;                            ///< Offset 14      Active Trip Point 1
  UINT8    Ac0TripPoint;                            ///< Offset 15      Active Trip Point
  UINT8    PassiveThermalTripPoint;                 ///< Offset 16      Passive Trip Point
  UINT8    PassiveTc1Value;                         ///< Offset 17      Passive Trip Point TC1 Value
  UINT8    PassiveTc2Value;                         ///< Offset 18      Passive Trip Point TC2 Value
  UINT8    PassiveTspValue;                         ///< Offset 19      Passive Trip Point TSP Value
  UINT8    CriticalThermalTripPoint;                ///< Offset 20      Critical Trip Point
  //
  // Revision Field:
  //
  UINT8    Revision;                                ///< Offset 21      Revison of GlobalNvsArea
  //
  // CPU Identification Registers:
  //
  UINT8    ApicEnable;                              ///< Offset 22      APIC Enabled by SBIOS (APIC Enabled = 1)
  UINT8    ThreadCount;                             ///< Offset 23      Number of Enabled Threads
  UINT8    CurentPdcState0;                         ///< Offset 24      PDC Settings, Processor 0
  UINT8    CurentPdcState1;                         ///< Offset 25      PDC Settings, Processor 1
  UINT8    MaximumPpcState;                         ///< Offset 26      Maximum PPC state
  UINT32   PpmFlags;                                ///< Offset 27      PPM Flags (Same as CFGD)
  UINT8    C6C7Latency;                             ///< Offset 31      C6/C7 Entry/Exit latency
  //
  // SIO Configuration Registers:
  //
  UINT8    DockedSioPresent;                        ///< Offset 32      National SIO Present
  UINT8    DockComA;                                ///< Offset 33      COM A Port
  UINT8    DockComB;                                ///< Offset 34      COM B Port
  UINT8    DockLpt;                                 ///< Offset 35      LPT Port
  UINT8    DockFdc;                                 ///< Offset 36      FDC Port
  UINT8    OnboardCom;                              ///< Offset 37      SMSC Com Port
  UINT8    OnboardComCir;                           ///< Offset 38      SMSC Com CIR Port
  UINT8    SMSC1007;                                ///< Offset 39      SMSC1007 SIO Present
  UINT8    WPCN381U;                                ///< Offset 40      WPCN381U SIO Present
  UINT8    SMSC1000;                                ///< Offset 41      SMSC1000 SIO Present
  //
  // Extended Mobile Access Values
  //
  UINT8    EmaEnable;                               ///< Offset 42      EMA Enable
  UINT16   EmaPointer;                              ///< Offset 43      EMA Pointer
  UINT16   EmaLength;                               ///< Offset 45      EMA Length
  //
  // MEF Registers:
  //
  UINT8    MefEnable;                               ///< Offset 47      MEF Enable
  //
  // PCIe Dock Status:
  //
  UINT8    PcieDockStatus;                          ///< Offset 48      PCIe Dock Status
  //
  // TPM Registers
  //
  UINT8    MorData;                                 ///< Offset 49      Memory Overwrite Request Data
  UINT8    TcgParamter;                             ///< Offset 50      Used for save the Mor and/or physical presence paramter
  UINT32   PPResponse;                              ///< Offset 51      Physical Presence request operation response
  UINT8    PPRequest;                               ///< Offset 55      Physical Presence request operation
  UINT8    LastPPRequest;                           ///< Offset 56      Last Physical Presence request operation
  //
  // SATA Registers:
  //
  UINT8    IdeMode;                                 ///< Offset 57      IDE Mode (Compatible\Enhanced)
  //
  // Board Id
  //
  UINT8    PlatformId;                              ///< Offset 58      Platform id
  UINT8    BoardType;                               ///< Offset 59      Board Type
  //
  // PCIe Hot Plug
  //
  UINT8    PcieOSCControl;                          ///< Offset 60      PCIE OSC Control
  UINT8    NativePCIESupport;                       ///< Offset 61      Native PCIE Setup Value
  //
  // USB Sideband Deferring Support
  //
  UINT8    HostAlertVector1;                        ///< Offset 62      USB Sideband Deferring GPE Vector (HOST_ALERT#1)
  UINT8    HostAlertVector2;                        ///< Offset 63      USB Sideband Deferring GPE Vector (HOST_ALERT#2)
  //
  // Embedded Controller Availability Flag.
  //
  UINT8    EcAvailable;                             ///< Offset 64      Embedded Controller Availability Flag.
  //
  // Global Variables
  //
  UINT8    DisplaySupportFlag;                      ///< Offset 65      _DOS Display Support Flag.
  UINT8    InterruptModeFlag;                       ///< Offset 66      Global IOAPIC/8259 Interrupt Mode Flag.
  UINT8    CoolingTypeFlag;                         ///< Offset 67      Global Cooling Type Flag.
  UINT8    L01Counter;                              ///< Offset 68      Global L01 Counter.
  UINT8    VirtualFan0Status;                       ///< Offset 69      Virtual Fan0 Status.
  UINT8    VirtualFan1Status;                       ///< Offset 70      Virtual Fan1 Status.
  UINT8    VirtualFan2Status;                       ///< Offset 71      Virtual Fan2 Status.
  UINT8    VirtualFan3Status;                       ///< Offset 72      Virtual Fan3 Status.
  UINT8    VirtualFan4Status;                       ///< Offset 73      Virtual Fan4 Status.
  UINT8    VirtualFan5Status;                       ///< Offset 74      Virtual Fan5 Status.
  UINT8    VirtualFan6Status;                       ///< Offset 75      Virtual Fan6 Status.
  UINT8    VirtualFan7Status;                       ///< Offset 76      Virtual Fan7 Status.
  UINT8    VirtualFan8Status;                       ///< Offset 77      Virtual Fan8 Status.
  UINT8    VirtualFan9Status;                       ///< Offset 78      Virtual Fan9 Status.
  //
  // Thermal
  //
  UINT32   PlatformCpuId;                           ///< Offset 79      CPUID Feature Information [EAX]
  UINT32   TBARB;                                   ///< Offset 83      Reserved for Thermal Base Low Address for BIOS
  UINT32   TBARBH;                                  ///< Offset 87      Reserved for Thermal Base High Address for BIOS
  UINT8    TsOnDimmEnabled;                         ///< Offset 91      TS-on-DIMM is chosen in SETUP and present on the DIMM
  //
  // Board info
  //
  UINT8    PlatformFlavor;                          ///< Offset 92      Platform Flavor
  UINT16   BoardRev;                                ///< Offset 93      Board Rev
  //
  // Package temperature
  //
  UINT8    PeciAccessMethod;                        ///< Offset 95      Peci Access Method
  UINT8    Ac0FanSpeed;                             ///< Offset 96      _AC0 Fan Speed
  UINT8    Ac1FanSpeed;                             ///< Offset 97      _AC1 Fan Speed
  //
  // XTU 3.0 Specification
  //
  UINT32   XTUBaseAddress;                          ///< Offset 98      XTU Continous structure Base Address
  UINT32   XTUSize;                                 ///< Offset 102     XMP Size
  UINT32   XMPBaseAddress;                          ///< Offset 106     XMP Base Address
  UINT8    DDRReferenceFreq;                        ///< Offset 110     DDR Reference Frequency
  UINT8    Rtd3Support;                             ///< Offset 111     Runtime D3 support.
  UINT8    Rtd3P0dl;                                ///< Offset 112     User selctable Delay for Device D0 transition.
  UINT8    Rtd3P3dl;                                ///< Offset 113     User selctable Delay for Device D3 transition.
  UINT8    Rtd3Pcie;                                ///< Offset 114     RTD3 Support for PCIe
  UINT8    LowPowerS0Idle;                          ///< Offset 115     Low Power S0 Idle Enable
  UINT8    PuisEnable;                              ///< Offset 116     Power Up In Standby mode
  //
  // BIOS only version of Config TDP
  //
  UINT8    ConfigTdpBios;                           ///< Offset 117     enable/disable BIOS only version of Config TDP
  UINT8    DockSmi;                                 ///< Offset 118     Dock SMI number
  //
  // LPC SIO configuration
  //
  UINT16   LpcSioPort1;                             ///< Offset 119     SIO config port 1
  UINT16   LpcSioPort2;                             ///< Offset 121     SIO config port 2
  UINT16   LpcSioPmeBar;                            ///< Offset 123     SIO PME Base Address
  UINT8    PcdSCH3223SIO;                           ///< Offset 125     SCH3223 SIO Present
  UINT8    PcdSCH3223COM1;                          ///< Offset 126     SCH3223 COM1
  UINT8    PcdSCH3223COM2;                          ///< Offset 127     SCH3223 COM2
  UINT8    Reserved0[371];                          ///< Offset 128:498
  UINT8    PseudoG3State;                           ///< Offset 499     Pseudo G3 Enable / Disable
  UINT8    UsbPowerResourceTest;                    ///< Offset 500     RTD3 USB Power Resource config
  UINT8    Rtd3I2C0SensorHub;                       ///< Offset 501     RTD3 support for I2C0 SH
  UINT8    VirtualGpioButtonSxBitmask;              ///< Offset 502     Virtual GPIO button Notify Sleep State Change
  UINT16   Rtd3AudioDelay;                          ///< Offset 503     RTD3 Audio Codec device delay
  UINT16   Rtd3TouchPadDelay;                       ///< Offset 505     RTD3 TouchPad delay time after applying power to device
  UINT16   Rtd3TouchPanelDelay;                     ///< Offset 507     RTD3 TouchPanel delay time after applying power to device
  UINT16   Rtd3SensorHub;                           ///< Offset 509     RTD3 SensorHub delay time after applying power to device
  UINT16   VRRampUpDelay;                           ///< Offset 511     VR Ramp up delay
  UINT8    PstateCapping;                           ///< Offset 513     P-state Capping
  UINT16   Rtd3I2C0ControllerPS0Delay;              ///< Offset 514     Delay in _PS0 after powering up I2C0 Controller
  UINT16   Rtd3I2C1ControllerPS0Delay;              ///< Offset 516     Delay in _PS0 after powering up I2C1 Controller
  UINT16   Rtd3Config0;                             ///< Offset 518     RTD3 Config Setting0(BIT0:ZPODD, BIT1:Reserved, BIT2:PCIe NVMe, BIT3:BT, BIT4:SKL SDS SIP I2C Touch, BIT6:Card Reader, BIT7:WWAN)
  UINT16   Rtd3Config1;                             ///< Offset 520     RTD3 Config Setting1(BIT0:Sata Port0, BIT1:Sata Port1, BIT2:Sata Port2, BIT3:Sata Port3)
  UINT8    StorageRtd3Support;                      ///< Offset 522     Storage Runtime D3 Support
  UINT8    Ps2MouseEnable;                          ///< Offset 523     Ps2 Mouse Enable
  UINT8    ScanMatrixEnable;                        ///< Offset 524     Scan Matrix Keyboard Support Enable
  UINT16   SSH0;                                    ///< Offset 525     SSCN-HIGH for I2C0
  UINT16   SSL0;                                    ///< Offset 527     SSCN-LOW  for I2C0
  UINT16   SSD0;                                    ///< Offset 529     SSCN-HOLD for I2C0
  UINT16   FMH0;                                    ///< Offset 531     FMCN-HIGH for I2C0
  UINT16   FML0;                                    ///< Offset 533     FMCN-LOW  for I2C0
  UINT16   FMD0;                                    ///< Offset 535     FMCN-HOLD for I2C0
  UINT16   FPH0;                                    ///< Offset 537     FPCN-HIGH for I2C0
  UINT16   FPL0;                                    ///< Offset 539     FPCN-LOW  for I2C0
  UINT16   FPD0;                                    ///< Offset 541     FPCN-HOLD for I2C0
  UINT16   HSH0;                                    ///< Offset 543     HSCN-HIGH for I2C0
  UINT16   HSL0;                                    ///< Offset 545     HSCN-LOW  for I2C0
  UINT16   HSD0;                                    ///< Offset 547     HSCN-HOLD for I2C0
  UINT8    Reserved1[24];                           ///< Offset 549:572
  UINT16   SSH1;                                    ///< Offset 573     SSCN-HIGH for I2C1
  UINT16   SSL1;                                    ///< Offset 575     SSCN-LOW  for I2C1
  UINT16   SSD1;                                    ///< Offset 577     SSCN-HOLD for I2C1
  UINT16   FMH1;                                    ///< Offset 579     FMCN-HIGH for I2C1
  UINT16   FML1;                                    ///< Offset 581     FMCN-LOW  for I2C1
  UINT16   FMD1;                                    ///< Offset 583     FMCN-HOLD for I2C1
  UINT16   FPH1;                                    ///< Offset 585     FPCN-HIGH for I2C1
  UINT16   FPL1;                                    ///< Offset 587     FPCN-LOW  for I2C1
  UINT16   FPD1;                                    ///< Offset 589     FPCN-HOLD for I2C1
  UINT16   HSH1;                                    ///< Offset 591     HSCN-HIGH for I2C1
  UINT16   HSL1;                                    ///< Offset 593     HSCN-LOW  for I2C1
  UINT16   HSD1;                                    ///< Offset 595     HSCN-HOLD for I2C1
  UINT8    Reserved2[1];                            ///< Offset 597:597
  UINT16   SSH2;                                    ///< Offset 598     SSCN-HIGH for I2C2
  UINT16   SSL2;                                    ///< Offset 600     SSCN-LOW  for I2C2
  UINT16   SSD2;                                    ///< Offset 602     SSCN-HOLD for I2C2
  UINT16   FMH2;                                    ///< Offset 604     FMCN-HIGH for I2C2
  UINT16   FML2;                                    ///< Offset 606     FMCN-LOW  for I2C2
  UINT16   FMD2;                                    ///< Offset 608     FMCN-HOLD for I2C2
  UINT16   FPH2;                                    ///< Offset 610     FPCN-HIGH for I2C2
  UINT16   FPL2;                                    ///< Offset 612     FPCN-LOW  for I2C2
  UINT16   FPD2;                                    ///< Offset 614     FPCN-HOLD for I2C2
  UINT16   HSH2;                                    ///< Offset 616     HSCN-HIGH for I2C2
  UINT16   HSL2;                                    ///< Offset 618     HSCN-LOW  for I2C2
  UINT16   HSD2;                                    ///< Offset 620     HSCN-HOLD for I2C2
  UINT8    Reserved3[1];                            ///< Offset 622:622
  UINT16   SSH3;                                    ///< Offset 623     SSCN-HIGH for I2C3
  UINT16   SSL3;                                    ///< Offset 625     SSCN-LOW  for I2C3
  UINT16   SSD3;                                    ///< Offset 627     SSCN-HOLD for I2C3
  UINT16   FMH3;                                    ///< Offset 629     FMCN-HIGH for I2C3
  UINT16   FML3;                                    ///< Offset 631     FMCN-LOW  for I2C3
  UINT16   FMD3;                                    ///< Offset 633     FMCN-HOLD for I2C3
  UINT16   FPH3;                                    ///< Offset 635     FPCN-HIGH for I2C3
  UINT16   FPL3;                                    ///< Offset 637     FPCN-LOW  for I2C3
  UINT16   FPD3;                                    ///< Offset 639     FPCN-HOLD for I2C3
  UINT16   HSH3;                                    ///< Offset 641     HSCN-HIGH for I2C3
  UINT16   HSL3;                                    ///< Offset 643     HSCN-LOW  for I2C3
  UINT16   HSD3;                                    ///< Offset 645     HSCN-HOLD for I2C3
  UINT8    Reserved4[1];                            ///< Offset 647:647
  UINT16   SSH4;                                    ///< Offset 648     SSCN-HIGH for I2C4
  UINT16   SSL4;                                    ///< Offset 650     SSCN-LOW  for I2C4
  UINT16   SSD4;                                    ///< Offset 652     SSCN-HOLD for I2C4
  UINT16   FMH4;                                    ///< Offset 654     FMCN-HIGH for I2C4
  UINT16   FML4;                                    ///< Offset 656     FMCN-LOW  for I2C4
  UINT16   FMD4;                                    ///< Offset 658     FMCN-HOLD for I2C4
  UINT16   FPH4;                                    ///< Offset 660     FPCN-HIGH for I2C4
  UINT16   FPL4;                                    ///< Offset 662     FPCN-LOW  for I2C4
  UINT16   FPD4;                                    ///< Offset 664     FPCN-HOLD for I2C4
  UINT16   HSH4;                                    ///< Offset 666     HSCN-HIGH for I2C4
  UINT16   HSL4;                                    ///< Offset 668     HSCN-LOW  for I2C4
  UINT16   HSD4;                                    ///< Offset 670     HSCN-HOLD for I2C4
  UINT8    Reserved5[1];                            ///< Offset 672:672
  UINT16   SSH5;                                    ///< Offset 673     SSCN-HIGH for I2C5
  UINT16   SSL5;                                    ///< Offset 675     SSCN-LOW  for I2C5
  UINT16   SSD5;                                    ///< Offset 677     SSCN-HOLD for I2C5
  UINT16   FMH5;                                    ///< Offset 679     FMCN-HIGH for I2C5
  UINT16   FML5;                                    ///< Offset 681     FMCN-LOW  for I2C5
  UINT16   FMD5;                                    ///< Offset 683     FMCN-HOLD for I2C5
  UINT16   FPH5;                                    ///< Offset 685     FPCN-HIGH for I2C5
  UINT16   FPL5;                                    ///< Offset 687     FPCN-LOW  for I2C5
  UINT16   FPD5;                                    ///< Offset 689     FPCN-HOLD for I2C5
  UINT16   HSH5;                                    ///< Offset 691     HSCN-HIGH for I2C5
  UINT16   HSL5;                                    ///< Offset 693     HSCN-LOW  for I2C5
  UINT16   HSD5;                                    ///< Offset 695     HSCN-HOLD for I2C5
  UINT8    Reserved6[1];                            ///< Offset 697:697
  UINT16   M0C0;                                    ///< Offset 698     M0D3 for I2C0
  UINT16   M1C0;                                    ///< Offset 700     M1D3 for I2C0
  UINT16   M0C1;                                    ///< Offset 702     M0D3 for I2C1
  UINT16   M1C1;                                    ///< Offset 704     M1D3 for I2C1
  UINT16   M0C2;                                    ///< Offset 706     M0D3 for I2C2
  UINT16   M1C2;                                    ///< Offset 708     M1D3 for I2C2
  UINT16   M0C3;                                    ///< Offset 710     M0D3 for I2C3
  UINT16   M1C3;                                    ///< Offset 712     M1D3 for I2C3
  UINT16   M0C4;                                    ///< Offset 714     M0D3 for I2C4
  UINT16   M1C4;                                    ///< Offset 716     M1D3 for I2C4
  UINT16   M0C5;                                    ///< Offset 718     M0D3 for I2C5
  UINT16   M1C5;                                    ///< Offset 720     M1D3 for I2C5
  UINT16   M0C6;                                    ///< Offset 722     M0D3 for SPI0
  UINT16   M1C6;                                    ///< Offset 724     M1D3 for SPI0
  UINT16   M0C7;                                    ///< Offset 726     M0D3 for SPI1
  UINT16   M1C7;                                    ///< Offset 728     M1D3 for SPI1
  UINT16   M0C8;                                    ///< Offset 730     M0D3 for SPI2
  UINT16   M1C8;                                    ///< Offset 732     M1D3 for SPI2
  UINT8    Reserved7[1];                            ///< Offset 734:734
  UINT16   M0C9;                                    ///< Offset 735     M0D3 for UART0
  UINT16   M1C9;                                    ///< Offset 737     M1D3 for UART0
  UINT16   M0CA;                                    ///< Offset 739     M0D3 for UART1
  UINT16   M1CA;                                    ///< Offset 741     M1D3 for UART1
  UINT16   M0CB;                                    ///< Offset 743     M0D3 for UART2
  UINT16   M1CB;                                    ///< Offset 745     M1D3 for UART2
  UINT8    Reserved8[1];                            ///< Offset 747:747
  //
  // Driver Mode
  //
  UINT32   GpioIrqRoute;                            ///< Offset 748     GPIO IRQ
  UINT8    DriverModeTouchPanel;                    ///< Offset 752     PIRQS 34,50(GPIO)
  UINT8    DriverModeTouchPad;                      ///< Offset 753     PIRQX 39,55(GPIO)
  UINT8    DriverModeSensorHub;                     ///< Offset 754     PIRQM 28,14(GPIO)
  UINT8    SensorStandby;                           ///< Offset 755     Sensor Standby mode
  UINT8    PL1LimitCS;                              ///< Offset 756     set PL1 limit when entering CS
  UINT16   PL1LimitCSValue;                         ///< Offset 757     PL1 limit value
  UINT8    EnableWwanTempSensorDevice;              ///< Offset 759     EnableWwanTempSensorDevice
  UINT8    EnableCpuVrTempSensorDevice;             ///< Offset 760     EnableCpuVrTempSensorDevice
  UINT8    EnableSsdTempSensorDevice;               ///< Offset 761     EnableSsdTempSensorDevice
  UINT8    EnableInletFanTempSensorDevice;          ///< Offset 762     EnableInletFanTempSensorDevice
  UINT8    ActiveThermalTripPointInletFan;          ///< Offset 763     ActiveThermalTripPointInletFan
  UINT8    PassiveThermalTripPointInletFan;         ///< Offset 764     PassiveThermalTripPointInletFan
  UINT8    CriticalThermalTripPointInletFan;        ///< Offset 765     CriticalThermalTripPointInletFan
  UINT8    HotThermalTripPointInletFan;             ///< Offset 766     HotThermalTripPointInletFan
  UINT8    UsbSensorHub;                            ///< Offset 767     Sensor Hub Type - (0)None, (1)USB, (2)I2C Intel, (3)I2C STM
  UINT8    BCV4;                                    ///< Offset 768     Broadcom's Bluetooth adapter's revision
  UINT8    WTV0;                                    ///< Offset 769     I2C0/WITT devices version
  UINT8    WTV1;                                    ///< Offset 770     I2C1/WITT devices version
  UINT8    AtmelPanelFwUpdate;                      ///< Offset 771     Atmel panel FW update Enable/Disable
  UINT8    Reserved9[6];                            ///< Offset 772:777
  UINT64   LowPowerS0IdleConstraint;                ///< Offset 778     PEP Constraints
  // Bit[1:0] - Storage (0:None, 1:Storage Controller, 2:Raid)
  // Bit[2]   - En/Dis UART0
  // Bit[3]   - En/Dis UART1
  // Bit[4]   - En/Dis I2C0
  // Bit[5]   - En/Dis I2C1
  // Bit[6]   - En/Dis XHCI
  // Bit[8:7] - HD Audio (includes ADSP) (0: No Constraints or 1: D0/F1 or 3:D3)
  // Bit[9]   - En/Dis Gfx
  // Bit[10]  - En/Dis CPU
  // Bit[11]  - En/Dis EMMC
  // Bit[12]  - En/Dis SDXC
  // Bit[13]  - En/Dis I2C2
  // Bit[14]  - En/Dis I2C3
  // Bit[15]  - En/Dis I2C4
  // Bit[16]  - En/Dis I2C5
  // Bit[17]  - En/Dis UART2
  // Bit[18]  - En/Dis SPI0
  // Bit[19]  - En/Dis SPI1
  // Bit[20]  - En/Dis SPI2
  // Bit[21]  - En/Dis IPU0
  // Bit[22]  - En/Dis CSME
  // Bit[23]  - En/Dis LAN(GBE)
  // Bit[24]  - Reserved
  // Bit[25]  - En/Dis THC0
  // Bit[26]  - En/Dis THC1
  // Bit[27]  - Reserved
  // Bit[28]  - En/Dis I2C6
  // Bit[29]  - En/Dis TCSS IPs
  // Bit[30]  - En/Dis GNA
  // Bit[31]  - En/Dis VMD0
  // Bit[32]  - En/Dis HECI3
  // Bit[34:33]  - PCIe Storage RP(0: No Constraints or 1: D0/F1 or 3:D3)
  // Bit[36:35] - Pcie Lan (0:No Constraint or 1:D0/F1 or 3:D3)
  // Bit[38:37] - Pcie Wlan (0:No Constraint or 1:D0/F1 or 3:D3)
  // Bit[40:39] - Pcie Gfx (0:No Constraint or 1:D0/F1 or 3:D3)
  // Bit[42:41] - Pcie Other (0:No Constraint or 1:D0/F1 or 3:D3)
  // Bit[43]  - En/Dis DG on x8 PEG port (PEG1)
  // Bit[44]  - En/Dis VPU0
  // BIT[45]  - En/Dis UFS0
  // BIT[46]  - En/Dis UFS1
  UINT16   VRStaggeringDelay;                       ///< Offset 786     VR Staggering delay
  UINT8    TenSecondPowerButtonEnable;              ///< Offset 788     10sec Power button support
  // Bit0: 10 sec P-button Enable/Disable
  // Bit1: Internal Flag
  // Bit2: Rotation Lock flag, 0:unlock, 1:lock
  // Bit3: Slate/Laptop Mode Flag, 0: Slate, 1: Laptop
  // Bit4: Undock / Dock Flag, 0: Undock, 1: Dock
  // Bit5: VBDL Flag. 0: VBDL is not called, 1: VBDL is called, Virtual Button Driver is loaded.
  // Bit7-6: Reserved for future use.
  //
  // Generation Id(Tock/Tick)
  //
  UINT8    GenerationId;                            ///< Offset 789     Generation Id(0=Shark bay, 1=Crescent Bay)
  UINT8    Reserved10[10];                          ///< Offset 790:799
  UINT32   AuxPowerLimit;                           ///< Offset 800     Maximum aux power available for PCIe root ports
  UINT8    EnumerateSataPortConstraints;            ///< Offset 804     Set to indicate to PEP that constraints at SATA ports should be enumerated
  UINT8    PowerSharingManagerEnable;               ///< Offset 805     PowerSharingManagerEnable
  UINT8    PsmSplcDomainType1;                      ///< Offset 806     PsmSplcDomainType1
  UINT32   PsmSplcPowerLimit1;                      ///< Offset 807     PsmSplcPowerLimit1
  UINT32   PsmSplcTimeWindow1;                      ///< Offset 811     PsmSplcTimeWindow1
  UINT8    PsmDplcDomainType1;                      ///< Offset 815     PsmDplcDomainType1
  UINT8    PsmDplcDomainPreference1;                ///< Offset 816     PsmDplcDomainPreference1
  UINT16   PsmDplcPowerLimitIndex1;                 ///< Offset 817     PsmDplcPowerLimitIndex1
  UINT16   PsmDplcDefaultPowerLimit1;               ///< Offset 819     PsmDplcDefaultPowerLimit1
  UINT32   PsmDplcDefaultTimeWindow1;               ///< Offset 821     PsmDplcDefaultTimeWindow1
  UINT16   PsmDplcMinimumPowerLimit1;               ///< Offset 825     PsmDplcMinimumPowerLimit1
  UINT16   PsmDplcMaximumPowerLimit1;               ///< Offset 827     PsmDplcMaximumPowerLimit1
  UINT16   PsmDplcMaximumTimeWindow1;               ///< Offset 829     PsmDplcMaximumTimeWindow1
  UINT8    WifiEnable;                              ///< Offset 831     WifiEnable
  UINT8    WifiDomainType1;                         ///< Offset 832     WifiDomainType1
  UINT16   WifiPowerLimit1;                         ///< Offset 833     WifiPowerLimit1
  UINT32   WifiTimeWindow1;                         ///< Offset 835     WifiTimeWindow1
  UINT8    TRxDelay0;                               ///< Offset 839     TRxDelay0
  UINT8    TRxCableLength0;                         ///< Offset 840     TRxCableLength0
  UINT8    TRxDelay1;                               ///< Offset 841     TRxDelay1
  UINT8    TRxCableLength1;                         ///< Offset 842     TRxCableLength1
  UINT8    WrddDomainType1;                         ///< Offset 843     WrddDomainType1
  UINT16   WrddCountryIdentifier1;                  ///< Offset 844     WrddCountryIdentifier1
  UINT8    Reserved11[173];                         ///< Offset 846:1018
  //
  // Intel Serial(R) IO Sensor Device Selection
  //
  UINT8    SDS0;                                    ///< Offset 1019    SerialIo Devices for controller0
  UINT8    SDS1;                                    ///< Offset 1020    SerialIo Devices for controller1
  UINT8    SDS2;                                    ///< Offset 1021    SerialIo Devices for controller2
  UINT8    SDS3;                                    ///< Offset 1022    SerialIo Devices for controller3
  UINT8    SDS4;                                    ///< Offset 1023    SerialIo Devices for controller4
  UINT8    SDS5;                                    ///< Offset 1024    SerialIo Devices for controller5
  UINT8    SDS6;                                    ///< Offset 1025    SerialIo Devices for controller6
  UINT8    SDS7;                                    ///< Offset 1026    SerialIo Devices for controller7
  UINT8    SDS8;                                    ///< Offset 1027    SerialIo Devices for controller8
  UINT8    SDS9;                                    ///< Offset 1028    SerialIo Devices for controller9
  UINT8    SDSA;                                    ///< Offset 1029    SerialIo Devices for controller10
  UINT8    TPLT;                                    ///< Offset 1030    I2C SerialIo Devices Type of TouchPanel
  UINT8    TPLM;                                    ///< Offset 1031    I2C SerialIo Devices Interrupt Mode for TouchPanel
  UINT8    TPLB;                                    ///< Offset 1032    I2C Custom TouchPanel's BUS Address
  UINT16   TPLH;                                    ///< Offset 1033    I2C Custom TouchPanel's HID Address
  UINT8    TPLS;                                    ///< Offset 1035    I2C Custom TouchPanel's BUS Speed
  UINT8    TPDT;                                    ///< Offset 1036    I2C SerialIo Devices Type of TouchPad
  UINT8    TPDM;                                    ///< Offset 1037    I2C SerialIo Devices Interrupt Mode for TouchPad
  UINT8    TPDB;                                    ///< Offset 1038    I2C Custom TouchPad's BUS Address
  UINT16   TPDH;                                    ///< Offset 1039    I2C Custom TouchPad's HID Address
  UINT8    TPDS;                                    ///< Offset 1041    I2C Custom TouchPad's BUS Speed
  UINT8    FPTT;                                    ///< Offset 1042    SPI SerialIo Devices Type of FingerPrint
  UINT8    FPTM;                                    ///< Offset 1043    SPI SerialIo Devices Interrupt Mode for FingerPrint
  UINT8    WTVX;                                    ///< Offset 1044    WITT test devices' version
  UINT8    WITX;                                    ///< Offset 1045    WITT test devices' connection point
  UINT8    GPTD;                                    ///< Offset 1046    GPIO test devices
  UINT16   GDBT;                                    ///< Offset 1047    GPIO test devices' debounce value,
  UINT8    UTKX;                                    ///< Offset 1049    UTK test devices' connection point
  UINT8    SPTD;                                    ///< Offset 1050    SerialIo additional test devices
  //
  // Test Devices for Serial IOs
  //
  UINT8    SIOI;                                    ///< Offset 1051    WITT SIO I2C test device enable/disable
  UINT8    SIOC;                                    ///< Offset 1052    WITT SIO I3C test device enable/disable
  UINT8    SIOS;                                    ///< Offset 1053    WITT SIO SPI test device enable/disable
  UINT8    SII0;                                    ///< Offset 1054    SIO I2C0 test devices' connection point
  UINT8    SII1;                                    ///< Offset 1055    SIO I2C1 test devices' connection point
  UINT8    SII2;                                    ///< Offset 1056    SIO I2C2 test devices' connection point
  UINT8    SII3;                                    ///< Offset 1057    SIO I2C3 test devices' connection point
  UINT8    SII4;                                    ///< Offset 1058    SIO I2C4 test devices' connection point
  UINT8    SII5;                                    ///< Offset 1059    SIO I2C5 test devices' connection point
  UINT8    SIC0;                                    ///< Offset 1060    SIO I3C0 test devices' connection point
  UINT8    SIC1;                                    ///< Offset 1061    SIO I3C1 test devices' connection point
  UINT8    SIS0;                                    ///< Offset 1062    SIO SPI0 test devices' connection point
  UINT8    SIS1;                                    ///< Offset 1063    SIO SPI1 test devices' connection point
  UINT8    SIS2;                                    ///< Offset 1064    SIO SPI2 test devices' connection point
  UINT8    Reserved12[1];                           ///< Offset 1065:1065
  UINT32   TableLoadBuffer;                         ///< Offset 1066    Buffer for runtime ACPI Table loading
  UINT8    SDM0;                                    ///< Offset 1070    interrupt mode for controller0 devices
  UINT8    SDM1;                                    ///< Offset 1071    interrupt mode for controller1 devices
  UINT8    SDM2;                                    ///< Offset 1072    interrupt mode for controller2 devices
  UINT8    SDM3;                                    ///< Offset 1073    interrupt mode for controller3 devices
  UINT8    SDM4;                                    ///< Offset 1074    interrupt mode for controller4 devices
  UINT8    SDM5;                                    ///< Offset 1075    interrupt mode for controller5 devices
  UINT8    SDM6;                                    ///< Offset 1076    interrupt mode for controller6 devices
  UINT8    SDM7;                                    ///< Offset 1077    interrupt mode for controller7 devices
  UINT8    SDM8;                                    ///< Offset 1078    interrupt mode for controller8 devices
  UINT8    SDM9;                                    ///< Offset 1079    interrupt mode for controller9 devices
  UINT8    SDMA;                                    ///< Offset 1080    interrupt mode for controller10 devices
  UINT8    SDMB;                                    ///< Offset 1081    interrupt mode for controller11 devices
  UINT8    Reserved13[1];                           ///< Offset 1082:1082
  UINT8    USTP;                                    ///< Offset 1083    use SerialIo timing parameters
  UINT8    Reserved14[41];                          ///< Offset 1084:1124
  UINT32   FingerPrintSleepGpio;                    ///< Offset 1125    Gpio for fingerprint sleep
  UINT32   FingerPrintIrqGpio;                      ///< Offset 1129    Gpio for fingerprint irq
  UINT32   WwanModemBaseBandResetGpio;              ///< Offset 1133    Gpio for modem reset
  UINT32   BtModuleRfKillGpio;                      ///< Offset 1137    Gpio for Bluetooth RfKill
  UINT32   BtModuleIrqGpio;                         ///< Offset 1141    Gpio for Bluetooth interrupt
  UINT32   TouchpadIrqGpio;                         ///< Offset 1145    Gpio for touchPaD Interrupt
  //
  // MipiCam specific
  //
  UINT8    MipiCamControlLogic0;                    ///< Offset 1149
  UINT8    MipiCamControlLogic1;                    ///< Offset 1150
  UINT8    MipiCamControlLogic2;                    ///< Offset 1151
  UINT8    MipiCamControlLogic3;                    ///< Offset 1152
  UINT8    MipiCamControlLogic4;                    ///< Offset 1153
  UINT8    MipiCamControlLogic5;                    ///< Offset 1154
  UINT8    MipiCamLink0Enabled;                     ///< Offset 1155
  UINT8    MipiCamLink1Enabled;                     ///< Offset 1156
  UINT8    MipiCamLink2Enabled;                     ///< Offset 1157
  UINT8    MipiCamLink3Enabled;                     ///< Offset 1158
  UINT8    MipiCamLink4Enabled;                     ///< Offset 1159
  UINT8    MipiCamLink5Enabled;                     ///< Offset 1160
  UINT8    MipiCamLink0DD_LanesClkDivision;         ///< Offset 1161    LanesClockDivision
  UINT8    MipiCamLink1DD_LanesClkDivision;         ///< Offset 1162    LanesClockDivision
  UINT8    MipiCamLink2DD_LanesClkDivision;         ///< Offset 1163    LanesClockDivision
  UINT8    MipiCamLink3DD_LanesClkDivision;         ///< Offset 1164    LanesClockDivision
  UINT8    MipiCamLink4DD_LanesClkDivision;         ///< Offset 1165    LanesClockDivision
  UINT8    MipiCamLink5DD_LanesClkDivision;         ///< Offset 1166    LanesClockDivision
  // Control Logic 0 options
  UINT8    MipiCamCtrlLogic0_Version;               ///< Offset 1167    Version of CLDB structure
  UINT8    MipiCamCtrlLogic0_Type;                  ///< Offset 1168    Type
  UINT8    MipiCamCtrlLogic0_CrdVersion;            ///< Offset 1169    Version of CRD
  UINT32   MipiCamCtrlLogic0_InputClock;            ///< Offset 1170    Input Clock
  UINT8    MipiCamCtrlLogic0_GpioPinsEnabled;       ///< Offset 1174    Number of GPIO Pins enabled
  UINT8    MipiCamCtrlLogic0_I2cBus;                ///< Offset 1175    I2C Serial Bus Number
  UINT16   MipiCamCtrlLogic0_I2cAddress;            ///< Offset 1176    I2C Address
  UINT8    MipiCamCtrlLogic0_GpioGroupPadNumber[6]; ///< Offset 1178    GPIO Group Pad Number
  UINT8    MipiCamCtrlLogic0_GpioGroupNumber[6];    ///< Offset 1184    GPIO Group Number
  UINT8    MipiCamCtrlLogic0_GpioFunction[6];       ///< Offset 1190    GPIO Function
  UINT8    MipiCamCtrlLogic0_GpioActiveValue[6];    ///< Offset 1196    GPIO Active Value
  UINT8    MipiCamCtrlLogic0_GpioInitialValue[6];   ///< Offset 1202    GPIO Initial Value
  UINT8    MipiCamCtrlLogic0_Pld;                   ///< Offset 1208    Camera Position
  UINT8    MipiCamCtrlLogic0_Wled1FlashMaxCurrent;  ///< Offset 1209    WLED1 Flash Max Current
  UINT8    MipiCamCtrlLogic0_Wled1TorchMaxCurrent;  ///< Offset 1210    WLED1 Torch Max Current
  UINT8    MipiCamCtrlLogic0_Wled2FlashMaxCurrent;  ///< Offset 1211    WLED2 Flash Max Current
  UINT8    MipiCamCtrlLogic0_Wled2TorchMaxCurrent;  ///< Offset 1212    WLED2 Torch Max Current
  UINT8    MipiCamCtrlLogic0_SubPlatformId;         ///< Offset 1213    Sub Platform Id
  UINT8    MipiCamCtrlLogic0_Wled1Type;             ///< Offset 1214    WLED1 Type
  UINT8    MipiCamCtrlLogic0_Wled2Type;             ///< Offset 1215    WLED2 Type
  UINT8    MipiCamCtrlLogic0_PchClockSource;        ///< Offset 1216    PCH Clock source
  // Control Logic 1 options
  UINT8    MipiCamCtrlLogic1_Version;               ///< Offset 1217    Version of CLDB structure
  UINT8    MipiCamCtrlLogic1_Type;                  ///< Offset 1218    Type
  UINT8    MipiCamCtrlLogic1_CrdVersion;            ///< Offset 1219    Version of CRD
  UINT32   MipiCamCtrlLogic1_InputClock;            ///< Offset 1220    Input Clock
  UINT8    MipiCamCtrlLogic1_GpioPinsEnabled;       ///< Offset 1224    Number of GPIO Pins enabled
  UINT8    MipiCamCtrlLogic1_I2cBus;                ///< Offset 1225    I2C Serial Bus Number
  UINT16   MipiCamCtrlLogic1_I2cAddress;            ///< Offset 1226    I2C Address
  UINT8    MipiCamCtrlLogic1_GpioGroupPadNumber[6]; ///< Offset 1228    GPIO Group Pad Number
  UINT8    MipiCamCtrlLogic1_GpioGroupNumber[6];    ///< Offset 1234    GPIO Group Number
  UINT8    MipiCamCtrlLogic1_GpioFunction[6];       ///< Offset 1240    GPIO Function
  UINT8    MipiCamCtrlLogic1_GpioActiveValue[6];    ///< Offset 1246    GPIO Active Value
  UINT8    MipiCamCtrlLogic1_GpioInitialValue[6];   ///< Offset 1252    GPIO Initial Value
  UINT8    MipiCamCtrlLogic1_Pld;                   ///< Offset 1258    Camera Position
  UINT8    MipiCamCtrlLogic1_Wled1FlashMaxCurrent;  ///< Offset 1259    WLED1 Flash Max Current
  UINT8    MipiCamCtrlLogic1_Wled1TorchMaxCurrent;  ///< Offset 1260    WLED1 Torch Max Current
  UINT8    MipiCamCtrlLogic1_Wled2FlashMaxCurrent;  ///< Offset 1261    WLED2 Flash Max Current
  UINT8    MipiCamCtrlLogic1_Wled2TorchMaxCurrent;  ///< Offset 1262    WLED2 Torch Max Current
  UINT8    MipiCamCtrlLogic1_SubPlatformId;         ///< Offset 1263    Sub Platform Id
  UINT8    MipiCamCtrlLogic1_Wled1Type;             ///< Offset 1264    WLED1 Type
  UINT8    MipiCamCtrlLogic1_Wled2Type;             ///< Offset 1265    WLED2 Type
  UINT8    MipiCamCtrlLogic1_PchClockSource;        ///< Offset 1266    PCH Clock source
  // Control Logic 2 options
  UINT8    MipiCamCtrlLogic2_Version;               ///< Offset 1267    Version of CLDB structure
  UINT8    MipiCamCtrlLogic2_Type;                  ///< Offset 1268    Type
  UINT8    MipiCamCtrlLogic2_CrdVersion;            ///< Offset 1269    Version of CRD
  UINT32   MipiCamCtrlLogic2_InputClock;            ///< Offset 1270    Input Clock
  UINT8    MipiCamCtrlLogic2_GpioPinsEnabled;       ///< Offset 1274    Number of GPIO Pins enabled
  UINT8    MipiCamCtrlLogic2_I2cBus;                ///< Offset 1275    I2C Serial Bus Number
  UINT16   MipiCamCtrlLogic2_I2cAddress;            ///< Offset 1276    I2C Address
  UINT8    MipiCamCtrlLogic2_GpioGroupPadNumber[6]; ///< Offset 1278    GPIO Group Pad Number
  UINT8    MipiCamCtrlLogic2_GpioGroupNumber[6];    ///< Offset 1284    GPIO Group Number
  UINT8    MipiCamCtrlLogic2_GpioFunction[6];       ///< Offset 1290    GPIO Function
  UINT8    MipiCamCtrlLogic2_GpioActiveValue[6];    ///< Offset 1296    GPIO Active Value
  UINT8    MipiCamCtrlLogic2_GpioInitialValue[6];   ///< Offset 1302    GPIO Initial Value
  UINT8    MipiCamCtrlLogic2_Pld;                   ///< Offset 1308    Camera Position
  UINT8    MipiCamCtrlLogic2_Wled1FlashMaxCurrent;  ///< Offset 1309    WLED1 Flash Max Current
  UINT8    MipiCamCtrlLogic2_Wled1TorchMaxCurrent;  ///< Offset 1310    WLED1 Torch Max Current
  UINT8    MipiCamCtrlLogic2_Wled2FlashMaxCurrent;  ///< Offset 1311    WLED2 Flash Max Current
  UINT8    MipiCamCtrlLogic2_Wled2TorchMaxCurrent;  ///< Offset 1312    WLED2 Torch Max Current
  UINT8    MipiCamCtrlLogic2_SubPlatformId;         ///< Offset 1313    Sub Platform Id
  UINT8    MipiCamCtrlLogic2_Wled1Type;             ///< Offset 1314    WLED1 Type
  UINT8    MipiCamCtrlLogic2_Wled2Type;             ///< Offset 1315    WLED2 Type
  UINT8    MipiCamCtrlLogic2_PchClockSource;        ///< Offset 1316    PCH Clock source
  // Control Logic 3 options
  UINT8    MipiCamCtrlLogic3_Version;               ///< Offset 1317    Version of CLDB structure
  UINT8    MipiCamCtrlLogic3_Type;                  ///< Offset 1318    Type
  UINT8    MipiCamCtrlLogic3_CrdVersion;            ///< Offset 1319    Version of CRD
  UINT32   MipiCamCtrlLogic3_InputClock;            ///< Offset 1320    Input Clock
  UINT8    MipiCamCtrlLogic3_GpioPinsEnabled;       ///< Offset 1324    Number of GPIO Pins enabled
  UINT8    MipiCamCtrlLogic3_I2cBus;                ///< Offset 1325    I2C Serial Bus Number
  UINT16   MipiCamCtrlLogic3_I2cAddress;            ///< Offset 1326    I2C Address
  UINT8    MipiCamCtrlLogic3_GpioGroupPadNumber[6]; ///< Offset 1328    GPIO Group Pad Number
  UINT8    MipiCamCtrlLogic3_GpioGroupNumber[6];    ///< Offset 1334    GPIO Group Number
  UINT8    MipiCamCtrlLogic3_GpioFunction[6];       ///< Offset 1340    GPIO Function
  UINT8    MipiCamCtrlLogic3_GpioActiveValue[6];    ///< Offset 1346    GPIO Active Value
  UINT8    MipiCamCtrlLogic3_GpioInitialValue[6];   ///< Offset 1352    GPIO Initial Value
  UINT8    MipiCamCtrlLogic3_Pld;                   ///< Offset 1358    Camera Position
  UINT8    MipiCamCtrlLogic3_Wled1FlashMaxCurrent;  ///< Offset 1359    WLED1 Flash Max Current
  UINT8    MipiCamCtrlLogic3_Wled1TorchMaxCurrent;  ///< Offset 1360    WLED1 Torch Max Current
  UINT8    MipiCamCtrlLogic3_Wled2FlashMaxCurrent;  ///< Offset 1361    WLED2 Flash Max Current
  UINT8    MipiCamCtrlLogic3_Wled2TorchMaxCurrent;  ///< Offset 1362    WLED2 Torch Max Current
  UINT8    MipiCamCtrlLogic3_SubPlatformId;         ///< Offset 1363    Sub Platform Id
  UINT8    MipiCamCtrlLogic3_Wled1Type;             ///< Offset 1364    WLED1 Type
  UINT8    MipiCamCtrlLogic3_Wled2Type;             ///< Offset 1365    WLED2 Type
  UINT8    MipiCamCtrlLogic3_PchClockSource;        ///< Offset 1366    PCH Clock source
  // Control Logic 4 options
  UINT8    MipiCamCtrlLogic4_Version;               ///< Offset 1367    Version of CLDB structure
  UINT8    MipiCamCtrlLogic4_Type;                  ///< Offset 1368    Type
  UINT8    MipiCamCtrlLogic4_CrdVersion;            ///< Offset 1369    Version of CRD
  UINT32   MipiCamCtrlLogic4_InputClock;            ///< Offset 1370    Input Clock
  UINT8    MipiCamCtrlLogic4_GpioPinsEnabled;       ///< Offset 1374    Number of GPIO Pins enabled
  UINT8    MipiCamCtrlLogic4_I2cBus;                ///< Offset 1375    I2C Serial Bus Number
  UINT16   MipiCamCtrlLogic4_I2cAddress;            ///< Offset 1376    I2C Address
  UINT8    MipiCamCtrlLogic4_GpioGroupPadNumber[6]; ///< Offset 1378    GPIO Group Pad Number
  UINT8    MipiCamCtrlLogic4_GpioGroupNumber[6];    ///< Offset 1384    GPIO Group Number
  UINT8    MipiCamCtrlLogic4_GpioFunction[6];       ///< Offset 1390    GPIO Function
  UINT8    MipiCamCtrlLogic4_GpioActiveValue[6];    ///< Offset 1396    GPIO Active Value
  UINT8    MipiCamCtrlLogic4_GpioInitialValue[6];   ///< Offset 1402    GPIO Initial Value
  UINT8    MipiCamCtrlLogic4_Pld;                   ///< Offset 1408    Camera Position
  UINT8    MipiCamCtrlLogic4_Wled1FlashMaxCurrent;  ///< Offset 1409    WLED1 Flash Max Current
  UINT8    MipiCamCtrlLogic4_Wled1TorchMaxCurrent;  ///< Offset 1410    WLED1 Torch Max Current
  UINT8    MipiCamCtrlLogic4_Wled2FlashMaxCurrent;  ///< Offset 1411    WLED2 Flash Max Current
  UINT8    MipiCamCtrlLogic4_Wled2TorchMaxCurrent;  ///< Offset 1412    WLED2 Torch Max Current
  UINT8    MipiCamCtrlLogic4_SubPlatformId;         ///< Offset 1413    Sub Platform Id
  UINT8    MipiCamCtrlLogic4_Wled1Type;             ///< Offset 1414    WLED1 Type
  UINT8    MipiCamCtrlLogic4_Wled2Type;             ///< Offset 1415    WLED2 Type
  UINT8    MipiCamCtrlLogic4_PchClockSource;        ///< Offset 1416    PCH Clock source
  // Control Logic 5 options
  UINT8    MipiCamCtrlLogic5_Version;               ///< Offset 1417    Version of CLDB structure
  UINT8    MipiCamCtrlLogic5_Type;                  ///< Offset 1418    Type
  UINT8    MipiCamCtrlLogic5_CrdVersion;            ///< Offset 1419    Version of CRD
  UINT32   MipiCamCtrlLogic5_InputClock;            ///< Offset 1420    Input Clock
  UINT8    MipiCamCtrlLogic5_GpioPinsEnabled;       ///< Offset 1424    Number of GPIO Pins enabled
  UINT8    MipiCamCtrlLogic5_I2cBus;                ///< Offset 1425    I2C Serial Bus Number
  UINT16   MipiCamCtrlLogic5_I2cAddress;            ///< Offset 1426    I2C Address
  UINT8    MipiCamCtrlLogic5_GpioGroupPadNumber[6]; ///< Offset 1428    GPIO Group Pad Number
  UINT8    MipiCamCtrlLogic5_GpioGroupNumber[6];    ///< Offset 1434    GPIO Group Number
  UINT8    MipiCamCtrlLogic5_GpioFunction[6];       ///< Offset 1440    GPIO Function
  UINT8    MipiCamCtrlLogic5_GpioActiveValue[6];    ///< Offset 1446    GPIO Active Value
  UINT8    MipiCamCtrlLogic5_GpioInitialValue[6];   ///< Offset 1452    GPIO Initial Value
  UINT8    MipiCamCtrlLogic5_Pld;                   ///< Offset 1458    Camera Position
  UINT8    MipiCamCtrlLogic5_Wled1FlashMaxCurrent;  ///< Offset 1459    WLED1 Flash Max Current
  UINT8    MipiCamCtrlLogic5_Wled1TorchMaxCurrent;  ///< Offset 1460    WLED1 Torch Max Current
  UINT8    MipiCamCtrlLogic5_Wled2FlashMaxCurrent;  ///< Offset 1461    WLED2 Flash Max Current
  UINT8    MipiCamCtrlLogic5_Wled2TorchMaxCurrent;  ///< Offset 1462    WLED2 Torch Max Current
  UINT8    MipiCamCtrlLogic5_SubPlatformId;         ///< Offset 1463    Sub Platform Id
  UINT8    MipiCamCtrlLogic5_Wled1Type;             ///< Offset 1464    WLED1 Type
  UINT8    MipiCamCtrlLogic5_Wled2Type;             ///< Offset 1465    WLED2 Type
  UINT8    MipiCamCtrlLogic5_PchClockSource;        ///< Offset 1466    PCH Clock source
  // Mipi Cam Link0 options
  UINT8    MipiCamLink0SensorModel;                 ///< Offset 1467    Sensor Model
  UINT8    MipiCamLink0UserHid[9];                  ///< Offset 1468    User defined HID ASCII character 0
                                                    ///< Offset 1476    User defined HID ASCII character 8
  UINT8    MipiCamLink0Pld;                         ///< Offset 1477    Camera Position
  UINT8    MipiCamLink0ModuleName[16];              ///< Offset 1478    Camera Module Name ASCII character 0
                                                    ///< Offset 1493    Camera Module Name ASCII character 15
  UINT8    MipiCamLink0I2cDevicesEnabled;           ///< Offset 1494    Number of I2C devices
  UINT8    MipiCamLink0I2cBus;                      ///< Offset 1495    I2C Serial Bus number
  UINT16   MipiCamLink0I2cAddrDev[12];              ///< Offset 1496    Address of I2C Device0 on Link0
                                                    ///< Offset 1498    Address of I2C Device1 on Link0
                                                    ///< Offset 1500    Address of I2C Device2 on Link0
                                                    ///< Offset 1502    Address of I2C Device3 on Link0
                                                    ///< Offset 1504    Address of I2C Device4 on Link0
                                                    ///< Offset 1506    Address of I2C Device5 on Link0
                                                    ///< Offset 1508    Address of I2C Device6 on Link0
                                                    ///< Offset 1510    Address of I2C Device7 on Link0
                                                    ///< Offset 1512    Address of I2C Device8 on Link0
                                                    ///< Offset 1514    Address of I2C Device9 on Link0
                                                    ///< Offset 1516    Address of I2C Device10 on Link0
                                                    ///< Offset 1518    Address of I2C Device11 on Link0
  UINT8    MipiCamLink0I2cDeviceType[12];           ///< Offset 1520    Type of I2C Device0 on Link0
                                                    ///< Offset 1521    Type of I2C Device1 on Link0
                                                    ///< Offset 1522    Type of I2C Device2 on Link0
                                                    ///< Offset 1523    Type of I2C Device3 on Link0
                                                    ///< Offset 1524    Type of I2C Device4 on Link0
                                                    ///< Offset 1525    Type of I2C Device5 on Link0
                                                    ///< Offset 1526    Type of I2C Device6 on Link0
                                                    ///< Offset 1527    Type of I2C Device7 on Link0
                                                    ///< Offset 1528    Type of I2C Device8 on Link0
                                                    ///< Offset 1529    Type of I2C Device9 on Link0
                                                    ///< Offset 1530    Type of I2C Device10 on Link0
                                                    ///< Offset 1531    Type of I2C Device11 on Link0
  UINT8    MipiCamLink0DD_Version;                  ///< Offset 1532    Version of SSDB structure
  UINT8    MipiCamLink0DD_CrdVersion;               ///< Offset 1533    Version of CRD
  UINT8    MipiCamLink0DD_LinkUsed;                 ///< Offset 1534    CSI2 Link used
  UINT8    MipiCamLink0DD_LaneUsed;                 ///< Offset 1535    MIPI-CSI2 Data Lane
  UINT8    MipiCamLink0DD_EepromType;               ///< Offset 1536    EEPROM Type
  UINT8    MipiCamLink0DD_VcmType;                  ///< Offset 1537    VCM Type
  UINT8    MipiCamLink0DD_FlashSupport;             ///< Offset 1538    Flash Support
  UINT8    MipiCamLink0DD_PrivacyLed;               ///< Offset 1539    Privacy LED
  UINT8    MipiCamLink0DD_Degree;                   ///< Offset 1540    Degree
  UINT32   MipiCamLink0DD_Mclk;                     ///< Offset 1541    MCLK
  UINT8    MipiCamLink0DD_ControlLogic;             ///< Offset 1545    Control Logic
  UINT8    MipiCamLink0DD_PmicPosition;             ///< Offset 1546    PMIC Position
  UINT8    MipiCamLink0DD_VoltageRail;              ///< Offset 1547    Voltage Rail
  UINT8    MipiCamLink0_FlashDriverSelection;       ///< Offset 1548    Flash Driver Selection
  // Mipi Cam Link1 options
  UINT8    MipiCamLink1SensorModel;                 ///< Offset 1549    Sensor Model
  UINT8    MipiCamLink1UserHid[9];                  ///< Offset 1550    User defined HID ASCII character 0
                                                    ///< Offset 1558    User defined HID ASCII character 8
  UINT8    MipiCamLink1Pld;                         ///< Offset 1559    Camera Position
  UINT8    MipiCamLink1ModuleName[16];              ///< Offset 1560    Camera Module Name ASCII character 0
                                                    ///< Offset 1575    Camera Module Name ASCII character 15
  UINT8    MipiCamLink1I2cDevicesEnabled;           ///< Offset 1576    Number of I2C devices
  UINT8    MipiCamLink1I2cBus;                      ///< Offset 1577    I2C Serial Bus number
  UINT16   MipiCamLink1I2cAddrDev[12];              ///< Offset 1578    Address of I2C Device0 on Link1
                                                    ///< Offset 1580    Address of I2C Device1 on Link1
                                                    ///< Offset 1582    Address of I2C Device2 on Link1
                                                    ///< Offset 1584    Address of I2C Device3 on Link1
                                                    ///< Offset 1586    Address of I2C Device4 on Link1
                                                    ///< Offset 1588    Address of I2C Device5 on Link1
                                                    ///< Offset 1590    Address of I2C Device6 on Link1
                                                    ///< Offset 1592    Address of I2C Device7 on Link1
                                                    ///< Offset 1594    Address of I2C Device8 on Link1
                                                    ///< Offset 1596    Address of I2C Device9 on Link1
                                                    ///< Offset 1598    Address of I2C Device10 on Link1
                                                    ///< Offset 1600    Address of I2C Device11 on Link1
  UINT8    MipiCamLink1I2cDeviceType[12];           ///< Offset 1602    Type of I2C Device0 on Link1
                                                    ///< Offset 1603    Type of I2C Device1 on Link1
                                                    ///< Offset 1604    Type of I2C Device2 on Link1
                                                    ///< Offset 1605    Type of I2C Device3 on Link1
                                                    ///< Offset 1606    Type of I2C Device4 on Link1
                                                    ///< Offset 1607    Type of I2C Device5 on Link1
                                                    ///< Offset 1608    Type of I2C Device6 on Link1
                                                    ///< Offset 1609    Type of I2C Device7 on Link1
                                                    ///< Offset 1610    Type of I2C Device8 on Link1
                                                    ///< Offset 1611    Type of I2C Device9 on Link1
                                                    ///< Offset 1612    Type of I2C Device10 on Link1
                                                    ///< Offset 1613    Type of I2C Device11 on Link1
  UINT8    MipiCamLink1DD_Version;                  ///< Offset 1614    Version of SSDB structure
  UINT8    MipiCamLink1DD_CrdVersion;               ///< Offset 1615    Version of CRD
  UINT8    MipiCamLink1DD_LinkUsed;                 ///< Offset 1616    CSI2 Link used
  UINT8    MipiCamLink1DD_LaneUsed;                 ///< Offset 1617    MIPI-CSI2 Data Lane
  UINT8    MipiCamLink1DD_EepromType;               ///< Offset 1618    EEPROM Type
  UINT8    MipiCamLink1DD_VcmType;                  ///< Offset 1619    VCM Type
  UINT8    MipiCamLink1DD_FlashSupport;             ///< Offset 1620    Flash Support
  UINT8    MipiCamLink1DD_PrivacyLed;               ///< Offset 1621    Privacy LED
  UINT8    MipiCamLink1DD_Degree;                   ///< Offset 1622    Degree
  UINT32   MipiCamLink1DD_Mclk;                     ///< Offset 1623    MCLK
  UINT8    MipiCamLink1DD_ControlLogic;             ///< Offset 1627    Control Logic
  UINT8    MipiCamLink1DD_PmicPosition;             ///< Offset 1628    PMIC Position
  UINT8    MipiCamLink1DD_VoltageRail;              ///< Offset 1629    Voltage Rail
  UINT8    MipiCamLink1_FlashDriverSelection;       ///< Offset 1630    Flash Driver Selection
  // Mipi Cam Link2 options
  UINT8    MipiCamLink2SensorModel;                 ///< Offset 1631    Sensor Model
  UINT8    MipiCamLink2UserHid[9];                  ///< Offset 1632    User defined HID ASCII character 0
                                                    ///< Offset 1640    User defined HID ASCII character 8
  UINT8    MipiCamLink2Pld;                         ///< Offset 1641    Camera Position
  UINT8    MipiCamLink2ModuleName[16];              ///< Offset 1642    Camera Module Name ASCII character 0
                                                    ///< Offset 1657    Camera Module Name ASCII character 15
  UINT8    MipiCamLink2I2cDevicesEnabled;           ///< Offset 1658    Number of I2C devices
  UINT8    MipiCamLink2I2cBus;                      ///< Offset 1659    I2C Serial Bus number
  UINT16   MipiCamLink2I2cAddrDev[12];              ///< Offset 1660    Address of I2C Device0 on Link2
                                                    ///< Offset 1662    Address of I2C Device1 on Link2
                                                    ///< Offset 1664    Address of I2C Device2 on Link2
                                                    ///< Offset 1666    Address of I2C Device3 on Link2
                                                    ///< Offset 1668    Address of I2C Device4 on Link2
                                                    ///< Offset 1670    Address of I2C Device5 on Link2
                                                    ///< Offset 1672    Address of I2C Device6 on Link2
                                                    ///< Offset 1674    Address of I2C Device7 on Link2
                                                    ///< Offset 1676    Address of I2C Device8 on Link2
                                                    ///< Offset 1678    Address of I2C Device9 on Link2
                                                    ///< Offset 1680    Address of I2C Device10 on Link2
                                                    ///< Offset 1682    Address of I2C Device11 on Link2
  UINT8    MipiCamLink2I2cDeviceType[12];           ///< Offset 1684    Type of I2C Device0 on Link2
                                                    ///< Offset 1685    Type of I2C Device1 on Link2
                                                    ///< Offset 1686    Type of I2C Device2 on Link2
                                                    ///< Offset 1687    Type of I2C Device3 on Link2
                                                    ///< Offset 1688    Type of I2C Device4 on Link2
                                                    ///< Offset 1689    Type of I2C Device5 on Link2
                                                    ///< Offset 1690    Type of I2C Device6 on Link2
                                                    ///< Offset 1691    Type of I2C Device7 on Link2
                                                    ///< Offset 1692    Type of I2C Device8 on Link2
                                                    ///< Offset 1693    Type of I2C Device9 on Link2
                                                    ///< Offset 1694    Type of I2C Device10 on Link2
                                                    ///< Offset 1695    Type of I2C Device11 on Link2
  UINT8    MipiCamLink2DD_Version;                  ///< Offset 1696    Version of SSDB structure
  UINT8    MipiCamLink2DD_CrdVersion;               ///< Offset 1697    Version of CRD
  UINT8    MipiCamLink2DD_LinkUsed;                 ///< Offset 1698    CSI2 Link used
  UINT8    MipiCamLink2DD_LaneUsed;                 ///< Offset 1699    MIPI-CSI2 Data Lane
  UINT8    MipiCamLink2DD_EepromType;               ///< Offset 1700    EEPROM Type
  UINT8    MipiCamLink2DD_VcmType;                  ///< Offset 1701    VCM Type
  UINT8    MipiCamLink2DD_FlashSupport;             ///< Offset 1702    Flash Support
  UINT8    MipiCamLink2DD_PrivacyLed;               ///< Offset 1703    Privacy LED
  UINT8    MipiCamLink2DD_Degree;                   ///< Offset 1704    Degree
  UINT32   MipiCamLink2DD_Mclk;                     ///< Offset 1705    MCLK
  UINT8    MipiCamLink2DD_ControlLogic;             ///< Offset 1709    Control Logic
  UINT8    MipiCamLink2DD_PmicPosition;             ///< Offset 1710    PMIC Position
  UINT8    MipiCamLink2DD_VoltageRail;              ///< Offset 1711    Voltage Rail
  UINT8    MipiCamLink2_FlashDriverSelection;       ///< Offset 1712    Flash Driver Selection
  // Mipi Cam Link3 options
  UINT8    MipiCamLink3SensorModel;                 ///< Offset 1713    Sensor Model
  UINT8    MipiCamLink3UserHid[9];                  ///< Offset 1714    User defined HID ASCII character 0
                                                    ///< Offset 1722    User defined HID ASCII character 8
  UINT8    MipiCamLink3Pld;                         ///< Offset 1723    Camera Position
  UINT8    MipiCamLink3ModuleName[16];              ///< Offset 1724    Camera Module Name ASCII character 0
                                                    ///< Offset 1739    Camera Module Name ASCII character 15
  UINT8    MipiCamLink3I2cDevicesEnabled;           ///< Offset 1740    Number of I2C devices
  UINT8    MipiCamLink3I2cBus;                      ///< Offset 1741    I2C Serial Bus number
  UINT16   MipiCamLink3I2cAddrDev[12];              ///< Offset 1742    Address of I2C Device0 on Link3
                                                    ///< Offset 1744    Address of I2C Device1 on Link3
                                                    ///< Offset 1746    Address of I2C Device2 on Link3
                                                    ///< Offset 1748    Address of I2C Device3 on Link3
                                                    ///< Offset 1750    Address of I2C Device4 on Link3
                                                    ///< Offset 1752    Address of I2C Device5 on Link3
                                                    ///< Offset 1754    Address of I2C Device6 on Link3
                                                    ///< Offset 1756    Address of I2C Device7 on Link3
                                                    ///< Offset 1758    Address of I2C Device8 on Link3
                                                    ///< Offset 1760    Address of I2C Device9 on Link3
                                                    ///< Offset 1762    Address of I2C Device10 on Link3
                                                    ///< Offset 1764    Address of I2C Device11 on Link3
  UINT8    MipiCamLink3I2cDeviceType[12];           ///< Offset 1766    Type of I2C Device0 on Link3
                                                    ///< Offset 1767    Type of I2C Device1 on Link3
                                                    ///< Offset 1768    Type of I2C Device2 on Link3
                                                    ///< Offset 1769    Type of I2C Device3 on Link3
                                                    ///< Offset 1770    Type of I2C Device4 on Link3
                                                    ///< Offset 1771    Type of I2C Device5 on Link3
                                                    ///< Offset 1772    Type of I2C Device6 on Link3
                                                    ///< Offset 1773    Type of I2C Device7 on Link3
                                                    ///< Offset 1774    Type of I2C Device8 on Link3
                                                    ///< Offset 1775    Type of I2C Device9 on Link3
                                                    ///< Offset 1776    Type of I2C Device10 on Link3
                                                    ///< Offset 1777    Type of I2C Device11 on Link3
  UINT8    MipiCamLink3DD_Version;                  ///< Offset 1778    Version of SSDB structure
  UINT8    MipiCamLink3DD_CrdVersion;               ///< Offset 1779    Version of CRD
  UINT8    MipiCamLink3DD_LinkUsed;                 ///< Offset 1780    CSI2 Link used
  UINT8    MipiCamLink3DD_LaneUsed;                 ///< Offset 1781    MIPI-CSI2 Data Lane
  UINT8    MipiCamLink3DD_EepromType;               ///< Offset 1782    EEPROM Type
  UINT8    MipiCamLink3DD_VcmType;                  ///< Offset 1783    VCM Type
  UINT8    MipiCamLink3DD_FlashSupport;             ///< Offset 1784    Flash Support
  UINT8    MipiCamLink3DD_PrivacyLed;               ///< Offset 1785    Privacy LED
  UINT8    MipiCamLink3DD_Degree;                   ///< Offset 1786    Degree
  UINT32   MipiCamLink3DD_Mclk;                     ///< Offset 1787    MCLK
  UINT8    MipiCamLink3DD_ControlLogic;             ///< Offset 1791    Control Logic
  UINT8    MipiCamLink3DD_PmicPosition;             ///< Offset 1792    PMIC Position
  UINT8    MipiCamLink3DD_VoltageRail;              ///< Offset 1793    Voltage Rail
  UINT8    MipiCamLink3_FlashDriverSelection;       ///< Offset 1794    Flash Driver Selection
  // Mipi Cam Link4 options
  UINT8    MipiCamLink4SensorModel;                 ///< Offset 1795    Sensor Model
  UINT8    MipiCamLink4UserHid[9];                  ///< Offset 1796    User defined HID ASCII character 0
                                                    ///< Offset 1804    User defined HID ASCII character 8
  UINT8    MipiCamLink4Pld;                         ///< Offset 1805    Camera Position
  UINT8    MipiCamLink4ModuleName[16];              ///< Offset 1806    Camera Module Name ASCII character 0
                                                    ///< Offset 1821    Camera Module Name ASCII character 15
  UINT8    MipiCamLink4I2cDevicesEnabled;           ///< Offset 1822    Number of I2C devices
  UINT8    MipiCamLink4I2cBus;                      ///< Offset 1823    I2C Serial Bus number
  UINT16   MipiCamLink4I2cAddrDev[12];              ///< Offset 1824    Address of I2C Device0 on Link4
                                                    ///< Offset 1826    Address of I2C Device1 on Link4
                                                    ///< Offset 1828    Address of I2C Device2 on Link4
                                                    ///< Offset 1830    Address of I2C Device3 on Link4
                                                    ///< Offset 1832    Address of I2C Device4 on Link4
                                                    ///< Offset 1834    Address of I2C Device5 on Link4
                                                    ///< Offset 1836    Address of I2C Device6 on Link4
                                                    ///< Offset 1838    Address of I2C Device7 on Link4
                                                    ///< Offset 1840    Address of I2C Device8 on Link4
                                                    ///< Offset 1842    Address of I2C Device9 on Link4
                                                    ///< Offset 1844    Address of I2C Device10 on Link4
                                                    ///< Offset 1846    Address of I2C Device11 on Link4
  UINT8    MipiCamLink4I2cDeviceType[12];           ///< Offset 1848    Type of I2C Device0 on Link4
                                                    ///< Offset 1849    Type of I2C Device1 on Link4
                                                    ///< Offset 1850    Type of I2C Device2 on Link4
                                                    ///< Offset 1851    Type of I2C Device3 on Link4
                                                    ///< Offset 1852    Type of I2C Device4 on Link4
                                                    ///< Offset 1853    Type of I2C Device5 on Link4
                                                    ///< Offset 1854    Type of I2C Device6 on Link4
                                                    ///< Offset 1855    Type of I2C Device7 on Link4
                                                    ///< Offset 1856    Type of I2C Device8 on Link4
                                                    ///< Offset 1857    Type of I2C Device9 on Link4
                                                    ///< Offset 1858    Type of I2C Device10 on Link4
                                                    ///< Offset 1859    Type of I2C Device11 on Link4
  UINT8    MipiCamLink4DD_Version;                  ///< Offset 1860    Version of SSDB structure
  UINT8    MipiCamLink4DD_CrdVersion;               ///< Offset 1861    Version of CRD
  UINT8    MipiCamLink4DD_LinkUsed;                 ///< Offset 1862    CSI2 Link used
  UINT8    MipiCamLink4DD_LaneUsed;                 ///< Offset 1863    MIPI-CSI2 Data Lane
  UINT8    MipiCamLink4DD_EepromType;               ///< Offset 1864    EEPROM Type
  UINT8    MipiCamLink4DD_VcmType;                  ///< Offset 1865    VCM Type
  UINT8    MipiCamLink4DD_FlashSupport;             ///< Offset 1866    Flash Support
  UINT8    MipiCamLink4DD_PrivacyLed;               ///< Offset 1867    Privacy LED
  UINT8    MipiCamLink4DD_Degree;                   ///< Offset 1868    Degree
  UINT32   MipiCamLink4DD_Mclk;                     ///< Offset 1869    MCLK
  UINT8    MipiCamLink4DD_ControlLogic;             ///< Offset 1873    Control Logic
  UINT8    MipiCamLink4DD_PmicPosition;             ///< Offset 1874    PMIC Position
  UINT8    MipiCamLink4DD_VoltageRail;              ///< Offset 1875    Voltage Rail
  UINT8    MipiCamLink4_FlashDriverSelection;       ///< Offset 1876    Flash Driver Selection
  // Mipi Cam Link5 options
  UINT8    MipiCamLink5SensorModel;                 ///< Offset 1877    Sensor Model
  UINT8    MipiCamLink5UserHid[9];                  ///< Offset 1878    User defined HID ASCII character 0
                                                    ///< Offset 1886    User defined HID ASCII character 8
  UINT8    MipiCamLink5Pld;                         ///< Offset 1887    Camera Position
  UINT8    MipiCamLink5ModuleName[16];              ///< Offset 1888    Camera Module Name ASCII character 0
                                                    ///< Offset 1903    Camera Module Name ASCII character 15
  UINT8    MipiCamLink5I2cDevicesEnabled;           ///< Offset 1904    Number of I2C devices
  UINT8    MipiCamLink5I2cBus;                      ///< Offset 1905    I2C Serial Bus number
  UINT16   MipiCamLink5I2cAddrDev[12];              ///< Offset 1906    Address of I2C Device0 on Link5
                                                    ///< Offset 1908    Address of I2C Device1 on Link5
                                                    ///< Offset 1910    Address of I2C Device2 on Link5
                                                    ///< Offset 1912    Address of I2C Device3 on Link5
                                                    ///< Offset 1914    Address of I2C Device4 on Link5
                                                    ///< Offset 1916    Address of I2C Device5 on Link5
                                                    ///< Offset 1918    Address of I2C Device6 on Link5
                                                    ///< Offset 1920    Address of I2C Device7 on Link5
                                                    ///< Offset 1922    Address of I2C Device8 on Link5
                                                    ///< Offset 1924    Address of I2C Device9 on Link5
                                                    ///< Offset 1926    Address of I2C Device10 on Link5
                                                    ///< Offset 1928    Address of I2C Device11 on Link5
  UINT8    MipiCamLink5I2cDeviceType[12];           ///< Offset 1930    Type of I2C Device0 on Link5
                                                    ///< Offset 1931    Type of I2C Device1 on Link5
                                                    ///< Offset 1932    Type of I2C Device2 on Link5
                                                    ///< Offset 1933    Type of I2C Device3 on Link5
                                                    ///< Offset 1934    Type of I2C Device4 on Link5
                                                    ///< Offset 1935    Type of I2C Device5 on Link5
                                                    ///< Offset 1936    Type of I2C Device6 on Link5
                                                    ///< Offset 1937    Type of I2C Device7 on Link5
                                                    ///< Offset 1938    Type of I2C Device8 on Link5
                                                    ///< Offset 1939    Type of I2C Device9 on Link5
                                                    ///< Offset 1940    Type of I2C Device10 on Link5
                                                    ///< Offset 1941    Type of I2C Device11 on Link5
  UINT8    MipiCamLink5DD_Version;                  ///< Offset 1942    Version of SSDB structure
  UINT8    MipiCamLink5DD_CrdVersion;               ///< Offset 1943    Version of CRD
  UINT8    MipiCamLink5DD_LinkUsed;                 ///< Offset 1944    CSI2 Link used
  UINT8    MipiCamLink5DD_LaneUsed;                 ///< Offset 1945    MIPI-CSI2 Data Lane
  UINT8    MipiCamLink5DD_EepromType;               ///< Offset 1946    EEPROM Type
  UINT8    MipiCamLink5DD_VcmType;                  ///< Offset 1947    VCM Type
  UINT8    MipiCamLink5DD_FlashSupport;             ///< Offset 1948    Flash Support
  UINT8    MipiCamLink5DD_PrivacyLed;               ///< Offset 1949    Privacy LED
  UINT8    MipiCamLink5DD_Degree;                   ///< Offset 1950    Degree
  UINT32   MipiCamLink5DD_Mclk;                     ///< Offset 1951    MCLK
  UINT8    MipiCamLink5DD_ControlLogic;             ///< Offset 1955    Control Logic
  UINT8    MipiCamLink5DD_PmicPosition;             ///< Offset 1956    PMIC Position
  UINT8    MipiCamLink5DD_VoltageRail;              ///< Offset 1957    Voltage Rail
  UINT8    MipiCamLink5_FlashDriverSelection;       ///< Offset 1958    Flash Driver Selection
  UINT8    MipiCamFlash0Model;                      ///< Offset 1959    Flash Driver Model
  UINT8    MipiCamFlash0Mode;                       ///< Offset 1960    Flash Mode Selection
  UINT8    MipiCamFlash0ModuleName[16];             ///< Offset 1961    Flash Module Name ASCII character 0
                                                    ///< Offset 1976    Flash Module Name ASCII character 15
  UINT8    MipiCamFlash0I2cBus;                     ///< Offset 1977    I2C Bus Number
  UINT16   MipiCamFlash0I2cAddrDev;                 ///< Offset 1978    I2C Target Device Address
  UINT8    MipiCamFlash0GpioGroupPadNumber;         ///< Offset 1980    GPIO Group Pad Number
  UINT16   MipiCamFlash0GpioGroupNumber;            ///< Offset 1981    GPIO Group Number
  UINT8    MipiCamFlash0GpioActiveValue;            ///< Offset 1983    GPIO Active Value
  UINT8    MipiCamFlash0GpioInitialValue;           ///< Offset 1984    GPIO Initial Value
  UINT8    MipiCamFlash0OperatingMode;              ///< Offset 1985    Flash Driver Operating Mode
  UINT8    MipiCamFlash1Model;                      ///< Offset 1986    Flash Driver Model
  UINT8    MipiCamFlash1Mode;                       ///< Offset 1987    Flash Mode Selection
  UINT8    MipiCamFlash1ModuleName[16];             ///< Offset 1988    Flash Module Name ASCII character 0
                                                    ///< Offset 2003    Flash Module Name ASCII character 15
  UINT8    MipiCamFlash1I2cBus;                     ///< Offset 2004    I2C Bus Number
  UINT16   MipiCamFlash1I2cAddrDev;                 ///< Offset 2005    I2C Target Device Address
  UINT8    MipiCamFlash1GpioGroupPadNumber;         ///< Offset 2007    GPIO Group Pad Number
  UINT16   MipiCamFlash1GpioGroupNumber;            ///< Offset 2008    GPIO Group Number
  UINT8    MipiCamFlash1GpioActiveValue;            ///< Offset 2010    GPIO Active Value
  UINT8    MipiCamFlash1GpioInitialValue;           ///< Offset 2011    GPIO Initial Value
  UINT8    MipiCamFlash1OperatingMode;              ///< Offset 2012    Flash Driver Operating Mode
  UINT8    MipiCamFlash2Model;                      ///< Offset 2013    Flash Driver Model
  UINT8    MipiCamFlash2Mode;                       ///< Offset 2014    Flash Mode Selection
  UINT8    MipiCamFlash2ModuleName[16];             ///< Offset 2015    Flash Module Name ASCII character 0
                                                    ///< Offset 2030    Flash Module Name ASCII character 15
  UINT8    MipiCamFlash2I2cBus;                     ///< Offset 2031    I2C Bus Number
  UINT16   MipiCamFlash2I2cAddrDev;                 ///< Offset 2032    I2C Target Device Address
  UINT8    MipiCamFlash2GpioGroupPadNumber;         ///< Offset 2034    GPIO Group Pad Number
  UINT16   MipiCamFlash2GpioGroupNumber;            ///< Offset 2035    GPIO Group Number
  UINT8    MipiCamFlash2GpioActiveValue;            ///< Offset 2037    GPIO Active Value
  UINT8    MipiCamFlash2GpioInitialValue;           ///< Offset 2038    GPIO Initial Value
  UINT8    MipiCamFlash2OperatingMode;              ///< Offset 2039    Flash Driver Operating Mode
  UINT8    MipiCamFlash3Model;                      ///< Offset 2040    Flash Driver Model
  UINT8    MipiCamFlash3Mode;                       ///< Offset 2041    Flash Mode Selection
  UINT8    MipiCamFlash3ModuleName[16];             ///< Offset 2042    Flash Module Name ASCII character 0
                                                    ///< Offset 2057    Flash Module Name ASCII character 15
  UINT8    MipiCamFlash3I2cBus;                     ///< Offset 2058    I2C Bus Number
  UINT16   MipiCamFlash3I2cAddrDev;                 ///< Offset 2059    I2C Target Device Address
  UINT8    MipiCamFlash3GpioGroupPadNumber;         ///< Offset 2061    GPIO Group Pad Number
  UINT16   MipiCamFlash3GpioGroupNumber;            ///< Offset 2062    GPIO Group Number
  UINT8    MipiCamFlash3GpioActiveValue;            ///< Offset 2064    GPIO Active Value
  UINT8    MipiCamFlash3GpioInitialValue;           ///< Offset 2065    GPIO Initial Value
  UINT8    MipiCamFlash3OperatingMode;              ///< Offset 2066    Flash Driver Operating Mode
  UINT8    MipiCamFlash4Model;                      ///< Offset 2067    Flash Driver Model
  UINT8    MipiCamFlash4Mode;                       ///< Offset 2068    Flash Mode Selection
  UINT8    MipiCamFlash4ModuleName[16];             ///< Offset 2069    Flash Module Name ASCII character 0
                                                    ///< Offset 2084    Flash Module Name ASCII character 15
  UINT8    MipiCamFlash4I2cBus;                     ///< Offset 2085    I2C Bus Number
  UINT16   MipiCamFlash4I2cAddrDev;                 ///< Offset 2086    I2C Target Device Address
  UINT8    MipiCamFlash4GpioGroupPadNumber;         ///< Offset 2088    GPIO Group Pad Number
  UINT16   MipiCamFlash4GpioGroupNumber;            ///< Offset 2089    GPIO Group Number
  UINT8    MipiCamFlash4GpioActiveValue;            ///< Offset 2091    GPIO Active Value
  UINT8    MipiCamFlash4GpioInitialValue;           ///< Offset 2092    GPIO Initial Value
  UINT8    MipiCamFlash4OperatingMode;              ///< Offset 2093    Flash Driver Operating Mode
  UINT8    MipiCamFlash5Model;                      ///< Offset 2094    Flash Driver Model
  UINT8    MipiCamFlash5Mode;                       ///< Offset 2095    Flash Mode Selection
  UINT8    MipiCamFlash5ModuleName[16];             ///< Offset 2096    Flash Module Name ASCII character 0
                                                    ///< Offset 2111    Flash Module Name ASCII character 15
  UINT8    MipiCamFlash5I2cBus;                     ///< Offset 2112    I2C Bus Number
  UINT16   MipiCamFlash5I2cAddrDev;                 ///< Offset 2113    I2C Target Device Address
  UINT8    MipiCamFlash5GpioGroupPadNumber;         ///< Offset 2115    GPIO Group Pad Number
  UINT16   MipiCamFlash5GpioGroupNumber;            ///< Offset 2116    GPIO Group Number
  UINT8    MipiCamFlash5GpioActiveValue;            ///< Offset 2118    GPIO Active Value
  UINT8    MipiCamFlash5GpioInitialValue;           ///< Offset 2119    GPIO Initial Value
  UINT8    MipiCamFlash5OperatingMode;              ///< Offset 2120    Flash Driver Operating Mode
  UINT8    Reserved15[4];                           ///< Offset 2121:2124
  UINT8    PciDelayOptimizationEcr;                 ///< Offset 2125
  UINT8    I2SC;                                    ///< Offset 2126    HD Audio I2S Codec Selection
  UINT32   I2SI;                                    ///< Offset 2127    HD Audio I2S Codec Interrupt Pin
  UINT8    I2SB;                                    ///< Offset 2131    HD Audio I2S Codec Connection to I2C bus controller instance (I2C[0-5])
  UINT32   UsbTypeCOpBaseAddr;                      ///< Offset 2132    USB Type C Opregion base address
  UINT8    PdPsOnEnable;                            ///< Offset 2136    PD PS_ON enable
  UINT8    Reserved16[5];                           ///< Offset 2137:2141
  UINT8    WirelessCharging;                        ///< Offset 2142    WirelessCharging
  UINT8    Reserved17[6];                           ///< Offset 2143:2148
  UINT32   HdaDspPpModuleMask;                      ///< Offset 2149    HD-Audio DSP Post-Processing Module Mask
  UINT64   HdaDspPpModCustomGuid1Low;               ///< Offset 2153    HDA PP module custom GUID 1 - first 64bit  [0-63]
  UINT64   HdaDspPpModCustomGuid1High;              ///< Offset 2161    HDA PP module custom GUID 1 - second 64bit [64-127]
  UINT64   HdaDspPpModCustomGuid2Low;               ///< Offset 2169    HDA PP module custom GUID 2 - first 64bit  [0-63]
  UINT64   HdaDspPpModCustomGuid2High;              ///< Offset 2177    HDA PP module custom GUID 2 - second 64bit [64-127]
  UINT64   HdaDspPpModCustomGuid3Low;               ///< Offset 2185    HDA PP module custom GUID 3 - first 64bit  [0-63]
  UINT64   HdaDspPpModCustomGuid3High;              ///< Offset 2193    HDA PP module custom GUID 3 - second 64bit [64-127]
  UINT8    HidEventFilterEnable;                    ///< Offset 2201    HID Event Filter Driver enable
  UINT8    XdciFnEnable;                            ///< Offset 2202    XDCI Enable/Disable status
  UINT8    WrdsWiFiSarEnable;                       ///< Offset 2203    WrdsWiFiSarEnable
  UINT8    WrdsWiFiSarTxPowerSet1Limit1;            ///< Offset 2204    WrdsWiFiSarTxPowerSet1Limit1
  UINT8    WrdsWiFiSarTxPowerSet1Limit2;            ///< Offset 2205    WrdsWiFiSarTxPowerSet1Limit2
  UINT8    WrdsWiFiSarTxPowerSet1Limit3;            ///< Offset 2206    WrdsWiFiSarTxPowerSet1Limit3
  UINT8    WrdsWiFiSarTxPowerSet1Limit4;            ///< Offset 2207    WrdsWiFiSarTxPowerSet1Limit4
  UINT8    WrdsWiFiSarTxPowerSet1Limit5;            ///< Offset 2208    WrdsWiFiSarTxPowerSet1Limit5
  UINT8    WrdsWiFiSarTxPowerSet1Limit6;            ///< Offset 2209    WrdsWiFiSarTxPowerSet1Limit6
  UINT8    WrdsWiFiSarTxPowerSet1Limit7;            ///< Offset 2210    WrdsWiFiSarTxPowerSet1Limit7
  UINT8    WrdsWiFiSarTxPowerSet1Limit8;            ///< Offset 2211    WrdsWiFiSarTxPowerSet1Limit8
  UINT8    WrdsWiFiSarTxPowerSet1Limit9;            ///< Offset 2212    WrdsWiFiSarTxPowerSet1Limit9
  UINT8    WrdsWiFiSarTxPowerSet1Limit10;           ///< Offset 2213    WrdsWiFiSarTxPowerSet1Limit10
  UINT8    EnableVoltageMargining;                  ///< Offset 2214    Enable Voltage Margining
  UINT16   DStateHSPort;                            ///< Offset 2215    D-State for xHCI HS port(BIT0:USB HS Port0 ~ BIT15:USB HS Port15)
  UINT16   DStateSSPort;                            ///< Offset 2217    D-State for xHCI SS port(BIT0:USB SS Port0 ~ BIT15:USB SS Port15)
  UINT8    DStateSataPort;                          ///< Offset 2219    D-State for SATA port(BIT0:SATA Port0 ~ BIT7:SATA Port7)
  UINT8    EwrdWiFiDynamicSarEnable;                ///< Offset 2220    EwrdWiFiDynamicSarEnable
  UINT8    EwrdWiFiDynamicSarRangeSets;             ///< Offset 2221    EwrdWiFiDynamicSarRangeSets
  UINT8    EwrdWiFiSarTxPowerSet2Limit1;            ///< Offset 2222    EwrdWiFiSarTxPowerSet2Limit1
  UINT8    EwrdWiFiSarTxPowerSet2Limit2;            ///< Offset 2223    EwrdWiFiSarTxPowerSet2Limit2
  UINT8    EwrdWiFiSarTxPowerSet2Limit3;            ///< Offset 2224    EwrdWiFiSarTxPowerSet2Limit3
  UINT8    EwrdWiFiSarTxPowerSet2Limit4;            ///< Offset 2225    EwrdWiFiSarTxPowerSet2Limit4
  UINT8    EwrdWiFiSarTxPowerSet2Limit5;            ///< Offset 2226    EwrdWiFiSarTxPowerSet2Limit5
  UINT8    EwrdWiFiSarTxPowerSet2Limit6;            ///< Offset 2227    EwrdWiFiSarTxPowerSet2Limit6
  UINT8    EwrdWiFiSarTxPowerSet2Limit7;            ///< Offset 2228    EwrdWiFiSarTxPowerSet2Limit7
  UINT8    EwrdWiFiSarTxPowerSet2Limit8;            ///< Offset 2229    EwrdWiFiSarTxPowerSet2Limit8
  UINT8    EwrdWiFiSarTxPowerSet2Limit9;            ///< Offset 2230    EwrdWiFiSarTxPowerSet2Limit9
  UINT8    EwrdWiFiSarTxPowerSet2Limit10;           ///< Offset 2231    EwrdWiFiSarTxPowerSet2Limit10
  UINT8    EwrdWiFiSarTxPowerSet3Limit1;            ///< Offset 2232    EwrdWiFiSarTxPowerSet3Limit1
  UINT8    EwrdWiFiSarTxPowerSet3Limit2;            ///< Offset 2233    EwrdWiFiSarTxPowerSet3Limit2
  UINT8    EwrdWiFiSarTxPowerSet3Limit3;            ///< Offset 2234    EwrdWiFiSarTxPowerSet3Limit3
  UINT8    EwrdWiFiSarTxPowerSet3Limit4;            ///< Offset 2235    EwrdWiFiSarTxPowerSet3Limit4
  UINT8    EwrdWiFiSarTxPowerSet3Limit5;            ///< Offset 2236    EwrdWiFiSarTxPowerSet3Limit5
  UINT8    EwrdWiFiSarTxPowerSet3Limit6;            ///< Offset 2237    EwrdWiFiSarTxPowerSet3Limit6
  UINT8    EwrdWiFiSarTxPowerSet3Limit7;            ///< Offset 2238    EwrdWiFiSarTxPowerSet3Limit7
  UINT8    EwrdWiFiSarTxPowerSet3Limit8;            ///< Offset 2239    EwrdWiFiSarTxPowerSet3Limit8
  UINT8    EwrdWiFiSarTxPowerSet3Limit9;            ///< Offset 2240    EwrdWiFiSarTxPowerSet3Limit9
  UINT8    EwrdWiFiSarTxPowerSet3Limit10;           ///< Offset 2241    EwrdWiFiSarTxPowerSet3Limit10
  UINT8    EwrdWiFiSarTxPowerSet4Limit1;            ///< Offset 2242    EwrdWiFiSarTxPowerSet4Limit1
  UINT8    EwrdWiFiSarTxPowerSet4Limit2;            ///< Offset 2243    EwrdWiFiSarTxPowerSet4Limit2
  UINT8    EwrdWiFiSarTxPowerSet4Limit3;            ///< Offset 2244    EwrdWiFiSarTxPowerSet4Limit3
  UINT8    EwrdWiFiSarTxPowerSet4Limit4;            ///< Offset 2245    EwrdWiFiSarTxPowerSet4Limit4
  UINT8    EwrdWiFiSarTxPowerSet4Limit5;            ///< Offset 2246    EwrdWiFiSarTxPowerSet4Limit5
  UINT8    EwrdWiFiSarTxPowerSet4Limit6;            ///< Offset 2247    EwrdWiFiSarTxPowerSet4Limit6
  UINT8    EwrdWiFiSarTxPowerSet4Limit7;            ///< Offset 2248    EwrdWiFiSarTxPowerSet4Limit7
  UINT8    EwrdWiFiSarTxPowerSet4Limit8;            ///< Offset 2249    EwrdWiFiSarTxPowerSet4Limit8
  UINT8    EwrdWiFiSarTxPowerSet4Limit9;            ///< Offset 2250    EwrdWiFiSarTxPowerSet4Limit9
  UINT8    EwrdWiFiSarTxPowerSet4Limit10;           ///< Offset 2251    EwrdWiFiSarTxPowerSet4Limit10
  UINT8    WgdsWiFiSarDeltaGroup1PowerMax1;         ///< Offset 2252    WgdsWiFiSarDeltaGroup1PowerMax1
  UINT8    WgdsWiFiSarDeltaGroup1PowerChainA1;      ///< Offset 2253    WgdsWiFiSarDeltaGroup1PowerChainA1
  UINT8    WgdsWiFiSarDeltaGroup1PowerChainB1;      ///< Offset 2254    WgdsWiFiSarDeltaGroup1PowerChainB1
  UINT8    WgdsWiFiSarDeltaGroup1PowerMax2;         ///< Offset 2255    WgdsWiFiSarDeltaGroup1PowerMax2
  UINT8    WgdsWiFiSarDeltaGroup1PowerChainA2;      ///< Offset 2256    WgdsWiFiSarDeltaGroup1PowerChainA2
  UINT8    WgdsWiFiSarDeltaGroup1PowerChainB2;      ///< Offset 2257    WgdsWiFiSarDeltaGroup1PowerChainB2
  UINT8    WgdsWiFiSarDeltaGroup2PowerMax1;         ///< Offset 2258    WgdsWiFiSarDeltaGroup2PowerMax1
  UINT8    WgdsWiFiSarDeltaGroup2PowerChainA1;      ///< Offset 2259    WgdsWiFiSarDeltaGroup2PowerChainA1
  UINT8    WgdsWiFiSarDeltaGroup2PowerChainB1;      ///< Offset 2260    WgdsWiFiSarDeltaGroup2PowerChainB1
  UINT8    WgdsWiFiSarDeltaGroup2PowerMax2;         ///< Offset 2261    WgdsWiFiSarDeltaGroup2PowerMax2
  UINT8    WgdsWiFiSarDeltaGroup2PowerChainA2;      ///< Offset 2262    WgdsWiFiSarDeltaGroup2PowerChainA2
  UINT8    WgdsWiFiSarDeltaGroup2PowerChainB2;      ///< Offset 2263    WgdsWiFiSarDeltaGroup2PowerChainB2
  UINT8    WgdsWiFiSarDeltaGroup3PowerMax1;         ///< Offset 2264    WgdsWiFiSarDeltaGroup3PowerMax1
  UINT8    WgdsWiFiSarDeltaGroup3PowerChainA1;      ///< Offset 2265    WgdsWiFiSarDeltaGroup3PowerChainA1
  UINT8    WgdsWiFiSarDeltaGroup3PowerChainB1;      ///< Offset 2266    WgdsWiFiSarDeltaGroup3PowerChainB1
  UINT8    WgdsWiFiSarDeltaGroup3PowerMax2;         ///< Offset 2267    WgdsWiFiSarDeltaGroup3PowerMax2
  UINT8    WgdsWiFiSarDeltaGroup3PowerChainA2;      ///< Offset 2268    WgdsWiFiSarDeltaGroup3PowerChainA2
  UINT8    WgdsWiFiSarDeltaGroup3PowerChainB2;      ///< Offset 2269    WgdsWiFiSarDeltaGroup3PowerChainB2
  UINT8    Reserved18[41];                          ///< Offset 2270:2310
  // Reserved for Groups 4 to 9, each needs 6 bytes and total 36 bytes reserved
  UINT8    WiFiDynamicSarAntennaACurrentSet;        ///< Offset 2311    WiFiDynamicSarAntennaACurrentSet
  UINT8    WiFiDynamicSarAntennaBCurrentSet;        ///< Offset 2312    WiFiDynamicSarAntennaBCurrentSet
  UINT8    BluetoothSar;                            ///< Offset 2313    BluetoothSar
  UINT8    BluetoothIncreasedPwrMode;               ///< Offset 2314    BluetoothIncreasedPwrMode
  UINT8    BluetoothChainA;                         ///< Offset 2315    BluetoothChainA
  UINT8    BluetoothSarBr;                          ///< Offset 2316    BluetoothSarBr
  UINT8    BluetoothSarEdr2;                        ///< Offset 2317    BluetoothSarEdr2
  UINT8    BluetoothSarEdr3;                        ///< Offset 2318    BluetoothSarEdr3
  UINT8    BluetoothSarLe;                          ///< Offset 2319    BluetoothSarLe
  UINT8    BluetoothSarLe2Mhz;                      ///< Offset 2320    BluetoothSarLe2Mhz
  UINT8    BluetoothSarLeLr;                        ///< Offset 2321    BluetoothSarLeLr
  // Reserved for Bluetooth Sar future use
  UINT8    AntennaDiversity;                        ///< Offset 2322    AntennaDiversity
  UINT8    CoExistenceManager;                      ///< Offset 2323    CoExistenceManager
  UINT8    RunTimeVmControl;                        ///< Offset 2324    RunTime VM Control
  //
  //Feature Specific Data Bits
  //
  UINT8    UsbTypeCSupport;                         ///< Offset 2325    USB Type C Supported
  UINT32   HebcValue;                               ///< Offset 2326    HebcValue
  UINT8    PcdTsOnDimmTemperature;                  ///< Offset 2330    TS-on-DIMM temperature
  UINT8    Reserved19[1];                           ///< Offset 2331:2331
  UINT8    CvfSupport;                              ///< Offset 2332    To enable/disable Clover Falls(CVF)
  UINT8    CvfTestDevice;                           ///< Offset 2333    To select CVF TestDevice
  UINT8    CvfUsbPort;                              ///< Offset 2334    CVF USB port number
  UINT8    PcdMipiCamSensor;                        ///< Offset 2335    Mipi Camera Sensor
  UINT8    PcdNCT6776FCOM;                          ///< Offset 2336    NCT6776F COM
  UINT8    PcdNCT6776FSIO;                          ///< Offset 2337    NCT6776F SIO
  UINT8    PcdNCT6776FHWMON;                        ///< Offset 2338    NCT6776F HWMON
  UINT8    PcdH8S2113SIO;                           ///< Offset 2339    H8S2113 SIO
  UINT8    PcdH8S2113UAR;                           ///< Offset 2340    H8S2113 UAR
  UINT8    PcdZPoddConfig;                          ///< Offset 2341    ZPODD
  UINT8    PcdRGBCameraAdr;                         ///< Offset 2342    RGB Camera Address
  UINT8    PcdDepthCameraAdr;                       ///< Offset 2343    Depth Camera Addresy
  UINT8    PcdConvertableDockSupport;               ///< Offset 2344    Convertable Dock Support
  UINT8    PcdVirtualGpioButtonSupport;             ///< Offset 2345    Virtual Button Support
  UINT8    PcdAcpiEnableAllButtonSupport;           ///< Offset 2346    Acpi Enable All Button Support
  UINT8    PcdAcpiHidDriverButtonSupport;           ///< Offset 2347    Acpi Hid Driver Button Support
  //
  // UCMC setup option, GPIO Pad
  //
  UINT8    UCMS;                                    ///< Offset 2348    Option to select UCSI/UCMC device
  UINT32   UcmcPort1Gpio;                           ///< Offset 2349    Gpio for UCMC Port 1 Interrupt
  UINT32   UcmcPort2Gpio;                           ///< Offset 2353    Gpio for UCMC Port 2 Interrupt
  UINT32   RetimerPowerStateGpio;                   ///< Offset 2357    Gpio for Retimer Power state
  UINT8    TbtRTD3En;                               ///< Offset 2361    DTBT RTD3 enable status
  UINT8    Reserved20[56];                          ///< Offset 2362:2417
  UINT8    Ufp2DfpGlobalFlag;                       ///< Offset 2418    Upstream Facing port or Downstream Facing port Global Flag from LPC EC
  UINT8    Ufp2DfpUsbPort;                          ///< Offset 2419    Upstream Facing port or Downstream Facing port number from LPC EC
  UINT8    DbcGlobalFlag;                           ///< Offset 2420    Debug Mode Global Flag from LPC EC
  UINT8    DbcUsbPort;                              ///< Offset 2421    Debug Mode USB Port Number from LPC EC
  UINT8    TotalTypeCPorts;                         ///< Offset 2422    Total Number of type C ports that are supported by platform
  UINT8    UsbTypeCPort1;                           ///< Offset 2423    Type C Connector 1  Port mapping within the controller the port exposed
  UINT8    UsbTypeCPort1Pch;                        ///< Offset 2424    Type C Connector 1  Port mapping within the PCH controller (If Split mode supported)
  UINT8    UsbCPort1Proterties;                     ///< Offset 2425    Type C Connector 1  Portperties Split Support/Controller(PCH/TBT/CPU)/Root port (vaild for TBT)
  UINT8    UsbTypeCPort2;                           ///< Offset 2426    Type C Connector 2  Port mapping within the controller the port exposed
  UINT8    UsbTypeCPort2Pch;                        ///< Offset 2427    Type C Connector 2  Port mapping within the PCH controller (If Split mode supported)
  UINT8    UsbCPort2Proterties;                     ///< Offset 2428    Type C Connector 2  Portperties Split Support/Controller(PCH/TBT/CPU)/Root port (vaild for TBT)
  UINT8    UsbTypeCPort3;                           ///< Offset 2429    Type C Connector 3  Port mapping within the controller the port exposed
  UINT8    UsbTypeCPort3Pch;                        ///< Offset 2430    Type C Connector 3  Port mapping within the PCH controller (If Split mode supported)
  UINT8    UsbCPort3Proterties;                     ///< Offset 2431    Type C Connector 3  Portperties Split Support/Controller(PCH/TBT/CPU)/Root port (vaild for TBT)
  UINT8    UsbTypeCPort4;                           ///< Offset 2432    Type C Connector 4  Port mapping within the controller the port exposed
  UINT8    UsbTypeCPort4Pch;                        ///< Offset 2433    Type C Connector 4  Port mapping within the PCH controller (If Split mode supported)
  UINT8    UsbCPort4Proterties;                     ///< Offset 2434    Type C Connector 4  Portperties Split Support/Controller(PCH/TBT/CPU)/Root port (vaild for TBT)
  UINT8    UsbTypeCPort5;                           ///< Offset 2435    Type C Connector 5  Port mapping within the controller the port exposed
  UINT8    UsbTypeCPort5Pch;                        ///< Offset 2436    Type C Connector 5  Port mapping within the PCH controller (If Split mode supported)
  UINT8    UsbCPort5Proterties;                     ///< Offset 2437    Type C Connector 5  Portperties Split Support/Controller(PCH/TBT/CPU)/Root port (vaild for TBT)
  UINT8    UsbTypeCPort6;                           ///< Offset 2438    Type C Connector 6  Port mapping within the controller the port exposed
  UINT8    UsbTypeCPort6Pch;                        ///< Offset 2439    Type C Connector 6  Port mapping within the PCH controller (If Split mode supported)
  UINT8    UsbCPort6Proterties;                     ///< Offset 2440    Type C Connector 6  Portperties Split Support/Controller(PCH/TBT/CPU)/Root port (vaild for TBT)
  UINT8    SystemTimeAndAlarmSource;                ///< Offset 2441    Select source for System time and alarm
  UINT8    WwanRtd3Option;                          ///< Offset 2442    WWAN RTD3 options
  UINT32   WwanPerstGpio;                           ///< Offset 2443    WWAN PERST Gpio pin
  UINT8    WwanPerstGpioPolarity;                   ///< Offset 2447    WWAN PERST Gpio polarity
  UINT32   PcieSlot1WakeGpio;                       ///< Offset 2448    Pcie Slot 1 Wake Gpio pin
  UINT8    PcieSlot1RpNumber;                       ///< Offset 2452    Pcie Slot 1 Root Port Number
  UINT32   PcieSlot1PowerEnableGpio;                ///< Offset 2453    Pcie Slot 1 Power Enable Gpio pin
  UINT8    PcieSlot1PowerEnableGpioPolarity;        ///< Offset 2457    Pcie Slot 1 Power Enable Gpio pin polarity
  UINT32   PcieSlot1RstGpio;                        ///< Offset 2458    Pcie Slot 1 Rest Gpio pin
  UINT8    PcieSlot1RstGpioPolarity;                ///< Offset 2462    Pcie Slot 1 Rest Gpio pin polarity
  UINT32   PcieSlot2WakeGpio;                       ///< Offset 2463    Pcie Slot 2 Wake Gpio pin
  UINT8    PcieSlot2RpNumber;                       ///< Offset 2467    Pcie Slot 2 Root Port Number
  UINT32   PcieSlot2PowerEnableGpio;                ///< Offset 2468    Pcie Slot 2 Power Enable Gpio pin
  UINT8    PcieSlot2PowerEnableGpioPolarity;        ///< Offset 2472    Pcie Slot 2 Power Enable Gpio pin polarity
  UINT32   PcieSlot2RstGpio;                        ///< Offset 2473    Pcie Slot 2 Rest Gpio pin
  UINT8    PcieSlot2RstGpioPolarity;                ///< Offset 2477    Pcie Slot 2 Rest Gpio pin polarity
  UINT32   PcieSlot3WakeGpio;                       ///< Offset 2478    Pcie Slot 3 Wake Gpio pin
  UINT8    PcieSlot3RpNumber;                       ///< Offset 2482    Pcie Slot 3 Root Port Number
  UINT32   PcieSlot3PowerEnableGpio;                ///< Offset 2483    Pcie Slot 3 Power Enable Gpio pin
  UINT8    PcieSlot3PowerEnableGpioPolarity;        ///< Offset 2487    Pcie Slot 3 Power Enable Gpio pin polarity
  UINT32   PcieSlot3RstGpio;                        ///< Offset 2488    Pcie Slot 3 Rest Gpio pin
  UINT8    PcieSlot3RstGpioPolarity;                ///< Offset 2492    Pcie Slot 3 Rest Gpio pin polarity
  UINT32   PcieSlot4WakeGpio;                       ///< Offset 2493    Pcie Slot 4 Wake Gpio pin
  UINT8    PcieSlot4RpNumber;                       ///< Offset 2497    Pcie Slot 4 Root Port Number
  UINT32   PcieSlot4PowerEnableGpio;                ///< Offset 2498    Pcie Slot 4 Power Enable Gpio pin
  UINT8    PcieSlot4PowerEnableGpioPolarity;        ///< Offset 2502    Pcie Slot 4 Power Enable Gpio pin polarity
  UINT32   PcieSlot4RstGpio;                        ///< Offset 2503    Pcie Slot 4 Rest Gpio pin
  UINT8    PcieSlot4RstGpioPolarity;                ///< Offset 2507    Pcie Slot 4 Rest Gpio pin polarity
  UINT32   PcieSlot5WakeGpio;                       ///< Offset 2508    Pcie Slot 5 Wake Gpio pin
  UINT8    PcieSlot5RpNumber;                       ///< Offset 2512    Pcie Slot 5 Root Port Number
  UINT32   PcieSlot5PowerEnableGpio;                ///< Offset 2513    Pcie Slot 5 Power Enable Gpio pin
  UINT8    PcieSlot5PowerEnableGpioPolarity;        ///< Offset 2517    Pcie Slot 5 Power Enable Gpio pin polarity
  UINT32   PcieSlot5RstGpio;                        ///< Offset 2518    Pcie Slot 5 Rest Gpio pin
  UINT8    PcieSlot5RstGpioPolarity;                ///< Offset 2522    Pcie Slot 5 Rest Gpio pin polarity
  UINT32   PcieSlot6WakeGpio;                       ///< Offset 2523    Pcie Slot 6 Wake Gpio pin
  UINT8    PcieSlot6RpNumber;                       ///< Offset 2527    Pcie Slot 6 Root Port Number
  UINT32   PcieSlot6PowerEnableGpio;                ///< Offset 2528    Pcie Slot 6 Power Enable Gpio pin
  UINT8    PcieSlot6PowerEnableGpioPolarity;        ///< Offset 2532    Pcie Slot 6 Power Enable Gpio pin polarity
  UINT32   PcieSlot6RstGpio;                        ///< Offset 2533    Pcie Slot 6 Rest Gpio pin
  UINT8    PcieSlot6RstGpioPolarity;                ///< Offset 2537    Pcie Slot 6 Rest Gpio pin polarity
  UINT32   PcieSlot7WakeGpio;                       ///< Offset 2538    Pcie Slot 7 Wake Gpio pin
  UINT8    PcieSlot7RpNumber;                       ///< Offset 2542    Pcie Slot 7 Root Port Number
  UINT32   PcieSlot7PowerEnableGpio;                ///< Offset 2543    Pcie Slot 7 Power Enable Gpio pin
  UINT8    PcieSlot7PowerEnableGpioPolarity;        ///< Offset 2547    Pcie Slot 7 Power Enable Gpio pin polarity
  UINT32   PcieSlot7RstGpio;                        ///< Offset 2548    Pcie Slot 7 Rest Gpio pin
  UINT8    PcieSlot7RstGpioPolarity;                ///< Offset 2552    Pcie Slot 7 Rest Gpio pin polarity
  UINT8    PowermeterDeviceEnable;                  ///< Offset 2553    Enable PowerMeter
  UINT32   WwanFullCardPowerOffGpio;                ///< Offset 2554    WWAN Full card power off gpio pin
  UINT8    WwanFullCardPowerOffGpioPolarity;        ///< Offset 2558    WWAN Full card power off gpio pin polarity
  UINT32   WwanBbrstGpio;                           ///< Offset 2559    WWAN BBRST Gpio pin
  UINT8    WwanBbrstGpioPolarity;                   ///< Offset 2563    WWAN BBRST Gpio pin polarity
  UINT32   WwanWakeGpio;                            ///< Offset 2564    WWAN Wake Gpio pin
  UINT32   Touchpanel0PowerEnableGpio;              ///< Offset 2568    Gpio for touchPaneL 0 power enable
  UINT32   Touchpanel0PowerRstGpio;                 ///< Offset 2572    Gpio for touchPaneL 0 Reset
  UINT8    Touchpanel0IrqGpioPolarity;              ///< Offset 2576    TouchPaneL 0 Interrupt Gpio pin polarity
  UINT8    Touchpanel0PowerEnableGpioPolarity;      ///< Offset 2577    TouchPaneL 0 power enable Gpio pin polarity
  UINT8    Touchpanel0PowerRstGpioPolarity;         ///< Offset 2578    TouchPaneL 0 Reset Gpio pin polarity
  UINT32   Touchpanel0IrqGpio;                      ///< Offset 2579    Gpio for touchPaneL 0 Interrupt
  UINT32   Touchpanel1PowerEnableGpio;              ///< Offset 2583    Gpio for touchPaneL 1 power enable
  UINT32   Touchpanel1PowerRstGpio;                 ///< Offset 2587    Gpio for touchPaneL 1 Reset
  UINT8    Touchpanel1IrqGpioPolarity;              ///< Offset 2591    TouchPaneL 1 Interrupt Gpio pin polarity
  UINT8    Touchpanel1PowerEnableGpioPolarity;      ///< Offset 2592    TouchPaneL 1 power enable Gpio pin polarity
  UINT8    Touchpanel1PowerRstGpioPolarity;         ///< Offset 2593    TouchPaneL 1 Reset Gpio pin polarity
  UINT32   Touchpanel1IrqGpio;                      ///< Offset 2594    Gpio for touchPaneL 1 interrupt
  UINT8    TouchpadIrqGpioPolarity;                 ///< Offset 2598    TouchPaD Interrupt Gpio pin polarity
  UINT32   WlanWakeGpio;                            ///< Offset 2599    WLAN Wake Gpio pin
  UINT8    WlanRootPortNumber;                      ///< Offset 2603    WLAN Root Port Number
  UINT32   SataPortPowerEnableGpio;                 ///< Offset 2604    Sata port Power Enable Gpio pin
  UINT8    SataPortPowerEnableGpioPolarity;         ///< Offset 2608    Sata port Power Enable Gpio pin polarity
  UINT32   M2Ssd1PowerEnableGpio;                   ///< Offset 2609    Pcie slot 1 M.2 SSD Power Enable Gpio pin
  UINT8    M2Ssd1PowerEnableGpioPolarity;           ///< Offset 2613    Pcie slot 1 M.2 SSD Power Enable Gpio pin polarity
  UINT32   M2Ssd1RstGpio;                           ///< Offset 2614    Pcie slot 1 M.2 SSD Reset Gpio pin
  UINT8    M2Ssd1RstGpioPolarity;                   ///< Offset 2618    Pcie slot 1 M.2 SSD Reset Gpio pin polarity
  UINT32   M2Ssd2PowerEnableGpio;                   ///< Offset 2619    Pcie slot 2 M.2 SSD Power Enable Gpio pin
  UINT8    M2Ssd2PowerEnableGpioPolarity;           ///< Offset 2623    Pcie slot 2 M.2 SSD Power Enable Gpio pin polarity
  UINT32   M2Ssd2RstGpio;                           ///< Offset 2624    Pcie slot 2 M.2 SSD Reset Gpio pin
  UINT8    M2Ssd2RstGpioPolarity;                   ///< Offset 2628    Pcie slot 2 M.2 SSD Reset Gpio pin polarity
  UINT32   M2Ssd3PowerEnableGpio;                   ///< Offset 2629    Pcie slot 3 M.2 SSD Power Enable Gpio pin
  UINT8    M2Ssd3PowerEnableGpioPolarity;           ///< Offset 2633    Pcie slot 3 M.2 SSD Power Enable Gpio pin polarity
  UINT32   M2Ssd3RstGpio;                           ///< Offset 2634    Pcie slot 3 M.2 SSD Reset Gpio pin
  UINT8    M2Ssd3RstGpioPolarity;                   ///< Offset 2638    Pcie slot 3 M.2 SSD Reset Gpio pin polarity
  UINT32   M2Ssd4PowerEnableGpio;                   ///< Offset 2639    Pcie slot 4 M.2 SSD Power Enable Gpio pin
  UINT8    M2Ssd4PowerEnableGpioPolarity;           ///< Offset 2643    Pcie slot 4 M.2 SSD Power Enable Gpio pin polarity
  UINT32   M2Ssd4RstGpio;                           ///< Offset 2644    Pcie slot 4 M.2 SSD Reset Gpio pin
  UINT8    M2Ssd4RstGpioPolarity;                   ///< Offset 2648    Pcie slot 4 M.2 SSD Reset Gpio pin polarity
  UINT32   M2Ssd5PowerEnableGpio;                   ///< Offset 2649    Pcie slot 5 M.2 SSD Power Enable Gpio pin
  UINT8    M2Ssd5PowerEnableGpioPolarity;           ///< Offset 2653    Pcie slot 5 M.2 SSD Power Enable Gpio pin polarity
  UINT32   M2Ssd5RstGpio;                           ///< Offset 2654    Pcie slot 5 M.2 SSD Reset Gpio pin
  UINT8    M2Ssd5RstGpioPolarity;                   ///< Offset 2658    Pcie slot 5 M.2 SSD Reset Gpio pin polarity
  UINT8    SdevXhciInterfaceNumber1;                ///< Offset 2659    SDEV xHCI Interface Number for device 1
  UINT8    SdevXhciInterfaceNumber2;                ///< Offset 2660    SDEV xHCI Interface Number for device 2
  UINT8    SdevXhciRootPortNumber1;                 ///< Offset 2661    SDEV xHCI Root Port Number for device 1
  UINT8    SdevXhciRootPortNumber2;                 ///< Offset 2662    SDEV xHCI Root Port Number for device 2
  UINT8    TsnPcsEnabled;                           ///< Offset 2663    TSN PCS device Enable
  UINT8    WwanSourceClock;                         ///< Offset 2664    WWAN Source Clock
  UINT32   CnvExtClock;                             ///< Offset 2665    CNV external 32KHz Clock
  UINT8    WwanRootPortNumber;                      ///< Offset 2669    WWAN Root Port Nmuber
  UINT8    WifiAntGainMode;                         ///< Offset 2670    Wifi Ant Gain Mode
  UINT8    WifiAntGainChainA1;                      ///< Offset 2671    Ant Gain Table Chain A 2400
  UINT8    WifiAntGainChainA2;                      ///< Offset 2672    Ant Gain Table Chain A 5180-5320
  UINT8    WifiAntGainChainA3;                      ///< Offset 2673    Ant Gain Table Chain A 5340-5440
  UINT8    WifiAntGainChainA4;                      ///< Offset 2674    Ant Gain Table Chain A 5460-5700
  UINT8    WifiAntGainChainA5;                      ///< Offset 2675    Ant Gain Table Chain A 5720-5825
  UINT8    WifiAntGainChainB1;                      ///< Offset 2676    Ant Gain Table Chain B 2400
  UINT8    WifiAntGainChainB2;                      ///< Offset 2677    Ant Gain Table Chain B 5150-5320
  UINT8    WifiAntGainChainB3;                      ///< Offset 2678    Ant Gain Table Chain B 5340-5440
  UINT8    WifiAntGainChainB4;                      ///< Offset 2679    Ant Gain Table Chain B 5460-5700
  UINT8    WifiAntGainChainB5;                      ///< Offset 2680    Ant Gain Table Chain B 5720-5825
  UINT8    WifiActiveChannelSrd;                    ///< Offset 2681    SRD Active Channels Selection
  UINT8    WifiIndonesia5GhzSupport;                ///< Offset 2682    Indonesia 5.15-5.35 GHz Band Support Selection
  UINT32   Dg1VramSRGpio;                           ///< Offset 2683    DG1 VRAM Self Refresh Gpio pin
  UINT32   LpmReqRegAddr;                           ///< Offset 2687    Low Power Mode required register Address
  UINT8    Gpe1BlockEnable;                         ///< Offset 2691    Enable GPE1
  UINT8    Reserved21[4];                           ///< Offset 2692:2695
  //
  // WifiPhyFilterConfigX is deprecated.
  //
  UINT32   WifiPhyFilterConfigA;                    ///< Offset 2696    Platform PHY Config Chain A.
  UINT32   WifiPhyFilterConfigB;                    ///< Offset 2700    Platform PHY Config Chain B.
  UINT32   WifiPhyFilterConfigC;                    ///< Offset 2704    Platform PHY Config Chain C.
  UINT32   WifiPhyFilterConfigD;                    ///< Offset 2708    Platform PHY Config Chain D.
  UINT8    WrdsWiFiSarTxPowerSet1Limit11;           ///< Offset 2712    WrdsWiFiSarTxPowerSet1Limit11
  UINT8    WrdsWiFiSarTxPowerSet1Limit12;           ///< Offset 2713    WrdsWiFiSarTxPowerSet1Limit12
  UINT8    WrdsWiFiSarTxPowerSet1Limit13;           ///< Offset 2714    WrdsWiFiSarTxPowerSet1Limit13
  UINT8    WrdsWiFiSarTxPowerSet1Limit14;           ///< Offset 2715    WrdsWiFiSarTxPowerSet1Limit14
  UINT8    WrdsWiFiSarTxPowerSet1Limit15;           ///< Offset 2716    WrdsWiFiSarTxPowerSet1Limit15
  UINT8    WrdsWiFiSarTxPowerSet1Limit16;           ///< Offset 2717    WrdsWiFiSarTxPowerSet1Limit16
  UINT8    WrdsWiFiSarTxPowerSet1Limit17;           ///< Offset 2718    WrdsWiFiSarTxPowerSet1Limit17
  UINT8    WrdsWiFiSarTxPowerSet1Limit18;           ///< Offset 2719    WrdsWiFiSarTxPowerSet1Limit18
  UINT8    WrdsWiFiSarTxPowerSet1Limit19;           ///< Offset 2720    WrdsWiFiSarTxPowerSet1Limit19
  UINT8    WrdsWiFiSarTxPowerSet1Limit20;           ///< Offset 2721    WrdsWiFiSarTxPowerSet1Limit20
  UINT8    WrdsWiFiSarTxPowerSet1Limit21;           ///< Offset 2722    WrdsWiFiSarTxPowerSet1Limit21
  UINT8    WrdsWiFiSarTxPowerSet1Limit22;           ///< Offset 2723    WrdsWiFiSarTxPowerSet1Limit22
  UINT8    EwrdWiFiSarTxPowerSet2Limit11;           ///< Offset 2724    EwrdWiFiSarTxPowerSet2Limit11
  UINT8    EwrdWiFiSarTxPowerSet2Limit12;           ///< Offset 2725    EwrdWiFiSarTxPowerSet2Limit12
  UINT8    EwrdWiFiSarTxPowerSet2Limit13;           ///< Offset 2726    EwrdWiFiSarTxPowerSet2Limit13
  UINT8    EwrdWiFiSarTxPowerSet2Limit14;           ///< Offset 2727    EwrdWiFiSarTxPowerSet2Limit14
  UINT8    EwrdWiFiSarTxPowerSet2Limit15;           ///< Offset 2728    EwrdWiFiSarTxPowerSet2Limit15
  UINT8    EwrdWiFiSarTxPowerSet2Limit16;           ///< Offset 2729    EwrdWiFiSarTxPowerSet2Limit16
  UINT8    EwrdWiFiSarTxPowerSet2Limit17;           ///< Offset 2730    EwrdWiFiSarTxPowerSet2Limit17
  UINT8    EwrdWiFiSarTxPowerSet2Limit18;           ///< Offset 2731    EwrdWiFiSarTxPowerSet2Limit18
  UINT8    EwrdWiFiSarTxPowerSet2Limit19;           ///< Offset 2732    EwrdWiFiSarTxPowerSet2Limit19
  UINT8    EwrdWiFiSarTxPowerSet2Limit20;           ///< Offset 2733    EwrdWiFiSarTxPowerSet2Limit20
  UINT8    EwrdWiFiSarTxPowerSet2Limit21;           ///< Offset 2734    EwrdWiFiSarTxPowerSet2Limit21
  UINT8    EwrdWiFiSarTxPowerSet2Limit22;           ///< Offset 2735    EwrdWiFiSarTxPowerSet2Limit22
  UINT8    EwrdWiFiSarTxPowerSet3Limit11;           ///< Offset 2736    EwrdWiFiSarTxPowerSet3Limit11
  UINT8    EwrdWiFiSarTxPowerSet3Limit12;           ///< Offset 2737    EwrdWiFiSarTxPowerSet3Limit12
  UINT8    EwrdWiFiSarTxPowerSet3Limit13;           ///< Offset 2738    EwrdWiFiSarTxPowerSet3Limit13
  UINT8    EwrdWiFiSarTxPowerSet3Limit14;           ///< Offset 2739    EwrdWiFiSarTxPowerSet3Limit14
  UINT8    EwrdWiFiSarTxPowerSet3Limit15;           ///< Offset 2740    EwrdWiFiSarTxPowerSet3Limit15
  UINT8    EwrdWiFiSarTxPowerSet3Limit16;           ///< Offset 2741    EwrdWiFiSarTxPowerSet3Limit16
  UINT8    EwrdWiFiSarTxPowerSet3Limit17;           ///< Offset 2742    EwrdWiFiSarTxPowerSet3Limit17
  UINT8    EwrdWiFiSarTxPowerSet3Limit18;           ///< Offset 2743    EwrdWiFiSarTxPowerSet3Limit18
  UINT8    EwrdWiFiSarTxPowerSet3Limit19;           ///< Offset 2744    EwrdWiFiSarTxPowerSet3Limit19
  UINT8    EwrdWiFiSarTxPowerSet3Limit20;           ///< Offset 2745    EwrdWiFiSarTxPowerSet3Limit20
  UINT8    EwrdWiFiSarTxPowerSet3Limit21;           ///< Offset 2746    EwrdWiFiSarTxPowerSet3Limit21
  UINT8    EwrdWiFiSarTxPowerSet3Limit22;           ///< Offset 2747    EwrdWiFiSarTxPowerSet3Limit22
  UINT8    EwrdWiFiSarTxPowerSet4Limit11;           ///< Offset 2748    EwrdWiFiSarTxPowerSet4Limit11
  UINT8    EwrdWiFiSarTxPowerSet4Limit12;           ///< Offset 2749    EwrdWiFiSarTxPowerSet4Limit12
  UINT8    EwrdWiFiSarTxPowerSet4Limit13;           ///< Offset 2750    EwrdWiFiSarTxPowerSet4Limit13
  UINT8    EwrdWiFiSarTxPowerSet4Limit14;           ///< Offset 2751    EwrdWiFiSarTxPowerSet4Limit14
  UINT8    EwrdWiFiSarTxPowerSet4Limit15;           ///< Offset 2752    EwrdWiFiSarTxPowerSet4Limit15
  UINT8    EwrdWiFiSarTxPowerSet4Limit16;           ///< Offset 2753    EwrdWiFiSarTxPowerSet4Limit16
  UINT8    EwrdWiFiSarTxPowerSet4Limit17;           ///< Offset 2754    EwrdWiFiSarTxPowerSet4Limit17
  UINT8    EwrdWiFiSarTxPowerSet4Limit18;           ///< Offset 2755    EwrdWiFiSarTxPowerSet4Limit18
  UINT8    EwrdWiFiSarTxPowerSet4Limit19;           ///< Offset 2756    EwrdWiFiSarTxPowerSet4Limit19
  UINT8    EwrdWiFiSarTxPowerSet4Limit20;           ///< Offset 2757    EwrdWiFiSarTxPowerSet4Limit20
  UINT8    EwrdWiFiSarTxPowerSet4Limit21;           ///< Offset 2758    EwrdWiFiSarTxPowerSet4Limit21
  UINT8    EwrdWiFiSarTxPowerSet4Limit22;           ///< Offset 2759    EwrdWiFiSarTxPowerSet4Limit22
  UINT8    WgdsWiFiSarDeltaGroup1PowerMax3;         ///< Offset 2760    WgdsWiFiSarDeltaGroup1PowerMax3
  UINT8    WgdsWiFiSarDeltaGroup1PowerChainA3;      ///< Offset 2761    WgdsWiFiSarDeltaGroup1PowerChainA3
  UINT8    WgdsWiFiSarDeltaGroup1PowerChainB3;      ///< Offset 2762    WgdsWiFiSarDeltaGroup1PowerChainB3
  UINT8    WgdsWiFiSarDeltaGroup2PowerMax3;         ///< Offset 2763    WgdsWiFiSarDeltaGroup2PowerMax3
  UINT8    WgdsWiFiSarDeltaGroup2PowerChainA3;      ///< Offset 2764    WgdsWiFiSarDeltaGroup2PowerChainA3
  UINT8    WgdsWiFiSarDeltaGroup2PowerChainB3;      ///< Offset 2765    WgdsWiFiSarDeltaGroup2PowerChainB3
  UINT8    WgdsWiFiSarDeltaGroup3PowerMax3;         ///< Offset 2766    WgdsWiFiSarDeltaGroup3PowerMax3
  UINT8    WgdsWiFiSarDeltaGroup3PowerChainA3;      ///< Offset 2767    WgdsWiFiSarDeltaGroup3PowerChainA3
  UINT8    WgdsWiFiSarDeltaGroup3PowerChainB3;      ///< Offset 2768    WgdsWiFiSarDeltaGroup3PowerChainB3
  UINT8    WifiAntGainChainA6;                      ///< Offset 2769    Ant Gain Table Chain A 5845-6135
  UINT8    WifiAntGainChainA7;                      ///< Offset 2770    Ant Gain Table Chain A 6155-6375
  UINT8    WifiAntGainChainA8;                      ///< Offset 2771    Ant Gain Table Chain A 6395-6495
  UINT8    WifiAntGainChainA9;                      ///< Offset 2772    Ant Gain Table Chain A 6515-6675
  UINT8    WifiAntGainChainA10;                     ///< Offset 2773    Ant Gain Table Chain A 6695-6835
  UINT8    WifiAntGainChainA11;                     ///< Offset 2774    Ant Gain Table Chain A 6855-7095
  UINT8    WifiAntGainChainB6;                      ///< Offset 2775    Ant Gain Table Chain B 5845-6135
  UINT8    WifiAntGainChainB7;                      ///< Offset 2776    Ant Gain Table Chain B 6155-6375
  UINT8    WifiAntGainChainB8;                      ///< Offset 2777    Ant Gain Table Chain B 6395-6495
  UINT8    WifiAntGainChainB9;                      ///< Offset 2778    Ant Gain Table Chain B 6515-6675
  UINT8    WifiAntGainChainB10;                     ///< Offset 2779    Ant Gain Table Chain B 6695-6835
  UINT8    WifiAntGainChainB11;                     ///< Offset 2780    Ant Gain Table Chain B 6855-7095
  UINT32   WifiUltraHighBandSupport;                ///< Offset 2781    WifiUltraHighBandSupport
  UINT8    AxSettingUkraine;                        ///< Offset 2785    11AxSettingUkraine
  UINT8    AxModeUkraine;                           ///< Offset 2786    11AxModeUkraine
  UINT8    AxSettingRussia;                         ///< Offset 2787    11AxSettingRussia
  UINT8    AxModeRussia;                            ///< Offset 2788    11AxModeRussia
  UINT8    WrdsCdbWiFiSarTxPowerSet1Limit1;         ///< Offset 2789    WrdsCdbWiFiSarTxPowerSet1Limit1
  UINT8    WrdsCdbWiFiSarTxPowerSet1Limit2;         ///< Offset 2790    WrdsCdbWiFiSarTxPowerSet1Limit2
  UINT8    WrdsCdbWiFiSarTxPowerSet1Limit3;         ///< Offset 2791    WrdsCdbWiFiSarTxPowerSet1Limit3
  UINT8    WrdsCdbWiFiSarTxPowerSet1Limit4;         ///< Offset 2792    WrdsCdbWiFiSarTxPowerSet1Limit4
  UINT8    WrdsCdbWiFiSarTxPowerSet1Limit5;         ///< Offset 2793    WrdsCdbWiFiSarTxPowerSet1Limit5
  UINT8    WrdsCdbWiFiSarTxPowerSet1Limit6;         ///< Offset 2794    WrdsCdbWiFiSarTxPowerSet1Limit6
  UINT8    WrdsCdbWiFiSarTxPowerSet1Limit7;         ///< Offset 2795    WrdsCdbWiFiSarTxPowerSet1Limit7
  UINT8    WrdsCdbWiFiSarTxPowerSet1Limit8;         ///< Offset 2796    WrdsCdbWiFiSarTxPowerSet1Limit8
  UINT8    WrdsCdbWiFiSarTxPowerSet1Limit9;         ///< Offset 2797    WrdsCdbWiFiSarTxPowerSet1Limit9
  UINT8    WrdsCdbWiFiSarTxPowerSet1Limit10;        ///< Offset 2798    WrdsCdbWiFiSarTxPowerSet1Limit10
  UINT8    WrdsCdbWiFiSarTxPowerSet1Limit11;        ///< Offset 2799    WrdsCdbWiFiSarTxPowerSet1Limit11
  UINT8    WrdsCdbWiFiSarTxPowerSet1Limit12;        ///< Offset 2800    WrdsCdbWiFiSarTxPowerSet1Limit12
  UINT8    WrdsCdbWiFiSarTxPowerSet1Limit13;        ///< Offset 2801    WrdsCdbWiFiSarTxPowerSet1Limit13
  UINT8    WrdsCdbWiFiSarTxPowerSet1Limit14;        ///< Offset 2802    WrdsCdbWiFiSarTxPowerSet1Limit14
  UINT8    WrdsCdbWiFiSarTxPowerSet1Limit15;        ///< Offset 2803    WrdsCdbWiFiSarTxPowerSet1Limit15
  UINT8    WrdsCdbWiFiSarTxPowerSet1Limit16;        ///< Offset 2804    WrdsCdbWiFiSarTxPowerSet1Limit16
  UINT8    WrdsCdbWiFiSarTxPowerSet1Limit17;        ///< Offset 2805    WrdsCdbWiFiSarTxPowerSet1Limit17
  UINT8    WrdsCdbWiFiSarTxPowerSet1Limit18;        ///< Offset 2806    WrdsCdbWiFiSarTxPowerSet1Limit18
  UINT8    WrdsCdbWiFiSarTxPowerSet1Limit19;        ///< Offset 2807    WrdsCdbWiFiSarTxPowerSet1Limit19
  UINT8    WrdsCdbWiFiSarTxPowerSet1Limit20;        ///< Offset 2808    WrdsCdbWiFiSarTxPowerSet1Limit20
  UINT8    WrdsCdbWiFiSarTxPowerSet1Limit21;        ///< Offset 2809    WrdsCdbWiFiSarTxPowerSet1Limit21
  UINT8    WrdsCdbWiFiSarTxPowerSet1Limit22;        ///< Offset 2810    WrdsCdbWiFiSarTxPowerSet1Limit22
  UINT8    EwrdCdbWiFiSarTxPowerSet2Limit1;         ///< Offset 2811    EwrdCdbWiFiSarTxPowerSet2Limit1
  UINT8    EwrdCdbWiFiSarTxPowerSet2Limit2;         ///< Offset 2812    EwrdCdbWiFiSarTxPowerSet2Limit2
  UINT8    EwrdCdbWiFiSarTxPowerSet2Limit3;         ///< Offset 2813    EwrdCdbWiFiSarTxPowerSet2Limit3
  UINT8    EwrdCdbWiFiSarTxPowerSet2Limit4;         ///< Offset 2814    EwrdCdbWiFiSarTxPowerSet2Limit4
  UINT8    EwrdCdbWiFiSarTxPowerSet2Limit5;         ///< Offset 2815    EwrdCdbWiFiSarTxPowerSet2Limit5
  UINT8    EwrdCdbWiFiSarTxPowerSet2Limit6;         ///< Offset 2816    EwrdCdbWiFiSarTxPowerSet2Limit6
  UINT8    EwrdCdbWiFiSarTxPowerSet2Limit7;         ///< Offset 2817    EwrdCdbWiFiSarTxPowerSet2Limit7
  UINT8    EwrdCdbWiFiSarTxPowerSet2Limit8;         ///< Offset 2818    EwrdCdbWiFiSarTxPowerSet2Limit8
  UINT8    EwrdCdbWiFiSarTxPowerSet2Limit9;         ///< Offset 2819    EwrdCdbWiFiSarTxPowerSet2Limit9
  UINT8    EwrdCdbWiFiSarTxPowerSet2Limit10;        ///< Offset 2820    EwrdCdbWiFiSarTxPowerSet2Limit10
  UINT8    EwrdCdbWiFiSarTxPowerSet2Limit11;        ///< Offset 2821    EwrdCdbWiFiSarTxPowerSet2Limit11
  UINT8    EwrdCdbWiFiSarTxPowerSet2Limit12;        ///< Offset 2822    EwrdCdbWiFiSarTxPowerSet2Limit12
  UINT8    EwrdCdbWiFiSarTxPowerSet2Limit13;        ///< Offset 2823    EwrdCdbWiFiSarTxPowerSet2Limit13
  UINT8    EwrdCdbWiFiSarTxPowerSet2Limit14;        ///< Offset 2824    EwrdCdbWiFiSarTxPowerSet2Limit14
  UINT8    EwrdCdbWiFiSarTxPowerSet2Limit15;        ///< Offset 2825    EwrdCdbWiFiSarTxPowerSet2Limit15
  UINT8    EwrdCdbWiFiSarTxPowerSet2Limit16;        ///< Offset 2826    EwrdCdbWiFiSarTxPowerSet2Limit16
  UINT8    EwrdCdbWiFiSarTxPowerSet2Limit17;        ///< Offset 2827    EwrdCdbWiFiSarTxPowerSet2Limit17
  UINT8    EwrdCdbWiFiSarTxPowerSet2Limit18;        ///< Offset 2828    EwrdCdbWiFiSarTxPowerSet2Limit18
  UINT8    EwrdCdbWiFiSarTxPowerSet2Limit19;        ///< Offset 2829    EwrdCdbWiFiSarTxPowerSet2Limit19
  UINT8    EwrdCdbWiFiSarTxPowerSet2Limit20;        ///< Offset 2830    EwrdCdbWiFiSarTxPowerSet2Limit20
  UINT8    EwrdCdbWiFiSarTxPowerSet2Limit21;        ///< Offset 2831    EwrdCdbWiFiSarTxPowerSet2Limit21
  UINT8    EwrdCdbWiFiSarTxPowerSet2Limit22;        ///< Offset 2832    EwrdCdbWiFiSarTxPowerSet2Limit22
  UINT8    EwrdCdbWiFiSarTxPowerSet3Limit1;         ///< Offset 2833    EwrdCdbWiFiSarTxPowerSet3Limit1
  UINT8    EwrdCdbWiFiSarTxPowerSet3Limit2;         ///< Offset 2834    EwrdCdbWiFiSarTxPowerSet3Limit2
  UINT8    EwrdCdbWiFiSarTxPowerSet3Limit3;         ///< Offset 2835    EwrdCdbWiFiSarTxPowerSet3Limit3
  UINT8    EwrdCdbWiFiSarTxPowerSet3Limit4;         ///< Offset 2836    EwrdCdbWiFiSarTxPowerSet3Limit4
  UINT8    EwrdCdbWiFiSarTxPowerSet3Limit5;         ///< Offset 2837    EwrdCdbWiFiSarTxPowerSet3Limit5
  UINT8    EwrdCdbWiFiSarTxPowerSet3Limit6;         ///< Offset 2838    EwrdCdbWiFiSarTxPowerSet3Limit6
  UINT8    EwrdCdbWiFiSarTxPowerSet3Limit7;         ///< Offset 2839    EwrdCdbWiFiSarTxPowerSet3Limit7
  UINT8    EwrdCdbWiFiSarTxPowerSet3Limit8;         ///< Offset 2840    EwrdCdbWiFiSarTxPowerSet3Limit8
  UINT8    EwrdCdbWiFiSarTxPowerSet3Limit9;         ///< Offset 2841    EwrdCdbWiFiSarTxPowerSet3Limit9
  UINT8    EwrdCdbWiFiSarTxPowerSet3Limit10;        ///< Offset 2842    EwrdCdbWiFiSarTxPowerSet3Limit10
  UINT8    EwrdCdbWiFiSarTxPowerSet3Limit11;        ///< Offset 2843    EwrdCdbWiFiSarTxPowerSet3Limit11
  UINT8    EwrdCdbWiFiSarTxPowerSet3Limit12;        ///< Offset 2844    EwrdCdbWiFiSarTxPowerSet3Limit12
  UINT8    EwrdCdbWiFiSarTxPowerSet3Limit13;        ///< Offset 2845    EwrdCdbWiFiSarTxPowerSet3Limit13
  UINT8    EwrdCdbWiFiSarTxPowerSet3Limit14;        ///< Offset 2846    EwrdCdbWiFiSarTxPowerSet3Limit14
  UINT8    EwrdCdbWiFiSarTxPowerSet3Limit15;        ///< Offset 2847    EwrdCdbWiFiSarTxPowerSet3Limit15
  UINT8    EwrdCdbWiFiSarTxPowerSet3Limit16;        ///< Offset 2848    EwrdCdbWiFiSarTxPowerSet3Limit16
  UINT8    EwrdCdbWiFiSarTxPowerSet3Limit17;        ///< Offset 2849    EwrdCdbWiFiSarTxPowerSet3Limit17
  UINT8    EwrdCdbWiFiSarTxPowerSet3Limit18;        ///< Offset 2850    EwrdCdbWiFiSarTxPowerSet3Limit18
  UINT8    EwrdCdbWiFiSarTxPowerSet3Limit19;        ///< Offset 2851    EwrdCdbWiFiSarTxPowerSet3Limit19
  UINT8    EwrdCdbWiFiSarTxPowerSet3Limit20;        ///< Offset 2852    EwrdCdbWiFiSarTxPowerSet3Limit20
  UINT8    EwrdCdbWiFiSarTxPowerSet3Limit21;        ///< Offset 2853    EwrdCdbWiFiSarTxPowerSet3Limit21
  UINT8    EwrdCdbWiFiSarTxPowerSet3Limit22;        ///< Offset 2854    EwrdCdbWiFiSarTxPowerSet3Limit22
  UINT8    EwrdCdbWiFiSarTxPowerSet4Limit1;         ///< Offset 2855    EwrdCdbWiFiSarTxPowerSet4Limit1
  UINT8    EwrdCdbWiFiSarTxPowerSet4Limit2;         ///< Offset 2856    EwrdCdbWiFiSarTxPowerSet4Limit2
  UINT8    EwrdCdbWiFiSarTxPowerSet4Limit3;         ///< Offset 2857    EwrdCdbWiFiSarTxPowerSet4Limit3
  UINT8    EwrdCdbWiFiSarTxPowerSet4Limit4;         ///< Offset 2858    EwrdCdbWiFiSarTxPowerSet4Limit4
  UINT8    EwrdCdbWiFiSarTxPowerSet4Limit5;         ///< Offset 2859    EwrdCdbWiFiSarTxPowerSet4Limit5
  UINT8    EwrdCdbWiFiSarTxPowerSet4Limit6;         ///< Offset 2860    EwrdCdbWiFiSarTxPowerSet4Limit6
  UINT8    EwrdCdbWiFiSarTxPowerSet4Limit7;         ///< Offset 2861    EwrdCdbWiFiSarTxPowerSet4Limit7
  UINT8    EwrdCdbWiFiSarTxPowerSet4Limit8;         ///< Offset 2862    EwrdCdbWiFiSarTxPowerSet4Limit8
  UINT8    EwrdCdbWiFiSarTxPowerSet4Limit9;         ///< Offset 2863    EwrdCdbWiFiSarTxPowerSet4Limit9
  UINT8    EwrdCdbWiFiSarTxPowerSet4Limit10;        ///< Offset 2864    EwrdCdbWiFiSarTxPowerSet4Limit10
  UINT8    EwrdCdbWiFiSarTxPowerSet4Limit11;        ///< Offset 2865    EwrdCdbWiFiSarTxPowerSet4Limit11
  UINT8    EwrdCdbWiFiSarTxPowerSet4Limit12;        ///< Offset 2866    EwrdCdbWiFiSarTxPowerSet4Limit12
  UINT8    EwrdCdbWiFiSarTxPowerSet4Limit13;        ///< Offset 2867    EwrdCdbWiFiSarTxPowerSet4Limit13
  UINT8    EwrdCdbWiFiSarTxPowerSet4Limit14;        ///< Offset 2868    EwrdCdbWiFiSarTxPowerSet4Limit14
  UINT8    EwrdCdbWiFiSarTxPowerSet4Limit15;        ///< Offset 2869    EwrdCdbWiFiSarTxPowerSet4Limit15
  UINT8    EwrdCdbWiFiSarTxPowerSet4Limit16;        ///< Offset 2870    EwrdCdbWiFiSarTxPowerSet4Limit16
  UINT8    EwrdCdbWiFiSarTxPowerSet4Limit17;        ///< Offset 2871    EwrdCdbWiFiSarTxPowerSet4Limit17
  UINT8    EwrdCdbWiFiSarTxPowerSet4Limit18;        ///< Offset 2872    EwrdCdbWiFiSarTxPowerSet4Limit18
  UINT8    EwrdCdbWiFiSarTxPowerSet4Limit19;        ///< Offset 2873    EwrdCdbWiFiSarTxPowerSet4Limit19
  UINT8    EwrdCdbWiFiSarTxPowerSet4Limit20;        ///< Offset 2874    EwrdCdbWiFiSarTxPowerSet4Limit20
  UINT8    EwrdCdbWiFiSarTxPowerSet4Limit21;        ///< Offset 2875    EwrdCdbWiFiSarTxPowerSet4Limit21
  UINT8    EwrdCdbWiFiSarTxPowerSet4Limit22;        ///< Offset 2876    EwrdCdbWiFiSarTxPowerSet4Limit22
  UINT32   WifiTASSelection;                        ///< Offset 2877    WifiTASSelection
  UINT8    WifiTASListEntries;                      ///< Offset 2881    WifiTASListEntries
  UINT16   WTASBlockedListEntry1;                   ///< Offset 2882    WTASBlockedListEntry1
  UINT16   WTASBlockedListEntry2;                   ///< Offset 2884    WTASBlockedListEntry2
  UINT16   WTASBlockedListEntry3;                   ///< Offset 2886    WTASBlockedListEntry3
  UINT16   WTASBlockedListEntry4;                   ///< Offset 2888    WTASBlockedListEntry4
  UINT16   WTASBlockedListEntry5;                   ///< Offset 2890    WTASBlockedListEntry5
  UINT16   WTASBlockedListEntry6;                   ///< Offset 2892    WTASBlockedListEntry6
  UINT16   WTASBlockedListEntry7;                   ///< Offset 2894    WTASBlockedListEntry7
  UINT16   WTASBlockedListEntry8;                   ///< Offset 2896    WTASBlockedListEntry8
  UINT16   WTASBlockedListEntry9;                   ///< Offset 2898    WTASBlockedListEntry9
  UINT16   WTASBlockedListEntry10;                  ///< Offset 2900    WTASBlockedListEntry10
  UINT16   WTASBlockedListEntry11;                  ///< Offset 2902    WTASBlockedListEntry11
  UINT16   WTASBlockedListEntry12;                  ///< Offset 2904    WTASBlockedListEntry12
  UINT16   WTASBlockedListEntry13;                  ///< Offset 2906    WTASBlockedListEntry13
  UINT16   WTASBlockedListEntry14;                  ///< Offset 2908    WTASBlockedListEntry14
  UINT16   WTASBlockedListEntry15;                  ///< Offset 2910    WTASBlockedListEntry15
  UINT16   WTASBlockedListEntry16;                  ///< Offset 2912    WTASBlockedListEntry16
  UINT8    BtTileMode;                              ///< Offset 2914    BT Tile Mode
  UINT8    TileS0;                                  ///< Offset 2915    The activity of Tile in S0
  UINT8    TileS0ix;                                ///< Offset 2916    The activity of Tile in S0ix
  UINT8    TileS4;                                  ///< Offset 2917    The activity of Tile in S4
  UINT8    TileS5;                                  ///< Offset 2918    The activity of Tile in S5
  UINT8    BtLedConfig;                             ///< Offset 2919    BT LED Config
  UINT8    LedPulseDuration;                        ///< Offset 2920    LED Duration in ms
  UINT8    LedPulseInterval;                        ///< Offset 2921    LED Duration in tenths of ms
  UINT8    PmaxDevice;                              ///< Offset 2922    PMAX Device
  UINT8    PmaxAudioCodec;                          ///< Offset 2923    PMAX Audio Codec
  UINT8    PmaxWfCamera;                            ///< Offset 2924    PMAX WF Camera
  UINT8    PmaxUfCamera;                            ///< Offset 2925    PMAX UF Camera
  UINT8    PmaxFlashDevice;                         ///< Offset 2926    PMAX Flash Device
  UINT16   PcieDevOnOffDelay;                       ///< Offset 2927    PCIE Device ON/OFF Delay
  //
  // XTU SMI base address
  //
  UINT32   XtuSmiMemoryAddress;                     ///< Offset 2929    XTU SMI memory in ACPI NVS
  UINT8    WwanEnable;                              ///< Offset 2933    WWAN Enable
  UINT8    WwanFwFlashDevice;                       ///< Offset 2934    Enable WWAN Firmware Flash Device
  UINT16   WwanTOn2ResDelayMs;                      ///< Offset 2935    FCPO# to BBRST# delay time during WWAN ON
  UINT16   WwanTOnRes2PerDelayMs;                   ///< Offset 2937    BBRST# to PERST# delay time during WWAN ON
  UINT16   WwanTOnPer2PdsDelayMs;                   ///< Offset 2939    The delay between de-assertion of PERST# to change of PDS state from 0 to 1 during WWAN ON
  UINT16   WwanTPer2ResDelayMs;                     ///< Offset 2941    PERST# to BBRST# delay time during WWAN OFF
  UINT16   WwanTRes2OffDelayMs;                     ///< Offset 2943    BBRST# to FCPO# delay time during WWAN OFF
  UINT16   WwanTOffDisDelayMs;                      ///< Offset 2945    Rigorous shutdown time
  UINT16   WwanTResTogDelayMs;                      ///< Offset 2947    The delay between assertion and de-assertion BBRST# during FLDR
  UINT16   WwanTRes2PdsDelayMs;                     ///< Offset 2949    The delay between de-assertion of BBRST# and change of PDS state from 0 to 1 after FLDR
  UINT8    EnablePcieTunnelingOverUsb4;             ///< Offset 2951    Enable PCIE tunnelling support over USB4 links.
  UINT8    PreBootCmMode;                           ///< Offset 2952    USB4 CM mode information in Pre-Boot
  UINT8    CmTbtMask;                               ///< Offset 2953    Indicate enabled dTBT and iTBT for CM
  UINT8    Usb4CmSwitchEnable;                      ///< Offset 2954    USB4 CM mode switch is enabled/disabled
  UINT8    Usb4ClassOption;                         ///< Offset 2955    Indicator of USB4 Host Router Class Code for loading USB4 driver
  UINT16   WwanOemSvid;                             ///< Offset 2956    WWAN OEM SVID
  UINT16   WwanSvidTimeout;                         ///< Offset 2958    WWAN SVID Timeout
  UINT8    DeepestUSBSleepWakeCapability;           ///< Offset 2960    Deepest USB Sleep Wake Capability
  UINT32   WifiRegulatoryConfigurations;            ///< Offset 2961    WiFi Regulatory Configurations
  UINT32   WifiUartConfigurations;                  ///< Offset 2965    WiFi UART Configurations
  UINT32   WifiUnii4;                               ///< Offset 2969    WiFi UNII-4
  UINT32   WifiIndoorControl;                       ///< Offset 2973    WiFi Indoor Control
  UINT32   WifiBandSelect;                          ///< Offset 2977    WifiBandSelect
  //
  // Data Role Swap:
  //
  UINT8    UsbcDataRoleSwap;                        ///< Offset 2981    Usbc Data Role Swap
  //
  // HybridGraphics Detection
  //
  UINT8    HybridGraphicsDetection;                 ///< Offset 2982    Primary Display (0=AUTO, 3=iGfx, 4=HG)
  UINT8    WccdEnable;                              ///< Offset 2983    WCCD Enable
  UINT8    WirelessLowBandIsolation;                ///< Offset 2984    WLAN/WWAN Low Band Isolation
  UINT8    WirelessHighBandIsolation;               ///< Offset 2985    WLAN/WWAN High Band Isolation
  UINT8    WgdsWiFiSarDeltaGroupNumber;             ///< Offset 2986    WgdsWiFiSarDeltaGroupNumber
  UINT8    WgdsWiFiSarDeltaGroup4PowerMax1;         ///< Offset 2987    WgdsWiFiSarDeltaGroup4PowerMax1
  UINT8    WgdsWiFiSarDeltaGroup4PowerChainA1;      ///< Offset 2988    WgdsWiFiSarDeltaGroup4PowerChainA1
  UINT8    WgdsWiFiSarDeltaGroup4PowerChainB1;      ///< Offset 2989    WgdsWiFiSarDeltaGroup4PowerChainB1
  UINT8    WgdsWiFiSarDeltaGroup4PowerMax2;         ///< Offset 2990    WgdsWiFiSarDeltaGroup4PowerMax2
  UINT8    WgdsWiFiSarDeltaGroup4PowerChainA2;      ///< Offset 2991    WgdsWiFiSarDeltaGroup4PowerChainA2
  UINT8    WgdsWiFiSarDeltaGroup4PowerChainB2;      ///< Offset 2992    WgdsWiFiSarDeltaGroup4PowerChainB2
  UINT8    WgdsWiFiSarDeltaGroup4PowerMax3;         ///< Offset 2993    WgdsWiFiSarDeltaGroup4PowerMax3
  UINT8    WgdsWiFiSarDeltaGroup4PowerChainA3;      ///< Offset 2994    WgdsWiFiSarDeltaGroup4PowerChainA3
  UINT8    WgdsWiFiSarDeltaGroup4PowerChainB3;      ///< Offset 2995    WgdsWiFiSarDeltaGroup4PowerChainB3
  UINT8    WgdsWiFiSarDeltaGroup5PowerMax1;         ///< Offset 2996    WgdsWiFiSarDeltaGroup5PowerMax1
  UINT8    WgdsWiFiSarDeltaGroup5PowerChainA1;      ///< Offset 2997    WgdsWiFiSarDeltaGroup5PowerChainA1
  UINT8    WgdsWiFiSarDeltaGroup5PowerChainB1;      ///< Offset 2998    WgdsWiFiSarDeltaGroup5PowerChainB1
  UINT8    WgdsWiFiSarDeltaGroup5PowerMax2;         ///< Offset 2999    WgdsWiFiSarDeltaGroup5PowerMax2
  UINT8    WgdsWiFiSarDeltaGroup5PowerChainA2;      ///< Offset 3000    WgdsWiFiSarDeltaGroup5PowerChainA2
  UINT8    WgdsWiFiSarDeltaGroup5PowerChainB2;      ///< Offset 3001    WgdsWiFiSarDeltaGroup5PowerChainB2
  UINT8    WgdsWiFiSarDeltaGroup5PowerMax3;         ///< Offset 3002    WgdsWiFiSarDeltaGroup5PowerMax3
  UINT8    WgdsWiFiSarDeltaGroup5PowerChainA3;      ///< Offset 3003    WgdsWiFiSarDeltaGroup5PowerChainA3
  UINT8    WgdsWiFiSarDeltaGroup5PowerChainB3;      ///< Offset 3004    WgdsWiFiSarDeltaGroup5PowerChainB3
  UINT8    WgdsWiFiSarDeltaGroup6PowerMax1;         ///< Offset 3005    WgdsWiFiSarDeltaGroup6PowerMax1
  UINT8    WgdsWiFiSarDeltaGroup6PowerChainA1;      ///< Offset 3006    WgdsWiFiSarDeltaGroup6PowerChainA1
  UINT8    WgdsWiFiSarDeltaGroup6PowerChainB1;      ///< Offset 3007    WgdsWiFiSarDeltaGroup6PowerChainB1
  UINT8    WgdsWiFiSarDeltaGroup6PowerMax2;         ///< Offset 3008    WgdsWiFiSarDeltaGroup6PowerMax2
  UINT8    WgdsWiFiSarDeltaGroup6PowerChainA2;      ///< Offset 3009    WgdsWiFiSarDeltaGroup6PowerChainA2
  UINT8    WgdsWiFiSarDeltaGroup6PowerChainB2;      ///< Offset 3010    WgdsWiFiSarDeltaGroup6PowerChainB2
  UINT8    WgdsWiFiSarDeltaGroup6PowerMax3;         ///< Offset 3011    WgdsWiFiSarDeltaGroup6PowerMax3
  UINT8    WgdsWiFiSarDeltaGroup6PowerChainA3;      ///< Offset 3012    WgdsWiFiSarDeltaGroup6PowerChainA3
  UINT8    WgdsWiFiSarDeltaGroup6PowerChainB3;      ///< Offset 3013    WgdsWiFiSarDeltaGroup6PowerChainB3
  UINT8    WgdsWiFiSarDeltaGroup7PowerMax1;         ///< Offset 3014    WgdsWiFiSarDeltaGroup7PowerMax1
  UINT8    WgdsWiFiSarDeltaGroup7PowerChainA1;      ///< Offset 3015    WgdsWiFiSarDeltaGroup7PowerChainA1
  UINT8    WgdsWiFiSarDeltaGroup7PowerChainB1;      ///< Offset 3016    WgdsWiFiSarDeltaGroup7PowerChainB1
  UINT8    WgdsWiFiSarDeltaGroup7PowerMax2;         ///< Offset 3017    WgdsWiFiSarDeltaGroup7PowerMax2
  UINT8    WgdsWiFiSarDeltaGroup7PowerChainA2;      ///< Offset 3018    WgdsWiFiSarDeltaGroup7PowerChainA2
  UINT8    WgdsWiFiSarDeltaGroup7PowerChainB2;      ///< Offset 3019    WgdsWiFiSarDeltaGroup7PowerChainB2
  UINT8    WgdsWiFiSarDeltaGroup7PowerMax3;         ///< Offset 3020    WgdsWiFiSarDeltaGroup7PowerMax3
  UINT8    WgdsWiFiSarDeltaGroup7PowerChainA3;      ///< Offset 3021    WgdsWiFiSarDeltaGroup7PowerChainA3
  UINT8    WgdsWiFiSarDeltaGroup7PowerChainB3;      ///< Offset 3022    WgdsWiFiSarDeltaGroup7PowerChainB3
  UINT8    WgdsWiFiSarDeltaGroup8PowerMax1;         ///< Offset 3023    WgdsWiFiSarDeltaGroup8PowerMax1
  UINT8    WgdsWiFiSarDeltaGroup8PowerChainA1;      ///< Offset 3024    WgdsWiFiSarDeltaGroup8PowerChainA1
  UINT8    WgdsWiFiSarDeltaGroup8PowerChainB1;      ///< Offset 3025    WgdsWiFiSarDeltaGroup8PowerChainB1
  UINT8    WgdsWiFiSarDeltaGroup8PowerMax2;         ///< Offset 3026    WgdsWiFiSarDeltaGroup8PowerMax2
  UINT8    WgdsWiFiSarDeltaGroup8PowerChainA2;      ///< Offset 3027    WgdsWiFiSarDeltaGroup8PowerChainA2
  UINT8    WgdsWiFiSarDeltaGroup8PowerChainB2;      ///< Offset 3028    WgdsWiFiSarDeltaGroup8PowerChainB2
  UINT8    WgdsWiFiSarDeltaGroup8PowerMax3;         ///< Offset 3029    WgdsWiFiSarDeltaGroup8PowerMax3
  UINT8    WgdsWiFiSarDeltaGroup8PowerChainA3;      ///< Offset 3030    WgdsWiFiSarDeltaGroup8PowerChainA3
  UINT8    WgdsWiFiSarDeltaGroup8PowerChainB3;      ///< Offset 3031    WgdsWiFiSarDeltaGroup8PowerChainB3
  //I2C6 Support
  UINT16   SSH6;                                    ///< Offset 3032    SSCN-HIGH for I2C6
  UINT16   SSL6;                                    ///< Offset 3034    SSCN-LOW  for I2C6
  UINT16   SSD6;                                    ///< Offset 3036    SSCN-HOLD for I2C6
  UINT16   FMH6;                                    ///< Offset 3038    FMCN-HIGH for I2C6
  UINT16   FML6;                                    ///< Offset 3040    FMCN-LOW  for I2C6
  UINT16   FMD6;                                    ///< Offset 3042    FMCN-HOLD for I2C6
  UINT16   FPH6;                                    ///< Offset 3044    FPCN-HIGH for I2C6
  UINT16   FPL6;                                    ///< Offset 3046    FPCN-LOW  for I2C6
  UINT16   FPD6;                                    ///< Offset 3048    FPCN-HOLD for I2C6
  UINT16   HSH6;                                    ///< Offset 3050    HSCN-HIGH for I2C6
  UINT16   HSL6;                                    ///< Offset 3052    HSCN-LOW  for I2C6
  UINT16   HSD6;                                    ///< Offset 3054    HSCN-HOLD for I2C6
  UINT16   M0CC;                                    ///< Offset 3056    M0D3 for I2C6
  UINT16   M1CC;                                    ///< Offset 3058    M1D3 for I2C6
  //I2C7 Support
  UINT16   SSH7;                                    ///< Offset 3060    SSCN-HIGH for I2C7
  UINT16   SSL7;                                    ///< Offset 3062    SSCN-LOW  for I2C7
  UINT16   SSD7;                                    ///< Offset 3064    SSCN-HOLD for I2C7
  UINT16   FMH7;                                    ///< Offset 3066    FMCN-HIGH for I2C7
  UINT16   FML7;                                    ///< Offset 3068    FMCN-LOW  for I2C7
  UINT16   FMD7;                                    ///< Offset 3070    FMCN-HOLD for I2C7
  UINT16   FPH7;                                    ///< Offset 3072    FPCN-HIGH for I2C7
  UINT16   FPL7;                                    ///< Offset 3074    FPCN-LOW  for I2C7
  UINT16   FPD7;                                    ///< Offset 3076    FPCN-HOLD for I2C7
  UINT16   HSH7;                                    ///< Offset 3078    HSCN-HIGH for I2C7
  UINT16   HSL7;                                    ///< Offset 3080    HSCN-LOW  for I2C7
  UINT16   HSD7;                                    ///< Offset 3082    HSCN-HOLD for I2C7
  UINT16   M0CD;                                    ///< Offset 3084    M0D3 for I2C7
  UINT16   M1CD;                                    ///< Offset 3086    M1D3 for I2C7
  //Flash ID support for discrete flash solution
  UINT8    MipiCamLink0DD_FlashID;                  ///< Offset 3088    Flash ID for Link0
  UINT8    MipiCamLink1DD_FlashID;                  ///< Offset 3089    Flash ID for Link1
  UINT8    MipiCamLink2DD_FlashID;                  ///< Offset 3090    Flash ID for Link2
  UINT8    MipiCamLink3DD_FlashID;                  ///< Offset 3091    Flash ID for Link3
  UINT8    MipiCamLink4DD_FlashID;                  ///< Offset 3092    Flash ID for Link4
  UINT8    MipiCamLink5DD_FlashID;                  ///< Offset 3093    Flash ID for Link5
  UINT16   MipiCamCtrlLogic0_GpioComNumber[6];      ///< Offset 3094    GPIO com number
  UINT16   MipiCamCtrlLogic1_GpioComNumber[6];      ///< Offset 3106    GPIO com number
  UINT16   MipiCamCtrlLogic2_GpioComNumber[6];      ///< Offset 3118    GPIO com number
  UINT16   MipiCamCtrlLogic3_GpioComNumber[6];      ///< Offset 3130    GPIO com number
  UINT16   MipiCamCtrlLogic4_GpioComNumber[6];      ///< Offset 3142    GPIO com number
  UINT16   MipiCamCtrlLogic5_GpioComNumber[6];      ///< Offset 3154    GPIO com number
  UINT16   MipiCamFlash0GpioComNumber;              ///< Offset 3166    GPIO Com Number
  UINT16   MipiCamFlash1GpioComNumber;              ///< Offset 3168    GPIO Com Number
  UINT16   MipiCamFlash2GpioComNumber;              ///< Offset 3170    GPIO Com Number
  UINT16   MipiCamFlash3GpioComNumber;              ///< Offset 3172    GPIO Com Number
  UINT16   MipiCamFlash4GpioComNumber;              ///< Offset 3174    GPIO Com Number
  UINT16   MipiCamFlash5GpioComNumber;              ///< Offset 3176    GPIO Com Number
  // Type-C NVS variables for TCSS Port 7-10
  UINT8    UsbTypeCPort7;                           ///< Offset 3178    Type C Connector 7  Port mapping within the controller the port exposed
  UINT8    UsbTypeCPort7Pch;                        ///< Offset 3179    Type C Connector 7  Port mapping within the PCH controller (If Split mode supported)
  UINT8    UsbCPort7Proterties;                     ///< Offset 3180    Type C Connector 7  Portperties Split Support/Controller(PCH/TBT/CPU)/Root port (vaild for TBT)
  UINT8    UsbTypeCPort8;                           ///< Offset 3181    Type C Connector 8  Port mapping within the controller the port exposed
  UINT8    UsbTypeCPort8Pch;                        ///< Offset 3182    Type C Connector 8  Port mapping within the PCH controller (If Split mode supported)
  UINT8    UsbCPort8Proterties;                     ///< Offset 3183    Type C Connector 8  Portperties Split Support/Controller(PCH/TBT/CPU)/Root port (vaild for TBT)
  UINT8    UsbTypeCPort9;                           ///< Offset 3184    Type C Connector 9  Port mapping within the controller the port exposed
  UINT8    UsbTypeCPort9Pch;                        ///< Offset 3185    Type C Connector 9  Port mapping within the PCH controller (If Split mode supported)
  UINT8    UsbCPort9Proterties;                     ///< Offset 3186    Type C Connector 9  Portperties Split Support/Controller(PCH/TBT/CPU)/Root port (vaild for TBT)
  UINT8    UsbTypeCPortA;                           ///< Offset 3187    Type C Connector A  Port mapping within the controller the port exposed
  UINT8    UsbTypeCPortAPch;                        ///< Offset 3188    Type C Connector A  Port mapping within the PCH controller (If Split mode supported)
  UINT8    UsbCPortAProterties;                     ///< Offset 3189    Type C Connector A  Portperties Split Support/Controller(PCH/TBT/CPU)/Root port (vaild for TBT)
  // UCSI/UCMX Driver Support. 0: Force Disable, 1: UCSI Driver support, 2: UCMX Driver support.
  UINT8    UsbCPort1UcxxSupport;                    ///< Offset 3190    Type C Connector 1  UCSI/UCMX Driver Support Enable/ Force Disable.
  UINT8    UsbCPort2UcxxSupport;                    ///< Offset 3191    Type C Connector 2  UCSI/UCMX Driver Support Enable/ Force Disable.
  UINT8    UsbCPort3UcxxSupport;                    ///< Offset 3192    Type C Connector 3  UCSI/UCMX Driver Support Enable/ Force Disable.
  UINT8    UsbCPort4UcxxSupport;                    ///< Offset 3193    Type C Connector 4  UCSI/UCMX Driver Support Enable/ Force Disable.
  UINT8    UsbCPort5UcxxSupport;                    ///< Offset 3194    Type C Connector 5  UCSI/UCMX Driver Support Enable/ Force Disable.
  UINT8    UsbCPort6UcxxSupport;                    ///< Offset 3195    Type C Connector 6  UCSI/UCMX Driver Support Enable/ Force Disable.
  UINT8    UsbCPort7UcxxSupport;                    ///< Offset 3196    Type C Connector 7  UCSI/UCMX Driver Support Enable/ Force Disable.
  UINT8    UsbCPort8UcxxSupport;                    ///< Offset 3197    Type C Connector 8  UCSI/UCMX Driver Support Enable/ Force Disable.
  UINT8    UsbCPort9UcxxSupport;                    ///< Offset 3198    Type C Connector 9  UCSI/UCMX Driver Support Enable/ Force Disable.
  UINT8    UsbCPortAUcxxSupport;                    ///< Offset 3199    Type C Connector A  UCSI/UCMX Driver Support Enable/ Force Disable.
  UINT64   DgBaseAddress;                           ///< Offset 3200    DG PCIe base address
  UINT32   DgOpRegionAddress;                       ///< Offset 3208    DG OpRegion base address
  UINT8    DgBrightnessPercentage;                  ///< Offset 3212    DG eDP Brightness Level Percentage
  UINT8    SkipVidDidCheck;                         ///< Offset 3213    Skip VDID check
  // ACPI debug NVS region
  UINT8    EnableAcpiDebug;                         ///< Offset 3214    Enable flag for ACPI debug
  UINT8    SerialPortAcpiDebug;                     ///< Offset 3215    Serial Port ACPI debug
  UINT32   WifiEnergyDetection;                     ///< Offset 3216    Wifi Energy Detection Threshold
  UINT32   Wifi7Control;                            ///< Offset 3220    Wi-Fi 7 Control
  UINT32   DisplayMuxGpioNo;                        ///< Offset 3224    Display Mux GPIO pin
  UINT8    DgNumberOfValidDeviceId;                 ///< Offset 3228    DG Number of Valid Device IDs
  UINT32   DgDeviceId1;                             ///< Offset 3229    DG Device ID 1
  UINT32   DgDeviceId2;                             ///< Offset 3233    DG Device ID 2
  UINT32   DgDeviceId3;                             ///< Offset 3237    DG Device ID 3
  UINT32   DgDeviceId4;                             ///< Offset 3241    DG Device ID 4
  UINT32   DgDeviceId5;                             ///< Offset 3245    DG Device ID 5
  UINT32   DgDeviceId6;                             ///< Offset 3249    DG Device ID 6
  UINT32   DgDeviceId7;                             ///< Offset 3253    DG Device ID 7
  UINT32   DgDeviceId8;                             ///< Offset 3257    DG Device ID 8
  UINT32   DgDeviceId9;                             ///< Offset 3261    DG Device ID 9
  UINT32   DgDeviceId10;                            ///< Offset 3265    DG Device ID 10
  UINT32   DgDeviceId11;                            ///< Offset 3269    DG Device ID 11
  UINT32   DgDeviceId12;                            ///< Offset 3273    DG Device ID 12
  UINT32   DgDeviceId13;                            ///< Offset 3277    DG Device ID 13
  UINT32   DgDeviceId14;                            ///< Offset 3281    DG Device ID 14
  UINT32   DgDeviceId15;                            ///< Offset 3285    DG Device ID 15
  UINT32   DgDeviceIdX;                             ///< Offset 3289    DG Device ID for eDP device
  UINT8    DgDisplaySupportFlag;                    ///< Offset 3293    _DOS DG Display Support Flag.
  UINT8    CnvDlvrRfim;                             ///< Offset 3294    CNVi RFI Mitigation
  UINT8    ClosedLidWovLighting;                    ///< Offset 3295    Closed Lid WoV LED Lighting Support Enable
  UINT8    ClwlI2cController;                       ///< Offset 3296    Closed Lid WoV LED Lighting I2C Controller Number
  UINT8    ClwlI2cSlaveAddress;                     ///< Offset 3297    Closed Lid WoV LED Lighting I2C Slave address
  UINT32   DiscreteWiFiRfKillGpio;                  ///< Offset 3298    Gpio for WiFi RF-Kill
  UINT32   WlanRstGpio;                             ///< Offset 3302    WLAN Reset Gpio pin
  UINT32   WlanVsecHeaderOffset;                    ///< Offset 3306    Wlan VSEC Control Reg Base Address
  UINT32   WlanDeviceCapRegOffset;                  ///< Offset 3310    Wlan Device Capabilities Reg Base Address
  UINT8    SocBusBase;                              ///< Offset 3314    SoC Bus Base
  UINT8    SocBusLimit;                             ///< Offset 3315    SoC Bus Limit
  UINT16   SocIoBase;                               ///< Offset 3316    SoC IO Base
  UINT16   SocIoLimit;                              ///< Offset 3318    SoC IO Limit
  UINT32   SocMem32Base;                            ///< Offset 3320    SoC Mem32 Base
  UINT32   SocMem32Limit;                           ///< Offset 3324    SoC Mem32 Limit
  UINT64   SocMem64Base;                            ///< Offset 3328    SoC Mem64 Base
  UINT64   SocMem64Limit;                           ///< Offset 3336    SoC Mem64 Limit
  UINT8    PchBusBase;                              ///< Offset 3344    PCH Bus Base
  UINT8    PchBusLimit;                             ///< Offset 3345    PCH Bus Limit
  UINT16   PchIoBase;                               ///< Offset 3346    PCH IO Base
  UINT16   PchIoLimit;                              ///< Offset 3348    PCH IO Limit
  UINT32   PchMem32Base;                            ///< Offset 3350    PCH Mem32 Base
  UINT32   PchMem32Limit;                           ///< Offset 3354    PCH Mem32 Limit
  UINT64   PchMem64Base;                            ///< Offset 3358    PCH Mem64 Base
  UINT64   PchMem64Limit;                           ///< Offset 3366    PCH Mem64 Limit
  // HD Audio
  UINT8    I2SE;                                    ///< Offset 3374    HD Audio I2S Enable
  // SIO Configuration Registers
  UINT8    PcdIT8659COM;                            ///< Offset 3375    IT8659 COM
  UINT8    PcdIT8659HWMON;                          ///< Offset 3376    IT8659 HWMON
  UINT8    PcdIT8659SIO;                            ///< Offset 3377    IT8659 SIO Present
  UINT8    SdevXhciEntry;                           ///< Offset 3378
  //ZPODD support
  UINT32   ZpoddDAGpio;                             ///< Offset 3379    ZPODD device attention gpio
  UINT32   ZpoddPRGpio;                             ///< Offset 3383    ZPODD device present gpio
  UINT32   ZpoddPWGpio;                             ///< Offset 3387    ZPODD device power gpio
  UINT8    ZpoddPWGpioPolarity;                     ///< Offset 3391    ZPODD device power gpio polarity
  UINT8    ZpoddPortBitmask;                        ///< Offset 3392    Bitmask of port support zpodd
  UINT8    IsArlMobile;                             ///< Offset 3393    ARL Mobile Presented
  UINT8    IsArlDesktop;                            ///< Offset 3394    ARL Desktop Presented
  //Foxville RTD3 support
  UINT32   FoxLanWakeGpio;                          ///< Offset 3395    Foxville I225 Wake Gpio pin
  UINT32   FoxLanRstGpio;                           ///< Offset 3399    Foxville I225 Reset Gpio pin
  UINT8    FoxLanRstGpioPolarity;                   ///< Offset 3403    Foxville I225 Reset Gpio pin polarity
  UINT32   FoxLanDisableNGpio;                      ///< Offset 3404    Foxville I225 Disable N Gpio pin
  UINT8    FoxLanDisableNGpioPolarity;              ///< Offset 3408    Foxville I225 Disable N Gpio pin polarity
  UINT8    FoxLanSupport;                           ///< Offset 3409    Foxville I225 support configuration
  UINT8    FoxLanRpNumber;                          ///< Offset 3410    Foxville I225 PCIe Root Port Number
  UINT8    CnviBtAudioOffload;                      ///< Offset 3411    CNVi BT Audio Offload
  UINT32   I2c0SpeedMode;                           ///< Offset 3412    I2C0 Speed Mode Selection
  UINT32   I2c1SpeedMode;                           ///< Offset 3416    I2C1 Speed Mode Selection
  UINT32   I2c2SpeedMode;                           ///< Offset 3420    I2C2 Speed Mode Selection
  UINT32   I2c3SpeedMode;                           ///< Offset 3424    I2C3 Speed Mode Selection
  UINT32   I2c4SpeedMode;                           ///< Offset 3428    I2C4 Speed Mode Selection
  UINT32   I2c5SpeedMode;                           ///< Offset 3432    I2C5 Speed Mode Selection
  UINT8    EnFanAutoMode;                           ///< Offset 3436    Enabling Auto Mode for SIO FAN
  UINT8    MipiCamLink0DD_PprValue;                 ///< Offset 3437    PPR Value
  UINT8    MipiCamLink0DD_PprUnit;                  ///< Offset 3438    PPR Unit
  UINT8    MipiCamLink1DD_PprValue;                 ///< Offset 3439    PPR Value
  UINT8    MipiCamLink1DD_PprUnit;                  ///< Offset 3440    PPR Unit
  UINT8    MipiCamLink2DD_PprValue;                 ///< Offset 3441    PPR Value
  UINT8    MipiCamLink2DD_PprUnit;                  ///< Offset 3442    PPR Unit
  UINT8    MipiCamLink3DD_PprUnit;                  ///< Offset 3443    PPR Unit
  UINT8    MipiCamLink3DD_PprValue;                 ///< Offset 3444    PPR Value
  UINT8    MipiCamLink4DD_PprUnit;                  ///< Offset 3445    PPR Unit
  UINT8    MipiCamLink4DD_PprValue;                 ///< Offset 3446    PPR Value
  UINT8    MipiCamLink5DD_PprUnit;                  ///< Offset 3447    PPR Unit
  UINT8    MipiCamLink5DD_PprValue;                 ///< Offset 3448    PPR Value
  //Audio Custom HID Link0
  UINT8    AudioLink0UserHid[9];                    ///< Offset 3449    User defined HID ASCII character 0
                                                    ///< Offset 3457    User defined HID ASCII character 8
  //Audio Custom HID Link1
  UINT8    AudioLink1UserHid[9];                    ///< Offset 3458    User defined HID ASCII character 0
                                                    ///< Offset 3466    User defined HID ASCII character 8
  UINT8    SCS0;                                    ///< Offset 3467    SPI0 ChipSelect 0 Device enabled
  UINT8    SCS1;                                    ///< Offset 3468    SPI0 ChipSelect 1 Device enabled
  UINT8    SCS2;                                    ///< Offset 3469    SPI1 ChipSelect 0 Device enabled
  UINT8    SCS3;                                    ///< Offset 3470    SPI1 ChipSelect 1 Device enabled
  UINT8    SCS4;                                    ///< Offset 3471    SPI2 ChipSelect 0 Device enabled
  UINT8    SCS5;                                    ///< Offset 3472    SPI2 ChipSelect 1 Device enabled
  // CNV GUID lock
  UINT8    CnvGuidLockStatus;                       ///< Offset 3473    CNV Guid Lock Status ACPI Indicator
  UINT8    EVSA;                                    ///< Offset 3474    Everest8326 I2c slave address
} PLATFORM_NVS_AREA;

#pragma pack(pop)
#endif
