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
* @file sym2srec_hash.c
* @brief Déclaration des fonctions de création de la table de hashage.
* @date 3 mai 2024
* @note https://blogs.oracle.com/solaris/post/gnu-hash-elf-sections
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

static uint32_t sym2srec_hash ( uint8_t* s )
{
   /* Déclaration des variables permettant de calculer le hash du symbole */
   uint32_t h = 5381;
   uint8_t c;

   for ( c = *s; c != '\0'; c = *++s)
      h = h * 33 + c;

   /* Retour */
   return ( h & 0xffffffff );
}

/**
 * @internal
 * @brief
 * @endinternal
 */

static void sym2srec_bloomFilter ( uint32_t* p_bloomFilter, uint32_t p_hash )
{
   /* Déclaration des variables permettant de mettre à jour le filtre de bloom */
   uint32_t l_hash1 = p_hash;
   uint32_t l_hash2 = l_hash1 >> K_SYM2SREC_ELF32_GNUHASH_BLOOMFILTER_SHIFT;
   uint32_t l_index = ( ( l_hash1 / K_SYM2SREC_ELF32_GNUHASH_BLOOMFILTER_MASKWORDS_SIZE ) % K_SYM2SREC_ELF32_GNUHASH_BLOOMFILTER_MASKWORDS_NUMBER );

   uint32_t l_bitmask = ( 1u << ( uint32_t ) ( l_hash1 % K_SYM2SREC_ELF32_GNUHASH_BLOOMFILTER_MASKWORDS_SIZE ) ) |
                        ( 1u << ( uint32_t ) ( l_hash2 % K_SYM2SREC_ELF32_GNUHASH_BLOOMFILTER_MASKWORDS_SIZE ) ) ;

   /* Mise à jour du filtre */
   p_bloomFilter [ l_index ] |= l_bitmask;

   /* Retour */
   return;
}

/**
 * @internal
 * @brief
 * @endinternal
 */

static void sym2srec_addSymbolToList ( ELF32SymbolTableList_t* p_list, ELF32SymbolTableItem_t* p_itemToAdd, ELF32SymbolTableEntry_t* p_entry, uint32_t p_hash, uint32_t p_modulo )
{
   /* Déclaration des variables de travail */
   ELF32SymbolTableItem_t* l_item = p_list->first, *l_previousItem = NULL;

   /* Configuration des attributs de l'élément à ajouter dans la liste */
   p_itemToAdd->entry = p_entry;
   p_itemToAdd->hashValue = p_hash;
   p_itemToAdd->moduloHash = p_hash % p_modulo;
   p_itemToAdd->next = NULL;

   /* Si le symbole répond à au moins une des conditions suivantes : */
   /*   - Symbole de type LOCAL */
   /*   - Symbole de type FILE */
   /*   - Symbole de type UNDEF */

   /* On ajoute l'élement dans la liste de 'local'. */
   if ( ( ( p_entry->stInfo & 0xF ) == 4 /* File */ ) || ( ( ( p_entry->stInfo >> 4 ) & 0xF ) == 0 /* LOCAL */ ) || ( p_entry->stShndx == 0 ) )
   {
      /* Si la liste est vide */
      if ( p_list->local == NULL )
      {
         /* L'élément est le premier de la liste */
         p_list->local = p_itemToAdd;
      }

      /* Sinon */
      else
      {
         /* Ajout de l'élément en fin de liste */
         l_item = p_list->local;

         while ( l_item != NULL )
         {
             l_previousItem = l_item;
             l_item = l_item->next;
         }

         l_previousItem->next = p_itemToAdd;
      }
   }

   /* Sinon, on ajoute l'élément dans la liste de symboles (fonctions, variables ...) */
   else
   {
      /* Si la liste est vide */
      if ( p_list->first == NULL )
      {
         p_list->first = p_itemToAdd;
      }

      /* Sinon */
      else
      {
         /* Parcours de la liste tant que le hash du nouvel élément et supérieur au hash de l'élément analysé */
         while ( ( l_item != NULL ) && ( p_itemToAdd->moduloHash > l_item->moduloHash ) )
         {
            l_previousItem = l_item;
            l_item = l_item->next;
         }

         /* Si l'élément doit étre placé en fin de liste */
         if ( l_item == NULL )
         {
            l_previousItem->next = p_itemToAdd;
         }

         /* Sinon l'élément doit être placé en début de liste */
         else if ( l_previousItem == NULL )
         {
            p_itemToAdd->next = p_list->first;
            p_list->first = p_itemToAdd;
         }

         /* Sinon (milieu de liste) */
         else
         {
            p_itemToAdd->next = l_item;
            l_previousItem->next = p_itemToAdd;
         }
      }
   }

   /* Retour */
   return;
}

