/*
 * compile.h
 *
 *  Created on: 19 Nov 2023
 *      Author: been
 */

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#ifndef COMPILE
#define COMPILE




typedef struct {
    uint8_t  e_ident[16];
    uint16_t e_type;
    uint16_t e_machine;
    uint32_t e_version;
    uint32_t e_entry;
    uint32_t e_phoff;
    uint32_t e_shoff;
    uint32_t e_flags;
    uint16_t e_ehsize;
    uint16_t e_phentsize;
    uint16_t e_phnum;
    uint16_t e_shentsize;
    uint16_t e_shnum;
    uint16_t e_shstrndx;
} ELF_Header;


typedef struct {
    uint32_t p_type;
    uint32_t p_offset;
    uint32_t p_vaddr;
    uint32_t p_paddr;
    uint32_t p_filesz;
    uint32_t p_memsz;
    uint32_t p_flags;
    uint32_t p_align;
} Program_Header;

int machine_code_length(uint16_t codeLength, char* code);

int parse(char* code, uint16_t** rp_startBrackets);

unsigned char* compile_ELF(char* code, short int length, int* rp_endlength);

int ELF_Write(unsigned char* machineCode, char* name, int machineCodeLength);



#endif
