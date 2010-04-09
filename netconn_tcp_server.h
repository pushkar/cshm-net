/*
 * netconn_tcp.h
 *
 *  Created on: Apr 11, 2009
 *      Author: pushkar
 */

#ifndef NETCONN_TCP_SERVER_H_
#define NETCONN_TCP_SERVER_H_

#include "netconn.h"
#include <achshm.h>
#include <cshm.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>

class TCPServer : public NetworkConnection {

private:
	char* receive_port;
	struct addrinfo hints, *p;
	int socket_fd;
	struct sockaddr_storage client_addr;
	int rv, yes;
	int new_fd;
	socklen_t sin_size;

	char* channel_name;

	cshmSubscriber* ach;
	cshm_buffer_info_t info;
	cshm_error err;

	size_t frame_size, frame_header_size;

public:
	TCPServer(const char* port, const char* channel_name);

	gnet_error SendStream(void *buffer, size_t buffer_size);

	gnet_error Listen();

	gnet_error Link();

	gnet_error LinkLatest();

	gnet_error Connect();

	void Close();

	~TCPServer();
};

#endif /* NETCONN_TCP_H_ */
