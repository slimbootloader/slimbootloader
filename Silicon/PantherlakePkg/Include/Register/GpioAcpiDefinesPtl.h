/** @file
  GPIO ASL header for PTL_PCD.

  Copyright (c) 2026, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef _GPIOV2_ACPI_DEFINES_PTL_PCD_H_
#define _GPIOV2_ACPI_DEFINES_PTL_PCD_H_

#define GPIO_HID_PTL_PCD_H "INTC10BB" // PTL PCD-H GPIO
#define GPIO_HID_PTL_PCD_P "INTC10BC" // PTL PCD-P GPIO

//
// PCH GPIO Community address
//
#define PTL_PCD_GPIOV2_COM0                               0x590000 // PID: 0xF259
#define PTL_PCD_GPIOV2_COM1                               0x5A0000 // PID: 0xF25A
#define PTL_PCD_GPIOV2_COM3                               0x5B0000 // PID: 0xF25B
#define PTL_PCD_GPIOV2_COM4                               0x5C0000 // PID: 0xF25C
#define PTL_PCD_GPIOV2_COM5                               0x5D0000 // PID: 0xF25D



//
// GPP_V
//
#define GPIOV2_PTL_PCD_DRIVER_xxgpp_v_0                   0    // GPP_V xxgpp_v_0
#define GPIOV2_PTL_PCD_DRIVER_xxgpp_v_1                   1    // GPP_V xxgpp_v_1
#define GPIOV2_PTL_PCD_DRIVER_xxgpp_v_2                   2    // GPP_V xxgpp_v_2
#define GPIOV2_PTL_PCD_DRIVER_xxgpp_v_3                   3    // GPP_V xxgpp_v_3
#define GPIOV2_PTL_PCD_DRIVER_xxgpp_v_4                   4    // GPP_V xxgpp_v_4
#define GPIOV2_PTL_PCD_DRIVER_xxgpp_v_5                   5    // GPP_V xxgpp_v_5
#define GPIOV2_PTL_PCD_DRIVER_xxgpp_v_6                   6    // GPP_V xxgpp_v_6
#define GPIOV2_PTL_PCD_DRIVER_xxgpp_v_7                   7    // GPP_V xxgpp_v_7
#define GPIOV2_PTL_PCD_DRIVER_xxgpp_v_8                   8    // GPP_V xxgpp_v_8
#define GPIOV2_PTL_PCD_DRIVER_xxgpp_v_9                   9    // GPP_V xxgpp_v_9
#define GPIOV2_PTL_PCD_DRIVER_xxgpp_v_10                  10   // GPP_V xxgpp_v_10
#define GPIOV2_PTL_PCD_DRIVER_xxgpp_v_11                  11   // GPP_V xxgpp_v_11
#define GPIOV2_PTL_PCD_DRIVER_xxgpp_v_12                  12   // GPP_V xxgpp_v_12
#define GPIOV2_PTL_PCD_DRIVER_xxgpp_v_13                  13   // GPP_V xxgpp_v_13
#define GPIOV2_PTL_PCD_DRIVER_xxgpp_v_14                  14   // GPP_V xxgpp_v_14
#define GPIOV2_PTL_PCD_DRIVER_xxgpp_v_15                  15   // GPP_V xxgpp_v_15
#define GPIOV2_PTL_PCD_DRIVER_xxgpp_v_16                  16   // GPP_V xxgpp_v_16
#define GPIOV2_PTL_PCD_DRIVER_xxgpp_v_17                  17   // GPP_V xxgpp_v_17
#define GPIOV2_PTL_PCD_DRIVER_xxsys_pwrok                 18   // GPP_V xxsys_pwrok
#define GPIOV2_PTL_PCD_DRIVER_xxsys_reset_b               19   // GPP_V xxsys_reset_b
#define GPIOV2_PTL_PCD_DRIVER_xxl_bklten                  20   // GPP_V xxl_bklten
#define GPIOV2_PTL_PCD_DRIVER_xxl_bkltctl                 21   // GPP_V xxl_bkltctl
#define GPIOV2_PTL_PCD_DRIVER_xxl_vdden                   22   // GPP_V xxl_vdden
#define GPIOV2_PTL_PCD_DRIVER_xxmlk_rst_b                 23   // GPP_V xxmlk_rst_b

//
// GPP_C
//
#define GPIOV2_PTL_PCD_DRIVER_xxgpp_c_0                   32   // GPP_C xxgpp_c_0
#define GPIOV2_PTL_PCD_DRIVER_xxgpp_c_1                   33   // GPP_C xxgpp_c_1
#define GPIOV2_PTL_PCD_DRIVER_xxgpp_c_2                   34   // GPP_C xxgpp_c_2
#define GPIOV2_PTL_PCD_DRIVER_xxgpp_c_3                   35   // GPP_C xxgpp_c_3
#define GPIOV2_PTL_PCD_DRIVER_xxgpp_c_4                   36   // GPP_C xxgpp_c_4
#define GPIOV2_PTL_PCD_DRIVER_xxgpp_c_5                   37   // GPP_C xxgpp_c_5
#define GPIOV2_PTL_PCD_DRIVER_xxgpp_c_6                   38   // GPP_C xxgpp_c_6
#define GPIOV2_PTL_PCD_DRIVER_xxgpp_c_7                   39   // GPP_C xxgpp_c_7
#define GPIOV2_PTL_PCD_DRIVER_xxgpp_c_8                   40   // GPP_C xxgpp_c_8
#define GPIOV2_PTL_PCD_DRIVER_xxgpp_c_9                   41   // GPP_C xxgpp_c_9
#define GPIOV2_PTL_PCD_DRIVER_xxgpp_c_10                  42   // GPP_C xxgpp_c_10
#define GPIOV2_PTL_PCD_DRIVER_xxgpp_c_11                  43   // GPP_C xxgpp_c_11
#define GPIOV2_PTL_PCD_DRIVER_xxgpp_c_12                  44   // GPP_C xxgpp_c_12
#define GPIOV2_PTL_PCD_DRIVER_xxgpp_c_13                  45   // GPP_C xxgpp_c_13
#define GPIOV2_PTL_PCD_DRIVER_xxgpp_c_14                  46   // GPP_C xxgpp_c_14
#define GPIOV2_PTL_PCD_DRIVER_xxgpp_c_15                  47   // GPP_C xxgpp_c_15
#define GPIOV2_PTL_PCD_DRIVER_xxgpp_c_16                  48   // GPP_C xxgpp_c_16
#define GPIOV2_PTL_PCD_DRIVER_xxgpp_c_17                  49   // GPP_C xxgpp_c_17
#define GPIOV2_PTL_PCD_DRIVER_xxgpp_c_18                  50   // GPP_C xxgpp_c_18
#define GPIOV2_PTL_PCD_DRIVER_xxgpp_c_19                  51   // GPP_C xxgpp_c_19
#define GPIOV2_PTL_PCD_DRIVER_xxgpp_c_20                  52   // GPP_C xxgpp_c_20
#define GPIOV2_PTL_PCD_DRIVER_xxgpp_c_21                  53   // GPP_C xxgpp_c_21
#define GPIOV2_PTL_PCD_DRIVER_xxgpp_c_22                  54   // GPP_C xxgpp_c_22
#define GPIOV2_PTL_PCD_DRIVER_xxgpp_c_23                  55   // GPP_C xxgpp_c_23

//
// GPP_F
//
#define GPIOV2_PTL_PCD_DRIVER_xxgpp_f_0                   64   // GPP_F xxgpp_f_0
#define GPIOV2_PTL_PCD_DRIVER_xxgpp_f_1                   65   // GPP_F xxgpp_f_1
#define GPIOV2_PTL_PCD_DRIVER_xxgpp_f_2                   66   // GPP_F xxgpp_f_2
#define GPIOV2_PTL_PCD_DRIVER_xxgpp_f_3                   67   // GPP_F xxgpp_f_3
#define GPIOV2_PTL_PCD_DRIVER_xxgpp_f_4                   68   // GPP_F xxgpp_f_4
#define GPIOV2_PTL_PCD_DRIVER_xxgpp_f_5                   69   // GPP_F xxgpp_f_5
#define GPIOV2_PTL_PCD_DRIVER_xxgpp_f_6                   70   // GPP_F xxgpp_f_6
#define GPIOV2_PTL_PCD_DRIVER_xxgpp_f_7                   71   // GPP_F xxgpp_f_7
#define GPIOV2_PTL_PCD_DRIVER_xxgpp_f_8                   72   // GPP_F xxgpp_f_8
#define GPIOV2_PTL_PCD_DRIVER_xxgpp_f_9                   73   // GPP_F xxgpp_f_9
#define GPIOV2_PTL_PCD_DRIVER_xxgpp_f_10                  74   // GPP_F xxgpp_f_10
#define GPIOV2_PTL_PCD_DRIVER_xxgpp_f_11                  75   // GPP_F xxgpp_f_11
#define GPIOV2_PTL_PCD_DRIVER_xxgpp_f_12                  76   // GPP_F xxgpp_f_12
#define GPIOV2_PTL_PCD_DRIVER_xxgpp_f_13                  77   // GPP_F xxgpp_f_13
#define GPIOV2_PTL_PCD_DRIVER_xxgpp_f_14                  78   // GPP_F xxgpp_f_14
#define GPIOV2_PTL_PCD_DRIVER_xxgpp_f_15                  79   // GPP_F xxgpp_f_15
#define GPIOV2_PTL_PCD_DRIVER_xxgpp_f_16                  80   // GPP_F xxgpp_f_16
#define GPIOV2_PTL_PCD_DRIVER_xxgpp_f_17                  81   // GPP_F xxgpp_f_17
#define GPIOV2_PTL_PCD_DRIVER_xxgpp_f_18                  82   // GPP_F xxgpp_f_18
#define GPIOV2_PTL_PCD_DRIVER_xxgpp_f_19                  83   // GPP_F xxgpp_f_19
#define GPIOV2_PTL_PCD_DRIVER_xxgpp_f_20                  84   // GPP_F xxgpp_f_20
#define GPIOV2_PTL_PCD_DRIVER_xxgpp_f_21                  85   // GPP_F xxgpp_f_21
#define GPIOV2_PTL_PCD_DRIVER_xxgpp_f_22                  86   // GPP_F xxgpp_f_22
#define GPIOV2_PTL_PCD_DRIVER_xxgpp_f_23                  87   // GPP_F xxgpp_f_23
#define GPIOV2_PTL_PCD_DRIVER_thc1_gspi1_i3c2_clk_loopbk  88   // GPP_F thc1_gspi1_i3c2_clk_loopbk
#define GPIOV2_PTL_PCD_DRIVER_a_gspi0_clk_loopbk          89   // GPP_F a_gspi0_clk_loopbk

//
// GPP_E
//
#define GPIOV2_PTL_PCD_DRIVER_xxgpp_e_0                   96   // GPP_E xxgpp_e_0
#define GPIOV2_PTL_PCD_DRIVER_xxgpp_e_1                   97   // GPP_E xxgpp_e_1
#define GPIOV2_PTL_PCD_DRIVER_xxgpp_e_2                   98   // GPP_E xxgpp_e_2
#define GPIOV2_PTL_PCD_DRIVER_xxgpp_e_3                   99   // GPP_E xxgpp_e_3
#define GPIOV2_PTL_PCD_DRIVER_xxgpp_e_4                   100  // GPP_E xxgpp_e_4
#define GPIOV2_PTL_PCD_DRIVER_xxgpp_e_5                   101  // GPP_E xxgpp_e_5
#define GPIOV2_PTL_PCD_DRIVER_xxgpp_e_6                   102  // GPP_E xxgpp_e_6
#define GPIOV2_PTL_PCD_DRIVER_xxgpp_e_7                   103  // GPP_E xxgpp_e_7
#define GPIOV2_PTL_PCD_DRIVER_xxgpp_e_8                   104  // GPP_E xxgpp_e_8
#define GPIOV2_PTL_PCD_DRIVER_xxgpp_e_9                   105  // GPP_E xxgpp_e_9
#define GPIOV2_PTL_PCD_DRIVER_xxgpp_e_10                  106  // GPP_E xxgpp_e_10
#define GPIOV2_PTL_PCD_DRIVER_xxgpp_e_11                  107  // GPP_E xxgpp_e_11
#define GPIOV2_PTL_PCD_DRIVER_xxgpp_e_12                  108  // GPP_E xxgpp_e_12
#define GPIOV2_PTL_PCD_DRIVER_xxgpp_e_13                  109  // GPP_E xxgpp_e_13
#define GPIOV2_PTL_PCD_DRIVER_xxgpp_e_14                  110  // GPP_E xxgpp_e_14
#define GPIOV2_PTL_PCD_DRIVER_xxgpp_e_15                  111  // GPP_E xxgpp_e_15
#define GPIOV2_PTL_PCD_DRIVER_xxgpp_e_16                  112  // GPP_E xxgpp_e_16
#define GPIOV2_PTL_PCD_DRIVER_xxgpp_e_17                  113  // GPP_E xxgpp_e_17
#define GPIOV2_PTL_PCD_DRIVER_xxgpp_e_18                  114  // GPP_E xxgpp_e_18
#define GPIOV2_PTL_PCD_DRIVER_xxgpp_e_19                  115  // GPP_E xxgpp_e_19
#define GPIOV2_PTL_PCD_DRIVER_xxgpp_e_20                  116  // GPP_E xxgpp_e_20
#define GPIOV2_PTL_PCD_DRIVER_xxgpp_e_21                  117  // GPP_E xxgpp_e_21
#define GPIOV2_PTL_PCD_DRIVER_xxgpp_e_22                  118  // GPP_E xxgpp_e_22
#define GPIOV2_PTL_PCD_DRIVER_xxboothalt_b                119  // GPP_E xxboothalt_b
#define GPIOV2_PTL_PCD_DRIVER_thc0_gspi0_clk_loopbk       120  // GPP_E thc0_gspi0_clk_loopbk

//
// CPUJTAG
//
#define GPIOV2_PTL_PCD_DRIVER_xxepd_on                    128  // CPUJTAG xxepd_on
#define GPIOV2_PTL_PCD_DRIVER_xxvdd2_pwrgd                129  // CPUJTAG xxvdd2_pwrgd
#define GPIOV2_PTL_PCD_DRIVER_xxjtag_mbpb0                130  // CPUJTAG xxjtag_mbpb0
#define GPIOV2_PTL_PCD_DRIVER_xxjtag_mbpb1                131  // CPUJTAG xxjtag_mbpb1
#define GPIOV2_PTL_PCD_DRIVER_xxjtag_mbpb2                132  // CPUJTAG xxjtag_mbpb2
#define GPIOV2_PTL_PCD_DRIVER_xxjtag_mbpb3                133  // CPUJTAG xxjtag_mbpb3
#define GPIOV2_PTL_PCD_DRIVER_xxjtag_tdo                  134  // CPUJTAG xxjtag_tdo
#define GPIOV2_PTL_PCD_DRIVER_xxprdy_b                    135  // CPUJTAG xxprdy_b
#define GPIOV2_PTL_PCD_DRIVER_xxpreq_b                    136  // CPUJTAG xxpreq_b
#define GPIOV2_PTL_PCD_DRIVER_xxjtag_tdi                  137  // CPUJTAG xxjtag_tdi
#define GPIOV2_PTL_PCD_DRIVER_xxjtag_tms                  138  // CPUJTAG xxjtag_tms
#define GPIOV2_PTL_PCD_DRIVER_xxjtag_tck                  139  // CPUJTAG xxjtag_tck
#define GPIOV2_PTL_PCD_DRIVER_xxdbg_pmode                 140  // CPUJTAG xxdbg_pmode
#define GPIOV2_PTL_PCD_DRIVER_xxjtag_trst_b               141  // CPUJTAG xxjtag_trst_b
#define GPIOV2_PTL_PCD_DRIVER_xxddsp_hpdalv               142  // CPUJTAG xxddsp_hpdalv

//
// GPP_H
//
#define GPIOV2_PTL_PCD_DRIVER_xxgpp_h_0                   160  // GPP_H xxgpp_h_0
#define GPIOV2_PTL_PCD_DRIVER_xxgpp_h_1                   161  // GPP_H xxgpp_h_1
#define GPIOV2_PTL_PCD_DRIVER_xxgpp_h_2                   162  // GPP_H xxgpp_h_2
#define GPIOV2_PTL_PCD_DRIVER_xxgpp_h_3                   163  // GPP_H xxgpp_h_3
#define GPIOV2_PTL_PCD_DRIVER_xxgpp_h_4                   164  // GPP_H xxgpp_h_4
#define GPIOV2_PTL_PCD_DRIVER_xxgpp_h_5                   165  // GPP_H xxgpp_h_5
#define GPIOV2_PTL_PCD_DRIVER_xxgpp_h_6                   166  // GPP_H xxgpp_h_6
#define GPIOV2_PTL_PCD_DRIVER_xxgpp_h_7                   167  // GPP_H xxgpp_h_7
#define GPIOV2_PTL_PCD_DRIVER_xxgpp_h_8                   168  // GPP_H xxgpp_h_8
#define GPIOV2_PTL_PCD_DRIVER_xxgpp_h_9                   169  // GPP_H xxgpp_h_9
#define GPIOV2_PTL_PCD_DRIVER_xxgpp_h_10                  170  // GPP_H xxgpp_h_10
#define GPIOV2_PTL_PCD_DRIVER_xxgpp_h_11                  171  // GPP_H xxgpp_h_11
#define GPIOV2_PTL_PCD_DRIVER_xxgpp_h_12                  172  // GPP_H xxgpp_h_12
#define GPIOV2_PTL_PCD_DRIVER_xxgpp_h_13                  173  // GPP_H xxgpp_h_13
#define GPIOV2_PTL_PCD_DRIVER_xxgpp_h_14                  174  // GPP_H xxgpp_h_14
#define GPIOV2_PTL_PCD_DRIVER_xxgpp_h_15                  175  // GPP_H xxgpp_h_15
#define GPIOV2_PTL_PCD_DRIVER_xxgpp_h_16                  176  // GPP_H xxgpp_h_16
#define GPIOV2_PTL_PCD_DRIVER_xxgpp_h_17                  177  // GPP_H xxgpp_h_17
#define GPIOV2_PTL_PCD_DRIVER_xxgpp_h_18                  178  // GPP_H xxgpp_h_18
#define GPIOV2_PTL_PCD_DRIVER_xxgpp_h_19                  179  // GPP_H xxgpp_h_19
#define GPIOV2_PTL_PCD_DRIVER_xxgpp_h_20                  180  // GPP_H xxgpp_h_20
#define GPIOV2_PTL_PCD_DRIVER_xxgpp_h_21                  181  // GPP_H xxgpp_h_21
#define GPIOV2_PTL_PCD_DRIVER_xxgpp_h_22                  182  // GPP_H xxgpp_h_22
#define GPIOV2_PTL_PCD_DRIVER_xxgpp_h_23                  183  // GPP_H xxgpp_h_23
#define GPIOV2_PTL_PCD_DRIVER_xxgpp_h_24                  184  // GPP_H xxgpp_h_24
#define GPIOV2_PTL_PCD_DRIVER_lpi3c1_clk_loopbk           185  // GPP_H lpi3c1_clk_loopbk
#define GPIOV2_PTL_PCD_DRIVER_lpi3c0_clk_loopbk           186  // GPP_H lpi3c0_clk_loopbk
#define GPIOV2_PTL_PCD_DRIVER_ishi3c1_clk_loopbk          187  // GPP_H ishi3c1_clk_loopbk

//
// GPPASPI0
//
#define GPIOV2_PTL_PCD_DRIVER_xxgpp_a_0                   192  // GPPASPI0 xxgpp_a_0
#define GPIOV2_PTL_PCD_DRIVER_xxgpp_a_1                   193  // GPPASPI0 xxgpp_a_1
#define GPIOV2_PTL_PCD_DRIVER_xxgpp_a_2                   194  // GPPASPI0 xxgpp_a_2
#define GPIOV2_PTL_PCD_DRIVER_xxgpp_a_3                   195  // GPPASPI0 xxgpp_a_3
#define GPIOV2_PTL_PCD_DRIVER_xxgpp_a_4                   196  // GPPASPI0 xxgpp_a_4
#define GPIOV2_PTL_PCD_DRIVER_xxgpp_a_5                   197  // GPPASPI0 xxgpp_a_5
#define GPIOV2_PTL_PCD_DRIVER_xxgpp_a_6                   198  // GPPASPI0 xxgpp_a_6
#define GPIOV2_PTL_PCD_DRIVER_xxgpp_a_7                   199  // GPPASPI0 xxgpp_a_7
#define GPIOV2_PTL_PCD_DRIVER_xxgpp_a_8                   200  // GPPASPI0 xxgpp_a_8
#define GPIOV2_PTL_PCD_DRIVER_xxgpp_a_9                   201  // GPPASPI0 xxgpp_a_9
#define GPIOV2_PTL_PCD_DRIVER_xxgpp_a_10                  202  // GPPASPI0 xxgpp_a_10
#define GPIOV2_PTL_PCD_DRIVER_xxgpp_a_11                  203  // GPPASPI0 xxgpp_a_11
#define GPIOV2_PTL_PCD_DRIVER_xxgpp_a_12                  204  // GPPASPI0 xxgpp_a_12
#define GPIOV2_PTL_PCD_DRIVER_xxgpp_a_13                  205  // GPPASPI0 xxgpp_a_13
#define GPIOV2_PTL_PCD_DRIVER_xxgpp_a_14                  206  // GPPASPI0 xxgpp_a_14
#define GPIOV2_PTL_PCD_DRIVER_xxgpp_a_15                  207  // GPPASPI0 xxgpp_a_15
#define GPIOV2_PTL_PCD_DRIVER_xxgpp_a_16                  208  // GPPASPI0 xxgpp_a_16
#define GPIOV2_PTL_PCD_DRIVER_xxgpp_a_17                  209  // GPPASPI0 xxgpp_a_17
#define GPIOV2_PTL_PCD_DRIVER_xxspi0_io_2                 210  // GPPASPI0 xxspi0_io_2
#define GPIOV2_PTL_PCD_DRIVER_xxspi0_io_3                 211  // GPPASPI0 xxspi0_io_3
#define GPIOV2_PTL_PCD_DRIVER_xxspi0_mosi_io_0            212  // GPPASPI0 xxspi0_mosi_io_0
#define GPIOV2_PTL_PCD_DRIVER_xxspi0_miso_io_1            213  // GPPASPI0 xxspi0_miso_io_1
#define GPIOV2_PTL_PCD_DRIVER_xxspi0_tpm_cs_b             214  // GPPASPI0 xxspi0_tpm_cs_b
#define GPIOV2_PTL_PCD_DRIVER_xxspi0_flash_0_cs_b         215  // GPPASPI0 xxspi0_flash_0_cs_b
#define GPIOV2_PTL_PCD_DRIVER_xxspi0_flash_1_cs_b         216  // GPPASPI0 xxspi0_flash_1_cs_b
#define GPIOV2_PTL_PCD_DRIVER_xxspi0_clk                  217  // GPPASPI0 xxspi0_clk
#define GPIOV2_PTL_PCD_DRIVER_espi_clk_loopbk             218  // GPPASPI0 espi_clk_loopbk
#define GPIOV2_PTL_PCD_DRIVER_spi0_clk_loopbk             219  // GPPASPI0 spi0_clk_loopbk

//
// vGPIO_3
//
#define GPIOV2_PTL_PCD_DRIVER_vGPIO_USB_0                 224  // vGPIO_3 vGPIO_USB_0
#define GPIOV2_PTL_PCD_DRIVER_vGPIO_USB_1                 225  // vGPIO_3 vGPIO_USB_1
#define GPIOV2_PTL_PCD_DRIVER_vGPIO_USB_2                 226  // vGPIO_3 vGPIO_USB_2
#define GPIOV2_PTL_PCD_DRIVER_vGPIO_USB_3                 227  // vGPIO_3 vGPIO_USB_3
#define GPIOV2_PTL_PCD_DRIVER_vGPIO_USB_4                 228  // vGPIO_3 vGPIO_USB_4
#define GPIOV2_PTL_PCD_DRIVER_vGPIO_USB_5                 229  // vGPIO_3 vGPIO_USB_5
#define GPIOV2_PTL_PCD_DRIVER_vGPIO_USB_6                 230  // vGPIO_3 vGPIO_USB_6
#define GPIOV2_PTL_PCD_DRIVER_vGPIO_USB_7                 231  // vGPIO_3 vGPIO_USB_7
#define GPIOV2_PTL_PCD_DRIVER_vGPIO_TS0                   232  // vGPIO_3 vGPIO_TS0
#define GPIOV2_PTL_PCD_DRIVER_vGPIO_TS1                   233  // vGPIO_3 vGPIO_TS1
#define GPIOV2_PTL_PCD_DRIVER_vGPIO_THC0                  234  // vGPIO_3 vGPIO_THC0
#define GPIOV2_PTL_PCD_DRIVER_vGPIO_THC1                  235  // vGPIO_3 vGPIO_THC1
#define GPIOV2_PTL_PCD_DRIVER_vGPIO_THC2                  236  // vGPIO_3 vGPIO_THC2
#define GPIOV2_PTL_PCD_DRIVER_vGPIO_THC3                  237  // vGPIO_3 vGPIO_THC3

//
// GPP_S
//
#define GPIOV2_PTL_PCD_DRIVER_xxgpp_s_0                   256  // GPP_S xxgpp_s_0
#define GPIOV2_PTL_PCD_DRIVER_xxgpp_s_1                   257  // GPP_S xxgpp_s_1
#define GPIOV2_PTL_PCD_DRIVER_xxgpp_s_2                   258  // GPP_S xxgpp_s_2
#define GPIOV2_PTL_PCD_DRIVER_xxgpp_s_3                   259  // GPP_S xxgpp_s_3
#define GPIOV2_PTL_PCD_DRIVER_xxgpp_s_4                   260  // GPP_S xxgpp_s_4
#define GPIOV2_PTL_PCD_DRIVER_xxgpp_s_5                   261  // GPP_S xxgpp_s_5
#define GPIOV2_PTL_PCD_DRIVER_xxgpp_s_6                   262  // GPP_S xxgpp_s_6
#define GPIOV2_PTL_PCD_DRIVER_xxgpp_s_7                   263  // GPP_S xxgpp_s_7

//
// GPP_B
//
#define GPIOV2_PTL_PCD_DRIVER_xxgpp_b_0                   288  // GPP_B xxgpp_b_0
#define GPIOV2_PTL_PCD_DRIVER_xxgpp_b_1                   289  // GPP_B xxgpp_b_1
#define GPIOV2_PTL_PCD_DRIVER_xxgpp_b_2                   290  // GPP_B xxgpp_b_2
#define GPIOV2_PTL_PCD_DRIVER_xxgpp_b_3                   291  // GPP_B xxgpp_b_3
#define GPIOV2_PTL_PCD_DRIVER_xxgpp_b_4                   292  // GPP_B xxgpp_b_4
#define GPIOV2_PTL_PCD_DRIVER_xxgpp_b_5                   293  // GPP_B xxgpp_b_5
#define GPIOV2_PTL_PCD_DRIVER_xxgpp_b_6                   294  // GPP_B xxgpp_b_6
#define GPIOV2_PTL_PCD_DRIVER_xxgpp_b_7                   295  // GPP_B xxgpp_b_7
#define GPIOV2_PTL_PCD_DRIVER_xxgpp_b_8                   296  // GPP_B xxgpp_b_8
#define GPIOV2_PTL_PCD_DRIVER_xxgpp_b_9                   297  // GPP_B xxgpp_b_9
#define GPIOV2_PTL_PCD_DRIVER_xxgpp_b_10                  298  // GPP_B xxgpp_b_10
#define GPIOV2_PTL_PCD_DRIVER_xxgpp_b_11                  299  // GPP_B xxgpp_b_11
#define GPIOV2_PTL_PCD_DRIVER_xxgpp_b_12                  300  // GPP_B xxgpp_b_12
#define GPIOV2_PTL_PCD_DRIVER_xxgpp_b_13                  301  // GPP_B xxgpp_b_13
#define GPIOV2_PTL_PCD_DRIVER_xxgpp_b_14                  302  // GPP_B xxgpp_b_14
#define GPIOV2_PTL_PCD_DRIVER_xxgpp_b_15                  303  // GPP_B xxgpp_b_15
#define GPIOV2_PTL_PCD_DRIVER_xxgpp_b_16                  304  // GPP_B xxgpp_b_16
#define GPIOV2_PTL_PCD_DRIVER_xxgpp_b_17                  305  // GPP_B xxgpp_b_17
#define GPIOV2_PTL_PCD_DRIVER_xxgpp_b_18                  306  // GPP_B xxgpp_b_18
#define GPIOV2_PTL_PCD_DRIVER_xxgpp_b_19                  307  // GPP_B xxgpp_b_19
#define GPIOV2_PTL_PCD_DRIVER_xxgpp_b_20                  308  // GPP_B xxgpp_b_20
#define GPIOV2_PTL_PCD_DRIVER_xxgpp_b_21                  309  // GPP_B xxgpp_b_21
#define GPIOV2_PTL_PCD_DRIVER_xxgpp_b_22                  310  // GPP_B xxgpp_b_22
#define GPIOV2_PTL_PCD_DRIVER_xxgpp_b_23                  311  // GPP_B xxgpp_b_23
#define GPIOV2_PTL_PCD_DRIVER_xxgpp_b_24                  312  // GPP_B xxgpp_b_24
#define GPIOV2_PTL_PCD_DRIVER_xxgpp_b_25                  313  // GPP_B xxgpp_b_25
#define GPIOV2_PTL_PCD_DRIVER_ishi3c0_clk_loopbk          314  // GPP_B ishi3c0_clk_loopbk

//
// GPP_D
//
#define GPIOV2_PTL_PCD_DRIVER_xxgpp_d_0                   320  // GPP_D xxgpp_d_0
#define GPIOV2_PTL_PCD_DRIVER_xxgpp_d_1                   321  // GPP_D xxgpp_d_1
#define GPIOV2_PTL_PCD_DRIVER_xxgpp_d_2                   322  // GPP_D xxgpp_d_2
#define GPIOV2_PTL_PCD_DRIVER_xxgpp_d_3                   323  // GPP_D xxgpp_d_3
#define GPIOV2_PTL_PCD_DRIVER_xxgpp_d_4                   324  // GPP_D xxgpp_d_4
#define GPIOV2_PTL_PCD_DRIVER_xxgpp_d_5                   325  // GPP_D xxgpp_d_5
#define GPIOV2_PTL_PCD_DRIVER_xxgpp_d_6                   326  // GPP_D xxgpp_d_6
#define GPIOV2_PTL_PCD_DRIVER_xxgpp_d_7                   327  // GPP_D xxgpp_d_7
#define GPIOV2_PTL_PCD_DRIVER_xxgpp_d_8                   328  // GPP_D xxgpp_d_8
#define GPIOV2_PTL_PCD_DRIVER_xxgpp_d_9                   329  // GPP_D xxgpp_d_9
#define GPIOV2_PTL_PCD_DRIVER_xxgpp_d_10                  330  // GPP_D xxgpp_d_10
#define GPIOV2_PTL_PCD_DRIVER_xxgpp_d_11                  331  // GPP_D xxgpp_d_11
#define GPIOV2_PTL_PCD_DRIVER_xxgpp_d_12                  332  // GPP_D xxgpp_d_12
#define GPIOV2_PTL_PCD_DRIVER_xxgpp_d_13                  333  // GPP_D xxgpp_d_13
#define GPIOV2_PTL_PCD_DRIVER_xxgpp_d_14                  334  // GPP_D xxgpp_d_14
#define GPIOV2_PTL_PCD_DRIVER_xxgpp_d_15                  335  // GPP_D xxgpp_d_15
#define GPIOV2_PTL_PCD_DRIVER_xxgpp_d_16                  336  // GPP_D xxgpp_d_16
#define GPIOV2_PTL_PCD_DRIVER_xxgpp_d_17                  337  // GPP_D xxgpp_d_17
#define GPIOV2_PTL_PCD_DRIVER_xxgpp_d_18                  338  // GPP_D xxgpp_d_18
#define GPIOV2_PTL_PCD_DRIVER_xxgpp_d_19                  339  // GPP_D xxgpp_d_19
#define GPIOV2_PTL_PCD_DRIVER_xxgpp_d_20                  340  // GPP_D xxgpp_d_20
#define GPIOV2_PTL_PCD_DRIVER_xxgpp_d_21                  341  // GPP_D xxgpp_d_21
#define GPIOV2_PTL_PCD_DRIVER_xxgpp_d_22                  342  // GPP_D xxgpp_d_22
#define GPIOV2_PTL_PCD_DRIVER_xxgpp_d_23                  343  // GPP_D xxgpp_d_23
#define GPIOV2_PTL_PCD_DRIVER_xxgpp_d_24                  344  // GPP_D xxgpp_d_24
#define GPIOV2_PTL_PCD_DRIVER_xxgpp_d_25                  345  // GPP_D xxgpp_d_25
#define GPIOV2_PTL_PCD_DRIVER_shd_i3c_clk_loopbk          346  // GPP_D shd_i3c_clk_loopbk

//
// vGPIO
//
#define GPIOV2_PTL_PCD_DRIVER_vGPIO_0                     352  // vGPIO vGPIO_0
#define GPIOV2_PTL_PCD_DRIVER_vGPIO_5                     353  // vGPIO vGPIO_5
#define GPIOV2_PTL_PCD_DRIVER_vGPIO_30                    354  // vGPIO vGPIO_30
#define GPIOV2_PTL_PCD_DRIVER_vGPIO_31                    355  // vGPIO vGPIO_31
#define GPIOV2_PTL_PCD_DRIVER_vGPIO_32                    356  // vGPIO vGPIO_32
#define GPIOV2_PTL_PCD_DRIVER_vGPIO_33                    357  // vGPIO vGPIO_33
#define GPIOV2_PTL_PCD_DRIVER_vGPIO_34                    358  // vGPIO vGPIO_34
#define GPIOV2_PTL_PCD_DRIVER_vGPIO_35                    359  // vGPIO vGPIO_35
#define GPIOV2_PTL_PCD_DRIVER_vGPIO_36                    360  // vGPIO vGPIO_36
#define GPIOV2_PTL_PCD_DRIVER_vGPIO_37                    361  // vGPIO vGPIO_37
#define GPIOV2_PTL_PCD_DRIVER_vGPIO_40                    362  // vGPIO vGPIO_40
#define GPIOV2_PTL_PCD_DRIVER_vGPIO_41                    363  // vGPIO vGPIO_41
#define GPIOV2_PTL_PCD_DRIVER_vGPIO_42                    364  // vGPIO vGPIO_42
#define GPIOV2_PTL_PCD_DRIVER_vGPIO_43                    365  // vGPIO vGPIO_43
#define GPIOV2_PTL_PCD_DRIVER_vGPIO_44                    366  // vGPIO vGPIO_44
#define GPIOV2_PTL_PCD_DRIVER_vGPIO_45                    367  // vGPIO vGPIO_45
#define GPIOV2_PTL_PCD_DRIVER_vGPIO_46                    368  // vGPIO vGPIO_46
#define GPIOV2_PTL_PCD_DRIVER_vGPIO_47                    369  // vGPIO vGPIO_47



#endif // _GPIOV2_ACPI_DEFINES_PTL_PCD_H_
