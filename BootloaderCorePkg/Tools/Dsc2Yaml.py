#!/usr/bin/env python
## @ Dsc2Yaml.py
# This script convert SBL DSC based CFGDATA format into YAML format
#
# Copyright (c) 2020, Intel Corporation. All rights reserved.<BR>
# SPDX-License-Identifier: BSD-2-Clause-Patent
#
##

import sys
import os
import re
import argparse
from   datetime    import date
from   collections import OrderedDict
from GenCfgDataDsc import CGenCfgData

__copyright_tmp__ = """## @file
#
#  Slim Bootloader CFGDATA %s File.
#
#  Copyright (c) %4d, Intel Corporation. All rights reserved.<BR>
#  SPDX-License-Identifier: BSD-2-Clause-Patent
#
##
"""

class dsc_to_yaml ():

    def __init__ (self):
        self._Hdr_key_list  = ['EMBED', 'STRUCT']
        self._Bsf_key_list  = ['NAME','HELP','TYPE','PAGE', 'PAGES', 'OPTION','CONDITION','ORDER', 'MARKER', 'SUBT', 'FIELD']
        self.gen_cfg_data = None
        self.cfg_reg_exp  = re.compile(r"^([_a-zA-Z0-9$\(\)]+)\s*\|\s*(0x[0-9A-F]+|\*)\s*\|\s*(\d+|0x[0-9a-fA-F]+)\s*\|\s*(.+)")
        self.bsf_reg_exp  = re.compile(r"(%s):{(.+?)}(?:$|\s+)" % '|'.join(self._Bsf_key_list))
        self.hdr_reg_exp  = re.compile("(%s):{(.+?)}" % '|'.join(self._Hdr_key_list))
        self.prefix       = ''
        self.unused_idx   = 0
        self.offset       = 0

    def load_config_data_from_dsc (self, file_name):
        """
        Load and parse a DSC CFGDATA file.
        """
        gen_cfg_data = CGenCfgData()
        if file_name.endswith('.dsc'):
            if gen_cfg_data.ParseDscFile(file_name) != 0:
                raise Exception('DSC file parsing error !')
            if gen_cfg_data.CreateVarDict() != 0:
                raise Exception('DSC variable creation error !')
        else:
            raise Exception('Unsupported file "%s" !' % file_name)
        gen_cfg_data.UpdateDefaultValue()
        self.gen_cfg_data = gen_cfg_data

    def print_dsc_line (self):
        """
        Debug function to print all DSC lines.
        """
        for line in self.gen_cfg_data._DscLines:
            print (line)

    def format_value (self, field, text, indent = ''):
        """
        Format a CFGDATA item into YAML format.
        """
        if (not text.startswith('!expand')) and (': ' in text):
            tgt = ':' if field == 'option' else '- '
            text = text.replace(': ', tgt)
        lines = text.splitlines()
        if len(lines) == 1 and field != 'help':
            return text
        else:
            return '>\n   ' + '\n   '.join ([indent + i.lstrip() for i in lines])

    def parse_dsc_line (self, dsc_line, config_dict, init_dict, include):
        """
        Parse a line in DSC and update the config dictionary accordingly.
        """
        init_dict.clear ()
        if dsc_line.startswith('gCfgData') or dsc_line.startswith('gPlatformFspPkgTokenSpaceGuid'):
            if dsc_line[8] == '.':
                start = 9
            elif dsc_line[29] == '.':
                start = 30
            else:
                raise Exception ("Unexpected format for dsc line '%s' !" % dsc_line)
            match = self.cfg_reg_exp.match(dsc_line[start:])
            if not match:
                return False
            config_dict['cname']  = self.prefix + match.group(1)
            value  = match.group(4).strip()
            length = match.group(3).strip()
            config_dict['length'] = length
            config_dict['value'] = value
            if match.group(2) == '*':
                self.offset += int (length, 0)
            else:
                offset = int (match.group(2), 0)
                if self.offset != offset:
                    if offset > self.offset:
                        init_dict['padding'] = offset - self.offset
                    else:
                        raise Exception ('Offset cannot go backwards at %s !' % config_dict['cname'] )
                self.offset = offset + int (length, 0)
            return True

        match = re.match(r"^\s*#\s+!([<>])\s+include\s+(.+)", dsc_line)
        if match and len(config_dict) == 0:
            # !include should not be inside a config field
            # if so, do not convert include into YAML
            init_dict = dict(config_dict)
            config_dict.clear()
            config_dict['cname']   = '$ACTION'
            if match.group(1) == '<':
                config_dict['include'] = match.group(2)
            else:
                config_dict['include'] = ''
            return True

        match = re.match(r"^\s*#\s+(!BSF|!HDR)\s+(.+)", dsc_line)
        if not match:
            return False

        remaining = match.group(2)
        if match.group(1) == '!BSF':
            result = self.bsf_reg_exp.findall (remaining)
            if not result:
                return False

            for each in result:
                key = each[0].lower()
                val = each[1]
                if key == 'field':
                    name = each[1]
                    if ':' not in name:
                        raise Exception ('Incorrect bit field format !')
                    parts = name.split(':')
                    config_dict['length'] = parts[1]
                    config_dict['cname']  = '@' + parts[0]
                    return True
                elif key == 'pages' or key == 'page':
                    init_dict = dict(config_dict)
                    config_dict.clear()
                    config_dict['cname'] = '$ACTION'
                    config_dict['page'] = val
                    return True
                elif key == 'subt':
                    config_dict.clear()
                    parts = each[1].split(':')
                    tmp_name = parts[0][:-5]
                    skey = parts[0].lower()
                    if tmp_name == 'CFGHDR':
                        cfg_tag = '_$FFF_'
                        sval = '!expand { %s_TMPL : [ ' % tmp_name + '%s, %s, ' % (parts[1], cfg_tag) + ', '.join (parts[2:]) + ' ] }'
                    else:
                        sval = '!expand { %s_TMPL : [ ' % tmp_name + ', '.join (parts[1:]) + ' ] }'
                    config_dict.clear()
                    config_dict['cname']  = tmp_name
                    config_dict['expand'] = sval
                    return True
                else:
                    if key in ['name', 'help', 'option'] and val.startswith('+'):
                        val = config_dict[key] + '\n' +  val[1:]
                    if val.strip() == '':
                        val = "''"
                    config_dict[key] = val

        else:
            match = self.hdr_reg_exp.match(remaining)
            if not match:
                return False
            key = match.group(1)
            remaining = match.group(2)
            if key  == 'EMBED':
                parts = remaining.split(':')
                names = parts[0].split(',')
                if parts[-1] == 'END':
                    prefix = '>'
                else:
                    prefix = '<'
                skip = False
                if parts[1].startswith('TAG_'):
                    tag_txt = '%s:%s' % (names[0], parts[1])
                else:
                    tag_txt = names[0]
                    if parts[2] in ['START', 'END']:
                        if names[0] == 'PCIE_RP_PIN_CTRL[]':
                            skip = True
                        else:
                            tag_txt = '%s:%s' % (names[0], parts[1])
                if not skip:
                    config_dict.clear()
                    config_dict['cname'] = prefix + tag_txt
                    return True

            if key  == 'STRUCT':
                text = remaining.strip()
                config_dict[key.lower()] = text

        return False


    def process_template_lines (self, lines):
        """
        Process a line in DSC template section.
        """
        template_name = ''
        bsf_temp_dict  = OrderedDict()
        temp_file_dict = OrderedDict()
        include_file  = ['.']

        for line in lines:
            match = re.match("^\s*#\s+!([<>])\s+include\s+(.+)", line)
            if match:
                if match.group(1) == '<':
                    include_file.append(match.group(2))
                else:
                    include_file.pop()

            match = re.match("^\s*#\s+(!BSF)\s+DEFT:{(.+?):(START|END)}", line)
            if match:
                if match.group(3) == 'START' and not template_name:
                    template_name = match.group(2).strip()
                    temp_file_dict[template_name] = list(include_file)
                    bsf_temp_dict[template_name]  = []
                if match.group(3) == 'END' and (template_name == match.group(2).strip()) and template_name:
                    template_name = ''
            else:
                if template_name:
                    bsf_temp_dict[template_name].append(line)
        return bsf_temp_dict, temp_file_dict


    def process_option_lines (self, lines):
        """
        Process a line in DSC config section.
        """
        cfgs = []
        config_dict  = dict()
        init_dict    = dict()
        include     = ['']
        for line in lines:
            ret = self.parse_dsc_line (line, config_dict, init_dict, include)
            if ret:

                if 'padding' in init_dict:
                    num = init_dict['padding']
                    init_dict.clear()
                    padding_dict = {}
                    cfgs.append (padding_dict)
                    padding_dict['cname']  = 'UnusedUpdSpace%d' % self.unused_idx
                    padding_dict['length'] = '0x%x' % num
                    padding_dict['value']  = '{ 0 }'
                    self.unused_idx += 1

                if cfgs and cfgs[-1]['cname'][0] != '@' and config_dict['cname'][0] == '@':
                    # it is a bit field, mark the previous one as virtual
                    cname = cfgs[-1]['cname']
                    new_cfg = dict(cfgs[-1])
                    new_cfg['cname'] = '@$STRUCT'
                    cfgs[-1].clear ()
                    cfgs[-1]['cname'] = cname

                    cfgs.append(new_cfg)

                if cfgs and cfgs[-1]['cname'] == 'CFGHDR' and config_dict['cname'][0] == '<':
                    # swap CfgHeader and the CFG_DATA order
                    if ':' in config_dict['cname']:
                        # replace the real TAG for CFG_DATA
                        cfgs[-1]['expand'] = cfgs[-1]['expand'].replace('_$FFF_', '0x%s' % config_dict['cname'].split(':')[1][4:])
                    cfgs.insert (-1, config_dict)
                else:
                    cfgs.append (config_dict)

                config_dict = dict(init_dict)
        return cfgs


    def get_fixup_dict (self):
        """
        Return a dictonary for fix up.
        """
        fix_dict = {
          '$Pcie'                :  '$PCIE_RP_CFG_DATA.Pcie',
          '$Hda'                 :  '$HDA_CFG_DATA.Hda',
          '$Dsp'                 :  '$HDA_CFG_DATA.Dsp',
          '$PlatformId'          :  '$PLATFORMID_CFG_DATA.PlatformId',
          '$$(1).Enable'         :  '$PID_GPIO_CFG_DATA.$(1).Enable',
          '$$(1)_Half0'          :  '$GPIO_CFG_DATA.$(1)_Half0',
          '$EnableSgx'           :  '$SGX_CFG_DATA.EnableSgx',
          '$EpochUpdate'         :  '$SGX_CFG_DATA.EpochUpdate',
          '$GpioPinConfig1_$(1)' :  '$GPIO_CFG_DATA.GpioPinConfig1_$(1)',
          '$Gpio'                :  '$GPIO_CFG_DATA.Gpio',
          '$ImageType_$(1)'      :  '$BOOT_OPTION_CFG_DATA_$(1).ImageType_$(1)',
          '$PayloadSelGpio'      :  '$PLATFORM_CFG_DATA.PayloadSelGpio',
          '$PlatformId'          :  '$PLATFORMID_CFG_DATA.PlatformId',
          '$RpFeatures$(1)'      :  '$PCIE_CFG_DATA.RpFeatures$(1)',
        }
        return fix_dict

    def variable_fixup (self, each):
        """
        Fix up some variable definitions for SBL.
        """
        fix_dict = self.get_fixup_dict()
        key = each
        val = self.gen_cfg_data._MacroDict[each]
        for each in fix_dict:
            val = val.replace (each, fix_dict[each])
        return key, val

    def template_fixup (self, tmp_name, tmp_list):
        """
        Fix up some special config templates for SBL
        """
        if tmp_name in ['CFGHDR_TMPL']:
            # handle CFGHDR template specially
            find = '((_LENGTH_$(1)_+8)/4):10b, $(2):4b, $(3):4b, _TAG_$(1)_:12b'
            for each in tmp_list:
                if each['cname'] == 'CfgHeader':
                    each['value'] = each['value'].replace(find, '((_LENGTH_$(1)_)/4):10b, $(3):4b, $(4):4b, $(2):12b')

        else:
            fix_dict = self.get_fixup_dict()
            for each in tmp_list:
                if 'condition' in each:
                    for find in fix_dict:
                        each['condition'] = each['condition'].replace(find, fix_dict[find])

        return

    def config_fixup (self, cfg_list):
        """
        Fix up some special config items for SBL.
        """
        fix_dict = self.get_fixup_dict()
        swap_idx = []
        for idx, cfg in enumerate(cfg_list):
            # For QEMU
            if cfg['cname'] in ['GpioItemSize', 'GpioItemCount']:
                # Adjust GPIO item size
                cfg['value'] = cfg['value'].replace('(_OFFSET_GPIO_DATA_GPP_A1_ - _OFFSET_GPIO_DATA_GPP_A0_)', '8')
                if cfg['cname'] in ['GpioItemCount']:
                    cfg['value'] = cfg['value'].replace('_LENGTH_GPIO_CFG_HDR_', '_LENGTH_GPIO_CFG_HDR_ - 8')

            if 'value' in cfg:
                # Try to find the unit length automatically
                value_str = cfg['value']
                if value_str[0] == '{' and value_str[-1] == '}' and ',' in value_str:
                    if (':' not in value_str) and ("'" not in value_str) and ('"' not in value_str):
                        parts = value_str[1:-1].split(',')
                        blen  = int (cfg['length'], 0)
                        if len(parts) != blen:
                            if  len(parts) * 2 == blen:
                                unit = 'W'
                            elif  len(parts) * 4 == blen:
                                unit = 'D'
                            elif  len(parts) * 8 == blen:
                                unit = 'Q'
                            else:
                                unit = ''
                            if unit:
                                cfg['value'] = '{0:0%s, ' % unit + cfg['value'][1:]

            if 'condition' in cfg and cfg['condition'] != 0:
                val = cfg['condition']
                for each in fix_dict:
                    val = val.replace (each, fix_dict[each])
                cfg['condition'] = val

            if re.match('<GPIO_CFG_HDR(:*)?', cfg['cname']):
                swap_idx.append (idx)

        for idx in swap_idx:
            cfg_list[idx], cfg_list[idx + 1] = cfg_list[idx + 1], cfg_list[idx]

        return

    def get_section_range (self, section_name):
        """
        Extract line number range from config file for a given section name.
        """
        start = -1
        end   = -1
        for idx, line in enumerate (self.gen_cfg_data._DscLines):
            if start <0 and line.startswith('[%s]' % section_name):
                start = idx
            elif start >= 0 and line.startswith('['):
                end   = idx
                break
        if start == -1:
            start = 0
        if end == -1:
            end = len(self.gen_cfg_data._DscLines)
        return start, end

    def normalize_file_name (self, file, is_temp = False):
        """
        Normalize file name convention so that it is consistent.
        """
        if file.endswith('.dsc'):
            file = file[:-4] + '.yaml'
        dir_name  = os.path.dirname (file)
        base_name = os.path.basename (file)
        if is_temp:
            if 'Template_' not in file:
                base_name = base_name.replace ('Template', 'Template_')
        else:
            if 'CfgData_' not in file:
                base_name = base_name.replace ('CfgData', 'CfgData_')
        if dir_name:
            path = dir_name + '/' + base_name
        else:
            path = base_name
        return path

    def output_variable (self):
        """
        Output variable block into a line list.
        """
        lines = []
        for each in self.gen_cfg_data._MacroDict:
            key, value = self.variable_fixup (each)
            lines.append ('%-30s : %s' % (key,  value))
        return lines

    def output_template (self):
        """
        Output template block into a line list.
        """
        self.offset  = 0
        start, end   = self.get_section_range ('PcdsDynamicVpd.Tmp')
        bsf_temp_dict, temp_file_dict = self.process_template_lines (self.gen_cfg_data._DscLines[start:end])
        template_dict = dict()
        lines = []
        file_lines = {}
        last_file = '.'
        file_lines[last_file] = []

        for tmp_name in temp_file_dict:
            temp_file_dict[tmp_name][-1] = self.normalize_file_name(temp_file_dict[tmp_name][-1], True)
            if len(temp_file_dict[tmp_name]) > 1:
                temp_file_dict[tmp_name][-2] = self.normalize_file_name(temp_file_dict[tmp_name][-2], True)

        for tmp_name in bsf_temp_dict:
            file = temp_file_dict[tmp_name][-1]
            if last_file != file and len(temp_file_dict[tmp_name]) > 1:
                inc_file = temp_file_dict[tmp_name][-2]
                file_lines[inc_file].extend (['', '- !include %s' % temp_file_dict[tmp_name][-1], ''])
            last_file = file
            if file not in file_lines:
                file_lines[file] = []
            lines = file_lines[file]
            text = bsf_temp_dict[tmp_name]
            tmp_list = self.process_option_lines (text)
            self.template_fixup (tmp_name, tmp_list)
            template_dict[tmp_name] = tmp_list
            lines.append ('%s: >' % tmp_name)
            lines.extend (self.output_dict (tmp_list, False)['.'])
            lines.append ('\n')
        return file_lines


    def output_config (self):
        """
        Output config block into a line list.
        """
        self.offset  = 0
        start, end = self.get_section_range ('PcdsDynamicVpd.Upd')
        cfgs = self.process_option_lines (self.gen_cfg_data._DscLines[start:end])
        self.config_fixup (cfgs)
        file_lines = self.output_dict (cfgs, True)
        return file_lines


    def output_dict (self, cfgs, is_configs ):
        """
        Output one config item into a line list.
        """
        file_lines = {}
        level = 0
        file  = '.'
        for each in cfgs:
            if 'include' in each:
                if each['include']:
                    each['include'] = self.normalize_file_name(each['include'])
                    file_lines[file].extend (['', '- !include %s' % each['include'], ''])
                    file = each['include']
                else:
                    file = '.'
                continue

            if file not in file_lines:
                file_lines[file] = []

            lines  = file_lines[file]
            name   = each['cname']

            prefix = name[0]
            if prefix == '<':
                level += 1

            padding = '  ' * level
            if prefix not in '<>@':
                padding += '  '
            else:
                name = name[1:]
                if prefix == '@':
                    padding += '    '

            if ':' in name:
                parts = name.split(':')
                name = parts[0]

            padding = padding[2:] if is_configs else padding

            if prefix != '>':
                if 'expand' in each:
                    lines.append ('%s- %s' % (padding, each['expand']))
                else:
                    lines.append ('%s- %-12s :' % (padding , name))

            for field in each:
                if field in ['cname', 'expand', 'include']:
                    continue
                value_str = self.format_value (field, each[field], padding + ' ' * 16)
                full_line = '  %s  %-12s : %s' % (padding, field, value_str)
                lines.extend(full_line.splitlines())

            if prefix == '>':
                level -= 1
                if level == 0:
                    lines.append ('')

        return file_lines

