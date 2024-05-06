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
* @file sym2srec_elf.h
* @brief Déclaration des fonctions et des constantes dédiées au parser de fichier elf32.
* @date 3 mai 2024
*
*/

#ifndef SYM2SREC_ELF_H
#define SYM2SREC_ELF_H

/**
 * @def K_SYM2SREC_ELF32_FILEHEADER_SIZE
 * @brief Définition de la taille [en octets] de l'entête d'un fichier ELF 32 bits.
 */

#define K_SYM2SREC_ELF32_FILEHEADER_SIZE 52

/**
 * @def K_SYM2SREC_ELF32_PROGRAMHEADER_ENTRY_SIZE
 * @brief Définition de la taille [en octets] d'une entrée de la 'Program Header Table'.
 */

#define K_SYM2SREC_ELF32_PROGRAMHEADER_ENTRY_SIZE 32

/**
 * @def K_SYM2SREC_ELF32_SECTIONHEADER_ENTRY_SIZE
 * @brief Définition de la taille [en octets] d'une entrée de la 'Section Header Table'.
 */

#define K_SYM2SREC_ELF32_SECTIONHEADER_ENTRY_SIZE 40

/**
 * @def K_SYM2SREC_ELF32_MAGICNUMBER
 * @brief Définition de l'identifiant d'un fichier ELF.
 */

#define K_SYM2SREC_ELF32_MAGICNUMBER 0x464C457F

/**
 * @def K_SYM2SREC_ELF32_GNUHASH_NBUCKET
 * @brief Définition du nombre de buckets de la table de hachage.
 */

#define K_SYM2SREC_ELF32_GNUHASH_NBUCKET 256

/**
 * @def K_SYM2SREC_ELF32_GNUHASH_BLOOMFILTER_SHIFT
 * @brief Définition de la valeur de décalage utilisée dans le calcul du filtre de BLOOM.
 *
 */

#define K_SYM2SREC_ELF32_GNUHASH_BLOOMFILTER_SHIFT 6

/**
 * @def K_SYM2SREC_ELF32_GNUHASH_BLOOMFILTER_MASKWORDS_NUMBER
 * @brief Définition du nombre de mots constituant le champ de bits du filtre de BLOOM.
 */

#define K_SYM2SREC_ELF32_GNUHASH_BLOOMFILTER_MASKWORDS_NUMBER K_SYM2SREC_ELF32_GNUHASH_NBUCKET

/**
 * @def K_SYM2SREC_ELF32_GNUHASH_BLOOMFILTER_MASKWORDS_SIZE
 * @brief Définition de la taille d'un mot [en bits] constituant le champ de bits du filtre de BLOOM.
 */

#define K_SYM2SREC_ELF32_GNUHASH_BLOOMFILTER_MASKWORDS_SIZE 32

/**
 *
 */

/**
 * @enum ELF32FileType_t
 * @brief Déclaration de l'énumération ELF32FileType_t.
 *
 */

typedef enum ELF32FileType_t
{
   K_SYM2SREC_ELF32_ET_NONE = 0,             /*!< Définition du type ET_NONE d'un fichier ELF32. */
   K_SYM2SREC_ELF32_ET_REL = 1,              /*!< Définition du type ET_REL d'un fichier ELF32. */
   K_SYM2SREC_ELF32_ET_EXEC = 2,             /*!< Définition du type ET_EXEC d'un fichier ELF32. */
   K_SYM2SREC_ELF32_ET_DYN = 3,              /*!< Définition du type ET_DYN d'un fichier ELF32. */
   K_SYM2SREC_ELF32_ET_CORE = 4              /*!< Définition du type ET_CORE d'un fichier ELF32. */
} ELF32FileType_t;

/**
 * @struct ELF32FileHeader_t
 * @brief Déclaration de la structure ELF32FileHeader_t.
 *
 */

