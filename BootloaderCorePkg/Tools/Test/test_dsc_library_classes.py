## @ test_dsc_library_classes.py
#
# Tests for CheckDscLibraryClasses.py, the static DSC library-class checker.
#
# The regression test at the bottom reproduces the real defect this checker
# exists to prevent: commit de041423 split CsmePerfIdToStrLib out of
# LoaderPerformanceLib and updated BootloaderCorePkg.dsc but not
# PayloadPkg.dsc, leaving PayloadPkg.dsc unbuildable for about seven months
# because no CI job built it standalone.
#
# Copyright (c) 2026, Intel Corporation. All rights reserved.<BR>
# SPDX-License-Identifier: BSD-2-Clause-Patent
#
##

import os

import pytest

import CheckDscLibraryClasses as Chk


# ---------------------------------------------------------------------------
# eval_condition: the DSC !if expression evaluator
# ---------------------------------------------------------------------------

@pytest.mark.parametrize ('expr, defines, expected', [
    # An unset feature knob must evaluate false, not blow up.
    ('$(ENABLE_USB_KB) == 1', {}, False),
    ('$(ENABLE_USB_KB) == 1', {'ENABLE_USB_KB': '1'}, True),
    # Hex values must not be mangled into identifiers.
    ('$(BUILD_CSME_UPDATE_DRIVER)', {'BUILD_CSME_UPDATE_DRIVER': '0x0'},
     False),
    ('$(ENABLE_FWU)', {'ENABLE_FWU': '0x1'}, True),
    ('$(UCODE_SIZE) > 0', {'UCODE_SIZE': '0x1000'}, True),
    ('$(UCODE_SIZE) > 0', {'UCODE_SIZE': '0x0'}, False),
    # String comparison against a bare word.
    ('$(TARGET) == RELEASE', {'TARGET': 'RELEASE'}, True),
    ('$(TARGET) == RELEASE', {'TARGET': 'DEBUG'}, False),
    # defined() is answered before macro expansion.
    ('defined($(FOO))', {'FOO': '1'}, True),
    ('defined($(FOO))', {}, False),
    # Boolean operators.
    ('$(A) == 1 and $(B) == 1', {'A': '1', 'B': '1'}, True),
    ('$(A) == 1 and $(B) == 1', {'A': '1', 'B': '0'}, False),
    ('TRUE', {}, True),
    ('FALSE', {}, False),
])
def test_eval_condition (expr, defines, expected):
    assert Chk.eval_condition (expr, defines) is expected


def test_eval_condition_unparsable_is_inclusive ():
    # An expression we cannot understand must include its body rather than
    # silently drop it, otherwise a missing mapping could hide inside.
    assert Chk.eval_condition ('$(A) ~~~ $(B)', {}) is True


# ---------------------------------------------------------------------------
# DSC / INF parsing
# ---------------------------------------------------------------------------

def test_component_override_block_is_parsed (workspace):
    """Stage1A maps FspApiLib inside its '{ <LibraryClasses> }' block only."""
    dsc_path = os.path.join (workspace, 'BootloaderCorePkg',
                             'BootloaderCorePkg.dsc')
    dsc = Chk.Dsc (dsc_path, workspace)
    stage1a = 'BootloaderCorePkg/Stage1A/Stage1A.inf'
    assert stage1a in dsc.components
    assert 'FspApiLib' in dsc.overrides[stage1a]
    assert dsc.overrides[stage1a]['FspApiLib'].endswith ('FsptApiLib.inf')
    # The override must not leak into the DSC-wide map.
    assert 'FspApiLib' not in dsc.lib_map


def test_override_wins_over_global_map (workspace):
    dsc_path = os.path.join (workspace, 'BootloaderCorePkg',
                             'BootloaderCorePkg.dsc')
    dsc = Chk.Dsc (dsc_path, workspace)
    stage1a = 'BootloaderCorePkg/Stage1A/Stage1A.inf'
    stage1b = 'BootloaderCorePkg/Stage1B/Stage1B.inf'
    assert dsc.map_for (stage1a)['FspApiLib'] != \
        dsc.map_for (stage1b)['FspApiLib']


