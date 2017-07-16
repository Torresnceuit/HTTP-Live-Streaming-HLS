#!/bin/sh

echo "Stopping service..."

cd ../../
process=`pwd | xargs -i basename {}`

CCD_PIDS=`ps -ef | grep "./${process}_ccd ../etc/${process}_ccd.conf" | grep -v grep | awk '{print $2}'`
CCD2_PIDS=`ps -ef | grep "./${process}_ccd2 ../etc/${process}_ccd2.conf" | grep -v grep | awk '{print $2}'`
CCD3_PIDS=`ps -ef | grep "./${process}_ccd3 ../etc/${process}_ccd3.conf" | grep -v grep | awk '{print $2}'`
CCD4_PIDS=`ps -ef | grep "./${process}_ccd4 ../etc/${process}_ccd4.conf" | grep -v grep | awk '{print $2}'`
CCD5_PIDS=`ps -ef | grep "./${process}_ccd5 ../etc/${process}_ccd5.conf" | grep -v grep | awk '{print $2}'`
CCD6_PIDS=`ps -ef | grep "./${process}_ccd6 ../etc/${process}_ccd6.conf" | grep -v grep | awk '{print $2}'`
MCD_PIDS=`ps -ef | grep "./${process}_mcd ../etc/${process}_mcd.conf" | grep -v grep | awk '{print $2}'`
SUBMCD_LOG_PIDS=`ps -ef | grep "./${process}_submcd_log ../etc/${process}_submcd_log.conf" | grep -v grep | awk '{print $2}'`
DCC_PIDS=`ps -ef | grep "./${process}_dcc ../etc/${process}_dcc.conf" | grep -v grep | awk '{print $2}'`
DCC2_PIDS=`ps -ef | grep "./${process}_dcc2 ../etc/${process}_dcc2.conf" | grep -v grep | awk '{print $2}'`

for MAIN_PID in $CCD_PIDS $CCD2_PIDS $CCD3_PIDS $CCD4_PIDS $CCD5_PIDS $CCD6_PIDS $MCD_PIDS $SUBMCD_LOG_PIDS $DCC_PIDS $DCC2_PIDS
do
	kill -s USR2 $MAIN_PID
done

if [ $? != "0" ]
then
echo "Can't stop."
exit 1
else
echo "Service stoped."
exit 0
fi
