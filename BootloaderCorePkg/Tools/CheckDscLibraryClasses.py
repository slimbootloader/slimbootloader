## @ CheckDscLibraryClasses.py
#
# Static check: every library class reachable from a DSC's [Components]
# modules must have an instance mapped for that module.
#
# This catches the class of defect where a module or a library gains a new
# library class dependency and only some of the DSC files that build it get
# updated. The EDK II build reports this as:
#
#   error 4000: Instance of library class [Xxx] is not found
#
# but only for the DSC/board combinations that CI actually compiles. This
# script performs the same reachability analysis without invoking a compiler,
# so every board can be checked in seconds.
#
# Usage
# -----
#   python BootloaderCorePkg/Tools/CheckDscLibraryClasses.py
#   python BootloaderCorePkg/Tools/CheckDscLibraryClasses.py --board qemu
#   python BootloaderCorePkg/Tools/CheckDscLibraryClasses.py --all-boards
#
# BootloaderCorePkg.dsc does '!include Platform.dsc', which BuildLoader.py
# generates from the board's BoardConfig.py. When a board is selected that
# file is generated to a temporary location and injected, so the core DSC can
# be checked per board. Without a board it is skipped rather than failed.
#
# Conditional handling
# --------------------
# !if / !ifdef / !elseif / !else are evaluated, not unioned, using the DEFINE
# macros in scope. An undefined macro expands to an empty string, which is how
# an unset feature knob behaves. A condition that cannot be parsed is treated
# as true, so its body is still checked rather than silently skipped.
#
# Copyright (c) 2026, Intel Corporation. All rights reserved.<BR>
# SPDX-License-Identifier: BSD-2-Clause-Patent
#
##

import os
import re
import sys
import glob
import argparse
import tempfile

#
# DSC files that resolve without any board context.
#
STANDALONE_DSC_LIST = [
    'PayloadPkg/PayloadPkg.dsc',
]

#
# DSC files that require a generated Platform.dsc (i.e. a board).
#
BOARD_DSC_LIST = [
    'BootloaderCorePkg/BootloaderCorePkg.dsc',
]

SECTION_RE = re.compile (r'^\s*\[([^\]]+)\]\s*$')
DIRECTIVE_RE = re.compile (r'^\s*!(\w+)\s*(.*)$')
MACRO_RE = re.compile (r'\$\(([^)]+)\)')
DEFINE_RE = re.compile (r'^\s*DEFINE\s+([A-Za-z_]\w*)\s*=\s*(.*)$')
DEFINED_RE = re.compile (
    r'defined\s*\(\s*(?:\$\(\s*)?([A-Za-z_]\w*)\s*\)?\s*\)', re.I)
WORD_RE = re.compile (r'(?<![\w.$])[A-Za-z_][\w./\\-]*')
PY_KEYWORDS = {'and', 'or', 'not', 'True', 'False', 'None'}


def strip_comment (line):
    return line.split ('#')[0].rstrip ()


def eval_condition (expr, defines):
    """Evaluate a DSC !if / !elseif expression.

    Undefined macros expand to an empty string, which is how an unset feature
    knob behaves in practice. Anything that cannot be evaluated returns True,
    so an unparsable condition includes its body rather than silently dropping
    it (dropping could hide a genuine missing mapping).
    """
    text = expr.strip ()
    if not text:
        return True

    # defined(X) must be answered before macro substitution.
    def defined_repl (match):
        return 'True' if match.group (1) in defines else 'False'

    text = DEFINED_RE.sub (defined_repl, text)

    def macro_repl (match):
        # An undefined macro becomes an empty string literal so that
        # '$(UNSET) == 1' evaluates to False instead of failing to parse.
        return defines.get (match.group (1), "''")

    for _ in range (8):
        new = MACRO_RE.sub (macro_repl, text)
        if new == text:
            break
        text = new

    text = text.replace ('&&', ' and ').replace ('||', ' or ')
    text = re.sub (r'!\s*=', '__NE__', text)
    text = re.sub (r'!', ' not ', text)
    text = text.replace ('__NE__', '!=')

    def word_repl (match):
        word = match.group (0)
        if word in PY_KEYWORDS:
            return word
        upper = word.upper ()
        if upper == 'TRUE':
            return 'True'
        if upper == 'FALSE':
            return 'False'
        if upper in ('IN', 'AND', 'OR', 'NOT'):
            return word.lower ()
        return repr (word)

    text = WORD_RE.sub (word_repl, text)

    if not text.strip ():
        # Every operand vanished, e.g. '$(UNDEFINED)'.
        return False

    try:
        return bool (eval (text, {'__builtins__': {}}, {}))   # noqa: S307
    except Exception:   # noqa: BLE001 - unparsable condition: include body
        return True


