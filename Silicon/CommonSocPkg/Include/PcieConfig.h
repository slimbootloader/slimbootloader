/** @file
  PCIe Config Block

  Copyright (c) 2026, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent
**/
#ifndef _PCIE_CONFIG_H_
#define _PCIE_CONFIG_H_

#define PCIE_LINK_EQ_COEFFICIENTS_MAX 10
#define PCIE_LINK_EQ_PRESETS_MAX 11

#pragma pack (push,1)

typedef struct {
  UINT16  LtrMaxSnoopLatency;                     ///< <b>(Test)</b> Latency Tolerance Reporting, Max Snoop Latency.
  UINT16  LtrMaxNoSnoopLatency;                   ///< <b>(Test)</b> Latency Tolerance Reporting, Max Non-Snoop Latency.
  UINT8   SnoopLatencyOverrideMode;               ///< <b>(Test)</b> Latency Tolerance Reporting, Snoop Latency Override Mode.
  UINT8   SnoopLatencyOverrideMultiplier;         ///< <b>(Test)</b> Latency Tolerance Reporting, Snoop Latency Override Multiplier.
  UINT16  SnoopLatencyOverrideValue;              ///< <b>(Test)</b> Latency Tolerance Reporting, Snoop Latency Override Value.
  UINT8   NonSnoopLatencyOverrideMode;            ///< <b>(Test)</b> Latency Tolerance Reporting, Non-Snoop Latency Override Mode.
  UINT8   NonSnoopLatencyOverrideMultiplier;      ///< <b>(Test)</b> Latency Tolerance Reporting, Non-Snoop Latency Override Multiplier.
  UINT16  NonSnoopLatencyOverrideValue;           ///< <b>(Test)</b> Latency Tolerance Reporting, Non-Snoop Latency Override Value.
  UINT8   LtrConfigLock;                          ///< <b>0: Disable</b>; 1: Enable.
  UINT8   ForceLtrOverride;
  UINT8   LtrOverrideSpecComplaint;
  UINT8   RsvdByte1;
} PCIE_LTR_CONFIG;

typedef struct {
  UINT32  PreCursor;    ///< Pre-cursor coefficient
  UINT32  PostCursor;   ///< Post-cursor coefficient
} PCIE_LINK_EQ_COEFFICIENTS;

