## @ SblBuilder04_CfgDataPage.py
#
# Copyright (c) 2026, Intel Corporation. All rights reserved.<BR>
# SPDX-License-Identifier: BSD-2-Clause-Patent
#
##

import glob
import os
import sys
import tkinter as tk
from tkinter import ttk, StringVar, filedialog, messagebox
from SblBuilderGuiComp import *

# Resolve the GenCfgData tool relative to this file's location.
# tabs/SblBuilder04_CfgDataPage.py -> tabs -> SblBuilderGui -> Tools (repo Tools dir)
_TOOLS_DIR = os.path.dirname(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
if _TOOLS_DIR not in sys.path:
    sys.path.insert(0, _TOOLS_DIR)

try:
    from GenCfgData import CGenCfgData
except ImportError:
    CGenCfgData = None

DEFAULT_CFGDATA_KEY_FILENAME = "ConfigTestKey_Priv_RSA3072.pem"
DEFAULT_OUTPUT_FILENAME = "slimbootloader_patched.bin"

class CfgDataPage(TabPage):
    def __init__(self, parent, global_variables, global_flags):
        super().__init__(parent, tabName="CfgData", tabTitle="Patch Config Data")
        self.global_variables = global_variables
        self.global_flags = global_flags

        self.sbl_source_path = None
        self.cfg_data_obj = None
        self.org_cfg_data_bin = None
        self.loaded_dlt_file_full_path = None
        self.cnfg_page_list = {}
        self.cnfg_current_page = ""
        self.cnfg_widget_map = {}

        self.yaml_file_path = StringVar(value="<Empty>")
        yaml_frame = LabelFrame(
            parent=self,
            info_text="Combined YAML configuration file generated under the build's FV output "
            "directory (Build/BootloaderCorePkg/<TARGET>_<TOOLCHAIN>/FV/CfgDataDef.yaml).",
            label_text="Master YAML:",
            bind_variable=self.yaml_file_path)
        yaml_frame.add_button("Load YAML", self.load_yaml_file).config(width=BUTTON_WIDTH + 4)

        self.dlt_file_path = StringVar(value="<Empty>")
        dlt_frame = LabelFrame(
            parent=self,
            info_text="Delta configuration file (.dlt) capturing config value overrides.",
            label_text="Delta File:",
            bind_variable=self.dlt_file_path)
        # Buttons are added right-to-left so the visible order reads left-to-right as
        # Load Delta, Save Delta, Apply to Image.
        dlt_frame.add_button("Apply to Image", self.apply_to_image).config(width=BUTTON_WIDTH + 6)
        dlt_frame.add_button("Save Delta", self.save_dlt_file).config(width=BUTTON_WIDTH + 4)
        dlt_frame.add_button("Load Delta", self.load_dlt_file).config(width=BUTTON_WIDTH + 4)

        self.sbl_image_path = StringVar(value="<Empty>")
        FileDirectoryEntry(
            parent=self,
            info_text="SBL image file that the delta configuration will be stitched into.",
            label_text="SBL Image:",
            bind_variable=self.sbl_image_path,
            browse_type="file",
            filetypes=[("Binary files", "*.bin"), ("All files", "*.*")],
            callback=self.on_image_selected,
            browse_title_text="Select Image")

        self.output_file_name = StringVar(value=DEFAULT_OUTPUT_FILENAME)
        output_frame = CommonFrame(self)
        tk.Label(output_frame, text="Output File (-o):", font=FONT_CONTENT, fg="black",
                 bg=COMMON_BG, wraplength=150).pack(side="left", padx=10, pady=5)
        tk.Entry(output_frame, textvariable=self.output_file_name, font=FONT_CONTENT).pack(
            side="left", padx=10, pady=5, fill="x", expand=True)

        # Paned window: page tree (left) + config items (right)
        paned = ttk.Panedwindow(self, orient=tk.HORIZONTAL)
        paned.pack(fill=tk.BOTH, expand=True, padx=50, pady=5)

        frame_left = ttk.Frame(paned, relief="groove")
        self.cnfg_tree = ttk.Treeview(frame_left, show="tree")
        tree_scroll = ttk.Scrollbar(frame_left, orient="vertical", command=self.cnfg_tree.yview)
        self.cnfg_tree.configure(yscrollcommand=tree_scroll.set)
        self.cnfg_tree.bind("<<TreeviewSelect>>", self.on_cnfg_page_select)
        self.cnfg_tree.pack(side="left", fill=tk.BOTH, expand=True, padx=2, pady=2)
        tree_scroll.pack(side="right", fill=tk.Y, pady=2, padx=(0, 2))

        frame_right = ttk.Frame(paned, relief="groove")
        self.cnfg_canvas = tk.Canvas(frame_right, highlightthickness=0, bg="white")
        cnfg_scroll = ttk.Scrollbar(frame_right, orient="vertical", command=self.cnfg_canvas.yview)
        self.cnfg_grid = tk.Frame(self.cnfg_canvas, bg="white")
        self.cnfg_canvas.configure(yscrollcommand=cnfg_scroll.set)
        self.cnfg_canvas.pack(side="left", fill=tk.BOTH, expand=True, pady=2, padx=2)
        cnfg_scroll.pack(side="right", fill=tk.Y, pady=2, padx=(0, 2))
        self.cnfg_canvas.create_window(0, 0, window=self.cnfg_grid, anchor="nw")
        self.cnfg_canvas.bind(
            "<Configure>",
            lambda e: self.cnfg_grid.grid_columnconfigure(0, minsize=e.width))

        paned.add(frame_left, weight=2)
        paned.add(frame_right, weight=8)

        self.terminal_log = TerminalWindow(self, title="CfgData Terminal Log")
        self.execution_manager = ExecutionManager(self, self.terminal_log)

        status_frame = tk.Frame(self, bg="white")
        status_frame.pack(pady=5)
        Button(status_frame, text="Show Log", command=self.terminal_log.show).pack(side="left", padx=5)

        self.update()

    # NOTE: Any call to set() functions on any items in self.global_variables will be instantly updated
    # across all tabs without selecting any current tab since it is shared mutable variable.
    # update() function is only used for updating state of the selected tab that depends on latest value
    # of self.global_variables
    def update(self):
        source_path = self.global_variables["sbl_source_path"].get()
        if source_path and source_path != "<Empty>" and source_path != self.sbl_source_path:
            self.sbl_source_path = source_path

    def on_closing(self):
        print("CfgDataPage: on_closing called. Performing any cleanup if necessary...")
        if self.execution_manager.is_busy:
            print("Terminating ongoing processes in CfgDataPage...")
            self.execution_manager.terminate_process(app_exit=True)

    def on_image_selected(self, selected_path):
        self.output_file_name.set(self._default_output_filename(selected_path))

    @staticmethod
    def _default_output_filename(image_path):
        if not image_path or image_path == "<Empty>":
            return DEFAULT_OUTPUT_FILENAME
        name, ext = os.path.splitext(os.path.basename(image_path))
        return f"{name}_patched{ext or '.bin'}"

    # ── FV output discovery ─────────────────────────────────────────────────

    def _find_fv_dir(self):
        """Locate the most recently built FV output directory containing CfgDataStitch.py."""
        source_path = self.global_variables["sbl_source_path"].get()
        if source_path in ("", "<Empty>"):
            return None
        pattern = os.path.join(source_path, "Build", "BootloaderCorePkg", "*", "FV", "CfgDataStitch.py")
        candidates = glob.glob(pattern)
        if not candidates:
            return None
        candidates.sort(key=os.path.getmtime, reverse=True)
        return os.path.dirname(candidates[0])

    def _relative_to_source(self, path):
        source_path = self.global_variables["sbl_source_path"].get()
        if not path:
            return ""
        if source_path not in ("", "<Empty>") and os.path.isdir(source_path):
            try:
                rel_path = os.path.relpath(path, source_path)
                if not rel_path.startswith(".."):
                    return rel_path
            except Exception:
                pass
        return path

    # ── YAML / delta file I/O ─────────────────────────────────────────────────

    def load_yaml_file(self):
        fv_dir = self._find_fv_dir()
        initial_dir = fv_dir if fv_dir else self.global_variables["sbl_source_path"].get()
        if initial_dir in ("", "<Empty>") or not os.path.isdir(initial_dir):
            initial_dir = None

        selected_file = filedialog.askopenfilename(
            title="Select YAML Configuration File",
            filetypes=[("YAML files", "*.yaml *.yml"), ("All files", "*.*")],
            initialdir=initial_dir)
        if not selected_file:
            return

        if CGenCfgData is None:
            messagebox.showerror("Error", "GenCfgData module not available.\nCannot load configuration.")
            return
        try:
            self.cfg_data_obj = CGenCfgData()
            self.cfg_data_obj.load_yaml(selected_file)
            self.org_cfg_data_bin = self.cfg_data_obj.generate_binary_array()
            self.build_cnfg_tree()
            self.yaml_file_path.set(self._relative_to_source(selected_file))
            self.dlt_file_path.set("<Empty>")
            self.loaded_dlt_file_full_path = None
        except Exception as e:
            messagebox.showerror("YAML Loading Error", f"Failed to load YAML file:\n\n{e}")
            self.cfg_data_obj = None

    def load_dlt_file(self):
        if self.cfg_data_obj is None:
            messagebox.showwarning("Warning", "Please load a YAML file first before loading a delta file.")
            return

        fv_dir = self._find_fv_dir()
        initial_dir = fv_dir if fv_dir else self.global_variables["sbl_source_path"].get()
        if initial_dir in ("", "<Empty>") or not os.path.isdir(initial_dir):
            initial_dir = None

        selected_file = filedialog.askopenfilename(
            title="Select Delta Configuration File",
            filetypes=[("Delta files", "*.dlt"), ("All files", "*.*")],
            initialdir=initial_dir)
        if not selected_file:
            return
        try:
            self.cfg_data_obj.override_default_value(selected_file)
            self.loaded_dlt_file_full_path = selected_file
            self.dlt_file_path.set(self._relative_to_source(selected_file))
            self.refresh_cnfg_page()
        except Exception as e:
            messagebox.showerror("Error", f"Failed to load delta file:\n{e}")

    def save_dlt_file(self):
        if self.cfg_data_obj is None:
            messagebox.showwarning("Warning", "Please load a YAML file first before saving a delta file.")
            return
        if self.org_cfg_data_bin is None:
            messagebox.showerror("Error", "No original configuration data available.")
            return

        if self.loaded_dlt_file_full_path and os.path.isdir(os.path.dirname(self.loaded_dlt_file_full_path)):
            save_path = self.loaded_dlt_file_full_path
        else:
            fv_dir = self._find_fv_dir()
            initial_dir = fv_dir if fv_dir else self.global_variables["sbl_source_path"].get()
            if initial_dir in ("", "<Empty>") or not os.path.isdir(initial_dir):
                initial_dir = None
            save_path = filedialog.asksaveasfilename(
                title="Save Delta Configuration File",
                filetypes=[("Delta files", "*.dlt"), ("All files", "*.*")],
                defaultextension=".dlt",
                initialdir=initial_dir)

        if not save_path:
            return
        try:
            self.update_config_data_on_page()
            new_data = self.cfg_data_obj.generate_binary_array()
            self.cfg_data_obj.generate_delta_file_from_bin(save_path, self.org_cfg_data_bin, new_data, True)
            self.loaded_dlt_file_full_path = save_path
            self.dlt_file_path.set(self._relative_to_source(save_path))
            messagebox.showinfo("Success", f"Delta file saved successfully!\n\nFile: {save_path}")
        except Exception as e:
            messagebox.showerror("Error", f"Failed to save delta file:\n{e}")

    def apply_to_image(self):
        if not self.loaded_dlt_file_full_path:
            messagebox.showwarning("Warning", "Please save a delta file first before applying to image.")
            return

        source_path = self.global_variables["sbl_source_path"].get()
        if source_path in ("", "<Empty>"):
            messagebox.showwarning("Warning", "Please select a source code path first.")
            return

        fv_dir = self._find_fv_dir()
        if not fv_dir:
            messagebox.showwarning(
                "Warning",
                "Could not find a built FV output directory with CfgDataStitch.py.\n"
                "Please build the project first.")
            return

        script_dir = os.path.join(source_path, "BootloaderCorePkg", "Tools")
        if not os.path.isdir(script_dir):
            messagebox.showwarning("Warning", f"Scripts directory not found:\n{script_dir}")
            return

        keys_path = self.global_variables["sbl_key_path"].get()
        if keys_path in ("", "<Empty>"):
            messagebox.showwarning("Warning", "Please set the SBL Keys Directory in the Setup tab first.")
            return
        key_file = os.path.join(keys_path, DEFAULT_CFGDATA_KEY_FILENAME)
        if not os.path.isfile(key_file):
            messagebox.showwarning("Warning", f"Config data signing key file not found:\n{key_file}")
            return

        image_file = self.sbl_image_path.get()
        if image_file in ("", "<Empty>") or not os.path.isfile(image_file):
            messagebox.showwarning("Warning", "Please select a valid SBL image first.")
            return

        output_name = self.output_file_name.get().strip() or self._default_output_filename(image_file)
        output_file = os.path.join(os.path.dirname(image_file), output_name)
        stitch_script = os.path.join(fv_dir, "CfgDataStitch.py")

        command_parts = [
            get_python_cmd(),
            self._relative_to_source(stitch_script),
            "-i", self._relative_to_source(image_file),
            "-k", self._relative_to_source(key_file),
            "-s", self._relative_to_source(script_dir),
            "-c", self._relative_to_source(fv_dir),
            "-o", self._relative_to_source(output_file),
        ]
        command = " ".join(command_parts)

        if not self.execution_manager.is_busy:
            self.execution_manager.status_bar.set(f"Running {command}...", color="black")
        self.execution_manager.execute_command(command, cwd=source_path)

    # ── Config tree / widget logic ─────────────────────────────────────────────

    def refresh_cnfg_page(self):
        if self.cfg_data_obj is None:
            return
        self.build_cnfg_tree()
        if self.cnfg_current_page:
            self.build_cnfg_page_items(self.cnfg_current_page)

    def build_cnfg_tree(self):
        self.cnfg_tree.delete(*self.cnfg_tree.get_children())
        self.cnfg_page_list.clear()
        if self.cfg_data_obj is None:
            return
        cfg_page = self.cfg_data_obj.get_cfg_page()
        self.build_cnfg_page_tree_recursive(cfg_page["root"], "")

    def build_cnfg_page_tree_recursive(self, cfg_page, parent):
        for page in cfg_page["child"]:
            page_id = next(iter(page))
            self.cnfg_page_list[page_id] = self.cfg_data_obj.get_cfg_list(page_id)
            self.cnfg_page_list[page_id].sort(key=lambda x: x["order"])
            page_name = self.cfg_data_obj.get_page_title(page_id)
            child = self.cnfg_tree.insert(parent, "end", iid=page_id, text=page_name)
            if page[page_id].get("child"):
                self.build_cnfg_page_tree_recursive(page[page_id], child)

    def on_cnfg_page_select(self, event):
        sel = self.cnfg_tree.selection()
        if sel:
            page_id = sel[0]
            self.cnfg_current_page = page_id
            self.build_cnfg_page_items(page_id)
            self.cnfg_canvas.update()
            self.cnfg_canvas.config(scrollregion=self.cnfg_canvas.bbox("all"))
            self.cnfg_canvas.yview_moveto(0)

    def clear_cnfg_widgets(self):
        for widget in self.cnfg_grid.winfo_children():
            widget.destroy()
        self.cnfg_widget_map.clear()

    def build_cnfg_page_items(self, page_id):
        self.clear_cnfg_widgets()
        if page_id not in self.cnfg_page_list:
            return
        items = sorted(self.cnfg_page_list[page_id], key=lambda x: x["order"])
        for row, item in enumerate(items):
            self.add_cnfg_item(item, row * 2)

    def add_cnfg_item(self, item, row):
        parent = self.cnfg_grid
        tk.Label(parent, text=item["name"], anchor="w", font=("Arial", 10), bg="white").grid(
            row=row, column=0, padx=10, pady=2, sticky="w")

        parts = item["type"].split(",")
        itype = parts[0].strip()
        widget = None

        if itype == "Combo":
            opt_list = self.cfg_data_obj.get_cfg_item_options(item)
            current_value = self.cfg_data_obj.get_cfg_item_value(item, False)
            option_list = []
            current = 0
            for idx, option in enumerate(opt_list):
                try:
                    oval = self.cfg_data_obj.get_value(option[0], len(option[0]), False)
                except Exception:
                    oval = 0
                if oval == current_value:
                    current = idx
                option_list.append(option[1])
            widget = ttk.Combobox(parent, values=option_list, state="readonly", width=40)
            widget.current(current)
            widget.bind("<<ComboboxSelected>>", lambda e, w=widget, i=item: self.on_widget_change(w, i))

        elif itype in ("EditNum", "EditText"):
            widget = tk.Entry(parent, width=40)
            value = (item["value"].strip()
                     if itype == "EditText"
                     else item["value"].strip("'").strip("{").strip("}").strip())
            try:
                item_length = self.cfg_data_obj.get_cfg_item_length(item)
                if "{" in item["value"] and "}" in item["value"]:
                    byte_values = [v.strip() for v in value.split(",") if v.strip()]
                    if len(byte_values) > item_length:
                        value = ", ".join(byte_values[:item_length])
            except Exception:
                pass
            widget.insert(0, value)
            widget.bind("<FocusOut>", lambda e, w=widget, i=item: self.on_widget_change(w, i))
            widget.bind("<Return>", lambda e, w=widget, i=item: self.on_widget_change(w, i))

        elif itype == "Table":
            widget = tk.Label(parent, text="[Table - use ConfigEditor to edit]",
                              anchor="w", font=("Arial", 9, "italic"), fg="gray", bg="white")
        elif itype and itype not in ("Reserved", "Constant"):
            widget = tk.Label(parent, text=f"[{itype}]", anchor="w",
                              font=("Arial", 9, "italic"), fg="gray", bg="white")

        if widget:
            widget.grid(row=row + 1, column=0, padx=10, pady=2, sticky="ew")
            self.cnfg_widget_map[id(widget)] = item["path"]
            if item.get("help"):
                tk.Label(parent, text=item["help"], anchor="w",
                         font=("Arial", 8), fg="#555", wraplength=500,
                         justify="left", bg="white").grid(
                    row=row + 1, column=1, padx=10, pady=2, sticky="w")

    def on_widget_change(self, widget, item):
        if self.cfg_data_obj is None:
            return
        try:
            old_value = item["value"]
            self.update_config_item_from_widget(widget, item)
            new_value = item["value"]
            if old_value != new_value:
                print(f"Config changed: '{item['path']}': '{old_value}' -> '{new_value}'")
        except Exception as e:
            print(f"Error updating config item '{item.get('path', 'unknown')}': {e}")

    def update_config_data_on_page(self):
        if self.cfg_data_obj is None:
            return
        for widget in self.cnfg_grid.winfo_children():
            widget_id = id(widget)
            if widget_id in self.cnfg_widget_map:
                path = self.cnfg_widget_map[widget_id]
                item = self.cfg_data_obj.get_item_by_path(path)
                if item:
                    self.update_config_item_from_widget(widget, item)

    def update_config_item_from_widget(self, widget, item):
        itype = item["type"].split(",")[0].strip()
        try:
            if itype == "Combo" and isinstance(widget, ttk.Combobox):
                opt_list = self.cfg_data_obj.get_cfg_item_options(item)
                idx = widget.current()
                if 0 <= idx < len(opt_list):
                    new_value = opt_list[idx][0]
                    if item["value"] != new_value:
                        item["value"] = new_value

            elif itype in ("EditNum", "EditText") and isinstance(widget, tk.Entry):
                value_str = widget.get()
                if value_str == "":
                    new_value = "''" if itype == "EditText" else "0"
                    if item["value"] != new_value:
                        item["value"] = new_value
                else:
                    try:
                        new_value = self.cfg_data_obj.reformat_value_str(
                            value_str,
                            self.cfg_data_obj.get_cfg_item_length(item),
                            item["value"])
                        if item["value"] != new_value:
                            item["value"] = new_value
                    except Exception as e:
                        print(f"Warning: Failed to format value '{value_str}' "
                              f"for '{item['path']}': {e}")
        except Exception as e:
            print(f"Error updating config item from widget: {e}")
