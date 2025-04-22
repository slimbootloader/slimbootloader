/** @file

  Copyright (c) 2021, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <Uefi/UefiBaseType.h>
#include <Library/BaseLib.h>

#define PCI_BUS_NUMBER_SPD       30
#define PCI_DEVICE_NUMBER_SPD    11
#define PCI_FUNC_NUMBER_SPD       0

#define MAX_IMC                  2 /* Maximum Number of Memory Controllers */
#define MAX_CHANNEL              3 /* Maximum Number of Memory Channels */
#define MAX_DIMM                 2 /* Maximum Number of DIMMs PER Memory Channel */
#define TDIFF_UNIT_US            1000000

#define SMB_CMD_CFG_OFFSET       0x80
#define SMB_STATUS_CFG_OFFSET    0x84
#define SMB_DATA_CFG_OFFSET      0x88
#define SMB_PERIOD_CFG_OFFSET    0x90

//
// SMBCNTL Register smb_dti bit definitions
//
#define DTI_EEPROM               0x0A  // '1010' specifies EEPROM's
#define DTI_WP_EEPROM            0x06  // '0110' specifies a write-protect operation for an EEPROM

//
// SPD Page Defines
//
#define SPD_EE_PAGE_SELECT_0     6   // Page 0 - lower 256 EEPROM bytes for DDR4
#define SPD_EE_PAGE_SELECT_1     7   // Page 1 - upper 256 EEPROM bytes for DDR4
#define SPD_EE_PAGE_SIZE         256 // Size of 1 SPD page for DDR4

//
// SMBUS Clk Period default set by PcdMrcSmbusSpeedDefault
//
#define SMB_CLK_100K       0
#define SMB_CLK_400K       1
#define SMB_CLK_700K       2
#define SMB_CLK_1M         3

//
// Available SMBus clock periods to be programmed.
//
typedef enum {
  SmbClk100K = SMB_CLK_100K,     // 100 Khz in I2C mode; 4Mhz in I3C mode
  SmbClk400K = SMB_CLK_400K,     // 400 Khz in I2C mode; 6Mhz in I3C mode
  SmbClk700K = SMB_CLK_700K,     // 700 Khz in I2C mode; 8Mhz in I3C mode
  SmbClk1M   = SMB_CLK_1M,       // 1 Mhz in I2C mode; 10Mhz in I3C mode
  SmbClkPeriodMax
} SMB_CLOCK_FREQUENCY;

// Clock Period values (for SMBUS freq) are based on 100Mhz FXCLK
#define SMB_100K_CLK_PERIOD    1000  // Clock period to be programmed for 100Khz
#define SMB_100K_CLK_OFFSET    35    // Clock offset to be programmed for 100Khz
#define SMB_400K_CLK_PERIOD    250   // Clock period to be programmed for 400Khz
#define SMB_400K_CLK_OFFSET    35    // Clock offset to be programmed for 400Khz
#define SMB_700K_CLK_PERIOD    144   // Clock period to be programmed for 700Khz
#define SMB_700K_CLK_OFFSET    30    // Clock offset to be programmed for 700Khz
#define SMB_1M_CLK_PERIOD      100   // Clock period to be programmed for 1Mhz
#define SMB_1M_CLK_OFFSET      19    // Clock offset to be programmed for 1Mhz


#define SMB_TIMEOUT            100000   // 100 ms


//
// Polling logic for the bits in status register
// smb_busy and smb_sbe are polling 0
// smb_rdo and smb_wod are polling 1
//
#define SMB_READ_DONE_POLLING_LOGIC  1
#define SMB_WRITE_DONE_POLLING_LOGIC 1

