## @ StitchIfwiConfig.py
#  This is an IFWI stitch config script for RPL Slim Bootloader
#
# Copyright (c) 2021 - 2023, Intel Corporation. All rights reserved. <BR>
# SPDX-License-Identifier: BSD-2-Clause-Patent
#
##
import sys
import os

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
       Assuming stitching workspace is at D:\Stitch and building ifwi for RPL platform
       To stitch IFWI with SPI QUAD mode and Boot Guard profile VM:
         StitchIfwi.py -b vm -p rpls -w D:\Stitch -s Stitch_Components.zip -c StitchIfwiConfig.py

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
      ('IbbFlags',              '0x12'),
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
        'rpls'    :   'Intel(R) AlderLake S Chipset - Consumer - SPI - B0',
    }
    return platform_sku

def get_component_replace_list(plt_params_list):
    replace_list = [
       #    Path                   file name              compress    Key                           SVN
      ('IFWI/BIOS/TS0/ACM0',      'Input/acm0.bin',        'dummy',    '',                            ''),
      ('IFWI/BIOS/TS1/ACM0',      'Input/acm0.bin',        'dummy',    '',                            ''),
       ]

    if 'tsn' in plt_params_list:
        if os.path.exists('IPFW/TsnSubRegion.bin'):
            print ("TsnSubRegion.bin found")
            replace_list.append (
                ('IFWI/BIOS/NRD/IPFW/TMAC', 'IPFW/TsnSubRegion.bin',     'lz4',     'KEY_ID_CONTAINER_COMP_RSA3072', 0),   # TSN MAC Address
            )
    if 'tcc' in plt_params_list:
      if os.path.exists('IPFW/TccCrlBinary.bin'):
          print ("TccCrlBinary.bin found")
          replace_list.append (
              ('IFWI/BIOS/NRD/IPFW/TCCM', 'IPFW/TccCrlBinary.bin',         'lz4',     'KEY_ID_CONTAINER_COMP_RSA3072', 0),   # TCC CRL binary
          )
      if os.path.exists('IPFW/TccCacheCfg.bin'):
          print ("TccCacheCfg.bin found")
          replace_list.append (
              ('IFWI/BIOS/NRD/IPFW/TCCC', 'IPFW/TccCacheCfg.bin',         'lz4',     'KEY_ID_CONTAINER_COMP_RSA3072', 0),   # TCC Cache Cfg binary
          )
      if os.path.exists('IPFW/TccStreamCfg.bin'):
          print ("TccStreamCfg.bin found")
          replace_list.append (
              ('IFWI/BIOS/NRD/IPFW/TCCT', 'IPFW/TccStreamCfg.bin',         'lz4',     'KEY_ID_CONTAINER_COMP_RSA3072', 0),   # TCC StreamCfg binary
          )

    return replace_list

