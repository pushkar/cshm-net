#include "netconn.h"
#include "netconn_tcp_server.h"
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

TCPServer::TCPServer(const char* port, const char* _channel_name) {
	channel_name = new char[100];
	memcpy(channel_name, _channel_name, 100);
	receive_port = new char[20];
	memcpy(receive_port, port, 20);
	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE; // use my IP

	sin_size = sizeof client_addr;
}

gnet_error TCPServer::SendStream(void *buffer, size_t buffer_size) {
	send(new_fd, buffer, buffer_size, 0);
	return GNET_OK;
}

gnet_error TCPServer::Listen() {
	if (listen(socket_fd, 10) == -1) {
		perror("listen");
		return GNET_LISTEN_FAILED;
	}

	new_fd = accept(socket_fd, (struct sockaddr*)&client_addr, &sin_size);
	if(new_fd == -1)
		return GNET_SEND_ERROR;

	return GNET_OK;
}

gnet_error TCPServer::Link() {
	if(Listen() != GNET_OK)
		return GNET_LISTEN_FAILED;

	ach = new cshmSubscriber(channel_name);
	err = ach->open();
	if(err != ACH_OK) {
		cshm_error_cstr(err);
		return GNET_SUBSCRIPTION_ERROR;
	}

	frame_size = ach->getFrameSize();
	frame_header_size = sizeof(cshm_buffer_info_t);
	unsigned char* buffer = new unsigned char[frame_size + frame_header_size];
	unsigned char* header = buffer + frame_size;
	size_t buffer_size = frame_size + frame_header_size;

	// TODO: Add zeros to buffer
	while(1) {
		err = ach->getNext(buffer, &info);
		if(err == ACH_OK) {
			memcpy(header, &info, frame_header_size);
			SendStream(buffer, buffer_size);
		}
		else
			cshm_error_cstr(err);
	}

	free(buffer);
	return GNET_OK;
}

gnet_error TCPServer::LinkLatest() {
	if(Listen() != GNET_OK)
		return GNET_LISTEN_FAILED;

	ach = new cshmSubscriber(channel_name);
	err = ach->open();
		if(err != ACH_OK) {
			cshm_error_cstr(err);
			return GNET_SUBSCRIPTION_ERROR;
		}

		frame_size = ach->getFrameSize();
		frame_header_size = sizeof(cshm_buffer_info_t);
		unsigned char* buffer = new unsigned char[frame_size + frame_header_size];
		unsigned char* header = buffer + frame_size;
		size_t buffer_size = frame_size + frame_header_size;

		// TODO: Add zeros to buffer
		while(1) {
			err = ach->getLatest(buffer, &info);
			if(err == ACH_OK) {
				memcpy(header, &info, frame_header_size);
				SendStream(buffer, buffer_size);
			}
			else
				cshm_error_cstr(err);
		}

	free(buffer);
	return GNET_OK;
}

gnet_error TCPServer::Connect() {
	if((rv = getaddrinfo(NULL, receive_port, &hints, &p)) != 0)
		return GNET_GETADDR_FAILED;

	if ((socket_fd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1)
		return GNET_SOCKET_FAILED;

	if (setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1) {
		close(socket_fd);
		return GNET_SOCKET_FAILED; // socket reset failed
	}

	if (bind(socket_fd, p->ai_addr, p->ai_addrlen) == -1) {
		close(socket_fd);
		return GNET_SOCKET_FAILED; // socket binding failed
	}

	if(p == NULL)
		return GNET_CONNECTION_FAILED;

	return GNET_OK;
}

void TCPServer::Close() {
	free(p);
	close(new_fd);
	close(socket_fd);
}

TCPServer::~TCPServer() {
	Close();
}
