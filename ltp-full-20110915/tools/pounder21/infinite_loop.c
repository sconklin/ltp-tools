/* Repeatedly run a program. */

/*
 * Copyright (C) 2003-2006 IBM
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of the
 * License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA
 * 02111-1307, USA.
 */

#include <stdio.h>
#include <string.h>
#include <strings.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#include "debug.h"

static int res = 0;
static char *progname;
static pid_t test_pgrp;
static FILE *tty_fp;

static void int_func(int signum) {
	pounder_fprintf(tty_fp, "%s: Killed by interrupt.  Last exit code = %d.\n",
		progname, res);
	kill(-test_pgrp, SIGTERM);
	exit(res);
}

int main(int argc, char *argv[]) {
	int stat;
	pid_t pid;
	struct sigaction zig;
	unsigned int revs = 0;

	if (argc < 2) {
		printf("Usage: %s command [args]\n", argv[0]);
		exit(1);
	}

	tty_fp = fdopen(3, "w+");
	if (tty_fp == NULL) {
		tty_fp = fopen("/dev/tty", "w+");
		if (tty_fp == NULL) {
			perror("stdout");
			exit(2);
		}
	}

	progname = rindex(argv[1], '/');
	if (progname == NULL) {
		progname = argv[1];
	} else {
		progname++;
	}

	/* Set up signals */
	memset(&zig, 0x00, sizeof(zig));
	zig.sa_handler = int_func;
	sigaction(SIGINT, &zig, NULL);
	sigaction(SIGTERM, &zig, NULL);

	/* set up process groups so that we can kill the
	 * loop test and descendants easily */

	while (1) {
		pounder_fprintf(tty_fp, "%s: %s loop #%d.\n", progname, start_msg, revs++);
		pid = fork();
		if (pid == 0) {
			if (setpgrp() < 0) {
				perror("setpgid");
			}

			// run the program
			if (argc > 3) {
				stat = execvp(argv[1], &argv[1]);
			} else {
				stat = execvp(argv[1], &argv[1]);
			}

			perror(argv[1]);

			exit(-1);
		}

		/* save the pgrp of the spawned process */
		test_pgrp = pid;

		// wait for it to be done
		if (waitpid(pid, &stat, 0) != pid) {
			perror("waitpid");
			exit(1);
		}

		// interrogate it
		if (WIFSIGNALED(stat)) {
			pounder_fprintf(tty_fp, "%s: %s on signal %d.\n",
				progname, fail_msg, WTERMSIG(stat));
			res = 255;
		} else {
			res = WEXITSTATUS(stat);
			if (res == 0) {
				pounder_fprintf(tty_fp, "%s: %s.\n", progname, pass_msg);
			} else if (res < 0 || res == 255) {
				pounder_fprintf(tty_fp, "%s: %s with code %d.\n",
					progname, abort_msg, res);
				exit(-1);
				// FIXME: add test to blacklist
			} else {
				pounder_fprintf(tty_fp, "%s: %s with code %d.\n",
					progname, fail_msg, res);
			}
		}
	}
}