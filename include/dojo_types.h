//
// Created by hazel on 26/05/25.
//
#ifndef DOJO_TYPES_H
#define DOJO_TYPES_H
#include "dojo.h" // 1.7.0

#define DOJO dojo_bindings
#define GET_DOJO_ERROR(result) ((result).err.message)
#define GET_DOJO_OK(result) ((result).ok)
#define GET_DOJO_OK_PTR(result) ((&result).ok)
/**
 * @file dojo_types.h
 * Declaraciones forward para los tipos de datos utilizados en Dojo
 *
 * Este archivo contiene las declaraciones forward de todos los tipos de datos
 * principales utilizados en el ecosistema Dojo, incluyendo estructuras para
 * clientes, proveedores, consultas, eventos y manejo de errores.
 *
 * Las declaraciones forward permiten reducir las dependencias de compilación
 * y mejorar los tiempos de compilación al evitar incluir dojo.h completo
 * cuando solo se necesitan referencias a los tipos.
 */

#endif // DOJO_TYPES_H
