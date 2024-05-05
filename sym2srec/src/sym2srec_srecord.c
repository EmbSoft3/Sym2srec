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
* @file sym2srec_srecord.c
* @brief Déclaration des fonctions de création du fichier srec de sortie.
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
 *
 */

static void sym2srec_getSectionsAddr ( Elf32FileParser_t* p_parser )
{
   /* Déclaration des variables de travail */
   uint32_t i = 0;
   uint8_t* l_sectionName = NULL;

   /* Déclaration des descripteurs de sections */
   ELF32SectionHeaderEntry_t* l_entry = NULL;
   ELF32SectionHeaderEntry_t* l_stringTableEntry = NULL;

   /* Initialisation des données de retour */
   p_parser->symtab = NULL;
   p_parser->strtab = NULL;

   /* Récupération de l'adresse de la section strtab */
   l_stringTableEntry = ( ELF32SectionHeaderEntry_t* )
       ( ( uint8_t* ) p_parser->buf + ( uint32_t ) ( ( uint32_t ) p_parser->fileHeader->eShoff + ( ( uint32_t ) p_parser->fileHeader->eShStrndx * ( uint32_t ) p_parser->fileHeader->eShentSize ) ) );

   /* Pour le nombre de sections présent dans le fichier */
   for ( i = 0 ; i < p_parser->fileHeader->eShnum ; i++ )
   {
       /* Récupération de la valeur et du nom de l'entrée courante */
       l_entry = ( ELF32SectionHeaderEntry_t* ) ( ( uint8_t* ) p_parser->buf + ( uint32_t ) ( p_parser->fileHeader->eShoff + ( i * p_parser->fileHeader->eShentSize ) ) );
       l_sectionName = ( uint8_t* ) p_parser->buf + l_stringTableEntry->sOffset + l_entry->sName;

       /* Enregistrent de l'offset des sections '.symtab' ou 'strtab' dans le parser. */
       if ( strcmp ( ( char* ) l_sectionName, ".symtab" ) == 0 ) p_parser->symtab = l_entry;
       else if ( strcmp ( ( char* ) l_sectionName, ".strtab" ) == 0 ) p_parser->strtab = l_entry;
       else { };
   }

   /* Retour */
   return;
}

/**
 * @internal
 * @brief
 * @endinternal
 */

static uint32_t sym2srec_writeSegments ( Elf32FileParser_t* p_parser )
{
  uint32_t l_counter;
  uint32_t l_addr, l_size, l_chunkSize;
  uint8_t* l_buf;

  /* Déclaration de la variable de retour */
  uint32_t l_result = K_SYM2SREC_SUCCESS;

  /* Déclaration d'un pointeur d'entrée */
  ELF32ProgramHeaderEntry_t* l_entry = NULL;

  /* Pour le nombre d'entrée dans la 'Program Header Table' */
  for ( l_counter = 0 ; ( l_counter < p_parser->fileHeader->ePhnum ) && ( l_result == K_SYM2SREC_SUCCESS ) ; l_counter++ )
  {
     /* Récupération de la valeur de l'entrée */
     l_entry = ( ELF32ProgramHeaderEntry_t* ) ( ( uint8_t* ) p_parser->buf + ( p_parser->fileHeader->ePhoff + ( l_counter * p_parser->fileHeader->ePhentSize ) ) );

     /* Si le type de l'entrée est 'LOAD', 'DYNAMIC' ou 'INTERP' */
     if ( ( l_entry->pType == PT_LOAD ) || ( l_entry->pType == PT_DYNAMIC ) || ( l_entry->pType == PT_INTERP ) )
     {
        /* Si une copie doit être effectuée dans la mémoire */
        if ( ( l_entry->pFileSize <= l_entry->pMemSize ) && ( l_entry->pFileSize != 0 )  )
        {
           /* Configuration des variables */
           l_addr = l_entry->pPhysicalAddr;
           l_buf = p_parser->buf + l_entry->pOffset;
           l_size = l_entry->pFileSize;

           /* Copie du segment dans le fichier de sortie */
           while ( ( l_size != 0 ) && ( l_result == K_SYM2SREC_SUCCESS ) )
           {
              /* Détermination du nombre de données dans une ligne */
              if ( l_size > 16 ) l_chunkSize = 16;
              else l_chunkSize = l_size;
              /* Ajout d'une nouvelle ligne dans le fichier SREC */
              l_result = sym2srec_addNewRecord ( p_parser, l_addr, l_buf, l_chunkSize );
              /* Actualisation des variables */
              l_buf = ( uint8_t* ) l_buf + l_chunkSize;
              l_size = l_size - l_chunkSize;
              l_addr = l_addr + l_chunkSize;
           }
        }
     }
  }

  /* Retour */
  return ( l_result );
}

