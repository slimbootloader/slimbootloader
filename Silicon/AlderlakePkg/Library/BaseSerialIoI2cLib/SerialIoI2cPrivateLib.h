/** @file

  Copyright (c) 2020 - 2024, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef _SERIAL_IO_I2C_PRIVATE_LIB_H_
#define _SERIAL_IO_I2C_PRIVATE_LIB_H_

/**
  Speed definitions for I2C Control Register
**/
typedef enum {
  I2cSpeedStandard = 0x1,      //[1KHz, 400KHz)
  I2cSpeedFast     = 0x2,      //[400KHz, 3400KHZ)
  I2cSpeedHigh     = 0x3,      //[3400, inf)
} SERIAL_IO_I2C_SPEED_SETTING;

/**
  Internal Timeout definitions for Serial Io I2C
**/
typedef enum {
  I2cEnableTimeout  = 50000,
  I2cDisableTimeout = 50000,
  I2cReceiveTimeout = 250000, // 250 ms -Applies to single byte timing
                             // The slowest Freq for 50 cycles to complete should be around 1ms, might consider lowering it
  I2cWaitTxEmptyTimeout = 50000
} SERIAL_IO_I2C_TIMERS;

// 0x0, I2C Control Register
typedef union {
  struct {
    UINT32  MasterMode     :1;    //[RW][0] This bit controls whether I2C master is enabled.
                                  //    0: Reserved
                                  //    1: Master Enabled
    UINT32  Speed          :2;    //[RW][1:2] These bits control at which speed the I2C operates; its setting is relevant only if
                                  //  one is operating the I2C in master mode
                                  //    01: Standard mode (0 to 100 kbit/s)
                                  //    10: Fast mode (<= 400 kbit/s)
                                  //    11: High Speed Mode (<= 3.4 Mbit/s)
    UINT32  Ic10bSlave     :1;    //[RW][3] When acting as a slave, this bit controls whether the DW_apb_i2c responds to 7- or 10-bit addresses.
    UINT32  Ic10bMaster    :1;    //[RO][4] Identifies if I2C operates in 7 or 10 bit addressing
                                  //    0: 7-bit addressing
                                  //    1: 10-bit addressing
    UINT32  IcRestartEn    :1;    //[RW][5] Determines whether RESTART conditions may be sent when I2C is acting as a master.
                                  //    0: Restart disable
                                  //    1: Restart enable
    UINT32  IcSlaveDis     :1;    //[RW][6] This bit controls whether I2C has its slave disabled.
                                  //    0:Reserved
                                  //    1: slave is disabled
    UINT32  StopDetSlave   :1;    //[RW][7] Dependencies: This register bit value is applicable in the slave mode only (MASTER_MODE = 1b0)
                                  //    1: issues the STOP_DET interrrupt only when it is addressed.
                                  //    0: issues the STOP_DET irrespective of whether its addressed or not
    UINT32  TxEmptyCtrl    :1;    //[RW][8] This bit controls the generation of the TX_EMPTY interrupt
    UINT32  RxFifoCtrl     :1;    //[RW][9] This bit controls whether DW_apb_i2c should hold the bus when the Rx FIFO is
                                  //        physically full to its RX_BUFFER_DEPTH
    UINT32  StopDetMstConf :1;    //[RO][10] In Master mode
                                  //    1: Issues the STOP_DET interrupt only when the master is active
                                  //    0: Issues the STOP_DET irrespective of whether the master is active
    UINT32  Reserved       :21;    //[RO][11:31] Reserved
  } Fields;
  UINT32 Data32;
} SERIAL_IO_I2C_MEM_CON;

// 0x04, I2C Target Address Register
typedef union {
  struct {
    UINT32  IcTar          :10;   //[RW][0:9] The target address for any master transaction.
                                  //          When transmitting a General Call, these bits are ignored.
                                  //          To generate a START BYTE, the CPU needs to write only once into these bits.
    UINT32  GcOrStart      :1;    //[RW][10] if bit 11 set, indicates whether a General Call or START byte command is to be performed by the I2C
                                  //    0: General Call Address after issuing a General Call
                                  //    1: START BYTE
    UINT32  Special        :1;    //[RW][11] This bit indicates whether software performs a General Call or START BYTE command.
                                  //    0: ignore bit 10 GC_OR_START and use IC_TAR normally.
                                  //    1: perform special I2C command as specified in GC_OR_START bit
    UINT32  Ic10bMaster    :1;    //[RW][12] This bit controls whether the I2C starts its transfers in 7-or 10-bit addressing mode when acting as a master.
                                  //    0: 7 bit addressing
                                  //    1: 10-bit addressing
    UINT32  Reserved       :19;    //[RO][13:31] Reserved
  } Fields;
  UINT32 Data32;
} SERIAL_IO_I2C_MEM_TAR;

