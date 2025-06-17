#!/usr/bin/env python
import os
import subprocess
import glob
import shutil
import platform as py_platform

# Config
PROJECT_NAME = "godot-dojo"
ENTRY_POINT = "dojoc_library_init"
GODOT_MIN_REQUIREMENT = "4.2"

def init_colors():
    """Inicializa colores ANSI de forma compatible con Windows"""

    # Si no es Windows, usar ANSI directamente
    if py_platform.system().lower() != "windows":
        return {
            'G': '\033[92m',  # Green
            'B': '\033[94m',  # Blue
            'R': '\033[91m',  # Red
            'Y': '\033[1;33m',  # Yellow
            'X': '\033[0m'    # Reset
        }

    # En Windows, intentar habilitar ANSI
    try:
        # Truco simple: llamar a os.system con string vacío
        # Esto a menudo habilita el modo ANSI en terminales modernos
        os.system("")

        # Verificar si estamos en Windows Terminal o PowerShell moderno
        if os.getenv('WT_SESSION') or 'WindowsPowerShell' in str(os.getenv('PSModulePath', '')):
            return {
                'G': '\033[92m',
                'B': '\033[94m',
                'R': '\033[91m',
                'Y': '\033[1;33m',
                'X': '\033[0m'
            }

        # Intentar con API de Windows
        import ctypes
        kernel32 = ctypes.windll.kernel32
        kernel32.SetConsoleMode(kernel32.GetStdHandle(-11), 7)

        return {
            'G': '\033[92m',
            'B': '\033[94m',
            'R': '\033[91m',
            'Y': '\033[1;33m',
            'X': '\033[0m'
        }

    except:
        # Si todo falla, sin colores
        return {
            'G': '', 'B': '', 'R': '', 'Y': '', 'X': ''
        }

# Inicializar colores
colors = init_colors()
G, B, R, Y, X = colors['G'], colors['B'], colors['R'], colors['Y'], colors['X']

print(f"{B}🚀 Building {PROJECT_NAME}{X}")

# FUNCIONES COMUNES (DISPONIBLES PARA CLEANING Y BUILD)
def detect_platform():
    system = py_platform.system().lower()
    if system == "linux":
        return "linux"
    elif system == "windows":
        return "windows"
    elif system == "darwin":
        return "macos"
    else:
        print(f"{Y}⚠️ Unknown system '{system}', defaulting to linux{X}")
        return "linux"

def detect_arch():
    machine = py_platform.machine().lower()
    if machine in ["x86_64", "amd64"]:
        return "x86_64"
    elif machine in ["aarch64", "arm64"]:
        return "arm64"
    elif machine.startswith("arm"):
        return "arm32"
    else:
        print(f"{Y}⚠️ Unknown architecture '{machine}', defaulting to x86_64{X}")
        return "x86_64"

# Cache para has_msvc - evitar múltiples ejecuciones
_msvc_cache = None

def has_msvc():
    """Detecta MSVC de forma simple - si Rust puede usarlo, nosotros también (CACHED)"""
    global _msvc_cache
    
    # Si ya fue detectado, usar cache
    if _msvc_cache is not None:
        return _msvc_cache
    
    if py_platform.system().lower() != "windows":
        _msvc_cache = False
        return _msvc_cache

    try:
        print(f"{B}🔍 Detecting MSVC toolchain...{X}")
        
        # Método 1: Verificar directamente cl.exe
        result = subprocess.run(['where', 'cl'], 
                              capture_output=True, text=True, shell=True)
        if result.returncode == 0:
            print(f"  ✅ MSVC compiler found: {result.stdout.strip().split()[0]}")
            _msvc_cache = True
            return _msvc_cache
        
        # Método 2: Test con cargo check en dojo.c directory
        result = subprocess.run([
            'cargo', 'check', '--target', 'x86_64-pc-windows-msvc'
        ], capture_output=True, text=True, cwd='external/dojo.c')
        
        if result.returncode == 0:
            print(f"  ✅ MSVC toolchain confirmed (Rust can use it)")
            _msvc_cache = True
            return _msvc_cache
        
        _msvc_cache = False
        return _msvc_cache
        
    except Exception as e:
        print(f"  ⚠️ MSVC detection inconclusive: {e}")
        # En Windows, usar método más conservador
        try:
            # Verificar si rustup conoce el target MSVC
            result = subprocess.run(['rustup', 'target', 'list'], 
                                  capture_output=True, text=True)
            if result.returncode == 0 and 'x86_64-pc-windows-msvc' in result.stdout:
                print(f"  ✅ MSVC target available in rustup")
                _msvc_cache = True
                return _msvc_cache
        except:
            pass
        
        _msvc_cache = False
        return _msvc_cache