typedef union {
  ///
  /// Individual bit fields
  ///
  struct {
    UINT32 smb_ba : 8;

                            /* Bits[7:0], Access Type=RW/P, default=None*/

                            /*
                               This field identifies the bus transaction
                               address to be accessed. Note: in WORD access,
                               23:16 specifies 2B access address. In Byte
                               access, 23:16 specified 1B access address. Also
                               known as the command code in SMBus terms.
                            */
    UINT32 smb_sa : 3;

                            /* Bits[10:8], Access Type=RW/P, default=None*/

                            /*
                               SMBus Slave Address. This field identifies the
                               DIMM SPD/TSOD to be accessed.
                            */
    UINT32 smb_dti : 4;

                            /* Bits[14:11], Access Type=RW/P, default=None*/

                            /*
                               This field specifies the device type identifier.
                               Only devices with these device-type will respond
                               to commands: 0011 specifies TSOD. 1010 specifies
                               EEPROMs. 0110 specifies a write-protect
                               operation for an EEPROM. 1110 specifies a
                               4-channel I2C-bus multiplexer. Other identifiers
                               can be specified to target non-EEPROM devices on
                               the SMBus. Note: IMC based hardware TSOD polling
                               uses hard coded DTI. Changing this field has no
                               effect on the hardware based TSOD polling.
                            */
    UINT32 smb_wrt : 2;

                            /* Bits[16:15], Access Type=RW/P, default=None*/

                            /*
                               These bits select SMBus Read/Write operation.
                               Bit[16:15]=00 for SMBus Read, Bit[16:15]=01 for
                               SMBus Write, Bit[16:15]=11 for writing to the
                               pointer register. Bit[16:15]=10 is an illegal
                               combination. Note 1: smb_pntr_sel and
                               smb_word_access are ignored when writing to the
                               pointer register. Note 2: smb_dis_wrt will NOT
                               disable WrtPntr update command.
                            */
    UINT32 smb_word_access : 1;

                            /* Bits[17:17], Access Type=RW/P, default=None*/

                            /*
                               Enables Word Access. The SMBus controller
                               performs SMBus word (2 bytes) access when set;
                               otherwise, it performs a byte access.
                            */
    UINT32 smb_pntr_sel : 1;

                            /* Bits[18:18], Access Type=RW/P, default=None*/

                            /*
                               When set, enables SMBus pointer based access;
                               otherwise, the random access protocol is used.
                               Hardware based TSOD polling will also use this
                               bit to enable the pointer word read. Important
                               Note: Hardware based TSOD polling can be
                               configured with pointer based access. If
                               software manually issue SMBus transaction to
                               other address, i.e. changing the pointer in the
                               slave device, it is software's responsibility to
                               restore the pointer in each TSOD before
                               returning to hardware based TSOD polling while
                               keeping the SMB_PNTR_SEL=1.
                            */
    UINT32 smb_cmd_trigger : 1;

                            /* Bits[19:19], Access Type=RW/V, default=0x00000000*/

                            /*
                               CMD trigger. After setting this bit to 1, the
                               SMBus master will issue the SMBus command using
                               the other fields written in SMB_CMD_CFG. Note:
                               Hardware will reset this bit when the SMBus
                               command is being started.
                            */
    UINT32 smb_tsod_poll_en : 1;

                            /* Bits[20:20], Access Type=RW, default=0x00000000*/

                            /*
                               Enables hardware-based TSOD Polling. This field
                               does not affect the current SMBus transaction in
                               flight. A change takes effect once the bus is no
                               longer busy. If BIOS/BMC would like to use
                               SMBus, it must first suspend TSOD polling by
                               setting this field to 0 and wait for the bus to
                               become free. When BIOS/BMC is done with the bus,
                               it must resume TSOD polling by setting this
                               field to 1.
                            */
    UINT32 reserved : 1;

                            /* Bits[21:21], Access Type=RW, default=0x00000000*/

                            /* Reserved for future expansion */
    UINT32 rsvd : 2;

                            /* Bits[23:22], Access Type=RO, default=None*/

                            /* Reserved */
    UINT32 smb_soft_rst : 1;

                            /* Bits[24:24], Access Type=RW, default=0x00000000*/

                            /*
                               SMBus software reset strobe to gracefully
                               terminate the pending transaction (after ACK)
                               and keep the SMB controller from issuing any
                               transaction until this bit is cleared. If the
                               slave device is hung, software can write this
                               bit to 1 and the SMB_CKOVRD to 0 (for more than
                               35ms) to force the hung SMB slaves to time-out
                               and put the controller in idle state without
                               using power good reset or warm reset. Note:
                               software needs to set the SMB_CKOVRD back to 1
                               after 35ms in order to force slave devices to
                               time-out in case there is any pending
                               transaction. The corresponding
                               SMB_STATUS_CFG.SMB_SBE error status bit may be
                               set if there was such pending transaction time-
                               out (non-graceful termination). If the pending
                               transaction was a write operation, the slave
                               device content may be corrupted by this clock
                               override operation. A subsequent SMBus command
                               will automatically clear the SMB_SBE. The IMC
                               H/W will perform SMBus time-out (if the
                               SMB_SBE_EN=1). Software should simply clear the
                               SMB_SBE and SMB_SOFT_RST sequentially after
                               writing the SMB_CKOVRD=0 and SMB_SOFT_RST=1
                               (asserting clock override and perform graceful
                               txn termination). Hardware will automatically
                               de-assert the SMB_CKOVRD (update to 1) after the
                               pre-configured 35ms/65ms time-out period.
                            */
    UINT32 smb_sbe_err0_en : 1;

                            /* Bits[25:25], Access Type=RW, default=0x00000000*/

                            /*
                               Enables ERR0 assertion when SMB_SBE is 0 --> 1.
                               If enabled, sends an ERROR message to Global IEH
                               when smb_sbe is detected.
                            */
    UINT32 smb_sbe_smi_en : 1;

                            /* Bits[26:26], Access Type=RW, default=0x00000000*/

                            /*
                               Enables SMI generation when SMB_SBE is 0 --> 1.
                               If enabled, sends an SMI interrupt when smb_sbe
                               is detected.
                            */
    UINT32 smb_sbe_en : 1;

                            /* Bits[27:27], Access Type=RW, default=0x00000000*/

                            /*
                               SMBus error recovery enable. When set, hardware
                               will automatically reset smb_ckovrd to 1 after
                               TLOW time-out counter (SMB_TLOW_TIMEOUT_CNTR)
                               expires.
                            */
    UINT32 smb_dis_wrt : 1;

                            /* Bits[28:28], Access Type=RW, default=0x00000000*/

                            /*
                               Disables SMBus Write. Writing a 0 to this bit
                               enables CMD write bit to be set to 1; Writing a
                               1 forces SMB_WRT[0] bit to be always 0, i.e.
                               disabling SMBus write. SMBus Read is not
                               affected. SMBus Write Pointer Update Command is
                               not affected. Either SMB_DIS_WRT=1 or the
                               FUSE_DIS_SMBUS_WRT=1 will disable the SMBus
                               Write Capability. Note: SMBus write commands are
                               silently converted to SMBus read commands. No
                               error is logged or signalled.
                            */
    UINT32 smb_ckovrd : 1;

                            /* Bits[29:29], Access Type=RW/V/P, default=None*/

                            /*
                               0 = Overrides Clock signal to low. 1 = Clock
                               signal is released high, allowing normal
                               operation of CMD. Toggling this bit can be used
                               to budge the port out of a stuck state. Software
                               can write this bit to 0 and the SMB_SOFT_RST to
                               1 to force hung SMBus controller and the SMB
                               slaves to idle state without using power good
                               reset or warm reset. Note: software needs to set
                               the SMB_CKOVRD back to 1 after 35ms in order to
                               force slave devices to time-out in case there is
                               any pending transaction. The corresponding
                               SMB_STATUS_CFG.SMB_SBE error status bit may be
                               set if there was such pending transaction time-
                               out (non-graceful termination). If the pending
                               transaction was a write operation, the slave
                               device content may be corrupted by this clock
                               override operation. A subsequent SMB command
                               will automatically clear the SMB_SBE.
                               Alternatively, if SMBUS error recovery is
                               enabled, the SMBus time-out control timer will
                               start when ckovrd is pulled low. When the time-
                               out control timer expires, the SMB_CKOVRD# will
                               de-assert, i.e. return to 1 value and clear the
                               SMB_SBE=0.
                            */
    UINT32 rsvd_30 : 2;

                            /* Bits[31:30], Access Type=RO, default=None*/

                            /* Reserved */

  } Bits;
  UINT32 Data;

} SMB_CMD_CFG;


