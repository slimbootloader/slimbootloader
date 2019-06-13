/** @file

  Copyright (c) 2017 - 2019, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

  // Define a Global region of ACPI NVS Region that may be used for any
  // type of implementation.  The starting offset and size will be fixed
  // up by the System BIOS during POST.  Note that the Size must be a word
  // in size to be fixed up correctly.


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
  UINT8    WPCN381U;                                ///< Offset 41      WPCN381U SIO Present
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
  UINT8    ActiveThermalTripPointSA;                ///< Offset 79      Active Trip Point for MCH
  UINT8    PassiveThermalTripPointSA;               ///< Offset 80      Passive Trip Point for MCH
  UINT32   PlatformCpuId;                           ///< Offset 81      CPUID Feature Information [EAX]
  UINT32   TBARB;                                   ///< Offset 85      Reserved for Thermal Base Low Address for BIOS
  UINT32   TBARBH;                                  ///< Offset 89      Reserved for Thermal Base High Address for BIOS
  UINT8    TsOnDimmEnabled;                         ///< Offset 93      TS-on-DIMM is chosen in SETUP and present on the DIMM
  //
  // Board info
  //
  UINT8    PlatformFlavor;                          ///< Offset 94      Platform Flavor
  UINT8    BoardRev;                                ///< Offset 95      Board Rev
  //
  // Package temperature
  //
  UINT8    PackageDTSTemperature;                   ///< Offset 96      Package Temperature
  UINT8    IsPackageTempMSRAvailable;               ///< Offset 97      Package Temperature MSR available
  UINT8    PeciAccessMethod;                        ///< Offset 98      Peci Access Method
  UINT8    Ac0FanSpeed;                             ///< Offset 99      _AC0 Fan Speed
  UINT8    Ac1FanSpeed;                             ///< Offset 100     _AC1 Fan Speed
  UINT8    Ap2DigitalThermalSensorTemperature;      ///< Offset 101     Digital Thermal Sensor 3 Reading
  UINT8    Ap3DigitalThermalSensorTemperature;      ///< Offset 102     Digital Thermal Sensor 4 Reading
  //
  // XTU 3.0 Specification
  //
  UINT32   XTUBaseAddress;                          ///< Offset 103     XTU Continous structure Base Address
  UINT32   XTUSize;                                 ///< Offset 107     XMP Size
  UINT32   XMPBaseAddress;                          ///< Offset 111     XMP Base Address
  UINT8    DDRReferenceFreq;                        ///< Offset 115     DDR Reference Frequency
  UINT8    Rtd3Support;                             ///< Offset 116     Runtime D3 support.
  UINT8    Rtd3P0dl;                                ///< Offset 117     User selctable Delay for Device D0 transition.
  //
  // DPTF Devices and trip points
  //
  UINT8    EnableDptf;                              ///< Offset 118     EnableDptf
  UINT16   EnableDCFG;                              ///< Offset 119     EnableDCFG
  UINT8    EnableSaDevice;                          ///< Offset 121     EnableSaDevice
  UINT8    CriticalThermalTripPointSA;              ///< Offset 122     CriticalThermalTripPointSa
  UINT8    HotThermalTripPointSA;                   ///< Offset 123     HotThermalTripPointSa
  UINT8    ThermalSamplingPeriodSA;                 ///< Offset 124     ThermalSamplingPeriodSA
  //
  // DPTF Policies
  //
  UINT8    EnableCtdpPolicy;                        ///< Offset 125     EnableCtdpPolicy
  //
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
  UINT8    EnableActivePolicy;                      ///< Offset 147     EnableActivePolicy
  UINT8    EnablePassivePolicy;                     ///< Offset 148     EnablePassivePolicy
  UINT8    EnableCriticalPolicy;                    ///< Offset 149     EnableCriticalPolicy
  UINT8    EnablePIDPolicy;                         ///< Offset 150     EnablePIDPolicy
  UINT8    TrtRevision;                             ///< Offset 151     TrtRevision
  //
  // CLPO (Current Logical Processor Off lining Setting)
  //
  UINT8    LPOEnable;                               ///< Offset 152     LPOEnable
  UINT8    LPOStartPState;                          ///< Offset 153     LPOStartPState
  UINT8    LPOStepSize;                             ///< Offset 154     LPOStepSize
  UINT8    LPOPowerControlSetting;                  ///< Offset 155     LPOPowerControlSetting
  UINT8    LPOPerformanceControlSetting;            ///< Offset 156     LPOPerformanceControlSetting
  //
  // Miscellaneous DPTF
  //
  UINT32   PpccStepSize;                            ///< Offset 157     PPCC Step Size
  UINT8    EnableDisplayParticipant;                ///< Offset 161     EnableDisplayParticipant
  //
  // BIOS Guard
  //
  UINT64   BiosGuardMemAddress;                     ///< Offset 162     BIOS Guard Memory Address for Tool Interface
  UINT8    BiosGuardMemSize;                        ///< Offset 170     BIOS Guard Memory Size for Tool Interface
  UINT16   BiosGuardIoTrapAddress;                  ///< Offset 171     BIOS Guard IoTrap Address for Tool Interface
  //
  // Never Sleep Technology
  //
  UINT8    IrmtCfg;                                 ///< Offset 173     Irmt Configuration
  //
  // Comms Hub
  //
  UINT8    CommsHubEnable;                          ///< Offset 174     Comms Hub Enable/Disable
  UINT8    LowPowerS0Idle;                          ///< Offset 175     Low Power S0 Idle Enable
  //
  // BIOS only version of Config TDP
  //
  UINT8    ConfigTdpBios;                           ///< Offset 176     enable/disable BIOS only version of Config TDP
  UINT8    DockSmi;                                 ///< Offset 177     Dock SMI number
  //
  // LPC SIO configuration
  //
  UINT8    Reserved0[322];                          ///< Offset 178:499
  UINT8    EnableWrlsParticipant;                   ///< Offset 500     EnableWrlsParticipant
  UINT8    ActiveThermalTripPointWrls;              ///< Offset 501     ActiveThermalTripPointWrls
  UINT8    PassiveThermalTripPointWrls;             ///< Offset 502     PassiveThermalTripPointWrls
  UINT8    CriticalThermalTripPointWrls;            ///< Offset 503     CriticalThermalTripPointWrls
  UINT8    HotThermalTripPointWrls;                 ///< Offset 504     HotThermalTripPointWrls
  UINT8    EnablePowerParticipant;                  ///< Offset 505     EnablePowerParticipant
  UINT16   DPTFRsvd0;                               ///< Offset 506     DPTFRsvd0
  UINT16   PowerParticipantPollingRate;             ///< Offset 508     PowerParticipantPollingRate
  UINT8    EnablePowerBossPolicy;                   ///< Offset 510     EnablePowerBossPolicy
  UINT8    EnableVSPolicy;                          ///< Offset 511     EnableVSPolicy
  UINT8    EnableRFIMPolicy;                        ///< Offset 512     RFI Mitigation
  UINT8    Reserved1[2];                            ///< Offset 513:514
  UINT8    UsbPowerResourceTest;                    ///< Offset 515     RTD3 USB Power Resource config
  UINT8    VirtualGpioButtonSxBitmask;              ///< Offset 516     Virtual GPIO button Notify Sleep State Change
  UINT8    IuerButtonEnable;                        ///< Offset 517     IUER Button Enable
  UINT8    IuerConvertibleEnable;                   ///< Offset 518     IUER Convertible Enable
  UINT8    IuerDockEnable;                          ///< Offset 519     IUER Dock Enable
  UINT8    CSNotifyEC;                              ///< Offset 520     EC Notification of Low Power S0 Idle State
  UINT16   Rtd3AudioDelay;                          ///< Offset 521     RTD3 Audio Codec device delay
  UINT16   Rtd3SensorHub;                           ///< Offset 523     RTD3 SensorHub delay time after applying power to device
  UINT16   Rtd3TouchPanelDelay;                     ///< Offset 525     RTD3 TouchPanel delay time after applying power to device
  UINT16   Rtd3TouchPadDelay;                       ///< Offset 527     RTD3 TouchPad delay time after applying power to device
  UINT16   VRRampUpDelay;                           ///< Offset 529     VR Ramp up delay
  UINT8    PstateCapping;                           ///< Offset 531     P-state Capping
  UINT16   Rtd3Config0;                             ///< Offset 532     RTD3 Config Setting0(BIT0:ZPODD, BIT1:Reserved, BIT2:PCIe NVMe, Bit4:SKL SDS SIP I2C Touch, BIT6:Card Reader, BIT7:WWAN)
  UINT16   Rtd3Config1;                             ///< Offset 534     RTD3 Config Setting1(BIT0:Sata Port0, BIT1:Sata Port1, BIT2:Sata Port2, BIT3:Sata Port3)
  UINT8    CSDebugLightEC;                          ///< Offset 536     EC Debug Light (CAPS LOCK) for when in Low Power S0 Idle State
  UINT8    Ps2MouseEnable;                          ///< Offset 537     Ps2 Mouse Enable
  UINT8    Ps2KbMsEnable;                           ///< Offset 538     Ps2 Keyboard and Mouse Enable
  UINT8    DiscreteWifiRtd3ColdSupport;             ///< Offset 539     Enable RTD3 Cold Support for Wifi
  UINT8    DiscreteWigigRtd3ColdSupport;            ///< Offset 540     Enable RTD3 Cold Support for Wigig  @deprecated since revision 6
  UINT8    DiscreteWwanRtd3ColdSupport;             ///< Offset 541     Enable RTD3 Cold Support for WWAN
  UINT16   SSH0;                                    ///< Offset 542     SSCN-HIGH for I2C0
  UINT16   SSL0;                                    ///< Offset 544     SSCN-LOW  for I2C0
  UINT16   SSD0;                                    ///< Offset 546     SSCN-HOLD for I2C0
  UINT16   FMH0;                                    ///< Offset 548     FMCN-HIGH for I2C0
  UINT16   FML0;                                    ///< Offset 550     FMCN-LOW  for I2C0
  UINT16   FMD0;                                    ///< Offset 552     FMCN-HOLD for I2C0
  UINT16   FPH0;                                    ///< Offset 554     FPCN-HIGH for I2C0
  UINT16   FPL0;                                    ///< Offset 556     FPCN-LOW  for I2C0
  UINT16   FPD0;                                    ///< Offset 558     FPCN-HOLD for I2C0
  UINT16   HMH0;                                    ///< Offset 560     HMCN-HIGH for I2C0
  UINT16   HML0;                                    ///< Offset 562     HMCN-LOW  for I2C0
  UINT16   HMD0;                                    ///< Offset 564     HMCN-HOLD for I2C0
  UINT8    Reserved2[7];                            ///< Offset 566:572
  UINT16   SSH1;                                    ///< Offset 573     SSCN-HIGH for I2C1
  UINT16   SSL1;                                    ///< Offset 575     SSCN-LOW  for I2C1
  UINT16   SSD1;                                    ///< Offset 577     SSCN-HOLD for I2C1
  UINT16   FMH1;                                    ///< Offset 579     FMCN-HIGH for I2C1
  UINT16   FML1;                                    ///< Offset 581     FMCN-LOW  for I2C1
  UINT16   FMD1;                                    ///< Offset 583     FMCN-HOLD for I2C1
  UINT16   FPH1;                                    ///< Offset 585     FPCN-HIGH for I2C1
  UINT16   FPL1;                                    ///< Offset 587     FPCN-LOW  for I2C1
  UINT16   FPD1;                                    ///< Offset 589     FPCN-HOLD for I2C1
  UINT16   HMH1;                                    ///< Offset 591     HMCN-HIGH for I2C1
  UINT16   HML1;                                    ///< Offset 593     HMCN-LOW  for I2C1
  UINT16   HMD1;                                    ///< Offset 595     HMCN-HOLD for I2C1
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
  UINT16   HMH2;                                    ///< Offset 616     HMCN-HIGH for I2C2
  UINT16   HML2;                                    ///< Offset 618     HMCN-LOW  for I2C2
  UINT16   HMD2;                                    ///< Offset 620     HMCN-HOLD for I2C2
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
  UINT16   HMH3;                                    ///< Offset 641     HMCN-HIGH for I2C3
  UINT16   HML3;                                    ///< Offset 643     HMCN-LOW  for I2C3
  UINT16   HMD3;                                    ///< Offset 645     HMCN-HOLD for I2C3
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
  UINT16   HMH4;                                    ///< Offset 666     HMCN-HIGH for I2C4
  UINT16   HML4;                                    ///< Offset 668     HMCN-LOW  for I2C4
  UINT16   HMD4;                                    ///< Offset 670     HMCN-HOLD for I2C4
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
  UINT16   HMH5;                                    ///< Offset 691     HMCN-HIGH for I2C5
  UINT16   HML5;                                    ///< Offset 693     HMCN-LOW  for I2C5
  UINT16   HMD5;                                    ///< Offset 695     HMCN-HOLD for I2C5
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
  UINT32   LowPowerS0IdleConstraint;                ///< Offset 778     PEP Constraints
  // Bit[1:0] - Storage (0:None, 1:Storage Controller, 2:Raid)
  // Bit[2]   - En/Dis UART0
  // Bit[3]   - En/Dis UART1
  // Bit[4]   - Unused
  // Bit[5]   - En/Dis I2C0
  // Bit[6]   - En/Dis I2C1
  // Bit[7]   - En/Dis XHCI
  // Bit[8]   - En/Dis HD Audio (includes ADSP)
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
  // Bit[20]  - [CNL] En/Dis SPI2
  // Bit[21]  - En/Dis IPU0
  // Bit[22]  - En/Dis CSME
  // Bit[23]  - En/Dis LAN(GBE)
  // Bit[24]  - En/Dis PEG0
  // Bit[25]  - En/Dis TBT RP
  UINT8    TenSecondPowerButtonEnable;              ///< Offset 782     10sec Power button support
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
  UINT8    GenerationId;                            ///< Offset 783     Generation Id(0=Shark bay, 1=Crescent Bay)
  //
  // DPTF
  //
  UINT8    EnableWWANParticipant;                   ///< Offset 784     EnableWWANParticipant
  UINT8    ActiveThermalTripPointWWAN;              ///< Offset 785     ActiveThermalTripPointWWAN
  UINT8    PassiveThermalTripPointWWAN;             ///< Offset 786     PassiveThermalTripPointWWAN
  UINT8    CriticalThermalTripPointWWAN;            ///< Offset 787     CriticalThermalTripPointWWAN
  UINT8    HotThermalTripPointWWAN;                 ///< Offset 788     HotThermalTripPointWWAN
  UINT8    Reserved11[18];                          ///< Offset 789:806
  UINT16   MinPowerLimit0;                          ///< Offset 807     Minimum Power Limit 0 for DPTF use via PPCC Object
  UINT8    EnableChargerParticipant;                ///< Offset 809     EnableChargerParticipant
  UINT8    CriticalThermalTripPointSaS3;            ///< Offset 810     CriticalThermalTripPointSaS3
  UINT8    CriticalThermalTripPointAmbientS3;       ///< Offset 811     CriticalThermalTripPointAmbientS3
  UINT8    CriticalThermalTripPointSkinS3;          ///< Offset 812     CriticalThermalTripPointSkinS3
  UINT8    CriticalThermalTripPointExhaustFanS3;    ///< Offset 813     CriticalThermalTripPointExhaustFanS3
  UINT8    CriticalThermalTripPointVrS3;            ///< Offset 814     CriticalThermalTripPointVRS3
  UINT8    CriticalThermalTripPointWrlsS3;          ///< Offset 815     CriticalThermalTripPointWrlsS3
  UINT8    CriticalThermalTripPointInletFanS3;      ///< Offset 816     CriticalThermalTripPointInletFanS3
  UINT8    CriticalThermalTripPointWwanS3;          ///< Offset 817     CriticalThermalTripPointWWANS3
  UINT8    CriticalThermalTripPointWGigS3;          ///< Offset 818     CriticalThermalTripPointWGigS3
  UINT8    SataPortState;                           ///< Offset 819     SATA port state @deprecated since revision 3
  //
  // DPTF
  //
  UINT8    Enable2DCameraParticipant;               ///< Offset 820     Enable2DCameraParticipant
  UINT8    EcLowPowerMode;                          ///< Offset 821     EC Low Power Mode: 1 - Enabled, 0 - Disabled
  UINT8    SensorSamplingPeriodSen1;                ///< Offset 822     SensorSamplingPeriodSen1
  UINT8    SensorSamplingPeriodSen2;                ///< Offset 823     SensorSamplingPeriodSen2
  UINT8    SensorSamplingPeriodSen3;                ///< Offset 824     SensorSamplingPeriodSen3
  UINT8    SensorSamplingPeriodSen4;                ///< Offset 825     SensorSamplingPeriodSen4
  UINT8    SensorSamplingPeriodSen5;                ///< Offset 826     SensorSamplingPeriodSen5
  UINT8    EnableStorageParticipantST1;             ///< Offset 827     EnableStorageParticipantST1
  UINT8    ActiveThermalTripPointST1;               ///< Offset 828     ActiveThermalTripPointST1
  UINT8    PassiveThermalTripPointST1;              ///< Offset 829     PassiveThermalTripPointST1
  UINT8    CriticalThermalTripPointST1;             ///< Offset 830     CriticalThermalTripPointST1
  UINT8    CriticalThermalTripPointS3ST1;           ///< Offset 831     CriticalThermalTripPointS3ST1
  UINT8    HotThermalTripPointST1;                  ///< Offset 832     HotThermalTripPointST1
  UINT8    EnableStorageParticipantST2;             ///< Offset 833     EnableStorageParticipantST2
  UINT8    ActiveThermalTripPointST2;               ///< Offset 834     ActiveThermalTripPointST2
  UINT8    PassiveThermalTripPointST2;              ///< Offset 835     PassiveThermalTripPointST2
  UINT8    CriticalThermalTripPointST2;             ///< Offset 836     CriticalThermalTripPointST2
  UINT8    CriticalThermalTripPointS3ST2;           ///< Offset 837     CriticalThermalTripPointS3ST2
  UINT8    HotThermalTripPointST2;                  ///< Offset 838     HotThermalTripPointST2
  UINT8    EnableVS1Participant;                    ///< Offset 839     EnableVS1Participant
  UINT8    ActiveThermalTripPointVS1;               ///< Offset 840     ActiveThermalTripPointVS1
  UINT8    PassiveThermalTripPointVS1;              ///< Offset 841     PassiveThermalTripPointVS1
  UINT8    CriticalThermalTripPointVS1;             ///< Offset 842     CriticalThermalTripPointVS1
  UINT8    CriticalThermalTripPointVS1S3;           ///< Offset 843     CriticalThermalTripPointVS1S3
  UINT8    HotThermalTripPointVS1;                  ///< Offset 844     HotThermalTripPointVS1
  UINT8    EnableVS2Participant;                    ///< Offset 845     EnableVS2Participant
  UINT8    ActiveThermalTripPointVS2;               ///< Offset 846     ActiveThermalTripPointVS2
  UINT8    PassiveThermalTripPointVS2;              ///< Offset 847     PassiveThermalTripPointVS2
  UINT8    CriticalThermalTripPointVS2;             ///< Offset 848     CriticalThermalTripPointVS2
  UINT8    CriticalThermalTripPointVS2S3;           ///< Offset 849     CriticalThermalTripPointVS2S3
  UINT8    HotThermalTripPointVS2;                  ///< Offset 850     HotThermalTripPointVS2
  UINT8    EnableSen1Participant;                   ///< Offset 851     EnableSen1Participant
  UINT8    ActiveThermalTripPointSen1;              ///< Offset 852     ActiveThermalTripPointSen1
  UINT8    PassiveThermalTripPointSen1;             ///< Offset 853     PassiveThermalTripPointSen1
  UINT8    CriticalThermalTripPointSen1;            ///< Offset 854     CriticalThermalTripPointSen1
  UINT8    HotThermalTripPointSen1;                 ///< Offset 855     HotThermalTripPointSen1
  UINT8    EnableSen2Participant;                   ///< Offset 856     EnableSen2Participant
  UINT8    ActiveThermalTripPointSen2;              ///< Offset 857     ActiveThermalTripPointSen2
  UINT8    PassiveThermalTripPointSen2;             ///< Offset 858     PassiveThermalTripPointSen2
  UINT8    CriticalThermalTripPointSen2;            ///< Offset 859     CriticalThermalTripPointSen2
  UINT8    HotThermalTripPointSen2;                 ///< Offset 860     HotThermalTripPointSen2
  UINT8    EnableSen3Participant;                   ///< Offset 861     EnableSen3Participant
  UINT8    ActiveThermalTripPointSen3;              ///< Offset 862     ActiveThermalTripPointSen3
  UINT8    PassiveThermalTripPointSen3;             ///< Offset 863     PassiveThermalTripPointSen3
  UINT8    CriticalThermalTripPointSen3;            ///< Offset 864     CriticalThermalTripPointSen3
  UINT8    HotThermalTripPointSen3;                 ///< Offset 865     HotThermalTripPointSen3
  UINT8    EnableSen4Participant;                   ///< Offset 866     EnableSen4Participant
  UINT8    ActiveThermalTripPointSen4;              ///< Offset 867     ActiveThermalTripPointSen4
  UINT8    PassiveThermalTripPointSen4;             ///< Offset 868     PassiveThermalTripPointSen4
  UINT8    CriticalThermalTripPointSen4;            ///< Offset 869     CriticalThermalTripPointSen4
  UINT8    HotThermalTripPointSen4;                 ///< Offset 870     HotThermalTripPointSen4
  UINT8    EnableSen5Participant;                   ///< Offset 871     EnableSen5Participant
  UINT8    ActiveThermalTripPointSen5;              ///< Offset 872     ActiveThermalTripPointSen5
  UINT8    PassiveThermalTripPointSen5;             ///< Offset 873     PassiveThermalTripPointSen5
  UINT8    CriticalThermalTripPointSen5;            ///< Offset 874     CriticalThermalTripPointSen5
  UINT8    HotThermalTripPointSen5;                 ///< Offset 875     HotThermalTripPointSen5
  UINT8    CriticalThermalTripPointSen1S3;          ///< Offset 876     CriticalThermalTripPointSen1S3
  UINT8    CriticalThermalTripPointSen2S3;          ///< Offset 877     CriticalThermalTripPointSen2S3
  UINT8    CriticalThermalTripPointSen3S3;          ///< Offset 878     CriticalThermalTripPointSen3S3
  UINT8    CriticalThermalTripPointSen4S3;          ///< Offset 879     CriticalThermalTripPointSen4S3
  UINT8    CriticalThermalTripPointSen5S3;          ///< Offset 880     CriticalThermalTripPointSen5S3
  UINT8    EnableBatteryParticipant;                ///< Offset 881     EnableBatteryParticipant
  UINT8    Reserved12[6];                           ///< Offset 882:887
  UINT8    PowerSharingManagerEnable;               ///< Offset 888     PowerSharingManagerEnable
  UINT8    PsmSplcDomainType1;                      ///< Offset 889     PsmSplcDomainType1
  UINT32   PsmSplcPowerLimit1;                      ///< Offset 890     PsmSplcPowerLimit1
  UINT32   PsmSplcTimeWindow1;                      ///< Offset 894     PsmSplcTimeWindow1
  UINT8    PsmSplcDomainType2;                      ///< Offset 898     PsmSplcDomainType2  @deprecated since revision 6
  UINT32   PsmSplcPowerLimit2;                      ///< Offset 899     PsmSplcPowerLimit2  @deprecated since revision 6
  UINT32   PsmSplcTimeWindow2;                      ///< Offset 903     PsmSplcTimeWindow2  @deprecated since revision 6
  UINT8    PsmDplcDomainType1;                      ///< Offset 907     PsmDplcDomainType1
  UINT8    PsmDplcDomainPreference1;                ///< Offset 908     PsmDplcDomainPreference1
  UINT16   PsmDplcPowerLimitIndex1;                 ///< Offset 909     PsmDplcPowerLimitIndex1
  UINT16   PsmDplcDefaultPowerLimit1;               ///< Offset 911     PsmDplcDefaultPowerLimit1
  UINT32   PsmDplcDefaultTimeWindow1;               ///< Offset 913     PsmDplcDefaultTimeWindow1
  UINT16   PsmDplcMinimumPowerLimit1;               ///< Offset 917     PsmDplcMinimumPowerLimit1
  UINT16   PsmDplcMaximumPowerLimit1;               ///< Offset 919     PsmDplcMaximumPowerLimit1
  UINT16   PsmDplcMaximumTimeWindow1;               ///< Offset 921     PsmDplcMaximumTimeWindow1
  UINT8    PsmDplcDomainType2;                      ///< Offset 923     PsmDplcDomainType2         @deprecated since revision 6
  UINT8    PsmDplcDomainPreference2;                ///< Offset 924     PsmDplcDomainPreference2   @deprecated since revision 6
  UINT16   PsmDplcPowerLimitIndex2;                 ///< Offset 925     PsmDplcPowerLimitIndex2    @deprecated since revision 6
  UINT16   PsmDplcDefaultPowerLimit2;               ///< Offset 927     PsmDplcDefaultPowerLimit2  @deprecated since revision 6
  UINT32   PsmDplcDefaultTimeWindow2;               ///< Offset 929     PsmDplcDefaultTimeWindow2  @deprecated since revision 6
  UINT16   PsmDplcMinimumPowerLimit2;               ///< Offset 933     PsmDplcMinimumPowerLimit2  @deprecated since revision 6
  UINT16   PsmDplcMaximumPowerLimit2;               ///< Offset 935     PsmDplcMaximumPowerLimit2  @deprecated since revision 6
  UINT16   PsmDplcMaximumTimeWindow2;               ///< Offset 937     PsmDplcMaximumTimeWindow2  @deprecated since revision 6
  UINT8    WifiEnable;                              ///< Offset 939     WifiEnable
  UINT8    WifiDomainType1;                         ///< Offset 940     WifiDomainType1
  UINT16   WifiPowerLimit1;                         ///< Offset 941     WifiPowerLimit1
  UINT32   WifiTimeWindow1;                         ///< Offset 943     WifiTimeWindow1
  UINT8    WifiDomainType2;                         ///< Offset 947     WifiDomainType2            @deprecated since revision 6
  UINT16   WifiPowerLimit2;                         ///< Offset 948     WifiPowerLimit2            @deprecated since revision 6
  UINT32   WifiTimeWindow2;                         ///< Offset 950     WifiTimeWindow2            @deprecated since revision 6
  UINT8    WifiDomainType3;                         ///< Offset 954     WifiDomainType3            @deprecated since revision 6
  UINT16   WifiPowerLimit3;                         ///< Offset 955     WifiPowerLimit3            @deprecated since revision 6
  UINT32   WifiTimeWindow3;                         ///< Offset 957     WifiTimeWindow3            @deprecated since revision 6
  UINT8    TRxDelay0;                               ///< Offset 961     TRxDelay0
  UINT8    TRxCableLength0;                         ///< Offset 962     TRxCableLength0
  UINT8    TRxDelay1;                               ///< Offset 963     TRxDelay1
  UINT8    TRxCableLength1;                         ///< Offset 964     TRxCableLength1
  UINT8    WrddDomainType1;                         ///< Offset 965     WrddDomainType1
  UINT16   WrddCountryIndentifier1;                 ///< Offset 966     WrddCountryIndentifier1
  UINT8    WrddDomainType2;                         ///< Offset 968     WrddDomainType2            @deprecated since revision 6
  UINT16   WrddCountryIndentifier2;                 ///< Offset 969     WrddCountryIndentifier2    @deprecated since revision 6
  UINT8    Reserved13[47];                          ///< Offset 971:1017
  UINT8    EnableAPPolicy;                          ///< Offset 1018    Adaptive Performance Policy
  UINT16   MinPowerLimit1;                          ///< Offset 1019    Minimum Power Limit 1 for DPTF use via PPCC Object
  UINT16   MinPowerLimit2;                          ///< Offset 1021    Minimum Power Limit 2 for DPTF use via PPCC Object
  //
  // Intel Serial(R) IO Sensor Device Selection
  //
  UINT8    SDS0;                                    ///< Offset 1023    SerialIo Devices for controller0
  UINT8    SDS1;                                    ///< Offset 1024    SerialIo Devices for controller1
  UINT8    SDS2;                                    ///< Offset 1025    SerialIo Devices for controller2
  UINT8    SDS3;                                    ///< Offset 1026    SerialIo Devices for controller3
  UINT8    SDS4;                                    ///< Offset 1027    SerialIo Devices for controller4
  UINT8    SDS5;                                    ///< Offset 1028    SerialIo Devices for controller5
  UINT8    SDS6;                                    ///< Offset 1029    SerialIo Devices for controller6
  UINT8    SDS7;                                    ///< Offset 1030    SerialIo Devices for controller7
  UINT8    SDS8;                                    ///< Offset 1031    SerialIo Devices for controller8
  UINT8    SDS9;                                    ///< Offset 1032    SerialIo Devices for controller9
  UINT8    SDSA;                                    ///< Offset 1033    SerialIo Devices for controller10
  UINT8    WTVX;                                    ///< Offset 1034    WITT test devices' version
  UINT8    WITX;                                    ///< Offset 1035    WITT test devices' connection point
  UINT8    GPTD;                                    ///< Offset 1036    GPIO test devices
  UINT16   GDBT;                                    ///< Offset 1037    GPIO test devices' debounce value,
  UINT8    UTKX;                                    ///< Offset 1039    UTK test devices' connection point
  UINT8    SPTD;                                    ///< Offset 1040    SerialIo additional test devices
  UINT8    Reserved14[24];                          ///< Offset 1041:1064
  UINT32   TableLoadBuffer;                         ///< Offset 1065    Buffer for runtime ACPI Table loading
  UINT8    SDM0;                                    ///< Offset 1069    interrupt mode for controller0 devices
  UINT8    SDM1;                                    ///< Offset 1070    interrupt mode for controller1 devices
  UINT8    SDM2;                                    ///< Offset 1071    interrupt mode for controller2 devices
  UINT8    SDM3;                                    ///< Offset 1072    interrupt mode for controller3 devices
  UINT8    SDM4;                                    ///< Offset 1073    interrupt mode for controller4 devices
  UINT8    SDM5;                                    ///< Offset 1074    interrupt mode for controller5 devices
  UINT8    SDM6;                                    ///< Offset 1075    interrupt mode for controller6 devices
  UINT8    SDM7;                                    ///< Offset 1076    interrupt mode for controller7 devices
  UINT8    SDM8;                                    ///< Offset 1077    interrupt mode for controller8 devices
  UINT8    SDM9;                                    ///< Offset 1078    interrupt mode for controller9 devices
  UINT8    SDMA;                                    ///< Offset 1079    interrupt mode for controller10 devices
  UINT8    SDMB;                                    ///< Offset 1080    interrupt mode for controller11 devices
  UINT8    Reserved15[2];                           ///< Offset 1081:1082
  UINT8    USTP;                                    ///< Offset 1083    use SerialIo timing parameters
  UINT8    Reserved16[41];                          ///< Offset 1084:1124
  UINT32   FingerPrintSleepGpio;                    ///< Offset 1125    Gpio for fingerprint sleep
  UINT32   FingerPrintIrqGpio;                      ///< Offset 1129    Gpio for fingerprint irq
  UINT8    DiscreteGnssModule;                      ///< Offset 1133    GNSS module and its interface, 0=disabled, 1=CG2000 over SerialIo Uart, 2=CG2000 over ISH Uart
  UINT32   DiscreteGnssModuleResetGpio;             ///< Offset 1134    Gpio for GNSS reset
  UINT32   DiscreteBtModuleRfKillGpio;              ///< Offset 1138    Gpio for Bluetooth RfKill
  UINT32   DiscreteBtModuleIrqGpio;                 ///< Offset 1142    Gpio for Bluetooth interrupt
  UINT32   TouchpadIrqGpio;                         ///< Offset 1146    Gpio for touchPaD Interrupt
  UINT32   TouchpanelIrqGpio;                       ///< Offset 1150    Gpio for touchPaneL Interrupt
  UINT8    DiscreteBtUartSupport;                   ///< Offset 1154    Switch to enable BT UART Support
  //
  // MipiCam specific
  //
  UINT8    MipiCamControlLogic0;                    ///< Offset 1155
  UINT8    MipiCamControlLogic1;                    ///< Offset 1156
  UINT8    MipiCamControlLogic2;                    ///< Offset 1157
  UINT8    MipiCamControlLogic3;                    ///< Offset 1158
  UINT8    MipiCamLink0Enabled;                     ///< Offset 1159
  UINT8    MipiCamLink1Enabled;                     ///< Offset 1160
  UINT8    MipiCamLink2Enabled;                     ///< Offset 1161
  UINT8    MipiCamLink3Enabled;                     ///< Offset 1162
  UINT8    MipiCamLanesClkDiv;                      ///< Offset 1163
  // Control Logic 0 options
  UINT8    MipiCamCtrlLogic0_Version;               ///< Offset 1164    Version of CLDB structure
  UINT8    MipiCamCtrlLogic0_Type;                  ///< Offset 1165    Type
  UINT8    MipiCamCtrlLogic0_CrdVersion;            ///< Offset 1166    Version of CRD
  UINT32   MipiCamCtrlLogic0_InputClock;            ///< Offset 1167    Input Clock
  UINT8    MipiCamCtrlLogic0_GpioPinsEnabled;       ///< Offset 1171    Number of GPIO Pins enabled
  UINT8    MipiCamCtrlLogic0_I2cBus;                ///< Offset 1172    I2C Serial Bus Number
  UINT16   MipiCamCtrlLogic0_I2cAddress;            ///< Offset 1173    I2C Address
  UINT8    MipiCamCtrlLogic0_GpioGroupPadNumber[4]; ///< Offset 1175    GPIO Group Pad Number
  UINT8    MipiCamCtrlLogic0_GpioGroupNumber[4];    ///< Offset 1179    GPIO Group Number
  UINT8    MipiCamCtrlLogic0_GpioFunction[4];       ///< Offset 1183    GPIO Function
  UINT8    MipiCamCtrlLogic0_GpioActiveValue[4];    ///< Offset 1187    GPIO Active Value
  UINT8    MipiCamCtrlLogic0_GpioInitialValue[4];   ///< Offset 1191    GPIO Initial Value
  UINT8    MipiCamCtrlLogic0_Pld;                   ///< Offset 1195    Camera Position
  UINT8    MipiCamCtrlLogic0_Wled1FlashMaxCurrent;  ///< Offset 1196    WLED1 Flash Max Current
  UINT8    MipiCamCtrlLogic0_Wled1TorchMaxCurrent;  ///< Offset 1197    WLED1 Torch Max Current
  UINT8    MipiCamCtrlLogic0_Wled2FlashMaxCurrent;  ///< Offset 1198    WLED2 Flash Max Current
  UINT8    MipiCamCtrlLogic0_Wled2TorchMaxCurrent;  ///< Offset 1199    WLED2 Torch Max Current
  UINT8    MipiCamCtrlLogic0_SubPlatformId;         ///< Offset 1200    Sub Platform Id
  UINT8    MipiCamCtrlLogic0_Wled1Type;             ///< Offset 1201    WLED1 Type
  UINT8    MipiCamCtrlLogic0_Wled2Type;             ///< Offset 1202    WLED2 Type
  UINT8    MipiCamCtrlLogic0_PchClockSource;        ///< Offset 1203    PCH Clock source
  // Control Logic 1 options
  UINT8    MipiCamCtrlLogic1_Version;               ///< Offset 1204    Version of CLDB structure
  UINT8    MipiCamCtrlLogic1_Type;                  ///< Offset 1205    Type
  UINT8    MipiCamCtrlLogic1_CrdVersion;            ///< Offset 1206    Version of CRD
  UINT32   MipiCamCtrlLogic1_InputClock;            ///< Offset 1207    Input Clock
  UINT8    MipiCamCtrlLogic1_GpioPinsEnabled;       ///< Offset 1211    Number of GPIO Pins enabled
  UINT8    MipiCamCtrlLogic1_I2cBus;                ///< Offset 1212    I2C Serial Bus Number
  UINT16   MipiCamCtrlLogic1_I2cAddress;            ///< Offset 1213    I2C Address
  UINT8    MipiCamCtrlLogic1_GpioGroupPadNumber[4]; ///< Offset 1215    GPIO Group Pad Number
  UINT8    MipiCamCtrlLogic1_GpioGroupNumber[4];    ///< Offset 1219    GPIO Group Number
  UINT8    MipiCamCtrlLogic1_GpioFunction[4];       ///< Offset 1223    GPIO Function
  UINT8    MipiCamCtrlLogic1_GpioActiveValue[4];    ///< Offset 1227    GPIO Active Value
  UINT8    MipiCamCtrlLogic1_GpioInitialValue[4];   ///< Offset 1231    GPIO Initial Value
  UINT8    MipiCamCtrlLogic1_Pld;                   ///< Offset 1235    Camera Position
  UINT8    MipiCamCtrlLogic1_Wled1FlashMaxCurrent;  ///< Offset 1236    WLED1 Flash Max Current
  UINT8    MipiCamCtrlLogic1_Wled1TorchMaxCurrent;  ///< Offset 1237    WLED1 Torch Max Current
  UINT8    MipiCamCtrlLogic1_Wled2FlashMaxCurrent;  ///< Offset 1238    WLED2 Flash Max Current
  UINT8    MipiCamCtrlLogic1_Wled2TorchMaxCurrent;  ///< Offset 1239    WLED2 Torch Max Current
  UINT8    MipiCamCtrlLogic1_SubPlatformId;         ///< Offset 1240    Sub Platform Id
  UINT8    MipiCamCtrlLogic1_Wled1Type;             ///< Offset 1241    WLED1 Type
  UINT8    MipiCamCtrlLogic1_Wled2Type;             ///< Offset 1242    WLED2 Type
  UINT8    MipiCamCtrlLogic1_PchClockSource;        ///< Offset 1243    PCH Clock source
  // Control Logic 2 options
  UINT8    MipiCamCtrlLogic2_Version;               ///< Offset 1244    Version of CLDB structure
  UINT8    MipiCamCtrlLogic2_Type;                  ///< Offset 1245    Type
  UINT8    MipiCamCtrlLogic2_CrdVersion;            ///< Offset 1246    Version of CRD
  UINT32   MipiCamCtrlLogic2_InputClock;            ///< Offset 1247    Input Clock
  UINT8    MipiCamCtrlLogic2_GpioPinsEnabled;       ///< Offset 1251    Number of GPIO Pins enabled
  UINT8    MipiCamCtrlLogic2_I2cBus;                ///< Offset 1252    I2C Serial Bus Number
  UINT16   MipiCamCtrlLogic2_I2cAddress;            ///< Offset 1253    I2C Address
  UINT8    MipiCamCtrlLogic2_GpioGroupPadNumber[4]; ///< Offset 1255    GPIO Group Pad Number
  UINT8    MipiCamCtrlLogic2_GpioGroupNumber[4];    ///< Offset 1259    GPIO Group Number
  UINT8    MipiCamCtrlLogic2_GpioFunction[4];       ///< Offset 1263    GPIO Function
  UINT8    MipiCamCtrlLogic2_GpioActiveValue[4];    ///< Offset 1267    GPIO Active Value
  UINT8    MipiCamCtrlLogic2_GpioInitialValue[4];   ///< Offset 1271    GPIO Initial Value
  UINT8    MipiCamCtrlLogic2_Pld;                   ///< Offset 1275    Camera Position
  UINT8    MipiCamCtrlLogic2_Wled1FlashMaxCurrent;  ///< Offset 1276    WLED1 Flash Max Current
  UINT8    MipiCamCtrlLogic2_Wled1TorchMaxCurrent;  ///< Offset 1277    WLED1 Torch Max Current
  UINT8    MipiCamCtrlLogic2_Wled2FlashMaxCurrent;  ///< Offset 1278    WLED2 Flash Max Current
  UINT8    MipiCamCtrlLogic2_Wled2TorchMaxCurrent;  ///< Offset 1279    WLED2 Torch Max Current
  UINT8    MipiCamCtrlLogic2_SubPlatformId;         ///< Offset 1280    Sub Platform Id
  UINT8    MipiCamCtrlLogic2_Wled1Type;             ///< Offset 1281    WLED1 Type
  UINT8    MipiCamCtrlLogic2_Wled2Type;             ///< Offset 1282    WLED2 Type
  UINT8    MipiCamCtrlLogic2_PchClockSource;        ///< Offset 1283    PCH Clock source
  // Control Logic 3 options
  UINT8    MipiCamCtrlLogic3_Version;               ///< Offset 1284    Version of CLDB structure
  UINT8    MipiCamCtrlLogic3_Type;                  ///< Offset 1285    Type
  UINT8    MipiCamCtrlLogic3_CrdVersion;            ///< Offset 1286    Version of CRD
  UINT32   MipiCamCtrlLogic3_InputClock;            ///< Offset 1287    Input Clock
  UINT8    MipiCamCtrlLogic3_GpioPinsEnabled;       ///< Offset 1291    Number of GPIO Pins enabled
  UINT8    MipiCamCtrlLogic3_I2cBus;                ///< Offset 1292    I2C Serial Bus Number
  UINT16   MipiCamCtrlLogic3_I2cAddress;            ///< Offset 1293    I2C Address
  UINT8    MipiCamCtrlLogic3_GpioGroupPadNumber[4]; ///< Offset 1295    GPIO Group Pad Number
  UINT8    MipiCamCtrlLogic3_GpioGroupNumber[4];    ///< Offset 1299    GPIO Group Number
  UINT8    MipiCamCtrlLogic3_GpioFunction[4];       ///< Offset 1303    GPIO Function
  UINT8    MipiCamCtrlLogic3_GpioActiveValue[4];    ///< Offset 1307    GPIO Active Value
  UINT8    MipiCamCtrlLogic3_GpioInitialValue[4];   ///< Offset 1311    GPIO Initial Value
  UINT8    MipiCamCtrlLogic3_Pld;                   ///< Offset 1315    Camera Position
  UINT8    MipiCamCtrlLogic3_Wled1FlashMaxCurrent;  ///< Offset 1316    WLED1 Flash Max Current
  UINT8    MipiCamCtrlLogic3_Wled1TorchMaxCurrent;  ///< Offset 1317    WLED1 Torch Max Current
  UINT8    MipiCamCtrlLogic3_Wled2FlashMaxCurrent;  ///< Offset 1318    WLED2 Flash Max Current
  UINT8    MipiCamCtrlLogic3_Wled2TorchMaxCurrent;  ///< Offset 1319    WLED2 Torch Max Current
  UINT8    MipiCamCtrlLogic3_SubPlatformId;         ///< Offset 1320    Sub Platform Id
  UINT8    MipiCamCtrlLogic3_Wled1Type;             ///< Offset 1321    WLED1 Type
  UINT8    MipiCamCtrlLogic3_Wled2Type;             ///< Offset 1322    WLED2 Type
  UINT8    MipiCamCtrlLogic3_PchClockSource;        ///< Offset 1323    PCH Clock source
  // Mipi Cam Link0 options
  UINT8    MipiCamLink0SensorModel;                 ///< Offset 1324    Sensor Model
  UINT8    MipiCamLink0UserHid[9];                  ///< Offset 1325    User defined HID ASCII character 0
                                                    ///< Offset 1333    User defined HID ASCII character 8
  UINT8    MipiCamLink0Pld;                         ///< Offset 1334    Camera Position
  UINT8    MipiCamLink0ModuleName[16];              ///< Offset 1335    Camera Module Name ASCII character 0
                                                    ///< Offset 1350    Camera Module Name ASCII character 15
  UINT8    MipiCamLink0I2cDevicesEnabled;           ///< Offset 1351    Number of I2C devices
  UINT8    MipiCamLink0I2cBus;                      ///< Offset 1352    I2C Serial Bus number
  UINT16   MipiCamLink0I2cAddrDev[12];              ///< Offset 1353    Address of I2C Device0 on Link0
                                                    ///< Offset 1355    Address of I2C Device1 on Link0
                                                    ///< Offset 1357    Address of I2C Device2 on Link0
                                                    ///< Offset 1359    Address of I2C Device3 on Link0
                                                    ///< Offset 1361    Address of I2C Device4 on Link0
                                                    ///< Offset 1363    Address of I2C Device5 on Link0
                                                    ///< Offset 1365    Address of I2C Device6 on Link0
                                                    ///< Offset 1367    Address of I2C Device7 on Link0
                                                    ///< Offset 1369    Address of I2C Device8 on Link0
                                                    ///< Offset 1371    Address of I2C Device9 on Link0
                                                    ///< Offset 1373    Address of I2C Device10 on Link0
                                                    ///< Offset 1375    Address of I2C Device11 on Link0
  UINT8    MipiCamLink0I2cDeviceType[12];           ///< Offset 1377    Type of I2C Device0 on Link0
                                                    ///< Offset 1378    Type of I2C Device1 on Link0
                                                    ///< Offset 1379    Type of I2C Device2 on Link0
                                                    ///< Offset 1380    Type of I2C Device3 on Link0
                                                    ///< Offset 1381    Type of I2C Device4 on Link0
                                                    ///< Offset 1382    Type of I2C Device5 on Link0
                                                    ///< Offset 1383    Type of I2C Device6 on Link0
                                                    ///< Offset 1384    Type of I2C Device7 on Link0
                                                    ///< Offset 1385    Type of I2C Device8 on Link0
                                                    ///< Offset 1386    Type of I2C Device9 on Link0
                                                    ///< Offset 1387    Type of I2C Device10 on Link0
                                                    ///< Offset 1388    Type of I2C Device11 on Link0
  UINT8    MipiCamLink0DD_Version;                  ///< Offset 1389    Version of SSDB structure
  UINT8    MipiCamLink0DD_CrdVersion;               ///< Offset 1390    Version of CRD
  UINT8    MipiCamLink0DD_LinkUsed;                 ///< Offset 1391    CSI2 Link used
  UINT8    MipiCamLink0DD_LaneUsed;                 ///< Offset 1392    MIPI-CSI2 Data Lane
  UINT8    MipiCamLink0DD_EepromType;               ///< Offset 1393    EEPROM Type
  UINT8    MipiCamLink0DD_VcmType;                  ///< Offset 1394    VCM Type
  UINT8    MipiCamLink0DD_FlashSupport;             ///< Offset 1395    Flash Support
  UINT8    MipiCamLink0DD_PrivacyLed;               ///< Offset 1396    Privacy LED
  UINT8    MipiCamLink0DD_Degree;                   ///< Offset 1397    Degree
  UINT32   MipiCamLink0DD_Mclk;                     ///< Offset 1398    MCLK
  UINT8    MipiCamLink0DD_ControlLogic;             ///< Offset 1402    Control Logic
  UINT8    MipiCamLink0DD_PmicPosition;             ///< Offset 1403    PMIC Position
  UINT8    MipiCamLink0DD_VoltageRail;              ///< Offset 1404    Voltage Rail
  // Mipi Cam Link1 options
  UINT8    MipiCamLink1SensorModel;                 ///< Offset 1405    Sensor Model
  UINT8    MipiCamLink1UserHid[9];                  ///< Offset 1406    User defined HID ASCII character 0
                                                    ///< Offset 1414    User defined HID ASCII character 8
  UINT8    MipiCamLink1Pld;                         ///< Offset 1415    Camera Position
  UINT8    MipiCamLink1ModuleName[16];              ///< Offset 1416    Camera Module Name ASCII character 0
                                                    ///< Offset 1431    Camera Module Name ASCII character 15
  UINT8    MipiCamLink1I2cDevicesEnabled;           ///< Offset 1432    Number of I2C devices
  UINT8    MipiCamLink1I2cBus;                      ///< Offset 1433    I2C Serial Bus number
  UINT16   MipiCamLink1I2cAddrDev[12];              ///< Offset 1434    Address of I2C Device0 on Link1
                                                    ///< Offset 1436    Address of I2C Device1 on Link1
                                                    ///< Offset 1438    Address of I2C Device2 on Link1
                                                    ///< Offset 1440    Address of I2C Device3 on Link1
                                                    ///< Offset 1442    Address of I2C Device4 on Link1
                                                    ///< Offset 1444    Address of I2C Device5 on Link1
                                                    ///< Offset 1446    Address of I2C Device6 on Link1
                                                    ///< Offset 1448    Address of I2C Device7 on Link1
                                                    ///< Offset 1450    Address of I2C Device8 on Link1
                                                    ///< Offset 1452    Address of I2C Device9 on Link1
                                                    ///< Offset 1454    Address of I2C Device10 on Link1
                                                    ///< Offset 1456    Address of I2C Device11 on Link1
  UINT8    MipiCamLink1I2cDeviceType[12];           ///< Offset 1458    Type of I2C Device0 on Link1
                                                    ///< Offset 1459    Type of I2C Device1 on Link1
                                                    ///< Offset 1460    Type of I2C Device2 on Link1
                                                    ///< Offset 1461    Type of I2C Device3 on Link1
                                                    ///< Offset 1462    Type of I2C Device4 on Link1
                                                    ///< Offset 1463    Type of I2C Device5 on Link1
                                                    ///< Offset 1464    Type of I2C Device6 on Link1
                                                    ///< Offset 1465    Type of I2C Device7 on Link1
                                                    ///< Offset 1466    Type of I2C Device8 on Link1
                                                    ///< Offset 1467    Type of I2C Device9 on Link1
                                                    ///< Offset 1468    Type of I2C Device10 on Link1
                                                    ///< Offset 1469    Type of I2C Device11 on Link1
  UINT8    MipiCamLink1DD_Version;                  ///< Offset 1470    Version of SSDB structure
  UINT8    MipiCamLink1DD_CrdVersion;               ///< Offset 1471    Version of CRD
  UINT8    MipiCamLink1DD_LinkUsed;                 ///< Offset 1472    CSI2 Link used
  UINT8    MipiCamLink1DD_LaneUsed;                 ///< Offset 1473    MIPI-CSI2 Data Lane
  UINT8    MipiCamLink1DD_EepromType;               ///< Offset 1474    EEPROM Type
  UINT8    MipiCamLink1DD_VcmType;                  ///< Offset 1475    VCM Type
  UINT8    MipiCamLink1DD_FlashSupport;             ///< Offset 1476    Flash Support
  UINT8    MipiCamLink1DD_PrivacyLed;               ///< Offset 1477    Privacy LED
  UINT8    MipiCamLink1DD_Degree;                   ///< Offset 1478    Degree
  UINT32   MipiCamLink1DD_Mclk;                     ///< Offset 1479    MCLK
  UINT8    MipiCamLink1DD_ControlLogic;             ///< Offset 1483    Control Logic
  UINT8    MipiCamLink1DD_PmicPosition;             ///< Offset 1484    PMIC Position
  UINT8    MipiCamLink1DD_VoltageRail;              ///< Offset 1485    Voltage Rail
  // Mipi Cam Link2 options
  UINT8    MipiCamLink2SensorModel;                 ///< Offset 1486    Sensor Model
  UINT8    MipiCamLink2UserHid[9];                  ///< Offset 1487    User defined HID ASCII character 0
                                                    ///< Offset 1495    User defined HID ASCII character 8
  UINT8    MipiCamLink2Pld;                         ///< Offset 1496    Camera Position
  UINT8    MipiCamLink2ModuleName[16];              ///< Offset 1497    Camera Module Name ASCII character 0
                                                    ///< Offset 1512    Camera Module Name ASCII character 15
  UINT8    MipiCamLink2I2cDevicesEnabled;           ///< Offset 1513    Number of I2C devices
  UINT8    MipiCamLink2I2cBus;                      ///< Offset 1514    I2C Serial Bus number
  UINT16   MipiCamLink2I2cAddrDev[12];              ///< Offset 1515    Address of I2C Device0 on Link2
                                                    ///< Offset 1517    Address of I2C Device1 on Link2
                                                    ///< Offset 1519    Address of I2C Device2 on Link2
                                                    ///< Offset 1521    Address of I2C Device3 on Link2
                                                    ///< Offset 1523    Address of I2C Device4 on Link2
                                                    ///< Offset 1525    Address of I2C Device5 on Link2
                                                    ///< Offset 1527    Address of I2C Device6 on Link2
                                                    ///< Offset 1529    Address of I2C Device7 on Link2
                                                    ///< Offset 1531    Address of I2C Device8 on Link2
                                                    ///< Offset 1533    Address of I2C Device9 on Link2
                                                    ///< Offset 1535    Address of I2C Device10 on Link2
                                                    ///< Offset 1537    Address of I2C Device11 on Link2
  UINT8    MipiCamLink2I2cDeviceType[12];           ///< Offset 1539    Type of I2C Device0 on Link2
                                                    ///< Offset 1540    Type of I2C Device1 on Link2
                                                    ///< Offset 1541    Type of I2C Device2 on Link2
                                                    ///< Offset 1542    Type of I2C Device3 on Link2
                                                    ///< Offset 1543    Type of I2C Device4 on Link2
                                                    ///< Offset 1544    Type of I2C Device5 on Link2
                                                    ///< Offset 1545    Type of I2C Device6 on Link2
                                                    ///< Offset 1546    Type of I2C Device7 on Link2
                                                    ///< Offset 1547    Type of I2C Device8 on Link2
                                                    ///< Offset 1548    Type of I2C Device9 on Link2
                                                    ///< Offset 1549    Type of I2C Device10 on Link2
                                                    ///< Offset 1550    Type of I2C Device11 on Link2
  UINT8    MipiCamLink2DD_Version;                  ///< Offset 1551    Version of SSDB structure
  UINT8    MipiCamLink2DD_CrdVersion;               ///< Offset 1552    Version of CRD
  UINT8    MipiCamLink2DD_LinkUsed;                 ///< Offset 1553    CSI2 Link used
  UINT8    MipiCamLink2DD_LaneUsed;                 ///< Offset 1554    MIPI-CSI2 Data Lane
  UINT8    MipiCamLink2DD_EepromType;               ///< Offset 1555    EEPROM Type
  UINT8    MipiCamLink2DD_VcmType;                  ///< Offset 1556    VCM Type
  UINT8    MipiCamLink2DD_FlashSupport;             ///< Offset 1557    Flash Support
  UINT8    MipiCamLink2DD_PrivacyLed;               ///< Offset 1558    Privacy LED
  UINT8    MipiCamLink2DD_Degree;                   ///< Offset 1559    Degree
  UINT32   MipiCamLink2DD_Mclk;                     ///< Offset 1560    MCLK
  UINT8    MipiCamLink2DD_ControlLogic;             ///< Offset 1564    Control Logic
  UINT8    MipiCamLink2DD_PmicPosition;             ///< Offset 1565    PMIC Position
  UINT8    MipiCamLink2DD_VoltageRail;              ///< Offset 1566    Voltage Rail
  // Mipi Cam Link3 options
  UINT8    MipiCamLink3SensorModel;                 ///< Offset 1567    Sensor Model
  UINT8    MipiCamLink3UserHid[9];                  ///< Offset 1568    User defined HID ASCII character 0
                                                    ///< Offset 1576    User defined HID ASCII character 8
  UINT8    MipiCamLink3Pld;                         ///< Offset 1577    Camera Position
  UINT8    MipiCamLink3ModuleName[16];              ///< Offset 1578    Camera Module Name ASCII character 0
                                                    ///< Offset 1593    Camera Module Name ASCII character 15
  UINT8    MipiCamLink3I2cDevicesEnabled;           ///< Offset 1594    Number of I2C devices
  UINT8    MipiCamLink3I2cBus;                      ///< Offset 1595    I2C Serial Bus number
  UINT16   MipiCamLink3I2cAddrDev[12];              ///< Offset 1596    Address of I2C Device0 on Link3
                                                    ///< Offset 1598    Address of I2C Device1 on Link3
                                                    ///< Offset 1600    Address of I2C Device2 on Link3
                                                    ///< Offset 1602    Address of I2C Device3 on Link3
                                                    ///< Offset 1604    Address of I2C Device4 on Link3
                                                    ///< Offset 1606    Address of I2C Device5 on Link3
                                                    ///< Offset 1608    Address of I2C Device6 on Link3
                                                    ///< Offset 1610    Address of I2C Device7 on Link3
                                                    ///< Offset 1612    Address of I2C Device8 on Link3
                                                    ///< Offset 1614    Address of I2C Device9 on Link3
                                                    ///< Offset 1616    Address of I2C Device10 on Link3
                                                    ///< Offset 1618    Address of I2C Device11 on Link3
  UINT8    MipiCamLink3I2cDeviceType[12];           ///< Offset 1620    Type of I2C Device0 on Link3
                                                    ///< Offset 1621    Type of I2C Device1 on Link3
                                                    ///< Offset 1622    Type of I2C Device2 on Link3
                                                    ///< Offset 1623    Type of I2C Device3 on Link3
                                                    ///< Offset 1624    Type of I2C Device4 on Link3
                                                    ///< Offset 1625    Type of I2C Device5 on Link3
                                                    ///< Offset 1626    Type of I2C Device6 on Link3
                                                    ///< Offset 1627    Type of I2C Device7 on Link3
                                                    ///< Offset 1628    Type of I2C Device8 on Link3
                                                    ///< Offset 1629    Type of I2C Device9 on Link3
                                                    ///< Offset 1630    Type of I2C Device10 on Link3
                                                    ///< Offset 1631    Type of I2C Device11 on Link3
  UINT8    MipiCamLink3DD_Version;                  ///< Offset 1632    Version of SSDB structure
  UINT8    MipiCamLink3DD_CrdVersion;               ///< Offset 1633    Version of CRD
  UINT8    MipiCamLink3DD_LinkUsed;                 ///< Offset 1634    CSI2 Link used
  UINT8    MipiCamLink3DD_LaneUsed;                 ///< Offset 1635    MIPI-CSI2 Data Lane
  UINT8    MipiCamLink3DD_EepromType;               ///< Offset 1636    EEPROM Type
  UINT8    MipiCamLink3DD_VcmType;                  ///< Offset 1637    VCM Type
  UINT8    MipiCamLink3DD_FlashSupport;             ///< Offset 1638    Flash Support
  UINT8    MipiCamLink3DD_PrivacyLed;               ///< Offset 1639    Privacy LED
  UINT8    MipiCamLink3DD_Degree;                   ///< Offset 1640    Degree
  UINT32   MipiCamLink3DD_Mclk;                     ///< Offset 1641    MCLK
  UINT8    MipiCamLink3DD_ControlLogic;             ///< Offset 1645    Control Logic
  UINT8    MipiCamLink3DD_PmicPosition;             ///< Offset 1646    PMIC Position
  UINT8    MipiCamLink3DD_VoltageRail;              ///< Offset 1647    Voltage Rail
  UINT8    Reserved17[2];                           ///< Offset 1648:1649
  UINT8    PciDelayOptimizationEcr;                 ///< Offset 1650
  UINT8    I2SC;                                    ///< Offset 1651    HD Audio I2S Codec Selection
  UINT32   I2SI;                                    ///< Offset 1652    HD Audio I2S Codec Interrupt Pin
  UINT8    I2SB;                                    ///< Offset 1656    HD Audio I2S Codec Connection to I2C bus controller instance (I2C[0-5])
  UINT8    OemDesignVariable0;                      ///< Offset 1657    DPTF Oem Design Variables
  UINT8    OemDesignVariable1;                      ///< Offset 1658    DPTF Oem Design Variables
  UINT8    OemDesignVariable2;                      ///< Offset 1659    DPTF Oem Design Variables
  UINT8    OemDesignVariable3;                      ///< Offset 1660    DPTF Oem Design Variables
  UINT8    OemDesignVariable4;                      ///< Offset 1661    DPTF Oem Design Variables
  UINT8    OemDesignVariable5;                      ///< Offset 1662    DPTF Oem Design Variables
  UINT32   UsbTypeCOpBaseAddr;                      ///< Offset 1663    USB Type C Opregion base address
  UINT8    Reserved18[5];                           ///< Offset 1667:1671
  UINT8    WirelessCharging;                        ///< Offset 1672    WirelessCharging
  // RTD3 Settings
  UINT8    Reserved20[7];                           ///< Offset 1673:1679
  UINT8    TPLT;                                    ///< Offset 1680    I2C SerialIo Devices Type of TouchPanel
  UINT8    TPLM;                                    ///< Offset 1681    I2C SerialIo Devices Interrupt Mode for TouchPanel
  UINT8    TPLB;                                    ///< Offset 1682    I2C Custom TouchPanel's BUS Address
  UINT16   TPLH;                                    ///< Offset 1683    I2C Custom TouchPanel's HID Address
  UINT8    TPLS;                                    ///< Offset 1685    I2C Custom TouchPanel's BUS Speed
  UINT8    TPDT;                                    ///< Offset 1686    I2C SerialIo Devices Type of TouchPad
  UINT8    TPDM;                                    ///< Offset 1687    I2C SerialIo Devices Interrupt Mode for TouchPad
  UINT8    TPDB;                                    ///< Offset 1688    I2C Custom TouchPad's BUS Address
  UINT16   TPDH;                                    ///< Offset 1689    I2C Custom TouchPad's HID Address
  UINT8    TPDS;                                    ///< Offset 1691    I2C Custom TouchPad's BUS Speed
  UINT8    FPTT;                                    ///< Offset 1692    SPI SerialIo Devices Type of FingerPrint
  UINT8    FPTM;                                    ///< Offset 1693    SPI SerialIo Devices Interrupt Mode for FingerPrint
  UINT32   HdaDspPpModuleMask;                      ///< Offset 1694    HD-Audio DSP Post-Processing Module Mask
  UINT64   HdaDspPpModCustomGuid1Low;               ///< Offset 1698    HDA PP module custom GUID 1 - first 64bit  [0-63]
  UINT64   HdaDspPpModCustomGuid1High;              ///< Offset 1706    HDA PP module custom GUID 1 - second 64bit [64-127]
  UINT64   HdaDspPpModCustomGuid2Low;               ///< Offset 1714    HDA PP module custom GUID 2 - first 64bit  [0-63]
  UINT64   HdaDspPpModCustomGuid2High;              ///< Offset 1722    HDA PP module custom GUID 2 - second 64bit [64-127]
  UINT64   HdaDspPpModCustomGuid3Low;               ///< Offset 1730    HDA PP module custom GUID 3 - first 64bit  [0-63]
  UINT64   HdaDspPpModCustomGuid3High;              ///< Offset 1738    HDA PP module custom GUID 3 - second 64bit [64-127]
  UINT8    HidEventFilterEnable;                    ///< Offset 1746    HID Event Filter Driver enable
  UINT8    XdciFnEnable;                            ///< Offset 1747    XDCI Enable/Disable status
  UINT8    WrdsWiFiSarEnable;                       ///< Offset 1748    WrdsWiFiSarEnable
  UINT8    WrdsWiFiSarTxPowerSet1Limit1;            ///< Offset 1749    WrdsWiFiSarTxPowerSet1Limit1
  UINT8    WrdsWiFiSarTxPowerSet1Limit2;            ///< Offset 1750    WrdsWiFiSarTxPowerSet1Limit2
  UINT8    WrdsWiFiSarTxPowerSet1Limit3;            ///< Offset 1751    WrdsWiFiSarTxPowerSet1Limit3
  UINT8    WrdsWiFiSarTxPowerSet1Limit4;            ///< Offset 1752    WrdsWiFiSarTxPowerSet1Limit4
  UINT8    WrdsWiFiSarTxPowerSet1Limit5;            ///< Offset 1753    WrdsWiFiSarTxPowerSet1Limit5
  UINT8    WrdsWiFiSarTxPowerSet1Limit6;            ///< Offset 1754    WrdsWiFiSarTxPowerSet1Limit6
  UINT8    WrdsWiFiSarTxPowerSet1Limit7;            ///< Offset 1755    WrdsWiFiSarTxPowerSet1Limit7
  UINT8    WrdsWiFiSarTxPowerSet1Limit8;            ///< Offset 1756    WrdsWiFiSarTxPowerSet1Limit8
  UINT8    WrdsWiFiSarTxPowerSet1Limit9;            ///< Offset 1757    WrdsWiFiSarTxPowerSet1Limit9
  UINT8    WrdsWiFiSarTxPowerSet1Limit10;           ///< Offset 1758    WrdsWiFiSarTxPowerSet1Limit10
  UINT8    EnableVoltageMargining;                  ///< Offset 1759    Enable Voltage Margining
  UINT16   DStateHSPort;                            ///< Offset 1760    D-State for xHCI HS port(BIT0:USB HS Port0 ~ BIT15:USB HS Port15)
  UINT16   DStateSSPort;                            ///< Offset 1762    D-State for xHCI SS port(BIT0:USB SS Port0 ~ BIT15:USB SS Port15)
  UINT8    DStateSataPort;                          ///< Offset 1764    D-State for SATA port(BIT0:SATA Port0 ~ BIT7:SATA Port7)
  UINT8    WigigRfe;                                ///< Offset 1765    WigigRfe                  @deprecated since revision 6
  UINT8    WiGigRfeCh1;                             ///< Offset 1766    WiGigRfeCh1            @deprecated since revision 6
  UINT8    WiGigRfeCh2;                             ///< Offset 1767    WiGigRfeCh2            @deprecated since revision 6
  UINT8    WiGigRfeCh3;                             ///< Offset 1768    WiGigRfeCh3            @deprecated since revision 6
  UINT8    WiGigRfeCh4;                             ///< Offset 1769    WiGigRfeCh4            @deprecated since revision 6
  UINT32   AwvClassIndex;                           ///< Offset 1770    AwvClassIndex        @deprecated since revision 6
  UINT8    EwrdWiFiDynamicSarEnable;                ///< Offset 1774    EwrdWiFiDynamicSarEnable
  UINT8    EwrdWiFiDynamicSarRangeSets;             ///< Offset 1775    EwrdWiFiDynamicSarRangeSets
  UINT8    EwrdWiFiSarTxPowerSet2Limit1;            ///< Offset 1776    EwrdWiFiSarTxPowerSet2Limit1
  UINT8    EwrdWiFiSarTxPowerSet2Limit2;            ///< Offset 1777    EwrdWiFiSarTxPowerSet2Limit2
  UINT8    EwrdWiFiSarTxPowerSet2Limit3;            ///< Offset 1778    EwrdWiFiSarTxPowerSet2Limit3
  UINT8    EwrdWiFiSarTxPowerSet2Limit4;            ///< Offset 1779    EwrdWiFiSarTxPowerSet2Limit4
  UINT8    EwrdWiFiSarTxPowerSet2Limit5;            ///< Offset 1780    EwrdWiFiSarTxPowerSet2Limit5
  UINT8    EwrdWiFiSarTxPowerSet2Limit6;            ///< Offset 1781    EwrdWiFiSarTxPowerSet2Limit6
  UINT8    EwrdWiFiSarTxPowerSet2Limit7;            ///< Offset 1782    EwrdWiFiSarTxPowerSet2Limit7
  UINT8    EwrdWiFiSarTxPowerSet2Limit8;            ///< Offset 1783    EwrdWiFiSarTxPowerSet2Limit8
  UINT8    EwrdWiFiSarTxPowerSet2Limit9;            ///< Offset 1784    EwrdWiFiSarTxPowerSet2Limit9
  UINT8    EwrdWiFiSarTxPowerSet2Limit10;           ///< Offset 1785    EwrdWiFiSarTxPowerSet2Limit10
  UINT8    EwrdWiFiSarTxPowerSet3Limit1;            ///< Offset 1786    EwrdWiFiSarTxPowerSet3Limit1
  UINT8    EwrdWiFiSarTxPowerSet3Limit2;            ///< Offset 1787    EwrdWiFiSarTxPowerSet3Limit2
  UINT8    EwrdWiFiSarTxPowerSet3Limit3;            ///< Offset 1788    EwrdWiFiSarTxPowerSet3Limit3
  UINT8    EwrdWiFiSarTxPowerSet3Limit4;            ///< Offset 1789    EwrdWiFiSarTxPowerSet3Limit4
  UINT8    EwrdWiFiSarTxPowerSet3Limit5;            ///< Offset 1790    EwrdWiFiSarTxPowerSet3Limit5
  UINT8    EwrdWiFiSarTxPowerSet3Limit6;            ///< Offset 1791    EwrdWiFiSarTxPowerSet3Limit6
  UINT8    EwrdWiFiSarTxPowerSet3Limit7;            ///< Offset 1792    EwrdWiFiSarTxPowerSet3Limit7
  UINT8    EwrdWiFiSarTxPowerSet3Limit8;            ///< Offset 1793    EwrdWiFiSarTxPowerSet3Limit8
  UINT8    EwrdWiFiSarTxPowerSet3Limit9;            ///< Offset 1794    EwrdWiFiSarTxPowerSet3Limit9
  UINT8    EwrdWiFiSarTxPowerSet3Limit10;           ///< Offset 1795    EwrdWiFiSarTxPowerSet3Limit10
  UINT8    EwrdWiFiSarTxPowerSet4Limit1;            ///< Offset 1796    EwrdWiFiSarTxPowerSet4Limit1
  UINT8    EwrdWiFiSarTxPowerSet4Limit2;            ///< Offset 1797    EwrdWiFiSarTxPowerSet4Limit2
  UINT8    EwrdWiFiSarTxPowerSet4Limit3;            ///< Offset 1798    EwrdWiFiSarTxPowerSet4Limit3
  UINT8    EwrdWiFiSarTxPowerSet4Limit4;            ///< Offset 1799    EwrdWiFiSarTxPowerSet4Limit4
  UINT8    EwrdWiFiSarTxPowerSet4Limit5;            ///< Offset 1800    EwrdWiFiSarTxPowerSet4Limit5
  UINT8    EwrdWiFiSarTxPowerSet4Limit6;            ///< Offset 1801    EwrdWiFiSarTxPowerSet4Limit6
  UINT8    EwrdWiFiSarTxPowerSet4Limit7;            ///< Offset 1802    EwrdWiFiSarTxPowerSet4Limit7
  UINT8    EwrdWiFiSarTxPowerSet4Limit8;            ///< Offset 1803    EwrdWiFiSarTxPowerSet4Limit8
  UINT8    EwrdWiFiSarTxPowerSet4Limit9;            ///< Offset 1804    EwrdWiFiSarTxPowerSet4Limit9
  UINT8    EwrdWiFiSarTxPowerSet4Limit10;           ///< Offset 1805    EwrdWiFiSarTxPowerSet4Limit10
  UINT8    WgdsWiFiSarDeltaGroup1PowerMax1;         ///< Offset 1806    WgdsWiFiSarDeltaGroup1PowerMax1
  UINT8    WgdsWiFiSarDeltaGroup1PowerChainA1;      ///< Offset 1807    WgdsWiFiSarDeltaGroup1PowerChainA1
  UINT8    WgdsWiFiSarDeltaGroup1PowerChainB1;      ///< Offset 1808    WgdsWiFiSarDeltaGroup1PowerChainB1
  UINT8    WgdsWiFiSarDeltaGroup1PowerMax2;         ///< Offset 1809    WgdsWiFiSarDeltaGroup1PowerMax2
  UINT8    WgdsWiFiSarDeltaGroup1PowerChainA2;      ///< Offset 1810    WgdsWiFiSarDeltaGroup1PowerChainA2
  UINT8    WgdsWiFiSarDeltaGroup1PowerChainB2;      ///< Offset 1811    WgdsWiFiSarDeltaGroup1PowerChainB2
  UINT8    WgdsWiFiSarDeltaGroup2PowerMax1;         ///< Offset 1812    WgdsWiFiSarDeltaGroup2PowerMax1
  UINT8    WgdsWiFiSarDeltaGroup2PowerChainA1;      ///< Offset 1813    WgdsWiFiSarDeltaGroup2PowerChainA1
  UINT8    WgdsWiFiSarDeltaGroup2PowerChainB1;      ///< Offset 1814    WgdsWiFiSarDeltaGroup2PowerChainB1
  UINT8    WgdsWiFiSarDeltaGroup2PowerMax2;         ///< Offset 1815    WgdsWiFiSarDeltaGroup2PowerMax2
  UINT8    WgdsWiFiSarDeltaGroup2PowerChainA2;      ///< Offset 1816    WgdsWiFiSarDeltaGroup2PowerChainA2
  UINT8    WgdsWiFiSarDeltaGroup2PowerChainB2;      ///< Offset 1817    WgdsWiFiSarDeltaGroup2PowerChainB2
  UINT8    WgdsWiFiSarDeltaGroup3PowerMax1;         ///< Offset 1818    WgdsWiFiSarDeltaGroup3PowerMax1
  UINT8    WgdsWiFiSarDeltaGroup3PowerChainA1;      ///< Offset 1819    WgdsWiFiSarDeltaGroup3PowerChainA1
  UINT8    WgdsWiFiSarDeltaGroup3PowerChainB1;      ///< Offset 1820    WgdsWiFiSarDeltaGroup3PowerChainB1
  UINT8    WgdsWiFiSarDeltaGroup3PowerMax2;         ///< Offset 1821    WgdsWiFiSarDeltaGroup3PowerMax2
  UINT8    WgdsWiFiSarDeltaGroup3PowerChainA2;      ///< Offset 1822    WgdsWiFiSarDeltaGroup3PowerChainA2
  UINT8    WgdsWiFiSarDeltaGroup3PowerChainB2;      ///< Offset 1823    WgdsWiFiSarDeltaGroup3PowerChainB2
  UINT8    Reserved21[18];                          ///< Offset 1824:1841
  // Reserved for Groups 4 to 9, each needs 6 bytes and total 36 bytes reserved
  UINT8    WiFiDynamicSarAntennaACurrentSet;        ///< Offset 1842    WiFiDynamicSarAntennaACurrentSet
  UINT8    WiFiDynamicSarAntennaBCurrentSet;        ///< Offset 1843    WiFiDynamicSarAntennaBCurrentSet
  UINT8    BluetoothSar;                            ///< Offset 1844    BluetoothSar
  UINT8    BluetoothSarBr;                          ///< Offset 1845    BluetoothSarBr
  UINT8    BluetoothSarEdr2;                        ///< Offset 1846    BluetoothSarEdr2
  UINT8    BluetoothSarEdr3;                        ///< Offset 1847    BluetoothSarEdr3
  UINT8    BluetoothSarLe;                          ///< Offset 1848    BluetoothSarLe
  UINT8    Reserved22[4];                           ///< Offset 1849:1852
  // Reserved for Bluetooth Sar future use
  UINT8    CoExistenceManager;                      ///< Offset 1853    CoExistenceManager
  UINT8    RunTimeVmControl;                        ///< Offset 1854    RunTime VM Control
  //
  //Feature Specific Data Bits
  //
  UINT8    UsbTypeCSupport;                         ///< Offset 1855    USB Type C Supported
  UINT8    UsbTypeCEcLess;                          ///< Offset 1856    USB Type C EC Less
  UINT32   HebcValue;                               ///< Offset 1857    HebcValue
  UINT8    PcdBatteryPresent;                       ///< Offset 1861    Battery Present - Bit0: Real Battery is supported on this platform. Bit1: Virtual Battery is supported on this platform.
  UINT8    PcdTsOnDimmTemperature;                  ///< Offset 1862    TS-on-DIMM temperature
  UINT8    Reserved23[2];                           ///< Offset 1863:1864
  UINT8    PcdRealBattery1Control;                  ///< Offset 1865    Real Battery 1 Control
  UINT8    PcdRealBattery2Control;                  ///< Offset 1866    Real Battery 2 Control
  UINT8    PcdMipiCamSensor;                        ///< Offset 1867    Mipi Camera Sensor
  UINT8    PcdNCT6776FCOM;                          ///< Offset 1868    NCT6776F COM
  UINT8    PcdNCT6776FSIO;                          ///< Offset 1869    NCT6776F SIO
  UINT8    PcdNCT6776FHWMON;                        ///< Offset 1870    NCT6776F HWMON
  UINT8    PcdH8S2113SIO;                           ///< Offset 1871    H8S2113 SIO
  UINT8    PcdZPoddConfig;                          ///< Offset 1872    ZPODD @deprecated since revision 2
  UINT8    PcdRGBCameraAdr;                         ///< Offset 1873    RGB Camera Address
  UINT8    PcdDepthCameraAdr;                       ///< Offset 1874    Depth Camera Addresy
  UINT32   PcdSmcRuntimeSciPin;                     ///< Offset 1875    SMC Runtime Sci Pin
  UINT8    PcdConvertableDockSupport;               ///< Offset 1879    Convertable Dock Support
  UINT8    PcdEcHotKeyF3Support;                    ///< Offset 1880    Ec Hotkey F3 Support
  UINT8    PcdEcHotKeyF4Support;                    ///< Offset 1881    Ec Hotkey F4 Support
  UINT8    PcdEcHotKeyF5Support;                    ///< Offset 1882    Ec Hotkey F5 Support
  UINT8    PcdEcHotKeyF6Support;                    ///< Offset 1883    Ec Hotkey F6 Support
  UINT8    PcdEcHotKeyF7Support;                    ///< Offset 1884    Ec Hotkey F7 Support
  UINT8    PcdEcHotKeyF8Support;                    ///< Offset 1885    Ec Hotkey F8 Support
  UINT8    PcdVirtualButtonVolumeUpSupport;         ///< Offset 1886    Virtual Button Volume Up Support
  UINT8    PcdVirtualButtonVolumeDownSupport;       ///< Offset 1887    Virtual Button Volume Down Support
  UINT8    PcdVirtualButtonHomeButtonSupport;       ///< Offset 1888    Virtual Button Home Button Support
  UINT8    PcdVirtualButtonRotationLockSupport;     ///< Offset 1889    Virtual Button Rotation Lock Support
  UINT8    PcdSlateModeSwitchSupport;               ///< Offset 1890    Slate Mode Switch Support
  UINT8    PcdAcDcAutoSwitchSupport;                ///< Offset 1891    Ac Dc Auto Switch Support
  UINT32   PcdPmPowerButtonGpioPin;                 ///< Offset 1892    Pm Power Button Gpio Pin
  UINT8    PcdAcpiEnableAllButtonSupport;           ///< Offset 1896    Acpi Enable All Button Support
  UINT8    PcdAcpiHidDriverButtonSupport;           ///< Offset 1897    Acpi Hid Driver Button Support
  UINT8    DisplayDepthLowerLimit;                  ///< Offset 1898    DPTF Display Depth Lower Limit in percent
  UINT8    DisplayDepthUpperLimit;                  ///< Offset 1899    DPTF Display Depth Upper Limit in percent
  UINT8    PepWiGigF1;                              ///< Offset 1900    PEP F1 constraints for WiGig device            @deprecated since revision 6
  UINT32   AuxPowerLimit;                           ///< Offset 1901    Maximum aux power available for PCIe root ports
  UINT8    Reserved24[12];                          ///< Offset 1905:1916
  UINT8    ThermalSamplingPeriodWrls;               ///< Offset 1917    ThermalSamplingPeriodWrls
  UINT32   EcLowPowerModeGpioPin;                   ///< Offset 1918    EcLowPowerModeGpioPin
  UINT32   EcSmiGpioPin;                            ///< Offset 1922    EcSmiGpioPin
  UINT8    WakeOnWiGigSupport;                      ///< Offset 1926    Wake on S3-S4 WiGig Docking Support                               @deprecated since revision 6
  //
  // UCMC setup option, GPIO Pad
  //
  UINT8    UCSI;                                    ///< Offset 1927    Option to Enable/Disable UCSI/UCMC device
  UINT32   UcmcPort1Gpio;                           ///< Offset 1928    Gpio for UCMC Port 1 Interrupt
  UINT32   UcmcPort2Gpio;                           ///< Offset 1932    Gpio for UCMC Port 2 Interrupt
  UINT32   UcmcPort3Gpio;                           ///< Offset 1936    Gpio for UCMC Port 3 Interrupt
  UINT32   UcmcPort4Gpio;                           ///< Offset 1940    Gpio for UCMC Port 4 Interrupt
  UINT8    Reserved25[15];                          ///< Offset 1944:1958
  UINT8    SerialPortAcpiDebug;                     ///< Offset 1959    Serial Port ACPI debug
  UINT8    TotalTypeCPorts;                         ///< Offset 1960    Total Number of type C ports that are supported by platform
  UINT8    UsbTypeCPort1;                           ///< Offset 1961    Type C Connector 1  Port mapping within the controller the port exposed
  UINT8    UsbTypeCPort1Pch;                        ///< Offset 1962    Type C Connector 1  Port mapping within the PCH controller (If Split mode supported)
  UINT8    UsbCPort1Proterties;                     ///< Offset 1963    Type C Connector 1  Portperties Split Support/Controller(PCH/TBT/CPU)/Root port (vaild for TBT)
  UINT8    UsbTypeCPort2;                           ///< Offset 1964    Type C Connector 2  Port mapping within the controller the port exposed
  UINT8    UsbTypeCPort2Pch;                        ///< Offset 1965    Type C Connector 2  Port mapping within the PCH controller (If Split mode supported)
  UINT8    UsbCPort2Proterties;                     ///< Offset 1966    Type C Connector 2  Portperties Split Support/Controller(PCH/TBT/CPU)/Root port (vaild for TBT)
  UINT8    UsbTypeCPort3;                           ///< Offset 1967    Type C Connector 3  Port mapping within the controller the port exposed
  UINT8    UsbTypeCPort3Pch;                        ///< Offset 1968    Type C Connector 3  Port mapping within the PCH controller (If Split mode supported)
  UINT8    UsbCPort3Proterties;                     ///< Offset 1969    Type C Connector 3  Portperties Split Support/Controller(PCH/TBT/CPU)/Root port (vaild for TBT)
  UINT8    UsbTypeCPort4;                           ///< Offset 1970    Type C Connector 4  Port mapping within the controller the port exposed
  UINT8    UsbTypeCPort4Pch;                        ///< Offset 1971    Type C Connector 4  Port mapping within the PCH controller (If Split mode supported)
  UINT8    UsbCPort4Proterties;                     ///< Offset 1972    Type C Connector 4  Portperties Split Support/Controller(PCH/TBT/CPU)/Root port (vaild for TBT)
  UINT8    UsbTypeCPort5;                           ///< Offset 1973    Type C Connector 5  Port mapping within the controller the port exposed
  UINT8    UsbTypeCPort5Pch;                        ///< Offset 1974    Type C Connector 5  Port mapping within the PCH controller (If Split mode supported)
  UINT8    UsbCPort5Proterties;                     ///< Offset 1975    Type C Connector 5  Portperties Split Support/Controller(PCH/TBT/CPU)/Root port (vaild for TBT)
  UINT8    UsbTypeCPort6;                           ///< Offset 1976    Type C Connector 6  Port mapping within the controller the port exposed
  UINT8    UsbTypeCPort6Pch;                        ///< Offset 1977    Type C Connector 6  Port mapping within the PCH controller (If Split mode supported)
  UINT8    UsbCPort6Proterties;                     ///< Offset 1978    Type C Connector 6  Portperties Split Support/Controller(PCH/TBT/CPU)/Root port (vaild for TBT)
  UINT8    Ufp2DfpGlobalFlag;                       ///< Offset 1979    Upstream Facing port or Downstream Facing port Global Flag from LPC EC
  UINT8    Ufp2DfpUsbPort;                          ///< Offset 1980    Upstream Facing port or Downstream Facing port number from LPC EC
  UINT8    DbcGlobalFlag;                           ///< Offset 1981    Debug Mode Global Flag from LPC EC
  UINT8    DbcUsbPort;                              ///< Offset 1982    Debug Mode USB Port Number from LPC EC
  //
  // XTU SMI base address
  //
  UINT32   XtuSmiBaseAddress;                       ///< Offset 1983    XTU SMI structure Base Address
  // SlpS0WithGBESupport Setup variable is main switch for support of Sleep S0 with GBE
  // and used in _DSM method supported by the Driver
  UINT8    SlpS0WithGBESupport;                     ///< Offset 1987    Enable/Disable the System to achieve SLP_S0 even with LAN(GBE) connected.
  UINT8    AntennaDiversity;                        ///< Offset 1988    AntennaDiversity
  UINT8    BluetoothSarLe2Mhz;                      ///< Offset 1989    BluetoothSarLe2Mhz
  UINT8    BluetoothSarLeLr;                        ///< Offset 1990    BluetoothSarLeLr
  UINT8    PowermeterDeviceEnable;                  ///< Offset 1991    Enable PowerMeter
  UINT8    WwanRtd3Option;                          ///< Offset 1992    WWAN RTD3 options
  UINT32   WwanPerstGpio;                           ///< Offset 1993    WWAN PERST Gpio pin
  UINT8    WwanPerstGpioPolarity;                   ///< Offset 1997    WWAN PERST Gpio polarity
  UINT8    PuisEnable;                              ///< Offset 1998    Power Up In Standby mode
  UINT16   WwanDeviceControlReg;                    ///< Offset 1999    WWAN Device Control register save area
  UINT16   WwanLinkControlReg;                      ///< Offset 2001    WWAN Link Control register save area
  UINT16   WwanDeviceControlReg2;                   ///< Offset 2003    WWAN Device Control register save area
  UINT16   WwanMaxSnoopLatency;                     ///< Offset 2005    WWAN LTR Max Snoop Latency save area
  UINT16   WwanMaxNoSnoopLatency;                   ///< Offset 2007    WWAN LTR Max No Snoop Latency save area
  UINT8    EnumerateSataPortConstraints;            ///< Offset 2009    Set to indicate to PEP that constraints at SATA ports should be enumerated
  UINT8    UcsiRetryEC;                             ///< Offset 2010    UcsiRetryEC
  UINT8    TjMaxTemperature;                        ///< Offset 2011    TjMax Temperature
  UINT8    SystemTimeAndAlarmSource;                ///< Offset 2012    Select source for System time and alarm
  UINT8    SdevXhciInterfaceNumber1;                ///< Offset 2013    SDEV xHCI Interface Number for device 1
  UINT8    SdevXhciInterfaceNumber2;                ///< Offset 2014    SDEV xHCI Interface Number for device 2
  UINT8    SdevXhciRootPortNumber1;                 ///< Offset 2015    SDEV xHCI Root Port Number for device 1
  UINT8    SdevXhciRootPortNumber2;                 ///< Offset 2016    SDEV xHCI Root Port Number for device 2
  UINT32   WlanWakeGpio;                            ///< Offset 2017    WLAN PEWAKE Gpio pin
  UINT8    WlanRootPortNumber;                      ///< Offset 2021    WLAN Root Port Number
} PLATFORM_NVS_AREA;

#pragma pack(pop)
#endif
