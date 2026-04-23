#!/usr/bin/env python
## @file
#  Generate YAML CfgData entries from FSP UPD header files.
#
#  Parses FspmUpd.h / FspsUpd.h and generates YAML files compatible
#  with SBL ConfigEditor.  Each UPD field becomes a CFGDATA entry
#  that can be edited via ConfigEditor.  The values are used by
#  GenFspUpdDelta.py to patch the FSPUPD.bin at build time.
#
#  Copyright (c) 2026, Intel Corporation. All rights reserved.<BR>
#  SPDX-License-Identifier: BSD-2-Clause-Patent
##

import os
import re
import sys
import struct
import argparse

# ---------------------------------------------------------------
# Constants
# ---------------------------------------------------------------
TYPE_MAP = {
    'UINT8':  1,
    'UINT16': 2,
    'UINT32': 4,
    'UINT64': 8,
    'INT8':   1,
    'INT16':  2,
    'INT32':  4,
    'INT64':  8,
    'CHAR8':  1,
    'BOOLEAN': 1,
}

# Fields that are always set at runtime (not from CFGDATA)
FSPM_RUNTIME_FIELDS = {
    'FspUpdHeader', 'FspmArchUpd',
    'TerminatorReserved', 'UpdTerminator',
}

FSPS_RUNTIME_FIELDS = {
    'FspUpdHeader', 'FspsArchUpd',
    'TerminatorReserved', 'UpdTerminator',
}


# ---------------------------------------------------------------
# Parser
# ---------------------------------------------------------------
class FspUpdField:
    """Represents one field in an FSP UPD structure."""
    __slots__ = ('offset', 'name', 'ctype', 'array_len', 'elem_size',
                 'total_size', 'comment', 'help_lines', 'options',
                 'is_reserved')

    def __init__(self):
        self.offset = 0
        self.name = ''
        self.ctype = ''
        self.array_len = 0
        self.elem_size = 0
        self.total_size = 0
        self.comment = ''
        self.help_lines = []
        self.options = ''
        self.is_reserved = False


