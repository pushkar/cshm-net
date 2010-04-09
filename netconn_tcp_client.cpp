#include "netconn.h"
#include "netconn_tcp_client.h"
#include <achshm.h>
#include <cshm.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>

TCPClient::TCPClient(const char* ip, const char* port, const char* _channel_name, int _total_frames, size_t _frame_size) {
	channel_name = new char[100];
	memcpy(channel_name, _channel_name, 100);
	total_frames = _total_frames;
	frame_size = _frame_size;
	receive_ip = new char[20];
	receive_port = new char[20];
	memcpy(receive_ip, ip, 20);
	memcpy(receive_port, port, 20);

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = 0;
	hints.ai_protocol = 0;
}

gnet_error TCPClient::Receive(void* buffer, size_t* buffer_size, size_t max_buffer_size) {
	int numbytes = 0;
	if ((numbytes = recv(socket_fd, buffer, max_buffer_size, 0)) == -1) {
		perror("recv");
		return GNET_RECV_ERROR;
	}

	*buffer_size = numbytes;
	return GNET_OK;
}

// TODO: Link in Client
gnet_error TCPClient::Link() {
	ach = new cshmVirtualPublisher(channel_name, total_frames, frame_size);
	err = ach->open();
	if(err != ACH_OK) {
		cshm_error_cstr(err);
		return GNET_PUBLISHING_ERROR;
	}

	cshm_buffer_info_t info;
	unsigned char* buffer = new unsigned char[frame_size + frame_header_size];
	unsigned char* header = buffer + frame_size;
	size_t buffer_size = frame_size + frame_header_size;

	// TODO: While loop checks
	while(1) {
		if(Receive(buffer, &buffer_size, buffer_size + 1) == GNET_OK) {
			memcpy(&info, header, frame_header_size);
			ach->set(buffer, &info);
		}
	}

	free(buffer);
	return GNET_OK;
}

gnet_error TCPClient::Connect() {
	if((rv = getaddrinfo(receive_ip, receive_port, &hints, &p)) != 0)
		return GNET_GETADDR_FAILED;

	if ((socket_fd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1)
		return GNET_SOCKET_FAILED;

	if (connect(socket_fd, p->ai_addr, p->ai_addrlen) == -1) {
		close(socket_fd);
		return GNET_CONNECTION_FAILED;
	}

	if(p == NULL) {
		fprintf(stderr, "Failed on first attempt\n");
		return GNET_CONNECTION_FAILED;
	}

	return GNET_OK;
}

void TCPClient::Close() {
	delete ach;
	free(p);
	close(socket_fd);
}

TCPClient::~TCPClient() {
	Close();
}
