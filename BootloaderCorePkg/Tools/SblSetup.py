#!/usr/bin/env python
## @ SblSetup.py
# Setup script to change SBL CFGDATA at runtime using MicroPython.
#
# Copyright (c) 2020 - 2021, Intel Corporation. All rights reserved.<BR>
# SPDX-License-Identifier: BSD-2-Clause-Patent
#

#
# This script can be execute on either host or target
# When running host, it is requied to use putty to connect to named pipe.
#   python SblSetup.py  cfg_json_file  cfg_bin_file
# When running on target, it needs to enable the MicroPython payload module.
#
try:
    from   ucollections import OrderedDict
    is_micro_py = True
except:
    import sys
    import time
    from   collections import OrderedDict
    import copy
    import msvcrt
    import marshal
    import win32pipe, win32file, win32con
    is_micro_py = False

class TERM:
    # bit0: GFX   bit1: TXT
    SCREEN_MODE    = 3
    SCREEN_SIZE    = (100,  30)
    COLOR_WINDOW   = (7,  1)
    COLOR_TEXT     = (15, 8)
    COLOR_DISABLED = (7, 8)
    COLOR_FOCUS    = (14, 3)
    COLOR_EDITING  = (11, 2)
    COLOR_POPUP    = (0,  3)

def is_mp():
    global  is_micro_py
    return  is_micro_py

def value_to_bytes (value, length):
    return value.to_bytes(length, 'little')

def bytes_to_value (bytes):
    return int.from_bytes (bytes, 'little')

def value_to_bytearray (value, length):
    return bytearray(value_to_bytes(value, length))

def string_decode (str):
    # MPY might have unicode disabled
    return ''.join([chr(i) for i in str])

def string_encode (str):
    # MPY might have unicode disabled
    return bytes([ord(i) for i in str])

def get_bits_from_bytes (bytes, start, length):
    if length == 0:
        return 0
    byte_start = (start)  // 8
    byte_end   = (start + length - 1) // 8
    bit_start  = start & 7
    mask = (1 << length) - 1
    val = bytes_to_value (bytes[byte_start:byte_end + 1])
    val = (val >> bit_start) & mask
    return val

def set_bits_to_bytes (bytes, start, length, bvalue):
    if length == 0:
        return
    byte_start = (start)  // 8
    byte_end   = (start + length - 1) // 8
    bit_start  = start & 7
    mask = (1 << length) - 1
    val  = bytes_to_value (bytes[byte_start:byte_end + 1])
    val &= ~(mask << bit_start)
    val |= ((bvalue & mask) << bit_start)
    bytes[byte_start:byte_end+1] = value_to_bytearray (val, byte_end + 1 - byte_start)

def check_quote (text):
    if len(text) > 0:
        if (text[0] == "'" and text[-1] == "'") or (text[0] == '"' and text[-1] == '"'):
            return True
    return False

def strip_quote (text):
    new_text = text.strip()
    if check_quote (new_text):
        return new_text[1:-1]
    return text

def strip_delimiter (text, delim):
    new_text = text.strip()
    if new_text:
        if new_text[0] == delim[0] and new_text[-1] == delim[-1]:
            return new_text[1:-1]
    return text

def bytes_to_bracket_str (bytes):
    return '{ %s }' % (', '.join('0x%02x' % i for i in bytes))

def array_str_to_value (val_str):
    val_str = val_str.strip()
    val_str = strip_delimiter (val_str, '{}')
    val_str = strip_quote (val_str)
    value = 0
    for each in val_str.split(',')[::-1]:
        each = each.strip()
        value = (value << 8) | int(each, 0)
    return value

def eval_expr (expr):
    try:
        result = int (eval (expr))
    except:
        print("ERROR: Express parsing for:")
        print("       %s" % expr)
        raise SystemExit ("Exception: Expression parsing error!")
    return result

def wrap_line (input_string, lim):
    lines = []
    for s in input_string.split("\n"):
        if s == '':
            lines.append('')
        w = 0
        l = []
        for d in s.split():
            if w + len(d) + 1 <= lim:
                l.append(d)
                w += len(d) + 1
            else:
                lines.append (" ".join(l))
                l = [d]
                w = len(d)
        if len(l):
            lines.append(" ".join(l))
    return lines

def get_char ():
    if is_mp():
       return pyb.getch()
    else:
       ch = g_pipe.recv()
       return ord(ch)

def get_ch_host ():
    if not msvcrt.kbhit():
        return b''

    ch = msvcrt.getch()
    if ch == b'\xe0':
        ch = msvcrt.getch()
        if ch == b'P':
             ch = b'down'
        elif ch == b'H':
             ch = b'up'
        elif ch == b'K':
             ch = b'left'
        elif ch == b'M':
             ch = b'right'
        elif ch == b'S':
             ch = b'del'
        elif ch == b'G':
             ch = b'home'
        elif ch == b'O':
             ch = b'end'
        elif ch == b'I':
             ch = b'pgup'
        elif ch == b'Q':
             ch = b'pgdw'
        elif ch == b'\x86':
             ch = b'ctr+pgup'
        elif ch == b'v':
             ch = b'ctr+pgdw'
        else:
             ch = b'unknown'
    return ch

def get_ch_target ():
    ch = chr(get_char())

    if ch == '\x00':
        return b''

    if ch == '\x1b':
        ch = chr(get_char())
        if ch in '\x00\x1b':
            return b'\x1b'
        elif ch == '[':
            ch = chr(get_char())
            if ch == 'A':
                return b'up'
            elif ch == 'B':
                return b'down'
            elif ch == 'C':
                return b'right'
            elif ch == 'D':
                return b'left'
            elif ch == '2':
                return b'ins'
            elif ch == '3':
                return b'del'
            elif ch == '1':
                return b'home'
            elif ch == '4':
                return b'end'
            elif ch == '5':
                return b'pgup'
            elif ch == '6':
                return b'pgdw'
            return b''
        elif ch in 'kh/?':
            if ch in 'k':
                return b'end'
            elif ch in 'h':
                return b'home'
            elif ch in '/':
                return b'pgdw'
            else:
                return b'pgup'
            return b''
    return bytes([ord(ch)])

def get_ch ():
    if isinstance(g_pipe, NamedPipeClient):
        return get_ch_host ()
    else:
        return get_ch_target ()

class NamedPipeServer:
    def __init__(self):
        self.hpipe = None
        pipe_name  = "\\\\.\\pipe\\TermPipe"
        open_mode  = win32con.GENERIC_READ | win32con.GENERIC_WRITE
        try:
            self.hpipe = win32pipe.CreateNamedPipe (
                                          pipe_name,
                                          win32pipe.PIPE_ACCESS_DUPLEX,
                                          win32pipe.PIPE_TYPE_MESSAGE,
                                          1, 65536, 65536,
                                          0,  # win32con.FILE_FLAG_OVERLAPPED,
                                          None)
        except:
            raise SystemExit ("ERROR: Could not create named pipe '%s' !" % pipe_name)
        win32pipe.ConnectNamedPipe(self.hpipe, None)

    def send(self, data, col, mode):
        win32file.WriteFile(self.hpipe, data)

    def recv(self):
        (ignore, available, ignore) = win32pipe.PeekNamedPipe(self.hpipe, 0)
        if available == 0:
            return b'\x00'
        else:
            return win32file.ReadFile(self.hpipe, 1, None)[1]

    def close(self):
        if self.hpipe:
            win32file.CloseHandle(self.hpipe)
            self.hpipe = None

    def __del__(self):
        self.close()


class NamedPipeClient:
    def __init__(self):
        self.hpipe = None
        pipe_name  = "\\\\.\\pipe\\TermPipe"
        open_mode  = win32con.GENERIC_READ | win32con.GENERIC_WRITE
        try:
            self.hpipe = win32file.CreateFile(pipe_name,
                                          open_mode,
                                          0,
                                          None,
                                          win32con.OPEN_EXISTING,
                                          0,
                                          None)
        except:
            raise SystemExit ("ERROR: Could not open named pipe '%s' !" % pipe_name)

        if self.hpipe:
            win32pipe.SetNamedPipeHandleState(
                self.hpipe, win32pipe.PIPE_READMODE_MESSAGE, None, None)

    def send(self, data, col, mode):
        win32pipe.TransactNamedPipe(self.hpipe, data, 2, None)

    def recv(self):
        return b'\x00'

    def close(self):
        if self.hpipe:
            win32file.CloseHandle(self.hpipe)
            self.hpipe = None

    def __del__(self):
        self.close()


class NamedPipeVirtual:
    def __init__(self):
        if (TERM.SCREEN_MODE & 1):
            pyb.console_init (TERM.SCREEN_SIZE[0], TERM.SCREEN_SIZE[1])

    def send(self, data, col, mode):
        if (mode & 1):
            pyb.fb_blt (data, TERM.SCREEN_SIZE[0])
        if (mode & 2):
            pyb.uart_write (data)