def parse_fsp_upd_header(header_path, struct_name):
    """Parse an FSP UPD header and extract field definitions.

    Args:
        header_path:  Path to FspmUpd.h or FspsUpd.h
        struct_name:  'FSP_M_CONFIG' or 'FSP_S_CONFIG'

    Returns:
        list of FspUpdField
    """
    with open(header_path, 'r', encoding='utf-8', errors='replace') as f:
        lines = f.readlines()

    # Find the struct boundaries by looking for the closing `} STRUCT_NAME;`
    # and then scanning backwards for the matching `typedef struct {`.
    struct_start = -1
    struct_end = -1

    # First find the closing line
    for i, line in enumerate(lines):
        m = re.match(r'}\s+' + re.escape(struct_name) + r'\s*;', line.strip())
        if m:
            struct_end = i
            break

    if struct_end < 0:
        raise ValueError(f"Could not find '}} {struct_name};' in {header_path}")

    # Scan backwards to find the matching `typedef struct {`
    # Skip comment blocks (/** ... **/) when counting braces
    depth = 0
    in_comment = False
    for i in range(struct_end, -1, -1):
        stripped = lines[i].strip()
        # Track comment blocks (scanning backwards, so **/ opens and /** closes)
        if stripped.endswith('**/') or stripped.endswith('*/'):
            in_comment = True
        if stripped.startswith('/**') or stripped.startswith('/*'):
            in_comment = False
            continue
        if in_comment:
            continue
        # Count braces only on non-comment lines
        for ch in lines[i]:
            if ch == '}':
                depth += 1
            elif ch == '{':
                depth -= 1
        if depth == 0:
            # This line contains the matching `typedef struct {`
            struct_start = i + 1
            break

    if struct_start < 0:
        raise ValueError(f"Could not find 'typedef struct {{ ... }} {struct_name};' in {header_path}")

    # Now parse fields between struct_start and struct_end
    fields = []
    current_comment_lines = []
    current_offset = None
    current_field_name_from_comment = None
    current_help = []
    current_options = ''

    i = struct_start
    while i < struct_end:
        line = lines[i]
        stripped = line.strip()

        # Detect comment block: /** Offset 0xNNNN - FieldName
        offset_match = re.match(r'/\*\*\s+Offset\s+(0x[0-9A-Fa-f]+)\s*(?:-\s*(.+))?', stripped)
        if offset_match:
            current_offset = int(offset_match.group(1), 16)
            current_field_name_from_comment = offset_match.group(2)
            if current_field_name_from_comment:
                current_field_name_from_comment = current_field_name_from_comment.strip()
            current_help = []
            current_options = ''
            option_lines = []

            # Read until closing */
            i += 1
            while i < struct_end:
                cline = lines[i].strip()
                if cline.endswith('**/') or cline.endswith('*/'):
                    cline = cline.replace('**/', '').replace('*/', '').strip()
                    if cline:
                        # Check for option pattern like "0:Disabled, 1:Enabled" or "0xF4:-12"
                        # Also handle lines starting with HTML tags like "<b>0xff:none</b>"
                        cline_stripped = re.sub(r'</?b\s*>', '', cline).strip()
                        if re.match(r'^(?:\d+|0x[0-9A-Fa-f]+)\s*:', cline_stripped, re.IGNORECASE) or cline_stripped.startswith('$'):
                            option_lines.append(cline)
                        else:
                            current_help.append(cline)
                    i += 1
                    break
                else:
                    # Check for option pattern
                    cline_stripped = re.sub(r'</?b\s*>', '', cline).strip()
                    if re.match(r'^(?:\d+|0x[0-9A-Fa-f]+)\s*:', cline_stripped, re.IGNORECASE) or cline_stripped.startswith('$'):
                        option_lines.append(cline)
                    else:
                        current_help.append(cline)
                    i += 1

            # Post-process option lines:
            if option_lines:
                # 1. If $EN_DIS (or similar macro) is present alongside explicit options,
                #    the macro takes precedence — move explicit option text to help.
                macro_lines = [l for l in option_lines if l.startswith('$')]
                if macro_lines and len(option_lines) > len(macro_lines):
                    for l in option_lines:
                        if not l.startswith('$'):
                            current_help.append(l)
                    option_lines = macro_lines

                # 2. Strip trailing periods from each option line (e.g. "2: Disable (Audio PLL).")
                option_lines = [l.rstrip('.') for l in option_lines]

                # 3. Deduplicate identical option lines (some headers repeat the same options)
                seen = []
                for l in option_lines:
                    if l not in seen:
                        seen.append(l)
                option_lines = seen

                # 4. When multiple non-macro option lines exist, check if some are
                #    merely help text fragments that happened to start with "N:".
                #    A "complete" option line has >=2 comma-separated N: entries.
                #    If ALL lines are complete, they are multi-line continuations — keep all.
                #    If only SOME are complete, the complete one(s) are the real options;
                #    single-entry fragments are help text.
                #    However, if any line ends with a comma, all lines are a continuation — keep all.
                if len(option_lines) > 1 and not any(l.startswith('$') for l in option_lines):
                    has_continuation = any(l.rstrip().endswith(',') for l in option_lines[:-1])
                    if not has_continuation:
                        complete = []
                        fragment = []
                        for idx, l in enumerate(option_lines):
                            parts = [p.strip() for p in l.split(',') if p.strip()]
                            opt_parts = [p for p in parts if re.match(r'^(?:\d+|0x[0-9A-Fa-f]+)\s*:', p, re.IGNORECASE)]
                            if len(opt_parts) >= 2:
                                complete.append(idx)
                            else:
                                fragment.append(idx)
                        if complete and fragment:
                            # Mix of complete and fragment lines — keep only complete ones
                            if len(complete) == 1:
                                best = complete[0]
                            else:
                                best = complete[-1]
                            for idx, l in enumerate(option_lines):
                                if idx != best:
                                    current_help.append(l)
                            option_lines = [option_lines[best]]
                    # else: continuation or all complete — keep all

            # Join multi-line options with proper comma separation
            if option_lines:
                # Strip HTML bold tags first (before separator normalization)
                option_lines = [re.sub(r'</?b\s*>', '', l).strip() for l in option_lines]

                # Ensure trailing comma on each line before joining (for multi-line continuations)
                joined_parts = []
                for ol in option_lines:
                    ol = ol.rstrip()
                    if joined_parts and not joined_parts[-1].endswith(','):
                        joined_parts[-1] += ','
                    joined_parts.append(ol)
                current_options = ' '.join(joined_parts)
                # Clean up: remove trailing comma if any
                current_options = current_options.rstrip(',').strip()
                # Fix period used as separator instead of comma (FSP header typos)
                # e.g. "0xa:4M. 0xb:8M" → "0xa:4M, 0xb:8M"
                current_options = re.sub(
                    r'\.\s+(?=(?:\d+|0x[0-9A-Fa-f]+)\s*:)',
                    ', ',
                    current_options,
                    flags=re.IGNORECASE
                )
                # Fix semicolons used as separators (e.g. "0: Disable; 1: Enable")
                current_options = re.sub(
                    r';\s+(?=(?:\d+|0x[0-9A-Fa-f]+)\s*:)',
                    ', ',
                    current_options,
                    flags=re.IGNORECASE
                )
                # Fix space-only separators between options (e.g. "0:A 1:B" → "0:A, 1:B")
                # But NOT if already preceded by comma
                current_options = re.sub(
                    r'(?<=[^,\s])\s+(?=(?:\d+|0x[0-9A-Fa-f]+)\s*:)',
                    ', ',
                    current_options,
                    flags=re.IGNORECASE
                )
                # Final cleanup: collapse multiple commas, trim
                current_options = re.sub(r',\s*,', ',', current_options).strip()
            continue

        # Detect field declaration
        field_match = re.match(
            r'\s*(UINT8|UINT16|UINT32|UINT64|INT8|INT16|INT32|INT64|CHAR8|BOOLEAN)'
            r'\s+(\w+)(?:\[(\d+)\])?\s*;',
            stripped
        )
        if field_match and current_offset is not None:
            f = FspUpdField()
            f.ctype = field_match.group(1)
            f.name = field_match.group(2)
            f.array_len = int(field_match.group(3)) if field_match.group(3) else 0
            f.elem_size = TYPE_MAP.get(f.ctype, 1)
            f.total_size = f.elem_size * f.array_len if f.array_len else f.elem_size
            f.offset = current_offset
            f.comment = current_field_name_from_comment or f.name
            f.help_lines = current_help
            f.options = current_options
            f.is_reserved = ('Rsvd' in f.name or 'Reserved' in f.name or
                             'Unused' in f.name or f.name.startswith('FspmUpdRsvd') or
                             f.name.startswith('FspsUpdRsvd') or
                             f.name.startswith('UnusedUpdSpace'))

            fields.append(f)

            # Reset for next field
            current_offset = None
            current_field_name_from_comment = None
            current_help = []
            current_options = ''

        i += 1

    # Sort by offset to ensure fields are in structure layout order
    fields.sort(key=lambda f: f.offset)

    return fields


