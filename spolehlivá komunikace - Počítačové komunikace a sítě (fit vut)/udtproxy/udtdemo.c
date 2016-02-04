/*
 ============================================================================
 Name        : rdpconnect.c
 Author      : Ondrej Rysavy
 Date        : Feb 26, 2009
 Copyright   : (c) Brno University of Technology
 Description : Simple UDT example.

 Allows to create an UDT end-point that communicates with the opposite side.
 Example:
 eva> ./udtdemo -p 15000 -P 15001
 eva> ./udtdemo -p 15001 -P 15000

 Written line on one side is send to the opposite one after ENTER is pressed
 and vice versa.
 ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include "udt.h"

#define MAXLINE 500
#define PROGRAM "udtdemo"
#define PROGRAM_INFO "UDT Demo, version 1.0 (Feb 27, 2009)\n\n"

in_addr_t remote_addr = 0x7f000001;
in_port_t remote_port = 0;
in_port_t local_port = 0;


int main(int argc, char **argv ) {
	char ch;
	char buf[80];
	char sendline[MAXLINE];
	char recvline[MAXLINE];
	int udt;

	while ((ch = getopt(argc,argv,"p:R:P:h")) != -1) {
		switch(ch) {
		case 'p':
				local_port = atol(optarg);
			break;
		case 'R':
				 remote_addr = ntohl(inet_addr(optarg)); //!!! needs nhotl as udt requires host order!
			break;
		case 'P':
				remote_port = atol(optarg);
			break;
		case 'h':
			fprintf(stdout, PROGRAM_INFO);
			fprintf(stdout, "usage: udtdemo -p port -P port [-R address]\n\n");
			fprintf(stdout, "  p port    : local UDP socket binds to `port'\n" );
			fprintf(stdout, "  P port    : UDP datagrams will be sent to the remote `port'\n" );
			fprintf(stdout, "  R address : UDP datagrams will be sent to the remote host \n              as specified by `address' (an implicit is 127.0.0.1)\n\n" );
			exit(EXIT_SUCCESS);
		}
	}
	fprintf(stderr, PROGRAM_INFO);
	// Complain if something is missing or wrong.
	if (remote_addr == 0 || remote_port == 0 || local_port == 0) {
		fprintf(stderr, "Missing required arguments! Type '%s -h' for help.\n", PROGRAM);
		exit(EXIT_FAILURE);
	}


	fprintf(stderr, "Data channel from localhost:%d to %s:%d.\n", local_port, inet_ntop(AF_INET, &remote_addr, buf, 80), remote_port);
	fprintf(stderr, "Write data content, press ENTER to send the packet.\n");

	// It is important to init UDT!
	udt = udt_init(local_port);

	fcntl(STDIN_FILENO, F_SETFL, O_NONBLOCK);	// make stdin reading non-clocking

	fd_set readfds;
	FD_ZERO(&readfds);
	FD_SET(udt, &readfds);
	FD_SET(STDIN_FILENO, &readfds);
	while (select(udt+1, &readfds, NULL, NULL, NULL)) {
		if (FD_ISSET(STDIN_FILENO, &readfds) && fgets(sendline, MAXLINE, stdin)>0) { // we have read a new line to send
			if (!udt_send(udt, remote_addr, remote_port, sendline, strlen(sendline))) {
				perror("udtdemo: ");	// some error
			}
		}
		if (FD_ISSET(udt, &readfds)) {	// We have a new message to print
			int n = udt_recv(udt, recvline, MAXLINE, NULL, NULL);
			recvline[n] = 0;
			fputs(recvline, stdout);
		}
		// and again!
		FD_ZERO(&readfds);
		FD_SET(udt, &readfds);
		FD_SET(STDIN_FILENO, &readfds);
	}
	return EXIT_SUCCESS;
}
