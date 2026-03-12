# Sym2srec

**Sym2srec** is a command-line tool that extracts the `.symtab` and `.strtab` sections
from an ELF32 executable and embeds them as loadable segments into a new S-Record file.

It is used as part of the [Mk](https://github.com/EmbSoft3/Mk) build pipeline to enable
dynamic loading: external applications can reference Mk kernel symbols at runtime without
statically linking against the kernel. A GNU hash table is also generated alongside the
symbol tables to accelerate symbol resolution.

---

## How it works

```
make all (Mk)
  ├── Link  → Mk.elf          (firmware + full debug symbol table)
  ├── Strip → Mk-Strip.elf    (debug symbols removed, only exported API symbols kept)
  └── sym2srec Mk-Strip.elf Mk.srec 0x002C0000
        └── Mk.srec           (firmware + Mk API symbol table → only file to flash)
```

`Mk.srec` is the only file written to the target. It encodes both the firmware binary and
the kernel symbol table with their target addresses. `Mk.elf` is preserved intact and
loaded in Eclipse/GDB as the debug symbol file.

---

## Usage

```
sym2srec <input.elf> <output.srec> <base_address>
```

| Argument | Description |
|----------|-------------|
| `input.elf` | ELF32 executable containing the symbols to export. |
| `output.srec` | Path of the S-Record file to generate. |
| `base_address` | Address where the symbol table will be loaded in memory. Must be 4-byte aligned, 8 hex digits (e.g. `0x002C0000`). |

**Example:**

```
sym2srec Mk-Strip.elf Mk.srec 0x002C0000
```

### Strip the input file first

Before running sym2srec, strip local and debug symbols from the input ELF to keep only
the exported API symbols in the generated symbol table:

```
arm-none-eabi-strip --discard-all --strip-debug Mk.elf -o Mk-Strip.elf
```

### Linux — set executable permission

After cloning the repository, mark the prebuilt binary as executable before use:

```bash
chmod +x Mk/Make/sym2srec
```

> Git does not always preserve executable permissions after a clone or checkout.
> Run this command once after cloning the repository.

---

## Symbol resolution

For a detailed description of the symbol resolution protocol (bloom filter, GNU hash table
lookup, relocation), see the [wiki](https://github.com/EmbSoft3/Sym2srec/wiki).

---

## Build from source

> Prebuilt executables for Windows and Linux are available in the
> [Releases](../../releases) section of this repository.
> Build from source only if you need to modify the tool.

### Why 32-bit?

sym2srec must be compiled as a 32-bit binary (`-m32`) because it directly maps ELF32
structures to C structs. Compiling as 64-bit would produce incorrect pointer sizes and
struct layouts, causing silent data corruption at runtime.

### Requirements — Windows

- [GCC 15.2.0 (POSIX threads) + MinGW-w64 13.0.0 UCRT (release 6)](https://github.com/brechtsanders/winlibs_mingw/releases/tag/15.2.0posix-13.0.0-ucrt-r6)
- GNU Make 4.3
- One of the following Unix shell environments (provides `sh`, `find`, `rm`):
  - [MSYS2](https://www.msys2.org/) *(recommended)*
  - [Git for Windows](https://git-scm.com/)

> The Makefile automatically detects MSYS2 or Git Bash at their default installation
> paths (`C:/msys64` and `C:/Program Files/Git`). If your installation is elsewhere,
> update `MSYS2_BIN` or `GITBASH_BIN` at the top of `sym2srec/make/makefile`.

**Steps:**

1. Set `MINGW_PATH` in `sym2srec/make/makefile` to your MinGW-w64 `bin/` directory.
2. Build:

```bash
make clean
make all
```

### Requirements — Linux

- GCC (system) with 32-bit multilib support
- GNU Make 4.3

Install the 32-bit build dependencies before building:

```bash
sudo apt-get install gcc-multilib g++-multilib
```

**Steps:**

```bash
make clean
make all
```

### Compiler versions

| Platform | Tool | Version |
|----------|------|---------|
| Windows | `gcc` (MinGW) | 15.2.0 POSIX ([WinLibs](https://github.com/brechtsanders/winlibs_mingw/releases/tag/15.2.0posix-13.0.0-ucrt-r6)) |
| Windows | `make` | GNU Make 4.3 |
| Linux | `gcc` | system GCC with `gcc-multilib` |
| Linux | `make` | GNU Make 4.3 |

---

## Continuous Integration

Every push and pull request is automatically built by **GitHub Actions**.
The workflow installs the required dependencies, runs `make all`,
and uploads `sym2srec` as a downloadable build artifact.

The latest successful build artifact is available on the
[Actions](../../actions) tab of this repository.

---

## License

Copyright © 2024 **Mathieu Renard**. All rights reserved.

This project is licensed under the **BSD 3-Clause License** — see the
[LICENSE.txt](LICENSE.txt) file for details.