# ---------------------------------------------------------------
# YAML generation
# ---------------------------------------------------------------
def make_yaml_type_and_option(field):
    """Generate YAML type and option strings for a field."""
    if field.array_len > 0:
        # Array fields always use Table type with byte-level columns,
        # even if they have options (options are per-element but ConfigEditor
        # evaluates the whole multi-byte value against the option list).
        cols = []
        for idx in range(field.total_size):
            cols.append(f'{idx:X}:1:HEX')
        return 'Table', ', '.join(cols)
    elif field.options:
        if field.options.startswith('$'):
            return 'Combo', field.options
        else:
            return 'Combo', field.options
    elif field.elem_size == 1:
        return 'EditNum, HEX, (0x00,0xFF)', ''
    elif field.elem_size == 2:
        return 'EditNum, HEX, (0x0000,0xFFFF)', ''
    elif field.elem_size == 4:
        return 'EditNum, HEX, (0x00000000,0xFFFFFFFF)', ''
    elif field.elem_size == 8:
        return 'EditNum, HEX, (0x0,0xFFFFFFFFFFFFFFFF)', ''
    else:
        return 'EditNum, HEX, (0x00,0xFF)', ''


def field_default_value(field, upd_data, base_offset):
    """Extract the default value from UPD binary data."""
    rel_offset = field.offset - base_offset
    if rel_offset < 0 or rel_offset + field.total_size > len(upd_data):
        # Fallback
        if field.array_len > 0:
            return '{ ' + ', '.join(['0x00'] * field.total_size) + ' }'
        return '0x0'

    raw = upd_data[rel_offset:rel_offset + field.total_size]

    if field.array_len > 0:
        # For Table type, GenCfgData.py { } notation expects raw bytes.
        # Always emit byte-level values for arrays regardless of elem_size.
        vals = [f'0x{b:02X}' for b in raw]
        return '{ ' + ', '.join(vals) + ' }'
    else:
        v = int.from_bytes(raw, 'little')
        return f'0x{v:0{field.elem_size * 2}X}'