// 0x10, I2C Data Command Register
typedef union {
  struct {
    UINT32  Data           :8;    //[RW][0:7] This register contains the data to be transmitted or received on the I2C bus.
    UINT32  Cmd           :1;    //[WO][8] This bit controls whether a read or a write is performed when I2C acts as a master
                                  //    1: Read.
                                  //    0: Write
                                  //  When a command is entered in the TX FIFO, this bit distinguishes the write and read commands.
                                  //  In slave-receiver mode:  this bit is irrelevant
                                  //  In slave-transmitter mode:
                                  //    0: indicates that the data in IC_DATA_CMD is to be transmitted.
    UINT32  Stop           :1;    //[WO][9] This bit controls whether a STOP is issued after the byte is sent or received.
                                  //    1: STOP is issued after this byte
                                  //    0: STOP is not issued after this byte,
    UINT32  Restart        :1;    //[WO][10] This bit controls whether a RESTART is issued before the byte is sent or received.
                                  //    1: RESTART is issued before the data is sent/received regardless if transfer direction is changing
                                  //    0: RESTART is issued only if the transfer direction is changing from the previous command
    UINT32  Reserved       :21;    //[RO][11:31] Reserved
  } Fields;
  UINT32 Data32;
} SERIAL_IO_I2C_MEM_DATA_CMD;

// 0x14, Standard Speed I2C Clock SCL High Count Register
typedef union {
  struct {
    UINT32  IcSsSclHcnt    :16;    //[RW][0:15] Sets the SCL clock high-period count for standard speed. Should be within the range {6, 65525}
    UINT32  Reserved       :16;    //[RW][16:31] Reserved
  } Fields;
  UINT32 Data32;
} SERIAL_IO_I2C_MEM_SS_SCL_HCNT;

// 0x18, Standard Speed I2C Clock SCL Low Count Register
typedef union {
  struct {
    UINT32  IcSsSclLcnt    :16;    //[RW][0:15] Standard Speed I2C Clock SCL Low Count Register. Should always be >= 8
    UINT32  Reserved       :16;    //[RO][16:31] Reserved
  } Fields;
  UINT32 Data32;
} SERIAL_IO_I2C_MEM_SS_SCL_LCNT;

// 0x1C, Fast Speed I2C Clock SCL High Count Register
typedef union {
  struct {
    UINT32  IcFsSclHcnt    :16;    //[RW][0:15] This register sets the SCL clock high-period count for fast speed.
                                   //           It is used in high-speed mode to send the Master Code and START BYTE or General CALL.
                                   //           The minimum value of this field is 6.
    UINT32  Reserved       :16;    //[RO][16:31] Reserved
  } Fields;
  UINT32 Data32;
} SERIAL_IO_I2C_MEM_FS_SCL_HCNT;

// 0x20, Fast Speed I2C Clock SCL Low Count Register
typedef union {
  struct {
    UINT32  IcFsSclLcnt    :16;    //[RW][0:15] This register sets the SCL clock low period count for fast speed.
                                   //           It is used in high-speed mode to send the Master Code and START BYTE or General CALL.
                                   //           The register should be programmed with a minimum value of 8.
    UINT32  Reserved       :16;    //[RO][16:31] Reserved
  } Fields;
  UINT32 Data32;
} SERIAL_IO_I2C_MEM_FS_SCL_LCNT;

// 0x24, High Speed I2C Clock SCL High Count Register
typedef union {
  struct {
    UINT32  IcHsSclHcnt    :16;    //[RW][0:15] This register sets the SCL clock high period count for high speed
    UINT32  Reserved       :16;    //[RO][16:31] Reserved
  } Fields;
  UINT32 Data32;
} SERIAL_IO_I2C_MEM_HS_SCL_HCNT;

// 0x28, High Speed I2C Clock SCL Low Count Register
typedef union {
  struct {
    UINT32  IcHsSclLcnt    :16;    //[RW][0:15]  This register sets the SCL clock low period count for high speed
    UINT32  Reserved       :16;    //[RO][16:31] Reserved
  } Fields;
  UINT32 Data32;
} SERIAL_IO_I2C_MEM_HS_SCL_LCNT;

