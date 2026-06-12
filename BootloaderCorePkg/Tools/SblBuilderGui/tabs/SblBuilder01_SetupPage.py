## @ SblBuilder01_SetupPage.py
#
# Copyright (c) 2026, Intel Corporation. All rights reserved.<BR>
# SPDX-License-Identifier: BSD-2-Clause-Patent
#
##

import tkinter as tk
from tkinter import ttk, filedialog
from tkinter import messagebox
from SblBuilderGuiComp import *
import threading
import os, glob
import sys, shutil, subprocess
import re

SETTING_JSON_FILENAME = "sblbuilder.setting.json"
SBL_REPO_URL = "https://github.com/slimbootloader/slimbootloader.git"

class SetupPage(TabPage):
    def __init__(self, parent, global_variables, global_flags):
        super().__init__(parent, tabName="Setup", tabTitle="Setup Workspace")
        self.global_variables = global_variables
        self.global_flags = global_flags
        self.root = self.winfo_toplevel()

        # Workspace directory
        FileDirectoryEntry(
            parent=self,
            info_text="Select the workspace directory where SBL source code will be stored.",
            label_text="Workspace:",
            bind_variable=self.global_variables["workspace_path"],
            callback=self.on_workspace_path_selected,
            browse_type="folder",
            browse_title_text="Select Workspace Directory")

        # SBL source code directory
        sbl_source_dir = LabelFrame(
            parent=self,
            info_text="Directory for slimbootloader repo will be cloned, updated, and cleaned.\n" \
                "Clean - All untracked and tracked files, except for ignored files/directories will be reset to last commit.\n"\
                "Update - Pull latest changes from remote repository.\n" \
                "Clone - Clone slimbootloader repo from remote repository to the specified directory.",
            label_text="SBL Source Code:",
            bind_variable=self.global_variables["sbl_source_path"])
        self.clone_update_button = sbl_source_dir.add_button("Clone", self.clone_update_repo)
        sbl_source_dir.add_button("Clean", self.clean_repo)

        # SBL key directory
        sbl_key_dir = FileDirectoryEntry(
            parent=self,
            info_text="Select the directory where your SBL keys are stored.",
            label_text="SBL Keys Directory:",
            bind_variable=self.global_variables["sbl_key_path"],
            browse_type="folder",
            browse_title_text="Select SBL Keys Directory")
        sbl_key_dir.add_button("Generate Keys", self.gen_sbl_keys)

        self.toolchain_info = StringVar(value="Parsing toolchain...")
        InfoFrame(
            parent=self,
            label_text="Toolchain Information:",
            bind_variable=self.toolchain_info)

        self.terminal_log = TerminalWindow(self, title="Setup Terminal Log")

        Button(self, text="Show Log", command=self.show_log).pack(pady=20)

        self.execution_manager = ExecutionManager(self, self.terminal_log)

        threading.Thread(target=self.update_toolchain_info, daemon=True).start()
        self.on_workspace_path_selected(self.global_variables["workspace_path"].get())

    # NOTE: Any call to set() functions on any items in self.global_variables will be instantly updated
    # across all tabs without selecting any current tab since it is shared mutable variable.
    # update() function is only used for updating state of the selected tab that depends on latest value
    # of self.global_variables
    def update(self):
        pass

    # TODO: If the workspace folder is not empty, use git to look for any subfolder with cloned slimbootloader repo. If one or more
    # cloned repo is found, prompt user to select one of the found repo to populate the SBL source code path
    def on_workspace_path_selected(self, workspace_path):
        """Handle workspace selection and populate default paths when they are empty."""
        if not workspace_path or workspace_path == "<Empty>":
            return

        # sbl_source_path = self.global_variables["sbl_source_path"].get()
        if self.check_for_sbl_repo(workspace_path):
            self.clone_update_button.config(text="Update")
            self.global_flags["sbl_path_update_from_setup"] = True
            return
        else:
            self.clone_update_button.config(text="Clone")
            self.global_variables["sbl_source_path"].set(os.path.join(workspace_path, "Slimbootloader"))

        sbl_key_path = self.global_variables["sbl_key_path"].get()
        if not sbl_key_path or sbl_key_path == "<Empty>":
            self.global_variables["sbl_key_path"].set(os.path.join(workspace_path, "SblKeys"))

        # self.load_settings(workspace_path)

    # Skipping compiler version check due to its complexity
    def update_toolchain_info(self):
        def get_openssl_version():
            openssl_cmd = version = None
            if os.name == 'nt':
                if 'OPENSSL_PATH' not in os.environ:
                    for openssl_dir in ["C:\\Openssl\\bin\\", "C:\\Openssl\\"]:
                        openssl_exe_path = os.path.join(openssl_dir, "openssl.exe")
                        if os.path.isfile(openssl_exe_path):
                            openssl_cmd = openssl_exe_path
                            break
                elif os.path.isfile(os.path.join(os.environ['OPENSSL_PATH'], 'openssl.exe')):
                    openssl_cmd = os.path.join(os.environ.get('OPENSSL_PATH', ''), 'openssl.exe')
            else:
                # Get openssl path for Linux cases
                openssl_cmd = shutil.which('openssl')

            if openssl_cmd:
                try:
                    version = subprocess.check_output([openssl_cmd, 'version']).decode().strip().split()[1]
                except:
                    print("Error getting OpenSSL version")
                    pass
            return version

        def get_nasm_version():
            nasm_cmd = version = None
            if os.name == 'nt':
                if os.path.isfile(os.path.join("C:\\Nasm", 'nasm.exe')):
                    nasm_cmd = os.path.join("C:\\Nasm", 'nasm.exe')
            else:
                # Get nasm path for Linux cases
                nasm_cmd = shutil.which('nasm')
            if nasm_cmd:
                try:
                    ver_str = subprocess.check_output([nasm_cmd, '-v']).decode().strip()
                    version = re.search(r'version\s*([\d.]+)', ver_str).group(1)
                except:
                    print('Error getting NASM version')
                    pass
            return version

        def get_iasl_version():
            iasl_cmd = version = None
            if os.name == 'nt':
                if os.path.isfile(os.path.join("C:\\ASL", 'iasl.exe')):
                    iasl_cmd = os.path.join("C:\\ASL", 'iasl.exe')
            else:
                # Get iasl path for Linux cases
                iasl_cmd = shutil.which('iasl')
            if iasl_cmd:
                try:
                    ver_str = subprocess.check_output([iasl_cmd, '-v']).decode().strip()
                    version = re.search(r'version\s*([\d.]+)', ver_str).group(1)
                except:
                    print('Error getting IASL version')
                    pass
            return version

        def get_git_version():
            version = None
            try:
                ver_str = subprocess.check_output(['git', '--version']).decode().strip()
                version = re.search(r'version\s*([a-zA-Z0-9.]+)', ver_str).group(1)
            except:
                print("Error getting Git version")
                pass
            return version

        toolchain_list = {
            "Python": sys.version.split()[0],
            "OpenSSL": get_openssl_version(),
            "NASM": get_nasm_version(),
            "IASL": get_iasl_version(),
            "Git": get_git_version(),
        }

        toolchain_info_str = ""
        for tool_name, tool_version in toolchain_list.items():
            toolchain_info_str += f"{tool_name:<10}: {tool_version if tool_version else 'Not found'}\n"

        self.toolchain_info.set(f"{toolchain_info_str.strip()}")

    def check_for_sbl_repo(self, workspace_path):
        def is_slimbootloader_repo(folder):
            repo_url = "https://github.com/slimbootloader/slimbootloader.git"
            try:
                folder = os.path.dirname(folder)
                command = ["git", "config", "--get", "remote.origin.url"]
                process = subprocess.run(command, cwd=folder, capture_output=True, text=True)
                parsed_url = process.stdout.strip()
                if parsed_url == repo_url:
                    print(f"Detected slimbootloader repo in {folder}")
                    return True
            except Exception as e:
                print(f"Error checking git repo in {folder}: {e}")
            return False

        sbl_source_path = self.global_variables["sbl_source_path"].get()
        if sbl_source_path and sbl_source_path != "<Empty>":
            if os.path.isdir(sbl_source_path) and is_slimbootloader_repo(sbl_source_path):
                return True
        """Check for existing slimbootloader repo in the selected workspace."""
        for folder in glob.glob(os.path.join(workspace_path, "**", ".git")):
            if is_slimbootloader_repo(folder):
                self.global_variables["sbl_source_path"].set(os.path.dirname(folder))
                return True

        return False

    def on_closing(self):
        print("SetupPage: on_closing called. Performing any cleanup if necessary...")
        if self.execution_manager.is_busy:
            print("Terminating ongoing processes in SetupPage...")
            self.execution_manager.terminate_process(app_exit=True)

    def gen_sbl_keys(self):
        print("Generating SBL keys...")
        workspace = self.global_variables["workspace_path"].get()
        source_path = self.global_variables["sbl_source_path"].get()
        keys_path = self.global_variables["sbl_key_path"].get()

        if not workspace or workspace == "<Empty>":
            messagebox.showwarning("Warning", "Please select a valid workspace directory before generating SBL keys.")
            return

        if keys_path == "<Empty>":
            keys_path = os.path.join(workspace, "SblKeys")
            self.global_variables["sbl_key_path"].set(keys_path)

        if not os.path.isdir(keys_path):
            os.makedirs(keys_path, exist_ok=True)

        # Do not overwrite an existing key set.
        has_existing_file = False
        for _ in os.scandir(keys_path):
            has_existing_file = True
            break
        if has_existing_file:
            messagebox.showwarning(
                "Warning",
                f"Key generation stopped. '{keys_path}' is not empty.")
            return

        self.execution_manager.status_bar.set("Generating SBL keys... ", color="black")
        generate_keys_script = os.path.join(source_path,  "BootloaderCorePkg", "Tools", "GenerateKeys.py")
        if not os.path.isfile(generate_keys_script):
            messagebox.showerror("Error", f"GenerateKeys.py script not found in {os.path.dirname(generate_keys_script)}. Please check your SBL source code path.")
            return
        command = f"{get_python_cmd()} {generate_keys_script} -k {keys_path}"
        print(f"Running command: {command} in {source_path}")
        self.execution_manager.execute_command(command, cwd=source_path)

    def clone_update_repo(self):
        def get_latest_commit_hash(repo_path):
            try:
                command = ["git", "rev-parse", "--short", "HEAD"]
                process = subprocess.run(command, cwd=repo_path, capture_output=True, text=True)
                if process.returncode == 0:
                    return process.stdout.strip()
            except Exception as e:
                print(f"Error getting latest commit hash: {e}")
            return None

        print("Cloning/Updating SBL source code repository...")

        if self.global_variables["sbl_source_path"].get() != "<Empty>":
            folder_name = os.path.basename(self.global_variables["sbl_source_path"].get())
        else:
            folder_name = "Slimbootloader"
        workspace = self.global_variables["workspace_path"].get()
        source_path = self.global_variables["sbl_source_path"].get()

        # Run git clone or update command
        if self.clone_update_button.cget("text") == "Clone":
            self.execution_manager.status_bar.set("Cloning SBL source code repository... ", color="black")
            command = f"git clone {SBL_REPO_URL} {folder_name}"
            self.execution_manager.execute_command(command, cwd=workspace)
            # Run git submodule update command
            command = f"git submodule update --init --recursive"
            self.execution_manager.execute_command(command, cwd=source_path, queue=True)
            self.global_flags["sbl_path_update_from_setup"] = True
        elif self.clone_update_button.cget("text") == "Update":
            self.execution_manager.status_bar.set("Updating SBL source code repository... ", color="black")
            commit_before = get_latest_commit_hash(source_path)
            command = "git pull"
            self.execution_manager.execute_command(command, cwd=source_path)
            # Run git submodule update command
            command = f"git submodule update --init --recursive"
            self.execution_manager.execute_command(command, cwd=source_path, queue=True)
            commit_after = get_latest_commit_hash(source_path)
            if commit_before != commit_after:
                self.global_flags["sbl_path_update_from_setup"] = True

    def clean_repo(self):
        print("Cleaning SBL source code repository...")
        source_path = self.global_variables["sbl_source_path"].get()

        self.execution_manager.status_bar.set("Cleaning SBL source code repository... ", color="black")
        command = "git reset --hard && git clean -fd"
        self.execution_manager.execute_command(command, cwd=source_path, shell=True)
        # Notify build page to reset certain build configuration options to default
        self.global_flags["git_clean_from_setup"] = True

    def show_log(self):
        self.terminal_log.show()