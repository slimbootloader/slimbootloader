/** @file
  Common USB policy shared between PCH and CPU
  Contains general features settings for xHCI and xDCI

  @copyright
  INTEL CONFIDENTIAL
  Copyright (C) 2017 Intel Corporation.

  This software and the related documents are Intel copyrighted materials,
  and your use of them is governed by the express license under which they
  were provided to you ("License"). Unless the License provides otherwise,
  you may not use, modify, copy, publish, distribute, disclose or transmit
  this software or the related documents without Intel's prior written
  permission.

  This software and the related documents are provided as is, with no
  express or implied warranties, other than those that are expressly stated
  in the License.

@par Specification Reference:
**/
#ifndef _USB_CONFIG_H_
#define _USB_CONFIG_H_

#define USB_CONFIG_REVISION 5
//extern EFI_GUID gUsbConfigGuid;

#define MAX_USB2_PORTS  16
#define MAX_USB3_PORTS  10

//
// USB OC Pin Type
//
#define PHYSICAL_OC      1
#define VW_OC            2

#pragma pack (push,1)

typedef UINT8                   USB_OVERCURRENT_PIN;
#define USB_OC_SKIP             0xFF
#define USB_OC_MAX_PINS         16       ///< Total OC pins number (both physical and virtual)

/**
  This structure configures per USB2.0 port settings like enabling and overcurrent protection
**/
typedef struct {
  /**
    These members describe the specific over current pin number of USB 2.0 Port N.
    It is SW's responsibility to ensure that a given port's bit map is set only for
    one OC pin Description. USB2 and USB3 on the same combo Port must use the same OC pin.
  **/
  UINT32     OverCurrentPin          :  8;
  UINT32     OverCurrentPinType      :  8;
  UINT32     Enable                  :  1;     ///< 0: Disable; <b>1: Enable</b>.
  UINT32     PortResetMessageEnable  :  1;     ///< 0: Disable USB2 Port Reset Message; 1: Enable USB2 Port Reset Message
  UINT32     SwDeviceModeEnable      :  1;     ///< 0: Disable USB2 Port Device Mode; 1: Force USB2 Port Device Mode
  UINT32     Usb3CompatiblePort      :  3;     ///< 0: Not PCH USB3 port; 1: PCH USB3 (eSS) port
  UINT32     Usb3CompatiblePortNum   :  5;     ///< 0: port1; 1: port2; 2: port3. Usb3CompatiblePort with 1 is the premise.
  UINT32     RsvdBits0               : 13;     ///< Reserved bits
} USB2_PORT_CONFIG;

/**
  This structure configures per USB3.x port settings like enabling and overcurrent protection
**/
typedef struct {
  /**
    These members describe the specific over current pin number of USB 3.x Port N.
    It is SW's responsibility to ensure that a given port's bit map is set only for
    one OC pin Description. USB2 and USB3 on the same combo Port must use the same OC pin.
  **/
  UINT32  OverCurrentPin               :  8;
  UINT32  OverCurrentPinType           :  8;
  UINT32  Enable                       :  1; ///< 0: Disable; <b>1: Enable</b>.
  UINT32  RsvdBits0                    : 11; ///< Reserved bits
} USB3_PORT_CONFIG;

/**
  The XDCI_CONFIG block describes the configurations
  of the xDCI Usb Device controller.
**/
typedef struct {
  /**
    This member describes whether or not the xDCI controller should be enabled.
    0: Disable; <b>1: Enable</b>.
  **/
  UINT32  Enable              :  1;
  UINT32  RsvdBits0           : 31;     ///< Reserved bits
} XDCI_CONFIG;