# Detectar si estamos en modo clean
is_cleaning = GetOption('clean')

if is_cleaning:
    print(f"{Y}🧹 Cleaning mode detected{X}")

    # 1. Limpiar dojo.c con cargo clean
    print(f"{Y}🦀 Cleaning dojo.c (cargo clean)...{X}")
    try:
        subprocess.run(["cargo", "clean"], cwd="external/dojo.c", check=True)
        print(f"{G}✅ dojo.c cleaned{X}")
    except subprocess.CalledProcessError as e:
        print(f"{R}❌ Failed to clean dojo.c: {e}{X}")
    except Exception as e:
        print(f"{R}❌ Error cleaning dojo.c: {e}{X}")

    # 2. Limpiar archivos generados adicionales
    print(f"{Y}🧹 Cleaning generated files...{X}")
    files_to_clean = [
        "demo/bin/",
        "external/dojo.c/dojo.hpp.backup"
    ]

    for path in files_to_clean:
        try:
            if os.path.isdir(path):
                shutil.rmtree(path, ignore_errors=True)
                print(f"  🗑️  Removed directory: {path}")
            elif os.path.isfile(path):
                os.remove(path)
                print(f"  🗑️  Removed file: {path}")
        except Exception as e:
            print(f"{Y}⚠️  Could not remove {path}: {e}{X}")

    # 3. Limpiar godot-cpp usando su propio SConstruct (simple)
    print(f"{Y}🔧 Cleaning godot-cpp...{X}")
    try:
        # Simplemente pasar argumentos de limpieza a godot-cpp
        cleanup_args = []
        for key, value in ARGUMENTS.items():
            cleanup_args.append(f"{key}={value}")
        
        cleanup_cmd = ["scons", "-C", "external/godot-cpp", "--clean"] + cleanup_args
        subprocess.run(cleanup_cmd, check=False)  # No fallar si hay error
        print(f"{G}✅ godot-cpp cleanup attempted{X}")
    except Exception as e:
        print(f"{Y}⚠️ godot-cpp cleanup failed: {e}{X}")

    print(f"{G}✅ Cleanup complete - SCons will handle the rest{X}")
    
    # No cargar environment complejo, solo dejar que SCons haga su trabajo básico
    Return()

