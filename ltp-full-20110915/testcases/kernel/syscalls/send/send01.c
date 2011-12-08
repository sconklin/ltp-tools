/*
 *
 *   Copyright (c) International Business Machines  Corp., 2001
 *
 *   This program is free software;  you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation; either version 2 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY;  without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See
 *   the GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program;  if not, write to the Free Software
 *   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 */

/*
 * Test Name: send01
 *
 * Test Description:
 *  Verify that send() returns the proper errno for various failure cases
 *
 * Usage:  <for command-line>
 *  send01 [-c n] [-e] [-i n] [-I x] [-p x] [-t]
 *     where,  -c n : Run n copies concurrently.
 *             -e   : Turn on errno logging.
 *	       -i n : Execute test n times.
 *	       -I x : Execute test for x seconds.
 *	       -P x : Pause for x seconds between iterations.
 *	       -t   : Turn on syscall timing.
 *
 * HISTORY
 *	07/2001 Ported by Wayne Boyer
 *
 * RESTRICTIONS:
 *  None.
 *
 */

#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/signal.h>
#include <sys/un.h>

#include <netinet/in.h>

#include "test.h"
#include "usctest.h"

char *TCID = "send01";		/* Test program identifier.    */
int testno;

char buf[1024], bigbuf[128 * 1024];
int s;				/* socket descriptor */
struct sockaddr_in sin1;
static int sfd;			/* shared between do_child and start_server */

void setup(void), setup0(void), setup1(void), setup2(void),
do_child(void), cleanup(void), cleanup0(void), cleanup1(void);

struct test_case_t {		/* test case structure */
	int domain;		/* PF_INET, PF_UNIX, ... */
	int type;		/* SOCK_STREAM, SOCK_DGRAM ... */
	int proto;		/* protocol number (usually 0 = default) */
	void *buf;		/* send data buffer */
	int buflen;		/* send's 3rd argument */
	unsigned flags;		/* send's 4th argument */
	int retval;		/* syscall return value */
	int experrno;		/* expected errno */
	void (*setup) (void);
	void (*cleanup) (void);
	char *desc;
} tdat[] = {
	{
	PF_INET, SOCK_STREAM, 0, buf, sizeof(buf), 0,
		    -1, EBADF, setup0, cleanup0, "bad file descriptor"}
	, {
	0, 0, 0, buf, sizeof(buf), 0,
		    -1, ENOTSOCK, setup0, cleanup0, "invalid socket"}
	,
#ifndef UCLINUX
	    /* Skip since uClinux does not implement memory protection */
	{
	PF_INET, SOCK_STREAM, 0, (void *)-1, sizeof(buf), 0,
		    -1, EFAULT, setup1, cleanup1, "invalid send buffer"}
	,
#endif
	{
	PF_INET, SOCK_DGRAM, 0, bigbuf, sizeof(bigbuf), 0,
		    -1, EMSGSIZE, setup1, cleanup1, "UDP message too big"}
	, {
	PF_INET, SOCK_STREAM, 0, buf, sizeof(buf), 0,
		    -1, EPIPE, setup2, cleanup1, "local endpoint shutdown"}
	,
#ifndef UCLINUX
	    /* Skip since uClinux does not implement memory protection */
	{
	PF_INET, SOCK_STREAM, 0, (void *)-1, sizeof(buf), -1,
		    -1, EFAULT, setup1, cleanup1, "invalid flags set"}
	,
#endif
};

int TST_TOTAL = sizeof(tdat) / sizeof(tdat[0]);	/* Total number of test cases. */

int exp_enos[] = { EBADF, ENOTSOCK, EFAULT, EMSGSIZE, EPIPE, EINVAL, 0 };


#ifdef UCLINUX
static char *argv0;
#endif

pid_t start_server(struct sockaddr_in *sin0)
{
	struct sockaddr_in sin1 = *sin0;
	pid_t pid;

	sfd = socket(PF_INET, SOCK_STREAM, 0);
	if (sfd < 0) {
		tst_brkm(TBROK|TERRNO, cleanup, "server socket failed");
		return -1;
	}
	if (bind(sfd, (struct sockaddr *)&sin1, sizeof(sin1)) < 0) {
		tst_brkm(TBROK|TERRNO, cleanup, "server bind failed");
		return -1;
	}
	if (listen(sfd, 10) < 0) {
		tst_brkm(TBROK|TERRNO, cleanup, "server listen failed");
		return -1;
	}
	switch ((pid = FORK_OR_VFORK())) {
	case 0:		/* child */
#ifdef UCLINUX
		if (self_exec(argv0, "d", sfd) < 0)
			tst_brkm(TBROK|TERRNO, cleanup, "server self_exec failed");
#else
		do_child();
#endif
		break;
	case -1:
		tst_brkm(TBROK|TERRNO, cleanup, "server fork failed");
		/* fall through */
	default:		/* parent */
		(void)close(sfd);
		return pid;
	}

	  exit(1);
}

