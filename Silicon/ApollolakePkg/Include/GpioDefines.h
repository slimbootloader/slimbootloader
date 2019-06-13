/** @file

  Copyright (c) 2018, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef _GPIO_DEFINES_H_
#define _GPIO_DEFINES_H_

#define GPIO_PADBAR                     0x500

#define GPIO_NORTH_COMMUNITY_LENGTH     0x76C
#define GPIO_NORTHWEST_COMMUNITY_LENGTH 0x764
#define GPIO_WEST_COMMUNITY_LENGTH      0x674
#define GPIO_SOUTHWEST_COMMUNITY_LENGTH 0x654

// PortID offsets
#define GPIO_MMIO_OFFSET_SW     0xC0
#define GPIO_MMIO_OFFSET_S      0xC2
#define GPIO_MMIO_OFFSET_NW     0xC4
#define GPIO_MMIO_OFFSET_N      0xC5
#define GPIO_MMIO_OFFSET_W      0xC7

#define BXT_GPIO_PAD_CONF0_OFFSET   0x0
#define BXT_GPIO_PAD_CONF1_OFFSET   0x4
#define NA                    0xFF
#define LOW           0
#define HIGH          1
#define MASK_WAKE     0
#define UNMASK_WAKE   1

#define ALL_COMMUNITY_GPIO_PIN_NUM   248
#define GPIO_REGS_SIZE                      8

// GPIO pad DW0 MASK
#define PAD_DW0_MASK  0xF961E0FE  // bit 27, 24,  22, 21, 16:13, 7:1 are RO, mask bit [31:28] as they are modified as usual, special operation for them if need.
// GPIO pad DW1 MASK
#define PAD_DW1_MASK  0xFFFC00FF // bit [17:14],[13:10],[9:8]  are R/W

#define PAD_BASE_ADDRESS      0x0C
#define PAD_OWNERSHIP_0       0x20
#define  V_PAD_OWNERSHIP_MASK 0x03
#define  V_PAD_OWNERSHIP_HOST 0x00
#define HOSTSW_OWNERSHIP_0    0x80
#define GPI_IS_0              0x100
#define GPI_IE_0              0x110
#define GPI_GPE_STS_0         0x120
#define GPI_GPE_EN_0          0x130
#define GPI_SMI_STS_0         0x140
#define GPI_SMI_EN_0          0x150
#define EVOUTEN_0             0x210
#define EVMAP_0               0x220

#define EVENT_MUX_SIZE        16 ///< MUX is 16:1 on Broxton
#define WAKE_EVENT_NUM        16 ///< 16 direct IRQ wake events are supported on Broxton in North and Northwest communities.

typedef enum
{
    Native =0xff,
    GPIO = 0,      // Native, no need to set PAD_VALUE
    GPI = 1,       // GPI, input only in PAD_VALUE
    GPO = 2,       // GPO, output only in PAD_VALUE
    HI_Z = 3,
} GPIO_En;

typedef enum
{
    LO = 0,
    HI = 1,
} GPO_D4;


// Mapping to CONF0 bit 27:24
// Note: Assume "Direct Irq En" is not set, unless specially notified

typedef enum
{
    P_NONE  = 0,        // Pull None
    P_5K_L  = 2,        // Pull Down  5K
    P_20K_L = 4,        // Pull Down  20K
    P_1K_H  = 9,        // Pull Up 1K
    P_2K_H  = 11,       // Pull Up 2K
    P_20K_H = 12,       // Pull Up 20K
    P_5K_H  = 10,       // Pull Up  5K
    P_1K2K_H  = 13,     // Pull Up  1K
    Native_control = 15 
} PULL_TYPE;

typedef enum {
    M0 = 0,
    M1,
    M2,
    M3,
    M4,
    M5
} PAD_MODE;

typedef enum
{
  Level =0,
  Edge =1,
  INT_DIS =2,
  BothEdge = 3,
} INT_Type;

typedef enum
{
  PowerGood = 0,
  DeepGPIO =1,
  GPIOReset =2,
  Inactive =3,
} Reset_Cfg;

typedef enum
{
  Maskable,
  NonMaskable,
} mask;

typedef enum
{
  SOUTH     = GPIO_MMIO_OFFSET_S,
  WEST      = GPIO_MMIO_OFFSET_W,
  NORTHWEST = GPIO_MMIO_OFFSET_NW,
  SOUTHWEST = GPIO_MMIO_OFFSET_SW,
  NORTH     = GPIO_MMIO_OFFSET_N
} coms;

typedef enum
{
  IOAPIC =8 ,
  SCI = 4,
  SMI = 2,
  NMI =1,
  NONE = 0,
} GPIO_Route;

/*
InvertRxTx 7:4
0 - No Inversion
1 - Inversion
[0] RX Enable
[1] TX Enable
[2] RX Data
[3] TX Data
*/
typedef enum
{
    No_invert = 0,
    Inverted  = 1,
} InvertRX;

