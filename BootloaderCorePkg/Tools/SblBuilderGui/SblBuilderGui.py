import os
import sys
import tkinter as tk
from tkinter import messagebox, StringVar
from SblBuilderGuiComp import *
import traceback
import importlib
import inspect
import json
import time

SBL_ICON_PATH=os.path.join(os.path.dirname(os.path.abspath(__file__)), "assets", "icon.ico")
SETTING_JSON_FILENAME = "sblbuilder.setting.json"

class SlimBootloaderApp:
    def __init__(self, root):
        self.root = root
        self.is_closing = False  # Flag to prevent UI updates after window closes
        self.root.title("Slim Bootloader Builder GUI")

        self.global_variables = {
            "workspace_path": StringVar(value="<Empty>"),
            "sbl_source_path": StringVar(value="<Empty>"),
            "sbl_key_path": StringVar(value="<Empty>"),
            "python_path": StringVar(value="<Empty>"),
            "build_platform": StringVar(value="<Empty>")
        }

        self.global_flags = {
            "git_clone_update_from_setup": False,
            "git_clean_from_setup": False,
        }

        self.load_settings()

        # Load and set window icon
        try:
            if os.path.exists(SBL_ICON_PATH):
                icon_path = SBL_ICON_PATH

                # iconbitmap with .ico is supported on Windows; skip on other platforms.
                if sys.platform == 'win32':
                    # Set icon for both window and taskbar
                    self.root.iconbitmap(icon_path)

                    # Force taskbar to update icon on Windows
                    def update_taskbar_icon():
                        if not self.is_closing:
                            try:
                                self.root.iconbitmap(icon_path)
                            except:
                                pass
                    self.root.after(100, update_taskbar_icon)
                    print(f"Icon set to: {icon_path}")
                else:
                    print("Info: Skipping .ico icon on non-Windows platform.")
            else:
                print(f"Warning: Logo file not found: {SBL_ICON_PATH}")
        except Exception as e:
            print(f"Warning: Could not load logo: {e}")

        self.root.geometry("1280x720")
        self.root.protocol("WM_DELETE_WINDOW", self.on_closing)
        self.root.bind("<KeyPress>", self.refresh_tabs)
        self.notebook = ttk.Notebook(root)
        self.notebook.pack(expand=True, fill="both")

        style = ttk.Style()
        style.theme_use("clam")

        # Configure the font for the standard Tab class
        style.configure("TNotebook.Tab", font=("Helvetica", 13))
        style.map("TNotebook.Tab",
                    background=[("selected", "#f0f0f0"), ("active", "#ececec")],
                    foreground=[("selected", "black")])

        # Configure rounded button style
        style.configure("Rounded.TButton",
                       font=FONT_BUTTON,
                       foreground=BUTTON_FG,
                       background=BUTTON_BG,
                       borderwidth=1,
                       relief="raised",
                       padding=(10, 5))
        self.notebook.bind("<<NotebookTabChanged>>", self.on_tab_changed)
        self.load_tabs()

    def load_tabs(self):
        # Dyanamically load tabs from the tabs directory
        tabs_dir = os.path.join(os.path.dirname(os.path.abspath(__file__)), "tabs")
        self.tab_list = {}
        for filename in sorted(os.listdir(tabs_dir)):
            if filename.endswith(".py"):
                module_name = filename[:-3]
                try:
                    module = importlib.import_module(f"tabs.{module_name}")
                    for name, tab_class in inspect.getmembers(module):
                        if inspect.isclass(tab_class) and issubclass(tab_class, TabPage) and tab_class is not TabPage:
                            print(f"Loading tab: {name} class from {filename}")
                            tab_instance = tab_class(self.notebook, self.global_variables, self.global_flags)
                            self.tab_list[name] = tab_instance
                except Exception as e:
                    print(f"Error loading tab {module_name}: {e}")
                    traceback.print_exc()
        self.notebook.select(0)

    def on_tab_changed(self, event):
        selected_tab = event.widget.select()
        selected_index = event.widget.index(selected_tab)
        selected_tab_name = list(self.tab_list.keys())[selected_index]
        print(f"Tab changed to index: {selected_index}, name: {selected_tab_name}")
        self.tab_list[selected_tab_name].update()

    def refresh_tabs(self, event):
        if event.keysym == "F5":
            if messagebox.askyesno("Refresh Tabs", "Refreshing all tabs?"):
                is_any_tab_busy = False
                for tab_name, tab in self.tab_list.items():
                    if hasattr(tab, "execution_manager") and tab.execution_manager.is_busy:
                        is_any_tab_busy = True
                        print(f"Tab {tab_name} is still busy")
                        break

                # Ask user for confirmation if any tab is still busy
                if is_any_tab_busy:
                    response = messagebox.askokcancel("Quit", "Some operations are still running. Do you want to terminate them?")
                    if not response:
                        return

                for tab_name, tab in self.tab_list.items():
                    print(f"Calling on_closing for tab: {tab_name}")
                    try:
                        tab.on_closing()
                    except Exception as e:
                        print(f"Error during on_closing of {tab_name}: {e}")
                        traceback.print_exc()
                        return

                for tab_name, tab in self.tab_list.items():
                    print(f"Refreshing tab: {tab_name}")
                    try:
                        for widget in tab.winfo_children():
                            widget.destroy()
                        tab.destroy()
                        module = importlib.reload(sys.modules[tab.__module__])
                        for name, tab_class in inspect.getmembers(module):
                            if inspect.isclass(tab_class) and issubclass(tab_class, TabPage) and tab_class is not TabPage:
                                tab_instance = tab_class(self.notebook, self.global_variables, self.global_flags)
                                self.tab_list[name] = tab_instance
                    except Exception as e:
                        print(f"Error refreshing tab {tab_name}: {e}")
                        traceback.print_exc()

    def load_settings(self):
        """Load global variables from settings file if present."""
        for settings_file_path in self._get_settings_file_candidates():
            if not os.path.exists(settings_file_path):
                continue
            try:
                with open(settings_file_path, "r") as f:
                    settings = json.load(f)
                if not isinstance(settings, dict):
                    print(f"Warning: Invalid settings format in {settings_file_path}")
                    continue

                for key, value in settings.items():
                    if key in self.global_variables and isinstance(self.global_variables[key], tk.StringVar):
                        self.global_variables[key].set(str(value))
                print(f"Loaded settings from {settings_file_path}")
                return
            except Exception as e:
                print(f"Warning: Could not load settings from {settings_file_path}: {e}")

    def _get_settings_file_candidates(self):
        local_settings = os.path.join(os.path.dirname(os.path.abspath(__file__)), SETTING_JSON_FILENAME)
        if sys.platform == "win32":
            config_root = os.environ.get("APPDATA", os.path.expanduser("~"))
        else:
            config_root = os.environ.get("XDG_CONFIG_HOME", os.path.join(os.path.expanduser("~"), ".config"))
        user_settings = os.path.join(config_root, "sblbuilder", SETTING_JSON_FILENAME)
        return [local_settings, user_settings]

    def app_exit(self, save_last_session=False):
        # Set flag to prevent UI updates during shutdown
        self.is_closing = True

        # Call on_closing for each tab to allow them to perform necessary cleanup in parallel
        is_any_tab_busy = False
        for tab_name, tab in self.tab_list.items():
            if hasattr(tab, "execution_manager") and tab.execution_manager.is_busy:
                is_any_tab_busy = True
                print(f"Tab {tab_name} is still busy")
                break

        # Ask user for confirmation if any tab is still busy
        if is_any_tab_busy:
            response = messagebox.askokcancel("Quit", "Some operations are still running. Do you want to quit anyway?")
            if not response:
                return

        for tab_name, tab in self.tab_list.items():
            print(f"Calling on_closing for tab: {tab_name}")
            try:
                tab.on_closing()
            except Exception as e:
                print(f"Error during on_closing of {tab_name}: {e}")
                traceback.print_exc()
                return

        # Save global variables to JSON file, extracting values from StringVar objects
        settings_to_save = {}
        for key, value in self.global_variables.items():
            if isinstance(value, tk.StringVar):
                settings_to_save[key] = value.get()
            # elif isinstance(value, (str, int, float, bool, list, dict)):
            #     settings_to_save[key] = value

        saved_path = None
        last_error = None
        for settings_file_path in self._get_settings_file_candidates():
            try:
                os.makedirs(os.path.dirname(settings_file_path), exist_ok=True)
                with open(settings_file_path, "w") as f:
                    json.dump(settings_to_save, f, indent=4)  # indent=4 makes it human-readable
                saved_path = settings_file_path
                break
            except Exception as e:
                last_error = e
                print(f"Warning: Could not save settings to {settings_file_path}: {e}")

        if saved_path:
            print(f"Saved settings to {saved_path}")
        elif last_error:
            print(f"Warning: Settings were not saved: {last_error}")

        # Destroy widgets before quitting to prevent ttk cleanup errors
        try:
            for widget in self.root.winfo_children():
                widget.destroy()
        except:
            pass

        self.root.quit()
        self.root.destroy()

    def on_closing(self):
        """Handle window close event (X button)"""
        self.app_exit()


if __name__ == "__main__":
    root = tk.Tk()
    SlimBootloaderApp(root)
    root.mainloop()