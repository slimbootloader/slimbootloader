/** @file
  ACPI Pep devices list.

  Copyright (c) 2026, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent
**/

//
// List all the devices that support Pep constraint.
//
// Note:
//  1. This file list all the devices that support PEP except
//    Procossors.
//  2. The developer needs to make sure the required External
//    declaration and initialization flow for the devices.
//

#define RP_DEVICE_REQUIRED_EXTERNAL(Device) \
        External (ROOT_COMPLEX.Device, DeviceObj) \
        External (ROOT_COMPLEX.Device._ADR) \
        External (ROOT_COMPLEX.Device.PXSX.PRES, MethodObj) \
        External (ROOT_COMPLEX.Device.PXSX.WWST, MethodObj) \
        External (ROOT_COMPLEX.Device.PXSX.GRPT, MethodObj)

//
// Init the PEP constraint for the PCIe RootPort.
// 1. Check if the RP is defined or not.
// 2. Check if the endpoint device is present or not.
// 3. Check if the endpoint device is WWAN device or not
//    a. If it is WWAN, set PEP value base on the WWAN RTD3 setup option
// 4. Check if the PEP override method is defined or not. If defined,
//  call the override method to decide the PEP value.
//
#define PEP_DEVICE_RP(Device) \
        PEPV = 0x00 \
        If (CondRefOf (ROOT_COMPLEX.Device)) { \
          ADBG (#Device) \
          ADBG (Concatenate ("PCIe Address:", ToHexString (ROOT_COMPLEX.Device._ADR))) \
          PEPV = 0x05 \
          If (ROOT_COMPLEX.Device.PXSX.PRES ()) { \
            PEPV = 0x01 \
            If (CondRefOf (ROOT_COMPLEX.Device.PXSX.WWST)) { \
              If (ROOT_COMPLEX.Device.PXSX.WWST ()) { \
                PEPV = 0x00 \
                If (LEqual (WRTO, 1)) { \
                  ADBG (Concatenate (#Device, " endpoint WWAN D0F1!")) \
                  PDIN ("USB\\VID_8087&PID_0AC9&MI*", "", 1) \
                } ElseIf (LEqual (WRTO, 3)) { \
                  PEPV = 0x03 \
                } \
              } \
            } \
          } \
          If (CondRefOf (\_SB.PEPD.RPCO)) { \
            PEPV = \_SB.PEPD.RPCO (PEPV, ROOT_COMPLEX.Device.PXSX.GRPT ()) \
          } \
        } \
        PDIN (ROOT_COMPLEX_STR, #Device, PEPV)

#define DEVICE_REQUIRED_EXTERNAL(Device) \
        External (ROOT_COMPLEX.Device, DeviceObj) \
        External (ROOT_COMPLEX.Device._ADR) \
        External (ROOT_COMPLEX.Device.POVR, MethodObj)

//
// Init the PEP constraint for the PCIe Device.
// 1. Check if the Device is defined or not.
// 2. Check if the Device is present or not. Base on the PCIe device _ADR
//    a. Use PFEK with the device address to decide the pep value.
//    b. If Device is present, the default PEP constraint value would be DState.
// 3. Check if the PEP override method is defined or not. If defined,
//  call the override method to decide the PEP value.
//
#define PEP_DEVICE(Device, DState) \
        PEPV = 0x00 \
        If (CondRefOf (ROOT_COMPLEX.Device)) { \
          PEPV = PFEK (ROOT_COMPLEX.Device._ADR, ROOT_COMPLEX_BUS, DState) \
          If (CondRefOf (ROOT_COMPLEX.Device.POVR)) { \
            PEPV = ROOT_COMPLEX.Device.POVR (PEPV) \
          } \
        } \
        PDIN (ROOT_COMPLEX_STR, #Device, PEPV)

#define NORMAL_DEVICE_REQUIRED_EXTERNAL(Device) \
        External (ROOT_COMPLEX.Device, DeviceObj) \
        External (ROOT_COMPLEX.Device._ADR)

//
// Init the PEP constraint for TBT devices.
// 1. Check if the PEP override method for TBT present or not, to decide the
//  default pep value for TBT.
// 2. If PEP is enable,
//  2.a check if TCSS XHCI option enable or not.
//  2.b If PEP is enable, check if the sub port device defined or not.
// 3. If opiton enable, chek if the device definition present or not.
//
#define PEP_DEVICE_TCSS(Device, DeviceEnable) \
        PEPV = 0x03 \
        ADBG (#Device) \
        If (CondRefOf (\_SB.PEPD.TSCO)) { \
          PEPV = \_SB.PEPD.TSCO (PEPV) \
        } \
        If (LGreaterEqual (PEPV, 1)) { \
          If (CondRefOf (DeviceEnable)) { \
            If (LEqual (DeviceEnable, 1)) { \
              If (CondRefOf (ROOT_COMPLEX.Device)) { \
                ADBG (Concatenate ("PCI Address: ", ToHexString (ROOT_COMPLEX.Device._ADR))) \
                PDIN (ROOT_COMPLEX_STR, #Device, PEPV) \
              } \
            } \
          } \
        }

#define SATA_PORT_DEVICE_REQUIRED_EXTERNAL(Device) \
        External (ROOT_COMPLEX.Device, DeviceObj) \
        External (ROOT_COMPLEX.Device.POVR, MethodObj) \
        External (ROOT_COMPLEX.Device.PRES, MethodObj)

#define PEP_DEVICE_SATA_PORT(SataDevice, Port, DState) \
        PEPV = DState \
        ADBG (#Port) \
        If (CondRefOf (ROOT_COMPLEX.SataDevice)) { \
          PEPV = PFEK (ROOT_COMPLEX.SataDevice._ADR, ROOT_COMPLEX_BUS, DState) \
          If (PEPV != 0x0) { \
            If (CondRefOf (ROOT_COMPLEX.SataDevice.Port.POVR)) { \
              PEPV = ROOT_COMPLEX.SataDevice.Port.POVR (PEPV) \
            } \
            If (ROOT_COMPLEX.SataDevice.Port.PRES ()) { \
              Store (Concatenate (#SataDevice, #Port), Local0) \
              PDIN (ROOT_COMPLEX_STR, Local0, PEPV) \
            } \
          } \
        }

//
//  Required Device External Declaration
//
DEVICE_REQUIRED_EXTERNAL (GFX0)
DEVICE_REQUIRED_EXTERNAL (UA00)
DEVICE_REQUIRED_EXTERNAL (UA01)
DEVICE_REQUIRED_EXTERNAL (UA02)
DEVICE_REQUIRED_EXTERNAL (I2C0)
DEVICE_REQUIRED_EXTERNAL (I2C1)
DEVICE_REQUIRED_EXTERNAL (I2C2)
DEVICE_REQUIRED_EXTERNAL (I2C3)
DEVICE_REQUIRED_EXTERNAL (I2C4)
DEVICE_REQUIRED_EXTERNAL (I2C5)
DEVICE_REQUIRED_EXTERNAL (I2C6)
DEVICE_REQUIRED_EXTERNAL (I2C7)
DEVICE_REQUIRED_EXTERNAL (SPI0)
DEVICE_REQUIRED_EXTERNAL (SPI1)
DEVICE_REQUIRED_EXTERNAL (SPI2)
DEVICE_REQUIRED_EXTERNAL (XHCI)
DEVICE_REQUIRED_EXTERNAL (HDAS)
DEVICE_REQUIRED_EXTERNAL (CNVW)
DEVICE_REQUIRED_EXTERNAL (BTPC)
DEVICE_REQUIRED_EXTERNAL (IPU0)
DEVICE_REQUIRED_EXTERNAL (NPU0)
DEVICE_REQUIRED_EXTERNAL (GNA0)
DEVICE_REQUIRED_EXTERNAL (VMD0)
DEVICE_REQUIRED_EXTERNAL (HECI)
DEVICE_REQUIRED_EXTERNAL (HEC3)
DEVICE_REQUIRED_EXTERNAL (GLAN)
DEVICE_REQUIRED_EXTERNAL (THC0)
DEVICE_REQUIRED_EXTERNAL (THC1)
DEVICE_REQUIRED_EXTERNAL (ISHD)
DEVICE_REQUIRED_EXTERNAL (PUF0)
DEVICE_REQUIRED_EXTERNAL (PUF1)
DEVICE_REQUIRED_EXTERNAL (PEMC)
DEVICE_REQUIRED_EXTERNAL (OSE0)
DEVICE_REQUIRED_EXTERNAL (OSE1)
DEVICE_REQUIRED_EXTERNAL (ESE0)

#ifndef _TCSS_DEVICE_PEP_OVERRIDE_METHOD_
#define _TCSS_DEVICE_PEP_OVERRIDE_METHOD_
External (\_SB.PEPD.TSCO, MethodObj)
#endif
#ifndef _TCSS_XHCI_ENABLE_NVS_EXTERNAL_
#define _TCSS_XHCI_ENABLE_NVS_EXTERNAL_
External (THCE, FieldUnitObj)
External (TRE0, FieldUnitObj)
External (TRE1, FieldUnitObj)
External (TRE2, FieldUnitObj)
External (TRE3, FieldUnitObj)
External (DME0, FieldUnitObj)
External (DME1, FieldUnitObj)
#endif
NORMAL_DEVICE_REQUIRED_EXTERNAL (TXHC)
NORMAL_DEVICE_REQUIRED_EXTERNAL (TRP0)
NORMAL_DEVICE_REQUIRED_EXTERNAL (TRP1)
NORMAL_DEVICE_REQUIRED_EXTERNAL (TRP2)
NORMAL_DEVICE_REQUIRED_EXTERNAL (TRP3)
NORMAL_DEVICE_REQUIRED_EXTERNAL (TDM0)
NORMAL_DEVICE_REQUIRED_EXTERNAL (TDM1)

DEVICE_REQUIRED_EXTERNAL (SAT0)
SATA_PORT_DEVICE_REQUIRED_EXTERNAL (SAT0.PRT0)
SATA_PORT_DEVICE_REQUIRED_EXTERNAL (SAT0.PRT1)
SATA_PORT_DEVICE_REQUIRED_EXTERNAL (SAT0.PRT2)
SATA_PORT_DEVICE_REQUIRED_EXTERNAL (SAT0.PRT3)
SATA_PORT_DEVICE_REQUIRED_EXTERNAL (SAT0.PRT4)
SATA_PORT_DEVICE_REQUIRED_EXTERNAL (SAT0.PRT5)
SATA_PORT_DEVICE_REQUIRED_EXTERNAL (SAT0.PRT6)
SATA_PORT_DEVICE_REQUIRED_EXTERNAL (SAT0.PRT7)

#ifndef _PCIE_RP_DEVICE_PEP_OVERRIDE_METHOD_
#define _PCIE_RP_DEVICE_PEP_OVERRIDE_METHOD_
External (\_SB.PEPD.RPCO, MethodObj)
#endif
//
// WRTO is the DSDT NVS for WWAN device type.
//
#ifndef _PCIE_WWAN_RTD3_NVS_EXTERNAL_
#define _PCIE_WWAN_RTD3_NVS_EXTERNAL_
External (WRTO, FieldUnitObj)
#endif
RP_DEVICE_REQUIRED_EXTERNAL (RP01)
RP_DEVICE_REQUIRED_EXTERNAL (RP02)
RP_DEVICE_REQUIRED_EXTERNAL (RP03)
RP_DEVICE_REQUIRED_EXTERNAL (RP04)
RP_DEVICE_REQUIRED_EXTERNAL (RP05)
RP_DEVICE_REQUIRED_EXTERNAL (RP06)
RP_DEVICE_REQUIRED_EXTERNAL (RP07)
RP_DEVICE_REQUIRED_EXTERNAL (RP08)
RP_DEVICE_REQUIRED_EXTERNAL (RP09)
RP_DEVICE_REQUIRED_EXTERNAL (RP10)
RP_DEVICE_REQUIRED_EXTERNAL (RP11)
RP_DEVICE_REQUIRED_EXTERNAL (RP12)
RP_DEVICE_REQUIRED_EXTERNAL (RP13)
RP_DEVICE_REQUIRED_EXTERNAL (RP14)
RP_DEVICE_REQUIRED_EXTERNAL (RP15)
RP_DEVICE_REQUIRED_EXTERNAL (RP16)
RP_DEVICE_REQUIRED_EXTERNAL (RP17)
RP_DEVICE_REQUIRED_EXTERNAL (RP18)
RP_DEVICE_REQUIRED_EXTERNAL (RP19)
RP_DEVICE_REQUIRED_EXTERNAL (RP20)
RP_DEVICE_REQUIRED_EXTERNAL (RP21)
RP_DEVICE_REQUIRED_EXTERNAL (RP22)
RP_DEVICE_REQUIRED_EXTERNAL (RP23)
RP_DEVICE_REQUIRED_EXTERNAL (RP24)
RP_DEVICE_REQUIRED_EXTERNAL (RP25)
RP_DEVICE_REQUIRED_EXTERNAL (RP26)
RP_DEVICE_REQUIRED_EXTERNAL (RP27)
RP_DEVICE_REQUIRED_EXTERNAL (RP28)

//
// PCIe RootPort has endpoint device definition PXSX.
// Using PXSX.PRES to check the endpoint.
// Other PCIe Device would have _ADR.
//
// PR01 ~ PRxx device would be initialized by a common method.
// CDC_MBIM device would be covered if the RPxx.PXSX is WWAN device.
//

//
// Normal PCIe devices List. Need to transfer the DeviceName and
// default d-state value.
//
PEP_DEVICE (GFX0, 3)
PEP_DEVICE (UA00, 3)
PEP_DEVICE (UA01, 3)
PEP_DEVICE (UA02, 3)
PEP_DEVICE (I2C0, 3)
PEP_DEVICE (I2C1, 3)
PEP_DEVICE (I2C2, 3)
PEP_DEVICE (I2C3, 3)
PEP_DEVICE (I2C4, 3)
PEP_DEVICE (I2C5, 3)
PEP_DEVICE (I2C6, 3)
PEP_DEVICE (I2C7, 3)
PEP_DEVICE (SPI0, 3)
PEP_DEVICE (SPI1, 3)
PEP_DEVICE (SPI2, 3)
PEP_DEVICE (XHCI, 3)
PEP_DEVICE (HDAS, 1)
PEP_DEVICE (CNVW, 3)
PEP_DEVICE (BTPC, 3)
PEP_DEVICE (IPU0, 3)
PEP_DEVICE (NPU0, 3)
PEP_DEVICE (GNA0, 3)
PEP_DEVICE (VMD0, 3)
PEP_DEVICE (HECI, 1)
PEP_DEVICE (HEC3, 1)
PEP_DEVICE (GLAN, 3)
PEP_DEVICE (THC0, 3)
PEP_DEVICE (THC1, 3)
PEP_DEVICE (ISHD, 3)
PEP_DEVICE (PUF0, 3)
PEP_DEVICE (PUF1, 3)
PEP_DEVICE (PEMC, 3)
PEP_DEVICE (OSE0, 3)
PEP_DEVICE (OSE1, 3)
PEP_DEVICE (ESE0, 3)

//
// TCSS devices list. Need to transfer the DeviceName and
// the NVS represent device present or not.
//
PEP_DEVICE_TCSS (TXHC, THCE)
PEP_DEVICE_TCSS (TRP0, TRE0)
PEP_DEVICE_TCSS (TRP1, TRE1)
PEP_DEVICE_TCSS (TRP2, TRE2)
PEP_DEVICE_TCSS (TRP3, TRE3)
PEP_DEVICE_TCSS (TDM0, DME0)
PEP_DEVICE_TCSS (TDM1, DME1)

//
// SATA devices and its downstream port. Need to provide
// the Devicename and default d-state value.
PEP_DEVICE (SAT0, 3)
PEP_DEVICE_SATA_PORT (SAT0, PRT0, 3)
PEP_DEVICE_SATA_PORT (SAT0, PRT1, 3)
PEP_DEVICE_SATA_PORT (SAT0, PRT2, 3)
PEP_DEVICE_SATA_PORT (SAT0, PRT3, 3)
PEP_DEVICE_SATA_PORT (SAT0, PRT4, 3)
PEP_DEVICE_SATA_PORT (SAT0, PRT5, 3)
PEP_DEVICE_SATA_PORT (SAT0, PRT6, 3)
PEP_DEVICE_SATA_PORT (SAT0, PRT7, 3)

//
// PCIe RootPort list.
//
PEP_DEVICE_RP (RP01)
PEP_DEVICE_RP (RP02)
PEP_DEVICE_RP (RP03)
PEP_DEVICE_RP (RP04)
PEP_DEVICE_RP (RP05)
PEP_DEVICE_RP (RP06)
PEP_DEVICE_RP (RP07)
PEP_DEVICE_RP (RP08)
PEP_DEVICE_RP (RP09)
PEP_DEVICE_RP (RP10)
PEP_DEVICE_RP (RP11)
PEP_DEVICE_RP (RP12)
PEP_DEVICE_RP (RP13)
PEP_DEVICE_RP (RP14)
PEP_DEVICE_RP (RP15)
PEP_DEVICE_RP (RP16)
PEP_DEVICE_RP (RP17)
PEP_DEVICE_RP (RP18)
PEP_DEVICE_RP (RP19)
PEP_DEVICE_RP (RP20)
PEP_DEVICE_RP (RP21)
PEP_DEVICE_RP (RP22)
PEP_DEVICE_RP (RP23)
PEP_DEVICE_RP (RP24)
PEP_DEVICE_RP (RP25)
PEP_DEVICE_RP (RP26)
PEP_DEVICE_RP (RP27)
PEP_DEVICE_RP (RP28)
