/*
 * netconn_tcp.h
 *
 *  Created on: Apr 11, 2009
 *      Author: pushkar
 */

#ifndef NETCONN_TCP_CLIENT_H_
#define NETCONN_TCP_CLIENT_H_

#include "netconn.h"
#include <achshm.h>
#include <cshm.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>

class TCPClient : public NetworkConnection {

private:
	int socket_fd, rv;
	struct addrinfo hints, *p;

	char* receive_ip;
	char* receive_port;

	char* channel_name;
	int total_frames;

	cshmVirtualPublisher *ach;
	cshm_buffer_info_t info;
	cshm_error err;
	size_t frame_size, frame_header_size;


public:
	TCPClient(const char* ip, const char* port, const char* _channel_name, int _total_frames, size_t _frame_size);

	gnet_error Receive(void* buffer, size_t* buffer_size, size_t max_buffer_size);

	gnet_error Link();

	gnet_error Connect();

	void Close();

	~TCPClient();
};

#endif /* NETCONN_TCP_H_ */
