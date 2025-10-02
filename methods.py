import os
import sys
from enum import Enum
import platform as host_platform
import re
import shutil
import subprocess

# Colors are disabled in non-TTY environments such as pipes. This means
# that if output is redirected to a file, it won't contain color codes.
# Colors are always enabled on continuous integration.
_colorize = bool(sys.stdout.isatty() or os.environ.get("CI"))


class ANSI(Enum):
    """
    Enum class for adding ansi colorcodes directly into strings.
    Automatically converts values to strings representing their
    internal value, or an empty string in a non-colorized scope.
    """

    RESET = "\x1b[0m"

    BOLD = "\x1b[1m"
    ITALIC = "\x1b[3m"
    UNDERLINE = "\x1b[4m"
    STRIKETHROUGH = "\x1b[9m"
    REGULAR = "\x1b[22;23;24;29m"

    BLACK = "\x1b[30m"
    RED = "\x1b[31m"
    GREEN = "\x1b[32m"
    YELLOW = "\x1b[33m"
    BLUE = "\x1b[34m"
    MAGENTA = "\x1b[35m"
    CYAN = "\x1b[36m"
    WHITE = "\x1b[37m"

    PURPLE = "\x1b[38;5;93m"
    PINK = "\x1b[38;5;206m"
    ORANGE = "\x1b[38;5;214m"
    GRAY = "\x1b[38;5;244m"

    def __str__(self) -> str:
        global _colorize
        return str(self.value) if _colorize else ""


def print_warning(*values: object) -> None:
    """Prints a warning message with formatting."""
    print(f"{ANSI.YELLOW}{ANSI.BOLD}WARNING:{ANSI.REGULAR}", *values, ANSI.RESET, file=sys.stderr)


def print_error(*values: object) -> None:
    """Prints an error message with formatting."""
    print(f"{ANSI.RED}{ANSI.BOLD}ERROR:{ANSI.REGULAR}", *values, ANSI.RESET, file=sys.stderr)


# Symbols
is_windows = host_platform.system().lower() == "windows"
if is_windows:
    # Use ASCII alternatives on Windows
    rocket = ">"
    broom = "-"
    check = "+"
    package = "#"
    clipboard = "="
    party = "!"
    cross = "x"
else:
    # Use emojis on other platforms
    rocket = "🚀"
    broom = "🧹"
    check = "✅"
    package = "📦"
    clipboard = "📋"
    party = "🎉"
    cross = "❌"


def apply_dojo_h_patch(Exit):
    """Applies a patch to dojo.h to fix an incomplete type issue."""
    print(f"{ANSI.YELLOW}{clipboard} Applying workaround patch to dojo.h...{ANSI.RESET}")
    # Check if the 'patch' command is available.
    if not shutil.which("patch"):
        print(f"{ANSI.RED}{cross} Error: The 'patch' command was not found in your system's PATH.{ANSI.RESET}")
        print(f"{ANSI.YELLOW}This is required to apply a necessary fix to a dependency.{ANSI.RESET}")
        print(f"{ANSI.BLUE}On Windows, the easiest way to get it is by installing 'Git for Windows':{ANSI.RESET}")
        print(f"{ANSI.GREEN}https://git-scm.com/download/win{ANSI.RESET}")
        print(f"{ANSI.YELLOW}Please install it and make sure its tools are added to your PATH, then try again.{ANSI.RESET}")
        Exit(1)

    patch_file = 'patches/fix_dojo_c_incomplete_type.patch'
    dojo_c_dir = 'external/dojo.c'
    target_to_patch = f'{dojo_c_dir}/dojo.h'

    try:
        subprocess.run(['patch', '-p1', '-d', dojo_c_dir, '-i', os.path.abspath(patch_file)], check=True)
        print(f"{ANSI.GREEN}{check} Patch applied successfully to {target_to_patch}.{ANSI.RESET}")
    except Exception as e:
        print(f"{ANSI.RED}{cross} Failed to apply patch: {e}{ANSI.RESET}")
        Exit(1)


