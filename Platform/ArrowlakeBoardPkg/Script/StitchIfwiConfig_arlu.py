## @ StitchIfwiConfig_arlu.py
#  This is an IFWI stitch config script for ARL-U Slim Bootloader
#
# Copyright (c) 2024, Intel Corporation. All rights reserved. <BR>
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
       Assuming stitching workspace is at D:\Stitch and building ifwi for ARL-U platform
       To stitch IFWI with SPI QUAD mode and Boot Guard profile VM:
         StitchIfwi.py -b vm -p arlu -w D:\Stitch -s Stitch_Components.zip -c StitchIfwiConfig.py

"""

def get_bpmgen2_params_change_list ():
    params_change_list = []
    params_change_list.append ([
      # variable                | value |
      # ===================================
      ('PlatformRules',         'ARL Client'),
      ('BpmStrutVersion',       '0x25'),
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
      ('CnbsInclude',           'TRUE'),
      ])
    return params_change_list

def get_platform_sku():
    platform_sku ={
        'arlu'    :   'Intel(R) MeteorLake P and ArrowLake P Chipsets - Consumer - SPI'
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
        'lp5'    : {},
        'crb'    : {},
        'rvp'    : {},
       }

    para_help = \
        """
        'sata'   -- Enable direct SATA ports, these are disabled by default
        'debug'  -- Enable DAM and DCI configuration (Only use for debug purpose but not for final production!)
        'lp5'    -- Enable LPDDR5 Board configuration
        'crb'    -- Enable CRB Board configuration
        'rvp'    -- Enable RVP Board configuration
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