// 0x2C, I2C Interrupt Status Register
typedef union {
  struct {
    UINT32  RRxUnder       :1;    //[RO][0] Set if the processor attempts to read the receive buffer when it is empty by
                                  //        reading from the IC_DATA_CMD register.
    UINT32  RRxOver        :1;    //[RO][1] Set if the receive buffer is completely filled to IC_RX_BUFFER_DEPTH
                                  //        and an additional byte is received from an external I2C device
    UINT32  RRxFull        :1;    //[RO][2] Set when the receive buffer reaches or goes above the RX_TL threshold in the IC_RX_TL register.
    UINT32  RTxOver        :1;    //[RO][3] Set during transmit if the transmit buffer is filled to IC_TX_BUFFER_DEPTH and
                                  //        the processor attempts to issue another I2C command by writing to the IC_DATA_CMD register.
    UINT32  RTxEmpty       :1;    //[RO][4] Bheavior differs based on the TX_EMPTY_CTRL selection in the IC_CON register.
                                  //    When TX_EMPTY_CTRL = 0:
                                  //      1:  the transmit buffer is at or below the threshold value set in the IC_TX_TL register.
                                  //    When TX_EMPTY_CTRL = 1:
                                  //      1:  the transmit buffer at or below the threshold value set in the IC_TX_TL register and
                                  //          transmission of address/data from internalshift register for most recently popped command is completed.
                                  //    When IC_ENABLE[0] = 0, the TX FIFO is flushed and held in reset.
                                  //      1: There is activity in master or slave state machines
    UINT32  RRdReq         :1;    //[RO][5] DW_apb_i2c slave with another I2C master status
                                  //      1: Set to 1 when DW_apb_i2c is acting as a slave and another I2C master is attempting
                                  //         to read data from DW_apb_i2c.
                                  //      0: The processor just read the IC_CLR_RD_REQ register
    UINT32  RTxAbrt        :1;    //[RO][6] This bit indicates if a transmit abort has occured
                                  //        if set, the IC_TX_ABRT_SOURCE register contains why the transmit abort has occured
    UINT32  RRxDone        :1;    //[RO][7] When the DW_apb_i2c is acting as a slave-transmitter, set if
                                  //        the master does not acknowledge a transmitted byte, which occurs if transmission is done
    UINT32  RActivity      :1;    //[RO][8] Captures DW_apb_i2c activity and stays set until it is cleared by
                                  //        Disabling DW_apb_i2c, reading IC_CLR_ACTIVITY reading IC_CLR_INTR register, or system reset
    UINT32  RStopDet       :1;    //[RO][9] Indicates whether a STOP condition has occurred on the I2C interface
    UINT32  RStartDet      :1;    //[RO][10] Indicates whether a START or RESTART condition has occurred on the I2C interface
    UINT32  RGenCall       :1;    //[RO][11] Set only when a General Call address is received and it is acknowledged.
                                  //         Cleared if DW_apb_i2c disabled or CPU reads bit 0 of the IC_CLR_GEN_CALL register.
    UINT32  RRestartDet    :1;    //[RO][12] Indicates whether a RESTART condition has occurred on the I2C interface if
                                  //  1. DW_apb_i2c is operating in slave mode
                                  //  2. The slave is the addressed slave
                                  //  3. IC_SLV_RESTART_DET_EN = 1
    UINT32  RMstrOnHold    :1;    //[RO][13] Indicates whether a master is holding the bus and the TX FIFO is empty.
                                  //         Enabled when I2C_DYNAMIC_TAR_UPDATE = 1 and IC_EMPTYFIFO_HOLD_MASTER_EN = 1
    UINT32  Reserved       :18;   //[RO][14:31] Reserved
  } Fields;
  UINT32 Data32;
} SERIAL_IO_I2C_MEM_INTR_STAT;

// 0x40, I2C Clear Combined and Individual Interrupt Register
typedef union {
  struct {
    UINT32  ClrIntr        :1;    //[RO][0] Read this register to clear software clearable interrupts. Clears
                                  // Clears
                                  //   1. The combined interrupt
                                  //   2. All individual interrupts
                                  //   3. The IC_TX_ABRT_SOURCE register
    UINT32  Reserved       :31;    //[RO][1:31] Reserved
  } Fields;
  UINT32 Data32;
} SERIAL_IO_I2C_MEM_CLR_INTR;

// 0x54, I2C Clear TX_ABRT Interrupt Register
typedef union {
  struct {
    UINT32  ClrTxAbrt      :1;    //[RO][0] Read this register to clear TX_ABRT interrupt (bit 6) of the IC_RAW_INTR_STAT register
                                  //        and IC_TX_ABRT_SOURCE register
    UINT32  Reserved       :31;   //[RO][1:31] Reserved
  } Fields;
  UINT32 Data32;
} SERIAL_IO_I2C_MEM_CLR_TX_ABRT;

// 0x6C, I2C Enable Register
typedef union {
  struct {
    UINT32  Enable         :1;    //[RW][0] Controls whether the DW_apb_i2c is enabled.
                                  //    0: Disables DW_apb_i2c (TX and RX FIFOs are held in an erased state)
                                  //    1: Enables DW_apb_i2c
    UINT32  Abort          :1;    //[RW][1] Sofware can abort I2C transfer by setting this bit. Cleared by HW once STOP detected
    UINT32  BusHold        :1;    //[RW][2] In Master mode
                                  //    1: Blocks the transmission of data on I2C bus even if Tx FIFO has data to transmit.
                                  //    0: The transmission of data starts on I2C bus automatically, as soon as
                                  //       the first data is available in the Tx FIFO.
    UINT32  Reserved       :29;    //[RO][3:31] Reserved
  } Fields;
  UINT32 Data32;
} SERIAL_IO_I2C_MEM_ENABLE;

