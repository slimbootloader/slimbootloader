

/** @file
  ACPI Platform NVS Data

  Copyright (c) 2022, Intel Corporation. All rights reserved.<BR>
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
  UINT8    PcdIT8659COM;                            ///< Offset 42      IT8659  COM
  UINT8    PcdIT8659HWMON;                          ///< Offset 43      IT8659  HWMON
  UINT8    PcdIT8659SIO;                            ///< Offset 44      IT8659  SIO Present
  //
  // Extended Mobile Access Values
  //
  UINT8    EmaEnable;                               ///< Offset 45      EMA Enable
  UINT16   EmaPointer;                              ///< Offset 46      EMA Pointer
  UINT16   EmaLength;                               ///< Offset 48      EMA Length
  //
  // MEF Registers:
  //
  UINT8    MefEnable;                               ///< Offset 50      MEF Enable
  //
  // PCIe Dock Status:
  //
  UINT8    PcieDockStatus;                          ///< Offset 51      PCIe Dock Status
  //
  // TPM Registers
  //
  UINT8    MorData;                                 ///< Offset 52      Memory Overwrite Request Data
  UINT8    TcgParamter;                             ///< Offset 53      Used for save the Mor and/or physical presence paramter
  UINT32   PPResponse;                              ///< Offset 54      Physical Presence request operation response
  UINT8    PPRequest;                               ///< Offset 58      Physical Presence request operation
  UINT8    LastPPRequest;                           ///< Offset 59      Last Physical Presence request operation
  //
  // SATA Registers:
  //
  UINT8    IdeMode;                                 ///< Offset 60      IDE Mode (Compatible\Enhanced)
  //
  // Board Id
  //
  UINT8    PlatformId;                              ///< Offset 61      Platform id
  UINT8    BoardType;                               ///< Offset 62      Board Type
  //
  // PCIe Hot Plug
  //
  UINT8    PcieOSCControl;                          ///< Offset 63      PCIE OSC Control
  UINT8    NativePCIESupport;                       ///< Offset 64      Native PCIE Setup Value
  //
  // USB Sideband Deferring Support
  //
  UINT8    HostAlertVector1;                        ///< Offset 65      USB Sideband Deferring GPE Vector (HOST_ALERT#1)
  UINT8    HostAlertVector2;                        ///< Offset 66      USB Sideband Deferring GPE Vector (HOST_ALERT#2)
  //
  // Embedded Controller Availability Flag.
  //
  UINT8    EcAvailable;                             ///< Offset 67      Embedded Controller Availability Flag.
  //
  // Global Variables
  //
  UINT8    DisplaySupportFlag;                      ///< Offset 68      _DOS Display Support Flag.
  UINT8    InterruptModeFlag;                       ///< Offset 69      Global IOAPIC/8259 Interrupt Mode Flag.
  UINT8    CoolingTypeFlag;                         ///< Offset 70      Global Cooling Type Flag.
  UINT8    L01Counter;                              ///< Offset 71      Global L01 Counter.
  UINT8    VirtualFan0Status;                       ///< Offset 72      Virtual Fan0 Status.
  UINT8    VirtualFan1Status;                       ///< Offset 73      Virtual Fan1 Status.
  UINT8    VirtualFan2Status;                       ///< Offset 74      Virtual Fan2 Status.
  UINT8    VirtualFan3Status;                       ///< Offset 75      Virtual Fan3 Status.
  UINT8    VirtualFan4Status;                       ///< Offset 76      Virtual Fan4 Status.
  UINT8    VirtualFan5Status;                       ///< Offset 77      Virtual Fan5 Status.
  UINT8    VirtualFan6Status;                       ///< Offset 78      Virtual Fan6 Status.
  UINT8    VirtualFan7Status;                       ///< Offset 79      Virtual Fan7 Status.
  UINT8    VirtualFan8Status;                       ///< Offset 80      Virtual Fan8 Status.
  UINT8    VirtualFan9Status;                       ///< Offset 81      Virtual Fan9 Status.
  //
  // Thermal
  //
  UINT32   PlatformCpuId;                           ///< Offset 82      CPUID Feature Information [EAX]
  UINT32   TBARB;                                   ///< Offset 86      Reserved for Thermal Base Low Address for BIOS
  UINT32   TBARBH;                                  ///< Offset 90      Reserved for Thermal Base High Address for BIOS
  UINT8    TsOnDimmEnabled;                         ///< Offset 94      TS-on-DIMM is chosen in SETUP and present on the DIMM
  //
  // Board info
  //
  UINT8    PlatformFlavor;                          ///< Offset 95      Platform Flavor
  UINT16   BoardRev;                                ///< Offset 96      Board Rev
  //
  // Package temperature
  //
  UINT8    PeciAccessMethod;                        ///< Offset 98      Peci Access Method
  UINT8    Ac0FanSpeed;                             ///< Offset 99      _AC0 Fan Speed
  UINT8    Ac1FanSpeed;                             ///< Offset 100     _AC1 Fan Speed
  //
  // XTU 3.0 Specification
  //
  UINT32   XTUBaseAddress;                          ///< Offset 101     XTU Continous structure Base Address
  UINT32   XTUSize;                                 ///< Offset 105     XMP Size
  UINT32   XMPBaseAddress;                          ///< Offset 109     XMP Base Address
  UINT8    DDRReferenceFreq;                        ///< Offset 113     DDR Reference Frequency
  UINT8    Rtd3Support;                             ///< Offset 114     Runtime D3 support.
  UINT8    Rtd3P0dl;                                ///< Offset 115     User selctable Delay for Device D0 transition.
  UINT8    Rtd3P3dl;                                ///< Offset 116     User selctable Delay for Device D3 transition.
  //
  // Intel(R) Dynamic Tuning Technology Devices and trip points
  //
  UINT8    EnableDptf;                              ///< Offset 117     EnableDptf
  UINT16   EnableDCFG;                              ///< Offset 118     EnableDCFG
  UINT8    EnableSaDevice;                          ///< Offset 120     EnableSaDevice
  //
  // DPPM Devices and trip points
  //
  UINT8    EnableFan1Device;                        ///< Offset 121     EnableFan1Device
  UINT8    EnableAmbientDevice;                     ///< Offset 122     EnableAmbientDevice
  UINT8    ActiveThermalTripPointAmbient;           ///< Offset 123     ActiveThermalTripPointAmbient
  UINT8    PassiveThermalTripPointAmbient;          ///< Offset 124     PassiveThermalTripPointAmbient
  UINT8    CriticalThermalTripPointAmbient;         ///< Offset 125     CriticalThermalTripPointAmbient
  UINT8    HotThermalTripPointAmbient;              ///< Offset 126     HotThermalTripPointAmbient
  UINT8    EnableSkinDevice;                        ///< Offset 127     EnableSkinDevice
  UINT8    ActiveThermalTripPointSkin;              ///< Offset 128     ActiveThermalTripPointSkin
  UINT8    PassiveThermalTripPointSkin;             ///< Offset 129     PassiveThermalTripPointSkin
  UINT8    CriticalThermalTripPointSkin;            ///< Offset 130     CriticalThermalTripPointSkin
  UINT8    HotThermalTripPointSkin;                 ///< Offset 131     HotThermalTripPointSkin
  UINT8    EnableExhaustFanDevice;                  ///< Offset 132     EnableExhaustFanDevice
  UINT8    ActiveThermalTripPointExhaustFan;        ///< Offset 133     ActiveThermalTripPointExhaustFan
  UINT8    PassiveThermalTripPointExhaustFan;       ///< Offset 134     PassiveThermalTripPointExhaustFan
  UINT8    CriticalThermalTripPointExhaustFan;      ///< Offset 135     CriticalThermalTripPointExhaustFan
  UINT8    HotThermalTripPointExhaustFan;           ///< Offset 136     HotThermalTripPointExhaustFan
  UINT8    EnableVRDevice;                          ///< Offset 137     EnableVRDevice
  UINT8    ActiveThermalTripPointVR;                ///< Offset 138     ActiveThermalTripPointVR
  UINT8    PassiveThermalTripPointVR;               ///< Offset 139     PassiveThermalTripPointVR
  UINT8    CriticalThermalTripPointVR;              ///< Offset 140     CriticalThermalTripPointVR
  UINT8    HotThermalTripPointVR;                   ///< Offset 141     HotThermalTripPointVR
  //
  // DPPM Policies
  //
  UINT8    EnableActivePolicy;                      ///< Offset 142     EnableActivePolicy @deprecated. Intel(R) Dynamic Tuning can dynamically enable/disable policies
  UINT8    EnablePassivePolicy;                     ///< Offset 143     EnablePassivePolicy @deprecated. Intel(R) Dynamic Tuning can dynamically enable/disable policies
  UINT8    EnableCriticalPolicy;                    ///< Offset 144     EnableCriticalPolicy @deprecated. Intel(R) Dynamic Tuning can dynamically enable/disable policies
  UINT8    EnablePIDPolicy;                         ///< Offset 145     EnablePIDPolicy @deprecated. Intel(R) Dynamic Tuning can dynamically enable/disable policies
  //
  // Miscellaneous Intel(R) Dynamic Tuning Technology
  //
  UINT32   PpccStepSize;                            ///< Offset 146     PPCC Step Size
  UINT8    Reserved0[6];                            ///< Offset 150:155
  //
  // Comms Hub
  //
  UINT8    CommsHubEnable;                          ///< Offset 156     Comms Hub Enable/Disable
  UINT8    LowPowerS0Idle;                          ///< Offset 157     Low Power S0 Idle Enable
  //
  // BIOS only version of Config TDP
  //
  UINT8    ConfigTdpBios;                           ///< Offset 158     enable/disable BIOS only version of Config TDP
  UINT8    DockSmi;                                 ///< Offset 159     Dock SMI number
  //
  // LPC SIO configuration
  //
  UINT16   LpcSioPort1;                             ///< Offset 160     SIO config port 1
  UINT16   LpcSioPort2;                             ///< Offset 162     SIO config port 2
  UINT16   LpcSioPmeBar;                            ///< Offset 164     SIO PME Base Address
  UINT8    Reserved1[333];                          ///< Offset 166:498
  UINT8    PseudoG3State;                           ///< Offset 499     Pseudo G3 Enable / Disable
  UINT8    EnablePowerParticipant;                  ///< Offset 500     EnablePowerParticipant
  UINT16   DPTFRsvd0;                               ///< Offset 501     DPTFRsvd0
  UINT16   PowerParticipantPollingRate;             ///< Offset 503     PowerParticipantPollingRate
  UINT8    EnablePowerBossPolicy;                   ///< Offset 505     EnablePowerBossPolicy @deprecated. Intel(R) Dynamic Tuning can dynamically enable/disable policies
  UINT8    EnableVSPolicy;                          ///< Offset 506     EnableVSPolicy @deprecated. Intel(R) Dynamic Tuning can dynamically enable/disable policies
  UINT8    EnableRFIMPolicy;                        ///< Offset 507     RFI Mitigation @deprecated. Intel(R) Dynamic Tuning can dynamically enable/disable policies
  UINT8    Reserved2[7];                            ///< Offset 508:514
  UINT8    UsbPowerResourceTest;                    ///< Offset 515     RTD3 USB Power Resource config
  UINT8    Rtd3I2C0SensorHub;                       ///< Offset 516     RTD3 support for I2C0 SH
  UINT8    VirtualGpioButtonSxBitmask;              ///< Offset 517     Virtual GPIO button Notify Sleep State Change
  UINT8    IuerButtonEnable;                        ///< Offset 518     IUER Button Enable
  UINT8    IuerConvertibleEnable;                   ///< Offset 519     IUER Convertible Enable
  UINT8    IuerDockEnable;                          ///< Offset 520     IUER Dock Enable
  UINT8    CSNotifyEC;                              ///< Offset 521     EC Notification of Low Power S0 Idle State
  UINT16   Rtd3AudioDelay;                          ///< Offset 522     RTD3 Audio Codec device delay
  UINT16   Rtd3TouchPadDelay;                       ///< Offset 524     RTD3 TouchPad delay time after applying power to device
  UINT16   Rtd3TouchPanelDelay;                     ///< Offset 526     RTD3 TouchPanel delay time after applying power to device
  UINT16   Rtd3SensorHub;                           ///< Offset 528     RTD3 SensorHub delay time after applying power to device
  UINT16   VRRampUpDelay;                           ///< Offset 530     VR Ramp up delay
  UINT8    PstateCapping;                           ///< Offset 532     P-state Capping
  UINT16   Rtd3I2C0ControllerPS0Delay;              ///< Offset 533     Delay in _PS0 after powering up I2C0 Controller
  UINT16   Rtd3I2C1ControllerPS0Delay;              ///< Offset 535     Delay in _PS0 after powering up I2C1 Controller
  UINT16   Rtd3Config0;                             ///< Offset 537     RTD3 Config Setting0(BIT0:ZPODD, BIT1:Reserved, BIT2:PCIe NVMe, BIT3:BT, BIT4:SKL SDS SIP I2C Touch, BIT6:Card Reader, BIT7:WWAN)
  UINT16   Rtd3Config1;                             ///< Offset 539     RTD3 Config Setting1(BIT0:Sata Port0, BIT1:Sata Port1, BIT2:Sata Port2, BIT3:Sata Port3)
  UINT8    Ps2MouseEnable;                          ///< Offset 541     Ps2 Mouse Enable
  UINT8    Ps2KbMsEnable;                           ///< Offset 542     Ps2 Keyboard and Mouse Enable
  UINT16   SSH0;                                    ///< Offset 543     SSCN-HIGH for I2C0
  UINT16   SSL0;                                    ///< Offset 545     SSCN-LOW  for I2C0
  UINT16   SSD0;                                    ///< Offset 547     SSCN-HOLD for I2C0
  UINT16   FMH0;                                    ///< Offset 549     FMCN-HIGH for I2C0
  UINT16   FML0;                                    ///< Offset 551     FMCN-LOW  for I2C0
  UINT16   FMD0;                                    ///< Offset 553     FMCN-HOLD for I2C0
  UINT16   FPH0;                                    ///< Offset 555     FPCN-HIGH for I2C0
  UINT16   FPL0;                                    ///< Offset 557     FPCN-LOW  for I2C0
  UINT16   FPD0;                                    ///< Offset 559     FPCN-HOLD for I2C0
  UINT16   HSH0;                                    ///< Offset 561     HSCN-HIGH for I2C0
  UINT16   HSL0;                                    ///< Offset 563     HSCN-LOW  for I2C0
  UINT16   HSD0;                                    ///< Offset 565     HSCN-HOLD for I2C0
  UINT8    Reserved3[6];                            ///< Offset 567:572
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
  UINT8    Reserved4[1];                            ///< Offset 597:597
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
  UINT8    Reserved5[1];                            ///< Offset 622:622
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
  UINT8    Reserved6[1];                            ///< Offset 647:647
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
  UINT8    Reserved7[1];                            ///< Offset 672:672
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
  UINT8    Reserved8[1];                            ///< Offset 697:697
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
  UINT8    Reserved9[1];                            ///< Offset 734:734
  UINT16   M0C9;                                    ///< Offset 735     M0D3 for UART0
  UINT16   M1C9;                                    ///< Offset 737     M1D3 for UART0
  UINT16   M0CA;                                    ///< Offset 739     M0D3 for UART1
  UINT16   M1CA;                                    ///< Offset 741     M1D3 for UART1
  UINT16   M0CB;                                    ///< Offset 743     M0D3 for UART2
  UINT16   M1CB;                                    ///< Offset 745     M1D3 for UART2
  UINT8    Reserved10[1];                           ///< Offset 747:747
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
  UINT8    Reserved11[6];                           ///< Offset 772:777
  UINT64   LowPowerS0IdleConstraint;                ///< Offset 778     PEP Constraints
  // Bit[1:0] - Storage (0:None, 1:Storage Controller, 2:Raid)
  // Bit[2]   - En/Dis UART0
  // Bit[3]   - En/Dis UART1
  // Bit[4]   - En/Dis I2C0
  // Bit[5]   - En/Dis I2C1
  // Bit[6]   - En/Dis XHCI
  // Bit[8:7]  - HD Audio (includes ADSP) (0: No Constraints or 1: D0/F1 or 3:D3)
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
  // Bit[24]  - En/Dis PEG0
  // Bit[25]  - En/Dis THC0
  // Bit[26]  - En/Dis THC1
  // Bit[27]  - Reserved
  // Bit[28]  - En/Dis I2C6
  // Bit[29]  - En/Dis TCSS IPs/DTBT
  // Bit[30]  - En/Dis GNA
  // Bit[31]  - En/Dis VMD0
  // Bit[32]  - En/Dis HECI3
  // Bit[34:33]  - PCIe Storage RP(0: No Constraints or 1: D0/F1 or 3:D3)
  // Bit[36:35] - Pcie Lan (0:No Constraint or 1:D0/F1 or 3:D3)
  // Bit[38:37] - Pcie Wlan (0:No Constraint or 1:D0/F1 or 3:D3)
  // Bit[40:39] - Pcie Gfx (0:No Constraint or 1:D0/F1 or 3:D3)
  // Bit[42:41] - Pcie Other (0:No Constraint or 1:D0/F1 or 3:D3)
  // Bit[43]  - En/Dis DG on x8 PEG port (PEG1)
  // Bit[44]  - En/Dis UFS0
  // Bit[45]  - En/Dis UFS1
  // Bit[46]  - En/Dis I2C7
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
  UINT8    Reserved12[10];                          ///< Offset 790:799
  UINT32   AuxPowerLimit;                           ///< Offset 800     Maximum aux power available for PCIe root ports
  //
  // Intel(R) Dynamic Tuning Technology
  //
  UINT8    Reserved13[3];                           ///< Offset 804:806
  UINT8    EnableChargerParticipant;                ///< Offset 807     EnableChargerParticipant
  UINT8    CriticalThermalTripPointAmbientS3;       ///< Offset 808     CriticalThermalTripPointAmbientS3
  UINT8    CriticalThermalTripPointSkinS3;          ///< Offset 809     CriticalThermalTripPointSkinS3
  UINT8    CriticalThermalTripPointExhaustFanS3;    ///< Offset 810     CriticalThermalTripPointExhaustFanS3
  UINT8    CriticalThermalTripPointVrS3;            ///< Offset 811     CriticalThermalTripPointVRS3
  UINT8    CriticalThermalTripPointInletFanS3;      ///< Offset 812     CriticalThermalTripPointInletFanS3
  UINT8    CriticalThermalTripPointWGigS3;          ///< Offset 813     CriticalThermalTripPointWGigS3
  UINT8    EnumerateSataPortConstraints;            ///< Offset 814     Set to indicate to PEP that constraints at SATA ports should be enumerated
  //
  // Intel(R) Dynamic Tuning Technology
  //
  UINT8    EnableBatteryParticipant;                ///< Offset 815     EnableBatteryParticipant
  UINT8    EnableInt3400Device;                     ///< Offset 816     EnableInt3400Device.
  UINT8    EcLowPowerMode;                          ///< Offset 817     EC Low Power Mode: 1 - Enabled, 0 - Disabled
  UINT8    ThermalSamplingPeriodTMEM;               ///< Offset 818     ThermalSamplingPeriodTMEM @deprecated. Memory Participant is not POR for Intel(R) Dynamic Tuning Technology
  UINT8    EnableSen1Participant;                   ///< Offset 819     EnableSen1Participant
  UINT8    EnableSen2Participant;                   ///< Offset 820     EnableSen2Participant
  UINT8    EnableSen3Participant;                   ///< Offset 821     EnableSen3Participant
  UINT8    EnableSen4Participant;                   ///< Offset 822     EnableSen4Participant
  UINT8    EnableSen5Participant;                   ///< Offset 823     EnableSen5Participant
  UINT8    CriticalThermalTripPointSen1S3;          ///< Offset 824     CriticalThermalTripPointSen1S3
  UINT8    CriticalThermalTripPointSen2S3;          ///< Offset 825     CriticalThermalTripPointSen2S3
  UINT8    CriticalThermalTripPointSen3S3;          ///< Offset 826     CriticalThermalTripPointSen3S3
  UINT8    CriticalThermalTripPointSen4S3;          ///< Offset 827     CriticalThermalTripPointSen4S3
  UINT8    CriticalThermalTripPointSen5S3;          ///< Offset 828     CriticalThermalTripPointSen5S3
  UINT8    PowerSharingManagerEnable;               ///< Offset 829     PowerSharingManagerEnable
  UINT8    PsmSplcDomainType1;                      ///< Offset 830     PsmSplcDomainType1
  UINT32   PsmSplcPowerLimit1;                      ///< Offset 831     PsmSplcPowerLimit1
  UINT32   PsmSplcTimeWindow1;                      ///< Offset 835     PsmSplcTimeWindow1
  UINT8    PsmDplcDomainType1;                      ///< Offset 839     PsmDplcDomainType1
  UINT8    PsmDplcDomainPreference1;                ///< Offset 840     PsmDplcDomainPreference1
  UINT16   PsmDplcPowerLimitIndex1;                 ///< Offset 841     PsmDplcPowerLimitIndex1
  UINT16   PsmDplcDefaultPowerLimit1;               ///< Offset 843     PsmDplcDefaultPowerLimit1
  UINT32   PsmDplcDefaultTimeWindow1;               ///< Offset 845     PsmDplcDefaultTimeWindow1
  UINT16   PsmDplcMinimumPowerLimit1;               ///< Offset 849     PsmDplcMinimumPowerLimit1
  UINT16   PsmDplcMaximumPowerLimit1;               ///< Offset 851     PsmDplcMaximumPowerLimit1
  UINT16   PsmDplcMaximumTimeWindow1;               ///< Offset 853     PsmDplcMaximumTimeWindow1
  UINT8    WifiEnable;                              ///< Offset 855     WifiEnable
  UINT8    WifiDomainType1;                         ///< Offset 856     WifiDomainType1
  UINT16   WifiPowerLimit1;                         ///< Offset 857     WifiPowerLimit1
  UINT32   WifiTimeWindow1;                         ///< Offset 859     WifiTimeWindow1
  UINT8    TRxDelay0;                               ///< Offset 863     TRxDelay0
  UINT8    TRxCableLength0;                         ///< Offset 864     TRxCableLength0
  UINT8    TRxDelay1;                               ///< Offset 865     TRxDelay1
  UINT8    TRxCableLength1;                         ///< Offset 866     TRxCableLength1
  UINT8    WrddDomainType1;                         ///< Offset 867     WrddDomainType1
  UINT16   WrddCountryIndentifier1;                 ///< Offset 868     WrddCountryIndentifier1
  UINT8    Reserved14[148];                         ///< Offset 870:1017
  UINT8    EnableAPPolicy;                          ///< Offset 1018    Adaptive Performance Policy @deprecated. Intel(R) Dynamic Tuning can dynamically enable/disable policies
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
  UINT8    SIOI;                                    ///< Offset 1046    WITT SIO I2C test devices' connection point
  UINT8    SIOS;                                    ///< Offset 1047    WITT SIO SPI test devices' connection point
  UINT8    GPTD;                                    ///< Offset 1048    GPIO test devices
  UINT16   GDBT;                                    ///< Offset 1049    GPIO test devices' debounce value,
  UINT8    UTKX;                                    ///< Offset 1051    UTK test devices' connection point
  UINT8    SPTD;                                    ///< Offset 1052    SerialIo additional test devices
  // Test Device for Serial IOs
  UINT8    SII0;                                    ///< Offset 1053    SIO I2C0 test devices' connection point
  UINT8    SII1;                                    ///< Offset 1054    SIO I2C1 test devices' connection point
  UINT8    SII2;                                    ///< Offset 1055    SIO I2C2 test devices' connection point
  UINT8    SII3;                                    ///< Offset 1056    SIO I2C3 test devices' connection point
  UINT8    SII4;                                    ///< Offset 1057    SIO I2C4 test devices' connection point
  UINT8    SII5;                                    ///< Offset 1058    SIO I2C5 test devices' connection point
  UINT8    SIS0;                                    ///< Offset 1059    SIO SPI0 test devices' connection point
  UINT8    SIS1;                                    ///< Offset 1060    SIO SPI1 test devices' connection point
  UINT8    SIS2;                                    ///< Offset 1061    SIO SPI2 test devices' connection point
  UINT8    Reserved15[4];                           ///< Offset 1062:1065
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
  UINT8    Reserved16[1];                           ///< Offset 1082:1082
  UINT8    USTP;                                    ///< Offset 1083    use SerialIo timing parameters
  UINT8    Reserved17[41];                          ///< Offset 1084:1124
  UINT32   FingerPrintSleepGpio;                    ///< Offset 1125    Gpio for fingerprint sleep
  UINT32   FingerPrintIrqGpio;                      ///< Offset 1129    Gpio for fingerprint irq
  UINT32   WwanModemBaseBandResetGpio;              ///< Offset 1133    Gpio for modem reset
  UINT32   DiscreteBtModuleRfKillGpio;              ///< Offset 1137    Gpio for Bluetooth RfKill
  UINT32   DiscreteBtModuleIrqGpio;                 ///< Offset 1141    Gpio for Bluetooth interrupt
  UINT32   TouchpadIrqGpio;                         ///< Offset 1145    Gpio for touchPaD Interrupt
  UINT32   TouchpanelIrqGpio;                       ///< Offset 1149    Gpio for touchPaneL Interrupt
  //
  // MipiCam specific
  //
  UINT8    MipiCamControlLogic0;                    ///< Offset 1153
  UINT8    MipiCamControlLogic1;                    ///< Offset 1154
  UINT8    MipiCamControlLogic2;                    ///< Offset 1155
  UINT8    MipiCamControlLogic3;                    ///< Offset 1156
  UINT8    MipiCamControlLogic4;                    ///< Offset 1157
  UINT8    MipiCamControlLogic5;                    ///< Offset 1158
  UINT8    MipiCamLink0Enabled;                     ///< Offset 1159
  UINT8    MipiCamLink1Enabled;                     ///< Offset 1160
  UINT8    MipiCamLink2Enabled;                     ///< Offset 1161
  UINT8    MipiCamLink3Enabled;                     ///< Offset 1162
  UINT8    MipiCamLink4Enabled;                     ///< Offset 1163
  UINT8    MipiCamLink5Enabled;                     ///< Offset 1164
  UINT8    MipiCamLanesClkDiv;                      ///< Offset 1165    MipiCamLanesClkDiv is @deprecated as the Laneclockdivision option is added and now can be set uniquely for all the links.
  // Control Logic 0 options
  UINT8    MipiCamCtrlLogic0_Version;               ///< Offset 1166    Version of CLDB structure
  UINT8    MipiCamCtrlLogic0_Type;                  ///< Offset 1167    Type
  UINT8    MipiCamCtrlLogic0_CrdVersion;            ///< Offset 1168    Version of CRD
  UINT32   MipiCamCtrlLogic0_InputClock;            ///< Offset 1169    Input Clock
  UINT8    MipiCamCtrlLogic0_GpioPinsEnabled;       ///< Offset 1173    Number of GPIO Pins enabled
  UINT8    MipiCamCtrlLogic0_I2cBus;                ///< Offset 1174    I2C Serial Bus Number
  UINT16   MipiCamCtrlLogic0_I2cAddress;            ///< Offset 1175    I2C Address
  UINT8    MipiCamCtrlLogic0_GpioGroupPadNumber[6]; ///< Offset 1177    GPIO Group Pad Number
  UINT8    MipiCamCtrlLogic0_GpioGroupNumber[6];    ///< Offset 1183    GPIO Group Number
  UINT8    MipiCamCtrlLogic0_GpioFunction[6];       ///< Offset 1189    GPIO Function
  UINT8    MipiCamCtrlLogic0_GpioActiveValue[6];    ///< Offset 1195    GPIO Active Value
  UINT8    MipiCamCtrlLogic0_GpioInitialValue[6];   ///< Offset 1201    GPIO Initial Value
  UINT8    MipiCamCtrlLogic0_Pld;                   ///< Offset 1207    Camera Position
  UINT8    MipiCamCtrlLogic0_Wled1FlashMaxCurrent;  ///< Offset 1208    WLED1 Flash Max Current
  UINT8    MipiCamCtrlLogic0_Wled1TorchMaxCurrent;  ///< Offset 1209    WLED1 Torch Max Current
  UINT8    MipiCamCtrlLogic0_Wled2FlashMaxCurrent;  ///< Offset 1210    WLED2 Flash Max Current
  UINT8    MipiCamCtrlLogic0_Wled2TorchMaxCurrent;  ///< Offset 1211    WLED2 Torch Max Current
  UINT8    MipiCamCtrlLogic0_SubPlatformId;         ///< Offset 1212    Sub Platform Id
  UINT8    MipiCamCtrlLogic0_Wled1Type;             ///< Offset 1213    WLED1 Type
  UINT8    MipiCamCtrlLogic0_Wled2Type;             ///< Offset 1214    WLED2 Type
  UINT8    MipiCamCtrlLogic0_PchClockSource;        ///< Offset 1215    PCH Clock source
  // Control Logic 1 options
  UINT8    MipiCamCtrlLogic1_Version;               ///< Offset 1216    Version of CLDB structure
  UINT8    MipiCamCtrlLogic1_Type;                  ///< Offset 1217    Type
  UINT8    MipiCamCtrlLogic1_CrdVersion;            ///< Offset 1218    Version of CRD
  UINT32   MipiCamCtrlLogic1_InputClock;            ///< Offset 1219    Input Clock
  UINT8    MipiCamCtrlLogic1_GpioPinsEnabled;       ///< Offset 1223    Number of GPIO Pins enabled
  UINT8    MipiCamCtrlLogic1_I2cBus;                ///< Offset 1224    I2C Serial Bus Number
  UINT16   MipiCamCtrlLogic1_I2cAddress;            ///< Offset 1225    I2C Address
  UINT8    MipiCamCtrlLogic1_GpioGroupPadNumber[6]; ///< Offset 1227    GPIO Group Pad Number
  UINT8    MipiCamCtrlLogic1_GpioGroupNumber[6];    ///< Offset 1233    GPIO Group Number
  UINT8    MipiCamCtrlLogic1_GpioFunction[6];       ///< Offset 1239    GPIO Function
  UINT8    MipiCamCtrlLogic1_GpioActiveValue[6];    ///< Offset 1245    GPIO Active Value
  UINT8    MipiCamCtrlLogic1_GpioInitialValue[6];   ///< Offset 1251    GPIO Initial Value
  UINT8    MipiCamCtrlLogic1_Pld;                   ///< Offset 1257    Camera Position
  UINT8    MipiCamCtrlLogic1_Wled1FlashMaxCurrent;  ///< Offset 1258    WLED1 Flash Max Current
  UINT8    MipiCamCtrlLogic1_Wled1TorchMaxCurrent;  ///< Offset 1259    WLED1 Torch Max Current
  UINT8    MipiCamCtrlLogic1_Wled2FlashMaxCurrent;  ///< Offset 1260    WLED2 Flash Max Current
  UINT8    MipiCamCtrlLogic1_Wled2TorchMaxCurrent;  ///< Offset 1261    WLED2 Torch Max Current
  UINT8    MipiCamCtrlLogic1_SubPlatformId;         ///< Offset 1262    Sub Platform Id
  UINT8    MipiCamCtrlLogic1_Wled1Type;             ///< Offset 1263    WLED1 Type
  UINT8    MipiCamCtrlLogic1_Wled2Type;             ///< Offset 1264    WLED2 Type
  UINT8    MipiCamCtrlLogic1_PchClockSource;        ///< Offset 1265    PCH Clock source
  // Control Logic 2 options
  UINT8    MipiCamCtrlLogic2_Version;               ///< Offset 1266    Version of CLDB structure
  UINT8    MipiCamCtrlLogic2_Type;                  ///< Offset 1267    Type
  UINT8    MipiCamCtrlLogic2_CrdVersion;            ///< Offset 1268    Version of CRD
  UINT32   MipiCamCtrlLogic2_InputClock;            ///< Offset 1269    Input Clock
  UINT8    MipiCamCtrlLogic2_GpioPinsEnabled;       ///< Offset 1273    Number of GPIO Pins enabled
  UINT8    MipiCamCtrlLogic2_I2cBus;                ///< Offset 1274    I2C Serial Bus Number
  UINT16   MipiCamCtrlLogic2_I2cAddress;            ///< Offset 1275    I2C Address
  UINT8    MipiCamCtrlLogic2_GpioGroupPadNumber[6]; ///< Offset 1277    GPIO Group Pad Number
  UINT8    MipiCamCtrlLogic2_GpioGroupNumber[6];    ///< Offset 1283    GPIO Group Number
  UINT8    MipiCamCtrlLogic2_GpioFunction[6];       ///< Offset 1289    GPIO Function
  UINT8    MipiCamCtrlLogic2_GpioActiveValue[6];    ///< Offset 1295    GPIO Active Value
  UINT8    MipiCamCtrlLogic2_GpioInitialValue[6];   ///< Offset 1301    GPIO Initial Value
  UINT8    MipiCamCtrlLogic2_Pld;                   ///< Offset 1307    Camera Position
  UINT8    MipiCamCtrlLogic2_Wled1FlashMaxCurrent;  ///< Offset 1308    WLED1 Flash Max Current
  UINT8    MipiCamCtrlLogic2_Wled1TorchMaxCurrent;  ///< Offset 1309    WLED1 Torch Max Current
  UINT8    MipiCamCtrlLogic2_Wled2FlashMaxCurrent;  ///< Offset 1310    WLED2 Flash Max Current
  UINT8    MipiCamCtrlLogic2_Wled2TorchMaxCurrent;  ///< Offset 1311    WLED2 Torch Max Current
  UINT8    MipiCamCtrlLogic2_SubPlatformId;         ///< Offset 1312    Sub Platform Id
  UINT8    MipiCamCtrlLogic2_Wled1Type;             ///< Offset 1313    WLED1 Type
  UINT8    MipiCamCtrlLogic2_Wled2Type;             ///< Offset 1314    WLED2 Type
  UINT8    MipiCamCtrlLogic2_PchClockSource;        ///< Offset 1315    PCH Clock source
  // Control Logic 3 options
  UINT8    MipiCamCtrlLogic3_Version;               ///< Offset 1316    Version of CLDB structure
  UINT8    MipiCamCtrlLogic3_Type;                  ///< Offset 1317    Type
  UINT8    MipiCamCtrlLogic3_CrdVersion;            ///< Offset 1318    Version of CRD
  UINT32   MipiCamCtrlLogic3_InputClock;            ///< Offset 1319    Input Clock
  UINT8    MipiCamCtrlLogic3_GpioPinsEnabled;       ///< Offset 1323    Number of GPIO Pins enabled
  UINT8    MipiCamCtrlLogic3_I2cBus;                ///< Offset 1324    I2C Serial Bus Number
  UINT16   MipiCamCtrlLogic3_I2cAddress;            ///< Offset 1325    I2C Address
  UINT8    MipiCamCtrlLogic3_GpioGroupPadNumber[6]; ///< Offset 1327    GPIO Group Pad Number
  UINT8    MipiCamCtrlLogic3_GpioGroupNumber[6];    ///< Offset 1333    GPIO Group Number
  UINT8    MipiCamCtrlLogic3_GpioFunction[6];       ///< Offset 1339    GPIO Function
  UINT8    MipiCamCtrlLogic3_GpioActiveValue[6];    ///< Offset 1345    GPIO Active Value
  UINT8    MipiCamCtrlLogic3_GpioInitialValue[6];   ///< Offset 1351    GPIO Initial Value
  UINT8    MipiCamCtrlLogic3_Pld;                   ///< Offset 1357    Camera Position
  UINT8    MipiCamCtrlLogic3_Wled1FlashMaxCurrent;  ///< Offset 1358    WLED1 Flash Max Current
  UINT8    MipiCamCtrlLogic3_Wled1TorchMaxCurrent;  ///< Offset 1359    WLED1 Torch Max Current
  UINT8    MipiCamCtrlLogic3_Wled2FlashMaxCurrent;  ///< Offset 1360    WLED2 Flash Max Current
  UINT8    MipiCamCtrlLogic3_Wled2TorchMaxCurrent;  ///< Offset 1361    WLED2 Torch Max Current
  UINT8    MipiCamCtrlLogic3_SubPlatformId;         ///< Offset 1362    Sub Platform Id
  UINT8    MipiCamCtrlLogic3_Wled1Type;             ///< Offset 1363    WLED1 Type
  UINT8    MipiCamCtrlLogic3_Wled2Type;             ///< Offset 1364    WLED2 Type
  UINT8    MipiCamCtrlLogic3_PchClockSource;        ///< Offset 1365    PCH Clock source
  // Control Logic 4 options
  UINT8    MipiCamCtrlLogic4_Version;               ///< Offset 1366    Version of CLDB structure
  UINT8    MipiCamCtrlLogic4_Type;                  ///< Offset 1367    Type
  UINT8    MipiCamCtrlLogic4_CrdVersion;            ///< Offset 1368    Version of CRD
  UINT32   MipiCamCtrlLogic4_InputClock;            ///< Offset 1369    Input Clock
  UINT8    MipiCamCtrlLogic4_GpioPinsEnabled;       ///< Offset 1373    Number of GPIO Pins enabled
  UINT8    MipiCamCtrlLogic4_I2cBus;                ///< Offset 1374    I2C Serial Bus Number
  UINT16   MipiCamCtrlLogic4_I2cAddress;            ///< Offset 1375    I2C Address
  UINT8    MipiCamCtrlLogic4_GpioGroupPadNumber[6]; ///< Offset 1377    GPIO Group Pad Number
  UINT8    MipiCamCtrlLogic4_GpioGroupNumber[6];    ///< Offset 1383    GPIO Group Number
  UINT8    MipiCamCtrlLogic4_GpioFunction[6];       ///< Offset 1389    GPIO Function
  UINT8    MipiCamCtrlLogic4_GpioActiveValue[6];    ///< Offset 1395    GPIO Active Value
  UINT8    MipiCamCtrlLogic4_GpioInitialValue[6];   ///< Offset 1401    GPIO Initial Value
  UINT8    MipiCamCtrlLogic4_Pld;                   ///< Offset 1407    Camera Position
  UINT8    MipiCamCtrlLogic4_Wled1FlashMaxCurrent;  ///< Offset 1408    WLED1 Flash Max Current
  UINT8    MipiCamCtrlLogic4_Wled1TorchMaxCurrent;  ///< Offset 1409    WLED1 Torch Max Current
  UINT8    MipiCamCtrlLogic4_Wled2FlashMaxCurrent;  ///< Offset 1410    WLED2 Flash Max Current
  UINT8    MipiCamCtrlLogic4_Wled2TorchMaxCurrent;  ///< Offset 1411    WLED2 Torch Max Current
  UINT8    MipiCamCtrlLogic4_SubPlatformId;         ///< Offset 1412    Sub Platform Id
  UINT8    MipiCamCtrlLogic4_Wled1Type;             ///< Offset 1413    WLED1 Type
  UINT8    MipiCamCtrlLogic4_Wled2Type;             ///< Offset 1414    WLED2 Type
  UINT8    MipiCamCtrlLogic4_PchClockSource;        ///< Offset 1415    PCH Clock source
  // Control Logic 5 options
  UINT8    MipiCamCtrlLogic5_Version;               ///< Offset 1416    Version of CLDB structure
  UINT8    MipiCamCtrlLogic5_Type;                  ///< Offset 1417    Type
  UINT8    MipiCamCtrlLogic5_CrdVersion;            ///< Offset 1418    Version of CRD
  UINT32   MipiCamCtrlLogic5_InputClock;            ///< Offset 1419    Input Clock
  UINT8    MipiCamCtrlLogic5_GpioPinsEnabled;       ///< Offset 1423    Number of GPIO Pins enabled
  UINT8    MipiCamCtrlLogic5_I2cBus;                ///< Offset 1424    I2C Serial Bus Number
  UINT16   MipiCamCtrlLogic5_I2cAddress;            ///< Offset 1425    I2C Address
  UINT8    MipiCamCtrlLogic5_GpioGroupPadNumber[6]; ///< Offset 1427    GPIO Group Pad Number
  UINT8    MipiCamCtrlLogic5_GpioGroupNumber[6];    ///< Offset 1433    GPIO Group Number
  UINT8    MipiCamCtrlLogic5_GpioFunction[6];       ///< Offset 1439    GPIO Function
  UINT8    MipiCamCtrlLogic5_GpioActiveValue[6];    ///< Offset 1445    GPIO Active Value
  UINT8    MipiCamCtrlLogic5_GpioInitialValue[6];   ///< Offset 1451    GPIO Initial Value
  UINT8    MipiCamCtrlLogic5_Pld;                   ///< Offset 1457    Camera Position
  UINT8    MipiCamCtrlLogic5_Wled1FlashMaxCurrent;  ///< Offset 1458    WLED1 Flash Max Current
  UINT8    MipiCamCtrlLogic5_Wled1TorchMaxCurrent;  ///< Offset 1459    WLED1 Torch Max Current
  UINT8    MipiCamCtrlLogic5_Wled2FlashMaxCurrent;  ///< Offset 1460    WLED2 Flash Max Current
  UINT8    MipiCamCtrlLogic5_Wled2TorchMaxCurrent;  ///< Offset 1461    WLED2 Torch Max Current
  UINT8    MipiCamCtrlLogic5_SubPlatformId;         ///< Offset 1462    Sub Platform Id
  UINT8    MipiCamCtrlLogic5_Wled1Type;             ///< Offset 1463    WLED1 Type
  UINT8    MipiCamCtrlLogic5_Wled2Type;             ///< Offset 1464    WLED2 Type
  UINT8    MipiCamCtrlLogic5_PchClockSource;        ///< Offset 1465    PCH Clock source
  // Mipi Cam Link0 options
  UINT8    MipiCamLink0SensorModel;                 ///< Offset 1466    Sensor Model
  UINT8    MipiCamLink0UserHid[9];                  ///< Offset 1467    User defined HID ASCII character 0
                                                    ///< Offset 1475    User defined HID ASCII character 8
  UINT8    MipiCamLink0Pld;                         ///< Offset 1476    Camera Position
  UINT8    MipiCamLink0ModuleName[16];              ///< Offset 1477    Camera Module Name ASCII character 0
                                                    ///< Offset 1492    Camera Module Name ASCII character 15
  UINT8    MipiCamLink0I2cDevicesEnabled;           ///< Offset 1493    Number of I2C devices
  UINT8    MipiCamLink0I2cBus;                      ///< Offset 1494    I2C Serial Bus number
  UINT16   MipiCamLink0I2cAddrDev[12];              ///< Offset 1495    Address of I2C Device0 on Link0
                                                    ///< Offset 1497    Address of I2C Device1 on Link0
                                                    ///< Offset 1499    Address of I2C Device2 on Link0
                                                    ///< Offset 1501    Address of I2C Device3 on Link0
                                                    ///< Offset 1503    Address of I2C Device4 on Link0
                                                    ///< Offset 1505    Address of I2C Device5 on Link0
                                                    ///< Offset 1507    Address of I2C Device6 on Link0
                                                    ///< Offset 1509    Address of I2C Device7 on Link0
                                                    ///< Offset 1511    Address of I2C Device8 on Link0
                                                    ///< Offset 1513    Address of I2C Device9 on Link0
                                                    ///< Offset 1515    Address of I2C Device10 on Link0
                                                    ///< Offset 1517    Address of I2C Device11 on Link0
  UINT8    MipiCamLink0I2cDeviceType[12];           ///< Offset 1519    Type of I2C Device0 on Link0
                                                    ///< Offset 1520    Type of I2C Device1 on Link0
                                                    ///< Offset 1521    Type of I2C Device2 on Link0
                                                    ///< Offset 1522    Type of I2C Device3 on Link0
                                                    ///< Offset 1523    Type of I2C Device4 on Link0
                                                    ///< Offset 1524    Type of I2C Device5 on Link0
                                                    ///< Offset 1525    Type of I2C Device6 on Link0
                                                    ///< Offset 1526    Type of I2C Device7 on Link0
                                                    ///< Offset 1527    Type of I2C Device8 on Link0
                                                    ///< Offset 1528    Type of I2C Device9 on Link0
                                                    ///< Offset 1529    Type of I2C Device10 on Link0
                                                    ///< Offset 1530    Type of I2C Device11 on Link0
  UINT8    MipiCamLink0DD_Version;                  ///< Offset 1531    Version of SSDB structure
  UINT8    MipiCamLink0DD_CrdVersion;               ///< Offset 1532    Version of CRD
  UINT8    MipiCamLink0DD_LinkUsed;                 ///< Offset 1533    CSI2 Link used
  UINT8    MipiCamLink0DD_LaneUsed;                 ///< Offset 1534    MIPI-CSI2 Data Lane
  UINT8    MipiCamLink0DD_EepromType;               ///< Offset 1535    EEPROM Type
  UINT8    MipiCamLink0DD_VcmType;                  ///< Offset 1536    VCM Type
  UINT8    MipiCamLink0DD_FlashSupport;             ///< Offset 1537    Flash Support
  UINT8    MipiCamLink0DD_PrivacyLed;               ///< Offset 1538    Privacy LED
  UINT8    MipiCamLink0DD_Degree;                   ///< Offset 1539    Degree
  UINT32   MipiCamLink0DD_Mclk;                     ///< Offset 1540    MCLK
  UINT8    MipiCamLink0DD_ControlLogic;             ///< Offset 1544    Control Logic
  UINT8    MipiCamLink0DD_PmicPosition;             ///< Offset 1545    PMIC Position
  UINT8    MipiCamLink0DD_VoltageRail;              ///< Offset 1546    Voltage Rail
  UINT8    MipiCamLink0_FlashDriverSelection;       ///< Offset 1547    Flash Driver Selection
  // Mipi Cam Link1 options
  UINT8    MipiCamLink1SensorModel;                 ///< Offset 1548    Sensor Model
  UINT8    MipiCamLink1UserHid[9];                  ///< Offset 1549    User defined HID ASCII character 0
                                                    ///< Offset 1557    User defined HID ASCII character 8
  UINT8    MipiCamLink1Pld;                         ///< Offset 1558    Camera Position
  UINT8    MipiCamLink1ModuleName[16];              ///< Offset 1559    Camera Module Name ASCII character 0
                                                    ///< Offset 1574    Camera Module Name ASCII character 15
  UINT8    MipiCamLink1I2cDevicesEnabled;           ///< Offset 1575    Number of I2C devices
  UINT8    MipiCamLink1I2cBus;                      ///< Offset 1576    I2C Serial Bus number
  UINT16   MipiCamLink1I2cAddrDev[12];              ///< Offset 1577    Address of I2C Device0 on Link1
                                                    ///< Offset 1579    Address of I2C Device1 on Link1
                                                    ///< Offset 1581    Address of I2C Device2 on Link1
                                                    ///< Offset 1583    Address of I2C Device3 on Link1
                                                    ///< Offset 1585    Address of I2C Device4 on Link1
                                                    ///< Offset 1587    Address of I2C Device5 on Link1
                                                    ///< Offset 1589    Address of I2C Device6 on Link1
                                                    ///< Offset 1591    Address of I2C Device7 on Link1
                                                    ///< Offset 1593    Address of I2C Device8 on Link1
                                                    ///< Offset 1595    Address of I2C Device9 on Link1
                                                    ///< Offset 1597    Address of I2C Device10 on Link1
                                                    ///< Offset 1599    Address of I2C Device11 on Link1
  UINT8    MipiCamLink1I2cDeviceType[12];           ///< Offset 1601    Type of I2C Device0 on Link1
                                                    ///< Offset 1602    Type of I2C Device1 on Link1
                                                    ///< Offset 1603    Type of I2C Device2 on Link1
                                                    ///< Offset 1604    Type of I2C Device3 on Link1
                                                    ///< Offset 1605    Type of I2C Device4 on Link1
                                                    ///< Offset 1606    Type of I2C Device5 on Link1
                                                    ///< Offset 1607    Type of I2C Device6 on Link1
                                                    ///< Offset 1608    Type of I2C Device7 on Link1
                                                    ///< Offset 1609    Type of I2C Device8 on Link1
                                                    ///< Offset 1610    Type of I2C Device9 on Link1
                                                    ///< Offset 1611    Type of I2C Device10 on Link1
                                                    ///< Offset 1612    Type of I2C Device11 on Link1
  UINT8    MipiCamLink1DD_Version;                  ///< Offset 1613    Version of SSDB structure
  UINT8    MipiCamLink1DD_CrdVersion;               ///< Offset 1614    Version of CRD
  UINT8    MipiCamLink1DD_LinkUsed;                 ///< Offset 1615    CSI2 Link used
  UINT8    MipiCamLink1DD_LaneUsed;                 ///< Offset 1616    MIPI-CSI2 Data Lane
  UINT8    MipiCamLink1DD_EepromType;               ///< Offset 1617    EEPROM Type
  UINT8    MipiCamLink1DD_VcmType;                  ///< Offset 1618    VCM Type
  UINT8    MipiCamLink1DD_FlashSupport;             ///< Offset 1619    Flash Support
  UINT8    MipiCamLink1DD_PrivacyLed;               ///< Offset 1620    Privacy LED
  UINT8    MipiCamLink1DD_Degree;                   ///< Offset 1621    Degree
  UINT32   MipiCamLink1DD_Mclk;                     ///< Offset 1622    MCLK
  UINT8    MipiCamLink1DD_ControlLogic;             ///< Offset 1626    Control Logic
  UINT8    MipiCamLink1DD_PmicPosition;             ///< Offset 1627    PMIC Position
  UINT8    MipiCamLink1DD_VoltageRail;              ///< Offset 1628    Voltage Rail
  UINT8    MipiCamLink1_FlashDriverSelection;       ///< Offset 1629    Flash Driver Selection
  // Mipi Cam Link2 options
  UINT8    MipiCamLink2SensorModel;                 ///< Offset 1630    Sensor Model
  UINT8    MipiCamLink2UserHid[9];                  ///< Offset 1631    User defined HID ASCII character 0
                                                    ///< Offset 1639    User defined HID ASCII character 8
  UINT8    MipiCamLink2Pld;                         ///< Offset 1640    Camera Position
  UINT8    MipiCamLink2ModuleName[16];              ///< Offset 1641    Camera Module Name ASCII character 0
                                                    ///< Offset 1656    Camera Module Name ASCII character 15
  UINT8    MipiCamLink2I2cDevicesEnabled;           ///< Offset 1657    Number of I2C devices
  UINT8    MipiCamLink2I2cBus;                      ///< Offset 1658    I2C Serial Bus number
  UINT16   MipiCamLink2I2cAddrDev[12];              ///< Offset 1659    Address of I2C Device0 on Link2
                                                    ///< Offset 1661    Address of I2C Device1 on Link2
                                                    ///< Offset 1663    Address of I2C Device2 on Link2
                                                    ///< Offset 1665    Address of I2C Device3 on Link2
                                                    ///< Offset 1667    Address of I2C Device4 on Link2
                                                    ///< Offset 1669    Address of I2C Device5 on Link2
                                                    ///< Offset 1671    Address of I2C Device6 on Link2
                                                    ///< Offset 1673    Address of I2C Device7 on Link2
                                                    ///< Offset 1675    Address of I2C Device8 on Link2
                                                    ///< Offset 1677    Address of I2C Device9 on Link2
                                                    ///< Offset 1679    Address of I2C Device10 on Link2
                                                    ///< Offset 1681    Address of I2C Device11 on Link2
  UINT8    MipiCamLink2I2cDeviceType[12];           ///< Offset 1683    Type of I2C Device0 on Link2
                                                    ///< Offset 1684    Type of I2C Device1 on Link2
                                                    ///< Offset 1685    Type of I2C Device2 on Link2
                                                    ///< Offset 1686    Type of I2C Device3 on Link2
                                                    ///< Offset 1687    Type of I2C Device4 on Link2
                                                    ///< Offset 1688    Type of I2C Device5 on Link2
                                                    ///< Offset 1689    Type of I2C Device6 on Link2
                                                    ///< Offset 1690    Type of I2C Device7 on Link2
                                                    ///< Offset 1691    Type of I2C Device8 on Link2
                                                    ///< Offset 1692    Type of I2C Device9 on Link2
                                                    ///< Offset 1693    Type of I2C Device10 on Link2
                                                    ///< Offset 1694    Type of I2C Device11 on Link2
  UINT8    MipiCamLink2DD_Version;                  ///< Offset 1695    Version of SSDB structure
  UINT8    MipiCamLink2DD_CrdVersion;               ///< Offset 1696    Version of CRD
  UINT8    MipiCamLink2DD_LinkUsed;                 ///< Offset 1697    CSI2 Link used
  UINT8    MipiCamLink2DD_LaneUsed;                 ///< Offset 1698    MIPI-CSI2 Data Lane
  UINT8    MipiCamLink2DD_EepromType;               ///< Offset 1699    EEPROM Type
  UINT8    MipiCamLink2DD_VcmType;                  ///< Offset 1700    VCM Type
  UINT8    MipiCamLink2DD_FlashSupport;             ///< Offset 1701    Flash Support
  UINT8    MipiCamLink2DD_PrivacyLed;               ///< Offset 1702    Privacy LED
  UINT8    MipiCamLink2DD_Degree;                   ///< Offset 1703    Degree
  UINT32   MipiCamLink2DD_Mclk;                     ///< Offset 1704    MCLK
  UINT8    MipiCamLink2DD_ControlLogic;             ///< Offset 1708    Control Logic
  UINT8    MipiCamLink2DD_PmicPosition;             ///< Offset 1709    PMIC Position
  UINT8    MipiCamLink2DD_VoltageRail;              ///< Offset 1710    Voltage Rail
  UINT8    MipiCamLink2_FlashDriverSelection;       ///< Offset 1711    Flash Driver Selection
  // Mipi Cam Link3 options
  UINT8    MipiCamLink3SensorModel;                 ///< Offset 1712    Sensor Model
  UINT8    MipiCamLink3UserHid[9];                  ///< Offset 1713    User defined HID ASCII character 0
                                                    ///< Offset 1721    User defined HID ASCII character 8
  UINT8    MipiCamLink3Pld;                         ///< Offset 1722    Camera Position
  UINT8    MipiCamLink3ModuleName[16];              ///< Offset 1723    Camera Module Name ASCII character 0
                                                    ///< Offset 1738    Camera Module Name ASCII character 15
  UINT8    MipiCamLink3I2cDevicesEnabled;           ///< Offset 1739    Number of I2C devices
  UINT8    MipiCamLink3I2cBus;                      ///< Offset 1740    I2C Serial Bus number
  UINT16   MipiCamLink3I2cAddrDev[12];              ///< Offset 1741    Address of I2C Device0 on Link3
                                                    ///< Offset 1743    Address of I2C Device1 on Link3
                                                    ///< Offset 1745    Address of I2C Device2 on Link3
                                                    ///< Offset 1747    Address of I2C Device3 on Link3
                                                    ///< Offset 1749    Address of I2C Device4 on Link3
                                                    ///< Offset 1751    Address of I2C Device5 on Link3
                                                    ///< Offset 1753    Address of I2C Device6 on Link3
                                                    ///< Offset 1755    Address of I2C Device7 on Link3
                                                    ///< Offset 1757    Address of I2C Device8 on Link3
                                                    ///< Offset 1759    Address of I2C Device9 on Link3
                                                    ///< Offset 1761    Address of I2C Device10 on Link3
                                                    ///< Offset 1763    Address of I2C Device11 on Link3
  UINT8    MipiCamLink3I2cDeviceType[12];           ///< Offset 1765    Type of I2C Device0 on Link3
                                                    ///< Offset 1766    Type of I2C Device1 on Link3
                                                    ///< Offset 1767    Type of I2C Device2 on Link3
                                                    ///< Offset 1768    Type of I2C Device3 on Link3
                                                    ///< Offset 1769    Type of I2C Device4 on Link3
                                                    ///< Offset 1770    Type of I2C Device5 on Link3
                                                    ///< Offset 1771    Type of I2C Device6 on Link3
                                                    ///< Offset 1772    Type of I2C Device7 on Link3
                                                    ///< Offset 1773    Type of I2C Device8 on Link3
                                                    ///< Offset 1774    Type of I2C Device9 on Link3
                                                    ///< Offset 1775    Type of I2C Device10 on Link3
                                                    ///< Offset 1776    Type of I2C Device11 on Link3
  UINT8    MipiCamLink3DD_Version;                  ///< Offset 1777    Version of SSDB structure
  UINT8    MipiCamLink3DD_CrdVersion;               ///< Offset 1778    Version of CRD
  UINT8    MipiCamLink3DD_LinkUsed;                 ///< Offset 1779    CSI2 Link used
  UINT8    MipiCamLink3DD_LaneUsed;                 ///< Offset 1780    MIPI-CSI2 Data Lane
  UINT8    MipiCamLink3DD_EepromType;               ///< Offset 1781    EEPROM Type
  UINT8    MipiCamLink3DD_VcmType;                  ///< Offset 1782    VCM Type
  UINT8    MipiCamLink3DD_FlashSupport;             ///< Offset 1783    Flash Support
  UINT8    MipiCamLink3DD_PrivacyLed;               ///< Offset 1784    Privacy LED
  UINT8    MipiCamLink3DD_Degree;                   ///< Offset 1785    Degree
  UINT32   MipiCamLink3DD_Mclk;                     ///< Offset 1786    MCLK
  UINT8    MipiCamLink3DD_ControlLogic;             ///< Offset 1790    Control Logic
  UINT8    MipiCamLink3DD_PmicPosition;             ///< Offset 1791    PMIC Position
  UINT8    MipiCamLink3DD_VoltageRail;              ///< Offset 1792    Voltage Rail
  UINT8    MipiCamLink3_FlashDriverSelection;       ///< Offset 1793    Flash Driver Selection
  // Mipi Cam Link4 options
  UINT8    MipiCamLink4SensorModel;                 ///< Offset 1794    Sensor Model
  UINT8    MipiCamLink4UserHid[9];                  ///< Offset 1795    User defined HID ASCII character 0
                                                    ///< Offset 1803    User defined HID ASCII character 8
  UINT8    MipiCamLink4Pld;                         ///< Offset 1804    Camera Position
  UINT8    MipiCamLink4ModuleName[16];              ///< Offset 1805    Camera Module Name ASCII character 0
                                                    ///< Offset 1820    Camera Module Name ASCII character 15
  UINT8    MipiCamLink4I2cDevicesEnabled;           ///< Offset 1821    Number of I2C devices
  UINT8    MipiCamLink4I2cBus;                      ///< Offset 1822    I2C Serial Bus number
  UINT16   MipiCamLink4I2cAddrDev[12];              ///< Offset 1823    Address of I2C Device0 on Link4
                                                    ///< Offset 1825    Address of I2C Device1 on Link4
                                                    ///< Offset 1827    Address of I2C Device2 on Link4
                                                    ///< Offset 1829    Address of I2C Device3 on Link4
                                                    ///< Offset 1831    Address of I2C Device4 on Link4
                                                    ///< Offset 1833    Address of I2C Device5 on Link4
                                                    ///< Offset 1835    Address of I2C Device6 on Link4
                                                    ///< Offset 1837    Address of I2C Device7 on Link4
                                                    ///< Offset 1839    Address of I2C Device8 on Link4
                                                    ///< Offset 1841    Address of I2C Device9 on Link4
                                                    ///< Offset 1843    Address of I2C Device10 on Link4
                                                    ///< Offset 1845    Address of I2C Device11 on Link4
  UINT8    MipiCamLink4I2cDeviceType[12];           ///< Offset 1847    Type of I2C Device0 on Link4
                                                    ///< Offset 1848    Type of I2C Device1 on Link4
                                                    ///< Offset 1849    Type of I2C Device2 on Link4
                                                    ///< Offset 1850    Type of I2C Device3 on Link4
                                                    ///< Offset 1851    Type of I2C Device4 on Link4
                                                    ///< Offset 1852    Type of I2C Device5 on Link4
                                                    ///< Offset 1853    Type of I2C Device6 on Link4
                                                    ///< Offset 1854    Type of I2C Device7 on Link4
                                                    ///< Offset 1855    Type of I2C Device8 on Link4
                                                    ///< Offset 1856    Type of I2C Device9 on Link4
                                                    ///< Offset 1857    Type of I2C Device10 on Link4
                                                    ///< Offset 1858    Type of I2C Device11 on Link4
  UINT8    MipiCamLink4DD_Version;                  ///< Offset 1859    Version of SSDB structure
  UINT8    MipiCamLink4DD_CrdVersion;               ///< Offset 1860    Version of CRD
  UINT8    MipiCamLink4DD_LinkUsed;                 ///< Offset 1861    CSI2 Link used
  UINT8    MipiCamLink4DD_LaneUsed;                 ///< Offset 1862    MIPI-CSI2 Data Lane
  UINT8    MipiCamLink4DD_EepromType;               ///< Offset 1863    EEPROM Type
  UINT8    MipiCamLink4DD_VcmType;                  ///< Offset 1864    VCM Type
  UINT8    MipiCamLink4DD_FlashSupport;             ///< Offset 1865    Flash Support
  UINT8    MipiCamLink4DD_PrivacyLed;               ///< Offset 1866    Privacy LED
  UINT8    MipiCamLink4DD_Degree;                   ///< Offset 1867    Degree
  UINT32   MipiCamLink4DD_Mclk;                     ///< Offset 1868    MCLK
  UINT8    MipiCamLink4DD_ControlLogic;             ///< Offset 1872    Control Logic
  UINT8    MipiCamLink4DD_PmicPosition;             ///< Offset 1873    PMIC Position
  UINT8    MipiCamLink4DD_VoltageRail;              ///< Offset 1874    Voltage Rail
  UINT8    MipiCamLink4_FlashDriverSelection;       ///< Offset 1875    Flash Driver Selection
  // Mipi Cam Link5 options
  UINT8    MipiCamLink5SensorModel;                 ///< Offset 1876    Sensor Model
  UINT8    MipiCamLink5UserHid[9];                  ///< Offset 1877    User defined HID ASCII character 0
                                                    ///< Offset 1885    User defined HID ASCII character 8
  UINT8    MipiCamLink5Pld;                         ///< Offset 1886    Camera Position
  UINT8    MipiCamLink5ModuleName[16];              ///< Offset 1887    Camera Module Name ASCII character 0
                                                    ///< Offset 1902    Camera Module Name ASCII character 15
  UINT8    MipiCamLink5I2cDevicesEnabled;           ///< Offset 1903    Number of I2C devices
  UINT8    MipiCamLink5I2cBus;                      ///< Offset 1904    I2C Serial Bus number
  UINT16   MipiCamLink5I2cAddrDev[12];              ///< Offset 1905    Address of I2C Device0 on Link5
                                                    ///< Offset 1907    Address of I2C Device1 on Link5
                                                    ///< Offset 1909    Address of I2C Device2 on Link5
                                                    ///< Offset 1911    Address of I2C Device3 on Link5
                                                    ///< Offset 1913    Address of I2C Device4 on Link5
                                                    ///< Offset 1915    Address of I2C Device5 on Link5
                                                    ///< Offset 1917    Address of I2C Device6 on Link5
                                                    ///< Offset 1919    Address of I2C Device7 on Link5
                                                    ///< Offset 1921    Address of I2C Device8 on Link5
                                                    ///< Offset 1923    Address of I2C Device9 on Link5
                                                    ///< Offset 1925    Address of I2C Device10 on Link5
                                                    ///< Offset 1927    Address of I2C Device11 on Link5
  UINT8    MipiCamLink5I2cDeviceType[12];           ///< Offset 1929    Type of I2C Device0 on Link5
                                                    ///< Offset 1930    Type of I2C Device1 on Link5
                                                    ///< Offset 1931    Type of I2C Device2 on Link5
                                                    ///< Offset 1932    Type of I2C Device3 on Link5
                                                    ///< Offset 1933    Type of I2C Device4 on Link5
                                                    ///< Offset 1934    Type of I2C Device5 on Link5
                                                    ///< Offset 1935    Type of I2C Device6 on Link5
                                                    ///< Offset 1936    Type of I2C Device7 on Link5
                                                    ///< Offset 1937    Type of I2C Device8 on Link5
                                                    ///< Offset 1938    Type of I2C Device9 on Link5
                                                    ///< Offset 1939    Type of I2C Device10 on Link5
                                                    ///< Offset 1940    Type of I2C Device11 on Link5
  UINT8    MipiCamLink5DD_Version;                  ///< Offset 1941    Version of SSDB structure
  UINT8    MipiCamLink5DD_CrdVersion;               ///< Offset 1942    Version of CRD
  UINT8    MipiCamLink5DD_LinkUsed;                 ///< Offset 1943    CSI2 Link used
  UINT8    MipiCamLink5DD_LaneUsed;                 ///< Offset 1944    MIPI-CSI2 Data Lane
  UINT8    MipiCamLink5DD_EepromType;               ///< Offset 1945    EEPROM Type
  UINT8    MipiCamLink5DD_VcmType;                  ///< Offset 1946    VCM Type
  UINT8    MipiCamLink5DD_FlashSupport;             ///< Offset 1947    Flash Support
  UINT8    MipiCamLink5DD_PrivacyLed;               ///< Offset 1948    Privacy LED
  UINT8    MipiCamLink5DD_Degree;                   ///< Offset 1949    Degree
  UINT32   MipiCamLink5DD_Mclk;                     ///< Offset 1950    MCLK
  UINT8    MipiCamLink5DD_ControlLogic;             ///< Offset 1954    Control Logic
  UINT8    MipiCamLink5DD_PmicPosition;             ///< Offset 1955    PMIC Position
  UINT8    MipiCamLink5DD_VoltageRail;              ///< Offset 1956    Voltage Rail
  UINT8    MipiCamLink5_FlashDriverSelection;       ///< Offset 1957    Flash Driver Selection
  UINT8    MipiCamFlash0Model;                      ///< Offset 1958    Flash Driver Model
  UINT8    MipiCamFlash0Mode;                       ///< Offset 1959    Flash Mode Selection
  UINT8    MipiCamFlash0ModuleName[16];             ///< Offset 1960    Flash Module Name ASCII character 0
                                                    ///< Offset 1975    Flash Module Name ASCII character 15
  UINT8    MipiCamFlash0I2cBus;                     ///< Offset 1976    I2C Bus Number
  UINT16   MipiCamFlash0I2cAddrDev;                 ///< Offset 1977    I2C Peripheral Device Address
  UINT8    MipiCamFlash0GpioGroupPadNumber;         ///< Offset 1979    GPIO Group Pad Number
  UINT16   MipiCamFlash0GpioGroupNumber;            ///< Offset 1980    GPIO Group Number
  UINT8    MipiCamFlash0GpioActiveValue;            ///< Offset 1982    GPIO Active Value
  UINT8    MipiCamFlash0GpioInitialValue;           ///< Offset 1983    GPIO Initial Value
  UINT8    MipiCamFlash0OperatingMode;              ///< Offset 1984    Flash Driver Operating Mode
  UINT8    MipiCamFlash1Model;                      ///< Offset 1985    Flash Driver Model
  UINT8    MipiCamFlash1Mode;                       ///< Offset 1986    Flash Mode Selection
  UINT8    MipiCamFlash1ModuleName[16];             ///< Offset 1987    Flash Module Name ASCII character 0
                                                    ///< Offset 2002    Flash Module Name ASCII character 15
  UINT8    MipiCamFlash1I2cBus;                     ///< Offset 2003    I2C Bus Number
  UINT16   MipiCamFlash1I2cAddrDev;                 ///< Offset 2004    I2C Peripheral Device Address
  UINT8    MipiCamFlash1GpioGroupPadNumber;         ///< Offset 2006    GPIO Group Pad Number
  UINT16   MipiCamFlash1GpioGroupNumber;            ///< Offset 2007    GPIO Group Number
  UINT8    MipiCamFlash1GpioActiveValue;            ///< Offset 2009    GPIO Active Value
  UINT8    MipiCamFlash1GpioInitialValue;           ///< Offset 2010    GPIO Initial Value
  UINT8    MipiCamFlash1OperatingMode;              ///< Offset 2011    Flash Driver Operating Mode
  UINT8    MipiCamFlash2Model;                      ///< Offset 2012    Flash Driver Model
  UINT8    MipiCamFlash2Mode;                       ///< Offset 2013    Flash Mode Selection
  UINT8    MipiCamFlash2ModuleName[16];             ///< Offset 2014    Flash Module Name ASCII character 0
                                                    ///< Offset 2029    Flash Module Name ASCII character 15
  UINT8    MipiCamFlash2I2cBus;                     ///< Offset 2030    I2C Bus Number
  UINT16   MipiCamFlash2I2cAddrDev;                 ///< Offset 2031    I2C Peripheral Device Address
  UINT8    MipiCamFlash2GpioGroupPadNumber;         ///< Offset 2033    GPIO Group Pad Number
  UINT16   MipiCamFlash2GpioGroupNumber;            ///< Offset 2034    GPIO Group Number
  UINT8    MipiCamFlash2GpioActiveValue;            ///< Offset 2036    GPIO Active Value
  UINT8    MipiCamFlash2GpioInitialValue;           ///< Offset 2037    GPIO Initial Value
  UINT8    MipiCamFlash2OperatingMode;              ///< Offset 2038    Flash Driver Operating Mode
  UINT8    MipiCamFlash3Model;                      ///< Offset 2039    Flash Driver Model
  UINT8    MipiCamFlash3Mode;                       ///< Offset 2040    Flash Mode Selection
  UINT8    MipiCamFlash3ModuleName[16];             ///< Offset 2041    Flash Module Name ASCII character 0
                                                    ///< Offset 2056    Flash Module Name ASCII character 15
  UINT8    MipiCamFlash3I2cBus;                     ///< Offset 2057    I2C Bus Number
  UINT16   MipiCamFlash3I2cAddrDev;                 ///< Offset 2058    I2C Peripheral Device Address
  UINT8    MipiCamFlash3GpioGroupPadNumber;         ///< Offset 2060    GPIO Group Pad Number
  UINT16   MipiCamFlash3GpioGroupNumber;            ///< Offset 2061    GPIO Group Number
  UINT8    MipiCamFlash3GpioActiveValue;            ///< Offset 2063    GPIO Active Value
  UINT8    MipiCamFlash3GpioInitialValue;           ///< Offset 2064    GPIO Initial Value
  UINT8    MipiCamFlash3OperatingMode;              ///< Offset 2065    Flash Driver Operating Mode
  UINT8    MipiCamFlash4Model;                      ///< Offset 2066    Flash Driver Model
  UINT8    MipiCamFlash4Mode;                       ///< Offset 2067    Flash Mode Selection
  UINT8    MipiCamFlash4ModuleName[16];             ///< Offset 2068    Flash Module Name ASCII character 0
                                                    ///< Offset 2083    Flash Module Name ASCII character 15
  UINT8    MipiCamFlash4I2cBus;                     ///< Offset 2084    I2C Bus Number
  UINT16   MipiCamFlash4I2cAddrDev;                 ///< Offset 2085    I2C Peripheral Device Address
  UINT8    MipiCamFlash4GpioGroupPadNumber;         ///< Offset 2087    GPIO Group Pad Number
  UINT16   MipiCamFlash4GpioGroupNumber;            ///< Offset 2088    GPIO Group Number
  UINT8    MipiCamFlash4GpioActiveValue;            ///< Offset 2090    GPIO Active Value
  UINT8    MipiCamFlash4GpioInitialValue;           ///< Offset 2091    GPIO Initial Value
  UINT8    MipiCamFlash4OperatingMode;              ///< Offset 2092    Flash Driver Operating Mode
  UINT8    MipiCamFlash5Model;                      ///< Offset 2093    Flash Driver Model
  UINT8    MipiCamFlash5Mode;                       ///< Offset 2094    Flash Mode Selection
  UINT8    MipiCamFlash5ModuleName[16];             ///< Offset 2095    Flash Module Name ASCII character 0
                                                    ///< Offset 2110    Flash Module Name ASCII character 15
  UINT8    MipiCamFlash5I2cBus;                     ///< Offset 2111    I2C Bus Number
  UINT16   MipiCamFlash5I2cAddrDev;                 ///< Offset 2112    I2C Peripheral Device Address
  UINT8    MipiCamFlash5GpioGroupPadNumber;         ///< Offset 2114    GPIO Group Pad Number
  UINT16   MipiCamFlash5GpioGroupNumber;            ///< Offset 2115    GPIO Group Number
  UINT8    MipiCamFlash5GpioActiveValue;            ///< Offset 2117    GPIO Active Value
  UINT8    MipiCamFlash5GpioInitialValue;           ///< Offset 2118    GPIO Initial Value
  UINT8    MipiCamFlash5OperatingMode;              ///< Offset 2119    Flash Driver Operating Mode
  UINT8    PciDelayOptimizationEcr;                 ///< Offset 2120
  UINT8    I2SC;                                    ///< Offset 2121    HD Audio I2S Codec Selection
  UINT32   I2SI;                                    ///< Offset 2122    HD Audio I2S Codec Interrupt Pin
  UINT8    I2SB;                                    ///< Offset 2126    HD Audio I2S Codec Connection to I2C bus controller instance (I2C[0-5])
  UINT8    OemDesignVariable0;                      ///< Offset 2127    Intel(R) Dynamic Tuning Technology Oem Design Variables
  UINT8    OemDesignVariable1;                      ///< Offset 2128    Intel(R) Dynamic Tuning Technology Oem Design Variables
  UINT8    OemDesignVariable2;                      ///< Offset 2129    Intel(R) Dynamic Tuning Technology Oem Design Variables
  UINT8    OemDesignVariable3;                      ///< Offset 2130    Intel(R) Dynamic Tuning Technology Oem Design Variables
  UINT8    OemDesignVariable4;                      ///< Offset 2131    Intel(R) Dynamic Tuning Technology Oem Design Variables
  UINT8    OemDesignVariable5;                      ///< Offset 2132    Intel(R) Dynamic Tuning Technology Oem Design Variables
  UINT32   UsbTypeCOpBaseAddr;                      ///< Offset 2133    USB Type C Opregion base address
  UINT8    WirelessCharging;                        ///< Offset 2137    WirelessCharging
  // RTD3 Settings
  UINT8    Reserved19[6];                           ///< Offset 2138:2143
  UINT32   HdaDspPpModuleMask;                      ///< Offset 2144    HD-Audio DSP Post-Processing Module Mask
  UINT64   HdaDspPpModCustomGuid1Low;               ///< Offset 2148    HDA PP module custom GUID 1 - first 64bit  [0-63]
  UINT64   HdaDspPpModCustomGuid1High;              ///< Offset 2156    HDA PP module custom GUID 1 - second 64bit [64-127]
  UINT64   HdaDspPpModCustomGuid2Low;               ///< Offset 2164    HDA PP module custom GUID 2 - first 64bit  [0-63]
  UINT64   HdaDspPpModCustomGuid2High;              ///< Offset 2172    HDA PP module custom GUID 2 - second 64bit [64-127]
  UINT64   HdaDspPpModCustomGuid3Low;               ///< Offset 2180    HDA PP module custom GUID 3 - first 64bit  [0-63]
  UINT64   HdaDspPpModCustomGuid3High;              ///< Offset 2188    HDA PP module custom GUID 3 - second 64bit [64-127]
  UINT8    HidEventFilterEnable;                    ///< Offset 2196    HID Event Filter Driver enable
  UINT8    XdciFnEnable;                            ///< Offset 2197    XDCI Enable/Disable status
  UINT8    WrdsWiFiSarEnable;                       ///< Offset 2198    WrdsWiFiSarEnable
  UINT8    WrdsWiFiSarTxPowerSet1Limit1;            ///< Offset 2199    WrdsWiFiSarTxPowerSet1Limit1
  UINT8    WrdsWiFiSarTxPowerSet1Limit2;            ///< Offset 2200    WrdsWiFiSarTxPowerSet1Limit2
  UINT8    WrdsWiFiSarTxPowerSet1Limit3;            ///< Offset 2201    WrdsWiFiSarTxPowerSet1Limit3
  UINT8    WrdsWiFiSarTxPowerSet1Limit4;            ///< Offset 2202    WrdsWiFiSarTxPowerSet1Limit4
  UINT8    WrdsWiFiSarTxPowerSet1Limit5;            ///< Offset 2203    WrdsWiFiSarTxPowerSet1Limit5
  UINT8    WrdsWiFiSarTxPowerSet1Limit6;            ///< Offset 2204    WrdsWiFiSarTxPowerSet1Limit6
  UINT8    WrdsWiFiSarTxPowerSet1Limit7;            ///< Offset 2205    WrdsWiFiSarTxPowerSet1Limit7
  UINT8    WrdsWiFiSarTxPowerSet1Limit8;            ///< Offset 2206    WrdsWiFiSarTxPowerSet1Limit8
  UINT8    WrdsWiFiSarTxPowerSet1Limit9;            ///< Offset 2207    WrdsWiFiSarTxPowerSet1Limit9
  UINT8    WrdsWiFiSarTxPowerSet1Limit10;           ///< Offset 2208    WrdsWiFiSarTxPowerSet1Limit10
  UINT8    EnableVoltageMargining;                  ///< Offset 2209    Enable Voltage Margining
  UINT16   DStateHSPort;                            ///< Offset 2210    D-State for xHCI HS port(BIT0:USB HS Port0 ~ BIT15:USB HS Port15)
  UINT16   DStateSSPort;                            ///< Offset 2212    D-State for xHCI SS port(BIT0:USB SS Port0 ~ BIT15:USB SS Port15)
  UINT8    DStateSataPort;                          ///< Offset 2214    D-State for SATA port(BIT0:SATA Port0 ~ BIT7:SATA Port7)
  UINT8    EwrdWiFiDynamicSarEnable;                ///< Offset 2215    EwrdWiFiDynamicSarEnable
  UINT8    EwrdWiFiDynamicSarRangeSets;             ///< Offset 2216    EwrdWiFiDynamicSarRangeSets
  UINT8    EwrdWiFiSarTxPowerSet2Limit1;            ///< Offset 2217    EwrdWiFiSarTxPowerSet2Limit1
  UINT8    EwrdWiFiSarTxPowerSet2Limit2;            ///< Offset 2218    EwrdWiFiSarTxPowerSet2Limit2
  UINT8    EwrdWiFiSarTxPowerSet2Limit3;            ///< Offset 2219    EwrdWiFiSarTxPowerSet2Limit3
  UINT8    EwrdWiFiSarTxPowerSet2Limit4;            ///< Offset 2220    EwrdWiFiSarTxPowerSet2Limit4
  UINT8    EwrdWiFiSarTxPowerSet2Limit5;            ///< Offset 2221    EwrdWiFiSarTxPowerSet2Limit5
  UINT8    EwrdWiFiSarTxPowerSet2Limit6;            ///< Offset 2222    EwrdWiFiSarTxPowerSet2Limit6
  UINT8    EwrdWiFiSarTxPowerSet2Limit7;            ///< Offset 2223    EwrdWiFiSarTxPowerSet2Limit7
  UINT8    EwrdWiFiSarTxPowerSet2Limit8;            ///< Offset 2224    EwrdWiFiSarTxPowerSet2Limit8
  UINT8    EwrdWiFiSarTxPowerSet2Limit9;            ///< Offset 2225    EwrdWiFiSarTxPowerSet2Limit9
  UINT8    EwrdWiFiSarTxPowerSet2Limit10;           ///< Offset 2226    EwrdWiFiSarTxPowerSet2Limit10
  UINT8    EwrdWiFiSarTxPowerSet3Limit1;            ///< Offset 2227    EwrdWiFiSarTxPowerSet3Limit1
  UINT8    EwrdWiFiSarTxPowerSet3Limit2;            ///< Offset 2228    EwrdWiFiSarTxPowerSet3Limit2
  UINT8    EwrdWiFiSarTxPowerSet3Limit3;            ///< Offset 2229    EwrdWiFiSarTxPowerSet3Limit3
  UINT8    EwrdWiFiSarTxPowerSet3Limit4;            ///< Offset 2230    EwrdWiFiSarTxPowerSet3Limit4
  UINT8    EwrdWiFiSarTxPowerSet3Limit5;            ///< Offset 2231    EwrdWiFiSarTxPowerSet3Limit5
  UINT8    EwrdWiFiSarTxPowerSet3Limit6;            ///< Offset 2232    EwrdWiFiSarTxPowerSet3Limit6
  UINT8    EwrdWiFiSarTxPowerSet3Limit7;            ///< Offset 2233    EwrdWiFiSarTxPowerSet3Limit7
  UINT8    EwrdWiFiSarTxPowerSet3Limit8;            ///< Offset 2234    EwrdWiFiSarTxPowerSet3Limit8
  UINT8    EwrdWiFiSarTxPowerSet3Limit9;            ///< Offset 2235    EwrdWiFiSarTxPowerSet3Limit9
  UINT8    EwrdWiFiSarTxPowerSet3Limit10;           ///< Offset 2236    EwrdWiFiSarTxPowerSet3Limit10
  UINT8    EwrdWiFiSarTxPowerSet4Limit1;            ///< Offset 2237    EwrdWiFiSarTxPowerSet4Limit1
  UINT8    EwrdWiFiSarTxPowerSet4Limit2;            ///< Offset 2238    EwrdWiFiSarTxPowerSet4Limit2
  UINT8    EwrdWiFiSarTxPowerSet4Limit3;            ///< Offset 2239    EwrdWiFiSarTxPowerSet4Limit3
  UINT8    EwrdWiFiSarTxPowerSet4Limit4;            ///< Offset 2240    EwrdWiFiSarTxPowerSet4Limit4
  UINT8    EwrdWiFiSarTxPowerSet4Limit5;            ///< Offset 2241    EwrdWiFiSarTxPowerSet4Limit5
  UINT8    EwrdWiFiSarTxPowerSet4Limit6;            ///< Offset 2242    EwrdWiFiSarTxPowerSet4Limit6
  UINT8    EwrdWiFiSarTxPowerSet4Limit7;            ///< Offset 2243    EwrdWiFiSarTxPowerSet4Limit7
  UINT8    EwrdWiFiSarTxPowerSet4Limit8;            ///< Offset 2244    EwrdWiFiSarTxPowerSet4Limit8
  UINT8    EwrdWiFiSarTxPowerSet4Limit9;            ///< Offset 2245    EwrdWiFiSarTxPowerSet4Limit9
  UINT8    EwrdWiFiSarTxPowerSet4Limit10;           ///< Offset 2246    EwrdWiFiSarTxPowerSet4Limit10
  UINT8    WgdsWiFiSarDeltaGroup1PowerMax1;         ///< Offset 2247    WgdsWiFiSarDeltaGroup1PowerMax1
  UINT8    WgdsWiFiSarDeltaGroup1PowerChainA1;      ///< Offset 2248    WgdsWiFiSarDeltaGroup1PowerChainA1
  UINT8    WgdsWiFiSarDeltaGroup1PowerChainB1;      ///< Offset 2249    WgdsWiFiSarDeltaGroup1PowerChainB1
  UINT8    WgdsWiFiSarDeltaGroup1PowerMax2;         ///< Offset 2250    WgdsWiFiSarDeltaGroup1PowerMax2
  UINT8    WgdsWiFiSarDeltaGroup1PowerChainA2;      ///< Offset 2251    WgdsWiFiSarDeltaGroup1PowerChainA2
  UINT8    WgdsWiFiSarDeltaGroup1PowerChainB2;      ///< Offset 2252    WgdsWiFiSarDeltaGroup1PowerChainB2
  UINT8    WgdsWiFiSarDeltaGroup2PowerMax1;         ///< Offset 2253    WgdsWiFiSarDeltaGroup2PowerMax1
  UINT8    WgdsWiFiSarDeltaGroup2PowerChainA1;      ///< Offset 2254    WgdsWiFiSarDeltaGroup2PowerChainA1
  UINT8    WgdsWiFiSarDeltaGroup2PowerChainB1;      ///< Offset 2255    WgdsWiFiSarDeltaGroup2PowerChainB1
  UINT8    WgdsWiFiSarDeltaGroup2PowerMax2;         ///< Offset 2256    WgdsWiFiSarDeltaGroup2PowerMax2
  UINT8    WgdsWiFiSarDeltaGroup2PowerChainA2;      ///< Offset 2257    WgdsWiFiSarDeltaGroup2PowerChainA2
  UINT8    WgdsWiFiSarDeltaGroup2PowerChainB2;      ///< Offset 2258    WgdsWiFiSarDeltaGroup2PowerChainB2
  UINT8    WgdsWiFiSarDeltaGroup3PowerMax1;         ///< Offset 2259    WgdsWiFiSarDeltaGroup3PowerMax1
  UINT8    WgdsWiFiSarDeltaGroup3PowerChainA1;      ///< Offset 2260    WgdsWiFiSarDeltaGroup3PowerChainA1
  UINT8    WgdsWiFiSarDeltaGroup3PowerChainB1;      ///< Offset 2261    WgdsWiFiSarDeltaGroup3PowerChainB1
  UINT8    WgdsWiFiSarDeltaGroup3PowerMax2;         ///< Offset 2262    WgdsWiFiSarDeltaGroup3PowerMax2
  UINT8    WgdsWiFiSarDeltaGroup3PowerChainA2;      ///< Offset 2263    WgdsWiFiSarDeltaGroup3PowerChainA2
  UINT8    WgdsWiFiSarDeltaGroup3PowerChainB2;      ///< Offset 2264    WgdsWiFiSarDeltaGroup3PowerChainB2
  UINT8    Reserved20[41];                          ///< Offset 2265:2305
  // Reserved for Groups 4 to 9, each needs 6 bytes and total 36 bytes reserved
  UINT8    WiFiDynamicSarAntennaACurrentSet;        ///< Offset 2306    WiFiDynamicSarAntennaACurrentSet
  UINT8    WiFiDynamicSarAntennaBCurrentSet;        ///< Offset 2307    WiFiDynamicSarAntennaBCurrentSet
  UINT8    BluetoothSar;                            ///< Offset 2308    BluetoothSar
  UINT8    BluetoothSarBr;                          ///< Offset 2309    BluetoothSarBr
  UINT8    BluetoothSarEdr2;                        ///< Offset 2310    BluetoothSarEdr2
  UINT8    BluetoothSarEdr3;                        ///< Offset 2311    BluetoothSarEdr3
  UINT8    BluetoothSarLe;                          ///< Offset 2312    BluetoothSarLe
  UINT8    BluetoothSarLe2Mhz;                      ///< Offset 2313    BluetoothSarLe2Mhz
  UINT8    BluetoothSarLeLr;                        ///< Offset 2314    BluetoothSarLeLr
  UINT8    Reserved21[2];                           ///< Offset 2315:2316
  // Reserved for Bluetooth Sar future use
  UINT8    AntennaDiversity;                        ///< Offset 2317    AntennaDiversity
  UINT8    CoExistenceManager;                      ///< Offset 2318    CoExistenceManager
  UINT8    RunTimeVmControl;                        ///< Offset 2319    RunTime VM Control
  //
  //Feature Specific Data Bits
  //
  UINT8    UsbTypeCSupport;                         ///< Offset 2320    USB Type C Supported
  UINT32   HebcValue;                               ///< Offset 2321    HebcValue
  UINT8    PcdBatteryPresent;                       ///< Offset 2325    Battery Present - Bit0: Real Battery is supported on this platform. Bit1: Virtual Battery is supported on this platform.
  UINT8    PcdTsOnDimmTemperature;                  ///< Offset 2326    TS-on-DIMM temperature
  UINT8    EnablePcieTunnelingOverUsb4;             ///< Offset 2327    Enable PCIE tunnelling support over USB4 links.
  UINT8    Reserved22[1];                           ///< Offset 2328:2328
  UINT8    PcdRealBattery1Control;                  ///< Offset 2329    Real Battery 1 Control
  UINT8    PcdRealBattery2Control;                  ///< Offset 2330    Real Battery 2 Control
  UINT8    PcdMipiCamSensor;                        ///< Offset 2331    Mipi Camera Sensor
  UINT8    PcdNCT6776FCOM;                          ///< Offset 2332    NCT6776F COM
  UINT8    PcdNCT6776FSIO;                          ///< Offset 2333    NCT6776F SIO
  UINT8    PcdNCT6776FHWMON;                        ///< Offset 2334    NCT6776F HWMON
  UINT8    PcdH8S2113SIO;                           ///< Offset 2335    H8S2113 SIO
  UINT8    PcdZPoddConfig;                          ///< Offset 2336    ZPODD
  UINT8    PcdRGBCameraAdr;                         ///< Offset 2337    RGB Camera Address
  UINT8    PcdDepthCameraAdr;                       ///< Offset 2338    Depth Camera Addresy
  UINT32   PcdSmcRuntimeSciPin;                     ///< Offset 2339    SMC Runtime Sci Pin
  UINT8    PcdConvertableDockSupport;               ///< Offset 2343    Convertable Dock Support
  UINT8    PcdEcHotKeyF3Support;                    ///< Offset 2344    Ec Hotkey F3 Support
  UINT8    PcdEcHotKeyF4Support;                    ///< Offset 2345    Ec Hotkey F4 Support
  UINT8    PcdEcHotKeyF5Support;                    ///< Offset 2346    Ec Hotkey F5 Support
  UINT8    PcdEcHotKeyF6Support;                    ///< Offset 2347    Ec Hotkey F6 Support
  UINT8    PcdEcHotKeyF7Support;                    ///< Offset 2348    Ec Hotkey F7 Support
  UINT8    PcdEcHotKeyF8Support;                    ///< Offset 2349    Ec Hotkey F8 Support
  UINT8    PcdVirtualButtonVolumeUpSupport;         ///< Offset 2350    Virtual Button Volume Up Support
  UINT8    PcdVirtualButtonVolumeDownSupport;       ///< Offset 2351    Virtual Button Volume Down Support
  UINT8    PcdVirtualButtonHomeButtonSupport;       ///< Offset 2352    Virtual Button Home Button Support
  UINT8    PcdVirtualButtonRotationLockSupport;     ///< Offset 2353    Virtual Button Rotation Lock Support
  UINT8    PcdSlateModeSwitchSupport;               ///< Offset 2354    Slate Mode Switch Support
  UINT8    PcdVirtualGpioButtonSupport;             ///< Offset 2355    Virtual Button Support
  UINT8    PcdAcDcAutoSwitchSupport;                ///< Offset 2356    Ac Dc Auto Switch Support
  UINT32   PcdPmPowerButtonGpioPin;                 ///< Offset 2357    Pm Power Button Gpio Pin
  UINT8    PcdAcpiEnableAllButtonSupport;           ///< Offset 2361    Acpi Enable All Button Support
  UINT8    PcdAcpiHidDriverButtonSupport;           ///< Offset 2362    Acpi Hid Driver Button Support
  UINT32   EcLowPowerModeGpioPin;                   ///< Offset 2363    EcLowPowerModeGpioPin
  UINT32   EcSmiGpioPin;                            ///< Offset 2367    EcSmiGpioPin
  //
  // UCMC setup option, GPIO Pad
  //
  UINT8    UCMS;                                    ///< Offset 2371    Option to select UCSI/UCMC device
  UINT32   UcmcPort1Gpio;                           ///< Offset 2372    Gpio for UCMC Port 1 Interrupt
  UINT32   UcmcPort2Gpio;                           ///< Offset 2376    Gpio for UCMC Port 2 Interrupt
  UINT32   UcmcPort3Gpio;                           ///< Offset 2380    Gpio for UCMC Port 3 Interrupt
  UINT32   UcmcPort4Gpio;                           ///< Offset 2384    Gpio for UCMC Port 4 Interrupt
  UINT8    UsbTypeCEcLess;                          ///< Offset 2388    USB Type C EC None EC PD
  UINT8    Reserved23[20];                          ///< Offset 2389:2408
  UINT8    EnablePchFivrParticipant;                ///< Offset 2409    EnablePchFivrParticipant
  UINT8    Reserved24[3];                           ///< Offset 2410:2412
  UINT8    Ufp2DfpGlobalFlag;                       ///< Offset 2413    Upstream Facing port or Downstream Facing port Global Flag from LPC EC
  UINT8    Ufp2DfpUsbPort;                          ///< Offset 2414    Upstream Facing port or Downstream Facing port number from LPC EC
  UINT8    DbcGlobalFlag;                           ///< Offset 2415    Debug Mode Global Flag from LPC EC
  UINT8    DbcUsbPort;                              ///< Offset 2416    Debug Mode USB Port Number from LPC EC
  UINT8    TotalTypeCPorts;                         ///< Offset 2417    Total Number of type C ports that are supported by platform
  UINT8    UsbTypeCPort1;                           ///< Offset 2418    Type C Connector 1  Port mapping within the controller the port exposed
  UINT8    UsbTypeCPort1Pch;                        ///< Offset 2419    Type C Connector 1  Port mapping within the PCH controller (If Split mode supported)
  UINT8    UsbCPort1Proterties;                     ///< Offset 2420    Type C Connector 1  Portperties Split Support/Controller(PCH/TBT/CPU)/Root port (vaild for TBT)
  UINT8    UsbTypeCPort2;                           ///< Offset 2421    Type C Connector 2  Port mapping within the controller the port exposed
  UINT8    UsbTypeCPort2Pch;                        ///< Offset 2422    Type C Connector 2  Port mapping within the PCH controller (If Split mode supported)
  UINT8    UsbCPort2Proterties;                     ///< Offset 2423    Type C Connector 2  Portperties Split Support/Controller(PCH/TBT/CPU)/Root port (vaild for TBT)
  UINT8    UsbTypeCPort3;                           ///< Offset 2424    Type C Connector 3  Port mapping within the controller the port exposed
  UINT8    UsbTypeCPort3Pch;                        ///< Offset 2425    Type C Connector 3  Port mapping within the PCH controller (If Split mode supported)
  UINT8    UsbCPort3Proterties;                     ///< Offset 2426    Type C Connector 3  Portperties Split Support/Controller(PCH/TBT/CPU)/Root port (vaild for TBT)
  UINT8    UsbTypeCPort4;                           ///< Offset 2427    Type C Connector 4  Port mapping within the controller the port exposed
  UINT8    UsbTypeCPort4Pch;                        ///< Offset 2428    Type C Connector 4  Port mapping within the PCH controller (If Split mode supported)
  UINT8    UsbCPort4Proterties;                     ///< Offset 2429    Type C Connector 4  Portperties Split Support/Controller(PCH/TBT/CPU)/Root port (vaild for TBT)
  UINT8    UsbTypeCPort5;                           ///< Offset 2430    Type C Connector 5  Port mapping within the controller the port exposed
  UINT8    UsbTypeCPort5Pch;                        ///< Offset 2431    Type C Connector 5  Port mapping within the PCH controller (If Split mode supported)
  UINT8    UsbCPort5Proterties;                     ///< Offset 2432    Type C Connector 5  Portperties Split Support/Controller(PCH/TBT/CPU)/Root port (vaild for TBT)
  UINT8    UsbTypeCPort6;                           ///< Offset 2433    Type C Connector 6  Port mapping within the controller the port exposed
  UINT8    UsbTypeCPort6Pch;                        ///< Offset 2434    Type C Connector 6  Port mapping within the PCH controller (If Split mode supported)
  UINT8    UsbCPort6Proterties;                     ///< Offset 2435    Type C Connector 6  Portperties Split Support/Controller(PCH/TBT/CPU)/Root port (vaild for TBT)
  UINT8    SystemTimeAndAlarmSource;                ///< Offset 2436    Select source for System time and alarm
  UINT8    WwanRtd3Option;                          ///< Offset 2437    WWAN RTD3 options
  UINT32   WwanPerstGpio;                           ///< Offset 2438    WWAN PERST Gpio pin
  UINT8    WwanPerstGpioPolarity;                   ///< Offset 2442    WWAN PERST Gpio polarity
  UINT8    PuisEnable;                              ///< Offset 2443    Power Up In Standby mode
  UINT32   PcieSlot1WakeGpio;                       ///< Offset 2444    Pcie Slot Wake Gpio pin
  UINT8    PcieSlot1RpNumber;                       ///< Offset 2448    Pcie Slot Root Port Number
  UINT8    PowermeterDeviceEnable;                  ///< Offset 2449    Enable PowerMeter
  UINT32   PcieSlot2WakeGpio;                       ///< Offset 2450    Pcie Slot 2 Wake Gpio pin
  UINT8    PcieSlot2RpNumber;                       ///< Offset 2454    Pcie Slot 2 Root Port Number
  UINT32   WwanFullCardPowerOffGpio;                ///< Offset 2455    WWAN Full card power off gpio pin
  UINT8    WwanFullCardPowerOffGpioPolarity;        ///< Offset 2459    WWAN Full card power off gpio pin polarity
  UINT32   WwanBbrstGpio;                           ///< Offset 2460    WWAN BBRST Gpio pin
  UINT8    WwanBbrstGpioPolarity;                   ///< Offset 2464    WWAN BBRST Gpio pin polarity
  UINT32   WwanWakeGpio;                            ///< Offset 2465    WWAN Wake Gpio pin
  UINT32   TouchpanelPowerEnableGpio;               ///< Offset 2469    Gpio for touchPaneL 0 power enable
  UINT32   TouchpanelPowerRstGpio;                  ///< Offset 2473    Gpio for touchPaneL 0 Reset
  UINT32   Touchpanel1IrqGpio;                      ///< Offset 2477    Gpio for touchPaneL 1 Interrupt
  UINT32   Touchpanel1PowerEnableGpio;              ///< Offset 2481    Gpio for touchPaneL 1 power enable
  UINT32   Touchpanel1PowerRstGpio;                 ///< Offset 2485    Gpio for touchPaneL 1 Reset
  UINT8    TouchpadIrqGpioPolarity;                 ///< Offset 2489    TouchPaD Interrupt Gpio pin polarity
  UINT8    TouchpanelIrqGpioPolarity;               ///< Offset 2490    TouchPaneL 0 Interrupt Gpio pin polarity
  UINT8    TouchpanelPowerEnableGpioPolarity;       ///< Offset 2491    TouchPaneL 0 power enable Gpio pin polarity
  UINT8    TouchpanelPowerRstGpioPolarity;          ///< Offset 2492    TouchPaneL 0 Reset Gpio pin polarity
  UINT8    Touchpanel1IrqGpioPolarity;              ///< Offset 2493    TouchPaneL 1 Interrupt Gpio pin polarity
  UINT8    Touchpanel1PowerEnableGpioPolarity;      ///< Offset 2494    TouchPaneL 1 power enable Gpio pin polarity
  UINT8    Touchpanel1PowerRstGpioPolarity;         ///< Offset 2495    TouchPaneL 1 Reset Gpio pin polarity
  UINT32   PcieSlot1PowerEnableGpio;                ///< Offset 2496    Pcie Slot 1 Power Enable Gpio pin
  UINT8    PcieSlot1PowerEnableGpioPolarity;        ///< Offset 2500    Pcie Slot 1 Power Enable Gpio pin polarity
  UINT32   PcieSlot1RstGpio;                        ///< Offset 2501    Pcie Slot 1 Rest Gpio pin
  UINT8    PcieSlot1RstGpioPolarity;                ///< Offset 2505    Pcie Slot 1 Rest Gpio pin polarity
  UINT32   PcieSlot2PowerEnableGpio;                ///< Offset 2506    Pcie Slot 2 Power Enable Gpio pin
  UINT8    PcieSlot2PowerEnableGpioPolarity;        ///< Offset 2510    Pcie Slot 2 Power Enable Gpio pin polarity
  UINT32   PcieSlot2RstGpio;                        ///< Offset 2511    Pcie Slot 2 Rest Gpio pin
  UINT8    PcieSlot2RstGpioPolarity;                ///< Offset 2515    Pcie Slot 2 Rest Gpio pin polarity
  UINT32   WlanWakeGpio;                            ///< Offset 2516    WLAN Wake Gpio pin
  UINT8    WlanRootPortNumber;                      ///< Offset 2520    WLAN Root Port Number
  UINT32   SataPortPowerEnableGpio;                 ///< Offset 2521    Sata port Power Enable Gpio pin
  UINT8    SataPortPowerEnableGpioPolarity;         ///< Offset 2525    Sata port Power Enable Gpio pin polarity
  UINT32   PchM2SsdPowerEnableGpio;                 ///< Offset 2526    Pch M.2 SSD Power Enable Gpio pin
  UINT8    PchM2SsdPowerEnableGpioPolarity;         ///< Offset 2530    Pch M.2 SSD Power Enable Gpio pin polarity
  UINT32   PchM2SsdRstGpio;                         ///< Offset 2531    Pch M.2 SSD Reset Gpio pin
  UINT8    PchM2SsdRstGpioPolarity;                 ///< Offset 2535    Pch M.2 SSD Reset Gpio pin polarity
  UINT32   M2Ssd2PowerEnableGpio;                   ///< Offset 2536    PCIe x4 M.2 SSD Power Enable Gpio pin
  UINT8    M2Ssd2PowerEnableGpioPolarity;           ///< Offset 2540    PCIe x4 M.2 SSD Power Enable Gpio pin polarity
  UINT32   M2Ssd2RstGpio;                           ///< Offset 2541    PCIe x4 M.2 SSD Reset Gpio pin
  UINT8    M2Ssd2RstGpioPolarity;                   ///< Offset 2545    PCIe x4 M.2 SSD Reset Gpio pin polarity
  UINT32   M2Ssd3PowerEnableGpio;                   ///< Offset 2546    PCIe x4 M.2 SSD Power Enable Gpio pin
  UINT8    M2Ssd3PowerEnableGpioPolarity;           ///< Offset 2550    PCIe x4 M.2 SSD Power Enable Gpio pin polarity
  UINT32   M2Ssd3RstGpio;                           ///< Offset 2551    PCIe x4 M.2 SSD Reset Gpio pin
  UINT8    M2Ssd3RstGpioPolarity;                   ///< Offset 2555    PCIe x4 M.2 SSD Reset Gpio pin polarity
  UINT32   M2DG2PowerEnableGpio;                    ///< Offset 2556    PCIe x5 M.2 Discrete Graphics Power Enable Gpio Pin
  UINT8    M2DG2PowerEnableGpioPolarity;            ///< Offset 2560    PCIe x5 M.2 Discrete Graphics Power Enable Gpio Pin polarity
  UINT32   M2DG2RstGpio;                            ///< Offset 2561    PCIe x5 M.2 Discrete Graphics Reset Gpio Pin
  UINT8    M2DG2RstGpioPolarity;                    ///< Offset 2565    PCIe x5 M.2 Discrete Graphics Reset Gpio Pin Polarity
  UINT32   M2DG2WakeGpioPin;                        ///< Offset 2566    PEG X8 DG/DG2 Wake Gpio pin
  UINT8    SdevXhciInterfaceNumber1;                ///< Offset 2570    SDEV xHCI Interface Number for device 1
  UINT8    SdevXhciInterfaceNumber2;                ///< Offset 2571    SDEV xHCI Interface Number for device 2
  UINT8    SdevXhciRootPortNumber1;                 ///< Offset 2572    SDEV xHCI Root Port Number for device 1
  UINT8    SdevXhciRootPortNumber2;                 ///< Offset 2573    SDEV xHCI Root Port Number for device 2
  UINT8    TsnPcsEnabled;                           ///< Offset 2574    TSN PCS device Enable
  UINT8    WwanSourceClock;                         ///< Offset 2575    WWAN Source Clock
  UINT8    CpuWakeEnFlag;                           ///< Offset 2576    CPU_WAKE_EN value
  UINT32   CnvExtClock;                             ///< Offset 2577    CNV external 32KHz Clock
  UINT8    WwanRootPortNumber;                      ///< Offset 2581    WWAN Root Port Number
  UINT8    PcieDeviceOnWwanSlot;                    ///< Offset 2582    PCIe Device On WWAN slot
  UINT8    WifiAntGainEnale;                        ///< Offset 2583    Wifi Ant Gain Enable
  UINT8    WifiAntGainChainA1;                      ///< Offset 2584    Ant Gain Table Chain A 2400
  UINT8    WifiAntGainChainA2;                      ///< Offset 2585    Ant Gain Table Chain A 5150-5350
  UINT8    WifiAntGainChainA3;                      ///< Offset 2586    Ant Gain Table Chain A 5350-5470
  UINT8    WifiAntGainChainA4;                      ///< Offset 2587    Ant Gain Table Chain A 5470-5725
  UINT8    WifiAntGainChainA5;                      ///< Offset 2588    Ant Gain Table Chain A 5725-5950
  UINT8    WifiAntGainChainB1;                      ///< Offset 2589    Ant Gain Table Chain B 2400
  UINT8    WifiAntGainChainB2;                      ///< Offset 2590    Ant Gain Table Chain B 5150-5350
  UINT8    WifiAntGainChainB3;                      ///< Offset 2591    Ant Gain Table Chain B 5350-5470
  UINT8    WifiAntGainChainB4;                      ///< Offset 2592    Ant Gain Table Chain B 5470-5725
  UINT8    WifiAntGainChainB5;                      ///< Offset 2593    Ant Gain Table Chain B 5725-5950
  UINT8    WifiSarGeoMappingMode;                   ///< Offset 2594    SAR GEO Mapping Mode    @deprecated.
  UINT8    WifiSarGeoOffset0;                       ///< Offset 2595    0 - DEFAULT            @deprecated.
  UINT8    WifiSarGeoOffset1;                       ///< Offset 2596    1 - FCC                @deprecated.
  UINT8    WifiSarGeoOffset2;                       ///< Offset 2597    2 - TW and alike       @deprecated.
  UINT8    WifiSarGeoOffset3;                       ///< Offset 2598    3 - Canada ISED        @deprecated.
  UINT8    WifiSarGeoOffset4;                       ///< Offset 2599    4 - ETSI 5G8SRD        @deprecated.
  UINT8    WifiSarGeoOffset5;                       ///< Offset 2600    5 - ETSI 5G8Med        @deprecated.
  UINT8    WifiSarGeoOffset6;                       ///< Offset 2601    6 - Japan              @deprecated.
  UINT8    WifiSarGeoOffset7;                       ///< Offset 2602    7 - Brazil             @deprecated.
  UINT8    WifiSarGeoOffset8;                       ///< Offset 2603    8 - ETSI 5G8FCC        @deprecated.
  UINT8    WifiSarGeoOffset9;                       ///< Offset 2604    9 - Indonesia          @deprecated.
  UINT8    WifiSarGeoOffset10;                      ///< Offset 2605    10 - South Korea        @deprecated.
  UINT8    WifiSarGeoOffset11;                      ///< Offset 2606    11 - Chile              @deprecated.
  UINT8    WifiSarGeoOffset12;                      ///< Offset 2607    12 - ETSI 5G8Pass       @deprecated.
  UINT8    WifiSarGeoOffset13;                      ///< Offset 2608    13 - Pakistan           @deprecated.
  UINT8    WifiSarGeoOffset14;                      ///< Offset 2609    14 - Egypt              @deprecated.
  UINT8    WifiSarGeoOffset15;                      ///< Offset 2610    15 - Tunisia            @deprecated.
  UINT8    WifiSarGeoOffset16;                      ///< Offset 2611    16 - China BIOS         @deprecated.
  UINT8    WifiSarGeoOffset17;                      ///< Offset 2612    17 - Russia             @deprecated.
  UINT8    WifiSarGeoOffset18;                      ///< Offset 2613    18 - EU + ETSI + 5G8SRD @deprecated.
  UINT8    WifiActiveChannelSrd;                    ///< Offset 2614    SRD Active Channels Selection
  UINT8    WifiIndonesia5GhzSupport;                ///< Offset 2615    Indonesia 5.15-5.35 GHz Band Support Selection
  UINT32   Dg1VramSRGpio;                           ///< Offset 2616    DG1 VRAM Self Refresh Gpio pin
  UINT32   LpmReqRegAddr;                           ///< Offset 2620    Low Power Mode required register Address
  UINT8    WifiSarGeoOffset19;                      ///< Offset 2624    19 - USA Only           @deprecated.
  UINT8    WifiSarGeoOffset20;                      ///< Offset 2625    20 - EU + ETSI + 5G8DFS @deprecated.
  UINT8    WifiSarGeoOffset21;                      ///< Offset 2626    21 - Qatar              @deprecated.
  UINT32   PegSlot1PwrEnableGpioNo;                 ///< Offset 2627    PEG slot 1 Power Enable Gpio pin
  UINT8    PegSlot1PwrEnableGpioPolarity;           ///< Offset 2631    PEG slot 1 Power Enable Gpio pin polarity
  UINT32   PegSlot1RstGpioNo;                       ///< Offset 2632    PEG slot 1 Reset Gpio pin
  UINT8    PegSlot1RstGpioPolarity;                 ///< Offset 2636    PEG slot 1 Reset Gpio pin polarity
  UINT32   PegSlot1WakeGpioPin;                     ///< Offset 2637    PEG slot 1 Wake Gpio pin
  UINT8    PegSlot1RootPort;                        ///< Offset 2641    PEG slot 1 Root Port
  UINT32   PegSlot2PwrEnableGpioNo;                 ///< Offset 2642    PEG slot 2 Power Enable Gpio pin
  UINT8    PegSlot2PwrEnableGpioPolarity;           ///< Offset 2646    PEG slot 2 Power Enable Gpio pin polarity
  UINT32   PegSlot2RstGpioNo;                       ///< Offset 2647    PEG slot 2 Reset Gpio pin
  UINT8    PegSlot2RstGpioPolarity;                 ///< Offset 2651    PEG slot 2 Reset Gpio pin polarity
  UINT32   PegSlot2WakeGpioPin;                     ///< Offset 2652    PEG slot 2 Wake Gpio pin
  UINT8    PegSlot2RootPort;                        ///< Offset 2656    PEG slot 2 Root Port
  UINT32   PcieSlot3PowerEnableGpio;                ///< Offset 2657    Pcie Slot 3 Power Enable Gpio pin
  UINT8    PcieSlot3PowerEnableGpioPolarity;        ///< Offset 2661    Pcie Slot 3 Power Enable Gpio pin polarity
  UINT32   PcieSlot3RstGpio;                        ///< Offset 2662    Pcie Slot 3 Rest Gpio pin
  UINT8    PcieSlot3RstGpioPolarity;                ///< Offset 2666    Pcie Slot 3 Rest Gpio pin polarity
  UINT32   PcieSlot3WakeGpio;                       ///< Offset 2667    Pcie Slot 3 Wake Gpio pin
  UINT8    PcieSlot3RpNumber;                       ///< Offset 2671    Pcie Slot 3 Root Port Number
  UINT32   PchM2Ssd2PowerEnableGpio;                ///< Offset 2672    Pch M.2 SSD2 Power Enable Gpio pin
  UINT8    PchM2Ssd2PowerEnableGpioPolarity;        ///< Offset 2676    Pch M.2 SSD2 Power Enable Gpio pin polarity
  UINT32   PchM2Ssd2RstGpio;                        ///< Offset 2677    Pch M.2 SSD2 Reset Gpio pin
  UINT8    PchM2Ssd2RstGpioPolarity;                ///< Offset 2681    Pch M.2 SSD2 Reset Gpio pin polarity
  UINT32   PchM2Ssd3PowerEnableGpio;                ///< Offset 2682    Pch M.2 SSD3 Power Enable Gpio pin
  UINT8    PchM2Ssd3PowerEnableGpioPolarity;        ///< Offset 2686    Pch M.2 SSD3 Power Enable Gpio pin polarity
  UINT32   PchM2Ssd3RstGpio;                        ///< Offset 2687    Pch M.2 SSD3 Reset Gpio pin
  UINT8    PchM2Ssd3RstGpioPolarity;                ///< Offset 2691    Pch M.2 SSD3 Reset Gpio pin polarity
  //
  // XTU SMI base address
  //
  UINT32   XtuSmiMemoryAddress;                     ///< Offset 2692    XTU SMI memory in ACPI NVS
  UINT8    DeepestUSBSleepWakeCapability;           ///< Offset 2696    Deepest USB Sleep Wake Capability
  UINT8    WrdsWiFiSarTxPowerSet1Limit11;           ///< Offset 2697    WrdsWiFiSarTxPowerSet1Limit11
  UINT8    WrdsWiFiSarTxPowerSet1Limit12;           ///< Offset 2698    WrdsWiFiSarTxPowerSet1Limit12
  UINT8    WrdsWiFiSarTxPowerSet1Limit13;           ///< Offset 2699    WrdsWiFiSarTxPowerSet1Limit13
  UINT8    WrdsWiFiSarTxPowerSet1Limit14;           ///< Offset 2700    WrdsWiFiSarTxPowerSet1Limit14
  UINT8    WrdsWiFiSarTxPowerSet1Limit15;           ///< Offset 2701    WrdsWiFiSarTxPowerSet1Limit15
  UINT8    WrdsWiFiSarTxPowerSet1Limit16;           ///< Offset 2702    WrdsWiFiSarTxPowerSet1Limit16
  UINT8    WrdsWiFiSarTxPowerSet1Limit17;           ///< Offset 2703    WrdsWiFiSarTxPowerSet1Limit17
  UINT8    WrdsWiFiSarTxPowerSet1Limit18;           ///< Offset 2704    WrdsWiFiSarTxPowerSet1Limit18
  UINT8    WrdsWiFiSarTxPowerSet1Limit19;           ///< Offset 2705    WrdsWiFiSarTxPowerSet1Limit19
  UINT8    WrdsWiFiSarTxPowerSet1Limit20;           ///< Offset 2706    WrdsWiFiSarTxPowerSet1Limit20
  UINT8    WrdsWiFiSarTxPowerSet1Limit21;           ///< Offset 2707    WrdsWiFiSarTxPowerSet1Limit21
  UINT8    WrdsWiFiSarTxPowerSet1Limit22;           ///< Offset 2708    WrdsWiFiSarTxPowerSet1Limit22
  UINT8    EwrdWiFiSarTxPowerSet2Limit11;           ///< Offset 2709    EwrdWiFiSarTxPowerSet2Limit11
  UINT8    EwrdWiFiSarTxPowerSet2Limit12;           ///< Offset 2710    EwrdWiFiSarTxPowerSet2Limit12
  UINT8    EwrdWiFiSarTxPowerSet2Limit13;           ///< Offset 2711    EwrdWiFiSarTxPowerSet2Limit13
  UINT8    EwrdWiFiSarTxPowerSet2Limit14;           ///< Offset 2712    EwrdWiFiSarTxPowerSet2Limit14
  UINT8    EwrdWiFiSarTxPowerSet2Limit15;           ///< Offset 2713    EwrdWiFiSarTxPowerSet2Limit15
  UINT8    EwrdWiFiSarTxPowerSet2Limit16;           ///< Offset 2714    EwrdWiFiSarTxPowerSet2Limit16
  UINT8    EwrdWiFiSarTxPowerSet2Limit17;           ///< Offset 2715    EwrdWiFiSarTxPowerSet2Limit17
  UINT8    EwrdWiFiSarTxPowerSet2Limit18;           ///< Offset 2716    EwrdWiFiSarTxPowerSet2Limit18
  UINT8    EwrdWiFiSarTxPowerSet2Limit19;           ///< Offset 2717    EwrdWiFiSarTxPowerSet2Limit19
  UINT8    EwrdWiFiSarTxPowerSet2Limit20;           ///< Offset 2718    EwrdWiFiSarTxPowerSet2Limit20
  UINT8    EwrdWiFiSarTxPowerSet2Limit21;           ///< Offset 2719    EwrdWiFiSarTxPowerSet2Limit21
  UINT8    EwrdWiFiSarTxPowerSet2Limit22;           ///< Offset 2720    EwrdWiFiSarTxPowerSet2Limit22
  UINT8    EwrdWiFiSarTxPowerSet3Limit11;           ///< Offset 2721    EwrdWiFiSarTxPowerSet3Limit11
  UINT8    EwrdWiFiSarTxPowerSet3Limit12;           ///< Offset 2722    EwrdWiFiSarTxPowerSet3Limit12
  UINT8    EwrdWiFiSarTxPowerSet3Limit13;           ///< Offset 2723    EwrdWiFiSarTxPowerSet3Limit13
  UINT8    EwrdWiFiSarTxPowerSet3Limit14;           ///< Offset 2724    EwrdWiFiSarTxPowerSet3Limit14
  UINT8    EwrdWiFiSarTxPowerSet3Limit15;           ///< Offset 2725    EwrdWiFiSarTxPowerSet3Limit15
  UINT8    EwrdWiFiSarTxPowerSet3Limit16;           ///< Offset 2726    EwrdWiFiSarTxPowerSet3Limit16
  UINT8    EwrdWiFiSarTxPowerSet3Limit17;           ///< Offset 2727    EwrdWiFiSarTxPowerSet3Limit17
  UINT8    EwrdWiFiSarTxPowerSet3Limit18;           ///< Offset 2728    EwrdWiFiSarTxPowerSet3Limit18
  UINT8    EwrdWiFiSarTxPowerSet3Limit19;           ///< Offset 2729    EwrdWiFiSarTxPowerSet3Limit19
  UINT8    EwrdWiFiSarTxPowerSet3Limit20;           ///< Offset 2730    EwrdWiFiSarTxPowerSet3Limit20
  UINT8    EwrdWiFiSarTxPowerSet3Limit21;           ///< Offset 2731    EwrdWiFiSarTxPowerSet3Limit21
  UINT8    EwrdWiFiSarTxPowerSet3Limit22;           ///< Offset 2732    EwrdWiFiSarTxPowerSet3Limit22
  UINT8    EwrdWiFiSarTxPowerSet4Limit11;           ///< Offset 2733    EwrdWiFiSarTxPowerSet4Limit11
  UINT8    EwrdWiFiSarTxPowerSet4Limit12;           ///< Offset 2734    EwrdWiFiSarTxPowerSet4Limit12
  UINT8    EwrdWiFiSarTxPowerSet4Limit13;           ///< Offset 2735    EwrdWiFiSarTxPowerSet4Limit13
  UINT8    EwrdWiFiSarTxPowerSet4Limit14;           ///< Offset 2736    EwrdWiFiSarTxPowerSet4Limit14
  UINT8    EwrdWiFiSarTxPowerSet4Limit15;           ///< Offset 2737    EwrdWiFiSarTxPowerSet4Limit15
  UINT8    EwrdWiFiSarTxPowerSet4Limit16;           ///< Offset 2738    EwrdWiFiSarTxPowerSet4Limit16
  UINT8    EwrdWiFiSarTxPowerSet4Limit17;           ///< Offset 2739    EwrdWiFiSarTxPowerSet4Limit17
  UINT8    EwrdWiFiSarTxPowerSet4Limit18;           ///< Offset 2740    EwrdWiFiSarTxPowerSet4Limit18
  UINT8    EwrdWiFiSarTxPowerSet4Limit19;           ///< Offset 2741    EwrdWiFiSarTxPowerSet4Limit19
  UINT8    EwrdWiFiSarTxPowerSet4Limit20;           ///< Offset 2742    EwrdWiFiSarTxPowerSet4Limit20
  UINT8    EwrdWiFiSarTxPowerSet4Limit21;           ///< Offset 2743    EwrdWiFiSarTxPowerSet4Limit21
  UINT8    EwrdWiFiSarTxPowerSet4Limit22;           ///< Offset 2744    EwrdWiFiSarTxPowerSet4Limit22
  UINT8    WgdsWiFiSarDeltaGroup1PowerMax3;         ///< Offset 2745    WgdsWiFiSarDeltaGroup1PowerMax3
  UINT8    WgdsWiFiSarDeltaGroup1PowerChainA3;      ///< Offset 2746    WgdsWiFiSarDeltaGroup1PowerChainA3
  UINT8    WgdsWiFiSarDeltaGroup1PowerChainB3;      ///< Offset 2747    WgdsWiFiSarDeltaGroup1PowerChainB3
  UINT8    WgdsWiFiSarDeltaGroup2PowerMax3;         ///< Offset 2748    WgdsWiFiSarDeltaGroup2PowerMax3
  UINT8    WgdsWiFiSarDeltaGroup2PowerChainA3;      ///< Offset 2749    WgdsWiFiSarDeltaGroup2PowerChainA3
  UINT8    WgdsWiFiSarDeltaGroup2PowerChainB3;      ///< Offset 2750    WgdsWiFiSarDeltaGroup2PowerChainB3
  UINT8    WgdsWiFiSarDeltaGroup3PowerMax3;         ///< Offset 2751    WgdsWiFiSarDeltaGroup3PowerMax3
  UINT8    WgdsWiFiSarDeltaGroup3PowerChainA3;      ///< Offset 2752    WgdsWiFiSarDeltaGroup3PowerChainA3
  UINT8    WgdsWiFiSarDeltaGroup3PowerChainB3;      ///< Offset 2753    WgdsWiFiSarDeltaGroup3PowerChainB3
  UINT8    WifiAntGainChainA6;                      ///< Offset 2754    Ant Gain Table Chain A 5945-6165git
  UINT8    WifiAntGainChainA7;                      ///< Offset 2755    Ant Gain Table Chain A 6165-6405
  UINT8    WifiAntGainChainA8;                      ///< Offset 2756    Ant Gain Table Chain A 6405-6525
  UINT8    WifiAntGainChainA9;                      ///< Offset 2757    Ant Gain Table Chain A 6525-6705
  UINT8    WifiAntGainChainA10;                     ///< Offset 2758    Ant Gain Table Chain A 6705-6865
  UINT8    WifiAntGainChainA11;                     ///< Offset 2759    Ant Gain Table Chain A 6865-7105
  UINT8    WifiAntGainChainB6;                      ///< Offset 2760    Ant Gain Table Chain B 5945-6165
  UINT8    WifiAntGainChainB7;                      ///< Offset 2761    Ant Gain Table Chain B 6165-6405
  UINT8    WifiAntGainChainB8;                      ///< Offset 2762    Ant Gain Table Chain B 6405-6525
  UINT8    WifiAntGainChainB9;                      ///< Offset 2763    Ant Gain Table Chain B 6525-6705
  UINT8    WifiAntGainChainB10;                     ///< Offset 2764    Ant Gain Table Chain B 6705-6865
  UINT8    WifiAntGainChainB11;                     ///< Offset 2765    Ant Gain Table Chain B 6865-7105
  UINT32   WifiUltraHighBandSupport;                ///< Offset 2766    WifiUltraHighBandSupport
  UINT8    AxSettingUkraine;                        ///< Offset 2770    11AxSettingUkraine
  UINT8    AxModeUkraine;                           ///< Offset 2771    11AxModeUkraine
  UINT8    AxSettingRussia;                         ///< Offset 2772    11AxSettingRussia
  UINT8    AxModeRussia;                            ///< Offset 2773    11AxModeRussia
  UINT8    WrdsCdbWiFiSarTxPowerSet1Limit1;         ///< Offset 2774    WrdsCdbWiFiSarTxPowerSet1Limit1
  UINT8    WrdsCdbWiFiSarTxPowerSet1Limit2;         ///< Offset 2775    WrdsCdbWiFiSarTxPowerSet1Limit2
  UINT8    WrdsCdbWiFiSarTxPowerSet1Limit3;         ///< Offset 2776    WrdsCdbWiFiSarTxPowerSet1Limit3
  UINT8    WrdsCdbWiFiSarTxPowerSet1Limit4;         ///< Offset 2777    WrdsCdbWiFiSarTxPowerSet1Limit4
  UINT8    WrdsCdbWiFiSarTxPowerSet1Limit5;         ///< Offset 2778    WrdsCdbWiFiSarTxPowerSet1Limit5
  UINT8    WrdsCdbWiFiSarTxPowerSet1Limit6;         ///< Offset 2779    WrdsCdbWiFiSarTxPowerSet1Limit6
  UINT8    WrdsCdbWiFiSarTxPowerSet1Limit7;         ///< Offset 2780    WrdsCdbWiFiSarTxPowerSet1Limit7
  UINT8    WrdsCdbWiFiSarTxPowerSet1Limit8;         ///< Offset 2781    WrdsCdbWiFiSarTxPowerSet1Limit8
  UINT8    WrdsCdbWiFiSarTxPowerSet1Limit9;         ///< Offset 2782    WrdsCdbWiFiSarTxPowerSet1Limit9
  UINT8    WrdsCdbWiFiSarTxPowerSet1Limit10;        ///< Offset 2783    WrdsCdbWiFiSarTxPowerSet1Limit10
  UINT8    WrdsCdbWiFiSarTxPowerSet1Limit11;        ///< Offset 2784    WrdsCdbWiFiSarTxPowerSet1Limit11
  UINT8    WrdsCdbWiFiSarTxPowerSet1Limit12;        ///< Offset 2785    WrdsCdbWiFiSarTxPowerSet1Limit12
  UINT8    WrdsCdbWiFiSarTxPowerSet1Limit13;        ///< Offset 2786    WrdsCdbWiFiSarTxPowerSet1Limit13
  UINT8    WrdsCdbWiFiSarTxPowerSet1Limit14;        ///< Offset 2787    WrdsCdbWiFiSarTxPowerSet1Limit14
  UINT8    WrdsCdbWiFiSarTxPowerSet1Limit15;        ///< Offset 2788    WrdsCdbWiFiSarTxPowerSet1Limit15
  UINT8    WrdsCdbWiFiSarTxPowerSet1Limit16;        ///< Offset 2789    WrdsCdbWiFiSarTxPowerSet1Limit16
  UINT8    WrdsCdbWiFiSarTxPowerSet1Limit17;        ///< Offset 2790    WrdsCdbWiFiSarTxPowerSet1Limit17
  UINT8    WrdsCdbWiFiSarTxPowerSet1Limit18;        ///< Offset 2791    WrdsCdbWiFiSarTxPowerSet1Limit18
  UINT8    WrdsCdbWiFiSarTxPowerSet1Limit19;        ///< Offset 2792    WrdsCdbWiFiSarTxPowerSet1Limit19
  UINT8    WrdsCdbWiFiSarTxPowerSet1Limit20;        ///< Offset 2793    WrdsCdbWiFiSarTxPowerSet1Limit20
  UINT8    WrdsCdbWiFiSarTxPowerSet1Limit21;        ///< Offset 2794    WrdsCdbWiFiSarTxPowerSet1Limit21
  UINT8    WrdsCdbWiFiSarTxPowerSet1Limit22;        ///< Offset 2795    WrdsCdbWiFiSarTxPowerSet1Limit22
  UINT8    EwrdCdbWiFiSarTxPowerSet2Limit1;         ///< Offset 2796    EwrdCdbWiFiSarTxPowerSet2Limit1
  UINT8    EwrdCdbWiFiSarTxPowerSet2Limit2;         ///< Offset 2797    EwrdCdbWiFiSarTxPowerSet2Limit2
  UINT8    EwrdCdbWiFiSarTxPowerSet2Limit3;         ///< Offset 2798    EwrdCdbWiFiSarTxPowerSet2Limit3
  UINT8    EwrdCdbWiFiSarTxPowerSet2Limit4;         ///< Offset 2799    EwrdCdbWiFiSarTxPowerSet2Limit4
  UINT8    EwrdCdbWiFiSarTxPowerSet2Limit5;         ///< Offset 2800    EwrdCdbWiFiSarTxPowerSet2Limit5
  UINT8    EwrdCdbWiFiSarTxPowerSet2Limit6;         ///< Offset 2801    EwrdCdbWiFiSarTxPowerSet2Limit6
  UINT8    EwrdCdbWiFiSarTxPowerSet2Limit7;         ///< Offset 2802    EwrdCdbWiFiSarTxPowerSet2Limit7
  UINT8    EwrdCdbWiFiSarTxPowerSet2Limit8;         ///< Offset 2803    EwrdCdbWiFiSarTxPowerSet2Limit8
  UINT8    EwrdCdbWiFiSarTxPowerSet2Limit9;         ///< Offset 2804    EwrdCdbWiFiSarTxPowerSet2Limit9
  UINT8    EwrdCdbWiFiSarTxPowerSet2Limit10;        ///< Offset 2805    EwrdCdbWiFiSarTxPowerSet2Limit10
  UINT8    EwrdCdbWiFiSarTxPowerSet2Limit11;        ///< Offset 2806    EwrdCdbWiFiSarTxPowerSet2Limit11
  UINT8    EwrdCdbWiFiSarTxPowerSet2Limit12;        ///< Offset 2807    EwrdCdbWiFiSarTxPowerSet2Limit12
  UINT8    EwrdCdbWiFiSarTxPowerSet2Limit13;        ///< Offset 2808    EwrdCdbWiFiSarTxPowerSet2Limit13
  UINT8    EwrdCdbWiFiSarTxPowerSet2Limit14;        ///< Offset 2809    EwrdCdbWiFiSarTxPowerSet2Limit14
  UINT8    EwrdCdbWiFiSarTxPowerSet2Limit15;        ///< Offset 2810    EwrdCdbWiFiSarTxPowerSet2Limit15
  UINT8    EwrdCdbWiFiSarTxPowerSet2Limit16;        ///< Offset 2811    EwrdCdbWiFiSarTxPowerSet2Limit16
  UINT8    EwrdCdbWiFiSarTxPowerSet2Limit17;        ///< Offset 2812    EwrdCdbWiFiSarTxPowerSet2Limit17
  UINT8    EwrdCdbWiFiSarTxPowerSet2Limit18;        ///< Offset 2813    EwrdCdbWiFiSarTxPowerSet2Limit18
  UINT8    EwrdCdbWiFiSarTxPowerSet2Limit19;        ///< Offset 2814    EwrdCdbWiFiSarTxPowerSet2Limit19
  UINT8    EwrdCdbWiFiSarTxPowerSet2Limit20;        ///< Offset 2815    EwrdCdbWiFiSarTxPowerSet2Limit20
  UINT8    EwrdCdbWiFiSarTxPowerSet2Limit21;        ///< Offset 2816    EwrdCdbWiFiSarTxPowerSet2Limit21
  UINT8    EwrdCdbWiFiSarTxPowerSet2Limit22;        ///< Offset 2817    EwrdCdbWiFiSarTxPowerSet2Limit22
  UINT8    EwrdCdbWiFiSarTxPowerSet3Limit1;         ///< Offset 2818    EwrdCdbWiFiSarTxPowerSet3Limit1
  UINT8    EwrdCdbWiFiSarTxPowerSet3Limit2;         ///< Offset 2819    EwrdCdbWiFiSarTxPowerSet3Limit2
  UINT8    EwrdCdbWiFiSarTxPowerSet3Limit3;         ///< Offset 2820    EwrdCdbWiFiSarTxPowerSet3Limit3
  UINT8    EwrdCdbWiFiSarTxPowerSet3Limit4;         ///< Offset 2821    EwrdCdbWiFiSarTxPowerSet3Limit4
  UINT8    EwrdCdbWiFiSarTxPowerSet3Limit5;         ///< Offset 2822    EwrdCdbWiFiSarTxPowerSet3Limit5
  UINT8    EwrdCdbWiFiSarTxPowerSet3Limit6;         ///< Offset 2823    EwrdCdbWiFiSarTxPowerSet3Limit6
  UINT8    EwrdCdbWiFiSarTxPowerSet3Limit7;         ///< Offset 2824    EwrdCdbWiFiSarTxPowerSet3Limit7
  UINT8    EwrdCdbWiFiSarTxPowerSet3Limit8;         ///< Offset 2825    EwrdCdbWiFiSarTxPowerSet3Limit8
  UINT8    EwrdCdbWiFiSarTxPowerSet3Limit9;         ///< Offset 2826    EwrdCdbWiFiSarTxPowerSet3Limit9
  UINT8    EwrdCdbWiFiSarTxPowerSet3Limit10;        ///< Offset 2827    EwrdCdbWiFiSarTxPowerSet3Limit10
  UINT8    EwrdCdbWiFiSarTxPowerSet3Limit11;        ///< Offset 2828    EwrdCdbWiFiSarTxPowerSet3Limit11
  UINT8    EwrdCdbWiFiSarTxPowerSet3Limit12;        ///< Offset 2829    EwrdCdbWiFiSarTxPowerSet3Limit12
  UINT8    EwrdCdbWiFiSarTxPowerSet3Limit13;        ///< Offset 2830    EwrdCdbWiFiSarTxPowerSet3Limit13
  UINT8    EwrdCdbWiFiSarTxPowerSet3Limit14;        ///< Offset 2831    EwrdCdbWiFiSarTxPowerSet3Limit14
  UINT8    EwrdCdbWiFiSarTxPowerSet3Limit15;        ///< Offset 2832    EwrdCdbWiFiSarTxPowerSet3Limit15
  UINT8    EwrdCdbWiFiSarTxPowerSet3Limit16;        ///< Offset 2833    EwrdCdbWiFiSarTxPowerSet3Limit16
  UINT8    EwrdCdbWiFiSarTxPowerSet3Limit17;        ///< Offset 2834    EwrdCdbWiFiSarTxPowerSet3Limit17
  UINT8    EwrdCdbWiFiSarTxPowerSet3Limit18;        ///< Offset 2835    EwrdCdbWiFiSarTxPowerSet3Limit18
  UINT8    EwrdCdbWiFiSarTxPowerSet3Limit19;        ///< Offset 2836    EwrdCdbWiFiSarTxPowerSet3Limit19
  UINT8    EwrdCdbWiFiSarTxPowerSet3Limit20;        ///< Offset 2837    EwrdCdbWiFiSarTxPowerSet3Limit20
  UINT8    EwrdCdbWiFiSarTxPowerSet3Limit21;        ///< Offset 2838    EwrdCdbWiFiSarTxPowerSet3Limit21
  UINT8    EwrdCdbWiFiSarTxPowerSet3Limit22;        ///< Offset 2839    EwrdCdbWiFiSarTxPowerSet3Limit22
  UINT8    EwrdCdbWiFiSarTxPowerSet4Limit1;         ///< Offset 2840    EwrdCdbWiFiSarTxPowerSet4Limit1
  UINT8    EwrdCdbWiFiSarTxPowerSet4Limit2;         ///< Offset 2841    EwrdCdbWiFiSarTxPowerSet4Limit2
  UINT8    EwrdCdbWiFiSarTxPowerSet4Limit3;         ///< Offset 2842    EwrdCdbWiFiSarTxPowerSet4Limit3
  UINT8    EwrdCdbWiFiSarTxPowerSet4Limit4;         ///< Offset 2843    EwrdCdbWiFiSarTxPowerSet4Limit4
  UINT8    EwrdCdbWiFiSarTxPowerSet4Limit5;         ///< Offset 2844    EwrdCdbWiFiSarTxPowerSet4Limit5
  UINT8    EwrdCdbWiFiSarTxPowerSet4Limit6;         ///< Offset 2845    EwrdCdbWiFiSarTxPowerSet4Limit6
  UINT8    EwrdCdbWiFiSarTxPowerSet4Limit7;         ///< Offset 2846    EwrdCdbWiFiSarTxPowerSet4Limit7
  UINT8    EwrdCdbWiFiSarTxPowerSet4Limit8;         ///< Offset 2847    EwrdCdbWiFiSarTxPowerSet4Limit8
  UINT8    EwrdCdbWiFiSarTxPowerSet4Limit9;         ///< Offset 2848    EwrdCdbWiFiSarTxPowerSet4Limit9
  UINT8    EwrdCdbWiFiSarTxPowerSet4Limit10;        ///< Offset 2849    EwrdCdbWiFiSarTxPowerSet4Limit10
  UINT8    EwrdCdbWiFiSarTxPowerSet4Limit11;        ///< Offset 2850    EwrdCdbWiFiSarTxPowerSet4Limit11
  UINT8    EwrdCdbWiFiSarTxPowerSet4Limit12;        ///< Offset 2851    EwrdCdbWiFiSarTxPowerSet4Limit12
  UINT8    EwrdCdbWiFiSarTxPowerSet4Limit13;        ///< Offset 2852    EwrdCdbWiFiSarTxPowerSet4Limit13
  UINT8    EwrdCdbWiFiSarTxPowerSet4Limit14;        ///< Offset 2853    EwrdCdbWiFiSarTxPowerSet4Limit14
  UINT8    EwrdCdbWiFiSarTxPowerSet4Limit15;        ///< Offset 2854    EwrdCdbWiFiSarTxPowerSet4Limit15
  UINT8    EwrdCdbWiFiSarTxPowerSet4Limit16;        ///< Offset 2855    EwrdCdbWiFiSarTxPowerSet4Limit16
  UINT8    EwrdCdbWiFiSarTxPowerSet4Limit17;        ///< Offset 2856    EwrdCdbWiFiSarTxPowerSet4Limit17
  UINT8    EwrdCdbWiFiSarTxPowerSet4Limit18;        ///< Offset 2857    EwrdCdbWiFiSarTxPowerSet4Limit18
  UINT8    EwrdCdbWiFiSarTxPowerSet4Limit19;        ///< Offset 2858    EwrdCdbWiFiSarTxPowerSet4Limit19
  UINT8    EwrdCdbWiFiSarTxPowerSet4Limit20;        ///< Offset 2859    EwrdCdbWiFiSarTxPowerSet4Limit20
  UINT8    EwrdCdbWiFiSarTxPowerSet4Limit21;        ///< Offset 2860    EwrdCdbWiFiSarTxPowerSet4Limit21
  UINT8    EwrdCdbWiFiSarTxPowerSet4Limit22;        ///< Offset 2861    EwrdCdbWiFiSarTxPowerSet4Limit22
  UINT32   WifiPhyFilterConfigA;                    ///< Offset 2862    Platform PHY Config Chain A  Deprecated
  UINT32   WifiPhyFilterConfigB;                    ///< Offset 2866    Platform PHY Config Chain B  Deprecated
  UINT32   WifiPhyFilterConfigC;                    ///< Offset 2870    Platform PHY Config Chain C  Deprecated
  UINT32   WifiPhyFilterConfigD;                    ///< Offset 2874    Platform PHY Config Chain D  Deprecated
  UINT8    MipiCamLink0DD_LanesClkDivision;         ///< Offset 2878    LanesClockDivision
  UINT8    MipiCamLink1DD_LanesClkDivision;         ///< Offset 2879    LanesClockDivision
  UINT8    MipiCamLink2DD_LanesClkDivision;         ///< Offset 2880    LanesClockDivision
  UINT8    MipiCamLink3DD_LanesClkDivision;         ///< Offset 2881    LanesClockDivision
  UINT8    MipiCamLink4DD_LanesClkDivision;         ///< Offset 2882    LanesClockDivision
  UINT8    MipiCamLink5DD_LanesClkDivision;         ///< Offset 2883    LanesClockDivision
  UINT32   POVP;                                    ///< Offset 2884    USBC_PSON_OVERRIDE_N Input pin
  UINT32   PSG1;                                    ///< Offset 2888    S0IX_EN_TRY_REQ Output pin
  UINT32   PSG2;                                    ///< Offset 2892    S0IX_EN_TRY_ACK Input pin
  UINT32   PPOE;                                    ///< Offset 2896    Option to enable/disable TCSS PD PS_ON
  UINT8    EnableFan2Device;                        ///< Offset 2900    EnableFan2Device
  UINT8    EnableFan3Device;                        ///< Offset 2901    EnableFan3Device
  UINT8    EnableDgpuParticipant;                   ///< Offset 2902    EnableDgpuParticipant
  UINT8    WifiTASSelection;                        ///< Offset 2903    WifiTASSelection
  UINT8    WifiTASListEntries;                      ///< Offset 2904    WifiTASListEntries
  UINT16   WTASBlockedListEntry1;                   ///< Offset 2905    WTASBlockedListEntry1
  UINT16   WTASBlockedListEntry2;                   ///< Offset 2907    WTASBlockedListEntry2
  UINT16   WTASBlockedListEntry3;                   ///< Offset 2909    WTASBlockedListEntry3
  UINT16   WTASBlockedListEntry4;                   ///< Offset 2911    WTASBlockedListEntry4
  UINT16   WTASBlockedListEntry5;                   ///< Offset 2913    WTASBlockedListEntry5
  UINT16   WTASBlockedListEntry6;                   ///< Offset 2915    WTASBlockedListEntry6
  UINT16   WTASBlockedListEntry7;                   ///< Offset 2917    WTASBlockedListEntry7
  UINT16   WTASBlockedListEntry8;                   ///< Offset 2919    WTASBlockedListEntry8
  UINT16   WTASBlockedListEntry9;                   ///< Offset 2921    WTASBlockedListEntry9
  UINT16   WTASBlockedListEntry10;                  ///< Offset 2923    WTASBlockedListEntry10
  UINT16   WTASBlockedListEntry11;                  ///< Offset 2925    WTASBlockedListEntry11
  UINT16   WTASBlockedListEntry12;                  ///< Offset 2927    WTASBlockedListEntry12
  UINT16   WTASBlockedListEntry13;                  ///< Offset 2929    WTASBlockedListEntry13
  UINT16   WTASBlockedListEntry14;                  ///< Offset 2931    WTASBlockedListEntry14
  UINT16   WTASBlockedListEntry15;                  ///< Offset 2933    WTASBlockedListEntry15
  UINT16   WTASBlockedListEntry16;                  ///< Offset 2935    WTASBlockedListEntry16
  UINT8    CvfSupport;                              ///< Offset 2937    To enable/disable Cloverfalls(CVF)
  UINT8    CvfUsbPort;                              ///< Offset 2938    CVF USB port number
  UINT64   DgBaseAddress;                           ///< Offset 2939    DG PCIe Base Address
  UINT32   DgOpRegionAddress;                       ///< Offset 2947    DG OpRegion Base Address
  UINT8    BtTileMode;                              ///< Offset 2951    BT Tile Mode
  UINT8    TileS0;                                  ///< Offset 2952    The activity of Tile in S0
  UINT8    TileS0ix;                                ///< Offset 2953    The activity of Tile in S0ix
  UINT8    TileS4;                                  ///< Offset 2954    The activity of Tile in S4
  UINT8    TileS5;                                  ///< Offset 2955    The activity of Tile in S5
  UINT8    SpecialLedConfig;                        ///< Offset 2956    Special LED Config
  UINT16   LedDuration;                             ///< Offset 2957    LED Duration
  UINT8    AirplaneMode;                            ///< Offset 2959    Tile Airplane Mode
  UINT8    StorageRtd3Support;                      ///< Offset 2960    Storage Runtime D3 Support
  UINT8    PmaxDevice;                              ///< Offset 2961    PMAX Device
  UINT8    PmaxAudioCodec;                          ///< Offset 2962    PMAX Audio Codec
  UINT8    PmaxWfCamera;                            ///< Offset 2963    PMAX WF Camera
  UINT8    PmaxUfCamera;                            ///< Offset 2964    PMAX UF Camera
  UINT8    PmaxFlashDevice;                         ///< Offset 2965    PMAX Flash Device
  UINT8    Reserved25;                              ///< Offset 2966    Reserved to remove Board Rework Option for ADLP/M
  UINT8    WwanEnable;                              ///< Offset 2967    WWAN Enable
  UINT8    WwanFwFlashDevice;                       ///< Offset 2968    Enable WWAN Firmware Flash Device
  UINT16   WwanTOn2ResDelayMs;                      ///< Offset 2969    FCPO# to BBRST# delay time during WWAN ON
  UINT16   WwanTOnRes2PerDelayMs;                   ///< Offset 2971    BBRST# to PERST# delay time during WWAN ON
  UINT16   WwanTOnPer2PdsDelayMs;                   ///< Offset 2973    The delay between de-assertion of PERST# to change of PDS state from 0 to 1 during WWAN ON
  UINT16   WwanTRes2OffDelayMs;                     ///< Offset 2975    BBRST# to FCPO# delay time during WWAN OFF
  UINT16   WwanTOffDisDelayMs;                      ///< Offset 2977    Rigorous shutdown time
  UINT16   WwanTResTogDelayMs;                      ///< Offset 2979    The delay between assertion and de-assertion BBRST# during FLDR
  UINT16   WwanTRes2PdsDelayMs;                     ///< Offset 2981    The delay between de-assertion of BBRST# and change of PDS state from 0 to 1 after FLDR
  UINT32   LidSwitchWakeGpio;                       ///< Offset 2983    Lid Switch Wake Gpio
  UINT8    PreBootCmMode;                           ///< Offset 2987    USB4 CM mode information in Pre-Boot
  UINT8    CmTbtMask;                               ///< Offset 2988    Indicate enabled dTBT and iTBT for CM
  UINT8    ClosedLidWovLighting;                    ///< Offset 2989    Closed Lid WoV LED Lighting Support Enable
  UINT8    ClwlI2cController;                       ///< Offset 2990    Closed Lid WoV LED Lighting I2C Controller Number
  UINT8    ClwlI2cSlaveAddress;                     ///< Offset 2991    Closed Lid WoV LED Lighting I2C Peripheral address
  // Type-C NVS variables for TCSS Port 7-10
  UINT8    UsbTypeCPort7;                           ///< Offset 2992    Type C Connector 7  Port mapping within the controller the port exposed
  UINT8    UsbTypeCPort7Pch;                        ///< Offset 2993    Type C Connector 7  Port mapping within the PCH controller (If Split mode supported)
  UINT8    UsbCPort7Proterties;                     ///< Offset 2994    Type C Connector 7  Portperties Split Support/Controller(PCH/TBT/CPU)/Root port (vaild for TBT)
  UINT8    UsbTypeCPort8;                           ///< Offset 2995    Type C Connector 8  Port mapping within the controller the port exposed
  UINT8    UsbTypeCPort8Pch;                        ///< Offset 2996    Type C Connector 8  Port mapping within the PCH controller (If Split mode supported)
  UINT8    UsbCPort8Proterties;                     ///< Offset 2997    Type C Connector 8  Portperties Split Support/Controller(PCH/TBT/CPU)/Root port (vaild for TBT)
  UINT8    UsbTypeCPort9;                           ///< Offset 2998    Type C Connector 9  Port mapping within the controller the port exposed
  UINT8    UsbTypeCPort9Pch;                        ///< Offset 2999    Type C Connector 9  Port mapping within the PCH controller (If Split mode supported)
  UINT8    UsbCPort9Proterties;                     ///< Offset 3000    Type C Connector 9  Portperties Split Support/Controller(PCH/TBT/CPU)/Root port (vaild for TBT)
  UINT8    UsbTypeCPortA;                           ///< Offset 3001    Type C Connector A  Port mapping within the controller the port exposed
  UINT8    UsbTypeCPortAPch;                        ///< Offset 3002    Type C Connector A  Port mapping within the PCH controller (If Split mode supported)
  UINT8    UsbCPortAProterties;                     ///< Offset 3003    Type C Connector A  Portperties Split Support/Controller(PCH/TBT/CPU)/Root port (vaild for TBT)
  // UCSI/UCMX Driver Support. 0: Force Disable, 1: UCSI Driver support, 2: UCMX Driver support.
  UINT8    UsbCPort1UcxxSupport;                    ///< Offset 3004    Type C Connector 1  UCSI/UCMX Driver Support Enable/ Force Disable.
  UINT8    UsbCPort2UcxxSupport;                    ///< Offset 3005    Type C Connector 2  UCSI/UCMX Driver Support Enable/ Force Disable.
  UINT8    UsbCPort3UcxxSupport;                    ///< Offset 3006    Type C Connector 3  UCSI/UCMX Driver Support Enable/ Force Disable.
  UINT8    UsbCPort4UcxxSupport;                    ///< Offset 3007    Type C Connector 4  UCSI/UCMX Driver Support Enable/ Force Disable.
  UINT8    UsbCPort5UcxxSupport;                    ///< Offset 3008    Type C Connector 5  UCSI/UCMX Driver Support Enable/ Force Disable.
  UINT8    UsbCPort6UcxxSupport;                    ///< Offset 3009    Type C Connector 6  UCSI/UCMX Driver Support Enable/ Force Disable.
  UINT8    UsbCPort7UcxxSupport;                    ///< Offset 3010    Type C Connector 7  UCSI/UCMX Driver Support Enable/ Force Disable.
  UINT8    UsbCPort8UcxxSupport;                    ///< Offset 3011    Type C Connector 8  UCSI/UCMX Driver Support Enable/ Force Disable.
  UINT8    UsbCPort9UcxxSupport;                    ///< Offset 3012    Type C Connector 9  UCSI/UCMX Driver Support Enable/ Force Disable.
  UINT8    UsbCPortAUcxxSupport;                    ///< Offset 3013    Type C Connector A  UCSI/UCMX Driver Support Enable/ Force Disable.
  UINT32   FoxLanWakeGpio;                          ///< Offset 3014    Foxville I225 Wake Gpio pin
  UINT32   FoxLanRstGpio;                           ///< Offset 3018    Foxville I225 Reset Gpio pin
  UINT8    FoxLanRstGpioPolarity;                   ///< Offset 3022    Foxville I225 Reset Gpio pin polarity
  UINT32   FoxLanDisableNGpio;                      ///< Offset 3023    Foxville I225 Disable N Gpio pin
  UINT8    FoxLanDisableNGpioPolarity;              ///< Offset 3027    Foxville I225 Disable N Gpio pin polarity
  UINT8    FoxLanSupport;                           ///< Offset 3028    Foxville I225 support configuration
  UINT8    FoxLanRpNumber;                          ///< Offset 3029    Foxville I225 PCIe Root Port Number
  UINT8    DPIW;                                    ///< Offset 3030    DPin Dynamic Switch
  UINT32   DPG1;                                    ///< Offset 3031    PcdDpMuxGpio
  //
  // Data Role Swap:
  //
  UINT8    UsbcDataRoleSwap;                        ///< Offset 3035    Usbc Data Role Swap
  UINT8    Usb4CmSwitchEnable;                      ///< Offset 3036    USB4 CM mode switch is enabled/disabled
  //Flash ID support for discrete flash solution
  UINT8    MipiCamLink0DD_FlashID;                  ///< Offset 3037    Flash ID for Link0
  UINT8    MipiCamLink1DD_FlashID;                  ///< Offset 3038    Flash ID for Link1
  UINT8    MipiCamLink2DD_FlashID;                  ///< Offset 3039    Flash ID for Link2
  UINT8    MipiCamLink3DD_FlashID;                  ///< Offset 3040    Flash ID for Link3
  UINT8    MipiCamLink4DD_FlashID;                  ///< Offset 3041    Flash ID for Link4
  UINT8    MipiCamLink5DD_FlashID;                  ///< Offset 3042    Flash ID for Link5
  //I2C6 Support
  UINT16   SSH6;                                    ///< Offset 3043    SSCN-HIGH for I2C6
  UINT16   SSL6;                                    ///< Offset 3045    SSCN-LOW  for I2C6
  UINT16   SSD6;                                    ///< Offset 3047    SSCN-HOLD for I2C6
  UINT16   FMH6;                                    ///< Offset 3049    FMCN-HIGH for I2C6
  UINT16   FML6;                                    ///< Offset 3051    FMCN-LOW  for I2C6
  UINT16   FMD6;                                    ///< Offset 3053    FMCN-HOLD for I2C6
  UINT16   FPH6;                                    ///< Offset 3055    FPCN-HIGH for I2C6
  UINT16   FPL6;                                    ///< Offset 3057    FPCN-LOW  for I2C6
  UINT16   FPD6;                                    ///< Offset 3059    FPCN-HOLD for I2C6
  UINT16   HSH6;                                    ///< Offset 3061    HSCN-HIGH for I2C6
  UINT16   HSL6;                                    ///< Offset 3063    HSCN-LOW  for I2C6
  UINT16   HSD6;                                    ///< Offset 3065    HSCN-HOLD for I2C6
  UINT16   M0CC;                                    ///< Offset 3067    M0D3 for I2C6
  UINT16   M1CC;                                    ///< Offset 3069    M1D3 for I2C6
  //I2C7 Support
  UINT16   SSH7;                                    ///< Offset 3071    SSCN-HIGH for I2C7
  UINT16   SSL7;                                    ///< Offset 3073    SSCN-LOW  for I2C7
  UINT16   SSD7;                                    ///< Offset 3075    SSCN-HOLD for I2C7
  UINT16   FMH7;                                    ///< Offset 3077    FMCN-HIGH for I2C7
  UINT16   FML7;                                    ///< Offset 3079    FMCN-LOW  for I2C7
  UINT16   FMD7;                                    ///< Offset 3081    FMCN-HOLD for I2C7
  UINT16   FPH7;                                    ///< Offset 3083    FPCN-HIGH for I2C7
  UINT16   FPL7;                                    ///< Offset 3085    FPCN-LOW  for I2C7
  UINT16   FPD7;                                    ///< Offset 3087    FPCN-HOLD for I2C7
  UINT16   HSH7;                                    ///< Offset 3089    HSCN-HIGH for I2C7
  UINT16   HSL7;                                    ///< Offset 3091    HSCN-LOW  for I2C7
  UINT16   HSD7;                                    ///< Offset 3093    HSCN-HOLD for I2C7
  UINT16   M0CD;                                    ///< Offset 3095    M0D3 for I2C7
  UINT16   M1CD;                                    ///< Offset 3097    M1D3 for I2C7
  UINT16   WwanTPer2ResDelayMs;                     ///< Offset 3099    PERST# to BBRST# delay time during WWAN OFF
  //ZPODD support
  UINT32   ZpoddDAGpio;                             ///< Offset 3101    ZPODD device attention gpio
  UINT32   ZpoddPRGpio;                             ///< Offset 3105    ZPODD device present gpio
  UINT32   ZpoddPWGpio;                             ///< Offset 3109    ZPODD device power gpio
  UINT8    ZpoddPWGpioPolarity;                     ///< Offset 3113    ZPODD device power gpio polarity
  UINT16   WwanOemSvid;                             ///< Offset 3114    WWAN OEM SVID
  UINT16   WwanSvidTimeout;                         ///< Offset 3116    WWAN SVID Timeout
  UINT8    PcdH8S2113UAR;                           ///< Offset 3118    H8S2113 UAR
  UINT8    BtLedConfig;                             ///< Offset 3119    BT LED Config
  UINT8    BtLedPulseDuration;                      ///< Offset 3120    BT LED Pulse Duration
  UINT8    BtLedPulseInterval;                      ///< Offset 3121    BT LED Pulse Interval
  UINT32   WifiRegulatoryConfigurations;            ///< Offset 3122    WiFi Regulatory Configurations
  UINT32   WifiUartConfigurations;                  ///< Offset 3126    WiFi UART Configurations
  UINT32   WifiUnii4;                               ///< Offset 3130    WiFi UNII-4
  UINT32   WifiIndoorControl;                       ///< Offset 3134    WiFi Indoor Control
  UINT8    ATSC;                                    ///< Offset 3138    Flag to indicate if TSC is linked to ART
  UINT8    Rp08D3ColdDisable;                       ///< Offset 3139    RootPort 08 D3 Cold Disable
  // Onboard MR support (for ADL S8 only)
  UINT32   OnBoardMr1PowerEnableGpio;               ///< Offset 3140    Onboard MR 1 Power Enable Gpio pin
  UINT8    OnBoardMr1PowerEnableGpioPolarity;       ///< Offset 3144    Onboard MR 1 Power Enable Gpio pin polarity
  UINT32   OnBoardMr1RstGpio;                       ///< Offset 3145    Onboard MR 1 Rest Gpio pin
  UINT8    OnBoardMr1RstGpioPolarity;               ///< Offset 3149    Onboard MR 1 Rest Gpio pin polarity
  UINT32   OnBoardMr1WakeGpio;                      ///< Offset 3150    Onboard MR 1 Wake Gpio pin
  UINT8    OnBoardMr1RpNumber;                      ///< Offset 3154    Onboard MR 1 Root Port Number
  // Onboard MR support
  UINT32   OnBoardMr2PowerEnableGpio;               ///< Offset 3155    Onboard MR 2 Power Enable Gpio pin
  UINT8    OnBoardMr2PowerEnableGpioPolarity;       ///< Offset 3159    Onboard MR 2 Power Enable Gpio pin polarity
  UINT32   OnBoardMr2RstGpio;                       ///< Offset 3160    Onboard MR 2 Rest Gpio pin
  UINT8    OnBoardMr2RstGpioPolarity;               ///< Offset 3164    Onboard MR 2 Rest Gpio pin polarity
  UINT32   OnBoardMr2WakeGpio;                      ///< Offset 3165    Onboard MR 2 Wake Gpio pin
  UINT8    OnBoardMr2RpNumber;                      ///< Offset 3169    Onboard MR 2 Root Port Number
  UINT8    WccdEnable;                              ///< Offset 3170    WCCD Enable
  UINT8    Rp08D3ColdSupport;                       ///< Offset 3171    RP08 D3Cold Support
  UINT8    WirelessLowBandIsolation;                ///< Offset 3172    WLAN/WWAN Low Band Isolation
  UINT8    WirelessHighBandIsolation;               ///< Offset 3173    WLAN/WWAN High Band Isolation
  UINT8    WgdsWiFiSarDeltaGroupNumber;             ///< Offset 3174    WgdsWiFiSarDeltaGroupNumber
  UINT8    WgdsWiFiSarDeltaGroup4PowerMax1;         ///< Offset 3175    WgdsWiFiSarDeltaGroup4PowerMax1
  UINT8    WgdsWiFiSarDeltaGroup4PowerChainA1;      ///< Offset 3176    WgdsWiFiSarDeltaGroup4PowerChainA1
  UINT8    WgdsWiFiSarDeltaGroup4PowerChainB1;      ///< Offset 3177    WgdsWiFiSarDeltaGroup4PowerChainB1
  UINT8    WgdsWiFiSarDeltaGroup4PowerMax2;         ///< Offset 3178    WgdsWiFiSarDeltaGroup4PowerMax2
  UINT8    WgdsWiFiSarDeltaGroup4PowerChainA2;      ///< Offset 3179    WgdsWiFiSarDeltaGroup4PowerChainA2
  UINT8    WgdsWiFiSarDeltaGroup4PowerChainB2;      ///< Offset 3180    WgdsWiFiSarDeltaGroup4PowerChainB2
  UINT8    WgdsWiFiSarDeltaGroup4PowerMax3;         ///< Offset 3181    WgdsWiFiSarDeltaGroup4PowerMax3
  UINT8    WgdsWiFiSarDeltaGroup4PowerChainA3;      ///< Offset 3182    WgdsWiFiSarDeltaGroup4PowerChainA3
  UINT8    WgdsWiFiSarDeltaGroup4PowerChainB3;      ///< Offset 3183    WgdsWiFiSarDeltaGroup4PowerChainB3
  UINT8    WgdsWiFiSarDeltaGroup5PowerMax1;         ///< Offset 3184    WgdsWiFiSarDeltaGroup5PowerMax1
  UINT8    WgdsWiFiSarDeltaGroup5PowerChainA1;      ///< Offset 3185    WgdsWiFiSarDeltaGroup5PowerChainA1
  UINT8    WgdsWiFiSarDeltaGroup5PowerChainB1;      ///< Offset 3186    WgdsWiFiSarDeltaGroup5PowerChainB1
  UINT8    WgdsWiFiSarDeltaGroup5PowerMax2;         ///< Offset 3187    WgdsWiFiSarDeltaGroup5PowerMax2
  UINT8    WgdsWiFiSarDeltaGroup5PowerChainA2;      ///< Offset 3188    WgdsWiFiSarDeltaGroup5PowerChainA2
  UINT8    WgdsWiFiSarDeltaGroup5PowerChainB2;      ///< Offset 3189    WgdsWiFiSarDeltaGroup5PowerChainB2
  UINT8    WgdsWiFiSarDeltaGroup5PowerMax3;         ///< Offset 3190    WgdsWiFiSarDeltaGroup5PowerMax3
  UINT8    WgdsWiFiSarDeltaGroup5PowerChainA3;      ///< Offset 3191    WgdsWiFiSarDeltaGroup5PowerChainA3
  UINT8    WgdsWiFiSarDeltaGroup5PowerChainB3;      ///< Offset 3192    WgdsWiFiSarDeltaGroup5PowerChainB3
  UINT8    WgdsWiFiSarDeltaGroup6PowerMax1;         ///< Offset 3193    WgdsWiFiSarDeltaGroup6PowerMax1
  UINT8    WgdsWiFiSarDeltaGroup6PowerChainA1;      ///< Offset 3194    WgdsWiFiSarDeltaGroup6PowerChainA1
  UINT8    WgdsWiFiSarDeltaGroup6PowerChainB1;      ///< Offset 3195    WgdsWiFiSarDeltaGroup6PowerChainB1
  UINT8    WgdsWiFiSarDeltaGroup6PowerMax2;         ///< Offset 3196    WgdsWiFiSarDeltaGroup6PowerMax2
  UINT8    WgdsWiFiSarDeltaGroup6PowerChainA2;      ///< Offset 3197    WgdsWiFiSarDeltaGroup6PowerChainA2
  UINT8    WgdsWiFiSarDeltaGroup6PowerChainB2;      ///< Offset 3198    WgdsWiFiSarDeltaGroup6PowerChainB2
  UINT8    WgdsWiFiSarDeltaGroup6PowerMax3;         ///< Offset 3199    WgdsWiFiSarDeltaGroup6PowerMax3
  UINT8    WgdsWiFiSarDeltaGroup6PowerChainA3;      ///< Offset 3200    WgdsWiFiSarDeltaGroup6PowerChainA3
  UINT8    WgdsWiFiSarDeltaGroup6PowerChainB3;      ///< Offset 3201    WgdsWiFiSarDeltaGroup6PowerChainB3
  UINT8    WgdsWiFiSarDeltaGroup7PowerMax1;         ///< Offset 3202    WgdsWiFiSarDeltaGroup7PowerMax1
  UINT8    WgdsWiFiSarDeltaGroup7PowerChainA1;      ///< Offset 3203    WgdsWiFiSarDeltaGroup7PowerChainA1
  UINT8    WgdsWiFiSarDeltaGroup7PowerChainB1;      ///< Offset 3204    WgdsWiFiSarDeltaGroup7PowerChainB1
  UINT8    WgdsWiFiSarDeltaGroup7PowerMax2;         ///< Offset 3205    WgdsWiFiSarDeltaGroup7PowerMax2
  UINT8    WgdsWiFiSarDeltaGroup7PowerChainA2;      ///< Offset 3206    WgdsWiFiSarDeltaGroup7PowerChainA2
  UINT8    WgdsWiFiSarDeltaGroup7PowerChainB2;      ///< Offset 3207    WgdsWiFiSarDeltaGroup7PowerChainB2
  UINT8    WgdsWiFiSarDeltaGroup7PowerMax3;         ///< Offset 3208    WgdsWiFiSarDeltaGroup7PowerMax3
  UINT8    WgdsWiFiSarDeltaGroup7PowerChainA3;      ///< Offset 3209    WgdsWiFiSarDeltaGroup7PowerChainA3
  UINT8    WgdsWiFiSarDeltaGroup7PowerChainB3;      ///< Offset 3210    WgdsWiFiSarDeltaGroup7PowerChainB3
  UINT8    WgdsWiFiSarDeltaGroup8PowerMax1;         ///< Offset 3211    WgdsWiFiSarDeltaGroup8PowerMax1
  UINT8    WgdsWiFiSarDeltaGroup8PowerChainA1;      ///< Offset 3212    WgdsWiFiSarDeltaGroup8PowerChainA1
  UINT8    WgdsWiFiSarDeltaGroup8PowerChainB1;      ///< Offset 3213    WgdsWiFiSarDeltaGroup8PowerChainB1
  UINT8    WgdsWiFiSarDeltaGroup8PowerMax2;         ///< Offset 3214    WgdsWiFiSarDeltaGroup8PowerMax2
  UINT8    WgdsWiFiSarDeltaGroup8PowerChainA2;      ///< Offset 3215    WgdsWiFiSarDeltaGroup8PowerChainA2
  UINT8    WgdsWiFiSarDeltaGroup8PowerChainB2;      ///< Offset 3216    WgdsWiFiSarDeltaGroup8PowerChainB2
  UINT8    WgdsWiFiSarDeltaGroup8PowerMax3;         ///< Offset 3217    WgdsWiFiSarDeltaGroup8PowerMax3
  UINT8    WgdsWiFiSarDeltaGroup8PowerChainA3;      ///< Offset 3218    WgdsWiFiSarDeltaGroup8PowerChainA3
  UINT8    WgdsWiFiSarDeltaGroup8PowerChainB3;      ///< Offset 3219    WgdsWiFiSarDeltaGroup8PowerChainB3
  UINT8    Rp08WakeReworkDone;                      ///< Offset 3220    Rp08WakeRework support
  UINT32   WifiTASSelectionValue;                   ///< Offset 3221    WifiTASSelectionValue
  UINT32   DisplayMuxGpioNo;                        ///< Offset 3225    Display Mux GPIO pin
  UINT8    DgBrightnessPercentage;                  ///< Offset 3229    DG eDP Brightness Level Percentage
  UINT8    DgNumberOfValidDeviceId;                 ///< Offset 3230    DG Number of Valid Device IDs
  UINT32   DgDeviceId1;                             ///< Offset 3231    DG Device ID 1
  UINT32   DgDeviceId2;                             ///< Offset 3235    DG Device ID 2
  UINT32   DgDeviceId3;                             ///< Offset 3239    DG Device ID 3
  UINT32   DgDeviceId4;                             ///< Offset 3243    DG Device ID 4
  UINT32   DgDeviceId5;                             ///< Offset 3247    DG Device ID 5
  UINT32   DgDeviceId6;                             ///< Offset 3251    DG Device ID 6
  UINT32   DgDeviceId7;                             ///< Offset 3255    DG Device ID 7
  UINT32   DgDeviceId8;                             ///< Offset 3259    DG Device ID 8
  UINT32   DgDeviceId9;                             ///< Offset 3263    DG Device ID 9
  UINT32   DgDeviceId10;                            ///< Offset 3267    DG Device ID 10
  UINT32   DgDeviceId11;                            ///< Offset 3271    DG Device ID 11
  UINT32   DgDeviceId12;                            ///< Offset 3275    DG Device ID 12
  UINT32   DgDeviceId13;                            ///< Offset 3279    DG Device ID 13
  UINT32   DgDeviceId14;                            ///< Offset 3283    DG Device ID 14
  UINT32   DgDeviceId15;                            ///< Offset 3287    DG Device ID 15
  UINT32   DgDeviceIdX;                             ///< Offset 3291    DG Device ID for eDP device
  UINT8    DgDisplaySupportFlag;                    ///< Offset 3295    _DOS DG Display Support Flag.
  UINT8    SdevXhciEntry;                           ///< Offset 3296    Secure biometric VTIO over USB camera
  UINT8    MipiCamLink0DD_PprValue;                 ///< Offset 3297    PPR Value
  UINT8    MipiCamLink0DD_PprUnit;                  ///< Offset 3298    PPR Unit
  UINT8    MipiCamLink1DD_PprValue;                 ///< Offset 3299    PPR Value
  UINT8    MipiCamLink1DD_PprUnit;                  ///< Offset 3300    PPR Unit
  UINT8    MipiCamLink2DD_PprValue;                 ///< Offset 3301    PPR Value
  UINT8    MipiCamLink2DD_PprUnit;                  ///< Offset 3302    PPR Unit
  UINT8    MipiCamLink3DD_PprUnit;                  ///< Offset 3303    PPR Unit
  UINT8    MipiCamLink3DD_PprValue;                 ///< Offset 3304    PPR Value
  UINT8    MipiCamLink4DD_PprUnit;                  ///< Offset 3305    PPR Unit
  UINT8    MipiCamLink4DD_PprValue;                 ///< Offset 3306    PPR Value
  UINT8    MipiCamLink5DD_PprUnit;                  ///< Offset 3307    PPR Unit
  UINT8    MipiCamLink5DD_PprValue;                 ///< Offset 3308    PPR Value
  UINT8    I2SE;                                    ///< Offset 3309    HD Audio I2S Enable
  UINT32   I2c0SpeedMode;                           ///< Offset 3310    I2C0 Speed Mode Selection
  UINT32   I2c1SpeedMode;                           ///< Offset 3314    I2C1 Speed Mode Selection
  UINT32   I2c2SpeedMode;                           ///< Offset 3318    I2C2 Speed Mode Selection
  UINT32   I2c3SpeedMode;                           ///< Offset 3322    I2C3 Speed Mode Selection
  UINT32   I2c4SpeedMode;                           ///< Offset 3326    I2C4 Speed Mode Selection
  UINT32   I2c5SpeedMode;                           ///< Offset 3330    I2C5 Speed Mode Selection
  UINT8    EnFanAutoMode;                           ///< Offset 3334    Enabling Auto Mode for DTT
} PLATFORM_NVS_AREA;

#pragma pack(pop)
#endif
