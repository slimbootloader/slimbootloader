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
import sys
from types import SimpleNamespace

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
    assert all (key[2] != 'FspApiLib' for key in dsc.lib_map)


def test_override_wins_over_global_map (workspace):
    dsc_path = os.path.join (workspace, 'BootloaderCorePkg',
                             'BootloaderCorePkg.dsc')
    dsc = Chk.Dsc (dsc_path, workspace)
    stage1a = 'BootloaderCorePkg/Stage1A/Stage1A.inf'
    stage1b = 'BootloaderCorePkg/Stage1B/Stage1B.inf'
    assert dsc.map_for (stage1a, 'IA32', 'PEIM')['FspApiLib'] != \
        dsc.map_for (stage1b, 'IA32', 'PEIM')['FspApiLib']


@pytest.mark.parametrize ('section', [
    'LibraryClasses.X64',
    'LibraryClasses.common.DXE_DRIVER',
])
def test_qualified_mapping_does_not_leak (workspace, tmp_path, section):
    """An IA32 PEIM cannot consume an X64 or DXE_DRIVER-only mapping."""
    component = tmp_path / 'Component.inf'
    component.write_text (
        '[Defines]\n  MODULE_TYPE = PEIM\n'
        '[LibraryClasses]\n  TestLibraryClass\n', encoding='utf-8')
    instance = tmp_path / 'Instance.inf'
    instance.write_text ('[Defines]\n  LIBRARY_CLASS = TestLibraryClass\n',
                         encoding='utf-8')
    dsc = tmp_path / 'Qualified.dsc'
    dsc.write_text (
        '[Defines]\n  SUPPORTED_ARCHITECTURES = IA32\n'
        '[%s]\n  TestLibraryClass|%s\n'
        '[Components]\n  %s\n' % (section, instance, component),
        encoding='utf-8')

    errors, stats = Chk.Checker (workspace).check_dsc (str (dsc))
    assert stats['components'] == 1
    assert any ('TestLibraryClass' in err and 'not mapped' in err
                for err in errors), errors


def test_specific_mapping_wins_over_common (workspace, tmp_path):
    """An arch-specific mapping overrides a common module-specific one."""
    component = tmp_path / 'Component.inf'
    component.write_text (
        '[Defines]\n  MODULE_TYPE = PEIM\n'
        '[LibraryClasses]\n  TestLibraryClass\n', encoding='utf-8')
    correct = tmp_path / 'Correct.inf'
    correct.write_text ('[Defines]\n  LIBRARY_CLASS = TestLibraryClass\n',
                        encoding='utf-8')
    wrong = tmp_path / 'Wrong.inf'
    wrong.write_text (
        '[Defines]\n  LIBRARY_CLASS = TestLibraryClass\n'
        '[LibraryClasses]\n  WrongOnlyDependency\n', encoding='utf-8')
    dsc = tmp_path / 'Precedence.dsc'
    dsc.write_text (
        '[Defines]\n  SUPPORTED_ARCHITECTURES = IA32\n'
        '[LibraryClasses.common.PEIM]\n  TestLibraryClass|%s\n'
        '[LibraryClasses.IA32]\n  TestLibraryClass|%s\n'
        '[Components]\n  %s\n' % (wrong, correct, component),
        encoding='utf-8')

    errors, stats = Chk.Checker (workspace).check_dsc (str (dsc))
    assert errors == [], errors
    assert stats['resolved'] == 1


def test_standalone_checks_each_supported_architecture (workspace, tmp_path):
    """An IA32-only map cannot satisfy an X64 build of the same DSC."""
    component = tmp_path / 'Component.inf'
    component.write_text (
        '[Defines]\n  MODULE_TYPE = PEIM\n'
        '[LibraryClasses]\n  TestLibraryClass\n', encoding='utf-8')
    instance = tmp_path / 'Instance.inf'
    instance.write_text ('[Defines]\n  LIBRARY_CLASS = TestLibraryClass\n',
                         encoding='utf-8')
    dsc = tmp_path / 'BothArches.dsc'
    dsc.write_text (
        '[Defines]\n  SUPPORTED_ARCHITECTURES = IA32|X64\n'
        '[LibraryClasses.IA32]\n  TestLibraryClass|%s\n'
        '[Components]\n  %s\n' % (instance, component),
        encoding='utf-8')

    errors, stats = Chk.Checker (workspace).check_dsc (str (dsc))
    assert stats['components'] == 1
    assert stats['resolved'] == 1
    assert len (errors) == 1, errors
    assert '[X64]: library class [TestLibraryClass]' in errors[0], errors
    assert 'not mapped' in errors[0], errors


def test_inf_declares_its_own_library_class (workspace):
    inf = Chk.Inf (os.path.join (
        workspace, 'BootloaderCommonPkg', 'Library', 'LoaderPerformanceLib',
        'LoaderPerformanceLib.inf'), workspace)
    assert inf.library_class == 'LoaderPerformanceLib'
    assert 'CsmePerfIdToStrLib' in inf.required


def test_macro_component_dependencies_are_checked (workspace, tmp_path):
    """A macro-valued [Components] INF must not hide its library classes."""
    dsc = tmp_path / 'MacroComponent.dsc'
    component = 'BootloaderCommonPkg/Library/LoaderPerformanceLib/LoaderPerformanceLib.inf'
    dsc.write_text (
        '[Defines]\n'
        '  DEFINE COMPONENT_INF = %s\n'
        '[Components]\n'
        '  $(COMPONENT_INF)\n' % component, encoding='utf-8')

    parsed = Chk.Dsc (str (dsc), workspace)
    assert parsed.components == [component]
    errors, stats = Chk.Checker (workspace).check_dsc (str (dsc))
    assert stats['components'] == 1
    assert any ('CsmePerfIdToStrLib' in err for err in errors), errors


