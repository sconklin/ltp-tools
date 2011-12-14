/*

 * Copyright (c) 2003, Intel Corporation. All rights reserved.
 * Created by:  salwan.searty REMOVE-THIS AT intel DOT com
 * This file is licensed under the GPL license.  For the full content
 * of this license, see the COPYING file at the top level of this
 * source tree.

 The resulting set shall be the intersection of the current set and the
 complement of the signal set pointed to by set, if the value of the
 argument how is SIG_UNBLOCK
*/

#include <signal.h>
#include <stdio.h>
#include "posixtest.h"

int handler_called = 0;

void handler(int signo)
{
	handler_called = 1;
}

int main()
{
	struct sigaction act;
	sigset_t set1, set2, pending_set;

	sigemptyset(&set1);
	sigaddset(&set1, SIGABRT);
	sigaddset(&set1, SIGUSR2);

	sigemptyset(&set2);
	sigaddset(&set2, SIGUSR2);

	act.sa_handler = handler;
	act.sa_flags = 0;
	sigemptyset(&act.sa_mask);

	if (sigaction(SIGABRT,  &act, 0) == -1) {
		perror("Unexpected error while attempting to setup test "
		       "pre-conditions");
		return PTS_UNRESOLVED;
	}

	if (sigaction(SIGUSR2,  &act, 0) == -1) {
		perror("Unexpected error while attempting to setup test "
		       "pre-conditions");
		return PTS_UNRESOLVED;
	}

	if (sigprocmask(SIG_SETMASK, &set1, NULL) == -1) {
		perror("Unexpected error while attempting to use sigprocmask.\n");
		return PTS_UNRESOLVED;
	}

	if (sigprocmask(SIG_UNBLOCK, &set2, NULL) == -1) {
		perror("Unexpected error while attempting to use sigprocmask.\n");
		return PTS_UNRESOLVED;
	}

	if (raise(SIGUSR2) == -1) {
		perror("Unexpected error while attempting to setup test "
		       "pre-conditions");
		return PTS_UNRESOLVED;
	}

	if (!handler_called) {
		printf("FAIL: Handler was not called for even though signal was removed from the signal mask\n");
		return PTS_UNRESOLVED;
	}

	handler_called = 0;
	if (raise(SIGABRT) == -1) {
		perror("Unexpected error while attempting to setup test "
		       "pre-conditions");
		return PTS_UNRESOLVED;
	}

	if (handler_called) {
		printf("FAIL: Hanlder was called for even though signal should have been in the signal mask\n");
		return PTS_FAIL;
	}

	if (sigpending(&pending_set) == -1) {
		perror("Unexpected error while attempting to use sigpending\n");
		return PTS_UNRESOLVED;
	}

	if (sigismember(&pending_set, SIGABRT) != 1) {
		perror("FAIL: sigismember did not return 1\n");
		return PTS_FAIL;
	}

	if (sigismember(&pending_set, SIGUSR2) != 0) {
		perror("FAIL: sigismember did not return 1\n");
		return PTS_FAIL;
	}

	printf("Test PASSED: signal was added to the process's signal mask\n");
	return 0;
}
