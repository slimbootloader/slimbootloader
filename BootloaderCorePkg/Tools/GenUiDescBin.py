#!/usr/bin/env python3
## @file

# Generate UI Descriptor binary from CfgDataDef YAML.

# This script uses GenCfgData.py parsing logic to build UiSetup descriptor
# metadata (pages, fields, options, conditions, strings),then emits either:
# - UiDesc.bin
# - UiDescData.h (if output file ends with .h)
#
# Usage:
# python GenUiDescBin.py <CfgDataDef.yaml> <UiDesc.bin>
# python GenUiDescBin.py <CfgDataDef.yaml> <UiDescData.h>
#
# Copyright (c) 2026, Intel Corporation. All rights reserved.
# SPDX-License-Identifier: BSD-2-Clause-Patent
#

import os
import sys
import struct
import re

from GenCfgData import CGenCfgData

# Binary format constants
UI_DESC_SIGNATURE = 0x42444955   # 'UIDB'
UI_DESC_VERSION   = 2

# Field types
UI_FIELD_TYPE_COMBO    = 0
UI_FIELD_TYPE_EDITNUM  = 1
UI_FIELD_TYPE_EDITTEXT = 2
UI_FIELD_TYPE_RESERVED = 3
UI_FIELD_TYPE_TABLE    = 4

# Condition operators
UI_COND_OP_EQUAL     = 0
UI_COND_OP_NOT_EQUAL = 1
UI_COND_OP_GREATER   = 2
UI_COND_OP_LESS      = 3