typedef struct ELF32FileHeader_t ELF32FileHeader_t;
struct ELF32FileHeader_t
{
   uint32_t  eIdentMagicNumber;              /*!< Magic Number :  0x7F, 0x45, 0x4C, 0x46). */
   uint8_t   eIdentClass;                    /*!< Format du fichier : 32bits <1> ou 64 bits <2>. */
   uint8_t   eIdentData;                     /*!< Endianness : little <1>, big <2>. */
   uint8_t   eIdentVersion;                  /*!< Version. */
   uint8_t   eIdentOSABI;                    /*!< OS Cible. */
   uint8_t   eIdentABIVersion;               /*!< Version de l'ABI. */
   uint8_t   eIdentPadding [7];              /*!< Padding. */
   uint16_t  eType;                          /*!< Type : No_File_Type <0>, Relocatable_File <1>, Execution_File <2>, Shared_Object_File <3>, Core_File <4>, Processor-specific <0xFF00 ou 0xFFFF>. */
   uint16_t  eMachine;                       /*!< Instruction Set : ARM <0x28>. */
   uint32_t  eVersion;                       /*!< Version : Invalide <0>, Valide <1>. */
   uint32_t  eEntry;                         /*!< Adresse de démarrage du programme. */
   uint32_t  ePhoff;                         /*!< Offset de la 'Programm header table'. */
   uint32_t  eShoff;                         /*!< Offset de la 'Section header table'. */
   uint32_t  eFlags;                         /*!< Flags : fonction de l'architecture. */
   uint16_t  eEhSize;                        /*!< Taille de cette entête (52 bytes for 32bits). */
   uint16_t  ePhentSize;                     /*!< Taille d'une entrée de la 'Programm header table'. */
   uint16_t  ePhnum;                         /*!< Nombre d'entrées dans la 'Programm header table'. */
   uint16_t  eShentSize;                     /*!< Taille d'une entrée de la 'Section header table'. */
   uint16_t  eShnum;                         /*!< Nombre d'entrées dans la 'Section header table'. */
   uint16_t  eShStrndx;                      /*!< Index de l'entrée de la 'Section header table' qui contient le nom des sections. */
};

/**
 * @struct ELF32SectionHeaderEntry_t
 * @brief Déclaration de la structure ELF32SectionHeaderEntry_t.
 * @note Il y a un descripteur de section (\ref ELF32SectionHeaderEntry_t ) pour chaque section qui est présente dans le fichier. Attention,
 * il peut exister des descripteurs de sections qui ne possédent pas de section associée.
 *
 */

typedef struct ELF32SectionHeaderEntry_t ELF32SectionHeaderEntry_t;
struct ELF32SectionHeaderEntry_t
{
   uint32_t  sName;                          /*!< Offset vers la chaine de caractères contenant le nom de la section. */
   uint32_t  sType;                          /*!< Type de l'entrée (PROGRAM_DATA <1>, SYMBOL_TABLE <2>, .... */
   uint32_t  sFlags;                         /*!< Attributs de la section (Writable <1>, Alloc <2>, ExecInstr <4> ...). */
   uint32_t  sAddr;                          /*!< Adresse de la section si celle-ci est chargée sinon 0. */
   uint32_t  sOffset;                        /*!< Offset de la section dans le fichier. */
   uint32_t  sSize;                          /*!< Taille de la section dans le fichier [en octets]. Peut étre 0. */
   uint32_t  sLink;                          /*!< Index d'une section associée. La signification dépend du type de section. */
   uint32_t  sInfo;                          /*!< Information complémentaires liée à la section. La signification dépend du type de section. */
   uint32_t  sAddrAlign;                     /*!< Alignement de la section en mémoire : No_alignement <0> ou <1>, sinon 2^p_Align. */
   uint32_t  sEntrySize;                     /*!< Taille des entrées d'une table de taille fixe (table des symboles par exemple) en octets. Valeur 0 si non applicable. */
};

/**
 *
 */

/**
 * @enum ELF32SegmentType_t
 * @brief Déclaration de l'énumération ELF32SegmentType_t.
 *
 */

