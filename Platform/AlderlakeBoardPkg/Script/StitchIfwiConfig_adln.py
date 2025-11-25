## @ StitchIfwiConfig.py for ADLN platform
#  This is an IFWI stitch config script for ADL Slim Bootloader
#
# Copyright (c) 2023, Intel Corporation. All rights reserved. <BR>
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
         StitchIfwi.py -b vm -p adln -w D:\Stitch -s Stitch_Components.zip -c StitchIfwiConfig.py

"""
import os

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
        'adln'    :   'Intel(R) TwinLake Chipset - Consumer - SPI',
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

    return replace_list

def check_parameter(para_list):
    print (para_list)
    para_supported = {
        'crb'    : {},
        'quad'   : {},
        'debug'  : {},
        'tsn'    : {}
       }

    para_help = \
        """
        'crb'    -- Stitch for CRB board
        'quad'   -- Stitch IFWI with SPI QUAD mode
        'debug'  -- Enable DAM and DCI configuration (Only use for debug purpose but not for final production!)
        'tsn'    -- Stitch sample Tsn Mac address binary along with TSN AIC softstraps
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
        ('./FlashLayout/BiosRegion/InputFile',                                       '$SourceDir\BiosRegion.bin'),
        ('./FlashLayout/Ifwi_IntelMePmcRegion/MeRegionFile',                         '$SourceDir\MeRegionFile.bin'),
        ('./FlashLayout/Ifwi_IntelMePmcRegion/ChipInitBinary',                       '$SourceDir\ChipInitBinary.bin'),
        ('./FlashLayout/Ifwi_IntelMePmcRegion/PmcBinary',                            '$SourceDir\PmcBinary.bin'),
        ('./FlashLayout/SubPartitions/PchcSubPartitionData/InputFile',               '$SourceDir\PchcSubPartitionData.bin'),
        ('./FlashLayout/SubPartitions/IunitSubPartition/InputFile',                  '$SourceDir\Iunit.bin'),
        ('./FlashSettings/VsccTable/VsccEntries/VsccEntry/VsccEntryName',            'VsccEntry0'),
        ('./FlashSettings/BiosConfiguration/BiosRedAssistance',                      'Disabled'),
        ('./PlatformProtection/PlatformIntegrity/OemPublicKeyHash',                  'F8 F0 E3 69 15 81 76 99 0A 54 9E D4 C3 6D 1A 86 39 D8 87 3D EF F7 ED 2D E3 4C B4 1B CC B3 04 76 CE 0A A0 63 BC 5B 7A AC FF D9 50 9E 96 40 C6 99'),
        ('./PlatformProtection/PlatformIntegrity/OemExtInputFile',                   '$SourceDir\OemExtInputFile.bin'),
        ('./PlatformProtection/BootGuardConfiguration/BtGuardKeyManifestId',         '0x1'),
        ('./PlatformProtection/TpmOverSpiBusConfiguration/SpiOverTpmBusEnable',      'Yes'),
        ('./Icc/IccPolicies/Profiles/Profile/ClockOutputConfiguration/ClkoutSRC0',   'Enabled'),
        ('./Icc/IccPolicies/Profiles/Profile/ClockOutputConfiguration/ClkoutSRC2',   'Enabled'),
        ('./Icc/IccPolicies/Profiles/Profile/ClockOutputConfiguration/ClkoutSRC3',   'Enabled'),
        ('./NetworkingConnectivity/WiredLanConfiguration/MELanPowerWell',            'SLP_LAN#'),
        ('./InternalPchBuses/EspiConfiguration/EspiEcSlvAtchdFlshMor',               'Multiple Outstanding Requests'),
        ('./IntegratedSensorHub/IshImage/InputFile',                                 '$SourceDir\IshImage.bin'),
        ('./IntegratedSensorHub/IshImage/Length',                                    '0x58000'),
        ('./IntegratedSensorHub/IshData/PdtBinary',                                  '$SourceDir\PdtBinary.bin'),
        ('./Debug/DelayedAuthenticationModeConfiguration/DelayedAuthMode',           'No'),
        ('./Debug/IntelMeFirmwareDebuggingOverrides/DbgOverridePreProdSi',           '0x7'),
        ('./Debug/DirectConnectInterfaceConfiguration/DciDbcEnable',                 'No'),
        ('./Debug/DirectConnectInterfaceConfiguration/Usb1DciOobEnable',             'No'),
        ('./FlexIO/SataPcieComboPortConfiguration/SataPCIeComboPort1',               'GPIO Polarity PCIe'),
        ('./FlexIO/Usb2PortConfiguration/USB2Prt8ConTypeSel',                        'Express Card / M.2 S2'),
        ('./FlexIO/Type-CSubsystemConfiguration/TypeCPort1Config',                   'No Thunderbolt'),
        ('./FlexIO/Type-CSubsystemConfiguration/IomBinaryFile',                      '$SourceDir\Iom.bin'),
        ('./FlexIO/Type-CSubsystemConfiguration/PhyBinaryFile',                      '$SourceDir\TypeC_NorthPHYRegion.bin'),
        ('./FlexIO/PowerDelivery_PdControllerConfiguration/TypeCPort1RetimerEnabled','No'),
        ('./FlexIO/PowerDelivery_PdControllerConfiguration/TypeCPort2RetimerEnabled','No'),
        ('./Gpio/GpioVccioVoltageControl/GppD10voltSelect',                          '1.8Volts'),
        ('./Gpio/GpioVccioVoltageControl/GppD12voltSelect',                          '1.8Volts'),
        ('./Gpio/GpioVccioVoltageControl/GppH15voltSelect',                          '3.3Volts'),
        ('./Gpio/GpioVccioVoltageControl/GppA7voltSelect',                           '1.8Volts'),
        ('./Gpio/GpioVccioVoltageControl/GppA11voltSelect',                          '1.8Volts'),
    ])

    if 'crb' in plt_params_list:
        print ("Applying changes to enable crb board")
        xml_change_list.append ([
          ('./BuildSettings/HarnessGlobalData/HarnessLabel',                           'v0.4 ADP-P w/TWL and ADL-N CPU'),
          ('./BuildSettings/HarnessGlobalData/HarnessRevision',                        '#25'),
          ('./BuildSettings/HarnessGlobalData/SelectedRvp',                            'TWL / ADL-N DDR5 (ADP-P + ADL-N / TWL)'),
          ('./FlashLayout/EcRegion/Enabled',                                           'Disabled'),
          ('./IntelMeKernel/IntelMeFirmwareUpdate/HmrfpoEnable',                       'Yes'),
          ('./CpuStraps/CpuStraps/Mipi124RailSrcPlat',                                 'No'),
          ('./CpuStraps/CpuStraps/VccanaVrLocation',                                   'VCCANA is CPU FIVR'),
          ('./Icc/IccPolicies/Profiles/Profile/ClockOutputConfiguration/ClkoutSRC1',   'Enabled'),
          ('./InternalPchBuses/EspiConfiguration/EspiEcSlave1DeviceEn',                'Yes'),
          ('./FlexIO/SataPcieComboPortConfiguration/SataPCIeComboPort0',               'SATA'),
          ('./FlexIO/Usb3PortConfiguration/USB3Port4SpdCap',                           'USB 3.1 Gen2'),
          ('./FlexIO/Usb3PortConfiguration/USB3PCIeComboPort3',                        'PCIe'),
          ('./FlexIO/Usb2PortConfiguration/USB2Prt2ConTypeSel',                        'Type A / Type C'),
          ('./FlexIO/Usb2PortConfiguration/USB2Prt3ConTypeSel',                        'Type A / Type C'),
          ('./FlexIO/Usb2PortConfiguration/USB2Prt4ConTypeSel',                        'Express Card / M.2 S2'),
          ('./FlexIO/Usb2PortConfiguration/USB2Prt5ConTypeSel',                        'Type A / Type C'),
          ('./FlexIO/Type-CSubsystemConfiguration/TypeCPort2Config',                   'DP Fixed Connection'),
          ('./FlexIO/PowerDelivery_PdControllerConfiguration/TypeCPort2Mode',          'No'),
          ('./FlexIO/PowerDelivery_PdControllerConfiguration/TypeCPort2SMBusAddr',     '0x0'),
          ('./Gpio/GpioVccioVoltageControl/GppD9voltSelect',                           '1.8Volts'),
          ('./Gpio/GpioVccioVoltageControl/GppD11voltSelect',                          '1.8Volts'),
          ('./Gpio/GpioVccioVoltageControl/GppD15voltSelect',                          '1.8Volts'),
          ('./Gpio/GpioVccioVoltageControl/GppH8voltSelect',                           '3.3Volts'),
          ('./Gpio/GpioVccioVoltageControl/GppH9voltSelect',                           '3.3Volts'),
          ('./Gpio/GpioVccioVoltageControl/GppH19voltSelect',                          '3.3Volts'),
          ('./Gpio/GpioVccioVoltageControl/GppH23voltSelect',                          '3.3Volts'),
          ('./Gpio/GpioVccioVoltageControl/GppA20voltSelect',                          '3.3Volts'),
          ('./Gpio/GpioVccioVoltageControl/GppA21voltSelect',                          '3.3Volts'),
          ('./Gpio/GpioVccioVoltageControl/GppA22voltSelect',                          '3.3Volts'),
          ('./Gpio/GpioVccioVoltageControl/GppB0voltSelect',                           '3.3Volts'),
          ('./Gpio/GpioVccioVoltageControl/GppB1voltSelect',                           '3.3Volts'),
          ('./Gpio/GpioVccioVoltageControl/GppF13voltSelect',                          '3.3Volts'),
          ('./NetworkingConnectivity/TimeSensitiveNetworkingConfiguration/TsnEnabled', 'TSN MIPI Clock'),
          ('./FlexIO/PciePortConfiguration/PCIeController3Config',                     '4x1'),
          ('./FlexIO/PcieLaneReversalConfiguration/PCIeCtrl3LnReversal',               'No'),
          ('./FlexIO/Type-CSubsystemConfiguration/XdciSplitDieConfig',                 'xDCI Split Die Enabled'),
          ('./FlexIO/Type-CSubsystemConfiguration/TcssPortEnMask',                     '0x3'),
        ])

    if 'tsn' in plt_params_list:
        print ("Applying changes to enable TSN")
        xml_change_list.append ([
          ('./NetworkingConnectivity/TimeSensitiveNetworkingConfiguration/TsnEnabled', 'TSN MIPI Clock'),
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
            ])

    return xml_change_list