typedef union {
  ///
  /// Individual bit fields
  ///
  struct {
    UINT32 smb_busy : 1;

                            /* Bits[0:0], Access Type=RO/V/P, default=None*/

                            /*
                               SMBus Busy state. This bit is set by iMC while
                               an SMBus/I2C command (including TSOD command
                               issued from IMC hardware) is executing. Any
                               transaction that is completed normally or
                               gracefully will clear this bit automatically. By
                               setting the SMB_SOFT_RST will also clear this
                               bit. This register bit is sticky across reset so
                               any surprise reset during pending SMBus
                               operation will sustain the bit assertion across
                               surprised warm-reset. BIOS reset handler can
                               read this bit before issuing any SMBus
                               transaction to determine whether a slave device
                               may need special care to force the slave to idle
                               state (e.g. via clock override toggling
                               (SMB_CKOVRD) and/or via induced time-out by
                               asserting SMB_CKOVRD for 25-35ms).
                            */
    UINT32 smb_sbe : 1;

                            /* Bits[1:1], Access Type=RO/V, default=0x00000000*/

                            /*
                               SMBus Error. This bit is set by H/W if an SMBus
                               transaction (including the TSOD polling or
                               message channel initiated SMBus access) that
                               does not complete successfully (non-Ack has been
                               received from slave at expected Ack slot of the
                               transferNote: Once SMB_SBE bit is set, H/W stops
                               issuing hardware initiated TSOD polling SMBUS
                               transactions until the SMB_SBE is cleared. H/W
                               will not increment the SMB_STATUS_CFG.TSOD_SA or
                               update SMB_STATUS_CFG.last_dti or
                               SMB_STATUS_CFG.last_branch_config until the
                               SMB_SBE is cleared. Manual SMBus command
                               interface is not affected, i.e. new command
                               issue will clear the SMB_SBE.
                            */
    UINT32 smb_rdo : 1;

                            /* Bits[2:2], Access Type=RO/V, default=0x00000000*/

                            /*
                               Read Data Valid. This bit is set by iMC when the
                               Data field of this register receives read data
                               from the SPD/TSOD after completion of an SMBus
                               read command. It is cleared by iMC when a
                               subsequent SMBus read command is issued.
                            */
    UINT32 smb_wod : 1;

                            /* Bits[3:3], Access Type=RO/V, default=0x00000000*/

                            /*
                               Write Operation Done. This bit is set by iMC
                               when a SMBus Write command has been completed on
                               the SMBus. It is cleared by iMC when a
                               subsequent SMBus Write command is issued
                            */
    UINT32 rsvd : 4;

                            /* Bits[7:4], Access Type=RO, default=None*/

                            /* Reserved */
    UINT32 tsod_sa : 3;

                            /* Bits[10:8], Access Type=RO/V, default=0x00000000*/

                            /*
                               Captures the slave address of the last TSOD
                               device polled.
                            */
    UINT32 last_dti : 4;

                            /* Bits[14:11], Access Type=RO/V, default=0x00000000*/

                            /*
                               Captures the Device Type Identified code
                               specified in the last issued BIOS/BMC or HW
                               command. BMC can use this information to
                               determine if an error on SMBus is caused by
                               SMBus mux or by a DIMM.
                            */
    UINT32 rsvd_15 : 1;

                            /* Bits[15:15], Access Type=RO, default=None*/

                            /* Reserved */
    UINT32 last_branch_cfg : 3;

                            /* Bits[18:16], Access Type=RO/V, default=0x00000000*/

                            /*
                               Captures the SMBus mux branch configuration in
                               the last issued BIOS/BMC or HW command to SMBUS
                               mux. BMC can use this information to determine
                               which DIMM on which branch has caused an error
                               on SMBus. Bit 2 is defined as the mux enable
                               bit. Bits [1:0] are defined as the branch to be
                               selected.
                            */
    UINT32 rsvd_19 : 1;

                            /* Bits[19:19], Access Type=RO, default=None*/

                            /* Reserved */
    UINT32 tsod_poll_en : 1;

                            /* Bits[20:20], Access Type=RO/V, default=0x00000000*/

                            /* Indicates if TSOD Polling is enabled on SMBus. */
    UINT32 rsvd_21 : 11;

                            /* Bits[31:21], Access Type=RO, default=None*/

                            /* Reserved */

  } Bits;
  UINT32 Data;

} SMB_STATUS_CFG;

