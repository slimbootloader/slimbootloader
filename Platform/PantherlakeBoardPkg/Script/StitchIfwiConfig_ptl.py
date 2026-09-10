## @ StitchIfwiConfig_ptl.py
#  This is an IFWI stitch config script for PTL/PTL-A Slim Bootloader
#
# Copyright (c) 2026, Intel Corporation. All rights reserved. <BR>
# SPDX-License-Identifier: BSD-2-Clause-Patent
#
##
import sys
import os
from StitchLoader import *

# Global variable to control TXT inclusion - can be overridden by check_parameter()
g_txt_enabled = False

extra_usage_txt = \
"""This is an IFWI stitch config script for Slim Bootloader For the FIT tool and
stitching ingredients listed in step 2 below, please contact your Intel representative.

  1. Create a stitching workspace directory. The paths mentioned below are all
     relative to it.

  2. Extract required tools and ingredients to stitching workspace.
     - FIT tool
       Copy 'fit.exe' or 'fit' and 'vsccommn.bin' to 'Fit' folder
     - BPMGEN2 Tool
       Copy the contents of the tool to Bpmgen2 folder
       Rename the bpmgen2 parameter to bpmgen2.params if its name is not this name.
      - Components
       Copy 'cse_image.bin'     to 'Input/cse_image.bin'
       Copy PMC firmware image  to 'Input/pmc.bin'.
       Copy EC firmware image   to 'Input/ec.bin'.
       copy ECregionpointer.bin to 'Input/ecregionpointer.bin'
       Copy GBE binary image    to 'Input/gbe.bin'.
       Copy ACM firmware image  to 'Input/acm.bin'.
       Copy DiagnosticACM firmware image  to 'Input/DiagnosticAcm.bin'.

  3. Openssl
       Openssl is required for stitch. the stitch tool will search evn OPENSSL_PATH,
       to find Openssl. If evn OPENSSL_PATH is not found, will find openssl from
       "C:\\Openssl\\Openssl"

  4. Stitch the final image
       EX:
       Assuming stitching workspace is at D:\Stitch and building ifwi for PTL/PTL-A platform
       To stitch IFWI with SPI QUAD mode and Boot Guard profile VM:
         StitchIfwi.py -b vm -p ptl -w D:\Stitch -s Stitch_Components.zip -c StitchIfwiConfig.py

"""

def get_bpmgen2_params_change_list ():
    global g_txt_enabled
    params_change_list = []
    # Base parameters
    base_params = [
      # variable                | value |
      # ===================================
      ('PlatformRules',         'MTL Client'),
      ('BpmStrutVersion',       '0x25'),
      ('BpmRevision',           '0x01'),
      ('BpmRevocation',         '1'),
      ('AcmRevocation',         '2'),
      ('NEMPages',              '3'),
      ('AcpiBase',              '0x1800'),
      ('IbbFlags',              '0x12'),
      ('IbbHashAlgID',          '0x0C:SHA384'),
      ('MCHBAR',                '0x00000000FEDC0000'),
      ('VTD_BAR',               '0x00000000FC800000'),
      ('TxtInclude',            'FALSE'),
      ('PcdInclude',            'TRUE'),
      ('BpmSigScheme',          '0x16:RSAPSS'),
      ('BpmSigPubKey',          r'BpmGen2/keys/bpm_pubkey_3072.pem'),
      ('BpmSigPrivKey',         r'BpmGen2/keys/bpm_privkey_3072.pem'),
      ('BpmKeySizeBits',        '3072'),
      ('BpmSigHashAlgID',       '0x0C:SHA384'),
      ('CnbsInclude',           'TRUE'),
      ]

    if g_txt_enabled == True:
        txt_params = [
            ('TxtInclude',            'TRUE'),
            ('PdSeconds',             '10'),
            ('PttCmosOffset0',        '0xFE'),
            ('PttCmosOffset1',        '0xFF'),
            ('CmosIndexOffset',       '255'),
            # PDRS fix: correct TpmByteOffset and TpmBitFieldWidth for TPM2.0 and PTT
            ('TPM2.0_LOCATION/TpmIndexHandle',      '0x01C10104'),
            ('TPM2.0_LOCATION/TpmByteOffset',        '4'),
            ('TPM2.0_LOCATION/TpmBitFieldWidth',     '0x04'),
            ('PTT_LOCATION/TpmIndexHandle',          '0x01C10104'),
            ('PTT_LOCATION/TpmByteOffset',           '4'),
            ('PTT_LOCATION/TpmBitFieldWidth',        '0x04'),
        ]
    else:
        txt_params = [
            ('TxtInclude',            'False'),
        ]

    # Combine all parameters
    all_params = base_params + txt_params
    params_change_list.append(all_params)
    return params_change_list

