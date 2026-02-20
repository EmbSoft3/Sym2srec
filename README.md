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

A prebuilt Windows executable is available in the Mk repository at
[`Mk/Make/`](https://github.com/EmbSoft3/Mk/tree/main/Mk/Make).

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

---

## Symbol resolution

For a detailed description of the symbol resolution protocol (bloom filter, GNU hash table
lookup, relocation), see the [wiki](https://github.com/EmbSoft3/Sym2srec/wiki).

---

## Build from source

> A prebuilt Windows executable is available at
> [`Mk/Make/`](https://github.com/EmbSoft3/Mk/tree/main/Mk/Make).
> Build from source only if you need to modify the tool.

**Requirements:**
- [MinGW-W64](https://www.mingw-w64.org/) gcc 8.1.0 (x86_64-posix-sjlj)
- GNU Make 4.x
- Windows — the Makefile uses Windows-specific commands
- 32-bit build required (`-m32`)

**Steps:**

1. Set `TOOLCHAIN_PATH` in `sym2srec/make/makefile` to your MinGW-W64 `bin/` directory.
2. Build:

```
make clean
make all
```

---

## License

Copyright © 2024 **Mathieu Renard**. All rights reserved.

This project is licensed under the **BSD 3-Clause License** — see the
[LICENSE.txt](LICENSE.txt) file for details.
