/** @file
  This file was automatically generated. Modify at your own risk.
  Note that no error checking is done in these functions so ensure that the correct values are passed.

  Copyright (c) 2020, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef __Pci000StructAdl0xxx_h__
#define __Pci000StructAdl0xxx_h__

#pragma pack(push, 1)
typedef union {
  struct {
    UINT32 PXPEPBAREN                              :  1;  // Bits 0:0
    UINT32                                         :  11;  // Bits 11:1
    UINT32 PXPEPBARLow                             :  20;  // Bits 31:12
    UINT32 PXPEPBARHigh                            :  10;  // Bits 41:32
  } Bits;
  UINT64 Data;
  UINT32 Data32[2];
  UINT16 Data16[4];
  UINT8  Data8[8];
} PXPEPBAR_0_0_0_PCI_STRUCT;
typedef union {
  struct {
    UINT32 MCHBAREN                                :  1;  // Bits 0:0
    UINT32                                         :  16;  // Bits 16:1
    UINT32 MCHBARLow                               :  15;  // Bits 31:17
    UINT32 MCHBARHigh                              :  10;  // Bits 41:32
  } Bits;
  UINT64 Data;
  UINT32 Data32[2];
  UINT16 Data16[4];
  UINT8  Data8[8];
} MCHBAR_0_0_0_PCI_STRUCT;
typedef union {
  struct {
    UINT32 GGCLCK                                  :  1;  // Bits 0:0
    UINT32 IVD                                     :  1;  // Bits 1:1
    UINT32 VAMEN                                   :  1;  // Bits 2:2
    UINT32                                         :  3;  // Bits 5:3
    UINT32 GGMS                                    :  2;  // Bits 7:6
    UINT32 GMS                                     :  8;  // Bits 15:8
    UINT32                                         :  16;  // Bits 31:16
  } Bits;
  UINT32 Data;
  UINT16 Data16[2];
  UINT8  Data8[4];
} GGC_0_0_0_PCI_STRUCT;
typedef union {
  struct {
    UINT32 D0EN                                    :  1;  // Bits 0:0
    UINT32 D1F2EN                                  :  1;  // Bits 1:1
    UINT32 D1F1EN                                  :  1;  // Bits 2:2
    UINT32 D1F0EN                                  :  1;  // Bits 3:3
    UINT32 D2EN                                    :  1;  // Bits 4:4
    UINT32 D3F0EN                                  :  1;  // Bits 5:5
    UINT32 D3F7EN                                  :  1;  // Bits 6:6
    UINT32 D4EN                                    :  1;  // Bits 7:7
    UINT32 SPARE9_8                                :  2;  // Bits 9:8
    UINT32 D5EN                                    :  1;  // Bits 10:10
    UINT32 SPARE11                                 :  1;  // Bits 11:11
    UINT32 D9EN                                    :  1;  // Bits 12:12
    UINT32 D6F0EN                                  :  1;  // Bits 13:13
    UINT32 D14F0EN                                 :  1;  // Bits 14:14
    UINT32 D8EN                                    :  1;  // Bits 15:15
    UINT32 SPARE16                                 :  1;  // Bits 16:16
    UINT32 D10EN                                   :  1;  // Bits 17:17
    UINT32 D6F1EN                                  :  1;  // Bits 18:18
    UINT32 Reserved                                :  13;  // Bits 31:19
  } Bits;
  UINT32 Data;
  UINT16 Data16[2];
  UINT8  Data8[4];
} DEVEN_0_0_0_PCI_STRUCT;
typedef union {
  struct {
    UINT32 PCME                                    :  1;  // Bits 0:0
    UINT32 PAVPE                                   :  1;  // Bits 1:1
    UINT32 PAVPLCK                                 :  1;  // Bits 2:2
    UINT32 HVYMODSEL                               :  1;  // Bits 3:3
    UINT32 OVTATTACK                               :  1;  // Bits 4:4
    UINT32 RSVD1                                   :  1;  // Bits 5:5
    UINT32 ASMFEN                                  :  1;  // Bits 6:6
    UINT32 RSVD2                                   :  13;  // Bits 19:7
    UINT32 PCMBASE                                 :  12;  // Bits 31:20
  } Bits;
  UINT32 Data;
  UINT16 Data16[2];
  UINT8  Data8[4];
} PAVPC_0_0_0_PCI_STRUCT;
typedef union {
  struct {
    UINT32 LOCK                                    :  1;  // Bits 0:0
    UINT32 PRS                                     :  1;  // Bits 1:1
    UINT32 EPM                                     :  1;  // Bits 2:2
    UINT32 RSVD                                    :  1;  // Bits 3:3
    UINT32 DPRSIZE                                 :  8;  // Bits 11:4
    UINT32                                         :  8;  // Bits 19:12
    UINT32 TopOfDPR                                :  12;  // Bits 31:20
  } Bits;
  UINT32 Data;
  UINT16 Data16[2];
  UINT8  Data8[4];
} DPR_0_0_0_PCI_STRUCT;
typedef union {
  struct {
    UINT32 PCIEXBAREN                              :  1;  // Bits 0:0
    UINT32 Length                                  :  3;  // Bits 3:1
    UINT32                                         :  22;  // Bits 25:4
    UINT32 ADMSK64                                 :  1;  // Bits 26:26
    UINT32 ADMSK128                                :  1;  // Bits 27:27
    UINT32 ADMSK256                                :  1;  // Bits 28:28
    UINT32 ADMSK512                                :  1;  // Bits 29:29
    UINT32 ADMSK1024                               :  1;  // Bits 30:30
    UINT32 PCIEXBARLow                             :  1;  // Bits 31:31
    UINT32 PCIEXBARHigh                            :  10;  // Bits 41:32
  } Bits;
  UINT64 Data;
  UINT32 Data32[2];
  UINT16 Data16[4];
  UINT8  Data8[8];
} PCIEXBAR_0_0_0_PCI_STRUCT;
typedef union {
  struct {
    UINT32 DMIBAREN                                :  1;  // Bits 0:0
    UINT32                                         :  11;  // Bits 11:1
    UINT32 DMIBARLow                               :  20;  // Bits 31:12
    UINT32 DMIBARHigh                              :  10;  // Bits 41:32
  } Bits;
  UINT64 Data;
  UINT32 Data32[2];
  UINT16 Data16[4];
  UINT8  Data8[8];
} DMIBAR_0_0_0_PCI_STRUCT;
typedef union {
  struct {
    UINT32 PCIE0SEG                                :  4;  // Bits 3:0
    UINT32 PCIE1SEG                                :  4;  // Bits 7:4
    UINT32 PCIE2SEG                                :  4;  // Bits 11:8
    UINT32 PCIE3SEG                                :  4;  // Bits 15:12
    UINT32 PCIE4SEG                                :  4;  // Bits 19:16
    UINT32 PCIE5SEG                                :  4;  // Bits 23:20
    UINT32 PCIE6SEG                                :  4;  // Bits 27:24
    UINT32 PCIE7SEG                                :  4;  // Bits 31:28
  } Bits;
  UINT32 Data;
  UINT16 Data16[2];
  UINT8  Data8[4];
} PCIESEGMAP_0_0_0_PCI_STRUCT;
typedef union {
  struct {
    UINT32 PCIE0FUNC                               :  3;  // Bits 2:0
    UINT32 PCIE1FUNC                               :  3;  // Bits 5:3
    UINT32 PCIE2FUNC                               :  3;  // Bits 8:6
    UINT32 PCIE3FUNC                               :  3;  // Bits 11:9
    UINT32 PCIE4FUNC                               :  3;  // Bits 14:12
    UINT32 PCIE5FUNC                               :  3;  // Bits 17:15
    UINT32 PCIE6FUNC                               :  3;  // Bits 20:18
    UINT32 PCIE7FUNC                               :  3;  // Bits 23:21
    UINT32                                         :  8;  // Bits 31:24
  } Bits;
  UINT32 Data;
  UINT16 Data16[2];
  UINT8  Data8[4];
} PCIEFUNCMAP_0_0_0_PCI_STRUCT;
typedef union {
  struct {
    UINT8  LOCK                                    :  1;  // Bits 0:0
    UINT8                                          :  3;  // Bits 3:1
    UINT8  HIENABLE                                :  2;  // Bits 5:4
    UINT8                                          :  2;  // Bits 7:6
  } Bits;
  UINT8  Data;
} PAM0_0_0_0_PCI_STRUCT;
typedef union {
  struct {
    UINT8  LOENABLE                                :  2;  // Bits 1:0
    UINT8                                          :  2;  // Bits 3:2
    UINT8  HIENABLE                                :  2;  // Bits 5:4
    UINT8                                          :  2;  // Bits 7:6
  } Bits;
  UINT8  Data;
} PAM1_0_0_0_PCI_STRUCT;

typedef PAM1_0_0_0_PCI_STRUCT PAM2_0_0_0_PCI_STRUCT;

typedef PAM1_0_0_0_PCI_STRUCT PAM3_0_0_0_PCI_STRUCT;

typedef PAM1_0_0_0_PCI_STRUCT PAM4_0_0_0_PCI_STRUCT;

typedef PAM1_0_0_0_PCI_STRUCT PAM5_0_0_0_PCI_STRUCT;

typedef PAM1_0_0_0_PCI_STRUCT PAM6_0_0_0_PCI_STRUCT;
typedef union {
  struct {
    UINT8  MDAP10                                  :  1;  // Bits 0:0
    UINT8  MDAP11                                  :  1;  // Bits 1:1
    UINT8  MDAP12                                  :  1;  // Bits 2:2
    UINT8  MDAP60                                  :  1;  // Bits 3:3
    UINT8  MDAPCIe                                 :  1;  // Bits 4:4
    UINT8                                          :  2;  // Bits 6:5
    UINT8  HEN                                     :  1;  // Bits 7:7
  } Bits;
  UINT8  Data;
} LAC_0_0_0_PCI_STRUCT;
typedef union {
  struct {
    UINT32 LOCK                                    :  1;  // Bits 0:0
    UINT32                                         :  19;  // Bits 19:1
    UINT32 TOMLow                                  :  12;  // Bits 31:20
    UINT32 TOMHigh                                 :  10;  // Bits 41:32
  } Bits;
  UINT64 Data;
  UINT32 Data32[2];
  UINT16 Data16[4];
  UINT8  Data8[8];
} TOM_0_0_0_PCI_STRUCT;
typedef union {
  struct {
    UINT32 LOCK                                    :  1;  // Bits 0:0
    UINT32                                         :  19;  // Bits 19:1
    UINT32 TOUUDLow                                :  12;  // Bits 31:20
    UINT32 TOUUDHigh                               :  10;  // Bits 41:32
  } Bits;
  UINT64 Data;
  UINT32 Data32[2];
  UINT16 Data16[4];
  UINT8  Data8[8];
} TOUUD_0_0_0_PCI_STRUCT;
typedef union {
  struct {
    UINT32 LOCK                                    :  1;  // Bits 0:0
    UINT32                                         :  19;  // Bits 19:1
    UINT32 BDSM                                    :  12;  // Bits 31:20
  } Bits;
  UINT32 Data;
  UINT16 Data16[2];
  UINT8  Data8[4];
} BDSM_0_0_0_PCI_STRUCT;
typedef union {
  struct {
    UINT32 LOCK                                    :  1;  // Bits 0:0
    UINT32                                         :  19;  // Bits 19:1
    UINT32 BGSM                                    :  12;  // Bits 31:20
  } Bits;
  UINT32 Data;
  UINT16 Data16[2];
  UINT8  Data8[4];
} BGSM_0_0_0_PCI_STRUCT;
typedef union {
  struct {
    UINT32 LOCK                                    :  1;  // Bits 0:0
    UINT32                                         :  19;  // Bits 19:1
    UINT32 TSEGMB                                  :  12;  // Bits 31:20
  } Bits;
  UINT32 Data;
  UINT16 Data16[2];
  UINT8  Data8[4];
} TSEGMB_0_0_0_PCI_STRUCT;
typedef union {
  struct {
    UINT32 LOCK                                    :  1;  // Bits 0:0
    UINT32                                         :  19;  // Bits 19:1
    UINT32 TOLUD                                   :  12;  // Bits 31:20
  } Bits;
  UINT32 Data;
  UINT16 Data16[2];
  UINT8  Data8[4];
} TOLUD_0_0_0_PCI_STRUCT;
typedef union {
  struct {
    UINT32 DID2OVR                                 :  8;  // Bits 7:0
    UINT32 OE2                                     :  1;  // Bits 8:8
    UINT32                                         :  7;  // Bits 15:9
    UINT32 DID0OVR                                 :  8;  // Bits 23:16
    UINT32 OE0                                     :  1;  // Bits 24:24
    UINT32                                         :  7;  // Bits 31:25
  } Bits;
  UINT32 Data;
  UINT16 Data16[2];
  UINT8  Data8[4];
} DIDOVR_0_0_0_PCI_STRUCT;
typedef union {
  struct {
    UINT32 MC0_DSERR                               :  1;  // Bits 0:0
    UINT32 MC0_DMERR                               :  1;  // Bits 1:1
    UINT32 FMITHERMERR                             :  1;  // Bits 2:2
    UINT32 FMIAN                                   :  1;  // Bits 3:3
    UINT32 FMCA                                    :  1;  // Bits 4:4
    UINT32 FMUR                                    :  1;  // Bits 5:5
    UINT32 IBECC_COR                               :  1;  // Bits 6:6
    UINT32 IBECC_UC                                :  1;  // Bits 7:7
    UINT32 MC1_DSERR                               :  1;  // Bits 8:8
    UINT32 MC1_DMERR                               :  1;  // Bits 9:9
    UINT32 MC0_DDR5_CRC                            :  1;  // Bits 10:10
    UINT32 MC1_DDR5_CRC                            :  1;  // Bits 11:11
    UINT32                                         :  20;  // Bits 31:12
  } Bits;
  UINT32 Data;
  UINT16 Data16[2];
  UINT8  Data8[4];
} ERRSTS_0_0_0_PCI_STRUCT;

typedef ERRSTS_0_0_0_PCI_STRUCT ERRCMD_0_0_0_PCI_STRUCT;
typedef union {
  struct {
    UINT32 MC0_DSESMI                              :  1;  // Bits 0:0
    UINT32 MC0_DMESMI                              :  1;  // Bits 1:1
    UINT32 FMITHERMERR                             :  1;  // Bits 2:2
    UINT32 FMIAN                                   :  1;  // Bits 3:3
    UINT32 FMCA                                    :  1;  // Bits 4:4
    UINT32 FMUR                                    :  1;  // Bits 5:5
    UINT32 IBECC_COR                               :  1;  // Bits 6:6
    UINT32 IBECC_UC                                :  1;  // Bits 7:7
    UINT32 MC1_DSESMI                              :  1;  // Bits 8:8
    UINT32 MC1_DMESMI                              :  1;  // Bits 9:9
    UINT32 MC0_DDR5_CRC                            :  1;  // Bits 10:10
    UINT32 MC1_DDR5_CRC                            :  1;  // Bits 11:11
    UINT32                                         :  20;  // Bits 31:12
  } Bits;
  UINT32 Data;
  UINT16 Data16[2];
  UINT8  Data8[4];
} SMICMD_0_0_0_PCI_STRUCT;
typedef union {
  struct {
    UINT32 MC0_DSESCI                              :  1;  // Bits 0:0
    UINT32 MC0_DMESCI                              :  1;  // Bits 1:1
    UINT32 FMITHERMERR                             :  1;  // Bits 2:2
    UINT32 FMIAN                                   :  1;  // Bits 3:3
    UINT32 FMCA                                    :  1;  // Bits 4:4
    UINT32 FMUR                                    :  1;  // Bits 5:5
    UINT32 IBECC_COR                               :  1;  // Bits 6:6
    UINT32 IBECC_UC                                :  1;  // Bits 7:7
    UINT32 MC1_DSESCI                              :  1;  // Bits 8:8
    UINT32 MC1_DMESCI                              :  1;  // Bits 9:9
    UINT32 MC0_DDR5_CRC                            :  1;  // Bits 10:10
    UINT32 MC1_DDR5_CRC                            :  1;  // Bits 11:11
    UINT32                                         :  20;  // Bits 31:12
  } Bits;
  UINT32 Data;
  UINT16 Data16[2];
  UINT8  Data8[4];
} SCICMD_0_0_0_PCI_STRUCT;
typedef union {
  struct {
    UINT32 SKPD                                    :  32;  // Bits 31:0
  } Bits;
  UINT32 Data;
  UINT16 Data16[2];
  UINT8  Data8[4];
} SKPD_0_0_0_PCI_STRUCT;
typedef union {
  struct {
    UINT32 CAP_ID                                  :  8;  // Bits 7:0
    UINT32 NEXT_CAP                                :  8;  // Bits 15:8
    UINT32                                         :  16;  // Bits 31:16
  } Bits;
  UINT32 Data;
  UINT16 Data16[2];
  UINT8  Data8[4];
} CAPID0_0_0_0_PCI_STRUCT;
typedef union {
  struct {
    UINT32 CAPIDLEN                                :  8;  // Bits 7:0
    UINT32 CAPID_VER                               :  4;  // Bits 11:8
    UINT32                                         :  20;  // Bits 31:12
  } Bits;
  UINT32 Data;
  UINT16 Data16[2];
  UINT8  Data8[4];
} CAPCTRL0_0_0_0_PCI_STRUCT;
typedef union {
  struct {
    UINT32 SPARE_0                                 :  1;  // Bits 0:0
    UINT32 NVME_F7D                                :  1;  // Bits 1:1
    UINT32 SPARE_2                                 :  1;  // Bits 2:2
    UINT32 DDR_OVERCLOCK                           :  1;  // Bits 3:3
    UINT32 CRID                                    :  4;  // Bits 7:4
    UINT32 SXP_2LM_SUPPORTED                       :  1;  // Bits 8:8
    UINT32 SPARE_9                                 :  1;  // Bits 9:9
    UINT32 DID0OE                                  :  1;  // Bits 10:10
    UINT32 IGD                                     :  1;  // Bits 11:11
    UINT32 PDCD                                    :  1;  // Bits 12:12
    UINT32 X2APIC_EN                               :  1;  // Bits 13:13
    UINT32 DDPCD                                   :  1;  // Bits 14:14
    UINT32 CDD                                     :  1;  // Bits 15:15
    UINT32 FUFRD                                   :  1;  // Bits 16:16
    UINT32 D1NM                                    :  1;  // Bits 17:17
    UINT32 PEG60D                                  :  1;  // Bits 18:18
    UINT32 DDRSZ                                   :  2;  // Bits 20:19
    UINT32 Reserved                                :  1;  // Bits 21:21
    UINT32 DMIG2DIS                                :  1;  // Bits 22:22
    UINT32 VTDD                                    :  1;  // Bits 23:23
    UINT32 FDEE                                    :  1;  // Bits 24:24
    UINT32 ECCDIS                                  :  1;  // Bits 25:25
    UINT32 DW                                      :  1;  // Bits 26:26
    UINT32 PELWUD                                  :  1;  // Bits 27:27
    UINT32 PEG10D                                  :  1;  // Bits 28:28
    UINT32 PEG11D                                  :  1;  // Bits 29:29
    UINT32 PEG12D                                  :  1;  // Bits 30:30
    UINT32 NVME_F0D                                :  1;  // Bits 31:31
  } Bits;
  UINT32 Data;
  UINT16 Data16[2];
  UINT8  Data8[4];
} CAPID0_A_0_0_0_PCI_STRUCT;
typedef union {
  struct {
    UINT32 SPEGFX1                                 :  1;  // Bits 0:0
    UINT32 DPEGFX1                                 :  1;  // Bits 1:1
    UINT32 VMD_DIS                                 :  1;  // Bits 2:2
    UINT32 SH_OPI_EN                               :  1;  // Bits 3:3
    UINT32 SPARE_4                                 :  1;  // Bits 4:4
    UINT32 SPARE_5                                 :  1;  // Bits 5:5
    UINT32 SPARE_6                                 :  1;  // Bits 6:6
    UINT32 DDD                                     :  1;  // Bits 7:7
    UINT32 GMM_DIS                                 :  1;  // Bits 8:8
    UINT32 SPARE_9                                 :  1;  // Bits 9:9
    UINT32 DEV10_DISABLED                          :  1;  // Bits 10:10
    UINT32 HDCPD                                   :  1;  // Bits 11:11
    UINT32 LTECH                                   :  3;  // Bits 14:12
    UINT32 DMIG3DIS                                :  1;  // Bits 15:15
    UINT32 PEGX16D                                 :  1;  // Bits 16:16
    UINT32 ADDGFXCAP                               :  1;  // Bits 17:17
    UINT32 ADDGFXEN                                :  1;  // Bits 18:18
    UINT32 PKGTYP                                  :  1;  // Bits 19:19
    UINT32 PEGG3_DIS                               :  1;  // Bits 20:20
    UINT32 PLL_REF100_CFG                          :  3;  // Bits 23:21
    UINT32 SVM_DISABLE                             :  1;  // Bits 24:24
    UINT32 CACHESZ                                 :  3;  // Bits 27:25
    UINT32 SMT                                     :  1;  // Bits 28:28
    UINT32 OC_ENABLED                              :  1;  // Bits 29:29
    UINT32 NPK_DIS                                 :  1;  // Bits 30:30
    UINT32 IMGU_DIS                                :  1;  // Bits 31:31
  } Bits;
  UINT32 Data;
  UINT16 Data16[2];
  UINT8  Data8[4];
} CAPID0_B_0_0_0_PCI_STRUCT;
typedef union {
  struct {
    UINT32 Reserved                                :  5;  // Bits 4:0
    UINT32 DISPLAY_PIPE3                           :  1;  // Bits 5:5
    UINT32 IDD                                     :  1;  // Bits 6:6
    UINT32 BCLKOCRANGE                             :  2;  // Bits 8:7
    UINT32 SE_DIS                                  :  1;  // Bits 9:9
    UINT32 FDSKUFP                                 :  4;  // Bits 13:10
    UINT32 QCLK_GV_DIS                             :  1;  // Bits 14:14
    UINT32 SPARE_15                                :  1;  // Bits 15:15
    UINT32 LPDDR4_EN                               :  1;  // Bits 16:16
    UINT32 MAX_DATA_RATE_LPDDR4                    :  5;  // Bits 21:17
    UINT32 DDR4_EN                                 :  1;  // Bits 22:22
    UINT32 MAX_DATA_RATE_DDR4                      :  5;  // Bits 27:23
    UINT32 PEGG4_DIS                               :  1;  // Bits 28:28
    UINT32 PEGG5_DIS                               :  1;  // Bits 29:29
    UINT32 PEG61D                                  :  1;  // Bits 30:30
    UINT32 SPARE_31                                :  1;  // Bits 31:31
  } Bits;
  UINT32 Data;
  UINT16 Data16[2];
  UINT8  Data8[4];
} CAPID0_C_0_0_0_PCI_STRUCT;
typedef union {
  struct {
    UINT32 LPDDR5_EN                               :  1;  // Bits 0:0
    UINT32 MAX_DATA_RATE_LPDDR5                    :  5;  // Bits 5:1
    UINT32 DDR5_EN                                 :  1;  // Bits 6:6
    UINT32 MAX_DATA_RATE_DDR5                      :  5;  // Bits 11:7
    UINT32 IBECC_DIS                               :  1;  // Bits 12:12
    UINT32 VDDQ_VOLTAGE_MAX                        :  11;  // Bits 23:13
    UINT32 CRASHLOG_DIS                            :  1;  // Bits 24:24
    UINT32 SPARE                                   :  7;  // Bits 31:25
  } Bits;
  UINT32 Data;
  UINT16 Data16[2];
  UINT8  Data8[4];
} CAPID0_E_0_0_0_PCI_STRUCT;
typedef union {
  struct {
    UINT32 PROC                                    :  8;  // Bits 7:0
    UINT32 FOUND                                   :  8;  // Bits 15:8
    UINT32 MSTEP                                   :  8;  // Bits 23:16
    UINT32 DOT                                     :  4;  // Bits 27:24
    UINT32                                         :  4;  // Bits 31:28
  } Bits;
  UINT32 Data;
  UINT16 Data16[2];
  UINT8  Data8[4];
} MID_0_0_0_PCI_STRUCT;
typedef union {
  struct {
    UINT32 RSVD_S0                                 :  3;  // Bits 2:0
    UINT32                                         :  1;  // Bits 3:3
    UINT32 RSVD_S1                                 :  3;  // Bits 6:4
    UINT32                                         :  1;  // Bits 7:7
    UINT32 RSVD_S2                                 :  3;  // Bits 10:8
    UINT32                                         :  1;  // Bits 11:11
    UINT32 RSVD_S3                                 :  3;  // Bits 14:12
    UINT32                                         :  1;  // Bits 15:15
    UINT32 RSVD_S4                                 :  3;  // Bits 18:16
    UINT32                                         :  1;  // Bits 19:19
    UINT32 RSVD_RW                                 :  12;  // Bits 31:20
  } Bits;
  UINT32 Data;
  UINT16 Data16[2];
  UINT8  Data8[4];
} DEBUP0_0_0_0_PCI_STRUCT;
typedef union {
  struct {
    UINT32 RSVD0                                   :  3;  // Bits 2:0
    UINT32 RSVD1                                   :  1;  // Bits 3:3
    UINT32 RSVD2                                   :  3;  // Bits 6:4
    UINT32 RSVD3                                   :  1;  // Bits 7:7
    UINT32 RSVD4                                   :  3;  // Bits 10:8
    UINT32 RSVD5                                   :  1;  // Bits 11:11
    UINT32 RSVD6                                   :  3;  // Bits 14:12
    UINT32 RSVD7                                   :  1;  // Bits 15:15
    UINT32 RSVD8                                   :  3;  // Bits 18:16
    UINT32 RSVD9                                   :  1;  // Bits 19:19
    UINT32 RSVD10                                  :  12;  // Bits 31:20
  } Bits;
  UINT32 Data;
  UINT16 Data16[2];
  UINT8  Data8[4];
} DEBUP1_0_0_0_PCI_STRUCT;
typedef union {
  struct {
    UINT32 PEG10FUNC                               :  3;  // Bits 2:0
    UINT32 PEG11FUNC                               :  3;  // Bits 5:3
    UINT32 PEG12FUNC                               :  3;  // Bits 8:6
    UINT32 PEG60FUNC                               :  3;  // Bits 11:9
    UINT32                                         :  20;  // Bits 31:12
  } Bits;
  UINT32 Data;
  UINT16 Data16[2];
  UINT8  Data8[4];
} PEGFUNCMAP_0_0_0_PCI_STRUCT;

#pragma pack(pop)
#endif
#ifndef __Pci000Adl0xxx_h__
#define __Pci000Adl0xxx_h__
/** @file
  This file was automatically generated. Modify at your own risk.
  Note that no error checking is done in these functions so ensure that the correct values are passed.

@copyright
  Copyright (c) 2010 - 2020 Intel Corporation. All rights reserved
  This software and associated documentation (if any) is furnished
  under a license and may only be used or copied in accordance
  with the terms of the license. Except as permitted by the
  license, no part of this software or documentation may be
  reproduced, stored in a retrieval system, or transmitted in any
  form or by any means without the express written consent of
  Intel Corporation.
  This file contains an 'Intel Peripheral Driver' and is uniquely
  identified as "Intel Reference Module" and is licensed for Intel
  CPUs and chipsets under the terms of your license agreement with
  Intel or your vendor. This file may be modified by the user, subject
  to additional terms of the license agreement.

@par Specification Reference:
**/

#pragma pack(push, 1)


#define PAM0_0_0_0_PCI_REG                                             (0x00000080)


#define PAM4_0_0_0_PCI_REG                                             (0x00000084)


#pragma pack(pop)
#endif
