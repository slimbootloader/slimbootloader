/** @file
  ACPI RTD3 SSDT table

  Copyright (c) 2026, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent
**/

External (\_SB.OSCO)
External (\_SB.PC00,DeviceObj)
External (\_SB.PC00.SAT0, DeviceObj)
External (\_SB.PC00.SAT0.PRT0, DeviceObj)
External (\_SB.PC00.SAT0.PRT1, DeviceObj)
External (\_SB.PC00.SAT0.PRT2, DeviceObj)
External (\_SB.PC00.SAT0.PRT3, DeviceObj)
External (\_SB.PC00.SAT0.PRT4, DeviceObj)
External (\_SB.PC00.SAT0.PRT5, DeviceObj)
External (\_SB.PC00.SAT0.PRT6, DeviceObj)
External (\_SB.PC00.SAT0.PRT7, DeviceObj)

Include ("PcieRpObjCommonExternalInclude.asl")

External (\_SB.PC00.RP01.VDID)
External (\_SB.PC00.RP02.VDID)
External (\_SB.PC00.RP03.VDID)
External (\_SB.PC00.RP04.VDID)
External (\_SB.PC00.RP05.VDID)
External (\_SB.PC00.RP06.VDID)
External (\_SB.PC00.RP07.VDID)
External (\_SB.PC00.RP08.VDID)
External (\_SB.PC00.RP09.VDID)
External (\_SB.PC00.RP10.VDID)
External (\_SB.PC00.RP11.VDID)
External (\_SB.PC00.RP12.VDID)
External (\_SB.PC00.RP13.VDID)
External (\_SB.PC00.RP17.VDID)
External (\_SB.PC00.RP21.VDID)

External (\_SB.PC00.RP01.L23D, MethodObj)
External (\_SB.PC00.RP02.L23D, MethodObj)
External (\_SB.PC00.RP03.L23D, MethodObj)
External (\_SB.PC00.RP04.L23D, MethodObj)
External (\_SB.PC00.RP05.L23D, MethodObj)
External (\_SB.PC00.RP06.L23D, MethodObj)
External (\_SB.PC00.RP07.L23D, MethodObj)
External (\_SB.PC00.RP08.L23D, MethodObj)
External (\_SB.PC00.RP09.L23D, MethodObj)
External (\_SB.PC00.RP10.L23D, MethodObj)
External (\_SB.PC00.RP11.L23D, MethodObj)
External (\_SB.PC00.RP12.L23D, MethodObj)
External (\_SB.PC00.RP13.L23D, MethodObj)
External (\_SB.PC00.RP17.L23D, MethodObj)
External (\_SB.PC00.RP21.L23D, MethodObj)

External (\_SB.PC00.RP01.DL23, MethodObj)
External (\_SB.PC00.RP02.DL23, MethodObj)
External (\_SB.PC00.RP03.DL23, MethodObj)
External (\_SB.PC00.RP04.DL23, MethodObj)
External (\_SB.PC00.RP05.DL23, MethodObj)
External (\_SB.PC00.RP06.DL23, MethodObj)
External (\_SB.PC00.RP07.DL23, MethodObj)
External (\_SB.PC00.RP08.DL23, MethodObj)
External (\_SB.PC00.RP09.DL23, MethodObj)
External (\_SB.PC00.RP10.DL23, MethodObj)
External (\_SB.PC00.RP11.DL23, MethodObj)
External (\_SB.PC00.RP12.DL23, MethodObj)
External (\_SB.PC00.RP13.DL23, MethodObj)
External (\_SB.PC00.RP17.DL23, MethodObj)
External (\_SB.PC00.RP21.DL23, MethodObj)

External (\_SB.PC02,DeviceObj)
External (\_SB.PC02.SAT0, DeviceObj)
External (\_SB.PC02.SAT0.PRT0, DeviceObj)
External (\_SB.PC02.SAT0.PRT1, DeviceObj)
External (\_SB.PC02.SAT0.PRT2, DeviceObj)
External (\_SB.PC02.SAT0.PRT3, DeviceObj)
External (\_SB.PC02.SAT0.PRT4, DeviceObj)
External (\_SB.PC02.SAT0.PRT5, DeviceObj)
External (\_SB.PC02.SAT0.PRT6, DeviceObj)
External (\_SB.PC02.SAT0.PRT7, DeviceObj)

External (\_SB.PC02.RP01.VDID)
External (\_SB.PC02.RP05.VDID)
External (\_SB.PC02.RP09.VDID)
External (\_SB.PC02.RP13.VDID)
External (\_SB.PC02.RP21.VDID)