/**
  PCIe Link EQ Platform Settings
**/
typedef struct {
  UINT8                      PcieLinkEqMethod;               ///< Tells BIOS which link EQ method should be used for this port. Please refer to PCIE_LINK_EQ_METHOD for details of supported methods. Default: PcieLinkHardwareEq
  UINT8                      PcieLinkEqMode;                 ///< Tells BIOS which mode should be used for PCIe link EQ. Please refer to PCIE_LINK_EQ_MODE for details of supported modes. Default: depends on SoC
  /**
    Specifies if BIOS should perform local transmitter override during phase 2 of EQ process.
    If enabled value in Ph2LocalTxOverridePreset must be valid.
    <b>0: Disabled</b>; 1: Enabled
  **/
  UINT8                      LocalTxOverrideEn;
  UINT8                      RemoteTransmitterOverrideEnable;
  UINT8                      DefaultPresetEnable[PCIE_LINK_EQ_PRESETS_MAX];       ///< Specifies if default settings should remain in use
  UINT8                      HapcsSearchEnable;              ///< Specifies setting for Hardware Autonomous Preset/Coefficient Search mechanism
  /**
    Specifies the preset that should be used during local transmitter override during phase 2 of EQ process.
    Used only if LocalTxOverrideEn is TRUE. Will be applied to all PCIe lanes of the root port.
    Valid up to the PCIE_LINK_EQ_PRESET_MAX value. <b>Default: 0<\b>
  **/
  UINT32                     Ph2LocalTxOverridePreset;
  UINT32                     PCETTimer;                  ///< PCET Timer value for single PCIe speed.
  UINT8                      RemotePresetCoeffoverride;  ///< Remote Transmitter Preset Coefficient Override for single PCIe speed.
  /**
  PCIe Equalization Phase 3 Enable Control
    - <b>Disabled</b>       (0x0) : Disable phase 3 (Default)
     - Enabled               (0x1) : Enable phase 3
  **/
  UINT8                      EqPh3Bypass;
  /**
  PCIe Equalization Phase 2-3 Enable Control
  - <b>Disabled</b>       (0x0) : Disable Phase 2 - Phase 3 (Default)
  - Enabled               (0x1) : Enable Phase 2 - Phase 3
  **/
  UINT8                      EqPh23Bypass;
  UINT8                      TsLockTimer;                 ///< 8.0GT/s Training Sequence Wait Latency For Presets / Coefficients Evaluation - Gen3 TS Lock Timer
  /**
  PCIe Equalization Phase Enable Control
  - <b>Disabled</b>       (0x0) : Disable Phase (Default)
  - Enabled               (0x1) : Enable Phase
  **/
  UINT8                      EqPhBypass;
  UINT8                      Rsvd[2];
  /**
    Tells BIOS how many presets/coefficients should be used during link EQ.
    Entries in the Ph3CoefficientsList or Ph3PresetList(depending on chosen mode) need to be valid up to the number specified in this field.
  **/
  UINT8                      Ph3NoOfPresetOrCoeff;
  PCIE_LINK_EQ_COEFFICIENTS  Ph3CoefficientsList[PCIE_LINK_EQ_COEFFICIENTS_MAX];  ///< List of the PCIe coefficients to be used during equalization process. Only valid if PcieLinkEqMode is PcieLinkEqCoefficientMode
  UINT32                     Ph3PresetList[PCIE_LINK_EQ_PRESETS_MAX];             ///< List of the PCIe preset values to be used during equalization process. Only valid if PcieLinkEqMode is PcieLinkEqPresetMode
  UINT32                     EndPointPresetList[PCIE_LINK_EQ_PRESETS_MAX];        ///< List of the PCIe preset values to be used during equalization process. Only valid if PcieLinkEqMode is PcieLinkEqPresetMode
  UINT32                     HintList[PCIE_LINK_EQ_PRESETS_MAX];                  ///< List of the Hint values to be used during equalization process for preset calculation.
  UINT32                     Ph1DpTxPreset;                                       ///< Specifies the value of the downstream port transmitter preset to be used during phase 1 of the equalization process. Will be applied to all lanes
  UINT32                     Ph1UpTxPreset;                                       ///< Specifies the value of the upstream port transmitter preset to be used during phase 1 of the equalization process. Will be applied to all lanes
} PCIE_LINK_EQ_PLATFORM_SETTINGS;

