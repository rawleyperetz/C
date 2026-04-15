// a code to read a linux elf binary and print out its header info
// the stuff readelf -h does

#include <stdio.h>
#include <stdlib.h>
#include <elf.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>

#define ARGCOUNT 2

int main(int argc, char **argv){

	// if(argc != ARGCOUNT){
	// 	fprintf(stderr, "Usage: %s <elf file>\n",argv[0]);
	// 	exit(EXIT_FAILURE);
	// }


	int opt;
	int flag_h = 0;
	int flag_S = 0;
	//int flag_s = 0;

	while ((opt = getopt(argc, argv, "hS:")) != -1) {
	    switch (opt) {
	        case 'h':
	            flag_h = 1;
	            break;
	        case 'S':
	            flag_S = 1;
	            break;
	        //case 's':
	        	//flag_s = 1;
	            //break;
	        default:
	            fprintf(stderr, "Usage: %s [-h | -S | -s] <file>\n", argv[0]);
	            return 1;
	    }
	}
	

	// optind is the index of first non-flag argument
	if(optind >= argc){
		fprintf(stderr,"Expected filename\n");
		return EXIT_FAILURE;
	}


	//char *elf_file = argv[1];
	FILE *elf_file = fopen(argv[optind], "rb");
	if(!elf_file){
		perror("Error opening the file");
		exit(EXIT_FAILURE);
	}


	Elf64_Ehdr *elfstruct=malloc(sizeof(Elf64_Ehdr));
	if(!elfstruct){
		perror("malloc failed");
		exit(EXIT_FAILURE);
	}

	size_t read_elf = fread(elfstruct, sizeof(Elf64_Ehdr), 1, elf_file);

	if(read_elf == 0){
		fprintf(stderr, "Nothing to see here\n");
		exit(EXIT_FAILURE);
	}

	if(flag_h){


	//char magic_num = elfstruct->e_ident[EI_NIDENT];
	char buffer[4] = {ELFMAG0,ELFMAG1,ELFMAG2,ELFMAG3};
	int i = 0;
	while (i < 4){
		if(elfstruct->e_ident[i] !=  buffer[i]){
			fprintf(stderr, "File is not an elf file\n");
			exit(EXIT_FAILURE);
		}
		i++;
	}

	printf("Magic:    %#x %#x %#x %#x\n",ELFMAG0,ELFMAG1,ELFMAG2,ELFMAG3 );//elfstruct->e_entry);
	
	if(elfstruct->e_type == ET_EXEC){
		printf("Type: EXEC (Executable)\n");
	}else if(elfstruct->e_type == ET_CORE){
		printf("Type: A core file\n");
	}else if(elfstruct->e_type == ET_DYN){
		printf("Type: A shared object\n");
	}else{
		printf("Type: Unknown Type\n");
	}

	printf("Entry:    %#lx\n", elfstruct->e_entry);
	
	printf("Sections: %d\n", elfstruct->e_shnum);
	

	
	printf("PH count: %d\n", elfstruct->e_phnum);
	

	free(elfstruct);

	}else if(flag_S){
		printf("There are %d section headers, starting at offset %#lx\n",elfstruct->e_shnum, elfstruct->e_shoff);
		
		int fseek_val = fseek(elf_file, elfstruct->e_shoff, SEEK_SET);
		if(fseek_val != 0){
			perror("Error fseeking");
			exit(EXIT_FAILURE);
		}

	
		Elf64_Shdr *elfsection_struct=malloc(sizeof(Elf64_Shdr)* elfstruct->e_shnum);
		if(!elfsection_struct){
			perror("malloc failed");
			exit(EXIT_FAILURE);
		}
			
			
		size_t read_elf = fread(elfsection_struct, sizeof(Elf64_Shdr), elfstruct->e_shnum, elf_file);
		
		if(read_elf == 0){
			fprintf(stderr, "No section to see here\n");
			exit(EXIT_FAILURE);
		}

		uint16_t i = 0;
		while(i < elfstruct->e_shnum){
			switch(elfsection_struct[i].sh_type){
				case SHT_NULL:
					printf("%d: NULL %zu\n", i, elfsection_struct[i].sh_size);
					break;
				case SHT_PROGBITS:
					printf("%d: PROGBITS %zu\n", i, elfsection_struct[i].sh_size);
					break;
				case SHT_SYMTAB:
					printf("%d: SYMTAB %zu\n", i, elfsection_struct[i].sh_size);
					break;
				case SHT_STRTAB:
					printf("%d: STRTAB %zu\n", i, elfsection_struct[i].sh_size);
					break;
				case SHT_RELA:
					printf("%d: RELA %zu\n", i, elfsection_struct[i].sh_size);
					break;
				case SHT_HASH:
					printf("%d: HASH %zu\n", i, elfsection_struct[i].sh_size);
					break;
				case SHT_DYNAMIC:
					printf("%d: DYNAMIC %zu\n", i, elfsection_struct[i].sh_size);
					break;
				case SHT_NOTE:
					printf("%d: NOTE %zu\n", i, elfsection_struct[i].sh_size);
					break;
				case SHT_NOBITS:
					printf("%d: NOTBITS %zu\n", i, elfsection_struct[i].sh_size);
					break;
				case SHT_REL:
					printf("%d: REL %zu\n", i, elfsection_struct[i].sh_size);
					break;
				case SHT_SHLIB:
					printf("%d: SHLIB %zu\n", i, elfsection_struct[i].sh_size);
					break;
				case SHT_DYNSYM:
					printf("%d: DYNSYM %zu\n", i, elfsection_struct[i].sh_size);
					break;
				case SHT_LOUSER:
					printf("%d: LOUSER %zu\n", i, elfsection_struct[i].sh_size);
					break;
				case SHT_HIUSER:
					printf("%d: HIUSER %zu\n", i, elfsection_struct[i].sh_size);
					break;
				default:
					printf("%d: UNKNOWN %zu\n", i, elfsection_struct[i].sh_size);
					break;
			}
			
			i++;
		}	

		free(elfsection_struct);
		free(elfstruct);
		
	}
	//else if(flag_s){
		//printf("do something here\n");
	//}	

	//free(elfstruct);

	fclose(elf_file);
	
	


	return 0;
}
