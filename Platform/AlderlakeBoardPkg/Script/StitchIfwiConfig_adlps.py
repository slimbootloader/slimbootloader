## @ StitchIfwiConfig.py
#  This is an IFWI stitch config script for ADL Slim Bootloader
#
# Copyright (c) 2022, Intel Corporation. All rights reserved. <BR>
# SPDX-License-Identifier: BSD-2-Clause-Patent
#
##

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
        'adlps'    :   'Intel(R) AlderLake P Chipset - Consumer - SPI',
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
        'crb'    : {},
        'rvp'    : {},
        'quad'   : {},
        'debug'  : {},
        'sata'   : {},
        'dual'   : {},
       }

    para_help = \
        """
        'crb'    -- Stitch for CRB board
        'rvp'    -- Stitch for RVP board(default)
        'debug'  -- Enable DAM and DCI configuration (Only use for debug purpose but not for final production!)
        'sata'   -- Enable direct SATA ports, these are disabled by default
        'dual'   -- Enable secure flash support
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
        ('./BuildSettings/HarnessGlobalData/SelectedRvp',                           'ADL-P DDR5 (ADL-P + ADP-P)'),
        ('./FlashLayout/BiosRegion/InputFile',                                      '$SourceDir\BiosRegion.bin'),
        ('./FlashLayout/Ifwi_IntelMePmcRegion/ChipInitBinary',                      '$SourceDir\ChipInitBinary.bin'),
        ('./FlashLayout/Ifwi_IntelMePmcRegion/MeRegionFile',                        '$SourceDir\MeRegionFile.bin'),
        ('./FlashLayout/Ifwi_IntelMePmcRegion/PmcBinary',                           '$SourceDir\PmcBinary.bin'),
        ('./FlashLayout/EcRegion/InputFile',                                        '$SourceDir\EcRegion.bin'),
        ('./FlashLayout/EcRegion/Enabled',                                          'Enabled'),
        ('./FlashLayout/EcRegion/EcRegionPointer',                                  '$SourceDir\EcRegionPointer.bin'),
        ('./FlashLayout/GbeRegion/InputFile',                                       '$SourceDir\GbeRegion.bin'),
        ('./FlashLayout/GbeRegion/Enabled',                                         'Enabled'),
        ('./FlashLayout/SubPartitions/PchcSubPartitionData/InputFile',              '$SourceDir\PchcSubPartitionData.bin'),
        ('./FlashSettings/FlashConfiguration/SpiDualIoReadEnable',                  'Yes'),
        ('./FlashSettings/FlashConfiguration/SpiDualOutReadEnable',                 'Yes'),
        ('./FlashSettings/FlashConfiguration/FastReadClockFreq',                    '50MHz'),
        ('./FlashSettings/FlashConfiguration/WriteEraseClockFreq',                  '50MHz'),
        ('./FlashSettings/FlashConfiguration/ReadIdAndReadStatClkFreq',             '50MHz'),
        ('./FlashSettings/VsccTable/VsccEntries/VsccEntry/VsccEntryName',           'VsccEntry0'),
        ('./PlatformProtection/PlatformIntegrity/OemPublicKeyHash',                 'F8 F0 E3 69 15 81 76 99 0A 54 9E D4 C3 6D 1A 86 39 D8 87 3D EF F7 ED 2D E3 4C B4 1B CC B3 04 76 CE 0A A0 63 BC 5B 7A AC FF D9 50 9E 96 40 C6 99'),
        ('./PlatformProtection/PlatformIntegrity/OemExtInputFile',                  '$SourceDir\OemExtInputFile.bin'),
        ('./PlatformProtection/ContentProtection/PavpSupported',                    'Yes'),
        ('./PlatformProtection/BootGuardConfiguration/BtGuardKeyManifestId',        '0x1'),
        ('./PlatformProtection/BiosGuardConfiguration/BiosGrdProtOvrdEn',           'Yes'),
        ('./Icc/IccPolicies/Profiles/Profile/ClockOutputConfiguration/ClkoutSRC0',  'Enabled'),
        ('./Icc/IccPolicies/Profiles/Profile/ClockOutputConfiguration/ClkoutSRC1',  'Enabled'),
        ('./Icc/IccPolicies/Profiles/Profile/ClockOutputConfiguration/ClkoutSRC2',  'Enabled'),
        ('./Icc/IccPolicies/Profiles/Profile/ClockOutputConfiguration/ClkoutSRC3',  'Enabled'),
        ('./Icc/IccPolicies/Profiles/Profile/ClockOutputConfiguration/ClkoutSRC4',  'Enabled'),
        ('./Icc/IccPolicies/Profiles/Profile/ClockOutputConfiguration/ClkoutSRC5',  'Enabled'),
        ('./Icc/IccPolicies/Profiles/Profile/ClockOutputConfiguration/ClkoutSRC6',  'Enabled'),
        ('./Icc/IccPolicies/Profiles/Profile/ClockOutputConfiguration/ClkoutSRC7',  'Enabled'),
        ('./Icc/IccPolicies/Profiles/Profile/ClockOutputConfiguration/ClkoutSRC8',  'Enabled'),
        ('./Icc/IccPolicies/Profiles/Profile/ClockOutputConfiguration/ClkoutSRC9',  'Enabled'),
        ('./NetworkingConnectivity/WiredLanConfiguration/PhyConnected',             'PHY on SMLink0'),
        ('./NetworkingConnectivity/WiredLanConfiguration/GbePCIePortSelect',        'Port7'),
        ('./NetworkingConnectivity/WirelessLanConfiguration/CnviWlanCrdEn',         'Disabled'),
        ('./NetworkingConnectivity/WirelessLanConfiguration/MeClinkEnable',         'No'),
        ('./InternalPchBuses/SmbusSmlinkConfiguration/SLink1Enable',                'Yes'),
        ('./IntegratedSensorHub/IntegratedSensorHub/IshPowerUpState',               'Enabled'),
        ('./IntegratedSensorHub/IshImage/InputFile',                                '$SourceDir\IshImage.bin'),
        ('./IntegratedSensorHub/IshData/PdtBinary',                                 '$SourceDir\PdtBinary.bin'),
        ('./Camera/IPUSecurity/SecureTouchCameraMask',                              '0x0'),
        ('./Debug/IntelMeFirmwareDebuggingOverrides/DbgOverridePreProdSi',          '0x0'),
        ('./Debug/DirectConnectInterfaceConfiguration/DciDbcEnable',                'No'),
        ('./Debug/DirectConnectInterfaceConfiguration/Usb1DciOobEnable',            'No'),
        ('./Debug/DelayedAuthenticationModeConfiguration/DelayedAuthMode',          'No'),
        ('./CpuStraps/CpuStraps/CpuPciePeg10En',                                    'No'),
        ('./FlexIO/Type-CSubsystemConfiguration/IomBinaryFile',                     '$SourceDir\Iom.bin'),
        ('./FlexIO/Type-CSubsystemConfiguration/PhyBinaryFile',                     '$SourceDir\TypeC_NorthPHYRegion.bin'),
        ('./FlexIO/Type-CSubsystemConfiguration/TbtBinaryFile',                     '$SourceDir\TBT.bin'),
        ('./FlexIO/PowerDelivery_PdControllerConfiguration/TypeCPort1SmbusAddr',    '0x20'),
        ('./FlexIO/PowerDelivery_PdControllerConfiguration/TypeCPort2SMBusAddr',    '0x24'),
        ('./FlexIO/PowerDelivery_PdControllerConfiguration/TypeCPort3SmbusAddr',    '0x21'),
        ('./FlexIO/PowerDelivery_PdControllerConfiguration/TypeCPort4SmbusAddr',    '0x25'),
        ('./Gpio/GpioVccioVoltageControl/GppH15voltSelect',                         '3.3Volts'),
        ('./Gpio/GpioVccioVoltageControl/GppA21voltSelect',                         '1.8Volts'),
        ('./IntelUniquePlatformId/EntitlementsConfiguration/IcpsSwSkuing',          'Yes'),
        ('./PlatformProtection/TpmOverSpiBusConfiguration/SpiOverTpmBusEnable',      'Yes'),
        ('./FlexIO/Type-CSubsystemConfiguration/XdciSplitDieConfig',                'xDCI Split Die Disabled'),
        ('./FlexIO/Usb3PortConfiguration/USB3Port4SpdCap',                          'USB 3.1 Gen1'),
    ])

    if 'crb' in plt_params_list:
        print ("Applying changes to enable crb board")
        xml_change_list.append ([
          ('./FlashLayout/EcRegion/Enabled',                                          'Disabled'),
          ('./FlashLayout/EcRegion/EcRegionPointer',                                  ''),
          ('./NetworkingConnectivity/WiredLanConfiguration/GbePCIePortSelect',        'Port9'),
          ('./NetworkingConnectivity/WirelessLanConfiguration/MEWlanPowerWell',       'Primary Well || SLP_SUS#'),
          ('./InternalPchBuses/SmbusSmlinkConfiguration/SLink1freq',                  '400 KHz'),
          ('./CpuStraps/CpuStraps/Vccp105CpuPg',                                      'VCCP 1.05 CPU PG Not present'),
          ('./FlexIO/PcieLaneReversalConfiguration/PCIeCtrl3LnReversal',              'No'),
          ('./FlexIO/PciePortConfiguration/PCIeController2Config',                    '1x4'),
          ('./FlexIO/PciePortConfiguration/PCIeController3Config',                    '4x1'),
          ('./FlexIO/Usb3PortConfiguration/USB3Prt4ConTypeSel',                       'Type A / Type C'),
          ('./FlexIO/Usb2PortConfiguration/USB2Prt2ConTypeSel',                       'Type A / Type C'),
          ('./FlexIO/Usb2PortConfiguration/USB2Prt3ConTypeSel',                       'Type A / Type C'),
          ('./FlexIO/Usb2PortConfiguration/USB2Prt4ConTypeSel',                       'Type A / Type C'),
          ('./FlexIO/PcieLaneReversalConfiguration/PCIeCtrl3LnReversal',              'No'),
          ('./FlexIO/PowerDelivery_PdControllerConfiguration/TypeCPort1Mode',         'No'),
          ('./FlexIO/PowerDelivery_PdControllerConfiguration/TypeCPort1RetimerEnabled',    'No'),
          ('./FlexIO/PowerDelivery_PdControllerConfiguration/TypeCPort1SmbusAddr',         '0x0'),
          ('./FlexIO/PowerDelivery_PdControllerConfiguration/TypeCPort2Mode',              'No'),
          ('./FlexIO/PowerDelivery_PdControllerConfiguration/TypeCPort2RetimerEnabled',    'No'),
          ('./FlexIO/PowerDelivery_PdControllerConfiguration/TypeCPort2SMBusAddr',         '0x0'),
          ('./FlexIO/PowerDelivery_PdControllerConfiguration/TypeCPort3RetimerEnabled',    'No'),
          ('./FlexIO/PowerDelivery_PdControllerConfiguration/USB2PortForTypeCPort3',       'USB2 Port 1'),
          ('./Gpio/GpioVccioVoltageControl/GppE7voltSelect',                          '1.8Volts'),
          ('./Gpio/GpioVccioVoltageControl/GppE12voltSelect',                         '3.3Volts'),
          ('./Gpio/GpioVccioVoltageControl/GppE13voltSelect',                         '3.3Volts'),
          ('./Gpio/GpioVccioVoltageControl/GppE15voltSelect',                         '3.3Volts'),
          ('./Gpio/GpioVccioVoltageControl/GppF4voltSelect',                          '3.3Volts'),
          ('./Gpio/GpioVccioVoltageControl/GppF14voltSelect',                         '3.3Volts'),
          ('./Gpio/GpioVccioVoltageControl/GppH4voltSelect',                          '3.3Volts'),
          ('./Gpio/GpioVccioVoltageControl/GppH5voltSelect',                          '3.3Volts'),
          ('./Gpio/GpioVccioVoltageControl/GppH9voltSelect',                          '3.3Volts'),
          ('./Gpio/GpioVccioVoltageControl/GppH12voltSelect',                         '3.3Volts'),
          ('./Gpio/GpioVccioVoltageControl/GppB0voltSelect',                          '3.3Volts'),
          ('./Gpio/GpioVccioVoltageControl/GppB1voltSelect',                          '3.3Volts'),
          ('./Gpio/GpioVccioVoltageControl/GppB7voltSelect',                          '3.3Volts'),
          ('./Gpio/GpioVccioVoltageControl/GppB8voltSelect',                          '3.3Volts'),
          ('./Gpio/GpioVccioVoltageControl/GppB16voltSelect',                         '3.3Volts'),
          ('./Gpio/GpioVccioVoltageControl/GppB17voltSelect',                         '3.3Volts'),
          ('./Gpio/ThunderboltLsxBssb-LsConfiguration/TbltLsxBssbLs1Cfg',             'Legacy VCCIO'),
          ('./FlexIO/PcieLaneReversalConfiguration/PCIeCtrl2LnReversal',               'Yes'),
        ])
    # These changes are specifically for enabling the HDMI-TCP0 port, do not remove these settings
    xml_change_list.append ([
        ('./FlexIO/Type-CSubsystemConfiguration/TypeCPort1Config',                  'DP Fixed Connection'),
        ('./Gpio/ThunderboltLsxBssb-LsConfiguration/TbltLsxBssbLs0Cfg',             'Legacy VCCIO'),
        ])

    if 'rvp' in plt_params_list:
        print ("Applying changes to rvp board")
        xml_change_list.append ([
            ('./FlexIO/Type-CSubsystemConfiguration/TypeCPort1Config',                  'No Restrictions'),
            ('./Gpio/ThunderboltLsxBssb-LsConfiguration/TbltLsxBssbLs0Cfg',             'TX VCCIO'),
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
            ('./Debug/DelayedAuthenticationModeConfiguration/DelayedAuthMode',                    'Yes'),
            ('./PlatformProtection/BootGuardConfiguration/BtGuardCpuDebugEnable',                 'Enabled'),
            ('./Debug/DirectConnectInterfaceConfiguration/DciDbcEnable',                          'Yes'),
            ('./Debug/DirectConnectInterfaceConfiguration/Usb1DciOobEnable',                      'Yes'),
            ('./Debug/DirectConnectInterfaceConfiguration/Usb2DciOobEnable',                      'Yes'),
            ('./Debug/DirectConnectInterfaceConfiguration/Usb3DciOobEnable',                      'Yes'),
            ('./Debug/DirectConnectInterfaceConfiguration/Usb4DciOobEnable',                      'Yes'),
            ('./Debug/DirectConnectInterfaceConfiguration/DciDbcEnable',                          'Yes'),
            ])

    if 'sata' in plt_params_list:
        print ("Applying changes to enable SATA port")
        xml_change_list.append ([
            ('./FlexIO/PciePortConfiguration/PCIeController3Config',                     '4x1'),
            ('./FlexIO/SataPcieComboPortConfiguration/SataPCIeComboPort0',               'SATA'),
            ('./FlexIO/SataPcieComboPortConfiguration/SataPCIeComboPort1',               'SATA'),
            ])

    if 'dual' in plt_params_list:
        print ("Applying changes to build Dual IFWI to support secure flash")
        xml_change_list.append ([
            ('./BuildSettings/BuildResults/NumberOfFlashComponents',                     '2'),
            ('./BuildSettings/BuildResults/FlashComponentsSizes',                        '4, 32'),
            ('./FlashLayout/EcRegion/EcRegionPointer',                                   '$SourceDir\EcRegionPointer_400000.bin'),
            ('./PlatformProtection/PchBindingRegionConfiguration/PchBindingRegion',      'PCH Binding on Region 13'),
            ('./PlatformProtection/PchBindingRegionConfiguration/Length',                '0x3FF000'),
            ])

    return xml_change_list
