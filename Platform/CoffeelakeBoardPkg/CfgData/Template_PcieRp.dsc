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
  # ================================= PCIERP_TMPL =========================================
  #
    =======================================================================================

  # !BSF DEFT:{PCIERP_PERPORT_TMPL:START}
    # !BSF PAGES:{PLT_PCIE_RP_$(1)_FEAT:PLT_PCIE_RP_FEAT:"RP $(1) Features"}
    # !BSF PAGE:{PLT_PCIE_RP_$(1)_FEAT}

    # !HDR STRUCT:{RP_FEATURES[]}

    # !BSF NAME:{RP $(1) Features}
    # !BSF TYPE:{EditNum, HEX}
    # !BSF HELP:{GPIO $(1) Feature set}
    gCfgData.RpFeatures$(1)                   |     * | 0x10 | $(2)

      # !BSF NAME:{PCIE RP Aspm}
      # !BSF TYPE:{Combo}
      # !BSF OPTION:{0x0:ASPM Not Supported, 0x1:L0s supported, 0x2:L1 supported, 0x3:L0s & L1 supported, 0x4:AutoConfig}
      # !BSF HELP:{PCI Express Active State Power Management settings.}
      # !BSF CONDITION:{$RpFeatures$(1).RpPresent == 1}
      # !BSF FIELD:{Aspm:4b}

      # !BSF NAME:{PCIE RP Physical Slot Number}
      # !BSF TYPE:{EditNum, HEX, (0x00, 0x18)}
      # !BSF HELP:{Indicates the slot number for the root port. Default is the value as root port index.}
      # !BSF CONDITION:{$RpFeatures$(1).RpPresent == 1}
      # !BSF FIELD:{PhySlotNum:6b}

      # !BSF NAME:{PCIE RP Gen3 Equalization Phase Method}
      # !BSF TYPE:{Combo}
      # !BSF OPTION:{0x0:DEPRECATED (hardware equalization), 0x1:hardware equalization, 0x4:Fixed equalization}
      # !BSF HELP:{PCIe Gen3 Eq Ph3 Method. 0: DEPRECATED, hardware equalization; 1: hardware equalization; 4: Fixed equalization, requires Coefficient settings per lane.}
      # !BSF CONDITION:{$RpFeatures$(1).RpPresent == 1}
      # !BSF FIELD:{Gen3EqPh3Method:4b}

      # !BSF NAME:{PCIE RP L1 Substates}
      # !BSF TYPE:{Combo}
      # !BSF OPTION:{0x0:L1SubStates_Disabled, 0x1:L1SubStatesL1_1, 0x2:L1SubStatesL1_1_2, 0x3:L1SubStatesMax}
      # !BSF HELP:{The L1 Substates configuration of the root port.}
      # !BSF CONDITION:{$RpFeatures$(1).RpPresent == 1}
      # !BSF FIELD:{L1SubStates:2b}

      # !BSF NAME:{PCIE RP Upstream Port Transmiter Preset}
      # !BSF TYPE:{EditNum, HEX, (0x0, 0xF)}
      # !BSF HELP:{Used during Gen3 Link Equalization. Used for all lanes.  Default is 5.}
      # !BSF CONDITION:{$RpFeatures$(1).RpPresent == 1}
      # !BSF FIELD:{Uptp:4b}

      # !BSF NAME:{PCIE RP Downstream Port Transmiter Preset}
      # !BSF TYPE:{EditNum, HEX, (0x0, 0xF)}
      # !BSF HELP:{Used during Gen3 Link Equalization. Used for all lanes.  Default is 7.}
      # !BSF CONDITION:{$RpFeatures$(1).RpPresent == 1}
      # !BSF FIELD:{Dptp:4b}

      # !BSF NAME:{PCIE Eq Ph3 Lane Param Cm (Coefficient C-1)}
      # !BSF TYPE:{EditNum, HEX, (0x0, 0xF)}
      # !BSF HELP:{PCIE_EQ_LANE_PARAM. Coefficient C-1.}
      # !BSF CONDITION:{$RpFeatures$(1).RpPresent == 1}
      # !BSF FIELD:{EqPh3Cm:4b}

      # !BSF NAME:{PCIE Eq Ph3 Lane Param Cp (Coefficient C+1)}
      # !BSF TYPE:{EditNum, HEX, (0x0, 0xF)}
      # !BSF HELP:{PCIE_EQ_LANE_PARAM. Coefficient C+1.}
      # !BSF CONDITION:{$RpFeatures$(1).RpPresent == 1}
      # !BSF FIELD:{EqPh3Cp:4b}

      # !BSF NAME:{PCIE RP Ltr Max Snoop Latency}
      # !BSF TYPE:{EditNum, HEX, (0x00, 0xFFFF)}
      # !BSF HELP:{Latency Tolerance Reporting, Max Snoop Latency.}
      # !BSF CONDITION:{$RpFeatures$(1).RpPresent == 1}
      # !BSF FIELD:{MaxSnoopLat:16b}

      # !BSF NAME:{PCIE RP Ltr Max No Snoop Latency}
      # !BSF TYPE:{EditNum, HEX, (0x00, 0xFFFF)}
      # !BSF HELP:{Latency Tolerance Reporting, Max Non-Snoop Latency.}
      # !BSF CONDITION:{$RpFeatures$(1).RpPresent == 1}
      # !BSF FIELD:{MaxNoSnoopLat:16b}

      # !BSF NAME:{PCIE RP Snoop Latency Override Value}
      # !BSF TYPE:{EditNum, HEX, (0x00, 0xFFFF)}
      # !BSF HELP:{Latency Tolerance Reporting, Snoop Latency Override Value.}
      # !BSF CONDITION:{$RpFeatures$(1).RpPresent == 1}
      # !BSF FIELD:{SnoopLatVal:16b}

      # !BSF NAME:{PCIE RP Non Snoop Latency Override Value}
      # !BSF TYPE:{EditNum, HEX, (0x00, 0xFFFF)}
      # !BSF HELP:{Latency Tolerance Reporting, Non-Snoop Latency Override Value.}
      # !BSF CONDITION:{$RpFeatures$(1).RpPresent == 1}
      # !BSF FIELD:{NoSnoopLatVal:16b}

      # !BSF NAME:{PCIE RP Snoop Latency Override Mode}
      # !BSF TYPE:{EditNum, HEX, (0x1, 0x2)}
      # !BSF HELP:{Latency Tolerance Reporting, Snoop Latency Override Mode.}
      # !BSF CONDITION:{$RpFeatures$(1).RpPresent == 1}
      # !BSF FIELD:{SnoopLatMode:2b}

      # !BSF NAME:{PCIE RP Non-Snoop Latency Override Mode}
      # !BSF TYPE:{EditNum, HEX, (0x1, 0x2)}
      # !BSF HELP:{Latency Tolerance Reporting, Non-Snoop Latency Override Mode.}
      # !BSF CONDITION:{$RpFeatures$(1).RpPresent == 1}
      # !BSF FIELD:{NoSnoopLatMode:2b}

      # !BSF NAME:{PCIE RP Snoop Latency Override Multiplier}
      # !BSF TYPE:{EditNum, HEX, (0x1, 0x2)}
      # !BSF HELP:{Latency Tolerance Reporting, Snoop Latency Override Multiplier.}
      # !BSF CONDITION:{$RpFeatures$(1).RpPresent == 1}
      # !BSF FIELD:{SnoopLatMul:2b}

      # !BSF NAME:{PCIE RP Non-Snoop Latency Override Multiplier}
      # !BSF TYPE:{EditNum, HEX, (0x1, 0x2)}
      # !BSF HELP:{Latency Tolerance Reporting, Non-Snoop Latency Override Multiplier.}
      # !BSF CONDITION:{$RpFeatures$(1).RpPresent == 1}
      # !BSF FIELD:{NoSnoopLatMul:2b}

      # !BSF NAME:{Enable PCIE RP Pm Sci} TYPE:{Combo} OPTION:{$EN_DIS}
      # !BSF HELP:{Indicate whether the root port power manager SCI is enabled.}
      # !BSF CONDITION:{$RpFeatures$(1).RpPresent == 1}
      # !BSF FIELD:{PmSciEn:1b}

      # !BSF NAME:{PCIE RP Access Control Services Extended Capability} TYPE:{Combo} OPTION:{$EN_DIS}
      # !BSF HELP:{Enable/Disable PCIE RP Access Control Services Extended Capability.}
      # !BSF CONDITION:{$RpFeatures$(1).RpPresent == 1}
      # !BSF FIELD:{AcsEn:1b}

      # !BSF NAME:{PCIE RP Max Payload} TYPE:{Combo}
      # !BSF OPTION:{0x0:MaxPayload128, 0x1:Maxpayload256}
      # !BSF HELP:{Max Payload Size supported, Default 128B.}
      # !BSF CONDITION:{$RpFeatures$(1).RpPresent == 1}
      # !BSF FIELD:{MaxPld:1b}

      # !BSF NAME:{PCH PCIe root port connection type} TYPE:{Combo} OPTION:{$EN_DIS}
      # !BSF HELP:{0: built-in device, 1:slot}
      # !BSF CONDITION:{$RpFeatures$(1).RpPresent == 1}
      # !BSF FIELD:{SlotImplemented:1b}

      # !BSF NAME:{PCIE RP Clock Power Management} TYPE:{Combo} OPTION:{$EN_DIS}
      # !BSF HELP:{Enable/Disable PCIE RP Clock Power Management, even if disabled, CLKREQ# signal can still be controlled by L1 PM substates mechanism}
      # !BSF CONDITION:{$RpFeatures$(1).RpPresent == 1}
      # !BSF FIELD:{CpmEn:1b}

      # !BSF NAME:{PCIE RP Ltr Enable} TYPE:{Combo} OPTION:{$EN_DIS}
      # !BSF HELP:{Latency Tolerance Reporting Mechanism.}
      # !BSF CONDITION:{$RpFeatures$(1).RpPresent == 1}
      # !BSF FIELD:{LtrEn:1b}

      # !BSF NAME:{Enable PCIE RP Clk Req Detect} TYPE:{Combo} OPTION:{$EN_DIS}
      # !BSF HELP:{Probe CLKREQ# signal before enabling CLKREQ# based power management.}
      # !BSF CONDITION:{$RpFeatures$(1).RpPresent == 1}
      # !BSF FIELD:{ClkReqDetect:1b}

      # !BSF NAME:{PCIE RP Advanced Error Report} TYPE:{Combo} OPTION:{$EN_DIS}
      # !BSF HELP:{Indicate whether the Advanced Error Reporting is enabled.}
      # !BSF CONDITION:{$RpFeatures$(1).RpPresent == 1}
      # !BSF FIELD:{AdvErrReport:1b}

      # !BSF NAME:{Reserved} TYPE:{Reserved}
      # !BSF FIELD:{Reserved:15b}

      # !BSF NAME:{RP Present} TYPE:{Reserved}
      # !BSF FIELD:{RpPresent:1b}


  # !BSF DEFT:{PCIERP_PERPORT_TMPL:START}