# NOTE:
# The following xml path value are automatically set based on -t (tpm) parameter in StitchIfwi.py script
# Refer to update_tpm_type function in Platform\ArrowlakeBoardPkg\Script\security_stitch_help_arl.py
# - ./PlatformProtection/IntelPttConfiguration/PttSupported
# - ./PlatformProtection/IntelPttConfiguration/PttPwrUpState
# - ./PlatformProtection/IntelPttConfiguration/PttSupportedFpf
# - ./PlatformProtection/TpmOverSpiBusConfiguration/SpiOverTpmBusEnable
#
# The following xml path value are automatically set by -b (btg_profile) parameter in StitchIfwi.py script
# Refer to update_btGuard_xml function in Platform\ArrowlakeBoardPkg\Script\security_stitch_help_arl.py
# - ./PlatformProtection/PlatformIntegrity/OemPublicKeyHash
# - ./PlatformProtection/BootGuardConfiguration/BtGuardKeyManifestId
# - ./PlatformProtection/BootGuardConfiguration/BtGuardProfileConfig
def get_xml_change_list (platform, plt_params_list):
    xml_change_list = []
    xml_change_list.append ([
        # Path                                                                      | value |
        # =========================================================================================
        #   Region Order
        ('./FlashSettings/BiosConfiguration/BiosRedAssistance',                                     'Disabled'),
        ('./FlashLayout/DescriptorRegion/HarnessGlobalData/SelectedRvp',                            'ARL-H DDR5 SBS (MTP-P + ARL-H)'),
        ('./FlashLayout/BiosRegion/InputFile',                                                      '$SourceDir\BiosRegion.bin'),
        ('./FlashLayout/Ifwi_IntelMePmcRegion/MeRegionFile',                                        '$SourceDir\ME Sub Partition.bin'),
        ('./FlashLayout/Ifwi_IntelMePmcRegion/PmcBinary',                                           '$SourceDir\CsePlugin#PMC.bin'),
        ('./FlashLayout/EcRegion/EcRegionPointer',                                                  '$SourceDir\DescriptorPlugin#EcRegionPointer.bin'),
        ('./FlashLayout/EcRegion/InputFile',                                                        '$SourceDir\SystemPlugin#EcRegion.bin'),
        ('./FlashLayout/EcRegion/Enabled',                                                          'Enabled'),
        ('./FlashLayout/GbeRegion/InputFile',                                                       '$SourceDir\SystemPlugin#GbeRegion.bin'),
        ('./FlashLayout/GbeRegion/Enabled',                                                         'Enabled'),
        ('./FlashLayout/SubPartitions/IunitSubPartition/InputFile',                                 '$SourceDir\CsePlugin#IUNIT.bin'),
        ('./FlashLayout/SubPartitions/SoccSubPartitionData/InputFile',                              '$SourceDir\CsePlugin#CSE_SOCC.bin'),
        ('./FlashLayout/SubPartitions/AceSubPartition/InputFile',                                   '$SourceDir\CsePlugin#ACE.bin'),
        ('./FlashLayout/SubPartitions/EsePackage/EseSubPartition/ESEBinary',                        '$SourceDir\ESEPlugin#ESEP.bin'),
        ('./FlashLayout/SubPartitions/EsePackage/DmuSubPartition/num_of_instance_DMUBinary_path',   '3'),
        ('./FlashLayout/SubPartitions/EsePackage/DmuSubPartition/DMUBinary',                        '$SourceDir\ESEPlugin#DMUP.bin'),
        ('./FlashLayout/SubPartitions/EsePackage/DmuSubPartition/DMUBinary2',                       '$SourceDir\ESEPlugin#DMUP2.bin'),
        ('./FlashLayout/SubPartitions/EsePackage/DmuSubPartition/DMUBinary3',                       '$SourceDir\ESEPlugin#DMUP3.bin'),
        ('./FlashLayout/SubPartitions/EsePackage/PunitSubPartition/num_of_instance_PUNITBinary_path', '3'),
        ('./FlashLayout/SubPartitions/EsePackage/PunitSubPartition/PUNITBinary',                    '$SourceDir\ESEPlugin#PUNIT.bin'),
        ('./FlashLayout/SubPartitions/EsePackage/PunitSubPartition/PUNITBinary2',                   '$SourceDir\ESEPlugin#PUNIT2.bin'),
        ('./FlashLayout/SubPartitions/EsePackage/PunitSubPartition/PUNITBinary3',                   '$SourceDir\ESEPlugin#PUNIT3.bin'),
        ('./FlashLayout/SubPartitions/EsePackage/AunitSubPartition/AUNITBinary',                    '$SourceDir\ESEPlugin#AUNIT.bin'),
        ('./PlatformProtection/BootGuardConfiguration/BtGuardCpuDebugEnable',                       'Enabled'),
        ('./NetworkingConnectivity/WiredLanConfiguration/PhyConnected',                             'PHY on SMLink0'),
        ('./NetworkingConnectivity/WiredLanConfiguration/LanEnable',                                'Yes'),
        ('./NetworkingConnectivity/WirelessLanConfiguration/MeClinkEnable',                         'Yes'),
        ('./Icc/SocClkOutCfg/BUFF_EN_SRC0',                                                         'enable'),
        ('./Icc/SocClkOutCfg/BUFF_EN_SRC1',                                                         'enable'),
        ('./Icc/SocClkOutCfg/BUFF_EN_SRC2',                                                         'enable'),
        ('./Icc/SocClkOutCfg/BUFF_EN_SRC3',                                                         'disable'),
        ('./Icc/SocClkOutCfg/BUFF_EN_SRC4',                                                         'disable'),
        ('./Icc/SocClkOutCfg/BUFF_EN_SRC5',                                                         'disable'),
        ('./Icc/SocClkOutCfg/SRC_MUXSEL_CFG0',                                                      'GPP_C09(SRCCLKREQ0#)'),
        ('./Icc/SocClkOutCfg/SRC_MUXSEL_CFG1',                                                      'GPP_C10(SRCCLKREQ1#)'),
        ('./Icc/SocClkOutCfg/SRC_MUXSEL_CFG2',                                                      'GPP_C11(SRCCLKREQ2#)'),
        ('./Icc/SocClkOutCfg/SRC_MUXSEL_CFG3',                                                      'GPP_C12(SRCCLKREQ3#)'),
        ('./Icc/SocClkOutCfg/SRC_MUXSEL_CFG4',                                                      'GPP_C13(SRCCLKREQ4#)'),
        ('./Icc/SocClkOutCfg/SRC_MUXSEL_CFG5',                                                      'GPP_D21(SRCCLKREQ5#)'),
        ('./Icc/SocClkOutCfg/SSC_en',                                                               'disable'),
        ('./Icc/IoeClkOutCfg/IOE_BUFF_EN_SRC0',                                                     'enable'),
        ('./Icc/IoeClkOutCfg/IOE_BUFF_EN_SRC1',                                                     'disable'),
        ('./Icc/IoeClkOutCfg/IOE_BUFF_EN_SRC2',                                                     'enable'),
        ('./Icc/IoeClkOutCfg/IOE_SRC_MUXSEL_CFG0',                                                  'GPP_D18(SRCCLKREQ6#)'),
        ('./Icc/IoeClkOutCfg/IOE_SRC_MUXSEL_CFG1',                                                  'GPP_D19(SRCCLKREQ7#)'),
        ('./Icc/IoeClkOutCfg/IOE_SRC_MUXSEL_CFG2',                                                  'GPP_D20(SRCCLKREQ8#)'),
        ('./Icc/IoeClkOutCfg/IOE_SSC_en',                                                           'disable'),
        ('./InternalPchBuses/SmbusSmlinkConfiguration/SLink1freq',                                  '400 KHz'),
        ('./IntegratedSensorHub/IshImage/InputFile',                                                '$SourceDir\CsePlugin#ISH.bin'),
        ('./IntegratedSensorHub/IshData/PdtBinary',                                                 '$SourceDir\ish_pdt_binary.bin'),
        ('./Debug/DelayedAuthenticationModeConfiguration/DelayedAuthMode',                          'Yes'),
        ('./Debug/IntelTraceHubTechnology/RomTracePreBiosEn',                                       'Trace Hub Pre-BIOS Enabled'),
        ('./Debug/IntelMeFirmwareDebuggingOverrides/DbgOverridePreProdSi',                          '0x7'),
        ('./Debug/DirectConnectInterfaceConfiguration/DciDbcEnable',                                'Yes'),
        ('./Debug/EarlyUsb2DbcOverType-AConfiguration/Usb2DbcPortEn',                               'No USB2 Ports'),
        ('./Debug/EarlyUsb2DbcOverType-AConfiguration/EnEarlyUsb2DbcCon',                           'Yes'),
        ('./CpuStraps/CpuStraps/PlatformImonDisable',                                               'Enabled'),
        ('./FlexIO/PciePortConfiguration/PCIeController1',                                          '1x4 Lane Reversed'),
        ('./FlexIO/PciePortConfiguration/PCIeController2',                                          '4x1'),
        ('./FlexIO/Usb3PortConfiguration/Usb32Port1and2SpdselPair',                                 'USB 3.2 Port 1 and 2 Gen 1x1'),
        ('./FlexIO/Usb3PortConfiguration/USB3Prt1ConTypeSel',                                       'Type A / Type C'),
        ('./FlexIO/Usb3PortConfiguration/USB3Prt2ConTypeSel',                                       'Type A / Type C'),
        ('./FlexIO/Usb2PortConfiguration/USB2Prt7ConTypeSel',                                       'Type A / Type C'),
        ('./FlexIO/Usb2PortConfiguration/USB2Prt8ConTypeSel',                                       'Type A / Type C'),
        ('./FlexIO/Usb2PortConfiguration/USB2Prt9ConTypeSel',                                       'Type A / Type C'),
        ('./FlexIO/Usb2PortConfiguration/USB2Prt10ConTypeSel',                                      'Type A / Type C'),
        ('./FlexIO/PhyConfiguration/PhyBinaryFile',                                                 '$SourceDir\CsePlugin#SPHY.bin'),
        ('./FlexIO/Type-CSubsystemConfiguration/IomBinaryFile',                                     '$SourceDir\CsePlugin#IOM.bin'),
        ('./FlexIO/Type-CSubsystemConfiguration/PhyBinaryFile',                                     '$SourceDir\CsePlugin#NPHY.bin'),
        ('./FlexIO/Type-CSubsystemConfiguration/TbtBinaryFile',                                     '$SourceDir\CsePlugin#TBT.bin'),
        ('./FlexIO/Type-CSubsystemConfiguration/TypeCPort1Config',                                  'DP Fixed Connection'),
        ('./FlexIO/Type-CSubsystemConfiguration/TypeCPort2Config',                                  'DP Fixed Connection'),
        ('./FlexIO/Type-CSubsystemConfiguration/XdciSplitDieConfig',                                'xDCI Split Die Enabled'),
        ('./FlexIO/Type-CSubsystemConfiguration/TboltPort12Retimer',                                'No Retimers'),
        ('./FlexIO/PowerDelivery_PdControllerConfiguration/TypeCPort1Mode',                         'No'),
        ('./FlexIO/PowerDelivery_PdControllerConfiguration/TypeCPort1RetimerEnabled',               'No'),
        ('./FlexIO/PowerDelivery_PdControllerConfiguration/TypeCPort1RetimerConfig',                'No'),
        ('./FlexIO/PowerDelivery_PdControllerConfiguration/TypeCPort1SmbusAddr',                    '0x0'),
        ('./FlexIO/PowerDelivery_PdControllerConfiguration/TypeCPort2Mode',                         'No'),
        ('./FlexIO/PowerDelivery_PdControllerConfiguration/TypeCPort2RetimerEnabled',               'No'),
        ('./FlexIO/PowerDelivery_PdControllerConfiguration/TypeCPort2RetimerConfig',                'No'),
        ('./FlexIO/PowerDelivery_PdControllerConfiguration/TypeCPort2SMBusAddr',                    '0x0'),
        ('./FlexIO/PowerDelivery_PdControllerConfiguration/USB2PortForTypeCPort3',                  'USB2 Port 1'),
        ('./FlexIO/PowerDelivery_PdControllerConfiguration/USB2PortForTypeCPort4',                  'USB2 Port 2'),
    ])

    if 'rvp' in plt_params_list:
        print ("Applying changes to enable RVP")
        xml_change_list.append ([
            ('./NetworkingConnectivity/WirelessLanConfiguration/MeClinkEnable',                         'No'),
            ('./Icc/SocClkOutCfg/BUFF_EN_SRC3',                                                         'enable'),
            ('./Icc/SocClkOutCfg/BUFF_EN_SRC4',                                                         'enable'),
            ('./Icc/SocClkOutCfg/BUFF_EN_SRC5',                                                         'enable'),
            ('./Icc/SocClkOutCfg/SSC_en',                                                               'enable'),
            ('./Icc/IoeClkOutCfg/IOE_BUFF_EN_SRC1',                                                     'enable'),
            ('./Icc/IoeClkOutCfg/IOE_SSC_en',                                                           'enable'),
            ('./InternalPchBuses/SmbusSmlinkConfiguration/SLink1freq',                                  '100 KHz'),
            ('./Debug/EarlyUsb2DbcOverType-AConfiguration/Usb2DbcPortEn',                               'USB2 Port 1'),
            ('./FlexIO/PciePortConfiguration/PCIeController1',                                          '1x4'),
            ('./FlexIO/Usb3PortConfiguration/Usb32Port1and2SpdselPair',                                 'USB 3.2 Port 1 and 2 Gen 2x1'),
            ('./FlexIO/Usb2PortConfiguration/USB2Prt3ConTypeSel',                                       'Type C'),
            ('./FlexIO/Usb2PortConfiguration/USB2Prt4ConTypeSel',                                       'Type C'),
            ('./FlexIO/Type-CSubsystemConfiguration/TypeCPort1Config',                                  'No Restrictions'),
            ('./FlexIO/Type-CSubsystemConfiguration/TypeCPort2Config',                                  'No Restrictions'),
            ('./FlexIO/Type-CSubsystemConfiguration/TboltPort12Retimer',                                'Ports 1 and 2 Retimer Enabled'),
            ('./FlexIO/PowerDelivery_PdControllerConfiguration/TypeCPort1Mode',                         'Yes'),
            ('./FlexIO/PowerDelivery_PdControllerConfiguration/TypeCPort1RetimerEnabled',               'Yes'),
            ('./FlexIO/PowerDelivery_PdControllerConfiguration/TypeCPort1RetimerConfig',                'Yes'),
            ('./FlexIO/PowerDelivery_PdControllerConfiguration/TypeCPort1SmbusAddr',                    '0x20'),
            ('./FlexIO/PowerDelivery_PdControllerConfiguration/TypeCPort2Mode',                         'Yes'),
            ('./FlexIO/PowerDelivery_PdControllerConfiguration/TypeCPort2RetimerEnabled',               'Yes'),
            ('./FlexIO/PowerDelivery_PdControllerConfiguration/TypeCPort2RetimerConfig',                'Yes'),
            ('./FlexIO/PowerDelivery_PdControllerConfiguration/TypeCPort2SMBusAddr',                    '0x24'),
            ('./FlexIO/PowerDelivery_PdControllerConfiguration/USB2PortForTypeCPort3',                  'USB2 Port 3'),
            ('./FlexIO/PowerDelivery_PdControllerConfiguration/USB2PortForTypeCPort4',                  'USB2 Port 4'),
    ])

    if 'lp5' in plt_params_list:
        print ("Applying changes to enable RVP")
        xml_change_list.append ([
            ('./FlashLayout/DescriptorRegion/HarnessGlobalData/SelectedRvp',                            'ARL-H LP5x-T4 (MTP-P + ARL-H)'),
            ('./NetworkingConnectivity/WirelessLanConfiguration/MeClinkEnable',                         'No'),
            ('./Icc/SocClkOutCfg/BUFF_EN_SRC3',                                                         'enable'),
            ('./Icc/SocClkOutCfg/BUFF_EN_SRC4',                                                         'enable'),
            ('./Icc/SocClkOutCfg/BUFF_EN_SRC5',                                                         'enable'),
            ('./Icc/SocClkOutCfg/SSC_en',                                                               'enable'),
            ('./Icc/IoeClkOutCfg/IOE_BUFF_EN_SRC1',                                                     'enable'),
            ('./Icc/IoeClkOutCfg/IOE_SSC_en',                                                           'enable'),
            ('./InternalPchBuses/SmbusSmlinkConfiguration/SLink1freq',                                  '100 KHz'),
            ('./Debug/EarlyUsb2DbcOverType-AConfiguration/Usb2DbcPortEn',                               'USB2 Port 1'),
            ('./FlexIO/PciePortConfiguration/PCIeController1',                                          '1x4'),
            ('./FlexIO/Usb3PortConfiguration/Usb32Port1and2SpdselPair',                                 'USB 3.2 Port 1 and 2 Gen 2x1'),
            ('./FlexIO/Usb2PortConfiguration/USB2Prt3ConTypeSel',                                       'Type C'),
            ('./FlexIO/Usb2PortConfiguration/USB2Prt4ConTypeSel',                                       'Type C'),
            ('./FlexIO/Type-CSubsystemConfiguration/TypeCPort1Config',                                  'No Restrictions'),
            ('./FlexIO/Type-CSubsystemConfiguration/TypeCPort2Config',                                  'No Restrictions'),
            ('./FlexIO/Type-CSubsystemConfiguration/TboltPort12Retimer',                                'Ports 1 and 2 Retimer Enabled'),
            ('./FlexIO/PowerDelivery_PdControllerConfiguration/TypeCPort1Mode',                         'Yes'),
            ('./FlexIO/PowerDelivery_PdControllerConfiguration/TypeCPort1RetimerEnabled',               'Yes'),
            ('./FlexIO/PowerDelivery_PdControllerConfiguration/TypeCPort1RetimerConfig',                'Yes'),
            ('./FlexIO/PowerDelivery_PdControllerConfiguration/TypeCPort1SmbusAddr',                    '0x20'),
            ('./FlexIO/PowerDelivery_PdControllerConfiguration/TypeCPort2Mode',                         'Yes'),
            ('./FlexIO/PowerDelivery_PdControllerConfiguration/TypeCPort2RetimerEnabled',               'Yes'),
            ('./FlexIO/PowerDelivery_PdControllerConfiguration/TypeCPort2RetimerConfig',                'Yes'),
            ('./FlexIO/PowerDelivery_PdControllerConfiguration/TypeCPort2SMBusAddr',                    '0x24'),
            ('./FlexIO/PowerDelivery_PdControllerConfiguration/USB2PortForTypeCPort3',                  'USB2 Port 3'),
            ('./FlexIO/PowerDelivery_PdControllerConfiguration/TypeCPort3RetimerConfigType',            '2 Re-Timers'),
            ('./FlexIO/PowerDelivery_PdControllerConfiguration/USB2PortForTypeCPort4',                  'USB2 Port 4'),
            ('./FlexIO/PowerDelivery_PdControllerConfiguration/TypeCPort4RetimerConfigType',            '2 Re-Timers'),
    ])

    return xml_change_list

