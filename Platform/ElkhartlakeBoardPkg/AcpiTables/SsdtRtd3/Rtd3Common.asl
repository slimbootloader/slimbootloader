/** @file
  ACPI RTD3 SSDT table

  Copyright (c) 2011 - 2021, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent
**/

External(\_SB.OSCO)
External(\_SB.PC00,DeviceObj)
External(\_SB.PC00.SAT0, DeviceObj)
External(\_SB.PC00.SAT0.PRT0, DeviceObj)
External(\_SB.PC00.SAT0.PRT1, DeviceObj)
External(\_SB.PC00.SAT0.PRT2, DeviceObj)
External(\_SB.PC00.SAT0.PRT3, DeviceObj)
External(\_SB.PC00.SAT0.PRT4, DeviceObj)
External(\_SB.PC00.SAT0.PRT5, DeviceObj)

External(\_SB.PC00.RP01, DeviceObj)
External(\_SB.PC00.RP02, DeviceObj)
External(\_SB.PC00.RP03, DeviceObj)
External(\_SB.PC00.RP04, DeviceObj)
External(\_SB.PC00.RP05, DeviceObj)
External(\_SB.PC00.RP06, DeviceObj)
External(\_SB.PC00.RP07, DeviceObj)

External(\_SB.PC00.RP01.VDID)
External(\_SB.PC00.RP02.VDID)
External(\_SB.PC00.RP03.VDID)
External(\_SB.PC00.RP04.VDID)
External(\_SB.PC00.RP05.VDID)
External(\_SB.PC00.RP06.VDID)
External(\_SB.PC00.RP07.VDID)

External(\_SB.PC00.RP01.SLOT)
External(\_SB.PC00.RP02.SLOT)
External(\_SB.PC00.RP03.SLOT)
External(\_SB.PC00.RP04.SLOT)
External(\_SB.PC00.RP05.SLOT)
External(\_SB.PC00.RP06.SLOT)
External(\_SB.PC00.RP07.SLOT)

External(\_SB.PC00.RP01.L23D, MethodObj)
External(\_SB.PC00.RP02.L23D, MethodObj)
External(\_SB.PC00.RP03.L23D, MethodObj)
External(\_SB.PC00.RP04.L23D, MethodObj)
External(\_SB.PC00.RP05.L23D, MethodObj)
External(\_SB.PC00.RP06.L23D, MethodObj)
External(\_SB.PC00.RP07.L23D, MethodObj)

External(\_SB.PC00.RP01.DL23, MethodObj)
External(\_SB.PC00.RP02.DL23, MethodObj)
External(\_SB.PC00.RP03.DL23, MethodObj)
External(\_SB.PC00.RP04.DL23, MethodObj)
External(\_SB.PC00.RP05.DL23, MethodObj)
External(\_SB.PC00.RP06.DL23, MethodObj)
External(\_SB.PC00.RP07.DL23, MethodObj)

External(\_SB.PC00.XHCI, DeviceObj)
External(\_SB.PC00.XDCI, DeviceObj)
External(GBES)
If (LNotEqual(GBES,0)) {
  External(\_SB.PC00.GLAN, DeviceObj)
}
External(\_SB.PC00.XHCI.MEMB)
External(\_SB.PC00.XDCI.D0I3)
External(\_SB.PC00.XDCI.XDCB)
External(\GBEP)
External(\_SB.PC00.XHCI.RHUB, DeviceObj)
External(\_SB.PC00.XHCI.RHUB.SS01, DeviceObj)
External(\_SB.PC00.XHCI.RHUB.SS02, DeviceObj)
External(\_SB.PC00.XHCI.RHUB.HS01, DeviceObj) //xHCI HSP port 1
External(\_SB.PC00.XHCI.RHUB.HS02, DeviceObj) //xHCI HSP port 2

External(PEP0)
External(XHPR)
External(RCG0, IntObj)     // RTD3 Config Setting0(BIT0:Reserved, BIT1:Reserved, BIT6:Card Reader, BIT7:WWAN, BIT8:WSB SIP FAB1 Card reader)
External(RCG1, IntObj)     // RTD3 Config Setting1(BIT0:Sata Port0, BIT1:Sata Port1, BIT2:Sata Port2, BIT3:Sata Port3)
External(\GPRW, MethodObj)
External(P8XH, MethodObj)
External(XDST, IntObj)
External(\SPST, IntObj)
//
// Externs common to ULT0RTD3.asl and FFRDRTD3.asl and exclude for BRRTD3.asl
//

// GPIO methods
External(\_SB.SPMV, MethodObj)
External(\_SB.SGRA, MethodObj)
External(\_SB.SGWP, MethodObj)
External(\_SB.GGIV, MethodObj)
External(\_SB.GGOV, MethodObj)
External(\_SB.SGOV, MethodObj)
External(\_SB.SGII, MethodObj)
External(\_SB.CAGS, MethodObj)
External(\_SB.ISME, MethodObj)


// RTD3 devices and variables
External(\_SB.PC00.I2C0, DeviceObj) //I2C0 Controller
External(\_SB.PC00.I2C0.TPD0, DeviceObj) // Touch pad
External(\_SB.PC00.I2C1, DeviceObj) //I2C1 Controller
External(\_SB.PC00.I2C1.TPL1, DeviceObj) // Touch panel
External(\UAMS)
External(SDS0,FieldUnitObj)
External(SDS1,FieldUnitObj)
External(AUDD,FieldUnitObj)
External(SHSB,FieldUnitObj)
External(VRRD,FieldUnitObj)

External(SPCO,MethodObj)

External(\_SB.CSD0, MethodObj)
External(\_SB.CSD3, MethodObj)
External(DVID)

include("GpioAcpiDefinesVer1.h")
include("HsioDefine.asl")

#define RCG1_RTD3_PRT0_ENABLED 0x01
#define RCG1_RTD3_PRT1_ENABLED 0x02
#define RCG1_RTD3_PRT2_ENABLED 0x04
#define RCG1_RTD3_PRT3_ENABLED 0x08
#define RCG1_RTD3_PRT4_ENABLED 0x10
#define RCG1_RTD3_PRT5_ENABLED 0x20
#define RCG1_RTD3_NVM1_ENABLED 0x40
#define RCG1_RTD3_NVM2_ENABLED 0x80
#define RCG1_RTD3_NVM3_ENABLED 0x100

//
// GPIO states mapping
//
#define PIN_GPIO_ACTIVE_LOW 0
#define PIN_GPIO_ACTIVE_HIGH 1

//
// Touch panel bus speed
//
#define TCH_PNL_BUS_SPEED_400K  1
#define TCH_PNL_BUS_SPEED_1M    2
