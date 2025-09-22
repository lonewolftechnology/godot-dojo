//
// Created by hazel on 26/05/25.
//
#ifndef DOJO_TYPES_H
#define DOJO_TYPES_H

/**
 * @file dojo_types.h
 * Punto de entrada principal para incluir las cabeceras de la librería Dojo.
 *
 * Este archivo aplica un workaround para un problema de compilación en dojo.h
 * y define macros de utilidad para interactuar con la librería.
 */

// Workaround para el problema de `cbindgen` con la declaración anticipada de enums.
// Esto hace que el preprocesador ignore la línea `enum ContractType;` en `dojo.h`.
#define ContractType
#include "dojo.h" // Incluimos la cabecera de la librería con el workaround activo.
#undef ContractType

// Macros de utilidad para facilitar el uso de la librería.
#define DOJO dojo_bindings
#define GET_DOJO_ERROR(result) ((result).err.message)
#define GET_DOJO_OK(result) ((result).ok)
#define GET_DOJO_OK_PTR(result) ((&result).ok)

#endif // DOJO_TYPES_H
