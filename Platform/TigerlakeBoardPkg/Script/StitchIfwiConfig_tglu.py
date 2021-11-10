## @ StitchIfwi.py
#  This is an IFWI stitch config script for Slim Bootloader
#
# Copyright (c) 2019, Intel Corporation. All rights reserved. <BR>
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
       Assuming stitching workspace is at D:\Stitch and building ifwi for TGL platform
       To stitch IFWI with SPI QUAD mode and Boot Guard profile VM:
         StitchIfwi.py -b vm -p tglu -w D:\Stitch -s Stitch_Components.zip -c StitchIfwiConfig.py

"""

def get_bpmgen2_params_change_list ():
    params_change_list = []
    params_change_list.append ([
      # variable                | value |
      # ===================================
      ('PlatformRules',         'TGL Client'),
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
        'tglu_b0'    :   'TGP-LP-B0 Premium U'
    }
    return platform_sku

def get_component_replace_list():
    replace_list = [
       #    Path                   file name                  compress     Key
       ('IFWI/BIOS/TS0/ACM0',      'Input/acm0.bin',          'dummy',   ''),
       ('IFWI/BIOS/TS1/ACM0',      'Input/acm0.bin',          'dummy',   ''),
       ('IFWI/BIOS/TS0/DACM',      'Input/DiagnosticAcm.bin',          'dummy',   ''),
       ('IFWI/BIOS/TS1/DACM',      'Input/DiagnosticAcm.bin',          'dummy',   ''),
       ]

    return replace_list

def check_parameter(para_list):
    print (para_list)
    para_supported = {
        'sata'   : {},
        'tsn7'   : {},
        'lp4'    : {},
        '32MB'   : {},
        'spi'    : {'25MHz', '33MHz', '50MHz', '100MHz'}
        }

    para_help = \
        """
        'sata' -- Enable sata direct port, by default disabled.
        'tsn7' -- Enable TSN Port 7, by disable tsn port is disabled
        'lp4'  -- Stitch for DDRLP4 board, by default for DDR4 board
        '32MB' -- Stitch image set to 32MB, by default use 16MB.
        'spi'  -- Set SPI frequency to be 25MHz, 33MHz, 50MHz, 100MHz.
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

