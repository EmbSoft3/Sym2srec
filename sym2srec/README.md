# Sym2srec

The purpose of Sym2srec is to insert both sections "**.symtab**" and "**.strtab**"
into a new executable file.

The software uses a destination address and an ELF32 file as inputs. It produces 
an S-Record (.srec) file as output. A hash table ([.gnu hash](https://flapenguin.me/elf-dt-gnu-hash)) 
is also produced and inserted into the S-Record file.

### Usage

Sym2srec can be run with the following command line:

>**sym2srec**  $(**BUILD_ARTIFACT_NAME**).elf $(**BUILD_ARTIFACT_NAME**).srec 
$(**SYMBOL_BASE_ADDR**)

Options are:

>**$(BUILD_ARTIFACT_NAME).elf**: executable file (Elf 32bits) containing the 
symbols to export to the srec file.

>**$(BUILD_ARTIFACT_NAME).srec**: S-Record file to create.

>**$(SYMBOL_BASE_ADDR)**: base address where symbols should be stored. Format 
is 8 digits (ex: 0x002C0000).

Example:

>**sym2srec**  myApps.elf ../build/myApps.srec 0x002C0000

### Strip

Before running the command, the executable file can be strip with the following 
command line:

>**strip** --discard-all --strip-debug  $(**STRIP_ARTIFACT_NAME**).elf

This command removes all non-global and debug symbols from the artefact.
Strip is a GNU Development Tools, it can be found in the GNU Development 
Toolchain (arm-none-eabi-strip, ...).

### Symbols

The wiki in the repository describes how used the symbol table exported in the 
S-Record file.

### Build

Sym2srec can be built using the [makefile](make/makefile) file on the repository.
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