typedef enum ELF32SegmentType_t
{
   PT_NULL = 0,                              /*!< Définition du type PT_NULL d'un segment de la 'Program Header Table'. */
   PT_LOAD = 1,                              /*!< Définition du type PT_LOAD d'un segment de la 'Program Header Table'. */
   PT_DYNAMIC = 2,                           /*!< Définition du type PT_DYNAMIC d'un segment de la 'Program Header Table'. */
   PT_INTERP = 3,                            /*!< Définition du type PT_INTERP d'un segment de la 'Program Header Table'. */
   PT_NOTE = 4,                              /*!< Définition du type PT_NOTE d'un segment de la 'Program Header Table'. */
   PT_SHLIB = 5,                             /*!< Définition du type PT_SHLIB d'un segment de la 'Program Header Table'. */
   PT_PHDR = 6                               /*!< Définition du type PT_PHDR d'un segment de la 'Program Header Table'. */
} ELF32SegmentType_t;

/**
 * @struct ELF32ProgramHeaderEntry_t
 * @brief Déclaration de la structure ELF32ProgramHeaderEntry_t.
 * @note La table 'Program Header' est consituée de plusieurs entrées. Elles indiquent au système
 *       comment créer l'image à télécharger à partir de segments. Chaque entrée décrit les caractéristiques d'un segment.
 *       Cette table n'a de signification que pour les types de fichiers 'Executable File' et "Shared Object File'.
 *
 */

typedef struct ELF32ProgramHeaderEntry_t ELF32ProgramHeaderEntry_t;
struct ELF32ProgramHeaderEntry_t
{
   uint32_t  pType;                          /*!< Type de l'entrée : PT_NULL <0>, PT_LOAD <1>, PT_DYNAMIC <2>, PT_PHDR <6>, ... */
   uint32_t  pOffset;                        /*!< Offset du segment de données dans le fichier. */
   uint32_t  pVirtualAddr;                   /*!< Adresse virtuelle du segment. */
   uint32_t  pPhysicalAddr;                  /*!< Adresse physique du segment. */
   uint32_t  pFileSize;                      /*!< Taille du segment dans le fichier [en octets]. Peut être 0. */
   uint32_t  pMemSize;                       /*!< Taille du segment dans la mémoire [en octets]. Peut être 0. */
   uint32_t  pFlags;                         /*!< Attributs du segment : Executable <1>, Writeable <2>, Readeable <4>. */
   uint32_t  pAlign;                         /*!< Alignement du segment en mémoire : No_alignement <0> ou <1>, sinon 2^p_Align. */
};

/**
 * @struct ELF32SymbolTableEntry_t
 * @brief Déclaration de la structure ELF32SymbolTableEntry_t.
 * @note Structure pointée par une section de type SHT_SYMTAB ou SHT_DYNSYM.
 *
 */

typedef struct ELF32SymbolTableEntry_t ELF32SymbolTableEntry_t;
struct ELF32SymbolTableEntry_t
{
   uint32_t  stName;                         /*!< Offset vers la chaine de caractères contenant le nom du symbole. */
   uint32_t  stValue;                        /*!< Valeur du symbole (valeur, adresse, ...) . */
   uint32_t  stSize;                         /*!< Taille du symbole [en octets]. Peut être 0. */
   uint8_t   stInfo;                         /*!< Type <3:0> (OBJECT, FUNC, SECTION, FILE, ...) et attributs de 'Binding' <7:4> du symbole (STB_LOCAL, STB_GLOBAL, STB_WEAK, ...). */
   uint8_t   stOther;                        /*!< Non implémenté (valeur 0). */
   uint16_t  stShndx;                        /*!< Index de l'entrée de la 'Section Header Table' auquel appartient le symbole. */
};

 /**
 * @struct GNUHashTable_t
 * @brief Déclaration de l'énumération GNUHashTable_t.
 *
 */

typedef struct GNUHashTable_t GNUHashTable_t;
struct GNUHashTable_t
{
    uint32_t  nbuckets;                                                             /*!< Contient le nombre de buckets de la table de hachage. */
    uint32_t  symoffset;                                                            /*!< Contient l'offset du premier symbole. */
    uint32_t  bloomSize;                                                            /*!< Contient la taille du filtre de bloom (en multiple de mots 32bits) . */
    uint32_t  bloomShift;                                                           /*!< Contient la valeur de décalage utilisée pour calculer le filtre de bloom. */
    uint32_t  bloom   [ K_SYM2SREC_ELF32_GNUHASH_BLOOMFILTER_MASKWORDS_NUMBER ];    /*!< Contient les bits du filtre de bloom. */
    uint32_t  buckets [ K_SYM2SREC_ELF32_GNUHASH_NBUCKET ];                         /*!< Contient l'index du premier symbole d'une bucket. */
    uint32_t* chain;                                                                /*!< Contient l'adresse de stockage des hash */
    uint32_t  numberOfHash;                                                         /*!< Contient le nombre de hash. */
};

