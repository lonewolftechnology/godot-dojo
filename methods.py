import os
import re
import subprocess
import sys
from enum import Enum

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

def _get_git_submodule_version(submodule_path):
    """
    Detects the version of a git submodule by trying to find a tag, then a branch name,
    then the branch from .gitmodules, and finally the short commit hash.
    Returns the found version string or None if all methods fail.
    """
    repo_path = os.path.join(os.getcwd(), submodule_path)
    version_source = None
    try:
        # Prefer the latest reachable tag.
        version_source = subprocess.check_output(
            ["git", "describe", "--tags", "--abbrev=0"], cwd=repo_path, stderr=subprocess.DEVNULL, text=True
        ).strip()
    except Exception:
        try:
            # Fallback to branch name if no tag is found.
            version_source = subprocess.check_output(
                ["git", "symbolic-ref", "-q", "--short", "HEAD"], cwd=repo_path, stderr=subprocess.DEVNULL, text=True
            ).strip()
        except Exception:
            # As a last resort, try reading the .gitmodules declared branch.
            try:
                with open(os.path.join(os.getcwd(), ".gitmodules"), "r") as gm:
                    gm_text = gm.read()
                    m = re.search(rf'\[submodule "{submodule_path}"\][\s\S]*?branch\s*=\s*([^\n\r]+)', gm_text)
                    if m:
                        version_source = m.group(1).strip()
            except Exception:
                # Final fallback to short commit hash.
                try:
                    version_source = subprocess.check_output(
                        ["git", "rev-parse", "--short", "HEAD"], cwd=repo_path, stderr=subprocess.DEVNULL, text=True
                    ).strip()
                except Exception:
                    return None # All methods failed
    return version_source


def _detect_godot_min_requirement():
    version_source = _get_git_submodule_version("external/godot-cpp")
    if version_source:
        m = re.search(r"(\d+)\.(\d+)", version_source)
        if m:
            return f"{m.group(1)}.{m.group(2)}"
    # Default fallback if detection fails.
    return "4.2"

def get_git_version():
    try:
        return subprocess.check_output(
            ["git", "describe", "--tags", "--always", "--dirty"],
            stderr=subprocess.DEVNULL,
            text=True
        ).strip()
    except Exception:
        return "0.0.0-unknown"