typedef union {
  ///
  /// Individual bit fields
  ///
  struct {
    UINT32 smb_rdata : 16;

                            /* Bits[15:0], Access Type=RO/V, default=0x00000000*/

                            /*
                               Read Data. Holds data read from SMBus Read
                               commands. Since TSOD/EEPROM are I2C devices and
                               the byte order is MSByte first in a word read,
                               reading of I2C using word read should return
                               SMB_RDATA[15:8]=I2C_MSB and
                               SMB_RDATA[7:0]=I2C_LSB. If reading of I2C using
                               byte read, the SMB_RDATA[15:8]=don`t care;
                               SMB_RDATA[7:0]=read_byte. If we have a SMB slave
                               connected on the bus, reading of the SMBus slave
                               using word read should return
                               SMB_RDATA[15:8]=SMB_LSB and
                               SMB_RDATA[7:0]=SMB_MSB. If the software is not
                               sure whether the target is I2C or SMBus slave,
                               please use byte access.
                            */
    UINT32 smb_wdata : 16;

                            /* Bits[31:16], Access Type=RW/P, default=None*/

                            /*
                               Write Data: Holds data to be written by SPDW
                               commands. Since TSOD/EEPROM are I2C devices and
                               the byte order is MSByte first in a word write,
                               writing of I2C using word write should use
                               SMB_WDATA[15:8]=I2C_MSB and
                               SMB_WDATA[7:0]=I2C_LSB. If writing of I2C using
                               byte write, the SMB_WDATA[15:8]=don`t care;
                               SMB_WDATA[7:0]=write_byte. If we have a SMB
                               slave connected on the bus, writing of the SMBus
                               slave using word write should use
                               SMB_WDATA[15:8]=SMB_LSB and
                               SMB_WDATA[7:0]=SMB_MSB. It is software
                               responsibility to figure out the byte order of
                               the slave access.
                            */

  } Bits;
  UINT32 Data;

} SMB_DATA_CFG;


