## @file
# Unit tests for GenCfgData.py.
#
# Covers the pure text/value helpers, the sandboxed expression evaluator,
# and the YAML -> binary -> YAML and delta override contracts that the
# configuration data build depends on.
#
# Copyright (c) 2026, Intel Corporation. All rights reserved.<BR>
# SPDX-License-Identifier: BSD-2-Clause-Patent
##

import os
import glob

import pytest

import GenCfgData as G


QEMU_CFG_DIR = os.path.join('Platform', 'QemuBoardPkg', 'CfgData')
PLATFORM_ID = 'PLATFORMID_CFG_DATA.PlatformId'


def all_cfg_def_yaml(workspace):
    pattern = os.path.join(workspace, 'Platform', '*', 'CfgData',
                           'CfgDataDef.yaml')
    return sorted(glob.glob(pattern))


def load_cfg(yaml_file, dlt_file=None):
    cfg = G.CGenCfgData()
    assert cfg.load_yaml(yaml_file) == 0, cfg.get_last_error()
    if dlt_file:
        assert cfg.override_default_value(dlt_file) == 0
    return cfg


def item_bytes(cfg, path, data):
    '''Return the bytes a configuration item occupies in a generated blob.'''
    top = cfg.locate_cfg_item(path, False)
    assert top is not None, 'item %s not found' % path
    act = cfg.get_item_by_index(top['indx'])
    offset = act['offset'] // 8
    length = act['length'] // 8
    return bytes(data[offset:offset + length])


#
# Text helpers.
#

@pytest.mark.parametrize('text, expected', [
    ("'abc'", True),
    ('"abc"', True),
    ('abc', False),
    ("'abc", False),
    ('"abc\'', False),
])
def test_check_quote(text, expected):
    assert G.check_quote(text) is expected


@pytest.mark.parametrize('text, expected', [
    ("'abc'", 'abc'),
    ('  "abc"  ', 'abc'),
    ('abc', 'abc'),
    ("'abc", "'abc"),
])
def test_strip_quote(text, expected):
    assert G.strip_quote(text) == expected


@pytest.mark.parametrize('text, expected', [
    ('{ 1, 2 }', ' 1, 2 '),
    ('  1, 2  ', '  1, 2  '),
    ('', ''),
])
def test_strip_delimiter(text, expected):
    assert G.strip_delimiter(text, '{}') == expected


def test_bytes_to_bracket_str():
    assert G.bytes_to_bracket_str(b'\x01\x02') == '{ 0x01, 0x02 }'
    assert G.bytes_to_bracket_str(b'') == '{  }'


@pytest.mark.parametrize('text, expected', [
    # Byte arrays are little endian: the first element is the low byte.
    ('{0x01, 0x02}', 0x0201),
    ('{1,2,3,4}', 0x04030201),
    ('0x1234', 0x1234),
    ('{ 0xff }', 0xff),
])
def test_array_str_to_value(text, expected):
    assert G.array_str_to_value(text) == expected


@pytest.mark.parametrize('name, count, expected', [
    ('FOO_BAR', 0, 'FooBar'),
    ('FOO_BAR', 3, 'FooBar[3]'),
])
def test_format_struct_field_name(name, count, expected):
    assert G.CGenCfgData.format_struct_field_name(name, count) == expected


def test_bytes_to_bracket_str_round_trips_through_array_str_to_value():
    raw = b'\x12\x34\x56'
    assert G.array_str_to_value(G.bytes_to_bracket_str(raw)) == 0x563412


#
# Expression evaluator.
#

@pytest.mark.parametrize('expr, variables, expected', [
    ('1 + 2', {}, 3),
    ('A * 2', {'A': 5}, 10),
    ('(1 + 2) * 3', {}, 9),
    ('1 == 1', {}, True),
    ('A > 2 and A < 10', {'A': 5}, True),
])
def test_expression_eval(expr, variables, expected):
    assert G.ExpressionEval().eval(expr, variables) == expected


def test_expression_eval_rejects_ternary():
    '''Only the operators the visitor implements are allowed. A ternary is
       not one of them, so it must raise rather than be silently mis-read.'''
    with pytest.raises(ValueError):
        G.ExpressionEval().eval('1 if 2 > 1 else 0', {})


@pytest.mark.parametrize('expr', [
    '__import__("os").system',
    'open("secret.txt")',
    'A.__class__',
])
def test_expression_eval_rejects_unsafe_nodes(expr):
    '''Config YAML conditions are evaluated, so the evaluator must not
       allow arbitrary attribute access or imports.'''
    with pytest.raises(Exception):
        G.ExpressionEval().eval(expr, {'A': 1})


#
# YAML loading and binary generation.
#

def test_qemu_yaml_loads(workspace):
    cfg = load_cfg(os.path.join(workspace, QEMU_CFG_DIR, 'CfgDataDef.yaml'))
    assert len(cfg.get_cfg_list()) > 0
    assert len(cfg.generate_binary_array()) > 0


