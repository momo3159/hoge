#pragma once
#include <stdint.h>

#define EI_NIDENT 16

#define PT_NULL    0
#define PT_LOAD    1
#define PT_DYNAMIC 2
#define PT_INTERP  3
#define PT_NOTE    4
#define PT_SHLIB   5
#define PT_PHDR    6
#define PT_TLS     7



typedef uintptr_t Elf64_Addr;
typedef uint64_t Elf64_Off;
typedef uint64_t Elf64_Xword;
typedef uint32_t Elf64_Word;
typedef uint16_t Elf64_Half;

// ファイルヘッダ
typedef struct {
  unsigned char e_ident[EI_NIDENT];
  Elf64_Half      e_type;
  Elf64_Half      e_machine;
  Elf64_Word      e_version;
  Elf64_Addr      e_entry;
  Elf64_Off       e_phoff; // プログラムヘッダのファイルオフセット（バイト）
  Elf64_Off       e_shoff; // セクションヘッダのファイルオフセット（バイト）
  Elf64_Word      e_flags;
  Elf64_Half      e_ehsize;
  Elf64_Half      e_phentsize; // プログラムヘッダのエントリのサイズ（バイト）
  Elf64_Half      e_phnum; // プログラムヘッダのエントリ数
  Elf64_Half      e_shentsize;
  Elf64_Half      e_shnum;
  Elf64_Half      e_shstrndx;
} Elf64_Ehdr;

typedef struct {
  Elf64_Word      p_type; // セグメントの種別
  Elf64_Word      p_flags;
  Elf64_Off       p_offset; // ファイルの先頭からのオフセット
  Elf64_Addr      p_vaddr; // 仮想アドレス
  Elf64_Addr      p_paddr; // 物理アドレス
  Elf64_Xword     p_filesz; // ファイルでのバイト数
  Elf64_Xword     p_memsz; // メモリでのバイト数
  Elf64_Xword     p_align;
} Elf64_Phdr;

typedef struct {
  Elf64_Word      sh_name;
  Elf64_Word      sh_type;
  Elf64_Xword     sh_flags;
  Elf64_Addr      sh_addr;
  Elf64_Off       sh_offset;
  Elf64_Xword     sh_size;
  Elf64_Word      sh_link;
  Elf64_Word      sh_info;
  Elf64_Xword     sh_addralign;
  Elf64_Xword     sh_entsize;
} Elf64_Shdr;