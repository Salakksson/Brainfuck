/*
 * compile.c
 *
 *  Created on: 19 Nov 2023
 *      Author: been
 */

#include "compile.h"


int machine_code_length(uint16_t codeLength, char* code)
{
	int length = 11;
	for (int i = 0; i < codeLength; i++)
	{
		switch (code[i])
		{
		case '>':
			length += 2;
			break;
		case '<':
			length += 2;
			break;
		case '+':
			length += 12;
			break;
		case '-':
			length += 12;
			break;
		case '.':
			length += 11;
			break;
		case ',':
			length += 13;
			break;
		case ']':
			length += 8;
			break;
		default:
			break;
		}
	}
	return length;
}

int parse(char* code, uint16_t** rp_startBrackets)
{
	char* ccode = code;
	int starts = 0;
	int ends = 0;
	int chars = 1;
	int lines = 1;
	while (*ccode)
	{
		switch (*ccode)
		{
		case '[':
			starts++;
			chars++;
			break;

		case ']':
			if(!starts)
			{
				printf("ERROR: No corresponding '[' to ']' at %d:%d\n", lines, chars);
			}
			ends++;
			chars++;
			break;

		case '\n':
			lines++;
			chars = 1;
			break;

		default:
			break;
		}

		ccode++;
	}

	if (starts != ends)
	{
		(starts > ends)
		?	printf("ERROR: more '[' than ']' found in code\n")
		: 	printf("ERROR: more ']' than '[' found in code\n");
		return -1;
	}
	*rp_startBrackets = malloc(sizeof(uint16_t) * starts);
	uint16_t* startBrackets = *rp_startBrackets;

	int arrptr = 0;
	ccode = code;
	while (*ccode)
	{
		if (*ccode == '[')
		{
			startBrackets[arrptr] = ccode - code;
			arrptr++;
		}
		ccode++;
	}


	return 0;
}

unsigned char* compile_ELF(char* code, short int length, int* rp_endlength)
{

	unsigned char beginning[] =
	{
		0x66, 0x31, 0xf6,
		0x31, 0xc0,
		0x31, 0xdb,
		0x31, 0xc9,
		0x31, 0xd2
	};
	printf("0x%x, %d\n", length, length);
	printf("0x%lx, %ld\n", sizeof(beginning), sizeof(beginning));

	unsigned char arr_low 		= (unsigned char)((length+9+sizeof(ELF_Header)+2*sizeof(Program_Header)) & 0xFF);
	unsigned char arr_high		= (unsigned char)(((length+9+sizeof(ELF_Header)+2*sizeof(Program_Header)) >> 8) & 0xFF);

	unsigned char exit[] =
	{
		0xb0, 0x01,
		0x67, 0x8a, 0x9c, arr_low, arr_high,
		0xcd, 0x80
	};

	*rp_endlength = sizeof(exit);
	unsigned char* machineCode  = (unsigned char*)malloc(length + *rp_endlength);
	unsigned char* currentPos = machineCode;

	unsigned char incptr[] =
	{
		0x66, 0x46
	};

	unsigned char decptr[] =
	{
		0x66, 0x4e
	};

	unsigned char plus[] =
	{
		0x67, 0x8a, 0x84, arr_low, arr_high,
		0xfe, 0xc0,
		0x67, 0x88, 0x84, arr_low, arr_high
	};

	unsigned char minus[] =
	{
		0x67, 0x8a, 0x84, arr_low, arr_high,
		0xfe, 0xc8,
		0x67, 0x88, 0x84, arr_low, arr_high
	};

	unsigned char out[] =
	{
		0xb0, 0x04,
		0xb3, 0x01,
		0x67, 0x8a, 0x8c, arr_low, arr_high,
		0xcd, 0x80
	};

	unsigned char in[] =
	{
		0xb0, 0x03,
		0xb3, 0x00,
		0x67, 0x8a, 0x8c, arr_low, arr_high,
		0xb2, 0x01,
		0xcd, 0x80
	};

	unsigned char brack_end[] =
	{
		0x66, 0x3b, 0x06, 0x00, 0x00,
		0x74 // 2 more bytes with location to start bracket
	};

	uint16_t* startBrackets;
	if (parse(code, &startBrackets))
	{
		return NULL;
	}
	memcpy(machineCode, beginning, sizeof(beginning));
	currentPos += sizeof(beginning);
	int arrptr = 0;
	char* ccode = code;
	while (*ccode)
	{
		switch(*ccode)
		{
		case '>':
			memcpy(currentPos, incptr, sizeof(incptr));
			currentPos += sizeof(incptr);
			break;
		case '<':
			memcpy(currentPos, decptr, sizeof(decptr));
			currentPos += sizeof(decptr);
			break;
		case '+':
			memcpy(currentPos, plus, sizeof(plus));
			currentPos += sizeof(plus);
			break;
		case '-':
			memcpy(currentPos, minus, sizeof(minus));
			currentPos += sizeof(minus);
			break;
		case '.':
			memcpy(currentPos, out, sizeof(out));
			currentPos += sizeof(out);
			break;
		case ',':
			memcpy(currentPos, in, sizeof(in));
			currentPos += sizeof(in);
			break;
		case '[':
			arrptr++;
			break;
		case ']':
			memcpy(currentPos, brack_end, sizeof(brack_end));
			currentPos += sizeof(brack_end);
			uint16_t lastbracketaddr = machine_code_length(startBrackets[arrptr], code);
			memcpy(currentPos, &lastbracketaddr, sizeof(uint16_t));
			currentPos += sizeof(uint16_t);
			arrptr++;
			break;
		}


		ccode++;
	}
	memcpy(currentPos, exit, *rp_endlength);
	return machineCode;
}

