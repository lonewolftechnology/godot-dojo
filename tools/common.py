def exists(env):
    return True

def generate(env):
    env.Append(CPPPATH=["src/", "include/", "external/boost/include"])
    if env["platform"] != "web":
        env.Append(CPPPATH=["bindings/"])
