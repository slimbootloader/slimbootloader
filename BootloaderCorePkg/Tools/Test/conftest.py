## @ conftest.py
#
# Shared pytest fixtures for the Slim Bootloader Python tool tests.
#
# Copyright (c) 2026, Intel Corporation. All rights reserved.<BR>
# SPDX-License-Identifier: BSD-2-Clause-Patent
#
##

import os
import sys

import pytest

TEST_DIR = os.path.dirname (os.path.abspath (__file__))
TOOLS_DIR = os.path.dirname (TEST_DIR)
WORKSPACE = os.path.normpath (os.path.join (TOOLS_DIR, '..', '..'))

# The tools are scripts rather than an installed package, so make them
# importable exactly as BuildLoader.py does.
if TOOLS_DIR not in sys.path:
    sys.path.insert (0, TOOLS_DIR)
if WORKSPACE not in sys.path:
    sys.path.insert (0, WORKSPACE)

os.environ.setdefault ('SBL_SOURCE', WORKSPACE)
os.environ.setdefault ('WORKSPACE', WORKSPACE)
os.environ.setdefault ('PLT_SOURCE', WORKSPACE)


@pytest.fixture (scope='session')
def workspace ():
    """Absolute path to the repository root."""
    return WORKSPACE


@pytest.fixture (scope='session')
def tools_dir ():
    """Absolute path to BootloaderCorePkg/Tools."""
    return TOOLS_DIR


# Maps each test module to the library/tool area it exercises, so a deployed
# log names what was actually tested rather than just a bare file name. Kept
# in sync with the "What is covered" table in ReadMe.md.
_TEST_AREA_BY_MODULE = {
    'test_dsc_library_classes.py': 'CheckDscLibraryClasses.py (DSC library-class resolution)',
    'test_common_utility.py':      'CommonUtility.py (bit/byte/alignment helpers)',
    'test_container_format.py':    'GenContainer.py (container structures and auth sizing)',
    'test_gen_cfg_data.py':        'GenCfgData.py (config data YAML/.dlt handling)',
    'test_board_config.py':        'Platform/*/BoardConfig*.py (per-board config load)',
}


def pytest_terminal_summary (terminalreporter, exitstatus, config):
    """Print a concise per-module pass/fail summary.

    ``-ra`` already reports individual failures, but a healthy run gives no
    indication of which library or test area was actually exercised. This
    adds one line per test module -- naming the area under test and its
    result -- without turning on noisy per-test verbosity or touching the
    DeprecationWarning reporting that ``-ra`` already provides.
    """
    counts = {}
    for outcome in ('passed', 'failed', 'error'):
        for report in terminalreporter.stats.get (outcome, []):
            # A single test yields a 'setup'/'call'/'teardown' report each;
            # only 'call' represents the test body itself for a pass, but a
            # setup/teardown failure must still be counted as a failure.
            if outcome == 'passed' and report.when != 'call':
                continue
            module = os.path.basename (report.nodeid.split ('::') [0])
            tally = counts.setdefault (module, {'passed': 0, 'failed': 0, 'error': 0})
            tally [outcome] += 1

    if not counts:
        return

    terminalreporter.write_sep ('=', 'library/test area summary')
    for module in sorted (counts):
        area = _TEST_AREA_BY_MODULE.get (module, module)
        tally = counts [module]
        if tally ['failed'] or tally ['error']:
            status = 'FAILED (%d passed, %d failed, %d errors)' % (
                tally ['passed'], tally ['failed'], tally ['error'])
        else:
            status = 'passed (%d tests)' % tally ['passed']
        terminalreporter.write_line ('%-28s %-55s %s' % (module, area, status))