/***************************************************/
/*     created by TheWebServerTeam 2/18/23         */
/***************************************************/

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include <errno.h>

#define ISVALIDSOCKET(s) ((s) >= 0)
#define CLOSESOCKET(s) close(s)
#define SOCKET int
#define GETSOCKETERRNO() (errno)

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include <unistd.h>
#include <fcntl.h>
#include <iostream>

int	main(int ac, char **av)
{
	int status;
	if (ac < 3) {
		fprintf(stderr, "usage: tcp_client hostname port\n");
		return 1;
	}

//=============================

	struct addrinfo hints;
	memset(&hints, 0, sizeof(hints));
	hints.ai_socktype = SOCK_STREAM; //TCP SOCK_DGRAM for UDP
	struct addrinfo *peer_address;
	status = getaddrinfo(av[1], av[2], &hints, &peer_address);
	if (status){
		fprintf(stderr, "getaddrinfo() failed. (%d)\n", GETSOCKETERRNO());
		return 1;
	}

//=============================

	printf("Remote address is: ");
	char address_buffer[100];
	char service_buffer[100];
	status = getnameinfo(peer_address->ai_addr, peer_address->ai_addrlen, address_buffer, 100, service_buffer, 100, NI_NUMERICHOST);
	if (status){
		fprintf(stderr, "getnameinfo() failed. (%d)\n", GETSOCKETERRNO());
		return 1;
	}
	printf("%s %s\n", address_buffer, service_buffer);

//=============================

	printf("Creating socket...\n");
	SOCKET socket_peer;
	socket_peer = socket(peer_address->ai_family, peer_address->ai_socktype, peer_address->ai_protocol);
	if (!ISVALIDSOCKET(socket_peer)) {
		fprintf(stderr, "socket() failed. (%d)\n", GETSOCKETERRNO());
		return 1;
	}

//=============================

	printf("Connecting...\n");
	status = connect(socket_peer, peer_address->ai_addr, peer_address->ai_addrlen);
	if (status){
		fprintf(stderr, "connect() failed. (%d)\n", GETSOCKETERRNO());
		return (1);
	}
	printf("Connected.\n");
	freeaddrinfo(peer_address);

//=============================

	//printf("To send data, enter text followed by enter.\n");
	int file = open("../Configuration/clientRequest.txt", O_RDONLY);
	if (file != -1){
		char buffer[1024];
		int nbRead;
		while ((nbRead = read(file, buffer, 1024))){
			int bytes_sent = send(socket_peer, buffer, nbRead, 0);
			printf("Sent %d bytes.\n", bytes_sent);
		}
	}else{
		printf("error\n");
		close(socket_peer);
		exit(1);
	}

	printf("Waiting\n");
	fd_set			reads;

	FD_ZERO(&reads);
	FD_SET(socket_peer, &reads);
	while (1) {
		fd_set tmp = reads;
		status = select(socket_peer + 1, &tmp, NULL, NULL, 0);
		if (status < 0) {
			fprintf(stderr, "select() failed. (%d)\n", GETSOCKETERRNO());
			return 1;
		}

		//=============================

		if (FD_ISSET(socket_peer, &tmp)) {
			char readChunk[1090];
			memset(readChunk,0,1090);
			ssize_t bytes_received = read(socket_peer, readChunk, 1090);
			std::cout << "byte = "  << bytes_received << std::endl;
			if (bytes_received < 1) {
				printf("Connection closed by peer.\n");
				break;
			}
			printf("pp\n");
			write(1, readChunk, bytes_received);
		}
	}

	printf("Closing socket...\n");
	CLOSESOCKET(socket_peer);
	printf("Finished.\n");
	return (0);
}