/**
 * @internal
 * @brief
 * @endinternal
 */

static uint32_t sym2srec_getLocalListNumberOfItems ( ELF32SymbolTableList_t* p_list )
{
   /* Dclaration des variables de travail */
   ELF32SymbolTableItem_t* l_item = p_list->local;
   uint32_t l_numberOfItems = 0;

   /* Parcours de la liste jusqu'à la fin */
   while ( l_item != NULL )
   {
      l_numberOfItems++;
      l_item = l_item->next;
   }

   /* Retour */
   return ( l_numberOfItems );
}

/**
 * @internal
 * @brief
 * @endinternal
 */

static uint32_t sym2srec_getHashListNumberOfItems ( ELF32SymbolTableList_t* p_list )
{
   /* Dclaration des variables de travail */
    ELF32SymbolTableItem_t* l_item = p_list->first;
    uint32_t l_numberOfItems = 0;

    /* Parcours de la liste jusqu'à la fin */
    while ( l_item != NULL )
    {
        l_numberOfItems++;
        l_item = l_item->next;
    }

    /* Retour */
    return ( l_numberOfItems );
}

/**
 * @internal
 * @brief
 * @endinternal
 */

static uint32_t sym2srec_getSymbolIndexByBucket ( ELF32SymbolTableList_t* p_list, uint32_t p_offset )
{
    /* Déclaration des variables de travail */
    ELF32SymbolTableItem_t* l_item = p_list->first;
    uint32_t l_offset = 0;

    /* Parcours de la liste jusqu'à la fin ou jusqu'à l'offset */
    while ( ( l_item != NULL ) && ( l_item->moduloHash != p_offset ) )
    {
        l_offset++;
        l_item = l_item->next;
    }

    /* Retour */
    return ( l_offset );
}

/**
 * @internal
 * @brief
 * @endinternal
 */