def generate_yaml(fields, tag_name, tag_id, page_name, page_title,
                  upd_data=None, base_offset=0, runtime_fields=None,
                  flags=0, max_items_per_page=60):
    """Generate YAML content for a set of UPD fields.

    Automatically splits into multiple CFGDATA tags if the total data
    exceeds the CFGDATA tag maximum payload (8180 bytes usable per tag,
    since the 11-bit length field limits tags to 2047*4=8188 bytes total
    including the 8-byte header).

    When max_items_per_page > 0, inserts $ACTION sub-page declarations
    so that ConfigEditor.py can display fields in smaller groups
    instead of rendering all items on a single slow page.

    Args:
        fields:         list of FspUpdField
        tag_name:       e.g. 'FSPM_UPD_CFG_DATA'
        tag_id:         e.g. '0x390'
        page_name:      e.g. 'FUPM'
        page_title:     e.g. 'FSP-M UPD Settings'
        upd_data:       bytes from FSP binary CfgRegion (optional)
        base_offset:    offset of config struct within the UPD (e.g., 0x60 for FSP_M_CONFIG)
        runtime_fields: set of field names to skip
        max_items_per_page: max visible items per sub-page (0 to disable)

    Returns:
        str: YAML content
    """
    if runtime_fields is None:
        runtime_fields = set()

    # CFGDATA tag max payload = 2047*4 - 8 (header) = 8180 bytes
    MAX_TAG_PAYLOAD = 8180

    # Filter out runtime fields and compute total size
    active_fields = [f for f in fields if f.name not in runtime_fields]

    # Split fields into chunks that fit within MAX_TAG_PAYLOAD
    chunks = []
    current_chunk = []
    current_size = 0
    for field in active_fields:
        if current_size + field.total_size > MAX_TAG_PAYLOAD and current_chunk:
            chunks.append(current_chunk)
            current_chunk = []
            current_size = 0
        current_chunk.append(field)
        current_size += field.total_size
    if current_chunk:
        chunks.append(current_chunk)

    tag_id_int = int(tag_id, 16) if isinstance(tag_id, str) else tag_id

    yaml_lines = []
    yaml_lines.append('## @file')
    yaml_lines.append('#')
    yaml_lines.append('#  Auto-generated FSP UPD configuration for ConfigEditor.')
    yaml_lines.append('#  Generated by GenFspUpdYaml.py - DO NOT EDIT MANUALLY')
    yaml_lines.append('#')
    yaml_lines.append('#  Copyright (c) 2026, Intel Corporation. All rights reserved.<BR>')
    yaml_lines.append('#  SPDX-License-Identifier: BSD-2-Clause-Patent')
    yaml_lines.append('#')
    yaml_lines.append('##')
    yaml_lines.append('')
    yaml_lines.append('')

    for chunk_idx, chunk_fields in enumerate(chunks):
        if len(chunks) == 1:
            cur_tag_name = tag_name
            cur_tag_id = f'0x{tag_id_int:03X}'
        else:
            # Put chunk number before _CFG_DATA so GenCfgData.py's [:-9]
            # produces unique macro names (e.g. FSPS_UPD0_CFG_DATA → CDATA_FSPS_UPD0_TAG)
            base = tag_name[:-9]  # strip '_CFG_DATA'
            cur_tag_name = f'{base}{chunk_idx}_CFG_DATA'
            cur_tag_id = f'0x{tag_id_int + chunk_idx:03X}'

        yaml_lines.append(f'- $ACTION      :')
        yaml_lines.append(f'    page         : {page_name}')
        yaml_lines.append(f'- {cur_tag_name} :')
        yaml_lines.append(f'  - !expand {{ CFGHDR_TMPL : [ {cur_tag_name}, {cur_tag_id}, {flags}, 0 ] }}')

        # Sub-page tracking: split visible items across sub-pages
        # Use a 2-char prefix derived from page_name (e.g. FUPM→FM, FUPS→FS)
        page_prefix = page_name[0] + page_name[-1]
        visible_count = 0
        subpage_idx = 0

        for field in chunk_fields:
            # For reserved fields, emit as non-editable padding
            if field.is_reserved:
                yaml_lines.append(f'  - {field.name} :')
                yaml_lines.append(f'      length       : 0x{field.total_size:02X}')
                if upd_data:
                    yaml_lines.append(f'      value        : {field_default_value(field, upd_data, base_offset)}')
                else:
                    if field.array_len > 0:
                        yaml_lines.append(f'      value        : {{ {", ".join(["0x00"] * field.total_size)} }}')
                    else:
                        yaml_lines.append(f'      value        : 0x{"00" * field.total_size}')
                continue

            # Insert sub-page boundary when item count reaches threshold
            if max_items_per_page > 0 and visible_count % max_items_per_page == 0:
                subpage_idx += 1
                sub_id = f'{page_prefix}{subpage_idx:02d}'
                sub_title = field.comment or field.name
                # Truncate long titles
                if len(sub_title) > 40:
                    sub_title = sub_title[:37] + '...'
                yaml_lines.append(f'  - $ACTION      :')
                yaml_lines.append(f'      page         : {sub_id}:{page_name}:"{sub_title}"')
            visible_count += 1

            # Determine type and option
            yaml_type, yaml_option = make_yaml_type_and_option(field)

            # Generate help text — prepend the human-readable comment so
            # the description is still visible when name shows the C field name.
            help_parts = []
            if field.comment:
                help_parts.append(field.comment + '.')
            if field.help_lines:
                help_parts.append(' '.join(field.help_lines).strip())
            help_text = ' '.join(help_parts) if help_parts else field.name

            # Sanitize help text (remove problematic YAML characters)
            help_text = help_text.replace('"', "'").replace('\\n', ' ').replace('\n', ' ')

            yaml_lines.append(f'  - {field.name} :')
            yaml_lines.append(f'      name         : {field.name}')
            if yaml_type.startswith('Table'):
                yaml_lines.append(f'      type         : Table')
                yaml_lines.append(f'      option       : {yaml_option}')
            elif yaml_type == 'Combo':
                yaml_lines.append(f'      type         : Combo')
                yaml_lines.append(f'      option       : {yaml_option}')
            else:
                yaml_lines.append(f'      type         : {yaml_type}')
            yaml_lines.append(f'      help         : >')
            yaml_lines.append(f'                     {help_text}')
            yaml_lines.append(f'      length       : 0x{field.total_size:02X}')

            # Default value
            if upd_data:
                yaml_lines.append(f'      value        : {field_default_value(field, upd_data, base_offset)}')
            else:
                if field.array_len > 0:
                    yaml_lines.append(f'      value        : {{ {", ".join(["0x00"] * field.total_size)} }}')
                else:
                    yaml_lines.append(f'      value        : 0x{"00" * field.elem_size}')

    yaml_lines.append('')
    return '\n'.join(yaml_lines)


