## @ StitchIfwiConfig_arls.py
#  This is an IFWI stitch config script for ARL-S Slim Bootloader
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
       Assuming stitching workspace is at D:\Stitch and building ifwi for ARL-S platform
       To stitch IFWI with SPI QUAD mode and Boot Guard profile VM:
         StitchIfwi.py -b vm -p arls -w D:\Stitch -s Stitch_Components.zip -c StitchIfwiConfig.py

"""

def get_bpmgen2_params_change_list ():
    params_change_list = []
    params_change_list.append ([
      # variable                | value |
      # ===================================
      ('PlatformRules',         'ARL Client'),
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
        'arls'    :   'Intel(R) MeteorLake S and ArrowLake S Chipsets - Consumer - SPI'
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
       }

    para_help = \
        """
        'sata'   -- Enable direct SATA ports, these are disabled by default
        'debug'  -- Enable DAM and DCI configuration (Only use for debug purpose but not for final production!)
        'lp5'    -- Enable LPDDR5 Board configuration
        'crb'    -- Enable CRB Board configuration
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
        ('./FlashSettings/BiosConfiguration/PchTopSwapOverride',                                    '512KB'),
        ('./FlashSettings/BiosConfiguration/SocTopSwapOverride',                                    '512KB'),
        ('./FlashLayout/DescriptorRegion/HarnessGlobalData/SelectedRvp',                            'MTL-S UDIMM 1DPC (MTP-S + MTL-S)'),
        ('./FlashLayout/BiosRegion/InputFile',                                                      '$SourceDir\BiosRegion.bin'),
        ('./FlashLayout/Ifwi_IntelMePmcRegion/MeRegionFile',                                        '$SourceDir\ME Sub Partition.bin'),
        ('./FlashLayout/Ifwi_IntelMePmcRegion/PmcBinary',                                           '$SourceDir\CsePlugin#PMC.bin'),
        ('./FlashLayout/EcRegion/EcRegionPointer',                                                  '$SourceDir\EcRegionPointer.bin'),
        ('./FlashLayout/EcRegion/InputFile',                                                        '$SourceDir\EcRegion.bin'),
        ('./FlashLayout/EcRegion/Enabled',                                                          'Enabled'),
        ('./FlashLayout/GbeRegion/InputFile',                                                       '$SourceDir\GbeRegion.bin'),
        ('./FlashLayout/GbeRegion/Enabled',                                                         'Enabled'),
        ('./FlashLayout/SubPartitions/PchcSubPartitionData/InputFile',                              '$SourceDir\CsePlugin#PCHC.bin'),
        ('./FlashLayout/SubPartitions/SoccSubPartitionData/InputFile',                              '$SourceDir\ESEPlugin#SOCC.bin'),
        ('./FlashLayout/SubPartitions/AceSubPartition/InputFile',                                   '$SourceDir\CsePlugin#ACE.bin'),
        ('./FlashLayout/SubPartitions/EsePackage/EseSubPartition/ESEBinary',                        '$SourceDir\ESEPlugin#ESEP.bin'),
        ('./FlashLayout/SubPartitions/EsePackage/DmuSubPartition/num_of_instance_DMUBinary_path',   '2'),
        ('./FlashLayout/SubPartitions/EsePackage/DmuSubPartition/DMUBinary',                        '$SourceDir\ESEPlugin#DMUP.bin'),
        ('./FlashLayout/SubPartitions/EsePackage/DmuSubPartition/DMUBinary2',                       '$SourceDir\ESEPlugin#DMUP2.bin'),
        ('./FlashLayout/SubPartitions/EsePackage/PunitSubPartition/num_of_instance_PUNITBinary_path', '2'),
        ('./FlashLayout/SubPartitions/EsePackage/PunitSubPartition/PUNITBinary',                    '$SourceDir\ESEPlugin#PUNIT.bin'),
        ('./FlashLayout/SubPartitions/EsePackage/PunitSubPartition/PUNITBinary2',                   '$SourceDir\ESEPlugin#PUNIT2.bin'),
        ('./FlashLayout/SubPartitions/EsePackage/AunitSubPartition/AUNITBinary',                    '$SourceDir\ESEPlugin#AUNIT.bin'),
        ('./FlashLayout/SubPartitions/EsePackage/SocEseSubPartition/SocEseBinary',                  '$SourceDir\ESEPlugin#SocEse.bin'),
        ('./FlashLayout/SubPartitions/EsePackage/SocSphySubPartition/SocSphyBinary',                '$SourceDir\ESEPlugin#SocSphy.bin'),
        ('./FlashLayout/SubPartitions/EsePackage/SocPmcSubPartition/SocPmcBinary',                  '$SourceDir\ESEPlugin#SocPmc.bin'),
        ('./FlashSettings/FlashConfiguration/SpiDualOutReadEnable',                                 'Yes'),
        ('./FlashSettings/FlashConfiguration/SpiDualIoReadEnable',                                  'Yes'),
        ('./FlashSettings/FlashConfiguration/QuadOutReadEnable',                                    'Yes'),
        ('./FlashSettings/FlashConfiguration/QuadIoReadEnable',                                     'Yes'),
        ('./FlashSettings/RpmcConfiguration/RpmcOverEc',                                            'Yes'),
        ('./NetworkingConnectivity/WiredLanConfiguration/PhyConnected',                             'PHY on SMLink0'),
        ('./IntegratedSensorHub/IshImage/InputFile',                                                '$SourceDir\CsePlugin#ISH.bin'),
        ('./IntegratedSensorHub/IshData/PdtBinary',                                                 '$SourceDir\ish_pdt_binary.bin'),
        ('./Debug/DelayedAuthenticationModeConfiguration/DelayedAuthMode',                          'Yes'),
        ('./Debug/DirectConnectInterfaceConfiguration/DciDbcEnable',                                'Yes'),
        ('./Debug/EarlyUsb2DbcOverType-AConfiguration/Usb2DbcPortEn',                               'USB2 Port 1'),
        ('./Debug/EarlyUsb2DbcOverType-AConfiguration/EnEarlyUsb2DbcCon',                           'Yes'),
        ('./CpuStraps/CpuStraps/NumActiveSmallCores',                                               'Disable All Cores'),
        ('./FlexIO/PciePortConfiguration/PCIeController1',                                          '1x4'),
        ('./FlexIO/PciePortConfiguration/PCIeController2',                                          '1x2, 2x1'),
        ('./FlexIO/PciePortConfiguration/PCIeController3',                                          '1x4'),
        ('./FlexIO/PciePortConfiguration/PCIeController6',                                          '1x4 Lane Reversed'),
        ('./FlexIO/SataPcieComboPortConfiguration/SataPcieComboPorts4_7',                           'SATA (Ports 17-20)'),
        ('./FlexIO/Usb3PortConfiguration/Usb32Port3and4SpdselPair',                                 'USB 3.2 Port 3 and 4 Gen 2x1'),
        ('./FlexIO/Usb3PortConfiguration/Usb32Port5and6SpdselPair',                                 'USB 3.2 Port 5 and 6 Gen 2x1'),
        ('./FlexIO/Usb3PortConfiguration/Usb32Port7and8SpdselPair',                                 'USB 3.2 Port 7 and 8 Gen 2x1'),
        ('./FlexIO/Usb3PortConfiguration/USB3Prt3ConTypeSel',                                       'Type A / Type C'),
        ('./FlexIO/Usb3PortConfiguration/USB3Prt4ConTypeSel',                                       'Type A / Type C'),
        ('./FlexIO/Usb3PortConfiguration/USB3Prt5ConTypeSel',                                       'Type A / Type C'),
        ('./FlexIO/Usb3PortConfiguration/USB3Prt6ConTypeSel',                                       'Type A / Type C'),
        ('./FlexIO/Usb3PortConfiguration/USB3Prt7ConTypeSel',                                       'Type A / Type C'),
        ('./FlexIO/Usb3PortConfiguration/USB3Prt8ConTypeSel',                                       'Type A / Type C'),
        ('./FlexIO/Usb2PortConfiguration/USB2Prt2ConTypeSel',                                       'Type A / Type C'),
        ('./FlexIO/Usb2PortConfiguration/USB2Prt3ConTypeSel',                                       'Type A / Type C'),
        ('./FlexIO/Usb2PortConfiguration/USB2Prt4ConTypeSel',                                       'Type A / Type C'),
        ('./FlexIO/Usb2PortConfiguration/USB2Prt7ConTypeSel',                                       'Type A / Type C'),
        ('./FlexIO/Usb2PortConfiguration/USB2Prt8ConTypeSel',                                       'Type A / Type C'),
        ('./FlexIO/Usb2PortConfiguration/USB2Prt10ConTypeSel',                                      'Type A / Type C'),
        ('./FlexIO/Usb2PortConfiguration/USB2Prt11ConTypeSel',                                      'Type A / Type C'),
        ('./FlexIO/Usb2PortConfiguration/USB2Prt12ConTypeSel',                                      'Type A / Type C'),
        ('./FlexIO/Usb2PortConfiguration/USB2Prt13ConTypeSel',                                      'Type A / Type C'),
        ('./FlexIO/Usb2PortConfiguration/USB2Prt14ConTypeSel',                                      'Type A / Type C'),
        ('./FlexIO/PhyConfiguration/PhyBinaryFile',                                                 '$SourceDir\CsePlugin#SPHY.bin'),
        ('./FlexIO/Type-CSubsystemConfiguration/IomBinaryFile',                                     '$SourceDir\CsePlugin#IOM.bin'),
        ('./FlexIO/Type-CSubsystemConfiguration/PhyBinaryFile',                                     '$SourceDir\CsePlugin#NPHY.bin'),
        ('./FlexIO/Type-CSubsystemConfiguration/TbtBinaryFile',                                     '$SourceDir\CsePlugin#TBT.bin'),
        ('./FlexIO/ThunderboltConfiguration/TboltEnable',                                           'Yes'),
        ('./FlexIO/PowerDelivery_PdControllerConfiguration/USB3PortForTypeCPort1Cpu',               'Type-C Port 1'),
        ('./FlexIO/PowerDelivery_PdControllerConfiguration/USB3PortForTypeCPort2Cpu',               'Type-C Port 2'),
        ('./FlexIO/PowerDelivery_PdControllerConfiguration/Pd3PortOwner',                           'PCH'),
        ('./FlexIO/PowerDelivery_PdControllerConfiguration/USB3PortForTypeCPort3Cpu',               'N/A'),
        ('./FlexIO/PowerDelivery_PdControllerConfiguration/USB3PortForTypeCPort3Pch',               'USB 3.2 Port 1'),
        ('./FlexIO/PowerDelivery_PdControllerConfiguration/Pd4PortOwner',                           'PCH'),
        ('./FlexIO/PowerDelivery_PdControllerConfiguration/USB3PortForTypeCPort4Cpu',               'N/A'),
        ('./FlexIO/PowerDelivery_PdControllerConfiguration/USB3PortForTypeCPort4Pch',               'USB 3.2 Port 9'),
        ('./FlexIO/PowerDelivery_PdControllerConfiguration/Pd1TypecUsb32Mode',                      'Yes'),
        ('./FlexIO/PowerDelivery_PdControllerConfiguration/Pd1TypecUsb32RetimerEnabled',            'Yes'),
        ('./FlexIO/PowerDelivery_PdControllerConfiguration/Pd1TypecUsb32RetimerConfig',             'Yes'),
        ('./FlexIO/PowerDelivery_PdControllerConfiguration/Pd1TypecUsb32SMBusAddr',                 '0x38'),
        ('./FlexIO/PowerDelivery_PdControllerConfiguration/Pd1Usb2PortForTypecUsb32',               'USB2 Port 1'),
        ('./FlexIO/PowerDelivery_PdControllerConfiguration/Pd2TypecUsb32Mode',                      'Yes'),
        ('./FlexIO/PowerDelivery_PdControllerConfiguration/Pd2TypecUsb32RetimerEnabled',            'Yes'),
        ('./FlexIO/PowerDelivery_PdControllerConfiguration/Pd2TypecUsb32RetimerConfig',             'Yes'),
        ('./FlexIO/PowerDelivery_PdControllerConfiguration/Pd2TypecUsb32SMBusAddr',                 '0x3F'),
        ('./FlexIO/PowerDelivery_PdControllerConfiguration/Pd2Usb2PortForTypecUsb32',               'USB2 Port 6'),
        ('./FlexIO/PowerDelivery_PdControllerConfiguration/Pd3TypecUsb32Mode',                      'Yes'),
        ('./FlexIO/PowerDelivery_PdControllerConfiguration/Pd3TypecUsb32SMBusAddr',                 '0xC'),
        ('./FlexIO/PowerDelivery_PdControllerConfiguration/Pd3Usb2PortForTypecUsb32',               'USB2 Port 9'),
        ('./FlexIO/PowerDelivery_PdControllerConfiguration/Pd4TypecUsb32Mode',                      'Yes'),
        ('./FlexIO/PowerDelivery_PdControllerConfiguration/Pd4TypecUsb32SMBusAddr',                 '0x1C'),
        ('./FlexIO/PowerDelivery_PdControllerConfiguration/Pd4Usb2PortForTypecUsb32',               'USB2 Port 5'),
        ('./Gpio/GpioVccioVoltageControl/GppImstrVoltSelect',                                       '1.8Volts'),
        ('./Gpio/GpioVccioVoltageControl/GppDmstrVoltSelect',                                       '1.8Volts'),
        ('./Gpio/GpioVccioVoltageControl/HdaVoltSelect',                                            '1.8Volts'),
    ])

    return xml_change_list

