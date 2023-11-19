/*
 * main.c
 *
 *  Created on: 18 Nov 2023
 *      Author: been
 */

#define extension 		".bf"

#include "compile.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


int main(int argc,char **argv)
{
	if (argc < 2)
	{
		printf("Syntax: %s <filename>\n", argv[0]);
		return -1;
	}
	char* name = (char*)malloc(strlen(argv[1]) + strlen(extension) + 1);
	*name = 0;
	strcat(name, argv[1]);
	strcat(name + strlen(argv[1]), extension);
	FILE* fp;
	fp = fopen(name, "r");
	if (!fp)
	{
		perror("Error opening file ");
		return -1;
	}
	fseek(fp, 0, SEEK_END);
    int codeLength = ftell(fp);
    rewind(fp);

	char* file = (char*)malloc(codeLength+1);
	*file = 0;
	fread(file, sizeof(char), codeLength, fp);
	fclose(fp);

	unsigned char* machineCode = compile(file, machine_code_length(codeLength, file));
	write(machineCode, "output", machine_code_length(codeLength, file));


	return 0;
}
