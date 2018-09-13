## @file
#
#  Copyright (c) 2018, Intel Corporation. All rights reserved.<BR>
#  This program and the accompanying materials
#  are licensed and made available under the terms and conditions of the BSD License
#  which accompanies this distribution.  The full text of the license may be found at
#  http://opensource.org/licenses/bsd-license.php
#
#  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
#  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.
#
##

  #
  # ========================= PCIERP_TMPL & PCIERP_CTRL_PIN_TMPL ==========================
  #
    =======================================================================================
  # !BSF DEFT:{PCIERP_TMPL:START}
    # !BSF PAGES:{PLT_PCIE_RP_FEAT_$(1):PLT_PCIE_RP_FEAT:"PCIE RP $(1) Features"}
    # !BSF PAGE:{PLT_PCIE_RP_FEAT_$(1)}
    # !BSF NAME:{PCIE RP $(1) Config Data}
    # !HDR STRUCT:{PCIE_RP_FEAT[]}
    gCfgData.PcieRpFeatures$(1)                   |     * | 0x01 | $(2)

      # !BSF NAME:{PCIE RP Enable}
      # !BSF TYPE:{Combo} OPTION:{$EN_DIS}
      # !BSF HELP:{ENABLE: Enable this PCIE RP. DISABLE: Disable this PCIE RP}
      # !BSF ORDER:{0000.0000}
      # !BSF FIELD:{En:1bB}

      # !BSF NAME:{PCIE RP Clk Req Support}
      # !BSF TYPE:{Combo} OPTION:{$EN_DIS}
      # !BSF HELP:{Enable/Disable CLKREQ# Support}
      # !BSF CONDITION:{$(COND_PCIE_RP_EN)}
      # !BSF FIELD:{ClkReqSup:1bB}

      # !BSF NAME:{PCIE RP Clk Req Number}
      # !BSF TYPE:{EditNum, HEX, (0x00, 0xFF)}
      # !BSF HELP:{Configure Root Port CLKREQ Number if CLKREQ is supported}
      # !BSF CONDITION:{$(COND_PCIE_RP_EN)}
      # !BSF CONDITION:{$(COND_PCIE_RP_CLK_REQ_SUP)}
      # !BSF FIELD:{ClkReqNum:3bB}

      # !BSF NAME:{PCIE RP Aspm}
      # !BSF TYPE:{EditNum, HEX, (0x00, 0xFF)}
      # !BSF HELP:{PCI Express Active State Power Management settings}
      # !BSF CONDITION:{$(COND_PCIE_RP_EN)}
      # !BSF FIELD:{Aspm:3bB}

  # !BSF DEFT:{PCIERP_TMPL:END}

    =======================================================================================
  # !BSF DEFT:{PCIERP_CTRL_PIN_TMPL:START}
    # !BSF PAGES:{PLT_PWR_$(1):PLT_PCIE_RP_CTRL_PIN_PWR:"PCIE RP $(1)"}
    # !BSF PAGE:{PLT_PWR_$(1)}

    # !HDR EMBED:{PCIE_RP_PIN_CTRL[]:PcieRpPinCtrlData$(1):START}

    # !BSF NAME:{PCIE RP $(1) Power}
    # !HDR STRUCT:{PCIE_RP_PIN_CTRL_PWR}
    # !BSF MARKER:{Power}
    gCfgData.PcieRpPower$(1)                      |      * | 0x04 | $(2)

      # !BSF NAME:{PCIE RP PIN CTRL Power Skip}
      # !BSF TYPE:{Combo} OPTION:{$EN_DIS}
      # !BSF HELP:{ENABLE: Skip GPIO Power programming for this PCIE RP. DISABLE: Program GPIO Power for this PCIE RP. }
      # !BSF ORDER:{0000.0000}
      # !BSF FIELD:{Skip:1b}

      # !BSF NAME:{Power Pad Community}
      # !BSF TYPE:{Combo}
      # !BSF OPTION:{0xC2: SOUTH, 0xC7: WEST, 0xC4: NORTHWEST, 0xC0: SOUTHWEST, 0xC5: NORTH}
      # !BSF HELP:{Specify Pad Community}
      # !BSF CONDITION:{$(COND_PCIE_RP_PWR_PIN_SKIP)}
      # !BSF FIELD:{Community:8b}

      # !BSF NAME:{Power Pad Number}
      # !BSF TYPE:{EditNum, INT, (0,4095)}
      # !BSF HELP:{Specify Pad Number}
      # !BSF CONDITION:{$(COND_PCIE_RP_PWR_PIN_SKIP)}
      # !BSF FIELD:{PadNum:12b}

      # !BSF NAME:{Power Drive : Outport Value}
      # !BSF TYPE:{Combo}
      # !BSF OPTION:{0x0:Disable, 0x1:Enable}
      # !BSF HELP:{Specify if should be driven or not}
      # !BSF CONDITION:{$(COND_PCIE_RP_PWR_PIN_SKIP)}
      # !BSF FIELD:{Drive:1b}

      # !BSF NAME:{Power Reserved} TYPE:{Reserved}
      # !BSF CONDITION:{$(COND_PCIE_RP_PWR_PIN_SKIP)}
      # !BSF FIELD:{Rsvd1:10b}

    # !BSF PAGES:{PLT_RST_$(1):PLT_PCIE_RP_CTRL_PIN_RST:"PCIE RP $(1)"}
    # !BSF PAGE:{PLT_RST_$(1)}
    # !BSF NAME:{PCIE RP $(1) Reset}
    # !HDR STRUCT:{PCIE_RP_PIN_CTRL_RST}
    # !BSF MARKER:{Reset}
    gCfgData.PcieRpReset$(1)                      |      * | 0x04 | $(3)

      # !BSF NAME:{PCIE RP PIN CTRL Reset Skip}
      # !BSF TYPE:{Combo} OPTION:{$EN_DIS}
      # !BSF HELP:{ENABLE: Skip GPIO Reset programming for this PCIE RP. DISABLE: Program GPIO Reset for this PCIE RP. }
      # !BSF ORDER:{0000.0000}
      # !BSF FIELD:{Skip:1b}

      # !BSF NAME:{Reset Pad Community}
      # !BSF TYPE:{Combo}
      # !BSF OPTION:{0xC2: SOUTH, 0xC7: WEST, 0xC4: NORTHWEST, 0xC0: SOUTHWEST, 0xC5: NORTH}
      # !BSF HELP:{Specify Pad Community}
      # !BSF CONDITION:{$(COND_PCIE_RP_RST_PIN_SKIP)}
      # !BSF FIELD:{Community:8b}

      # !BSF NAME:{Reset Pad Number}
      # !BSF TYPE:{EditNum, INT, (0,4095)}
      # !BSF HELP:{Specify Pad Number}
      # !BSF CONDITION:{$(COND_PCIE_RP_RST_PIN_SKIP)}
      # !BSF FIELD:{PadNum:12b}

      # !BSF NAME:{Reset Drive : Outport Value}
      # !BSF TYPE:{Combo}
      # !BSF OPTION:{0x0:Disable, 0x1:Enable}
      # !BSF HELP:{Specify if should be driven or not}
      # !BSF CONDITION:{$(COND_PCIE_RP_RST_PIN_SKIP)}
      # !BSF FIELD:{Drive:1b}

      # !BSF CONDITION:{@#endif}
      # !BSF NAME:{Reset Reserved} TYPE:{Reserved}
      # !BSF CONDITION:{$(COND_PCIE_RP_RST_PIN_SKIP)}
      # !BSF FIELD:{Rsvd1:10b}

    # !HDR EMBED:{PCIE_RP_PIN_CTRL[]:PcieRpPinCtrlData$(1):END}

  # !BSF DEFT:{PCIERP_CTRL_PIN_TMPL:END}