typedef union {
  ///
  /// Individual bit fields
  ///
  struct {
    UINT32 smb_clk_prd : 16;

                            /* Bits[15:0], Access Type=RW/P, default=None*/

                            /*
                               This field specifies SMBUS Clock period in
                               number of FXCLK. It is recommend to program an
                               even value in this field since the hardware is
                               simply doing a right shift for the divided by 2
                               operation. The value of this field must be less
                               than or equal to 0x2710 (i.e. 10KHz or faster).
                               Note the 100KHz SMB_CLK_PRD default value is
                               calculated based on 100 MHz FXCLK. Time high and
                               time low of SMBUS clock is defined as: TIME_HIGH
                               = SMB_CLK_PRD/2 - SMB_CLK_OFFSET +
                               SMB_CLK_RISE_OFFSET, TIME_LOW = SMB_CLK_PRD/2 +
                               SMB_CLK_OFFSET - SMB_CLK_RISE_OFFSET
                            */
    UINT32 smb_clk_offset : 8;

                            /* Bits[23:16], Access Type=RW/P, default=None*/

                            /*
                               Signed value in units of FXCLK to adjust the
                               duty cycle of SMBUS clock. When value =0,
                               TIME_HIGH = SMB_CLK_PRD/2 + SMB_CLK_RISE_OFFSET
                               and TIME_LOW = SMB_CLK_PRD/2 -
                               SMB_CLK_RISE_OFFSET. When value = non-zero:
                               TIME_HIGH = SMB_CLK_PRD/2-SMB_CLK_OFFSET+SMB_CLK
                               _RISE_OFFSET, TIME_LOW = SMB_CLK_PRD/2 +
                               SMB_CLK_OFFSET - SMB_CLK_RISE_OFFSET. Must be
                               within -(SMB_CLK_PRD/4-1) to (SMB_CLK_PRD/4-1).
                            */
    UINT32 rsvd : 8;

                            /* Bits[31:24], Access Type=RO, default=None*/

                            /* Reserved */

  } Bits;
  UINT32 Data;

} SMB_PERIOD_CFG;


/**
  Main driver function to get SPD from SMBus

  @param[in]  Channel   Channel Index
  @param[in]  Dimm      Dimm Index
  @param[in]  Count     Number of SPD bytes (DDR4 = 512)
  @param[out] SpdBytes  Pointer to SpdBytes array

  @retval EFI_SUCCESS           Smbus SPD get successfully
  @retval EFI_INVALID_PARAMETER NULL input pointer
  @retval EFI_UNSUPPORTED       If no DIMM present or invalid SPD data
  @retval EFI_ERROR             otherwise
**/
EFI_STATUS
EFIAPI
SpdGetFromSmb (
  IN  UINT8     Channel,
  IN  UINT8     Dimm,
  IN  UINT16    Count,
  OUT UINT8    *SpdBytes
);
