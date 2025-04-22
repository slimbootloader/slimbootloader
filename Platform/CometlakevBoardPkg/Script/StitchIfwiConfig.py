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
       Assuming stitching workspace is at D:\Stitch and building ifwi for CMLV platform
       To stitch IFWI with SPI QUAD mode and Boot Guard profile VM:
         StitchIfwi.py -b vm -p cmlv -w D:\Stitch -s Stitch_Components.zip -c StitchIfwiConfig.py

"""

def get_bpmgen2_params_change_list ():
    params_change_list = []
    params_change_list.append ([
      # variable                | value |
      # ===================================
      ('PlatformRules',         'CMLV Embedded'),
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
        'cmlv'    :   'H410'
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
      ('./FlashSettings/FlashComponents/SpiResHldDelay',                           '8us'),
      ('./FlashSettings/VsccTable/VsccEntries/VsccEntry/VsccEntryName',            'VsccEntry0'),
      ('./FlashSettings/VsccTable/VsccEntries/VsccEntry/VsccEntryVendorId',        '0xEF'),
      ('./FlashSettings/VsccTable/VsccEntries/VsccEntry/VsccEntryDeviceId0',       '0x40'),
      ('./FlashSettings/VsccTable/VsccEntries/VsccEntry/VsccEntryDeviceId1',       '0x19'),
      ('./IntelMeKernel/IntelMeBootConfiguration/PrtcBackupPower',                 'None'),
      ('./PlatformProtection/ContentProtection/Lspcon4kdisp',                      'PortD'),
      ('./PlatformProtection/PlatformIntegrity/OemPublicKeyHash',                  '4D 19 B4 F2 3F F9 17 0C 2C 46 B3 D7 6B F0 59 19 A7 FA 8B 6B 11 3D F5 3C 86 C0 E8 00 3C 23 A8 DC'),
      ('./PlatformProtection/PlatformIntegrity/OemExtInputFile',                   '$SourceDir\OemExtInputFile.bin'),
      ('./PlatformProtection/BootGuardConfiguration/BtGuardKeyManifestId',         '0x1'),
      ('./PlatformProtection/IntelPttConfiguration/PttSupported',                  'No'),
      ('./PlatformProtection/IntelPttConfiguration/PttPwrUpState',                 'Disabled'),
      ('./PlatformProtection/IntelPttConfiguration/PttSupportedFpf',               'No'),
      ('./PlatformProtection/TpmOverSpiBusConfiguration/SpiOverTpmBusEnable',      'Yes'),
      ('./Icc/IccPolicies/Profiles/Profile/ClockOutputConfiguration/ClkoutSRC3',   'Disabled'),
      ('./Icc/IccPolicies/Profiles/Profile/ClockOutputConfiguration/ClkoutSRC6',   'Disabled'),
      ('./Icc/IccPolicies/Profiles/Profile/ClockOutputConfiguration/ClkoutSRC9',   'Disabled'),
      ('./Icc/IccPolicies/Profiles/Profile/ClockOutputConfiguration/ClkoutSRC10',  'Disabled'),
      ('./Icc/IccPolicies/Profiles/Profile/ClockOutputConfiguration/ClkoutSRC11',  'Disabled'),
      ('./Icc/IccPolicies/Profiles/Profile/ClockOutputConfiguration/ClkoutSRC12',  'Disabled'),
      ('./Icc/IccPolicies/Profiles/Profile/ClockOutputConfiguration/ClkoutSRC13',  'Disabled'),
      ('./Icc/IccPolicies/Profiles/Profile/ClockOutputConfiguration/ClkoutSRC14',  'Disabled'),
      ('./Icc/IccPolicies/Profiles/Profile/ClockOutputConfiguration/ClkoutSRC15',  'Disabled'),
      ('./NetworkingConnectivity/WiredLanConfiguration/GbePCIePortSelect',         'Port13'),
      ('./NetworkingConnectivity/WiredLanConfiguration/PhyConnected',              'PHY on SMLink0'),
      ('./InternalPchBuses/PchTimerConfiguration/t573TimingConfig',                '100ms'),
      ('./InternalPchBuses/PchTimerConfiguration/TscClearWarmReset',               'Yes'),
      ('./Debug/IntelTraceHubTechnology/UnlockToken',                              '$SourceDir\\UnlockToken.bin'),
      ('./Debug/EspiFeatureOverrides/EspiEcLowFreqOvrd',                           'Yes'),
      ('./CpuStraps/PlatformImonDisable',                                          'Enabled'),
      ('./CpuStraps/IaVrOffsetVid',                                                'No'),
      ('./StrapsDifferences/PCH_Strap_CSME_SMT2_TCOSSEL_Diff',                     '0x00000000'),
      ('./StrapsDifferences/PCH_Strap_CSME_SMT3_TCOSSEL_Diff',                     '0x00000000'),
      ('./StrapsDifferences/PCH_Strap_PN1_RPCFG_2_Diff',                           '0x00000003'),
      ('./StrapsDifferences/PCH_Strap_PN2_RPCFG_2_Diff',                           '0x00000003'),
      ('./StrapsDifferences/PCH_Strap_ISH_ISH_BaseClass_code_SoftStrap_Diff',      '0x00000000'),
      ('./StrapsDifferences/PCH_Strap_SMB_spi_strap_smt3_en_Diff',                 '0x00000001'),
      ('./StrapsDifferences/PCH_Strap_GBE_SMLink1_Frequency_Diff',                 '0x00000001'),
      ('./StrapsDifferences/PCH_Strap_GBE_SMLink3_Frequency_Diff',                 '0x00000003'),
      ('./StrapsDifferences/PCH_Strap_USBX_XHC_PORT6_OWNERSHIP_STRAP_Diff',        '0x00000000'),
      ('./StrapsDifferences/PCH_Strap_USBX_XHC_PORT5_OWNERSHIP_STRAP_Diff',        '0x00000000'),
      ('./StrapsDifferences/PCH_Strap_USBX_XHC_PORT2_OWNERSHIP_STRAP_Diff',        '0x00000000'),
      ('./StrapsDifferences/PCH_Strap_PMC_MMP0_DIS_STRAP_Diff',                    '0x00000001'),
      ('./StrapsDifferences/PCH_Strap_PMC_EPOC_DATA_STRAP_Diff',                   '0x00000002'),
      ('./StrapsDifferences/PCH_Strap_spth_modphy_softstraps_com1_com0_pllwait_cntr_2_0_Diff', '0x00000001'),
      ('./StrapsDifferences/PCH_Strap_SPI_SPI_EN_D0_DEEP_PWRDN_Diff',              '0x00000000'),
      ('./StrapsDifferences/PCH_Strap_SPI_cs1_respmod_dis_Diff',                   '0x00000000'),
      ('./StrapsDifferences/PCH_Strap_DMI_OPDMI_LW_Diff',                          '0x00000003'),
      ('./StrapsDifferences/PCH_Strap_DMI_OPDMI_TLS_Diff',                         '0x00000003'),
      ('./StrapsDifferences/PCH_Strap_DMI_OPDMI_PAD_Diff',                         '0x0000000F'),
      ('./StrapsDifferences/PCH_Strap_DMI_OPDMI_ECCE_Diff',                        '0x00000001'),
      ('./FlexIO/IntelRstForPcieConfiguration/RstPCIeController3',                 '1x4'),
      ('./FlexIO/PcieLaneReversalConfiguration/PCIeCtrl3LnReversal',               'No'),
      ('./FlexIO/SataPcieComboPortConfiguration/SataPCIeComboPort2',               'PCIe'),
      ('./FlexIO/SataPcieComboPortConfiguration/SataPCIeComboPort4',               'SATA'),
      ('./FlexIO/SataPcieComboPortConfiguration/SataPCIeComboPort5',               'SATA'),
      ('./FlexIO/Usb3PortConfiguration/USB3PCIeComboPort2',                        'PCIe'),
      ('./FlexIO/PcieGen3PllClockControl/PCIeSecGen3PllEnable',                    'Yes'),
      ('./IntelPreciseTouchAndStylus/IntelPreciseTouchAndStylusConfiguration/Touch1MaxFreq', '17 MHz'),
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

