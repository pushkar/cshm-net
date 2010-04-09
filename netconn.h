/*
 * netconn.h
 *
 *  Created on: Apr 1, 2009
 *      Desc  : Networking library to replicate channels
 */

#ifndef NETCONN_H_
#define NETCONN_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef enum {GNET_OK,
	GNET_GETADDR_FAILED,
	GNET_CONNECTION_FAILED,
	GNET_LISTEN_FAILED,
	GNET_SOCKET_FAILED,
	GNET_RECV_ERROR,
	GNET_SEND_ERROR,
	GNET_SUBSCRIPTION_ERROR,
	GNET_PUBLISHING_ERROR
	} gnet_error;

class NetworkConnection {
public:
	virtual gnet_error Connect() = 0;

	virtual gnet_error Link() = 0;

	virtual void Close() = 0;

	~NetworkConnection() {
	}
};

void network_connection_error_cstr(gnet_error err);

#endif /* NETCONN_H_ */