def test_inf_declares_its_own_library_class (workspace):
    inf = Chk.Inf (os.path.join (
        workspace, 'BootloaderCommonPkg', 'Library', 'LoaderPerformanceLib',
        'LoaderPerformanceLib.inf'), workspace)
    assert inf.library_class == 'LoaderPerformanceLib'
    assert 'CsmePerfIdToStrLib' in inf.required


# ---------------------------------------------------------------------------
# Whole-tree checks
# ---------------------------------------------------------------------------

def test_standalone_dsc_resolves (workspace):
    checker = Chk.Checker (workspace)
    for dsc_rel in Chk.STANDALONE_DSC_LIST:
        errors, _ = checker.check_dsc (dsc_rel)
        assert errors == [], '\n'.join (errors)


def test_every_board_resolves (workspace, tmp_path):
    """BootloaderCorePkg.dsc must resolve for every board in the tree."""
    boards, load_errors = Chk.load_boards (workspace)
    assert not load_errors, 'BoardConfig failed to load: %s' % load_errors
    assert len (boards) > 5, 'suspiciously few boards discovered'

    checker = Chk.Checker (workspace)
    failures = []
    for name in sorted (boards):
        out_path = str (tmp_path / ('%s-Platform.dsc' % name))
        Chk.gen_platform_dsc (boards[name], out_path)
        for dsc_rel in Chk.BOARD_DSC_LIST:
            errors, _ = checker.check_dsc (
                dsc_rel, {'Platform.dsc': out_path}, '%s [%s]'
                % (dsc_rel, name))
            failures.extend (errors)
    assert failures == [], '\n'.join (failures)


# ---------------------------------------------------------------------------
# Regression: the defect this checker was written for
# ---------------------------------------------------------------------------

def test_detects_missing_library_class_mapping (workspace, tmp_path):
    """Removing the CsmePerfIdToStrLib mapping must be reported."""
    src = os.path.join (workspace, 'PayloadPkg', 'PayloadPkg.dsc')
    with open (src, 'r', encoding='utf-8') as fp:
        lines = fp.readlines ()

    kept = [ln for ln in lines if 'CsmePerfIdToStrLib' not in ln]
    assert len (kept) == len (lines) - 1, \
        'expected exactly one CsmePerfIdToStrLib mapping in PayloadPkg.dsc'

    broken = tmp_path / 'PayloadPkg.dsc'
    broken.write_text (''.join (kept), encoding='utf-8')

    checker = Chk.Checker (workspace)
    errors, _ = checker.check_dsc (str (broken))

    assert any ('CsmePerfIdToStrLib' in err for err in errors), \
        'checker did not flag the removed mapping: %s' % errors
    # The message must name the library that pulled the class in, otherwise
    # it is not actionable.
    assert any ('LoaderPerformanceLib' in err for err in errors), errors


def test_missing_include_is_reported_not_ignored (workspace, tmp_path):
    """An !include that cannot be resolved must be reported, never skipped.

    This is deliberately written against a synthetic DSC rather than against
    BootloaderCorePkg.dsc.  That file includes the generated Platform.dsc,
    which is gitignored: it is absent in a fresh clone but present the moment
    anyone runs a build or the board checker.  Asserting on the real file made
    the result depend on whether the developer had built before, which is
    exactly the kind of flaky, environment-dependent test that stops people
    trusting the suite.
    """
    dsc = tmp_path / 'Synthetic.dsc'
    dsc.write_text (
        '[Defines]\n'
        '!include NoSuchDirectory/DefinitelyMissing.dsc\n'
        '[LibraryClasses]\n'
        '  BaseLib|MdePkg/Library/BaseLib/BaseLib.inf\n',
        encoding='utf-8')

    checker = Chk.Checker (workspace)
    _, stats = checker.check_dsc (str (dsc))

    assert any ('DefinitelyMissing.dsc' in inc for inc in stats['missing_includes']), \
        'unresolved !include was silently ignored: %s' % stats['missing_includes']