class UiDescGenerator:
    """Generate the UI descriptor binary from parsed YAML config data."""

    def __init__(self, gen_cfg_data):
        self.gen_cfg = gen_cfg_data
        self.pages = []
        self.fields = []
        self.options = []
        self.conditions = []
        self.strings = bytearray()
        self.str_offsets = {}
        self.combo_option_cache = {}

    def add_string(self, s):
        """Add a string to the string table, deduplicating."""
        if not s:
            s = ""
        if s in self.str_offsets:
            return self.str_offsets[s]
        offset = len(self.strings)
        self.str_offsets[s] = offset
        self.strings.extend(s.encode('utf-8') + b'\x00')
        return offset

    def get_table_elem_bytes(self, item):
        """Infer table element width (bytes) from option tokens like '0-1F:8:HEX'."""
        option = str(item.get('option', '')).strip()
        if not option:
            return 1

        token = option.split(',')[0].strip()
        parts = token.split(':')
        if len(parts) < 2:
            return 1

        try:
            elem_bytes = int(parts[1], 0)
        except ValueError:
            elem_bytes = 1

        if elem_bytes <= 0:
            elem_bytes = 1
        return elem_bytes

    def get_table_preview_elems(self, item):
        """Infer table element count from option spec; fall back to default if unclear."""
        option = str(item.get('option', '')).strip()
        if not option:
            return 1

        tokens = [tok.strip() for tok in option.split(',') if tok.strip()]
        if not tokens:
            return 1

        m = re.match(r'^([0-9A-Fa-fxX]+)-([0-9A-Fa-fxX]+):(\d+):', tokens[0])
        if m is not None:
            try:
                start = int(m.group(1), 0)
            except ValueError:
                start = int(m.group(1), 16)
            try:
                end = int(m.group(2), 0)
            except ValueError:
                end = int(m.group(2), 16)
            if end >= start:
                return end - start + 1

        all_triplets = True
        for tok in tokens:
            if len(tok.split(':')) < 3:
                all_triplets = False
                break
        if all_triplets:
            return len(tokens)

        return 1

    def get_field_type(self, item):
        """Determine UI field type from item's type string."""
        itype = item.get('type', 'Reserved')
        if itype.startswith('Reserved'):
            return UI_FIELD_TYPE_RESERVED
        elif itype.startswith('Table'):
            return UI_FIELD_TYPE_TABLE
        elif itype.startswith('Combo'):
            return UI_FIELD_TYPE_COMBO
        elif itype.startswith('EditNum'):
            return UI_FIELD_TYPE_EDITNUM
        elif itype.startswith('EditText'):
            return UI_FIELD_TYPE_EDITTEXT
        else:
            return UI_FIELD_TYPE_RESERVED

    def parse_condition(self, condition_str, cfg_list):
        """
        Parse a condition string like:
          $GPIO_CFG_DATA.GpioPinConfig1_X.GPIOSkip_X == 0
        Returns (tag_id, bit_offset, bit_length, operator, compare_value) or None.
        """
        if not condition_str:
            return None

        # Simple pattern: $TAG.item.field == value
        match = re.match(r'^\$(\w+)\.(\w+)\.(\w+)\s*(==|!=|>|<)\s*(\S+)$', condition_str.strip())
        if not match:
            # Try simpler: $TAG.field == value
            match = re.match(r'^\$(\w+)\.(\w+)\s*(==|!=|>|<)\s*(\S+)$', condition_str.strip())
            if not match:
                return None
            tag_name = match.group(1)
            field_name = match.group(2)
            op_str = match.group(3)
            val_str = match.group(4)
        else:
            tag_name = match.group(1)
            field_name = match.group(3)
            op_str = match.group(4)
            val_str = match.group(5)

        # Find data start bit offset for each tag (first non-header field).
        tag_data_offset = {}
        current_tag = None
        for item in cfg_list:
            if 'tag' in item:
                current_tag = item['tag']
                tag_data_offset[current_tag] = None

            if current_tag is None:
                continue

            cname = item['cname']
            if (cname not in ('CfgHeader', 'CondValue')) and (tag_data_offset[current_tag] is None):
                tag_data_offset[current_tag] = item['offset']

        # Find the tag ID for the referenced item
        tag_id = None
        bit_offset = 0
        bit_length = 0

        current_tag = None
        for item in cfg_list:
            if 'tag' in item:
                current_tag = item['tag']
            cname = item['cname']
            if cname == 'CfgHeader' or cname == 'CondValue':
                continue
            # Check if this item's path contains the tag name
            if item['path'].startswith(tag_name + '.') or ('.' + tag_name + '.') in item['path']:
                if cname == field_name:
                    tag_id = current_tag
                    data_start = tag_data_offset.get(current_tag)
                    if data_start is None:
                        data_start = item['offset']

                    # bit_offset is relative to the tag data start.
                    bit_offset = item['offset'] - data_start
                    bit_length = item['length']
                    break

        if tag_id is None:
            return None

        op_map = {'==': UI_COND_OP_EQUAL, '!=': UI_COND_OP_NOT_EQUAL,
                  '>': UI_COND_OP_GREATER, '<': UI_COND_OP_LESS}
        operator = op_map.get(op_str, UI_COND_OP_EQUAL)

        try:
            compare_value = int(val_str, 0)
        except ValueError:
            compare_value = 0

        return (tag_id, bit_offset, bit_length, operator, compare_value)

    def collect_pages(self):
        """Walk the _cfg_page tree and flatten into a page list."""
        cfg_page = self.gen_cfg.get_cfg_page()
        page_id_counter = [1]  # mutable for closure
        page_map = {}          # page_key -> page_id

        def walk_pages(node, parent_id):
            for child_node in node['child']:
                page_key = next(iter(child_node))
                page_info = child_node[page_key]

                title = page_info.get('title', page_key)
                pid = page_id_counter[0]
                page_id_counter[0] += 1
                page_map[page_key] = pid

                self.pages.append({
                    'page_id': pid,
                    'parent_id': parent_id,
                    'page_key': page_key,
                    'title': title,
                    'field_start': 0,
                    'field_count': 0,
                })

                # Recurse into children
                walk_pages(page_info, pid)

        walk_pages(cfg_page['root'], 0)
        return page_map

    def collect_fields(self, page_map):
        """Collect fields organized by page."""
        cfg_list = self.gen_cfg.get_cfg_list()

        # Determine current tag for each item
        current_tag = 0
        tag_data_offset = {}  # tag -> bit offset of first data item after header

        # First pass: identify tag boundaries
        for item in cfg_list:
            if 'tag' in item:
                current_tag = item['tag']
                tag_data_offset[current_tag] = None

        # Second pass: find data start for each tag
        current_tag = 0
        header_bits = 0
        for item in cfg_list:
            if 'tag' in item:
                current_tag = item['tag']
                header_bits = 0
            cname = item['cname']
            if cname in ('CfgHeader', 'CondValue'):
                header_bits += item['length']
            elif tag_data_offset.get(current_tag) is None:
                tag_data_offset[current_tag] = item['offset']

        # Now collect fields per page
        for page_entry in self.pages:
            page_key = page_entry['page_key']
            page_items = self.gen_cfg.get_cfg_list(page_key)

            page_entry['field_start'] = len(self.fields)
            field_count = 0

            for item in page_items:
                cname = item['cname']
                if cname in ('CfgHeader', 'CondValue'):
                    continue

                field_type = self.get_field_type(item)
                if field_type == UI_FIELD_TYPE_RESERVED:
                    continue

                # Find the tag for this item
                item_tag = None
                cur_t = 0
                for ci in cfg_list:
                    if 'tag' in ci:
                        cur_t = ci['tag']
                    if ci is item:
                        item_tag = cur_t
                        break

                if item_tag is None:
                    continue

                # Compute bit offset relative to tag data start
                data_start = tag_data_offset.get(item_tag, 0)
                if data_start is None:
                    data_start = 0
                rel_bit_offset = item['offset'] - data_start

                # Process condition
                cond_idx = 0xFF
                condition_str = item.get('condition', '')
                if condition_str and len(self.conditions) < 254:
                    cond_result = self.parse_condition(condition_str, cfg_list)
                    if cond_result is not None:
                        cond_idx = len(self.conditions)
                        self.conditions.append(cond_result)

                # Process options
                opt_start = len(self.options)
                opt_count = 0
                if field_type == UI_FIELD_TYPE_COMBO:
                    opt_list = self.gen_cfg.get_cfg_item_options(item)
                    parsed_opts = []
                    for (op_val, op_str) in opt_list:
                        try:
                            val = int(op_val.strip(), 0)
                        except ValueError:
                            val = 0
                        label_off = self.add_string(op_str.strip())
                        parsed_opts.append((val, label_off))

                    opt_key = tuple(parsed_opts)
                    if opt_key in self.combo_option_cache:
                        opt_start, opt_count = self.combo_option_cache[opt_key]
                    else:
                        opt_start = len(self.options)
                        for val, label_off in parsed_opts:
                            self.options.append({'value': val, 'label_offset': label_off})
                        opt_count = len(parsed_opts)
                        self.combo_option_cache[opt_key] = (opt_start, opt_count)

                # Keep table fields compact in descriptor. Runtime UI reads element
                # metadata from option_start_idx/option_count.
                itype = item.get('type', 'Reserved')
                if itype.startswith('Table'):
                    total_bits = int(item.get('length', 0))
                    elem_bytes = self.get_table_elem_bytes(item)
                    elem_bits = elem_bytes * 8
                    if elem_bits <= 0:
                        continue

                    elem_count = total_bits // elem_bits
                    if elem_count <= 0:
                        continue

                    name = item.get('name', cname)
                    help_text = item.get('help', '')
                    field_path = item.get('path', '')
                    name_off = self.add_string(name)
                    help_off = self.add_string(help_text)
                    path_off = self.add_string(field_path)

                    self.fields.append({
                        'tag_id': item_tag,
                        'bit_offset': rel_bit_offset,
                        'bit_length': total_bits,
                        'field_type': UI_FIELD_TYPE_TABLE,
                        'condition_idx': cond_idx,
                        'name_str_offset': name_off,
                        'help_str_offset': help_off,
                        'path_str_offset': path_off,
                        'option_start_idx': elem_bits,
                        'option_count': elem_count,
                    })
                    field_count += 1

                    continue

                # Add field
                name = item.get('name', cname)
                help_text = item.get('help', '')
                field_path = item.get('path', '')
                name_off = self.add_string(name)
                help_off = self.add_string(help_text)
                path_off = self.add_string(field_path)

                self.fields.append({
                    'tag_id': item_tag,
                    'bit_offset': rel_bit_offset,
                    'bit_length': item['length'],
                    'field_type': field_type,
                    'condition_idx': cond_idx,
                    'name_str_offset': name_off,
                    'help_str_offset': help_off,
                    'path_str_offset': path_off,
                    'option_start_idx': opt_start,
                    'option_count': opt_count,
                })
                field_count += 1

            page_entry['field_count'] = field_count

    def generate_binary(self):
        """Serialize everything into the UiDesc binary format."""
        page_count = len(self.pages)
        field_count = len(self.fields)
        option_count = len(self.options)
        condition_count = len(self.conditions)

        # Calculate sizes
        header_size = 24  # UI_DESC_HEADER
        pages_size = page_count * 12  # UI_PAGE_ENTRY
        fields_size = field_count * 24  # UI_FIELD_ENTRY
        options_size = option_count * 8  # UI_OPTION_ENTRY
        conditions_size = condition_count * 12  # UI_CONDITION_ENTRY
        string_table_offset = header_size + pages_size + fields_size + options_size + conditions_size
        total_size = string_table_offset + len(self.strings)

        data = bytearray()

        # Header
        data += struct.pack('<IBBHHHHHII',
            UI_DESC_SIGNATURE, UI_DESC_VERSION, 0,
            page_count, field_count, option_count, condition_count, 0,
            string_table_offset, total_size)

        # Pages
        for page in self.pages:
            name_off = self.add_string(page['title'])
            data += struct.pack('<HHIHH',
                page['page_id'], page['parent_id'], name_off,
                page['field_start'], page['field_count'])

        # Fields
        for field in self.fields:
            cond_idx = field['condition_idx']
            if cond_idx > 254:
                cond_idx = 0xFF  # Cap at unconditional
            data += struct.pack('<HHHBBIIIHH',
                field['tag_id'], field['bit_offset'], field['bit_length'],
                field['field_type'], cond_idx,
                field['name_str_offset'], field['help_str_offset'], field['path_str_offset'],
                field['option_start_idx'], field['option_count'])

        # Options
        for opt in self.options:
            data += struct.pack('<II', opt['value'], opt['label_offset'])

        # Conditions
        for cond in self.conditions:
            tag_id, bit_off, bit_len, operator, compare_val = cond
            data += struct.pack('<HHHBBI',
                tag_id, bit_off, bit_len, operator, 0, compare_val)

        # Fix string table offset (strings may have grown during page name serialization)
        # Recalculate
        actual_str_offset = len(data)
        struct.pack_into('<I', data, 16, actual_str_offset)  # StringTableOffset

        # String table
        data += self.strings

        # Fix total size
        struct.pack_into('<I', data, 20, len(data))  # TotalSize

        return bytes(data)

    def generate(self, output_file):
        """Main generation entry point."""
        page_map = self.collect_pages()

        if len(self.pages) == 0:
            print("Warning: No pages found in YAML configuration")
            return

        self.collect_fields(page_map)

        binary_data = self.generate_binary()

        if output_file.lower().endswith('.h'):
            # Generate C header with byte array
            self.write_c_header(binary_data, output_file)
        else:
            # Write raw binary
            with open(output_file, 'wb') as f:
                f.write(binary_data)

        print(f"Generated UiDesc: {len(self.pages)} pages, {len(self.fields)} fields, "
              f"{len(self.options)} options, {len(self.conditions)} conditions, "
              f"{len(binary_data)} bytes total")

    def write_c_header(self, data, filename):
        """Write binary data as a C header file with UINT8 array."""
        with open(filename, 'w') as f:
            f.write("/** @file\n")
            f.write("  AUTO-GENERATED UI Descriptor binary data.\n")
            f.write("  Generated by GenUiDescBin.py from CfgDataDef.yaml\n")
            f.write("  DO NOT EDIT MANUALLY.\n\n")
            f.write("  Copyright (c) 2024, Intel Corporation. All rights reserved.<BR>\n")
            f.write("  SPDX-License-Identifier: BSD-2-Clause-Patent\n")
            f.write("**/\n\n")
            f.write("#ifndef _UI_DESC_DATA_H_\n")
            f.write("#define _UI_DESC_DATA_H_\n\n")
            f.write("STATIC CONST UINT8 mUiDescBin[%d] = {\n" % len(data))

            for i in range(0, len(data), 16):
                chunk = data[i:i+16]
                hex_str = ', '.join('0x%02X' % b for b in chunk)
                f.write("  %s,\n" % hex_str)

            f.write("};\n\n")
            f.write("#endif // _UI_DESC_DATA_H_\n")


def main():
    if len(sys.argv) < 3:
        print("Usage: GenUiDescBin.py <CfgDataDef.yaml> <output.bin|output.h>")
        print("  Generates UI descriptor binary or C header from YAML config definition.")
        return 1

    yaml_file = sys.argv[1]
    output_file = sys.argv[2]

    if not os.path.exists(yaml_file):
        print(f"Error: YAML file '{yaml_file}' not found")
        return 1

    # Load YAML using GenCfgData
    gen_cfg_data = CGenCfgData()
    gen_cfg_data.load_yaml(yaml_file)

    # Generate UI descriptor
    generator = UiDescGenerator(gen_cfg_data)
    generator.generate(output_file)

    return 0


if __name__ == '__main__':
    sys.exit(main())
