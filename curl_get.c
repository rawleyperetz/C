// get_example.c
#include <stdio.h>
#include <stdlib.h>
#include <curl/curl.h>

size_t write_callback(void *ptr, size_t size, size_t nmemb, void *userdata) {
    return fwrite(ptr, size, nmemb, stdout);  // print to terminal
}

int main(void) {
    CURL *curl = curl_easy_init();
    if (curl) {
        curl_easy_setopt(curl, CURLOPT_URL, "https://cheat.sh/ls");
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
        curl_easy_perform(curl); // perform the GET
        curl_easy_cleanup(curl);
    }
    return 0;
}