// 0x70, I2C Status Register
typedef union {
  struct {
    UINT32  IcStatusAct    :1;    //[RO][0] I2C Activity Status
    UINT32  TFNF           :1;    //[RO][1] Transmit FIFO Not Full Status
                                  //    1: Transmit FIFO is not full
                                  //    0: Transmit FIFO is full
    UINT32  TFE            :1;    //[RO][2] Transmit FIFO Empty Status
                                  //    1: Transmit FIFO is empty
                                  //    0: Transmit FIFO is not empty
    UINT32  RFNE           :1;    //[RO][3] Receive FIFO Not Empty Status
                                  //    1: Receive FIFO is not empty
                                  //    0: Receive FIFO is empty
    UINT32  RFF            :1;    //[RO][4] Receive FIFO Full Status
                                  //    1: Receive FIFO is full
                                  //    0: Receive FIFO is not full
    UINT32  MstActivity    :1;    //[RO][5] Master FSM Activity Status
                                  //    1: Master FSM is not in IDLE state so the Master part of DW_apb_i2c is Active
                                  //    0: Master FSM is in IDLE state so the Master part of DW_apb_i2c is not Active
    UINT32  SlvActivity    :1;    //[RO][6] Slave FSM Activity Status.
                                  //    1: Slave FSM is not in IDLE state so the Slave part of DW_apb_i2c is Active.
                                  //    0: Slave FSM is in IDLE state so the Slave part of DW_apb_i2c is not Active
    UINT32  Reserved       :25;    //[RO][7:31] Reserved
  } Fields;
  UINT32 Data32;
} SERIAL_IO_I2C_MEM_STATUS;

// 0x74, I2C Transmit FIFO Level Register
//
// This register contains the number of valid data entries in the transmit FIFO buffer.
// It is cleared whenever:
// - The I2C is disabled
// - There is a transmit abort TX_ABRT bit is set in the IC_RAW_INTR_STAT register
// - The slave bulk transmit mode is aborted
// The register increments whenever data is placed into the transmit FIFO and decrements when data is taken from the transmit FIFO.
//
typedef union {
  struct {
    UINT32  TXFLR          :9;    //[RO][0:8] TXFLR
                                  //    Transmit FIFO Level. Contains the number of valid data entries in the transmit FIFO.
    UINT32  Reserved       :23;    //[RO][9:31] Reserved
  } Fields;
  UINT32 Data32;
} SERIAL_IO_I2C_MEM_TRANSMIT_FIFO;

// 0x78, I2C Recieve FIFO Level Register
//
// This register contains the number of valid data entries in the receive FIFO buffer. It is cleared
// whenever:
// - The I2C is disabled
// - Whenever there is a transmit abort caused by any of the events tracked in IC_TX_ABRT_SOURCE
// The register increments whenever data is placed into the receive FIFO and decrements when data is taken
// from the receive FIFO.
//
typedef union {
  struct {
    UINT32  RXFLR          :9;    //[RO][0:8] RXFLR
                                  //    Receive FIFO Level. Contains the number of valid data entries in the receive FIFO
    UINT32  Reserved       :23;    //[RO][9:31] Reserved
  } Fields;
  UINT32 Data32;
} SERIAL_IO_I2C_MEM_RECEIVE_FIFO;

// 0x7C, I2C SDA Hold Time Length Register
typedef union {
  struct {
    UINT32  IcSdaTxHold    :16;   //[RW][0:15] Sets the required SDA hold time in units of ic_clk period
                                  //    when the I2C Host Controller acts as a transmitter
    UINT32  IcSdaRxHold    :8;    //[RW][16:23] Sets the required SDA hold time in units of ic_clk period,
                                  //    when the I2C Host Controller acts as a receiver
    UINT32  Reserved       :8;    //[RO][24:31] Reserved
  } Fields;
  UINT32 Data32;
} SERIAL_IO_I2C_MEM_SDA_HOLD;

