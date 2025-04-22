

/** @file
  ACPI DSDT table

  Copyright (c) 2011 - 2020, Intel Corporation. All rights reserved.<BR>
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
  UINT8    EnableDigitalThermalSensor;              ///< Offset 21      Digital Thermal Sensor Enable
  UINT8    BspDigitalThermalSensorTemperature;      ///< Offset 22      Digital Thermal Sensor 1 Reading
  UINT8    ApDigitalThermalSensorTemperature;       ///< Offset 23      Digital Thermal Sensor 2 Reading
  UINT8    DigitalThermalSensorSmiFunction;         ///< Offset 24      DTS SMI Function Call
  //
  // Revision Field:
  //
  UINT8    Revision;                                ///< Offset 25      Revison of GlobalNvsArea
  //
  // CPU Identification Registers:
  //
  UINT8    ApicEnable;                              ///< Offset 26      APIC Enabled by SBIOS (APIC Enabled = 1)
  UINT8    ThreadCount;                             ///< Offset 27      Number of Enabled Threads
  UINT8    CurentPdcState0;                         ///< Offset 28      PDC Settings, Processor 0
  UINT8    CurentPdcState1;                         ///< Offset 29      PDC Settings, Processor 1
  UINT8    MaximumPpcState;                         ///< Offset 30      Maximum PPC state
  UINT32   PpmFlags;                                ///< Offset 31      PPM Flags (Same as CFGD)
  UINT8    C6C7Latency;                             ///< Offset 35      C6/C7 Entry/Exit latency
  //
  // SIO Configuration Registers:
  //
  UINT8    DockedSioPresent;                        ///< Offset 36      National SIO Present
  UINT8    DockComA;                                ///< Offset 37      COM A Port
  UINT8    DockComB;                                ///< Offset 38      COM B Port
  UINT8    DockLpt;                                 ///< Offset 39      LPT Port
  UINT8    DockFdc;                                 ///< Offset 40      FDC Port
  UINT8    OnboardCom;                              ///< Offset 41      SMSC Com Port
  UINT8    OnboardComCir;                           ///< Offset 42      SMSC Com CIR Port
  UINT8    SMSC1007;                                ///< Offset 43      SMSC1007 SIO Present
  UINT8    WPCN381U;                                ///< Offset 44      WPCN381U SIO Present
  UINT8    SMSC1000;                                ///< Offset 45      SMSC1000 SIO Present
  //
  // Extended Mobile Access Values
  //
  UINT8    EmaEnable;                               ///< Offset 46      EMA Enable
  UINT16   EmaPointer;                              ///< Offset 47      EMA Pointer
  UINT16   EmaLength;                               ///< Offset 49      EMA Length
  //
  // MEF Registers:
  //
  UINT8    MefEnable;                               ///< Offset 51      MEF Enable
  //
  // PCIe Dock Status:
  //
  UINT8    PcieDockStatus;                          ///< Offset 52      PCIe Dock Status
  //
  // TPM Registers
  //
  UINT8    MorData;                                 ///< Offset 53      Memory Overwrite Request Data
  UINT8    TcgParamter;                             ///< Offset 54      Used for save the Mor and/or physical presence paramter
  UINT32   PPResponse;                              ///< Offset 55      Physical Presence request operation response
  UINT8    PPRequest;                               ///< Offset 59      Physical Presence request operation
  UINT8    LastPPRequest;                           ///< Offset 60      Last Physical Presence request operation
  //
  // SATA Registers:
  //
  UINT8    IdeMode;                                 ///< Offset 61      IDE Mode (Compatible\Enhanced)
  //
  // Board Id
  //
  UINT8    PlatformId;                              ///< Offset 62      Platform id
  UINT8    BoardType;                               ///< Offset 63      Board Type
  //
  // PCIe Hot Plug
  //
  UINT8    PcieOSCControl;                          ///< Offset 64      PCIE OSC Control
  UINT8    NativePCIESupport;                       ///< Offset 65      Native PCIE Setup Value
  //
  // USB Sideband Deferring Support
  //
  UINT8    HostAlertVector1;                        ///< Offset 66      USB Sideband Deferring GPE Vector (HOST_ALERT#1)
  UINT8    HostAlertVector2;                        ///< Offset 67      USB Sideband Deferring GPE Vector (HOST_ALERT#2)
  //
  // Embedded Controller Availability Flag.
  //
  UINT8    EcAvailable;                             ///< Offset 68      Embedded Controller Availability Flag.
  //
  // Global Variables
  //
  UINT8    DisplaySupportFlag;                      ///< Offset 69      _DOS Display Support Flag.
  UINT8    InterruptModeFlag;                       ///< Offset 70      Global IOAPIC/8259 Interrupt Mode Flag.
  UINT8    CoolingTypeFlag;                         ///< Offset 71      Global Cooling Type Flag.
  UINT8    L01Counter;                              ///< Offset 72      Global L01 Counter.
  UINT8    VirtualFan0Status;                       ///< Offset 73      Virtual Fan0 Status.
  UINT8    VirtualFan1Status;                       ///< Offset 74      Virtual Fan1 Status.
  UINT8    VirtualFan2Status;                       ///< Offset 75      Virtual Fan2 Status.
  UINT8    VirtualFan3Status;                       ///< Offset 76      Virtual Fan3 Status.
  UINT8    VirtualFan4Status;                       ///< Offset 77      Virtual Fan4 Status.
  UINT8    VirtualFan5Status;                       ///< Offset 78      Virtual Fan5 Status.
  UINT8    VirtualFan6Status;                       ///< Offset 79      Virtual Fan6 Status.
  UINT8    VirtualFan7Status;                       ///< Offset 80      Virtual Fan7 Status.
  UINT8    VirtualFan8Status;                       ///< Offset 81      Virtual Fan8 Status.
  UINT8    VirtualFan9Status;                       ///< Offset 82      Virtual Fan9 Status.
  //
  // Thermal
  //
  UINT32   PlatformCpuId;                           ///< Offset 83      CPUID Feature Information [EAX]
  UINT32   TBARB;                                   ///< Offset 87      Reserved for Thermal Base Low Address for BIOS
  UINT32   TBARBH;                                  ///< Offset 91      Reserved for Thermal Base High Address for BIOS
  UINT8    TsOnDimmEnabled;                         ///< Offset 95      TS-on-DIMM is chosen in SETUP and present on the DIMM
  //
  // Board info
  //
  UINT8    PlatformFlavor;                          ///< Offset 96      Platform Flavor
  UINT16   BoardRev;                                ///< Offset 97      Board Rev
  //
  // Package temperature
  //
  UINT8    PackageDTSTemperature;                   ///< Offset 99      Package Temperature
  UINT8    IsPackageTempMSRAvailable;               ///< Offset 100     Package Temperature MSR available
  UINT8    PeciAccessMethod;                        ///< Offset 101     Peci Access Method
  UINT8    Ac0FanSpeed;                             ///< Offset 102     _AC0 Fan Speed
  UINT8    Ac1FanSpeed;                             ///< Offset 103     _AC1 Fan Speed
  UINT8    Ap2DigitalThermalSensorTemperature;      ///< Offset 104     Digital Thermal Sensor 3 Reading
  UINT8    Ap3DigitalThermalSensorTemperature;      ///< Offset 105     Digital Thermal Sensor 4 Reading
  //
  // XTU 3.0 Specification
  //
  UINT32   XTUBaseAddress;                          ///< Offset 106     XTU Continous structure Base Address
  UINT32   XTUSize;                                 ///< Offset 110     XMP Size
  UINT32   XMPBaseAddress;                          ///< Offset 114     XMP Base Address
  UINT8    DDRReferenceFreq;                        ///< Offset 118     DDR Reference Frequency
  UINT8    Rtd3Support;                             ///< Offset 119     Runtime D3 support.
  UINT8    Rtd3P0dl;                                ///< Offset 120     User selctable Delay for Device D0 transition.
  UINT8    Rtd3P3dl;                                ///< Offset 121     User selctable Delay for Device D3 transition.
  //
  // Intel(R) Dynamic Tuning Technology Devices and trip points
  //
  UINT8    EnableDptf;                              ///< Offset 122     EnableDptf
  UINT16   EnableDCFG;                              ///< Offset 123     EnableDCFG
  UINT8    EnableSaDevice;                          ///< Offset 125     EnableSaDevice
  // DPPM Devices and trip points
  //
  UINT8    EnableFan1Device;                        ///< Offset 126     EnableFan1Device
  UINT8    EnableAmbientDevice;                     ///< Offset 127     EnableAmbientDevice
  UINT8    ActiveThermalTripPointAmbient;           ///< Offset 128     ActiveThermalTripPointAmbient
  UINT8    PassiveThermalTripPointAmbient;          ///< Offset 129     PassiveThermalTripPointAmbient
  UINT8    CriticalThermalTripPointAmbient;         ///< Offset 130     CriticalThermalTripPointAmbient
  UINT8    HotThermalTripPointAmbient;              ///< Offset 131     HotThermalTripPointAmbient
  UINT8    EnableSkinDevice;                        ///< Offset 132     EnableSkinDevice
  UINT8    ActiveThermalTripPointSkin;              ///< Offset 133     ActiveThermalTripPointSkin
  UINT8    PassiveThermalTripPointSkin;             ///< Offset 134     PassiveThermalTripPointSkin
  UINT8    CriticalThermalTripPointSkin;            ///< Offset 135     CriticalThermalTripPointSkin
  UINT8    HotThermalTripPointSkin;                 ///< Offset 136     HotThermalTripPointSkin
  UINT8    EnableExhaustFanDevice;                  ///< Offset 137     EnableExhaustFanDevice
  UINT8    ActiveThermalTripPointExhaustFan;        ///< Offset 138     ActiveThermalTripPointExhaustFan
  UINT8    PassiveThermalTripPointExhaustFan;       ///< Offset 139     PassiveThermalTripPointExhaustFan
  UINT8    CriticalThermalTripPointExhaustFan;      ///< Offset 140     CriticalThermalTripPointExhaustFan
  UINT8    HotThermalTripPointExhaustFan;           ///< Offset 141     HotThermalTripPointExhaustFan
  UINT8    EnableVRDevice;                          ///< Offset 142     EnableVRDevice
  UINT8    ActiveThermalTripPointVR;                ///< Offset 143     ActiveThermalTripPointVR
  UINT8    PassiveThermalTripPointVR;               ///< Offset 144     PassiveThermalTripPointVR
  UINT8    CriticalThermalTripPointVR;              ///< Offset 145     CriticalThermalTripPointVR
  UINT8    HotThermalTripPointVR;                   ///< Offset 146     HotThermalTripPointVR
  //
  // DPPM Policies
  //
  UINT8    EnableActivePolicy;                      ///< Offset 147     EnableActivePolicy @deprecated. Intel(R) Dynamic Tuning can dynamically enable/disable policies
  UINT8    EnablePassivePolicy;                     ///< Offset 148     EnablePassivePolicy @deprecated. Intel(R) Dynamic Tuning can dynamically enable/disable policies
  UINT8    EnableCriticalPolicy;                    ///< Offset 149     EnableCriticalPolicy @deprecated. Intel(R) Dynamic Tuning can dynamically enable/disable policies
  UINT8    EnablePIDPolicy;                         ///< Offset 150     EnablePIDPolicy @deprecated. Intel(R) Dynamic Tuning can dynamically enable/disable policies
  //
  // Miscellaneous Intel(R) Dynamic Tuning Technology
  //
  UINT32   PpccStepSize;                            ///< Offset 151     PPCC Step Size
  //
  // Never Sleep Technology
  //
  UINT8    IrmtCfg;                                 ///< Offset 155     Irmt Configuration
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
  UINT8    Reserved0[334];                          ///< Offset 166:499
  UINT8    EnablePowerParticipant;                  ///< Offset 500     EnablePowerParticipant
  UINT16   DPTFRsvd0;                               ///< Offset 501     DPTFRsvd0
  UINT16   PowerParticipantPollingRate;             ///< Offset 503     PowerParticipantPollingRate
  UINT8    EnablePowerBossPolicy;                   ///< Offset 505     EnablePowerBossPolicy @deprecated. Intel(R) Dynamic Tuning can dynamically enable/disable policies
  UINT8    EnableVSPolicy;                          ///< Offset 506     EnableVSPolicy @deprecated. Intel(R) Dynamic Tuning can dynamically enable/disable policies
  UINT8    EnableRFIMPolicy;                        ///< Offset 507     RFI Mitigation @deprecated. Intel(R) Dynamic Tuning can dynamically enable/disable policies
  UINT8    Reserved1[7];                            ///< Offset 508:514
  UINT8    UsbPowerResourceTest;                    ///< Offset 515     RTD3 USB Power Resource config
  UINT8    Rtd3I2C0SensorHub;                       ///< Offset 516     RTD3 support for I2C0 SH
  UINT8    VirtualGpioButtonSxBitmask;              ///< Offset 517     Virtual GPIO button Notify Sleep State Change
  UINT8    IuerButtonEnable;                        ///< Offset 518     IUER Button Enable
  UINT8    IuerConvertibleEnable;                   ///< Offset 519     IUER Convertible Enable
  UINT8    IuerDockEnable;                          ///< Offset 520     IUER Dock Enable
  UINT8    CSNotifyEC;                              ///< Offset 521     EC Notification of Low Power S0 Idle State
  UINT16   Rtd3AudioDelay;                          ///< Offset 522     RTD3 Audio Codec device delay
  UINT16   Rtd3SensorHub;                           ///< Offset 524     RTD3 SensorHub delay time after applying power to device
  UINT16   Rtd3TouchPanelDelay;                     ///< Offset 526     RTD3 TouchPanel delay time after applying power to device
  UINT16   Rtd3TouchPadDelay;                       ///< Offset 528     RTD3 TouchPad delay time after applying power to device
  UINT16   VRRampUpDelay;                           ///< Offset 530     VR Ramp up delay
  UINT8    PstateCapping;                           ///< Offset 532     P-state Capping
  UINT16   Rtd3I2C0ControllerPS0Delay;              ///< Offset 533     Delay in _PS0 after powering up I2C0 Controller
  UINT16   Rtd3I2C1ControllerPS0Delay;              ///< Offset 535     Delay in _PS0 after powering up I2C1 Controller
  UINT16   Rtd3Config0;                             ///< Offset 537     RTD3 Config Setting0(BIT0:ZPODD, BIT1:Reserved, BIT2:PCIe NVMe, BIT3:BT, BIT4:SKL SDS SIP I2C Touch, BIT6:Card Reader, BIT7:WWAN)
  UINT16   Rtd3Config1;                             ///< Offset 539     RTD3 Config Setting1(BIT0:Sata Port0, BIT1:Sata Port1, BIT2:Sata Port2, BIT3:Sata Port3)
  UINT8    CSDebugLightEC;                          ///< Offset 541     EC Debug Light (CAPS LOCK) for when in Low Power S0 Idle State
  UINT8    Ps2MouseEnable;                          ///< Offset 542     Ps2 Mouse Enable
  UINT8    Ps2KbMsEnable;                           ///< Offset 543     Ps2 Keyboard and Mouse Enable
  UINT16   SSH0;                                    ///< Offset 544     SSCN-HIGH for I2C0
  UINT16   SSL0;                                    ///< Offset 546     SSCN-LOW  for I2C0
  UINT16   SSD0;                                    ///< Offset 548     SSCN-HOLD for I2C0
  UINT16   FMH0;                                    ///< Offset 550     FMCN-HIGH for I2C0
  UINT16   FML0;                                    ///< Offset 552     FMCN-LOW  for I2C0
  UINT16   FMD0;                                    ///< Offset 554     FMCN-HOLD for I2C0
  UINT16   FPH0;                                    ///< Offset 556     FPCN-HIGH for I2C0
  UINT16   FPL0;                                    ///< Offset 558     FPCN-LOW  for I2C0
  UINT16   FPD0;                                    ///< Offset 560     FPCN-HOLD for I2C0
  UINT16   HSH0;                                    ///< Offset 562     HSCN-HIGH for I2C0
  UINT16   HSL0;                                    ///< Offset 564     HSCN-LOW  for I2C0
  UINT16   HSD0;                                    ///< Offset 566     HSCN-HOLD for I2C0
  UINT8    Reserved2[5];                            ///< Offset 568:572
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
  UINT8    Reserved3[1];                            ///< Offset 597:597
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
  UINT8    Reserved4[1];                            ///< Offset 622:622
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
  UINT8    Reserved5[1];                            ///< Offset 647:647
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
  UINT8    Reserved6[1];                            ///< Offset 672:672
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
  UINT8    Reserved7[1];                            ///< Offset 697:697
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
  UINT8    Reserved8[1];                            ///< Offset 734:734
  UINT16   M0C9;                                    ///< Offset 735     M0D3 for UART0
  UINT16   M1C9;                                    ///< Offset 737     M1D3 for UART0
  UINT16   M0CA;                                    ///< Offset 739     M0D3 for UART1
  UINT16   M1CA;                                    ///< Offset 741     M1D3 for UART1
  UINT16   M0CB;                                    ///< Offset 743     M0D3 for UART2
  UINT16   M1CB;                                    ///< Offset 745     M1D3 for UART2
  UINT8    Reserved9[1];                            ///< Offset 747:747
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
  UINT8    Reserved10[6];                           ///< Offset 772:777
  UINT64   LowPowerS0IdleConstraint;                ///< Offset 778     PEP Constraints
  // Bit[1:0] - Storage (0:None, 1:Adapter D0/F1, 2:Raid, 3:Adapter D3)
  // Bit[2]   - En/Dis UART0
  // Bit[3]   - En/Dis UART1
  // Bit[4]   - En/Dis PCIe NVMe/AHCI
  // Bit[5]   - En/Dis I2C0
  // Bit[6]   - En/Dis I2C1
  // Bit[7]   - En/Dis XHCI
  // Bit[9:8] - En/Dis HD Audio (includes ADSP)
  // Bit[10]  - En/Dis GFX
  // Bit[11]  - En/Dis CPU
  // Bit[12]  - En/Dis EMMC
  // Bit[13]  - En/Dis SDXC
  // Bit[14]  - En/Dis I2C2
  // Bit[15]  - En/Dis I2C3
  // Bit[16]  - En/Dis I2C4
  // Bit[17]  - En/Dis I2C5
  // Bit[18]  - En/Dis UART2
  // Bit[19]  - En/Dis SPI0
  // Bit[20]  - En/Dis SPI1
  // Bit[21]  - En/Dis SPI2
  // Bit[22]  - En/Dis IPU0
  // Bit[23]  - En/Dis CSME
  // Bit[24]  - En/Dis LAN(GBE)
  // Bit[25]  - En/Dis PEG NVMe/AHCI
  // Bit[26]  - En/Dis THC0
  // Bit[27]  - En/Dis THC1
  // Bit[28]  - En/Dis IDA
  // Bit[29]  - En/Dis I2C6
  // Bit[30]  - En/Dis TCSS IPs
  // Bit[31]  - En/Dis GNA0
  // Bit[32]  - En/Dis VMD
  // Bit[33]  - En/Dis DG PEG0
  // Bit[34]  - En/Dis HECI3
  // Bit[35]  - En/Dis DG PEG1
  // Bit[36]  - En/Dis PEP Constraint Override
  // Bit[38:37] - PCIe RP03 (0: No Constraints or 1: D0/F1 or 3:D3)
  // Bit[40:39] - PCIe RP05 (0: No Constraints or 1: D0/F1 or 3:D3)
  // Bit[42:41] - PCIe RP09 (0: No Constraints or 1: D0/F1 or 3:D3)
  // Bit[44:43] - PEG0 (0: No Constraints or 1: D0/F1 or 3:D3)
  // Bit[46:45] - PEG1 (0: No Constraints or 1: D0/F1 or 3:D3)
  // Bit[48:47] - PEG2 (0: No Constraints or 1: D0/F1 or 3:D3)
  // Bit[50:49] - PEG3 (0: No Constraints or 1: D0/F1 or 3:D3)
  // Bit[52:51] - PCIe RP01 (0: No Constraints or 1: D0/F1 or 3:D3)
  // Bit[54:53] - PCIe RP08 (0: No Constraints or 1: D0/F1 or 3:D3)
  // Bit[56:55] - PCIe RP17 (0: No Constraints or 1: D0/F1 or 3:D3)
  // Bit[58:57] - PCIe RP21 (0: No Constraints or 1: D0/F1 or 3:D3)
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
  //
  // Intel(R) Dynamic Tuning Technology
  //
  UINT8    Reserved11[17];                          ///< Offset 790:806
  UINT8    EnableChargerParticipant;                ///< Offset 807     EnableChargerParticipant
  UINT8    CriticalThermalTripPointAmbientS3;       ///< Offset 808     CriticalThermalTripPointAmbientS3
  UINT8    CriticalThermalTripPointSkinS3;          ///< Offset 809     CriticalThermalTripPointSkinS3
  UINT8    CriticalThermalTripPointExhaustFanS3;    ///< Offset 810     CriticalThermalTripPointExhaustFanS3
  UINT8    CriticalThermalTripPointVrS3;            ///< Offset 811     CriticalThermalTripPointVRS3
  UINT8    CriticalThermalTripPointInletFanS3;      ///< Offset 812     CriticalThermalTripPointInletFanS3
  UINT8    CriticalThermalTripPointWGigS3;          ///< Offset 813     CriticalThermalTripPointWGigS3
  //
  // Intel(R) Dynamic Tuning Technology
  //
  UINT8    EnableBatteryParticipant;                ///< Offset 814     EnableBatteryParticipant
  UINT8    EnableInt3400Device;                     ///< Offset 815     EnableInt3400Device. new ACPI ID is INTC1040
  UINT8    EcLowPowerMode;                          ///< Offset 816     EC Low Power Mode: 1 - Enabled, 0 - Disabled
  UINT8    ThermalSamplingPeriodTMEM;               ///< Offset 817     ThermalSamplingPeriodTMEM @deprecated. Memory Participant is not POR for Intel(R) Dynamic Tuning Technology
  UINT8    EnableSen1Participant;                   ///< Offset 818     EnableSen1Participant
  UINT8    EnableSen2Participant;                   ///< Offset 819     EnableSen2Participant
  UINT8    EnableSen3Participant;                   ///< Offset 820     EnableSen3Participant
  UINT8    EnableSen4Participant;                   ///< Offset 821     EnableSen4Participant
  UINT8    EnableSen5Participant;                   ///< Offset 822     EnableSen5Participant
  UINT8    CriticalThermalTripPointSen1S3;          ///< Offset 823     CriticalThermalTripPointSen1S3
  UINT8    CriticalThermalTripPointSen2S3;          ///< Offset 824     CriticalThermalTripPointSen2S3
  UINT8    CriticalThermalTripPointSen3S3;          ///< Offset 825     CriticalThermalTripPointSen3S3
  UINT8    CriticalThermalTripPointSen4S3;          ///< Offset 826     CriticalThermalTripPointSen4S3
  UINT8    CriticalThermalTripPointSen5S3;          ///< Offset 827     CriticalThermalTripPointSen5S3
  UINT8    PowerSharingManagerEnable;               ///< Offset 828     PowerSharingManagerEnable
  UINT8    PsmSplcDomainType1;                      ///< Offset 829     PsmSplcDomainType1
  UINT32   PsmSplcPowerLimit1;                      ///< Offset 830     PsmSplcPowerLimit1
  UINT32   PsmSplcTimeWindow1;                      ///< Offset 834     PsmSplcTimeWindow1
  UINT8    PsmDplcDomainType1;                      ///< Offset 838     PsmDplcDomainType1
  UINT8    PsmDplcDomainPreference1;                ///< Offset 839     PsmDplcDomainPreference1
  UINT16   PsmDplcPowerLimitIndex1;                 ///< Offset 840     PsmDplcPowerLimitIndex1
  UINT16   PsmDplcDefaultPowerLimit1;               ///< Offset 842     PsmDplcDefaultPowerLimit1
  UINT32   PsmDplcDefaultTimeWindow1;               ///< Offset 844     PsmDplcDefaultTimeWindow1
  UINT16   PsmDplcMinimumPowerLimit1;               ///< Offset 848     PsmDplcMinimumPowerLimit1
  UINT16   PsmDplcMaximumPowerLimit1;               ///< Offset 850     PsmDplcMaximumPowerLimit1
  UINT16   PsmDplcMaximumTimeWindow1;               ///< Offset 852     PsmDplcMaximumTimeWindow1
  UINT8    WifiEnable;                              ///< Offset 854     WifiEnable
  UINT8    WifiDomainType1;                         ///< Offset 855     WifiDomainType1
  UINT16   WifiPowerLimit1;                         ///< Offset 856     WifiPowerLimit1
  UINT32   WifiTimeWindow1;                         ///< Offset 858     WifiTimeWindow1
  UINT8    TRxDelay0;                               ///< Offset 862     TRxDelay0
  UINT8    TRxCableLength0;                         ///< Offset 863     TRxCableLength0
  UINT8    TRxDelay1;                               ///< Offset 864     TRxDelay1
  UINT8    TRxCableLength1;                         ///< Offset 865     TRxCableLength1
  UINT8    WrddDomainType1;                         ///< Offset 866     WrddDomainType1
  UINT16   WrddCountryIndentifier1;                 ///< Offset 867     WrddCountryIndentifier1
  UINT8    Reserved12[149];                         ///< Offset 869:1017
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
  UINT8    GPTD;                                    ///< Offset 1046    GPIO test devices
  UINT16   GDBT;                                    ///< Offset 1047    GPIO test devices' debounce value,
  UINT8    UTKX;                                    ///< Offset 1049    UTK test devices' connection point
  UINT8    SPTD;                                    ///< Offset 1050    SerialIo additional test devices
  UINT8    Reserved13[15];                          ///< Offset 1051:1065
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
  UINT8    Reserved14[1];                           ///< Offset 1082:1082
  UINT8    USTP;                                    ///< Offset 1083    use SerialIo timing parameters
  UINT8    Reserved15[41];                          ///< Offset 1084:1124
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
  UINT8    MipiCamCtrlLogic0_GpioGroupPadNumber[4]; ///< Offset 1177    GPIO Group Pad Number
  UINT8    MipiCamCtrlLogic0_GpioGroupNumber[4];    ///< Offset 1181    GPIO Group Number
  UINT8    MipiCamCtrlLogic0_GpioFunction[4];       ///< Offset 1185    GPIO Function
  UINT8    MipiCamCtrlLogic0_GpioActiveValue[4];    ///< Offset 1189    GPIO Active Value
  UINT8    MipiCamCtrlLogic0_GpioInitialValue[4];   ///< Offset 1193    GPIO Initial Value
  UINT8    MipiCamCtrlLogic0_Pld;                   ///< Offset 1197    Camera Position
  UINT8    MipiCamCtrlLogic0_Wled1FlashMaxCurrent;  ///< Offset 1198    WLED1 Flash Max Current
  UINT8    MipiCamCtrlLogic0_Wled1TorchMaxCurrent;  ///< Offset 1199    WLED1 Torch Max Current
  UINT8    MipiCamCtrlLogic0_Wled2FlashMaxCurrent;  ///< Offset 1200    WLED2 Flash Max Current
  UINT8    MipiCamCtrlLogic0_Wled2TorchMaxCurrent;  ///< Offset 1201    WLED2 Torch Max Current
  UINT8    MipiCamCtrlLogic0_SubPlatformId;         ///< Offset 1202    Sub Platform Id
  UINT8    MipiCamCtrlLogic0_Wled1Type;             ///< Offset 1203    WLED1 Type
  UINT8    MipiCamCtrlLogic0_Wled2Type;             ///< Offset 1204    WLED2 Type
  UINT8    MipiCamCtrlLogic0_PchClockSource;        ///< Offset 1205    PCH Clock source
  // Control Logic 1 options
  UINT8    MipiCamCtrlLogic1_Version;               ///< Offset 1206    Version of CLDB structure
  UINT8    MipiCamCtrlLogic1_Type;                  ///< Offset 1207    Type
  UINT8    MipiCamCtrlLogic1_CrdVersion;            ///< Offset 1208    Version of CRD
  UINT32   MipiCamCtrlLogic1_InputClock;            ///< Offset 1209    Input Clock
  UINT8    MipiCamCtrlLogic1_GpioPinsEnabled;       ///< Offset 1213    Number of GPIO Pins enabled
  UINT8    MipiCamCtrlLogic1_I2cBus;                ///< Offset 1214    I2C Serial Bus Number
  UINT16   MipiCamCtrlLogic1_I2cAddress;            ///< Offset 1215    I2C Address
  UINT8    MipiCamCtrlLogic1_GpioGroupPadNumber[4]; ///< Offset 1217    GPIO Group Pad Number
  UINT8    MipiCamCtrlLogic1_GpioGroupNumber[4];    ///< Offset 1221    GPIO Group Number
  UINT8    MipiCamCtrlLogic1_GpioFunction[4];       ///< Offset 1225    GPIO Function
  UINT8    MipiCamCtrlLogic1_GpioActiveValue[4];    ///< Offset 1229    GPIO Active Value
  UINT8    MipiCamCtrlLogic1_GpioInitialValue[4];   ///< Offset 1233    GPIO Initial Value
  UINT8    MipiCamCtrlLogic1_Pld;                   ///< Offset 1237    Camera Position
  UINT8    MipiCamCtrlLogic1_Wled1FlashMaxCurrent;  ///< Offset 1238    WLED1 Flash Max Current
  UINT8    MipiCamCtrlLogic1_Wled1TorchMaxCurrent;  ///< Offset 1239    WLED1 Torch Max Current
  UINT8    MipiCamCtrlLogic1_Wled2FlashMaxCurrent;  ///< Offset 1240    WLED2 Flash Max Current
  UINT8    MipiCamCtrlLogic1_Wled2TorchMaxCurrent;  ///< Offset 1241    WLED2 Torch Max Current
  UINT8    MipiCamCtrlLogic1_SubPlatformId;         ///< Offset 1242    Sub Platform Id
  UINT8    MipiCamCtrlLogic1_Wled1Type;             ///< Offset 1243    WLED1 Type
  UINT8    MipiCamCtrlLogic1_Wled2Type;             ///< Offset 1244    WLED2 Type
  UINT8    MipiCamCtrlLogic1_PchClockSource;        ///< Offset 1245    PCH Clock source
  // Control Logic 2 options
  UINT8    MipiCamCtrlLogic2_Version;               ///< Offset 1246    Version of CLDB structure
  UINT8    MipiCamCtrlLogic2_Type;                  ///< Offset 1247    Type
  UINT8    MipiCamCtrlLogic2_CrdVersion;            ///< Offset 1248    Version of CRD
  UINT32   MipiCamCtrlLogic2_InputClock;            ///< Offset 1249    Input Clock
  UINT8    MipiCamCtrlLogic2_GpioPinsEnabled;       ///< Offset 1253    Number of GPIO Pins enabled
  UINT8    MipiCamCtrlLogic2_I2cBus;                ///< Offset 1254    I2C Serial Bus Number
  UINT16   MipiCamCtrlLogic2_I2cAddress;            ///< Offset 1255    I2C Address
  UINT8    MipiCamCtrlLogic2_GpioGroupPadNumber[4]; ///< Offset 1257    GPIO Group Pad Number
  UINT8    MipiCamCtrlLogic2_GpioGroupNumber[4];    ///< Offset 1261    GPIO Group Number
  UINT8    MipiCamCtrlLogic2_GpioFunction[4];       ///< Offset 1265    GPIO Function
  UINT8    MipiCamCtrlLogic2_GpioActiveValue[4];    ///< Offset 1269    GPIO Active Value
  UINT8    MipiCamCtrlLogic2_GpioInitialValue[4];   ///< Offset 1273    GPIO Initial Value
  UINT8    MipiCamCtrlLogic2_Pld;                   ///< Offset 1277    Camera Position
  UINT8    MipiCamCtrlLogic2_Wled1FlashMaxCurrent;  ///< Offset 1278    WLED1 Flash Max Current
  UINT8    MipiCamCtrlLogic2_Wled1TorchMaxCurrent;  ///< Offset 1279    WLED1 Torch Max Current
  UINT8    MipiCamCtrlLogic2_Wled2FlashMaxCurrent;  ///< Offset 1280    WLED2 Flash Max Current
  UINT8    MipiCamCtrlLogic2_Wled2TorchMaxCurrent;  ///< Offset 1281    WLED2 Torch Max Current
  UINT8    MipiCamCtrlLogic2_SubPlatformId;         ///< Offset 1282    Sub Platform Id
  UINT8    MipiCamCtrlLogic2_Wled1Type;             ///< Offset 1283    WLED1 Type
  UINT8    MipiCamCtrlLogic2_Wled2Type;             ///< Offset 1284    WLED2 Type
  UINT8    MipiCamCtrlLogic2_PchClockSource;        ///< Offset 1285    PCH Clock source
  // Control Logic 3 options
  UINT8    MipiCamCtrlLogic3_Version;               ///< Offset 1286    Version of CLDB structure
  UINT8    MipiCamCtrlLogic3_Type;                  ///< Offset 1287    Type
  UINT8    MipiCamCtrlLogic3_CrdVersion;            ///< Offset 1288    Version of CRD
  UINT32   MipiCamCtrlLogic3_InputClock;            ///< Offset 1289    Input Clock
  UINT8    MipiCamCtrlLogic3_GpioPinsEnabled;       ///< Offset 1293    Number of GPIO Pins enabled
  UINT8    MipiCamCtrlLogic3_I2cBus;                ///< Offset 1294    I2C Serial Bus Number
  UINT16   MipiCamCtrlLogic3_I2cAddress;            ///< Offset 1295    I2C Address
  UINT8    MipiCamCtrlLogic3_GpioGroupPadNumber[4]; ///< Offset 1297    GPIO Group Pad Number
  UINT8    MipiCamCtrlLogic3_GpioGroupNumber[4];    ///< Offset 1301    GPIO Group Number
  UINT8    MipiCamCtrlLogic3_GpioFunction[4];       ///< Offset 1305    GPIO Function
  UINT8    MipiCamCtrlLogic3_GpioActiveValue[4];    ///< Offset 1309    GPIO Active Value
  UINT8    MipiCamCtrlLogic3_GpioInitialValue[4];   ///< Offset 1313    GPIO Initial Value
  UINT8    MipiCamCtrlLogic3_Pld;                   ///< Offset 1317    Camera Position
  UINT8    MipiCamCtrlLogic3_Wled1FlashMaxCurrent;  ///< Offset 1318    WLED1 Flash Max Current
  UINT8    MipiCamCtrlLogic3_Wled1TorchMaxCurrent;  ///< Offset 1319    WLED1 Torch Max Current
  UINT8    MipiCamCtrlLogic3_Wled2FlashMaxCurrent;  ///< Offset 1320    WLED2 Flash Max Current
  UINT8    MipiCamCtrlLogic3_Wled2TorchMaxCurrent;  ///< Offset 1321    WLED2 Torch Max Current
  UINT8    MipiCamCtrlLogic3_SubPlatformId;         ///< Offset 1322    Sub Platform Id
  UINT8    MipiCamCtrlLogic3_Wled1Type;             ///< Offset 1323    WLED1 Type
  UINT8    MipiCamCtrlLogic3_Wled2Type;             ///< Offset 1324    WLED2 Type
  UINT8    MipiCamCtrlLogic3_PchClockSource;        ///< Offset 1325    PCH Clock source
  // Control Logic 4 options
  UINT8    MipiCamCtrlLogic4_Version;               ///< Offset 1326    Version of CLDB structure
  UINT8    MipiCamCtrlLogic4_Type;                  ///< Offset 1327    Type
  UINT8    MipiCamCtrlLogic4_CrdVersion;            ///< Offset 1328    Version of CRD
  UINT32   MipiCamCtrlLogic4_InputClock;            ///< Offset 1329    Input Clock
  UINT8    MipiCamCtrlLogic4_GpioPinsEnabled;       ///< Offset 1333    Number of GPIO Pins enabled
  UINT8    MipiCamCtrlLogic4_I2cBus;                ///< Offset 1334    I2C Serial Bus Number
  UINT16   MipiCamCtrlLogic4_I2cAddress;            ///< Offset 1335    I2C Address
  UINT8    MipiCamCtrlLogic4_GpioGroupPadNumber[4]; ///< Offset 1337    GPIO Group Pad Number
  UINT8    MipiCamCtrlLogic4_GpioGroupNumber[4];    ///< Offset 1341    GPIO Group Number
  UINT8    MipiCamCtrlLogic4_GpioFunction[4];       ///< Offset 1345    GPIO Function
  UINT8    MipiCamCtrlLogic4_GpioActiveValue[4];    ///< Offset 1349    GPIO Active Value
  UINT8    MipiCamCtrlLogic4_GpioInitialValue[4];   ///< Offset 1353    GPIO Initial Value
  UINT8    MipiCamCtrlLogic4_Pld;                   ///< Offset 1357    Camera Position
  UINT8    MipiCamCtrlLogic4_Wled1FlashMaxCurrent;  ///< Offset 1358    WLED1 Flash Max Current
  UINT8    MipiCamCtrlLogic4_Wled1TorchMaxCurrent;  ///< Offset 1359    WLED1 Torch Max Current
  UINT8    MipiCamCtrlLogic4_Wled2FlashMaxCurrent;  ///< Offset 1360    WLED2 Flash Max Current
  UINT8    MipiCamCtrlLogic4_Wled2TorchMaxCurrent;  ///< Offset 1361    WLED2 Torch Max Current
  UINT8    MipiCamCtrlLogic4_SubPlatformId;         ///< Offset 1362    Sub Platform Id
  UINT8    MipiCamCtrlLogic4_Wled1Type;             ///< Offset 1363    WLED1 Type
  UINT8    MipiCamCtrlLogic4_Wled2Type;             ///< Offset 1364    WLED2 Type
  UINT8    MipiCamCtrlLogic4_PchClockSource;        ///< Offset 1365    PCH Clock source
  // Control Logic 5 options
  UINT8    MipiCamCtrlLogic5_Version;               ///< Offset 1366    Version of CLDB structure
  UINT8    MipiCamCtrlLogic5_Type;                  ///< Offset 1367    Type
  UINT8    MipiCamCtrlLogic5_CrdVersion;            ///< Offset 1368    Version of CRD
  UINT32   MipiCamCtrlLogic5_InputClock;            ///< Offset 1369    Input Clock
  UINT8    MipiCamCtrlLogic5_GpioPinsEnabled;       ///< Offset 1373    Number of GPIO Pins enabled
  UINT8    MipiCamCtrlLogic5_I2cBus;                ///< Offset 1374    I2C Serial Bus Number
  UINT16   MipiCamCtrlLogic5_I2cAddress;            ///< Offset 1375    I2C Address
  UINT8    MipiCamCtrlLogic5_GpioGroupPadNumber[4]; ///< Offset 1377    GPIO Group Pad Number
  UINT8    MipiCamCtrlLogic5_GpioGroupNumber[4];    ///< Offset 1381    GPIO Group Number
  UINT8    MipiCamCtrlLogic5_GpioFunction[4];       ///< Offset 1385    GPIO Function
  UINT8    MipiCamCtrlLogic5_GpioActiveValue[4];    ///< Offset 1389    GPIO Active Value
  UINT8    MipiCamCtrlLogic5_GpioInitialValue[4];   ///< Offset 1393    GPIO Initial Value
  UINT8    MipiCamCtrlLogic5_Pld;                   ///< Offset 1397    Camera Position
  UINT8    MipiCamCtrlLogic5_Wled1FlashMaxCurrent;  ///< Offset 1398    WLED1 Flash Max Current
  UINT8    MipiCamCtrlLogic5_Wled1TorchMaxCurrent;  ///< Offset 1399    WLED1 Torch Max Current
  UINT8    MipiCamCtrlLogic5_Wled2FlashMaxCurrent;  ///< Offset 1400    WLED2 Flash Max Current
  UINT8    MipiCamCtrlLogic5_Wled2TorchMaxCurrent;  ///< Offset 1401    WLED2 Torch Max Current
  UINT8    MipiCamCtrlLogic5_SubPlatformId;         ///< Offset 1402    Sub Platform Id
  UINT8    MipiCamCtrlLogic5_Wled1Type;             ///< Offset 1403    WLED1 Type
  UINT8    MipiCamCtrlLogic5_Wled2Type;             ///< Offset 1404    WLED2 Type
  UINT8    MipiCamCtrlLogic5_PchClockSource;        ///< Offset 1405    PCH Clock source
  // Mipi Cam Link0 options
  UINT8    MipiCamLink0SensorModel;                 ///< Offset 1406    Sensor Model
  UINT8    MipiCamLink0UserHid[9];                  ///< Offset 1407    User defined HID ASCII character 0
                                                    ///< Offset 1415    User defined HID ASCII character 8
  UINT8    MipiCamLink0Pld;                         ///< Offset 1416    Camera Position
  UINT8    MipiCamLink0ModuleName[16];              ///< Offset 1417    Camera Module Name ASCII character 0
                                                    ///< Offset 1432    Camera Module Name ASCII character 15
  UINT8    MipiCamLink0I2cDevicesEnabled;           ///< Offset 1433    Number of I2C devices
  UINT8    MipiCamLink0I2cBus;                      ///< Offset 1434    I2C Serial Bus number
  UINT16   MipiCamLink0I2cAddrDev[12];              ///< Offset 1435    Address of I2C Device0 on Link0
                                                    ///< Offset 1437    Address of I2C Device1 on Link0
                                                    ///< Offset 1439    Address of I2C Device2 on Link0
                                                    ///< Offset 1441    Address of I2C Device3 on Link0
                                                    ///< Offset 1443    Address of I2C Device4 on Link0
                                                    ///< Offset 1445    Address of I2C Device5 on Link0
                                                    ///< Offset 1447    Address of I2C Device6 on Link0
                                                    ///< Offset 1449    Address of I2C Device7 on Link0
                                                    ///< Offset 1451    Address of I2C Device8 on Link0
                                                    ///< Offset 1453    Address of I2C Device9 on Link0
                                                    ///< Offset 1455    Address of I2C Device10 on Link0
                                                    ///< Offset 1457    Address of I2C Device11 on Link0
  UINT8    MipiCamLink0I2cDeviceType[12];           ///< Offset 1459    Type of I2C Device0 on Link0
                                                    ///< Offset 1460    Type of I2C Device1 on Link0
                                                    ///< Offset 1461    Type of I2C Device2 on Link0
                                                    ///< Offset 1462    Type of I2C Device3 on Link0
                                                    ///< Offset 1463    Type of I2C Device4 on Link0
                                                    ///< Offset 1464    Type of I2C Device5 on Link0
                                                    ///< Offset 1465    Type of I2C Device6 on Link0
                                                    ///< Offset 1466    Type of I2C Device7 on Link0
                                                    ///< Offset 1467    Type of I2C Device8 on Link0
                                                    ///< Offset 1468    Type of I2C Device9 on Link0
                                                    ///< Offset 1469    Type of I2C Device10 on Link0
                                                    ///< Offset 1470    Type of I2C Device11 on Link0
  UINT8    MipiCamLink0DD_Version;                  ///< Offset 1471    Version of SSDB structure
  UINT8    MipiCamLink0DD_CrdVersion;               ///< Offset 1472    Version of CRD
  UINT8    MipiCamLink0DD_LinkUsed;                 ///< Offset 1473    CSI2 Link used
  UINT8    MipiCamLink0DD_LaneUsed;                 ///< Offset 1474    MIPI-CSI2 Data Lane
  UINT8    MipiCamLink0DD_EepromType;               ///< Offset 1475    EEPROM Type
  UINT8    MipiCamLink0DD_VcmType;                  ///< Offset 1476    VCM Type
  UINT8    MipiCamLink0DD_FlashSupport;             ///< Offset 1477    Flash Support
  UINT8    MipiCamLink0DD_PrivacyLed;               ///< Offset 1478    Privacy LED
  UINT8    MipiCamLink0DD_Degree;                   ///< Offset 1479    Degree
  UINT32   MipiCamLink0DD_Mclk;                     ///< Offset 1480    MCLK
  UINT8    MipiCamLink0DD_ControlLogic;             ///< Offset 1484    Control Logic
  UINT8    MipiCamLink0DD_PmicPosition;             ///< Offset 1485    PMIC Position
  UINT8    MipiCamLink0DD_VoltageRail;              ///< Offset 1486    Voltage Rail
  UINT8    MipiCamLink0DD_PprValue;                 ///< Offset 1487    PPR Value
  UINT8    MipiCamLink0DD_PprUnit;                  ///< Offset 1488    PPR Unit
  UINT8    MipiCamLink0_FlashDriverSelection;       ///< Offset 1489    Flash Driver Selection
  // Mipi Cam Link1 options
  UINT8    MipiCamLink1SensorModel;                 ///< Offset 1490    Sensor Model
  UINT8    MipiCamLink1UserHid[9];                  ///< Offset 1491    User defined HID ASCII character 0
                                                    ///< Offset 1499    User defined HID ASCII character 8
  UINT8    MipiCamLink1Pld;                         ///< Offset 1500    Camera Position
  UINT8    MipiCamLink1ModuleName[16];              ///< Offset 1501    Camera Module Name ASCII character 0
                                                    ///< Offset 1516    Camera Module Name ASCII character 15
  UINT8    MipiCamLink1I2cDevicesEnabled;           ///< Offset 1517    Number of I2C devices
  UINT8    MipiCamLink1I2cBus;                      ///< Offset 1518    I2C Serial Bus number
  UINT16   MipiCamLink1I2cAddrDev[12];              ///< Offset 1519    Address of I2C Device0 on Link1
                                                    ///< Offset 1521    Address of I2C Device1 on Link1
                                                    ///< Offset 1523    Address of I2C Device2 on Link1
                                                    ///< Offset 1525    Address of I2C Device3 on Link1
                                                    ///< Offset 1527    Address of I2C Device4 on Link1
                                                    ///< Offset 1529    Address of I2C Device5 on Link1
                                                    ///< Offset 1531    Address of I2C Device6 on Link1
                                                    ///< Offset 1533    Address of I2C Device7 on Link1
                                                    ///< Offset 1535    Address of I2C Device8 on Link1
                                                    ///< Offset 1537    Address of I2C Device9 on Link1
                                                    ///< Offset 1539    Address of I2C Device10 on Link1
                                                    ///< Offset 1541    Address of I2C Device11 on Link1
  UINT8    MipiCamLink1I2cDeviceType[12];           ///< Offset 1543    Type of I2C Device0 on Link1
                                                    ///< Offset 1544    Type of I2C Device1 on Link1
                                                    ///< Offset 1545    Type of I2C Device2 on Link1
                                                    ///< Offset 1546    Type of I2C Device3 on Link1
                                                    ///< Offset 1547    Type of I2C Device4 on Link1
                                                    ///< Offset 1548    Type of I2C Device5 on Link1
                                                    ///< Offset 1549    Type of I2C Device6 on Link1
                                                    ///< Offset 1550    Type of I2C Device7 on Link1
                                                    ///< Offset 1551    Type of I2C Device8 on Link1
                                                    ///< Offset 1552    Type of I2C Device9 on Link1
                                                    ///< Offset 1553    Type of I2C Device10 on Link1
                                                    ///< Offset 1554    Type of I2C Device11 on Link1
  UINT8    MipiCamLink1DD_Version;                  ///< Offset 1555    Version of SSDB structure
  UINT8    MipiCamLink1DD_CrdVersion;               ///< Offset 1556    Version of CRD
  UINT8    MipiCamLink1DD_LinkUsed;                 ///< Offset 1557    CSI2 Link used
  UINT8    MipiCamLink1DD_LaneUsed;                 ///< Offset 1558    MIPI-CSI2 Data Lane
  UINT8    MipiCamLink1DD_EepromType;               ///< Offset 1559    EEPROM Type
  UINT8    MipiCamLink1DD_VcmType;                  ///< Offset 1560    VCM Type
  UINT8    MipiCamLink1DD_FlashSupport;             ///< Offset 1561    Flash Support
  UINT8    MipiCamLink1DD_PrivacyLed;               ///< Offset 1562    Privacy LED
  UINT8    MipiCamLink1DD_Degree;                   ///< Offset 1563    Degree
  UINT32   MipiCamLink1DD_Mclk;                     ///< Offset 1564    MCLK
  UINT8    MipiCamLink1DD_ControlLogic;             ///< Offset 1568    Control Logic
  UINT8    MipiCamLink1DD_PmicPosition;             ///< Offset 1569    PMIC Position
  UINT8    MipiCamLink1DD_VoltageRail;              ///< Offset 1570    Voltage Rail
  UINT8    MipiCamLink1DD_PprValue;                 ///< Offset 1571    PPR Value
  UINT8    MipiCamLink1DD_PprUnit;                  ///< Offset 1572    PPR Unit
  UINT8    MipiCamLink1_FlashDriverSelection;       ///< Offset 1573    Flash Driver Selection
  // Mipi Cam Link2 options
  UINT8    MipiCamLink2SensorModel;                 ///< Offset 1574    Sensor Model
  UINT8    MipiCamLink2UserHid[9];                  ///< Offset 1575    User defined HID ASCII character 0
                                                    ///< Offset 1583    User defined HID ASCII character 8
  UINT8    MipiCamLink2Pld;                         ///< Offset 1584    Camera Position
  UINT8    MipiCamLink2ModuleName[16];              ///< Offset 1585    Camera Module Name ASCII character 0
                                                    ///< Offset 1600    Camera Module Name ASCII character 15
  UINT8    MipiCamLink2I2cDevicesEnabled;           ///< Offset 1601    Number of I2C devices
  UINT8    MipiCamLink2I2cBus;                      ///< Offset 1602    I2C Serial Bus number
  UINT16   MipiCamLink2I2cAddrDev[12];              ///< Offset 1603    Address of I2C Device0 on Link2
                                                    ///< Offset 1605    Address of I2C Device1 on Link2
                                                    ///< Offset 1607    Address of I2C Device2 on Link2
                                                    ///< Offset 1609    Address of I2C Device3 on Link2
                                                    ///< Offset 1611    Address of I2C Device4 on Link2
                                                    ///< Offset 1613    Address of I2C Device5 on Link2
                                                    ///< Offset 1615    Address of I2C Device6 on Link2
                                                    ///< Offset 1617    Address of I2C Device7 on Link2
                                                    ///< Offset 1619    Address of I2C Device8 on Link2
                                                    ///< Offset 1621    Address of I2C Device9 on Link2
                                                    ///< Offset 1623    Address of I2C Device10 on Link2
                                                    ///< Offset 1625    Address of I2C Device11 on Link2
  UINT8    MipiCamLink2I2cDeviceType[12];           ///< Offset 1627    Type of I2C Device0 on Link2
                                                    ///< Offset 1628    Type of I2C Device1 on Link2
                                                    ///< Offset 1629    Type of I2C Device2 on Link2
                                                    ///< Offset 1630    Type of I2C Device3 on Link2
                                                    ///< Offset 1631    Type of I2C Device4 on Link2
                                                    ///< Offset 1632    Type of I2C Device5 on Link2
                                                    ///< Offset 1633    Type of I2C Device6 on Link2
                                                    ///< Offset 1634    Type of I2C Device7 on Link2
                                                    ///< Offset 1635    Type of I2C Device8 on Link2
                                                    ///< Offset 1636    Type of I2C Device9 on Link2
                                                    ///< Offset 1637    Type of I2C Device10 on Link2
                                                    ///< Offset 1638    Type of I2C Device11 on Link2
  UINT8    MipiCamLink2DD_Version;                  ///< Offset 1639    Version of SSDB structure
  UINT8    MipiCamLink2DD_CrdVersion;               ///< Offset 1640    Version of CRD
  UINT8    MipiCamLink2DD_LinkUsed;                 ///< Offset 1641    CSI2 Link used
  UINT8    MipiCamLink2DD_LaneUsed;                 ///< Offset 1642    MIPI-CSI2 Data Lane
  UINT8    MipiCamLink2DD_EepromType;               ///< Offset 1643    EEPROM Type
  UINT8    MipiCamLink2DD_VcmType;                  ///< Offset 1644    VCM Type
  UINT8    MipiCamLink2DD_FlashSupport;             ///< Offset 1645    Flash Support
  UINT8    MipiCamLink2DD_PrivacyLed;               ///< Offset 1646    Privacy LED
  UINT8    MipiCamLink2DD_Degree;                   ///< Offset 1647    Degree
  UINT32   MipiCamLink2DD_Mclk;                     ///< Offset 1648    MCLK
  UINT8    MipiCamLink2DD_ControlLogic;             ///< Offset 1652    Control Logic
  UINT8    MipiCamLink2DD_PmicPosition;             ///< Offset 1653    PMIC Position
  UINT8    MipiCamLink2DD_VoltageRail;              ///< Offset 1654    Voltage Rail
  UINT8    MipiCamLink2DD_PprValue;                 ///< Offset 1655    PPR Value
  UINT8    MipiCamLink2DD_PprUnit;                  ///< Offset 1656    PPR Unit
  UINT8    MipiCamLink2_FlashDriverSelection;       ///< Offset 1657    Flash Driver Selection
  // Mipi Cam Link3 options
  UINT8    MipiCamLink3SensorModel;                 ///< Offset 1658    Sensor Model
  UINT8    MipiCamLink3UserHid[9];                  ///< Offset 1659    User defined HID ASCII character 0
                                                    ///< Offset 1667    User defined HID ASCII character 8
  UINT8    MipiCamLink3Pld;                         ///< Offset 1668    Camera Position
  UINT8    MipiCamLink3ModuleName[16];              ///< Offset 1669    Camera Module Name ASCII character 0
                                                    ///< Offset 1684    Camera Module Name ASCII character 15
  UINT8    MipiCamLink3I2cDevicesEnabled;           ///< Offset 1685    Number of I2C devices
  UINT8    MipiCamLink3I2cBus;                      ///< Offset 1686    I2C Serial Bus number
  UINT16   MipiCamLink3I2cAddrDev[12];              ///< Offset 1687    Address of I2C Device0 on Link3
                                                    ///< Offset 1689    Address of I2C Device1 on Link3
                                                    ///< Offset 1691    Address of I2C Device2 on Link3
                                                    ///< Offset 1693    Address of I2C Device3 on Link3
                                                    ///< Offset 1695    Address of I2C Device4 on Link3
                                                    ///< Offset 1697    Address of I2C Device5 on Link3
                                                    ///< Offset 1699    Address of I2C Device6 on Link3
                                                    ///< Offset 1701    Address of I2C Device7 on Link3
                                                    ///< Offset 1703    Address of I2C Device8 on Link3
                                                    ///< Offset 1705    Address of I2C Device9 on Link3
                                                    ///< Offset 1707    Address of I2C Device10 on Link3
                                                    ///< Offset 1709    Address of I2C Device11 on Link3
  UINT8    MipiCamLink3I2cDeviceType[12];           ///< Offset 1711    Type of I2C Device0 on Link3
                                                    ///< Offset 1712    Type of I2C Device1 on Link3
                                                    ///< Offset 1713    Type of I2C Device2 on Link3
                                                    ///< Offset 1714    Type of I2C Device3 on Link3
                                                    ///< Offset 1715    Type of I2C Device4 on Link3
                                                    ///< Offset 1716    Type of I2C Device5 on Link3
                                                    ///< Offset 1717    Type of I2C Device6 on Link3
                                                    ///< Offset 1718    Type of I2C Device7 on Link3
                                                    ///< Offset 1719    Type of I2C Device8 on Link3
                                                    ///< Offset 1720    Type of I2C Device9 on Link3
                                                    ///< Offset 1721    Type of I2C Device10 on Link3
                                                    ///< Offset 1722    Type of I2C Device11 on Link3
  UINT8    MipiCamLink3DD_Version;                  ///< Offset 1723    Version of SSDB structure
  UINT8    MipiCamLink3DD_CrdVersion;               ///< Offset 1724    Version of CRD
  UINT8    MipiCamLink3DD_LinkUsed;                 ///< Offset 1725    CSI2 Link used
  UINT8    MipiCamLink3DD_LaneUsed;                 ///< Offset 1726    MIPI-CSI2 Data Lane
  UINT8    MipiCamLink3DD_EepromType;               ///< Offset 1727    EEPROM Type
  UINT8    MipiCamLink3DD_VcmType;                  ///< Offset 1728    VCM Type
  UINT8    MipiCamLink3DD_FlashSupport;             ///< Offset 1729    Flash Support
  UINT8    MipiCamLink3DD_PrivacyLed;               ///< Offset 1730    Privacy LED
  UINT8    MipiCamLink3DD_Degree;                   ///< Offset 1731    Degree
  UINT32   MipiCamLink3DD_Mclk;                     ///< Offset 1732    MCLK
  UINT8    MipiCamLink3DD_ControlLogic;             ///< Offset 1736    Control Logic
  UINT8    MipiCamLink3DD_PmicPosition;             ///< Offset 1737    PMIC Position
  UINT8    MipiCamLink3DD_VoltageRail;              ///< Offset 1738    Voltage Rail
  UINT8    MipiCamLink3DD_PprValue;                 ///< Offset 1739    PPR Value
  UINT8    MipiCamLink3DD_PprUnit;                  ///< Offset 1740    PPR Unit
  UINT8    MipiCamLink3_FlashDriverSelection;       ///< Offset 1741    Flash Driver Selection
  // Mipi Cam Link4 options
  UINT8    MipiCamLink4SensorModel;                 ///< Offset 1742    Sensor Model
  UINT8    MipiCamLink4UserHid[9];                  ///< Offset 1743    User defined HID ASCII character 0
                                                    ///< Offset 1751    User defined HID ASCII character 8
  UINT8    MipiCamLink4Pld;                         ///< Offset 1752    Camera Position
  UINT8    MipiCamLink4ModuleName[16];              ///< Offset 1753    Camera Module Name ASCII character 0
                                                    ///< Offset 1768    Camera Module Name ASCII character 15
  UINT8    MipiCamLink4I2cDevicesEnabled;           ///< Offset 1769    Number of I2C devices
  UINT8    MipiCamLink4I2cBus;                      ///< Offset 1770    I2C Serial Bus number
  UINT16   MipiCamLink4I2cAddrDev[12];              ///< Offset 1771    Address of I2C Device0 on Link4
                                                    ///< Offset 1773    Address of I2C Device1 on Link4
                                                    ///< Offset 1775    Address of I2C Device2 on Link4
                                                    ///< Offset 1777    Address of I2C Device3 on Link4
                                                    ///< Offset 1779    Address of I2C Device4 on Link4
                                                    ///< Offset 1781    Address of I2C Device5 on Link4
                                                    ///< Offset 1783    Address of I2C Device6 on Link4
                                                    ///< Offset 1785    Address of I2C Device7 on Link4
                                                    ///< Offset 1787    Address of I2C Device8 on Link4
                                                    ///< Offset 1789    Address of I2C Device9 on Link4
                                                    ///< Offset 1791    Address of I2C Device10 on Link4
                                                    ///< Offset 1793    Address of I2C Device11 on Link4
  UINT8    MipiCamLink4I2cDeviceType[12];           ///< Offset 1795    Type of I2C Device0 on Link4
                                                    ///< Offset 1796    Type of I2C Device1 on Link4
                                                    ///< Offset 1797    Type of I2C Device2 on Link4
                                                    ///< Offset 1798    Type of I2C Device3 on Link4
                                                    ///< Offset 1799    Type of I2C Device4 on Link4
                                                    ///< Offset 1800    Type of I2C Device5 on Link4
                                                    ///< Offset 1801    Type of I2C Device6 on Link4
                                                    ///< Offset 1802    Type of I2C Device7 on Link4
                                                    ///< Offset 1803    Type of I2C Device8 on Link4
                                                    ///< Offset 1804    Type of I2C Device9 on Link4
                                                    ///< Offset 1805    Type of I2C Device10 on Link4
                                                    ///< Offset 1806    Type of I2C Device11 on Link4
  UINT8    MipiCamLink4DD_Version;                  ///< Offset 1807    Version of SSDB structure
  UINT8    MipiCamLink4DD_CrdVersion;               ///< Offset 1808    Version of CRD
  UINT8    MipiCamLink4DD_LinkUsed;                 ///< Offset 1809    CSI2 Link used
  UINT8    MipiCamLink4DD_LaneUsed;                 ///< Offset 1810    MIPI-CSI2 Data Lane
  UINT8    MipiCamLink4DD_EepromType;               ///< Offset 1811    EEPROM Type
  UINT8    MipiCamLink4DD_VcmType;                  ///< Offset 1812    VCM Type
  UINT8    MipiCamLink4DD_FlashSupport;             ///< Offset 1813    Flash Support
  UINT8    MipiCamLink4DD_PrivacyLed;               ///< Offset 1814    Privacy LED
  UINT8    MipiCamLink4DD_Degree;                   ///< Offset 1815    Degree
  UINT32   MipiCamLink4DD_Mclk;                     ///< Offset 1816    MCLK
  UINT8    MipiCamLink4DD_ControlLogic;             ///< Offset 1820    Control Logic
  UINT8    MipiCamLink4DD_PmicPosition;             ///< Offset 1821    PMIC Position
  UINT8    MipiCamLink4DD_VoltageRail;              ///< Offset 1822    Voltage Rail
  UINT8    MipiCamLink4DD_PprValue;                 ///< Offset 1823    PPR Value
  UINT8    MipiCamLink4DD_PprUnit;                  ///< Offset 1824    PPR Unit
  UINT8    MipiCamLink4_FlashDriverSelection;       ///< Offset 1825    Flash Driver Selection
  // Mipi Cam Link5 options
  UINT8    MipiCamLink5SensorModel;                 ///< Offset 1826    Sensor Model
  UINT8    MipiCamLink5UserHid[9];                  ///< Offset 1827    User defined HID ASCII character 0
                                                    ///< Offset 1835    User defined HID ASCII character 8
  UINT8    MipiCamLink5Pld;                         ///< Offset 1836    Camera Position
  UINT8    MipiCamLink5ModuleName[16];              ///< Offset 1837    Camera Module Name ASCII character 0
                                                    ///< Offset 1852    Camera Module Name ASCII character 15
  UINT8    MipiCamLink5I2cDevicesEnabled;           ///< Offset 1853    Number of I2C devices
  UINT8    MipiCamLink5I2cBus;                      ///< Offset 1854    I2C Serial Bus number
  UINT16   MipiCamLink5I2cAddrDev[12];              ///< Offset 1855    Address of I2C Device0 on Link5
                                                    ///< Offset 1857    Address of I2C Device1 on Link5
                                                    ///< Offset 1859    Address of I2C Device2 on Link5
                                                    ///< Offset 1861    Address of I2C Device3 on Link5
                                                    ///< Offset 1863    Address of I2C Device4 on Link5
                                                    ///< Offset 1865    Address of I2C Device5 on Link5
                                                    ///< Offset 1867    Address of I2C Device6 on Link5
                                                    ///< Offset 1869    Address of I2C Device7 on Link5
                                                    ///< Offset 1871    Address of I2C Device8 on Link5
                                                    ///< Offset 1873    Address of I2C Device9 on Link5
                                                    ///< Offset 1875    Address of I2C Device10 on Link5
                                                    ///< Offset 1877    Address of I2C Device11 on Link5
  UINT8    MipiCamLink5I2cDeviceType[12];           ///< Offset 1879    Type of I2C Device0 on Link5
                                                    ///< Offset 1880    Type of I2C Device1 on Link5
                                                    ///< Offset 1881    Type of I2C Device2 on Link5
                                                    ///< Offset 1882    Type of I2C Device3 on Link5
                                                    ///< Offset 1883    Type of I2C Device4 on Link5
                                                    ///< Offset 1884    Type of I2C Device5 on Link5
                                                    ///< Offset 1885    Type of I2C Device6 on Link5
                                                    ///< Offset 1886    Type of I2C Device7 on Link5
                                                    ///< Offset 1887    Type of I2C Device8 on Link5
                                                    ///< Offset 1888    Type of I2C Device9 on Link5
                                                    ///< Offset 1889    Type of I2C Device10 on Link5
                                                    ///< Offset 1890    Type of I2C Device11 on Link5
  UINT8    MipiCamLink5DD_Version;                  ///< Offset 1891    Version of SSDB structure
  UINT8    MipiCamLink5DD_CrdVersion;               ///< Offset 1892    Version of CRD
  UINT8    MipiCamLink5DD_LinkUsed;                 ///< Offset 1893    CSI2 Link used
  UINT8    MipiCamLink5DD_LaneUsed;                 ///< Offset 1894    MIPI-CSI2 Data Lane
  UINT8    MipiCamLink5DD_EepromType;               ///< Offset 1895    EEPROM Type
  UINT8    MipiCamLink5DD_VcmType;                  ///< Offset 1896    VCM Type
  UINT8    MipiCamLink5DD_FlashSupport;             ///< Offset 1897    Flash Support
  UINT8    MipiCamLink5DD_PrivacyLed;               ///< Offset 1898    Privacy LED
  UINT8    MipiCamLink5DD_Degree;                   ///< Offset 1899    Degree
  UINT32   MipiCamLink5DD_Mclk;                     ///< Offset 1900    MCLK
  UINT8    MipiCamLink5DD_ControlLogic;             ///< Offset 1904    Control Logic
  UINT8    MipiCamLink5DD_PmicPosition;             ///< Offset 1905    PMIC Position
  UINT8    MipiCamLink5DD_VoltageRail;              ///< Offset 1906    Voltage Rail
  UINT8    MipiCamLink5DD_PprValue;                 ///< Offset 1907    PPR Value
  UINT8    MipiCamLink5DD_PprUnit;                  ///< Offset 1908    PPR Unit
  UINT8    MipiCamLink5_FlashDriverSelection;       ///< Offset 1909    Flash Driver Selection
  UINT8    MipiCamFlash0Model;                      ///< Offset 1910    Flash Driver Model
  UINT8    MipiCamFlash0Mode;                       ///< Offset 1911    Flash Mode Selection
  UINT8    MipiCamFlash0ModuleName[16];             ///< Offset 1912    Flash Module Name ASCII character 0
                                                    ///< Offset 1927    Flash Module Name ASCII character 15
  UINT8    MipiCamFlash0I2cBus;                     ///< Offset 1928    I2C Bus Number
  UINT16   MipiCamFlash0I2cAddrDev;                 ///< Offset 1929    I2C Slave Device Address
  UINT8    MipiCamFlash0GpioGroupPadNumber;         ///< Offset 1931    GPIO Group Pad Number
  UINT16   MipiCamFlash0GpioGroupNumber;            ///< Offset 1932    GPIO Group Number
  UINT8    MipiCamFlash0GpioActiveValue;            ///< Offset 1934    GPIO Active Value
  UINT8    MipiCamFlash0GpioInitialValue;           ///< Offset 1935    GPIO Initial Value
  UINT8    MipiCamFlash0OperatingMode;              ///< Offset 1936    Flash Driver Operating Mode
  UINT8    MipiCamFlash1Model;                      ///< Offset 1937    Flash Driver Model
  UINT8    MipiCamFlash1Mode;                       ///< Offset 1938    Flash Mode Selection
  UINT8    MipiCamFlash1ModuleName[16];             ///< Offset 1939    Flash Module Name ASCII character 0
                                                    ///< Offset 1954    Flash Module Name ASCII character 15
  UINT8    MipiCamFlash1I2cBus;                     ///< Offset 1955    I2C Bus Number
  UINT16   MipiCamFlash1I2cAddrDev;                 ///< Offset 1956    I2C Slave Device Address
  UINT8    MipiCamFlash1GpioGroupPadNumber;         ///< Offset 1958    GPIO Group Pad Number
  UINT16   MipiCamFlash1GpioGroupNumber;            ///< Offset 1959    GPIO Group Number
  UINT8    MipiCamFlash1GpioActiveValue;            ///< Offset 1961    GPIO Active Value
  UINT8    MipiCamFlash1GpioInitialValue;           ///< Offset 1962    GPIO Initial Value
  UINT8    MipiCamFlash1OperatingMode;              ///< Offset 1963    Flash Driver Operating Mode
  UINT8    MipiCamFlash2Model;                      ///< Offset 1964    Flash Driver Model
  UINT8    MipiCamFlash2Mode;                       ///< Offset 1965    Flash Mode Selection
  UINT8    MipiCamFlash2ModuleName[16];             ///< Offset 1966    Flash Module Name ASCII character 0
                                                    ///< Offset 1981    Flash Module Name ASCII character 15
  UINT8    MipiCamFlash2I2cBus;                     ///< Offset 1982    I2C Bus Number
  UINT16   MipiCamFlash2I2cAddrDev;                 ///< Offset 1983    I2C Slave Device Address
  UINT8    MipiCamFlash2GpioGroupPadNumber;         ///< Offset 1985    GPIO Group Pad Number
  UINT16   MipiCamFlash2GpioGroupNumber;            ///< Offset 1986    GPIO Group Number
  UINT8    MipiCamFlash2GpioActiveValue;            ///< Offset 1988    GPIO Active Value
  UINT8    MipiCamFlash2GpioInitialValue;           ///< Offset 1989    GPIO Initial Value
  UINT8    MipiCamFlash2OperatingMode;              ///< Offset 1990    Flash Driver Operating Mode
  UINT8    MipiCamFlash3Model;                      ///< Offset 1991    Flash Driver Model
  UINT8    MipiCamFlash3Mode;                       ///< Offset 1992    Flash Mode Selection
  UINT8    MipiCamFlash3ModuleName[16];             ///< Offset 1993    Flash Module Name ASCII character 0
                                                    ///< Offset 2008    Flash Module Name ASCII character 15
  UINT8    MipiCamFlash3I2cBus;                     ///< Offset 2009    I2C Bus Number
  UINT16   MipiCamFlash3I2cAddrDev;                 ///< Offset 2010    I2C Slave Device Address
  UINT8    MipiCamFlash3GpioGroupPadNumber;         ///< Offset 2012    GPIO Group Pad Number
  UINT16   MipiCamFlash3GpioGroupNumber;            ///< Offset 2013    GPIO Group Number
  UINT8    MipiCamFlash3GpioActiveValue;            ///< Offset 2015    GPIO Active Value
  UINT8    MipiCamFlash3GpioInitialValue;           ///< Offset 2016    GPIO Initial Value
  UINT8    MipiCamFlash3OperatingMode;              ///< Offset 2017    Flash Driver Operating Mode
  UINT8    MipiCamFlash4Model;                      ///< Offset 2018    Flash Driver Model
  UINT8    MipiCamFlash4Mode;                       ///< Offset 2019    Flash Mode Selection
  UINT8    MipiCamFlash4ModuleName[16];             ///< Offset 2020    Flash Module Name ASCII character 0
                                                    ///< Offset 2035    Flash Module Name ASCII character 15
  UINT8    MipiCamFlash4I2cBus;                     ///< Offset 2036    I2C Bus Number
  UINT16   MipiCamFlash4I2cAddrDev;                 ///< Offset 2037    I2C Slave Device Address
  UINT8    MipiCamFlash4GpioGroupPadNumber;         ///< Offset 2039    GPIO Group Pad Number
  UINT16   MipiCamFlash4GpioGroupNumber;            ///< Offset 2040    GPIO Group Number
  UINT8    MipiCamFlash4GpioActiveValue;            ///< Offset 2042    GPIO Active Value
  UINT8    MipiCamFlash4GpioInitialValue;           ///< Offset 2043    GPIO Initial Value
  UINT8    MipiCamFlash4OperatingMode;              ///< Offset 2044    Flash Driver Operating Mode
  UINT8    MipiCamFlash5Model;                      ///< Offset 2045    Flash Driver Model
  UINT8    MipiCamFlash5Mode;                       ///< Offset 2046    Flash Mode Selection
  UINT8    MipiCamFlash5ModuleName[16];             ///< Offset 2047    Flash Module Name ASCII character 0
                                                    ///< Offset 2062    Flash Module Name ASCII character 15
  UINT8    MipiCamFlash5I2cBus;                     ///< Offset 2063    I2C Bus Number
  UINT16   MipiCamFlash5I2cAddrDev;                 ///< Offset 2064    I2C Slave Device Address
  UINT8    MipiCamFlash5GpioGroupPadNumber;         ///< Offset 2066    GPIO Group Pad Number
  UINT16   MipiCamFlash5GpioGroupNumber;            ///< Offset 2067    GPIO Group Number
  UINT8    MipiCamFlash5GpioActiveValue;            ///< Offset 2069    GPIO Active Value
  UINT8    MipiCamFlash5GpioInitialValue;           ///< Offset 2070    GPIO Initial Value
  UINT8    MipiCamFlash5OperatingMode;              ///< Offset 2071    Flash Driver Operating Mode
  UINT8    PciDelayOptimizationEcr;                 ///< Offset 2072
  UINT8    I2SC;                                    ///< Offset 2073    HD Audio I2S Codec Selection
  UINT32   I2SI;                                    ///< Offset 2074    HD Audio I2S Codec Interrupt Pin
  UINT8    I2SB;                                    ///< Offset 2078    HD Audio I2S Codec Connection to I2C bus controller instance (I2C[0-5])
  UINT8    OemDesignVariable0;                      ///< Offset 2079    Intel(R) Dynamic Tuning Technology Oem Design Variables
  UINT8    OemDesignVariable1;                      ///< Offset 2080    Intel(R) Dynamic Tuning Technology Oem Design Variables
  UINT8    OemDesignVariable2;                      ///< Offset 2081    Intel(R) Dynamic Tuning Technology Oem Design Variables
  UINT8    OemDesignVariable3;                      ///< Offset 2082    Intel(R) Dynamic Tuning Technology Oem Design Variables
  UINT8    OemDesignVariable4;                      ///< Offset 2083    Intel(R) Dynamic Tuning Technology Oem Design Variables
  UINT8    OemDesignVariable5;                      ///< Offset 2084    Intel(R) Dynamic Tuning Technology Oem Design Variables
  UINT32   UsbTypeCOpBaseAddr;                      ///< Offset 2085    USB Type C Opregion base address
  UINT8    WirelessCharging;                        ///< Offset 2089    WirelessCharging
  #ifdef TESTMENU_FLAG
  // RTD3 Settings
  UINT8    TestRtd3PcieRootports;                   ///< Offset 2090    Test menu Pcie Root Ports RTD3 Configuration
  UINT8    TestRtd3Usb;                             ///< Offset 2091    Test menu Usb RTD3 Configuration
  UINT8    TestRtd3AudioCodec;                      ///< Offset 2092    Test menu AudioCodec RTD3 Configuration
  UINT8    TestRtd3HdAudio;                         ///< Offset 2093    Test menu HD Audio RTD3 Configuration
  UINT8    TestRtd3TouchPanel;                      ///< Offset 2094    Test menu Touch Panel RTD3 Configuration
  // Pcie LAN WoL support
  UINT8    TestPcieFoxVilleWolEnable;               ///< Offset 2095    Test menu FoxVille Wake On Lane Enable
  #else
  UINT8    Reserved16[6];                           ///< Offset 2090:2095
  #endif
  UINT32   HdaDspPpModuleMask;                      ///< Offset 2096    HD-Audio DSP Post-Processing Module Mask
  UINT64   HdaDspPpModCustomGuid1Low;               ///< Offset 2100    HDA PP module custom GUID 1 - first 64bit  [0-63]
  UINT64   HdaDspPpModCustomGuid1High;              ///< Offset 2108    HDA PP module custom GUID 1 - second 64bit [64-127]
  UINT64   HdaDspPpModCustomGuid2Low;               ///< Offset 2116    HDA PP module custom GUID 2 - first 64bit  [0-63]
  UINT64   HdaDspPpModCustomGuid2High;              ///< Offset 2124    HDA PP module custom GUID 2 - second 64bit [64-127]
  UINT64   HdaDspPpModCustomGuid3Low;               ///< Offset 2132    HDA PP module custom GUID 3 - first 64bit  [0-63]
  UINT64   HdaDspPpModCustomGuid3High;              ///< Offset 2140    HDA PP module custom GUID 3 - second 64bit [64-127]
  UINT8    HidEventFilterEnable;                    ///< Offset 2148    HID Event Filter Driver enable
  UINT8    XdciFnEnable;                            ///< Offset 2149    XDCI Enable/Disable status
  UINT8    WrdsWiFiSarEnable;                       ///< Offset 2150    WrdsWiFiSarEnable
  UINT8    WrdsWiFiSarTxPowerSet1Limit1;            ///< Offset 2151    WrdsWiFiSarTxPowerSet1Limit1
  UINT8    WrdsWiFiSarTxPowerSet1Limit2;            ///< Offset 2152    WrdsWiFiSarTxPowerSet1Limit2
  UINT8    WrdsWiFiSarTxPowerSet1Limit3;            ///< Offset 2153    WrdsWiFiSarTxPowerSet1Limit3
  UINT8    WrdsWiFiSarTxPowerSet1Limit4;            ///< Offset 2154    WrdsWiFiSarTxPowerSet1Limit4
  UINT8    WrdsWiFiSarTxPowerSet1Limit5;            ///< Offset 2155    WrdsWiFiSarTxPowerSet1Limit5
  UINT8    WrdsWiFiSarTxPowerSet1Limit6;            ///< Offset 2156    WrdsWiFiSarTxPowerSet1Limit6
  UINT8    WrdsWiFiSarTxPowerSet1Limit7;            ///< Offset 2157    WrdsWiFiSarTxPowerSet1Limit7
  UINT8    WrdsWiFiSarTxPowerSet1Limit8;            ///< Offset 2158    WrdsWiFiSarTxPowerSet1Limit8
  UINT8    WrdsWiFiSarTxPowerSet1Limit9;            ///< Offset 2159    WrdsWiFiSarTxPowerSet1Limit9
  UINT8    WrdsWiFiSarTxPowerSet1Limit10;           ///< Offset 2160    WrdsWiFiSarTxPowerSet1Limit10
  UINT8    EnableVoltageMargining;                  ///< Offset 2161    Enable Voltage Margining
  UINT16   DStateHSPort;                            ///< Offset 2162    D-State for xHCI HS port(BIT0:USB HS Port0 ~ BIT15:USB HS Port15)
  UINT16   DStateSSPort;                            ///< Offset 2164    D-State for xHCI SS port(BIT0:USB SS Port0 ~ BIT15:USB SS Port15)
  UINT8    DStateSataPort;                          ///< Offset 2166    D-State for SATA port(BIT0:SATA Port0 ~ BIT7:SATA Port7)
  UINT8    EwrdWiFiDynamicSarEnable;                ///< Offset 2167    EwrdWiFiDynamicSarEnable
  UINT8    EwrdWiFiDynamicSarRangeSets;             ///< Offset 2168    EwrdWiFiDynamicSarRangeSets
  UINT8    EwrdWiFiSarTxPowerSet2Limit1;            ///< Offset 2169    EwrdWiFiSarTxPowerSet2Limit1
  UINT8    EwrdWiFiSarTxPowerSet2Limit2;            ///< Offset 2170    EwrdWiFiSarTxPowerSet2Limit2
  UINT8    EwrdWiFiSarTxPowerSet2Limit3;            ///< Offset 2171    EwrdWiFiSarTxPowerSet2Limit3
  UINT8    EwrdWiFiSarTxPowerSet2Limit4;            ///< Offset 2172    EwrdWiFiSarTxPowerSet2Limit4
  UINT8    EwrdWiFiSarTxPowerSet2Limit5;            ///< Offset 2173    EwrdWiFiSarTxPowerSet2Limit5
  UINT8    EwrdWiFiSarTxPowerSet2Limit6;            ///< Offset 2174    EwrdWiFiSarTxPowerSet2Limit6
  UINT8    EwrdWiFiSarTxPowerSet2Limit7;            ///< Offset 2175    EwrdWiFiSarTxPowerSet2Limit7
  UINT8    EwrdWiFiSarTxPowerSet2Limit8;            ///< Offset 2176    EwrdWiFiSarTxPowerSet2Limit8
  UINT8    EwrdWiFiSarTxPowerSet2Limit9;            ///< Offset 2177    EwrdWiFiSarTxPowerSet2Limit9
  UINT8    EwrdWiFiSarTxPowerSet2Limit10;           ///< Offset 2178    EwrdWiFiSarTxPowerSet2Limit10
  UINT8    EwrdWiFiSarTxPowerSet3Limit1;            ///< Offset 2179    EwrdWiFiSarTxPowerSet3Limit1
  UINT8    EwrdWiFiSarTxPowerSet3Limit2;            ///< Offset 2180    EwrdWiFiSarTxPowerSet3Limit2
  UINT8    EwrdWiFiSarTxPowerSet3Limit3;            ///< Offset 2181    EwrdWiFiSarTxPowerSet3Limit3
  UINT8    EwrdWiFiSarTxPowerSet3Limit4;            ///< Offset 2182    EwrdWiFiSarTxPowerSet3Limit4
  UINT8    EwrdWiFiSarTxPowerSet3Limit5;            ///< Offset 2183    EwrdWiFiSarTxPowerSet3Limit5
  UINT8    EwrdWiFiSarTxPowerSet3Limit6;            ///< Offset 2184    EwrdWiFiSarTxPowerSet3Limit6
  UINT8    EwrdWiFiSarTxPowerSet3Limit7;            ///< Offset 2185    EwrdWiFiSarTxPowerSet3Limit7
  UINT8    EwrdWiFiSarTxPowerSet3Limit8;            ///< Offset 2186    EwrdWiFiSarTxPowerSet3Limit8
  UINT8    EwrdWiFiSarTxPowerSet3Limit9;            ///< Offset 2187    EwrdWiFiSarTxPowerSet3Limit9
  UINT8    EwrdWiFiSarTxPowerSet3Limit10;           ///< Offset 2188    EwrdWiFiSarTxPowerSet3Limit10
  UINT8    EwrdWiFiSarTxPowerSet4Limit1;            ///< Offset 2189    EwrdWiFiSarTxPowerSet4Limit1
  UINT8    EwrdWiFiSarTxPowerSet4Limit2;            ///< Offset 2190    EwrdWiFiSarTxPowerSet4Limit2
  UINT8    EwrdWiFiSarTxPowerSet4Limit3;            ///< Offset 2191    EwrdWiFiSarTxPowerSet4Limit3
  UINT8    EwrdWiFiSarTxPowerSet4Limit4;            ///< Offset 2192    EwrdWiFiSarTxPowerSet4Limit4
  UINT8    EwrdWiFiSarTxPowerSet4Limit5;            ///< Offset 2193    EwrdWiFiSarTxPowerSet4Limit5
  UINT8    EwrdWiFiSarTxPowerSet4Limit6;            ///< Offset 2194    EwrdWiFiSarTxPowerSet4Limit6
  UINT8    EwrdWiFiSarTxPowerSet4Limit7;            ///< Offset 2195    EwrdWiFiSarTxPowerSet4Limit7
  UINT8    EwrdWiFiSarTxPowerSet4Limit8;            ///< Offset 2196    EwrdWiFiSarTxPowerSet4Limit8
  UINT8    EwrdWiFiSarTxPowerSet4Limit9;            ///< Offset 2197    EwrdWiFiSarTxPowerSet4Limit9
  UINT8    EwrdWiFiSarTxPowerSet4Limit10;           ///< Offset 2198    EwrdWiFiSarTxPowerSet4Limit10
  UINT8    WgdsWiFiSarDeltaGroup1PowerMax1;         ///< Offset 2199    WgdsWiFiSarDeltaGroup1PowerMax1
  UINT8    WgdsWiFiSarDeltaGroup1PowerChainA1;      ///< Offset 2200    WgdsWiFiSarDeltaGroup1PowerChainA1
  UINT8    WgdsWiFiSarDeltaGroup1PowerChainB1;      ///< Offset 2201    WgdsWiFiSarDeltaGroup1PowerChainB1
  UINT8    WgdsWiFiSarDeltaGroup1PowerMax2;         ///< Offset 2202    WgdsWiFiSarDeltaGroup1PowerMax2
  UINT8    WgdsWiFiSarDeltaGroup1PowerChainA2;      ///< Offset 2203    WgdsWiFiSarDeltaGroup1PowerChainA2
  UINT8    WgdsWiFiSarDeltaGroup1PowerChainB2;      ///< Offset 2204    WgdsWiFiSarDeltaGroup1PowerChainB2
  UINT8    WgdsWiFiSarDeltaGroup2PowerMax1;         ///< Offset 2205    WgdsWiFiSarDeltaGroup2PowerMax1
  UINT8    WgdsWiFiSarDeltaGroup2PowerChainA1;      ///< Offset 2206    WgdsWiFiSarDeltaGroup2PowerChainA1
  UINT8    WgdsWiFiSarDeltaGroup2PowerChainB1;      ///< Offset 2207    WgdsWiFiSarDeltaGroup2PowerChainB1
  UINT8    WgdsWiFiSarDeltaGroup2PowerMax2;         ///< Offset 2208    WgdsWiFiSarDeltaGroup2PowerMax2
  UINT8    WgdsWiFiSarDeltaGroup2PowerChainA2;      ///< Offset 2209    WgdsWiFiSarDeltaGroup2PowerChainA2
  UINT8    WgdsWiFiSarDeltaGroup2PowerChainB2;      ///< Offset 2210    WgdsWiFiSarDeltaGroup2PowerChainB2
  UINT8    WgdsWiFiSarDeltaGroup3PowerMax1;         ///< Offset 2211    WgdsWiFiSarDeltaGroup3PowerMax1
  UINT8    WgdsWiFiSarDeltaGroup3PowerChainA1;      ///< Offset 2212    WgdsWiFiSarDeltaGroup3PowerChainA1
  UINT8    WgdsWiFiSarDeltaGroup3PowerChainB1;      ///< Offset 2213    WgdsWiFiSarDeltaGroup3PowerChainB1
  UINT8    WgdsWiFiSarDeltaGroup3PowerMax2;         ///< Offset 2214    WgdsWiFiSarDeltaGroup3PowerMax2
  UINT8    WgdsWiFiSarDeltaGroup3PowerChainA2;      ///< Offset 2215    WgdsWiFiSarDeltaGroup3PowerChainA2
  UINT8    WgdsWiFiSarDeltaGroup3PowerChainB2;      ///< Offset 2216    WgdsWiFiSarDeltaGroup3PowerChainB2
  UINT8    Reserved17[41];                          ///< Offset 2217:2257
  // Reserved for Groups 4 to 9, each needs 6 bytes and total 36 bytes reserved
  UINT8    WiFiDynamicSarAntennaACurrentSet;        ///< Offset 2258    WiFiDynamicSarAntennaACurrentSet
  UINT8    WiFiDynamicSarAntennaBCurrentSet;        ///< Offset 2259    WiFiDynamicSarAntennaBCurrentSet
  UINT8    BluetoothSar;                            ///< Offset 2260    BluetoothSar
  UINT8    BluetoothSarBr;                          ///< Offset 2261    BluetoothSarBr
  UINT8    BluetoothSarEdr2;                        ///< Offset 2262    BluetoothSarEdr2
  UINT8    BluetoothSarEdr3;                        ///< Offset 2263    BluetoothSarEdr3
  UINT8    BluetoothSarLe;                          ///< Offset 2264    BluetoothSarLe
  UINT8    BluetoothSarLe2Mhz;                      ///< Offset 2265    BluetoothSarLe2Mhz
  UINT8    BluetoothSarLeLr;                        ///< Offset 2266    BluetoothSarLeLr
  UINT8    Reserved18[2];                           ///< Offset 2267:2268
  // Reserved for Bluetooth Sar future use
  UINT8    AntennaDiversity;                        ///< Offset 2269    AntennaDiversity
  UINT8    CoExistenceManager;                      ///< Offset 2270    CoExistenceManager
  UINT8    RunTimeVmControl;                        ///< Offset 2271    RunTime VM Control
  //
  //Feature Specific Data Bits
  //
  UINT8    UsbTypeCSupport;                         ///< Offset 2272    USB Type C Supported
  UINT32   HebcValue;                               ///< Offset 2273    HebcValue
  UINT8    PcdBatteryPresent;                       ///< Offset 2277    Battery Present - Bit0: Real Battery is supported on this platform. Bit1: Virtual Battery is supported on this platform.
  UINT8    PcdTsOnDimmTemperature;                  ///< Offset 2278    TS-on-DIMM temperature
  UINT8    Reserved19[2];                           ///< Offset 2279:2280
  UINT8    PcdRealBattery1Control;                  ///< Offset 2281    Real Battery 1 Control
  UINT8    PcdRealBattery2Control;                  ///< Offset 2282    Real Battery 2 Control
  UINT8    PcdMipiCamSensor;                        ///< Offset 2283    Mipi Camera Sensor
  UINT8    PcdNCT6776FCOM;                          ///< Offset 2284    NCT6776F COM
  UINT8    PcdNCT6776FSIO;                          ///< Offset 2285    NCT6776F SIO
  UINT8    PcdNCT6776FHWMON;                        ///< Offset 2286    NCT6776F HWMON
  UINT8    PcdH8S2113SIO;                           ///< Offset 2287    H8S2113 SIO
  UINT8    PcdZPoddConfig;                          ///< Offset 2288    ZPODD
  UINT8    PcdRGBCameraAdr;                         ///< Offset 2289    RGB Camera Address
  UINT8    PcdDepthCameraAdr;                       ///< Offset 2290    Depth Camera Addresy
  UINT32   PcdSmcRuntimeSciPin;                     ///< Offset 2291    SMC Runtime Sci Pin
  UINT8    PcdConvertableDockSupport;               ///< Offset 2295    Convertable Dock Support
  UINT8    PcdEcHotKeyF3Support;                    ///< Offset 2296    Ec Hotkey F3 Support
  UINT8    PcdEcHotKeyF4Support;                    ///< Offset 2297    Ec Hotkey F4 Support
  UINT8    PcdEcHotKeyF5Support;                    ///< Offset 2298    Ec Hotkey F5 Support
  UINT8    PcdEcHotKeyF6Support;                    ///< Offset 2299    Ec Hotkey F6 Support
  UINT8    PcdEcHotKeyF7Support;                    ///< Offset 2300    Ec Hotkey F7 Support
  UINT8    PcdEcHotKeyF8Support;                    ///< Offset 2301    Ec Hotkey F8 Support
  UINT8    PcdVirtualButtonVolumeUpSupport;         ///< Offset 2302    Virtual Button Volume Up Support
  UINT8    PcdVirtualButtonVolumeDownSupport;       ///< Offset 2303    Virtual Button Volume Down Support
  UINT8    PcdVirtualButtonHomeButtonSupport;       ///< Offset 2304    Virtual Button Home Button Support
  UINT8    PcdVirtualButtonRotationLockSupport;     ///< Offset 2305    Virtual Button Rotation Lock Support
  UINT8    PcdSlateModeSwitchSupport;               ///< Offset 2306    Slate Mode Switch Support
  UINT8    PcdVirtualGpioButtonSupport;             ///< Offset 2307    Virtual Button Support
  UINT8    PcdAcDcAutoSwitchSupport;                ///< Offset 2308    Ac Dc Auto Switch Support
  UINT32   PcdPmPowerButtonGpioPin;                 ///< Offset 2309    Pm Power Button Gpio Pin
  UINT8    PcdAcpiEnableAllButtonSupport;           ///< Offset 2313    Acpi Enable All Button Support
  UINT8    PcdAcpiHidDriverButtonSupport;           ///< Offset 2314    Acpi Hid Driver Button Support
  UINT32   EcLowPowerModeGpioPin;                   ///< Offset 2315    EcLowPowerModeGpioPin
  UINT32   EcSmiGpioPin;                            ///< Offset 2319    EcSmiGpioPin
  //
  // UCMC setup option, GPIO Pad
  //
  UINT8    UCMS;                                    ///< Offset 2323    Option to select UCSI/UCMC device
  UINT32   UcmcPort1Gpio;                           ///< Offset 2324    Gpio for UCMC Port 1 Interrupt
  UINT32   UcmcPort2Gpio;                           ///< Offset 2328    Gpio for UCMC Port 2 Interrupt
  UINT8    Reserved20[29];                          ///< Offset 2332:2360
  UINT8    EnablePchFivrParticipant;                ///< Offset 2361    EnablePchFivrParticipant
  UINT8    Reserved21[3];                           ///< Offset 2362:2364
  UINT8    Ufp2DfpGlobalFlag;                       ///< Offset 2365    Upstream Facing port or Downstream Facing port Global Flag from LPC EC
  UINT8    Ufp2DfpUsbPort;                          ///< Offset 2366    Upstream Facing port or Downstream Facing port number from LPC EC
  UINT8    DbcGlobalFlag;                           ///< Offset 2367    Debug Mode Global Flag from LPC EC
  UINT8    DbcUsbPort;                              ///< Offset 2368    Debug Mode USB Port Number from LPC EC
  UINT8    TotalTypeCPorts;                         ///< Offset 2369    Total Number of type C ports that are supported by platform
  UINT8    UsbTypeCPort1;                           ///< Offset 2370    Type C Connector 1  Port mapping within the controller the port exposed
  UINT8    UsbTypeCPort1Pch;                        ///< Offset 2371    Type C Connector 1  Port mapping within the PCH controller (If Split mode supported)
  UINT8    UsbCPort1Proterties;                     ///< Offset 2372    Type C Connector 1  Portperties Split Support/Controller(PCH/TBT/CPU)/Root port (vaild for TBT)
  UINT8    UsbTypeCPort2;                           ///< Offset 2373    Type C Connector 2  Port mapping within the controller the port exposed
  UINT8    UsbTypeCPort2Pch;                        ///< Offset 2374    Type C Connector 2  Port mapping within the PCH controller (If Split mode supported)
  UINT8    UsbCPort2Proterties;                     ///< Offset 2375    Type C Connector 2  Portperties Split Support/Controller(PCH/TBT/CPU)/Root port (vaild for TBT)
  UINT8    UsbTypeCPort3;                           ///< Offset 2376    Type C Connector 3  Port mapping within the controller the port exposed
  UINT8    UsbTypeCPort3Pch;                        ///< Offset 2377    Type C Connector 3  Port mapping within the PCH controller (If Split mode supported)
  UINT8    UsbCPort3Proterties;                     ///< Offset 2378    Type C Connector 3  Portperties Split Support/Controller(PCH/TBT/CPU)/Root port (vaild for TBT)
  UINT8    UsbTypeCPort4;                           ///< Offset 2379    Type C Connector 4  Port mapping within the controller the port exposed
  UINT8    UsbTypeCPort4Pch;                        ///< Offset 2380    Type C Connector 4  Port mapping within the PCH controller (If Split mode supported)
  UINT8    UsbCPort4Proterties;                     ///< Offset 2381    Type C Connector 4  Portperties Split Support/Controller(PCH/TBT/CPU)/Root port (vaild for TBT)
  UINT8    UsbTypeCPort5;                           ///< Offset 2382    Type C Connector 5  Port mapping within the controller the port exposed
  UINT8    UsbTypeCPort5Pch;                        ///< Offset 2383    Type C Connector 5  Port mapping within the PCH controller (If Split mode supported)
  UINT8    UsbCPort5Proterties;                     ///< Offset 2384    Type C Connector 5  Portperties Split Support/Controller(PCH/TBT/CPU)/Root port (vaild for TBT)
  UINT8    UsbTypeCPort6;                           ///< Offset 2385    Type C Connector 6  Port mapping within the controller the port exposed
  UINT8    UsbTypeCPort6Pch;                        ///< Offset 2386    Type C Connector 6  Port mapping within the PCH controller (If Split mode supported)
  UINT8    UsbCPort6Proterties;                     ///< Offset 2387    Type C Connector 6  Portperties Split Support/Controller(PCH/TBT/CPU)/Root port (vaild for TBT)
  UINT8    SystemTimeAndAlarmSource;                ///< Offset 2388    Select source for System time and alarm
  UINT8    WwanRtd3Option;                          ///< Offset 2389    WWAN RTD3 options
  UINT32   WwanPerstGpio;                           ///< Offset 2390    WWAN PERST Gpio pin
  UINT8    WwanPerstGpioPolarity;                   ///< Offset 2394    WWAN PERST Gpio polarity
  UINT32   PcieSlot1WakeGpio;                       ///< Offset 2395    Pcie Slot Wake Gpio pin
  UINT8    PcieSlot1RpNumber;                       ///< Offset 2399    Pcie Slot Root Port Number
  UINT8    PowermeterDeviceEnable;                  ///< Offset 2400    Enable PowerMeter
  UINT32   PcieSlot2WakeGpio;                       ///< Offset 2401    Pcie Slot 2 Wake Gpio pin
  UINT8    PcieSlot2RpNumber;                       ///< Offset 2405    Pcie Slot 2 Root Port Number
  UINT32   WwanFullCardPowerOffGpio;                ///< Offset 2406    WWAN Full card power off gpio pin
  UINT8    WwanFullCardPowerOffGpioPolarity;        ///< Offset 2410    WWAN Full card power off gpio pin polarity
  UINT32   WwanBbrstGpio;                           ///< Offset 2411    WWAN BBRST Gpio pin
  UINT8    WwanBbrstGpioPolarity;                   ///< Offset 2415    WWAN BBRST Gpio pin polarity
  UINT32   WwanWakeGpio;                            ///< Offset 2416    WWAN Wake Gpio pin
  UINT32   TouchpanelPowerEnableGpio;               ///< Offset 2420    Gpio for touchPaneL 0 power enable
  UINT32   TouchpanelPowerRstGpio;                  ///< Offset 2424    Gpio for touchPaneL 0 Reset
  UINT32   Touchpanel1IrqGpio;                      ///< Offset 2428    Gpio for touchPaneL 1 Interrupt
  UINT32   Touchpanel1PowerEnableGpio;              ///< Offset 2432    Gpio for touchPaneL 1 power enable
  UINT32   Touchpanel1PowerRstGpio;                 ///< Offset 2436    Gpio for touchPaneL 1 Reset
  UINT8    TouchpadIrqGpioPolarity;                 ///< Offset 2440    TouchPaD Interrupt Gpio pin polarity
  UINT8    TouchpanelIrqGpioPolarity;               ///< Offset 2441    TouchPaneL 0 Interrupt Gpio pin polarity
  UINT8    TouchpanelPowerEnableGpioPolarity;       ///< Offset 2442    TouchPaneL 0 power enable Gpio pin polarity
  UINT8    TouchpanelPowerRstGpioPolarity;          ///< Offset 2443    TouchPaneL 0 Reset Gpio pin polarity
  UINT8    Touchpanel1IrqGpioPolarity;              ///< Offset 2444    TouchPaneL 1 Interrupt Gpio pin polarity
  UINT8    Touchpanel1PowerEnableGpioPolarity;      ///< Offset 2445    TouchPaneL 1 power enable Gpio pin polarity
  UINT8    Touchpanel1PowerRstGpioPolarity;         ///< Offset 2446    TouchPaneL 1 Reset Gpio pin polarity
  UINT32   PcieSlot1PowerEnableGpio;                ///< Offset 2447    Pcie Slot 1 Power Enable Gpio pin
  UINT8    PcieSlot1PowerEnableGpioPolarity;        ///< Offset 2451    Pcie Slot 1 Power Enable Gpio pin polarity
  UINT32   PcieSlot1RstGpio;                        ///< Offset 2452    Pcie Slot 1 Reset Gpio pin
  UINT8    PcieSlot1RstGpioPolarity;                ///< Offset 2456    Pcie Slot 1 Reset Gpio pin polarity
  UINT32   PcieSlot2PowerEnableGpio;                ///< Offset 2457    Pcie Slot 2 Power Enable Gpio pin
  UINT8    PcieSlot2PowerEnableGpioPolarity;        ///< Offset 2461    Pcie Slot 2 Power Enable Gpio pin polarity
  UINT32   PcieSlot2RstGpio;                        ///< Offset 2462    Pcie Slot 2 Reset Gpio pin
  UINT8    PcieSlot2RstGpioPolarity;                ///< Offset 2466    Pcie Slot 2 Reset Gpio pin polarity
  UINT32   WlanWakeGpio;                            ///< Offset 2467    WLAN Wake Gpio pin
  UINT8    WlanRootPortNumber;                      ///< Offset 2471    WLAN Root Port Number
  UINT32   SataPortPowerEnableGpio;                 ///< Offset 2472    Sata port Power Enable Gpio pin
  UINT8    SataPortPowerEnableGpioPolarity;         ///< Offset 2476    Sata port Power Enable Gpio pin polarity
  UINT32   PchM2SsdPowerEnableGpio;                 ///< Offset 2477    Pch M.2 SSD Power Enable Gpio pin
  UINT8    PchM2SsdPowerEnableGpioPolarity;         ///< Offset 2481    Pch M.2 SSD Power Enable Gpio pin polarity
  UINT32   PchM2SsdRstGpio;                         ///< Offset 2482    Pch M.2 SSD Reset Gpio pin
  UINT8    PchM2SsdRstGpioPolarity;                 ///< Offset 2486    Pch M.2 SSD Reset Gpio pin polarity
  UINT32   PchM2Ssd2PowerEnableGpio;                ///< Offset 2487    Pch M.2 SSD #2 Power Enable Gpio pin
  UINT8    PchM2Ssd2PowerEnableGpioPolarity;        ///< Offset 2491    Pch M.2 SSD #2 Power Enable Gpio pin polarity
  UINT32   PchM2Ssd2RstGpio;                        ///< Offset 2492    Pch M.2 SSD #2 Reset Gpio pin
  UINT8    PchM2Ssd2RstGpioPolarity;                ///< Offset 2496    Pch M.2 SSD #2 Reset Gpio pin polarity
  UINT32   M2Ssd2PowerEnableGpio;                   ///< Offset 2497    PCIe x4 M.2 SSD Power Enable Gpio pin
  UINT8    M2Ssd2PowerEnableGpioPolarity;           ///< Offset 2501    PCIe x4 M.2 SSD Power Enable Gpio pin polarity
  UINT32   M2Ssd2RstGpio;                           ///< Offset 2502    PCIe x4 M.2 SSD Reset Gpio pin
  UINT8    M2Ssd2RstGpioPolarity;                   ///< Offset 2506    PCIe x4 M.2 SSD Reset Gpio pin polarity
  UINT8    SdevXhciInterfaceNumber1;                ///< Offset 2507    SDEV xHCI Interface Number for device 1
  UINT8    SdevXhciInterfaceNumber2;                ///< Offset 2508    SDEV xHCI Interface Number for device 2
  UINT8    SdevXhciRootPortNumber1;                 ///< Offset 2509    SDEV xHCI Root Port Number for device 1
  UINT8    SdevXhciRootPortNumber2;                 ///< Offset 2510    SDEV xHCI Root Port Number for device 2
  UINT8    TsnPcsEnabled;                           ///< Offset 2511    TSN PCS device Enable
  UINT8    WwanSourceClock;                         ///< Offset 2512    WWAN Source Clock
  UINT8    CpuWakeEnFlag;                           ///< Offset 2513    CPU_WAKE_EN value
  UINT32   CnvExtClock;                             ///< Offset 2514    CNV external 32KHz Clock
  UINT8    WwanRootPortNumber;                      ///< Offset 2518    WWAN Root Port Nmuber
  UINT8    WifiAntGainEnale;                        ///< Offset 2519    Wifi Ant Gain Enable
  UINT8    WifiAntGainChainA1;                      ///< Offset 2520    Ant Gain Table Chain A 2400
  UINT8    WifiAntGainChainA2;                      ///< Offset 2521    Ant Gain Table Chain A 5150-5350
  UINT8    WifiAntGainChainA3;                      ///< Offset 2522    Ant Gain Table Chain A 5350-5470
  UINT8    WifiAntGainChainA4;                      ///< Offset 2523    Ant Gain Table Chain A 5470-5725
  UINT8    WifiAntGainChainA5;                      ///< Offset 2524    Ant Gain Table Chain A 5725-5945
  UINT8    WifiAntGainChainB1;                      ///< Offset 2525    Ant Gain Table Chain B 2400
  UINT8    WifiAntGainChainB2;                      ///< Offset 2526    Ant Gain Table Chain B 5150-5350
  UINT8    WifiAntGainChainB3;                      ///< Offset 2527    Ant Gain Table Chain B 5350-5470
  UINT8    WifiAntGainChainB4;                      ///< Offset 2528    Ant Gain Table Chain B 5470-5725
  UINT8    WifiAntGainChainB5;                      ///< Offset 2529    Ant Gain Table Chain B 5725-5945
  UINT32   WifiActiveChannelSrd;                    ///< Offset 2530    SRD Active Channels Selection
  UINT32   WifiIndonesia5GhzSupport;                ///< Offset 2534    Indonesia 5.15-5.35 GHz Band Support Selection
  UINT32   Dg1VramSRGpio;                           ///< Offset 2538    DG1 VRAM Self Refresh Gpio pin
  UINT8    Dg1VramSRGpioPolarity;                   ///< Offset 2542    DG1 VRAM Self Refresh Gpio pin polarity
  UINT32   PegSlotAPwrEnableGpioNo;                 ///< Offset 2543    PEG slot A power enable gpio
  UINT8    PegSlotAPwrEnableGpioPolarity;           ///< Offset 2547    power enable polarity
  UINT32   PegSlotARstGpioNo;                       ///< Offset 2548    PEG slot A reset gpio
  UINT8    PegSlotARstGpioPolarity;                 ///< Offset 2552    reset polarity
  UINT32   PegSlotAWakeGpioPin;                     ///< Offset 2553    PEG slot A wake gpio
  UINT8    PegSlotARootPort;                        ///< Offset 2557    PEG slot A Root Port
  UINT32   PegSlotBPwrEnableGpioNo;                 ///< Offset 2558    PEG slot B power enable gpio
  UINT8    PegSlotBPwrEnableGpioPolarity;           ///< Offset 2562    power enable polarity
  UINT32   PegSlotBRstGpioNo;                       ///< Offset 2563    PEG slot B reset gpio
  UINT8    PegSlotBRstGpioPolarity;                 ///< Offset 2567    reset polarity
  UINT32   PegSlotBWakeGpioPin;                     ///< Offset 2568    PEG slot B wake gpio
  UINT8    PegSlotBRootPort;                        ///< Offset 2572    PEG slot B Root Port
  UINT32   PegSlotCPwrEnableGpioNo;                 ///< Offset 2573    PEG slot C power enable gpio
  UINT8    PegSlotCPwrEnableGpioPolarity;           ///< Offset 2577    power enable polarity
  UINT32   PegSlotCRstGpioNo;                       ///< Offset 2578    PEG slot C reset gpio
  UINT8    PegSlotCRstGpioPolarity;                 ///< Offset 2582    reset polarity
  UINT32   PegSlotCWakeGpioPin;                     ///< Offset 2583    PEG slot C wake gpio
  UINT8    PegSlotCRootPort;                        ///< Offset 2587    PEG slot C Root Port
  UINT32   LpmReqRegAddr;                           ///< Offset 2588    Low Power Mode required register Address
  UINT8    MipiCamCvfSupport;                       ///< Offset 2592    To enable/disable cloverfalls(CVF)
  UINT8    NCNV;                                    ///< Offset 2593    scrachpad WA
  UINT32   Dg1OpRegionAddress;                      ///< Offset 2594    Dg1 OpRegion base address
  UINT8    WrdsWiFiSarTxPowerSet1Limit11;           ///< Offset 2598    WrdsWiFiSarTxPowerSet1Limit11
  UINT8    WrdsWiFiSarTxPowerSet1Limit12;           ///< Offset 2599    WrdsWiFiSarTxPowerSet1Limit12
  UINT8    WrdsWiFiSarTxPowerSet1Limit13;           ///< Offset 2600    WrdsWiFiSarTxPowerSet1Limit13
  UINT8    WrdsWiFiSarTxPowerSet1Limit14;           ///< Offset 2601    WrdsWiFiSarTxPowerSet1Limit14
  UINT8    WrdsWiFiSarTxPowerSet1Limit15;           ///< Offset 2602    WrdsWiFiSarTxPowerSet1Limit15
  UINT8    WrdsWiFiSarTxPowerSet1Limit16;           ///< Offset 2603    WrdsWiFiSarTxPowerSet1Limit16
  UINT8    WrdsWiFiSarTxPowerSet1Limit17;           ///< Offset 2604    WrdsWiFiSarTxPowerSet1Limit17
  UINT8    WrdsWiFiSarTxPowerSet1Limit18;           ///< Offset 2605    WrdsWiFiSarTxPowerSet1Limit18
  UINT8    WrdsWiFiSarTxPowerSet1Limit19;           ///< Offset 2606    WrdsWiFiSarTxPowerSet1Limit19
  UINT8    WrdsWiFiSarTxPowerSet1Limit20;           ///< Offset 2607    WrdsWiFiSarTxPowerSet1Limit20
  UINT8    WrdsWiFiSarTxPowerSet1Limit21;           ///< Offset 2608    WrdsWiFiSarTxPowerSet1Limit21
  UINT8    WrdsWiFiSarTxPowerSet1Limit22;           ///< Offset 2609    WrdsWiFiSarTxPowerSet1Limit22
  UINT8    EwrdWiFiSarTxPowerSet2Limit11;           ///< Offset 2610    EwrdWiFiSarTxPowerSet2Limit11
  UINT8    EwrdWiFiSarTxPowerSet2Limit12;           ///< Offset 2611    EwrdWiFiSarTxPowerSet2Limit12
  UINT8    EwrdWiFiSarTxPowerSet2Limit13;           ///< Offset 2612    EwrdWiFiSarTxPowerSet2Limit13
  UINT8    EwrdWiFiSarTxPowerSet2Limit14;           ///< Offset 2613    EwrdWiFiSarTxPowerSet2Limit14
  UINT8    EwrdWiFiSarTxPowerSet2Limit15;           ///< Offset 2614    EwrdWiFiSarTxPowerSet2Limit15
  UINT8    EwrdWiFiSarTxPowerSet2Limit16;           ///< Offset 2615    EwrdWiFiSarTxPowerSet2Limit16
  UINT8    EwrdWiFiSarTxPowerSet2Limit17;           ///< Offset 2616    EwrdWiFiSarTxPowerSet2Limit17
  UINT8    EwrdWiFiSarTxPowerSet2Limit18;           ///< Offset 2617    EwrdWiFiSarTxPowerSet2Limit18
  UINT8    EwrdWiFiSarTxPowerSet2Limit19;           ///< Offset 2618    EwrdWiFiSarTxPowerSet2Limit19
  UINT8    EwrdWiFiSarTxPowerSet2Limit20;           ///< Offset 2619    EwrdWiFiSarTxPowerSet2Limit20
  UINT8    EwrdWiFiSarTxPowerSet2Limit21;           ///< Offset 2620    EwrdWiFiSarTxPowerSet2Limit21
  UINT8    EwrdWiFiSarTxPowerSet2Limit22;           ///< Offset 2621    EwrdWiFiSarTxPowerSet2Limit22
  UINT8    EwrdWiFiSarTxPowerSet3Limit11;           ///< Offset 2622    EwrdWiFiSarTxPowerSet3Limit11
  UINT8    EwrdWiFiSarTxPowerSet3Limit12;           ///< Offset 2623    EwrdWiFiSarTxPowerSet3Limit12
  UINT8    EwrdWiFiSarTxPowerSet3Limit13;           ///< Offset 2624    EwrdWiFiSarTxPowerSet3Limit13
  UINT8    EwrdWiFiSarTxPowerSet3Limit14;           ///< Offset 2625    EwrdWiFiSarTxPowerSet3Limit14
  UINT8    EwrdWiFiSarTxPowerSet3Limit15;           ///< Offset 2626    EwrdWiFiSarTxPowerSet3Limit15
  UINT8    EwrdWiFiSarTxPowerSet3Limit16;           ///< Offset 2627    EwrdWiFiSarTxPowerSet3Limit16
  UINT8    EwrdWiFiSarTxPowerSet3Limit17;           ///< Offset 2628    EwrdWiFiSarTxPowerSet3Limit17
  UINT8    EwrdWiFiSarTxPowerSet3Limit18;           ///< Offset 2629    EwrdWiFiSarTxPowerSet3Limit18
  UINT8    EwrdWiFiSarTxPowerSet3Limit19;           ///< Offset 2630    EwrdWiFiSarTxPowerSet3Limit19
  UINT8    EwrdWiFiSarTxPowerSet3Limit20;           ///< Offset 2631    EwrdWiFiSarTxPowerSet3Limit20
  UINT8    EwrdWiFiSarTxPowerSet3Limit21;           ///< Offset 2632    EwrdWiFiSarTxPowerSet3Limit21
  UINT8    EwrdWiFiSarTxPowerSet3Limit22;           ///< Offset 2633    EwrdWiFiSarTxPowerSet3Limit22
  UINT8    EwrdWiFiSarTxPowerSet4Limit11;           ///< Offset 2634    EwrdWiFiSarTxPowerSet4Limit11
  UINT8    EwrdWiFiSarTxPowerSet4Limit12;           ///< Offset 2635    EwrdWiFiSarTxPowerSet4Limit12
  UINT8    EwrdWiFiSarTxPowerSet4Limit13;           ///< Offset 2636    EwrdWiFiSarTxPowerSet4Limit13
  UINT8    EwrdWiFiSarTxPowerSet4Limit14;           ///< Offset 2637    EwrdWiFiSarTxPowerSet4Limit14
  UINT8    EwrdWiFiSarTxPowerSet4Limit15;           ///< Offset 2638    EwrdWiFiSarTxPowerSet4Limit15
  UINT8    EwrdWiFiSarTxPowerSet4Limit16;           ///< Offset 2639    EwrdWiFiSarTxPowerSet4Limit16
  UINT8    EwrdWiFiSarTxPowerSet4Limit17;           ///< Offset 2640    EwrdWiFiSarTxPowerSet4Limit17
  UINT8    EwrdWiFiSarTxPowerSet4Limit18;           ///< Offset 2641    EwrdWiFiSarTxPowerSet4Limit18
  UINT8    EwrdWiFiSarTxPowerSet4Limit19;           ///< Offset 2642    EwrdWiFiSarTxPowerSet4Limit19
  UINT8    EwrdWiFiSarTxPowerSet4Limit20;           ///< Offset 2643    EwrdWiFiSarTxPowerSet4Limit20
  UINT8    EwrdWiFiSarTxPowerSet4Limit21;           ///< Offset 2644    EwrdWiFiSarTxPowerSet4Limit21
  UINT8    EwrdWiFiSarTxPowerSet4Limit22;           ///< Offset 2645    EwrdWiFiSarTxPowerSet4Limit22
  UINT8    WgdsWiFiSarDeltaGroup1PowerMax3;         ///< Offset 2646    WgdsWiFiSarDeltaGroup1PowerMax3
  UINT8    WgdsWiFiSarDeltaGroup1PowerChainA3;      ///< Offset 2647    WgdsWiFiSarDeltaGroup1PowerChainA3
  UINT8    WgdsWiFiSarDeltaGroup1PowerChainB3;      ///< Offset 2648    WgdsWiFiSarDeltaGroup1PowerChainB3
  UINT8    WgdsWiFiSarDeltaGroup2PowerMax3;         ///< Offset 2649    WgdsWiFiSarDeltaGroup2PowerMax3
  UINT8    WgdsWiFiSarDeltaGroup2PowerChainA3;      ///< Offset 2650    WgdsWiFiSarDeltaGroup2PowerChainA3
  UINT8    WgdsWiFiSarDeltaGroup2PowerChainB3;      ///< Offset 2651    WgdsWiFiSarDeltaGroup2PowerChainB3
  UINT8    WgdsWiFiSarDeltaGroup3PowerMax3;         ///< Offset 2652    WgdsWiFiSarDeltaGroup3PowerMax3
  UINT8    WgdsWiFiSarDeltaGroup3PowerChainA3;      ///< Offset 2653    WgdsWiFiSarDeltaGroup3PowerChainA3
  UINT8    WgdsWiFiSarDeltaGroup3PowerChainB3;      ///< Offset 2654    WgdsWiFiSarDeltaGroup3PowerChainB3
  UINT8    WifiAntGainChainA6;                      ///< Offset 2655    Ant Gain Table Chain A 5945-6165
  UINT8    WifiAntGainChainA7;                      ///< Offset 2656    Ant Gain Table Chain A 6165-6405
  UINT8    WifiAntGainChainA8;                      ///< Offset 2657    Ant Gain Table Chain A 6405-6525
  UINT8    WifiAntGainChainA9;                      ///< Offset 2658    Ant Gain Table Chain A 6525-6705
  UINT8    WifiAntGainChainA10;                     ///< Offset 2659    Ant Gain Table Chain A 6705-6865
  UINT8    WifiAntGainChainA11;                     ///< Offset 2660    Ant Gain Table Chain A 6865-7105
  UINT8    WifiAntGainChainB6;                      ///< Offset 2661    Ant Gain Table Chain B 5945-6165
  UINT8    WifiAntGainChainB7;                      ///< Offset 2662    Ant Gain Table Chain B 6165-6405
  UINT8    WifiAntGainChainB8;                      ///< Offset 2663    Ant Gain Table Chain B 6405-6525
  UINT8    WifiAntGainChainB9;                      ///< Offset 2664    Ant Gain Table Chain B 6525-6705
  UINT8    WifiAntGainChainB10;                     ///< Offset 2665    Ant Gain Table Chain B 6705-6865
  UINT8    WifiAntGainChainB11;                     ///< Offset 2666    Ant Gain Table Chain B 6865-7105
  UINT32   WifiUltraHighBandSupport;                ///< Offset 2667    WifiUltraHighBandSupport
  UINT8    DeepestUSBSleepWakeCapability;           ///< Offset 2671    Deepest USB Sleep Wake Capability
  UINT64   Dg1BaseAddress;                          ///< Offset 2672    Dg1 PCIe base address
  UINT8    MipiCamLink0DD_LanesClkDivision;         ///< Offset 2680    LanesClockDivision
  UINT8    MipiCamLink1DD_LanesClkDivision;         ///< Offset 2681    LanesClockDivision
  UINT8    MipiCamLink2DD_LanesClkDivision;         ///< Offset 2682    LanesClockDivision
  UINT8    MipiCamLink3DD_LanesClkDivision;         ///< Offset 2683    LanesClockDivision
  UINT8    MipiCamLink4DD_LanesClkDivision;         ///< Offset 2684    LanesClockDivision
  UINT8    MipiCamLink5DD_LanesClkDivision;         ///< Offset 2685    LanesClockDivision
  UINT32   I2c0SpeedMode;                           ///< Offset 2686    I2C0 Speed Mode Selection
  UINT32   I2c1SpeedMode;                           ///< Offset 2690    I2C1 Speed Mode Selection
  UINT32   I2c2SpeedMode;                           ///< Offset 2694    I2C2 Speed Mode Selection
  UINT32   I2c3SpeedMode;                           ///< Offset 2698    I2C3 Speed Mode Selection
  UINT32   I2c4SpeedMode;                           ///< Offset 2702    I2C4 Speed Mode Selection
  UINT32   I2c5SpeedMode;                           ///< Offset 2706    I2C5 Speed Mode Selection
  //
  // XTU SMI memory address
  //
  UINT32   XtuSmiMemoryAddress;                     ///< Offset 2710    XTU SMI memory in ACPI NVS
  UINT8    WwanEnable;                              ///< Offset 2714    WWAN Enable - 0:Disabled, 1:7360/7560, 2:M80
  UINT16   WwanTOn2ResDelayMs;                      ///< Offset 2715    FCPO# to BBRST# delay time during WWAN ON
  UINT16   WwanTOnRes2PerDelayMs;                   ///< Offset 2717    BBRST# to PERST# delay time during WWAN ON
  UINT16   WwanTOnPer2PdsDelayMs;                   ///< Offset 2719    The delay between de-assertion of PERST# to change of PDS state from 0 to 1 during WWAN ON
  UINT16   WwanTRes2OffDelayMs;                     ///< Offset 2721    BBRST# to FCPO# delay time during WWAN OFF
  UINT16   WwanTOffDisDelayMs;                      ///< Offset 2723    Rigorous shutdown time
  UINT16   WwanTResTogDelayMs;                      ///< Offset 2725    The delay between assertion and de-assertion BBRST# during FLDR
  UINT16   WwanTRes2PdsDelayMs;                     ///< Offset 2727    The delay between de-assertion of BBRST# and change of PDS state from 0 to 1 after FLDR
  UINT64   Dg2BaseAddress;                          ///< Offset 2729    DG2 PCIe base address
  UINT32   Dg2OpRegionAddress;                      ///< Offset 2737    DG2 OpRegion base address

} PLATFORM_NVS_AREA;

#pragma pack(pop)
#endif
