#!/bin/bash

# This script runs the default ltp tests, capturing the output,
# then processes that output to generate an xml report file

XMLFILE="ltp-test.xml"

BASEDIR="/src/ltp-tools"

TESTCASES=$*
echo "TESTCASES = $TESTCASES"
if [ -s $TESTCASES ]; then
    $TCARG="-f $TESTCASES";
else
    $TCARG=""
fi

echo "TCARG = $TCARG"

SCRIPTNAME=runltp
LTP_LOC="$BASEDIR/ltp-full-20110915/ubuntu"

TEMPFILE="./$(basename $0).$$.tmp"
TMPDIR="$PWD/tmpdir.$$"
RESULTS_FILE="$PWD/result-log.$$"
FAILED_FILE="$PWD/result-output.$$"

SCRIPTARGS="$TCARG -l $RESULTS_FILE -C $FAILED_FILE -d $TMPDIR"
RSCRIPT="$LTP_LOC/$SCRIPTNAME "

echo "Temp file is $TEMPFILE"
echo "Tmpdir is $TMPDIR"
echo Starting Test

# make a local results directory
#runltp -c 2 -i 2 -m 2,4,10240,1 -D 2,10,10240,1 -p -q  -l /tmp/result-log.25955 -o /tmp/result-output.25955 -C /tmp/result-failed.25955 -d /src/ltp-tools/ltp-full-20110915/ubuntu
$RSCRIPT $SCRIPTARGS | tee $TEMPFILE

echo Processing results using $BASEDIR/ltp2JUnit
rm $XMLFILE
$BASEDIR/ltp2JUnit $TEMPFILE > $XMLFILE

echo removing tmp stuff
#rm -rf $RESULTS_FILE $FAILED_FILE $OUTPUT_FILE $TMPDIR $TEMPFILE
#rm -rf $RESULTS_FILE $FAILED_FILE $TMPDIR $TEMPFILE
echo Done