/** @file
  Touch Host Controller ACPI

  Copyright (c) 2024, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

  //
  // Optional callback reference
  // Scope (\_SB.PC0n.THCn.CINI)
  //
  // Custom INI (CINI)
  //   - Performs device specific action during initalization
  //
  // Arg0: THC_RST_PAD
  // Arg1: THC_RESET_DELAY
  //
  External(CINI, MethodObj)

  Name (_ADR, THC_ADR)
  Name (RSTL, 0) // Reset SW lock

  OperationRegion (THCR, PCI_Config, 0x00, 0x100)
  Field (THCR, ByteAcc, NoLock, Preserve) {
      VDID, 32,      // 0x00, VID DID
  }

  If (LNotEqual (VDID, 0xFFFFFFFF)) {
    //
    // QuickI2C -> _CRS _DSD equivalent
    //
    Name(ICRS, Buffer(12){})
    CreateWordField (ICRS, 0x00, DADR)  // WORD: HIDI2C Device address
    CreateQWordField (ICRS, 0x02, DSPD) // Integer: Requested Connection Speed
    CreateByteField (ICRS, 0xA, DADM)   // BYTE: Addressing Mode
    //
    // I2C SubIP configuration data
    //
    Name(ISUB, Buffer(145){})
    CreateQWordField (ISUB, 0x00, SMHX) // Integer: Standard Mode (100 kbit/s) Serial Clock Line HIGH Period
    CreateQWordField (ISUB, 0x08, SMLX) // Integer: Standard Mode (100 kbit/s) Serial Clock Line LOW Period
    CreateQWordField (ISUB, 0x10, SMTD) // Integer: Standard Mode (100 kbit/s) Serial Data Line Transmit Hold Period
    CreateQWordField (ISUB, 0x18, SMRD) // Integer: Standard Mode (100 kbit/s) Serial Data Receive Hold Period
    CreateQWordField (ISUB, 0x20, FMHX) // Integer: Fast Mode (400 kbit/s) Serial Clock Line HIGH Period
    CreateQWordField (ISUB, 0x28, FMLX) // Integer: Fast Mode (400 kbit/s) Serial Clock Line LOW Period
    CreateQWordField (ISUB, 0x30, FMTD) // Integer: Fast Mode (400 kbit/s) Serial Data Line Transmit Hold Period
    CreateQWordField (ISUB, 0x38, FMRD) // Integer: Fast Mode (400 kbit/s) Serial Data Line Receive Hold Period
    CreateQWordField (ISUB, 0x40, FMSL) // Integer: Maximum length (in ic_clk_cycles) of suppressed spikes in Std Mode, Fast Mode and Fast Mode Plus.
    CreateQWordField (ISUB, 0x48, FPHX) // Integer: Fast Mode Plus (1Mbit/sec) Serial Clock Line HIGH Period
    CreateQWordField (ISUB, 0x50, FPLX) // Integer: Fast Mode Plus (1Mbit/sec) Serial Clock Line LOW Period
    CreateQWordField (ISUB, 0x58, FPTD) // Integer: Fast Mode Plus (1Mbit/sec) Serial Data Line Transmit HOLD Period
    CreateQWordField (ISUB, 0x60, FPRD) // Integer: Fast Mode Plus (1Mbit/sec) Serial Data Line Receive HOLD Period
    CreateQWordField (ISUB, 0x68, HMHX) // Integer: High Speed Mode Plus (3.4Mbits/sec) Serial Clock Line HIGH Period
    CreateQWordField (ISUB, 0x70, HMLX) // Integer: High Speed Mode Plus (3.4Mbits/sec) Serial Clock Line LOW Period
    CreateQWordField (ISUB, 0x78, HMTD) // Integer: High Speed Mode Plus (3.4Mbits/sec) Serial Data Line Transmit HOLD Period
    CreateQWordField (ISUB, 0x80, HMRD) // Integer: High Speed Mode Plus (3.4Mbits/sec) Serial Data Line Receive HOLD Period
    CreateQWordField (ISUB, 0x88, HMSL) // Integer: Maximum length (in ic_clk_cycles) of suppressed spikes in High Speed Mode.

    Store (THC_HIDI2C_DEVICE_ADDRESS, DADR)
    Store (THC_HIDI2C_CONNECTION_SPEED, DSPD)
    Store (THC_HIDI2C_ADDRESSING_MODE, DADM)
    Store (THC_HIDI2C_SM_SCL_HIGH_PERIOD, SMHX)
    Store (THC_HIDI2C_SM_SCL_LOW_PERIOD, SMLX)
    Store (THC_HIDI2C_SM_SDA_HOLD_TX_PERIOD, SMTD)
    Store (THC_HIDI2C_SM_SDA_HOLD_RX_PERIOD, SMRD)
    Store (THC_HIDI2C_FM_SCL_HIGH_PERIOD, FMHX)
    Store (THC_HIDI2C_FM_SCL_LOW_PERIOD, FMLX)
    Store (THC_HIDI2C_FM_SDA_HOLD_TX_PERIOD, FMTD)
    Store (THC_HIDI2C_FM_SDA_HOLD_RX_PERIOD, FMRD)
    Store (THC_HIDI2C_SUPPRESSED_SPIKES_S_F_FP, FMSL)
    Store (THC_HIDI2C_FMP_SCL_HIGH_PERIOD, FPHX)
    Store (THC_HIDI2C_FMP_SCL_LOW_PERIOD, FPLX)
    Store (THC_HIDI2C_FMP_SDA_HOLD_TX_PERIOD, FPTD)
    Store (THC_HIDI2C_FMP_SDA_HOLD_RX_PERIOD, FPRD)
    Store (THC_HIDI2C_HM_SCL_HIGH_PERIOD, HMHX)
    Store (THC_HIDI2C_HM_SCL_LOW_PERIOD, HMLX)
    Store (THC_HIDI2C_HM_SDA_HOLD_TX_PERIOD, HMTD)
    Store (THC_HIDI2C_HM_SDA_HOLD_RX_PERIOD, HMRD)
    Store (THC_HIDI2C_SUPPRESSED_SPIKES_H, HMSL)

    //
    // _DSM - Device-Specific Method
    //
    // Arg0:    UUID       Unique function identifier
    // Arg1:    Integer    Revision Level - Will be 2 for HidSpi V1
    // Arg2:    Integer    Function Index (0 = Return Supported Functions)
    // Arg3:    Package    Parameters
    //
    Method (_DSM, 4, Serialized, 0, UnknownObj, {BuffObj, IntObj, IntObj, PkgObj}) {
      If (PCIC (Arg0)) { Return (PCID (Arg0,Arg1,Arg2,Arg3)) }
      If (LEqual (THC_MODE, 0x1)) {
        If (LEqual (Arg0,ToUUID ("6E2AC436-0FCF-41AF-A265-B32A220DCFAB"))) {
          //
          // Switch on the function index
          //
          switch (ToInteger (Arg2)) {
            case (0) {
              // Switch on the revision level
              switch (ToInteger (Arg1)) {
                case (2) {
                  // HidSpi v1 : Functions 0-6 inclusive are supported (0b01111111)
                  Return (Buffer () {0x7F})
                }
                default {
                  // Unsupported revision
                  Return (Buffer () {0x00})
                }
              }
            } // End case0
            case (1) {
              ADBG ("THC THC_INPUT_REPORT_HEADER_ADDRESS")
              Return (ToInteger (THC_INPUT_REPORT_HEADER_ADDRESS))
            }
            case (2) {
              ADBG ("THC THC_INPUT_REPORT_BODY_ADDRESS")
              Return (ToInteger (THC_INPUT_REPORT_BODY_ADDRESS))
            }
            case (3) {
              ADBG ("THC THC_OUTPUT_REPORT_ADDRESS")
              Return (ToInteger (THC_OUTPUT_REPORT_ADDRESS))
            }
            case (4) {
              ADBG ("THC THC_READ_OPCODE")
              Name (BUF4, Buffer(1) {})
              Store (ToBuffer (THC_READ_OPCODE), Local0)
              Store (DerefOf (Index (Local0, 0)), Index (BUF4,0))
              Return (BUF4)
            }
            case (5) {
              ADBG ("THC THC_WRITE_OPCODE")
              Name (BUF5, Buffer(1) {})
              Store (ToBuffer (THC_WRITE_OPCODE), Local1)
              Store (DerefOf (Index (Local1, 0)), Index (BUF5,0))
              Return (BUF5)
            }
            case (6) {
              /*
              Bit 0-12: Reserved
              Bit 13: SPI Write Mode.
               0b0 - Writes are carried in single SPI mode
               0b1 - Writes are carried out in Multi-SPI mode as specified by bit 14-15
              Bit 14-15: Multi-SPI Mode
               0b00 - Single SPI Mode
               0b01 - Dual SPI Mode
               0b10 - Quad SPI Mode
               0b11 - Reserved
              */
              ADBG ("THC THC_FLAGS")
              Return (ToInteger (THC_FLAGS))
            }
            default {
              // Unsupported function index
              Return (Buffer () {0})
            }
          } //EndSwitch
          //
          // No functions are supported for this UUID.
          //
          Return (Buffer () {0})
        } //EndIfUUID
        If (LEqual (Arg0,ToUUID ("300D35B7-AC20-413E-8E9C-92E4DAFD0AFE"))) {
          switch (ToInteger (Arg2)) {
            case (0) {
              // Functions 1-3 inclusive are supported (0b00000111)
              Return (Buffer () {0x7})
            }
            case (1) {
              ADBG ("THC THC_CONNECTION_SPEED")
              /*
               Connection Speed in Hz
               Driver will round down to the nearest Spi frequency that THC supports.
               Max Speed is specific to each Soc design.
              */
              Return (ToInteger (THC_CONNECTION_SPEED))
            }
            case (2) {
              ADBG ("THC THC_LIMIT_PACKET_SIZE")
              /*
              Bit 0: LimitPacketSize
                When set, limits SPI read & write packet size to 64B.
                Otherwise, THC uses Max Soc packet size for SPI Read and Write
                0 - Max Soc Packet Size
                1 - 64 Bytes
              Bit 1-31: Reserved
              */
              Return (ToInteger (THC_LIMIT_PACKET_SIZE))
            }
            case (3) {
              ADBG ("THC THC_PERFORMANCE_LIMITATION")
              /*
              Bit 0-15: Performance Limitation
                Minimum amount of delay the THC/QUICKSPI driver must wait between end of write operation
                and begin of read operation. This value shall be in 10us multiples
                0 - Disabled
                1 - 65535 (0xFFFF) - up to 655350 us
              Bit 16-31: Reserved
              */
              Return (ToInteger (THC_PERFORMANCE_LIMITATION))
            }
            default {
              // Unsupported function index
              Return (Buffer () {0})
            }
          } // End Switch
        } // End UUID
      } // End THC HID Mode
      If (LEqual (Arg0,ToUUID ("84005682-5B71-41A4-8D66-8130F787A138"))) {
        switch (ToInteger (Arg2)) {
          case (0) {
            // Function 1/2 are supported (0b00000011)
            Return (Buffer () {0x3})
          }
          case (1) {
            ADBG ("THC THC_ACTIVE_LTR")
            Return (ToInteger (THC_ACTIVE_LTR))
          }
          case (2) {
            ADBG ("THC THC_IDLE_LTR")
            Return (ToInteger (THC_IDLE_LTR))
          }
          default {
            // Unsupported function index
            Return (Buffer () {0})
          }
        } // End Switch
      } // End UUID
      If (LEqual (Arg0,ToUUID ("9B1E77C4-53DB-43BD-A276-F7287604D1BD"))) {
        switch (ToInteger (Arg2)) {
          case (0) {
            // Function 1 supported (0b00000001)
            Return (Buffer () {0x1})
          }
          case (1) {
            ADBG ("THC THC_DISPLAY_FRAME_SYNC_PERIOD")
            Return (ToInteger (THC_DISPLAY_FRAME_SYNC_PERIOD))
          }
          default {
            // Unsupported function index
            Return (Buffer () {0})
          }
        } // End Switch
      } // End UUID
      //
      // HIDI2C
      //
      If (LEqual (Arg0,ToUUID ("3CDFF6F7-4267-4555-AD05-B30A3D8938DE"))) {
        switch (ToInteger (Arg2)) {
          case (0) {
            // Function 1 supported (0b00000001)
            Return (Buffer () {0x1})
          }
          case (1) {
            ADBG ("THC THC_HIDI2C_DEVICE_ADDRESS")
            Return (ToInteger (THC_HIDI2C_DEVICE_ADDRESS))
          }
          default {
            // Unsupported function index
            Return (Buffer () {0})
          }
        } // End Switch
      } // End UUID
      Return (Buffer () {})
    } // End _DSM
    //
    // Expose THC Resources only when THC Wake is enabled
    //
    If (LNotEqual (THC_WAKE_INT, 0)) {
      Name (_S0W, 3)
      Method (_CRS, 0x0, Serialized) {
        Return (TINT (THC_WAKE_INT))
      }
    }
    If (LNotEqual (THC_MODE, 0x0)) {
      Method (_INI) {
        ADBG ("THC _INI")
        // configure gpio pad in gpio driver mode
        GPIO_CONTROLLER_PATH.SHPO (THC_RST_PAD, 1)
        // Make sure both pads are in GPIO mode
        GPIO_CONTROLLER_PATH.SPMV (THC_RST_PAD, 0)
        // Put device in inital reset state
        GPIO_CONTROLLER_PATH.SPC0 (THC_RST_PAD, Or (0x42000200, And (Not (And (THC_RST_TRIGGER,1)),1)))
        //
        // Check for Custom INI (CINI) THC handling presence and call it if supported
        //
        If (CondRefOf (^CINI)) {
          CINI (THC_RST_PAD, THC_RESET_DELAY)
        }
      }
      Method (_RST, 0, Serialized) {
        ADBG ("THC _RST")
        // Wait until Lock is freed
        // Note: Lock should be not required because Method is serialized
        //       added to avoid race conditions
        While (LEqual (RSTL, 1)) {
          Sleep (10)
        }
        // Acquire Lock
        Store (1, RSTL)
        GPIO_CONTROLLER_PATH.SGOV (THC_RST_PAD, And (THC_RST_TRIGGER,1))
        Sleep (THC_RESET_DELAY)
        GPIO_CONTROLLER_PATH.SGOV (THC_RST_PAD, And (Not (And (THC_RST_TRIGGER,1)), 1))
        // Release Lock
        Store (0, RSTL)
      }
      Name (_DSD, Package() {
        ToUUID("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
        ICRS,
        ToUUID("dbb8e3e6-5886-4ba6-8795-1319f52a966b"),
        ISUB
      })
    }
  }