/**
 * @internal
 * @brief
 * @endinternal
 */

static uint32_t sym2srec_writeHeader ( Elf32FileParser_t* p_parser, uint32_t p_baseAddr, uint32_t p_symtabAddr, uint32_t p_strtabAddr, uint32_t p_gnuHashAddr )
{
   uint8_t* l_buf = NULL;
   uint32_t l_size, l_chunkSize, l_addr;

   /* Déclaration de la variable de retour */
   uint32_t l_result = K_SYM2SREC_SUCCESS;

   /* Déclaration d'une entête */
   SymbolsAreaHeader_t l_header;

   /* Configuration de l'entête */
   l_header.magicNumber = 0x53594D42; /* SYMB */
   l_header.headerSize = 40;
   l_header.padding = 0xFFFFFFFF;
   l_header.version = 1;
   l_header.symtabBaseAddr = p_symtabAddr;
   l_header.symtabSize = p_parser->symtab->sSize;
   l_header.strtabBaseAddr = p_strtabAddr;
   l_header.strtabSize = p_parser->strtab->sSize;
   l_header.gnuHashBaseAddr = p_gnuHashAddr;
   l_header.gnuHashSize = sizeof ( GNUHashTable_t ) + ( p_parser->hashTableHeader.numberOfHash * sizeof ( uint32_t ) ) - 2*sizeof ( uint32_t );

   /* Initialisation des variables */
   l_size = l_header.headerSize;
   l_addr = p_baseAddr;
   l_buf = ( uint8_t* ) &l_header;

   /* Tant que l'intégralité de l'entête n'a pas été copiée dans le fichier srecord */
   while ( ( l_size != 0 ) && ( l_result == K_SYM2SREC_SUCCESS ) )
   {
      /* Détermination du nombre de données dans une ligne */
      if ( l_size > 16 ) l_chunkSize = 16;
      else l_chunkSize = l_size;

      /* Ajout d'une nouvelle ligne dans le fichier SREC */
      l_result = sym2srec_addNewRecord ( p_parser, l_addr, l_buf, l_chunkSize );

      /* Actualisation des variables */
      l_buf = ( uint8_t* ) l_buf + l_chunkSize;
      l_size = l_size - l_chunkSize;
      l_addr = l_addr + l_chunkSize;
   }

   /* Retour */
   return ( l_result );
}

/**
 * @internal
 * @brief
 * @endinternal
 */

static uint32_t sym2srec_writeSymbolTable ( Elf32FileParser_t* p_parser, uint32_t p_symtabAddr )
{
   uint8_t* l_buf = NULL;
   uint32_t l_size, l_chunkSize, l_addr;

   /* Déclaration de la variable de retour */
   uint32_t l_result = K_SYM2SREC_SUCCESS;

   /* Initialisation des variables */
   l_buf = ( uint8_t* ) p_parser->sortsymtab;
   l_size = p_parser->symtab->sSize;
   l_chunkSize = 0;
   l_addr = p_symtabAddr;

   /* Tant que l'intégralité de la section '.symtab' n'a pas été copiée dans le fichier srecord */
   while ( ( l_size != 0 ) && ( l_result == K_SYM2SREC_SUCCESS ) )
   {
      /* Détermination du nombre de données dans une ligne */
      if ( l_size > 16 ) l_chunkSize = 16;
      else l_chunkSize = l_size;

      /* Ajout d'une nouvelle ligne dans le fichier SREC */
      l_result = sym2srec_addNewRecord ( p_parser, l_addr, l_buf, l_chunkSize );

      /* Actualisation des variables */
      l_buf = ( uint8_t* ) l_buf + l_chunkSize;
      l_size = l_size - l_chunkSize;
      l_addr = l_addr + l_chunkSize;
   }

   /* Retour */
   return ( l_result );
}

