#!/bin/bash

# This script runs the default ltp tests, capturing the output,
# then processes that output to generate an xml report file

TESTCASES=quickhit

LTP_LOC=$PWD/ltp-full-20110915/ubuntu
RUNSCRIPT=runltp
SCRIPTARSG="-f $TESTCASES"
TFILE="./$(basename $0).$$.tmp"

$LTPLOC/$RUNSCRIPT $SCRIPTARGS > $TFILE

./ltp2JUnit $TFILE > ltp-test.xml

