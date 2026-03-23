from SCons.Variables import BoolVariable
try:
    from SCons.Tool import clang, clangxx
except ImportError:
    pass

def options(opts):
    opts.Add(BoolVariable("use_llvm", "Use the LLVM compiler - only effective when targeting Linux", False))

def exists(env):
    return True

def generate(env):
    if env.get("use_llvm", False):
        try:
            clang.generate(env)
            clangxx.generate(env)
        except Exception:
            pass
    elif env.get("use_hot_reload", False):
        # Required for extensions to truly unload.
        env.Append(CXXFLAGS=["-fno-gnu-unique"])

    env.Append(CCFLAGS=["-fPIC", "-Wwrite-strings"])
    env.Append(LINKFLAGS=["-Wl,-R,'$$ORIGIN'"])

    arch = env.get("arch", "")
    if arch == "x86_64":
        env.Append(CCFLAGS=["-m64", "-march=x86-64"])
        env.Append(LINKFLAGS=["-m64", "-march=x86-64"])
    elif arch == "x86_32":
        env.Append(CCFLAGS=["-m32", "-march=i686"])
        env.Append(LINKFLAGS=["-m32", "-march=i686"])
    elif arch == "arm64":
        env.Append(CCFLAGS=["-march=armv8-a"])
        env.Append(LINKFLAGS=["-march=armv8-a"])
    elif arch == "rv64":
        env.Append(CCFLAGS=["-march=rv64gc"])
        env.Append(LINKFLAGS=["-march=rv64gc"])

    env.Append(CPPDEFINES=["LINUX_ENABLED", "UNIX_ENABLED"])

    try:
        import common_compiler_flags
        common_compiler_flags.generate(env)
    except:
        pass
