## @file
#
#  Copyright (c) 2018, Intel Corporation. All rights reserved.<BR>
#  SPDX-License-Identifier: BSD-2-Clause-Patent
#
##

  # !BSF PAGES:{PLT_PIDGPIO:PLT:"Platform ID GPIO Pins"}
  # !BSF PAGE:{PLT_PIDGPIO}
  # !BSF SUBT:{CFGHDR_TMPL:PID_GPIO_CFG_DATA:0:0}

  # !HDR EMBED:{PID_GPIO_CFG_DATA:TAG_800:START}

  # !BSF SUBT:{PIDGPIO_TMPL: PidPin0 : 0x0000F0C4}
  # !BSF SUBT:{PIDGPIO_TMPL: PidPin1 : 0x0000D0C4}
  # !BSF SUBT:{PIDGPIO_TMPL: PidPin2 : 0x0000C8C4}
  # !BSF SUBT:{PIDGPIO_TMPL: PidPin3 : 0x0000C0C4}

  # !HDR EMBED:{PID_GPIO_CFG_DATA:TAG_800:END}