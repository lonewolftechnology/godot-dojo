from SCons.Variables import BoolVariable

def options(opts):
    opts.Add(BoolVariable("use_mingw", "Use the MinGW compiler instead of MSVC - only effective on Windows", False))

def exists(env):
    return True

def generate(env):
    if env["use_mingw"]:
        env.Append(CCFLAGS=["-Wwrite-strings"])
        env.Append(LINKFLAGS=["-Wl,--export-all-symbols"])

    env.Append(CPPDEFINES=["WINDOWS_ENABLED"])

    try:
        import common_compiler_flags
        common_compiler_flags.generate(env)
    except:
        pass