// 0x80, I2C Transmit Abort Status Register
typedef union {
  struct {
    UINT32  Abrt7bAddrNoAck     :1;    //[RO][0] 1: Master in 7-bit address mode and
                                       //           the address sent was not acknowledged by any slave.
    UINT32  Abrt10bAddr1NoAck   :1;    //[RO][1] 1: Master in 10-bit address mode and
                                       //           the first 10-bit address byte was not acknowledged by any slave.
    UINT32  Abrt10bAddr2NoAck   :1;    //[RO][2] 1: Master is in 10-bit address mode and
                                       //           the second address byte of the 10-bit address was not acknowledged by any slave.
    UINT32  AbrtTxDataNoAck     :1;    //[RO][3] 1: Master mode only: Master has received an acknowledgement for the
                                       //           address, but when it sent data byte(s) following the address, it did not receive an
                                       //           acknowledge from the remote slave(s).
    UINT32  AbrtGCallNoAck      :1;    //[RO][4] 1: DW_apb_i2c in master mode sent a General Call and no slave on bus acknowledged the General Call.
    UINT32  AbrtGCallRead       :1;    //[RO][5] 1: DW_apb_i2c in master mode sent a General Call but the user programmed the byte
                                       //           following the General Call to be a read from the bus (IC_DATA_CMD[9] is set to 1).
    UINT32  AbrtHsAckDet        :1;    //[RO][6] 1: Master is in High Speed mode and the High Speed Master code was acknowledged (wrong behavior)
    UINT32  AbrtSbAckDet        :1;    //[RO][7] 1: Master has sent a START Byte and the START Byte was acknowledged (wrong behavior).
    UINT32  AbrtHsNoRst         :1;    //[RO][8] 1: The restart is disabled (IC_RESTART_EN bit (IC_CON[5]) = 0) and the user is
                                       //           trying to use the master to transfer data in High Speed mode.
    UINT32  AbrtSbNoRst         :1;    //[RO][9] 1: The restart is disabled (IC_RESTART_EN bit (IC_CON[5]) = 0) and the user is
                                       //           trying to send a START Byte.
    UINT32  Abrt10bRdNoRst      :1;    //[RO][10] 1: The restart is disabled (IC_RESTART_EN bit (IC_CON[5]) = 0) and the master
                                       //            sends a read command in 10-bit addressing mode.
    UINT32  AbrtMasterDis       :1;    //[RO][11] 1: User tries to initiate a Master operation with the Master mode disabled.
    UINT32  ArbLost             :1;    //[RO][12] 1: Master has lost arbitration
                                       //            if IC_TX_ABRT_SOURCE[14] is also set, then the slave transmitter has lost arbitration.
    UINT32  AbrtSlFlushTxFifo   :1;    //[RO][13] 1: Slave has received a read command and some data exists in the TX FIFO so the
                                       //            slave issues a TX_ABRT interrupt to flush old data in TX FIFO.
    UINT32  AbrtSlvArbLost      :1;    //[RO][14] 1: Slave lost the bus while transmitting data to a remote master.
    UINT32  AbrtSlvRdIntx       :1;    //[RO][15] 1: Occurs when the processor side responds to a slave mode request for data to be transmitted
                                       //            to a remote master and user writes a 1 in CMD (bit 8) of IC_DATA_CMD register.
    UINT32  AbrtUserAbrt        :1;    //[RO][16] Master only: Master has detected the user initiated transfer abort (IC_ENABLE[1])
    UINT32  Reserved            :6;    //[RO][17:22] Reserved
    UINT32  TxFlushCnt          :9;    //[RO][23:31] This field preserves the TXFLR value prior to the last TX_ABRT event.
                                       //            It is cleared whenever I2C is disabled.
  } Fields;
  UINT32 Data32;
} SERIAL_IO_I2C_MEM_TX_ABRT_SOURCE;

// 0x9C, I2C Enable Status Register
// used to report the DW_apb_i2c hardware status when the IC_ENABLE register is set from 1 to 0; that is, when DW_apb_i2c is disabled.
// If IC_ENABLE has been set to 1, bits 2:1 are forced to 0, and bit 0 is forced to 1.
// If IC_ENABLE has been set to 0, bits 2:1 is only be valid as soon as bit 0 is read as 0
// When IC_ENABLE has been written with 0 a delay occurs for bit 0 to be read as 0 because disabling the DW_apb_i2c
// depends on I2C bus activities
typedef union {
  struct {
    UINT32  IcEn           :1;    //[RO][0] ic_en Status. This bit always reflects the value driven on the output port ic_en
                                  //    1: DW_apb_i2c is deemed to be in an enabled state.
                                  //    0: DW_apb_i2c is deemed completely inactive.
    UINT32  SlvDisWBusy    :1;    //[RO][1] This bit indicates if a potential or active Slave operation has been aborted due to the
                                  //        setting of the IC_ENABLE register from 1 to 0. This bit is set when the CPU writes a 0
    UINT32  SlvRxDataLost  :1;    //[RO][2] This bit indicates if a Slave-Receiver operation has been aborted with at least one data
                                  //        byte received from an I2C transfer due to the setting of IC_ENABLE from 1 to 0.
    UINT32  Reserved       :29;   //[RO][3:31] Reserved
  } Fields;
  UINT32 Data32;
} SERIAL_IO_I2C_MEM_ENABLE_STATUS;

// 0x204, I2C Soft Reset Register
typedef union {
  struct {
    UINT32  ResetI2C       :2;    //[RW][0:1] I2C Host Controller reset.
                                  //    00 : I2C Host Controller is in reset (Reset Asserted)
                                  //    11 : I2C Host Controller is NOT at reset (Reset Released)
    UINT32  ResetDma       :1;    //[RW][2] iDMA Software Reset Control
                                  //    0 : IP is in reset (Reset Asserted)
                                  //    1 : IP is NOT at reset (Reset Released)
    UINT32  Reserved       :29;   //[N/A][3:31] Reserved
  } Fields;
  UINT32 Data32;
} SERIAL_IO_I2C_MEM_RESETS;