class MetaFile:
    """Minimal DSC/INF reader.

    Yields (section_name_lower, line) pairs with comments removed, !include
    expanded and conditional directives flattened (every branch retained).
    Also collects DEFINE macros.
    """

    def __init__ (self, path, workspace, extra_includes=None):
        self.path = path
        self.workspace = workspace
        self.extra_includes = extra_includes or {}
        self.entries = []
        self.defines = {}
        self.missing_includes = []
        self._parse (path, set ())

    def _resolve_include (self, raw, cur_dir):
        inc = raw.strip ().strip ('"')
        key = inc.replace ('\\', '/')
        if key in self.extra_includes:
            return self.extra_includes[key]
        base_name = os.path.basename (key)
        if base_name in self.extra_includes:
            return self.extra_includes[base_name]
        for base in (cur_dir, self.workspace):
            cand = os.path.normpath (os.path.join (base, inc))
            if os.path.isfile (cand):
                return cand
        return None

    def _parse (self, path, seen):
        real = os.path.normpath (os.path.abspath (path))
        if real in seen:
            return
        seen = seen | {real}
        section = ''
        cur_dir = os.path.dirname (real)
        with open (real, 'r', encoding='utf-8', errors='replace') as fp:
            lines = fp.read ().splitlines ()

        # Each entry: [body_active, any_branch_taken, enclosing_active]
        cond_stack = []

        def active ():
            return all (entry[0] for entry in cond_stack)

        for raw in lines:
            line = strip_comment (raw)
            if not line.strip ():
                continue

            match = DIRECTIVE_RE.match (line)
            if match:
                name = match.group (1).lower ()
                body = match.group (2).strip ()
                if name in ('if', 'ifdef', 'ifndef'):
                    outer = active ()
                    if name == 'ifdef':
                        cond = body.strip ('$()') in self.defines
                    elif name == 'ifndef':
                        cond = body.strip ('$()') not in self.defines
                    else:
                        cond = eval_condition (body, self.defines)
                    cond_stack.append ([outer and cond, cond, outer])
                elif name == 'elseif':
                    if cond_stack:
                        entry = cond_stack[-1]
                        cond = eval_condition (body, self.defines)
                        entry[0] = entry[2] and not entry[1] and cond
                        entry[1] = entry[1] or cond
                elif name == 'else':
                    if cond_stack:
                        entry = cond_stack[-1]
                        entry[0] = entry[2] and not entry[1]
                        entry[1] = True
                elif name == 'endif':
                    if cond_stack:
                        cond_stack.pop ()
                elif name == 'include' and active ():
                    inc_path = self._resolve_include (body, cur_dir)
                    if inc_path:
                        self._parse (inc_path, seen)
                    else:
                        self.missing_includes.append (body)
                continue

            if not active ():
                continue

            match = SECTION_RE.match (line)
            if match:
                section = match.group (1).strip ().lower ()
                continue

            text = line.strip ()
            match = DEFINE_RE.match (text)
            if match:
                self.defines.setdefault (match.group (1),
                                         match.group (2).strip ())
            self.entries.append ((section, text))

    def section_lines (self, prefix):
        """Lines from every section whose name is prefix or prefix + '.x'."""
        out = []
        for section, line in self.entries:
            for name in section.split (','):
                name = name.strip ()
                if name == prefix or name.startswith (prefix + '.'):
                    out.append (line)
                    break
        return out

    def expand (self, text):
        """Expand $(MACRO) using collected DEFINEs. Bounded to avoid cycles."""
        for _ in range (8):
            if '$(' not in text:
                break

            def repl (match):
                return self.defines.get (match.group (1), match.group (0))

            new = MACRO_RE.sub (repl, text)
            if new == text:
                break
            text = new
        return text


