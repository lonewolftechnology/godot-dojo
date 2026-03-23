def options(opts):
    pass

def exists(env):
    return True

def generate(env):
    env.Append(CPPDEFINES=["ANDROID_ENABLED", "UNIX_ENABLED"])
    
    try:
        import common_compiler_flags
        common_compiler_flags.generate(env)
    except:
        pass
