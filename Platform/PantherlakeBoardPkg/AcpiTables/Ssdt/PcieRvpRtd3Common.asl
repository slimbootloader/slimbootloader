 /** @file
  Intel RVP RTD3 common code

  Copyright (c) 2026, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

// Board configuration PTL rvp1  Default topology Rvp1
// ____________________________________________________________________________________________
// | RP Number | config | End Point                                 | ClkSrc# | Lane Reversal |
// ____________________________________________________________________________________________
// |   RP01    |   x1   | GBE LAN                                   |   3     |  No           |
// |   RP03    |   x1   | x1 PCIe Slot                              |   2     |  No           |
// |   RP04    |   x1   | M.2 WLAN                                  |   4     |  No           |
// |   RP05    |   x4   | Gen4 M.2 SSD                              |   6     |  No           |
// |   RP09    |   x4   | Gen5 M.2 SSD/ X4 pcieslot(Rework))        |   1     |  No           |
// |   RP11    |   x8   | x8 PCIe Slot                              |   0     |  No           |
// ____________________________________________________________________________________________
//
// Board configuration PTL  Default topology Rvp3
// ____________________________________________________________________________________________
// | RP Number | config | End Point                                 | ClkSrc# | Lane Reversal |
// ____________________________________________________________________________________________
// |   RP01    |   x1   | GBE LAN                                   |   3     |  Yes          |
// |   RP03    |   x1   | x2 PCIe Slot                              |   2     |  Yes          |
// |   RP05    |   x4   | Gen4 M.2 SSD                              |   6     |  No           |
// |   RP09    |   x4   | Gen5 M.2 SSD                              |   1     |  No           |
// |   RP11    |   x8   | x8 PCIe Slot (X4 pcie slot rework)        |   0     |  No           |
// |   rp12    |   X4   | X4 pcie slot                              |   7     |  No           |
// ____________________________________________________________________________________________
//

// Board configuration PTL  Default topology Rvp4
// ____________________________________________________________________________________________
// | RP Number | config | End Point                                 | ClkSrc# | Lane Reversal |
// ____________________________________________________________________________________________
// |   RP01    |   x1   | GBE LAN                                   |   3     |  Yes          |
// |   RP03    |   x1   | x2 PCIe Slot                              |   2     |  Yes          |
// |   RP05    |   x4   | Gen4 M.2 SSD                              |   6     |  No           |
// |   RP09    |   x4   | Gen5 M.2 SSD                              |   1     |  No           |
// |   RP11    |   x8   | x8 PCIe Slot                              |   0     |  Yes          |
// ____________________________________________________________________________________________
//
External (RW04)
Include ("Rtd3Common.asl")
External (PRST)

//
// PCIe Slot 1 (x1 RP03)
//
External (P1RN)
External (P1PG)
External (P1EP)
External (P1RG)
External (P1RP)
External (RW03)
External (RW01)

//
// Gen4 M.2 SSD (x4 RP05)
//
External (SSDP)
External (SDPP)
External (SSDR)
External (SDRP)
External (RW05)
External (\_SB.PC00.RP05.PXSX, DeviceObj)
External (\_SB.PC00.RP05.PXSX.PNVM, MethodObj)
External (\_SB.PC00.RP05.PXSX.PAHC, MethodObj)

//
// Gen5 M.2 SSD (RP09)
//
External (SD2P)
External (SDP1)
External (SD2R)
External (SDR1)
External (RW09)
External (\_SB.PC00.RP09.PXSX, DeviceObj)
External (\_SB.PC00.RP09.PXSX.PNVM, MethodObj)
External (\_SB.PC00.RP09.PXSX.PAHC, MethodObj)

//
// PCIe Slot 2 (x8/x4 RP11)
//
External (P2PG)
External (P2EP)
External (P2RG)
External (P2RP)
External (RW11)
External (RW02)
External (P2RN)

//
// PCIe Slot 2 (x8 RP12)
//
External (P3PG)
External (P3EP)
External (P3RG)
External (P3RP)
External (RW12)
External (P3RN)

#define PCIE_RP_SCOPE_BEGIN(Device_Object) \
  If (CondRefOf (Device_Object)) { \
    Scope (Device_Object) {

#define PCIE_RP_SCOPE_BODY(SlotPowerPin,SlotPowerPolarity,SlotResetPin,SlotResetPolarity,SlotWakePin,Slotclock) \
  Name (RSTG, Package () {0, 0})\
  Store (SlotResetPin, Index (RSTG, 0)) \
  Store (SlotResetPolarity, Index (RSTG, 1)) \
  Name (PWRG, Package () {0, 0}) \
  Store (SlotPowerPin, Index (PWRG, 0)) \
  Store (SlotPowerPolarity, Index (PWRG, 1)) \
  Name (WAKG, 0) \
  Store (SlotWakePin, WAKG) \
  Name (SCLK,0) \
  Store (Slotclock,SCLK) \
  Name (WAKP, 0) \

#define PCIE_RP_SCOPE_END } \
  }

#define PCIE_SSD_EP_SCOPE_BEGIN(Device_Object) \
 If (LOr(Device_Object.PXSX.PNVM(),Device_Object.PXSX.PAHC())) { \
  Scope (Device_Object.PXSX) {

#define PCIE_SSD_EP_SCOPE_END } \
 }

#define PCIE_RP_L6F_WAKE(PCIE_SLOT_ROOT_PORT) \
  If (CondRefOf(PCIE_SLOT_ROOT_PORT.WAKG)) { \
    If (LNotEqual(PCIE_SLOT_ROOT_PORT.WAKG, 0)) { \
      If (\_SB.ISME (PCIE_SLOT_ROOT_PORT.WAKG)) { \
        ADBG (Concatenate (" Wake event  ", PCIE_SLOT_ROOT_PORT.WAKG)) \
        \_SB.SHPO (PCIE_SLOT_ROOT_PORT.WAKG, 1) \
        Notify (PCIE_SLOT_ROOT_PORT, 0x02) \
        \_SB.CAGS (PCIE_SLOT_ROOT_PORT.WAKG) \
      } \
    } \
  }
