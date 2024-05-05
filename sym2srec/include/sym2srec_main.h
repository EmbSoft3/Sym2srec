/**
*
* @copyright Copyright (C) 2024 RENARD Mathieu.
*
* Redistribution and use in source and binary forms, with or without modification,
* are permitted provided that the following conditions are met:
*
*    1. Redistributions of source code must retain the above copyright notice,
*       this list of conditions and the following disclaimer.
*    2. Redistributions in binary form must reproduce the above copyright notice,
*       this list of conditions and the following disclaimer in the documentation
*       and/or other materials provided with the distribution.
*    3. Neither the name of the copyright holder nor the names of its contributors
*       may be used to endorse or promote products derived from this software
*       without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
* ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
* WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
* IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
* INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
* BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
* DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
* LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
* OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
* ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*
* @file sym2srec_main.h
* @brief Déclaration des fonctions dédiées au programme principale.
* @date 3 mai 2024
*
*/

#ifndef SYM2SREC_MAIN_H
#define SYM2SREC_MAIN_H

/**
 * @def K_SYM2SREC_SUCCESS
 * @brief Définition du code de succès 0.
 */

#define K_SYM2SREC_SUCCESS 0x0

/**
 * @def K_SYM2SREC_ERROR_OPEN_FUNCTION_FAILED
 * @brief Définition du code de retour 1.
 */

#define K_SYM2SREC_ERROR_OPEN_FUNCTION_FAILED 0x1

/**
 * @def K_SYM2SREC_ERROR_MALLOC_FUNCTION_FAILED
 * @brief Définition du code de retour 2.
 */

#define K_SYM2SREC_ERROR_MALLOC_FUNCTION_FAILED 0x2

/**
 * @def K_SYM2SREC_ERROR_GETSIZE_FUNCTION_FAILED
 * @brief Définition du code de retour 3.
 */

#define K_SYM2SREC_ERROR_GETSIZE_FUNCTION_FAILED 0x3

/**
 * @def K_SYM2SREC_ERROR_READ_FUNCTION_FAILED
 * @brief Définition du code de retour 4.
 */

#define K_SYM2SREC_ERROR_READ_FUNCTION_FAILED 0x4

/**
 * @def K_SYM2SREC_ERROR_CHECKFILE_FUNCTION_FAILED
 * @brief Définition du code de retour 5.
 */

#define K_SYM2SREC_ERROR_CHECKFILE_FUNCTION_FAILED 0x5

/**
 * @def K_SYM2SREC_ERROR_COULD_NOT_ALLOCATE_SYMBOLS_LIST
 * @brief Définition du code de retour 6.
 */

#define K_SYM2SREC_ERROR_COULD_NOT_ALLOCATE_SYMBOLS_LIST 0x6

/**
 * @def K_SYM2SREC_ERROR_COULD_NOT_ALLOCATE_BUCKETS_MEMORY
 * @brief Définition du code de retour 7.
 */

#define K_SYM2SREC_ERROR_COULD_NOT_ALLOCATE_BUCKETS_MEMORY 0x7

/**
 * @def K_SYM2SREC_ERROR_COULD_NOT_ALLOCATE_SYMBOLS_MEMORY
 * @brief Définition du code de retour 8.
 */

#define K_SYM2SREC_ERROR_COULD_NOT_ALLOCATE_SYMBOLS_MEMORY 0x8

/**
 * @def K_SYM2SREC_ERROR_WRITE_FUNCTION_FAILED
 * @brief Définition du code de retour 9.
 */

#define K_SYM2SREC_ERROR_WRITE_FUNCTION_FAILED 0x9

/**
 * @def K_SYM2SREC_ERROR_CREATE_FUNCTION_FAILED
 * @brief Définition du code de retour 10.
 */

#define K_SYM2SREC_ERROR_CREATE_FUNCTION_FAILED 0xA

/**
 * @def K_SYM2SREC_ERROR_SECTIONS_DO_NOT_EXIST
 * @brief Définition du code de retour 11.
 */

#define K_SYM2SREC_ERROR_SECTIONS_DO_NOT_EXIST 0xB

/**
 * @def K_SYM2SREC_ERROR_INVALID_ELF_FILE
 * @brief Définition du code de retour 12.
 */

#define K_SYM2SREC_ERROR_INVALID_ELF_FILE 0xC
#ifdef _DEBUG

/**
 * @def K_SYM2SREC_VERSION
 * @brief Définition d'une chaine de caractères contenant la version du logiciel
 */

#define K_SYM2SREC_VERSION "Sym2srec version 1.0 (debug) \n"
#else

/**
 * @def K_SYM2SREC_VERSION
 * @brief Définition d'une chaine de caractères contenant la version du logiciel
 */

#define K_SYM2SREC_VERSION "Sym2srec version 1.0 (release) \n"
#endif

/**
 *
 */

uint32_t sym2srec_createSrecord ( Elf32FileParser_t* p_parser, uint8_t* p_addr );
uint32_t sym2srec_createHashTable ( Elf32FileParser_t* p_parser );

/**
 *
 */

uint32_t sym2srec_hexatoi ( uint8_t* p_buf, uint32_t p_numberOfDigits );
uint32_t sym2srec_addNewRecord ( Elf32FileParser_t* p_parser, uint32_t p_recordAddr, uint8_t* p_buf, uint32_t p_size );
uint32_t sym2srec_addHeaderRecord ( Elf32FileParser_t* p_parser, uint32_t p_startAddress );

/**
 *
 */

#endif
