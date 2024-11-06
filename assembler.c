#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <ctype.h>


int reserved_keywords_size = 24;
char reserved_keywords[24][4] = {"A", "B", "C", "D", "E", "F", "G", "H", "nop", "hlt", "jz", "jnz", "jmp", "add", "sub", "and", "or", "xor", "not", "sll", "srl", "mov", "lod", "str"};

char instructions[16][4] = {"nop", "mov", "str", "lod", "jmp", "jz", "jnz", "hlt", "add", "sub", "and", "or", "not", "xor", "sll", "srl"};

struct label
{
	char* label_name;
	int code_line;
};

struct instruction_copy
{
	char** array;
	int array_size;
};

void free_memory(struct instruction_copy* instruction_array, int instruction_count, struct label* label_array, int label_len)
{
	for(int i = 0; i < instruction_count; i++)
	{
		for(int j = 0; j < instruction_array[i].array_size; j++)
		{
			free(instruction_array[i].array[j]);
		}

		free(instruction_array[i].array);
	}

	free(instruction_array);

	for (int i = 0; i < label_len; i++) {
        	free(label_array[i].label_name);
    	}
	free(label_array);
}

bool is_register(char* str)
{
	if(strlen(str) != 1)
	{
		return 0;
	}

	if(str[0] >= 'A' && str[0] <= 'H')
	{
		return 1;
	}

	return 0;
}

bool is_hex(char *str)
{
 	if (strlen(str) < 3 || str[0] != '0' || (str[1] != 'x' && str[1] != 'X')) {
        	return 0;  // Return false if the prefix is missing or string is too short
    	}

	return 1;
}

bool is_number(char *str)
{
	bool is_hex_val = is_hex(str);	
	printf("%s %d\n", str, is_hex_val);
	int i = (is_hex_val) ? 2 : 0;

	for(i; i < strlen(str); i++)
	{
		if(!is_hex_val && (str[i] < '0' || str[i] > '9'))
		{
			return 0;
		}

		if(is_hex_val && !isxdigit((unsigned char)str[i])) {
			return 0;
		}
	}

	return 1;
}

bool is_label(struct label* label_array, int label_len, char* str, unsigned int* label_value)
{
	for(int i = 0; i < label_len; i++)
	{
		if(!strcmp(label_array[i].label_name, str))
		{
			*label_value = label_array[i].code_line;
			return 1;
		}
	}

	return 0;
}