def check_parameter(para_list):
    print (para_list)
    para_supported = {
        's14'    : {},
        's17'    : {},
        'debug'  : {},
        'spi'    : {'25MHz', '33MHz', '50MHz', '100MHz'},
        'tsn'    : {'1G1G', '1G25G', '25G1G'},
        'tcc'     : {}
        }

    para_help = \
        """
        's14'  -- Stitch for S14 board
        's17'  -- Stitch for S17 board
        'tsn'  -- Enable TSN softstraps with different linkspeeds ('1G1G', '1G2.5G', '2.5G1G')
        'debug'-- Enable DAM and DCI configuration (Only use for debug purpose but not for final production!)
        'spi'  -- Set SPI frequency to be '25MHz', '33MHz', '50MHz' or '100MHz'. By default use 25Mhz.
        'tcc'  -- Stitch TCC binaries into BIOS region.
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

        if para == 'tsn':
            if para_list[para] == None:
                print ("TSN option expects a linkspeed value")
                print (para_help)
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
      # Path                                                                         | value |
      # =========================================================================================
        ('./FlashLayout/BiosRegion/InputFile',                                       '$SourceDir\BiosRegion.bin'),
        ('./FlashLayout/Ifwi_IntelMePmcRegion/MeRegionFile',                         '$SourceDir\MeRegionFile.bin'),
        ('./FlashLayout/Ifwi_IntelMePmcRegion/PmcBinary',                            '$SourceDir\PmcBinary.bin'),
        ('./FlashLayout/EcRegion/EcRegionPointer',                                   '$SourceDir\EcRegionPointer.bin'),
        ('./FlashLayout/EcRegion/InputFile',                                         '$SourceDir\EcRegion.bin'),
        ('./FlashLayout/EcRegion/Enabled',                                           'Enabled'),
        ('./FlashLayout/GbeRegion/InputFile',                                        '$SourceDir\GbeRegion.bin'),
        ('./FlashLayout/GbeRegion/Enabled',                                          'Enabled'),
        ('./FlashLayout/SubPartitions/PchcSubPartitionData/InputFile',               '$SourceDir\PchcSubPartitionData.bin'),
        ('./FlashSettings/FlashConfiguration/SpiDualOutReadEnable',                  'Yes'),
        ('./FlashSettings/FlashConfiguration/FastReadClockFreq',                     '25MHz'),
        ('./FlashSettings/FlashConfiguration/ReadIdAndReadStatClkFreq',              '25MHz'),
        ('./FlashSettings/FlashConfiguration/WriteEraseClockFreq',                   '25MHz'),
        ('./FlashSettings/VsccTable/VsccEntries/VsccEntry/VsccEntryName',            'VsccEntry0'),
        ('./FlashSettings/BiosConfiguration/BiosRedAssistance',                      'Disabled'),
        ('./PlatformProtection/PlatformIntegrity/OemExtInputFile',                   '$SourceDir\OemExtInputFile.bin'),
        ('./PlatformProtection/PlatformIntegrity/OemPublicKeyHash',                  'F8 F0 E3 69 15 81 76 99 0A 54 9E D4 C3 6D 1A 86 39 D8 87 3D EF F7 ED 2D E3 4C B4 1B CC B3 04 76 CE 0A A0 63 BC 5B 7A AC FF D9 50 9E 96 40 C6 99'),
        ('./PlatformProtection/BootGuardConfiguration/BtGuardKeyManifestId',         '0x1'),
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
        ('./Icc/IccPolicies/Profiles/Profile/ClockOutputConfiguration/ClkoutSRC10',  'Enabled'),
        ('./Icc/IccPolicies/Profiles/Profile/ClockOutputConfiguration/ClkoutSRC11',  'Enabled'),
        ('./Icc/IccPolicies/Profiles/Profile/ClockOutputConfiguration/ClkoutSRC12',  'Enabled'),
        ('./Icc/IccPolicies/Profiles/Profile/ClockOutputConfiguration/ClkoutSRC13',  'Enabled'),
        ('./Icc/IccPolicies/Profiles/Profile/ClockOutputConfiguration/ClkoutSRC14',  'Enabled'),
        ('./Icc/IccPolicies/Profiles/Profile/ClockOutputConfiguration/ClkoutSRC15',  'Enabled'),
        ('./Icc/IccPolicies/Profiles/Profile/ClockOutputConfiguration/ClkoutSRC16',  'Enabled'),
        ('./Icc/IccPolicies/Profiles/Profile/ClockOutputConfiguration/ClkoutSRC17',  'Enabled'),
        ('./Icc/IccPolicies/Profiles/Profile/PwrManagementConfiguration/ClkreqMapSRC2', 'GPP_H_5'),
        ('./Icc/IccPolicies/Profiles/Profile/PwrManagementConfiguration/ClkreqMapSRC4', 'GPP_H_4'),
        ('./Icc/IccPolicies/Profiles/Profile/PwrManagementConfiguration/ClkreqMapSRC6', 'GPP_H_7'),
        ('./Icc/IccPolicies/Profiles/Profile/PwrManagementConfiguration/ClkreqMapSRC7', 'GPP_H_6'),
        ('./Icc/IccPolicies/Profiles/Profile/PwrManagementConfiguration/ClkreqMapSRC9', 'GPP_H_5'),
        ('./Icc/IccPolicies/Profiles/Profile/PwrManagementConfiguration/ClkreqMapSRC10', 'GPP_D_11'),
        ('./Icc/IccPolicies/Profiles/Profile/PwrManagementConfiguration/ClkreqMapSRC11', 'GPP_H_3'),
        ('./Icc/IccPolicies/Profiles/Profile/PwrManagementConfiguration/ClkreqMapSRC13', 'GPP_D_13'),
        ('./Icc/IccPolicies/Profiles/Profile/PwrManagementConfiguration/ClkreqMapSRC14', 'GPP_J_8'),
        ('./Icc/IccPolicies/Profiles/Profile/PwrManagementConfiguration/ClkreqMapSRC16', 'GPP_H_8'),
        ('./Icc/IccPolicies/Profiles/Profile/PwrManagementConfiguration/ClkreqMapSRC17', 'GPP_D_2'),
        ('./NetworkingConnectivity/WiredLanConfiguration/PhyConnected',              'PHY on SMLink0'),
        ('./IntegratedSensorHub/IshImage/InputFile',                                 '$SourceDir\IshImage.bin'),
        ('./IntegratedSensorHub/IshData/PdtBinary',                                  '$SourceDir\PdtBinary.bin'),
        ('./Debug/IntelMeFirmwareDebuggingOverrides/DbgOverridePreProdSi',           '0x7'),
        ('./Debug/IntelMeFirmwareDebuggingOverrides/DbgOverrideProdSi',              '0x7'),
        ('./Debug/DirectConnectInterfaceConfiguration/Usb1DciOobEnable',             'No'),
        ('./Debug/DirectConnectInterfaceConfiguration/Usb2DciOobEnable',             'No'),
        ('./Debug/DirectConnectInterfaceConfiguration/Usb3DciOobEnable',             'No'),
        ('./Debug/DirectConnectInterfaceConfiguration/Usb4DciOobEnable',             'No'),
        ('./Debug/DirectConnectInterfaceConfiguration/Usb5DciOobEnable',             'No'),
        ('./Debug/DirectConnectInterfaceConfiguration/Usb6DciOobEnable',             'No'),
        ('./Debug/DirectConnectInterfaceConfiguration/Usb7DciOobEnable',             'No'),
        ('./Debug/DirectConnectInterfaceConfiguration/Usb8DciOobEnable',             'No'),
        ('./Debug/DirectConnectInterfaceConfiguration/Usb9DciOobEnable',             'No'),
        ('./Debug/DirectConnectInterfaceConfiguration/Usb10DciOobEnable',            'No'),
        ('./CpuStraps/CpuStraps/RplPlatformEn',                                      'Yes'),
        ('./FlexIO/PciePortConfiguration/PCIeController5Config',                     '1x4'),
        ('./FlexIO/SataPcieComboPortConfiguration/SataPCIeComboPortCustom0_3',       'GPIO Polarity PCIe'),
        ('./FlexIO/PhyConfiguration/PhyBinaryFile',                                  '$SourceDir\PhyBinaryFile.bin'),
        ('./FlexIO/Type-CSubsystemConfiguration/PhyBinaryFile',                      '$SourceDir\CsePlugin#NPHY.bin'),
        ('./Gpio/GpioModeControl/BattLowGpd0Config',                                 'Enable as GPD0'),
        ('./Gpio/GpioVccioVoltageControl/GppDmstrVoltSelect',                        '1.8Volts'),
    ])

    if 's17' in plt_params_list:
        xml_change_list.append ([
        ('./BuildSettings/HarnessGlobalData/SelectedRvp',                            'RPL-S DDR5 SODIMM S17 (ADP-S + RPL-S)'),
        ('./FlashSettings/FlashConfiguration/SpiDualIoReadEnable',                   'Yes'),
        ('./Icc/IccPolicies/Profiles/Profile/PwrManagementConfiguration/ClkreqMapSRC12', 'GPP_D_14'),
        ('./NetworkingConnectivity/WiredLanConfiguration/GbeMacSmbAddrsEn',          'No'),
        ('./NetworkingConnectivity/WiredLanConfiguration/GbePCIePortSelect',         'None'),
        ('./InternalPchBuses/SmbusSmlinkConfiguration/SLink0MctpAddress',            '0x62'),
        ('./InternalPchBuses/SmbusSmlinkConfiguration/SLink0MctpEnable',             'Yes'),
        ('./FlexIO/PciePortConfiguration/PCIeController2Config',                     '4x1'),
        ('./IntelUniquePlatformId/EntitlementsConfiguration/IcpsSwSkuing',           'Yes'),
        ])

    if 's14' in plt_params_list:
        xml_change_list.append ([
        ('./BuildSettings/HarnessGlobalData/SelectedRvp',                            'RPL-S DDR5 UDIMM S-14 (ADP-S + RPL-S)'),
        ('./PlatformProtection/ContentProtection/PavpSupported',                     'No'),
        ('./Icc/IccPolicies/Profiles/Profile/PwrManagementConfiguration/ClkreqMapSRC12', 'GPP_D_11'),
        ('./FlexIO/PciePortConfiguration/PCIeController1Config',                     '1x2, 2x1'),
        ('./FlexIO/PciePortConfiguration/PCIeController2Config',                     '1x4'),
        ('./FlexIO/PowerDelivery_PdControllerConfiguration/TypeCPort1SmbusAddr',     '0x3F'),
        ('./FlexIO/PowerDelivery_PdControllerConfiguration/USB2PortForTypeCPort1',   'USB2 Port 1'),
        ('./FlexIO/PowerDelivery_PdControllerConfiguration/USB3PortForTypeCPort1',   'Type-C Port 7'),
        ('./FlexIO/PowerDelivery_PdControllerConfiguration/TypeCPort2SMBusAddr',     '0x38'),
        ('./FlexIO/PowerDelivery_PdControllerConfiguration/USB2PortForTypeCPort2',   'USB2 Port 9'),
        ('./FlexIO/PowerDelivery_PdControllerConfiguration/USB3PortForTypeCPort2',   'Type-C Port 1'),
        ('./Gpio/GpioModeControl/GppG5SlpDramConfig',                                'Enable as GPP_G5'),
        ])

    if 'tsn' in plt_params_list:
        tsn = plt_params_list.get('tsn')
        print ("Applying changes to set tsn setting to %s" % tsn)
        if tsn == '1G1G':
            xml_change_list.append ([
              ('./NetworkingConnectivity/TimeSensitiveNetworkingConfiguration/TsnEnabled', 'TSN  1 and TSN 2 Enabled'),
              ('./NetworkingConnectivity/TimeSensitiveNetworkingConfiguration/TsnSpeedSelect', 'TSN 1 1 Gig and TSN 2 1 Gig'),
              ])
        elif tsn == '1G25G':
            xml_change_list.append ([
              ('./NetworkingConnectivity/TimeSensitiveNetworkingConfiguration/TsnEnabled', 'TSN  1 and TSN 2 Enabled'),
              ('./NetworkingConnectivity/TimeSensitiveNetworkingConfiguration/TsnSpeedSelect', 'TSN 1 1 Gig and TSN 2 2.5 Gig'),
              ])
        elif tsn == '25G1G':
            xml_change_list.append ([
              ('./NetworkingConnectivity/TimeSensitiveNetworkingConfiguration/TsnEnabled', 'TSN  1 and TSN 2 Enabled'),
              ('./NetworkingConnectivity/TimeSensitiveNetworkingConfiguration/TsnSpeedSelect', 'TSN 1 2.5 Gig and TSN 2 1 Gig'),
              ])

    if 'spi' in plt_params_list:
        frequency = plt_params_list.get('spi')
        print ("Applying changes to set SPI frequency to %s" % frequency)
        xml_change_list.append ([
            ('./FlashSettings/FlashConfiguration/FastReadClockFreq',                     frequency),
            ('./FlashSettings/FlashConfiguration/ReadIdAndReadStatClkFreq',              frequency),
            ('./FlashSettings/FlashConfiguration/WriteEraseClockFreq',                   frequency),
            ])

    if 'debug' in plt_params_list:
        print ("Applying changes to enable DAM")
        xml_change_list.append ([
            ('./Debug/DelayedAuthenticationModeConfiguration/DelayedAuthMode',           'Yes'),
            ])

    return xml_change_list