typedef enum
{
  ACPI_D = 0, // ACPI driver
  GPIO_D = 1, // GPIO driver
} HOSTSW;

/*
I/O Standby State
0 = Latch last value driven on TX, TX Enable and RX Enable
1 = Drive 0 with RX disabled and RX drive 0 internally
2 = Drive 0 with RX disabled and RX drive 1 internally
3 = Drive 1 with RX disabled and RX drive 0 internally
4 = Drive 1 with RX disabled and RX drive 1 internally
5 = Drive 0 with RX enabled
6 = Drive 1 with RX enabled
7 = Hi-Z with RX drive 0 internally
8 = Hi-Z with RX drive 1 internally
9 = TX Disabled and RX Enabled (i.e. wake or interrupt)
15 = IO Standby signal is masked for this pad.
*/
typedef enum
{
  Last_Value = 0,
  D0RxDRx0I  = 1,
  D0RxDRx1I  = 2,
  D1RxDRx0I  = 3,
  D1RxDRx1I  = 4,
  D0RxE      = 5,
  D1RxE      = 6,
  HizRx0I    = 7,
  HizRx1I    = 8,
  TxDRxE     = 9,// (i.e. wake or interrupt)
  IOS_Masked = 15,
}IOSSTATE;

/*
I/O Standby Termination
0 = Same as functional pull
1 = Disable Pullup and Pulldown 
2 = Enable Pulldown 
3 = Enable Pullup
*/
typedef enum
{
  SAME    = 0,
  DisPuPd = 1,
  EnPd    = 2,
  EnPu    = 3,
}IOSTERM;

// Wake enable or disable for direct IRQ GPIO pin
typedef enum
{
  Wake_Disabled = FALSE,
  Wake_Enabled = TRUE,
}WAKE_CAPABLE;

typedef union{
    UINT32 padCnf0;
    struct {
      UINT32 GPIOTxState:1; // 0     GPIO TX State
      UINT32 GPIORxState:1; // 1     GPIO RX State, RO
      UINT32 Reserved1:6;   // 2-7   Reserved, RO
      UINT32 GPIORxTxDis:2; // 8-9  GPIO RX Disable[9], GPIO TX Disable[8]
      UINT32 PMode:3;       // 10-12 Pad Mode, 0h = GPIO Controller controls the Pad; 1h = Native Function 1, if applicable, controls the pad; 2h = Native Function 2, if applicable, controls the pad, etc.
      UINT32 Reserved2:4;   // 13-16 Reserved, RO
      UINT32 GPIRout:4;     // 17-20, Route to IOxAPIC[20], SCI[19], SMI[18], NMI[17]
      UINT32 RXTXEnCfg:2;   // 21-22 RX/TX Enable Config (RXTXEnCfg) RO
      UINT32 RXINV:1;       // 23   RX Invert, 0 = No inversion; 1 = Inversion
      UINT32 PreGfRXSel:1;  // 24 Pre Glitch Filter Stage RX Pad State Select,  RO, not support in BXT
      UINT32 RxEvCfg:2;     // 25-26 0h = Level 1h  = Edge (RxInv=0 for rising edge; 1 for falling edge), 2h  = Disabled ,3h  =  Either rising edge or falling edge
      UINT32 Reserved3:1;   // 27, RO
      UINT32 RXRAW1:1;      // 28 oeverride the RX to 1
      UINT32 RXPadStSel:1;  // 29 RX Pad State Select
      UINT32 PadRstCfg:2;   // 30-31 Pad Reset Config
    }r;
} BXT_CONF_PAD0;

