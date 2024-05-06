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
* @file sym2srec_main.c
* @brief Déclaration du programme principale.
* @date 3 mai 2024
*
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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

static uint32_t sym2srec_clean ( Elf32FileParser_t* p_parser, uint32_t p_code )
{
   /* Déclaration de la variable de retour */
   uint32_t l_result = p_code;

   /* Dans cette fonction, les codes de retour ne sont pas vérifié car le programme est en */
   /* mode dégradé. */

   /* Fermeture de l'instance du fichier d'entrée */
   if ( p_parser->file != NULL )
      fclose ( p_parser->file );

   /* Fermeture de l'instance du fichier de sortie */
   if ( p_parser->srecFile != NULL )
      fclose ( p_parser->srecFile );

   /* Désallocation de la mémoire allouée pour lire le fichier d'entrée */
   if ( p_parser->buf != NULL )
      free ( p_parser->buf );

   /* Désallocation de la table de hashage */
   if ( p_parser->hashTableHeader.chain != NULL )
      free ( p_parser->hashTableHeader.chain );

   /* Désallocation de la table des symboles */
   if ( p_parser->sortsymtab != NULL )
      free ( p_parser->sortsymtab );

   /* Affichage d'un message */
   if ( l_result == K_SYM2SREC_SUCCESS ) printf ( "Sym2srec run successfuly.\n" );
   else printf ( "Sym2srec encountered an error (code %dd). See --help.\n", ( int ) l_result );

   /* Retour */
   return ( l_result );
}

/**
 * @internal
 * @brief
 * @endinternal
 */

static uint32_t sym2srec_checkFileHeader ( Elf32FileParser_t* p_parser )
{
   /* Vérification de la validité de l'entête du fichier */
   if ( ( p_parser->fileHeader->eIdentMagicNumber != K_SYM2SREC_ELF32_MAGICNUMBER ) ||
        ( p_parser->fileHeader->eEhSize != K_SYM2SREC_ELF32_FILEHEADER_SIZE ) ||
        ( p_parser->fileHeader->ePhentSize != K_SYM2SREC_ELF32_PROGRAMHEADER_ENTRY_SIZE ) ||
        ( p_parser->fileHeader->eShentSize != K_SYM2SREC_ELF32_SECTIONHEADER_ENTRY_SIZE ) ||
        ( p_parser->fileHeader->eShStrndx == 0 ) )
   {
      /* Actualisation de la variable de retour */
      return ( K_SYM2SREC_ERROR_CHECKFILE_FUNCTION_FAILED );
   }

   /* Retour */
   return ( K_SYM2SREC_SUCCESS );
}

/**
 * @internal
 * @brief
 * @endinternal
 */

static uint32_t sym2srec_getFileSize ( Elf32FileParser_t* p_parser )
{
   /* Déclaration de la variable de retour */
   uint32_t l_numberOfBytesToRead = 0;

   /* Déclaration de la variable de retour locale */
   int32_t l_localResult = K_SYM2SREC_SUCCESS;

   /* Configuration du pointeur de fichier à la fin de celui-ci */
   if ( fseek ( p_parser->file, 0, SEEK_END ) != -1 )
   {
      /* Récupération de la taille du fichier */
      l_localResult = ftell ( p_parser->file );
      if ( l_localResult != -1 ) l_numberOfBytesToRead = ( uint32_t ) l_localResult;

      rewind ( p_parser->file );
   }

   /* Retour */
   return ( l_numberOfBytesToRead );
}

/**
 * @internal
 * @brief
 * @endinternal
 */

