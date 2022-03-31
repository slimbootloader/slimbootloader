## @ StitchIfwi.py
#  This is an IFWI stitch config script for Slim Bootloader
#
# Copyright (c) 2020, Intel Corporation. All rights reserved. <BR>
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

  3. Openssl
       Openssl is required for stitch. the stitch tool will search evn OPENSSL_PATH,
       to find Openssl. If evn OPENSSL_PATH is not found, will find openssl from
       "C:\\Openssl\\Openssl"

  4. Stitch the final image
       EX:
       Assuming stitching workspace is at D:\Stitch and building ifwi for CML platform
       To stitch IFWI with SPI QUAD mode and Boot Guard profile VM:
         StitchIfwi.py -b vm -p cml -w D:\Stitch -s Stitch_Components.zip -c StitchIfwiConfig.py

"""

def get_bpmgen2_params_change_list ():
    params_change_list = []
    params_change_list.append ([
      # variable                | value |
      # ===================================
      ('PlatformRules',         'CML Embedded'),
      ('BpmStrutVersion',       '0x20'),
      ('BpmRevision',           '0x01'),
      ('BpmRevocation',         '1'),
      ('AcmRevocation',         '2'),
      ('NEMPages',              '3'),
      ('IbbFlags',              '0x2'),
      ('IbbHashAlgID',          '0x0B:SHA256'),
      ('TxtInclude',            'FALSE'),
      ('PcdInclude',            'TRUE'),
      ('BpmSigScheme',          '0x14:RSASSA'),
      ('BpmSigPubKey',          r'Bpmgen2\keys\bpm_pubkey_2048.pem'),
      ('BpmSigPrivKey',         r'Bpmgen2\keys\bpm_privkey_2048.pem'),
      ('BpmKeySizeBits',        '2048'),
      ('BpmSigHashAlgID',       '0x0B:SHA256'),
      ('VTD_BAR',               '0x00000000FED91000'),
      ])
    return params_change_list

def get_platform_sku():
    platform_sku ={
        'cml'    :   'CMP-H No Emulation'
    }
    return platform_sku

def get_oemkeymanifest_change_list():
    xml_change_list = []
    xml_change_list.append ([
        # Path                                                                            | value |
        # =========================================================================================
        ('./KeyManifestEntries/KeyManifestEntry/Usage',                                    'OemDebugManifest'),
        ('./KeyManifestEntries/KeyManifestEntry/HashBinary',                               'Temp/kmsigpubkey.hash'),
    ])
    return xml_change_list


def get_xml_change_list (platform, spi_quad):
    xml_change_list = []
    xml_change_list.append ([
      # Path                                                                            | value |
      # =========================================================================================
      #Region Order
      ('./BuildSettings/BuildResults/RegionOrder',                                 '45321'),
      ('./FlashLayout/DescriptorRegion/OemBinary',                                 '$SourceDir\OemBinary.bin'),
      ('./FlashLayout/BiosRegion/InputFile',                                       '$SourceDir\BiosRegion.bin'),
      ('./FlashLayout/Ifwi_IntelMePmcRegion/MeRegionFile',                         '$SourceDir\MeRegionFile.bin'),
      ('./FlashLayout/Ifwi_IntelMePmcRegion/PmcBinary',                            '$SourceDir\PmcBinary.bin'),
      ('./FlashLayout/EcRegion/InputFile',                                         '$SourceDir\EcRegion.bin'),
      ('./FlashLayout/EcRegion/Enabled',                                           'Enabled'),
      ('./FlashLayout/EcRegion/EcRegionPointer',                                   '$SourceDir\EcRegionPointer.bin'),
      ('./FlashLayout/GbeRegion/InputFile',                                        '$SourceDir\GbeRegion.bin'),
      ('./FlashLayout/GbeRegion/Enabled',                                          'Enabled'),
      ('./FlashLayout/SubPartitions/PchcSubPartitionData/InputFile',               '$SourceDir\PchcSubPartitionData.bin'),
      ('./FlashSettings/FlashComponents/FlashComponent1Size',                      '32MB'),
      ('./FlashSettings/VsccTable/VsccEntries/VsccEntry/VsccEntryName',            'VsccEntry0'),
      ('./FlashSettings/VsccTable/VsccEntries/VsccEntry/VsccEntryVendorId',        '0xEF'),
      ('./FlashSettings/VsccTable/VsccEntries/VsccEntry/VsccEntryDeviceId0',       '0x40'),
      ('./FlashSettings/VsccTable/VsccEntries/VsccEntry/VsccEntryDeviceId1',       '0x19'),
      ('./PlatformProtection/PlatformIntegrity/OemPublicKeyHash',                  '4D 19 B4 F2 3F F9 17 0C 2C 46 B3 D7 6B F0 59 19 A7 FA 8B 6B 11 3D F5 3C 86 C0 E8 00 3C 23 A8 DC'),
      ('./PlatformProtection/PlatformIntegrity/OemExtInputFile',                   '$SourceDir\OemExtInputFile.bin'),
      ('./PlatformProtection/BootGuardConfiguration/BtGuardKeyManifestId',         '0x1'),
      ('./Icc/IccPolicies/Profiles/Profile/ClockOutputConfiguration/ClkoutITPXDP', 'Disabled'),
      ('./Icc/IccPolicies/Profiles/Profile/ClockOutputConfiguration/ClkoutSRC11',  'Disabled'),
      ('./Icc/IccPolicies/Profiles/Profile/ClockOutputConfiguration/ClkoutSRC12',  'Disabled'),
      ('./Icc/IccPolicies/Profiles/Profile/ClockOutputConfiguration/ClkoutSRC13',  'Disabled'),
      ('./Icc/IccPolicies/Profiles/Profile/ClockOutputConfiguration/ClkoutSRC15',  'Disabled'),
      ('./NetworkingConnectivity/WiredLanConfiguration/PhyConnected',              'PHY on SMLink0'),
      ('./IntegratedSensorHub/IshData/PdtBinary',                                  '$SourceDir\PdtBinary.bin'),
      ('./Debug/IntelTraceHubTechnology/UnlockToken',                              '$SourceDir\\UnlockToken.bin'),
      ('./Debug/DirectConnectInterfaceConfiguration/DciEnable',                    'No'),
      ('./FlexIO/Usb3PortConfiguration/USB3Prt6ConTypeSel',                        'Type A'),
      ('./FlexIO/Usb2PortConfiguration/USB2Prt7ConTypeSel',                        'Type A'),
      ('./FlexIO/Usb2PortConfiguration/USB2Prt9ConTypeSel',                        'Type A'),
      ('./IntelPreciseTouchAndStylus/IntegratedTouchConfiguration/TouchEnabled',   'No'),
      ('./FWUpdateImage/FWMeRegion/InputFile',                                     '$SourceDir\FWMeRegion.bin'),
      ('./FWUpdateImage/FWPmcRegion/InputFile',                                    '$SourceDir\FWPmcRegion.bin'),
      ('./FWUpdateImage/FWOemKmRegion/InputFile',                                  '$SourceDir\FWOemKmRegion.bin'),
      ('./FWUpdateImage/FWPchcRegion/InputFile',                                   '$SourceDir\FWPchcRegion.bin'),
      ('./FlashSettings/BiosConfiguration/TopSwapOverride',                        '256KB'),

])
    return xml_change_list

def get_component_replace_list():
    replace_list = [
       #    Path                   file name                  compress     Key
       ('IFWI/BIOS/TS0/ACM0',      'Input/acm.bin',          'dummy',   ''),
       ('IFWI/BIOS/TS1/ACM0',      'Input/acm.bin',          'dummy',   ''),
       ]

    return replace_list

