#  This is an IFWI stitch config script for Slim Bootloader
#
# Copyright (c) 2021, Intel Corporation. All rights reserved. <BR>
# SPDX-License-Identifier: BSD-2-Clause-Patent
#
##
# Global variable to control TXT inclusion - can be overridden by check_parameter()
g_txt_enabled = False

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
         StitchIfwi.py -b vm -p tglh -w D:\Stitch -s Stitch_Components.zip -c StitchIfwiConfig.py

"""

def get_bpmgen2_params_change_list ():
    global g_txt_enabled
    params_change_list = []
    # Base parameters
    base_params = [
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
      ('PcdInclude',            'TRUE'),
      ('BpmSigScheme',          '0x16:RSAPSS'),
      ('BpmSigPubKey',          r'BpmGen2/keys/bpm_pubkey_3072.pem'),
      ('BpmSigPrivKey',         r'BpmGen2/keys/bpm_privkey_3072.pem'),
      ('BpmKeySizeBits',        '3072'),
      ('BpmSigHashAlgID',       '0x0C:SHA384'),
    ]

    if g_txt_enabled == True:
        txt_params = [
            ('TxtInclude',            'TRUE'),
            ('PdSeconds',             '10'),
            ('PttCmosOffset0',        '0xFE'),
            ('PttCmosOffset1',        '0xFF'),
            ('CmosIndexOffset',       '255'),
        ]
    else:
        txt_params = [
            ('TxtInclude',            'False'),
        ]

    # Combine all parameters
    all_params = base_params + txt_params
    params_change_list.append(all_params)
    return params_change_list

def get_platform_sku():
    platform_sku ={
        'tglh'    :   'QM580',
    }
    return platform_sku

def get_component_replace_list():
    replace_list = [
       #    Path                   file name                  compress     Key
       ('IFWI/BIOS/TS0/ACM0',      'Input/acm0.bin',          'dummy',      ''),
       ('IFWI/BIOS/TS1/ACM0',      'Input/acm0.bin',          'dummy',      ''),
       ('IFWI/BIOS/TS0/DACM',      'Input/DiagnosticAcm.bin', 'dummy',      ''),
       ('IFWI/BIOS/TS1/DACM',      'Input/DiagnosticAcm.bin', 'dummy',      ''),
       ]

    return replace_list

def check_parameter(para_list):
    print (para_list)
    para_supported = {
        'sata'   : {},
        'dbg'     : {},
        'tsn'     : {},
        '32MB'   : {},
        'txt'    : {},
        'spi'    : {'25MHz', '50MHz'},
        'cfg3'   : {}
        }

    para_help = \
        """
        'sata'  -- Enable bottom slot (J5V1) for M.2 SATA. By default for M.2 NVMe.
        'tsn'   -- Enable TSN 1 and TSN2, by disable TSN is disabled
        'dbg'   -- Enable Debug Override Production Silicon. Allows the OEM
                   to control FW features to assist with production platform
                   debugging.
                   Note: Certain options will do not work when the descriptor
                   is locked (See FW Bring-up Guide for setting details)."
        '32MB' -- Stitch image set to 32MB, by default use 16MB.
        'txt'  -- Enable Intel TXT support.
        'spi'  -- Set SPI frequency to be 25MHz, 50MHz
        'cfg3' -- Stitch image to boot on Config 3 TGL-H RVP board.
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
        ('./Icc/IccPolicies/Profiles/Profile/PwrManagementConfiguration/ClkreqMapSRC2', 'GPP_B8'),
        ('./Icc/IccPolicies/Profiles/Profile/PwrManagementConfiguration/ClkreqMapSRC3', 'GPP_B7'),
        ('./NetworkingConnectivity/WiredLanConfiguration/PhyConnected',              'PHY on SMLink0'),
        ('./NetworkingConnectivity/WirelessLanConfiguration/MeClinkEnable',          'No'),
        ('./IntegratedSensorHub/IshPowerUpState',                                    'Enabled'),
        ('./IntegratedSensorHub/IshImage/InputFile',                                 '$SourceDir\IshImage.bin'),
        ('./IntegratedSensorHub/IshData/PdtBinary',                                  '$SourceDir\PdtBinary.bin'),
        ('./Debug/DelayedAuthenticationModeConfiguration/DelayedAuthMode',           'No'),
        ('./Debug/IntelMeFirmwareDebuggingOverrides/DbgOverridePreProdSi',           '0x00000007'),
        ('./Debug/DirectConnectInterfaceConfiguration/DciDbcEnable',                 'No'),
        ('./FlexIO/SataPcieComboPortConfiguration/SataPCIeComboPort2',               'SATA'),
        ('./FlexIO/SataPcieComboPortConfiguration/SataPCIeComboPort3',               'SATA'),
        ('./FlexIO/Usb2PortConfiguration/USB2Prt14ConTypeSel',                       'Type A / Type C'),
        ('./FlexIO/Type-CSubsystemConfiguration/IomBinaryFile',                      '$SourceDir\IomBinaryFile.bin'),
        ('./FlexIO/Type-CSubsystemConfiguration/PhyBinaryFile',                      '$SourceDir\PhyBinaryFile.bin'),
        ('./FlexIO/Type-CSubsystemConfiguration/TbtBinaryFile',                      '$SourceDir\TbtBinaryFile.bin'),
        ('./FlexIO/Type-CSubsystemConfiguration/TypeCPort1Config',                   'DP Fixed Connection'),
        ('./FlexIO/Type-CSubsystemConfiguration/TypeCPort4Config',                   'DP Fixed Connection'),
        ('./Gpio/GpioVccioVoltageControl/GppG1voltSelect',                           '3.3Volts'),
        ('./Gpio/GpioVccioVoltageControl/GppG5voltSelect',                           '3.3Volts'),
        ('./Gpio/GpioVccioVoltageControl/GppG8voltSelect',                           '3.3Volts'),
        ('./Gpio/GpioVccioVoltageControl/GppG9voltSelect',                           '3.3Volts'),
        ('./Gpio/GpioVccioVoltageControl/GppG10voltSelect',                          '3.3Volts'),
        ('./Gpio/GpioVccioVoltageControl/GppG11voltSelect',                          '3.3Volts'),
        ('./Gpio/GpioVccioVoltageControl/GppG12voltSelect',                          '3.3Volts'),
        ('./Gpio/GpioVccioVoltageControl/GppG13voltSelect',                          '3.3Volts'),
        ('./Gpio/GpioVccioVoltageControl/GppG14voltSelect',                          '3.3Volts'),
        ('./Gpio/GpioVccioVoltageControl/GppG15voltSelect',                          '3.3Volts'),
        ('./Gpio/ThunderboltLsxBssb-LsConfiguration/TbltLsxBssbLs0Cfg',              'Legacy VCCIO'),
        ('./Gpio/ThunderboltLsxBssb-LsConfiguration/TbltLsxBssbLs3Cfg',              'Legacy VCCIO'),
        ('./FlashSettings/BiosConfiguration/TopSwapOverride',                        '512KB'),
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
            ('./FlexIO/SataPcieComboPortConfiguration/SataPCIeComboPort1',               'SATA'),
            ])

    if 'dbg' in plt_params_list:
        print ("Applying changes to enable ME debug override")
        xml_change_list.append ([
            ('./Debug/IntelMeFirmwareDebuggingOverrides/DbgOverrideProdSi',              '0x00000007'),
            ('./Debug/EarlyUsb2DbcOverType-AConfiguration/EnEarlyUsb2DbcCon',            'Yes'),
            ('./Debug/DelayedAuthenticationModeConfiguration/DelayedAuthMode',           'Yes'),
             ])

    if 'tsn' in plt_params_list:
        print ("Applying changes to enable TSN")
        xml_change_list.append ([
            ('./NetworkingConnectivity/TimeSensitiveNetworkingConfiguration/TsnEnabled', 'TSN 1 and TSN 2 Enabled'),
            ])
    if 'cfg3' in plt_params_list:
        print ("Applying changes to boot on config 3 board")
        xml_change_list.append ([
            ('./InternalPchBuses/DmiConfiguration/DmiLaneReversal', 'Yes'),
            ('./FlexIO/PciePortConfiguration/PCIeContoller3Config', '4x1'),
            ('./FlexIO/SataPcieComboPortConfiguration/SataPCIeComboPort1', 'PCIe'),
            ('./FlexIO/Usb2PortConfiguration/USB2Prt11ConTypeSel', 'Express Card / M.2 S2'),
            ])
    else:
        xml_change_list.append ([
            ('./Gpio/GpioVccioVoltageControl/GppG4voltSelect',                           '3.3Volts'),
            ])

    if 'txt' in plt_params_list:
        print ("Applying changes to enable TXT support")
        global g_txt_enabled
        g_txt_enabled = True
        xml_change_list.append ([
            ('./PlatformProtection/TxtConfiguration/TxtSupported',                        'Yes'),
            ])

    return xml_change_list