class Rect:
    def __init__(self, *args, **kwargs):
        if len(args) == 4:
            self.x = args[0]
            self.y = args[1]
            self.w = args[2]
            self.h = args[3]
        elif len(args) == 1 and type(args[0]) is Rect:
            self.x = args[0].x
            self.y = args[0].y
            self.w = args[0].w
            self.h = args[0].h
        else:
            raise Exception ('Invalid constructor arguments for Rect !')

    def __str__ (self):
        return 'RECT(x=%d, y=%d, w=%d, h=%d)' % (self.x, self.y, self.w, self.h)

    def adjust (self, x=0, y=0, w=0, h=0):
        self.x += x
        self.y += y
        self.w += w
        self.h += h

    def inflate (self, w=0, h=None):
        if h is None:
            h = w
        self.x -= w
        self.y -= h
        self.w += w * 2
        self.h += h * 2


class Window:
    def __init__(self, parent, rc, id):
        self.parent     = parent
        self.vrc        = Rect(rc)
        self.wrc        = Rect(rc)
        self.id         = id
        self.top        = 0
        self.screen     = bytearray (b'\x20\x07' * self.wrc.h * self.wrc.w)
        self.caret_x    = 0
        self.caret_y    = 0
        self.fore_color = 0x0f
        self.back_color = 0x00
        self.sel_back_color = 0x07
        self.sel_fore_color = 0x00
        self.child      = []
        self.freeze     = False
        self.color_map  = [  0,  4,  2,  6,  4,  5,  3,  7,
                            60, 61, 62, 66, 64, 65, 63, 67 ]
        if (TERM.SCREEN_MODE & 2):
            self.screen_old = bytearray (self.screen)

    def get_id (self):
        return self.id

    def add_child  (self, child):
        self.child.append (child)
        if self.vrc.h < child.rc.y + child.rc.h:
            self.grow_view (child.rc.y + child.rc.h - self.vrc.h)

    def get_child (self):
        return self.child

    def remove_child (self, child = None):
        self.vrc.h = self.wrc.h
        self.child.clear()
        self.clear_screen ()
        self.set_top (0)
        self.refresh ()

    def clear_screen (self, fore_color=None, back_color=None):
        ch = bytearray (b'\x20\x00')
        fore_color = self.fore_color if fore_color is None else fore_color
        back_color = self.back_color if back_color is None else back_color
        ch[1] = (back_color << 4) + fore_color
        self.screen = bytearray (bytes(ch) * (self.vrc.h * self.vrc.w))
        if (TERM.SCREEN_MODE & 2):
            if self.parent is None:
                g_pipe.send(b'\x1b[?25h\x1b[0;%d;%dm\x1b[2J' % (30+self.color_map[fore_color], 40+self.color_map[back_color]), 0, 2)
                self.screen_old = bytearray (self.screen)

    def get_buffer (self):
        return self.screen

    def grow_view (self, height):
        self.vrc.h += height
        val    = bytearray(b'\x20\x0F')
        val[1] = (self.back_color << 4) + self.fore_color
        self.screen.extend (bytes(val) * height * self.vrc.w)

    def print_buffer (self):
        width = self.vrc.w * 2
        buf = self.get_buffer()
        off = 0
        for i in range (self.vrc.h):
            print (''.join([chr(i) for i in buf[off:off+width:2]]))
            off += width

    def blt_from (self, rc, buf):
        if len(buf) != rc.w * rc.h * 2:
            raise Exception ('Invalid buffer for (%d x %d = %d), but got %d !' % (rc.w, rc.h, rc.w * rc.h, len(buf)//2))
        offset1 = self.get_offset (rc.x, rc.y)
        offset2 = 0
        width1  = self.vrc.w * 2
        width2  = rc.w * 2
        for y in range(rc.h):
            self.screen[offset1:offset1 + width2] = buf[offset2:offset2 + width2]
            offset1 += width1
            offset2 += width2


    def set_top (self, top):
        if self.top != top:
            self.top  = top
            return True
        else:
            return False

    def get_top (self):
        return self.top

    def set_pos (self, x, y):
        if x >=0 and x < self.vrc.w:
            self.caret_x = x
        if y >=0 and y < self.vrc.h:
            self.caret_y = y

    def get_pos (self):
        return (self.caret_x, self.caret_y)

    def move_next_pos (self):
        x,y = self.get_pos ()
        x += 1
        if x >= self.vrc.w:
            x  = 0
            y += 1
        if y >= self.vrc.h:
            y = self.caret_y - 1
        self.set_pos (x, y)

    def set_char(self, ch, fore_color=None, back_color=None):
        off = self.get_offset()
        if type(ch) is str:
            if len(ch) == 0:
                ch = self.screen[off+0]
            else:
                ch = ord(ch[0])
        self.screen[off+0] = ch
        if fore_color is None:
            fore_color = self.fore_color
        elif fore_color == 0xff:
            fore_color = self.screen[off+1] & 0x0f
        else:
            fore_color = fore_color & 0x0f

        if back_color is None:
            back_color = self.back_color
        elif fore_color == 0xff:
            back_color = (self.screen[off+1] & 0xf0) >> 4
        else:
            back_color = back_color & 0x0f
        self.screen[off+1] = (back_color << 4) + fore_color

    def get_offset (self, x = None, y = None):
        if x is None:
            x = self.caret_x
        if y is None:
            y = self.caret_y
        return (y * self.vrc.w + x) * 2

    def set_color (self, fore_color = None, back_color = None):
        old = self.get_color()
        if fore_color is not None:
            self.fore_color = fore_color & 0x0f
        if back_color is not None:
            self.back_color = back_color & 0x0f
        return old

    def get_color (self):
        return (self.fore_color, self.back_color)

    def restore_color (self, color):
        self.fore_color, self.back_color = color

    def set_sel_color (self, fore_color = None, back_color = None):
        if fore_color is not None:
            self.sel_fore_color = fore_color & 0x0f
        if back_color is not None:
            self.sel_back_color = back_color & 0x0f
        return self.get_sel_color()

    def get_sel_color (self):
        return (self.sel_fore_color, self.sel_fore_color)

    def restore_sel_color (self, color):
        self.sel_fore_color, sel_back_color = color

    def get_rect_buffer (self, rc):
        ret = bytearray ()
        for y in range(rc.h):
            offset = self.get_offset(rc.x, rc.y + y)
            ret.extend (self.screen[offset:offset + rc.w * 2])
        return ret

    def set_rect_buffer (self, rc, data):
        idx = 0
        for y in range(rc.h):
            offset = self.get_offset(rc.x, rc.y + y)
            self.screen[offset:offset + rc.w * 2] = data[idx:idx + rc.w * 2]
            idx += rc.w * 2

    def write (self, line):
        for each in line:
            self.set_char (each)
            self.move_next_pos ()

    def set_freeze (self, freeze):
        self.freeze = freeze
        if not freeze:
            self.refresh ()

    def get_freeze (self, freeze):
        return self.freeze

    def build_ansi_seq (self):
        x  = 1
        y  = 1
        bk = 0
        fr = 7
        line_data = [b'\x1B[0;0H\x1B[37;40m']
        screen    = self.get_buffer()
        for idx in range(0, len(screen), 2):
             if self.screen_old[idx:idx+2] != screen[idx:idx+2]:
                  nx  , ny  = (idx >> 1) % self.wrc.w + 1, (idx >> 1) // self.wrc.w + 1
                  nbk , nfr = (screen[idx+1]>>4) & 0xf, screen[idx+1] & 0xf
                  if nbk != bk or nfr != fr:
                      line_data.append (b'\x1b[%d;%dm' % (30 + self.color_map[nfr], 40 + self.color_map[nbk]))
                      bk, fr = nbk, nfr
                  if ny != y or nx != x + 1:
                      line_data.append (b'\x1b[%d;%dH' % (ny, nx))
                      x, y = nx, ny
                  else:
                      x += 1
                  line_data.append(screen[idx].to_bytes(1, 'little'))
        if len(line_data) > 1:
            line_data.append(b'\x1B[0;0H\x1B[37;40m')
            data = b''.join(line_data)
        else:
            data = b''
        return data

    def refresh(self):
        if not self.freeze:
            if self.parent is None:
                screen = self.get_buffer()
                if (TERM.SCREEN_MODE & 1):
                    g_pipe.send(screen, self.wrc.w, 1)
                if (TERM.SCREEN_MODE & 2):
                    data = self.build_ansi_seq ()
                    if len(data) > 0:
                        g_pipe.send(data, 0, 2)
                    self.screen_old[:] = screen
            else:
                rc   = Rect(self.wrc)
                dlen = rc.w * rc.h * 2
                off  = rc.w * 2 * self.get_top()
                buf  = self.get_buffer()
                self.parent.blt_from (rc, buf[off:off+dlen])
                self.parent.refresh ()

    def draw_text(self, rc, line, marker=''):
        self.set_pos(rc.x, rc.y)
        fmt  = '%%-%ds' % rc.w
        disp = fmt % line[:rc.w]
        if marker == 'DW':
          ch = '\xaf'
          disp = disp [:-1] + ch
        self.write (disp)

    def draw_list(self, rc, lines, select = 0, start = 0):
        nlen = len(lines)
        if start >= nlen:
            start = rc.h - nlen
        if start < 0:
            start = 0
        if select   >= nlen:
            select   = nlen - 1
        curr = 0
        fmt = ' %%-%ds ' % (rc.w - 2)
        for line in lines[start:]:
            self.set_pos(rc.x, rc.y + curr)
            text = fmt % line[:rc.w]
            if select == start + curr:
                old_color = self.set_color (self.sel_fore_color, self.sel_back_color)
                self.write (text)
                self.restore_color (old_color)
            else:
                self.write (text)
            curr += 1
            if curr == rc.h:
                break
        while curr < rc.h:
            self.set_pos(rc.x, rc.y + curr)
            self.write (fmt % ' ')
            curr += 1

    def draw_vscroll (self, rc, curr, total):
        chrs = '\xf0\xf0\xdb\xb0'
        top   = rc.y + 1
        right = rc.x + rc.w - 1
        self.set_pos(right, top)
        self.write (chrs[0])
        top += 1
        bot  = rc.y + rc.h - 2
        if curr + 1 == total:
            pos = bot - 1
        else:
            pos = int (curr * (bot - top) // total) + top
        while top < bot:
            self.set_pos(right, top)
            if pos == top:
                self.write (chrs[2])
            else:
                self.write (chrs[3])
            top += 1
        self.set_pos(right, top)
        self.write (chrs[1])
        self.refresh()

    def draw_box (self, rc, style = 0, brd_mask = 0x0f):
        left, top, width, height  = rc.x, rc.y, rc.w, rc.h
        tbls = [ '\xda\xbf\xc0\xd9\xc4\xb3\xc3\xb4\xcb\xc1',
                 '\xc9\xbb\xc8\xbc\xcd\xba\xcc\xb9\xcb\xca']
        tbl  = tbls[style]
        bottom = top + height - 1
        self.set_pos(left, top)
        corner =list(tbl[:4])
        if not (brd_mask & 1):
            corner[0] = tbl[6]
            corner[1] = tbl[7]
        orner =list(tbl[:4])
        if not (brd_mask & 4):
            corner[0] = tbl[8]
            corner[2] = tbl[9]
        self.write (corner[0])
        if brd_mask & 1:
            self.write (tbl[4] * (width - 2))
        else:
            self.set_pos(left + (width - 1), top)
        self.write (corner[1])
        self.set_pos(left, bottom)
        self.write (corner[2])
        if brd_mask & 2:
            self.write (tbl[4] * (width - 2))
        self.write (corner[3])
        top += 1
        while top < bottom:
            self.set_pos(left, top)
            if brd_mask & 4:
                self.write (tbl[5])
            self.set_pos (left + width - 1, top)
            if brd_mask & 8:
                self.write (tbl[5])
            top += 1


class BaseWidget ():
    WM_IDLE       = 0
    WM_CHAR       = 1
    WM_KILL_FOCUS = 2
    WM_SET_FOCUS  = 3
    WM_REDRAW     = 4

    def __init__(self, window, rc, id = ''):
        self.window    = window
        self.rc        = Rect(rc)
        self.focused   = False
        self.enable    = True
        self.back_col  = 1
        self.fore_col  = 7
        self.text      = ''
        self.help      = ''
        self.window.add_child (self)
        self.editable  = False
        self.id        = id
        self.data      = None
        self.value     = ''

    def get_value (self):
        return self.value

    def set_value (self, value):
        self.value = value

    def get_data (self):
        return self.data

    def set_data (self, data):
        self.data = data

    def get_window (self):
        return self.window

    def get_help (self):
        return self.help

    def set_help (self, help):
        self.help = help

    def set_text(self, text):
        self.text   = text

    def get_text(self):
        return self.text

    def get_id(self):
        return self.id

    def set_color(self,  fore = None, back = None):
        if back is not None:
            self.back_col  = back
        if fore is not None:
            self.fore_col  = fore

    def set_enable (self, enable):
        self.enable = enable

    def is_enabled (self):
        return self.enable

    def set_editable (self, editable):
        if self.editable != editable:
            self.editable = editable
            self.redraw()

    def is_editable (self):
        return self.editable

    def redraw (self):
        self.window.refresh()

    def is_focusable (self):
        return self.enable

    def set_focus (self, focus = True):
        if focus != self.focused:
            self.focused = focus
            if not focus:
                self.set_editable (False)
            self.redraw()

    def has_focus (self):
        return self.focused


class Label (BaseWidget):
    def __init__(self, parent, rc, id = ''):
        super(Label, self).__init__(parent, rc, id)

    def draw_title (self):
        rc    = Rect(self.rc)
        old_color = self.window.set_color (self.fore_col, self.back_col)
        if rc.h > 1:
          lines = self.text.splitlines()
          if rc.h > len(lines):
              for i in range(rc.h - len(lines)):
                  lines.append('')
          for line in lines[:rc.h]:
              self.window.draw_text (rc, line)
              rc.adjust (0, 1, 0, 0)
        else:
          self.window.draw_text (rc, self.text)
        self.window.restore_color (old_color)

    def show (self):
        self.redraw ()

    def redraw (self):
        self.draw_title ()
        self.window.refresh()

    def set_text(self, text):
        self.text = text
        self.redraw ()

    def is_focusable (self):
        return False

    def proc (self, msg = BaseWidget.WM_IDLE, param1 = None, param2 = None):
        return -2


class Edit (BaseWidget):
    def __init__(self, parent, rc, id = ''):
        super(Edit, self).__init__(parent, rc, id)
        self.edit_text = ''
        self.caret = 0
        self.editable   = False
        self.edit_limit = 256
        self.min_val  = None
        self.max_val  = None
        self.org_len  = None
        self.org_val  = None
        self.old_text = ''

    def set_focus (self, focus = True):
        if focus != self.focused:
            self.focused = focus
            if not focus:
                self.set_editable (False)
            else:
                self.old_text = self.edit_text
                self.set_caret(0xffffffff)
            self.redraw()

    def draw_title (self):
        rc    = Rect(self.rc)
        old_color = self.window.set_color (TERM.COLOR_WINDOW[0], TERM.COLOR_WINDOW[1])
        self.window.draw_text (rc, self.text)
        self.window.restore_color (old_color)

    def draw_edit (self):
        rc    = Rect(self.rc)
        rc.y += 1
        rc.h -= 1
        if self.has_focus():
            old_color = self.window.set_color (TERM.COLOR_FOCUS[0], TERM.COLOR_FOCUS[1])
        else:
            old_color = self.window.set_color (TERM.COLOR_TEXT[0], TERM.COLOR_TEXT[1])
        line = self.edit_text
        self.window.draw_text (rc, line)

        if self.has_focus():
            self.window.set_pos (rc.x + self.caret, rc.y)
            if self.editable:
                self.window.set_char('', None, 5)
        self.window.restore_color (old_color)

    def set_edit_text (self, text):
        self.edit_text = text[:self.edit_limit]

    def validate_value (self):
        new_text = self.edit_text
        valid = True
        if self.min_val is not None or self.max_val is not None:
            try:
                val = array_str_to_value (new_text)
            except:
                valid = False
            if valid:
                if self.max_val is not None and val > self.max_val:
                    valid = False
                if self.min_val is not None and val < self.min_val:
                    valid = False
        if valid:
            if self.org_len is not None:
                new_text = reformat_value_str (new_text, self.org_len, self.org_val)
            self.edit_text = new_text
            self.old_text  = new_text
        else:
            self.edit_text = self.old_text
            self.set_caret (0xffffffff)
        return valid

    def set_format_info (self, val_str, bit_len):
        self.org_val = val_str
        self.org_len = bit_len

    def set_edit_limit (self, limit):
        if limit < 64:
            self.edit_limit = limit

    def set_edit_range (self, min = None, max = None):
        if min is not None:
            self.min_val = min
        if max is not None:
            self.max_val = max

    def show (self):
        self.draw_title ()
        self.redraw ()

    def redraw (self):
        self.draw_edit ()
        self.window.refresh()

    def set_caret (self, pos):
        if pos > len(self.edit_text):
            pos = len(self.edit_text)
        self.caret = pos

    def move_caret (self, dir):
        pos = self.caret + dir
        self.set_caret (pos)

    def proc (self, msg = BaseWidget.WM_IDLE, param1 = None, param2 = None):
        ret     = 0
        if self.is_editable():
            if msg == BaseWidget.WM_CHAR:
                ch      = param1
                if len(ch) > 1:
                    if   ch == b'left':
                        self.move_caret(-1)
                    elif ch == b'right':
                        self.move_caret(1)
                    elif ch == b'up':
                        self.set_caret(0)
                    elif ch == b'down':
                        self.set_caret(0xFFFF)
                    elif ch == b'del':
                        if len(self.edit_text) > 0:
                            line = self.edit_text[:self.caret] + self.edit_text[self.caret+1:]
                            self.set_edit_text (line)
                else:
                    if ch == b'\r':
                        valid = self.validate_value ()
                        self.set_value (self.edit_text)
                        if valid:
                            ret = -2
                    elif ch == b'\x1b':
                        self.set_edit_text (self.old_text)
                        ret = -2
                    else:
                        if ch == b'\x08':
                            if len(self.edit_text) > 0 and self.caret > 0:
                                line = self.edit_text[:self.caret - 1] + self.edit_text[self.caret:]
                                self.set_edit_text (line)
                                self.move_caret(-1)
                        elif ch >= b'\x20' and ch  < b'\x80':
                            line = self.edit_text[:self.caret] + chr(ch[0]) + self.edit_text[self.caret:]
                            self.set_edit_text (line)
                            self.move_caret(1)

                self.redraw ()

        return ret


class ListBox (BaseWidget):
    def __init__(self, parent, rc, id = ''):
        super(ListBox, self).__init__(parent, rc, id)
        self.options = []
        self.path    = []
        self.caret   = 0
        self.select  = 0
        self.first_time   = True
        self.option_start = 0
        self.client_rc = Rect(self.rc)
        self.client_rc.inflate (-1)

    def clear(self):
        self.options.clear()

    def push (self, id):
        self.path.append ((id, self.select, self.option_start))

    def pop (self):
        if len(self.path) > 1:
            id, self.select, self.option_start = self.path.pop()
            return id
        else:
            return None

    def peek (self):
        return self.path[-1][0]

    def add(self, option):
        options = []
        if type(option) is list:
            options.extend (option)
        else:
            options.append (option)
        self.options = options
        self.set_select (0)


    def set_focus (self, focus = True):
        if focus != self.focused:
            self.focused = focus
            if not focus:
                self.set_editable (False)
            else:
                self.set_editable (True)
            self.redraw()


    def set_select (self, sel):
        if sel >= len(self.options):
            sel = len(self.options) - 1
        if sel < 0:
            sel = 0
        self.select = sel

    def get_select (self):
        return self.select

    def get_select_text (self):
        return self.options[self.get_select()]

    def show (self):
        self.redraw ()

    def redraw (self):
        self.draw_list ()
        self.window.refresh()

    def draw_list (self):
        sel = self.get_select()
        if sel >= self.option_start + self.client_rc.h:
            self.option_start = sel - (self.client_rc.h - 1)
        elif sel < self.option_start:
            self.option_start = sel

        old_color = self.window.set_color (self.fore_col, self.back_col)
        if self.has_focus():
            old_sel_color = self.window.set_sel_color (TERM.COLOR_FOCUS[0], TERM.COLOR_FOCUS[1])
        else:
            old_sel_color = self.window.set_sel_color (TERM.COLOR_TEXT[0], TERM.COLOR_TEXT[1])
        self.window.draw_list (self.client_rc, self.options, self.get_select(), self.option_start)

        count = len(self.options)
        if count > self.client_rc.h:
            self.window.draw_vscroll (self.rc, self.get_select(), count)
        else:
            self.window.draw_vscroll (self.rc, -1, count)

        self.window.restore_color (old_color)
        if old_sel_color is not None:
            self.window.restore_sel_color (old_sel_color)

    def proc (self, msg = BaseWidget.WM_IDLE, param1 = None, param2 = None):
        ret = 0
        if self.is_editable():
            redraw  = False
            if msg == BaseWidget.WM_CHAR:
                old_sel = self.get_select()
                ch = param1
                if   ch == b'pgdw':
                    self.set_select (self.get_select() + self.rc.h)
                    redraw = True
                elif ch == b'pgup':
                    self.set_select (self.get_select() - self.rc.h)
                    redraw = True
                elif ch == b'home':
                    self.set_select (0)
                    redraw = True
                elif ch == b'end':
                    self.set_select (0xffffffff)
                    redraw = True
                elif   ch == b'down':
                    self.set_select (self.get_select() + 1)
                    redraw = True
                elif ch == b'up':
                    self.set_select (self.get_select() - 1)
                    redraw = True
                elif ch == b'left':
                    ret = 2
                elif ch == b'\r' or ch == b'right':
                    ret = 1
                elif ch == b'\x1b':
                    ret = -1
                elif ch == b'\t':
                    ret = -2
                if ret == 0 and old_sel  != self.get_select():
                    ret = 3
            if redraw:
                self.redraw ()

        if self.first_time:
          self.first_time = False
          ret = 3

        return ret


class ComboBox (BaseWidget):
    def __init__(self, parent, rc, id = ''):
        super(ComboBox, self).__init__(parent, rc, id)
        self.rc.h               = 2
        self.dropdown_rc        = None
        self.dropdown_client_rc = None
        self.save_buf           = bytearray()
        self.options            = []
        self.values             = []
        self.select             = 0
        self.old_select         = 0
        self.option_start       = 0

    def add(self, option):
        options = []
        if type(option) is list:
            options.extend (option)
        else:
            options.append (option)
        for idx, each in enumerate(options):
            try:
                val = each.index(':')
            except:
                val = -1
            if val >= 1:
                self.options.append(each[val + 1:])
                self.values.append(int(each[:val], 0))
            else:
                self.options.append(each)
                self.values.append(idx)

    def set_select (self, sel):
        if sel >= len(self.options):
            sel = len(self.options) - 1
        if sel < 0:
            sel = 0
        self.select = sel

    def set_select_by_value (self, value):
        sel = 0
        for idx, v in enumerate(self.values):
            if v == value:
                sel = idx
                break
        self.set_select (idx)

    def get_select (self):
        return self.select

    def set_editable (self, editable):
        if self.editable != editable:
            self.editable = editable
            if editable:
                # show drop down
                self.draw_dropdown ()
            else:
                # show combo
                if len(self.save_buf) > 0:
                    self.window.set_rect_buffer(self.dropdown_rc, self.save_buf)
                    self.save_buf = bytearray()
            self.redraw()

    def clear(self):
        self.options.clear()
        self.values.clear()

    def show (self):
        self.draw_title ()
        self.redraw ()

    def redraw (self):
        if self.is_editable():
            self.draw_dropdown_inner ()
        else:
            self.draw_combo ()
        self.window.refresh()

    def draw_title (self):
        rc    = Rect(self.rc)
        rc.h  = 1
        old_color = self.window.set_color (TERM.COLOR_WINDOW[0], TERM.COLOR_WINDOW[1])
        self.window.draw_text (rc, self.text)
        self.window.restore_color (old_color)

    def draw_combo (self):
        rc    = Rect(self.rc)
        rc.h  = 1
        rc.y += 1
        if not self.is_enabled():
            old_color = self.window.set_color (TERM.COLOR_DISABLED[0], TERM.COLOR_DISABLED[1])
        elif self.has_focus():
            old_color = self.window.set_color (TERM.COLOR_FOCUS[0], TERM.COLOR_FOCUS[1])
        else:
            old_color = self.window.set_color (TERM.COLOR_TEXT[0], TERM.COLOR_TEXT[1])
        self.window.draw_text (rc, self.options[self.select], 'DW')
        self.window.restore_color (old_color)

    def draw_dropdown_inner (self):
        sel = self.get_select()
        if sel >= self.option_start + self.dropdown_client_rc.h:
            self.option_start = sel - (self.dropdown_client_rc.h - 1)
        elif sel < self.option_start:
            self.option_start = sel
        old_color = self.window.set_color (TERM.COLOR_POPUP[0], TERM.COLOR_POPUP[1])
        self.window.draw_list (self.dropdown_client_rc, self.options, self.get_select(), self.option_start)
        count = len(self.options)
        if count > self.dropdown_client_rc.h:
            self.window.draw_vscroll (self.dropdown_rc, self.get_select(), count)
        self.window.restore_color (old_color)

    def draw_dropdown (self):
        self.option_start = 0

        self.dropdown_rc = Rect(self.rc)
        count = len(self.options)
        if count > self.window.wrc.h - 4:
            count = self.window.wrc.h - 4

        # calculate dropdown window height
        self.dropdown_rc.adjust (0, 0, 0, - self.dropdown_rc.h + count + 2)
        dropdown_bot = self.dropdown_rc.y + self.dropdown_rc.h
        window_bot   = self.window.get_top() + self.window.wrc.h
        if dropdown_bot > window_bot:
          self.dropdown_rc.y -= (dropdown_bot - window_bot)

        self.dropdown_client_rc = Rect(self.dropdown_rc)
        self.dropdown_client_rc.inflate (-1)

        # save the original state and screen buffer
        self.old_select = self.get_select()
        if len(self.save_buf) == 0:
            self.save_buf = self.window.get_rect_buffer(self.dropdown_rc)

        # draw box frame and contents
        old_color = self.window.set_color (TERM.COLOR_POPUP[0], TERM.COLOR_POPUP[1])
        self.window.draw_box (self.dropdown_rc)
        self.window.restore_color (old_color)
        self.draw_dropdown_inner()
        self.editable = True


    def proc (self, msg = BaseWidget.WM_IDLE, param1 = None, param2 = None):
        ret     = 0
        if self.is_editable():
            if msg == BaseWidget.WM_CHAR:
                ch      = param1
                redraw  = False
                if   ch == b'down':
                    self.set_select (self.get_select() + 1)
                    redraw = True
                elif ch == b'up':
                    self.set_select (self.get_select() - 1)
                    redraw = True
                elif ch == b'\x1b':
                    self.set_select (self.old_select)
                    ret = -2
                elif ch == b'\r':
                    self.set_value (hex(self.values[self.get_select ()]))
                    ret = -2
                if redraw:
                    self.redraw ()
        return ret


class Table (BaseWidget):
    def __init__(self, parent, rc, id = ''):
        super(Table, self).__init__(parent, rc, id)
        self.rows = 1
        self.cols = 1
        self.size = 0
        self.addr_len = 7
        self.bins = bytearray()
        self.col_byte_len = []
        self.col_hdr = ''
        self.col_hdr_color = 6
        self.col_hdr_sel_color = 14
        self.data_sel_fore_color = TERM.COLOR_FOCUS[0]
        self.data_sel_back_color = TERM.COLOR_FOCUS[1]
        self.data_edit_fore_color = TERM.COLOR_EDITING[0]
        self.data_edit_back_color = TERM.COLOR_EDITING[1]

        self.curr_offset = 0
        self.top_row     = 0
        self.editable    = False
        self.editing     = None

    def set_focus (self, focus = True):
        if focus != self.focused:
            self.focused = focus
            if not focus:
                self.set_editable (False)
            else:
                self.set_top (0)
                self.set_offset (0)
            self.redraw()

    def get_top (self):
        return self.top_row

    def set_top (self, row):
        if row > self.rows - (self.rc.h - 2):
            row = self.rows - (self.rc.h - 2)
        self.top_row = row

    def set_editing (self, editing = None):
        self.editing = editing

    def get_editing (self):
        return self.editing

    def get_cell_pos (self):
        return self.offset_to_cell_pos (self.get_offset())

    def set_cell_pos (self, col = None, row = None):
        if col is None or row is None:
            cur_col, cur_row = self.get_cell_pos ()
            if col is None:
                col = cur_col
            else:
                row = cur_row

        col, row = self.normalize_cell_pos (col, row)
        offset = self.cell_pos_to_offset(col, row)
        self.set_offset (offset)

    def normalize_cell_pos (self, col, row):
        cell_offset = row * self.cols + col
        if cell_offset < 0:
            cell_offset = 0
        row = cell_offset // self.cols
        col = cell_offset  % self.cols
        if row >= self.rows:
            row = self.rows - 1
        if row == self.rows - 1:
            row_byte_len = sum(self.col_byte_len)
            remain = self.size % row_byte_len
            if remain:
                max_col = self.row_byte_offset_to_col (remain)
                if col > max_col:
                    col = max_col
        return (col, row)

    def row_byte_offset_to_col (self, offset):
        for col, col_byte in enumerate(self.col_byte_len):
            if offset >= col_byte:
                offset -= col_byte
            else:
                offset = 0
                break
        if offset != 0:
            col = self.cols - 1
        return col

    def get_cell_length (self):
        col, row = self.get_cell_pos ()
        return  self.col_byte_len[col]

    def get_cell_data (self):
        length = self.get_cell_length ()
        offset = self.get_offset()
        value = bytes_to_value (self.bins[offset:offset+length])
        return value

    def get_cell_data_str (self):
        length = self.get_cell_length ()
        value  = self.get_cell_data ()
        return ("%%0%dX" % (length * 2) ) % value

    def set_cell_data_str (self, data_str):
        length = self.get_cell_length ()
        data = value_to_bytes (int(data_str, 16), length)
        self.update_data (self.get_offset(), data)

    def offset_to_cell_pos (self, offset):
        offset = self.normalize_offset (offset)
        row_byte_len = sum(self.col_byte_len)
        row    = offset // row_byte_len
        remain = offset  % row_byte_len
        col = self.row_byte_offset_to_col (remain)
        return (col, row)

    def cell_pos_to_offset (self, col, row):
        col, row = self.normalize_cell_pos (col, row)
        row_byte_len = sum(self.col_byte_len)
        if col > 0:
            offset = sum(self.col_byte_len[:col])
        else:
            offset = 0
        offset += row * row_byte_len
        return offset

    def adjust_cell_pos (self, delta_x = 0, delta_y = 0):
        col, row = self.get_cell_pos()
        col += delta_x
        row += delta_y
        if row < 0:
            row = 0
        if row >= self.rows:
            row = self.rows - 1
        self.set_cell_pos (col, row)

    def normalize_offset (self, pos):
        if pos < 0:
            pos = 0
        if pos >= self.size:
            pos = self.size - 1
        return pos

    def get_offset (self):
        return self.curr_offset

    def set_offset (self, offset):
        offset = self.normalize_offset (offset)
        self.curr_offset = offset
        self.set_editing ()
        # if pos is outside, adjust top
        col, row = self.offset_to_cell_pos(offset)
        top = self.get_top()
        if row < top:
            self.set_top (row)
        elif row > top + self.rc.h - 3:
            top = top + row - (top + self.rc.h - 3)
            self.set_top(top)

    def update_data (self, offset, data):
        if offset >= 0 and offset + len(data) <= self.size:
            self.bins[offset:offset+len(data)] = data

    def set_binary (self, bins):
        self.bins = bytearray (bins)
        self.size = len(bins)
        byte_len  = sum(self.col_byte_len)
        rows = (len(bins) + byte_len - 1) // byte_len
        self.rows = rows
        self.set_offset (0)

    @staticmethod
    def get_col_bytes (option):
        col_hdr = option.split(',')
        return [int(col.split(':')[1]) for col in col_hdr]

    def set_col_header (self, option):
        col_hdr = option.split(',')
        self.col_hdr = [each.strip() for each in col_hdr]
        self.cols = len(self.col_hdr)
        self.col_byte_len = [int(col.split(':')[1]) for col in col_hdr]

    def draw_title (self):
        rc    = Rect(self.rc)
        rc.h  = 1
        old_color = self.window.set_color (self.fore_col, self.back_col)
        self.window.draw_text (rc, self.text)
        self.window.restore_color (old_color)

    def draw_table_header (self):
        fmt = '%%-%ds' % self.rc.w
        old_color = self.window.set_color (self.col_hdr_color)
        rc = Rect (self.rc)
        rc.adjust (0, 1, -rc.w + self.addr_len, -rc.h + 1)
        if self.get_top() > 0:
            up = '\xec'
        else:
            up = ' '

        if self.rows > self.get_top() + self.rc.h - 2:
            down = '\xed'
        else:
            down = ' '
        self.window.draw_text (rc, up + '  ' + down)

        rc = Rect (self.rc)
        rc.adjust (self.addr_len, 1, 0, -rc.h + 1)
        cur_col, cur_row = self.get_cell_pos ()
        for col in range (self.cols):
            text = self.col_hdr[col].split(':')[0]
            rc.w = self.col_byte_len[col] * 2
            padding = ' ' * ((rc.w - len(text)) // 2)
            text = fmt % (padding + text)
            hight_lite = False
            if self.editable and  cur_col == col:
                hight_lite = True
                curr_color = self.window.set_color (self.col_hdr_sel_color)
            self.window.draw_text (rc, text)
            if hight_lite:
                self.window.restore_color (curr_color)
            rc.adjust (rc.w + 1, 0, 0, 0)

        rc = Rect (self.rc)
        rc.adjust (0, 2, -rc.w + self.addr_len, -rc.h + 1)
        addr_fmt = '%%0%dX:' % (self.addr_len - 3)
        row_byte_len = sum (self.col_byte_len)
        top = self.get_top()
        for row in range(top, top + self.rc.h - 2):  #Rows
            text = addr_fmt % (row * row_byte_len)
            hight_lite = False
            if self.editable:
                if cur_row == row:
                    hight_lite = True
            if hight_lite:
                curr_color = self.window.set_color (self.col_hdr_sel_color)
            self.window.draw_text (rc, text)
            if hight_lite:
                self.window.restore_color (curr_color)
            rc.adjust (0, 1, 0, 0)

        self.window.restore_color (old_color)


    def draw_table_data (self):
        fmt = '%%-%ds' % (self.rc.w - self.addr_len)
        idx = 0
        rc  = Rect (self.rc)
        rc.adjust (-rc.x + self.addr_len, 2, -self.addr_len, -rc.h + 1)
        col_hdr = self.col_hdr
        bytes_per_row = sum(self.col_byte_len)
        idx = bytes_per_row * self.get_top()
        top = self.get_top()
        for row in range(top, top + self.rc.h - 2): #Rows
            vals = []
            for col in range(self.cols):  #Columns
                if idx >= len(self.bins):
                    break
                byte_len  = self.col_byte_len[col]
                value = bytes_to_value (self.bins[idx:idx+byte_len])
                hex = ("%%0%dX" % (byte_len * 2) ) % value
                vals.append (hex)
                idx += byte_len
            text = ' '.join(vals)
            text = fmt % text[:self.rc.w]
            self.window.draw_text (rc, text)
            rc.adjust (0, 1)

            if idx >= self.size:
                break

    def draw_focus_data (self):
        # draw focus
        if self.editable or self.has_focus():
            col, row = self.get_cell_pos ()
            offset   = 0
            if col > 0:
                offset = sum (self.col_byte_len[:col]) * 2 + col
            byte_len = self.col_byte_len[col]
            rc  = Rect (self.addr_len, self.rc.y + 2, byte_len * 2, 1)
            rc.adjust (offset, row - self.get_top())
            cur_offset = self.get_offset()
            editing = self.get_editing ()
            if editing:
                hex = ("%%-%ds" % (byte_len * 2) ) % self.editing
            else:
                hex = self.get_cell_data_str ()

            if self.is_editable():
                old_color = self.window.set_color (self.data_edit_fore_color, self.data_edit_back_color)
            else:
                old_color = self.window.set_color (self.data_sel_fore_color, self.data_sel_back_color)

            self.window.draw_text (rc, hex)
            self.window.restore_color (old_color)


    def show (self):
        self.draw_title ()
        self.redraw ()

    def redraw (self):
        self.draw_table_header ()
        self.draw_table_data ()
        self.draw_focus_data ()
        self.window.refresh()

    def proc (self, msg = BaseWidget.WM_IDLE, param1 = None, param2 = None):
        ret     = 0
        if msg == BaseWidget.WM_REDRAW:
            self.redraw ()
            return ret

        elif msg != BaseWidget.WM_CHAR:
            return ret

        if not self.is_editable():
            return ret

        ch      = param1
        if len(ch) > 1:
            if   ch == b'left':
                self.adjust_cell_pos(-1, 0)
            elif ch == b'right':
                self.adjust_cell_pos(1, 0)
            elif ch == b'up':
                self.adjust_cell_pos(0, -1)
            elif ch == b'down':
                self.adjust_cell_pos(0, 1)
            elif ch == b'home':
                self.set_cell_pos(0)
            elif ch == b'end':
                self.set_cell_pos(self.cols - 1)
            elif ch == b'ctr+pgup':
                self.set_offset(0)
            elif ch == b'ctr+pgdw':
                self.set_offset(0xffffffff)
            elif ch == b'pgup':
                self.adjust_cell_pos(0, -(self.rc.h // 2))
            elif ch == b'pgdw':
                self.adjust_cell_pos(0,  (self.rc.h // 2))
        else:
            if ch in b'0123456789abcdefABCDEF\x08':
                length = self.get_cell_length()
                if ch == b'\x08':
                    if self.editing is None:
                        self.editing = ("%%0%dX" % (length * 2) ) % self.get_cell_data()
                    self.editing = self.editing[:-1]
                else:
                    new_ch = '%s' % chr(ch[0])
                    if self.editing is None:
                        self.editing = new_ch
                    else:
                        self.editing = self.editing + new_ch
                        if len(self.editing) == length * 2:
                            ch = b'\r'
            if ch == b'\r':
                if self.editing:
                    self.set_cell_data_str (self.editing)
                    self.set_value (bytes_to_bracket_str (self.bins))
                self.adjust_cell_pos(1, 0)
                if self.editing:
                    ret = -3
            elif ch == b'\x1b':
                if self.editing:
                    self.adjust_cell_pos(0, 0)
                else:
                    ret = -2

        self.redraw ()
        return ret


class MessageBox ():
    def __init__ (self, parent, rc):
        self.window    = parent
        self.rc        = Rect(rc)
        self.select    = 'n'

    def get_select (self):
        return self.select

    def wait_key (self):
        sel  = ''
        loop = True
        while True:
            ch = get_ch ()
            if len(ch) == 0:
                continue
            if ch in b'yn\x1b\r':
                if ch == b'\x1b':
                    sel = 'a'
                elif ch != b'\r':
                    sel = chr(ord(ch))
                loop = False
                break
            elif ch == b'left':
                sel = 'y'
                break
            elif ch == b'right':
                sel = 'n'
                break
        if sel:
            self.select = sel
        return loop

    def show(self, text):
        old_color = self.window.set_color(TERM.COLOR_POPUP[0], TERM.COLOR_POPUP[1])
        save_buf = self.window.get_rect_buffer(self.rc)
        self.window.draw_box (self.rc, 1)
        start = 0
        sel   = 'n'
        loop  = True
        while loop:
            rc = Rect(self.rc)
            rc.inflate (-1)
            fmt  = '%%-%ds' % rc.w
            for idx in range(rc.h):
                if idx == start:
                    line = text
                elif idx == start + 2:
                    line = 'YES  NO'
                else:
                    line = ''
                padding = (rc.w - len(line)) // 2
                if padding < 0:
                    padding = 0
                self.window.set_color(TERM.COLOR_TEXT[0])
                self.window.draw_text (rc, fmt % (' ' * padding + line))
                if idx == start + 2:
                    btn_rc = Rect (rc)
                    btn_rc.h  = 1
                    btn_rc.x += padding
                    btn_rc.w  = 3
                    self.window.set_color(TERM.COLOR_FOCUS[0] if sel == 'y' else TERM.COLOR_WINDOW[0])
                    self.window.draw_text (btn_rc, 'YES')
                    btn_rc.x += 5
                    btn_rc.w  = 2
                    self.window.set_color(TERM.COLOR_FOCUS[0] if sel != 'y' else TERM.COLOR_WINDOW[0])
                    self.window.draw_text (btn_rc, 'NO')
                rc.adjust (0, 1, 0, 0)
            self.window.refresh()

            loop = self.wait_key ()
            sel  = self.get_select ()

        self.window.set_rect_buffer(self.rc, save_buf)
        self.window.refresh()
        self.window.restore_color (old_color)


class Pages:
    def __init__ (self, cfg_tree):
        self.cfg_list  = cfg_tree['_cfg_list']
        self.page_tree = cfg_tree['_cfg_page']
        self.page_list = {}
        self.curr_page = ''
        self.widget = None

    def get_page_list (self, id = 'root'):
        page_list = []
        tree = self.find_page_id (id)
        for each in tree[id]['child']:
            page_id = next(iter(each))
            if len(each[page_id]['child']) > 0:
                prefix = '+ '
            else:
                prefix = '  '
            page_list.append(prefix + page_id)
        return page_list

    def get_cfg_list (self, page_id = None):
        if page_id is None:
            # return full list
            return self.cfg_list
        else:
            # build a new list for items under a page ID
            cfgs =  [i for i in self.cfg_list if i['cname'] and (i['page'] == page_id)]
            cfgs.sort(key=lambda x:x['order'])
            return cfgs

    def build_cfg_page (self, page_id):
        if page_id not in self.page_list:
            self.page_list[page_id] = self.get_cfg_list (page_id)

    def get_widget (self):
        return self.widget

    def set_widget (self, widget):
        self.widget = widget

    def find_page_id (self, page_id, top = None):
        if top is None:
            top = self.page_tree['root']
            if page_id == 'root':
                return self.page_tree
        for node in top['child']:
            page_key = next(iter(node))
            if page_id == page_key:
                return node
            else:
                result = self.find_page_id (page_id, node[page_key])
                if result is not None:
                    return result
        return None

    def get_page_title (self, id = 'root'):
        tree = self.find_page_id (id)
        return tree[id]['title']

    def set_active_page (self, id):
        self.curr_page = id


def load_sbl_cfgdata (pkl_file_name):
    def deep_convert_list (layer):
        if isinstance(layer, list):
            od = OrderedDict({})
            for each in layer:
                if isinstance(each, dict):
                    key = next(iter(each))
                    od[key] = deep_convert_list(each[key])
            return od
        else:
            return layer
    if is_mp():
        data = pyb.load_file (pkl_file_name)
    else:
        data = open(pkl_file_name, 'r').read()
    if len(data) == 0:
        raise Exception ('Cannot open file !')
    cfg_data = eval(data)
    return deep_convert_list (cfg_data)


def get_next_focus (widgets, idx, dir):
    next_focus  = idx
    last  = len(widgets)
    cfg   = None
    for first, widget in enumerate(widgets):
        if widget.get_window().get_id() == 'cfg':
            cfg = widget.get_window()
            break
    if   dir ==  2:
        next_focus = last - 1
    elif dir == -2:
        next_focus = first
    elif dir == -1 or dir == 1:
        start = idx + dir
        end = last if dir > 0 else first - 1
        for i in range (start, end, dir):
            if widgets[i].is_focusable():
                next_focus = i
                break
    elif dir == -3 or dir == 3:
        if dir > 0:
            target = cfg.get_top() + cfg.wrc.h
        else:
            target = cfg.get_top() - cfg.wrc.h
        if target >= cfg.vrc.h - cfg.wrc.h:
            target = cfg.vrc.h - cfg.wrc.h
        if target < 0:
            target = 0

        next_focus = -1
        for i in range (first, last):
            if dir > 0:
                next_focus = i
                if widgets[next_focus].rc.y > target:
                    break
            else:
                next_focus = first + last - i - 1
                if widgets[next_focus].rc.y < target:
                    break
        if next_focus >= 0:
            cfg.set_top (target)
        else:
            next_focus = idx

    return next_focus


def find_widget_by_id (widgets, id):
    for idx, widget in enumerate(widgets):
        if widget.get_id() == id:
            return idx, widget
    return None, None

def generate_values (cfg_tree):
    cfgs = cfg_tree['_cfg_list']
    cfg  = cfgs[-1]
    dlen = (cfg['length'] + cfg['offset'] + 7) // 8
    data = bytearray (dlen)
    for cfg in cfgs:
        value = format_str_to_value (cfg['value'], cfg['length'], False)
        set_bits_to_bytes (data, cfg['offset'], cfg['length'], value)
    return data

def update_values (cfg_tree,  data):
    cfgs = cfg_tree['_cfg_list']
    for cfg in cfgs:
        value = get_bits_from_bytes (data, cfg['offset'], cfg['length'])
        cfg['value'] = format_value_to_str (value, cfg['length'], cfg['value'])
        if 'order' not in cfg:
            cfg['order'] = cfg['offset']

def get_cfg_item_options (item):
    tmp_list = []
    builtin_option = {'$EN_DIS' : [('0', 'Disable'), ('1', 'Enable')]}
    if item['option'] in builtin_option:
        for op_val, op_str in builtin_option[item['option']]:
            tmp_list.append((op_val, op_str))
    else:
        opt_list = item['option'].split(',')
        for option in opt_list:
            option = option.strip()
            try:
                (op_val, op_str) = option.split(':')
            except:
                raise SystemExit ("Exception: Invalid option format '%s' !" % option)
            tmp_list.append((op_val, op_str))
    return  tmp_list


def format_value_to_str (value, bit_length, old_value = ''):
    # value is always int
    length    = (bit_length + 7) // 8
    fmt = ''
    if old_value.startswith ('0x'):
        fmt = '0x'
    elif old_value and (old_value[0] in ['"', "'", '{']):
        fmt = old_value[0]
    else:
        fmt = ''

    bvalue = value_to_bytearray (value, length)
    if fmt in ['"', "'"]:
        newval = bytes(bvalue).rstrip(b'\x00')
        svalue = string_decode (newval)
        value_str = fmt + svalue + fmt
    elif fmt == "{":
        value_str = '{ ' + ', '.join(['0x%02x' % i for i in bvalue]) + ' }'
    elif fmt == '0x':
        hex_len = length * 2
        if len(old_value) == hex_len + 2:
            fstr = '0x%%0%dX' % hex_len
        else:
            fstr = '0x%X'
        value_str = fstr % value
    else:
        if length <= 2:
            value_str = '%d'   % value
        elif length <= 8:
            value_str = '0x%x' % value
        else:
            value_str = '{ ' + ', '.join(['0x%02x' % i for i in bvalue]) + ' }'
    return value_str

def format_str_to_value (value_str, bit_length, array = True):
    value_str = value_str.strip()
    if check_quote (value_str):
        value_str = value_str[1:-1]
        bvalue = string_encode (value_str)
        if len(bvalue) == 0:
            bvalue = bytearray(b'\x00')
        if array:
            return  bvalue
        else:
            return  bytes_to_value (bvalue)
    else:
        if len(value_str)>0 and value_str[0] in '{' :
            value_str = value_str[1:-1].strip()
        value = 0
        for each in value_str.split(',')[::-1]:
            each = each.strip()
            value = (value << 8) | int(each, 0)
        if array:
            length = (bit_length + 7) // 8
            return value_to_bytearray (value, length)
        else:
            return value

def reformat_value_str (value_str, bit_length, old_value = ''):
    value = format_str_to_value (value_str, bit_length, False)
    return  format_value_to_str (value, bit_length, old_value)

def expand_cfg_var (cfg_list, expr):
    vars = {}
    var_name = None
    for i in expr:
        if i == '$':
            var_name = []
            continue
        if var_name is not None:
            if i.isalpha() or i in '_.0123456789':
                var_name.append(i)
            else:
                vars[''.join(var_name)] = 0
                var_name = None
    for each in vars:
        cfg_items = [cfg for cfg in cfg_list if cfg['path'] == each]
        if len(cfg_items) == 0:
            raise Exception ("Failed to find CFG variable '%s' !" % each)
        cfg = cfg_items[0]
        expr = expr.replace ('$' + each,  hex(format_str_to_value (cfg['value'], cfg['length'], False)))

    return expr

def evaluate_condition (cfg, cfg_list):
    if cfg['condition']:
        # 0: Hide   1: Visible
        expr  = expand_cfg_var (cfg_list, cfg['condition'])
        state = eval_expr (expr) & 1
    else:
        state = 1
    return state

def recheck_condition (visible_widgets, pages):
    if not pages.curr_page:
        return False

    page_id = pages.get_widget().get_select_text()[2:]
    page_cfg_list = pages.get_cfg_list(page_id)
    full_cfg_list = pages.get_cfg_list()

    visible_cfg_list = [widget.get_data() for widget in visible_widgets]
    for cfg in page_cfg_list:
        if cfg['condition']:
            state   = evaluate_condition (cfg, full_cfg_list)
            visible = cfg in visible_cfg_list
            if (state == 0) and visible:
                return True
            if (state == 1) and not visible:
                return True

    return False


def rebuild_cfgs (cfg_win, pages, page_id):
    pages.build_cfg_page (page_id)

    cfg_win.set_freeze (True)

    cfg_win.remove_child ()

    rc = Rect(0, 0, cfg_win.wrc.w, 2)
    rc.adjust(1, 0, -2, 0)

    for cfg in pages.page_list[page_id]:
        parts = cfg['type'].split(',')
        itype = parts[0]
        state = evaluate_condition (cfg, pages.get_cfg_list())
        if itype == 'Combo' :
            if state == 1 :
                combo = ComboBox(cfg_win, rc)
                combo.set_text(cfg['name'])
                ops_list = get_cfg_item_options (cfg)
                value = int(cfg['value'], 0)
                combo.add ([i[1] for i in ops_list])
                combo.set_select_by_value (value)
                combo.set_help (cfg['help'])
                combo.set_data(cfg)
                if cfg['path'] == 'PLATFORMID_CFG_DATA.PlatformId':
                    combo.set_enable (0)
                combo.show ()
                rc.adjust (0, rc.h + 1, 0, 0)

        elif itype in ["EditNum", "EditText"]:
            if state == 1:
                edit  = Edit(cfg_win, rc)
                edit.set_text(cfg['name'])
                if itype in ["EditText"]:
                    value = cfg['value'].strip("'")
                    edit.set_edit_limit(cfg['length'] // 8)
                elif itype in ["EditNum"]:
                    edit.set_format_info (cfg['value'], cfg['length'])
                    value = cfg['value'].strip("{").strip("}").strip()
                    parts = cfg['type'].split(',')
                    if len(parts) > 3:
                        min = parts[2].lstrip()[1:]
                        max = parts[3].rstrip()[:-1]
                        min_val = array_str_to_value(min)
                        max_val = array_str_to_value(max)
                        edit.set_edit_range(min_val, max_val)
                edit.set_edit_text(value)
                edit.set_help (cfg['help'])
                edit.set_data(cfg)
                edit.show ()
                rc.adjust (0, rc.h + 1, 0, 0)

        elif itype == 'Table':
            if state == 1:
                col_bytes = Table.get_col_bytes (cfg['option'])
                bytes_per_row = sum(col_bytes)
                data = format_str_to_value (cfg['value'], cfg['length'])
                rc.h = (len(data) + (bytes_per_row - 1)) // bytes_per_row + 2
                if rc.h > cfg_win.wrc.h:
                    rc.h = cfg_win.wrc.h - 4
                tabel1  = Table(cfg_win, rc)
                tabel1.set_text(cfg['name'])
                tabel1.set_col_header (cfg['option'])
                tabel1.set_data(cfg)
                tabel1.set_binary (data)
                tabel1.set_help (cfg['help'])
                tabel1.show()
                rc.adjust (0, rc.h + 1, 0, 0)

        elif itype in ['Reserved', 'Constant']:
            pass

        else:
            raise Exception ("Unknow type '%s' !" % itype)

    cfg_win.set_freeze (False)

def adjust_cfg_top (cfg,  widgets, curr_focus):
    wid    = cfg.get_id()
    if wid == 'cfg':
        top_out = -1
        bot_out = -1
        if widgets[curr_focus].rc.y < cfg.get_top():
            top_out = widgets[curr_focus].rc.y
        elif widgets[curr_focus].rc.y + widgets[curr_focus].rc.h > cfg.get_top() + cfg.wrc.h:
            bot_out = (widgets[curr_focus].rc.y + widgets[curr_focus].rc.h) -  cfg.wrc.h
        if top_out >= 0 and bot_out >= 0:
            # the full widget cannot fit in window
            new = (top_out + bot_out) // 2
        elif top_out >= 0:
            new = top_out
        elif bot_out >= 0:
            new = bot_out
        else:
            new = -1
        if new >= 0:
            if cfg.set_top (new):
                cfg.refresh()

def update_tree (cfg_tree):
    path   = ''
    offset = 0
    for cfg in cfg_tree['_cfg_list']:
        # build offset
        cfg['offset'] = offset
        offset += cfg['length']
        # build path
        if 'path' in cfg:
            path = cfg['path']
        cfg['path'] = '%s.%s' % (path, cfg['cname'])
        # build value
        if 'value' in cfg:
            if cfg['value'].startswith('x'):
                if len(cfg['value']) == 1:
                    cfg['value'] = '0x'
                else:
                    hex_len = int(cfg['value'][1:], 10)
                    fstr = '0x%%0%dX' % hex_len
                    cfg['value'] = fstr % 0
        else:
            cfg['value'] = '0'
    return offset

def main():
    # load cfgdata
    if is_mp():
        file = '!SETP/CFGJ'
    else:
        file = sys.argv[1]
    cfg_tree = load_sbl_cfgdata (file)
    blen = update_tree (cfg_tree)

    if is_mp ():
        data = bytearray(blen // 8)
        pyb.build_cfgd (data)
    else:
        data_file = sys.argv[2]
        fo = open(data_file, 'rb')
        data = bytearray(fo.read())
        fo.close()
    update_values (cfg_tree, data)

    # creat main screen
    scr = Window(None, Rect (0, 0, TERM.SCREEN_SIZE[0], TERM.SCREEN_SIZE[1]), 'scr')
    scr.set_color (TERM.COLOR_WINDOW[0], TERM.COLOR_WINDOW[1])
    scr.clear_screen ()

    splitter = TERM.SCREEN_SIZE[0] // 4
    start_y  = 1
    end_y    = 4

    # draw title
    text  = 'Slim Bootloader Setup'
    rc = Rect((TERM.SCREEN_SIZE[0] - len(text)) // 2, 0, scr.wrc.w, start_y)
    rc.adjust (0, 0, -rc.x * 2, -rc.y)

    title = Label(scr, rc)
    padding = ' ' * ((rc.w - len(text)) // 2)
    title.set_color (TERM.COLOR_TEXT[0])
    title.set_text (padding + text)
    title.show ()

    # draw left
    height   = scr.wrc.h - end_y
    rc = Rect(0, start_y, splitter, height)
    rc.adjust (0, 0, -rc.x, -rc.y)
    scr.draw_box (rc, 1)

    # add pages
    title_rc = Rect(rc)
    title_rc.inflate (-1)
    page_title = Label (scr, title_rc, 'page_title')
    page_title.set_text('/')
    page_title.set_color (TERM.COLOR_TEXT[0])
    page_title.show ()

    page_id = 'root'
    pages = Pages(cfg_tree)
    rc.adjust (0, 1, 0, -1)
    page_tree = ListBox (scr, rc, 'page_tree')
    pages.set_widget (page_tree)
    page_list = pages.get_page_list(page_id)
    pages.set_active_page (page_id)
    page_tree.push(page_id)
    page_tree.add(page_list)

    # draw right
    rc = Rect(splitter, start_y, scr.wrc.w, height)
    rc.adjust (-1, 0, -rc.x + 1, -rc.y )
    scr.draw_box (rc, 1, brd_mask = 0b1011)
    scr.draw_vscroll (rc, 0, 10)
    #scr.refresh ()

    # draw bottom
    bot_rc = Rect(0, scr.wrc.h - end_y - 1, scr.wrc.w, end_y + 1)
    scr.draw_box (bot_rc, 1, brd_mask = 0b1110)

    # creat cfg scroll screen
    rc.inflate (-1)
    cfg = Window(scr, Rect(rc), 'cfg')
    cfg.set_color (TERM.COLOR_WINDOW[0], TERM.COLOR_WINDOW[1])
    cfg.clear_screen ()

    # creat help
    rc = Rect(bot_rc)
    rc.inflate (-1)
    rc.adjust (1,0,-2)
    help = Label(scr, rc, 'help')
    help.set_color (TERM.COLOR_TEXT[0])
    help.show ()

    rc  = Rect(bot_rc)
    sel = main_loop (scr, cfg, pages)

    if is_mp ():
        if sel == 'y':
            data = generate_values (cfg_tree)
            pyb.save_cfgd (data)

        pyb.reboot (1)
        while True:
            utime.sleep_ms (20)
    else:
        if sel == 'y':
            data = generate_values (cfg_tree)
            fo = open(data_file, 'wb')
            fo.write (data)
            fo.close()

def main_loop (scr_win, cfg_win, pages):
    # message loop
    page_tree  = pages.get_widget()
    widgets    = scr_win.get_child() + cfg_win.get_child()
    idx, page_title = find_widget_by_id (widgets, 'page_title')
    hlp_idx, help = find_widget_by_id (widgets, 'help')

    curr_focus   = 0
    last_focus   = -1
    cfg_start    = -1
    adjust_top   = False
    req_quit     = False
    save_sel     = 'n'


    while True:

        scr_win.set_freeze (True)

        rebuild_page = False

        param1 = None
        param2 = None
        msg    = BaseWidget.WM_IDLE
        cfg    = widgets[curr_focus].get_window()
        wid    = cfg.get_id()

        if not widgets[curr_focus].has_focus():
            # adjust focus
            if last_focus >= 0:
                widgets[last_focus].set_focus (False)
            widgets[curr_focus].set_focus ()
            last_focus = curr_focus

            # update help window text
            lines = wrap_line (widgets[curr_focus].get_help(), help.rc.w)
            help.set_text ('\n'.join(lines))

            # set scroll pos for cfg page
            if wid == 'cfg':
                rc = Rect(cfg_win.wrc)
                rc.inflate(1)
                cfg_len = len(cfg_win.get_child())
                if cfg_len > 0:
                    curr = curr_focus - cfg_start
                    scr_win.draw_vscroll (rc, curr, cfg_len)

        # handle key
        next_focus = curr_focus
        ch  = get_ch ()
        if len(ch) > 0:
            msg = BaseWidget.WM_CHAR
            param1 = ch
            param2 = None
            if not widgets[curr_focus].is_editable() and wid == 'cfg':
                # widget has focus, but not in edit mode yet
                msg  = BaseWidget.WM_IDLE
                if ch == b'\t':
                    next_focus, widget = find_widget_by_id (widgets, 'page_tree')
                elif ch == b'\r':
                    widgets[curr_focus].set_editable(True)
                elif ch == b'\x1b':
                    req_quit = True
                    #next_focus = get_next_focus (widgets, curr_focus, 1)
                elif ch in [b'home', b'end']:
                    next_focus = get_next_focus (widgets, curr_focus, -2 if ch == b'home' else 2)
                elif ch in [b'down', b'up']:
                    next_focus = get_next_focus (widgets, curr_focus, -1 if ch == b'up'   else 1)
                elif ch in [b'pgdw', b'pgup']:
                    next_focus = get_next_focus (widgets, curr_focus, -3 if ch == b'pgup' else 3)

        ret = widgets[curr_focus].proc (msg, param1, param2)
        if ret  == -1:
            # quit
            req_quit = True

        elif ret == -2 or ret == -3:
            # widget changed its value
            # -2: changed, lost focus  -3: changed, keep focus
            if wid == 'cfg':
                cfgi  = widgets[curr_focus].get_data()
                new_value_str = widgets[curr_focus].get_value()
                if len(new_value_str) > 0:
                    # the value might have been changed
                    if check_quote(cfgi['value']):
                        new_value_str = "'%s'" % new_value_str
                    new_value = format_str_to_value (new_value_str, cfgi['length'], False)
                    cfgi['value'] = format_value_to_str (new_value, cfgi['length'], cfgi['value'])

            if ret == -2:
                # move to next focus
                next_focus = get_next_focus (widgets, curr_focus, 1)
                if next_focus == curr_focus:
                    # no more item on page, quit edit mode
                    widgets[curr_focus].set_editable (False)

            # widget changed its value, recheck condition
            if recheck_condition (widgets, pages):
                rebuild_page = True

            ret = 0


        if  widgets[curr_focus].get_id() == 'page_tree':
            if ret == 1 :
                # enter child pages
                # update the path at top
                page_id   = page_tree.get_select_text()[2:]
                page_list = pages.get_page_list(page_id)
                if len(page_list) > 0:
                    select    = widgets[curr_focus].get_select()
                    curr_page = pages.curr_page
                    pages.set_active_page (page_id)
                    page_tree.push ((curr_page, select))
                    select = 0
                ret = 3


            elif ret == 2:
                # back to up level
                page_info = page_tree.pop ()
                if page_info is not None:
                    page_id, select = page_info
                    page_list = pages.get_page_list(page_id)
                    pages.set_active_page (page_id)
                ret = 3

            else:
                page_list = []

            if ret == 3:
                # rebuild cfg tree
                if len(page_list) > 0:
                    new_page = page_id
                    page_tree.add(page_list)
                    widgets[curr_focus].set_select (select)
                    page_tree.show ()

                # select changed
                text     = widgets[curr_focus].get_select_text ()
                page_id  = text[2:]
                page_txt = '/' if pages.curr_page == 'root' else '../' + pages.curr_page
                text = pages.get_page_title (page_id)
                if text:
                    page_txt = page_txt + ' (%s)' % text
                page_title.set_text(page_txt)
                rebuild_page = True

        if rebuild_page:
            widgets[last_focus].set_focus (False)
            last_focus  =  -1
            # rebuild all widgets on cfg page
            rebuild_cfgs (cfg_win, pages, page_id)
            widgets   = scr_win.get_child() + cfg_win.get_child()
            cfg_start = len(scr_win.get_child())
            if curr_focus >= cfg_start:
                # current focus is on cfg page, so reset to 1st cfg item on page
                # otherwise, focus is on cfg tree, keep its original focus
                curr_focus = cfg_start
        else:
            # adjust display
            adjust_cfg_top (cfg, widgets, curr_focus)
            if curr_focus != next_focus:
                curr_focus = next_focus

        scr_win.set_freeze (False)

        if req_quit:
            # prompt to save or not
            req_quit = False
            msg = 'Save setup CFGDATA to flash ?'
            rc  = Rect((TERM.SCREEN_SIZE[0] - 40) // 2, (TERM.SCREEN_SIZE[1] - 5) // 2, 40, 5)
            msg_box = MessageBox (scr_win, rc)
            msg_box.show (msg)
            save_sel = msg_box.get_select()
            if save_sel != 'a':
                break

    scr_win.set_freeze (False)

    return save_sel

# Initialize communication pipe
if is_mp():
    g_pipe = NamedPipeVirtual()
else:
    if len(sys.argv) != 3:
        print ('Usage:\n  python SblSetup.py  cfg_json_file  cfg_bin_file')
        sys.exit (0)

    if TERM.SCREEN_MODE & 2:
        TERM.SCREEN_MODE = 2
        g_pipe = NamedPipeServer()
    else:
        TERM.SCREEN_MODE = 1
        g_pipe = NamedPipeClient()

main()