class Inf:
    def __init__ (self, path, workspace):
        self.path = path
        meta = MetaFile (path, workspace)
        self.library_class = None
        self.module_type = None
        for line in meta.section_lines ('defines'):
            if '=' not in line:
                continue
            key, _, val = line.partition ('=')
            key = key.strip ().upper ()
            if key == 'LIBRARY_CLASS':
                self.library_class = val.split ('|')[0].strip ()
            elif key == 'MODULE_TYPE':
                self.module_type = val.strip ()

        self.required = []
        for line in meta.section_lines ('libraryclasses'):
            name = line.split ('|')[0].strip ()
            if not name or name.upper () == 'NULL':
                continue
            if name not in self.required:
                self.required.append (name)


def parse_map_line (line):
    """'Foo | Path/Foo.inf' -> ('Foo', 'Path/Foo.inf'), else None."""
    if '|' not in line:
        return None
    name, _, inst = line.partition ('|')
    name = name.strip ()
    inst = inst.strip ()
    if not name or not inst:
        return None
    return (name, inst)


class Dsc:
    def __init__ (self, path, workspace, extra_includes=None):
        self.path = path
        self.workspace = workspace
        self.meta = MetaFile (path, workspace, extra_includes)
        self.missing_includes = self.meta.missing_includes

        self.lib_map = {}
        for line in self.meta.section_lines ('libraryclasses'):
            pair = parse_map_line (line)
            if not pair or pair[0].upper () == 'NULL':
                continue
            self.lib_map.setdefault (pair[0], [])
            if pair[1] not in self.lib_map[pair[0]]:
                self.lib_map[pair[0]].append (pair[1])

        # [Components] entries may carry a '{ <LibraryClasses> ... }' override
        # block that applies to that module only.
        self.components = []
        self.overrides = {}
        current = None
        sub = None
        in_block = False
        for line in self.meta.section_lines ('components'):
            text = line.strip ()
            if not in_block:
                token = text.split ('{')[0].strip ()
                if token.lower ().endswith ('.inf'):
                    current = token
                    if current not in self.components:
                        self.components.append (current)
                        self.overrides.setdefault (current, {})
                    if text.endswith ('{'):
                        in_block = True
                        sub = None
                continue
            # Inside an override block.
            if text.startswith ('}'):
                in_block = False
                current = None
                sub = None
                continue
            if text.startswith ('<') and text.endswith ('>'):
                sub = text[1:-1].strip ().lower ()
                continue
            if sub == 'libraryclasses' and current:
                pair = parse_map_line (text)
                if pair and pair[0].upper () != 'NULL':
                    self.overrides[current][pair[0]] = pair[1]

    def map_for (self, component):
        """Effective class -> [instances] for one component."""
        merged = dict (self.lib_map)
        for name, inst in self.overrides.get (component, {}).items ():
            merged[name] = [inst]
        return merged


