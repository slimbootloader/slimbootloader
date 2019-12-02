## @file
#
#  Copyright (c) 2018, Intel Corporation. All rights reserved.<BR>
#  SPDX-License-Identifier: BSD-2-Clause-Patent
#
##

  #
  # ========================= USB_TMPL ==========================
  #


  # !BSF DEFT:{USB_TMPL:START}

    # !BSF NAME:{USB Over Current Pin Control}
    # !BSF HELP:{Over Current Pin Settings for USB 2/3 ports}
    # !HDR STRUCT:{USB_OC_PIN_CTRL}
    gCfgData.OverCurrentPin                   |      * | 0x04 | $(1)

      # !BSF NAME:{USB 2.0 Port 0 Over Current Pin} TYPE:{Combo} OPTION:{0x0:Pin0, 0x1:Pin1, 0x2:PinSkip}
      # !BSF HELP:{USB 2.0 Port 0 Over Current Pin Control setting. 0x0:Pin0, 0x1:Pin1, 0x2:PinSkip}
      # !BSF FIELD:{PortUs20bOverCurrentPin0:2b}

      # !BSF NAME:{USB 2.0 Port 1 Over Current Pin} TYPE:{Combo} OPTION:{0x0:Pin0, 0x1:Pin1, 0x2:PinSkip}
      # !BSF HELP:{USB 2.0 Port 1 Over Current Pin Control setting. 0x0:Pin0, 0x1:Pin1, 0x2:PinSkip}
      # !BSF FIELD:{PortUs20bOverCurrentPin1:2b}

      # !BSF NAME:{USB 2.0 Port 2 Over Current Pin} TYPE:{Combo} OPTION:{0x0:Pin0, 0x1:Pin1, 0x2:PinSkip}
      # !BSF HELP:{USB 2.0 Port 2 Over Current Pin Control setting. 0x0:Pin0, 0x1:Pin1, 0x2:PinSkip}
      # !BSF FIELD:{PortUs20bOverCurrentPin2:2b}

      # !BSF NAME:{USB 2.0 Port 3 Over Current Pin} TYPE:{Combo} OPTION:{0x0:Pin0, 0x1:Pin1, 0x2:PinSkip}
      # !BSF HELP:{USB 2.0 Port 3 Over Current Pin Control setting. 0x0:Pin0, 0x1:Pin1, 0x2:PinSkip}
      # !BSF FIELD:{PortUs20bOverCurrentPin3:2b}

      # !BSF NAME:{USB 2.0 Port 4 Over Current Pin} TYPE:{Combo} OPTION:{0x0:Pin0, 0x1:Pin1, 0x2:PinSkip}
      # !BSF HELP:{USB 2.0 Port 4 Over Current Pin Control setting. 0x0:Pin0, 0x1:Pin1, 0x2:PinSkip}
      # !BSF FIELD:{PortUs20bOverCurrentPin4:2b}

      # !BSF NAME:{USB 2.0 Port 5 Over Current Pin} TYPE:{Combo} OPTION:{0x0:Pin0, 0x1:Pin1, 0x2:PinSkip}
      # !BSF HELP:{USB 2.0 Port 5 Over Current Pin Control setting. 0x0:Pin0, 0x1:Pin1, 0x2:PinSkip}
      # !BSF FIELD:{PortUs20bOverCurrentPin5:2b}

      # !BSF NAME:{USB 2.0 Port 6 Over Current Pin} TYPE:{Combo} OPTION:{0x0:Pin0, 0x1:Pin1, 0x2:PinSkip}
      # !BSF HELP:{USB 2.0 Port 6 Over Current Pin Control setting. 0x0:Pin0, 0x1:Pin1, 0x2:PinSkip}
      # !BSF FIELD:{PortUs20bOverCurrentPin6:2b}

      # !BSF NAME:{USB 2.0 Port 7 Over Current Pin} TYPE:{Combo} OPTION:{0x0:Pin0, 0x1:Pin1, 0x2:PinSkip}
      # !BSF HELP:{USB 2.0 Port 7 Over Current Pin Control setting. 0x0:Pin0, 0x1:Pin1, 0x2:PinSkip}
      # !BSF FIELD:{PortUs20bOverCurrentPin7:2b}


      # !BSF NAME:{USB 3.0 Port 0 Over Current Pin} TYPE:{Combo} OPTION:{0x0:Pin0, 0x1:Pin1, 0x2:PinSkip}
      # !BSF HELP:{USB 3.0 Port 0 Over Current Pin Control setting. 0x0:Pin0, 0x1:Pin1, 0x2:PinSkip}
      # !BSF FIELD:{PortUs30bOverCurrentPin0:2b}

      # !BSF NAME:{USB 3.0 Port 1 Over Current Pin} TYPE:{Combo} OPTION:{0x0:Pin0, 0x1:Pin1, 0x2:PinSkip}
      # !BSF HELP:{USB 3.0 Port 1 Over Current Pin Control setting. 0x0:Pin0, 0x1:Pin1, 0x2:PinSkip}
      # !BSF FIELD:{PortUs30bOverCurrentPin1:2b}

      # !BSF NAME:{USB 3.0 Port 2 Over Current Pin} TYPE:{Combo} OPTION:{0x0:Pin0, 0x1:Pin1, 0x2:PinSkip}
      # !BSF HELP:{USB 3.0 Port 2 Over Current Pin Control setting. 0x0:Pin0, 0x1:Pin1, 0x2:PinSkip}
      # !BSF FIELD:{PortUs30bOverCurrentPin2:2b}

      # !BSF NAME:{USB 3.0 Port 3 Over Current Pin} TYPE:{Combo} OPTION:{0x0:Pin0, 0x1:Pin1, 0x2:PinSkip}
      # !BSF HELP:{USB 3.0 Port 3 Over Current Pin Control setting. 0x0:Pin0, 0x1:Pin1, 0x2:PinSkip}
      # !BSF FIELD:{PortUs30bOverCurrentPin3:2b}

      # !BSF NAME:{USB 3.0 Port 4 Over Current Pin} TYPE:{Combo} OPTION:{0x0:Pin0, 0x1:Pin1, 0x2:PinSkip}
      # !BSF HELP:{USB 3.0 Port 4 Over Current Pin Control setting. 0x0:Pin0, 0x1:Pin1, 0x2:PinSkip}
      # !BSF FIELD:{PortUs30bOverCurrentPin4:2b}

      # !BSF NAME:{USB 3.0 Port 5 Over Current Pin} TYPE:{Combo} OPTION:{0x0:Pin0, 0x1:Pin1, 0x2:PinSkip}
      # !BSF HELP:{USB 3.0 Port 5 Over Current Pin Control setting. 0x0:Pin0, 0x1:Pin1, 0x2:PinSkip}
      # !BSF FIELD:{PortUs30bOverCurrentPin5:2b}


      # !BSF NAME:{Reserved} TYPE:{Reserved}
      # !BSF HELP:{Reserved}
      # !BSF FIELD:{Rsvd:4b}

  # !BSF DEFT:{USB_TMPL:END}
