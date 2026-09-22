#include <arpa/inet.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#define TCP_MONITOR_PATH "/proc/net/tcp"
#define BUFFER_SIZE 64
#define IP_OCTETS 4
// #define LINE_SIZE 64

struct connect_entry {
  int number;
  const char *name;
};

struct connect_entry connect_table[] = {
    {1, "ESTABLISHED"}, {2, "SYN_SENT"},  {3, "SYN_RECV"}, {4, "FIN_WAIT1"},
    {5, "FIN_WAIT2"},   {6, "TIME_WAIT"}, {7, "CLOSE"},    {8, "CLOSE_WAIT"},
    {9, "LAST_ACK"},    {10, "LISTEN"},   {11, "CLOSING"},
};

const char *get_connect_name(int connect_number) {
  int connect_len = sizeof(connect_table) / sizeof(connect_table[0]);
  for (int i = 0; i < connect_len; i++) {
    if (connect_table[i].number == connect_number) {
      return connect_table[i].name;
    }
  }
  return "unknown";
}

void reverse_array(uint8_t *arr) {
  uint8_t start = 0;
  uint8_t end = IP_OCTETS - 1;

  while (start < end) {
    uint8_t temp = arr[start];
    arr[start] = arr[end];
    arr[end] = temp;
    start++;
    end--;
  }
}

int get_address(char *local_ip, char **p) {
  uint8_t toBytes[IP_OCTETS];
  int index = 0;
  char OneByte[3] = {'\0'};
  while (index < IP_OCTETS) {
    memcpy(OneByte, *p, sizeof(OneByte));
    if (sscanf(OneByte, "%2hhx", &toBytes[index]) < 0) {
      return 1;
    }
    index++;
    *p += 2;
  }

  reverse_array(toBytes);
  if (inet_ntop(AF_INET, toBytes, local_ip, INET_ADDRSTRLEN) == NULL)
    return 1;
  return 0;
}

int get_port(uint16_t *port, char **p) {
  unsigned int temp_port = 0;
  int bytes_read = 0;
  if (sscanf(*p, "%4x%n", &temp_port, &bytes_read) != 1) {
    return 1;
  }
  *p += bytes_read;
  *port = (uint16_t)temp_port;
  return 0;
}

int get_connection_status(uint8_t *status, char **p) {
  unsigned int temp_status = 0;
  if (sscanf(*p, "%2x", &temp_status) != 1) {
    return 1;
  }
  *status = (uint8_t)temp_status;
  return 0;
}

int main() {
  FILE *fptr = fopen(TCP_MONITOR_PATH, "r");
  if (!fptr) {
    perror("fopen");
    return -1;
  }

  char buf[BUFFER_SIZE];
  fgets(buf, sizeof(buf), fptr);

  while (fgets(buf, sizeof(buf), fptr) != NULL) {
    buf[BUFFER_SIZE - 1] = '\0';
    // create and get pointer to the first semi-colon
    char *p = strchr(buf, ':');
    if (p == NULL) {
      // printf("\n");
      continue;
    }
    p += 2;

    char local_ip[INET_ADDRSTRLEN];
    if (get_address(local_ip, &p) == 1)
      continue;

    p += 1;

    uint16_t local_port;
    if (get_port(&local_port, &p) == 1) {
      continue;
    }

    p += 1;
    // printf("%c\n\n", *p);
    char rem_ip[INET_ADDRSTRLEN];
    if (get_address(rem_ip, &p) == 1)
      continue;

    p += 1;

    uint16_t rem_port;
    if (get_port(&rem_port, &p) == 1)
      continue;

    p += 1;

    uint8_t connect_status;
    if (get_connection_status(&connect_status, &p) == 1)
      continue;

    time_t now = time(NULL);
    struct tm *t = localtime(&now);

    printf("[%d:%d:%d] %s:%d -> %s:%d \t [%s]\n", t->tm_hour, t->tm_min,
           t->tm_sec, local_ip, local_port, rem_ip, rem_port,
           get_connect_name(connect_status));
  }

  fclose(fptr);
  return 0;
}