class Checker:
    def __init__ (self, workspace):
        self.workspace = workspace
        self._inf_cache = {}

    def load_inf (self, rel_path):
        key = rel_path.replace ('\\', '/')
        if key in self._inf_cache:
            return self._inf_cache[key]
        full = os.path.normpath (os.path.join (self.workspace, key))
        inf = Inf (full, self.workspace) if os.path.isfile (full) else None
        self._inf_cache[key] = inf
        return inf

    def check_dsc (self, dsc_rel, extra_includes=None, label=None):
        label = label or dsc_rel
        full = os.path.normpath (os.path.join (self.workspace, dsc_rel))
        if not os.path.isfile (full):
            return (['%s: file not found' % label], {})

        dsc = Dsc (full, self.workspace, extra_includes)
        errors = []
        warnings = []
        resolved = set ()

        for comp in dsc.components:
            comp_inf = self.load_inf (comp)
            if comp_inf is None:
                errors.append ('%s: [Components] entry not found: %s'
                               % (label, comp))
                continue
            lib_map = dsc.map_for (comp)
            pending = [(cls, comp) for cls in comp_inf.required]
            visited = set ()
            while pending:
                cls, requester = pending.pop ()
                if cls in visited:
                    continue
                visited.add (cls)
                inst_list = lib_map.get (cls)
                if not inst_list:
                    errors.append (
                        '%s: library class [%s] required by %s is not mapped'
                        % (label, cls, requester))
                    continue
                resolved.add (cls)
                for inst in inst_list:
                    inst = dsc.meta.expand (inst)
                    if '$(' in inst:
                        # Board supplies this path at build time.
                        msg = ('%s: instance for [%s] is macro-valued, not '
                               'followed: %s' % (label, cls, inst))
                        if msg not in warnings:
                            warnings.append (msg)
                        continue
                    inst_inf = self.load_inf (inst)
                    if inst_inf is None:
                        msg = '%s: instance file not found: %s' % (label, inst)
                        if msg not in errors:
                            errors.append (msg)
                        continue
                    for dep in inst_inf.required:
                        if dep not in visited:
                            pending.append ((dep, inst))

        stats = {
            'components': len (dsc.components),
            'mapped': len (dsc.lib_map),
            'resolved': len (resolved),
            'warnings': warnings,
            'missing_includes': dsc.missing_includes,
        }
        return (errors, stats)


def find_workspace ():
    env = os.environ.get ('SBL_SOURCE')
    if env and os.path.isdir (env):
        return os.path.normpath (env)
    return os.path.normpath (
        os.path.join (os.path.dirname (os.path.abspath (__file__)),
                      '..', '..'))


def _board_module_name (cfg_path):
    """Reproduce BuildLoader.py's board module naming.

    'Platform/QemuBoardPkg/BoardConfig.py' -> 'QemuBoardConfig'. Override
    board files import their base by that name, so it must match exactly.
    """
    pkg = os.path.basename (os.path.dirname (cfg_path))
    return pkg[:-8] + os.path.basename (cfg_path)[:-3]


def load_boards (workspace):
    """Load every BoardConfig*.py the way BuildLoader.py does.

    Returns {board_name: module}. Files that fail to load are skipped; a
    board that cannot be loaded is reported later rather than silently
    passing.
    """
    os.environ.setdefault ('SBL_SOURCE', workspace)
    os.environ.setdefault ('WORKSPACE', workspace)
    os.environ.setdefault ('PLT_SOURCE', workspace)
    for extra in (workspace,
                  os.path.join (workspace, 'BootloaderCorePkg', 'Tools')):
        if extra not in sys.path:
            sys.path.insert (0, extra)

    import BuildLoader

    boards = {}
    errors = {}
    pattern = os.path.join (workspace, 'Platform', '*', 'BoardConfig*.py')
    # Sorted so a base BoardConfig.py is registered before any override that
    # imports it.
    for cfg in sorted (glob.glob (pattern)):
        name = _board_module_name (cfg)
        try:
            module = BuildLoader.load_source (name, cfg)
            board_name = module.Board ().BOARD_NAME
        except Exception as exc:   # noqa: BLE001 - recorded, not fatal
            errors[os.path.relpath (cfg, workspace)] = \
                '%s: %s' % (type (exc).__name__, exc)
            continue
        if board_name:
            boards.setdefault (board_name, module)
    return boards, errors