//Note: This structure will be expanded to hold all common PCIe policies between SA and PCH RootPort
typedef struct {
  UINT32  HotPlug                         :  1;   ///< Indicate whether the root port is hot plug available. <b>0: Disable</b>; 1: Enable.
  UINT32  PmSci                           :  1;   ///< Indicate whether the root port power manager SCI is enabled. 0: Disable; <b>1: Enable</b>.
  UINT32  TransmitterHalfSwing            :  1;   ///< Indicate whether the Transmitter Half Swing is enabled. <b>0: Disable</b>; 1: Enable.
  UINT32  AcsEnabled                      :  1;   ///< Indicate whether the ACS is enabled. 0: Disable; <b>1: Enable</b>.
  //
  // Error handlings
  //
  UINT32  AdvancedErrorReporting          :  1;   ///< Indicate whether the Advanced Error Reporting is enabled. <b>0: Disable</b>; 1: Enable.
  UINT32  UnsupportedRequestReport        :  1;   ///< Indicate whether the Unsupported Request Report is enabled. <b>0: Disable</b>; 1: Enable.
  UINT32  FatalErrorReport                :  1;   ///< Indicate whether the Fatal Error Report is enabled. <b>0: Disable</b>; 1: Enable.
  UINT32  NoFatalErrorReport              :  1;   ///< Indicate whether the No Fatal Error Report is enabled. <b>0: Disable</b>; 1: Enable.
  UINT32  CorrectableErrorReport          :  1;   ///< Indicate whether the Correctable Error Report is enabled. <b>0: Disable</b>; 1: Enable.
  UINT32  SystemErrorOnFatalError         :  1;   ///< Indicate whether the System Error on Fatal Error is enabled. <b>0: Disable</b>; 1: Enable.
  UINT32  SystemErrorOnNonFatalError      :  1;   ///< Indicate whether the System Error on Non Fatal Error is enabled. <b>0: Disable</b>; 1: Enable.
  UINT32  SystemErrorOnCorrectableError   :  1;   ///< Indicate whether the System Error on Correctable Error is enabled. <b>0: Disable</b>; 1: Enable.
  /**
    Max Payload Size supported, Default <b>128B</b>, see enum CPU_PCIE_MAX_PAYLOAD
    Changes Max Payload Size Supported field in Device Capabilities of the root port.
  **/
  UINT32  MaxPayload                      :  3;
  UINT32  DpcEnabled                      :  1;   ///< Downstream Port Containment. 0: Disable; <b>1: Enable</b>
  UINT32  RpDpcExtensionsEnabled          :  1;   ///< RP Extensions for Downstream Port Containment. 0: Disable; <b>1: Enable</b>
  /**
    Indicates how this root port is connected to endpoint. 0: built-in device; <b>1: slot</b>
    Built-in is incompatible with hotplug-capable ports.
  **/
  UINT32  SlotImplemented                 :  1;
  UINT32  PtmEnabled                      :  1;   ///< Enables PTM capability
  UINT32  RsvdBits0                       : 13;   ///< Reserved bits.
  UINT32  SlotPowerLimitScale             :  2;   ///< <b>(Test)</b> Specifies scale used for slot power limit value. Leave as 0 to set to default. Default is <b>zero</b>.
  UINT32  SlotPowerLimitValue             : 12;   //< <b>(Test)</b> Specifies upper limit on power supplies by slot. Leave as 0 to set to default. Default is <b>zero</b>.
  /**
    Probe CLKREQ# signal before enabling CLKREQ# based power management.
    Conforming device shall hold CLKREQ# low until CPM is enabled. This feature attempts
    to verify CLKREQ# signal is connected by testing pad state before enabling CPM.
    In particular this helps to avoid issues with open-ended PCIe slots.
    This is only applicable to non hot-plug ports.
    <b>0: Disable</b>; 1: Enable.
  **/
  UINT32  ClkReqDetect                    :  1;
  /**
    Set if the slot supports manually operated retention latch.
  **/
  UINT32  MrlSensorPresent                :  1;
  UINT32  RelaxedOrder                    :  1;
  UINT32  NoSnoop                         :  1;
  ///
  /// This member describes whether Peer Memory Writes are enabled on the platform. <b>0: Disable</b>; 1: Enable.
  ///
  UINT32  EnablePeerMemoryWrite           :  1;
  ///
  /// This member describes whether the PCI Express Clock Gating for each root port
  /// is enabled by platform modules. <b>0: Disable</b>; 1: Enable.
  ///
  UINT32  ClockGating                     :  1;
  ///
  /// This member describes whether the PCI Express Power Gating for each root port
  /// is enabled by platform modules. <b>0: Disable</b>; 1: Enable.
  ///
  UINT32  PowerGating                     :  1;
  UINT32  PcieFomsCp                      :  3;    ///< FOM Score Board Control Policy
  UINT32  LedManagement                   :  1;   ///< Indicate whether the VMD Led Management is enabled in root port. <b>0: Disable</b>; 1: Enable.
  UINT32  RsvdBits1                       :  7;   ///< Reserved bits.
  UINT16  PhysicalSlotNumber;                     ///< Indicates the slot number for the root port. Default is the value as root port index.
  /**
    PCIe Gen3 Equalization Phase 3 Method (see CPU_PCIE_EQ_METHOD).
    0: DEPRECATED, hardware equalization; <b>1: hardware equalization</b>; 4: Fixed Coefficients
  **/
  UINT8   Gen3EqPh3Method;
  UINT8   CompletionTimeout;                      ///< The completion timeout configuration of the root port (see: CPU_PCIE_COMPLETION_TIMEOUT). Default is <b>PchPcieCompletionTO_Default</b>.
  /**
  <b>(Test)</b>
  Forces LTR override to be permanent
  The default way LTR override works is:
  rootport uses LTR override values provided by BIOS until connected device sends an LTR message, then it will use values from the message
  This settings allows force override of LTR mechanism. If it's enabled, then:
  rootport will use LTR override values provided by BIOS forever; LTR messages sent from connected device will be ignored
  **/
  PCIE_LTR_CONFIG               PcieRpLtrConfig;            ///< <b>(Test)</b> Latency Tolerance Reporting Policies including LTR limit and Override
  //
  // Power Management
  //
  UINT8   Aspm;                                   ///< The ASPM configuration of the root port (see: CPU_PCIE_ASPM_CONTROL). Default is <b>PchPcieAspmAutoConfig</b>.
  UINT8   L1Substates;                            ///< The L1 Substates configuration of the root port (see: CPU_PCIE_L1SUBSTATES_CONTROL). Default is <b>PchPcieL1SubstatesL1_1_2</b>.
  UINT8   L1ExitLatency;                          ///< L1 Exit Latency
  UINT8   LtrEnable;                              ///< Latency Tolerance Reporting Mechanism. <b>0: Disable</b>; 1: Enable.
  UINT8   EnableCpm;                              ///< Enables Clock Power Management; even if disabled, CLKREQ# signal can still be controlled by L1 PM substates mechanism
  UINT8   PcieSpeed;                              ///< Contains speed of PCIe bus (see: PCIE_SPEED)
    /**
    The number of milliseconds reference code will wait for link to exit Detect state for enabled ports
    before assuming there is no device and potentially disabling the port.
    It's assumed that the link will exit detect state before root port initialization (sufficient time
    elapsed since PLTRST de-assertion) therefore default timeout is zero. However this might be useful
    if device power-up seqence is controlled by BIOS or a specific device requires more time to detect.
    In case of non-common clock enabled the default timout is 15ms.
    <b>Default: 0</b>
  **/
  UINT16  DetectTimeoutMs;
  UINT8   FormFactor; // Please check PCIE_FORM_FACTOR for supported values
  UINT8   L1Low;                                  ///< L1.LOW enable/disable. <b>0: Disable</b>; 1: Enable.
  UINT8   LinkDownGpios;
  ///
  /// <b>0: Use project default equalization settings</b>; 1: Use equalization settings from PcieLinkEqPlatformSettings
  ///
  UINT8   OverrideEqualizationDefaults;
  UINT8   PcieCfgDump;
  UINT8   Rsvd[11];
  PCIE_LINK_EQ_PLATFORM_SETTINGS    PcieGen3LinkEqPlatformSettings;  ///< Global PCIe Gen3 link EQ settings that BIOS will use during PCIe link EQ for every port.
  PCIE_LINK_EQ_PLATFORM_SETTINGS    PcieGen4LinkEqPlatformSettings;  ///< Global PCIe Gen4 link EQ settings that BIOS will use during PCIe link EQ for every port.
  PCIE_LINK_EQ_PLATFORM_SETTINGS    PcieGen5LinkEqPlatformSettings;  ///< Global PCIe Gen5 link EQ settings that BIOS will use during PCIe link EQ for every port.
} PCIE_ROOT_PORT_COMMON_CONFIG;

#pragma pack (pop)
#endif // _PCIE_CONFIG_H_
