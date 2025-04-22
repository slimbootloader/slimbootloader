/** @file
  ACPI DSDT table

  Copyright (c) 2011 - 2021, Intel Corporation. All rights reserved.<BR>
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
  UINT8    ActiveThermalTripPointSA;                ///< Offset 83      Active Trip Point for MCH
  UINT8    PassiveThermalTripPointSA;               ///< Offset 84      Passive Trip Point for MCH
  UINT32   PlatformCpuId;                           ///< Offset 85      CPUID Feature Information [EAX]
  UINT32   TBARB;                                   ///< Offset 89      Reserved for Thermal Base Low Address for BIOS
  UINT32   TBARBH;                                  ///< Offset 93      Reserved for Thermal Base High Address for BIOS
  UINT8    TsOnDimmEnabled;                         ///< Offset 97      TS-on-DIMM is chosen in SETUP and present on the DIMM
  //
  // Board info
  //
  UINT8    PlatformFlavor;                          ///< Offset 98      Platform Flavor
  UINT16   BoardRev;                                ///< Offset 99      Board Rev
  //
  // Package temperature
  //
  UINT8    PackageDTSTemperature;                   ///< Offset 101     Package Temperature
  UINT8    IsPackageTempMSRAvailable;               ///< Offset 102     Package Temperature MSR available
  UINT8    PeciAccessMethod;                        ///< Offset 103     Peci Access Method
  UINT8    Ac0FanSpeed;                             ///< Offset 104     _AC0 Fan Speed
  UINT8    Ac1FanSpeed;                             ///< Offset 105     _AC1 Fan Speed
  UINT8    Ap2DigitalThermalSensorTemperature;      ///< Offset 106     Digital Thermal Sensor 3 Reading
  UINT8    Ap3DigitalThermalSensorTemperature;      ///< Offset 107     Digital Thermal Sensor 4 Reading
  //
  // XTU 3.0 Specification
  //
  UINT32   XTUBaseAddress;                          ///< Offset 108     XTU Continous structure Base Address
  UINT32   XTUSize;                                 ///< Offset 112     XMP Size
  UINT32   XMPBaseAddress;                          ///< Offset 116     XMP Base Address
  UINT8    DDRReferenceFreq;                        ///< Offset 120     DDR Reference Frequency
  UINT8    Rtd3Support;                             ///< Offset 121     Runtime D3 support.
  UINT8    Rtd3P0dl;                                ///< Offset 122     User selctable Delay for Device D0 transition.
  UINT8    Rtd3P3dl;                                ///< Offset 123     User selctable Delay for Device D3 transition.
  //
  // DPTF Devices and trip points
  //
  UINT8    EnableDptf;                              ///< Offset 124     EnableDptf
  UINT16   EnableDCFG;                              ///< Offset 125     EnableDCFG
  UINT8    EnableSaDevice;                          ///< Offset 127     EnableSaDevice
  UINT8    CriticalThermalTripPointSA;              ///< Offset 128     CriticalThermalTripPointSa
  UINT8    HotThermalTripPointSA;                   ///< Offset 129     HotThermalTripPointSa
  UINT8    ThermalSamplingPeriodSA;                 ///< Offset 130     ThermalSamplingPeriodSA
  //
  // DPTF Policies
  //
  UINT8    EnableCtdpPolicy;                        ///< Offset 131     EnableCtdpPolicy
  //
  // DPPM Devices and trip points
  //
  UINT8    EnableFan1Device;                        ///< Offset 132     EnableFan1Device
  UINT8    EnableAmbientDevice;                     ///< Offset 133     EnableAmbientDevice
  UINT8    ActiveThermalTripPointAmbient;           ///< Offset 134     ActiveThermalTripPointAmbient
  UINT8    PassiveThermalTripPointAmbient;          ///< Offset 135     PassiveThermalTripPointAmbient
  UINT8    CriticalThermalTripPointAmbient;         ///< Offset 136     CriticalThermalTripPointAmbient
  UINT8    HotThermalTripPointAmbient;              ///< Offset 137     HotThermalTripPointAmbient
  UINT8    EnableSkinDevice;                        ///< Offset 138     EnableSkinDevice
  UINT8    ActiveThermalTripPointSkin;              ///< Offset 139     ActiveThermalTripPointSkin
  UINT8    PassiveThermalTripPointSkin;             ///< Offset 140     PassiveThermalTripPointSkin
  UINT8    CriticalThermalTripPointSkin;            ///< Offset 141     CriticalThermalTripPointSkin
  UINT8    HotThermalTripPointSkin;                 ///< Offset 142     HotThermalTripPointSkin
  UINT8    EnableExhaustFanDevice;                  ///< Offset 143     EnableExhaustFanDevice
  UINT8    ActiveThermalTripPointExhaustFan;        ///< Offset 144     ActiveThermalTripPointExhaustFan
  UINT8    PassiveThermalTripPointExhaustFan;       ///< Offset 145     PassiveThermalTripPointExhaustFan
  UINT8    CriticalThermalTripPointExhaustFan;      ///< Offset 146     CriticalThermalTripPointExhaustFan
  UINT8    HotThermalTripPointExhaustFan;           ///< Offset 147     HotThermalTripPointExhaustFan
  UINT8    EnableVRDevice;                          ///< Offset 148     EnableVRDevice
  UINT8    ActiveThermalTripPointVR;                ///< Offset 149     ActiveThermalTripPointVR
  UINT8    PassiveThermalTripPointVR;               ///< Offset 150     PassiveThermalTripPointVR
  UINT8    CriticalThermalTripPointVR;              ///< Offset 151     CriticalThermalTripPointVR
  UINT8    HotThermalTripPointVR;                   ///< Offset 152     HotThermalTripPointVR
  //
  // DPPM Policies
  //
  UINT8    EnableActivePolicy;                      ///< Offset 153     EnableActivePolicy
  UINT8    EnablePassivePolicy;                     ///< Offset 154     EnablePassivePolicy
  UINT8    EnableCriticalPolicy;                    ///< Offset 155     EnableCriticalPolicy
  UINT8    EnablePIDPolicy;                         ///< Offset 156     EnablePIDPolicy
  UINT8    TrtRevision;                             ///< Offset 157     TrtRevision
  //
  // CLPO (Current Logical Processor Off lining Setting)
  //
  UINT8    LPOEnable;                               ///< Offset 158     LPOEnable
  UINT8    LPOStartPState;                          ///< Offset 159     LPOStartPState
  UINT8    LPOStepSize;                             ///< Offset 160     LPOStepSize
  UINT8    LPOPowerControlSetting;                  ///< Offset 161     LPOPowerControlSetting
  UINT8    LPOPerformanceControlSetting;            ///< Offset 162     LPOPerformanceControlSetting
  //
  // Miscellaneous DPTF
  //
  UINT32   PpccStepSize;                            ///< Offset 163     PPCC Step Size
  UINT8    EnableDisplayParticipant;                ///< Offset 167     EnableDisplayParticipant
  //
  // BIOS Guard
  //
  UINT64   BiosGuardMemAddress;                     ///< Offset 168     BIOS Guard Memory Address for Tool Interface
  UINT8    BiosGuardMemSize;                        ///< Offset 176     BIOS Guard Memory Size for Tool Interface
  UINT16   BiosGuardIoTrapAddress;                  ///< Offset 177     BIOS Guard IoTrap Address for Tool Interface
  //
  // Never Sleep Technology
  //
  UINT8    IrmtCfg;                                 ///< Offset 179     Irmt Configuration
  //
  // Comms Hub
  //
  UINT8    CommsHubEnable;                          ///< Offset 180     Comms Hub Enable/Disable
  UINT8    LowPowerS0Idle;                          ///< Offset 181     Low Power S0 Idle Enable
  //
  // BIOS only version of Config TDP
  //
  UINT8    ConfigTdpBios;                           ///< Offset 182     enable/disable BIOS only version of Config TDP
  UINT8    DockSmi;                                 ///< Offset 183     Dock SMI number
  //
  // LPC SIO configuration
  //
  UINT16   LpcSioPort1;                             ///< Offset 184     SIO config port 1
  UINT16   LpcSioPort2;                             ///< Offset 186     SIO config port 2
  UINT16   LpcSioPmeBar;                            ///< Offset 188     SIO PME Base Address
  UINT8    Reserved0[310];                          ///< Offset 190:499
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
  UINT16   Rtd3Config0;                             ///< Offset 537     RTD3 Config Setting0(BIT0:ZPODD, BIT1:Reserved, BIT2:PCIe NVMe, Bit4:SKL SDS SIP I2C Touch, BIT6:Card Reader, BIT7:WWAN)
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
  UINT32   LowPowerS0IdleConstraint;                ///< Offset 778     PEP Constraints
  // Bit[1:0] - Storage (0:None, 1:Storage Controller, 2:Raid)
  // Bit[2]   - En/Dis UART0
  // Bit[3]   - En/Dis UART1
  // Bit[4]   - En/Dis PCIe NVMe/AHCI
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
  // Bit[20]  - En/Dis SPI2
  // Bit[21]  - En/Dis IDA
  // Bit[22]  - En/Dis CSME
  // Bit[23]  - En/Dis I2C6
  // Bit[24]  - En/Dis I2C7
  // Bit[25]  - En/Dis THC0
  // Bit[26]  - En/Dis THC1
  // Bit[27]  - En/Dis ISI
  // Bit[28]  - En/Dis GBE TSN
  // Bit[29]  - En/Dis PSE
  UINT32   LowPowerS0IdleConstraint2;               ///< Offset 782     PEP Constraints 2
  // Bit[0]   - En/Dis PSE TSN
  // Bit[1]   - En/Dis PSE DMA
  // Bit[2]   - En/Dis PSE PWM
  // Bit[3]   - En/Dis PSE ADC
  // Bit[4]   - En/Dis PSE SPI0
  // Bit[5]   - En/Dis PSE SPI1
  // Bit[6]   - En/Dis PSE SPI2
  // Bit[7]   - En/Dis PSE SPI3
  // Bit[8]   - En/Dis PSE I2C0
  // Bit[9]   - En/Dis PSE I2C1
  // Bit[10]  - En/Dis PSE I2C2
  // Bit[11]  - En/Dis PSE I2C3
  // Bit[12]  - En/Dis PSE I2C4
  // Bit[13]  - En/Dis PSE I2C5
  // Bit[14]  - En/Dis PSE I2C6
  // Bit[15]  - En/Dis PSE I2C7
  // Bit[16]  - En/Dis PSE UART0
  // Bit[17]  - En/Dis PSE UART1
  // Bit[18]  - En/Dis PSE UART2
  // Bit[19]  - En/Dis PSE UART3
  // Bit[20]  - En/Dis PSE UART4
  // Bit[21]  - En/Dis PSE UART5
  // Bit[22]  - En/Dis PSE I2S0
  // Bit[23]  - En/Dis PSE I2S1
  // Bit[24]  - En/Dis PSE GPIO 0
  // Bit[25]  - En/Dis PSE GPIO 1
  // Bit[26]  - En/Dis PSE CAN 0
  // Bit[27]  - En/Dis PSE CAN 1
  // Bit[28]  - En/Dis PSE QEP 0
  // Bit[29]  - En/Dis PSE QEP 1
  // Bit[30]  - En/Dis PSE QEP 2
  // Bit[31]  - En/Dis PSE QEP 3
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
  // DPTF
  //
  UINT8    EnableWWANParticipant;                   ///< Offset 790     EnableWWANParticipant
  UINT8    ActiveThermalTripPointWWAN;              ///< Offset 791     ActiveThermalTripPointWWAN
  UINT8    PassiveThermalTripPointWWAN;             ///< Offset 792     PassiveThermalTripPointWWAN
  UINT8    CriticalThermalTripPointWWAN;            ///< Offset 793     CriticalThermalTripPointWWAN
  UINT8    HotThermalTripPointWWAN;                 ///< Offset 794     HotThermalTripPointWWAN
  UINT8    Reserved11[12];                          ///< Offset 795:806
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
  UINT8    SataPortState;                           ///< Offset 819     SATA port state, Bit0 - Port0, Bit1 - Port1, Bit2 - Port2, Bit3 - Port3
  //
  // DPTF
  //
  UINT8    Enable2DCameraParticipant;               ///< Offset 820     Enable2DCameraParticipant
  UINT8    EnableBatteryParticipant;                ///< Offset 821     EnableBatteryParticipant
  UINT8    EcLowPowerMode;                          ///< Offset 822     EC Low Power Mode: 1 - Enabled, 0 - Disabled
  UINT8    SensorSamplingPeriodSen1;                ///< Offset 823     SensorSamplingPeriodSen1
  UINT8    SensorSamplingPeriodSen2;                ///< Offset 824     SensorSamplingPeriodSen2
  UINT8    SensorSamplingPeriodSen3;                ///< Offset 825     SensorSamplingPeriodSen3
  UINT8    SensorSamplingPeriodSen4;                ///< Offset 826     SensorSamplingPeriodSen4
  UINT8    SensorSamplingPeriodSen5;                ///< Offset 827     SensorSamplingPeriodSen5
  UINT8    ThermalSamplingPeriodTMEM;               ///< Offset 828     ThermalSamplingPeriodTMEM @deprecated. Memory Participant is not POR for DPTF
  UINT8    EnableStorageParticipantST1;             ///< Offset 829     EnableStorageParticipantST1
  UINT8    ActiveThermalTripPointST1;               ///< Offset 830     ActiveThermalTripPointST1
  UINT8    PassiveThermalTripPointST1;              ///< Offset 831     PassiveThermalTripPointST1
  UINT8    CriticalThermalTripPointST1;             ///< Offset 832     CriticalThermalTripPointST1
  UINT8    CriticalThermalTripPointS3ST1;           ///< Offset 833     CriticalThermalTripPointS3ST1
  UINT8    HotThermalTripPointST1;                  ///< Offset 834     HotThermalTripPointST1
  UINT8    EnableStorageParticipantST2;             ///< Offset 835     EnableStorageParticipantST2
  UINT8    ActiveThermalTripPointST2;               ///< Offset 836     ActiveThermalTripPointST2
  UINT8    PassiveThermalTripPointST2;              ///< Offset 837     PassiveThermalTripPointST2
  UINT8    CriticalThermalTripPointST2;             ///< Offset 838     CriticalThermalTripPointST2
  UINT8    CriticalThermalTripPointS3ST2;           ///< Offset 839     CriticalThermalTripPointS3ST2
  UINT8    HotThermalTripPointST2;                  ///< Offset 840     HotThermalTripPointST2
  UINT8    EnableVS1Participant;                    ///< Offset 841     EnableVS1Participant
  UINT8    ActiveThermalTripPointVS1;               ///< Offset 842     ActiveThermalTripPointVS1
  UINT8    PassiveThermalTripPointVS1;              ///< Offset 843     PassiveThermalTripPointVS1
  UINT8    CriticalThermalTripPointVS1;             ///< Offset 844     CriticalThermalTripPointVS1
  UINT8    CriticalThermalTripPointVS1S3;           ///< Offset 845     CriticalThermalTripPointVS1S3
  UINT8    HotThermalTripPointVS1;                  ///< Offset 846     HotThermalTripPointVS1
  UINT8    EnableVS2Participant;                    ///< Offset 847     EnableVS2Participant
  UINT8    ActiveThermalTripPointVS2;               ///< Offset 848     ActiveThermalTripPointVS2
  UINT8    PassiveThermalTripPointVS2;              ///< Offset 849     PassiveThermalTripPointVS2
  UINT8    CriticalThermalTripPointVS2;             ///< Offset 850     CriticalThermalTripPointVS2
  UINT8    CriticalThermalTripPointVS2S3;           ///< Offset 851     CriticalThermalTripPointVS2S3
  UINT8    HotThermalTripPointVS2;                  ///< Offset 852     HotThermalTripPointVS2
  UINT8    EnableSen1Participant;                   ///< Offset 853     EnableSen1Participant
  UINT8    ActiveThermalTripPointSen1;              ///< Offset 854     ActiveThermalTripPointSen1
  UINT8    PassiveThermalTripPointSen1;             ///< Offset 855     PassiveThermalTripPointSen1
  UINT8    CriticalThermalTripPointSen1;            ///< Offset 856     CriticalThermalTripPointSen1
  UINT8    HotThermalTripPointSen1;                 ///< Offset 857     HotThermalTripPointSen1
  UINT8    EnableSen2Participant;                   ///< Offset 858     EnableSen2Participant
  UINT8    ActiveThermalTripPointSen2;              ///< Offset 859     ActiveThermalTripPointSen2
  UINT8    PassiveThermalTripPointSen2;             ///< Offset 860     PassiveThermalTripPointSen2
  UINT8    CriticalThermalTripPointSen2;            ///< Offset 861     CriticalThermalTripPointSen2
  UINT8    HotThermalTripPointSen2;                 ///< Offset 862     HotThermalTripPointSen2
  UINT8    EnableSen3Participant;                   ///< Offset 863     EnableSen3Participant
  UINT8    ActiveThermalTripPointSen3;              ///< Offset 864     ActiveThermalTripPointSen3
  UINT8    PassiveThermalTripPointSen3;             ///< Offset 865     PassiveThermalTripPointSen3
  UINT8    CriticalThermalTripPointSen3;            ///< Offset 866     CriticalThermalTripPointSen3
  UINT8    HotThermalTripPointSen3;                 ///< Offset 867     HotThermalTripPointSen3
  UINT8    EnableSen4Participant;                   ///< Offset 868     EnableSen4Participant
  UINT8    ActiveThermalTripPointSen4;              ///< Offset 869     ActiveThermalTripPointSen4
  UINT8    PassiveThermalTripPointSen4;             ///< Offset 870     PassiveThermalTripPointSen4
  UINT8    CriticalThermalTripPointSen4;            ///< Offset 871     CriticalThermalTripPointSen4
  UINT8    HotThermalTripPointSen4;                 ///< Offset 872     HotThermalTripPointSen4
  UINT8    EnableSen5Participant;                   ///< Offset 873     EnableSen5Participant
  UINT8    ActiveThermalTripPointSen5;              ///< Offset 874     ActiveThermalTripPointSen5
  UINT8    PassiveThermalTripPointSen5;             ///< Offset 875     PassiveThermalTripPointSen5
  UINT8    CriticalThermalTripPointSen5;            ///< Offset 876     CriticalThermalTripPointSen5
  UINT8    HotThermalTripPointSen5;                 ///< Offset 877     HotThermalTripPointSen5
  UINT8    CriticalThermalTripPointSen1S3;          ///< Offset 878     CriticalThermalTripPointSen1S3
  UINT8    CriticalThermalTripPointSen2S3;          ///< Offset 879     CriticalThermalTripPointSen2S3
  UINT8    CriticalThermalTripPointSen3S3;          ///< Offset 880     CriticalThermalTripPointSen3S3
  UINT8    CriticalThermalTripPointSen4S3;          ///< Offset 881     CriticalThermalTripPointSen4S3
  UINT8    CriticalThermalTripPointSen5S3;          ///< Offset 882     CriticalThermalTripPointSen5S3
  UINT8    PowerSharingManagerEnable;               ///< Offset 883     PowerSharingManagerEnable
  UINT8    PsmSplcDomainType1;                      ///< Offset 884     PsmSplcDomainType1
  UINT32   PsmSplcPowerLimit1;                      ///< Offset 885     PsmSplcPowerLimit1
  UINT32   PsmSplcTimeWindow1;                      ///< Offset 889     PsmSplcTimeWindow1
  UINT8    PsmDplcDomainType1;                      ///< Offset 893     PsmDplcDomainType1
  UINT8    PsmDplcDomainPreference1;                ///< Offset 894     PsmDplcDomainPreference1
  UINT16   PsmDplcPowerLimitIndex1;                 ///< Offset 895     PsmDplcPowerLimitIndex1
  UINT16   PsmDplcDefaultPowerLimit1;               ///< Offset 897     PsmDplcDefaultPowerLimit1
  UINT32   PsmDplcDefaultTimeWindow1;               ///< Offset 899     PsmDplcDefaultTimeWindow1
  UINT16   PsmDplcMinimumPowerLimit1;               ///< Offset 903     PsmDplcMinimumPowerLimit1
  UINT16   PsmDplcMaximumPowerLimit1;               ///< Offset 905     PsmDplcMaximumPowerLimit1
  UINT16   PsmDplcMaximumTimeWindow1;               ///< Offset 907     PsmDplcMaximumTimeWindow1
  UINT8    WifiEnable;                              ///< Offset 909     WifiEnable
  UINT8    WifiDomainType1;                         ///< Offset 910     WifiDomainType1
  UINT16   WifiPowerLimit1;                         ///< Offset 911     WifiPowerLimit1
  UINT32   WifiTimeWindow1;                         ///< Offset 913     WifiTimeWindow1
  UINT8    TRxDelay0;                               ///< Offset 917     TRxDelay0
  UINT8    TRxCableLength0;                         ///< Offset 918     TRxCableLength0
  UINT8    TRxDelay1;                               ///< Offset 919     TRxDelay1
  UINT8    TRxCableLength1;                         ///< Offset 920     TRxCableLength1
  UINT8    WrddDomainType1;                         ///< Offset 921     WrddDomainType1
  UINT16   WrddCountryIndentifier1;                 ///< Offset 922     WrddCountryIndentifier1
  UINT8    Reserved12[94];                          ///< Offset 924:1017
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
  UINT8    TPLT;                                    ///< Offset 1034    I2C SerialIo Devices Type of TouchPanel
  UINT8    TPLM;                                    ///< Offset 1035    I2C SerialIo Devices Interrupt Mode for TouchPanel
  UINT8    TPLB;                                    ///< Offset 1036    I2C Custom TouchPanel's BUS Address
  UINT16   TPLH;                                    ///< Offset 1037    I2C Custom TouchPanel's HID Address
  UINT8    TPLS;                                    ///< Offset 1039    I2C Custom TouchPanel's BUS Speed
  UINT8    TPDT;                                    ///< Offset 1040    I2C SerialIo Devices Type of TouchPad
  UINT8    TPDM;                                    ///< Offset 1041    I2C SerialIo Devices Interrupt Mode for TouchPad
  UINT8    TPDB;                                    ///< Offset 1042    I2C Custom TouchPad's BUS Address
  UINT16   TPDH;                                    ///< Offset 1043    I2C Custom TouchPad's HID Address
  UINT8    TPDS;                                    ///< Offset 1045    I2C Custom TouchPad's BUS Speed
  UINT8    FPTT;                                    ///< Offset 1046    SPI SerialIo Devices Type of FingerPrint
  UINT8    FPTM;                                    ///< Offset 1047    SPI SerialIo Devices Interrupt Mode for FingerPrint
  UINT8    WTVX;                                    ///< Offset 1048    WITT test devices' version
  UINT8    SIOI;                                    ///< Offset 1049    WITT SIO I2C test devices' connection point
  UINT8    SIOS;                                    ///< Offset 1050    WITT SIO SPI test devices' connection point
  UINT8    SIOU;                                    ///< Offset 1051    SIO UART test devices' connection point
  UINT8    GPTD;                                    ///< Offset 1052    GPIO test devices
  UINT16   GDBT;                                    ///< Offset 1053    GPIO test devices' debounce value,
  UINT8    SPTD;                                    ///< Offset 1055    SerialIo additional test devices
  UINT8    DMTX;                                    ///< Offset 1056    DMA Test Enable/Disable
  UINT8    PIOI;                                    ///< Offset 1057    WITT PSE I2C test devices' connection point
  UINT8    PIOS;                                    ///< Offset 1058    WITT PSE SPI test devices' connection point
  UINT8    PIOU;                                    ///< Offset 1059    PSE UART test devices' connection point
  UINT8    Reserved13[6];                           ///< Offset 1060:1065
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
  UINT8    MipiCamLanesClkDiv;                      ///< Offset 1165
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
  // Mipi Cam Link1 options
  UINT8    MipiCamLink1SensorModel;                 ///< Offset 1489    Sensor Model
  UINT8    MipiCamLink1UserHid[9];                  ///< Offset 1490    User defined HID ASCII character 0
                                                    ///< Offset 1498    User defined HID ASCII character 8
  UINT8    MipiCamLink1Pld;                         ///< Offset 1499    Camera Position
  UINT8    MipiCamLink1ModuleName[16];              ///< Offset 1500    Camera Module Name ASCII character 0
                                                    ///< Offset 1515    Camera Module Name ASCII character 15
  UINT8    MipiCamLink1I2cDevicesEnabled;           ///< Offset 1516    Number of I2C devices
  UINT8    MipiCamLink1I2cBus;                      ///< Offset 1517    I2C Serial Bus number
  UINT16   MipiCamLink1I2cAddrDev[12];              ///< Offset 1518    Address of I2C Device0 on Link1
                                                    ///< Offset 1520    Address of I2C Device1 on Link1
                                                    ///< Offset 1522    Address of I2C Device2 on Link1
                                                    ///< Offset 1524    Address of I2C Device3 on Link1
                                                    ///< Offset 1526    Address of I2C Device4 on Link1
                                                    ///< Offset 1528    Address of I2C Device5 on Link1
                                                    ///< Offset 1530    Address of I2C Device6 on Link1
                                                    ///< Offset 1532    Address of I2C Device7 on Link1
                                                    ///< Offset 1534    Address of I2C Device8 on Link1
                                                    ///< Offset 1536    Address of I2C Device9 on Link1
                                                    ///< Offset 1538    Address of I2C Device10 on Link1
                                                    ///< Offset 1540    Address of I2C Device11 on Link1
  UINT8    MipiCamLink1I2cDeviceType[12];           ///< Offset 1542    Type of I2C Device0 on Link1
                                                    ///< Offset 1543    Type of I2C Device1 on Link1
                                                    ///< Offset 1544    Type of I2C Device2 on Link1
                                                    ///< Offset 1545    Type of I2C Device3 on Link1
                                                    ///< Offset 1546    Type of I2C Device4 on Link1
                                                    ///< Offset 1547    Type of I2C Device5 on Link1
                                                    ///< Offset 1548    Type of I2C Device6 on Link1
                                                    ///< Offset 1549    Type of I2C Device7 on Link1
                                                    ///< Offset 1550    Type of I2C Device8 on Link1
                                                    ///< Offset 1551    Type of I2C Device9 on Link1
                                                    ///< Offset 1552    Type of I2C Device10 on Link1
                                                    ///< Offset 1553    Type of I2C Device11 on Link1
  UINT8    MipiCamLink1DD_Version;                  ///< Offset 1554    Version of SSDB structure
  UINT8    MipiCamLink1DD_CrdVersion;               ///< Offset 1555    Version of CRD
  UINT8    MipiCamLink1DD_LinkUsed;                 ///< Offset 1556    CSI2 Link used
  UINT8    MipiCamLink1DD_LaneUsed;                 ///< Offset 1557    MIPI-CSI2 Data Lane
  UINT8    MipiCamLink1DD_EepromType;               ///< Offset 1558    EEPROM Type
  UINT8    MipiCamLink1DD_VcmType;                  ///< Offset 1559    VCM Type
  UINT8    MipiCamLink1DD_FlashSupport;             ///< Offset 1560    Flash Support
  UINT8    MipiCamLink1DD_PrivacyLed;               ///< Offset 1561    Privacy LED
  UINT8    MipiCamLink1DD_Degree;                   ///< Offset 1562    Degree
  UINT32   MipiCamLink1DD_Mclk;                     ///< Offset 1563    MCLK
  UINT8    MipiCamLink1DD_ControlLogic;             ///< Offset 1567    Control Logic
  UINT8    MipiCamLink1DD_PmicPosition;             ///< Offset 1568    PMIC Position
  UINT8    MipiCamLink1DD_VoltageRail;              ///< Offset 1569    Voltage Rail
  UINT8    MipiCamLink1DD_PprValue;                 ///< Offset 1570    PPR Value
  UINT8    MipiCamLink1DD_PprUnit;                  ///< Offset 1571    PPR Unit
  // Mipi Cam Link2 options
  UINT8    MipiCamLink2SensorModel;                 ///< Offset 1572    Sensor Model
  UINT8    MipiCamLink2UserHid[9];                  ///< Offset 1573    User defined HID ASCII character 0
                                                    ///< Offset 1581    User defined HID ASCII character 8
  UINT8    MipiCamLink2Pld;                         ///< Offset 1582    Camera Position
  UINT8    MipiCamLink2ModuleName[16];              ///< Offset 1583    Camera Module Name ASCII character 0
                                                    ///< Offset 1598    Camera Module Name ASCII character 15
  UINT8    MipiCamLink2I2cDevicesEnabled;           ///< Offset 1599    Number of I2C devices
  UINT8    MipiCamLink2I2cBus;                      ///< Offset 1600    I2C Serial Bus number
  UINT16   MipiCamLink2I2cAddrDev[12];              ///< Offset 1601    Address of I2C Device0 on Link2
                                                    ///< Offset 1603    Address of I2C Device1 on Link2
                                                    ///< Offset 1605    Address of I2C Device2 on Link2
                                                    ///< Offset 1607    Address of I2C Device3 on Link2
                                                    ///< Offset 1609    Address of I2C Device4 on Link2
                                                    ///< Offset 1611    Address of I2C Device5 on Link2
                                                    ///< Offset 1613    Address of I2C Device6 on Link2
                                                    ///< Offset 1615    Address of I2C Device7 on Link2
                                                    ///< Offset 1617    Address of I2C Device8 on Link2
                                                    ///< Offset 1619    Address of I2C Device9 on Link2
                                                    ///< Offset 1621    Address of I2C Device10 on Link2
                                                    ///< Offset 1623    Address of I2C Device11 on Link2
  UINT8    MipiCamLink2I2cDeviceType[12];           ///< Offset 1625    Type of I2C Device0 on Link2
                                                    ///< Offset 1626    Type of I2C Device1 on Link2
                                                    ///< Offset 1627    Type of I2C Device2 on Link2
                                                    ///< Offset 1628    Type of I2C Device3 on Link2
                                                    ///< Offset 1629    Type of I2C Device4 on Link2
                                                    ///< Offset 1630    Type of I2C Device5 on Link2
                                                    ///< Offset 1631    Type of I2C Device6 on Link2
                                                    ///< Offset 1632    Type of I2C Device7 on Link2
                                                    ///< Offset 1633    Type of I2C Device8 on Link2
                                                    ///< Offset 1634    Type of I2C Device9 on Link2
                                                    ///< Offset 1635    Type of I2C Device10 on Link2
                                                    ///< Offset 1636    Type of I2C Device11 on Link2
  UINT8    MipiCamLink2DD_Version;                  ///< Offset 1637    Version of SSDB structure
  UINT8    MipiCamLink2DD_CrdVersion;               ///< Offset 1638    Version of CRD
  UINT8    MipiCamLink2DD_LinkUsed;                 ///< Offset 1639    CSI2 Link used
  UINT8    MipiCamLink2DD_LaneUsed;                 ///< Offset 1640    MIPI-CSI2 Data Lane
  UINT8    MipiCamLink2DD_EepromType;               ///< Offset 1641    EEPROM Type
  UINT8    MipiCamLink2DD_VcmType;                  ///< Offset 1642    VCM Type
  UINT8    MipiCamLink2DD_FlashSupport;             ///< Offset 1643    Flash Support
  UINT8    MipiCamLink2DD_PrivacyLed;               ///< Offset 1644    Privacy LED
  UINT8    MipiCamLink2DD_Degree;                   ///< Offset 1645    Degree
  UINT32   MipiCamLink2DD_Mclk;                     ///< Offset 1646    MCLK
  UINT8    MipiCamLink2DD_ControlLogic;             ///< Offset 1650    Control Logic
  UINT8    MipiCamLink2DD_PmicPosition;             ///< Offset 1651    PMIC Position
  UINT8    MipiCamLink2DD_VoltageRail;              ///< Offset 1652    Voltage Rail
  UINT8    MipiCamLink2DD_PprValue;                 ///< Offset 1653    PPR Value
  UINT8    MipiCamLink2DD_PprUnit;                  ///< Offset 1654    PPR Unit
  // Mipi Cam Link3 options
  UINT8    MipiCamLink3SensorModel;                 ///< Offset 1655    Sensor Model
  UINT8    MipiCamLink3UserHid[9];                  ///< Offset 1656    User defined HID ASCII character 0
                                                    ///< Offset 1664    User defined HID ASCII character 8
  UINT8    MipiCamLink3Pld;                         ///< Offset 1665    Camera Position
  UINT8    MipiCamLink3ModuleName[16];              ///< Offset 1666    Camera Module Name ASCII character 0
                                                    ///< Offset 1681    Camera Module Name ASCII character 15
  UINT8    MipiCamLink3I2cDevicesEnabled;           ///< Offset 1682    Number of I2C devices
  UINT8    MipiCamLink3I2cBus;                      ///< Offset 1683    I2C Serial Bus number
  UINT16   MipiCamLink3I2cAddrDev[12];              ///< Offset 1684    Address of I2C Device0 on Link3
                                                    ///< Offset 1686    Address of I2C Device1 on Link3
                                                    ///< Offset 1688    Address of I2C Device2 on Link3
                                                    ///< Offset 1690    Address of I2C Device3 on Link3
                                                    ///< Offset 1692    Address of I2C Device4 on Link3
                                                    ///< Offset 1694    Address of I2C Device5 on Link3
                                                    ///< Offset 1696    Address of I2C Device6 on Link3
                                                    ///< Offset 1698    Address of I2C Device7 on Link3
                                                    ///< Offset 1700    Address of I2C Device8 on Link3
                                                    ///< Offset 1702    Address of I2C Device9 on Link3
                                                    ///< Offset 1704    Address of I2C Device10 on Link3
                                                    ///< Offset 1706    Address of I2C Device11 on Link3
  UINT8    MipiCamLink3I2cDeviceType[12];           ///< Offset 1708    Type of I2C Device0 on Link3
                                                    ///< Offset 1709    Type of I2C Device1 on Link3
                                                    ///< Offset 1710    Type of I2C Device2 on Link3
                                                    ///< Offset 1711    Type of I2C Device3 on Link3
                                                    ///< Offset 1712    Type of I2C Device4 on Link3
                                                    ///< Offset 1713    Type of I2C Device5 on Link3
                                                    ///< Offset 1714    Type of I2C Device6 on Link3
                                                    ///< Offset 1715    Type of I2C Device7 on Link3
                                                    ///< Offset 1716    Type of I2C Device8 on Link3
                                                    ///< Offset 1717    Type of I2C Device9 on Link3
                                                    ///< Offset 1718    Type of I2C Device10 on Link3
                                                    ///< Offset 1719    Type of I2C Device11 on Link3
  UINT8    MipiCamLink3DD_Version;                  ///< Offset 1720    Version of SSDB structure
  UINT8    MipiCamLink3DD_CrdVersion;               ///< Offset 1721    Version of CRD
  UINT8    MipiCamLink3DD_LinkUsed;                 ///< Offset 1722    CSI2 Link used
  UINT8    MipiCamLink3DD_LaneUsed;                 ///< Offset 1723    MIPI-CSI2 Data Lane
  UINT8    MipiCamLink3DD_EepromType;               ///< Offset 1724    EEPROM Type
  UINT8    MipiCamLink3DD_VcmType;                  ///< Offset 1725    VCM Type
  UINT8    MipiCamLink3DD_FlashSupport;             ///< Offset 1726    Flash Support
  UINT8    MipiCamLink3DD_PrivacyLed;               ///< Offset 1727    Privacy LED
  UINT8    MipiCamLink3DD_Degree;                   ///< Offset 1728    Degree
  UINT32   MipiCamLink3DD_Mclk;                     ///< Offset 1729    MCLK
  UINT8    MipiCamLink3DD_ControlLogic;             ///< Offset 1733    Control Logic
  UINT8    MipiCamLink3DD_PmicPosition;             ///< Offset 1734    PMIC Position
  UINT8    MipiCamLink3DD_VoltageRail;              ///< Offset 1735    Voltage Rail
  UINT8    MipiCamLink3DD_PprValue;                 ///< Offset 1736    PPR Value
  UINT8    MipiCamLink3DD_PprUnit;                  ///< Offset 1737    PPR Unit
  // Mipi Cam Link4 options
  UINT8    MipiCamLink4SensorModel;                 ///< Offset 1738    Sensor Model
  UINT8    MipiCamLink4UserHid[9];                  ///< Offset 1739    User defined HID ASCII character 0
                                                    ///< Offset 1747    User defined HID ASCII character 8
  UINT8    MipiCamLink4Pld;                         ///< Offset 1748    Camera Position
  UINT8    MipiCamLink4ModuleName[16];              ///< Offset 1749    Camera Module Name ASCII character 0
                                                    ///< Offset 1764    Camera Module Name ASCII character 15
  UINT8    MipiCamLink4I2cDevicesEnabled;           ///< Offset 1765    Number of I2C devices
  UINT8    MipiCamLink4I2cBus;                      ///< Offset 1766    I2C Serial Bus number
  UINT16   MipiCamLink4I2cAddrDev[12];              ///< Offset 1767    Address of I2C Device0 on Link4
                                                    ///< Offset 1769    Address of I2C Device1 on Link4
                                                    ///< Offset 1771    Address of I2C Device2 on Link4
                                                    ///< Offset 1773    Address of I2C Device3 on Link4
                                                    ///< Offset 1775    Address of I2C Device4 on Link4
                                                    ///< Offset 1777    Address of I2C Device5 on Link4
                                                    ///< Offset 1779    Address of I2C Device6 on Link4
                                                    ///< Offset 1781    Address of I2C Device7 on Link4
                                                    ///< Offset 1783    Address of I2C Device8 on Link4
                                                    ///< Offset 1785    Address of I2C Device9 on Link4
                                                    ///< Offset 1787    Address of I2C Device10 on Link4
                                                    ///< Offset 1789    Address of I2C Device11 on Link4
  UINT8    MipiCamLink4I2cDeviceType[12];           ///< Offset 1791    Type of I2C Device0 on Link4
                                                    ///< Offset 1792    Type of I2C Device1 on Link4
                                                    ///< Offset 1793    Type of I2C Device2 on Link4
                                                    ///< Offset 1794    Type of I2C Device3 on Link4
                                                    ///< Offset 1795    Type of I2C Device4 on Link4
                                                    ///< Offset 1796    Type of I2C Device5 on Link4
                                                    ///< Offset 1797    Type of I2C Device6 on Link4
                                                    ///< Offset 1798    Type of I2C Device7 on Link4
                                                    ///< Offset 1799    Type of I2C Device8 on Link4
                                                    ///< Offset 1800    Type of I2C Device9 on Link4
                                                    ///< Offset 1801    Type of I2C Device10 on Link4
                                                    ///< Offset 1802    Type of I2C Device11 on Link4
  UINT8    MipiCamLink4DD_Version;                  ///< Offset 1803    Version of SSDB structure
  UINT8    MipiCamLink4DD_CrdVersion;               ///< Offset 1804    Version of CRD
  UINT8    MipiCamLink4DD_LinkUsed;                 ///< Offset 1805    CSI2 Link used
  UINT8    MipiCamLink4DD_LaneUsed;                 ///< Offset 1806    MIPI-CSI2 Data Lane
  UINT8    MipiCamLink4DD_EepromType;               ///< Offset 1807    EEPROM Type
  UINT8    MipiCamLink4DD_VcmType;                  ///< Offset 1808    VCM Type
  UINT8    MipiCamLink4DD_FlashSupport;             ///< Offset 1809    Flash Support
  UINT8    MipiCamLink4DD_PrivacyLed;               ///< Offset 1810    Privacy LED
  UINT8    MipiCamLink4DD_Degree;                   ///< Offset 1811    Degree
  UINT32   MipiCamLink4DD_Mclk;                     ///< Offset 1812    MCLK
  UINT8    MipiCamLink4DD_ControlLogic;             ///< Offset 1816    Control Logic
  UINT8    MipiCamLink4DD_PmicPosition;             ///< Offset 1817    PMIC Position
  UINT8    MipiCamLink4DD_VoltageRail;              ///< Offset 1818    Voltage Rail
  UINT8    MipiCamLink4DD_PprValue;                 ///< Offset 1819    PPR Value
  UINT8    MipiCamLink4DD_PprUnit;                  ///< Offset 1820    PPR Unit
  // Mipi Cam Link5 options
  UINT8    MipiCamLink5SensorModel;                 ///< Offset 1821    Sensor Model
  UINT8    MipiCamLink5UserHid[9];                  ///< Offset 1822    User defined HID ASCII character 0
                                                    ///< Offset 1830    User defined HID ASCII character 8
  UINT8    MipiCamLink5Pld;                         ///< Offset 1831    Camera Position
  UINT8    MipiCamLink5ModuleName[16];              ///< Offset 1832    Camera Module Name ASCII character 0
                                                    ///< Offset 1847    Camera Module Name ASCII character 15
  UINT8    MipiCamLink5I2cDevicesEnabled;           ///< Offset 1848    Number of I2C devices
  UINT8    MipiCamLink5I2cBus;                      ///< Offset 1849    I2C Serial Bus number
  UINT16   MipiCamLink5I2cAddrDev[12];              ///< Offset 1850    Address of I2C Device0 on Link5
                                                    ///< Offset 1852    Address of I2C Device1 on Link5
                                                    ///< Offset 1854    Address of I2C Device2 on Link5
                                                    ///< Offset 1856    Address of I2C Device3 on Link5
                                                    ///< Offset 1858    Address of I2C Device4 on Link5
                                                    ///< Offset 1860    Address of I2C Device5 on Link5
                                                    ///< Offset 1862    Address of I2C Device6 on Link5
                                                    ///< Offset 1864    Address of I2C Device7 on Link5
                                                    ///< Offset 1866    Address of I2C Device8 on Link5
                                                    ///< Offset 1868    Address of I2C Device9 on Link5
                                                    ///< Offset 1870    Address of I2C Device10 on Link5
                                                    ///< Offset 1872    Address of I2C Device11 on Link5
  UINT8    MipiCamLink5I2cDeviceType[12];           ///< Offset 1874    Type of I2C Device0 on Link5
                                                    ///< Offset 1875    Type of I2C Device1 on Link5
                                                    ///< Offset 1876    Type of I2C Device2 on Link5
                                                    ///< Offset 1877    Type of I2C Device3 on Link5
                                                    ///< Offset 1878    Type of I2C Device4 on Link5
                                                    ///< Offset 1879    Type of I2C Device5 on Link5
                                                    ///< Offset 1880    Type of I2C Device6 on Link5
                                                    ///< Offset 1881    Type of I2C Device7 on Link5
                                                    ///< Offset 1882    Type of I2C Device8 on Link5
                                                    ///< Offset 1883    Type of I2C Device9 on Link5
                                                    ///< Offset 1884    Type of I2C Device10 on Link5
                                                    ///< Offset 1885    Type of I2C Device11 on Link5
  UINT8    MipiCamLink5DD_Version;                  ///< Offset 1886    Version of SSDB structure
  UINT8    MipiCamLink5DD_CrdVersion;               ///< Offset 1887    Version of CRD
  UINT8    MipiCamLink5DD_LinkUsed;                 ///< Offset 1888    CSI2 Link used
  UINT8    MipiCamLink5DD_LaneUsed;                 ///< Offset 1889    MIPI-CSI2 Data Lane
  UINT8    MipiCamLink5DD_EepromType;               ///< Offset 1890    EEPROM Type
  UINT8    MipiCamLink5DD_VcmType;                  ///< Offset 1891    VCM Type
  UINT8    MipiCamLink5DD_FlashSupport;             ///< Offset 1892    Flash Support
  UINT8    MipiCamLink5DD_PrivacyLed;               ///< Offset 1893    Privacy LED
  UINT8    MipiCamLink5DD_Degree;                   ///< Offset 1894    Degree
  UINT32   MipiCamLink5DD_Mclk;                     ///< Offset 1895    MCLK
  UINT8    MipiCamLink5DD_ControlLogic;             ///< Offset 1899    Control Logic
  UINT8    MipiCamLink5DD_PmicPosition;             ///< Offset 1900    PMIC Position
  UINT8    MipiCamLink5DD_VoltageRail;              ///< Offset 1901    Voltage Rail
  UINT8    MipiCamLink5DD_PprValue;                 ///< Offset 1902    PPR Value
  UINT8    MipiCamLink5DD_PprUnit;                  ///< Offset 1903    PPR Unit
  UINT8    Reserved16[5];                           ///< Offset 1904:1908
  UINT8    PciDelayOptimizationEcr;                 ///< Offset 1909
  UINT8    I2SC;                                    ///< Offset 1910    HD Audio I2S Codec Selection
  UINT32   I2SI;                                    ///< Offset 1911    HD Audio I2S Codec Interrupt Pin
  UINT8    I2SB;                                    ///< Offset 1915    HD Audio I2S Codec Connection to I2C bus controller instance (I2C[0-5])
  UINT8    OemDesignVariable0;                      ///< Offset 1916    DPTF Oem Design Variables
  UINT8    OemDesignVariable1;                      ///< Offset 1917    DPTF Oem Design Variables
  UINT8    OemDesignVariable2;                      ///< Offset 1918    DPTF Oem Design Variables
  UINT8    OemDesignVariable3;                      ///< Offset 1919    DPTF Oem Design Variables
  UINT8    OemDesignVariable4;                      ///< Offset 1920    DPTF Oem Design Variables
  UINT8    OemDesignVariable5;                      ///< Offset 1921    DPTF Oem Design Variables
  UINT32   UsbTypeCOpBaseAddr;                      ///< Offset 1922    USB Type C Opregion base address
  UINT8    Reserved17[1];                           ///< Offset 1926:1926
  UINT8    WirelessCharging;                        ///< Offset 1927    WirelessCharging
  // RTD3 Settings
  #ifdef TESTMENU_FLAG
  UINT8    TestRtd3PcieRootports;                   ///< Offset 1928    Test menu Pcie Root Ports RTD3 Configuration
  UINT8    TestRtd3Usb;                             ///< Offset 1929    Test menu Usb RTD3 Configuration
  UINT8    TestRtd3AudioCodec;                      ///< Offset 1930    Test menu AudioCodec RTD3 Configuration
  UINT8    TestRtd3HdAudio;                         ///< Offset 1931    Test menu HD Audio RTD3 Configuration
  UINT8    TestRtd3TouchPanel;                      ///< Offset 1932    Test menu Touch Panel RTD3 Configuration
  UINT8    Reserved18[1];                           ///< Offset 1933:1933
  #else
  UINT8    Reserved19[6];                           ///< Offset 1928:1933
  #endif
  UINT32   HdaDspPpModuleMask;                      ///< Offset 1934    HD-Audio DSP Post-Processing Module Mask
  UINT64   HdaDspPpModCustomGuid1Low;               ///< Offset 1938    HDA PP module custom GUID 1 - first 64bit  [0-63]
  UINT64   HdaDspPpModCustomGuid1High;              ///< Offset 1946    HDA PP module custom GUID 1 - second 64bit [64-127]
  UINT64   HdaDspPpModCustomGuid2Low;               ///< Offset 1954    HDA PP module custom GUID 2 - first 64bit  [0-63]
  UINT64   HdaDspPpModCustomGuid2High;              ///< Offset 1962    HDA PP module custom GUID 2 - second 64bit [64-127]
  UINT64   HdaDspPpModCustomGuid3Low;               ///< Offset 1970    HDA PP module custom GUID 3 - first 64bit  [0-63]
  UINT64   HdaDspPpModCustomGuid3High;              ///< Offset 1978    HDA PP module custom GUID 3 - second 64bit [64-127]
  UINT8    HidEventFilterEnable;                    ///< Offset 1986    HID Event Filter Driver enable
  UINT8    XdciFnEnable;                            ///< Offset 1987    XDCI Enable/Disable status
  UINT8    WrdsWiFiSarEnable;                       ///< Offset 1988    WrdsWiFiSarEnable
  UINT8    WrdsWiFiSarTxPowerSet1Limit1;            ///< Offset 1989    WrdsWiFiSarTxPowerSet1Limit1
  UINT8    WrdsWiFiSarTxPowerSet1Limit2;            ///< Offset 1990    WrdsWiFiSarTxPowerSet1Limit2
  UINT8    WrdsWiFiSarTxPowerSet1Limit3;            ///< Offset 1991    WrdsWiFiSarTxPowerSet1Limit3
  UINT8    WrdsWiFiSarTxPowerSet1Limit4;            ///< Offset 1992    WrdsWiFiSarTxPowerSet1Limit4
  UINT8    WrdsWiFiSarTxPowerSet1Limit5;            ///< Offset 1993    WrdsWiFiSarTxPowerSet1Limit5
  UINT8    WrdsWiFiSarTxPowerSet1Limit6;            ///< Offset 1994    WrdsWiFiSarTxPowerSet1Limit6
  UINT8    WrdsWiFiSarTxPowerSet1Limit7;            ///< Offset 1995    WrdsWiFiSarTxPowerSet1Limit7
  UINT8    WrdsWiFiSarTxPowerSet1Limit8;            ///< Offset 1996    WrdsWiFiSarTxPowerSet1Limit8
  UINT8    WrdsWiFiSarTxPowerSet1Limit9;            ///< Offset 1997    WrdsWiFiSarTxPowerSet1Limit9
  UINT8    WrdsWiFiSarTxPowerSet1Limit10;           ///< Offset 1998    WrdsWiFiSarTxPowerSet1Limit10
  UINT8    EnableVoltageMargining;                  ///< Offset 1999    Enable Voltage Margining
  UINT16   DStateHSPort;                            ///< Offset 2000    D-State for xHCI HS port(BIT0:USB HS Port0 ~ BIT15:USB HS Port15)
  UINT16   DStateSSPort;                            ///< Offset 2002    D-State for xHCI SS port(BIT0:USB SS Port0 ~ BIT15:USB SS Port15)
  UINT8    DStateSataPort;                          ///< Offset 2004    D-State for SATA port(BIT0:SATA Port0 ~ BIT7:SATA Port7)
  UINT8    EwrdWiFiDynamicSarEnable;                ///< Offset 2005    EwrdWiFiDynamicSarEnable
  UINT8    EwrdWiFiDynamicSarRangeSets;             ///< Offset 2006    EwrdWiFiDynamicSarRangeSets
  UINT8    EwrdWiFiSarTxPowerSet2Limit1;            ///< Offset 2007    EwrdWiFiSarTxPowerSet2Limit1
  UINT8    EwrdWiFiSarTxPowerSet2Limit2;            ///< Offset 2008    EwrdWiFiSarTxPowerSet2Limit2
  UINT8    EwrdWiFiSarTxPowerSet2Limit3;            ///< Offset 2009    EwrdWiFiSarTxPowerSet2Limit3
  UINT8    EwrdWiFiSarTxPowerSet2Limit4;            ///< Offset 2010    EwrdWiFiSarTxPowerSet2Limit4
  UINT8    EwrdWiFiSarTxPowerSet2Limit5;            ///< Offset 2011    EwrdWiFiSarTxPowerSet2Limit5
  UINT8    EwrdWiFiSarTxPowerSet2Limit6;            ///< Offset 2012    EwrdWiFiSarTxPowerSet2Limit6
  UINT8    EwrdWiFiSarTxPowerSet2Limit7;            ///< Offset 2013    EwrdWiFiSarTxPowerSet2Limit7
  UINT8    EwrdWiFiSarTxPowerSet2Limit8;            ///< Offset 2014    EwrdWiFiSarTxPowerSet2Limit8
  UINT8    EwrdWiFiSarTxPowerSet2Limit9;            ///< Offset 2015    EwrdWiFiSarTxPowerSet2Limit9
  UINT8    EwrdWiFiSarTxPowerSet2Limit10;           ///< Offset 2016    EwrdWiFiSarTxPowerSet2Limit10
  UINT8    EwrdWiFiSarTxPowerSet3Limit1;            ///< Offset 2017    EwrdWiFiSarTxPowerSet3Limit1
  UINT8    EwrdWiFiSarTxPowerSet3Limit2;            ///< Offset 2018    EwrdWiFiSarTxPowerSet3Limit2
  UINT8    EwrdWiFiSarTxPowerSet3Limit3;            ///< Offset 2019    EwrdWiFiSarTxPowerSet3Limit3
  UINT8    EwrdWiFiSarTxPowerSet3Limit4;            ///< Offset 2020    EwrdWiFiSarTxPowerSet3Limit4
  UINT8    EwrdWiFiSarTxPowerSet3Limit5;            ///< Offset 2021    EwrdWiFiSarTxPowerSet3Limit5
  UINT8    EwrdWiFiSarTxPowerSet3Limit6;            ///< Offset 2022    EwrdWiFiSarTxPowerSet3Limit6
  UINT8    EwrdWiFiSarTxPowerSet3Limit7;            ///< Offset 2023    EwrdWiFiSarTxPowerSet3Limit7
  UINT8    EwrdWiFiSarTxPowerSet3Limit8;            ///< Offset 2024    EwrdWiFiSarTxPowerSet3Limit8
  UINT8    EwrdWiFiSarTxPowerSet3Limit9;            ///< Offset 2025    EwrdWiFiSarTxPowerSet3Limit9
  UINT8    EwrdWiFiSarTxPowerSet3Limit10;           ///< Offset 2026    EwrdWiFiSarTxPowerSet3Limit10
  UINT8    EwrdWiFiSarTxPowerSet4Limit1;            ///< Offset 2027    EwrdWiFiSarTxPowerSet4Limit1
  UINT8    EwrdWiFiSarTxPowerSet4Limit2;            ///< Offset 2028    EwrdWiFiSarTxPowerSet4Limit2
  UINT8    EwrdWiFiSarTxPowerSet4Limit3;            ///< Offset 2029    EwrdWiFiSarTxPowerSet4Limit3
  UINT8    EwrdWiFiSarTxPowerSet4Limit4;            ///< Offset 2030    EwrdWiFiSarTxPowerSet4Limit4
  UINT8    EwrdWiFiSarTxPowerSet4Limit5;            ///< Offset 2031    EwrdWiFiSarTxPowerSet4Limit5
  UINT8    EwrdWiFiSarTxPowerSet4Limit6;            ///< Offset 2032    EwrdWiFiSarTxPowerSet4Limit6
  UINT8    EwrdWiFiSarTxPowerSet4Limit7;            ///< Offset 2033    EwrdWiFiSarTxPowerSet4Limit7
  UINT8    EwrdWiFiSarTxPowerSet4Limit8;            ///< Offset 2034    EwrdWiFiSarTxPowerSet4Limit8
  UINT8    EwrdWiFiSarTxPowerSet4Limit9;            ///< Offset 2035    EwrdWiFiSarTxPowerSet4Limit9
  UINT8    EwrdWiFiSarTxPowerSet4Limit10;           ///< Offset 2036    EwrdWiFiSarTxPowerSet4Limit10
  UINT8    WgdsWiFiSarDeltaGroup1PowerMax1;         ///< Offset 2037    WgdsWiFiSarDeltaGroup1PowerMax1
  UINT8    WgdsWiFiSarDeltaGroup1PowerChainA1;      ///< Offset 2038    WgdsWiFiSarDeltaGroup1PowerChainA1
  UINT8    WgdsWiFiSarDeltaGroup1PowerChainB1;      ///< Offset 2039    WgdsWiFiSarDeltaGroup1PowerChainB1
  UINT8    WgdsWiFiSarDeltaGroup1PowerMax2;         ///< Offset 2040    WgdsWiFiSarDeltaGroup1PowerMax2
  UINT8    WgdsWiFiSarDeltaGroup1PowerChainA2;      ///< Offset 2041    WgdsWiFiSarDeltaGroup1PowerChainA2
  UINT8    WgdsWiFiSarDeltaGroup1PowerChainB2;      ///< Offset 2042    WgdsWiFiSarDeltaGroup1PowerChainB2
  UINT8    WgdsWiFiSarDeltaGroup2PowerMax1;         ///< Offset 2043    WgdsWiFiSarDeltaGroup2PowerMax1
  UINT8    WgdsWiFiSarDeltaGroup2PowerChainA1;      ///< Offset 2044    WgdsWiFiSarDeltaGroup2PowerChainA1
  UINT8    WgdsWiFiSarDeltaGroup2PowerChainB1;      ///< Offset 2045    WgdsWiFiSarDeltaGroup2PowerChainB1
  UINT8    WgdsWiFiSarDeltaGroup2PowerMax2;         ///< Offset 2046    WgdsWiFiSarDeltaGroup2PowerMax2
  UINT8    WgdsWiFiSarDeltaGroup2PowerChainA2;      ///< Offset 2047    WgdsWiFiSarDeltaGroup2PowerChainA2
  UINT8    WgdsWiFiSarDeltaGroup2PowerChainB2;      ///< Offset 2048    WgdsWiFiSarDeltaGroup2PowerChainB2
  UINT8    WgdsWiFiSarDeltaGroup3PowerMax1;         ///< Offset 2049    WgdsWiFiSarDeltaGroup3PowerMax1
  UINT8    WgdsWiFiSarDeltaGroup3PowerChainA1;      ///< Offset 2050    WgdsWiFiSarDeltaGroup3PowerChainA1
  UINT8    WgdsWiFiSarDeltaGroup3PowerChainB1;      ///< Offset 2051    WgdsWiFiSarDeltaGroup3PowerChainB1
  UINT8    WgdsWiFiSarDeltaGroup3PowerMax2;         ///< Offset 2052    WgdsWiFiSarDeltaGroup3PowerMax2
  UINT8    WgdsWiFiSarDeltaGroup3PowerChainA2;      ///< Offset 2053    WgdsWiFiSarDeltaGroup3PowerChainA2
  UINT8    WgdsWiFiSarDeltaGroup3PowerChainB2;      ///< Offset 2054    WgdsWiFiSarDeltaGroup3PowerChainB2
  UINT8    Reserved20[41];                          ///< Offset 2055:2095
  // Reserved for Groups 4 to 9, each needs 6 bytes and total 36 bytes reserved
  UINT8    WiFiDynamicSarAntennaACurrentSet;        ///< Offset 2096    WiFiDynamicSarAntennaACurrentSet
  UINT8    WiFiDynamicSarAntennaBCurrentSet;        ///< Offset 2097    WiFiDynamicSarAntennaBCurrentSet
  UINT8    BluetoothSar;                            ///< Offset 2098    BluetoothSar
  UINT8    BluetoothSarBr;                          ///< Offset 2099    BluetoothSarBr
  UINT8    BluetoothSarEdr2;                        ///< Offset 2100    BluetoothSarEdr2
  UINT8    BluetoothSarEdr3;                        ///< Offset 2101    BluetoothSarEdr3
  UINT8    BluetoothSarLe;                          ///< Offset 2102    BluetoothSarLe
  UINT8    BluetoothSarLe2Mhz;                      ///< Offset 2103    BluetoothSarLe2Mhz
  UINT8    BluetoothSarLeLr;                        ///< Offset 2104    BluetoothSarLeLr
  UINT8    Reserved21[2];                           ///< Offset 2105:2106
  // Reserved for Bluetooth Sar future use
  UINT8    AntennaDiversity;                        ///< Offset 2107    AntennaDiversity
  UINT8    CoExistenceManager;                      ///< Offset 2108    CoExistenceManager
  UINT8    RunTimeVmControl;                        ///< Offset 2109    RunTime VM Control
  //
  //Feature Specific Data Bits
  //
  UINT8    UsbTypeCSupport;                         ///< Offset 2110    USB Type C Supported
  UINT32   HebcValue;                               ///< Offset 2111    HebcValue
  UINT8    PcdBatteryPresent;                       ///< Offset 2115    Battery Present - Bit0: Real Battery is supported on this platform. Bit1: Virtual Battery is supported on this platform.
  UINT8    PcdTsOnDimmTemperature;                  ///< Offset 2116    TS-on-DIMM temperature
  UINT8    Reserved22[2];                           ///< Offset 2117:2118
  UINT8    PcdRealBattery1Control;                  ///< Offset 2119    Real Battery 1 Control
  UINT8    PcdRealBattery2Control;                  ///< Offset 2120    Real Battery 2 Control
  UINT8    PcdMipiCamSensor;                        ///< Offset 2121    Mipi Camera Sensor
  UINT8    PcdNCT6776FCOM;                          ///< Offset 2122    NCT6776F COM
  UINT8    PcdNCT6776FSIO;                          ///< Offset 2123    NCT6776F SIO
  UINT8    PcdNCT6776FHWMON;                        ///< Offset 2124    NCT6776F HWMON
  UINT8    PcdH8S2113SIO;                           ///< Offset 2125    H8S2113 SIO
  UINT8    PcdZPoddConfig;                          ///< Offset 2126    ZPODD
  UINT8    PcdRGBCameraAdr;                         ///< Offset 2127    RGB Camera Address
  UINT8    PcdDepthCameraAdr;                       ///< Offset 2128    Depth Camera Addresy
  UINT32   PcdSmcRuntimeSciPin;                     ///< Offset 2129    SMC Runtime Sci Pin
  UINT8    PcdConvertableDockSupport;               ///< Offset 2133    Convertable Dock Support
  UINT8    PcdEcHotKeyF3Support;                    ///< Offset 2134    Ec Hotkey F3 Support
  UINT8    PcdEcHotKeyF4Support;                    ///< Offset 2135    Ec Hotkey F4 Support
  UINT8    PcdEcHotKeyF5Support;                    ///< Offset 2136    Ec Hotkey F5 Support
  UINT8    PcdEcHotKeyF6Support;                    ///< Offset 2137    Ec Hotkey F6 Support
  UINT8    PcdEcHotKeyF7Support;                    ///< Offset 2138    Ec Hotkey F7 Support
  UINT8    PcdEcHotKeyF8Support;                    ///< Offset 2139    Ec Hotkey F8 Support
  UINT8    PcdVirtualButtonVolumeUpSupport;         ///< Offset 2140    Virtual Button Volume Up Support
  UINT8    PcdVirtualButtonVolumeDownSupport;       ///< Offset 2141    Virtual Button Volume Down Support
  UINT8    PcdVirtualButtonHomeButtonSupport;       ///< Offset 2142    Virtual Button Home Button Support
  UINT8    PcdVirtualButtonRotationLockSupport;     ///< Offset 2143    Virtual Button Rotation Lock Support
  UINT8    PcdSlateModeSwitchSupport;               ///< Offset 2144    Slate Mode Switch Support
  UINT8    PcdVirtualGpioButtonSupport;             ///< Offset 2145    Virtual Button Support
  UINT8    PcdAcDcAutoSwitchSupport;                ///< Offset 2146    Ac Dc Auto Switch Support
  UINT32   PcdPmPowerButtonGpioPin;                 ///< Offset 2147    Pm Power Button Gpio Pin
  UINT8    PcdAcpiEnableAllButtonSupport;           ///< Offset 2151    Acpi Enable All Button Support
  UINT8    PcdAcpiHidDriverButtonSupport;           ///< Offset 2152    Acpi Hid Driver Button Support
  UINT8    DisplayDepthLowerLimit;                  ///< Offset 2153    DPTF Display Depth Lower Limit in percent
  UINT8    DisplayDepthUpperLimit;                  ///< Offset 2154    DPTF Display Depth Upper Limit in percent
  UINT8    ThermalSamplingPeriodWrls;               ///< Offset 2155    ThermalSamplingPeriodWrls
  UINT32   EcLowPowerModeGpioPin;                   ///< Offset 2156    EcLowPowerModeGpioPin
  UINT32   EcSmiGpioPin;                            ///< Offset 2160    EcSmiGpioPin
  UINT32   VirtualBatterySwitchGpioPin;             ///< Offset 2164    Virtual Battery Switch Gpio Pin
  //
  // UCMC setup option, GPIO Pad
  //
  UINT8    UCMS;                                    ///< Offset 2168    Option to select UCSI/UCMC device
  UINT32   UcmcPort1Gpio;                           ///< Offset 2169    Gpio for UCMC Port 1 Interrupt
  UINT32   UcmcPort2Gpio;                           ///< Offset 2173    Gpio for UCMC Port 2 Interrupt
  UINT8    Reserved23[22];                          ///< Offset 2177:2198
  UINT8    EnablePchFivrParticipant;                ///< Offset 2199    EnablePchFivrParticipant
  UINT8    Reserved24[5];                           ///< Offset 2200:2204
  UINT8    Ufp2DfpGlobalFlag;                       ///< Offset 2205    Upstream Facing port or Downstream Facing port Global Flag from LPC EC
  UINT8    Ufp2DfpUsbPort;                          ///< Offset 2206    Upstream Facing port or Downstream Facing port number from LPC EC
  UINT8    DbcGlobalFlag;                           ///< Offset 2207    Debug Mode Global Flag from LPC EC
  UINT8    DbcUsbPort;                              ///< Offset 2208    Debug Mode USB Port Number from LPC EC
  UINT8    TotalTypeCPorts;                         ///< Offset 2209    Total Number of type C ports that are supported by platform
  UINT8    UsbTypeCPort1;                           ///< Offset 2210    Type C Connector 1  Port mapping within the controller the port exposed
  UINT8    UsbTypeCPort1Pch;                        ///< Offset 2211    Type C Connector 1  Port mapping within the PCH controller (If Split mode supported)
  UINT8    UsbCPort1Proterties;                     ///< Offset 2212    Type C Connector 1  Portperties Split Support/Controller(PCH/TBT/CPU)/Root port (vaild for TBT)
  UINT8    UsbTypeCPort2;                           ///< Offset 2213    Type C Connector 2  Port mapping within the controller the port exposed
  UINT8    UsbTypeCPort2Pch;                        ///< Offset 2214    Type C Connector 2  Port mapping within the PCH controller (If Split mode supported)
  UINT8    UsbCPort2Proterties;                     ///< Offset 2215    Type C Connector 2  Portperties Split Support/Controller(PCH/TBT/CPU)/Root port (vaild for TBT)
  UINT8    UsbTypeCPort3;                           ///< Offset 2216    Type C Connector 3  Port mapping within the controller the port exposed
  UINT8    UsbTypeCPort3Pch;                        ///< Offset 2217    Type C Connector 3  Port mapping within the PCH controller (If Split mode supported)
  UINT8    UsbCPort3Proterties;                     ///< Offset 2218    Type C Connector 3  Portperties Split Support/Controller(PCH/TBT/CPU)/Root port (vaild for TBT)
  UINT8    UsbTypeCPort4;                           ///< Offset 2219    Type C Connector 4  Port mapping within the controller the port exposed
  UINT8    UsbTypeCPort4Pch;                        ///< Offset 2220    Type C Connector 4  Port mapping within the PCH controller (If Split mode supported)
  UINT8    UsbCPort4Proterties;                     ///< Offset 2221    Type C Connector 4  Portperties Split Support/Controller(PCH/TBT/CPU)/Root port (vaild for TBT)
  UINT8    UsbTypeCPort5;                           ///< Offset 2222    Type C Connector 5  Port mapping within the controller the port exposed
  UINT8    UsbTypeCPort5Pch;                        ///< Offset 2223    Type C Connector 5  Port mapping within the PCH controller (If Split mode supported)
  UINT8    UsbCPort5Proterties;                     ///< Offset 2224    Type C Connector 5  Portperties Split Support/Controller(PCH/TBT/CPU)/Root port (vaild for TBT)
  UINT8    UsbTypeCPort6;                           ///< Offset 2225    Type C Connector 6  Port mapping within the controller the port exposed
  UINT8    UsbTypeCPort6Pch;                        ///< Offset 2226    Type C Connector 6  Port mapping within the PCH controller (If Split mode supported)
  UINT8    UsbCPort6Proterties;                     ///< Offset 2227    Type C Connector 6  Portperties Split Support/Controller(PCH/TBT/CPU)/Root port (vaild for TBT)
  UINT8    SystemTimeAndAlarmSource;                ///< Offset 2228    Select source for System time and alarm
  UINT8    WwanRtd3Option;                          ///< Offset 2229    WWAN RTD3 options
  UINT32   WwanPerstGpio;                           ///< Offset 2230    WWAN PERST Gpio pin
  UINT8    WwanPerstGpioPolarity;                   ///< Offset 2234    WWAN PERST Gpio polarity
  UINT32   PcieSlot1WakeGpio;                       ///< Offset 2235    Pcie Slot Wake Gpio pin
  UINT8    PcieSlot1RpNumber;                       ///< Offset 2239    Pcie Slot Root Port Number
  UINT8    PowermeterDeviceEnable;                  ///< Offset 2240    Enable PowerMeter
  UINT32   PcieSlot2WakeGpio;                       ///< Offset 2241    Pcie Slot 2 Wake Gpio pin
  UINT8    PcieSlot2RpNumber;                       ///< Offset 2245    Pcie Slot 2 Root Port Number
  UINT32   WwanFullCardPowerOffGpio;                ///< Offset 2246    WWAN Full card power off gpio pin
  UINT8    WwanFullCardPowerOffGpioPolarity;        ///< Offset 2250    WWAN Full card power off gpio pin polarity
  UINT32   WwanBbrstGpio;                           ///< Offset 2251    WWAN BBRST Gpio pin
  UINT8    WwanBbrstGpioPolarity;                   ///< Offset 2255    WWAN BBRST Gpio pin polarity
  UINT32   WwanWakeGpio;                            ///< Offset 2256    WWAN Wake Gpio pin
  UINT32   TouchpanelPowerEnableGpio;               ///< Offset 2260    Gpio for touchPaneL 0 power enable
  UINT32   TouchpanelPowerRstGpio;                  ///< Offset 2264    Gpio for touchPaneL 0 Reset
  UINT32   Touchpanel1IrqGpio;                      ///< Offset 2268    Gpio for touchPaneL 1 Interrupt
  UINT32   Touchpanel1PowerEnableGpio;              ///< Offset 2272    Gpio for touchPaneL 1 power enable
  UINT32   Touchpanel1PowerRstGpio;                 ///< Offset 2276    Gpio for touchPaneL 1 Reset
  UINT8    TouchpadIrqGpioPolarity;                 ///< Offset 2280    TouchPaD Interrupt Gpio pin polarity
  UINT8    TouchpanelIrqGpioPolarity;               ///< Offset 2281    TouchPaneL 0 Interrupt Gpio pin polarity
  UINT8    TouchpanelPowerEnableGpioPolarity;       ///< Offset 2282    TouchPaneL 0 power enable Gpio pin polarity
  UINT8    TouchpanelPowerRstGpioPolarity;          ///< Offset 2283    TouchPaneL 0 Reset Gpio pin polarity
  UINT8    Touchpanel1IrqGpioPolarity;              ///< Offset 2284    TouchPaneL 1 Interrupt Gpio pin polarity
  UINT8    Touchpanel1PowerEnableGpioPolarity;      ///< Offset 2285    TouchPaneL 1 power enable Gpio pin polarity
  UINT8    Touchpanel1PowerRstGpioPolarity;         ///< Offset 2286    TouchPaneL 1 Reset Gpio pin polarity
  UINT32   PcieSlot1PowerEnableGpio;                ///< Offset 2287    Pcie Slot 1 Power Enable Gpio pin
  UINT8    PcieSlot1PowerEnableGpioPolarity;        ///< Offset 2291    Pcie Slot 1 Power Enable Gpio pin polarity
  UINT32   PcieSlot1RstGpio;                        ///< Offset 2292    Pcie Slot 1 Rest Gpio pin
  UINT8    PcieSlot1RstGpioPolarity;                ///< Offset 2296    Pcie Slot 1 Rest Gpio pin polarity
  UINT32   PcieSlot2PowerEnableGpio;                ///< Offset 2297    Pcie Slot 2 Power Enable Gpio pin
  UINT8    PcieSlot2PowerEnableGpioPolarity;        ///< Offset 2301    Pcie Slot 2 Power Enable Gpio pin polarity
  UINT32   PcieSlot2RstGpio;                        ///< Offset 2302    Pcie Slot 2 Rest Gpio pin
  UINT8    PcieSlot2RstGpioPolarity;                ///< Offset 2306    Pcie Slot 2 Rest Gpio pin polarity
  UINT32   WlanWakeGpio;                            ///< Offset 2307    WLAN Wake Gpio pin
  UINT32   SataPortPowerEnableGpio;                 ///< Offset 2311    Sata port Power Enable Gpio pin
  UINT8    SataPortPowerEnableGpioPolarity;         ///< Offset 2315    Sata port Power Enable Gpio pin polarity
  UINT32   PchM2SsdPowerEnableGpio;                 ///< Offset 2316    Pch M.2 SSD Power Enable Gpio pin
  UINT8    PchM2SsdPowerEnableGpioPolarity;         ///< Offset 2320    Pch M.2 SSD Power Enable Gpio pin polarity
  UINT32   PchM2SsdRstGpio;                         ///< Offset 2321    Pch M.2 SSD Reset Gpio pin
  UINT8    PchM2SsdRstGpioPolarity;                 ///< Offset 2325    Pch M.2 SSD Reset Gpio pin polarity
  UINT32   M2Ssd2PowerEnableGpio;                   ///< Offset 2326    PCIe x4 M.2 SSD Power Enable Gpio pin
  UINT8    M2Ssd2PowerEnableGpioPolarity;           ///< Offset 2330    PCIe x4 M.2 SSD Power Enable Gpio pin polarity
  UINT32   M2Ssd2RstGpio;                           ///< Offset 2331    PCIe x4 M.2 SSD Reset Gpio pin
  UINT8    M2Ssd2RstGpioPolarity;                   ///< Offset 2335    PCIe x4 M.2 SSD Reset Gpio pin polarity
  UINT8    SdevXhciInterfaceNumber1;                ///< Offset 2336    SDEV xHCI Interface Number for device 1
  UINT8    SdevXhciInterfaceNumber2;                ///< Offset 2337    SDEV xHCI Interface Number for device 2
  UINT8    SdevXhciRootPortNumber1;                 ///< Offset 2338    SDEV xHCI Root Port Number for device 1
  UINT8    SdevXhciRootPortNumber2;                 ///< Offset 2339    SDEV xHCI Root Port Number for device 2
  UINT8    SCS0;                                    ///< Offset 2340    SPI0 ChipSelect 0 Device enabled
  UINT8    SCS1;                                    ///< Offset 2341    SPI0 ChipSelect 1 Device enabled
  UINT8    SCS2;                                    ///< Offset 2342    SPI1 ChipSelect 0 Device enabled
  UINT8    SCS3;                                    ///< Offset 2343    SPI1 ChipSelect 1 Device enabled
  UINT8    SCS4;                                    ///< Offset 2344    SPI2 ChipSelect 0 Device enabled
  UINT8    SCS5;                                    ///< Offset 2345    SPI2 ChipSelect 1 Device enabled
  //
  // Generic Buttons Device policies - Begin
  //
  UINT32   HomeButtonInterrupt;                     ///< Offset 2346
  UINT32   VolumeUpButtonInterrupt;                 ///< Offset 2350
  UINT32   VolumeDownButtonInterrupt;               ///< Offset 2354
  //
  // Generic Buttons Device policies - End
  //
  UINT8    TsnEnabled;                              ///< Offset 2358
  UINT8    PseTsn0Enabled;                          ///< Offset 2359
  UINT8    PseTsn1Enabled;                          ///< Offset 2360
  UINT8    PseUart0Rs485Enabled;                    ///< Offset 2361    Enable RS485 mode for PSE UART 0
  UINT8    PseUart1Rs485Enabled;                    ///< Offset 2362    Enable RS485 mode for PSE UART 0
  UINT8    PseUart2Rs485Enabled;                    ///< Offset 2363    Enable RS485 mode for PSE UART 0
  UINT8    PseUart3Rs485Enabled;                    ///< Offset 2364    Enable RS485 mode for PSE UART 0
  UINT8    PSC0;                                    ///< Offset 2365    PSE SPI0 ChipSelect0 Device enabled
  UINT8    PSC1;                                    ///< Offset 2366    PSE SPI1 ChipSelect0 Device enabled
  UINT8    PSC2;                                    ///< Offset 2367    PSE SPI2 ChipSelect0 Device enabled
  UINT8    PSC3;                                    ///< Offset 2368    PSE SPI3 ChipSelect0 Device enabled
  UINT8    PS00;                                    ///< Offset 2369    PSE SPI0 ChipSelect1 Device enabled
  UINT8    PS01;                                    ///< Offset 2370    PSE SPI1 ChipSelect1 Device enabled
  UINT8    PS02;                                    ///< Offset 2371    PSE SPI2 ChipSelect1 Device enabled
  UINT8    PS03;                                    ///< Offset 2372    PSE SPI3 ChipSelect1 Device enabled
  UINT16   SSH6;                                    ///< Offset 2373    SSCN-HIGH for I2C6
  UINT16   SSL6;                                    ///< Offset 2375    SSCN-LOW  for I2C6
  UINT16   SSD6;                                    ///< Offset 2377    SSCN-HOLD for I2C6
  UINT16   FMH6;                                    ///< Offset 2379    FMCN-HIGH for I2C6
  UINT16   FML6;                                    ///< Offset 2381    FMCN-LOW  for I2C6
  UINT16   FMD6;                                    ///< Offset 2383    FMCN-HOLD for I2C6
  UINT16   FPH6;                                    ///< Offset 2385    FPCN-HIGH for I2C6
  UINT16   FPL6;                                    ///< Offset 2387    FPCN-LOW  for I2C6
  UINT16   FPD6;                                    ///< Offset 2389    FPCN-HOLD for I2C6
  UINT16   HSH6;                                    ///< Offset 2391    HSCN-HIGH for I2C6
  UINT16   HSL6;                                    ///< Offset 2393    HSCN-LOW  for I2C6
  UINT16   HSD6;                                    ///< Offset 2395    HSCN-HOLD for I2C6
  UINT16   SSH7;                                    ///< Offset 2397    SSCN-HIGH for I2C7
  UINT16   SSL7;                                    ///< Offset 2399    SSCN-LOW  for I2C7
  UINT16   SSD7;                                    ///< Offset 2401    SSCN-HOLD for I2C7
  UINT16   FMH7;                                    ///< Offset 2403    FMCN-HIGH for I2C7
  UINT16   FML7;                                    ///< Offset 2405    FMCN-LOW  for I2C7
  UINT16   FMD7;                                    ///< Offset 2407    FMCN-HOLD for I2C7
  UINT16   FPH7;                                    ///< Offset 2409    FPCN-HIGH for I2C7
  UINT16   FPL7;                                    ///< Offset 2411    FPCN-LOW  for I2C7
  UINT16   FPD7;                                    ///< Offset 2413    FPCN-HOLD for I2C7
  UINT16   HSH7;                                    ///< Offset 2415    HSCN-HIGH for I2C7
  UINT16   HSL7;                                    ///< Offset 2417    HSCN-LOW  for I2C7
  UINT16   HSD7;                                    ///< Offset 2419    HSCN-HOLD for I2C7
  UINT16   M0CC;                                    ///< Offset 2421    M0D3 for I2C6
  UINT16   M1CC;                                    ///< Offset 2423    M1D3 for I2C6
  UINT16   M0CD;                                    ///< Offset 2425    M0D3 for I2C7
  UINT16   M1CD;                                    ///< Offset 2427    M1D3 for I2C7
  UINT32   IC0S;                                    ///< Offset 2429    I2C0 Speed - Standard mode/Fast mode/FastPlus mode/HighSpeed mode
  UINT32   IC1S;                                    ///< Offset 2433    I2C1 Speed - Standard mode/Fast mode/FastPlus mode/HighSpeed mode
  UINT32   IC2S;                                    ///< Offset 2437    I2C2 Speed - Standard mode/Fast mode/FastPlus mode/HighSpeed mode
  UINT32   IC3S;                                    ///< Offset 2441    I2C3 Speed - Standard mode/Fast mode/FastPlus mode/HighSpeed mode
  UINT32   IC4S;                                    ///< Offset 2445    I2C4 Speed - Standard mode/Fast mode/FastPlus mode/HighSpeed mode
  UINT32   IC5S;                                    ///< Offset 2449    I2C5 Speed - Standard mode/Fast mode/FastPlus mode/HighSpeed mode
  UINT32   IC6S;                                    ///< Offset 2453    I2C6 Speed - Standard mode/Fast mode/FastPlus mode/HighSpeed mode
  UINT32   IC7S;                                    ///< Offset 2457    I2C7 Speed - Standard mode/Fast mode/FastPlus mode/HighSpeed mode
  UINT8    PcieSlot1ClkSrc;                         ///< Offset 2461    Pcie Slot 1 Clk Src
  UINT8    PcieSlot2ClkSrc;                         ///< Offset 2462    Pcie Slot 2 Clk Src
  UINT64   TBAS;                                    ///< Offset 2463    PTCT Table Memory Address
  // Test Device for Serial IOs
  UINT8    SII0;                                    ///< Offset 2471    SIO I2C0 test devices' connection point
  UINT8    SII1;                                    ///< Offset 2472    SIO I2C1 test devices' connection point
  UINT8    SII2;                                    ///< Offset 2473    SIO I2C2 test devices' connection point
  UINT8    SII3;                                    ///< Offset 2474    SIO I2C3 test devices' connection point
  UINT8    SII4;                                    ///< Offset 2475    SIO I2C4 test devices' connection point
  UINT8    SII5;                                    ///< Offset 2476    SIO I2C5 test devices' connection point
  UINT8    SII6;                                    ///< Offset 2477    SIO I2C6 test devices' connection point
  UINT8    SII7;                                    ///< Offset 2478    SIO I2C7 test devices' connection point
  UINT8    SIS0;                                    ///< Offset 2479    SIO SPI0 test devices' connection point
  UINT8    SIS1;                                    ///< Offset 2480    SIO SPI1 test devices' connection point
  UINT8    SIS2;                                    ///< Offset 2481    SIO SPI2 test devices' connection point
  UINT8    SIU0;                                    ///< Offset 2482    SIO UART0 test devices' connection point
  UINT8    SIU1;                                    ///< Offset 2483    SIO UART1 test devices' connection point
  UINT8    SIU2;                                    ///< Offset 2484    SIO UART2 test devices' connection point
  // Test Device for PSE IOs
  UINT8    PII0;                                    ///< Offset 2485    PSE I2C0 test devices' connection point
  UINT8    PII1;                                    ///< Offset 2486    PSE I2C1 test devices' connection point
  UINT8    PII2;                                    ///< Offset 2487    PSE I2C2 test devices' connection point
  UINT8    PII3;                                    ///< Offset 2488    PSE I2C3 test devices' connection point
  UINT8    PII4;                                    ///< Offset 2489    PSE I2C4 test devices' connection point
  UINT8    PII5;                                    ///< Offset 2490    PSE I2C5 test devices' connection point
  UINT8    PII6;                                    ///< Offset 2491    PSE I2C6 test devices' connection point
  UINT8    PII7;                                    ///< Offset 2492    PSE I2C7 test devices' connection point
  UINT8    PIS0;                                    ///< Offset 2493    PSE SPI0 test devices' connection point
  UINT8    PIS1;                                    ///< Offset 2494    PSE SPI1 test devices' connection point
  UINT8    PIS2;                                    ///< Offset 2495    PSE SPI2 test devices' connection point
  UINT8    PIS3;                                    ///< Offset 2496    PSE SPI3 test devices' connection point
  UINT8    PIU0;                                    ///< Offset 2497    PSE UART0 test devices' connection point
  UINT8    PIU1;                                    ///< Offset 2498    PSE UART1 test devices' connection point
  UINT8    PIU2;                                    ///< Offset 2499    PSE UART2 test devices' connection point
  UINT8    PIU3;                                    ///< Offset 2500    PSE UART3 test devices' connection point
  UINT8    PIU4;                                    ///< Offset 2501    PSE UART4 test devices' connection point
  UINT8    PIU5;                                    ///< Offset 2502    PSE UART5 test devices' connection point
  //
  // Eclite Device Enabe/Disable
  //
  UINT8    EcliteServiceEnable;                     ///< Offset 2503    PSE Eclite Service Enable/Disable
  UINT8    PseWolServiceEnable;                     ///< Offset 2504    PSE WoL Service Enable/Disable
  UINT8    PseDashBoardSmiAddress;                  ///< Offset 2505    PSE Dashboard update IoTrap Address
  UINT8    PseDashBoardUpdateValue;                 ///< Offset 2506    PSE Dashboard update IoTrap Value
  UINT8    WifiAntGainEnale;                        ///< Offset 2507    Wifi Ant Gain Enable
  UINT8    NewGpioSchemeEnable;                     ///< Offset 2508    New GPIO Scheme Enable/Disable
  UINT8    ArtTscLinkFlag;                          ///< Offset 2509    ART traceability to TSC
} PLATFORM_NVS_AREA;

#pragma pack(pop)
#endif
