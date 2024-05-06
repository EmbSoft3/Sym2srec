/**
*
* @copyright Copyright (C) 2024 RENARD Mathieu. All rights reserved.
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
* @file sym2srec_utils.c
* @brief Déclaration des fonctions utilitaires.
* @date 3 mai 2024
*
*/

#include <stdio.h>
#include <stdlib.h>

/**
 *
 */

#include "sym2srec_types.h"
#include "sym2srec_elf.h"
#include "sym2srec_main.h"

/**
 * @internal
 * @brief
 * @endinternal
 */

uint32_t sym2srec_hexatoi ( uint8_t* p_buf, uint32_t p_numberOfDigits )
{
   /* Déclaration de la variable de retour */
   uint32_t l_result = 0;

   /* Déclaration des variables de travail */
   uint32_t l_byte, l_counter;

   /* Pour le nombre de digits à traiter */
   for ( l_counter = 0 ; l_counter < p_numberOfDigits ; l_counter++ )
   {
      /* Décalage de la variable de retour */
      l_result = ( uint32_t ) ( l_result << 4 );

      /* Conversion du caractère courant en entier */
      if ( ( p_buf [ l_counter ] >= '0' ) && ( p_buf [ l_counter ] <= '9' ) )
      {
         l_byte = ( p_buf [ l_counter ] - '0' ) & 0xF;
      }

      else
      {
         l_byte = ( 10 + ( p_buf [ l_counter ] - 'A' ) ) & 0xF;
      }

      /* Actualisation de la variable de retour */
      l_result |= l_byte;
   }

   /* Retour */
   return ( l_result );
}

/**
 * @internal
 * @brief
 * @endinternal
 */

static void sym2srec_reverse ( uint8_t* p_str, uint32_t p_length )
{
   /* Déclaration des variables de travail */
   uint32_t l_start = 0;
   uint32_t l_end = p_length - 1;
   uint8_t l_tmp = 0;

   /* Tant que la chaine n'a pas été permutée */
   while ( l_start < l_end)
   {
      /* Enregistrement du caractére courant */
      l_tmp = ( uint8_t ) *( p_str + l_end );

      /* Echange */
      * ( p_str + l_end )   = * ( p_str + l_start );
      * ( p_str + l_start ) =   ( l_tmp );

      /* Actualisation des variables */
      l_start++;
      l_end--;
   }

   /* Retour */
   return;
}

/**
 * @internal
 * @brief
 * @endinternal
 */

static void sym2srec_hexitoa ( uint32_t p_number, uint8_t* p_result, uint32_t p_digit )
{
   /* Déclaration des variables de travail */
   uint32_t l_offset = 0;
   uint32_t l_remainder = 0;

   /* Si l'entier à convertir est nul */
   if ( p_number == 0 )
   {
      /* Gestion de la valeur nulle */
      p_result [ l_offset++ ] = '0';
   }

   /* Sinon */
   else
   {
      /* Convertion des digits successif en chaine de caractères (base 16) */
      while ( p_number != 0 )
      {
         l_remainder = p_number % 16;

         if ( l_remainder > 0x09 ) p_result [ l_offset++ ] = ( uint8_t ) ( 'A' + ( l_remainder - 0x0A ) );
         else p_result [ l_offset++ ] = ( uint8_t ) ( '0' + l_remainder ) ;

         p_number = p_number / 16;
      }
   }

   /* Pour le nombre de digit à afficher */
   for ( ; l_offset < p_digit ; l_offset++ )
   {
      /* Ecriture des digits de poids fort */
      p_result [ l_offset ] = '0';
   }

   /* Permutation de la chaine */
   sym2srec_reverse ( p_result, l_offset );

   /* Ajout du caractére de fin */
   p_result [ l_offset ] = '\0';

   /* Retour */
   return;
}

/**
 * @internal
 * @brief
 * @endinternal
 */

static uint8_t sym2srec_checksum ( uint32_t p_addr, uint8_t* p_buf, uint32_t p_size )
{
   uint32_t i = 0;
   uint8_t l_computeChecksum = 0;

   /* Réalisation de la somme des octets de la ligne du srec */
   uint32_t l_sum = ( ( p_addr >> 24 ) & 0xFF ) + ( ( p_addr >> 16 ) & 0xFF ) +
         ( ( p_addr >> 8 ) & 0xFF ) + ( ( p_addr >> 0 ) & 0xFF ) + ( ( p_size + 5 ) & 0xFF );

   for ( i = 0 ; ( i < p_size ) && ( p_buf != NULL ) ; i++ ) l_sum = l_sum + p_buf[i];

   /* Détermination de la valeur du checksum */
   l_computeChecksum = 0xFF - ( l_sum & 0xFF );

   /* Retour */
   return ( l_computeChecksum );
}

