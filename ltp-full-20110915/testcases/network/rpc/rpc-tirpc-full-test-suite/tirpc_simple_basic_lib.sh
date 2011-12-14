#!/bin/bash

# This script is a part of RPC & TI-RPC Test Suite
# (c) 2007 BULL S.A.S.
# Please refer to RPC & TI-RPC Test Suite documentation.
# More details at http://nfsv4.bullopensource.org/doc/rpc_testsuite.php

# This scripts launch everything needed to test RPC & TI-RPC
# Never try to launch alone, run "rpc_ts_run.sh" instead
# Note : this script could be in more than one copy depending on what
#        tests series you want to run

# By C. LACABANNE - cyril.lacabanne@bull.net
# creation : 2007-04-19 revision : 2007-05-23

# **********************
# *** INITIALISATION ***
# **********************

# simple test suite identification
TESTSUITENAME="TIRPC simple domain"
TESTSUITEVERS="0.1"
TESTSUITEAUTH="Cyril LACABANNE"
TESTSUITEDATE="2007-05-15"
TESTSUITECOMM="First series of tests for TI-RPC Libs, for dev. purpose"

TESTSERVER_1_PATH="tirpc_svc_1"
TESTSERVER_1_BIN="tirpc_svc_1.bin"
TESTSERVER_1=$SERVERTSTPACKDIR/$TESTSERVER_1_PATH/$TESTSERVER_1_BIN

export TESTSERVER_1_PATH
export TESTSERVER_1_BIN
export TESTSERVER_1

# check if tests run locally or not
# if not, logs directory will be change to remote directory
if [ "$LOCALIP" != "$CLIENTIP" ]
then
	LOGDIR=/tmp/$LOGDIR
	if [ $VERBOSE -eq 1 ]
	then
		echo " - log dir changes to client log dir : "$LOGDIR # debug !
	fi
fi

# *****************
# *** PROCESSUS ***
# *****************

echo "*** Starting Test Suite : "$TESTSUITENAME" (v "$TESTSUITEVERS") ***"

#-- start TIRPC Server # 1 for that following tests series
if [ "$TESTWAY" = "onetomany" ]
then
	# run one server for one or more client
	$REMOTESHELL $SERVERUSER@$SERVERIP "$TESTSERVER_1 $PROGNUMBASE"&
else
	# launch as much server instances as client instances
	for ((a=0; a < TESTINSTANCE ; a++))
	do
		$REMOTESHELL $SERVERUSER@$SERVERIP "$TESTSERVER_1 `expr $PROGNUMBASE + $a`"&
	done
fi

#-- start another instance of TIRPC server for simple API call type test
$REMOTESHELL $SERVERUSER@$SERVERIP "$TESTSERVER_1 $PROGNUMNOSVC"&

# wait for server creation and initialization
sleep $SERVERTIMEOUT

./$SCRIPTSDIR/tirpc_simple_rpc_reg.basic.sh
./$SCRIPTSDIR/tirpc_simple_rpc_call.basic.sh
./$SCRIPTSDIR/tirpc_simple_rpc_broadcast.sh
./$SCRIPTSDIR/tirpc_simple_rpc_broadcast_exp.sh

#-- Cleanup
$REMOTESHELL $SERVERUSER@$SERVERIP "killall -9 "$TESTSERVER_1_BIN

# ***************
# *** RESULTS ***
# ***************