def get_platform_sku():
    platform_sku ={
        'ptl'    :   'Intel(R) PantherLake H Chipset - Corporate - SPI'
    }
    return platform_sku

def get_component_replace_list(plt_params_list):
    replace_list = [
       #    Path                   file name              compress    Key                           SVN
      ('IFWI/BIOS/TS0/ACM0',      'Input/acm0.bin',        'dummy',    '',                            0),
      ('IFWI/BIOS/TS1/ACM0',      'Input/acm0.bin',        'dummy',    '',                            0),
       ]

    if 'fusa' in plt_params_list:
      print("Replace the DACM")
      replace_list.extend ([
      ('IFWI/BIOS/TS0/DACM',      'Input/DiagnosticAcm.bin', 'dummy',      '',                        0),
      ('IFWI/BIOS/TS1/DACM',      'Input/DiagnosticAcm.bin', 'dummy',      '',                        0),
      ])
      if os.path.exists('FUFW/FusaStartupBist.fv'):
            print ("FFusaStartupBist.fv found")
            replace_list.extend ([
            ('IFWI/BIOS/NRD/FUFW/FSBP', 'FUFW/FusaStartupBist.fv',     'lz4',     'KEY_ID_CONTAINER_COMP_RSA3072', 0)   # FuSa Startup BIST Pattern file
            ])

    if 'posc' in plt_params_list:
        if os.path.exists('IPFW/PreOsChecker.bin'):
            print ("PreOsChecker.bin found")
            replace_list.append (
                ('IFWI/BIOS/NRD/IPFW/POSC', 'IPFW/PreOsChecker.bin',     'lz4',     'KEY_ID_CONTAINER_COMP_RSA3072', 0),   # Pre-OS Checker binar
            )

    return replace_list

def check_parameter(para_list):
    print (para_list)
    para_supported = {
        'debug'   : {},
        'rbs'     : {},
        'rvp1_lp5': {},
        'rvp3_lp5': {},
        'rvp_ddr5': {},
        'crb'     : {},
        '64MB'   : {},
        'posc'   : {},
        'fusa'   : {},
        'txt'    : {}
       }

    para_help = \
        """
        'debug'      -- Enable DAM and DCI configuration (Only use for debug purpose but not for final production!)
        'rbs'        -- Enable Robinson Lake Board configuration
        'rvp1_lp5'   -- Enable RVP1 LP5 CCG Board configuration
        'rvp3_lp5'   -- Enable RVP3 LP5 CCG Board configuration
        'rvp_ddr5'   -- Enable RVP DDR5 CCG Board configuration
        'crb'        -- Enable CRB Board configuration
        '64MB'       -- Build for 64MB component to allow space for FuSa BIST pattern data
        'posc'       -- Stitch POSC binary into BIOS region.
        'fusa'       -- Stitch Fusa DiagnosticAcm binary into BIOS region
        'txt'        -- Enable Intel TXT support.
        """
    for para in para_list:
        if para == '':
            print ("No platform specific parameter provided")
            return True
        if para not in para_supported:
            print ("NOT support platform parameter: %s" % para)
            print ("platform parameter list:\n")
            print (para_help)
            return False

        if para_list[para] != None:
            if para_list[para] not in para_supported[para]:
                print ("NOT support platform parameter: %s" %  para_list[para])
                print ("platform parameter list:", para_supported[para])
                return False
    return True

