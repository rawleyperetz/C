// my mini scheduler

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <stdbool.h>

#define MAX_JOBS 128
#define MAX_LINE 256
#define MAX_PATH 128

typedef struct {
    int hour;
    int minute;
    char path[MAX_PATH];

} parseJob;

void createDefaultConfig() {
    FILE *f = fopen("scheduler.conf", "w");
    if (!f) {
        fprintf(stderr, "Failed to create config file\n");
        exit(EXIT_FAILURE);
    }

    fprintf(f, "# Rawley’s scheduling program\n");
    fprintf(f, "# Syntax: hour minute /absolute/path/to/script.sh\n");
    fclose(f);
    printf("Created default config: scheduler.conf\n");
}

int isFileExists(const char *path)
{
    // Try to open file
    FILE *fptr = fopen(path, "r");

    // If file does not exists
    if (fptr == NULL)
        return 0;

    // File exists hence close file and return true.
    fclose(fptr);

    return 1;
}


int main(){
    FILE *file = fopen("scheduler.conf", "r");
    if (file == NULL) {
        //fprintf(stderr, "Error opening file!\n");
        char choice;
        printf("scheduler.conf not found. Create a new one? [y/n]: ");
        scanf(" %c", &choice); // NOTE: leading space skips whitespace

        if (choice == 'y' || choice == 'Y') {
            createDefaultConfig();
            printf("Please edit 'scheduler.conf' and restart the program.\n");
            return 0;
        } else {
            printf("Exiting...\n");
            return 0;
        }

    }

    parseJob jobs[MAX_JOBS];
    size_t jobCount = 0;

    char line[MAX_LINE];

    while (fgets(line, sizeof(line), file)){

        if (line[0] == '#' || line[0] == '\n')
            continue;

        parseJob j;


        if (sscanf(line, "%d %d %127s", &j.hour, &j.minute, j.path) == 3){


            if (isFileExists(j.path) != 1){
                perror("One or more of the paths do not exists\n");
                return 1;
            }
            if (j.hour > 23 || j.hour < 0 || j.minute > 59 || j.minute < 0){
                fprintf(stderr, "Invalid time in line: %s\n", line);
                continue;
            }

            if (jobCount == MAX_JOBS){
                fprintf(stderr, "Job limit reached (%d); ignoring extra lines.\n", MAX_JOBS);
                break;
            }

            jobs[jobCount++] = j;
        } else {
            fprintf(stderr, "Malformed line (ignored): %s", line);
            return 1;
        }
    }

    fclose(file);



    while(true){
        time_t now = time(NULL);

        struct tm *tm_struct = localtime(&now);

        int hour = tm_struct->tm_hour;
        int minute = tm_struct->tm_min;

        int status;
        pid_t pid;

        FILE *logFile = fopen("scheduler.log", "a");
        if (!logFile) {
            perror("Could not open log file");
            return 1;
        }

        while ((pid = waitpid(-1, &status, WNOHANG)) > 0) {
            fprintf(logFile, "[%02d:%02d] Reaped child %d, status %d\n", hour, minute, pid, WEXITSTATUS(status));
            fflush(logFile);
        }

        for (size_t i = 0; i < jobCount; ++i){
            if(jobs[i].hour == hour && jobs[i].minute == minute){
                int rc = fork();

                if(rc < 0){
                    fprintf(stderr, "fork failed\n");
                    exit(1);
                } else if(rc == 0){
                    char *argv[] = {"sh", jobs[i].path, NULL };
                    execvp(argv[0], argv);  // Replace with the job command
                    // If exec fails:
                    perror("execvp failed");
                    exit(EXIT_FAILURE);
                }else{
                    fprintf(logFile, "[%02d:%02d] Started job: %s (pid: %d)\n", hour, minute, jobs[i].path, rc);
                    fflush(logFile);
                }

            }
        }

        time_t next = now - (now % 60) + 60;
        int sleepTime = (int)(next - time(NULL));
        if (sleepTime > 0)
            sleep(sleepTime);

    }

    return 0;
}
