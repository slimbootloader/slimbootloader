## @ StitchIfwiConfig.py
#  This is an IFWI stitch config script for ADL Slim Bootloader
#
# Copyright (c) 2021 - 2022, Intel Corporation. All rights reserved. <BR>
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
       Assuming stitching workspace is at D:\Stitch and building ifwi for ADL platform
       To stitch IFWI with SPI QUAD mode and Boot Guard profile VM:
         StitchIfwi.py -b vm -p adl -w D:\Stitch -s Stitch_Components.zip -c StitchIfwiConfig.py

"""

def get_bpmgen2_params_change_list ():
    params_change_list = []
    params_change_list.append ([
      # variable                | value |
      # ===================================
      ('PlatformRules',         'ADL Client'),
      ('BpmStrutVersion',       '0x20'),
      ('BpmRevision',           '0x01'),
      ('BpmRevocation',         '1'),
      ('AcmRevocation',         '2'),
      ('NEMPages',              '3'),
      ('AcpiBase',              '0x1800'),
      ('IbbFlags',              '0x2'),
      ('IbbHashAlgID',          '0x0C:SHA384'),
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
        'adlp'    :   'Intel(R) AlderLake P Chipset - Consumer - SPI',
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
        'ddr5'   : {},
        'ddr4'   : {},
        'lp5'    : {},
        'lp4'    : {},
        'quad'   : {},
        'debug'  : {},
        'sata'   : {},
        'ufs'   : {},
       }

    para_help = \
        """
        'ddr5'   -- Stitch for DDR5 board, default configuration is set for LP4 board
        'ddr4'   -- Stitch for DDR4 board, default configuration is set for LP4 board
        'lp5'    -- Stitch for LP5 board, default configuration is set for LP4 board
        'debug'  -- Enable DAM and DCI configuration (Only use for debug purpose but not for final production!)
        'sata'   -- Enable direct SATA ports, these are disabled by default
        'ufs'    -- Enable Ufs Controllers, these are disabled by default
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
        ('./BuildSettings/BuildResults/RegionOrder',                                '53241'),
        ('./FlashLayout/BiosRegion/InputFile',                                      '$SourceDir\BiosRegion.bin'),
        ('./FlashLayout/Ifwi_IntelMePmcRegion/MeRegionFile',                        '$SourceDir\MeRegionFile.bin'),
        ('./FlashLayout/Ifwi_IntelMePmcRegion/PmcBinary',                           '$SourceDir\PmcBinary.bin'),
        ('./FlashLayout/EcRegion/InputFile',                                        '$SourceDir\EcRegion.bin'),
        ('./FlashLayout/EcRegion/Enabled',                                          'Enabled'),
        ('./FlashLayout/EcRegion/EcRegionPointer',                                  '$SourceDir\EcRegionPointer.bin'),
        ('./FlashLayout/GbeRegion/InputFile',                                       '$SourceDir\GbeRegion.bin'),
        ('./FlashLayout/GbeRegion/Enabled',                                         'Enabled'),
        ('./FlashLayout/SubPartitions/PchcSubPartitionData/InputFile',              '$SourceDir\PchcSubPartitionData.bin'),
        ('./FlashSettings/FlashConfiguration/SpiDualIoReadEnable',                   'Yes'),
        ('./FlashSettings/FlashConfiguration/SpiDualOutReadEnable',                  'Yes'),
        ('./FlashSettings/BiosConfiguration/TopSwapOverride',                        '512KB'),
        ('./PlatformProtection/PlatformIntegrity/OemPublicKeyHash',                  'F8 F0 E3 69 15 81 76 99 0A 54 9E D4 C3 6D 1A 86 39 D8 87 3D EF F7 ED 2D E3 4C B4 1B CC B3 04 76 CE 0A A0 63 BC 5B 7A AC FF D9 50 9E 96 40 C6 99'),
        ('./PlatformProtection/PlatformIntegrity/OemExtInputFile',                   '$SourceDir\OemExtInputFile.bin'),
        ('./PlatformProtection/ContentProtection/PavpSupported',                     'Yes'),
        ('./PlatformProtection/BootGuardConfiguration/BtGuardKeyManifestId',         '0x1'),
        ('./PlatformProtection/BiosGuardConfiguration/BiosGrdProtOvrdEn',            'Yes'),
        ('./PlatformProtection/TpmOverSpiBusConfiguration/SpiOverTpmBusEnable',      'Yes'),
        ('./Icc/IccPolicies/Profiles/Profile/ClockOutputConfiguration/ClkoutSRC0',   'Enabled'),
        ('./Icc/IccPolicies/Profiles/Profile/ClockOutputConfiguration/ClkoutSRC1',   'Enabled'),
        ('./Icc/IccPolicies/Profiles/Profile/ClockOutputConfiguration/ClkoutSRC2',   'Enabled'),
        ('./Icc/IccPolicies/Profiles/Profile/ClockOutputConfiguration/ClkoutSRC3',   'Enabled'),
        ('./Icc/IccPolicies/Profiles/Profile/ClockOutputConfiguration/ClkoutSRC4',   'Enabled'),
        ('./Icc/IccPolicies/Profiles/Profile/ClockOutputConfiguration/ClkoutSRC5',   'Enabled'),
        ('./Icc/IccPolicies/Profiles/Profile/ClockOutputConfiguration/ClkoutSRC6',   'Enabled'),
        ('./Icc/IccPolicies/Profiles/Profile/ClockOutputConfiguration/ClkoutSRC7',   'Enabled'),
        ('./Icc/IccPolicies/Profiles/Profile/ClockOutputConfiguration/ClkoutSRC8',   'Enabled'),
        ('./Icc/IccPolicies/Profiles/Profile/ClockOutputConfiguration/ClkoutSRC9',   'Enabled'),
        ('./NetworkingConnectivity/WiredLanConfiguration/PhyConnected',              'PHY on SMLink0'),
        ('./NetworkingConnectivity/WiredLanConfiguration/GbePCIePortSelect',         'Port7'),
        ('./NetworkingConnectivity/WirelessLanConfiguration/MeClinkEnable',          'No'),
        ('./IntegratedSensorHub/IntegratedSensorHub/IshPowerUpState',                'Enabled'),
        ('./IntegratedSensorHub/IshImage/InputFile',                                 '$SourceDir\IshImage.bin'),
        ('./IntegratedSensorHub/IshData/PdtBinary',                                  '$SourceDir\PdtBinary.bin'),
        ('./Camera/IPUSecurity/SecureTouchCameraMask',                               '0x0'),
        ('./Debug/DelayedAuthenticationModeConfiguration/DelayedAuthMode',           'No'),
        ('./Debug/IntelMeFirmwareDebuggingOverrides/DbgOverridePreProdSi',           '0x7'),
        ('./Debug/DirectConnectInterfaceConfiguration/DciDbcEnable',                 'No'),
        ('./Debug/DirectConnectInterfaceConfiguration/Usb1DciOobEnable',             'No'),
        ('./Debug/DirectConnectInterfaceConfiguration/Usb2DciOobEnable',             'No'),
        ('./FlexIO/Type-CSubsystemConfiguration/IomBinaryFile',                      '$SourceDir\Iom_L0R0.bin'),
        ('./FlexIO/Type-CSubsystemConfiguration/PhyBinaryFile',                      '$SourceDir\TypeC_NorthPHYRegion_L1.bin'),
        ('./FlexIO/Type-CSubsystemConfiguration/TbtBinaryFile',                      '$SourceDir\TBT.bin'),
        ('./FlexIO/Type-CSubsystemConfiguration/IomBinaryFile',                      '$SourceDir\Iom.bin'),
        ('./FlexIO/Type-CSubsystemConfiguration/PhyBinaryFile',                      '$SourceDir\TypeC_NorthPHYRegion.bin'),
        ('./IntelUniquePlatformId/EntitlementsConfiguration/IcpsSwSkuing',           'Yes'),
        ('./FlexIO/Type-CSubsystemConfiguration/XdciSplitDieConfig',                 'xDCI Split Die Disabled'),
    ])

    if 'lp5' in plt_params_list:
        print ("Applying changes to enable lp5 board")
        xml_change_list.append ([
          ('./BuildSettings/HarnessGlobalData/SelectedRvp',                            'ADL-P LP5 (ADL-P + ADP-P)'),
          ('./CpuStraps/CpuStraps/Mipi124RailSrcPlat',                                 'Yes'),
          ('./FlexIO/Usb2PortConfiguration/USB2Prt4ConTypeSel',                        'Type C'),
          ('./FlexIO/Usb2PortConfiguration/USB2Prt5ConTypeSel',                        'Type A / Type C'),
          ('./FlexIO/Type-CSubsystemConfiguration/TypeCPort3Config',                   'No Thunderbolt'),
          ('./FlexIO/PowerDelivery_PdControllerConfiguration/TypeCPort3RetimerEnabled','No'),
          ('./FlexIO/PowerDelivery_PdControllerConfiguration/USB2PortForTypeCPort4',   'USB2 Port 4'),
        ])

    if 'lp4' in plt_params_list:
        print ("Applying changes to enable lp4 board")

    if 'ddr5' in plt_params_list:
        print ("Applying changes to enable ddr5 board")
        xml_change_list.append ([
          ('./BuildSettings/HarnessGlobalData/SelectedRvp',                                 'ADL-P DDR5 (ADL-P + ADP-P)'),
          ('./IntegratedSensorHub/IshData/PdtBinary',                                       '$SourceDir\PdtBinary_BOM2.bin'),
          ('./FlexIO/PowerDelivery_PdControllerConfiguration/TypeCPort1SmbusAddr',          '0x50'),
          ('./FlexIO/PowerDelivery_PdControllerConfiguration/TypeCPort2SMBusAddr',          '0x51'),
          ('./FlexIO/PowerDelivery_PdControllerConfiguration/TypeCPort3SmbusAddr',          '0x52'),
          ('./FlexIO/PowerDelivery_PdControllerConfiguration/TypeCPort4SmbusAddr',          '0x53'),
          ('./Gpio/GpioVccioVoltageControl/GppH15voltSelect',                               '3.3Volts'),
          ('./Gpio/GpioVccioVoltageControl/GppA21voltSelect',                               '1.8Volts'),
        ])

    if 'ddr4' in plt_params_list:
        print ("Applying changes to enable ddr4 board")
        xml_change_list.append ([
          ('./BuildSettings/HarnessGlobalData/SelectedRvp',                                 'ADL-P DDR4 (ADL-P + ADP-P)'),
          ('./IntegratedSensorHub/IshData/PdtBinary',                                       '$SourceDir\PdtBinary_BOM2.bin'),
          ('./FlexIO/SataPcieComboPortConfiguration/SataPCIeComboPort1',                    'GPIO Polarity PCIe'),
          ('./FlexIO/Usb2PortConfiguration/USB2Prt3ConTypeSel',                             'Type A / Type C'),
          ('./FlexIO/Type-CSubsystemConfiguration/TypeCPort4Config',                        'DP Fixed Connection'),
          ('./FlexIO/PowerDelivery_PdControllerConfiguration/TypeCPort1SmbusAddr',          '0x50'),
          ('./FlexIO/PowerDelivery_PdControllerConfiguration/TypeCPort2SMBusAddr',          '0x51'),
          ('./FlexIO/PowerDelivery_PdControllerConfiguration/TypeCPort3Mode',               'No'),
          ('./FlexIO/PowerDelivery_PdControllerConfiguration/TypeCPort3SmbusAddr',          '0x52'),
          ('./FlexIO/PowerDelivery_PdControllerConfiguration/TypeCPort4RetimerEnabled',     'No'),
          ('./FlexIO/PowerDelivery_PdControllerConfiguration/TypeCPort4SmbusAddr',          '0x53'),
          ('./Gpio/GpioVccioVoltageControl/GppH15voltSelect',                               '3.3Volts'),
          ('./Gpio/GpioVccioVoltageControl/GppA21voltSelect',                               '1.8Volts'),
          ('./Gpio/ThunderboltLsxBssb-LsConfiguration/TbltLsxBssbLs2Cfg',                   'Legacy VCCIO'),
        ])

    if 'quad' in plt_params_list:
        print ("Applying changes to enable spi quad")
        xml_change_list.append ([
            ('./FlashSettings/FlashConfiguration/QuadOutReadEnable',                       'Yes'),
            ('./FlashSettings/FlashConfiguration/QuadIoReadEnable',                        'Yes'),
            ('./InternalPchBuses/EspiConfiguration/EspiEcBusfreq',                         '60MHZ'),
            ('./InternalPchBuses/EspiConfiguration/EspiEcMaxIoMode',                       'Single, Dual and Quad'),
            ])

    if 'debug' in plt_params_list:
        print ("Applying changes to enable DAM and DCI")
        xml_change_list.append ([
            ('./Debug/DelayedAuthenticationModeConfiguration/DelayedAuthMode',             'Yes'),
            ('./PlatformProtection/BootGuardConfiguration/BtGuardCpuDebugEnable',          'Enabled'),
            ('./Debug/DirectConnectInterfaceConfiguration/DciDbcEnable',                   'Yes'),
            ('./Debug/DirectConnectInterfaceConfiguration/Usb1DciOobEnable',               'Yes'),
            ('./Debug/DirectConnectInterfaceConfiguration/Usb2DciOobEnable',               'Yes'),
            ('./Debug/DirectConnectInterfaceConfiguration/Usb3DciOobEnable',               'Yes'),
            ('./Debug/DirectConnectInterfaceConfiguration/Usb4DciOobEnable',               'Yes'),
            ])

    if 'sata' in plt_params_list:
        print ("Applying changes to enable SATA port")
        xml_change_list.append ([
            ('./FlexIO/PciePortConfiguration/PCIeController3Config',                       '1x2, 2x1'),
            ('./FlexIO/SataPcieComboPortConfiguration/SataPCIeComboPort0',                 'SATA'),
            ('./FlexIO/SataPcieComboPortConfiguration/SataPCIeComboPort1',                 'SATA'),
            ])

    if 'ufs' in plt_params_list:
        print ("Applying changes to enable UFS Controllers")
        xml_change_list.append ([
            ('./FlexIO/UfsConfiguration/UfsCont1Config',                                   'X2'),
            ])

    return xml_change_list