/**
  This member describes the expected configuration of the USB controller,
  Platform modules may need to refer Setup options, schematic, BIOS specification to update this field.
  The Usb20OverCurrentPins and Usb30OverCurrentPins field must be updated by referring the schematic.

  <b>Revision 1</b>:
  - Initial version.
  <b>Revision 2</b>:
  - Added UaolEnable to control USB Audio Offload Capability.
  <b>Revision 3</b>:
  - Add HsiiEnable enable option to control HSII feature
  <b>Revision 4</b>:
  - Add Usb31Speed to select gen1 or gen2 speed
  - Add SwDeviceModeEnable to control device mode of usb2 port
  <b>Revision 5</b>:
  - Add DwbEnable to control the Deferred Write Buffer feature
**/
typedef struct {
  CONFIG_BLOCK_HEADER     Header;                   ///< Config Block Header
  /**
    This policy option when set will make BIOS program Port Disable Override register during PEI phase.
    When disabled BIOS will not program the PDO during PEI phase and leave PDO register unlocked for later programming.
    If this is disabled, platform code MUST set it before booting into OS.
    <b>1: Enable</b>
    0: Disable
  **/
  UINT32                  PdoProgramming               :  1;
  /**
    This option allows for control whether USB should program the Overcurrent Pins mapping into xHCI.
    Disabling this feature will disable overcurrent detection functionality.
    Overcurrent Pin mapping data is contained in respective port structures (i.e. USB30_PORT_CONFIG) in OverCurrentPin field.
    By default this Overcurrent functionality should be enabled and disabled only for OBS debug usage.
    <b>1: Will program USB OC pin mapping in respective xHCI controller registers</b>
    0: Will clear OC pin mapping allow for OBS usage of OC pins
  **/
  UINT32                  OverCurrentEnable            :  1;


  /**
    This option allows for control whether USB should program the Physical Overcurrent Pins or VIrutal Overcurrent mapping into xHCI.
    Disabling this Pin will disable overcurrent pin type detection functionality.
    Overcurrent Pin mapping data is contained in respective port structures (i.e. USB30_PORT_CONFIG) in OverCurrentPin field.
    By default this Overcurrent functionality should be enabled and disabled only for OBS debug usage.
    <b>1: Will program Physical USB OC pin mapping in respective xHCI controller registers</b>
    0: Will program Virtual USB OC pin mapping in respective xHCI controller registers<
  **/
  UINT32                  OverCurrentPinType            : 1;
  /**
    <b>(Test)</b>
    If this policy option is enabled then BIOS will program OCCFDONE bit in xHCI meaning that OC mapping data will be
    consumed by xHCI and OC mapping registers will be locked. OverCurrent mapping data is taken from respective port data
    structure from OverCurrentPin field.
    If EnableOverCurrent policy is enabled this also should be enabled, otherwise xHCI won't consume OC mapping data.
    <b>1: Program OCCFDONE bit and make xHCI consume OverCurrent mapping data</b>
    0: Do not program OCCFDONE bit making it possible to use OBS debug on OC pins.
  **/
  UINT32                  XhciOcLock                   :  1;
  /**
    Enabling this feature will allow for overriding LTR values for xHCI controller.
    Values used for programming will be taken from this config block and BIOS will disregard recommended ones.
    <b>0: disable - do not override recommended LTR values</b>
    1: enable - override recommended LTR values
  **/
  UINT32                  LtrOverrideEnable            :  1;
  /**
    Enable/disable option for USB Audio Offload feture. Disabling this will disable UAOL capability
    in xHCI and UAOL ACPI definitions will be hidden.
    0: disable UAOL
    <b>1: enable UAOL</b>
  **/
  UINT32                  UaolEnable                   :  1;
  /**
    Enable HS Interrupt IN Alarm
    0: disable - Disable HSII
    <b>1: enable - Enable HSII</b>
  **/
  UINT32                  HsiiEnable                   :  1;
  /**
    Disable Xhci Controller
    0: Disable; 1: Enable.
  **/
  UINT32                  ControllerDisable            : 1;
    /**
    Enable DWB
    0: disable
    <b>1: enable</b>
  **/
  UINT32                  DwbEnable                    :  1;
  UINT32                  RsvdBits0                    : 24;     ///< Reserved bits
  /**
    High Idle Time Control override value
    This setting is used only if LtrOverrideEnable is enabled
  **/
  UINT32                  LtrHighIdleTimeOverride;
  /**
    Medium Idle Time Control override value
    This setting is used only if LtrOverrideEnable is enabled
  **/
  UINT32                  LtrMediumIdleTimeOverride;
  /**
    Low Idle Time Control override value
    This setting is used only if LtrOverrideEnable is enabled
  **/
  UINT32                  LtrLowIdleTimeOverride;
  /**
    These members describe whether the USB2 Port N of PCH is enabled by platform modules.
  **/
  USB2_PORT_CONFIG        PortUsb20[MAX_USB2_PORTS];
  /**
    These members describe whether the USB3 Port N of PCH is enabled by platform modules.
  **/
  USB3_PORT_CONFIG        PortUsb30[MAX_USB3_PORTS];
  /**
    This member describes whether or not the xDCI controller should be enabled.
  **/
  XDCI_CONFIG             XdciConfig;
  /**
    USB3.1 Speeed selection. Each bit corresponds to a port.
  **/
  UINT32                  Usb31Speed;
//
// PCH:RestrictedBegin
//
  /**
  Ater xHCI is initialized, BIOS should lock the xHCI configuration registers to RO.
  This prevents any unintended changes. BIOS should set these bits to lock down the settings prior to end of POST
  0: POR is XHCI Access Control Bit is set and locks the registers;
  1: Enable XHCI Access Control Bit is set and locks the registers;
  2: Clear/Disable XHCI Access Control Bit is cleared and the registers are unlocked.
  **/
  UINT32  TestXhciAccessControlLock  : 2;
  UINT32  RsvdBits1                  : 30;     ///< Reserved bits
//
// PCH:RestrictedEnd
//
} USB_CONFIG;

#pragma pack (pop)

#endif // _USB_CONFIG_H_
