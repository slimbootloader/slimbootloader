## @ StitchIfwiConfig.py
#  This is an IFWI stitch config script for Slim Bootloader
#
# Copyright (c) 2020 - 2022, Intel Corporation. All rights reserved. <BR>
# SPDX-License-Identifier: BSD-2-Clause-Patent
#
##

from abc import abstractmethod
from StitchLoader import *

class StitchIfwiConfigBase:
    extra_usage_txt = \
        """
          1.  Assuming stitching workspace is at D:\Stitch and building ifwi for BC platform
              To stitch IFWI:
                StitchIfwi.py  -p BC  -w D:\Stitch  -s Stitch_Components.zip  -c StitchIfwiConfig.py

        """
    platforms = { 'BC' : 'Brighton City', 'MC' : 'Moro City' }
    sku = 'CDF'

    @staticmethod
    def get_usage_txt():
        return StitchIfwiConfigBase.extra_usage_txt

    @staticmethod
    def get_platforms():
        return StitchIfwiConfigBase.platforms

    @staticmethod
    def get_sku():
        return StitchIfwiConfigBase.sku

    def get_bpmgen2_params_change_list (self):
        params_change_list = [
            # variable                | value |
            # ===================================
            ('PlatformRules',         'Server'),
            ('BpmStrutVersion',       '0x22'),
            ('BpmRevision',           '0x01'),
            ('BpmRevocation',         '1'),
            ('AcmRevocation',         '2'),
            ('NEMPages',              '0x40'),
            ('AcpiBase',              '0x500'),
            ('IbbFlags',              '0x3'),
            ('IbbHashAlgID',          '0x0B:SHA256'),
            ('TxtInclude',            'FALSE'),
            ('PcdInclude',            'TRUE'),
            ('BpmSigScheme',          '0x14:RSASSA'),
            ('BpmSigPubKey',          'Bpmgen2\\keys\\bpm_pubkey_2048.pem'),
            ('BpmSigPrivKey',         'Bpmgen2\\keys\\bpm_privkey_2048.pem'),
            ('BpmKeySizeBits',        '2048'),
            ('BpmSigHashAlgID',       '0x0B:SHA256'),
        ]
        return params_change_list

    def get_component_replace_list(self):
        replace_list = [
            #    Path                   file name                  compress     Key
            ('IFWI/BIOS/TS0/ACM0',      'Input/acm0.bin',          'dummy',   ''),
            ('IFWI/BIOS/TS1/ACM0',      'Input/acm0.bin',          'dummy',   ''),
            ]
        if os.path.exists('IPFW/TccCacheCfg.bin'):
            print ("TccCacheCfg.bin found")
            replace_list.append (
                ('IFWI/BIOS/NRD/IPFW/TCCC', 'IPFW/TccCacheCfg.bin',  'lz4',   'KEY_ID_CONTAINER_COMP_RSA3072'), # TCC Cache Cfg binary
            )
        if os.path.exists('IPFW/TccCrlBinary.bin'):
            print ("TccCrlBinary.bin found")
            replace_list.append (
                ('IFWI/BIOS/NRD/IPFW/TCCM', 'IPFW/TccCrlBinary.bin', 'lz4',   'KEY_ID_CONTAINER_COMP_RSA3072'), # TCC Crl Cfg binary
            )
        if os.path.exists('IPFW/TccStreamCfg.bin'):
            print ("TccStreamCfg found")
            replace_list.append (
                ('IFWI/BIOS/NRD/IPFW/TCCT', 'IPFW/TccStreamCfg.bin', 'lz4',   'KEY_ID_CONTAINER_COMP_RSA3072'), # TCC Stream Cfg binary
            )
        return replace_list

    def get_btGuard_xml_change_dict (self):
        btGuard_xml_change_dict = {
            'BtGuardKeyManifestId'  : './PlatformProtection/BootGuardConfiguration/BtGuardKeyManifestId',
            'BtGuardProfileConfig'  : './PlatformProtection/BootGuardConfiguration/BtGuardProfileConfig',
            'OemPublicKeyHash'      : './PlatformProtection/OemData/OemPublicKeyHash',
            'OemExtInputFile'       : './PlatformProtection/OemData/OemExtInputFile',
            'ME_Region_OEM_Key_Manifest_Present' : './PlatformProtection/OemData/ME_Region_OEM_Key_Manifest_Present',
        }
        return btGuard_xml_change_dict

    def get_igfw_btGuard_xml_change_dict (self):
        igfw_btGuard_xml_change_dict = {
            'BtGuardKeyManifestId'  : './container/setting[@name=\'BtGuardKeyManifestId\']',
            'BtGuardProfileConfig'  : './container/setting[@name=\'Boot_Profile\']',
            'OemPublicKeyHash'      : './container/setting[@name=\'OEM_Public_Key_Hash\']',
            'ME_Region_OEM_Key_Manifest_Present' : './container/setting[@name=\'ME_Region_OEM_Key_Manifest_Present\']',
        }
        return igfw_btGuard_xml_change_dict

    @abstractmethod
    def get_xml_change_list (self, platform, plt_params_list):
        pass

