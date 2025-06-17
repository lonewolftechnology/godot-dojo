# cmake/PatchDojoHpp.cmake
message(STATUS "=== INICIANDO PATCH DE DOJO.HPP ===")

# Variables recibidas
if(NOT DOJO_HEADER_FILE)
    message(FATAL_ERROR "DOJO_HEADER_FILE no está definido")
endif()

message(STATUS "Archivo objetivo: ${DOJO_HEADER_FILE}")

# Verificar que el archivo existe
if(NOT EXISTS "${DOJO_HEADER_FILE}")
    message(FATAL_ERROR "El archivo ${DOJO_HEADER_FILE} no existe")
endif()

message(STATUS "Archivo encontrado: ${DOJO_HEADER_FILE}")

# Leer el contenido del archivo
file(READ "${DOJO_HEADER_FILE}" HEADER_CONTENT)

# Verificar si ya existe 'struct Clause;'
string(FIND "${HEADER_CONTENT}" "struct Clause;" CLAUSE_FOUND)

if(CLAUSE_FOUND EQUAL -1)
    message(STATUS "Insertando 'struct Clause;' después de 'struct Account;'...")
    
    # Verificar si existe 'struct Account;'
    string(FIND "${HEADER_CONTENT}" "struct Account;" ACCOUNT_FOUND)
    
    if(ACCOUNT_FOUND EQUAL -1)
        message(WARNING "'struct Account;' no encontrado. Insertando 'struct Clause;' al inicio del archivo.")
        set(HEADER_CONTENT "struct Clause;\n${HEADER_CONTENT}")
    else()
        # Usar regex para reemplazar solo la primera ocurrencia
        string(REGEX REPLACE "(struct Account;)" "\\1\nstruct Clause;" HEADER_CONTENT "${HEADER_CONTENT}")
    endif()
    
    # Escribir el archivo modificado
    file(WRITE "${DOJO_HEADER_FILE}" "${HEADER_CONTENT}")
    message(STATUS "¡ÉXITO! 'struct Clause;' ha sido insertado en ${DOJO_HEADER_FILE}")
else()
    message(STATUS "'struct Clause;' ya existe en ${DOJO_HEADER_FILE} - no se necesita modificación")
endif()

message(STATUS "=== PATCH COMPLETADO ===")