## @ ConfigEditor.py
#
# Copyright (c) 2018 - 2021, Intel Corporation. All rights reserved.<BR>
# SPDX-License-Identifier: BSD-2-Clause-Patent
#
##

import os
import re
import sys
import marshal
from pathlib import Path

sys.dont_write_bytecode = True

import tkinter
import tkinter.ttk as ttk
import tkinter.messagebox as messagebox
import tkinter.filedialog as filedialog

from GenCfgData import CGenCfgData, bytes_to_value, bytes_to_bracket_str, value_to_bytes, array_str_to_value

class create_tool_tip(object):
    '''
    create a tooltip for a given widget
    '''
    in_progress = False

    def __init__(self, widget, text=''):
        self.top_win = None
        self.widget = widget
        self.text = text
        self.widget.bind("<Enter>", self.enter)
        self.widget.bind("<Leave>", self.leave)

    def enter(self, event=None):
        if self.in_progress:
            return
        if self.widget.winfo_class() == 'Treeview':
            # Only show help when cursor is on row header.
            rowid  = self.widget.identify_row(event.y)
            if rowid != '':
                return
        else:
            x, y, cx, cy = self.widget.bbox("insert")

        # limit the tooltip window width
        tool_tip_width = self.widget.winfo_width()
        if tool_tip_width < 200:
            tool_tip_width = 200
        if tool_tip_width > 500:
            tool_tip_width = 500

        cursor = self.widget.winfo_pointerxy()
        x = self.widget.winfo_rootx() + 35
        y = self.widget.winfo_rooty() + 20
        if cursor[1] > y and cursor[1] < y + 20:
            y += 20

        # creates a toplevel window
        self.top_win = tkinter.Toplevel(self.widget)
        # Leaves only the label and removes the app window
        self.top_win.wm_overrideredirect(True)
        self.top_win.wm_geometry("+%d+%d" % (x, y))
        label = tkinter.Message(self.top_win,
                      text=self.text,
                      width=tool_tip_width,
                      justify='left',
                      background='bisque',
                      relief='solid',
                      borderwidth=1,
                      font=("times", "10", "normal"))
        label.pack(ipadx=1)
        self.in_progress = True

    def leave(self, event=None):
        if self.top_win:
            self.top_win.destroy()
            self.in_progress = False


class validating_entry(tkinter.Entry):
    def __init__(self, master, **kw):
        tkinter.Entry.__init__(*(self, master), **kw)
        self.parent    = master
        self.old_value  = ''
        self.last_value = ''
        self.variable  = tkinter.StringVar()
        self.variable.trace("w", self.callback)
        self.config(textvariable=self.variable)
        self.config({"background": "#c0c0c0"})
        self.bind("<Return>", self.move_next)
        self.bind("<Tab>", self.move_next)
        self.bind("<Escape>", self.cancel)
        for each in ['BackSpace', 'Delete']:
            self.bind("<%s>" % each, self.ignore)
        self.display (None)

    def ignore (self, even):
        return "break"

    def move_next (self, event):
        if self.row < 0:
            return
        row, col = self.row, self.col
        txt, row_id, col_id = self.parent.get_next_cell (row, col)
        self.display (txt, row_id, col_id)
        return "break"

    def cancel (self, event):
        self.variable.set(self.old_value)
        self.display (None)

    def display (self, txt, row_id = '', col_id = ''):
        if txt is None:
            self.row = -1
            self.col = -1
            self.place_forget()
        else:
            row      = int('0x' + row_id[1:], 0) - 1
            col      = int(col_id[1:]) - 1
            self.row = row
            self.col = col
            self.old_value  = txt
            self.last_value = txt
            x, y, width, height = self.parent.bbox(row_id, col)
            self.place(x=x, y=y, w=width)
            self.variable.set(txt)
            self.focus_set()
            self.icursor(0)

    def callback(self, *Args):
        cur_val = self.variable.get()
        new_val = self.validate(cur_val)
        if new_val is not None and self.row >= 0:
            self.last_value = new_val
            self.parent.set_cell (self.row , self.col, new_val)
        self.variable.set(self.last_value)


    def validate(self, value):
        if len(value) > 0:
            try:
                int(value, 16)
            except:
                return None

        # Normalize the cell format
        self.update()
        cell_width = self.winfo_width ()
        max_len = custom_table.to_byte_length(cell_width) * 2
        cur_pos = self.index("insert")
        if cur_pos == max_len + 1:
            value = value[-max_len:]
        else:
            value = value[:max_len]
        if value == '':
            value = '0'
        fmt =  '%%0%dX' % max_len
        return fmt % int(value, 16)


