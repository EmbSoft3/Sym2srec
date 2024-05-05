/**
*
* @copyright Mk - Copyright (C) 2017-2018 RENARD Mathieu.
*
* This file is part of Mk.
*
* Mk is free software: you can redistribute it and/or modify
* it under the terms of the GNU Lesser General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* Mk is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU Lesser General Public License for more details.
*
* You should have received a copy of the GNU Lesser General Public License
* along with Mk.  If not, see <https://www.gnu.org/licenses/>.
*
* @file types.h
* @brief Déclaration des types dédiés au parser de fichier ".elf".
* @date 21 déc. 2023
*
*/


#ifndef TYPES_H
#define TYPES_H

/**
 * @typedef T_int8
 * @brief Surcharge du type "signed char"
 */

typedef signed char T_int8;

/**
 * @typedef T_int16
 * @brief Surcharge du type "signed short"
 */

typedef signed short T_int16;

/**
 * @typedef T_int32
 * @brief Surcharge du type "signed long"
 */

typedef signed long T_int32;

/**
 * @typedef T_int64
 * @brief Surcharge du type "signed long long"
 */

typedef signed long long T_int64;

/**
 * @typedef T_uint8
 * @brief Surcharge du type "unsigned char"
 */

typedef unsigned char T_uint8;

/**
 * @typedef T_uint16
 * @brief Surcharge du type "unsigned short"
 */

typedef unsigned short T_uint16;

/**
 * @typedef T_uint32
 * @brief Surcharge du type "unsigned long"
 */

typedef unsigned long T_uint32;

/**
 * @typedef T_uint64
 * @brief Surcharge du type "unsigned long long"
 */

typedef unsigned long long T_uint64;

/**
 * @typedef T_real32
 * @brief Surcharge du type "float"
 */

typedef float T_real32;

/**
 * @typedef T_real64
 * @brief Surcharge du type "float"
 */

typedef double T_real64;

/**
 * @typedef T_vint8
 * @brief Surcharge du type "volatile signed char"
 */

typedef volatile signed char T_vint8;

/**
 * @typedef T_vint16
 * @brief Surcharge du type "volatile signed short"
 */

typedef volatile signed short T_vint16;

/**
 * @typedef T_vint32
 * @brief Surcharge du type "volatile signed long"
 */

typedef volatile signed long T_vint32;

/**
 * @typedef T_vuint8
 * @brief Surcharge du type "volatile unsigned char"
 */

typedef volatile unsigned char T_vuint8;

/**
 * @typedef T_vuint16
 * @brief Surcharge du type "volatile unsigned short"
 */

typedef volatile unsigned short T_vuint16;

/**
 * @typedef T_vuint32
 * @brief Surcharge du type "volatile unsigned long"
 */

typedef volatile unsigned long T_vuint32;

/**
 * @typedef T_str8
 * @brief Surcharge du type "char"
 */

typedef char* T_str8;

/**
 * @typedef T_str16
 * @brief Surcharge du type "short"
 */

typedef short* T_str16;

/**
 * @typedef T_str8
 * @brief Surcharge du type "char"
 */

typedef long* T_str32;

/**
 * @typedef T_ADDR
 * @brief Surcharge du type "volatile unsigned long*"
 */

typedef unsigned long* T_ADDR;

/**
 * @typedef T_void
 * @brief Déclaration du type T_mkAddr.
 */

typedef void ( *T_void );

#endif // TYPES_H
