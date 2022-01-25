/** @file

  Copyright (c) 2020, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/
//
// Intel Display Audio Controllers definition
//
Scope(HDAS)
{
  //
  // Address (_ADR) encoding:
  // Bits 28-31 - Link Type
  // 0 = HD-Audio, 1 = DSP, 2 = PDM, 3 = SSP, 4 = SoundWire, 5 = SIO / USB Audio Offload
  // Bits 0-3 - Device Instance ID (unique to virtual bus).

  //
  // Intel Display Audio
  //
  Device(IDA) {
    Name(_ADR, 0x10000000)
  } // END Device(IDA)

} // END Scope(HDAS)
