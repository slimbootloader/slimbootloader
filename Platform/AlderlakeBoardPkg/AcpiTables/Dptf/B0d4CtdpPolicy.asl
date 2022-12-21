/** @file
  Intel ACPI Reference Code for Intel Configurable TDP

  Copyright (c) 2022, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent
**/

Scope(\_SB.IETM)
{
   Name (CTSP, Package ()
    {
    //
    // ConfigTDP Policy
    //
      ToUUID("E145970A-E4C1-4d73-900E-C9C5A69DD067")
    })
}

Scope(\_SB.PC00.TCPU)
{
    // TDPL (Thermal Design Power List)
    //
    // Returns the list of all supported Configurable TDP points available for the participant device.
    //
    // Arguments: (0)
    //   None
    // Return Value:
    //   Package () // Zeroth TDP State
    //   {
    //     TDPPower, // DWordConst
    //     Frequency, // DWordConst
    //     TDPControl, // DWordConst
    //     FrequencyControl, // DWordConst
    //     Reserved1, // DWordConst
    //   },
    //     .
    //     .
    //     .
    //   Package () // Last TDP State
    //   {
    //     TDPPower, // DWordConst
    //     Frequency, // DWordConst
    //     TDPControl, // DWordConst
    //     FrequencyControl, // DWordConst
    //     Reserved1, // DWordConst
    //   }
    //
    Method(TDPL,0,Serialized,,PkgObj)
    {
        Name(AAAA,0) // PACKAGE_POWER_SKU
        Name(BBBB,0) // CONFIG_TDP_LEVEL1
        Name(CCCC,0) // CONFIG_TDP_LEVEL2

        Store(CTNL, Local0)  // CONFIG_TDP_NUM_LEVELS
        If(LOr(LEqual(Local0,1),LEqual(Local0,2))){ // check for CTDP support
          Store(\_SB.CLVL,Local0)  // use PPM supplied value for CTDP level
        }
        Else{
          Return(Package (){0}) // no CTDP support
        }

        If(LEqual(CLCK,1)){  // check the CONFIG_TDP_LOCK bit
          Store(1,Local0)
        }

        Store(CPNU(\_SB.PL10,1),AAAA)  // convert Custom 0 Power Limit1 to milliwatts
        Store(CPNU(\_SB.PL11,1),BBBB)  // convert Custom 1 Power Limit1 to milliwatts
        Store(CPNU(\_SB.PL12,1),CCCC)  // convert Custom 2 Power Limit1 to milliwatts

        //
        // Define a package of packages that contain 5 DWords.
        //
        Name (TMP1, Package (1){
            Package (){0x80000000,0x80000000,0x80000000,0x80000000,0x80000000}
        })
        Name (TMP2, Package (2){
            Package (){0x80000000,0x80000000,0x80000000,0x80000000,0x80000000},
            Package (){0x80000000,0x80000000,0x80000000,0x80000000,0x80000000}
        })
        Name (TMP3, Package (3){
            Package (){0x80000000,0x80000000,0x80000000,0x80000000,0x80000000},
            Package (){0x80000000,0x80000000,0x80000000,0x80000000,0x80000000},
            Package (){0x80000000,0x80000000,0x80000000,0x80000000,0x80000000}
        })

        //
        // if 3 TDP's, use PL0, PL1 and PL2 values
        //
        If(LEqual(Local0,3)){
          If (LGreater(AAAA,BBBB)){
            If (LGreater(AAAA,CCCC)){
              If (LGreater(BBBB,CCCC)){
                // abc
                Store(0,Local3)
                Store(0,LEV0)
                Store(1,Local4)
                Store(1,LEV1)
                Store(2,Local5)
                Store(2,LEV2)
                }Else{
                // acb
                Store(0,Local3)
                Store(0,LEV0)
                Store(1,Local5)
                Store(2,LEV1)
                Store(2,Local4)
                Store(1,LEV2)
              }
            }Else{
              // cab
              Store(0,Local5)
              Store(2,LEV0)
              Store(1,Local3)
              Store(0,LEV1)
              Store(2,Local4)
              Store(1,LEV2)
            }
          }Else{  // b > a here
            If(LGreater(BBBB,CCCC)){
              If(LGreater(AAAA,CCCC)){
                // bac
                Store(0,Local4)
                Store(1,LEV0)
                Store(1,Local3)
                Store(0,LEV1)
                Store(2,Local5)
                Store(2,LEV2)
              }Else{
                // bca
                Store(0,Local4)
                Store(1,LEV0)
                Store(1,Local5)
                Store(2,LEV1)
                Store(2,Local3)
                Store(0,LEV2)
              }
            }Else{
              // cba
              Store(0,Local5)
              Store(2,LEV0)
              Store(1,Local4)
              Store(1,LEV1)
              Store(2,Local3)
              Store(0,LEV2)
            }
          }

          // copy for PL0
          Store(Add(\_SB.TAR0,1),Local1)
          Multiply(Local1,100,Local2)
          Store(AAAA,Index(DeRefOf(Index(TMP3, Local3)),0))
          Store(Local2,Index(DeRefOf(Index(TMP3, Local3)),1))
          Store(\_SB.CTC0,Index(DeRefOf(Index(TMP3, Local3)),2))
          Store(Local1,Index(DeRefOf(Index(TMP3, Local3)),3))
          Store(0,Index(DeRefOf(Index(TMP3, Local3)),4))

          // copy for PL1
          Store(Add(\_SB.TAR1,1),Local1)
          Multiply(Local1,100,Local2)
          Store(BBBB,Index(DeRefOf(Index(TMP3, Local4)),0))
          Store(Local2,Index(DeRefOf(Index(TMP3, Local4)),1))
          Store(\_SB.CTC1,Index(DeRefOf(Index(TMP3, Local4)),2))
          Store(Local1,Index(DeRefOf(Index(TMP3, Local4)),3))
          Store(0,Index(DeRefOf(Index(TMP3, Local4)),4))

          // copy for PL2
          Store(Add(\_SB.TAR2,1),Local1)
          Multiply(Local1,100,Local2)
          Store(CCCC,Index(DeRefOf(Index(TMP3, Local5)),0))
          Store(Local2,Index(DeRefOf(Index(TMP3, Local5)),1))
          Store(\_SB.CTC2,Index(DeRefOf(Index(TMP3, Local5)),2))
          Store(Local1,Index(DeRefOf(Index(TMP3, Local5)),3))
          Store(0,Index(DeRefOf(Index(TMP3, Local5)),4))

          Return(TMP3)
        }

        //
        // if 2 TDP's, use PL0 and PL1 values
        //
        If(LEqual(Local0,2)){
          If(LGreater(AAAA,BBBB)){
            Store(0,Local3) // store PL0 in Zeroth package
            Store(1,Local4) // store PL1 in next package
            Store(0,LEV0)
            Store(1,LEV1)
            Store(0,LEV2) // level 2 is not used so put 0 for safety
          }Else{
            Store(0,Local4) // store PL1 in Zeroth package
            Store(1,Local3) // store PL0 in next package
            Store(1,LEV0)
            Store(0,LEV1)
            Store(0,LEV2) // level 2 is not used so put 0 for safety
          }

          // copy for PL0
          Store(Add(\_SB.TAR0,1),Local1)
          Multiply(Local1,100,Local2)
          Store(AAAA,Index(DeRefOf(Index(TMP2, Local3)),0))
          Store(Local2,Index(DeRefOf(Index(TMP2, Local3)),1))
          Store(\_SB.CTC0,Index(DeRefOf(Index(TMP2, Local3)),2))
          Store(Local1,Index(DeRefOf(Index(TMP2, Local3)),3))
          Store(0,Index(DeRefOf(Index(TMP2, Local3)),4))

          // copy for PL1
          Store(Add(\_SB.TAR1,1),Local1)
          Multiply(Local1,100,Local2)
          Store(BBBB,Index(DeRefOf(Index(TMP2, Local4)),0))
          Store(Local2,Index(DeRefOf(Index(TMP2, Local4)),1))
          Store(\_SB.CTC1,Index(DeRefOf(Index(TMP2, Local4)),2))
          Store(Local1,Index(DeRefOf(Index(TMP2, Local4)),3))
          Store(0,Index(DeRefOf(Index(TMP2, Local4)),4))

          Return(TMP2)
        }

        //
        // if 1 TDP, return one PL based on boot index
        //
        If(LEqual(Local0,1)){
          Switch(ToInteger(\_SB.CBMI)){
            case(0){
              // copy for PL0
              Store(Add(\_SB.TAR0,1),Local1)
              Multiply(Local1,100,Local2)
              Store(AAAA,Index(DeRefOf(Index(TMP1, 0)),0))
              Store(Local2,Index(DeRefOf(Index(TMP1, 0)),1))
              Store(\_SB.CTC0,Index(DeRefOf(Index(TMP1, 0)),2))
              Store(Local1,Index(DeRefOf(Index(TMP1, 0)),3))
              Store(0,Index(DeRefOf(Index(TMP1, 0)),4))
              Store(0,LEV0)
              Store(0,LEV1)
              Store(0,LEV2)
            }

            case(1){
              // copy for PL1
              Store(Add(\_SB.TAR1,1),Local1)
              Multiply(Local1,100,Local2)
              Store(BBBB,Index(DeRefOf(Index(TMP1, 0)),0))
              Store(Local2,Index(DeRefOf(Index(TMP1, 0)),1))
              Store(\_SB.CTC1,Index(DeRefOf(Index(TMP1, 0)),2))
              Store(Local1,Index(DeRefOf(Index(TMP1, 0)),3))
              Store(0,Index(DeRefOf(Index(TMP1, 0)),4))
              Store(1,LEV0)
              Store(1,LEV1)
              Store(1,LEV2)
            }

            case(2){
              // copy for PL2
              Store(Add(\_SB.TAR2,1),Local1)
              Multiply(Local1,100,Local2)
              Store(CCCC,Index(DeRefOf(Index(TMP1, 0)),0))
              Store(Local2,Index(DeRefOf(Index(TMP1, 0)),1))
              Store(\_SB.CTC2,Index(DeRefOf(Index(TMP1, 0)),2))
              Store(Local1,Index(DeRefOf(Index(TMP1, 0)),3))
              Store(0,Index(DeRefOf(Index(TMP1, 0)),4))
              Store(2,LEV0)
              Store(2,LEV1)
              Store(2,LEV2)
            }
          }

          Return(TMP1)
        }

        Return(0) // we should never get here, this eliminates a compiler warning
    } // End of TDPL object

    //
    // Returns the max TDP configurable for the platform.
    //
    Name(MAXT,0)

    // TDPC (Thermal Design Power Capabilities)
    //
    // The TDPC object evaluates to an integer that identifies the maximum usable TDP point
    // from the table returned by the TDPL object at any given time.
    //
    // Arguments: (0)
    //   None
    // Return Value:
    //   This value is the zero based index into the TDPL object's entries.
    //
    Method(TDPC)  // For CRB, please return 0.
    {
      Return(MAXT)    // Return valid TDP point.
    }//end Method(TDPC)

    Name(LEV0,0)
    Name(LEV1,0)
    Name(LEV2,0)

    // STDP (Set current operating TDP Point)
    //
    // The STDP object communicates to the platform BIOS the current operating TDP point whenever it is changed due to any reason.
    // In response to this object evaluation, the platform BIOS modifies the packages returned in PPCC object and notifies the driver of
    // a change in power limit capabilities as appropriate.
    //
    // Arguments: (1)
    //   Arg0 = Config TDP level selected
    //     0 = Highest Level values from MSR
    //     1 = Middle Level values from MSR
    //     2 = Lowest Level values from MSR
    //     3 = reserved
    // Return Value:
    //   None
    //
    // Currently we are using hard coded values for the PPCC values.
    // A future reference code release will use PPCC values from the PPM reference code.
    // This will allow PPCC values to be selected from SETUP options.
    //
    Method(STDP,1,Serialized)
    {
      If (LGreaterEqual(Arg0,\_SB.CLVL)){ // bounds check the input
        Return() // invalid input
      }

      //
      // translate requested index value to the correct PL using the sorted TDPL value.
      //
      Switch (ToInteger(Arg0)){
        Case (0) {
          Store(LEV0,Local0)
        }
        Case (1) {
          Store(LEV1,Local0)
        }
        Case (2) {
          Store(LEV2,Local0)
        }
      }

      Switch (ToInteger(Local0)){
        Case (0) {
          CPL0() // copy PL0 values to PPCC
        }
        Case (1) {
          CPL1() // copy PL1 values to PPCC
        }
        Case (2) {
          CPL2() // copy PL2 values to PPCC
        }
      }

      Notify (\_SB.PC00.TCPU, 0x83)    // Send a Power Capabilities Changed notification to self
    }//end Method(STDP)

}//end Scope(\_SB.PC00.TCPU)

