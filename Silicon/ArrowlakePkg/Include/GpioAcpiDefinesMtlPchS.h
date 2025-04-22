/** @file
  GPIO ASL header for MTL-PCH-S

  Copyright (c) 2024, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/
//
// Definition for GPIO groups and pads
//
#ifndef _GPIO_ACPI_DEFINES_MTL_PCH_
#define _GPIO_ACPI_DEFINES_MTL_PCH_

#define GPIOV2_MTL_PCH_S_HID "INTC1084" //MTL-PCH-S
#define GPIO_HID_MTL_SOC_S "INTC1082"

//
// PCH GPIO Community address
//
#define MTL_PCH_GPIO_COM0 0x006E0000 // PID_GPIOCOM0 = 0x6E
#define MTL_PCH_GPIO_COM1 0x006D0000 // PID_GPIOCOM1 = 0x6D
#define MTL_PCH_GPIO_COM2 0x006C0000 // PID_GPIOCOM2 = 0x6C
#define MTL_PCH_GPIO_COM3 0x006B0000 // PID_GPIOCOM3 = 0x6B
#define MTL_PCH_GPIO_COM4 0x006A0000 // PID_GPIOCOM4 = 0x6A
#define MTL_PCH_GPIO_COM5 0x00690000 // PID_GPIOCOM5 = 0x69


//
//  GPP_D
//
#define GPIOV2_MTL_PCH_S_DRIVER_GPP_D_0                0
#define GPIOV2_MTL_PCH_S_DRIVER_GPP_D_1                1
#define GPIOV2_MTL_PCH_S_DRIVER_GPP_D_2                2
#define GPIOV2_MTL_PCH_S_DRIVER_GPP_D_3                3
#define GPIOV2_MTL_PCH_S_DRIVER_GPP_D_4                4
#define GPIOV2_MTL_PCH_S_DRIVER_GPP_D_5                5
#define GPIOV2_MTL_PCH_S_DRIVER_GPP_D_6                6
#define GPIOV2_MTL_PCH_S_DRIVER_GPP_D_7                7
#define GPIOV2_MTL_PCH_S_DRIVER_GPP_D_8                8
#define GPIOV2_MTL_PCH_S_DRIVER_GPP_D_9                9
#define GPIOV2_MTL_PCH_S_DRIVER_GPP_D_10               10
#define GPIOV2_MTL_PCH_S_DRIVER_GPP_D_11               11
#define GPIOV2_MTL_PCH_S_DRIVER_GPP_D_12               12
#define GPIOV2_MTL_PCH_S_DRIVER_GPP_D_13               13
#define GPIOV2_MTL_PCH_S_DRIVER_GPP_D_14               14
#define GPIOV2_MTL_PCH_S_DRIVER_GPP_D_15               15
#define GPIOV2_MTL_PCH_S_DRIVER_GPP_D_16               16
#define GPIOV2_MTL_PCH_S_DRIVER_GPP_D_17               17
#define GPIOV2_MTL_PCH_S_DRIVER_GPP_D_18               18
#define GPIOV2_MTL_PCH_S_DRIVER_GPP_D_19               19
#define GPIOV2_MTL_PCH_S_DRIVER_GPP_D_20               20
#define GPIOV2_MTL_PCH_S_DRIVER_GPP_D_21               21
#define GPIOV2_MTL_PCH_S_DRIVER_GPP_D_22               22
#define GPIOV2_MTL_PCH_S_DRIVER_GPP_D_23               23
#define GPIOV2_MTL_PCH_S_DRIVER_GSPI3_THC1_CLK_LOOPBK  24

//
// GPP_R
//
#define GPIOV2_MTL_PCH_S_DRIVER_GPP_R_0                32
#define GPIOV2_MTL_PCH_S_DRIVER_GPP_R_1                33
#define GPIOV2_MTL_PCH_S_DRIVER_GPP_R_2                34
#define GPIOV2_MTL_PCH_S_DRIVER_GPP_R_3                35
#define GPIOV2_MTL_PCH_S_DRIVER_GPP_R_4                36
#define GPIOV2_MTL_PCH_S_DRIVER_GPP_R_5                37
#define GPIOV2_MTL_PCH_S_DRIVER_GPP_R_6                38
#define GPIOV2_MTL_PCH_S_DRIVER_GPP_R_7                39
#define GPIOV2_MTL_PCH_S_DRIVER_GPP_R_8                40
#define GPIOV2_MTL_PCH_S_DRIVER_GPP_R_9                41
#define GPIOV2_MTL_PCH_S_DRIVER_GPP_R_10               42
#define GPIOV2_MTL_PCH_S_DRIVER_GPP_R_11               43
#define GPIOV2_MTL_PCH_S_DRIVER_GPP_R_12               44
#define GPIOV2_MTL_PCH_S_DRIVER_GSPI2_CLK_LOOPBK       45

//
// GPP_J
//
#define GPIOV2_MTL_PCH_S_DRIVER_GPP_J_0                64
#define GPIOV2_MTL_PCH_S_DRIVER_GPP_J_1                65
#define GPIOV2_MTL_PCH_S_DRIVER_GPP_J_2                66
#define GPIOV2_MTL_PCH_S_DRIVER_GPP_J_3                67
#define GPIOV2_MTL_PCH_S_DRIVER_GPP_J_4                68
#define GPIOV2_MTL_PCH_S_DRIVER_GPP_J_5                69
#define GPIOV2_MTL_PCH_S_DRIVER_GPP_J_6                70
#define GPIOV2_MTL_PCH_S_DRIVER_GPP_J_7                71
#define GPIOV2_MTL_PCH_S_DRIVER_GPP_J_8                72
#define GPIOV2_MTL_PCH_S_DRIVER_GPP_J_9                73
#define GPIOV2_MTL_PCH_S_DRIVER_GPP_J_10               74
#define GPIOV2_MTL_PCH_S_DRIVER_GPP_J_11               75
#define GPIOV2_MTL_PCH_S_DRIVER_GPP_J_12               76
#define GPIOV2_MTL_PCH_S_DRIVER_GPP_J_13               77
#define GPIOV2_MTL_PCH_S_DRIVER_GPP_J_14               78
#define GPIOV2_MTL_PCH_S_DRIVER_GPP_J_15               79
#define GPIOV2_MTL_PCH_S_DRIVER_GPP_J_16               80
#define GPIOV2_MTL_PCH_S_DRIVER_RESET_SYNCB            81

//
// VGPIO
//
#define GPIOV2_MTL_PCH_S_DRIVER_VGPIO_0                96
#define GPIOV2_MTL_PCH_S_DRIVER_VGPIO_4                97
#define GPIOV2_MTL_PCH_S_DRIVER_VGPIO_5                98
#define GPIOV2_MTL_PCH_S_DRIVER_VGPIO_6                99
#define GPIOV2_MTL_PCH_S_DRIVER_VGPIO_7                100
#define GPIOV2_MTL_PCH_S_DRIVER_VGPIO_8                101
#define GPIOV2_MTL_PCH_S_DRIVER_VGPIO_9                102
#define GPIOV2_MTL_PCH_S_DRIVER_VGPIO_10               103
#define GPIOV2_MTL_PCH_S_DRIVER_VGPIO_11               104
#define GPIOV2_MTL_PCH_S_DRIVER_VGPIO_12               105
#define GPIOV2_MTL_PCH_S_DRIVER_VGPIO_13               106
#define GPIOV2_MTL_PCH_S_DRIVER_VGPIO_18               107
#define GPIOV2_MTL_PCH_S_DRIVER_VGPIO_19               108
#define GPIOV2_MTL_PCH_S_DRIVER_VGPIO_20               109
#define GPIOV2_MTL_PCH_S_DRIVER_VGPIO_21               110
#define GPIOV2_MTL_PCH_S_DRIVER_VGPIO_22               111
#define GPIOV2_MTL_PCH_S_DRIVER_VGPIO_23               112
#define GPIOV2_MTL_PCH_S_DRIVER_VGPIO_24               113
#define GPIOV2_MTL_PCH_S_DRIVER_VGPIO_25               114
#define GPIOV2_MTL_PCH_S_DRIVER_VGPIO_30               115
#define GPIOV2_MTL_PCH_S_DRIVER_VGPIO_31               116
#define GPIOV2_MTL_PCH_S_DRIVER_VGPIO_32               117
#define GPIOV2_MTL_PCH_S_DRIVER_VGPIO_33               118
#define GPIOV2_MTL_PCH_S_DRIVER_VGPIO_34               119
#define GPIOV2_MTL_PCH_S_DRIVER_VGPIO_35               120
#define GPIOV2_MTL_PCH_S_DRIVER_VGPIO_36               121
#define GPIOV2_MTL_PCH_S_DRIVER_VGPIO_37               122
#define GPIOV2_MTL_PCH_S_DRIVER_VGPIO_THC0             123
#define GPIOV2_MTL_PCH_S_DRIVER_VGPIO_THC1             124
#define GPIOV2_MTL_PCH_S_DRIVER_VGPIO_THC2             125
#define GPIOV2_MTL_PCH_S_DRIVER_VGPIO_THC3             126

//
// GPP_A
//
#define GPIOV2_MTL_PCH_S_DRIVER_GPP_A_0                128
#define GPIOV2_MTL_PCH_S_DRIVER_GPP_A_1                129
#define GPIOV2_MTL_PCH_S_DRIVER_GPP_A_2                130
#define GPIOV2_MTL_PCH_S_DRIVER_GPP_A_3                131
#define GPIOV2_MTL_PCH_S_DRIVER_GPP_A_4                132
#define GPIOV2_MTL_PCH_S_DRIVER_GPP_A_5                133
#define GPIOV2_MTL_PCH_S_DRIVER_GPP_A_6                134
#define GPIOV2_MTL_PCH_S_DRIVER_GPP_A_7                135
#define GPIOV2_MTL_PCH_S_DRIVER_GPP_A_8                136
#define GPIOV2_MTL_PCH_S_DRIVER_GPP_A_9                137
#define GPIOV2_MTL_PCH_S_DRIVER_GPP_A_10               138
#define GPIOV2_MTL_PCH_S_DRIVER_GPP_A_11               139
#define GPIOV2_MTL_PCH_S_DRIVER_GPP_A_12               140
#define GPIOV2_MTL_PCH_S_DRIVER_GPP_A_13               141
#define GPIOV2_MTL_PCH_S_DRIVER_ESPI_CLK_LOOPBK        142


//
// DIR_ESPI
//
#define GPIOV2_MTL_PCH_S_DRIVER_PWRBTNB_OUT            160
#define GPIOV2_MTL_PCH_S_DRIVER_DMI_PERSTB             161
#define GPIOV2_MTL_PCH_S_DRIVER_DMI_CLKREQB            162
#define GPIOV2_MTL_PCH_S_DRIVER_DIR_ESPI_IO_0          163
#define GPIOV2_MTL_PCH_S_DRIVER_DIR_ESPI_IO_1          164
#define GPIOV2_MTL_PCH_S_DRIVER_DIR_ESPI_IO_2          165
#define GPIOV2_MTL_PCH_S_DRIVER_DIR_ESPI_IO_3          166
#define GPIOV2_MTL_PCH_S_DRIVER_DIR_ESPI_CSB           167
#define GPIOV2_MTL_PCH_S_DRIVER_DIR_ESPI_RESETB        168
#define GPIOV2_MTL_PCH_S_DRIVER_DIR_ESPI_CLK           169
#define GPIOV2_MTL_PCH_S_DRIVER_DIR_ESPI_RCLK          170
#define GPIOV2_MTL_PCH_S_DRIVER_DIR_ESPI_ALERTB        171

//
// GPP_B
//
#define GPIOV2_MTL_PCH_S_DRIVER_GPP_B_0                192
#define GPIOV2_MTL_PCH_S_DRIVER_GPP_B_1                193
#define GPIOV2_MTL_PCH_S_DRIVER_GPP_B_2                194
#define GPIOV2_MTL_PCH_S_DRIVER_GPP_B_3                195
#define GPIOV2_MTL_PCH_S_DRIVER_GPP_B_4                196
#define GPIOV2_MTL_PCH_S_DRIVER_GPP_B_5                197
#define GPIOV2_MTL_PCH_S_DRIVER_GPP_B_6                198
#define GPIOV2_MTL_PCH_S_DRIVER_GPP_B_7                199
#define GPIOV2_MTL_PCH_S_DRIVER_GPP_B_8                200
#define GPIOV2_MTL_PCH_S_DRIVER_GPP_B_9                201
#define GPIOV2_MTL_PCH_S_DRIVER_GPP_B_10               202
#define GPIOV2_MTL_PCH_S_DRIVER_GPP_B_11               203
#define GPIOV2_MTL_PCH_S_DRIVER_GPP_B_12               204
#define GPIOV2_MTL_PCH_S_DRIVER_GPP_B_13               205
#define GPIOV2_MTL_PCH_S_DRIVER_GPP_B_14               206
#define GPIOV2_MTL_PCH_S_DRIVER_GPP_B_15               207
#define GPIOV2_MTL_PCH_S_DRIVER_GPP_B_16               208
#define GPIOV2_MTL_PCH_S_DRIVER_GPP_B_17               209
#define GPIOV2_MTL_PCH_S_DRIVER_GPP_B_18               210
#define GPIOV2_MTL_PCH_S_DRIVER_GPP_B_19               211
#define GPIOV2_MTL_PCH_S_DRIVER_GPP_B_20               212
#define GPIOV2_MTL_PCH_S_DRIVER_GPP_B_21               213

//
// SPI
//
#define GPIOV2_MTL_PCH_S_DRIVER_SPI0_IO_2              224
#define GPIOV2_MTL_PCH_S_DRIVER_SPI0_IO_3              225
#define GPIOV2_MTL_PCH_S_DRIVER_SPI0_MOSI_IO_0         226
#define GPIOV2_MTL_PCH_S_DRIVER_SPI0_MISO_IO_1         227
#define GPIOV2_MTL_PCH_S_DRIVER_SPI0_TPM_CSB           228
#define GPIOV2_MTL_PCH_S_DRIVER_SPI0_FLASH_0_CSB       229
#define GPIOV2_MTL_PCH_S_DRIVER_SPI0_FLASH_1_CSB       230
#define GPIOV2_MTL_PCH_S_DRIVER_SPI0_CLK               231
#define GPIOV2_MTL_PCH_S_DRIVER_SPI0_CLK_LOOPBK        232

//
// GPP_C
//
#define GPIOV2_MTL_PCH_S_DRIVER_GPP_C_0                256
#define GPIOV2_MTL_PCH_S_DRIVER_GPP_C_1                257
#define GPIOV2_MTL_PCH_S_DRIVER_GPP_C_2                258
#define GPIOV2_MTL_PCH_S_DRIVER_GPP_C_3                259
#define GPIOV2_MTL_PCH_S_DRIVER_GPP_C_4                260
#define GPIOV2_MTL_PCH_S_DRIVER_GPP_C_5                261
#define GPIOV2_MTL_PCH_S_DRIVER_GPP_C_6                262
#define GPIOV2_MTL_PCH_S_DRIVER_GPP_C_7                263
#define GPIOV2_MTL_PCH_S_DRIVER_GPP_C_8                264
#define GPIOV2_MTL_PCH_S_DRIVER_GPP_C_9                265
#define GPIOV2_MTL_PCH_S_DRIVER_GPP_C_10               266
#define GPIOV2_MTL_PCH_S_DRIVER_GPP_C_11               267
#define GPIOV2_MTL_PCH_S_DRIVER_GPP_C_12               268
#define GPIOV2_MTL_PCH_S_DRIVER_GPP_C_13               269
#define GPIOV2_MTL_PCH_S_DRIVER_GPP_C_14               270
#define GPIOV2_MTL_PCH_S_DRIVER_GPP_C_15               271
#define GPIOV2_MTL_PCH_S_DRIVER_GPP_C_16               272
#define GPIOV2_MTL_PCH_S_DRIVER_GPP_C_17               273
#define GPIOV2_MTL_PCH_S_DRIVER_GPP_C_18               274
#define GPIOV2_MTL_PCH_S_DRIVER_GPP_C_19               275
#define GPIOV2_MTL_PCH_S_DRIVER_GPP_C_20               276
#define GPIOV2_MTL_PCH_S_DRIVER_GPP_C_21               277
#define GPIOV2_MTL_PCH_S_DRIVER_GPP_C_22               278
#define GPIOV2_MTL_PCH_S_DRIVER_GPP_C_23               279

//
// GPP_H
//
#define GPIOV2_MTL_PCH_S_DRIVER_GPP_H_0                288
#define GPIOV2_MTL_PCH_S_DRIVER_GPP_H_1                289
#define GPIOV2_MTL_PCH_S_DRIVER_GPP_H_2                290
#define GPIOV2_MTL_PCH_S_DRIVER_GPP_H_3                291
#define GPIOV2_MTL_PCH_S_DRIVER_GPP_H_4                292
#define GPIOV2_MTL_PCH_S_DRIVER_GPP_H_5                293
#define GPIOV2_MTL_PCH_S_DRIVER_GPP_H_6                294
#define GPIOV2_MTL_PCH_S_DRIVER_GPP_H_7                295
#define GPIOV2_MTL_PCH_S_DRIVER_GPP_H_8                296
#define GPIOV2_MTL_PCH_S_DRIVER_GPP_H_9                297
#define GPIOV2_MTL_PCH_S_DRIVER_GPP_H_10               298
#define GPIOV2_MTL_PCH_S_DRIVER_GPP_H_11               299
#define GPIOV2_MTL_PCH_S_DRIVER_GPP_H_12               300
#define GPIOV2_MTL_PCH_S_DRIVER_GPP_H_13               301
#define GPIOV2_MTL_PCH_S_DRIVER_GPP_H_14               302
#define GPIOV2_MTL_PCH_S_DRIVER_GPP_H_15               303
#define GPIOV2_MTL_PCH_S_DRIVER_GPP_H_16               304
#define GPIOV2_MTL_PCH_S_DRIVER_GPP_H_17               305
#define GPIOV2_MTL_PCH_S_DRIVER_GPP_H_18               306
#define GPIOV2_MTL_PCH_S_DRIVER_GPP_H_19               307

//
// VGPIO_3
//
#define GPIOV2_MTL_PCH_S_DRIVER_VGPIO_PCIE_80          320
#define GPIOV2_MTL_PCH_S_DRIVER_VGPIO_PCIE_81          321
#define GPIOV2_MTL_PCH_S_DRIVER_VGPIO_PCIE_82          322
#define GPIOV2_MTL_PCH_S_DRIVER_VGPIO_PCIE_83          323

//
// VGPIO_0
//
#define GPIOV2_MTL_PCH_S_DRIVER_VGPIO_USB_0            352
#define GPIOV2_MTL_PCH_S_DRIVER_VGPIO_USB_1            353
#define GPIOV2_MTL_PCH_S_DRIVER_VGPIO_USB_2            354
#define GPIOV2_MTL_PCH_S_DRIVER_VGPIO_USB_3            355
#define GPIOV2_MTL_PCH_S_DRIVER_VGPIO_USB_8            356
#define GPIOV2_MTL_PCH_S_DRIVER_VGPIO_USB_9            357
#define GPIOV2_MTL_PCH_S_DRIVER_VGPIO_USB_10           358
#define GPIOV2_MTL_PCH_S_DRIVER_VGPIO_USB_11           359

//
// VGPIO_4
//
#define GPIOV2_MTL_PCH_S_DRIVER_VGPIO_ISCLK_0          384
#define GPIOV2_MTL_PCH_S_DRIVER_VGPIO_ISCLK_1          385
#define GPIOV2_MTL_PCH_S_DRIVER_VGPIO_SLPC_0           386
#define GPIOV2_MTL_PCH_S_DRIVER_VGPIO_SLPC_1           387
#define GPIOV2_MTL_PCH_S_DRIVER_VGPIO_SLPC_2           388
#define GPIOV2_MTL_PCH_S_DRIVER_VGPIO_SLPC_3           389
#define GPIOV2_MTL_PCH_S_DRIVER_VGPIO_SLPC_4           390
#define GPIOV2_MTL_PCH_S_DRIVER_VGPIO_SLPC_5           391
#define GPIOV2_MTL_PCH_S_DRIVER_VGPIO_SLPC_6           392
#define GPIOV2_MTL_PCH_S_DRIVER_VGPIO_SPARE_0          393
#define GPIOV2_MTL_PCH_S_DRIVER_VGPIO_SPARE_1          394
#define GPIOV2_MTL_PCH_S_DRIVER_VGPIO_SPARE_2          395
#define GPIOV2_MTL_PCH_S_DRIVER_VGPIO_SPARE_3          396
#define GPIOV2_MTL_PCH_S_DRIVER_VGPIO_SPARE_4          397
#define GPIOV2_MTL_PCH_S_DRIVER_VGPIO_SPARE_5          398
#define GPIOV2_MTL_PCH_S_DRIVER_VGPIO_SPARE_6          399
#define GPIOV2_MTL_PCH_S_DRIVER_VGPIO_SPARE_7          400
#define GPIOV2_MTL_PCH_S_DRIVER_VGPIO_SPARE_8          401
#define GPIOV2_MTL_PCH_S_DRIVER_VGPIO_SPARE_9          402
#define GPIOV2_MTL_PCH_S_DRIVER_VGPIO_SPARE_10         403
#define GPIOV2_MTL_PCH_S_DRIVER_VGPIO_SPARE_11         404
#define GPIOV2_MTL_PCH_S_DRIVER_VGPIO_SPARE_12         405
#define GPIOV2_MTL_PCH_S_DRIVER_VGPIO_SPARE_13         406
#define GPIOV2_MTL_PCH_S_DRIVER_VGPIO_SPARE_14         407
#define GPIOV2_MTL_PCH_S_DRIVER_VGPIO_SPARE_15         408
#define GPIOV2_MTL_PCH_S_DRIVER_VGPIO_SPARE_16         409
#define GPIOV2_MTL_PCH_S_DRIVER_VGPIO_SPARE_17         410
#define GPIOV2_MTL_PCH_S_DRIVER_VGPIO_SPARE_18         411
#define GPIOV2_MTL_PCH_S_DRIVER_VGPIO_SPARE_19         412
#define GPIOV2_MTL_PCH_S_DRIVER_VGPIO_SPARE_20         413
#define GPIOV2_MTL_PCH_S_DRIVER_VGPIO_SPARE_21         414

//
// GPP_S
//
#define GPIOV2_MTL_PCH_S_DRIVER_GPP_S_0                416
#define GPIOV2_MTL_PCH_S_DRIVER_GPP_S_1                417
#define GPIOV2_MTL_PCH_S_DRIVER_GPP_S_2                418
#define GPIOV2_MTL_PCH_S_DRIVER_GPP_S_3                419
#define GPIOV2_MTL_PCH_S_DRIVER_GPP_S_4                420
#define GPIOV2_MTL_PCH_S_DRIVER_GPP_S_5                421
#define GPIOV2_MTL_PCH_S_DRIVER_GPP_S_6                422
#define GPIOV2_MTL_PCH_S_DRIVER_GPP_S_7                423

//
// GPP_E
//
#define GPIOV2_MTL_PCH_S_DRIVER_GPP_E_0                448
#define GPIOV2_MTL_PCH_S_DRIVER_GPP_E_1                449
#define GPIOV2_MTL_PCH_S_DRIVER_GPP_E_2                450
#define GPIOV2_MTL_PCH_S_DRIVER_GPP_E_3                451
#define GPIOV2_MTL_PCH_S_DRIVER_GPP_E_4                452
#define GPIOV2_MTL_PCH_S_DRIVER_GPP_E_5                453
#define GPIOV2_MTL_PCH_S_DRIVER_GPP_E_6                454
#define GPIOV2_MTL_PCH_S_DRIVER_GPP_E_7                455
#define GPIOV2_MTL_PCH_S_DRIVER_GPP_E_8                456
#define GPIOV2_MTL_PCH_S_DRIVER_GPP_E_9                457
#define GPIOV2_MTL_PCH_S_DRIVER_GPP_E_10               458
#define GPIOV2_MTL_PCH_S_DRIVER_GPP_E_11               459
#define GPIOV2_MTL_PCH_S_DRIVER_GPP_E_12               460
#define GPIOV2_MTL_PCH_S_DRIVER_GPP_E_13               461
#define GPIOV2_MTL_PCH_S_DRIVER_GPP_E_14               462
#define GPIOV2_MTL_PCH_S_DRIVER_GPP_E_15               463
#define GPIOV2_MTL_PCH_S_DRIVER_GPP_E_16               464
#define GPIOV2_MTL_PCH_S_DRIVER_GPP_E_17               465
#define GPIOV2_MTL_PCH_S_DRIVER_GPP_E_18               466
#define GPIOV2_MTL_PCH_S_DRIVER_GPP_E_19               467
#define GPIOV2_MTL_PCH_S_DRIVER_GPP_E_20               468
#define GPIOV2_MTL_PCH_S_DRIVER_GPP_E_21               469
#define GPIOV2_MTL_PCH_S_DRIVER_SPI1_THC0_CLK_LOOPBK   470

//
//  GPP_K
//
#define GPIOV2_MTL_PCH_S_DRIVER_GPP_K_0                480
#define GPIOV2_MTL_PCH_S_DRIVER_GPP_K_1                481
#define GPIOV2_MTL_PCH_S_DRIVER_GPP_K_2                482
#define GPIOV2_MTL_PCH_S_DRIVER_GPP_K_3                483
#define GPIOV2_MTL_PCH_S_DRIVER_GPP_K_4                484
#define GPIOV2_MTL_PCH_S_DRIVER_GPP_K_5                485
#define GPIOV2_MTL_PCH_S_DRIVER_GPP_K_6                486
#define GPIOV2_MTL_PCH_S_DRIVER_GPP_K_7                487
#define GPIOV2_MTL_PCH_S_DRIVER_GPP_K_8                488
#define GPIOV2_MTL_PCH_S_DRIVER_GPP_K_9                489
#define GPIOV2_MTL_PCH_S_DRIVER_GPP_K_10               490
#define GPIOV2_MTL_PCH_S_DRIVER_MISC_SPARE             491
#define GPIOV2_MTL_PCH_S_DRIVER_SYS_RESETB             492
#define GPIOV2_MTL_PCH_S_DRIVER_MLK_RSTB               493

//
// GPP_F
//
#define GPIOV2_MTL_PCH_S_DRIVER_GPP_F_0                512
#define GPIOV2_MTL_PCH_S_DRIVER_GPP_F_1                513
#define GPIOV2_MTL_PCH_S_DRIVER_GPP_F_2                514
#define GPIOV2_MTL_PCH_S_DRIVER_GPP_F_3                515
#define GPIOV2_MTL_PCH_S_DRIVER_GPP_F_4                516
#define GPIOV2_MTL_PCH_S_DRIVER_GPP_F_5                517
#define GPIOV2_MTL_PCH_S_DRIVER_GPP_F_6                518
#define GPIOV2_MTL_PCH_S_DRIVER_GPP_F_7                519
#define GPIOV2_MTL_PCH_S_DRIVER_GPP_F_8                520
#define GPIOV2_MTL_PCH_S_DRIVER_GPP_F_9                521
#define GPIOV2_MTL_PCH_S_DRIVER_GPP_F_10               522
#define GPIOV2_MTL_PCH_S_DRIVER_GPP_F_11               523
#define GPIOV2_MTL_PCH_S_DRIVER_GPP_F_12               524
#define GPIOV2_MTL_PCH_S_DRIVER_GPP_F_13               525
#define GPIOV2_MTL_PCH_S_DRIVER_GPP_F_14               526
#define GPIOV2_MTL_PCH_S_DRIVER_GPP_F_15               527
#define GPIOV2_MTL_PCH_S_DRIVER_GPP_F_16               528
#define GPIOV2_MTL_PCH_S_DRIVER_GPP_F_17               529
#define GPIOV2_MTL_PCH_S_DRIVER_GPP_F_18               530
#define GPIOV2_MTL_PCH_S_DRIVER_GPP_F_19               531
#define GPIOV2_MTL_PCH_S_DRIVER_GPP_F_20               532
#define GPIOV2_MTL_PCH_S_DRIVER_GPP_F_21               533
#define GPIOV2_MTL_PCH_S_DRIVER_GPP_F_22               534
#define GPIOV2_MTL_PCH_S_DRIVER_GPP_F_23               535

//
//  GPP_I
//
#define GPIOV2_MTL_PCH_S_DRIVER_GPP_I_0                544
#define GPIOV2_MTL_PCH_S_DRIVER_GPP_I_1                545
#define GPIOV2_MTL_PCH_S_DRIVER_GPP_I_2                546
#define GPIOV2_MTL_PCH_S_DRIVER_GPP_I_3                547
#define GPIOV2_MTL_PCH_S_DRIVER_GPP_I_4                548
#define GPIOV2_MTL_PCH_S_DRIVER_GPP_I_5                549
#define GPIOV2_MTL_PCH_S_DRIVER_GPP_I_6                550
#define GPIOV2_MTL_PCH_S_DRIVER_GPP_I_7                551
#define GPIOV2_MTL_PCH_S_DRIVER_GPP_I_8                552
#define GPIOV2_MTL_PCH_S_DRIVER_GPP_I_9                553
#define GPIOV2_MTL_PCH_S_DRIVER_GPP_I_10               554
#define GPIOV2_MTL_PCH_S_DRIVER_GPP_I_11               555
#define GPIOV2_MTL_PCH_S_DRIVER_GPP_I_12               556
#define GPIOV2_MTL_PCH_S_DRIVER_GPP_I_13               557
#define GPIOV2_MTL_PCH_S_DRIVER_GPP_I_14               558
#define GPIOV2_MTL_PCH_S_DRIVER_GPP_I_15               559
#define GPIOV2_MTL_PCH_S_DRIVER_GPP_I_16               560
#define GPIOV2_MTL_PCH_S_DRIVER_GSPI0_CLK_LOOPBK       561
#define GPIOV2_MTL_PCH_S_DRIVER_GSPI1_CLK_LOOPBK       562
#define GPIOV2_MTL_PCH_S_DRIVER_ISH_I3C0_CLK_LOOPBK    563
#define GPIOV2_MTL_PCH_S_DRIVER_I3C0_CLK_LOOPBK        564

//
// JTAG
//
#define GPIOV2_MTL_PCH_S_DRIVER_JTAG_TDO               576
#define GPIOV2_MTL_PCH_S_DRIVER_JTAGX                  577
#define GPIOV2_MTL_PCH_S_DRIVER_PRDYB                  578
#define GPIOV2_MTL_PCH_S_DRIVER_PREQB                  579
#define GPIOV2_MTL_PCH_S_DRIVER_JTAG_TDI               580
#define GPIOV2_MTL_PCH_S_DRIVER_JTAG_TMS               581
#define GPIOV2_MTL_PCH_S_DRIVER_JTAG_TCK               582
#define GPIOV2_MTL_PCH_S_DRIVER_DBG_PMODE              583
#define GPIOV2_MTL_PCH_S_DRIVER_CPU_TRSTB              584
#define GPIOV2_MTL_PCH_S_DRIVER_CPUPWRGD               585
#define GPIOV2_MTL_PCH_S_DRIVER_PM_SPARE0              586
#define GPIOV2_MTL_PCH_S_DRIVER_PM_SPARE1              587
#define GPIOV2_MTL_PCH_S_DRIVER_CRASHLOG_TRIG_N        588
#define GPIOV2_MTL_PCH_S_DRIVER_TRIGGER_IN             589
#define GPIOV2_MTL_PCH_S_DRIVER_TRIGGER_OUT            590
#define GPIOV2_MTL_PCH_S_DRIVER_FBRK_OUT_N             591

#endif // _GPIO_ACPI_DEFINES_MTL_PCH_
