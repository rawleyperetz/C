// a C program to monitor the filesystem

#include <stdio.h>
#include <stdlib.h>
#include <sys/inotify.h>
#include <errno.h>
#include <dirent.h>
#include <sys/stat.h>
#include <limits.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include "uthash.h"


#define ARGCOUNT 2

struct wd_entry{
	int wd; // this is the key
	char path[PATH_MAX];
	UT_hash_handle hh;
};

struct wd_entry *table = NULL;


void walk_dir(const char *dir_path, int inotify_fd){
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
			perror("full_path");
			continue;
		}

		struct wd_entry *e = malloc(sizeof(*e));
		if(!e){ perror("malloc failed"); exit(EXIT_FAILURE);}

		if(S_ISREG(st.st_mode) || S_ISDIR(st.st_mode)){
			// create file descriptor for files at each level and add to inotify
			int temp_fd = inotify_add_watch(inotify_fd, full_path,  IN_DELETE | IN_CREATE | IN_MODIFY | IN_DELETE_SELF);
			if(temp_fd == -1){
				free(e);
				if(errno == EACCES){
					perror("permission denied");
					continue;
				}
				perror("watch file descriptor");
				continue;
			}
			e->wd = temp_fd;
			strncpy(e->path, full_path, PATH_MAX);
			HASH_ADD_INT(table, wd, e);
			
			if(S_ISDIR(st.st_mode)){
				walk_dir(full_path,inotify_fd);				
			}
			
		}else{
			free(e);
		} 						

	}

	closedir(dir);
	
}



int main(int argc, char **argv){

	if(argc != ARGCOUNT){
		fprintf(stderr, "Usage: %s <dir_name>\n", argv[0]);
		return EXIT_FAILURE;
	}

	char *path_name = argv[1];


	int inotify_fd = inotify_init();
	if(inotify_fd == -1){
		perror("inotify initialization");
		return EXIT_FAILURE;
	}

	int watch_dir_fd = inotify_add_watch(inotify_fd, path_name, IN_DELETE | IN_CREATE | IN_DELETE_SELF | IN_MODIFY);
	if(watch_dir_fd == -1){
		if(errno == EACCES){
			perror("permission denied");
		}
		perror("watch file descriptor");
		return EXIT_FAILURE;
	}	

	
	walk_dir(path_name, inotify_fd);

	char buf[4096] __attribute__((aligned(__alignof__(struct inotify_event))));

	struct wd_entry *result;
	while(1){
		ssize_t len = read(inotify_fd, buf, sizeof(buf));
		if(len == -1){ perror("read"); break; }

		char *ptr = buf;
		while(ptr < buf + len){
			struct inotify_event *event = (struct inotify_event *) ptr;
			// inspect event -> mask, event->name, event->wd
			HASH_FIND_INT(table, &event->wd, result);
			if(result) printf("Path: %s\n", result->path);

			if(event->mask & IN_CREATE){
				if(event->len > 0)
					printf("Created: %s\n", event->name);
				else
					printf("Created: (no name)\n");
			}
			if(event->mask & IN_DELETE){
				if(event->len > 0)
					printf("Deleted: %s\n", event->name);
				else
					printf("Deleted: (no name)\n");
			}
			if(event->mask & IN_MODIFY){
				if(event->len > 0)
					printf("Modified: %s\n", event->name);
				else
					printf("Modified: (no name)\n");
			}
			// if(event->mask & IN_ACCESS){
			// 	if(event->len > 0)
			// 		printf("Accessed: %s\n", event->name);
			// 	else
			// 		printf("Accessed: (no name)\n");
			// }

			ptr += sizeof(struct inotify_event) + event->len;
		}
	}

	return EXIT_SUCCESS;
}
