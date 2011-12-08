#!/bin/bash

# This script runs the default ltp tests, capturing the output,
# then processes that output to generate an xml report file

TESTCASES=$*
SCRIPTNAME=runltp
LTP_LOC="$PWD/ltp-full-20110915/ubuntu"
SCRIPTARGS="-f $TESTCASES"
TFILE="./$(basename $0).$$.tmp"

RS="$LTP_LOC/$SCRIPTNAME "

echo Starting Test

$RS $SCRIPTARGS | tee $TFILE

echo Processing reults

./ltp2JUnit $TFILE > ltp-test.xml

echo Done