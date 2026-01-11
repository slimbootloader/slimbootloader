/** @file
  ACPI Common Include File for PCIE RP ACPI Objects.

  Copyright (c) 2026, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent
**/

#include <PcieRpMacroDefinition.h>

ACPI_DEBUG_EXTERNAL_REFERENCE

//
// MACRO Definations for Printing PCIE RP Info: Start
// Note: These below MACRO will only work under PCIE RP Scope.
//
#define GET_STRING_PCIE_RP_INDEX_FROM_SLOT_VAR   Concatenate(" And Index : ", ToHexString(SLOT))
#define GET_STRING_PCIE_RP_TYPE_FROM_PRTP_VAR    Concatenate(" Type (2: PCH, 4: CPU) : ", ToHexString(PRTP))
#define GET_STRING_PCIE_RP_INFO_PRTP_SLOT        Concatenate(GET_STRING_PCIE_RP_TYPE_FROM_PRTP_VAR, GET_STRING_PCIE_RP_INDEX_FROM_SLOT_VAR)
#define GET_STRING_PCIE_RP_INFO                  Concatenate("PCIE RP", GET_STRING_PCIE_RP_INFO_PRTP_SLOT)
#define ADBG_PRINT_PCIE_RP_INFO_AFTER(I_STRING)  ADBG(Concatenate(I_STRING, GET_STRING_PCIE_RP_INFO))
#define ADBG_PRINT_PCIE_RP_INFO_BEFORE(I_STRING) ADBG(Concatenate(GET_STRING_PCIE_RP_INFO, I_STRING))
//
// Note: These above MACRO will only work under PCIE RP Scope.
// MACRO Definations for Printing PCIE RP Info: End
//

External (\_SB.PC00,DeviceObj)
// External Variable for PCIE Rp Device Objects
External (\_SB.PC00.RP01, DeviceObj)
External (\_SB.PC00.RP02, DeviceObj)
External (\_SB.PC00.RP03, DeviceObj)
External (\_SB.PC00.RP04, DeviceObj)
External (\_SB.PC00.RP05, DeviceObj)
External (\_SB.PC00.RP06, DeviceObj)
External (\_SB.PC00.RP07, DeviceObj)
External (\_SB.PC00.RP08, DeviceObj)
External (\_SB.PC00.RP09, DeviceObj)
External (\_SB.PC00.RP10, DeviceObj)
External (\_SB.PC00.RP11, DeviceObj)
External (\_SB.PC00.RP12, DeviceObj)

// External Variable holding PCIE Rp Index Number
External (\_SB.PC00.RP01.SLOT, IntObj)
External (\_SB.PC00.RP02.SLOT, IntObj)
External (\_SB.PC00.RP03.SLOT, IntObj)
External (\_SB.PC00.RP04.SLOT, IntObj)
External (\_SB.PC00.RP05.SLOT, IntObj)
External (\_SB.PC00.RP06.SLOT, IntObj)
External (\_SB.PC00.RP07.SLOT, IntObj)
External (\_SB.PC00.RP08.SLOT, IntObj)
External (\_SB.PC00.RP09.SLOT, IntObj)
External (\_SB.PC00.RP10.SLOT, IntObj)
External (\_SB.PC00.RP11.SLOT, IntObj)
External (\_SB.PC00.RP12.SLOT, IntObj)

// External Variable holding PCIE Rp Type Information
External (\_SB.PC00.RP01.PRTP, IntObj)
External (\_SB.PC00.RP02.PRTP, IntObj)
External (\_SB.PC00.RP03.PRTP, IntObj)
External (\_SB.PC00.RP04.PRTP, IntObj)
External (\_SB.PC00.RP05.PRTP, IntObj)
External (\_SB.PC00.RP06.PRTP, IntObj)
External (\_SB.PC00.RP07.PRTP, IntObj)
External (\_SB.PC00.RP08.PRTP, IntObj)
External (\_SB.PC00.RP09.PRTP, IntObj)
External (\_SB.PC00.RP10.PRTP, IntObj)
External (\_SB.PC00.RP11.PRTP, IntObj)
External (\_SB.PC00.RP12.PRTP, IntObj)

External (\_SB.PC02, DeviceObj)
External (\_SB.PC02.RP01, DeviceObj)
External (\_SB.PC02.RP05, DeviceObj)
External (\_SB.PC02.RP08, DeviceObj)
External (\_SB.PC02.RP09, DeviceObj)
External (\_SB.PC02.RP13, DeviceObj)
External (\_SB.PC02.RP21, DeviceObj)

External (\_SB.PC02.RP01.SLOT, IntObj)
External (\_SB.PC02.RP05.SLOT, IntObj)
External (\_SB.PC02.RP08.SLOT, IntObj)
External (\_SB.PC02.RP09.SLOT, IntObj)
External (\_SB.PC02.RP13.SLOT, IntObj)
External (\_SB.PC02.RP21.SLOT, IntObj)

External (\_SB.PC02.RP01.PRTP, IntObj)
External (\_SB.PC02.RP05.PRTP, IntObj)
External (\_SB.PC02.RP08.PRTP, IntObj)
External (\_SB.PC02.RP09.PRTP, IntObj)
External (\_SB.PC02.RP13.PRTP, IntObj)
External (\_SB.PC02.RP21.PRTP, IntObj)

External (\_SB.PC00.RP01.RD3C, IntObj)
External (\_SB.PC00.RP02.RD3C, IntObj)
External (\_SB.PC00.RP03.RD3C, IntObj)
External (\_SB.PC00.RP04.RD3C, IntObj)
External (\_SB.PC00.RP05.RD3C, IntObj)
External (\_SB.PC00.RP06.RD3C, IntObj)
External (\_SB.PC00.RP07.RD3C, IntObj)
External (\_SB.PC00.RP08.RD3C, IntObj)
External (\_SB.PC00.RP09.RD3C, IntObj)
External (\_SB.PC00.RP10.RD3C, IntObj)
External (\_SB.PC00.RP11.RD3C, IntObj)
External (\_SB.PC00.RP12.RD3C, IntObj)

External (\_SB.PC00.RP01.PRMV, IntObj)
External (\_SB.PC00.RP02.PRMV, IntObj)
External (\_SB.PC00.RP03.PRMV, IntObj)
External (\_SB.PC00.RP04.PRMV, IntObj)
External (\_SB.PC00.RP05.PRMV, IntObj)
External (\_SB.PC00.RP06.PRMV, IntObj)
External (\_SB.PC00.RP07.PRMV, IntObj)
External (\_SB.PC00.RP08.PRMV, IntObj)
External (\_SB.PC00.RP09.PRMV, IntObj)
External (\_SB.PC00.RP10.PRMV, IntObj)
External (\_SB.PC00.RP11.PRMV, IntObj)
External (\_SB.PC00.RP12.PRMV, IntObj)