typedef union{
    UINT32 padCnf1;
    struct {
         UINT32 IntSel:7;      // 0-6 Interrupt Select. RO
         UINT32 Reserved:1;    // 7  Reserved.         
         UINT32 IOSTerm:2;     // 8-9 I/O Standby Termination (IOSTerm) RW
         UINT32 Term:4;        // 10-13 Termination, 
                               // 0 000: none;0 010: 5k wpd;0 100: 20k wpd;1 000: none;1 001: 1k wpu;1 011: 2k wpu;1 010: 5k wpu;
                               // 1 100: 20k wpu;1 101: 1k & 2k wpu;1 111: (optional) Native controller selected by Pad Mode controls the Termination
         UINT32 IOSState:4;    // 14-17 I/O Standby State, I/O Standby is not implemented in BXT, RW
         UINT32 CFIOPadCfg:14; // 18-31 For BXT, this is done thru Family Register if necessary. RO
     }r;
} BXT_CONF_PAD1;



typedef struct {
        BXT_CONF_PAD0  padConfg0;
        BXT_CONF_PAD1  padConfg1;
        UINT8          Community;
        UINT16         MMIO_ADDRESS;
        BOOLEAN        HostSw;
        BOOLEAN        WakeEnabled; // wake enable for direct IRQ pin
        CHAR16*        pad_name;// GPIO Pin Name for debug purpose
} BXT_GPIO_PAD_INIT;

typedef struct {
        UINT32         CommAndOffset;
        CHAR16*        pad_name;
}BXT_GPIO_PAD_TABLE;

typedef struct {
        UINT32         CommAndOffset;
        BXT_CONF_PAD0  padConfg0;
        BXT_CONF_PAD1  padConfg1;
        UINT8          HostSw;
        BOOLEAN        WakeEnabled;
} BXT_GPIO_CONFIG;

typedef union{
    UINT32 DW;
    struct {
        UINT32         Community:     8;
        UINT32         Offset:        12;
        UINT32         HostSw:        1;
        UINT32         DirectIrqWake: 1;
        UINT32         Reserved0:     10;
    }r;
}GPIO_CONFIG_SMIP_HALF0;

typedef union{
    UINT32 DW;
    struct {
        UINT32         padConfig0:    20; // GPIO Pad DW0's R/W bits: [0], [8:12], [17:26], [28:31]
        UINT32         padConfig1:    10; // GPIO pad DW1's R/W bits:[8:17]
        UINT32         Reserved0:     2;
    }r;
}GPIO_CONFIG_SMIP_HALF1;

typedef struct{
    GPIO_CONFIG_SMIP_HALF0 Half0;
    GPIO_CONFIG_SMIP_HALF1 Half1;
}GPIO_CONFIG_SMIP;

// product all items of BXT_GPIO_PAD_INIT
#define BXT_GPIO_PAD_CONF(pad_name, PMode, GPIO_Config, HostSw, GPO_STATE, INT_Trigger, WakeEnabled, Term_H_L, Inverted, GPI_ROUT, IOSState, IOSTerm, MMIO_Offset, Community ) { \
        {((((INT_Trigger) != NA) ? ((UINT32) INT_Trigger << 25) : (UINT32)INT_DIS << 25 )  | (((Inverted) != NA) ? ((UINT32) Inverted << 23) : (UINT32)No_invert << 23) | (((GPI_ROUT) != NA) ? ((UINT32) GPI_ROUT << 17) : (UINT32)NONE << 17) | ((UINT32) PMode << 10) |  (((GPIO_Config) != NA) ? ((UINT32) GPIO_Config << 8 ): (UINT32)0 << 8) | (((GPO_STATE) == HI ) ? HI : LO)) }, \
        {(((IOSState != NA) ? ((UINT32) IOSState << 14 ) : Last_Value) | (((Term_H_L) != NA) ? ((UINT32) Term_H_L << 10) : P_NONE) | ((IOSTerm != NA) ? ((UINT32) IOSTerm << 8) : SAME))}, \
        Community, \
        (MMIO_Offset != NA) ? (MMIO_Offset) : 0, \
        ((HostSw == NA)||(HostSw == ACPI_D))? ACPI_D : GPIO_D,\
        WakeEnabled,\
        NULL \
        }

#define SMIP_Config0_Convert(SmipPadConfig0) ( \
          (((UINT32)SmipPadConfig0 & (BIT16 | BIT17 | BIT18 | BIT19)) << 12) | \
          (((UINT32)SmipPadConfig0 & (0xFFC0)) << 11) | \
          (((UINT32)SmipPadConfig0 & (BIT1 | BIT2 | BIT3 |BIT4 | BIT5))  << 7) | \
          ((UINT32)SmipPadConfig0 & (BIT0)) \
        )

#define SMIP_Config1_Convert(SmipPadConfig1) ( \
          ((UINT32)SmipPadConfig1 & 0x3FF) << 8 \
        )

#endif