/**
 * @internal
 * @brief
 * @endinternal
 */

uint32_t sym2srec_addNewRecord ( Elf32FileParser_t* p_parser, uint32_t p_recordAddr, uint8_t* p_buf, uint32_t p_size )
{
   /* Déclaration de la variable de retour */
   uint32_t l_result = K_SYM2SREC_SUCCESS;

   /* Déclaration des variables de travail */
   uint32_t i, j, l_size;

   /* Déclaration d'un record : Type<2> + ByteCount<2> + Addr_data <2*255> + Fin<2> */
   uint8_t l_record [ 517 ], l_str [ 32 ];
   uint8_t l_checksum;

   /* Ecriture de l'entête du record */
   l_record [0] = 'S';
   l_record [1] = '3';

    /* Ecriture de la taille de la ligne */
    l_size = 5 + p_size;
    sym2srec_hexitoa ( l_size, l_str, 2 );
    for ( i = 0 ; i < 2 ; i++ ) l_record [ 2+i ] = l_str [ i ];

    /* Ecriture de l'adresse */
    sym2srec_hexitoa ( p_recordAddr, l_str, 8 );
    for ( i = 0 ; i < 8 ; i++ ) l_record[ 4+i ]= l_str [ i ];

    /* Ecriture des données */
    for ( j=0 ; j < p_size ; j++ )
    {
       sym2srec_hexitoa ( p_buf [ j ], l_str, 2 );
       for ( i = 0 ; i < 2 ; i++ ) l_record [ 12 + ( 2*j ) + i ]= l_str[ i ];
    }

    /* Ecriture du CRC */
    l_checksum = sym2srec_checksum ( p_recordAddr, p_buf, p_size );
    sym2srec_hexitoa ( l_checksum, l_str, 2 );
    for ( i = 0 ; i < 2 ; i++ ) l_record [ 12 + 2*p_size + i ] = l_str [ i ];

    /* Fin de ligne */
    l_record [ 14 + 2 * p_size ] = '\r';
    l_record [ 15 + 2 * p_size ] = '\n';
    l_record [ 16 + 2 * p_size ] = '\0';

    /* Ecriture du record dans le fichier */
    if ( fputs ( ( const char* ) l_record, p_parser->srecFile ) < 0 )
           l_result = K_SYM2SREC_ERROR_WRITE_FUNCTION_FAILED;

    /* Retour */
    return ( l_result );
}

/**
 * @internal
 * @brief
 * @endinternal
 */

uint32_t sym2srec_addHeaderRecord ( Elf32FileParser_t* p_parser, uint32_t p_startAddress )
{
   /* Déclaration de la variable de retour */
   uint32_t l_result = K_SYM2SREC_SUCCESS;

   /* Déclaration des variables de travail */
   uint32_t i;

   /* Déclaration d'un record : Type<2> + ByteCount<2> + Addr_data <2*255> + Fin<2> */
   uint8_t l_record [ 516 ], l_str [ 32 ];
   uint8_t l_checksum;

   /* Ecriture de l'entête du record */
   l_record [0] = 'S';
   l_record [1] = '7';
   l_record [2] = '0';
   l_record [3] = '5';

   /* Ecriture de l'adresse */
   sym2srec_hexitoa ( p_startAddress, l_str, 8 );
   for ( i = 0 ; i < 8 ; i++ ) l_record [ 4 + i ] = l_str [ i ];

   /* Ecriture du CRC */
   l_checksum = sym2srec_checksum ( p_startAddress, NULL, 0);
   sym2srec_hexitoa ( l_checksum, l_str, 2 );
   for ( i = 0 ; i < 2 ; i++ ) l_record [ 12 + i ] = l_str [ i ];

   /* Fin de ligne */
   l_record [ 14 ] = '\r';
   l_record [ 15 ] = '\n';
   l_record [ 16 ] = '\0';

   /* Ecriture du record dans le fichier */
   if ( fputs ( ( const char* ) l_record, p_parser->srecFile ) < 0 )
          l_result = K_SYM2SREC_ERROR_WRITE_FUNCTION_FAILED;

   /* Retour */
   return ( l_result );
}


