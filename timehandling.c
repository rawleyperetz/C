// time handling

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main(){
    time_t now = time(NULL);

    printf("%d\n", now);

    char *string_now = ctime(&now);

    printf("%s\n", string_now);

    return 0;

}