uint32_t sym2srec_createHashTable ( Elf32FileParser_t* p_parser )
{
   /* Déclaration des variables permettant de créer une liste de symboles triées par hash */
   ELF32SymbolTableEntry_t* l_symbolEntry = NULL;
   ELF32SymbolTableItem_t* l_symbolPool = NULL;
   ELF32SymbolTableItem_t* l_currentEntry = NULL;
   ELF32SymbolTableList_t l_list = { NULL };

   /* Déclaration de la variable de retour */
   uint32_t l_result = K_SYM2SREC_SUCCESS;

   /* Déclaration des variables de travail */
   uint8_t* l_symbolName;
   uint32_t l_numberOfInternalSymbols = 0, l_counter = 0, l_hash = 0;

   /* Déclaration d'un filtre de bloom */
   uint32_t l_bloomFilter [ K_SYM2SREC_ELF32_GNUHASH_BLOOMFILTER_MASKWORDS_NUMBER ] = { 0 };

   /* Détermination du nombre de symboles dans la table */
   l_numberOfInternalSymbols = p_parser->symtab->sSize / sizeof ( ELF32SymbolTableEntry_t );

   /* Allocation des 'n' item d'une liste */
   l_symbolPool = ( ELF32SymbolTableItem_t* ) malloc ( l_numberOfInternalSymbols * sizeof ( ELF32SymbolTableItem_t ) );

   /* Si aucune erreur ne s'est produite */
   if ( l_symbolPool != NULL )
   {
      /* Pour tous les symboles présents dans la table des symboles du fichier elf */
      for ( l_counter = 0 ; ( l_counter < l_numberOfInternalSymbols ) ; l_counter++ )
      {
         /* Récupération de l'adresse du symbole en cours d'analyse */
         l_symbolEntry = ( ELF32SymbolTableEntry_t* ) ( ( uint8_t* ) p_parser->buf + p_parser->symtab->sOffset
               + ( uint32_t ) ( l_counter * sizeof ( ELF32SymbolTableEntry_t ) ) );

         /* Récupération du nom du symbole en cours d'analyse */
         l_symbolName = ( uint8_t* ) p_parser->buf + p_parser->strtab->sOffset + l_symbolEntry->stName;

         /* Récupération du hash du symbole */
         l_hash = sym2srec_hash ( l_symbolName );

         /* Actualisation de la valeur du filtre de bloom */
         sym2srec_bloomFilter ( l_bloomFilter, l_hash );

         /* Ajout du symbole dans la liste correspondant à son type */
         sym2srec_addSymbolToList ( &l_list, &l_symbolPool[l_counter], l_symbolEntry, l_hash, K_SYM2SREC_ELF32_GNUHASH_NBUCKET );
      }

      /* Les listes ont été créées, il faut maintenant construire les structures à écrire dans le fichier de sortie */
      /* Configuration de l'entête de la table de hachage */
      p_parser->hashTableHeader.nbuckets = K_SYM2SREC_ELF32_GNUHASH_NBUCKET;
      p_parser->hashTableHeader.symoffset = sym2srec_getLocalListNumberOfItems ( &l_list );
      p_parser->hashTableHeader.bloomSize = K_SYM2SREC_ELF32_GNUHASH_BLOOMFILTER_MASKWORDS_NUMBER;
      p_parser->hashTableHeader.bloomShift = K_SYM2SREC_ELF32_GNUHASH_BLOOMFILTER_SHIFT;
      /* Ecriture du filtre de bloom */
      for ( l_counter = 0 ; l_counter < K_SYM2SREC_ELF32_GNUHASH_BLOOMFILTER_MASKWORDS_NUMBER ; l_counter++ )
         p_parser->hashTableHeader.bloom [ l_counter ] = l_bloomFilter [ l_counter ];
      /* Ecriture de l'index de démarrage de chaque bucket de la table de hachage */
      for ( l_counter = 0 ; l_counter < K_SYM2SREC_ELF32_GNUHASH_NBUCKET ; l_counter++ )
         p_parser->hashTableHeader.buckets [ l_counter ] = sym2srec_getSymbolIndexByBucket ( &l_list, l_counter ) + p_parser->hashTableHeader.symoffset;

      /* Il faut maintenant construire les différentes buckets (entrées) de la table de hachage. */
      /* Allocation d'un buffer de donnée, la désallocation sera effectuée dans la suite du programme */
      p_parser->hashTableHeader.numberOfHash = sym2srec_getHashListNumberOfItems ( &l_list );
      p_parser->hashTableHeader.chain = ( uint32_t* ) malloc ( p_parser->hashTableHeader.numberOfHash * sizeof ( uint32_t ) );

      /* Si aucune erreur ne s'est produite */
      if ( p_parser->hashTableHeader.chain != NULL )
      {
         l_counter = 0;
         l_currentEntry = l_list.first;
         while ( l_currentEntry != NULL )
         {
            /* Ecriture du hash du symbole */
            p_parser->hashTableHeader.chain [ l_counter ] = l_currentEntry->hashValue & 0xFFFFFFFE;
            /* Si dernier symbole de la bucket, on ajoute le marqueur */
            if ( (l_currentEntry->next == NULL) || ( l_currentEntry->moduloHash != l_currentEntry->next->moduloHash ) )
                p_parser->hashTableHeader.chain[l_counter] |= 0x1; /* Fin de liste */
            /*printf ("[%d]-[%d]-%x-%x\n", l_counter, l_currentEntry->moduloHash, l_currentEntry->hashValue, p_parser->hashTableHeader.chain[l_counter]);*/
            l_currentEntry = l_currentEntry->next; l_counter++;
         }
      }
      else l_result = K_SYM2SREC_ERROR_COULD_NOT_ALLOCATE_BUCKETS_MEMORY;

      /* Si aucune erreur ne s'est produite */
      if ( l_result == K_SYM2SREC_SUCCESS )
      {
         /* Il faut maintenant construire la table des symboles triés. */
         /* Allocation d'un nouveau bloc mémoire contenant la table des symboles triées, la désallocation sera effectuée dans la suite du programme. */
         p_parser->sortsymtab = ( ELF32SymbolTableEntry_t* ) malloc ( l_numberOfInternalSymbols * sizeof ( ELF32SymbolTableEntry_t ) );

         /* Si aucune erreur ne s'est produite */
         if ( p_parser->sortsymtab != NULL )
         {
             /* Ecriture des symboles locaux */
             l_counter=0;
             l_currentEntry = l_list.local;
             while ( l_currentEntry != NULL )
             {
                memcpy ( &p_parser->sortsymtab [ l_counter ], l_currentEntry->entry, sizeof( ELF32SymbolTableEntry_t ) );
                l_currentEntry = l_currentEntry->next;
                l_counter++;
             }

             /* Ecriture des symboles utiles */
             l_currentEntry = l_list.first;
             while ( l_currentEntry != NULL )
             {
                memcpy( &p_parser->sortsymtab [ l_counter ], l_currentEntry->entry, sizeof ( ELF32SymbolTableEntry_t ) );
                l_currentEntry = l_currentEntry->next;
                l_counter++;
             }
         }
         else l_result = K_SYM2SREC_ERROR_COULD_NOT_ALLOCATE_SYMBOLS_MEMORY;
      }

      /* Désallocation des listes de symboles */
      free ( l_symbolPool );
    }

    /* Sinon (erreur d'allocation) */
    else l_result = K_SYM2SREC_ERROR_COULD_NOT_ALLOCATE_SYMBOLS_LIST ;

    /* Retour */
    return ( l_result );
}