class StitchIfwiConfig(StitchIfwiConfigBase):
    @abstractmethod
    def get_xml_change_list (self, platform, plt_params_list):
        xml_change_list = [
            # Path                                                      | value |
            # =========================================================================================
            #Region Order
            ('./BuildSettings/BuildResults/RegionOrder',                          'D24BF356789ACE1'),
            #Platform Protection
            ('./PlatformProtection/OemData/OemExtInputFile',                      ''),
            ('./PlatformProtection/OemData/ME_Region_OEM_Key_Manifest_Present',   '0x0'),
            ('./PlatformProtection/TxtConfiguration/TxtSupported',                '0x0'),

            #Flash layout
            ('./FlashLayout/Regions/BiosRegion/InputFile',            '$SourceDir\BiosRegion.bin'),
            ('./FlashLayout/Regions/BiosRegion/Enabled',              '0x1'),
            ('./FlashLayout/Regions/MeRegion/InputFile',              '$SourceDir\me_region.bin'),
            ('./FlashLayout/Regions/MeRegion/PmcInputFile',           '$SourceDir\pmc_patch.bin'),
            ('./FlashLayout/Regions/GbeARegion/InputFile',            '$SourceDir\img_nis_region.bin'),
            ('./FlashLayout/Regions/GbeARegion/Enabled',              '0x1'),
            ('./FlashLayout/Regions/Spare1Region/InputFile',          '$SourceDir\irc_patch.bin'),
            ('./FlashLayout/Regions/Spare1Region/Enabled',            '0x1'),

            #Flash Settings
            ('./FlashSettings/FlashComponents/FlashComponent1Size',         '0x7'),
            ('./FlashSettings/FlashConfiguration/ReadIdAndReadStatClkFreq', '0x1'),
            ('./FlashSettings/FlashConfiguration/WriteEraseClockFreq',      '0x1'),
            ('./FlashSettings/FlashConfiguration/DualIoReadEnable',         '0x1'),
            ('./FlashSettings/FlashConfiguration/DualOutputReadEnable',     '0x1'),

            #PchStraps
            ('./PchStraps/PchStrap78/PchStrap78SS78bit1ProtectedRangeandTopSwapOverrideEnable', '0x1'),
            ('./PchStraps/PchStrap51/PchStrap51SS51bits64SPI-BootBlockSize',                    '0x3'),
            ('./PchStraps/PchStrap56/PchStrap56SS56bit17IntelRDCI',                             '0x0'),
            ('./Debug/DelayedAuthenticationModeConfiguration/DelayedAuthMode',                  '0x0'),
        ]

        # Update additional options
        if plt_params_list != None:
            value = plt_params_list.get('pdr')
            if value is not None:
                xml_change_list.extend([
                    ('./FlashLayout/Regions/PdrRegion/InputFile', '$SourceDir\%s' % value),
                    ('./FlashLayout/Regions/PdrRegion/Enabled', '0x1'),
                ])

            value = plt_params_list.get('dci')
            if value is not None:
                xml_change_list.extend([
                    ('./PchStraps/PchStrap56/PchStrap56SS56bit17IntelRDCI', value),
                ])

            value = plt_params_list.get('dam')
            if value is not None:
                xml_change_list.extend([
                    ('./Debug/DelayedAuthenticationModeConfiguration/DelayedAuthMode', value),
                ])

        return xml_change_list

def get_stitch_ifwi_config_instance():
    return StitchIfwiConfig()
