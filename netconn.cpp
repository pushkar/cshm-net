/*
 * netconn.cpp
 *
 *  Created on: Apr 24, 2009
 *      Author: pushkar
 */


#include "netconn.h"


void network_connection_error_cstr(gnet_error err) {
	{
		switch(err) {
		case GNET_OK: fprintf(stderr, "Ok.\n"); break;
		case GNET_GETADDR_FAILED: fprintf(stderr, "Get Address Failed.\n"); break;
		case GNET_CONNECTION_FAILED: fprintf(stderr, "Get Connection Failed.\n"); break;
		case GNET_LISTEN_FAILED: fprintf(stderr, "Listen Failed.\n"); break;
		case GNET_SOCKET_FAILED: fprintf(stderr, "Socket Binding Failed.\n"); break;
		case GNET_RECV_ERROR: fprintf(stderr, "Failed to receive.\n"); break;
		case GNET_SEND_ERROR: fprintf(stderr, "Failed to send.\n"); break;
		case GNET_SUBSCRIPTION_ERROR: fprintf(stderr, "Failed to subscribe to data.\n"); break;
		case GNET_PUBLISHING_ERROR: fprintf(stderr, "Failed to publish data.\n"); break;
		default:
			break;
		}
	}
}

