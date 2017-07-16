#!/bin/sh

echo "Begin Restart Mcd..."
cd ../../
PROCESS=`pwd | xargs -i basename {}`"_mcd"

COUNT=`ps -ef|grep "./${PROCESS} ../etc/${PROCESS}.conf"|grep -v grep|wc -l`
if [ $COUNT -lt 1 ]; then
    echo "process $PROCESS not exist!"
    exit 1    
fi

MAIN_PIDS=`ps -ef | grep "./${PROCESS} ../etc/${PROCESS}.conf" | grep -v grep | grep -v awk | awk '{print $2}'`

for MAIN_PID in $MAIN_PIDS;do
	echo "kill -s USR2 ${MAIN_PID}"
    kill -s USR2 $MAIN_PID
done

sleep 1
cd ./tools/cron/
./check_mcd.sh

if [ $? != "0" ]; then
    echo "Restart Mcd Failed."
    exit 1
else
    echo "Restart Mcd OK."
    exit 0
fi