int main(int argc, char* argv[])
{
	if(argc == 1)
	{
		printf("Error: Not enough arguments\n");
		return 1;
	}
	
	if(argc > 3)
	{
		printf("Error: To much arguments\n");
		return 1;
	}

	FILE *source_file;
	char* dest_file_name = "output.bin";
	source_file = fopen(argv[1], "r");
	
	if(source_file == NULL)
	{
		perror("Error");
		return 1;
	}
	
	if(argc == 3)
	{
		dest_file_name = argv[2];
	}


	char *line = NULL;
	size_t line_len = 0;
	
	int label_len = 0;
	struct label* label_array = NULL;
	
	int current_instruction_line = 0;
	struct instruction_copy* instruction_array = NULL;

	while (getline(&line, &line_len, source_file) != -1)
	{
		char **array = NULL;
		int array_index = 0;
		char* comments_out = strtok(line, "#");
		char* buf = strtok(comments_out, " \t,\n");

		while(buf != NULL)
		{
			if(strlen(buf) != 0)
			{
				array = realloc(array, (array_index + 1) * sizeof(char*));
				array[array_index] = strdup(buf);
				array_index++;
			}
			
			
			buf = strtok(NULL, " \t,\n");
		}
		
		//check label
		if(array_index == 1 && strlen(array[0]) > 1 && array[0][strlen(array[0]) - 1] == ':')
		{
			char* name = strtok(array[0], ":");

			for(int i = 0; i < reserved_keywords_size; i++)
			{	
				if(!strcmp(reserved_keywords[i], name))
				{
					printf("Error: %s is reserved keyword\n", name);
					return 1;
				}
			}

			if(label_len > 0)
			{
				for(int i = 0; i < label_len; i++)
				{
					if(!strcmp(name, label_array[i].label_name))
					{
						printf("Error: %s is already used\n", name);
						return 1;
					}
				}
			}
			
			label_len++;
			label_array = realloc(label_array, label_len * sizeof(struct label));
			label_array[label_len - 1].label_name = strdup(name);
			label_array[label_len - 1].code_line = current_instruction_line;
		}

		else if(array_index > 0)
		{
			current_instruction_line++;
			instruction_array = realloc(instruction_array, current_instruction_line * sizeof(struct instruction_copy));
			instruction_array[current_instruction_line - 1].array = malloc(array_index * sizeof(char*));

			for(int i = 0; i < array_index; i++)
			{
				instruction_array[current_instruction_line - 1].array[i] = strdup(array[i]);
			}

			instruction_array[current_instruction_line - 1].array_size = array_index;
		}

		for(int i = 0; i < array_index; i++)
		{
			free(array[i]);
		}

		free(array);		
	}
	
	free(line);
	fclose(source_file);
	
	uint16_t hex_instruction_array[current_instruction_line];

	//translate instructions to hexadecimal format
	for(int i = 0; i < current_instruction_line; i++)
	{
		char opcode = -1;
		unsigned short instruction = 0;

		for(int j = 0; j < 16; j++)
		{
			if(!strcmp(instruction_array[i].array[0], instructions[j]))
			{
				opcode = j;
			}
		}

		if(opcode == -1)
		{
			printf("Error: invalid instruction %s\n", instruction_array[i].array[0]);
			free_memory(instruction_array, current_instruction_line, label_array, label_len);
		        return 1;
		}
		
		//printf("opcode: %d\n", opcode);

		//check for zero operand instructions: nop and hlt
		if(opcode == 0 || opcode == 7)
		{
			if(instruction_array[i].array_size != 1)
			{
				printf("Error: %s instruction requires 0 arguments\n", instruction_array[i].array[0]);
				free_memory(instruction_array, current_instruction_line, label_array, label_len);
				return 1;
			}

			instruction = opcode;
			instruction <<= 12;
		}
		
		//check for absolute jmp instruction
		else if(opcode == 4)
		{
			if(instruction_array[i].array_size != 2)
			{
				printf("Error: jmp instruction requires 1 arguments\n");
				free_memory(instruction_array, current_instruction_line, label_array, label_len);
				return 1;
			}

			bool immediate = false;
			unsigned char value = 0;
			
			unsigned int label_value = 0;

			if(is_register(instruction_array[i].array[1]))
			{
				immediate = false;
				value = (unsigned char)(instruction_array[i].array[1][0] - 'A');
			}

			else if(is_number(instruction_array[i].array[1]))
			{
				immediate = true;
				value = strtol(instruction_array[i].array[1], NULL, is_hex(instruction_array[i].array[1]) ? 16 : 10);
			}
			
			else if(is_label(label_array, label_len, instruction_array[i].array[1], &label_value))
			{
				immediate = true;
				value = label_value;
			}

			else
			{
				printf("Error: %s argument is invalid\n", instruction_array[i].array[1]);
				free_memory(instruction_array, current_instruction_line, label_array, label_len);
				return 1;
			}

			instruction = opcode;
			instruction <<= 1;
			instruction |= immediate;
			instruction <<= 11;
			instruction |= value;
		}

		else
		{
			if(instruction_array[i].array_size != 3)
			{
				printf("Error: %s instruction requires 2 arguments\n", instruction_array[i].array[0]);
				free_memory(instruction_array, current_instruction_line, label_array, label_len);
				return 1;
			}

			if(!is_register(instruction_array[i].array[1]))
			{
				printf("Error: %s argument isn't register\n", instruction_array[i].array[1]);
				free_memory(instruction_array, current_instruction_line, label_array, label_len);
				return 1;
			}

			unsigned char register_dest = (unsigned char)(instruction_array[i].array[1][0] - 'A');
			
			bool immediate = false;
			unsigned char value= 0;
			unsigned int label_value = 0;

			if(is_register(instruction_array[i].array[2]))
			{
				immediate = false;
				value = (unsigned char)(instruction_array[i].array[2][0] - 'A');
			}

			else if(is_number(instruction_array[i].array[2]))
			{
				immediate = true;
				value = strtol(instruction_array[i].array[2], NULL, is_hex(instruction_array[i].array[2]) ? 16 : 10);
			}
			
			else if(is_label(label_array, label_len, instruction_array[i].array[2], &label_value))
			{
				immediate = true;
				value = label_value;
			}

			else
			{
				printf("Error: %s argument is invalid\n", instruction_array[i].array[2]);
				free_memory(instruction_array, current_instruction_line, label_array, label_len);
				return 1;
			}

			instruction = opcode;
			instruction <<= 1;
			instruction |= immediate;
			instruction <<= 3;
			instruction |= register_dest;
			instruction <<= 8;
			instruction |= value;
		}

		hex_instruction_array[i] = instruction;
	}

	free_memory(instruction_array, current_instruction_line, label_array, label_len);

	FILE *dest_file = fopen(dest_file_name, "wb");

	if(dest_file == NULL)
	{
		perror("Error");
		return 1;
	}

        size_t elements_written = fwrite(hex_instruction_array, sizeof(uint16_t), current_instruction_line, dest_file);

    	if (elements_written != current_instruction_line) 
	{
        	perror("Error");
        	fclose(dest_file);
        	return 1;
	}		
	
	fclose(dest_file);

	return 0;
}
	