def main():
    parser = argparse.ArgumentParser()

    parser.add_argument('-i', dest='dsc_file', type=str,  required=True, help='SBL DSC config file path')
    parser.add_argument('-o', dest='out_dir',  type=str,  default = '.', help='Output directory for generated files')
    parser.add_argument('-a', dest='all',  action="store_true", help='Output all config data including common and platform config')

    args = parser.parse_args()

    dsc_file = args.dsc_file
    out_dir  = args.out_dir
    gen_all  = args.all

    dsc2yaml = dsc_to_yaml ()
    dsc2yaml.load_config_data_from_dsc (dsc_file)

    cfgs = {}
    for cfg in ['Template', 'Option']:
        if cfg == 'Template':
            file_lines = dsc2yaml.output_template ()
        else:
            file_lines = dsc2yaml.output_config ()
        for file in file_lines:
            lines = file_lines[file]
            if file == '.':
                cfgs[cfg] = lines
            else:
                if ('/' in file or '\\' in file) and not gen_all:
                    continue
                file = os.path.basename(file)
                fo = open(os.path.join(out_dir, file), 'w')
                fo.write (__copyright_tmp__ % (cfg, date.today().year) + '\n\n')
                for line in lines:
                    fo.write(line + '\n')
                fo.close()

    variables = dsc2yaml.output_variable ()
    fo = open(os.path.join(out_dir, 'CfgDataDef.yaml'), 'w')
    fo.write (__copyright_tmp__ % ('Default', date.today().year))
    if len(variables) > 0:
        fo.write ('\n\nvariable:\n')
        for line in variables:
            fo.write('  ' + line + '\n')

    fo.write ('\n\ntemplate:\n')
    for line in cfgs['Template']:
        fo.write('  ' + line + '\n')

    fo.write ('\n\nconfigs:\n')
    for line in cfgs['Option']:
        fo.write('  ' + line + '\n')

    fo.close()

    return 0


if __name__ == '__main__':
    main ()