# ---------------------------------------------------------------
# FSP binary CfgRegion extraction (reused from GenFspUpdDelta.py)
# ---------------------------------------------------------------
FSP_INFO_HEADER_OFF = 0x94
FIH_CFGREGION_OFF_OFF = 0x24
FIH_CFGREGION_SIZE_OFF = 0x28


def get_fsp_cfg_region(fsp_bin_path):
    """Extract CfgRegion data from an FSP binary."""
    with open(fsp_bin_path, 'rb') as f:
        data = f.read()

    h = FSP_INFO_HEADER_OFF
    cfg_offset = struct.unpack_from('<I', data, h + FIH_CFGREGION_OFF_OFF)[0]
    cfg_size = struct.unpack_from('<I', data, h + FIH_CFGREGION_SIZE_OFF)[0]

    if cfg_size == 0:
        return None, 0

    return data[cfg_offset:cfg_offset + cfg_size], cfg_size


# ---------------------------------------------------------------
# High-level API
# ---------------------------------------------------------------
def gen_fsp_upd_yaml_files(fsp_path, fv_dir, cfg_dir):
    """Generate CfgData_FspM.yaml and CfgData_FspS.yaml.

    Parses FSP UPD headers and extracts defaults from rebased FSP
    binaries, then writes YAML files into cfg_dir.

    Args:
        fsp_path:  Directory containing FspmUpd.h / FspsUpd.h.
        fv_dir:    Build FV directory containing FSP_M.bin / FSP_S.bin.
        cfg_dir:   Output directory for the generated YAML files.
    """
    fspm_hdr = os.path.join(fsp_path, 'FspmUpd.h')
    fsps_hdr = os.path.join(fsp_path, 'FspsUpd.h')
    if not os.path.exists(fspm_hdr) or not os.path.exists(fsps_hdr):
        raise Exception('FSP UPD headers not found in %s' % fsp_path)

    fspm_bin = os.path.join(fv_dir, 'FSP_M.bin')
    fsps_bin = os.path.join(fv_dir, 'FSP_S.bin')

    print('Generating FSP UPD YAML files ...')

    fspm_fields = parse_fsp_upd_header(fspm_hdr, 'FSP_M_CONFIG')
    print('  FSP-M: %d fields from %s' % (len(fspm_fields), os.path.basename(fspm_hdr)))

    fsps_fields = parse_fsp_upd_header(fsps_hdr, 'FSP_S_CONFIG')
    print('  FSP-S: %d fields from %s' % (len(fsps_fields), os.path.basename(fsps_hdr)))

    fspm_data = None
    fsps_data = None
    if os.path.exists(fspm_bin):
        fspm_data, _ = get_fsp_cfg_region(fspm_bin)
    if os.path.exists(fsps_bin):
        fsps_data, _ = get_fsp_cfg_region(fsps_bin)

    fspm_yaml_text = generate_yaml(
        fspm_fields, 'FSPM_UPD_CFG_DATA', '0x500',
        'FUPM', 'FSP-M UPD Settings',
        upd_data=fspm_data, base_offset=0,
        runtime_fields=FSPM_RUNTIME_FIELDS, flags=4)
    fsps_yaml_text = generate_yaml(
        fsps_fields, 'FSPS_UPD_CFG_DATA', '0x520',
        'FUPS', 'FSP-S UPD Settings',
        upd_data=fsps_data, base_offset=0,
        runtime_fields=FSPS_RUNTIME_FIELDS, flags=4)

    fspm_yaml = os.path.join(cfg_dir, 'CfgData_FspM.yaml')
    fsps_yaml = os.path.join(cfg_dir, 'CfgData_FspS.yaml')
    with open(fspm_yaml, 'w', newline='\n') as f:
        f.write(fspm_yaml_text)
    with open(fsps_yaml, 'w', newline='\n') as f:
        f.write(fsps_yaml_text)

    print('  Generated %s' % fspm_yaml)
    print('  Generated %s' % fsps_yaml)


