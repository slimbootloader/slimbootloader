## @ SblBuilder03_FwUpdatePage.py
#
# Copyright (c) 2026, Intel Corporation. All rights reserved.<BR>
# SPDX-License-Identifier: BSD-2-Clause-Patent
#
##

import os
import tkinter as tk
from tkinter import ttk, StringVar, messagebox
from SblBuilderGuiComp import *

# Only the SBL/BIOS region payload type is supported by this page.
PAYLOAD_TYPE = "BIOS"
HASH_TYPES = ["AUTO", "SHA2_256", "SHA2_384"]
SIGN_SCHEMES = ["RSA_PSS", "RSA_PKCS1"]
DEFAULT_KEY_FILENAME = "FirmwareUpdateTestKey_Priv_RSA3072.pem"
DEFAULT_OUTPUT_FILENAME = "FwuImage.bin"

class FwUpdatePage(TabPage):
    def __init__(self, parent, global_variables, global_flags):
        super().__init__(parent, tabName="FwUpdate", tabTitle="Firmware Update Generation")
        self.global_variables = global_variables
        self.global_flags = global_flags

        self.sbl_source_path = None

        LabelFrame(
            parent=self,
            info_text="Directory for slimbootloader repo will be cloned, updated, and cleaned.",
            label_text="SBL Source Code:",
            bind_variable=self.global_variables["sbl_source_path"])

        self.payload_type = StringVar(value=PAYLOAD_TYPE)
        LabelFrame(
            parent=self,
            info_text="Component GUID targeted by this firmware update payload.",
            label_text="Payload Type (-p):",
            bind_variable=self.payload_type)

        self.payload_path = StringVar(value="<Empty>")
        FileDirectoryEntry(
            parent=self,
            info_text="Binary image file to be signed and wrapped into the firmware update capsule.",
            label_text="Payload File:",
            bind_variable=self.payload_path,
            browse_type="file",
            filetypes=[("Binary files", "*.bin"), ("All files", "*.*")],
            callback=self.update_command_preview,
            browse_title_text="Select FWUpdate Payload")

        self.key_path = StringVar(value="<Empty>")
        FileDirectoryEntry(
            parent=self,
            info_text="Private RSA key (PEM) used to sign the firmware update capsule.",
            label_text="Key File (-k):",
            bind_variable=self.key_path,
            browse_type="file",
            filetypes=[("PEM files", "*.pem"), ("All files", "*.*")],
            callback=self.update_command_preview,
            browse_title_text="Select FWUpdate Key")

        self.output_name = StringVar(value=DEFAULT_OUTPUT_FILENAME)
        output_frame = CommonFrame(self)
        tk.Label(output_frame, text="Output File (-o):", font=FONT_CONTENT, fg="black",
                 bg=COMMON_BG, wraplength=150).pack(side="left", padx=10, pady=5)
        tk.Entry(output_frame, textvariable=self.output_name, font=FONT_CONTENT).pack(
            side="left", padx=10, pady=5, fill="x", expand=True)
        self.output_name.trace_add("write", self.update_command_preview)

        options_frame = CommonFrame(self)
        tk.Label(options_frame, text="Signing Options", font=("Arial", 11, "bold"), fg="black",
                 bg=COMMON_BG, justify="left").pack(anchor="w", padx=10, pady=(10, 0))
        self.hash_type = RadioButtonGroup(
            parent=options_frame,
            label_text="Hash Type (-a):",
            options=HASH_TYPES,
            default_value=0,
            callback=self.update_command_preview)
        self.sign_scheme = RadioButtonGroup(
            parent=options_frame,
            label_text="Sign Scheme (-s):",
            options=SIGN_SCHEMES,
            default_value=0,
            callback=self.update_command_preview)

        flags_frame = CommonFrame(self)
        self.force_bios_update = tk.BooleanVar(value=False)
        self.skip_all = tk.BooleanVar(value=False)
        tk.Checkbutton(flags_frame, text="Force BIOS Update (-f)", variable=self.force_bios_update,
                       command=self.update_command_preview, bg=COMMON_BG).pack(side="left", padx=10)
        tk.Checkbutton(flags_frame, text="Skip All (-sa)", variable=self.skip_all,
                       command=self.update_command_preview, bg=COMMON_BG).pack(side="left", padx=10)

        self.command_preview = StringVar(value="")
        LabelFrame(
            parent=self,
            info_text="Preview of the FWUpdate command that will be executed.",
            label_text="Command:",
            bind_variable=self.command_preview)

        self.terminal_log = TerminalWindow(self, title="FWUpdate Terminal Log")
        self.execution_manager = ExecutionManager(self, self.terminal_log)

        button_frame = tk.Frame(self, bg="white")
        button_frame.pack(pady=5)
        Button(button_frame, text="Run FWUpdate", command=self.run_fwupdate).pack(side="left", padx=5)
        Button(button_frame, text="Terminate", width=10, command=self.terminate_fwupdate).pack(side="left", padx=5)
        Button(button_frame, text="Show Log", command=self.terminal_log.show).pack(side="left", padx=5)

        self.update()

    # NOTE: Any call to set() functions on any items in self.global_variables will be instantly updated
    # across all tabs without selecting any current tab since it is shared mutable variable.
    # update() function is only used for updating state of the selected tab that depends on latest value
    # of self.global_variables
    def update(self):
        source_path = self.global_variables["sbl_source_path"].get()
        if source_path and source_path != "<Empty>" and source_path != self.sbl_source_path:
            self.sbl_source_path = source_path
            if self.payload_path.get() in ("", "<Empty>"):
                self.payload_path.set(self._default_payload_path())
            if self.key_path.get() in ("", "<Empty>"):
                self.key_path.set(self._default_key_path())
        self.update_command_preview()

    def on_closing(self):
        print("FwUpdatePage: on_closing called. Performing any cleanup if necessary...")
        if self.execution_manager.is_busy:
            print("Terminating ongoing processes in FwUpdatePage...")
            self.execution_manager.terminate_process(app_exit=True)

    def _default_payload_path(self):
        source_path = self.global_variables["sbl_source_path"].get()
        platform = self.global_variables["build_platform"].get()
        if source_path in ("", "<Empty>") or platform in ("", "<Empty>"):
            return ""
        return os.path.join(source_path, "Outputs", platform, "SlimBootloader.bin")

    def _default_key_path(self):
        keys_path = self.global_variables["sbl_key_path"].get()
        if keys_path in ("", "<Empty>"):
            return ""
        return os.path.join(keys_path, DEFAULT_KEY_FILENAME)

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

    def update_command_preview(self, *args):
        source_path = self.global_variables["sbl_source_path"].get()
        payload_path = self.payload_path.get()
        key_path = self.key_path.get()
        if (source_path in ("", "<Empty>") or payload_path in ("", "<Empty>")
                or key_path in ("", "<Empty>")):
            self.command_preview.set("")
            return

        output_name = self.output_name.get().strip() or DEFAULT_OUTPUT_FILENAME
        script_relpath = os.path.join("BootloaderCorePkg", "Tools", "GenCapsuleFirmware.py")

        command = f"{get_python_cmd()} {script_relpath}"
        command += f" -p {self.payload_type.get()} {self._relative_to_source(payload_path)}"
        command += f" -k {self._relative_to_source(key_path)}"
        command += f" -o {output_name}"
        if self.hash_type.get_option() and self.hash_type.get_option() != "AUTO":
            command += f" -a {self.hash_type.get_option()}"
        if self.sign_scheme.get_option() and self.sign_scheme.get_option() != "RSA_PSS":
            command += f" -s {self.sign_scheme.get_option()}"
        if self.force_bios_update.get():
            command += " -f"
        if self.skip_all.get():
            command += " -sa"
        self.command_preview.set(command)

    def run_fwupdate(self):
        source_path = self.global_variables["sbl_source_path"].get()
        payload_path = self.payload_path.get()
        key_path = self.key_path.get()
        script_fullpath = os.path.join(source_path, "BootloaderCorePkg", "Tools", "GenCapsuleFirmware.py")

        if source_path in ("", "<Empty>"):
            messagebox.showwarning("Warning", "Please select a source code path first.")
            return
        if not os.path.isfile(script_fullpath):
            messagebox.showwarning("Warning", f"GenCapsuleFirmware.py not found at:\n{script_fullpath}")
            return
        if not payload_path or payload_path == "<Empty>" or not os.path.isfile(payload_path):
            messagebox.showwarning("Warning", "Please select a valid payload file first.")
            return
        if not key_path or key_path == "<Empty>" or not os.path.isfile(key_path):
            messagebox.showwarning(
                "Warning", "Firmware update key file not found. Please select a valid key file first.")
            return
        if self.force_bios_update.get() and self.skip_all.get():
            messagebox.showwarning(
                "Warning", "'Skip All' (-sa) cannot be used together with 'Force BIOS Update' (-f).")
            return

        command = self.command_preview.get()
        if not command:
            messagebox.showwarning("Warning", "Command is empty. Please check your FWUpdate configuration.")
            return

        if not self.execution_manager.is_busy:
            self.execution_manager.status_bar.set(f"Running {command}...", color="black")
        self.execution_manager.execute_command(command, cwd=source_path)

    def terminate_fwupdate(self):
        self.execution_manager.terminate_process()