def test_undefined_active_component_macro_fails (workspace, tmp_path):
    """Only a disabled conditional may omit a board's undefined macro."""
    dsc = tmp_path / 'UndefinedMacro.dsc'
    dsc.write_text (
        '[Components]\n'
        '  $(UNDEFINED_INF_FILE)\n'
        '!if FALSE\n'
        '  $(DISABLED_INF_FILE)\n'
        '!endif\n', encoding='utf-8')
    errors, stats = Chk.Checker (workspace).check_dsc (str (dsc))
    assert stats['components'] == 0
    assert any ('UNDEFINED_INF_FILE' in err for err in errors), errors
    assert not any ('DISABLED_INF_FILE' in err for err in errors), errors


def test_generated_board_macro_components_are_included (workspace, tmp_path):
    """Exercise the same generated Platform.dsc path as --all-boards."""
    boards, load_errors = Chk.load_boards (workspace)
    assert not load_errors
    checker = Chk.Checker (workspace)
    for name in ('qemu', 'arlh'):
        platform = str (tmp_path / ('%s-Platform.dsc' % name))
        Chk.gen_platform_dsc (boards[name], platform)
        generated = Chk.MetaFile (platform, workspace)
        arch = 'X64' if name == 'arlh' else 'IA32'
        assert generated.defines['BUILD_ARCH'] == arch
        assert any (section == 'libraryclasses.%s' % arch.lower ()
                    for section, _ in generated.entries)
        dsc = Chk.Dsc (os.path.join (workspace, 'BootloaderCorePkg',
                                     'BootloaderCorePkg.dsc'),
                       workspace, {'Platform.dsc': platform})
        acpi = dsc.meta.defines['ACPI_TABLE_INF_FILE']
        assert acpi in dsc.components
        if name == 'arlh':
            microcode = dsc.meta.defines['MICROCODE_INF_FILE']
            assert microcode in dsc.components
        errors, stats = checker.check_dsc (
            'BootloaderCorePkg/BootloaderCorePkg.dsc',
            {'Platform.dsc': platform}, name)
        assert not errors, errors
        assert stats['components'] == len (dsc.components)


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


def test_unresolved_include_fails_standalone_check (workspace, tmp_path,
                                                    monkeypatch, capsys):
    """A missing include must fail both the checker and the CLI."""
    dsc = tmp_path / 'MissingInclude.dsc'
    dsc.write_text ('[Defines]\n!include NoSuchDirectory/Missing.dsc\n',
                    encoding='utf-8')
    errors, _ = Chk.Checker (workspace).check_dsc (str (dsc))
    assert any ('Missing.dsc' in err for err in errors), errors

    monkeypatch.setattr (Chk, 'STANDALONE_DSC_LIST', [str (dsc)])
    monkeypatch.setattr (sys, 'argv', ['CheckDscLibraryClasses.py',
                                      '--workspace', workspace])
    assert Chk.main () == 1
    output = capsys.readouterr ().out
    assert 'FAIL %s' % dsc in output
    assert 'SKIP %s' % dsc not in output


def test_unresolved_inf_include_fails_check (workspace, tmp_path):
    """An INF include can also hide required library classes."""
    inf = tmp_path / 'Component.inf'
    inf.write_text ('[Defines]\n!include MissingInfDefines.inc\n',
                    encoding='utf-8')
    dsc = tmp_path / 'Component.dsc'
    dsc.write_text ('[Components]\n  %s\n' % inf, encoding='utf-8')
    errors, _ = Chk.Checker (workspace).check_dsc (str (dsc))
    assert any ('MissingInfDefines.inc' in err for err in errors), errors


def test_all_boards_fails_on_board_load_error (workspace, monkeypatch, capsys):
    """A failed import is not a successful check of every board."""
    cfg = os.path.join ('Platform', 'BrokenBoardPkg', 'BoardConfig.py')
    monkeypatch.setattr (Chk, 'load_boards',
                         lambda workspace: ({}, {cfg: 'ImportError: broken'}))
    monkeypatch.setattr (sys, 'argv', ['CheckDscLibraryClasses.py',
                                      '--workspace', workspace, '--all-boards'])
    assert Chk.main () == 1
    output = capsys.readouterr ().out
    assert 'FAIL could not load %s' % cfg in output


def test_duplicate_board_name_is_a_load_error (tmp_path, monkeypatch):
    """Do not silently discard a second BoardConfig with the same name."""
    import BuildLoader

    for pkg in ('FirstBoardPkg', 'SecondBoardPkg'):
        cfg = tmp_path / 'Platform' / pkg / 'BoardConfig.py'
        cfg.parent.mkdir (parents=True)
        cfg.write_text ('# Test BoardConfig\n', encoding='utf-8')
    module = SimpleNamespace (
        Board=lambda: SimpleNamespace (BOARD_NAME='duplicate'))
    monkeypatch.setattr (BuildLoader, 'load_source',
                         lambda name, path: module)
    boards, errors = Chk.load_boards (str (tmp_path))
    assert list (boards) == ['duplicate']
    assert any ('duplicate' in err and 'FirstBoardPkg' in err
                for err in errors.values ()), errors
