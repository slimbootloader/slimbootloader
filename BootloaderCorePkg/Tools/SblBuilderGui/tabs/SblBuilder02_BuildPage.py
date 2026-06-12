## @ SblBuilder02_BuildPage.py
#
# Copyright (c) 2026, Intel Corporation. All rights reserved.<BR>
# SPDX-License-Identifier: BSD-2-Clause-Patent
#
##

import os, glob, sys
from importlib.machinery import SourceFileLoader
import tkinter as tk
from tkinter import ttk, filedialog
from SblBuilderGuiComp import *
import re
from functools import partial
import inspect

class BuildPage(TabPage):
    def __init__(self, parent, global_variables, global_flags):
        super().__init__(parent, tabName="Build", tabTitle="Build SBL")
        self.global_variables = global_variables
        self.global_flags = global_flags

        self.sbl_source_path = None
        self.board_name_list = []
        self.board_config_list = {}

        LabelFrame(
            parent=self,
            info_text="Directory for slimbootloader repo will be cloned, updated, and cleaned.",
            label_text="SBL Source Code:",
            bind_variable=self.global_variables["sbl_source_path"])

        self.platform = self.global_variables["build_platform"]
        platform_frame = LabelFrame(
            parent=self,
            info_text="Select the platform you want to build.",
            label_text="Platform:",
            bind_variable=self.platform)
        self.platform_listbox = platform_frame.add_listbox(
            options=["<Empty>"],
            command=self.on_platform_selected)

        self.board_config = StringVar(value="<Empty>")
        self.board_obj = None
        self.board_attr = {}

        LabelFrame(
            parent=self,
            info_text="Board configuration file based on platform selected.",
            label_text="Board Config:",
            bind_variable=self.board_config)

        self.uefi_payload = StringVar(value="N/A")
        self.selected_uefi_payload = None
        self.uefi_payload_frame = LabelFrame(
            parent=self,
            info_text="UEFI payload file to be used for build. \n" \
            "Note that Payload Type option set to 'UEFI' will be ignored if 'N/A' is selected.\n" \
            "Click Refresh to update the list of .fd and .efi files in Binaries folder.",
            label_text="UEFI Payload:",
            bind_variable=self.uefi_payload,
            has_warning=True)
        self.uefi_payload_listbox = self.uefi_payload_frame.add_listbox(
            options=["N/A"],
            command=self.on_uefi_payload_selected)
        self.uefi_payload_frame.add_button("Refresh", self.refresh_uefi_payload_list)

        # This frame has to be custom made specifically for build configurations
        sbl_config_frame = CommonFrame(self)
        tk.Label(sbl_config_frame, text="Build Configurations", font=("Arial", 11, "bold"), fg="black", bg=COMMON_BG, justify="left").pack(anchor="w", padx=10, pady=(10, 0))
        self.build_mode = RadioButtonGroup(
            parent=sbl_config_frame,
            label_text="Build Mode:",
            options=["Debug", "Release"],
            default_value=0,
            callback=self.update_command_preview
        )
        self.fsp_build_type = RadioButtonGroup(
            parent=sbl_config_frame,
            label_text="FSP Build Type:",
            options=["Debug", "Release"],
            default_value=1,
            callback=self.update_command_preview
        )
        self.payload_type = RadioButtonGroup(
            parent=sbl_config_frame,
            label_text="Payload Type:",
            options=["OS Loader", "UEFI"],
            default_value=0,
            callback=partial(self.modify_file, "payload_type")
        )
        self.fast_boot = RadioButtonGroup(
            parent=sbl_config_frame,
            label_text="Fast Boot:",
            options=["Disabled", "Enabled"],
            default_value=0,
            callback=partial(self.modify_file, "fast_boot")
        )

        self.command_preview = StringVar(value="")
        LabelFrame(
            parent=self,
            info_text="Preview of the build command that will be executed.",
            label_text="Command:",
            bind_variable=self.command_preview)

        self.terminal_log = TerminalWindow(self, title="Build SBL Terminal Log")
        self.execution_manager = ExecutionManager(self, self.terminal_log)

        # Create a frame for buttons and center it
        button_frame = tk.Frame(self, bg="white")
        button_frame.pack(pady=5)

        # Pack buttons side by side in the frame
        Button(button_frame, text="Build", command=self.run_build).pack(side="left", padx=5)
        Button(button_frame, text="Terminate", width=10, command=self.terminate_build).pack(side="left", padx=5)
        sblbuild_clean_button = Button(button_frame, text="SblBuild Clean", width=12, command=self.clean_build)
        sblbuild_clean_button.pack(side="left", padx=5)
        Tooltip(sblbuild_clean_button, text="Clean the SBL build environment.")
        git_clean_button = Button(button_frame, text="Git Reset & Clean", width=15, command=self.git_clean)
        git_clean_button.pack(side="left", padx=5)
        Tooltip(git_clean_button, text="Reset and clean the SBL source code repository.")
        Button(button_frame, text="Show Log", command=self.terminal_log.show).pack(side="left", padx=5)

        self.update()

    # NOTE: Any call to set() functions on any items in self.global_variables will be instantly updated
    # across all tabs without selecting any current tab since it is shared mutable variable.
    # update() function is only used for updating state of the selected tab that depends on latest value
    # of self.global_variables
    def update(self):
        # On first load, populate platform list if sbl_source_path is already set
        if self.sbl_source_path is None:
            saved_path = self.global_variables["sbl_source_path"].get()
            if saved_path and saved_path != "<Empty>" and os.path.isdir(saved_path):
                print("Initial load: SBL source path already set. Refreshing platform list...")
                self.sbl_source_path = saved_path
                self.find_all_platforms()

        # Only update platform list when SBL source path is changed
        if self.global_flags.get("sbl_path_update_from_setup", False):
            self.global_flags["sbl_path_update_from_setup"] = False
            print("SBL source code path updated. Refreshing platform list...")
            self.sbl_source_path = self.global_variables["sbl_source_path"].get()
            if os.path.isdir(self.sbl_source_path):
                self.find_all_platforms()
            else:
                print(f"Invalid SBL source path: {self.sbl_source_path}")
                self.clear_platform_list()
            self.reset_build_configurations()

        # Reset build configurations to default if git clean is triggered from setup page
        if self.global_flags.get("git_clean_from_setup", False):
            print("Git clean from setup tab detected. Resetting build configurations to default...")
            self.reset_build_configurations()
            self.global_flags["git_clean_from_setup"] = False

    def on_closing(self):
        print("BuildPage: on_closing called. Performing any cleanup if necessary...")
        if self.execution_manager.is_busy:
            print("Terminating ongoing processes in BuildPage...")
            self.execution_manager.terminate_process(app_exit=True)

    def clear_platform_list(self):
        self.platform_listbox.config(values=["<Empty>"])
        self.platform_listbox.set("<Empty>")
        self.platform.set("<Empty>")
        self.board_config_list = {}
        self.update_platform_info()
        self.update_command_preview()
        print("No selected project.")

    def reset_build_configurations(self):
        for radio_button_group in [self.payload_type, self.fast_boot]:
            radio_button_group.set_idx(-1)
        self.uefi_payload_frame.clear_warning()

    def on_platform_selected(self, event=None):
        selected_platform = self.platform_listbox.get()
        if selected_platform != self.platform.get():
            self.platform.set(selected_platform)
            print(f"Selected platform: {selected_platform}")
            self.update_platform_info()
            # Reset certain build configuration options to default when platform is changed
            self.reset_build_configurations()
            self.update_command_preview()

    def update_platform_info(self):
        def parse_board_attr():
            self.board_attr = {}
            main_board_config = sys.modules[self.board_obj.__class__.__module__].__file__
            self.board_attr["plt_pkg_dir"] = os.path.dirname(main_board_config)

            # Some Board class in BoardConfig*.py may be inherited from other BoardConfig*.py.
            # This is to determine the file path of all the parent classes.
            self.board_attr["board_cfg_files"] = [os.path.abspath(main_board_config)]
            for cls in self.board_obj.__class__.__mro__[1:]:
                if cls.__module__ == "BuildLoader":
                    break
                self.board_attr["board_cfg_files"].append(os.path.abspath(inspect.getfile(cls)))

            platform_pkg_name = os.path.basename(self.board_attr["plt_pkg_dir"])
            platform_pkg_name_alt = getattr(self.board_obj, "BOARD_PKG_NAME_OVERRIDE", None)

            if platform_pkg_name_alt and platform_pkg_name_alt != platform_pkg_name:
                alt_platform_dir = os.path.join(self.sbl_source_path, "Platform", platform_pkg_name_alt)
                if os.path.isdir(alt_platform_dir):
                    self.board_attr["alt_plt_pkg_dir"] = alt_platform_dir

            dlt_files = []
            for cfg_data_attr in ["_CFGDATA_EXT_FILE", "_CFGDATA_INT_FILE"]:
                dlt_files.extend([dlt_file.replace('Autogen_', '') for dlt_file in getattr(self.board_obj, cfg_data_attr, [])])
            self.board_attr["dlt_files"] = []
            for dlt_file in dlt_files:
                dlt_file_path = os.path.join(self.board_attr["plt_pkg_dir"], "CfgData", dlt_file)
                if os.path.isfile(dlt_file_path):
                    self.board_attr["dlt_files"].append(dlt_file_path)
                elif "alt_plt_pkg_dir" in self.board_attr:
                    alt_dlt_file_path = os.path.join(self.board_attr["alt_plt_pkg_dir"], "CfgData", dlt_file)
                    if os.path.isfile(alt_dlt_file_path):
                        self.board_attr["dlt_files"].append(alt_dlt_file_path)
                else:
                    print(f"Warning: {dlt_file} not found for platform {self.platform.get()}")

        selected_platform = self.platform_listbox.get()
        if selected_platform in self.board_config_list:
            self.board_config.set(os.path.relpath(self.board_config_list[selected_platform][0], self.sbl_source_path))
            self.board_obj = self.board_config_list[selected_platform][1]
            parse_board_attr()
            for key, value in self.board_attr.items():
                if isinstance(value, list):
                    print(f"  {key}:")
                    for item in value:
                        print(f"    {os.path.relpath(item, self.sbl_source_path)}")
                else:
                    print(f"  {key}: {value}")
            self.refresh_uefi_payload_list()
        else:
            self.board_config.set("<Empty>")
            self.board_obj = None
            self.board_attr = {}
            self.uefi_payload_listbox.config(values=["N/A"])
            self.uefi_payload_listbox.set("N/A")
            self.uefi_payload.set("N/A")

    def find_all_platforms(self):
        """Find all platform names by searching BoardConfig*.py files in Platform folders."""
        try:
            self.board_config_list = {}

            # Temporary add Slimbootloader to allow importing BuildLoader.py to load BoardConfig*.py modules
            append_path = os.path.join(self.sbl_source_path)
            glob_pattern = os.path.join(self.sbl_source_path, 'Platform', '**', 'BoardConfig*.py')
            sys.path.append(append_path)
            for board_cfg_file in glob.glob(glob_pattern):
                try:
                    module_name = os.path.basename(os.path.dirname(board_cfg_file))[:-8] + os.path.basename(board_cfg_file)[:-3]
                    module = SourceFileLoader(module_name, board_cfg_file).load_module()
                    board_obj = module.Board()
                    if board_obj.BOARD_NAME != "":
                        board_cfg_file = board_cfg_file.replace('\\', '/')
                        self.board_config_list[board_obj.BOARD_NAME] = (board_cfg_file, board_obj)
                except Exception as e:
                    print(f"Error loading {board_cfg_file}: {e}")
            sys.path.remove(append_path)

            # Sort board names alphabetically
            board_names = list(self.board_config_list.keys())
            board_names.sort()
            if len(board_names) > 0:
                print(f"Found {len(board_names)} boards: {board_names}")
                self.platform_listbox.config(values=board_names)
                if self.platform.get() in board_names:
                    self.platform_listbox.set(self.platform.get())
                else:
                    self.platform_listbox.set(board_names[0])
                    self.platform.set(board_names[0])
                print(f"Selected platform set to: {self.platform_listbox.get()}")
                self.update_platform_info()
                self.update_command_preview()
            else:
                self.clear_platform_list()

        except Exception as e:
            print(f"Error finding boards: {e}")
            self.clear_platform_list()

    def on_uefi_payload_selected(self, event=None):
        self.selected_uefi_payload = self.uefi_payload_listbox.get()
        self.uefi_payload.set(self.selected_uefi_payload)
        print(f"Selected UEFI payload: {self.selected_uefi_payload}")
        self.update_command_preview()

    def refresh_uefi_payload_list(self):
        payload_list = ["N/A"]
        if self.board_attr and "plt_pkg_dir" in self.board_attr:
            payload_dir = os.path.join(self.board_attr["plt_pkg_dir"], "Binaries")
            if os.path.isdir(payload_dir):
                for file in os.listdir(payload_dir):
                    if file.lower().endswith(('.efi', '.fd')):
                        payload_list.append(file)
            print(f"UEFI payload list refreshed in {payload_dir}: {payload_list}")
        self.uefi_payload_listbox.config(values=payload_list)
        self.uefi_payload_listbox.set(payload_list[1] if len(payload_list) > 1 else payload_list[0])
        self.on_uefi_payload_selected()

    def modify_file(self, attribute, new_value):
        def modify_dlt_file(attr_name, attr_value):
            for dlt_file in self.board_attr.get("dlt_files", []):
                print(f"Modifying {os.path.relpath(dlt_file, self.sbl_source_path)}")
                with open(dlt_file) as f:
                    file_content = f.read()

                rgex = rf"(\s*{attr_name}\s*\|)(.*)"
                rgex_value = rf"\1 {attr_value}"
                print(f"  Updating/adding {attr_name} -> {attr_value}")
                rgex_found = re.search(rgex, file_content)
                if not rgex_found:
                    new_line = f"\n{attr_name} | {attr_value}\n"
                    file_content += new_line
                    continue
                file_content = re.sub(rgex, rgex_value, file_content)

                with open(dlt_file, "w") as f:
                    f.write(file_content)

        def modify_board_cfg_file(attr_name, attr_value):
            for board_config_file in self.board_attr.get("board_cfg_files", []):
                print(f"Modifying {os.path.relpath(board_config_file, self.sbl_source_path)}")
                with open(board_config_file) as f:
                    file_content = f.read()

                rgex = rf"(\s*{attr_name}\s*=)(.*)"
                rgex_value = rf"\1 {attr_value}"
                print(f"  Updating {attr_name} -> {attr_value}")
                if re.search(rgex, file_content):
                    file_content = re.sub(rgex, rgex_value, file_content)

                    with open(board_config_file, "w") as f:
                        f.write(file_content)
                    # modify the first board config file that contains the attribute
                    break

        if self.board_obj is None:
            return
        if attribute == "payload_type":
            attr_value = "'UEFI'" if new_value == "UEFI" else "''"
            modify_dlt_file("GEN_CFG_DATA.PayloadId", attr_value)
            self.update_command_preview()
        elif attribute == "fast_boot":
            attr_value = "1" if new_value == "Enabled" else "0"
            modify_board_cfg_file("self.ENABLE_FAST_BOOT", attr_value)

    def update_command_preview(self, selected_option=None):
        if self.platform.get() == "<Empty>":
            self.command_preview.set("")
            return
        command  = f"{get_python_cmd()} BuildLoader.py build {self.platform.get()}"
        if self.build_mode.get_option() == "Release":
            command += " -r"
        if self.fsp_build_type.get_option() == "Debug":
            command += " -fd"
        if self.payload_type.get_option() == "UEFI":
            if self.selected_uefi_payload != "N/A":
                self.uefi_payload_frame.clear_warning()
                command += f" -p OsLoader.efi:LLDR:Lz4;{self.selected_uefi_payload}:UEFI:Lzma"
            else:
                self.uefi_payload_frame.set_warning("UEFI payload is selected as payload ID, but no UEFI payload binary is selected.")
        else:
            self.uefi_payload_frame.clear_warning()
        self.command_preview.set(command)

    def run_build(self):
        if self.platform.get() == "<Empty>":
            print("No platform selected. Cannot build.")
            return
        os.environ["SBL_KEY_DIR"] = self.global_variables["sbl_key_path"].get()
        command = self.command_preview.get()
        if not self.execution_manager.is_busy:
            self.execution_manager.status_bar.set(f"Running {command}...", color="black")
        self.execution_manager.execute_command(command, cwd=self.sbl_source_path)

    def terminate_build(self):
        self.execution_manager.terminate_process()

    def clean_build(self):
        if not os.path.isfile(os.path.join(self.sbl_source_path, "BuildLoader.py")):
            print("BuildLoader.py not found in SBL source path. Cannot clean build.")
            return
        command = f"{get_python_cmd()} BuildLoader.py clean"
        if not self.execution_manager.is_busy:
            self.execution_manager.status_bar.set(f"Running {command}...", color="black")
        self.execution_manager.execute_command(command, cwd=self.sbl_source_path)

    def git_clean(self):
        if not os.path.isdir(os.path.join(self.sbl_source_path, ".git")):
            print("Not a git repository. Cannot perform git clean.")
            return
        command = f"git reset --hard && git clean -fd"
        if not self.execution_manager.is_busy:
            self.execution_manager.status_bar.set(f"Running {command}...", color="black")
        self.execution_manager.execute_command(command, cwd=self.sbl_source_path, shell=True)
        self.reset_build_configurations()