// 0x208, I2C Convergence Layer General Register
typedef union {
  struct {
    UINT32  Reserved0      :2;   //[N/A][0:1] Reserved
    UINT32  LtrMode        :1;    //[RW][2] LTR Mode Configure
                                  //    1: SW Mode: The software will write to the host controller's SW LTR register (offset 0x810)
                                  //    0: Auto Mode: The BIOS will write to the host controller's Auto LTR register (offset 0x814)
    UINT32  IoVoltageSel   :1;    //[RW][3] IO voltage select bit in i2c GENERAL reg
    UINT32  TxLastBFlag    :1;    //[RW][4] Last byte Acknowledged of a TX transaction
    UINT32  Reserved1      :19;   //[N/A][5:23] Reserved bits in i2c GENERAL reg
    UINT32  SclRdPostDr    :1;    //[RO][24] I2c_scl_rd_post_drive, scl line state
                                  //    1: high impedance
                                  //    0: output drives low
    UINT32  SclRdPreDr     :1;    //[RO][25] I2c_scl_rd_pre_drive, scl line control from IP
                                  //    1: output driver enabled (drives low)
                                  //    0: high impedance
    UINT32  SdaRdPostDr    :1;    //[RO][26] I2c_sda_rd_post_drive, sda line state
                                  //    1: high impedance
                                  //    0: output drives low
    UINT32  SdaRdPreDr     :1;    //[RO][27] I2c_sda_rd_pre_drive, sda line control from IP
                                  //    1: output driver enabled (drives low)
                                  //    0: high impedance
    UINT32  SclSigState    :1;    //[RW][28] Scl_signal_state, scl line control from SW reg
                                  //    1: output driver enabled (drives low)
                                  //    0: high impedance
    UINT32  SclMuxSel     :1;    //[RW][29] Scl_mux_sel
                                  //    1: SW override
                                  //    0: native mode
    UINT32  SdaSigState   :1;     //[RW][30] Sda_signal_state, sda line control from SW reg
                                  //    1: output driver enabled (drives low)
                                  //    0: high impedance
    UINT32  SdaMuxSel     :1;    //[RW][31] Sda_mux_sel,
                                  //    1: SW override
                                  //    0: native mode
  } Fields;
  UINT32 Data32;
} SERIAL_IO_I2C_GENERAL;

//I2C Post Code
/**
  Enable I2C device when in PCI/Hidden if it has assigned Memory resources

  - Enable Memory space decoding
  - Place device in D0
  - Get it out of reset

  Also checks for SCL/SDA line states.

  @param[in]       PciCfgBase    Pci Config Base Address of specified I2C controller
  @param[in][out]  BaseAddress   MMIO Base Address of specified I2C controller

  @retval   TRUE    I2C successfully enabled
  @retval   FALSE   I2C does not have memory assigned or SDA/SCL are not high
**/
BOOLEAN
SerialIoI2cEnable (
  IN UINTN           PciCfgBase,
  IN OUT UINT64      *BaseAddress
  );


/**
  Enable I2C device when in Hidden only if it has assigned Memory resources

  - Enable Memory space decoding
  - Place device in D0
  - Get it out of reset

  Also checks for SCL/SDA line states.

  @param[in]       PciCfgBase    Pci Config Base Address of specified I2C controller
  @param[in][out]  BaseAddress   MMIO Base Address of specified I2C controller

  @retval   TRUE    I2C successfully enabled
  @retval   FALSE   I2C does not have memory assigned or SDA/SCL are not high
**/
BOOLEAN
SerialIoI2cHiddenEnable (
  IN UINTN           PciCfgBase,
  IN OUT UINT64      *BaseAddress
  );


/**
  Enable I2C device when in PCI only if it has assigned Memory resources

  - Enable Memory space decoding
  - Place device in D0
  - Get it out of reset

  Also checks for SCL/SDA line states.

  @param[in]       PciCfgBase    Pci Config Base Address of specified I2C controller
  @param[in][out]  BaseAddress   MMIO Base Address of specified I2C controller

  @retval   TRUE    I2C successfully enabled
  @retval   FALSE   I2C does not have memory assigned or SDA/SCL are not high
**/
BOOLEAN
SerialIoI2cPciEnable (
  IN UINTN           PciCfgBase,
  IN OUT UINT64      *BaseAddress
  );

/**
  Disable I2C device when in PCI only if it has assigned Memory resources

  - Disables DW_apb_i2c (TX and RX FIFOs are held in an erased state)
  - Place it in reset state
  - Place device in D3
  - Disable Memory space decoding

  @param[in]       PciCfgBase    Pci Config Base Address of specified I2C controller

  @retval   TRUE    I2C successfully enabled
  @retval   FALSE   I2C does not have memory assigned
**/
VOID
SerialIoI2cPciDisable (
  IN UINTN           PciCfgBase
  );

