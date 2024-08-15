## @ StitchIfwiConfig_mtlp.py
#  This is an IFWI stitch config script for MTL Slim Bootloader
#
# Copyright (c) 2021 - 2024, Intel Corporation. All rights reserved. <BR>
# SPDX-License-Identifier: BSD-2-Clause-Patent
#
##
from StitchLoader import *

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
       Assuming stitching workspace is at D:\Stitch and building ifwi for MTL platform
       To stitch IFWI with SPI QUAD mode and Boot Guard profile VM:
         StitchIfwi.py -b vm -p mtlp -w D:\Stitch -s Stitch_Components.zip -c StitchIfwiConfig.py

"""

def get_bpmgen2_params_change_list ():
    params_change_list = []
    params_change_list.append ([
      # variable                | value |
      # ===================================
      ('PlatformRules',         'MTL Client'),
      ('BpmStrutVersion',       '0x20'),
      ('BpmRevision',           '0x01'),
      ('BpmRevocation',         '1'),
      ('AcmRevocation',         '2'),
      ('NEMPages',              '3'),
      ('AcpiBase',              '0x1800'),
      ('IbbFlags',              '0x12'),
      ('IbbHashAlgID',          '0x0C:SHA384'),
      ('MCHBAR',                '0x00000000FEDC0000'),
      ('VTD_BAR',               '0x00000000D0000000'),
      ('TxtInclude',            'FALSE'),
      ('PcdInclude',            'TRUE'),
      ('BpmSigScheme',          '0x16:RSAPSS'),
      ('BpmSigPubKey',          r'BpmGen2/keys/bpm_pubkey_3072.pem'),
      ('BpmSigPrivKey',         r'BpmGen2/keys/bpm_privkey_3072.pem'),
      ('BpmKeySizeBits',        '3072'),
      ('BpmSigHashAlgID',       '0x0C:SHA384'),
      ])
    return params_change_list

def get_platform_sku():
    platform_sku ={
        'mtlp'    :   'Intel(R) MeteorLake P Chipset - Consumer - SPI'
    }
    return platform_sku

def get_component_replace_list(plt_params_list):
    replace_list = [
       #    Path                   file name              compress    Key                           SVN
      ('IFWI/BIOS/TS0/ACM0',      'Input/acm0.bin',        'dummy',    '',                            ''),
      ('IFWI/BIOS/TS1/ACM0',      'Input/acm0.bin',        'dummy',    '',                            ''),
       ]

    return replace_list

def check_parameter(para_list):
    print (para_list)
    para_supported = {
        'sata'   : {},
        'debug'  : {},
       }

    para_help = \
        """
        'sata'   -- Enable direct SATA ports, these are disabled by default
        'debug'  -- Enable DAM and DCI configuration (Only use for debug purpose but not for final production!)
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
      # Path                                                      | value |
      # =========================================================================================
      #Region Order
        ('./FlashLayout/BiosRegion/InputFile',                                      '$SourceDir\BiosRegion.bin'),
        ('./FlashLayout/DescriptorRegion/HarnessGlobalData/SelectedRvp',            'MTL-P DDR5 (MTL-P + MTP-P)'),
        ('./FlashLayout/Ifwi_IntelMePmcRegion/MeRegionFile',                        '$SourceDir\MeRegionFile.bin'),
        ('./FlashLayout/Ifwi_IntelMePmcRegion/PmcBinary',                           '$SourceDir\PmcBinary.bin'),
        ('./FlashLayout/EcRegion/InputFile',                                        '$SourceDir\EcRegion.bin'),
        ('./FlashLayout/EcRegion/Enabled',                                          'Enabled'),
        ('./FlashLayout/EcRegion/EcRegionPointer',                                  '$SourceDir\EcRegionPointer.bin'),
        ('./FlashLayout/GbeRegion/InputFile',                                       '$SourceDir\GbeRegion.bin'),
        ('./FlashLayout/GbeRegion/Enabled',                                         'Disabled'),
        ('./FlashLayout/PdrRegion/InputFile',                                       ''),
        ('./FlashLayout/PdrRegion/Enabled',                                         'Disabled'),
        ('./FlashLayout/SubPartitions/IunitSubPartition/InputFile',                 '$SourceDir\CsePlugin#IUNIT.bin'),
        ('./FlashLayout/SubPartitions/SoccSubPartitionData/InputFile',              '$SourceDir\CsePlugin#CSE_SOCC.bin'),
        ('./FlashLayout/SubPartitions/AceSubPartition/InputFile',                   '$SourceDir\CsePlugin#ACE.bin'),
        ('./FlashLayout/SubPartitions/EsePackage/EseSubPartition/ESEBinary',        '$SourceDir\ESEPlugin#ESEP.bin'),
        ('./FlashLayout/SubPartitions/EsePackage/DmuSubPartition/DMUBinary',        '$SourceDir\ESEPlugin#DMUP.bin'),
        ('./FlashLayout/SubPartitions/EsePackage/PunitSubPartition/PUNITBinary',    '$SourceDir\ESEPlugin#PUNIT.bin'),
        ('./FlashSettings/FlashConfiguration/SpiDualIoReadEnable',                   'Yes'),
        ('./FlashSettings/FlashConfiguration/SpiDualOutReadEnable',                  'Yes'),
        ('./FlashSettings/BiosConfiguration/BiosRedAssistance',                      'Disabled'),
        ('./PlatformProtection/PlatformIntegrity/OemPublicKeyHash',                  'F8 F0 E3 69 15 81 76 99 0A 54 9E D4 C3 6D 1A 86 39 D8 87 3D EF F7 ED 2D E3 4C B4 1B CC B3 04 76 CE 0A A0 63 BC 5B 7A AC FF D9 50 9E 96 40 C6 99'),
        ('./PlatformProtection/PlatformIntegrity/OemExtInputFile',                   '$SourceDir\OemExtInputFile.bin'),
        ('./PlatformProtection/BootGuardConfiguration/BtGuardKeyManifestId',         '0x1'),
        ('./PlatformProtection/BootGuardConfiguration/BtGuardCpuDebugEnable',        'Enabled'),
        ('./PlatformProtection/IntelPttConfiguration/PttSupported',                  'No'),
        ('./PlatformProtection/IntelPttConfiguration/PttPwrUpState',                 'Disabled'),
        ('./PlatformProtection/IntelPttConfiguration/PttSupportedFpf',               'No'),
        ('./PlatformProtection/BiosGuardConfiguration/BiosGrdProtOvrdEn',            'Yes'),
        ('./PlatformProtection/TpmOverSpiBusConfiguration/SpiOverTpmBusEnable',      'Yes'),
        ('./NetworkingConnectivity/WiredLanConfiguration/PhyConnected',              'PHY on SMLink0'),
        ('./NetworkingConnectivity/WirelessLanConfiguration/MeClinkEnable',          'No'),
        ('./IntegratedSensorHub/IntegratedSensorHub/IshPowerUpState',                'Enabled'),
        ('./IntegratedSensorHub/IshImage/InputFile',                                 '$SourceDir\IshImage.bin'),
        ('./IntegratedSensorHub/IshData/PdtBinary',                                  '$SourceDir\\NVARS#IshPdt#IshPdtBinary.bin'),
        ('./CpuStraps/CpuStraps/PlatformImonDisable',                                'Enabled'),
        ('./Debug/DelayedAuthenticationModeConfiguration/DelayedAuthMode',           'No'),
        ('./FlexIO/Type-CSubsystemConfiguration/IomBinaryFile',                      '$SourceDir\Iom_L0R0.bin'),
        ('./FlexIO/Type-CSubsystemConfiguration/PhyBinaryFile',                      '$SourceDir\CsePlugin#NPHY.bin'),
        ('./FlexIO/Type-CSubsystemConfiguration/TbtBinaryFile',                      '$SourceDir\TBT.bin'),
        ('./FlexIO/Type-CSubsystemConfiguration/IomBinaryFile',                      '$SourceDir\Iom.bin'),
        ('./FlexIO/Type-CSubsystemConfiguration/XdciSplitDieConfig',                 'xDCI Split Die Enabled'),
        ('./FlexIO/PhyConfiguration/PhyBinaryFile',                                  '$SourceDir\CsePlugin#SPHY.bin'),
        ('./FlexIO/PciePortConfiguration/PCIeController1',                           '4x1'),
        ('./FlexIO/PciePortConfiguration/PCIeController2',                           '4x1'),
        ('./FlexIO/PciePortConfiguration/PCIeController3Config',                     '1x4'),
        ('./FlexIO/PciePortConfiguration/PCIeController4Config',                     '1x4'),
        ('./FlexIO/PciePortConfiguration/PCIeController5Config',                     '1x4'),
        ('./FlexIO/PciePortConfiguration/PCIeGen5Controller6Config',                 '1x8 Lane Reversed'),

        ('./FlexIO/Usb3PortConfiguration/USB3Prt1ConTypeSel',                        'Type A / Type C'),
        ('./FlexIO/Usb3PortConfiguration/USB3Prt2ConTypeSel',                        'Type A / Type C'),
        ('./FlexIO/Usb2PortConfiguration/USB2Prt2ConTypeSel',                        'Type C'),
        ('./FlexIO/Usb2PortConfiguration/USB2Prt3ConTypeSel',                        'Type C'),
        ('./FlexIO/Usb2PortConfiguration/USB2Prt4ConTypeSel',                        'Type C'),
        ('./FlexIO/Usb2PortConfiguration/USB2Prt7ConTypeSel',                        'Type A / Type C'),
        ('./FlexIO/Usb2PortConfiguration/USB2Prt8ConTypeSel',                        'Type A / Type C'),
        ('./FlexIO/Usb2PortConfiguration/USB2Prt9ConTypeSel',                        'Type A / Type C'),
        ('./FlexIO/Usb2PortConfiguration/USB2Prt10ConTypeSel',                       'Type A / Type C'),
        ('./FlexIO/Usb2PortConfiguration/USB2Prt1ConTypeSel',                        'Type A / Type C'),
        ('./NetworkingConnectivity/WiredLanConfiguration/LanEnable',                 'No'),
        ('./NetworkingConnectivity/WiredLanConfiguration/LanPhyPwrCtrlGppv11Config', 'Enable as GPP_V11'),
        ('./NetworkingConnectivity/WiredLanConfiguration/SlpLanGppv12Config',        'Enable as GPP_V12'),
        ('./Icc/SocClkOutCfg/BUFF_EN_SRC0',                                          'enable'),
        ('./Icc/SocClkOutCfg/BUFF_EN_SRC1',                                          'enable'),
        ('./Icc/SocClkOutCfg/BUFF_EN_SRC2',                                          'enable'),
        ('./Icc/SocClkOutCfg/BUFF_EN_SRC3',                                          'enable'),
        ('./Icc/SocClkOutCfg/BUFF_EN_SRC4',                                          'enable'),
        ('./Icc/SocClkOutCfg/BUFF_EN_SRC5',                                          'enable'),
        ('./Icc/SocClkOutCfg/SRC_MUXSEL_CFG0',                                       'GPP_C09(SRCCLKREQ0#)'),
        ('./Icc/SocClkOutCfg/SRC_MUXSEL_CFG1',                                       'GPP_C10(SRCCLKREQ1#)'),
        ('./Icc/SocClkOutCfg/SRC_MUXSEL_CFG2',                                       'GPP_C11(SRCCLKREQ2#)'),
        ('./Icc/SocClkOutCfg/SRC_MUXSEL_CFG3',                                       'GPP_C12(SRCCLKREQ3#)'),
        ('./Icc/SocClkOutCfg/SRC_MUXSEL_CFG4',                                       'GPP_C13(SRCCLKREQ4#)'),
        ('./Icc/SocClkOutCfg/SRC_MUXSEL_CFG5',                                       'GPP_D21(SRCCLKREQ5#)'),
        ('./Icc/SocClkOutCfg/SSC_en',                                                'enable'),
        ('./Icc/IoeClkOutCfg/IOE_BUFF_EN_SRC0',                                      'enable'),
        ('./Icc/IoeClkOutCfg/IOE_BUFF_EN_SRC1',                                      'enable'),
        ('./Icc/IoeClkOutCfg/IOE_BUFF_EN_SRC2',                                      'enable'),
        ('./Icc/IoeClkOutCfg/IOE_SRC_MUXSEL_CFG0',                                   'GPP_D18(SRCCLKREQ6#)'),
        ('./Icc/IoeClkOutCfg/IOE_SRC_MUXSEL_CFG1',                                   'GPP_D19(SRCCLKREQ7#)'),
        ('./Icc/IoeClkOutCfg/IOE_SRC_MUXSEL_CFG2',                                   'GPP_D20(SRCCLKREQ8#)'),
        ('./Icc/IoeClkOutCfg/IOE_SSC_en',                                            'enable'),
        ('./FlexIO/Type-CSubsystemConfiguration/TypeCPort1Config',                   'No Thunderbolt'),
        ('./FlexIO/Type-CSubsystemConfiguration/TypeCPort2Config',                   'No Restrictions'),
        ('./FlexIO/Type-CSubsystemConfiguration/TypeCPort3Config',                   'DP Fixed Connection'),
        ('./FlexIO/Type-CSubsystemConfiguration/TypeCPort4Config',                   'DP Fixed Connection'),
        ('./FlexIO/Type-CSubsystemConfiguration/TboltPort12Retimer',                 'Port 1 Retimer Enabled'),
        ('./FlexIO/Type-CSubsystemConfiguration/TboltPort34Retimer',                 'No Retimers'),
        ('./FlexIO/TypeCPortConfiguration/TypeCPorts1and2SpdselPair',                'Type C Port 1 and 2 Gen 2x1'),
        ('./FlexIO/TypeCPortConfiguration/TypeCPorts3and4SpdselPair',                'Type C Port 1 and 2 Gen 2x1'),
        ('./FlexIO/PowerDelivery_PdControllerConfiguration/TypeCPort1RetimerEnabled',      'No'),
        ('./FlexIO/PowerDelivery_PdControllerConfiguration/TypeCPort2Mode',                'No'),
        ('./FlexIO/PowerDelivery_PdControllerConfiguration/TypeCPort3Mode',                'No'),
        ('./FlexIO/PowerDelivery_PdControllerConfiguration/TypeCPort4Mode',                'No'),
            ('./FlexIO/PciePortConfiguration/PCIeController1',                                '1x4 Lane Reversed'),
            ('./FlexIO/Usb3PortConfiguration/Usb32Port1and2SpdselPair',                       'USB 3.2 Port 1 and 2 Gen 1x1'),
            ('./FlexIO/Usb2PortConfiguration/USB2Prt3ConTypeSel',                             'Type A / Type C'),
            ('./FlexIO/Usb2PortConfiguration/USB2Prt4ConTypeSel',                             'Type A / Type C'),
            ('./FlexIO/Type-CSubsystemConfiguration/TboltPort12Retimer',                      'No Retimers'),
            ('./FlexIO/PowerDelivery_PdControllerConfiguration/TypeCPort1Mode',               'No'),
            ('./FlexIO/PowerDelivery_PdControllerConfiguration/TypeCPort1RetimerEnabled',     'No'),
            ('./FlexIO/PowerDelivery_PdControllerConfiguration/TypeCPort1RetimerConfig',      'No'),
            ('./FlexIO/PowerDelivery_PdControllerConfiguration/USB2PortForTypeCPort1',        'USB2 Port 9'),
            ('./FlexIO/PowerDelivery_PdControllerConfiguration/TypeCPort2Mode',               'No'),
            ('./FlexIO/PowerDelivery_PdControllerConfiguration/TypeCPort2RetimerEnabled',     'No'),
            ('./FlexIO/PowerDelivery_PdControllerConfiguration/TypeCPort2RetimerConfig',      'No'),
            ('./FlexIO/PowerDelivery_PdControllerConfiguration/USB2PortForTypeCPort2',        'USB2 Port 10'),
            ('./FlexIO/PowerDelivery_PdControllerConfiguration/USB2PortForTypeCPort3',        'USB2 Port 1'),
            ('./FlexIO/PowerDelivery_PdControllerConfiguration/USB2PortForTypeCPort4',        'USB2 Port 2'),
            ('./FlexIO/Type-CSubsystemConfiguration/TypeCPort3Config',                        'No Restrictions'),
            ('./FlexIO/Type-CSubsystemConfiguration/TypeCPort4Config',                        'No Restrictions'),
            ('./FlexIO/Type-CSubsystemConfiguration/TboltPort34Retimer',                      'Ports 3 and 4 Retimer Enabled'),
            ('./FlexIO/TypeCPortConfiguration/TypeCPorts1and2SpdselPair',                     'Type C Port 1 and 2 Gen 2x2'),
            ('./FlexIO/PowerDelivery_PdControllerConfiguration/TypeCPort3Mode',               'Yes'),
            ('./FlexIO/PowerDelivery_PdControllerConfiguration/TypeCPort4Mode',               'Yes'),
            ('./FlexIO/Usb2PortConfiguration/USB2Prt1ConTypeSel',                             'Type C'),
            ('./NetworkingConnectivity/WirelessLanConfiguration/MeClinkEnable',               'Yes'),
            ('./Icc/SocClkOutCfg/BUFF_EN_SRC3',                                               'enable'),
            ('./Icc/SocClkOutCfg/BUFF_EN_SRC4',                                               'enable'),
            ('./Icc/SocClkOutCfg/BUFF_EN_SRC5',                                               'enable'),
    ])

    if 'debug' in plt_params_list:
        print ("Applying changes to enable DAM and DCI")
        xml_change_list.append ([
            ('./Debug/DelayedAuthenticationModeConfiguration/DelayedAuthMode',             'Yes'),
            ('./PlatformProtection/BootGuardConfiguration/BtGuardCpuDebugEnable',          'Enabled'),
            ('./Debug/DirectConnectInterfaceConfiguration/DciDbcEnable',                   'Yes'),
            ('./Debug/DirectConnectInterfaceConfiguration/Usb1DciOobEnable',               'Yes'),
            ('./Debug/DirectConnectInterfaceConfiguration/Usb2DciOobEnable',               'Yes'),
            ('./Debug/EarlyUsb2DbcOverType-AConfiguration/Usb2DbcPortEn',                  'USB2 Port 1'),
            ('./Debug/IntelMeFirmwareDebuggingOverrides/DbgOverridePreProdSi',             '0x107'),
            ])

    if 'sata' in plt_params_list:
        print ("Applying changes to enable SATA port")
        xml_change_list.append ([
            ('./FlexIO/PciePortConfiguration/PCIeController1',                            '4x1'),
            ('./FlexIO/SataPcieComboPortConfiguration/SataPcieComboPorts0',               'SATA'),
            ('./FlexIO/SataPcieComboPortConfiguration/SataPcieComboPorts1',               'SATA'),
            ('./FlexIO/SataPcieComboPortConfiguration/SataPcieCombo0ModeSel',             'DEVSLP#'),
            ('./FlexIO/SataPcieComboPortConfiguration/SataPcieCombo1ModeSel',             'DEVSLP#'),
        ])

    return xml_change_list

