// integrity checker using 


#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <dirent.h>
#include <getopt.h>
#include <string.h>
#include <limits.h>
#include <sys/stat.h>
#include <openssl/sha.h>
// #include <unistd.h>
// #include <fcntl.h>


void sha256_file(const char *path, unsigned char output[SHA256_DIGEST_LENGTH]){
	FILE *fp = fopen(path, "rb");
	if(!fp){ perror(path); return;}
	SHA256_CTX ctx;
	SHA256_Init(&ctx);
	unsigned char buf[8192];
	size_t n;
	while((n = fread(buf, 1, sizeof(buf), fp)) > 0)
		SHA256_Update(&ctx, buf, n);
	SHA256_Final(output, ctx);
	fclose(fp);
}

void hex_digest(const unsigned char *digest, size_t len, char *out){
	for(size_t i = 0; i < len; i++){
		sprintf(out + i * 2, "%02x", digest[i]);
	}
	out[len * 2] = '\0';
}

bool file_exists(const char *filename)
{
    FILE *fp = fopen(filename, "r");
    bool is_exist = false;
    if (fp != NULL)
    {
        is_exist = true;
        fclose(fp); // close the file
    }
    return is_exist;
}

int isDirectoryExists(const char *path)
{
    struct stat stats;

    if(stat(path, &stats) != 0){
    	return 0;
    }

    return S_ISDIR(stats.st_mode);
}


void walk_dir(const char *dir_path, FILE *fp_out){
	DIR *dir = opendir(dir_path);
	if(!dir){ perror(dir_path); return; }

	struct dirent *entry;
	while((entry = readdir(dir)) != NULL){
		if(strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0){
			continue;
		}

		char full_path[PATH_MAX];
		snprintf(full_path, sizeof(full_path), "%s/%s", dir_path, entry->d_name);

		struct stat st;
		if(stat(full_path, &st) != 0){
			perror(full_path);
			continue;
		}

		if(S_ISREG(st.st_mode)){
			unsigned char digest[SHA256_DIGEST_LENGTH];
			sha256_file(full_path, digest);
			char hex[SHA256_DIGEST_LENGTH * 2 + 1];
			hex_digest(digest, SHA256_DIGEST_LENGTH, hex);
			fprintf(fp_out, "%s %s\n", hex, full_path);
			printf("[+] %s %s\n", hex, full_path);
		} else if(S_ISDIR(st.st_mode)){
			walk_dir(full_path,fp_out);
		}
	}

	closedir(dir);
	
}

int main(int argc, char *argv[]){

	char *in_dir = NULL;
	char *out_file = NULL;
	//char *hash_type = NULL;
	int option;

	
	struct option long_options[] = {
		//{"hash-func", required_argument, 0, 'h'},
		{"dir", required_argument, 0, 'd'},
		{"out", required_argument, 0, 'o'},
		{0,0,0,0}
	};


	while ((option = getopt_long(argc, argv, "d:o:", long_options, NULL)) != -1) {
	        switch (option) {
	            // case 'h':
	            //     hash_type = optarg;
	            //     break;
	
	            case 'o':
	            	out_file = optarg;
	                break;
	
	            case 'd':
	                in_dir = optarg;
	                if(!isDirectoryExists(in_dir)){
	                	fprintf(stderr, "Exiting program...\n");
	                	exit(EXIT_FAILURE);
	                }
	                break;
	
	            default:
	                fprintf(stderr, "Usage: %s -h <hash-func> -d <input directory> -o <output file>)\n", argv[0]);
	                exit(EXIT_FAILURE);
	        }
	    }

	if(!in_dir || !out_file){
		fprintf(stderr, "Both --dir and --out are required.\n");
		exit(EXIT_FAILURE);
	}


	// out_file = fopen(optarg,"w");
	// 	                if (!ofile) {
	// 	                    perror("Error opening output file"); 
	// 	                    exit(EXIT_FAILURE);
	// 	                } 
	
		//char c;
		if(file_exists(out_file)){
			char c;
		    printf("Do you wish to overwrite the file, (y)es or (n)o: ");
		    scanf(" %c", &c);
		    if(c != 'y' && c != 'Y'){
		        fprintf(stderr, "Exiting program...\n");
		     	exit(EXIT_FAILURE);
		    }			
		} else{
			printf("'%s' not found, creating it.\n", out_file);	
		}

    FILE *fp_out = fopen(out_file, "w");
	if(!fp_out){ perror("fopen"); exit(EXIT_FAILURE);}

	walk_dir(in_dir, fp_out);

	fclose(fp_out);
	printf("\n[*] Manifest written to %s\n", out_file);
		
	return 0;
}