class custom_table(ttk.Treeview):
    _Padding   = 20
    _Char_width = 6

    def __init__(self, parent, col_hdr, bins):
        cols = len(col_hdr)

        col_byte_len = []
        for col in range(cols):  #Columns
            col_byte_len.append(int(col_hdr[col].split(':')[1]))

        byte_len = sum(col_byte_len)
        rows = (len(bins) + byte_len - 1) // byte_len

        self.rows = rows
        self.cols = cols
        self.col_byte_len = col_byte_len
        self.col_hdr = col_hdr

        self.size = len(bins)
        self.last_dir = ''

        style = ttk.Style()
        style.configure("Custom.Treeview.Heading", font=('calibri', 10, 'bold'),  foreground="blue")
        ttk.Treeview.__init__(self, parent, height=rows, columns=[''] + col_hdr, show='headings', style="Custom.Treeview", selectmode='none')
        self.bind("<Button-1>", self.click)
        self.bind("<FocusOut>", self.focus_out)
        self.entry = validating_entry(self, width=4,  justify=tkinter.CENTER)

        self.heading(0, text='LOAD')
        self.column (0, width=60, stretch=0, anchor=tkinter.CENTER)

        for col in range(cols):  #Columns
            text = col_hdr[col].split(':')[0]
            byte_len  = int(col_hdr[col].split(':')[1])
            self.heading(col+1, text=text)
            self.column(col+1, width=self.to_cell_width(byte_len), stretch=0, anchor=tkinter.CENTER)

        idx = 0
        for row in range(rows):  #Rows
            text = '%04X' % (row * len(col_hdr))
            vals = ['%04X:' % (cols * row)]
            for col in range(cols):  #Columns
                if idx >= len(bins):
                    break
                byte_len  = int(col_hdr[col].split(':')[1])
                value = bytes_to_value (bins[idx:idx+byte_len])
                hex = ("%%0%dX" % (byte_len * 2) ) % value
                vals.append (hex)
                idx += byte_len
            self.insert('', 'end', values=tuple(vals))
            if idx >= len(bins):
                break

    @staticmethod
    def to_cell_width(byte_len):
        return byte_len * 2 * custom_table._Char_width + custom_table._Padding

    @staticmethod
    def to_byte_length(cell_width):
        return (cell_width - custom_table._Padding) // (2 * custom_table._Char_width)

    def focus_out (self, event):
        self.entry.display (None)

    def refresh_bin (self, bins):
        if not bins:
            return

        # Reload binary into widget
        bin_len = len(bins)
        for row in range(self.rows):
            iid  = self.get_children()[row]
            for col in range(self.cols):
                idx = row * sum(self.col_byte_len) + sum(self.col_byte_len[:col])
                byte_len = self.col_byte_len[col]
                if idx + byte_len <= self.size:
                    byte_len  = int(self.col_hdr[col].split(':')[1])
                    if idx + byte_len > bin_len:
                      val = 0
                    else:
                      val = bytes_to_value (bins[idx:idx+byte_len])
                    hex_val = ("%%0%dX" % (byte_len * 2) ) % val
                    self.set (iid, col + 1, hex_val)

    def get_cell (self, row, col):
        iid  = self.get_children()[row]
        txt  = self.item(iid, 'values')[col]
        return txt

    def get_next_cell (self, row, col):
        rows  = self.get_children()
        col  += 1
        if col > self.cols:
          col = 1
          row +=1
        cnt = row * sum(self.col_byte_len) + sum(self.col_byte_len[:col])
        if cnt > self.size:
          # Reached the last cell, so roll back to beginning
          row  = 0
          col  = 1

        txt   = self.get_cell(row, col)
        row_id = rows[row]
        col_id = '#%d' % (col + 1)
        return (txt, row_id, col_id)

    def set_cell (self, row, col, val):
        iid  = self.get_children()[row]
        self.set (iid, col, val)

    def load_bin (self):
        # Load binary from file
        path = filedialog.askopenfilename(
            initialdir=self.last_dir,
            title="Load binary file",
            filetypes=(("Binary files", "*.bin"), (
                "binary files", "*.bin")))
        if path:
            self.last_dir = os.path.dirname(path)
            fd = open(path, 'rb')
            bins = bytearray(fd.read())[:self.size]
            fd.close()
            bins.extend (b'\x00' * (self.size - len(bins)))
            return bins

        return None

    def click (self, event):
        row_id  = self.identify_row(event.y)
        col_id  = self.identify_column(event.x)
        if row_id == '' and col_id == '#1':
            # Clicked on "LOAD" cell
            bins = self.load_bin ()
            self.refresh_bin (bins)
            return

        if col_id == '#1':
            # Clicked on column 1 (Offset column)
            return

        item = self.identify('item', event.x, event.y)
        if not item or not col_id:
            # Not clicked on valid cell
            return

        # Clicked cell
        row    = int('0x' + row_id[1:], 0) - 1
        col    = int(col_id[1:]) - 1
        if row * self.cols + col > self.size:
            return

        vals = self.item(item, 'values')
        if col < len(vals):
            txt = self.item(item, 'values')[col]
            self.entry.display (txt, row_id, col_id)

    def get(self):
        bins = bytearray()
        row_ids = self.get_children()
        for row_id in row_ids:
            row = int('0x' + row_id[1:], 0) - 1
            for col in range(self.cols):
              idx = row * sum(self.col_byte_len) + sum(self.col_byte_len[:col])
              byte_len = self.col_byte_len[col]
              if idx + byte_len > self.size:
                  break
              hex = self.item(row_id, 'values')[col + 1]
              values = value_to_bytes (int(hex, 16) & ((1 << byte_len * 8) - 1), byte_len)
              bins.extend(values)
        return bins