def detect_godot_min_requirement():
    repo_path = os.path.join(os.getcwd(), "external", "godot-cpp")
    version_source = None
    try:
        # Prefer the latest reachable tag.
        tag = subprocess.check_output([
            "git", "-C", repo_path, "describe", "--tags", "--abbrev=0"
        ], stderr=subprocess.DEVNULL, text=True).strip()
        version_source = tag
    except Exception:
        try:
            # Fallback to branch name if no tag is found.
            branch = subprocess.check_output([
                "git", "-C", repo_path, "symbolic-ref", "-q", "--short", "HEAD"
            ], stderr=subprocess.DEVNULL, text=True).strip()
            version_source = branch
        except Exception:
            pass

    # As a last resort, try reading the .gitmodules declared branch.
    if not version_source:
        try:
            with open(os.path.join(os.getcwd(), ".gitmodules"), "r") as gm:
                gm_text = gm.read()
                # Look for the external/godot-cpp section and capture a version-like pattern.
                # Example: branch = godot-4.3-stable
                m = re.search(r"\[submodule \"external/godot-cpp\"\][\s\S]*?branch\s*=\s*([^\n\r]+)", gm_text)
                if m:
                    version_source = m.group(1).strip()
        except Exception:
            pass

    if version_source:
        m = re.search(r"(\d+)\.(\d+)", version_source)
        if m:
            return f"{m.group(1)}.{m.group(2)}"

    # Default fallback if detection fails.
    return "4.2"


def copy_web_artifacts(target, source, env):
    print(f"{ANSI.YELLOW}{clipboard} Preparing web export files...{ANSI.RESET}")
    build_mode = "release" if env.get("target", "template_debug") == "template_release" else "debug"

    addon_dir = "demo/addons/godot-dojo"
    out_name = f"dojo_c_{build_mode}"

    js_path = f"{addon_dir}/{out_name}.js"
    wasm_bg_path = f"{addon_dir}/{out_name}_bg.wasm"
    wasm_final_path = f"{addon_dir}/{out_name}.wasm"

    # 1. Rename the wasm file (e.g., dojo_c_release_bg.wasm -> dojo_c_release.wasm)
    print(f"{ANSI.YELLOW}Renaming {os.path.basename(wasm_bg_path)} to {os.path.basename(wasm_final_path)}...{ANSI.RESET}")
    shutil.move(wasm_bg_path, wasm_final_path)

    # 2. Patch the JS file to load the renamed wasm file
    print(f"{ANSI.YELLOW}Patching {os.path.basename(js_path)} to load the new wasm file...{ANSI.RESET}")
    with open(js_path, 'r+') as f:
        content = f.read()
        # Replace the old wasm filename with the new one
        content = content.replace(f"'{out_name}_bg.wasm'", f"'{out_name}.wasm'")
        f.seek(0)
        f.write(content)
        f.truncate()

    print(f"{ANSI.GREEN}{check} Web artifacts prepared in {addon_dir}{ANSI.RESET}")

    # Copy the custom HTML template to the addon directory
    template_html_src = "web/godot-template.html"
    shutil.copy(template_html_src, f"{addon_dir}/godot-template.html")
    print(f"{ANSI.GREEN}{check} Custom HTML template copied to {addon_dir}{ANSI.RESET}")

    print(f"")
    print(f"{ANSI.YELLOW}********************************************************************************{ANSI.RESET}")
    print(f"{ANSI.YELLOW}** {clipboard} ACTION REQUIRED: Configure Godot Web Export                        **{ANSI.RESET}")
    print(f"{ANSI.YELLOW}********************************************************************************{ANSI.RESET}")
    print(f"{ANSI.BLUE}To complete the web export, you MUST configure the Godot export preset:{ANSI.RESET}")
    print(f"{ANSI.BLUE}1. Go to {ANSI.GREEN}Project -> Export...{ANSI.RESET} and select the {ANSI.GREEN}Web{ANSI.RESET} preset.{ANSI.RESET}")
    print(f"{ANSI.BLUE}2. In the options, find {ANSI.GREEN}Shell -> Custom HTML Shell{ANSI.RESET}.{ANSI.RESET}")
    print(f"{ANSI.BLUE}3. Select the file: {ANSI.GREEN}res://addons/godot-dojo/godot-template.html{ANSI.RESET}{ANSI.RESET}")
    print(f"{ANSI.YELLOW}********************************************************************************{ANSI.RESET}")