# ---------------------------------------------------------------
# Main
# ---------------------------------------------------------------
def main():
    parser = argparse.ArgumentParser(
        description='Generate YAML CfgData entries from FSP UPD header files')
    parser.add_argument('--fspm-header', required=True,
                        help='Path to FspmUpd.h')
    parser.add_argument('--fsps-header', required=True,
                        help='Path to FspsUpd.h')
    parser.add_argument('--fspm-bin', default='',
                        help='Path to FSP_M.bin (to extract default values)')
    parser.add_argument('--fsps-bin', default='',
                        help='Path to FSP_S.bin (to extract default values)')
    parser.add_argument('--fspm-yaml', default='CfgData_FspM.yaml',
                        help='Output YAML for FSP-M UPD')
    parser.add_argument('--fsps-yaml', default='CfgData_FspS.yaml',
                        help='Output YAML for FSP-S UPD')
    parser.add_argument('--fspm-tag', default='0x390',
                        help='CFGDATA tag ID for FSP-M UPD')
    parser.add_argument('--fsps-tag', default='0x3A0',
                        help='CFGDATA tag ID for FSP-S UPD')
    parser.add_argument('--fspm-page', default='FUPM::"FSP-M UPD Settings"',
                        help='Page definition for FSP-M')
    parser.add_argument('--fsps-page', default='FUPS::"FSP-S UPD Settings"',
                        help='Page definition for FSP-S')
    parser.add_argument('--flags', type=int, default=0,
                        help='CFGHDR Flags value (e.g. 4 for delta-only)')
    args = parser.parse_args()

    # Parse FSP-M
    print(f"Parsing {args.fspm_header} ...")
    fspm_fields = parse_fsp_upd_header(args.fspm_header, 'FSP_M_CONFIG')
    print(f"  Found {len(fspm_fields)} fields in FSP_M_CONFIG")

    # Parse FSP-S
    print(f"Parsing {args.fsps_header} ...")
    fsps_fields = parse_fsp_upd_header(args.fsps_header, 'FSP_S_CONFIG')
    print(f"  Found {len(fsps_fields)} fields in FSP_S_CONFIG")

    # Extract default values from FSP binaries if provided
    fspm_data = None
    fsps_data = None
    if args.fspm_bin and os.path.exists(args.fspm_bin):
        fspm_data, _ = get_fsp_cfg_region(args.fspm_bin)
        print(f"  Extracted {len(fspm_data) if fspm_data else 0} bytes from FSP-M CfgRegion")
    if args.fsps_bin and os.path.exists(args.fsps_bin):
        fsps_data, _ = get_fsp_cfg_region(args.fsps_bin)
        print(f"  Extracted {len(fsps_data) if fsps_data else 0} bytes from FSP-S CfgRegion")

    # The CfgRegion data IS the entire FSPM_UPD / FSPS_UPD struct
    # Field offsets in the header are relative to the UPD struct base (offset 0)
    # So base_offset should be 0 to use field.offset directly as index into CfgRegion
    fspm_config_base = 0
    # FSP_S_CONFIG starts at offset 0x40 within FSPS_UPD
    fsps_config_base = 0

    # Extract page name from page definition
    fspm_page_name = args.fspm_page.split(':')[0]
    fsps_page_name = args.fsps_page.split(':')[0]

    # Generate YAML
    fspm_yaml = generate_yaml(
        fspm_fields, 'FSPM_UPD_CFG_DATA', args.fspm_tag,
        fspm_page_name, 'FSP-M UPD Settings',
        upd_data=fspm_data, base_offset=fspm_config_base,
        runtime_fields=FSPM_RUNTIME_FIELDS,
        flags=args.flags
    )
    fsps_yaml = generate_yaml(
        fsps_fields, 'FSPS_UPD_CFG_DATA', args.fsps_tag,
        fsps_page_name, 'FSP-S UPD Settings',
        upd_data=fsps_data, base_offset=fsps_config_base,
        runtime_fields=FSPS_RUNTIME_FIELDS,
        flags=args.flags
    )

    # Write output
    with open(args.fspm_yaml, 'w', newline='\n') as f:
        f.write(fspm_yaml)
    print(f"Generated {args.fspm_yaml}")

    with open(args.fsps_yaml, 'w', newline='\n') as f:
        f.write(fsps_yaml)
    print(f"Generated {args.fsps_yaml}")


if __name__ == '__main__':
    main()