void do_child()
{
	fd_set afds, rfds;
	int nfds, cc, fd;
	struct sockaddr_in fsin;

	FD_ZERO(&afds);
	FD_SET(sfd, &afds);

	nfds = getdtablesize();

	/* accept connections until killed */
	while (1) {
		socklen_t fromlen;

		memcpy(&rfds, &afds, sizeof(rfds));

		if (select(nfds, &rfds, (fd_set *) 0, (fd_set *) 0,
			   (struct timeval *)0) < 0)
			if (errno != EINTR)
				exit(1);
		if (FD_ISSET(sfd, &rfds)) {
			int newfd;

			fromlen = sizeof(fsin);
			newfd = accept(sfd, (struct sockaddr *)&fsin, &fromlen);
			if (newfd >= 0)
				FD_SET(newfd, &afds);
		}
		for (fd = 0; fd < nfds; ++fd) {
			if (fd != sfd && FD_ISSET(fd, &rfds)) {
				cc = read(fd, buf, sizeof(buf));
				if (cc == 0 || (cc < 0 && errno != EINTR)) {
					(void)close(fd);
					FD_CLR(fd, &afds);
				}
			}
		}
	}
}

int main(int ac, char *av[])
{
	int lc;			/* loop counter */
	char *msg;		/* message returned from parse_opts */

	/* Parse standard options given to run the test. */
	if ((msg = parse_opts(ac, av, NULL, NULL)) != NULL)
		tst_brkm(TBROK, NULL, "OPTION PARSING ERROR - %s", msg);
#ifdef UCLINUX
	argv0 = av[0];
	maybe_run_child(&do_child, "d", &sfd);
#endif

	setup();

	TEST_EXP_ENOS(exp_enos);

	for (lc = 0; TEST_LOOPING(lc); ++lc) {

		Tst_count = 0;

		for (testno = 0; testno < TST_TOTAL; ++testno) {
			tdat[testno].setup();

			TEST(send
			     (s, tdat[testno].buf, tdat[testno].buflen,
			      tdat[testno].flags));

			if (TEST_RETURN != -1) {
				tst_resm(TFAIL, "call succeeded unexpectedly");
				continue;
			}

			TEST_ERROR_LOG(TEST_ERRNO);

			if (TEST_ERRNO != tdat[testno].experrno) {
				tst_resm(TFAIL, "%s ; returned"
					 " %ld (expected %d), errno %d (expected"
					 " %d)", tdat[testno].desc,
					 TEST_RETURN, tdat[testno].retval,
					 TEST_ERRNO, tdat[testno].experrno);
			} else {
				tst_resm(TPASS, "%s successful",
					 tdat[testno].desc);
			}
			tdat[testno].cleanup();
		}
	}
	cleanup();

	tst_exit();
}

pid_t pid;

void setup(void)
{
	TEST_PAUSE;		/* if -P option specified */

	/* initialize sockaddr's */
	sin1.sin_family = AF_INET;
	sin1.sin_port = htons((getpid() % 32768) + 11000);
	sin1.sin_addr.s_addr = INADDR_ANY;
	pid = start_server(&sin1);

	(void)signal(SIGPIPE, SIG_IGN);
}

void cleanup(void)
{
	(void)kill(pid, SIGKILL);	/* kill server */
	TEST_CLEANUP;

}

void setup0(void)
{
	if (tdat[testno].experrno == EBADF)
		s = 400;	/* anything not an open file */
	else if ((s = open("/dev/null", O_WRONLY)) == -1)
		tst_brkm(TBROK|TERRNO, cleanup, "open(/dev/null) failed");
}

void cleanup0(void)
{
	s = -1;
}

void setup1(void)
{
	s = socket(tdat[testno].domain, tdat[testno].type, tdat[testno].proto);
	if (s < 0)
		tst_brkm(TBROK|TERRNO, cleanup, "socket setup failed");
	if (connect(s, (const struct sockaddr *)&sin1, sizeof(sin1)) < 0)
		tst_brkm(TBROK|TERRNO, cleanup, "connect failed");
}

void cleanup1(void)
{
	(void)close(s);
	s = -1;
}

void setup2(void)
{
	setup1();		/* get a socket in s */
	if (shutdown(s, 1) < 0)
		tst_brkm(TBROK|TERRNO, cleanup, "socket setup failed connect "
			 "test %d", testno);
}