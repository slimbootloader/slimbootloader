/**@file
  ACPI definition for Timed GPIO controller

  Copyright (c) 2021, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent
**/
  //----------------------------
  //  Timed GPIO
  //----------------------------
#include <Register/PmcRegs.h>

Scope (\_SB) {
  Device (TGI0){
    Name(_HID, "INTC1023")
    Name(_STR, Unicode ("Timed GPIO 1"))
    Method (_STA, 0x0, NotSerialized) {
      if(LEqual(TGEA, 0x01)) {
        Return(0x0F)
      } else {
        Return(0x00)
      }
    }
    Method (_CRS, 0x0, NotSerialized) {
      Name(TGRA,ResourceTemplate(){
        Memory32Fixed (ReadWrite, 0x0,0x38, TGA1)
      })
      CreateDWordField(TGRA,TGA1._BAS,TMH1)
      Store( Add(PWRM,R_PMC_PWRM_TGPIOCTL0), TMH1)
      Return(TGRA)
    }
  } // END Device(TGI0)

  Device (TGI1){
    Name(_HID, "INTC1024")
    Name(_STR, Unicode ("Timed GPIO 2"))
    Method (_STA, 0x0, NotSerialized) {
      if(LEqual(TGEB, 0x01)) {
        Return(0x0F)
      } else {
        Return(0x00)
      }
    }
    Method (_CRS, 0x0, NotSerialized) {
      Name(TGRB,ResourceTemplate(){
        Memory32Fixed (ReadWrite, 0x0,0x38, TGA2)
      })
      CreateDWordField(TGRB,TGA2._BAS,TMH2)
      Store( Add(PWRM,R_PMC_PWRM_TGPIOCTL1), TMH2)
      Return(TGRB)
    }
  }// END Device(TGI1)
}// END Scope
