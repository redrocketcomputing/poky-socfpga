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

#include <getopt.h>
#include <mcapi.h>
#include "mcapi_demo.h"
#include <signal.h>
#include <stdlib.h>
#include <stdio.h>


static struct sigaction oldactions[32];
unsigned long local;
unsigned long remote;

static void usage(const char *name)
{
	printf("Usage: %s [options] <local node id> <remote node id>\n"
			"Options:\n"
			"  -l, --loop      send and receive messages until killed.\n"
			"  -h, --help      displays help and exit.\n",
			name);
}

/* Clean up that will be called on exit */
static void cleanup(void)
{
	mcapi_status_t status;

	printf("%s\n", __func__);
	disconnect(local, remote);
	mcapi_finalize(&status);
}

static void signalled(int signal, siginfo_t *info, void *context)
{
	struct sigaction *action;

	action = &oldactions[signal];

	if ((action->sa_flags & SA_SIGINFO) && action->sa_sigaction)
		action->sa_sigaction(signal, info, context);
	else if (action->sa_handler)
		action->sa_handler(signal);

	exit(signal);
}

struct sigaction action = {
	.sa_sigaction = signalled,
	.sa_flags = SA_SIGINFO,
};

int main(int argc, char *argv[])
{
	int c;
	int loop = 0;

	while (1) {
		static struct option long_options[] = {
			{"loop", 0, 0, 'l'},
			{"help", 0, 0, 'h'},
			{NULL, 0, 0, 0},
		};
		int option_index = 0;

		c = getopt_long(argc, argv, "l:h:", long_options, &option_index);

		if (c == -1)
			break;

		switch (c) {
		case 'l':
			loop = 1;
			break;
		case 'h':
			usage(argv[0]);
			exit(0);
		default:
			usage(argv[0]);
			exit(1);
		}
	}

	if (optind == argc - 2) {
		local = strtoul(argv[optind], NULL, 0);
		remote = strtoul(argv[optind + 1], NULL, 0);
	} else {
		usage(argv[0]);
		exit(1);
	}

	atexit(cleanup);
	sigaction(SIGQUIT, &action, &oldactions[SIGQUIT]);
	sigaction(SIGABRT, &action, &oldactions[SIGABRT]);
	sigaction(SIGTERM, &action, &oldactions[SIGTERM]);
	sigaction(SIGINT,  &action, &oldactions[SIGINT]);

	printf("Start MCAPI Demo in Linux\n");
	startup(local, remote);

	demo(local, loop);

	return 0;
}