def get_xml_change_list (platform, plt_params_list):
    xml_change_list = []
    xml_change_list.append ([
      # Path                                                                            | value |
      # =========================================================================================
      #Region Order
        ('./BuildSettings/BuildResults/RegionOrder',                                 '45321'),
        ('./FlashLayout/BiosRegion/InputFile',                                       '$SourceDir\BiosRegion.bin'),
        ('./FlashLayout/Ifwi_IntelMePmcRegion/MeRegionFile',                         '$SourceDir\MeRegionFile.bin'),
        ('./FlashLayout/Ifwi_IntelMePmcRegion/ChipInitBinary',                       '$SourceDir\ChipInitBinary.bin'),
        ('./FlashLayout/Ifwi_IntelMePmcRegion/PmcBinary',                            '$SourceDir\PmcBinary.bin'),
        ('./FlashLayout/EcRegion/InputFile',                                         '$SourceDir\EcRegion.bin'),
        ('./FlashLayout/EcRegion/Enabled',                                           'Enabled'),
        ('./FlashLayout/EcRegion/EcRegionPointer',                                   '$SourceDir\EcRegionPointer.bin'),
        ('./FlashLayout/GbeRegion/InputFile',                                        '$SourceDir\GbeRegion.bin'),
        ('./FlashLayout/GbeRegion/Enabled',                                          'Enabled'),
        ('./FlashLayout/SubPartitions/PchcSubPartitionData/InputFile',               '$SourceDir\PchcSubPartitionData.bin'),
        ('./FlashSettings/FlashComponents/FlashComponent1Size',                      '16MB'),
        ('./FlashSettings/FlashConfiguration/FastReadClockFreq',                     '25MHz'),
        ('./FlashSettings/FlashConfiguration/ReadIdAndReadStatClkFreq',              '25MHz'),
        ('./FlashSettings/FlashConfiguration/WriteEraseClockFreq',                   '25MHz'),
        ('./FlashSettings/VsccTable/VsccEntries/VsccEntry/VsccEntryName',            'VsccEntry0'),
        ('./FlashSettings/VsccTable/VsccEntries/VsccEntry/VsccEntryVendorId',        '0xEF'),
        ('./FlashSettings/VsccTable/VsccEntries/VsccEntry/VsccEntryDeviceId0',       '0x40'),
        ('./FlashSettings/VsccTable/VsccEntries/VsccEntry/VsccEntryDeviceId1',       '0x19'),
        ('./IntelAmt/IntelAmtConfiguration/AmtSupported',                            'No'),
        ('./IntelAmt/IntelAmtConfiguration/NetServicesSupported',                    'No'),
        ('./IntelAmt/IntelAmtConfiguration/MngAppSupported',                         'No'),
        ('./PlatformProtection/PlatformIntegrity/OemPublicKeyHash',                  'F8 F0 E3 69 15 81 76 99 0A 54 9E D4 C3 6D 1A 86 39 D8 87 3D EF F7 ED 2D E3 4C B4 1B CC B3 04 76 CE 0A A0 63 BC 5B 7A AC FF D9 50 9E 96 40 C6 99'),
        ('./PlatformProtection/PlatformIntegrity/OemExtInputFile',                   ''),
        ('./PlatformProtection/BootGuardConfiguration/BtGuardKeyManifestId',         '0x1'),
        ('./PlatformProtection/BiosGuardConfiguration/BiosGrdProtOvrdEn',            'Yes'),
        ('./Icc/IccPolicies/Profiles/Profile/ClockOutputConfiguration/ClkoutSRC0',   'Enabled'),
        ('./Icc/IccPolicies/Profiles/Profile/ClockOutputConfiguration/ClkoutSRC1',   'Enabled'),
        ('./Icc/IccPolicies/Profiles/Profile/ClockOutputConfiguration/ClkoutSRC2',   'Enabled'),
        ('./Icc/IccPolicies/Profiles/Profile/ClockOutputConfiguration/ClkoutSRC3',   'Enabled'),
        ('./Icc/IccPolicies/Profiles/Profile/ClockOutputConfiguration/ClkoutSRC4',   'Enabled'),
        ('./Icc/IccPolicies/Profiles/Profile/ClockOutputConfiguration/ClkoutSRC5',   'Enabled'),
        ('./Icc/IccPolicies/Profiles/Profile/ClockOutputConfiguration/ClkoutSRC6',   'Enabled'),
        ('./NetworkingConnectivity/WiredLanConfiguration/GbePCIePortSelect',         'Port7'),
        ('./NetworkingConnectivity/WiredLanConfiguration/PhyConnected',              'PHY on SMLink0'),
        ('./NetworkingConnectivity/WirelessLanConfiguration/MeClinkEnable',          'No'),
        ('./IntegratedSensorHub/IshPowerUpState',                                    'Enabled'),
        ('./IntegratedSensorHub/IshImage/InputFile',                                 '$SourceDir\IshImage.bin'),
        ('./IntegratedSensorHub/IshData/PdtBinary',                                  '$SourceDir\PdtBinary.bin'),
        ('./Debug/DirectConnectInterfaceConfiguration/DciDbcEnable',                 'No'),
        ('./FlexIO/Type-CSubsystemConfiguration/IomBinaryFile',                      '$SourceDir\IomBinaryFile.bin'),
        ('./FlexIO/Type-CSubsystemConfiguration/PhyBinaryFile',                      '$SourceDir\PhyBinaryFile.bin'),
        ('./FlexIO/Type-CSubsystemConfiguration/TbtBinaryFile',                      '$SourceDir\TbtBinaryFile.bin'),
        ('./FlashSettings/BiosConfiguration/TopSwapOverride',                        '512KB'),
        ('./IntelMeKernel/IntelMeMeasuredBootConfiguration/MeMeasuredBootState',     'Disabled'),
    ])

    if (platform == 'tglu_b0'):
        print ("Applying changes to tglu_b0 silicon")
        xml_change_list.append ([
        ('./FlexIO/Type-CSubsystemConfiguration/TypeCPort4Config',                   'DP Fixed Connection'),
        ('./Gpio/ThunderboltLsxBssb-LsConfiguration/TbltLsxBssbLs3Cfg',              'Legacy VCCIO'),

            ])

    if 'spi' in plt_params_list:
        frequency = plt_params_list.get('spi')
        print ("Applying changes to set SPI frequency to %s" % frequency)
        xml_change_list.append ([
            ('./FlashSettings/FlashConfiguration/FastReadClockFreq',                     frequency),
            ('./FlashSettings/FlashConfiguration/ReadIdAndReadStatClkFreq',              frequency),
            ('./FlashSettings/FlashConfiguration/WriteEraseClockFreq',                   frequency),
            ])

    if '32MB' in plt_params_list:
        print ("Applying changes to generate 32MB image")
        xml_change_list.append ([
            ('./FlashSettings/FlashComponents/FlashComponent1Size',                      '32MB'),
            ])

    if 'sata' in plt_params_list:
        print ("Applying changes to enable SATA port")
        xml_change_list.append ([
            ('./FlexIO/PciePortConfiguration/PCIeContoller3Config',                      '4x1'),
            ('./FlexIO/SataPcieComboPortConfiguration/SataPCIeComboPort0',               'SATA'),
            ])

    if 'tsn7' in plt_params_list:
        print ("Applying changes to enable TSN at port 7")
        xml_change_list.append ([
            ('./NetworkingConnectivity/TimeSensitiveNetworkingConfiguration/TsnEnabled', 'TSN Enabled'),
            ('./NetworkingConnectivity/TimeSensitiveNetworkingConfiguration/TsnGbePortSelect', 'TSN Port 7'),
            ])

    if 'lp4' in plt_params_list:
        print ("Applying changes to enable DDRLP4 board")
        xml_change_list.append ([
            ('./Gpio/GpioVccioVoltageControl/GppF8voltSelect',                           '3.3Volts'),
            ])

    return xml_change_list