static uint32_t sym2srec_launch ( int argc, char *argv[] )
{
   /* Déclaration de la variable de retour */
   uint32_t l_result = K_SYM2SREC_SUCCESS;

   /* Déclaration des variables de travail */
   uint32_t l_numberOfBytesToRead = 0, l_numberOfByteRead = 0;

   /* Déclaration d'un parser de fichier ELF */
   Elf32FileParser_t l_parser = { 0 };

   /* Ouverture du fichier ELF */
   l_parser.file = fopen ( argv[1], "rb" );
   if ( l_parser.file == NULL ) l_result = K_SYM2SREC_ERROR_OPEN_FUNCTION_FAILED;

   /* Récupération de la taille du fichier */
   if ( l_result == K_SYM2SREC_SUCCESS )
   {
      l_numberOfBytesToRead = sym2srec_getFileSize ( &l_parser );
      if ( l_numberOfBytesToRead == 0 ) l_result = K_SYM2SREC_ERROR_GETSIZE_FUNCTION_FAILED;
   }

   /* Allocation dynamique d'un buffer */
   if ( l_result == K_SYM2SREC_SUCCESS )
   {
      l_parser.buf = ( uint8_t* ) malloc ( ( uint32_t ) l_numberOfBytesToRead );
      if ( l_parser.buf == NULL ) l_result = K_SYM2SREC_ERROR_MALLOC_FUNCTION_FAILED;
   }

   /* Lecture du fichier fourni par l'utilisateur */
   if ( l_result == K_SYM2SREC_SUCCESS )
   {
      l_numberOfByteRead = fread ( l_parser.buf, 1, l_numberOfBytesToRead, l_parser.file );
      if ( l_numberOfByteRead != l_numberOfBytesToRead ) l_result = K_SYM2SREC_ERROR_READ_FUNCTION_FAILED;
      else l_parser.fileHeader = ( ELF32FileHeader_t* ) l_parser.buf;
   }

   /* Vérification de la validité du fichier fourni par l'utilisateur */
   if ( l_result == K_SYM2SREC_SUCCESS )
   {
      l_result = sym2srec_checkFileHeader ( &l_parser );
   }

   /* Création du fichier srecord */
   if ( l_result == K_SYM2SREC_SUCCESS )
   {
      /* Création du fichier srecord */
      l_parser.srecFile = fopen ( argv[2], "w+b" );
      if ( l_parser.srecFile == NULL ) l_result = K_SYM2SREC_ERROR_CREATE_FUNCTION_FAILED;
      else setbuf( l_parser.srecFile, NULL);
   }

   /* Réalisation du dump des sections .symtab, .strtab et .gnuhash dans un fichier srecord */
   if ( l_result == K_SYM2SREC_SUCCESS )
   {
      /* argv[3] : Adresse de base où les symboles doivent être ajouté */
      l_result = sym2srec_createSrecord ( &l_parser, ( uint8_t* ) &argv[3][2] );
   }

   /* Libération de la mémoire et des instances allouées */
   sym2srec_clean ( &l_parser, l_result );

   /* Suppression warning */
   ( void ) argc;

   /* Retour */
   return ( l_result );
}

/**
 * @internal
 * @brief
 * @endinternal
 */

int main (int argc, char *argv[])
{
   /* sym2srec [path/fichier.elf] [path/fichier.srec] [baseAddr:0xAABBCCDD] */
   /* argv[0] : Commande */
   /* argv[1] : Elf File */
   /* argv[2] : Srec File */
   /* argv[3] : Base Addr */

   /* Déclaration de la variable de retour */
   uint32_t l_result = K_SYM2SREC_SUCCESS;

   /* Affichage du logo */
   printf ( "\n" );
   printf ( " ____                  ____                    \n" );
   printf ( "/ ___| _   _ _ __ ___ |___ \\ ___ _ __ ___  ___ \n" );
   printf ( "\\___ \\| | | | '_ ` _ \\  __) / __| '__/ _ \\/ __|\n" );
   printf ( " ___) | |_| | | | | | |/ __/\\__ \\ | |  __/ (__ \n" );
   printf ( "|____/ \\__, |_| |_| |_|_____|___/_|  \\___|\\___|\n" );
   printf ( "       |___/                                   \n" );
   printf ( "\n" );

   /* Affichage du prompt */
   printf ( K_SYM2SREC_VERSION );
   printf ( "Copyright (C) 2024 RENARD Mathieu. All rights reserved.\n" );
   printf ( "This is free software; see the source for copying conditions.  There is NO\n" );
   printf ( "warranty; not even for MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.\n" );
   printf ( "\n" );

   /* Si aucun paramètre n'a été fourni, si l'option --help est présente ou */
   /* si le préfixe '0x' n'est pas présent devant l'adresse de base. */
   if ( ( argc < 4 ) || ( strcmp ( argv [ 1 ], "--help" ) == 0 ) ||
        ( argv [ 3 ] [ 0 ] != '0' ) || ( argv [ 3 ] [ 1 ] != 'x' ) )
   {
      printf ( "Sym2srec: nothing to do.\n" );
      printf ( "Usage:\n\n  sym2srec $(BUILD_ARTIFACT_NAME).elf $(BUILD_ARTIFACT_NAME).srec $(SYMBOL_BASE_ADDR) \n\n" );
      printf ( "  $(BUILD_ARTIFACT_NAME).elf  : elf file (32bits) containing the symbols to export to the srec file.\n" );
      printf ( "  $(BUILD_ARTIFACT_NAME).srec : srecord file to create.\n" );
      printf ( "  $(SYMBOL_BASE_ADDR)         : base address where symbols should be stored. Format is 8 digits (ex: 0x002C0000).\n\n");
   }

   /* Sinon */
   else
   {
      /* Lancement du programme */
      l_result = sym2srec_launch ( argc, argv );
   }

   /* Fin du programme */
   exit ( ( int ) l_result );

   /* Retour */
   return ( 0 );
}

