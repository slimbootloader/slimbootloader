/** @file
  Register names for Smbus Device.

  Conventions:

  - Prefixes:
    Definitions beginning with "R_" are registers
    Definitions beginning with "B_" are bits within registers
    Definitions beginning with "V_" are meaningful values of bits within the registers
    Definitions beginning with "S_" are register sizes
    Definitions beginning with "N_" are the bit position
  - In general, SC registers are denoted by "_SC_" in register names
  - Registers / bits that are different between SC generations are denoted by
    "_SC_<generation_name>_" in register/bit names.
  - Registers / bits that are different between SKUs are denoted by "_<SKU_name>"
    at the end of the register/bit names
  - Registers / bits of new devices introduced in a SC generation will be just named
    as "_SC_" without <generation_name> inserted.

  Copyright (c) 1999 - 2016, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#ifndef _REGS_SMBUS_H_
#define _REGS_SMBUS_H_

///
/// SMBus Controller Registers (D31:F3)
///
#define PCI_DEVICE_NUMBER_SMBUS        31
#define PCI_FUNCTION_NUMBER_SMBUS      1

#define R_SMBUS_VENDOR_ID              0x00  ///< Vendor ID
#define V_SMBUS_VENDOR_ID              V_INTEL_VENDOR_ID ///< Intel Vendor ID

#define R_SMBUS_DEVICE_ID              0x02  ///< Device ID
#define V_SMBUS_DEVICE_ID              0x5AD4

#define R_SMBUS_PCICMD                 0x04  ///< CMD register enables/disables, Memory/IO space access and interrupt
#define B_SMBUS_PCICMD_INTR_DIS        BIT10 ///< Interrupt Disable
#define B_SMBUS_PCICMD_FBE             BIT9  ///< FBE - reserved as '0'
#define B_SMBUS_PCICMD_SERR_EN         BIT8  ///< SERR Enable - reserved as '0'
#define B_SMBUS_PCICMD_WCC             BIT7  ///< Wait Cycle Control - reserved as '0'
#define B_SMBUS_PCICMD_PER             BIT6  ///< Parity Error - reserved as '0'
#define B_SMBUS_PCICMD_VPS             BIT5  ///< VGA Palette Snoop - reserved as '0'
#define B_SMBUS_PCICMD_PMWE            BIT4  ///< Postable Memory Write Enable - reserved as '0'
#define B_SMBUS_PCICMD_SCE             BIT3  ///< Special Cycle Enable - reserved as '0'
#define B_SMBUS_PCICMD_BME             BIT2  ///< Bus Master Enable - reserved as '0'
#define B_SMBUS_PCICMD_MSE             BIT1  ///< Memory Space Enable
#define B_SMBUS_PCICMD_IOSE            BIT0  ///< I/O Space Enable

#define R_SMBUS_PCISTS                 0x06  ///< Configuration status register
#define B_SMBUS_PCISTS_DPE             BIT15 ///< Detect Parity Error - reserved as '0'
#define B_SMBUS_PCISTS_SSE             BIT14 ///< Signaled System Error - reserved as '0'
#define B_SMBUS_PCISTS_RMA             BIT13 ///< Received Master Abort - reserved as '0'
#define B_SMBUS_PCISTS_RTA             BIT12 ///< Received Target Abort - reserved as '0'
#define B_SMBUS_PCISTS_STA             BIT11 ///< Signaled Target Abort - reserved as '0'
#define B_SMBUS_PCISTS_DEVT            (BIT10 | BIT9) ///< Devsel Timing Status
#define B_SMBUS_PCISTS_DPED            BIT8  ///< Data Parity Error Detected - reserved as '0'
#define B_SMBUS_PCISTS_FB2BC           BIT7  ///< Fast Back To Back Capable - reserved as '1'
#define B_SMBUS_PCISTS_UDF             BIT6  ///< User Defined Features - reserved as '0'
#define B_SMBUS_PCISTS_66MHZ_CAP       BIT5  ///< 66 MHz Capable - reserved as '0'
#define B_SMBUS_PCISTS_CAP_LIST        BIT4  ///< Capabilities List Indicator - reserved as '0'
#define B_SMBUS_PCISTS_INTS            BIT3  ///< Interrupt Status

#define R_SMBUS_RID                    0x08  ///< Revision ID
#define B_SMBUS_RID                    0xFF  ///< Revision ID

#define R_SMBUS_PRGIF                  0x09  ///< Programming Interface
#define B_SMBUS_PRGIF                  0xFF  ///< Programming Interface

#define R_SMBUS_SCC                    0x0A  ///< Sub Class Code
#define V_SMBUS_SCC                    0x05  ///< A value of 05h indicates that this device is a SM Bus serial controller

#define R_SMBUS_BCC                    0x0B  ///< Base Class Code
#define V_SMBUS_BCC                    0x0C  ///< A value of 0Ch indicates that this device is a serial controller

#define R_SMBUS_BAR0                   0x10  ///< The memory bar low
#define B_SMBUS_BAR0_BAR               0xFFFFFFE0 ///< Base Address
#define B_SMBUS_BAR0_PREF              BIT3  ///< Hardwired to 0. Indicated that SMBMBAR is not prefetchable
#define B_SMBUS_BAR0_ADDRNG            (BIT2 | BIT1)
#define B_SMBUS_BAR0_MSI               BIT0  ///< Memory Space Indicator

#define R_SMBUS_BAR1                   0x14  ///< The memory bar high
#define B_SMBUS_BAR1_BAR               0xFFFFFFFF ///< Base Address

#define R_SMBUS_BASE                   0x20  ///< The I/O memory bar
#define B_SMBUS_BASE_BAR               0x0000FFE0 ///< Base Address
#define B_SMBUS_BASE_IOSI              BIT0  ///< IO Space Indicator

#define R_SMBUS_SVID                   0x2C  ///< Subsystem Vendor ID
#define B_SMBUS_SVID                   0xFFFF ///< Subsystem Vendor ID

#define R_SMBUS_SID                    0x2E  ///< Subsystem ID
#define B_SMBUS_SID                    0xFFFF ///< Subsystem ID

#define R_SMBUS_INT_LN                 0x3C  ///< Interrupt Line
#define B_SMBUS_INT_LN                 0xFF  ///< Interrupt Line

#define R_SMBUS_INT_PN                 0x3D  ///< Interrupt Pin
#define B_SMBUS_INT_PN                 0xFF  ///< Interrupt Pin

#define R_PCH_SMBUS_HSTS               0x00  // Host Status Register R/W
#define B_PCH_SMBUS_HBSY               0x01
#define R_PCH_SMBUS_HCTL               0x02  // Host Control Register R/W
#define B_PCH_SMBUS_START              BIT6  // Start
#define B_PCH_SMBUS_DERR               0x04
#define B_PCH_SMBUS_BERR               0x08
#define B_PCH_SMBUS_IUS                0x40
#define B_PCH_SMBUS_BYTE_DONE_STS      0x80
#define B_PCH_SMBUS_HSTS_ALL           0xFF
#define V_PCH_SMBUS_SMB_CMD_BYTE_DATA  0x08  // Byte Data
#define V_PCH_SMBUS_SMB_CMD_BLOCK      0x14  // Block

#define R_SMBUS_HOSTC                  0x40  ///< Host Configuration Register
#define B_SMBUS_HOSTC_SPD_WD           BIT4  ///< SPD Write Disable
#define B_SMBUS_HOSTC_SSRESET          BIT3  ///< Soft SMBus Reset
#define B_SMBUS_HOSTC_I2C_EN           BIT2  ///< I2C Enable Bit
#define B_SMBUS_HOSTC_SMI_EN           BIT1  ///< SMI Enable Bit
#define B_SMBUS_HOSTC_HST_EN           BIT0  ///< Host Controller Enable Bit

#define R_SMBUS_TCOBASE                0x50 ///< TCO Base Address
#define B_SMBUS_TCOBASE_BAR            0x0000FFE0

#define R_SMBUS_TCOCTL                 0x54 ///< TCO Control
#define B_SMBUS_TCOCTL_TCO_BASE_EN     BIT8  ///< TCO Base Enable
#define R_SMBUS_TCOCTL_TCO_BASE_LOCK   BIT0  ///< TCO Base Lock

#define R_SMBUS_MANID                  0xF8  ///< Manufacturer's ID Register
#define B_SMBUS_MANID_DOTID            0x0F000000 ///< DOT ID
#define B_SMBUS_MANID_SID              0x00FF0000 ///< Stepping ID
#define B_SMBUS_MANID_MID              0x0000FF00 ///< Manufacturer ID
#define B_SMBUS_MANID_PPID             0x000000FF ///< Process ID

///
/// SMBus I/O Registers
///
#define R_SMBUS_HSTS                   0x00  ///< Host Status Register R/W
#define B_SMBUS_HSTS_ALL               0xFF
#define B_SMBUS_BYTE_DONE_STS          BIT7  ///< Byte Done Status
#define B_SMBUS_IUS                    BIT6  ///< In Use Status
#define B_SMBUS_SMBALERT_STS           BIT5  ///< SMBUS Alert
#define B_SMBUS_FAIL                   BIT4  ///< Failed
#define B_SMBUS_BERR                   BIT3  ///< Bus Error
#define B_SMBUS_DERR                   BIT2  ///< Device Error
#define B_SMBUS_ERRORS                 (B_SMBUS_FAIL | B_SMBUS_BERR | B_SMBUS_DERR)
#define B_SMBUS_INTR                   BIT1  ///< Interrupt
#define B_SMBUS_HBSY                   BIT0  ///< Host Busy

#define R_SMBUS_HCTL                   0x02  ///< Host Control Register R/W
#define B_SMBUS_PEC_EN                 BIT7  ///< Packet Error Checking Enable
#define B_SMBUS_START                  BIT6  ///< Start
#define B_SMBUS_LAST_BYTE              BIT5  ///< Last Byte
#define B_SMBUS_SMB_CMD                0x1C  ///< SMB Command
#define V_SMBUS_SMB_CMD_BLOCK_PROCESS  0x1C  ///< Block Process
#define V_SMBUS_SMB_CMD_IIC_READ       0x18  ///< I2C Read
#define V_SMBUS_SMB_CMD_BLOCK          0x14  ///< Block
#define V_SMBUS_SMB_CMD_PROCESS_CALL   0x10  ///< Process Call
#define V_SMBUS_SMB_CMD_WORD_DATA      0x0C  ///< Word Data
#define V_SMBUS_SMB_CMD_BYTE_DATA      0x08  ///< Byte Data
#define V_SMBUS_SMB_CMD_BYTE           0x04  ///< Byte
#define V_SMBUS_SMB_CMD_QUICK          0x00  ///< Quick
#define B_SMBUS_KILL                   BIT1  ///< Kill
#define B_SMBUS_INTREN                 BIT0  ///< Interrupt Enable

#define R_SMBUS_HCMD                   0x03  ///< Host Command Register R/W
#define B_SMBUS_HCMD                   0xFF  ///< Command to be transmitted

#define R_SMBUS_TSA                    0x04  ///< Transmit Slave Address Register R/W
#define B_SMBUS_ADDRESS                0xFE  ///< 7-bit address of the targeted slave
#define B_SMBUS_RW_SEL                 BIT0  ///< Direction of the host transfer, 1 = read, 0 = write
#define B_SMBUS_RW_SEL_READ            0x01  ///< Read
#define B_SMBUS_RW_SEL_WRITE           0x00  ///< Write

#define R_SMBUS_HD0                    0x05  ///< Data 0 Register R/W
#define R_SMBUS_HD1                    0x06  ///< Data 1 Register R/W
#define R_SMBUS_HBD                    0x07  ///< Host Block Data Register R/W
#define R_SMBUS_PEC                    0x08  ///< Packet Error Check Data Register R/W

#define R_SMBUS_RSA                    0x09  ///< Receive Slave Address Register R/W
#define B_SMBUS_SLAVE_ADDR             0x7F  ///< TCO slave address (Not used, reserved)

#define R_SMBUS_SD                     0x0A  ///< Receive Slave Data Register R/W

#define R_SMBUS_AUXS                   0x0C  ///< Auxiliary Status Register R/WC
#define B_SMBUS_CRCE                   BIT0  ///< CRC Error

#define R_SMBUS_AUXC                   0x0D  ///< Auxiliary Control Register R/W
#define B_SMBUS_E32B                   BIT1  ///< Enable 32-byte Buffer
#define B_SMBUS_AAC                    BIT0  ///< Automatically Append CRC

#define R_SMBUS_SMLC                   0x0E  ///< SMLINK Pin Control Register R/W
#define B_SMBUS_SMLINK_CLK_CTL         BIT2  ///< Not supported
#define B_SMBUS_SMLINK1_CUR_STS        BIT1  ///< Not supported
#define B_SMBUS_SMLINK0_CUR_STS        BIT0  ///< Not supported

#define R_SMBUS_SMBC                   0x0F  ///< SMBus Pin Control Register R/W
#define B_SMBUS_SMBCLK_CTL             BIT2  ///< SMBCLK Control
#define B_SMBUS_SMBDATA_CUR_STS        BIT1  ///< SMBDATA Current Status
#define B_SMBUS_SMBCLK_CUR_STS         BIT0  ///< SMBCLK Current Status

#define R_SMBUS_SSTS                   0x10  ///< Slave Status Register R/WC
#define B_SMBUS_HOST_NOTIFY_STS        BIT0  ///< Host Notify Status

#define R_SMBUS_SCMD                   0x11  ///< Slave Command Register R/W
#define B_SMBUS_SMBALERT_DIS           BIT2  ///< Not supported
#define B_SMBUS_HOST_NOTIFY_WKEN       BIT1  ///< Host Notify Wake Enable
#define B_SMBUS_HOST_NOTIFY_INTREN     BIT0  ///< Host Notify Interrupt Enable

#define R_SMBUS_NDA                    0x14  ///< Notify Device Address Register RO
#define B_SMBUS_DEVICE_ADDRESS         0xFE  ///< Device Address

#define R_SMBUS_NDLB                   0x16  ///< Notify Data Low Byte Register RO
#define R_SMBUS_NDHB                   0x17  ///< Notify Data High Byte Register RO

#endif