/**
 * @internal
 * @brief
 * @endinternal
 */

static uint32_t sym2srec_writeStrTable ( Elf32FileParser_t* p_parser, uint32_t p_strtabAddr )
{
   uint8_t* l_buf = NULL;
   uint32_t l_size, l_chunkSize, l_addr;

   /* Déclaration de la variable de retour */
   uint32_t l_result = K_SYM2SREC_SUCCESS;

   /* Initialisation des variables */
   l_buf = ( uint8_t* ) p_parser->buf + p_parser->strtab->sOffset;
   l_size = p_parser->strtab->sSize;
   l_chunkSize = 0;
   l_addr = p_strtabAddr;

   /* Tant que l'intégralité de la section '.strtab' n'a pas été copiée dans le fichier srecord */
   while ( ( l_size != 0 ) && ( l_result == K_SYM2SREC_SUCCESS ) )
   {
      /* Détermination du nombre de données dans une ligne */
      if ( l_size > 16 ) l_chunkSize = 16;
      else l_chunkSize = l_size;

      /* Ajout d'une nouvelle ligne dans le fichier SREC */
      l_result = sym2srec_addNewRecord ( p_parser, l_addr, l_buf, l_chunkSize );

      /* Actualisation des variables */
      l_buf = ( uint8_t* ) l_buf + l_chunkSize;
      l_size = l_size - l_chunkSize;
      l_addr = l_addr + l_chunkSize;
   }

   /* Retour */
   return ( l_result );
}

/**
 * @internal
 * @brief
 * @endinternal
 */

static uint32_t sym2srec_writeGnuHashTable ( Elf32FileParser_t* p_parser, uint32_t p_gnuHashAddr )
{
   uint8_t* l_buf = NULL;
   uint32_t l_size, l_chunkSize, l_addr;

   /* Déclaration de la variable de retour */
   uint32_t l_result = K_SYM2SREC_SUCCESS;

   /* Initialisation des variables */
   l_buf = ( uint8_t* ) &p_parser->hashTableHeader;
   l_size = sizeof (GNUHashTable_t) - ( 2 * sizeof ( uint32_t ) );
   l_chunkSize = 0;
   l_addr = p_gnuHashAddr;

   /* Tant que l'entête de la table n'a pas été copiée dans le fichier srecord */
   while ( ( l_size != 0 ) && ( l_result == K_SYM2SREC_SUCCESS ) )
   {
      /* Détermination du nombre de données dans une ligne */
      if ( l_size > 16 ) l_chunkSize = 16;
      else l_chunkSize = l_size;

      /* Ajout d'une nouvelle ligne dans le fichier SREC */
      l_result = sym2srec_addNewRecord ( p_parser, l_addr, l_buf, l_chunkSize );

      /* Actualisation des variables */
      l_buf = ( uint8_t* ) l_buf + l_chunkSize;
      l_size = l_size - l_chunkSize;
      l_addr = l_addr + l_chunkSize;
   }

   /* Initialisation des variables */
   l_buf = ( uint8_t* ) p_parser->hashTableHeader.chain;
   l_size = p_parser->hashTableHeader.numberOfHash * sizeof ( uint32_t );
   l_chunkSize = 0;
   l_addr = p_gnuHashAddr + sizeof ( GNUHashTable_t ) - ( 2 * sizeof ( uint32_t ) );

   /* Tant que la table n'a pas été copiée dans le fichier srecord */
   while ( ( l_size != 0 ) && ( l_result == K_SYM2SREC_SUCCESS ) )
   {
      /* Détermination du nombre de données dans une ligne */
      if ( l_size > 16 ) l_chunkSize = 16;
      else l_chunkSize = l_size;

      /* Ajout d'une nouvelle ligne dans le fichier SREC */
      l_result = sym2srec_addNewRecord ( p_parser, l_addr, l_buf, l_chunkSize );

      /* Actualisation des variables */
      l_buf = ( uint8_t* ) l_buf + l_chunkSize;
      l_size = l_size - l_chunkSize;
      l_addr = l_addr + l_chunkSize;
   }

   /* Retour */
   return ( l_result );
}

/**
 * @internal
 * @brief
 * @endinternal
 */