def gen_platform_dsc (module, out_path):
    """Generate Platform.dsc for a board by reusing BuildLoader's own code."""
    import BuildLoader
    BuildLoader.Build (module.Board ()).create_dsc_inc_file (out_path)


def report (label, errors, stats, show_warnings):
    """Print one result. Returns the number of errors."""
    if errors:
        print ('FAIL %s' % label)
        for err in errors:
            print ('  %s' % err)
    else:
        print ('PASS %s  (%d module(s), %d class(es) resolved)'
               % (label, stats.get ('components', 0), stats.get ('resolved', 0)))
    if show_warnings:
        for warn in stats.get ('warnings', []):
            print ('  note: %s' % warn)
    return len (errors)


def main ():
    parser = argparse.ArgumentParser (
        description='Verify that every library class reachable from a DSC is '
                    'mapped in that DSC.')
    parser.add_argument ('dsc', nargs='*',
                         help='extra DSC files to check')
    parser.add_argument ('-b', '--board', action='append', default=[],
                         help='board name; enables BootloaderCorePkg.dsc '
                              'checking for that board (repeatable)')
    parser.add_argument ('-A', '--all-boards', action='store_true',
                         help='check BootloaderCorePkg.dsc for every board')
    parser.add_argument ('-w', '--workspace', default=None,
                         help='workspace root (default: auto-detected)')
    parser.add_argument ('-W', '--show-warnings', action='store_true',
                         help='print non-fatal notes')
    args = parser.parse_args ()

    workspace = os.path.normpath (args.workspace) if args.workspace \
        else find_workspace ()
    checker = Checker (workspace)

    failures = 0
    checked = 0

    targets = list (STANDALONE_DSC_LIST)
    targets += [d.replace ('\\', '/') for d in args.dsc]
    for dsc_rel in targets:
        errors, stats = checker.check_dsc (dsc_rel)
        checked += 1
        if stats.get ('missing_includes'):
            print ('SKIP %s  (unresolved !include: %s)'
                   % (dsc_rel, ', '.join (stats['missing_includes'])))
            continue
        failures += report (dsc_rel, errors, stats, args.show_warnings)

    boards, board_errors = load_boards (workspace)
    board_names = sorted (boards) if args.all_boards else list (args.board)

    if args.all_boards and board_errors:
        for cfg, err in sorted (board_errors.items ()):
            print ('WARN could not load %s (%s)' % (cfg, err))

    if not board_names:
        print ('SKIP BootloaderCorePkg/BootloaderCorePkg.dsc  '
               '(needs --board <name> or --all-boards)')

    for name in board_names:
        if name not in boards:
            print ('FAIL board %s: no BoardConfig.py found' % name)
            failures += 1
            continue
        tmp_fd, out_path = tempfile.mkstemp (prefix='sbl-%s-' % name,
                                             suffix='-Platform.dsc')
        os.close (tmp_fd)
        try:
            try:
                gen_platform_dsc (boards[name], out_path)
            except Exception as exc:   # noqa: BLE001 - report and continue
                print ('FAIL board %s: could not generate Platform.dsc '
                       '(%s: %s)' % (name, type (exc).__name__, exc))
                failures += 1
                continue
            for dsc_rel in BOARD_DSC_LIST:
                label = '%s [%s]' % (dsc_rel, name)
                errors, stats = checker.check_dsc (
                    dsc_rel, {'Platform.dsc': out_path}, label)
                checked += 1
                failures += report (label, errors, stats, args.show_warnings)
        finally:
            if os.path.exists (out_path):
                os.remove (out_path)

    print ('')
    if failures:
        print ('%d unresolved library class error(s) across %d check(s)'
               % (failures, checked))
        return 1
    print ('All DSC library classes resolve (%d check(s)).' % checked)
    return 0


if __name__ == '__main__':
    sys.exit (main ())
