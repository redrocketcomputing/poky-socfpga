/*
 * Copyright (c) 2013-2014, Altera Corporation <www.altera.com>
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 * - Redistributions of source code must retain the above copyright
 *   notice, this list of conditions and the following disclaimer.
 * - Redistributions in binary form must reproduce the above copyright
 *   notice, this list of conditions and the following disclaimer in the
 *   documentation and/or other materials provided with the distribution.
 * - Neither the name of the Altera Corporation nor the
 *   names of its contributors may be used to endorse or promote products
 *   derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL ALTERA CORPORATION BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <assert.h>
#include <errno.h>
#include <fcntl.h>
#include <limits.h>
#include <mcapi.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>

#define WAIT_TIMEOUT_MS 10000

#define mcapi_assert_success(s) \
	if (s != MCAPI_SUCCESS) { printf("%s:%d status %d\n", __FILE__, __LINE__, s); abort(); }

const int tx_port = 1000;
const int rx_port = 1001;

mcapi_pktchan_recv_hndl_t	send_handle;
mcapi_pktchan_recv_hndl_t	recv_handle;

/* Start the connection of the endpoints
   This will create all the endpoints, establish a connection and open the connection for
   data transfers */
static void connect(int local, int remote)
{
	mcapi_endpoint_t local_send_endpoint;
	mcapi_endpoint_t local_recv_endpoint;
	mcapi_endpoint_t remote_recv_endpoint;
	mcapi_request_t  request;
	mcapi_request_t  send_request;
	mcapi_request_t  recv_request;
	mcapi_status_t   status;
	size_t           size;

	printf("Node %d: Creating tx port %d\n", local, tx_port);
	local_send_endpoint = mcapi_create_endpoint(tx_port, &status);
	mcapi_assert_success(status);

	printf("Node %d: Creating rx port %d\n", local, rx_port);
	local_recv_endpoint = mcapi_create_endpoint(rx_port, &status);
	mcapi_assert_success(status);

	printf("Node %d: Get remote rx port %d from node %d\n", local, rx_port, remote);
	remote_recv_endpoint = mcapi_get_endpoint(remote, rx_port, &status);
	mcapi_assert_success(status);

	printf("Node %d: Connecting %d:%d to %d:%d\n",local, local, tx_port,
	       remote, rx_port);
	mcapi_connect_pktchan_i(local_send_endpoint, remote_recv_endpoint,
	                        &request, &status);
	mcapi_assert_success(status);

	printf("Node %d: Waiting for connection setup with Node %d\n", local, rx_port);
	mcapi_wait(&request, &size, &status, WAIT_TIMEOUT_MS);
	mcapi_assert_success(status);

	printf("Node %d: Connection complete\n", local);

	printf("Node %d: Opening send endpoint\n", local);
	mcapi_open_pktchan_send_i(&send_handle, local_send_endpoint, &send_request,
							  &status);

	printf("Node %d: Opening receive endpoint\n", local);
	mcapi_open_pktchan_recv_i(&recv_handle, local_recv_endpoint, &recv_request,
							  &status);

	mcapi_wait(&send_request, &size, &status, WAIT_TIMEOUT_MS);
	mcapi_assert_success(status);

	mcapi_wait(&recv_request, &size, &status, WAIT_TIMEOUT_MS);
	mcapi_assert_success(status);

	printf("Node %d: MCAPI negotiation complete! \n", local);
}

/* Disconnects and close all endpoint connections */
void disconnect(int local, int remote)
{
	mcapi_request_t  request;
	mcapi_request_t  send_request;
	mcapi_request_t  recv_request;
	mcapi_status_t   status;
	size_t           size;

	printf("Node %d: Disconnecting ...\n", local);

	printf("Node %d: Closing send endpoint\n", local);
	mcapi_packetchan_send_close_i(send_handle, &send_request, &status);

	printf("Node %d: Closing receive endpoint\n", local);
	mcapi_packetchan_recv_close_i(recv_handle, &recv_request, &status);

	/* This does not necessarily success. If the connection hasn't been
	 * established, the correct behavior is to timeout */
	mcapi_wait(&send_request, &size, &status, WAIT_TIMEOUT_MS);
	if(status != 0)
		printf("Error %d waiting for node %d send endpoint to be closed\n",
		status, local);

	mcapi_wait(&recv_request, &size, &status, WAIT_TIMEOUT_MS);
	if(status != 0)
		printf("Error %d waiting for node %d receive endpoint to be closed\n",
		status, local);

	printf("Node %d: MCAPI disconnection complete! \n", local);
}

/* Startup and initialization sequence */
void startup(unsigned int local, unsigned int remote)
{
	mcapi_status_t status;
	mcapi_version_t version;

	printf("Node %d: MCAPI Initialized\n",local);
	mcapi_initialize(local, &version, &status);
	mcapi_assert_success(status);

	connect(local, remote);
}

/* The data transfer that exchange text data with the other node */
void demo(unsigned int node, int loop)
{
	char outgoing[100];
	char *incoming;
	size_t bytes;
	mcapi_status_t status;
	int i = 0;

	do {
		memset(outgoing, 0, 100);
		sprintf(outgoing, "hi from node %d - %d", node, i);

		mcapi_pktchan_send(send_handle, outgoing, strlen(outgoing)+1,
			&status);
		mcapi_assert_success(status);

		mcapi_pktchan_recv(recv_handle, (void *)&incoming, &bytes,
			&status);

		printf("received message %d: %s\n", i, incoming);
		i++;

		mcapi_pktchan_free(incoming, &status);
		mcapi_assert_success(status);

		sleep(1);
	} while (loop);
}