uint32_t sym2srec_createSrecord ( Elf32FileParser_t* p_parser, uint8_t* p_baseAddr )
{
   /* Déclaration de la variable de retour */
   uint32_t l_result = K_SYM2SREC_SUCCESS;

   /* Déclaration des pointeurs d'adresse de chaque section */
   uint32_t l_symtabAddr, l_strtabAddr, l_gnuHashAddr;

   /* Conversion de l'adresse en entier */
   p_parser->baseAddr = sym2srec_hexatoi ( p_baseAddr, 8 );

   /* Récupération de l'adresse des sections '.symtab' et '.strtab'. */
   sym2srec_getSectionsAddr ( p_parser );

   /* Si au moins une des 2 sections n'est pas présentes dans le fichier elf */
   if ( ( p_parser->strtab == NULL ) || ( p_parser->symtab == NULL ) ) l_result = K_SYM2SREC_ERROR_SECTIONS_DO_NOT_EXIST;

   /* Si aucune erreur ne s'est produite */
   if ( l_result == K_SYM2SREC_SUCCESS )
   {
      /* Si aucun probléme n'alignement n'est présent */
      if ( ( ( p_parser->baseAddr % p_parser->strtab->sAddrAlign ) != 0 ) ||
           ( ( p_parser->baseAddr % p_parser->symtab->sAddrAlign ) != 0 ) ||
           ( p_parser->strtab->sSize == 0 ) ||
           ( p_parser->symtab->sSize == 0 ) ) l_result = K_SYM2SREC_ERROR_INVALID_ELF_FILE;
   }

   /* Création de la table de hachage */
   if ( l_result == K_SYM2SREC_SUCCESS ) l_result = sym2srec_createHashTable ( p_parser );

   /* Ecriture des segments de données dans le fichier srecord */
   if ( l_result == K_SYM2SREC_SUCCESS ) l_result = sym2srec_writeSegments ( p_parser );

   /* Détermination des adresses des 3 sections */
   if ( l_result == K_SYM2SREC_SUCCESS )
   {
      /* Détermination de l'adresse de la section .symtab */
      l_symtabAddr = p_parser->baseAddr + sizeof ( SymbolsAreaHeader_t );
      l_symtabAddr = l_symtabAddr + ( p_parser->symtab->sAddrAlign - ( l_symtabAddr % p_parser->symtab->sAddrAlign ) );

      /* Détermination de l'adresse de la section .strtab */
      l_strtabAddr = l_symtabAddr + p_parser->symtab->sSize;
      l_strtabAddr = l_strtabAddr + ( p_parser->strtab->sAddrAlign - ( l_strtabAddr % p_parser->strtab->sAddrAlign ) );

      /* Détermination de l'adresse de la section .gnuhash */
      l_gnuHashAddr = l_strtabAddr + p_parser->strtab->sSize;
      l_gnuHashAddr = l_gnuHashAddr + ( sizeof ( uint32_t ) - ( l_gnuHashAddr % sizeof ( uint32_t ) ) );
   }

   /* Ecriture de l'entête dans le fichier srecord */
   if ( l_result == K_SYM2SREC_SUCCESS ) l_result = sym2srec_writeHeader ( p_parser, p_parser->baseAddr, l_symtabAddr, l_strtabAddr, l_gnuHashAddr );

   /* Ecriture de la section .symtab dans le fichier srecord */
   if ( l_result == K_SYM2SREC_SUCCESS ) l_result = sym2srec_writeSymbolTable ( p_parser, l_symtabAddr );

   /* Ecriture de la section .strtab dans le fichier srecord */
   if ( l_result == K_SYM2SREC_SUCCESS ) l_result = sym2srec_writeStrTable ( p_parser, l_strtabAddr );

   /* Ecriture de la section .gnuhash dans le fichier srecord */
   if ( l_result == K_SYM2SREC_SUCCESS ) l_result = sym2srec_writeGnuHashTable ( p_parser, l_gnuHashAddr );

   /* Ecriture du point d'entrée dans le fichier srec */
   if ( l_result == K_SYM2SREC_SUCCESS ) sym2srec_addHeaderRecord ( p_parser, p_parser->fileHeader->eEntry );

   /* Retour */
   return ( l_result );
 }