/**
 * @struct ELF32SymbolTableItem_t
 * @brief Déclaration de la structure ELF32SymbolTableItem_t.
 *
 */

typedef struct ELF32SymbolTableItem_t ELF32SymbolTableItem_t;
struct ELF32SymbolTableItem_t
{
    uint32_t hashValue;                      /*!< Contient le hash du symbole. */
    uint32_t moduloHash;                     /*!< Contient l'index de l'entrée dans la table de hachage (index de bucket). */
    ELF32SymbolTableEntry_t* entry;          /*!< Contient un pointeur sur l'entrée contenant les caractéristiques du symboles. */
    ELF32SymbolTableItem_t* next;            /*!< Contient un pointeur sur le prochain élément de la table de hachage. */
};

/**
 * @struct ELF32SymbolTableList_t
 * @brief Déclaration de la structure ELF32SymbolTableList_t.
 *
 */

typedef struct ELF32SymbolTableList_t ELF32SymbolTableList_t;
struct ELF32SymbolTableList_t
{
    ELF32SymbolTableItem_t* local;           /*!< Contient le premier élément présent dans la table des symboles locaux (LOCAL, FILE, UNDEF). */
    ELF32SymbolTableItem_t* first;           /*!< Contient le premier élément présent dans la table des symboles classique (FUNC, DATA, ...). */
};

/**
 * @struct SymbolsAreaHeader_t
 * @brief Déclaration de l'énumération SymbolsAreaHeader_t.
 *
 */

typedef struct SymbolsAreaHeader_t SymbolsAreaHeader_t;
struct SymbolsAreaHeader_t
{
   uint32_t  magicNumber;                    /*!< Contient la séquence d'identification de l'entête. */
   uint32_t  headerSize;                     /*!< Contient la taille de l'entête [en octets]. */
   uint32_t  padding;                        /*!< Padding. */
   uint32_t  version;                        /*!< Version de l'entête */
   uint32_t  symtabBaseAddr;                 /*!< Adresse de base de la section .symtab. */
   uint32_t  symtabSize;                     /*!< Taille de la section symtab [en octets]. */
   uint32_t  strtabBaseAddr;                 /*!< Adresse de base de la section .strtab. */
   uint32_t  strtabSize;                     /*!< Taille de la section strtab [en octets]. */
   uint32_t  gnuHashBaseAddr;                /*!< Adresse de base de la section .gnuhash. */
   uint32_t  gnuHashSize;                    /*!< Taille de la section .gnuhash [en octets]. */
};

/**
 * @struct Elf32FileParser_t
 * @brief Déclaration de la structure Elf32FileParser_t.
 *
 */

typedef struct Elf32FileParser_t Elf32FileParser_t;
struct Elf32FileParser_t
{
   uint8_t* buf;                             /*!< Ce membre contient le contenu du fichier elf. */
   uint32_t baseAddr;                        /*!< Ce membre contient l'adresse de base des symboles. */
   FILE* file;                               /*!< Ce membre contient l'instance du fichier elf fourni par l'utilisateur. */
   FILE* srecFile;                           /*!< Ce membre contient l'instance du fichier srec à créer. */
   ELF32FileHeader_t* fileHeader;            /*!< Ce membre contient l'entête du fichier ELF32. */
   ELF32SectionHeaderEntry_t* symtab;        /*!< Ce membre contient l'adresse de la première entrée symtab. */
   ELF32SectionHeaderEntry_t* strtab;        /*!< Ce membre contient l'adresse de la première entrée strtab. */
   ELF32SymbolTableEntry_t* sortsymtab;      /*!< Ce membre contient l'adresse de la première entrée symtab triée. */
   GNUHashTable_t hashTableHeader;           /*!< Ce membre contient l'entête de la GNUHashTable. */
};

/**
 *
 */

#endif