/**
  Enables DW_apb_i2c and checks for ic_en status

  @param[in]  MmioBase   MMIO Base Address of specified I2C controller

  @retval   EFI_SUCCESS    I2C successfully enabled
  @retval   EFI_TIMEOUT    I2C did not enable in given time refer to I2cEnableTimeout
**/
EFI_STATUS
SerialIoI2cEnableController (
  IN UINTN   MmioBase
  );

/**
  Disables DW_apb_i2c (TX and RX FIFOs are held in an erased state) and checks for ic_en status

  @param[in]  MmioBase   MMIO Base Address of specified I2C controller

  @retval   EFI_SUCCESS    I2C successfully disabled
  @retval   EFI_TIMEOUT    I2C did not enable in given time refer to I2cDisableTimeout
**/
EFI_STATUS
SerialIoI2cDisableController (
  IN UINTN   MmioBase
  );

/**
  Clear software clearable interrupts and TX_ABRT interrupt of IC_RAW_INTR_STAT and ABRT_SOUCE registers
  This clear is done by reading the I2C_MEM_CLR_INTR and I2C_MEM_CLR_TX_ABRT registers

  @param[in]   MmioBase   MMIO Base Address of specified I2C controller
**/
VOID
SerialIoI2cClearErrors (
  IN UINTN  MmioBase
  );

/**
  Set the SDA Hold and speed specific SCL high count and SCL low count registers

  @param[in]       MmioBase       MMIO Base Address of specified I2C controller
  @param[in][out]  Frequency      Pointer to bus clock frequency (in kHz)
  @param[in]       TimingConfig   Optional: Setup variable struct containing override values for SCL LCNT/HCNT
                                    User can override version specific timer values in PCH Setup or specify NULL
                                    to use default config
**/
VOID
SerialIoI2cSetFrequency (
  IN     UINTN                 MmioBase,
  IN OUT UINTN                 *Frequency,
  IN     SERIAL_IO_I2C_TIMING  *TimingConfig   OPTIONAL
  );

/**
  Write the given slave address to the I2C Target Address Register

  @param[in]   MmioBase       MMIO Base Address of specified I2C controller
  @param[in]   SlaveAddress   Address of the end point device
**/
VOID
SerialIoI2cSetSlaveAddress (
  IN UINTN   MmioBase,
  IN UINT32  SlaveAddress
  );

/**
  Configure the I2C Control Register and start the controller

  @param[in]   MmioBase     MMIO Base Address of specified I2C controller
  @param[in]   Frequency    Pointer to bus clock frequency (in kHz)

  @retval   EFI_TIMEOUT   I2C enable confirmation timed out
  @retval   EFI_SUCCESS   I2C successfully enabled
**/
VOID
SerialIoI2cStartOperation (
  IN UINTN   MmioBase,
  IN UINTN   *Frequency
  );

/**
  Handle the writing operations of the I2C controller

  @param[in][out]   WriteLength  Pointer to write length
  @param[in][out]   WriteBuffer  Double pointer to data in write buffer
  @param[in]        MmioBase     MMIO Base Address of specified I2C controller
  @param[in]        Stop         Issues STOP on the last byte
  @param[in]        Restart      Indication if restart is needed

  @retval   TRUE       Data added to TX FIFO
            FALSE      TX FIFO is full or at its limit - Write did not complete
**/
BOOLEAN
SerialIoI2cPrivateWrite (
  IN OUT UINT32  *WriteLength,
  IN OUT UINT8   **WriteBuffer,
  IN     UINTN   MmioBase,
  IN     BOOLEAN Stop,
  IN     BOOLEAN Restart
  );

/**
  Handle the reading operations of the I2C controller

  @param[in][out]   ReadLength   Pointer to read length
  @param[in][out]   ReadBuffer   Double pointer to data in read buffer
  @param[in]        MmioBase     MMIO Base Address of specified I2C controller
  @param[in]        Restart      Indication if restart is needed
**/
EFI_STATUS
SerialIoI2cPrivateRead (
  IN OUT UINT32  *ReadLength,
  IN OUT UINT8   **ReadBuffer,
  IN     UINTN   MmioBase,
  IN     BOOLEAN Restart
  );

/**
  Set up the I2C Controller for reading and/or writing

  Before any transaction:
   1. Verify I2C Controller is active
   2. Disable I2C Controller
   3. Clear any errors / interrupts
   4. Set I2C frequency values
   5. Write slave address to I2C target address register
   6. Initiate I2C Controller

  @param[in]   MmioBase        MMIO Base Address of specified I2C controller
  @param[in]   Slave Address  -Address of the end point device
  @param[in]   *Frequency      Pointer to bus clock frequency (in kHz)
  @param[in]   *TimingConfig   Pointer to optional struct passed down to SerialIoI2cSetFrequency ()

  @retval   EFI_TIMEOUT        Timeout occurred in I2C disable/enable
  @retval   EFI_DEVICE_ERROR   The verification failed
  @retval   EFI_SUCCESS        I2C W/R successfully initialized

**/
EFI_STATUS
SerialIoI2cInitializeWriteRead (
  IN UINTN                 MmioBase,
  IN UINT32                SlaveAddress,
  IN UINTN                 *Frequency,
  IN SERIAL_IO_I2C_TIMING  *TimingConfig  OPTIONAL
  );