def test_every_board_cfg_yaml_loads_and_round_trips(workspace):
    '''YAML -> binary -> reload -> binary must be byte identical for every
       board. A mismatch means a config item cannot be represented by the
       blob it generates, which silently corrupts board settings.'''
    yaml_files = all_cfg_def_yaml(workspace)
    assert yaml_files, 'no CfgDataDef.yaml found'

    for yaml_file in yaml_files:
        cfg = load_cfg(yaml_file)
        first = bytes(cfg.generate_binary_array())
        assert len(first) > 0, yaml_file

        cfg.load_default_from_bin(bytearray(first))
        second = bytes(cfg.generate_binary_array())
        assert first == second, 'round trip mismatch for %s' % yaml_file


def test_load_default_from_bin_applies_the_supplied_data(workspace):
    yaml_file = os.path.join(workspace, QEMU_CFG_DIR, 'CfgDataDef.yaml')
    cfg = load_cfg(yaml_file)
    data = bytearray(cfg.generate_binary_array())

    top = cfg.locate_cfg_item(PLATFORM_ID, False)
    act = cfg.get_item_by_index(top['indx'])
    offset = act['offset'] // 8
    assert data[offset:offset + 2] == bytearray(b'\x00\x00')

    data[offset] = 0x42
    cfg.load_default_from_bin(data)
    assert item_bytes(cfg, PLATFORM_ID,
                      cfg.generate_binary_array()) == b'\x42\x00'


#
# Delta (.dlt) overrides.
#

def test_dlt_changes_the_generated_binary(workspace):
    cfg_dir = os.path.join(workspace, QEMU_CFG_DIR)
    yaml_file = os.path.join(cfg_dir, 'CfgDataDef.yaml')

    base = load_cfg(yaml_file)
    base_bin = bytes(base.generate_binary_array())
    assert item_bytes(base, PLATFORM_ID, base_bin) == b'\x00\x00'

    over = load_cfg(yaml_file, os.path.join(cfg_dir, 'CfgDataExt_Brd1.dlt'))
    over_bin = bytes(over.generate_binary_array())

    assert len(over_bin) == len(base_bin)
    assert over_bin != base_bin
    assert item_bytes(over, PLATFORM_ID, over_bin) == b'\x01\x00'


def write_dlt(tmp_path, text):
    dlt = tmp_path / 'test.dlt'
    dlt.write_text(text)
    return str(dlt)


def platform_id_value(workspace, tmp_path, text):
    yaml_file = os.path.join(workspace, QEMU_CFG_DIR, 'CfgDataDef.yaml')
    cfg = load_cfg(yaml_file, write_dlt(tmp_path, text))
    top = cfg.locate_cfg_item(PLATFORM_ID, False)
    return cfg.get_item_by_index(top['indx'])['value']


def test_dlt_last_assignment_wins(workspace, tmp_path):
    text = '%s | 3\n%s | 7\n' % (PLATFORM_ID, PLATFORM_ID)
    assert platform_id_value(workspace, tmp_path, text) == '0x0007'


def test_dlt_ignores_comments_and_blank_lines(workspace, tmp_path):
    text = '# %s | 3\n\n   \n%s | 5\n' % (PLATFORM_ID, PLATFORM_ID)
    assert platform_id_value(workspace, tmp_path, text) == '0x0005'


def test_dlt_expands_include_directive(workspace, tmp_path):
    include = os.path.join(workspace, QEMU_CFG_DIR, 'CfgDataExt_Inc.dlt')
    text = '!include %s\n%s | 9\n' % (include.replace('\\', '/'), PLATFORM_ID)
    assert platform_id_value(workspace, tmp_path, text) == '0x0009'


def test_dlt_rejects_malformed_line(workspace, tmp_path):
    with pytest.raises(Exception):
        platform_id_value(workspace, tmp_path, 'this is not a delta line\n')


def test_dlt_rejects_unknown_config_path(workspace, tmp_path):
    '''A typo in a .dlt must fail the build rather than be ignored, or the
       board silently keeps the default value.'''
    with pytest.raises(Exception):
        platform_id_value(workspace, tmp_path, 'NO_SUCH_CFG.Nope | 1\n')


def test_generated_delta_reproduces_the_modified_binary(workspace, tmp_path):
    '''Deltas are generated from binaries during firmware update and board
       bring up, then replayed. Generate -> apply must be lossless.'''
    cfg_dir = os.path.join(workspace, QEMU_CFG_DIR)
    yaml_file = os.path.join(cfg_dir, 'CfgDataDef.yaml')
    brd1_dlt = os.path.join(cfg_dir, 'CfgDataExt_Brd1.dlt')

    base_bin = bytes(load_cfg(yaml_file).generate_binary_array())

    over = load_cfg(yaml_file, brd1_dlt)
    over_bin = bytes(over.generate_binary_array())

    delta = str(tmp_path / 'generated.dlt')
    over.generate_delta_file_from_bin(delta, bytearray(base_bin),
                                      bytearray(over_bin))
    assert os.path.getsize(delta) > 0

    replayed = load_cfg(yaml_file, delta)
    assert bytes(replayed.generate_binary_array()) == over_bin