int ELF_Write(unsigned char* machineCode, char* name, int machineCodeLength, int endlength)
{
	ELF_Header elfHeader =
	{
        {0x7F, 'E', 'L', 'F', 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0}, // e_ident
        2,              									// e_type (executable)
        3,              									// e_machine (x86) 
        1,              									// e_version
        0x08048054,      									// e_entry (entry point address)
        sizeof(ELF_Header),  								// e_phoff (program header)
        0,     											    // e_shoff (section header)
        0,              									// e_flags (flags)
        sizeof(ELF_Header),  								// e_ehsize	(size of header)
        sizeof(Program_Header),								// e_phentsize (size of one program header)
        1,              									// e_phnum (number of program headers)
        28,				 		            				// e_shentsize (size of one section header)
        0,													// e_shnum	(number of section headers)
        0               									// e_shstrndx
	};

	Program_Header programHeader =
	{
        1,              									// p_type (loadable segment)
        sizeof(ELF_Header) + sizeof(Program_Header),  		// p_offset
        0x08048054,       									// p_vaddr (virtual address)
        0,       											// p_paddr (unused)
        machineCodeLength + endlength,						// p_filesz (size in file)
        machineCodeLength + endlength,						// p_memsz (size in memory)
        5,              									// p_flags (1=X 2=W 4=R)
        0          											// p_align
    };

	Program_Header dataHeader =
	{
        1,              									// p_type (loadable segment)
        sizeof(ELF_Header) + sizeof(Program_Header)			// p_offset
		+ machineCodeLength + endlength,  					// 
        0x08048054,       									// p_vaddr (virtual address)
        0,       											// p_paddr (unused)
        0,													// p_filesz (size in file)
        0x10000,											// p_memsz (size in memory)
        6,              									// p_flags (1=X 2=W 4=R)
        0		          									// p_align						// I have tried 0x1000 but i think it was causing problems so i aint know
    };

	FILE* file = fopen(name, "wb");
    if (!file)
    {
        perror("Error opening file ");
        return 1;
    }

    fwrite(&elfHeader, sizeof(ELF_Header), 1, file);
    fwrite(&programHeader, sizeof(Program_Header), 1, file);
	fwrite(&dataHeader, sizeof(Program_Header), 1, file);
	
    fwrite(machineCode, sizeof(unsigned char), machineCodeLength + endlength, file);

    fclose(file);



	return 0;
}









