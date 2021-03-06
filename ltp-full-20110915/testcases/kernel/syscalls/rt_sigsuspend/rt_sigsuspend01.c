/********************************************************************************/
/* Copyright (c) Crackerjack Project., 2007				   	*/
/*									    	*/
/* This program is free software;  you can redistribute it and/or modify      	*/
/* it under the terms of the GNU General Public License as published by       	*/
/* the Free Software Foundation; either version 2 of the License, or	  	*/
/* (at your option) any later version.						*/
/*									    	*/
/* This program is distributed in the hope that it will be useful,	    	*/
/* but WITHOUT ANY WARRANTY;  without even the implied warranty of	    	*/
/* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See		  	*/
/* the GNU General Public License for more details.			   	*/
/*									    	*/
/* You should have received a copy of the GNU General Public License	  	*/
/* along with this program;  if not, write to the Free Software	       	*/
/* Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA    	*/
/*									    	*/
/******************************************************************************	*/
/******************************************************************************	*/
/*									    	*/
/* File:	rt_sigsuspend01.c					   	*/
/*									    	*/
/* Description: This tests the rt_sigsuspend() syscall.		      	*/
/*									       	*/
/* Usage:  <for command-line>						 	*/
/* rt_sigsuspend01 [-c n] [-e][-i n] [-I x] [-p x] [-t]		     	*/
/*      where,  -c n : Run n copies concurrently.			     	*/
/*	      -e   : Turn on errno logging.				 	*/
/*	      -i n : Execute test n times.				  	*/
/*	      -I x : Execute test for x seconds.			    	*/
/*	      -P x : Pause for x seconds between iterations.			*/
/*	      -t   : Turn on syscall timing.					*/
/*									    	*/
/* Total Tests: 2							     	*/
/*									    	*/
/* Test Name:   rt_sigsuspend01					     	*/
/* History:     Porting from Crackerjack to LTP is done by		    	*/
/*	      Manas Kumar Nayak maknayak@in.ibm.com>				*/
/********************************************************************************/

#include <stdio.h>
#include <signal.h>
#include <errno.h>

/* Harness Specific Include Files. */
#include "test.h"
#include "usctest.h"
#include "linux_syscall_numbers.h"
#define LTP_RT_SIG_TEST
#include "ltp_signal.h"

/* Extern Global Variables */

/* Global Variables */
char *TCID = "rt_sigsuspend01"; /* Test program identifier.		 */
int  testno;
int  TST_TOTAL = 1;	      /* total number of tests in this file.	 */

/* Extern Global Functions */
/******************************************************************************/
/*									    */
/* Function:    cleanup						       */
/*									    */
/* Description: Performs all one time clean up for this test on successful    */
/*	      completion,  premature exit or  failure. Closes all temporary */
/*	      files, removes all temporary directories exits the test with  */
/*	      appropriate return code by calling tst_exit() function.       */
/*									    */
/* Input:       None.							 */
/*									    */
/* Output:      None.							 */
/*									    */
/* Return:      On failure - Exits calling tst_exit(). Non '0' return code.   */
/*	      On success - Exits calling tst_exit(). With '0' return code.  */
/*									    */
/******************************************************************************/
void cleanup() {

	TEST_CLEANUP;
	tst_rmdir();

}

/* Local  Functions */
/******************************************************************************/
/*									    */
/* Function:    setup							 */
/*									    */
/* Description: Performs all one time setup for this test. This function is   */
/*	      typically used to capture signals, create temporary dirs      */
/*	      and temporary files that may be used in the course of this    */
/*	      test.							 */
/*									    */
/* Input:       None.							 */
/*									    */
/* Output:      None.							 */
/*									    */
/* Return:      On failure - Exits by calling cleanup().		      */
/*	      On success - returns 0.				       */
/*									    */
/******************************************************************************/
void setup() {
	/* Capture signals if any */
	/* Create temporary directories */
	TEST_PAUSE;
	tst_tmpdir();
}

void sig_handler(int sig)
{
}

int main(int ac, char **av) {

	sigset_t set, set1, set2;
	int lc;			/* loop counter */
	char *msg;		/* message returned from parse_opts */

	/* parse standard options */
	if ((msg = parse_opts(ac, av, NULL, NULL)) != NULL) {
	     tst_brkm(TBROK, NULL, "OPTION PARSING ERROR - %s", msg);
	}

	setup();

	for (lc = 0; TEST_LOOPING(lc); ++lc) {

		Tst_count = 0;

		if (sigemptyset(&set) < 0) {
			tst_brkm(TFAIL|TERRNO, cleanup, "sigemptyset failed");
		}
#ifdef __x86_64__
		struct kernel_sigaction act, oact;
		sig_initial(SIGALRM);
		memset(&act,0,sizeof(act));
		memset(&oact,0,sizeof(oact));
		act.sa_flags |= SA_RESTORER;
		act.sa_restorer = restore_rt;
		act.k_sa_handler = sig_handler;
#else
		struct sigaction act, oact;
		memset(&act,0,sizeof(act));
		memset(&oact,0,sizeof(oact));
		act.sa_handler = sig_handler;
#endif

		TEST(syscall(__NR_rt_sigaction, SIGALRM, &act, &oact,
				SIGSETSIZE));
		if (TEST_RETURN == -1) {
			tst_brkm(TFAIL|TTERRNO, cleanup, "rt_sigaction failed");
		}
		TEST(syscall(__NR_rt_sigprocmask, SIG_UNBLOCK, 0,
				&set1, SIGSETSIZE));
		if (TEST_RETURN == -1) {
			tst_brkm(TFAIL|TTERRNO,	cleanup,
				"rt_sigprocmask failed");
		}

		TEST(alarm(5));
		int result;
		TEST(result = syscall(__NR_rt_sigsuspend, &set, SIGSETSIZE));
		TEST(alarm(0));
		if (result == -1 && TEST_ERRNO != EINTR) {
			TEST(syscall(__NR_rt_sigprocmask, SIG_UNBLOCK, 0, &set2,
					SIGSETSIZE));
			if (TEST_RETURN == -1) {
				tst_brkm(TFAIL|TTERRNO, cleanup,
					"rt_sigprocmask failed");
			} else if (set1.__val[0] != set2.__val[0]) {
				tst_brkm(TFAIL|TTERRNO, cleanup,
					"rt_sigsuspend failed to "
					"preserve signal mask");
			} else {
				tst_resm(TPASS,	"rt_sigsuspend PASSED");
			}
		} else {
			tst_resm(TFAIL|TTERRNO,	"rt_sigsuspend failed");
		}

	}

	cleanup();

	tst_exit();
}