def get_oemkeymanifest_change_list():
    xml_change_list = []
    xml_change_list.append ([
        # Path                                                                            | value |
        # =========================================================================================
        ('./KeyManifestEntries/KeyManifestEntry/Usage',                                    'OemDebugManifest'),
        ('./KeyManifestEntries/KeyManifestEntry/HashBinary',                               'Temp/kmsigpubkey.hash'),
    ])

    return xml_change_list

def get_xml_change_list (platform, plt_params_list):
    xml_change_list = []
    xml_change_list.append ([
      # Path                                                                        | value |
      # =========================================================================================
      #Region Order
        ('./FlashLayout/BiosRegion/InputFile',                                       '$SourceDir\BiosRegion.bin'),
        ('./FlashLayout/DescriptorRegion/OemBinary',                                 '$SourceDir\OemBinary.bin'),
        ('./FlashLayout/Ifwi_IntelMePmcRegion/MeRegionFile',                         '$SourceDir\MeRegionFileCorp.bin'),
        ('./FlashLayout/Ifwi_IntelMePmcRegion/PmcBinary',                            '$SourceDir\PmcBinary.bin'),
        ('./FlashLayout/EcRegion/InputFile',                                         '$SourceDir\EcRegion.bin'),
        ('./FlashLayout/EcRegion/Enabled',                                           'Enabled'),
        ('./FlashLayout/EcRegion/EcRegionPointer',                                   '$SourceDir\EcRegionPointer.bin'),
        ('./FlashLayout/GbeRegion/InputFile',                                        '$SourceDir\GbeRegion.bin'),
        ('./FlashLayout/GbeRegion/Enabled',                                          'Enabled'),
        ('./FlashLayout/PdrRegion/InputFile',                                        ''),
        ('./FlashLayout/PseRegion/PseEnable',                                        'Disabled'),
        ('./FlashLayout/SubPartitions/IunitSubPartition/InputFile',                  '$SourceDir\IunitBinary.bin'),
        ('./FlashLayout/SubPartitions/SoccSubPartitionData/InputFile',               '$SourceDir\SoccBinary.bin'),
        ('./FlashLayout/SubPartitions/AceSubPartition/InputFile',                    '$SourceDir\AceBinary.bin'),
        ('./FlashLayout/SubPartitions/EsePackage/EseSubPartition/ESEBinary',         '$SourceDir\EseBinary.bin'),
        ('./FlashLayout/SubPartitions/EsePackage/PunitSubPartition/PUNITBinary',     '$SourceDir\PunitBinary.bin'),
        ('./FlashLayout/SubPartitions/EsePackage/AunitSubPartition/AUNITBinary',     '$SourceDir\AunitBinary.bin'),
        ('./FlashLayout/SubPartitions/EsePackage/CNViSubPartition/CNViBinary',       '$SourceDir\CNViBinary.bin'),
        ('./FlashSettings/BiosConfiguration/BiosRedAssistance',                      'Disabled'),
        ('./FlashLayout/SubPartitions/EsePackage/SocSphySubPartition/SocSphyBinary', '$SourceDir\SocSphyBinary.bin'),
        ('./PlatformProtection/PlatformIntegrity/OemPublicKeyHash',                  'F8 F0 E3 69 15 81 76 99 0A 54 9E D4 C3 6D 1A 86 39 D8 87 3D EF F7 ED 2D E3 4C B4 1B CC B3 04 76 CE 0A A0 63 BC 5B 7A AC FF D9 50 9E 96 40 C6 99'),
        ('./PlatformProtection/PlatformIntegrity/OemExtInputFile',                   '$SourceDir\OemExtInputFile.bin'),
        ('./PlatformProtection/TxtConfiguration/TxtSupported',                       'Yes'),
        ('./IntegratedSensorHub/IshImage/InputFile',                                 '$SourceDir\IshImage.bin'),
        ('./IntegratedSensorHub/IshData/PdtBinary',                                  '$SourceDir\PdtBinary.bin'),
        ('./FlexIO/Type-CSubsystemConfiguration/IomBinaryFile',                      '$SourceDir\IomBinary.bin'),
        ('./FlexIO/Type-CSubsystemConfiguration/PhyBinaryFile',                      '$SourceDir\PhyBinary.bin'),
        ('./FlexIO/Type-CSubsystemConfiguration/TbtBinaryFile',                      '$SourceDir\TbtBinary.bin'),
        ('./FlexIO/PowerDelivery_PdControllerConfiguration/CheckPdReady',            'Yes'),
        ('./NetworkingConnectivity/WiredLanConfiguration/LanEnable',                 'Yes'),
        ('./Icc/SocClkOutCfg/BUFF_EN_SRC0',                                          'enable'),
        ('./Icc/SocClkOutCfg/BUFF_EN_SRC1',                                          'enable'),
        ('./Icc/SocClkOutCfg/BUFF_EN_SRC2',                                          'enable'),
        ('./Icc/SocClkOutCfg/BUFF_EN_SRC3',                                          'enable'),
        ('./Icc/SocClkOutCfg/BUFF_EN_SRC4',                                          'enable'),
        ('./Icc/SocClkOutCfg/BUFF_EN_SRC5',                                          'enable'),
        ('./Icc/SocClkOutCfg/BUFF_EN_SRC6',                                          'enable'),
        ('./Icc/SocClkOutCfg/BUFF_EN_SRC7',                                          'disable'),
        ('./Icc/SocClkOutCfg/BUFF_EN_SRC8',                                          'disable'),
        ('./Icc/SocClkOutCfg/SRC_MUXSEL_CFG0',                                       'GPP_C_9(SRCCLKREQ0#)'),
        ('./Icc/SocClkOutCfg/SRC_MUXSEL_CFG1',                                       'GPP_C_10(SRCCLKREQ1#)'),
        ('./Icc/SocClkOutCfg/SRC_MUXSEL_CFG2',                                       'GPP_C_11(SRCCLKREQ2#)'),
        ('./Icc/SocClkOutCfg/SRC_MUXSEL_CFG3',                                       'GPP_C_12(SRCCLKREQ3#)'),
        ('./Icc/SocClkOutCfg/SRC_MUXSEL_CFG4',                                       'GPP_C_13(SRCCLKREQ4#)'),
        ('./Icc/SocClkOutCfg/SRC_MUXSEL_CFG5',                                       'GPP_C_14(SRCCLKREQ5#)'),
        ('./Icc/SocClkOutCfg/SRC_MUXSEL_CFG6',                                       'GPP_D_18(SRCCLKREQ6#)'),
        ('./Icc/SocClkOutCfg/SRC_MUXSEL_CFG7',                                       'GPP_D_20(SRCCLKREQ7#)'),
        ('./Icc/SocClkOutCfg/SRC_MUXSEL_CFG8',                                       'GPP_D_21(SRCCLKREQ8#)'),
        ('./Icc/SocClkOutCfg/SSC_en',                                                'enable'),
        ('./BrandIdentityAndEntitlements/BrandConfiguration/PlatformBrandIdentity',  'Intel vPro(R) Enterprise'),
        ('./BrandIdentityAndEntitlements/EntitlementsConfiguration/IcpsEntitlementUI','ICPS Entitlement'),
    ])

    if 'debug' in plt_params_list:
        print ("Applying changes to enable DAM and DCI")
        xml_change_list.append ([
            ('./Debug/DelayedAuthenticationModeConfiguration/DelayedAuthMode',                'Yes'),
            ('./Debug/IntelTraceHubTechnology/RomTracePreBiosEn',                             'Trace Hub Pre-BIOS Enabled'),
            ('./Debug/IntelMeFirmwareDebuggingOverrides/DbgOverrideProdSi',                   '0x7'),
            ('./Debug/DirectConnectInterfaceConfiguration/DciDbcEnable',                      'Yes'),
            ('./Debug/EarlyUsb2DbcOverType-AConfiguration/Usb2DbcPortEn',                     'USB2 Port 1'),
            ('./Debug/EarlyUsb2DbcOverType-AConfiguration/EnEarlyUsb2DbcCon',                 'Yes')
        ])

    if 'rbs' in plt_params_list:
        print ("Applying changes to enable Robinson Lake Board configuration")
        xml_change_list.append ([
            ('./FlashLayout/DescriptorRegion/HarnessGlobalData/SelectedRvp',                  'PTL-H LP5x T4 RVP-3 (PTP-H + PTL-H)'),
            ('./FlashLayout/EcRegion/Enabled',                                                'Disabled'),
            ('./FlashLayout/GbeRegion/Enabled',                                               'Disabled'),
            ('./FlashSettings/FlashConfiguration/FastReadClockFreq',                          '50MHz'),
            ('./FlashSettings/FlashConfiguration/ReadIdAndReadStatClkFreq',                   '50MHz'),
            ('./FlashSettings/FlashConfiguration/WriteEraseClockFreq',                        '50MHz'),
            ('./InternalPchBuses/PchTimerConfiguration/tPch45TimingConfig',                   '100ms'),
            ('./InternalPchBuses/EspiConfiguration/EspiEcBusfreq',                            '25MHz'),
            ('./InternalPchBuses/EspiConfiguration/EspiEcMaxIoMode',                          'Single and Dual'),
            ('./InternalPchBuses/EspiConfiguration/EspiEcSlv1DevBusfreq',                     '25MHz'),
            ('./InternalPchBuses/EspiConfiguration/EspiEcSlv1DevMaxIoMode',                   'Single and Duel'),
            ('./Power/PlatformPower/SuswarnSusackEn',                                         'No'),
            ('./IntegratedSensorHub/IntegratedSensorHub/IshPowerUpState',                     'Disabled'),
            ('./IntegratedSensorHub/IntegratedSensorHub/IshSupported',                        'No'),
            ('./Debug/DelayedAuthenticationModeConfiguration/DelayedAuthMode',                'No'),
            ('./Debug/IntelTraceHubTechnology/RomTracePreBiosEn',                             'Trace Hub Pre-BIOS Enabled'),
            ('./Debug/IntelMeFirmwareDebuggingOverrides/DbgOverrideProdSi',                   '0x0'),
            ('./CpuStraps/CpuStraps/PlatformImonDisable',                                     'Enabled'),
            ('./FlexIO/PciePortConfiguration/PCIeController1Config',                          '1x2, 2x1'),
            ('./FlexIO/PciePortConfiguration/PCIeController2Config',                          '4x1'),
            ('./FlexIO/Usb2PortConfiguration/USB2Prt3ConTypeSel',                             'Type A / Type C'),
            ('./FlexIO/Usb2PortConfiguration/USB2Prt4ConTypeSel',                             'Type A / Type C'),
            ('./FlexIO/Type-CSubsystemConfiguration/TboltPort12Retimer',                      'No Retimers'),
            ('./FlexIO/Type-CSubsystemConfiguration/TboltPort34Retimer',                      'No Retimers'),
            ('./FlexIO/Type-CSubsystemConfiguration/TypeCPort1Config',                        'USB and ALT-DP only'),
            ('./FlexIO/Type-CSubsystemConfiguration/TypeCPort2Config',                        'HDMI Only'),
            ('./FlexIO/Type-CSubsystemConfiguration/TypeCPort3Config',                        'USB and ALT-DP only'),
            ('./FlexIO/Type-CSubsystemConfiguration/TypeCPort4Config',                        'USB4 40G'),
            ('./FlexIO/PowerDelivery_PdControllerConfiguration/TypeCPort1SmbusAddr',          '0x20'),
            ('./FlexIO/PowerDelivery_PdControllerConfiguration/TypeCPort3SMBusAddr',          '0x24'),
            ('./FlexIO/PowerDelivery_PdControllerConfiguration/TypeCPort2Mode',               'No'),
            ('./FlexIO/PowerDelivery_PdControllerConfiguration/USB2PortForTypeCPort3',        'USB2 Port 2'),
            ('./BrandIdentityAndEntitlements/BrandConfiguration/PlatformBrandIdentity',       'Intel(R) Based PC'),
            ('./BrandIdentityAndEntitlements/EntitlementsConfiguration/IcpsEntitlementUI',    'None'),
            ('./Icc/SocClkOutCfg/BUFF_EN_SRC7',                                               'enable'),
            ('./InternalPchBuses/SmbusSmlinkConfiguration/SLink0MctpAddress',                 '0x62'),
            ('./InternalPchBuses/SmbusSmlinkConfiguration/SLink0MctpEnable',                  'Yes'),
            ('./InternalPchBuses/SmbusSmlinkConfiguration/SLink0I2cAddress',                  '0x74'),
            ('./InternalPchBuses/SmbusSmlinkConfiguration/SMLink0I2cEnable',                  'Yes'),
            ('./NetworkingConnectivity/WiredLanConfiguration/GbeMacSmbAddrsEn',               'No'),
            ('./NetworkingConnectivity/WiredLanConfiguration/LanEnable',                      'No'),
        ])

    if 'rvp1_lp5' in plt_params_list:
        print ("Applying changes to enable RVP LP5 Board configuration")
        xml_change_list.append ([
            ('./FlashLayout/DescriptorRegion/HarnessGlobalData/SelectedRvp',                  'PTL-H LP5x T3 RVP-1 (PTP-H + PTL-H)'),
            ('./FlashLayout/GbeRegion/Enabled',                                               'Enabled'),
            ('./FlashLayout/SubPartitions/EsePackage/PunitSubPartition/PunitGresetTimerEn',   'No'),
            ('./FlashSettings/FlashConfiguration/SpiDualIoReadEnable',                        'No'),
            ('./FlashSettings/FlashConfiguration/SpiDualOutReadEnable',                       'No'),
            ('./PlatformProtection/PlatformIntegrity/OemExtInputFile',                        '$SourceDir\LNL_OEMKeyManifest.bin'),
            ('./NetworkingConnectivity/WiredLanConfiguration/PhyConnected',                   'PHY on SMLink0'),
            ('./NetworkingConnectivity/WiredLanConfiguration/LanPhyPwrCtrlGppv10Config',      'Enable as LANPHYPC'),
            ('./NetworkingConnectivity/WiredLanConfiguration/SlpLanGppv11Config',             'Enable as SLP_LAN#'),
            ('./CpuStraps/CpuStraps/IaVrOffsetVid',                                           'Yes'),
            ('./CpuStraps/CpuStraps/PlatformImonDisable',                                     'Enabled'),
            ('./FlexIO/Type-CSubsystemConfiguration/TboltPort34Retimer',                      'Ports 3 and 4 Retimer Enabled'),
            ('./FlexIO/Type-CSubsystemConfiguration/TypeCPort4Config',                        'HDMI Only'),
            ('./FlexIO/PciePortConfiguration/PCIeController1Config',                          '4x1'),
            ('./FlexIO/PowerDelivery_PdControllerConfiguration/TypeCPort4Mode',               'No'),
            ('./InternalPchBuses/PchTimerConfiguration/tPch45TimingConfig',                   '100ms'),
            ('./Power/PlatformPower/SlpS5Gppv09Config',                                       'Enable as SLP_S5#'),
            ('./Power/PlatformPower/SusclkGppv07Config',                                      'Enable as SUSCLK'),
        ])

    if 'rvp3_lp5' in plt_params_list:
        print ("Applying changes to enable RVP LP5 Board configuration")
        xml_change_list.append ([
            ('./FlashLayout/DescriptorRegion/HarnessGlobalData/SelectedRvp',                  'PTL-H LP5x T4 RVP-3 (PTP-H + PTL-H)'),
            ('./FlashSettings/FlashConfiguration/SpiDualIoReadEnable',                        'No'),
            ('./FlashSettings/FlashConfiguration/SpiDualOutReadEnable',                       'No'),
            ('./PlatformProtection/PlatformIntegrity/OemExtInputFile',                        '$SourceDir\LNL_OEMKeyManifest.bin'),
            ('./NetworkingConnectivity/WiredLanConfiguration/PhyConnected',                   'PHY on SMLink0'),
            ('./NetworkingConnectivity/WiredLanConfiguration/LanPhyPwrCtrlGppv10Config',      'Enable as LANPHYPC'),
            ('./NetworkingConnectivity/WiredLanConfiguration/SlpLanGppv11Config',             'Enable as SLP_LAN#'),
            ('./CpuStraps/CpuStraps/IaVrOffsetVid',                                           'Yes'),
            ('./CpuStraps/CpuStraps/PlatformImonDisable',                                     'Enabled'),
            ('./FlexIO/Type-CSubsystemConfiguration/TboltPort34Retimer',                      'Ports 3 and 4 Retimer Enabled'),
            ('./FlexIO/Type-CSubsystemConfiguration/TypeCPort4Config',                        'USB4 40G'),
            ('./Icc/SocClkOutCfg/BUFF_EN_SRC4',                                               'disable'),
            ('./Icc/SocClkOutCfg/BUFF_EN_SRC7',                                               'enable'),
            ('./FlexIO/PciePortConfiguration/PCIeController1Config',                          '1x2, 2x1 Lane Reversed'),
            ('./FlexIO/PowerDelivery_PdControllerConfiguration/TypeCPort4Mode',               'Yes'),
            ('./InternalPchBuses/PchTimerConfiguration/tPch45TimingConfig',                   '100ms'),
            ('./InternalPchBuses/SmbusSmlinkConfiguration/SLink1Enable',                      'Yes'),
            ('./Power/PlatformPower/SlpS5Gppv09Config',                                       'Enable as SLP_S5#'),
            ('./Power/PlatformPower/SusclkGppv07Config',                                      'Enable as SUSCLK'),
        ])

    if 'rvp_ddr5' in plt_params_list:
        print ("Applying changes to enable RVP DDR5 Board configuration")
        xml_change_list.append ([
            ('./FlashLayout/DescriptorRegion/HarnessGlobalData/SelectedRvp',                  'PTL-H DDR5 T3 RVP-4 (PTP-H + PTL-H)'),
            ('./FlashSettings/FlashConfiguration/SpiDualIoReadEnable',                        'No'),
            ('./FlashSettings/FlashConfiguration/SpiDualOutReadEnable',                       'No'),
            ('./PlatformProtection/PlatformIntegrity/OemExtInputFile',                        '$SourceDir\LNL_OEMKeyManifest.bin'),
            ('./NetworkingConnectivity/WiredLanConfiguration/PhyConnected',                   'PHY on SMLink0'),
            ('./NetworkingConnectivity/WiredLanConfiguration/LanPhyPwrCtrlGppv10Config',      'Enable as LANPHYPC'),
            ('./NetworkingConnectivity/WiredLanConfiguration/SlpLanGppv11Config',             'Enable as SLP_LAN#'),
            ('./CpuStraps/CpuStraps/IaVrOffsetVid',                                           'Yes'),
            ('./CpuStraps/CpuStraps/PlatformImonDisable',                                     'Enabled'),
            ('./FlexIO/Type-CSubsystemConfiguration/TboltPort34Retimer',                      'Ports 3 and 4 Retimer Enabled'),
            ('./FlexIO/Type-CSubsystemConfiguration/TypeCPort4Config',                        'USB4 40G'),
            ('./Icc/SocClkOutCfg/BUFF_EN_SRC4',                                               'disable'),
            ('./FlexIO/PciePortConfiguration/PCIeController1Config',                          '1x2, 2x1 Lane Reversed'),
            ('./FlexIO/PowerDelivery_PdControllerConfiguration/TypeCPort4Mode',               'Yes'),
            ('./FlexIO/PciePortConfiguration/PCIeGen5Controller4Config',                      '1x8 Lane Reversed'),
            ('./InternalPchBuses/PchTimerConfiguration/tPch45TimingConfig',                   '100ms'),
            ('./Power/PlatformPower/SlpS5Gppv09Config',                                       'Enable as SLP_S5#'),
            ('./Power/PlatformPower/SusclkGppv07Config',                                      'Enable as SUSCLK'),
        ])

    if 'crb' in plt_params_list:
        print ("Applying changes to enable CRB")
        xml_change_list.append ([
            ('./FlashLayout/DescriptorRegion/HarnessGlobalData/SelectedRvp',                  'PTL-H DDR5 T3 RVP-4 (PTP-H + PTL-H)'),
            ('./NetworkingConnectivity/WiredLanConfiguration/PhyConnected',                   'PHY on SMLink0'),
            ('./InternalPchBuses/PchTimerConfiguration/tPch45TimingConfig',                   '100ms'),
            ('./InternalPchBuses/EspiConfiguration/EspiEcBusfreq',                            '25MHz'),
            ('./InternalPchBuses/EspiConfiguration/EspiEcMaxIoMode',                          'Single and Dual'),
            ('./InternalPchBuses/EspiConfiguration/EspiEcSlave1DeviceEn',                     'Yes'),
            ('./InternalPchBuses/EspiConfiguration/EspiEcSlv1DevBusfreq',                     '25MHz'),
            ('./InternalPchBuses/EspiConfiguration/EspiEcSlv1DevMaxIoMode',                   'Single and Duel'),
            ('./Power/PlatformPower/SuswarnSusackEn',                                         'No'),
            ('./IntegratedSensorHub/IntegratedSensorHub/IshSupported',                        'No'),
            ('./CpuStraps/CpuStraps/PlatformImonDisable',                                     'Enabled'),
            ('./FlexIO/PciePortConfiguration/PCIeController1Config',                          '4x1'),
            ('./FlexIO/PciePortConfiguration/PCIeGen5Controller4Config',                      '1x8'),
            ('./FlexIO/Type-CSubsystemConfiguration/TboltPort34Retimer',                      'Ports 3 and 4 Retimer Enabled'),
            ('./FlexIO/Type-CSubsystemConfiguration/TypeCPort4Config',                        'USB4 40G'),
            ('./FlexIO/PowerDelivery_PdControllerConfiguration/TypeCPort4Mode',               'Yes')
        ])

    if '64MB' in plt_params_list:
        xml_change_list.append ([
            ('./GlobalSettings/ImageSettings/ImageSubdivisionsSizes',                         '64'),
        ])

    if 'txt' in plt_params_list:
        print ("Applying changes to enable TXT support")
        global g_txt_enabled
        g_txt_enabled = True
        xml_change_list.append ([
            ('./PlatformProtection/TxtConfiguration/TxtSupported',                        'Yes'),
            ])

    return xml_change_list