/**
  Get the I2C SDA Hold value for the given frequency

  @param[in]       MmioBase    MMIO Base Address of specified I2C controller
  @param[in][out]  SdaHold     Pointer to register union for R_I2C_MEM_SDA_HOLD
  @param[in]       Frequency   Pointer to bus clock frequency (in kHz)
**/
VOID
SerialIoI2cGetSdaHold (
  IN      UINTN                       MmioBase,
  IN OUT  SERIAL_IO_I2C_MEM_SDA_HOLD  *SdaHold,
  IN      UINTN                       *Frequency
  );

/**
  Get Standard Speed I2C Clock SCL High/Low Count Register value
  Applies to frequency in range [100kHz, 400kHz)

  @param[in]        MmioBase    MMIO Base Address of specified I2C controller
  @param[in][out]   SsSclHcnt   Pointer to register union for R_I2C_MEM_SS_SCL_HCNT
  @param[in][out]   SsSclLcnt   Pointer to register union for R_I2C_MEM_SS_SCL_LCNT
  @param[in]        Frequency   Pointer to bus clock frequency (in kHz)
**/
VOID
SerialIoI2cGetSsScl (
  IN      UINTN                           MmioBase,
  IN OUT  SERIAL_IO_I2C_MEM_SS_SCL_HCNT  *SsSclHcnt,
  IN OUT  SERIAL_IO_I2C_MEM_SS_SCL_LCNT  *SsSclLcnt,
  IN      UINTN                          *Frequency
  );

/**
  Get Fast Speed I2C Clock SCL High/Low Count Register value
  Applies to frequency in range [400kHz, 3.4MHz)

  @param[in]        MmioBase    MMIO Base Address of specified I2C controller
  @param[in][out]   FsSclHcnt   Pointer to register union for R_I2C_MEM_FS_SCL_HCNT
  @param[in][out]   FsSclLcnt   Pointer to register union for R_I2C_MEM_FS_SCL_LCNT
  @param[in]        Frequency   Pointer to bus clock frequency (in kHz)
**/
VOID
SerialIoI2cGetFsScl (
  IN      UINTN                           MmioBase,
  IN OUT  SERIAL_IO_I2C_MEM_FS_SCL_HCNT  *FsSclHcnt,
  IN OUT  SERIAL_IO_I2C_MEM_FS_SCL_LCNT  *FsSclLcnt,
  IN      UINTN                          *Frequency
  );

/**
  Get High Speed I2C Clock SCL High/Low Count Register value
  Applies to frequency in range [3.4MHz, inf)

  @param[in]        MmioBase    MMIO Base Address of specified I2C controller
  @param[in][out]   HsSclHcnt   Pointer to register union for R_I2C_MEM_HS_SCL_HCNT
  @param[in][out]   HsSclLcnt   Pointer to register union for R_I2C_MEM_HS_SCL_LCNT
  @param[in]        Frequency   Pointer to bus clock frequency (in kHz)
**/
VOID
SerialIoI2cGetHsScl (
  IN      UINTN                           MmioBase,
  IN OUT  SERIAL_IO_I2C_MEM_HS_SCL_HCNT  *HsSclHcnt,
  IN OUT  SERIAL_IO_I2C_MEM_HS_SCL_LCNT  *HsSclLcnt,
  IN      UINTN                          *Frequency
  );

/**
  Checks if transmission was aborted

  @param[in]  MmioBase    MMIO Base Address of specified I2C controller

  @retval     TRUE        Tx Abort occured
  @retval     FALSE       No Tx Abort errors
**/
BOOLEAN
SerialIoI2cIsRTxAbrt (
  UINTN   MmioBase
  );

/**
  Checks if Transmit FIFO is empty

  @param[in]  MmioBase    MMIO Base Address of specified I2C controller

  @retval     TRUE        Tx FIFO is empty
  @retval     FALSE       Tx FIFO not empty
**/
BOOLEAN
SerialIoI2cIsTxFifoEmpty (
  UINTN   MmioBase
  );

/**
  Dumps all Tx Abort Source register fields

  @param[in]  MmioBase    MMIO Base Address of specified I2C controller
**/
VOID
SerialIoI2cDumpTxAbort (
  UINTN   MmioBase
  );

/**
  Waits until Tx Fifo is empty and I2C no longer Active

  @param[in]  MmioBase    MMIO Base Address of specified I2C controller

  @retval   EFI_TIMEOUT   TX FIFO did not clear or I2C was still active
  @retval   EFI_SUCCESS   TX FIFO is empty and I2C is idle
**/
EFI_STATUS
SerialIoI2cWaitTxFifoEmptyIcNotActive (
  UINTN   MmioBase
  );

#endif //_SERIAL_IO_I2C_PRIVATE_LIB_H_
