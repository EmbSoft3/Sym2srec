# Sym2srec

**Sym2srec** is a CLI program that copies both sections "*.symtab*" and 
"*.strtab*" of an executable file into loadable segments of a new executable 
file. The goal is to allow an external program to run in main memory (dynamic 
loading). 

The software uses a destination address and an **ELF32** file (.elf) as inputs. 
It produces an **S-Record** file (.srec) as output. A hash table 
([.gnu hash](https://flapenguin.me/elf-dt-gnu-hash)) is also produced and copied 
into the S-Record file to accelerate symbol relocations.

### Usage

**Sym2srec** can be run from the command line interface with the following command:

>**sym2srec**  $(**BUILD_ARTIFACT_NAME**).**elf** $(**OUTPUT_ARTIFACT_NAME**).**srec** 
$(**SYMBOL_BASE_ADDR**)

Options are:

>**$(BUILD_ARTIFACT_NAME).elf**: executable file containing the 
symbols to export to the srec file. Format of file is 32bits.

>**$(OUTPUT_ARTIFACT_NAME).srec**: S-Record file to create.

>**$(SYMBOL_BASE_ADDR)**: base address where symbols should be copied. Format 
is 8 digits and 4 bytes alignment (ex: 0x002C0000).

Example:

>**sym2srec**  myApps.elf ../build/myApps.srec 0x002C0000

### Strip

Before running the command, the executable file may be strip with the following 
command line:

>**strip** --discard-all --strip-debug  $(**STRIP_ARTIFACT_NAME**).elf

This command removes all non-global and debug symbols from the artefact.
Strip is a GNU Development Tools, it can be found in the GNU Development 
Toolchain (arm-none-eabi-strip, ...).

### Symbol Resolution

The wiki in the repository describes how a dynamic loader may load external
applications by resolving symbols.

### Build

Sym2srec can be built using the [makefile](sym2srec/make/makefile) file on the repository.
The variable **TOOLCHAIN_PATH** must be updated with the path of the MinGW-W64
toolchain.

First issue a **make clean** command then build the target with **make all**.

Currently versions of compiler used are the followings:

- **gcc** (x86_64-posix-sjlj-rev0, Built by MinGW-W64 project) 8.1.0 
- **g++** (x86_64-posix-sjlj-rev0, Built by MinGW-W64 project) 8.1.0

Please note, the application must be built in 32bits (option **-m32**). 

### License

**Copyright (C)** 2024 **RENARD Mathieu**. All rights reserved.

Sym2srec is free software; There is NO warranty; not even for MERCHANTABILITY or 
FITNESS FOR A PARTICULAR PURPOSE.

The content of this repository is bound by the [BSD-3-Clause](LICENSE.txt) license.
