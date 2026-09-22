#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#define EXPECTED_ARGC 2
#define DNS_PACKET_LEN 512

struct DNSHeader {
  uint16_t id;
  uint16_t flags;
  uint16_t q_count;
  uint16_t ans_count;
  uint16_t auth_count;
  uint16_t add_count;
};

void write_u16(unsigned char *buf, size_t *pos, uint16_t value) {
  value = htons(value);
  memcpy(buf + *pos, &value, sizeof(value));
  *pos += sizeof(value);
}

void change_to_dns_format(char *url, unsigned char *buf, size_t *pos,
                          uint16_t qtype, uint16_t qclass) {
  char *token = strtok(url, ".");
  while (token != NULL) {
    size_t getlen = strlen(token);
    // append length to dns question
    buf[(*pos)++] = getlen;
    // append all char of token to dns question
    char *p = token;
    while (*p) {
      // append char to dns question
      buf[(*pos)++] = (unsigned char)*p++;
    }
    token = strtok(NULL, ".");
  }
  buf[(*pos)++] = '\0';
  write_u16(buf, pos, qtype);
  write_u16(buf, pos, qclass);
}

// void parse_response_ntohs(unsigned char *buf, size_t *pos, uint16_t value){
// 	value = ntohs()
// }

void get_name(unsigned char *buf, size_t *pos) {
  while (buf[*pos] != 0) {
    uint8_t label_len = buf[*pos];

    (*pos)++;

    for (int i = 0; i < label_len; i++) {
      putchar(buf[*pos]);
      (*pos)++;
    }
    putchar('.');
  }
  (*pos)++;
}

int main(int argc, char **argv) {
  if (argc < EXPECTED_ARGC) {
    fprintf(stderr, "Usage: %s <web-site>\n", argv[0]);
    return 1;
  }

  char *url = argv[1];
  // size_t urlen = strlen(url);

  uint16_t qtype = 1;
  uint16_t qclass = 1;

  if (argc == 3) {
    qtype = atoi(argv[2]);
  } else if (argc == 4) {
    qtype = atoi(argv[2]);
    qclass = atoi(argv[3]);
  }

  int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
  if (sockfd < 0) {
    perror("socket");
    return 1;
  }

  struct sockaddr_in sa;
  memset(&sa, 0, sizeof(sa));
  sa.sin_family = AF_INET;
  sa.sin_port = htons(53);

  int result = inet_pton(AF_INET, "8.8.8.8", &(sa.sin_addr));
  if (result <= 0) {
    perror("inet_pton failed");
    close(sockfd);
    return 1;
  }

  unsigned char buf[DNS_PACKET_LEN];
  memset(buf, 0, sizeof(buf));

  // struct Dns_structure *dns = (struct Dns_structure *)&buf[0];
  // dns->header.id = htons(0x1234);    // this is the transaction id
  // dns->header.flags = htons(0x0100); // recursion Desire (RD = 1)
  // dns->header.q_count = htons(1);    // 1 Question only
  size_t pos = 0;
  write_u16(buf, &pos, 0x1234); // transaction id
  write_u16(buf, &pos, 0x0100); // flags: RD = 1
  write_u16(buf, &pos, 1);      // qdcount
  write_u16(buf, &pos, 0);      // ancount
  write_u16(buf, &pos, 0);      // nscount
  write_u16(buf, &pos, 0);      // arcount

  // populating the question name
  change_to_dns_format(url, buf, &pos, qtype, qclass);
  printf("DNS packet length: %zu bytes\n", pos);

  // printingg packet in hexadecimal so we can inspect it
  for (size_t i = 0; i < pos; i++) {
    printf("%02x ", buf[i]);
    if ((i + 1) % 16 == 0) {
      printf("\n");
    }
  }

  printf("\n");

  ssize_t packetSent =
      sendto(sockfd, buf, pos, 0, (struct sockaddr *)&sa, sizeof(sa));
  if (packetSent < 0) {
    perror("sendto");
    close(sockfd);
    return 1;
  }

  // receive DNS response
  ssize_t packetRead = recvfrom(sockfd, buf, sizeof(buf), 0, NULL, NULL);
  if (packetRead < 0) {
    perror("recvfrom");
    close(sockfd);
    return 1;
  }

  // printing the number of bytes received
  printf("Received %zd bytes\n", packetRead);

  struct DNSHeader *header = (struct DNSHeader *)buf;

  uint16_t id = ntohs(header->id);
  // uint16_t flags = ntohs(header->flags);
  uint16_t q_count = ntohs(header->q_count);
  uint16_t ans_count = ntohs(header->ans_count);
  // uint16_t auth_count = ntohs(header->auth_count);
  // uint16_t add_count = ntohs(header->add_count);

  printf("Id: %u\n", id);
  printf("Q_COUNT: %u\n", q_count);
  printf("ANS_COUNT: %u\n", ans_count);

  pos = 12;

  printf("QNAME:");
  get_name(buf, &pos);
  printf("\n");
  // pos++;

  // uint16_t qtype;
  memcpy(&qtype, buf + pos, sizeof(qtype));
  qtype = ntohs(qtype);
  printf("QTYPE: %u\n", qtype);
  pos += 2;

  // uint16_t qclass;
  memcpy(&qclass, buf + pos, sizeof(qclass));
  qclass = ntohs(qclass);
  printf("QCLASS: %u\n", qclass);
  pos += 2;

  // printf("NAME:");
  for (int i = 0; i < ans_count; i++) {
    printf("\nANSWER %d\n", i + 1);

    if ((buf[pos] & 0xc0) == 0xc0) {
      printf("NAME: compression pointer\n");
      pos += 2;
    } else {
      printf("NAME: ");
      get_name(buf, &pos);
      printf("\n");
    }

    uint16_t type;
    memcpy(&type, buf + pos, sizeof(type));
    type = ntohs(type);
    printf("TYPE: %u\n", type);
    pos += 2;

    uint16_t answer_class;
    memcpy(&answer_class, buf + pos, sizeof(answer_class));
    answer_class = ntohs(answer_class);
    printf("CLASS: %u\n", answer_class);
    pos += 2;

    uint32_t ttl;
    memcpy(&ttl, buf + pos, sizeof(ttl));
    ttl = ntohs(ttl);
    printf("TTL: %u\n", ttl);
    pos += 4;

    uint16_t rdlength;
    memcpy(&rdlength, buf + pos, sizeof(rdlength));
    rdlength = ntohs(rdlength);
    printf("RDLENGTH: %d\n", rdlength);
    pos += 2;

    if (type == 1 && rdlength == 4) {
      char ip[INET_ADDRSTRLEN];
      if (inet_ntop(AF_INET, buf + pos, ip, sizeof(ip)) != NULL) {
        printf("IP ADDRESS: %s\n", ip);
      }
    }

    pos += rdlength;
  }

  close(sockfd);
  return 0;
}
