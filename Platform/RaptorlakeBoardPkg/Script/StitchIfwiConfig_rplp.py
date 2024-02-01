## @ StitchIfwiConfig.py
#  This is an IFWI stitch config script for RPL Slim Bootloader
#
# Copyright (c) 2023, Intel Corporation. All rights reserved. <BR>
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
         StitchIfwi.py -b vm -p rplp -w D:\Stitch -s Stitch_Components.zip -c StitchIfwiConfig.py

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
        'rplp'    :   'Intel(R) AlderLake P Chipset - Consumer - SPI',
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
        'lp4'    : {},
        'lp5'    : {},
        'ddr5'   : {},
        'crb'    : {},
        'sata'  : {},
        'spi'    : {'25MHz', '33MHz', '50MHz', '100MHz'},
        'tsn'   : {'PCIe7','PCIe8'},
        'dual'  : {},
        '64MB'  : {},
        'debug' : {},
        'posc'  : {}
        }

    para_help = \
        """
        'lp4'  -- Stitch for DDRLP4 board
        'lp5'  -- Stitch for LPDDR5 board
        'ddr5' -- Stitch for DDR5 board
        'crb'  -- Stitch for DDR5 COM-HPC CRB board
        'sata' -- Enable sata port0 and port1 support
        'spi'  -- Set SPI frequency to be '25MHz', '33MHz', '50MHz' or '100MHz'. By default use 25Mhz.
        'tsn'  -- Enable TSN softstraps for different TSN ports (either PCIe7 or PCIe8)
        'dual' -- Build For 2 flash components to allow space for FuSa BIST pattern data
        '64MB' -- Build for 64MB component to allow space for FuSa BIST pattern data
        'debug' -- Enable DAM and DCI
        'posc' -- Stitch POSC binary into BIOS region.
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
                print ("TSN option expects a PCIe Port value")
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
        ('./BuildSettings/HarnessGlobalData/SelectedRvp',                            'RPL-P DDR5 (RPL-P + ADP-P)'),
        ('./FlashLayout/BiosRegion/InputFile',                                       '$SourceDir\BiosRegion.bin'),
        ('./FlashLayout/Ifwi_IntelMePmcRegion/MeRegionFile',                         '$SourceDir\MeRegionFile.bin'),
        ('./FlashLayout/Ifwi_IntelMePmcRegion/PmcBinary',                            '$SourceDir\PmcBinary.bin'),
        ('./FlashLayout/Ifwi_IntelMePmcRegion/ChipInitBinary',                       '$SourceDir\ChipInitBinary.bin'),
        ('./FlashLayout/EcRegion/EcRegionPointer',                                   '$SourceDir\EcRegionPointer.bin'),
        ('./FlashLayout/EcRegion/InputFile',                                         '$SourceDir\EcRegion.bin'),
        ('./FlashLayout/EcRegion/Enabled',                                           'Enabled'),
        ('./FlashLayout/GbeRegion/InputFile',                                        '$SourceDir\GbeRegion.bin'),
        ('./FlashLayout/GbeRegion/Enabled',                                          'Enabled'),
        ('./FlashLayout/SubPartitions/IunitSubPartition/InputFile',                  '$SourceDir\IunitSubPartition.bin'),
        ('./FlashLayout/SubPartitions/PchcSubPartitionData/InputFile',               '$SourceDir\PchcSubPartitionData.bin'),
        ('./FlashSettings/FlashConfiguration/QuadOutReadEnable',                     'Yes'),
        ('./FlashSettings/FlashConfiguration/QuadIoReadEnable',                      'Yes'),
        ('./FlashSettings/VsccTable/VsccEntries/VsccEntry/VsccEntryName',            'VsccEntry0'),
        ('./FlashSettings/BiosConfiguration/BiosRedAssistance',                      'Disabled'),
        ('./PlatformProtection/PlatformIntegrity/OemExtInputFile',                   '$SourceDir\OemExtInputFile.bin'),
        ('./PlatformProtection/PlatformIntegrity/OemPublicKeyHash',                  'F8 F0 E3 69 15 81 76 99 0A 54 9E D4 C3 6D 1A 86 39 D8 87 3D EF F7 ED 2D E3 4C B4 1B CC B3 04 76 CE 0A A0 63 BC 5B 7A AC FF D9 50 9E 96 40 C6 99'),
        ('./PlatformProtection/BootGuardConfiguration/BtGuardKeyManifestId',         '0x1'),
        ('./PlatformProtection/BiosGuardConfiguration/BiosGrdProtOvrdEn',            'Yes'),
        ('./Icc/IccPolicies/Profiles/Profile/ClockOutputConfiguration/ClkoutSRC0',   'Enabled'),
        ('./Icc/IccPolicies/Profiles/Profile/ClockOutputConfiguration/ClkoutSRC1',   'Enabled'),
        ('./Icc/IccPolicies/Profiles/Profile/ClockOutputConfiguration/ClkoutSRC2',   'Enabled'),
        ('./Icc/IccPolicies/Profiles/Profile/ClockOutputConfiguration/ClkoutSRC3',   'Enabled'),
        ('./Icc/IccPolicies/Profiles/Profile/ClockOutputConfiguration/ClkoutSRC4',   'Enabled'),
        ('./Icc/IccPolicies/Profiles/Profile/ClockOutputConfiguration/ClkoutSRC5',   'Enabled'),
        ('./Icc/IccPolicies/Profiles/Profile/ClockOutputConfiguration/ClkoutSRC6',   'Enabled'),
        ('./Icc/IccPolicies/Profiles/Profile/ClockOutputConfiguration/ClkoutSRC7',   'Enabled'),
        ('./NetworkingConnectivity/WiredLanConfiguration/PhyConnected',              'PHY on SMLink0'),
        ('./IntegratedSensorHub/IshImage/InputFile',                                 '$SourceDir\IshImage.bin'),
        ('./IntegratedSensorHub/IshData/PdtBinary',                                  '$SourceDir\PdtBinary.bin'),
        ('./Debug/DelayedAuthenticationModeConfiguration/DelayedAuthMode',           'No'),
        ('./FlexIO/Type-CSubsystemConfiguration/IomBinaryFile',                      '$SourceDir\IomBinaryFile.bin'),
        ('./FlexIO/Type-CSubsystemConfiguration/PhyBinaryFile',                      '$SourceDir\PhyBinaryFile.bin'),
        ('./FlexIO/Type-CSubsystemConfiguration/TbtBinaryFile',                      '$SourceDir\TbtBinaryFile.bin'),
        ('./IntelUniquePlatformId/EntitlementsConfiguration/IcpsSwSkuing',           'Yes'),
        ('./FlexIO/Usb3PortConfiguration/USB3Port4SpdCap',                           'USB 3.1 Gen1'),
        ('./FlexIO/Type-CSubsystemConfiguration/XdciSplitDieConfig',                 'xDCI Split Die Disabled'),
        ('./FlashSettings/FlashConfiguration/FastReadClockFreq',                     '25MHz'),
        ('./FlashSettings/FlashConfiguration/ReadIdAndReadStatClkFreq',              '25MHz'),
        ('./FlashSettings/FlashConfiguration/WriteEraseClockFreq',                   '25MHz'),
    ])

    if 'dual' in plt_params_list:
        xml_change_list.append ([
            ('./BuildSettings/BuildResults/NumberOfFlashComponents',                 '2'),
            ('./BuildSettings/BuildResults/FlashComponentsSizes',                    '32,32'),
        ])
        if '64MB' in plt_params_list:
            raise Exception('ERROR: dual 64MB flash not supported!')

    if '64MB' in plt_params_list:
        xml_change_list.append ([
            ('./BuildSettings/BuildResults/FlashComponentsSizes',                    '64'),
    ])

    if 'lp4' in plt_params_list:
        xml_change_list.append ([
        ('./BuildSettings/HarnessGlobalData/SelectedRvp',                            'ADL-P LP4x (ADL-P + ADP-P)'),
        ('./Icc/IccPolicies/Profiles/Profile/ClockOutputConfiguration/ClkoutSRC8',   'Enabled'),
        ('./Icc/IccPolicies/Profiles/Profile/ClockOutputConfiguration/ClkoutSRC9',   'Enabled'),
        ('./NetworkingConnectivity/WirelessLanConfiguration/MeClinkEnable',          'No'),
        ('./Camera/IPUSecurity/SecureTouchCameraMask',                               '0x0'),
        ])

    if 'ddr5' in plt_params_list:
        xml_change_list.append ([
        ('./FlexIO/PowerDelivery_PdControllerConfiguration/TypeCPort1SmbusAddr',     '0x50'),
        ('./FlexIO/PowerDelivery_PdControllerConfiguration/TypeCPort2SMBusAddr',     '0x51'),
        ('./FlexIO/PowerDelivery_PdControllerConfiguration/TypeCPort3SmbusAddr',     '0x52'),
        ('./FlexIO/PowerDelivery_PdControllerConfiguration/TypeCPort4SmbusAddr',     '0x53'),
        ('./Gpio/GpioVccioVoltageControl/GppE7voltSelect',                           '1.8Volts'),
        ('./Gpio/GpioVccioVoltageControl/GppH15voltSelect',                          '3.3Volts'),
        ('./Gpio/GpioVccioVoltageControl/GppA21voltSelect',                          '1.8Volts'),
        ('./Icc/IccPolicies/Profiles/Profile/ClockOutputConfiguration/ClkoutSRC8',   'Enabled'),
        ('./Icc/IccPolicies/Profiles/Profile/ClockOutputConfiguration/ClkoutSRC9',   'Enabled'),
        ('./PlatformProtection/TpmOverSpiBusConfiguration/SpiOverTpmBusEnable',      'No'),

        ])

    if 'crb' in plt_params_list:
        xml_change_list.append ([
        ('./BuildSettings/BuildResults/Sku',                                         'Premium'),
        ('./BuildSettings/HarnessGlobalData/SelectedRvp',                            'RPL-P DDR5 (RPL-P + ADP-P)'),
        ('./FlexIO/PowerDelivery_PdControllerConfiguration/TypeCPort1SmbusAddr',     '0x50'),
        ('./FlexIO/PowerDelivery_PdControllerConfiguration/TypeCPort2SMBusAddr',     '0x0'),
        ('./FlexIO/PowerDelivery_PdControllerConfiguration/TypeCPort3SmbusAddr',     '0x52'),
        ('./FlexIO/PowerDelivery_PdControllerConfiguration/TypeCPort4SmbusAddr',     '0x53'),
        ('./Gpio/GpioVccioVoltageControl/GppE2voltSelect',                           '3.3Volts'),
        ('./Gpio/GpioVccioVoltageControl/GppE10voltSelect',                          '3.3Volts'),
        ('./Gpio/GpioVccioVoltageControl/GppE11voltSelect',                          '3.3Volts'),
        ('./Gpio/GpioVccioVoltageControl/GppE12voltSelect',                          '3.3Volts'),
        ('./Gpio/GpioVccioVoltageControl/GppE13voltSelect',                          '3.3Volts'),
        ('./Gpio/GpioVccioVoltageControl/GppE15voltSelect',                          '1.8Volts'),
        ('./Gpio/GpioVccioVoltageControl/GppE16voltSelect',                          '1.8Volts'),
        ('./Gpio/GpioVccioVoltageControl/GppE17voltSelect',                          '3.3Volts'),
        ('./Gpio/GpioVccioVoltageControl/GppA21voltSelect',                          '1.8Volts'),
        ('./Gpio/GpioVccioVoltageControl/GppF6voltSelect',                           '3.3Volts'),
        ('./Gpio/GpioVccioVoltageControl/GppF11voltSelect',                          '3.3Volts'),
        ('./Gpio/GpioVccioVoltageControl/GppF12voltSelect',                          '3.3Volts'),
        ('./Gpio/GpioVccioVoltageControl/GppF13voltSelect',                          '3.3Volts'),
        ('./Gpio/GpioVccioVoltageControl/GppF16voltSelect',                          '3.3Volts'),
        ('./Gpio/GpioVccioVoltageControl/GppF17voltSelect',                          '3.3Volts'),
        ('./Gpio/GpioVccioVoltageControl/GppF18voltSelect',                          '3.3Volts'),
        ('./Gpio/GpioVccioVoltageControl/GppD1voltSelect',                           '3.3Volts'),
        ('./Gpio/GpioVccioVoltageControl/GppD2voltSelect',                           '3.3Volts'),
        ('./Gpio/GpioVccioVoltageControl/GppD3voltSelect',                           '3.3Volts'),
        ('./Gpio/GpioVccioVoltageControl/GppH3voltSelect',                           '3.3Volts'),
        ('./Gpio/GpioVccioVoltageControl/GppH4voltSelect',                           '3.3Volts'),
        ('./Gpio/GpioVccioVoltageControl/GppH5voltSelect',                           '3.3Volts'),
        ('./Gpio/GpioVccioVoltageControl/GppH8voltSelect',                           '3.3Volts'),
        ('./Gpio/GpioVccioVoltageControl/GppH12voltSelect',                          '3.3Volts'),
        ('./Gpio/GpioVccioVoltageControl/GppH15voltSelect',                          '3.3Volts'),
        ('./Gpio/GpioVccioVoltageControl/GppB7voltSelect',                           '3.3Volts'),
        ('./Gpio/GpioVccioVoltageControl/GppB8voltSelect',                           '3.3Volts'),
        ('./Gpio/GpioVccioVoltageControl/GppB14voltSelect',                          '3.3Volts'),
        ('./Gpio/GpioVccioVoltageControl/GppB15voltSelect',                          '3.3Volts'),
        ('./Gpio/GpioVccioVoltageControl/GppB23voltSelect',                          '3.3Volts'),
        ('./Gpio/GpioVccioVoltageControl/GppT2voltSelect',                           '1.8Volts'),
        ('./Gpio/GpioVccioVoltageControl/GppT3voltSelect',                           '1.8Volts'),
        ('./Icc/IccPolicies/Profiles/Profile/ClockOutputConfiguration/ClkoutSRC1',   'Disabled'),
        ('./Icc/IccPolicies/Profiles/Profile/ClockOutputConfiguration/ClkoutSRC5',   'Disabled'),
        ('./Icc/IccPolicies/Profiles/Profile/ClockOutputConfiguration/ClkoutSRC7',   'Disabled'),
        ('./CpuStraps/CpuStraps/Vccp105CpuPg',                                       'VCCP 1.05 CPU PG Not present'),
        ('./NetworkingConnectivity/WiredLanConfiguration/GbePCIePortSelect',         'Port8'),
        ('./NetworkingConnectivity/WirelessLanConfiguration/CnviWlanCrdEn',          'Disabled'),
        ('./NetworkingConnectivity/WirelessLanConfiguration/MEWlanPowerWell',        'Primary Well || SLP_SUS#'),
        ('./FlashSettings/FlashConfiguration/SpiDualOutReadEnable',                  'Yes'),
        ('./FlashSettings/FlashConfiguration/SpiDualIoReadEnable',                   'Yes'),
        ('./Gpio/ThunderboltLsxBssb-LsConfiguration/TbltLsxBssbLs0Cfg',              'Legacy VCCIO'),
        ('./Gpio/ThunderboltLsxBssb-LsConfiguration/TbltLsxBssbLs1Cfg',              'Legacy VCCIO'),
        ('./Power/DeepSx/DeepSxSupportEnable',                                       'No'),
        ('./FlexIO/PowerDelivery_PdControllerConfiguration/TypeCPort1Mode',          'No'),
        ('./FlexIO/PowerDelivery_PdControllerConfiguration/TypeCPort1RetimerEnabled', 'No'),
        ('./FlexIO/PowerDelivery_PdControllerConfiguration/TypeCPort1SmbusAddr',     '0x0'),
        ('./FlexIO/PowerDelivery_PdControllerConfiguration/TypeCPort2Mode',          'No'),
        ('./FlexIO/PowerDelivery_PdControllerConfiguration/TypeCPort2RetimerEnabled', 'No'),
        ('./FlexIO/PowerDelivery_PdControllerConfiguration/USB2PortForTypeCPort2',   'USB2 Port 1'),
        ('./FlexIO/PowerDelivery_PdControllerConfiguration/TypeCPort3SmbusAddr',     '0x21'),
        ('./FlexIO/PowerDelivery_PdControllerConfiguration/USB2PortForTypeCPort3',   'USB2 Port 1'),
        ('./FlexIO/PowerDelivery_PdControllerConfiguration/TypeCPort4SmbusAddr',     '0x25'),
        ('./FlexIO/PowerDelivery_PdControllerConfiguration/USB2PortForTypeCPort4',   'USB2 Port 2'),
        ('./FlexIO/Type-CSubsystemConfiguration/TypeCPort1Config',                   'DP Fixed Connection'),
        ('./FlexIO/Type-CSubsystemConfiguration/TypeCPort2Config',                   'DP Fixed Connection'),
        ('./FlexIO/PciePortConfiguration/PCIeController2Config',                     '1x2, 2x1'),
        ('./FlexIO/PciePortConfiguration/PCIeController3Config',                     '4x1'),
        ('./FlexIO/Usb3PortConfiguration/USB3Port1SpdCap',                           'USB 3.1 Gen1'),
        ('./FlexIO/Usb3PortConfiguration/USB3Port2SpdCap',                           'USB 3.1 Gen1'),
        ('./FlexIO/Usb3PortConfiguration/USB3Port3SpdCap',                           'USB 3.1 Gen1'),
        ('./FlexIO/Usb3PortConfiguration/USB3Prt4ConTypeSel',                        'Type A / Type C'),
        ('./FlexIO/Usb2PortConfiguration/USB2Prt3ConTypeSel',                        'Type A / Type C'),
        ('./FlexIO/Usb2PortConfiguration/USB2Prt4ConTypeSel',                        'Type A / Type C'),
        ('./FlexIO/Usb2PortConfiguration/USB2Prt5ConTypeSel',                        'Type A / Type C'),
        ('./FlexIO/Usb2PortConfiguration/USB2Prt7ConTypeSel',                        'Express Card / M.2 S2'),
        ('./FlexIO/Usb2PortConfiguration/USB2Prt8ConTypeSel',                        'Express Card / M.2 S2'),
        ('./FlexIO/Usb2PortConfiguration/USB2Prt10ConTypeSel',                       'Type A / Type C'),
        ('./Camera/IPUSecurity/SecureTouchCameraMask',                               '0x0'),
        ('./Debug/DirectConnectInterfaceConfiguration/Usb4DciOobEnable',             'Yes'),
        ])

    if 'lp5' in plt_params_list:
        xml_change_list.append ([
        ('./BuildSettings/HarnessGlobalData/SelectedRvp',                            'RPL-P LP5 (RPL-P + ADP-P)'),
        ('./IntelMeKernel/IntelMeBootConfiguration/PrtcBackupPower',                 'None'),
        ('./InternalPchBuses/EspiConfiguration/EspiEcSlvAtchdFlshMor',               'Multiple Outstanding Requests'),
        ('./FlexIO/Type-CSubsystemConfiguration/TypeCPort3Config',                   'No Thunderbolt'),
        ('./FlexIO/PowerDelivery_PdControllerConfiguration/TypeCPort1RetimerConfig', 'Yes'),
        ('./FlexIO/PowerDelivery_PdControllerConfiguration/TypeCPort2RetimerConfig', 'Yes'),
        ('./FlexIO/PowerDelivery_PdControllerConfiguration/TypeCPort3RetimerEnabled', 'No'),
        ('./FlexIO/PowerDelivery_PdControllerConfiguration/TypeCPort4RetimerConfig', 'Yes'),
        ('./Gpio/GpioVccioVoltageControl/GppE7voltSelect',                           '1.8Volts'),
        ('./PlatformProtection/TpmOverSpiBusConfiguration/SpiOverTpmBusEnable',      'No'),
        ('./FlexIO/Type-CSubsystemConfiguration/TypeCPort2Config',                   'DP Fixed Connection'),
        ('./FlexIO/UfsConfiguration/UfsCont1Config',                                 'X1'),
        ('./Gpio/ThunderboltLsxBssb-LsConfiguration/TbltLsxBssbLs1Cfg',              'Legacy VCCIO'),
        ])

    if 'tsn' in plt_params_list:
        tsn = plt_params_list.get('tsn')
        print ("Applying changes to set tsn setting to %s" % tsn)
        if tsn == 'PCIe7':
            xml_change_list.append ([
            ('./NetworkingConnectivity/TimeSensitiveNetworkingConfiguration/TsnEnabled', 'TSN on PCIe 7 Enabled'),
            ])
        elif tsn == 'PCIe8':
            xml_change_list.append ([
            ('./NetworkingConnectivity/TimeSensitiveNetworkingConfiguration/TsnEnabled', 'TSN on PCIe 8 Enabled'),
            ])

    if 'spi' in plt_params_list:
        frequency = plt_params_list.get('spi')
        print ("Applying changes to set SPI frequency to %s" % frequency)
        xml_change_list.append ([
            ('./FlashSettings/FlashConfiguration/FastReadClockFreq',                     frequency),
            ('./FlashSettings/FlashConfiguration/ReadIdAndReadStatClkFreq',              frequency),
            ('./FlashSettings/FlashConfiguration/WriteEraseClockFreq',                   frequency),
            ])

    if 'sata' in plt_params_list:
        print ("Applying changes to enable sata")
        xml_change_list.append ([
            ('./FlexIO/SataPcieComboPortConfiguration/SataPCIeComboPort0',               'SATA')])
        # COM-HPC board only has SATA0 connected and has different bifurcation settings
        if 'crb' not in plt_params_list:
            xml_change_list.append ([
                ('./FlexIO/PciePortConfiguration/PCIeController3Config',                     '1x2, 2x1'),
                ('./FlexIO/SataPcieComboPortConfiguration/SataPCIeComboPort1',               'SATA'),
                ])

    if 'debug' in plt_params_list:
        print ("Applying changes to enable DAM and DCI")
        xml_change_list.append ([
            ('./Debug/DelayedAuthenticationModeConfiguration/DelayedAuthMode',             'Yes'),
            ('./PlatformProtection/BootGuardConfiguration/BtGuardCpuDebugEnable',           'Enabled'),
            ('./Debug/DirectConnectInterfaceConfiguration/DciDbcEnable',                              'Yes'),
            ('./Debug/DirectConnectInterfaceConfiguration/Usb1DciOobEnable',                      'Yes'),
            ('./Debug/DirectConnectInterfaceConfiguration/Usb2DciOobEnable',                      'Yes'),
            ('./Debug/DirectConnectInterfaceConfiguration/Usb3DciOobEnable',                      'Yes'),
            ('./Debug/DirectConnectInterfaceConfiguration/Usb4DciOobEnable',                      'Yes'),
            ('./Debug/IntelMeFirmwareDebuggingOverrides/DbgOverridePreProdSi',           '0x7'),
            ('./Debug/IntelMeFirmwareDebuggingOverrides/DbgOverrideProdSi',              '0x7'),
            ('./Debug/DirectConnectInterfaceConfiguration/Usb3DciOobEnable',             'Yes'),
            ])
    return xml_change_list
