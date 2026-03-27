/** @file
  TCSS PEI PreMem policy

  @copyright
  INTEL CONFIDENTIAL
  Copyright (C) 2024 Intel Corporation.

  This software and the related documents are Intel copyrighted materials,
  and your use of them is governed by the express license under which they
  were provided to you ("License"). Unless the License provides otherwise,
  you may not use, modify, copy, publish, distribute, disclose or transmit
  this software or the related documents without Intel's prior written
  permission.

  This software and the related documents are provided as is, with no
  express or implied warranties, other than those that are expressly stated
  in the License.

@par Specification Reference:
**/
#ifndef _TCSS_PEI_PREMEM_CONFIG_VER_1_H_
#define _TCSS_PEI_PREMEM_CONFIG_VER_1_H_

#include <ConfigBlock.h>
#include <TcssInfo.h>

//extern EFI_GUID gTcssPeiPreMemConfigGuid;

#define TCSS_PEI_PREMEM_CONFIG_REVISION 3
#define MAX_IOM_AUX_BIAS_COUNT          4
#define IOM_DP                          0x1
#define IOM_HDMI                        0x2
#define IOM_EDP                         0x3

#pragma pack (push,1)

/// TCSS USBC SI Policy capability
#define _TCSS_PORT_CAP
typedef enum {
  UsbCDisable   = 0,   ///< The USBC lane function disabled.
  DpOnly        = 1,   ///< The USBC lane supports Display and Display tunneling.
  NoThunderbolt = 2,   ///< The USBC lane supports Display, native USB3.
  NoPcie        = 3,   ///< The USBC lane supports Display, native USB3, Display tunneling and USB3 tunneling.
  FullFunction  = 7,   ///< The USBC lane supports Display, native USB3, Display tunneling, USB3 tunneling and PCIe tunneling.
} TCSS_PORT_CAP;

#define IS_TC_PORT_TUNNELING_SUPPORTED(PortCap) \
        ( (((UINT8)PortCap > UsbCDisable) && ((UINT8)PortCap != NoThunderbolt)) ? TRUE : FALSE )
#define IS_TC_PORT_USB_SUPPORTED(PortCap) \
        ( ((UINT8)PortCap > DpOnly) ? TRUE : FALSE )
#define IS_TC_PORT_USB4_SUPPORTED(PortCap) \
        ( ((UINT8)PortCap == FullFunction) ? TRUE : FALSE )
#define IS_TC_XHCI_NEEDED(PortCap) \
        ( ((UINT8)PortCap > UsbCDisable) ? TRUE : FALSE )
#define TCSS_DEVEN_DMA_EN(PortIndex) \
        ( (BIT10 << ((UINT8)PortIndex / 2)) & 0xC00 )
#define TCSS_DEVEN_PCIE_EN(PortIndex) \
        ( (BIT0 << ((UINT8)PortIndex)) & 0x0F )
#define IS_TCSS_SETUP_PCIE_EN(PortCap) \
        ( ((UINT8)PortCap == FullFunction) ? TRUE : FALSE )
#define IS_TCSS_SETUP_DMA_EN(PortCap) \
        ( (((UINT8)PortCap != UsbCDisable) && ((UINT8)PortCap != NoThunderbolt)) ? TRUE : FALSE )

///
/// The TCSS_USBTC_PEI_PERMEM_CONFIG block describes IOM settings for TCSS.
///
typedef union {
  UINT32      PortEnData32;
  struct {
    UINT8 CapPolicy[MAX_ITBT_PCIE_PORT];
  } PortIndex;
} TCSS_USBTC_PEI_PERMEM_CONFIG;

///
/// The IOM_AUX_ORI_GPIO_PAD_CONFIG describes IOM TypeC port map GPIO pin.
/// Those GPIO setting for DP Aux Orientation Bias Control when the TypeC port didn't have re-timer.
/// IOM needs to know SBU1 and SBU2 pins for Bias control
///
typedef struct {
  UINT32     ReceptacleSbu1BiasCtrl; ///< SBU1 Pin number that is for IOM indicate the pull up pin from TypeC port.
  UINT32     ReceptacleSbu2BiasCtrl; ///< SBU2 Pin number that is for IOM indicate the pull down pin from TypeC port.
  UINT32     AuxIsoCtrl;             ///< AUX ISO control pin GPIO number that is IOM indicate the AUX active from SOC.
} IOM_AUX_ORI_GPIO_PAD_CONFIG;

typedef struct {
  UINT16     AuxOri;     ///< Bits defining value for IOM Aux Orientation Register
  UINT16     HslOri;     ///< Bits defining value for IOM HSL Orientation Register
} TCSS_IOM_ORI_OVERRIDE;

///
/// The TCSS_IOM_PEI_PERMEM_CONFIG block describes IOM settings for TCSS.
///
typedef struct {
  IOM_AUX_ORI_GPIO_PAD_CONFIG   IomAuxPortPad[MAX_IOM_AUX_BIAS_COUNT];   ///< The RECEPTACLE_SBUx_BIAS_CTL port config setting.
  TCSS_IOM_ORI_OVERRIDE         IomOverrides;
  UINT8                         IomUsbCDpConfig[MAX_TCSS_USB3_PORTS];
} TCSS_IOM_PEI_PERMEM_CONFIG;

/**
  This configuration block describes TCSS settings.\n
  <b>Revision 1</b>:
  - Initial version.
  <b>Revision 2</b>:
  - Added TCSS_IOM_PEI_PERMEM_CONFIG
  <b>Revision 3</b>:
  - Added TcssPlatConf
**/
typedef struct {
  CONFIG_BLOCK_HEADER            Header;       ///< Offset 0-27 Config Block Header
  TCSS_USBTC_PEI_PERMEM_CONFIG   UsbTcConfig;  ///< USB Type C Port Configuration
  TCSS_IOM_PEI_PERMEM_CONFIG     IomConfig;    ///< The Iom PreMem Config
  UINT8                          TcssEnable;   ///< TCSS is enabled or not
  UINT8                          TcssPlatConf; ///< Platform configuration (retimer)
  UINT8                          Rsvd[2];      ///< Reserved for future use
} TCSS_PEI_PREMEM_CONFIG;

#pragma pack (pop)

/**
  Print TCSS_PEI_PREMEM_CONFIG and serial out.

  @param[in] TcssPeiPreMemConfig          Pointer to a TCSS_PEI_PREMEM_CONFIG
  @param[in] MaxTcssPortNumber            Max number of TCSS Root Ports
**/
VOID
TcssPreMemConfigDump (
  IN TCSS_PEI_PREMEM_CONFIG     *TcssPeiPreMemConfig,
  IN UINT8                      MaxTcssPortNumber
  );

#endif /* _TCSS_PEI_PREMEM_CONFIG_VER1_H_ */
