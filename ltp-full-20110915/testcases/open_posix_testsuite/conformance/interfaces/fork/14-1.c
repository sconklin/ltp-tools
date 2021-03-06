/*
* Copyright (c) 2004, Bull S.A..  All rights reserved.
* Created by: Sebastien Decugis

* This program is free software; you can redistribute it and/or modify it
* under the terms of version 2 of the GNU General Public License as
* published by the Free Software Foundation.
*
* This program is distributed in the hope that it would be useful, but
* WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
*
* You should have received a copy of the GNU General Public License along
* with this program; if not, write the Free Software Foundation, Inc., 59
* Temple Place - Suite 330, Boston MA 02111-1307, USA.

* This sample test aims to check the following assertion:
*
* Opened semaphores are inherited in the child process..

* The steps are:
* -> Open 2 named semaphores, then unlink one.
* -> fork
* -> The child tries and posts both semaphores, then terminates.
* -> The parent waits for the child, then check the semaphores were posted

* The test fails if any semaphore has not been posted in the child.

*/

/* We are testing conformance to IEEE Std 1003.1, 2003 Edition */
#define _POSIX_C_SOURCE 200112L

/********************************************************************************************/
/****************************** standard includes *****************************************/
/********************************************************************************************/
#include <pthread.h>
 #include <stdarg.h>
 #include <stdio.h>
 #include <stdlib.h>
 #include <string.h>
 #include <unistd.h>

#include <sys/wait.h>
 #include <errno.h>

#include <semaphore.h>
#include <fcntl.h>

/********************************************************************************************/
/******************************   Test framework   *****************************************/
/********************************************************************************************/
#include "testfrmw.h"
 #include "testfrmw.c"
/* This header is responsible for defining the following macros:
 * UNRESOLVED(ret, descr);
 *    where descr is a description of the error and ret is an int (error code for example)
 * FAILED(descr);
 *    where descr is a short text saying why the test has failed.
 * PASSED();
 *    No parameter.
 *
 * Both three macros shall terminate the calling process.
 * The testcase shall not terminate in any other maneer.
 *
 * The other file defines the functions
 * void output_init()
 * void output(char * string, ...)
 *
 * Those may be used to output information.
 */

/********************************************************************************************/
/********************************** Configuration ******************************************/
/********************************************************************************************/
#ifndef VERBOSE
#define VERBOSE 1
#endif

/********************************************************************************************/
/***********************************    Test case   *****************************************/
/********************************************************************************************/
/* The main test function. */
int main(int argc, char * argv[])
{
	int ret, status;
	pid_t child, ctl;

	sem_t * sem_linked, *sem_unlinked;

	/* Initialize output */
	output_init();

	sem_linked = sem_open("/fork_14_1a", O_CREAT, O_RDWR, 0);

	if (sem_linked == SEM_FAILED)
	{
		UNRESOLVED(errno, "Failed to create the named semaphore");
	}

	sem_unlinked = sem_open("/fork_14_1b", O_CREAT, O_RDWR, 0);

	if (sem_unlinked == SEM_FAILED)
	{
		UNRESOLVED(errno, "Failed to create the named semaphore");
	}

	ret = sem_unlink("/fork_14_1b");

	if (ret != 0)
	{
		UNRESOLVED(errno, "Failed to unlink the semaphore");
	}

	/* Create the child */
	child = fork();

	if (child == -1)
	{
		UNRESOLVED(errno, "Failed to fork");
	}

	/* child */
	if (child == 0)
	{
		do
		{
			ret = sem_post(sem_linked);
		}
		while ((ret != 0) && (errno == EINTR));

		if (ret != 0)
		{
			UNRESOLVED(errno, "Failed to post semaphore A");
		}

		do
		{
			ret = sem_post(sem_unlinked);
		}
		while ((ret != 0) && (errno == EINTR));

		if (ret != 0)
		{
			UNRESOLVED(errno, "Failed to post semaphore B");
		}

		/* We're done */
		exit(PTS_PASS);
	}

	/* Parent joins the child */
	ctl = waitpid(child, &status, 0);

	if (ctl != child)
	{
		UNRESOLVED(errno, "Waitpid returned the wrong PID");
	}

	if (!WIFEXITED(status) || (WEXITSTATUS(status) != PTS_PASS))
	{
		FAILED("Child exited abnormally");
	}

	/* Check both semaphores have been posted */
	do
	{
		ret = sem_trywait(sem_linked);
	}
	while ((ret != 0) && (errno == EINTR));

	if (ret != 0)
	{
		if (errno == EAGAIN)
		{
			FAILED("Child did not inherit the semaphore A");
		}
		else
		{
			UNRESOLVED(errno, "sem_trywait failed");
		}
	}

	do
	{
		ret = sem_trywait(sem_unlinked);
	}
	while ((ret != 0) && (errno == EINTR));

	if (ret != 0)
	{
		if (errno == EAGAIN)
		{
			FAILED("Child did not inherit the semaphore B");
		}
		else
		{
			UNRESOLVED(errno, "sem_trywait failed");
		}
	}

	ret = sem_unlink("/fork_14_1a");

	if (ret != 0)
	{
		UNRESOLVED(errno, "Failed to unlink semaphore A");
	}

	ret = sem_close(sem_linked);

	if (ret != 0)
	{
		UNRESOLVED(errno, "Failed to close semaphore A");
	}

	ret = sem_close(sem_unlinked);

	if (ret != 0)
	{
		UNRESOLVED(errno, "Failed to close semaphore B");
	}

	/* Test passed */
#if VERBOSE > 0

	output("Test passed\n");

#endif

	PASSED;
}