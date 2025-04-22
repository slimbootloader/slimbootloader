/**@file
  ACPI definition for Timed GPIO controller

 Copyright (c) 2018 - 2019, Intel Corporation. All rights reserved.<BR>
 SPDX-License-Identifier: BSD-2-Clause-Patent
**/
  //----------------------------
  //  Timed GPIO
  //----------------------------
#include <Register/PmcRegs.h>

Scope(\_SB) {

  If (LEqual(TGEA,1)) {
    Device (TGIA){
      Name(_HID, "INTC1021")
      Name(_STR, Unicode ("Timed GPIO 1"))
      Method (_STA, 0x0, NotSerialized) {
          Return (0xF)
      }
      Method (_CRS, 0x0, Serialized) {
        Name(TGRA,ResourceTemplate() {
          Memory32Fixed (ReadWrite, 0x0,0x38, TGA1)
        })
        CreateDWordField(TGRA,TGA1._BAS,TMH1)
        Store( Add(PWRM,R_PMC_PWRM_TIMED_GPIO_CONTROL_0), TMH1)
        Return(TGRA)
      }
    } // END Device(TGIA)
  } // end if TGEA

  If (LEqual(TGEB,1)) {
    Device (TGIB){
      Name(_HID, "INTC1022")
      Name(_STR, Unicode ("Timed GPIO 2"))
      Method (_STA, 0x0, NotSerialized) {
          Return (0xF)
      }
      Method (_CRS, 0x0, Serialized) {
        Name(TGRB,ResourceTemplate() {
          Memory32Fixed (ReadWrite, 0x0,0x38, TGA2)
        })
        CreateDWordField(TGRB,TGA2._BAS,TMH2)
        Store( Add(PWRM,R_PMC_PWRM_TIMED_GPIO_CONTROL_1), TMH2)
        Return(TGRB)
      }
    } // END Device(TGIB)
  } // end if TGEB
} // END Scope