class state:
    def __init__(self):
        self.state = False

    def set(self, value):
        self.state = value

    def get(self):
        return self.state

class application(tkinter.Frame):
    def __init__(self, master=None):
        root = master

        self.debug = True
        self.page_id = ''
        self.page_list = {}
        self.conf_list = {}
        self.cfg_data_obj = None
        self.org_cfg_data_bin = None
        self.in_left  = state()
        self.in_right = state()
        self.search_text = ''
        self.search_key  = 0

        # Check if current directory contains a file with a .yaml extension
        # if not default self.last_dir to a Platform directory where it is easier to locate *BoardPkg\CfgData\*Def.yaml files
        self.last_dir = '.'
        if not any(fname.endswith('.yaml') for fname in os.listdir('.')):
            platform_path = Path(os.path.realpath(__file__)).parents[2].joinpath('Platform')
            if  platform_path.exists():
                self.last_dir = platform_path

        tkinter.Frame.__init__(self, master, borderwidth=2)

        self.menu_string = [
            'Save Config Data to Binary', 'Load Config Data from Binary',
            'Load Config Changes from Delta File',
            'Save Config Changes to Delta File',
            'Save Full Config Data to Delta File'
        ]

        root.geometry("1200x800")

        # Search string
        fram = tkinter.Frame(root)
        # adding label to search box
        tkinter.Label(fram, text='Text to find:').pack(side=tkinter.LEFT)
        # adding of single line text box
        self.edit = tkinter.Entry(fram, width=30)
        # positioning of text box
        self.edit.pack(
            side=tkinter.LEFT, fill=tkinter.BOTH, expand=1, pady=2, padx=(4, 4))
        self.edit.bind("<Return>", (lambda event: self.search_bar()))
        # setting focus
        self.edit.focus_set()
        # adding of search button
        butt = tkinter.Button(fram, text='Search', relief=tkinter.GROOVE,
                              command=self.search_bar)
        butt.pack(side=tkinter.RIGHT, padx=(4, 4))
        fram.pack(side=tkinter.TOP, anchor=tkinter.SE)

        paned = ttk.Panedwindow(root, orient=tkinter.HORIZONTAL)
        paned.pack(fill=tkinter.BOTH, expand=True, padx=(4, 4))

        status = tkinter.Label(master, text="", bd=1, relief=tkinter.SUNKEN, anchor=tkinter.W)
        status.pack(side=tkinter.BOTTOM, fill=tkinter.X)

        frame_left = ttk.Frame(paned, height=800, relief="groove")

        self.left = ttk.Treeview(frame_left, show="tree")

        # Set up tree HScroller
        pady = (10, 10)
        self.tree_scroll = ttk.Scrollbar(frame_left,
                                        orient="vertical",
                                        command=self.left.yview)
        self.left.configure(yscrollcommand=self.tree_scroll.set)
        self.left.bind("<<TreeviewSelect>>", self.on_config_page_select_change)
        self.left.bind("<Enter>",  lambda e: self.in_left.set(True))
        self.left.bind("<Leave>",  lambda e: self.in_left.set(False))
        self.left.bind("<MouseWheel>",  self.on_tree_scroll)

        self.left.pack(side='left',
                       fill=tkinter.BOTH,
                       expand=True,
                       padx=(5, 0),
                       pady=pady)
        self.tree_scroll.pack(side='right', fill=tkinter.Y, pady=pady, padx=(0, 5))

        frame_right = ttk.Frame(paned, relief="groove")
        self.frame_right = frame_right

        self.conf_canvas = tkinter.Canvas(frame_right, highlightthickness=0)
        self.page_scroll = ttk.Scrollbar(frame_right,
                                        orient="vertical",
                                        command=self.conf_canvas.yview)
        self.right_grid = ttk.Frame(self.conf_canvas)
        self.conf_canvas.configure(yscrollcommand=self.page_scroll.set)
        self.conf_canvas.pack(side='left',
                             fill=tkinter.BOTH,
                             expand=True,
                             pady=pady,
                             padx=(5, 0))
        self.page_scroll.pack(side='right', fill=tkinter.Y, pady=pady, padx=(0, 5))
        self.conf_canvas.create_window(0, 0, window=self.right_grid, anchor='nw')
        self.conf_canvas.bind('<Enter>',  lambda e: self.in_right.set(True))
        self.conf_canvas.bind('<Leave>',  lambda e: self.in_right.set(False))
        self.conf_canvas.bind("<Configure>", self.on_canvas_configure)
        self.conf_canvas.bind_all("<MouseWheel>", self.on_page_scroll)

        paned.add(frame_left, weight=2)
        paned.add(frame_right, weight=10)

        style = ttk.Style()
        style.layout("Treeview", [('Treeview.treearea', {'sticky': 'nswe'})])

        menubar = tkinter.Menu(root)
        file_menu = tkinter.Menu(menubar, tearoff=0)
        file_menu.add_command(label="Open Config YAML file...",
                             command=self.load_from_yaml)
        file_menu.add_command(label=self.menu_string[0],
                             command=self.save_to_bin,
                             state='disabled')
        file_menu.add_command(label=self.menu_string[1],
                             command=self.load_from_bin,
                             state='disabled')
        file_menu.add_command(label=self.menu_string[2],
                             command=self.load_from_delta,
                             state='disabled')
        file_menu.add_command(label=self.menu_string[3],
                             command=self.save_to_delta,
                             state='disabled')
        file_menu.add_command(label=self.menu_string[4],
                             command=self.save_full_to_delta,
                             state='disabled')
        file_menu.add_command(label="About", command=self.about)
        menubar.add_cascade(label="File", menu=file_menu)
        self.file_menu = file_menu

        root.config(menu=menubar)

        if len(sys.argv) > 1:
            path = sys.argv[1]
            if not path.endswith('.yaml') and not path.endswith('.pkl'):
                messagebox.showerror('LOADING ERROR', "Unsupported file '%s' !" % path)
                return
            else:
                self.load_cfg_file (path)

        if len(sys.argv) > 2:
            path = sys.argv[2]
            if path.endswith('.dlt'):
                self.load_delta_file (path)
            elif path.endswith('.bin'):
                self.load_bin_file (path)
            else:
                messagebox.showerror('LOADING ERROR', "Unsupported file '%s' !" % path)
                return

    def get_widget_from_config (self, cfg_item):
        for widget in self.right_grid.winfo_children():
            item = self.get_config_data_item_from_widget (widget)
            if item is None:
                continue
            if item is not cfg_item:
                continue
            return widget
        return None

    def ensure_config_visible (self, widget, focus = False):
        min, max = self.conf_canvas.yview()
        if max < 0.999:
            fra = (widget.winfo_rooty() - self.right_grid.winfo_rooty() - 32) / self.right_grid.winfo_height()
            self.conf_canvas.yview_moveto (fra)
        if focus:
            widget.focus_set ()

    def ensure_page_visible (self, page_id):
        page_path = self.cfg_data_obj.find_page_path (page_id)

        # expand page from top level to leaf level
        tree      = self.left
        children  = tree.get_children()
        for page_id in page_path:
            for child_id in children:
                if child_id == page_id:
                    tree.selection_set(child_id)
                    tree.focus(child_id)
                    tree.item(child_id, open=True)
                    children = tree.get_children(child_id)
                    break
        self.refresh_config_data_page ()


    def search_bar(self):
        # get data from text box
        new_text = self.edit.get()
        if (not new_text) or (not self.search_text) or (new_text != self.search_text):
            self.search_text = new_text
            self.search_key  = 0

        if not new_text:
            return

        # find the text
        while True:
            cfg_item  = self.cfg_data_obj.find_item_by_name(self.search_text, self.search_key)
            if cfg_item is None:
                self.search_key  = 0
                messagebox.showerror('SEARCHING INFO', "Could not find more matching !")
                break

            # use key to identify each match
            self.search_key = self.search_key + 1

            # expand the page tree to ensure the required page is visiable
            self.ensure_page_visible (cfg_item['page'])

            widget = self.get_widget_from_config (cfg_item)
            if widget and widget.winfo_ismapped():
                # scroll to item so that the config item is visible
                self.ensure_config_visible (widget, True)
                break

    def set_object_name(self, widget, name):
        self.conf_list[id(widget)] = name

    def get_object_name(self, widget):
        if id(widget) in self.conf_list:
            return self.conf_list[id(widget)]
        else:
            return None

    def limit_entry_size(self, variable, limit):
        value = variable.get()
        if len(value) > limit:
            variable.set(value[:limit])

    def on_canvas_configure(self, event):
        self.right_grid.grid_columnconfigure(0, minsize=event.width)

    def on_tree_scroll(self, event):
        if not self.in_left.get() and self.in_right.get():
            # This prevents scroll event from being handled by both left and
            # right frame at the same time.
            self.on_page_scroll (event)
            return 'break'

    def on_page_scroll(self, event):
        if self.in_right.get():
            # Only scroll when it is in active area
            min, max = self.page_scroll.get()
            if not ((min == 0.0) and (max == 1.0)):
                self.conf_canvas.yview_scroll(-1 * int(event.delta / 120), 'units')

    def update_visibility_for_widget(self, widget, args):

        visible = True
        item = self.get_config_data_item_from_widget(widget, True)
        if item is None:
            return visible
        elif not item:
            return visible

        result = 1
        if item['condition']:
            result = self.evaluate_condition(item)
            if result == 2:
                # Gray
                if not isinstance(widget, custom_table):
                    widget.configure(state='disabled')
            elif result == 0:
                # Hide
                visible = False
                widget.grid_remove()
            else:
                # Show
                widget.grid()
                if not isinstance(widget, custom_table):
                    widget.configure(state='normal')


        return visible

    def update_widgets_visibility_on_page(self):
        self.walk_widgets_in_layout(self.right_grid,
                                 self.update_visibility_for_widget)

    def combo_select_changed(self, event):
        self.update_config_data_from_widget(event.widget, None)
        self.update_widgets_visibility_on_page()
        self.update_page_scroll_bar(False)

    def edit_num_finished(self, event):
        widget = event.widget
        item = self.get_config_data_item_from_widget(widget)
        if not item:
            return
        parts = item['type'].split(',')
        if len(parts) > 3:
            min = parts[2].lstrip()[1:]
            max = parts[3].rstrip()[:-1]
            min_val = array_str_to_value(min)
            max_val = array_str_to_value(max)
            text = widget.get()
            if ',' in text:
                text = '{ %s }' % text
            try:
                value = array_str_to_value(text)
                if value < min_val or value > max_val:
                    raise Exception('Invalid input!')
                self.set_config_item_value(item, text)
            except Exception as e:
                pass

            text = item['value'].strip('{').strip('}').strip()
            widget.delete(0, tkinter.END)
            widget.insert(0, text)

        self.update_widgets_visibility_on_page()

    def update_page_scroll_bar(self, toTop=True):
        # Update scrollbar
        self.frame_right.update()
        self.conf_canvas.config(scrollregion=self.conf_canvas.bbox("all"))
        if toTop:
            self.conf_canvas.yview_moveto (0)

    def on_config_page_select_change(self, event):
        self.update_config_data_on_page()
        sel = self.left.selection()
        if len(sel) > 0:
            page_id = sel[0]
            self.build_config_data_page(page_id)
            self.update_widgets_visibility_on_page()
            self.update_page_scroll_bar()

    def walk_widgets_in_layout(self, parent, callback_function, args=None):
        for widget in parent.winfo_children():
            callback_function(widget, args)

    def clear_widgets_inLayout(self, parent=None):
        if parent is None:
            parent = self.right_grid

        for widget in parent.winfo_children():
            widget.destroy()

        parent.grid_forget()
        self.conf_list.clear()

    def build_config_page_tree(self, cfg_page, parent):
        for page in cfg_page['child']:
            page_id = next(iter(page))
            # Put CFG items into related page list
            self.page_list[page_id] = self.cfg_data_obj.get_cfg_list (page_id)
            self.page_list[page_id].sort (key=lambda x: x['order'])
            page_name = self.cfg_data_obj.get_page_title(page_id)
            child = self.left.insert(
                parent, 'end',
                iid=page_id, text=page_name,
                value=0)
            if len(page[page_id]) > 0:
                self.build_config_page_tree(page[page_id], child)


    def is_config_data_loaded(self):
        return True if len(self.page_list) else False

    def set_current_config_page(self, page_id):
        self.page_id = page_id

    def get_current_config_page(self):
        return self.page_id

    def get_current_config_data(self):
        page_id = self.get_current_config_page()
        if page_id in self.page_list:
            return self.page_list[page_id]
        else:
            return []

    def build_config_data_page(self, page_id):
        self.clear_widgets_inLayout()
        self.set_current_config_page(page_id)
        disp_list = []
        for item in self.get_current_config_data():
            disp_list.append(item)
        row = 0
        disp_list.sort(key=lambda x:x['order'])
        for item in disp_list:
            self.add_config_item (item, row)
            row += 2

    def load_config_data(self, file_name):
        gen_cfg_data = CGenCfgData()
        if file_name.endswith('.pkl'):
            with open(file_name, "rb") as pkl_file:
                gen_cfg_data.__dict__ = marshal.load(pkl_file)
            gen_cfg_data.prepare_marshal (False)
        elif file_name.endswith('.yaml'):
            if gen_cfg_data.load_yaml(file_name) != 0:
                raise Exception(gen_cfg_data.get_last_error())
        else:
            raise Exception('Unsupported file "%s" !' % file_name)
        return gen_cfg_data

    def about(self):
        msg = 'Configuration Editor\n--------------------------------\nVersion 0.8\n2020'
        lines = msg.split('\n')
        width = 30
        text = []
        for line in lines:
            text.append(line.center(width, ' '))
        messagebox.showinfo('Config Editor', '\n'.join(text))

    def update_last_dir (self, path):
        self.last_dir = os.path.dirname(path)

    def get_open_file_name(self, ftype):
        if self.is_config_data_loaded():
            if ftype == 'dlt':
                question = ''
            elif ftype == 'bin':
                question = 'All configuration will be reloaded from BIN file, continue ?'
            elif ftype == 'yaml':
                question = ''
            else:
                raise Exception('Unsupported file type !')
            if question:
                reply = messagebox.askquestion('', question, icon='warning')
                if reply == 'no':
                    return None

        if ftype == 'yaml':
            file_type = 'YAML or PKL'
            file_ext  = 'pkl *Def.yaml'
        else:
            file_type = ftype.upper()
            file_ext  = ftype

        path = filedialog.askopenfilename(
            initialdir=self.last_dir,
            title="Load file",
            filetypes=(("%s files" % file_type, "*.%s" % file_ext), (
                "all files", "*.*")))
        if path:
            self.update_last_dir (path)
            return path
        else:
            return None


    def load_from_delta(self):
        path = self.get_open_file_name('dlt')
        if not path:
            return
        self.load_delta_file (path)

    def load_delta_file (self, path):
        self.reload_config_data_from_bin(self.org_cfg_data_bin)
        try:
            self.cfg_data_obj.override_default_value(path)
        except Exception as e:
            messagebox.showerror('LOADING ERROR', str(e))
            return
        self.update_last_dir (path)
        self.refresh_config_data_page()

    def load_from_bin(self):
        path = self.get_open_file_name('bin')
        if not path:
            return
        self.load_bin_file (path)

    def load_bin_file (self, path):
        with open(path, 'rb') as fd:
            bin_data = bytearray(fd.read())
        if len(bin_data) < len(self.org_cfg_data_bin):
            messagebox.showerror('Binary file size is smaller than what YAML requires !')
            return

        try:
            self.reload_config_data_from_bin(bin_data)
        except Exception as e:
            messagebox.showerror('LOADING ERROR', str(e))
            return

    def load_cfg_file(self, path):
        # Save current values in widget and clear  database
        self.clear_widgets_inLayout()
        self.left.delete(*self.left.get_children())

        self.cfg_data_obj = self.load_config_data(path)

        self.update_last_dir (path)
        self.org_cfg_data_bin = self.cfg_data_obj.generate_binary_array()
        self.build_config_page_tree(self.cfg_data_obj.get_cfg_page()['root'], '')

        for menu in self.menu_string:
            self.file_menu.entryconfig(menu, state="normal")

        return 0

    def load_from_yaml(self):
        path = self.get_open_file_name('yaml')
        if not path:
            return

        self.load_cfg_file(path)

    def get_save_file_name (self, extension):
        path = filedialog.asksaveasfilename(
                  initialdir=self.last_dir,
                  title="Save file",
                  defaultextension=extension)
        if path:
            self.last_dir = os.path.dirname(path)
            return path
        else:
            return None

    def save_delta_file(self, full=False):
        path = self.get_save_file_name (".dlt")
        if not path:
            return

        self.update_config_data_on_page()
        new_data = self.cfg_data_obj.generate_binary_array()
        self.cfg_data_obj.generate_delta_file_from_bin (path, self.org_cfg_data_bin, new_data, full)

    def save_to_delta(self):
        self.save_delta_file()

    def save_full_to_delta(self):
        self.save_delta_file(True)

    def save_to_bin(self):
        path = self.get_save_file_name (".bin")
        if not path:
            return

        self.update_config_data_on_page()
        with open(path, 'wb') as fd:
            bins = self.cfg_data_obj.generate_binary_array()
            fd.write(bins)


    def refresh_config_data_page(self):
        self.clear_widgets_inLayout()
        self.on_config_page_select_change(None)

    def reload_config_data_from_bin(self, bin_dat):
        self.cfg_data_obj.load_default_from_bin(bin_dat)
        self.refresh_config_data_page()

    def set_config_item_value(self, item, value_str):
        itype = item['type'].split(',')[0]
        if itype == "Table":
            new_value = value_str
        elif itype == "EditText":
            length = (self.cfg_data_obj.get_cfg_item_length(item) + 7) // 8
            new_value = value_str[:length]
            if item['value'].startswith("'"):
                new_value = "'%s'" % new_value
        else:
            try:
                new_value = self.cfg_data_obj.reformat_value_str (value_str, self.cfg_data_obj.get_cfg_item_length(item), item['value'])
            except:
                print("WARNING: Failed to format value string '%s' for '%s' !" % (value_str, item['path']))
                new_value = item['value']

        if item['value'] != new_value:
            if self.debug:
                print('Update %s from %s to %s !' % (item['cname'],  item['value'], new_value))
            item['value'] = new_value


    def get_config_data_item_from_widget(self, widget, label=False):
        name = self.get_object_name(widget)
        if not name or not len(self.page_list):
            return None

        if name.startswith('LABEL_'):
            if label:
                path = name[6:]
            else:
                return None
        else:
            path = name
        item = self.cfg_data_obj.get_item_by_path (path)
        return item


    def update_config_data_from_widget(self, widget, args):
        item = self.get_config_data_item_from_widget(widget)
        if item is None:
            return
        elif not item:
            if isinstance(widget, tkinter.Label):
                return
            raise Exception('Failed to find "%s" !' % self.get_object_name(widget))

        itype = item['type'].split(',')[0]
        if itype == "Combo":
            opt_list = self.cfg_data_obj.get_cfg_item_options (item)
            tmp_list = [opt[0] for opt in opt_list]
            idx = widget.current()
            self.set_config_item_value(item, tmp_list[idx])
        elif itype in ["EditNum", "EditText"]:
            self.set_config_item_value(item, widget.get())
        elif itype in ["Table"]:
            new_value = bytes_to_bracket_str(widget.get())
            self.set_config_item_value(item, new_value)


    def evaluate_condition(self, item):
        try:
            result = self.cfg_data_obj.evaluate_condition(item)
        except:
            print("WARNING: Condition '%s' is invalid for '%s' !" % (item['condition'], item['path']))
            result = 1
        return result


    def add_config_item(self, item, row):
        parent = self.right_grid

        name = tkinter.Label(parent, text=item['name'], anchor="w")

        parts = item['type'].split(',')
        itype = parts[0]
        widget = None

        if itype == "Combo":
            # Build
            opt_list = self.cfg_data_obj.get_cfg_item_options (item)
            current_value = self.cfg_data_obj.get_cfg_item_value (item, False)
            option_list = []
            current = None

            for idx, option in enumerate(opt_list):
                option_str   = option[0]
                try:
                    option_value = self.cfg_data_obj.get_value(option_str, len(option_str), False)
                except:
                    option_value = 0
                    print('WARNING: Option "%s" has invalid format for "%s" !' % (option_str, item['path']))
                if option_value == current_value:
                    current = idx
                option_list.append(option[1])


            widget = ttk.Combobox(parent, value=option_list, state="readonly")
            widget.bind("<<ComboboxSelected>>", self.combo_select_changed)
            widget.unbind_class("TCombobox", "<MouseWheel>")

            if current is None:
                print('WARNING: Value "%s" is an invalid option for "%s" !' %
                      (current_value, item['path']))
            else:
                widget.current(current)

        elif itype in ["EditNum", "EditText"]:
            txt_val = tkinter.StringVar()
            widget = tkinter.Entry(parent, textvariable=txt_val)
            value = item['value'].strip("'")
            if itype in ["EditText"]:
                txt_val.trace(
                    'w',
                    lambda *args: self.limit_entry_size(txt_val, (self.cfg_data_obj.get_cfg_item_length(item) + 7) // 8))
            elif itype in ["EditNum"]:
                value = item['value'].strip("{").strip("}").strip()
                widget.bind("<FocusOut>", self.edit_num_finished)
            txt_val.set(value)

        elif itype in ["Table"]:
            bins = self.cfg_data_obj.get_cfg_item_value(item, True)
            col_hdr = item['option'].split(',')
            widget = custom_table(parent, col_hdr, bins)

        else:
            if itype and itype not in ["Reserved", "Constant"]:
                print ("WARNING: Type '%s' is invalid for '%s' !" % (itype, item['path']))

        if widget:
            help_txt = '%s:\n%s' % (item['cname'], item['help'])
            ttp = create_tool_tip(widget, help_txt)
            self.set_object_name(name,   'LABEL_' + item['path'])
            self.set_object_name(widget, item['path'])
            name.grid(row=row, column=0, padx=10, pady=5, sticky="nsew")
            widget.grid(row=row + 1, rowspan=1, column=0, padx=10, pady=5, sticky="nsew")

    def update_config_data_on_page(self):
        self.walk_widgets_in_layout(self.right_grid,
                                 self.update_config_data_from_widget)



if __name__ == '__main__':
    root = tkinter.Tk()
    app  = application(master=root)
    root.title("Config Editor")
    root.mainloop()

