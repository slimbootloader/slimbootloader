/** @file

  Copyright (c) 2020, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#define PIN_GPIO_PAD_INDEX  0
#define PIN_GPIO_POLARITY_INDEX  1

//
// This library provides functions that configure pin states.
//
// GPIO package definition:
// Package() {
//   GpioPad : IntObj
//   GpioActiveStatePolarity : IntObj
//   }
//

//
// GPIO library imports
//

Scope(\) {

  //
  // Note this is not a real power resource we simply use this to declare new named scope
  //
  PowerResource(PIN, 0, 0) {
    //
    // Check pin status function
    // @param[in]  Arg0  Pin descriptor package
    //
    // @return GPIO pin status relative to GpioActiveStatePolarity value
    //
    Method(STA, 1, Serialized) {
      If(LEqual(\_SB.GGOV(DeRefOf(Index(Arg0, PIN_GPIO_PAD_INDEX))), DeRefOf(Index(Arg0, PIN_GPIO_POLARITY_INDEX)))) {
        Return(1)
      } Else {
        Return(0)
      }
    }

    //
    // Drive pin to active state function.
    // @param[in]  Arg0  Pin descriptor package
    //
    Method(ON, 1, Serialized) {
      \_SB.SGOV(DeRefOf(Index(Arg0, PIN_GPIO_PAD_INDEX)), DeRefOf(Index(Arg0, PIN_GPIO_POLARITY_INDEX)))
    }

    //
    // Drive pin to inactive state.
    // @param[in]  Arg0  Pin descriptor package
    //
    Method(OFF, 1, Serialized) {
      \_SB.SGOV(DeRefOf(Index(Arg0, PIN_GPIO_PAD_INDEX)), Not(DeRefOf(Index(Arg0, PIN_GPIO_POLARITY_INDEX))))
    }

    // Dummy standard methods for OSPM.
    // PowerResource is required to implement those.
    Method(_STA) {Return(0)}
    Method(_ON) {}
    Method(_OFF) {}
  }
}

