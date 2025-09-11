// this code does skdl

#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <netinet/ip_icmp.h>
#include <sys/time.h>

#define EXPECTED_ARGCOUNT 2
#define PACKET_SIZE 64

struct icmp_hdr{
	uint8_t type;
	uint8_t code;
	uint16_t checksum;
	uint16_t id;
	uint16_t seq;
};

unsigned short checksum(void *b, int len){
	unsigned short *buf = b;
	unsigned int sum = 0;
	unsigned short result;

	for (sum = 0; len > 1; len -= 2)
		sum += *buf++;
	if(len == 1)
		sum += *(unsigned char*)buf;
	sum = (sum >> 16) + (sum & 0xFFFF);
	sum += (sum >> 16);
	result = ~sum;
	return result;	
}


int main(int argc, char **argv){
	if (argc != EXPECTED_ARGCOUNT){
		fprintf(stderr,"Usage: %s <IP address>\n", argv[0]);
		exit(1);
	}

	// check for root access
	if (getuid() != 0){
		fprintf(stderr, "%s: This program requires root privilege", argv[0]);
		exit(1);
	}
	// create a socket
	int sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);

	if (sockfd < 0){
		perror("Socket creation failed");
		exit(1);
	}

	char sendbuf[PACKET_SIZE], recvbuf[1024];
	struct icmphdr *icmp;
	struct iphdr *ip;
	struct timeval start, end;

	struct sockaddr_in dest_addr;
	memset(&dest_addr, 0, sizeof(dest_addr));
	dest_addr.sin_family = AF_INET;
	

	if (inet_pton(AF_INET, argv[1], &dest_addr.sin_addr) <= 0) {
	    perror("Invalid IP address");
	    exit(EXIT_FAILURE);
	}

	printf("Socket created. Ready to build ICMP packet to %s\n", argv[1]);

	//struct icmp_hdr icmp;
	memset(sendbuf,0,PACKET_SIZE);
	icmp = (struct icmphdr *)sendbuf;
	icmp->type = ICMP_ECHO;
	icmp->code = 0;
	icmp->un.echo.id = getpid() & 0xFFFF;
	icmp->un.echo.sequence = 1;
	//icmp->id = htons(123);
	//icmp->seq = htons(1); 
	icmp->checksum = 0;
	icmp->checksum = checksum(&icmp, sizeof(icmp));

	// Send packet
	gettimeofday(&start, NULL);
	if (sendto(sockfd, sendbuf, PACKET_SIZE, 0, (struct sockaddr*)&dest_addr, sizeof(dest_addr)) <= 0) {
	    perror("sendto failed");
	    exit(1);
	}
	
	// Receive reply
	socklen_t addr_len = sizeof(dest_addr);
	ssize_t n = recvfrom(sockfd, recvbuf, sizeof(recvbuf), 0, (struct sockaddr*)&dest_addr, &addr_len);
	if (n <= 0) {
	    perror("recvfrom failed");
	    exit(1);
	}
	gettimeofday(&end, NULL);

	 // Extract IP and ICMP headers
	ip = (struct iphdr *)recvbuf;
	icmp = (struct icmphdr *)(recvbuf + (ip->ihl * 4));

	if (icmp->type == ICMP_ECHOREPLY) {
	    double rtt = (end.tv_sec - start.tv_sec) * 1000.0 + (end.tv_usec - start.tv_usec) / 1000.0;
	    printf("Reply from %s: icmp_seq=%d time=%.3f ms\n", argv[1], icmp->un.echo.sequence, rtt);
	} else {
	    printf("Received ICMP type %d code %d\n", icmp->type, icmp->code);
	}
	

	
	close(sockfd);
	return 0;
}