# Si no estamos limpiando, continuar con el build normal
if not is_cleaning:
    # Obtener parámetros con detección automática
    platform = ARGUMENTS.get('platform', detect_platform())
    target = ARGUMENTS.get('target', 'template_debug')
    arch = ARGUMENTS.get('arch', detect_arch())

    # Detectar host system para decisiones de toolchain
    host_platform = detect_platform()
    
    # Auto-configuración simple para Windows + MSVC (usa cache)
    def auto_setup_windows():
        """Setup automático para Windows - configura target MSVC en línea de comandos"""
        if py_platform.system().lower() != "windows":
            return None
        
        print(f"{B}Windows detected - checking for MSVC...{X}")
        
        # Verificar si MSVC está disponible (usa cache)
        if has_msvc():
            print(f"{G}✅ MSVC detected - will use x86_64-pc-windows-msvc target{X}")
            
            # Instalar target MSVC si no existe (silenciosamente)
            try:
                subprocess.run(['rustup', 'target', 'add', 'x86_64-pc-windows-msvc'], 
                              capture_output=True, check=False)
            except:
                pass
            
            return "x86_64-pc-windows-msvc"
        else:
            print(f"{Y}⚠️ MSVC not detected - will use GNU target{X}")
            return None

    # Llamar automáticamente para determinar el target
    auto_msvc_target = auto_setup_windows()
    
    # LÓGICA MODIFICADA: Siempre preferir MSVC en Windows si está disponible (usa cache)
    if host_platform == "windows" and platform == "windows":
        use_msvc = has_msvc()  # Usa cache
        if use_msvc:
            print(f"{B}Windows detected: Forcing MSVC toolchain{X}")
        else:
            print(f"{Y}⚠️ MSVC not found, falling back to MinGW{X}")
    else:
        use_msvc = False

    # Mostrar información del sistema detectado
    detected_platform = detect_platform()
    detected_arch = detect_arch()
    print(f"{B}System detected: {detected_platform} ({detected_arch}){X}")
    print(f"{B}Building for: {platform} ({arch}) - {target}{X}")
    if use_msvc:
        print(f"{B}Toolchain: MSVC (native Windows){X}")
    elif platform == "windows":
        print(f"{B}Toolchain: MinGW/GCC (cross-compile){X}")
    else:
        print(f"{B}Toolchain: GCC/Clang{X}")

    # Crear directorios necesarios
    os.makedirs("demo/bin", exist_ok=True)

    # 1. COMPILAR DOJO.C
    print(f"{Y}📦 Step 1: Compiling dojo.c...{X}")
    os.chdir("external/dojo.c")

    # Configurar target de Rust basado en plataforma, arquitectura y toolchain
    def get_rust_target(platform, arch, use_msvc, auto_target=None):
        # Si hay auto-detección de MSVC, usarla
        if auto_target and platform == "windows":
            return auto_target
            
        if platform == "windows":
            if use_msvc:
                # MSVC targets
                if arch == "x86_64":
                    return "x86_64-pc-windows-msvc"
                elif arch == "arm64":
                    return "aarch64-pc-windows-msvc"
            else:
                # MinGW targets
                if arch == "x86_64":
                    return "x86_64-pc-windows-gnu"
                elif arch == "arm64":
                    return "aarch64-pc-windows-gnullvm"
        elif platform == "linux":
            if arch == "x86_64":
                return "x86_64-unknown-linux-gnu"
            elif arch == "arm64":
                return "aarch64-unknown-linux-gnu"
            elif arch == "arm32":
                return "armv7-unknown-linux-gnueabihf"
        elif platform == "macos":
            if arch == "x86_64":
                return "x86_64-apple-darwin"
            elif arch == "arm64":
                return "aarch64-apple-darwin"

        # Default fallback
        return "x86_64-unknown-linux-gnu"

    rust_target = get_rust_target(platform, arch, use_msvc, auto_msvc_target)
    print(f"  🦀 Rust target: {rust_target}")

    # Comando cargo
    cargo_cmd = ["cargo", "build", "--target", rust_target]
    if target == "template_release":
        cargo_cmd.append("--release")

    print(f"  🦀 Running: {' '.join(cargo_cmd)}")
    subprocess.run(cargo_cmd, check=True)

    os.chdir("../..")
    print(f"{G}✅ Step 1 complete: dojo.c compiled{X}")

    # 2. PARCHEAR DOJO.HPP
    print(f"{Y}🔧 Step 2: Patching dojo.hpp...{X}")
    dojo_hpp_path = "external/dojo.c/dojo.hpp"

    # Verificar si el archivo existe
    if not os.path.exists(dojo_hpp_path):
        print(f"{R}❌ dojo.hpp not found at: {dojo_hpp_path}{X}")
        Exit(1)

    try:
        with open(dojo_hpp_path, 'r', encoding='utf-8') as f:
            content = f.read()

        # Buscar la declaración del namespace y agregar struct Clause; después
        if "struct Clause;" not in content:
            # Buscar patrón del namespace (puede variar)
            import re
            namespace_pattern = r'(namespace\s+\w+\s*{)'
            match = re.search(namespace_pattern, content)

            if match:
                # Insertar después del namespace
                insert_pos = match.end()
                content = content[:insert_pos] + '\nstruct Clause;' + content[insert_pos:]
                print(f"  ✏️  Added 'struct Clause;' after namespace declaration")
            else:
                # Fallback: agregar al inicio del archivo
                content = "struct Clause;\n" + content
                print(f"  ✏️  Added 'struct Clause;' at beginning of file")

            with open(dojo_hpp_path, 'w', encoding='utf-8') as f:
                f.write(content)

            print(f"  ✅ Successfully patched dojo.hpp")
        else:
            print(f"  ℹ️  'struct Clause;' already present, skipping patch")

    except Exception as e:
        print(f"{R}❌ Error patching dojo.hpp: {e}{X}")
        Exit(1)

    print(f"{G}✅ Step 2 complete: dojo.hpp patched{X}")

    # 3. COMPILAR GODOT-CPP Y CARGAR SU ENVIRONMENT
    print(f"{Y}⚙️ Step 3: Compiling godot-cpp and loading environment...{X}")

    # FORZAR MSVC EN WINDOWS ANTES DE CARGAR GODOT-CPP (usa cache)
    if platform == "windows" and host_platform == "windows":
        if use_msvc:
            print(f"  🔧 Forcing MSVC toolchain for Windows build")
            # Configurar argumentos para forzar MSVC en godot-cpp
            ARGUMENTS['use_msvc'] = 'yes'
            # Remover use_mingw si existe
            if 'use_mingw' in ARGUMENTS:
                del ARGUMENTS['use_mingw']
        else:
            print(f"  🔧 MSVC not available, falling back to MinGW")
            ARGUMENTS['use_mingw'] = 'yes'
    elif platform == "windows" and not use_msvc:
        print(f"  🔧 Setting use_mingw=yes for cross-compilation")
        ARGUMENTS['use_mingw'] = 'yes'

    # Cargar environment de godot-cpp usando SConscript
    print(f"  🔨 Loading godot-cpp environment...")
    env = SConscript("external/godot-cpp/SConstruct")

    print(f"{G}✅ Step 3 complete: godot-cpp compiled and environment loaded{X}")

    # 4. COMPILAR LIBRERÍA GDEXTENSION USANDO ENVIRONMENT DE GODOT-CPP
    print(f"{Y}🏗️ Step 4: Compiling GDExtension library with godot-cpp environment...{X}")

    # Configurar para quitar prefijo "lib" en shared libraries
    env['SHLIBPREFIX'] = ''
    print(f"  🔧 Removed 'lib' prefix from shared library names")

    # Agregar paths del proyecto al environment de godot-cpp
    env.Append(CPPPATH=[
        "src/",
        "include/",
        "external/dojo.c"
    ])

    # Configuración por plataforma para linking adicional
    if platform == "linux":
        env.Append(LINKFLAGS=['-ldbus-1'])
    elif platform == "windows":
        if use_msvc:
            print(f"  🔧 Using MSVC toolchain")
        else:
            print(f"  🔧 Using MinGW toolchain")

    # Buscar fuentes del proyecto - AHORA RECURSIVO pero sin duplicados
    print(f"  🔍 Looking for source files recursively in src/...")

    # Buscar recursivamente en src/ y subdirectorios
    project_sources = []
    project_sources.extend(glob.glob("src/**/*.cpp", recursive=True))

    # Eliminar duplicados usando set y ordenar para consistencia
    project_sources = sorted(list(set(project_sources)))

    print(f"  📁 Found {len(project_sources)} source files:")
    for src in project_sources:
        print(f"    📄 {src}")

    if not project_sources:
        print(f"{R}❌ No source files found in src/ (including subdirectories)!{X}")
        Exit(1)

    # Agregar librería de Rust (dojo.c) - CORREGIDO para linking directo
    build_mode = "release" if target == "template_release" else "debug"

    # Para Windows, linking es diferente entre MSVC y MinGW
    if platform == "windows":
        if rust_target.endswith("-msvc"):
            # MSVC usa .dll.lib para import libraries
            rust_lib_name = "dojo_c.dll.lib"
            print(f"  📚 Using MSVC import library: {rust_lib_name}")
        else:
            # MinGW linkea directamente contra la DLL
            rust_lib_name = "dojo_c.dll"
            print(f"  📚 Using MinGW direct DLL linking: {rust_lib_name}")
    else:
        # Linux/macOS usan librerías estáticas normales
        rust_lib_name = "libdojo_c.a"
        print(f"  📚 Using static library: {rust_lib_name}")

    rust_lib_path = f"external/dojo.c/target/{rust_target}/{build_mode}/{rust_lib_name}"

    if os.path.exists(rust_lib_path):
        env.Append(LIBS=[File(rust_lib_path)])
        print(f"  📚 Linked dojo.c: {rust_lib_path}")
    else:
        print(f"{R}❌ dojo.c library not found: {rust_lib_path}{X}")
        if platform == "windows":
            if rust_target.endswith("-msvc"):
                print(f"  💡 Expected MSVC import library (.dll.lib)")
            else:
                print(f"  💡 Expected DLL for direct MinGW linking")

    # Determinar sufijo de librería basado en plataforma y arquitectura
    if platform == "linux":
        lib_suffix = f".linux.{target}.{arch}.so"
    elif platform == "windows":
        lib_suffix = f".windows.{target}.{arch}.dll"
    elif platform == "macos":
        # macOS usa "universal" para múltiples arquitecturas o específica
        if arch == "arm64":
            lib_suffix = f".macos.{target}.arm64.dylib"
        elif arch == "x86_64":
            lib_suffix = f".macos.{target}.x86_64.dylib"
        else:
            lib_suffix = f".macos.{target}.universal.dylib"

    # Compilar la librería compartida usando environment de godot-cpp
    lib_output = f"demo/bin/{PROJECT_NAME}{lib_suffix}"
    library = env.SharedLibrary(target=lib_output, source=project_sources)

    print(f"  🎯 Target: {lib_output}")
    print(f"{G}✅ Step 4 complete: GDExtension library configured with godot-cpp environment{X}")

    # 5. GENERAR ARCHIVO .GDEXTENSION
    print(f"{Y}📝 Step 5: Generating .gdextension file...{X}")

    # Leer template
    with open("plugin_template.gdextension.in", 'r') as f:
        template_content = f.read()

    # Reemplazar variables
    gdext_content = template_content.replace("${PROJECT_NAME}", PROJECT_NAME)
    gdext_content = gdext_content.replace("${ENTRY_POINT}", ENTRY_POINT)
    gdext_content = gdext_content.replace("${GODOT_MIN_REQUIREMENT}", GODOT_MIN_REQUIREMENT)

    # Guardar archivo
    gdext_output = f"demo/bin/{PROJECT_NAME}.gdextension"
    with open(gdext_output, 'w') as f:
        f.write(gdext_content)

    print(f"  📄 Generated: {gdext_output}")
    print(f"{G}✅ Step 5 complete: .gdextension file generated{X}")

    # 6. COPIAR ARCHIVOS ADICIONALES
    print(f"{Y}📋 Step 6: Copying additional files...{X}")

    # Copiar dojo_c.dll para Windows (nombre es siempre el mismo)
    if platform == "windows":
        dll_name = "dojo_c.dll"  # Nombre siempre igual para Rust
        dll_source = f"external/dojo.c/target/{rust_target}/{build_mode}/{dll_name}"
        dll_dest = f"demo/bin/{dll_name}"

        if os.path.exists(dll_source):
            shutil.copy2(dll_source, dll_dest)
            print(f"  📁 Copied: {dll_source} → {dll_dest}")
        else:
            print(f"{Y}⚠️  Warning: {dll_source} not found{X}")

    print(f"{G}✅ Step 6 complete: Additional files copied{X}")

    # Configurar targets por defecto
    Default(library)

    print(f"{G}🎉 Build configuration complete!{X}")