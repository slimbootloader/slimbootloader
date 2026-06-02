## @ StitchIfwiConfig.py
#  This is an IFWI stitch config script for Slim Bootloader
#
# Copyright (c) 2026, Intel Corporation. All rights reserved. <BR>
# SPDX-License-Identifier: BSD-2-Clause-Patent
#
##

from abc import abstractmethod
from StitchLoader import *

class StitchIfwiConfigBase:
    extra_usage_txt = \
        """
          1.  Assuming stitching workspace is at D:\Stitch and building ifwi for ksv platform
              To stitch IFWI:
                StitchIfwi.py  -p ksv  -w D:\Stitch  -s Stitch_Components.zip  -c StitchIfwiConfig.py

          2.  To apply BMCless IBL Strap overrides (IBLStrap2=0x02191D0A, IBLStrap3=0x148028C1):
                StitchIfwi.py  -p ksv  -w D:\Stitch  -s Stitch_Components.zip  -c StitchIfwiConfig.py  -o bmcless
        """
    platforms = { 'ksv'}

    @staticmethod
    def get_usage_txt():
        return StitchIfwiConfigBase.extra_usage_txt

    def get_bpmgen2_params_change_list (self):
        params_change_list = [
            # variable                | value |
            # ===================================
            ('PlatformRules',         'Server'),
            ('BpmStrutVersion',       '0x24'),
            ('BpmRevision',           '0x01'),
            ('BpmRevocation',         '1'),
            ('AcmRevocation',         '2'),
            ('NEMPages',              '0x40'),
            ('AcpiBase',              '0x500'),
            ('IbbFlags',              '0x3'),
            ('IbbHashAlgID',          '0x0C:SHA384'),
            ('TxtInclude',            'FALSE'),
            ('PcdInclude',            'TRUE'),
            ('BpmSigScheme',          '0x16:RSAPSS'),
            ('BpmSigPubKey',          r'BpmGen2/keys/bpm_pubkey_3072.pem'),
            ('BpmSigPrivKey',         r'BpmGen2/keys/bpm_privkey_3072.pem'),
            ('BpmKeySizeBits',        '3072'),
            ('BpmSigHashAlgID',       '0x0C:SHA384'),
        ]
        return params_change_list

    def get_component_replace_list(self):
        replace_list = [
            #    Path                 file name              compress            Key             SVN
            ('IFWI/BIOS/NRD/ACM0',    'Input/acm0.bin',      'dummy',            '',             ''),
            ]
        return replace_list

    def get_btGuard_xml_change_dict (self):
        btGuard_xml_change_dict = {
            'BtGuardKeyManifestId'  : './container[@name=\'btg\']/setting[@name=\'key_manifest_id\']',
            'BtGuardProfileConfig'  : './container[@name=\'btg\']/setting[@name=\'Boot_Profile\']',
            'OemPublicKeyHash'      : './container[@name=\'btg\']/setting[@name=\'boot_policy_key_hash\']',
            'OemExtInputFile'       : './container[@name=\'descriptor\']/setting[@name=\'OemBinary\']',
        }
        return btGuard_xml_change_dict

    @abstractmethod
    def get_xml_change_list (self, platform, plt_params_list, btg_profile):
        pass

class StitchIfwiConfig(StitchIfwiConfigBase):
    @abstractmethod
    def get_xml_change_list (self, platform, plt_params_list, btg_profile):
        xml_change_list = [
            # Path                                                                            | attribute |   | value |
            # ===========================================================================================================
            #Region Order
            ('./container[@name=\'layout\']/setting[@name=\'reserved_size\']',                 "value" ,     '0x800000'),
            ('./container[@name=\'descriptor\']/setting[@name=\'NumberOfSpiComponents\']',     "value" ,     '0x0'),
            ('./container[@name=\'bios\']',                                                    "enabled" ,   'True'),
            ('./container[@name=\'nac1\']',                                                    "enabled" ,   'True'),
            ('./container[@name=\'nac0\']',                                                    "enabled" ,   'True'),
            ('./container[@name=\'pfr\']/setting[@name=\'offline_signing_root\']',             "value" ,     '0x1'),
            ('./container[@name=\'pfr\']/setting[@name=\'offline_signing_csk\']',              "value" ,     '0x1'),

            #Platform Protection
            ('./container[@name=\'btg\']/setting[@name=\'txt_supported\']',                    "value" ,     '0x0'),

            #Test value
            ('./container[@name=\'bios\']/setting[@name=\'input_file\']',                      "value",      "$SourceDir/SlimBootloader.bin"),
            ('./container[@name=\'nac0\']',                                                    "binary",     "$SourceDir/nac0.bin"),
            ('./container[@name=\'nac1\']',                                                    "binary",     "$SourceDir/nac1.bin"),

        ]
        if plt_params_list != None:
            # Enable Consent and DAM for BTG5 IFWI for both HCC and XCC
            # Disable otherwise
            if (btg_profile == 'fvme'):
                xml_change_list.extend([
                    ('./container[@name=\'btg\']/setting[@name=\'Consent\']',                  "value" ,     '0x1'),
                    ('./container[@name=\'btg\']/setting[@name=\'Dam\']',                      "value" ,     '0x1')
                ])
            else:
                xml_change_list.extend([
                    ('./container[@name=\'btg\']/setting[@name=\'Consent\']',                  "value" ,     '0x0'),
                    ('./container[@name=\'btg\']/setting[@name=\'Dam\']',                      "value" ,     '0x0')
                ])
            if 'bmcless' in plt_params_list:
                xml_change_list.extend([
                    ('./container[@name=\'ibl_straps\']/setting[@name=\'IBLStrap2\']',         "value",      '0x02191D0A'),
                    ('./container[@name=\'ibl_straps\']/setting[@name=\'IBLStrap3\']',         "value",      '0x148028C1'),
                ])
        xml_remove_list = {
            "./container[@name=\'pfm\']" : [
                "./setting[@name=\'pfm_entries[" + str(index) + "]" + prefix + "\']"
                    for index in range(12) for prefix in [
                                                          ":label",
                                                          ".name",
                                                          ".start_offset",
                                                          ".end_offset",
                                                          ".read",
                                                          ".write",
                                                          ".during_1",
                                                          ".during_2",
                                                          ".during_3",
                                                          ".hash_algorithm_info",
                                                          ".include",
                                                          ".measurement_config_type",
                                                          ".measurement_value_index",
                                                          ".measurement_data_type",
                                                          ".measurement"]
            ]
        }

        xml_insert_list = {
            "./container[@name=\'pfm\']" : [
                "<setting name='pfm_entries[" + str(index) + "]:remove' value='true'/>\n"
                    for index in range(12)
            ]
        }

        return xml_change_list, xml_remove_list, xml_insert_list

def get_stitch_ifwi_config_instance():
    return StitchIfwiConfig()
