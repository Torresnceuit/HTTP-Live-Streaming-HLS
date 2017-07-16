#!/bin/sh

echo "Begin Reload Submcd Cfg..."
cd ../../
PROCESS=`pwd | xargs -i basename {}`"_submcd_log"

COUNT=`ps -ef|grep "./${PROCESS} ../etc/${PROCESS}.conf"|grep -v grep|wc -l`
if [ $COUNT -lt 1 ]; then
    echo "process $PROCESS not exist!"
    exit 1    
fi

MAIN_PIDS=`ps -ef | grep "./${PROCESS} ../etc/${PROCESS}.conf" | grep -v grep | grep -v awk | awk '{print $2}'`

for MAIN_PID in $MAIN_PIDS;do
	echo "kill -s USR1 ${MAIN_PID}"
    kill -s USR1 $MAIN_PID
done

if [ $? != "0" ]; then
    echo "ReloadCfg Failed."
    exit 1
else
    echo "ReloadCfg OK."
    exit 0
fi