External (\_SB.PC02.RP01.L23D, MethodObj)
External (\_SB.PC02.RP05.L23D, MethodObj)
External (\_SB.PC02.RP09.L23D, MethodObj)
External (\_SB.PC02.RP13.L23D, MethodObj)
External (\_SB.PC02.RP21.L23D, MethodObj)

External (\_SB.PC02.RP01.DL23, MethodObj)
External (\_SB.PC02.RP05.DL23, MethodObj)
External (\_SB.PC02.RP09.DL23, MethodObj)
External (\_SB.PC02.RP13.DL23, MethodObj)
External (\_SB.PC02.RP21.DL23, MethodObj)

External (\_SB.PC00.XHCI, DeviceObj)

External (\_SB.PC02.XDCI, DeviceObj)
External (\_SB.PC00.XDCI, DeviceObj)

External (GBES)
If (LNotEqual (GBES,0)) {
  External (\_SB.PC00.GLAN, DeviceObj)
  External (\_SB.PC02.GLAN, DeviceObj)
}
External (\_SB.PC00.XHCI.MEMB)
External (\_SB.PC02.XDCI.D0I3)
External (\_SB.PC00.XDCI.D0I3)
// Standalone XDCI Controller
External (XDCE)
External (\_SB.PC02.XDCI.DVID)
External (\_SB.PC02.XDCI.XDCB)
External (\_SB.PC02.XDCI.XDBA, MethodObj)

External (\_SB.PC00.XDCI.DVID)
External (\_SB.PC00.XDCI.XDCB)
External (\_SB.PC00.XDCI.XDBA, MethodObj)

External (\GBEP)
External (\_SB.PC00.XHCI.RHUB, DeviceObj)
External (\_SB.PC00.XHCI.RHUB.SS01, DeviceObj)
External (\_SB.PC00.XHCI.RHUB.SS02, DeviceObj)
External (\_SB.PC00.XHCI.RHUB.HS01, DeviceObj) //xHCI HSP port 1
External (\_SB.PC00.XHCI.RHUB.HS02, DeviceObj) //xHCI HSP port 2

External (GBES)
External (\_SB.PC00.GLAN, DeviceObj)
External (\_SB.PC02.GBES)
External (\_SB.PC02.GLAN, DeviceObj)

External (PEP0)
External (XHPR)
External (RCG0, IntObj)     // RTD3 Config Setting0(BIT0:Reserved, BIT1:Reserved, Bit3: BT, BIT6:Card Reader, BIT8:WSB SIP FAB1 Card reader)
External (RCG1, IntObj)     // RTD3 Config Setting1(BIT0:Sata Port0, BIT1:Sata Port1, BIT2:Sata Port2, BIT3:Sata Port3)
External (\GPRW, MethodObj)
External (P8XH, MethodObj)
External (XDST, IntObj)

//
// Externs common to ULT0RTD3.asl and FFRDRTD3.asl and exclude for BRRTD3.asl
//

// GPIO methods
External (\_SB.SPMV, MethodObj)
External (\_SB.SGRA, MethodObj)
External (\_SB.SGWP, MethodObj)
External (\_SB.GGIV, MethodObj)
External (\_SB.GGOV, MethodObj)
External (\_SB.SGOV, MethodObj)
External (\_SB.SGII, MethodObj)
External (\_SB.CAGS, MethodObj)
External (\_SB.ISME, MethodObj)
External (\_SB.PGPI.IGPI, MethodObj)
External (\_SB.PGPI.CGPI, MethodObj)

// HDAS Controller
External(\_SB.PC00.HDAS, DeviceObj)
External(\_SB.PC00.HDAS.VDID)
External (\UAMS)

// RTD3 devices and variables
External (\_SB.PC00.I2C0, DeviceObj)           //I2C0 Controller
External (\_SB.PC00.I2C0.TPD0, DeviceObj)      // Touch pad
External (\_SB.PC00.I2C1, DeviceObj)           //I2C1 Controller
External (\_SB.PC00.I2C1.TPL1, DeviceObj)      // Touch panel

External (SDS0,FieldUnitObj)
External (SDS1,FieldUnitObj)
External (IC1D,FieldUnitObj)
External (IC1S,FieldUnitObj)
External (IC0D,FieldUnitObj)
External (AUDD,FieldUnitObj)
External (SHSB,FieldUnitObj)
External (VRRD,FieldUnitObj)

External (SPCO,MethodObj) // Clock Req Enable/Disable

External (\_SB.PCLK.SPCO, MethodObj)

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