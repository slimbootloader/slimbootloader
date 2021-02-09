/** @file
  ACPI RTD3 Common Code for USB.

@copyright
  INTEL CONFIDENTIAL
  Copyright 2018 Intel Corporation.

  The source code contained or described herein and all documents related to the
  source code ("Material") are owned by Intel Corporation or its suppliers or
  licensors. Title to the Material remains with Intel Corporation or its suppliers
  and licensors. The Material may contain trade secrets and proprietary and
  confidential information of Intel Corporation and its suppliers and licensors,
  and is protected by worldwide copyright and trade secret laws and treaty
  provisions. No part of the Material may be used, copied, reproduced, modified,
  published, uploaded, posted, transmitted, distributed, or disclosed in any way
  without Intel's prior express written permission.

  No license under any patent, copyright, trade secret or other intellectual
  property right is granted to or conferred upon you by disclosure or delivery
  of the Materials, either expressly, by implication, inducement, estoppel or
  otherwise. Any license under such intellectual property rights must be
  express and approved by Intel in writing.

  Unless otherwise agreed by Intel in writing, you may not remove or alter
  this notice or any other notice embedded in Materials by Intel or
  Intel's suppliers or licensors in any way.

  This file contains a 'Sample Driver' and is licensed as such under the terms
  of your license agreement with Intel or your vendor. This file may be modified
  by the user, subject to the additional terms of the license agreement.

@par Specification Reference:
**/

  //
  // Defining PSx methods for informing _psx method of XHCI.RHUB device
  // in PchXHCi.asl file that RTD3 is supported by platform or not. So depending
  // on these function it will Set/clear XHCI bit in MODPHY register. So don't
  // remove this functions.
  //
  Scope(\_SB.PC00.XHCI.RHUB){ //USB XHCI RHUB
    /// PS0X method is called by PS0 method in PchXHCi.asl
    Method(PS0X,0,Serialized)
    {
    }
    ///@defgroup XHCI_rhub_ps0x XHCI RHUB PS0X Method

    /// PS2X method is called by PS2 method in PchXHCIi.asl
    Method(PS2X,0,Serialized)
    {
    } // End of PS2 method
    ///@defgroup XHCI_rhub_ps2x XHCI RHUB PS2X Method

    /// PS3X method is called by _PS3 method in PchXHCi.asl
    Method(PS3X,0,Serialized)
    {
    } // End of PS3 method
    ///@defgroup XHCI_rhub_ps3x XHCI RHUB PS3X Method
  }
