/** @file

  Copyright (c) 2026, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

Include ("Gmio.asl")                  // Provided by CnvFeaturePkg

Scope (\_SB.PC00.RP01.PXSX)
{
  #undef PCIE_ROOT_PORT
  #define PCIE_ROOT_PORT \_SB.PC00.RP01
  Include ("EpOpRegion.asl")          // Provided by CnvFeaturePkg

  //
  // _DSM : Device Specific Method
  //
  // Arg0: UUID Unique function identifier
  // Arg1: Integer Revision Level
  // Arg2: Integer Function Index
  // Arg3: Package Parameters
  //
#if FixedPcdGetBool (PcdWclSupport) == 1
  If (LAnd (CondRefOf (\DLRM), (LNotEqual (\DLRM, 0))))
  {
    Method (_DSM, 4, Serialized) {
      //
      // DLRM support
      //
      If (LEqual (Arg0, ToUUID ("C41F8AFB-4701-F0EB-1D26-0296648C30E4")))
      {
        If (LEqual (1, ToInteger (Arg1)))        // Revision 1.
        {
          Switch (ToInteger (Arg2))            // Switch to Function Index.
          {
            //
            // Function 0, Query of supported functions.
            //
            Case (0)
            {
              Return (Buffer () {0x03})
            }

            //
            // Function 1, DLRM Support for Storage to reduce active power usage in D3.
            //
            Case (1)
            {
              // Only return support if platform enabled DLRM via setup.
              If (LAnd (PNVM (), LAnd (LNotEqual (\DLRM, 0), LEqual (S1G4, 1)))) {
                ADBG ("Enable DLRM for Storage")
                Return (1)
              } Else {
                ADBG ("Disable DLRM for Storage")
                Return (0)
              }
            }
          }
        } Else {
          ADBG ("Revision 0: No function supported")
        }
        Return (Buffer() {0x00})
      }
      // If the code falls through to this point, just return a buffer of 0.
      Return (Buffer () {0x00})
    }  // End _DSM Method
  }
#endif // FixedPcdGetBool (PcdWclSupport) == 1
}

Scope (\_SB.PC00.RP02.PXSX)
{
  #undef PCIE_ROOT_PORT
  #define PCIE_ROOT_PORT \_SB.PC00.RP02
  Include ("EpOpRegion.asl")          // Provided by CnvFeaturePkg
}

Scope (\_SB.PC00.RP03.PXSX)
{
  #undef PCIE_ROOT_PORT
  #define PCIE_ROOT_PORT \_SB.PC00.RP03
  Include ("EpOpRegion.asl")          // Provided by CnvFeaturePkg
}

Scope (\_SB.PC00.RP04.PXSX)
{
  #undef PCIE_ROOT_PORT
  #define PCIE_ROOT_PORT \_SB.PC00.RP04
  Include ("EpOpRegion.asl")          // Provided by CnvFeaturePkg
}

Scope (\_SB.PC00.RP05.PXSX)
{
  #undef PCIE_ROOT_PORT
  #define PCIE_ROOT_PORT \_SB.PC00.RP05
  Include ("EpOpRegion.asl")          // Provided by CnvFeaturePkg

#if FixedPcdGetBool (PcdWclSupport) == 0
  //
  // _DSM : Device Specific Method
  //
  // Arg0: UUID Unique function identifier
  // Arg1: Integer Revision Level
  // Arg2: Integer Function Index
  // Arg3: Package Parameters
  //
  If (LAnd (CondRefOf (\DLRM), (LNotEqual (\DLRM, 0))))
  {
    Method (_DSM, 4, Serialized)
    {
      //
      // DLRM support
      //
      If (LEqual (Arg0, ToUUID ("C41F8AFB-4701-F0EB-1D26-0296648C30E4")))
      {
        If (LEqual (1, ToInteger (Arg1)))        // Revision 1.
        {
          Switch (ToInteger (Arg2))            // Switch to Function Index.
          {
            //
            // Function 0, Query of supported functions.
            //
            Case (0)
            {
              Return (Buffer () {0x03})
            }

            //
            // Function 1, DLRM Support for Storage to reduce active power usage in D3.
            //
            Case (1)
            {
              // Only return support if platform enabled DLRM via setup.
              If (LAnd (PNVM (), LAnd (LNotEqual (\DLRM, 0), LEqual (S1G4, 1)))) {
                ADBG ("Enable DLRM for Storage")
                Return (1)
              } Else {
                ADBG ("Disable DLRM for Storage")
                Return (0)
              }
            }
          }
        } Else {
          ADBG ("DLRM Revision 0: No function supported")
        }
        Return (Buffer() {0x00})
      }

      Return (Buffer () {0x00})
    }  // End _DSM Method
  }
#endif // FixedPcdGetBool (PcdWclSupport) == 0
}

Scope (\_SB.PC00.RP06.PXSX)
{
  #undef PCIE_ROOT_PORT
  #define PCIE_ROOT_PORT \_SB.PC00.RP06
  Include ("EpOpRegion.asl")          // Provided by CnvFeaturePkg
}

#if FixedPcdGetBool (PcdWclSupport) == 0
Scope (\_SB.PC00.RP09.PXSX)
{
  #undef PCIE_ROOT_PORT
  #define PCIE_ROOT_PORT \_SB.PC00.RP09
  Include ("EpOpRegion.asl")          // Provided by CnvFeaturePkg

  //
  // _DSM : Device Specific Method
  //
  // Arg0: UUID Unique function identifier
  // Arg1: Integer Revision Level
  // Arg2: Integer Function Index
  // Arg3: Package Parameters
  //
  //
  If (LAnd (CondRefOf (\DLRM), (LNotEqual (\DLRM, 0))))
  {
    Method (_DSM, 4, Serialized)
    {
      //
      // DLRM support
      //
      If (LEqual (Arg0, ToUUID ("C41F8AFB-4701-F0EB-1D26-0296648C30E4")))
      {
        If (LEqual (1, ToInteger (Arg1)))        // Revision 1.
        {
          Switch (ToInteger (Arg2))            // Switch to Function Index.
          {
            //
            // Function 0, Query of supported functions.
            //
            Case (0)
            {
              Return (Buffer () {0x03})
            }

            //
            // Function 1, DLRM Support for Storage to reduce active power usage in D3.
            //
            Case (1)
            {
              // Only return support if platform enabled DLRM via setup.
              If (LAnd (PNVM (), LAnd (LNotEqual (\DLRM, 0), LEqual (S2G4, 1)))) {
                ADBG ("Enable DLRM for Storage")
                Return (1)
              } Else {
                ADBG ("Disable DLRM for Storage")
                Return (0)
              }
            }
          }
        } Else {
          ADBG ("DLRM Revision 0: No function supported")
        }
        Return (Buffer () {0x00})
      }

      Return (Buffer () {0x00})
    }  // End _DSM Method
  }
}
#endif // FixedPcdGetBool (PcdWclSupport) == 0