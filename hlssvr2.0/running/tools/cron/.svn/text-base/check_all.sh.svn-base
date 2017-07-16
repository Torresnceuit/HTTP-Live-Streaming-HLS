#! /bin/sh
PATH=/sbin:/usr/sbin:/usr/local/sbin:/opt/gnome/sbin:/usr/local/bin:/usr/bin:/usr/X11R6/bin:/bin:/usr/games:/opt/gnome/bin:/root/bin:/usr/local/bin:/usr/local/sbin:/usr/bin:/usr/sbin:/bin:/sbin:/usr/lib/mit/bin:/usr/lib/mit/sbin

cd /data/app/muxsvr/tools/op

#for process number check and alert send
processnum=`/bin/ps -ef | /bin/grep muxsvr_|/bin/grep -v grep |/usr/bin/wc -l`
if [ $processnum -lt 10 ]
then
	/bin/sleep 1
	processnum=`/bin/ps -ef | /bin/grep muxsvr_|/bin/grep -v grep |/usr/bin/wc -l`
fi


mcd_pro=`/bin/ps -ef | /bin/grep muxsvr_mcd|/bin/grep -v grep|/usr/bin/wc -l`
if [ $mcd_pro -lt 1 ]
then
	/bin/sleep 1
	mcd_pro=`/bin/ps -ef | /bin/grep muxsvr_mcd|/bin/grep -v grep|/usr/bin/wc -l`
fi


if [ $mcd_pro -lt 1 ]
then
	/usr/local/agenttools/agent/agentRepStr 16868 "muxsvr server mcd process is $processnum less than 1,please check"
	cd /data/app/muxsvr/tools/op
	./stop.sh
	sleep 2
	./ipc_clean.sh	
	./start.sh
elif [ $processnum -lt 11 ] 
then
	/usr/local/agenttools/agent/agentRepStr 16868 "muxsvr server processnum is $processnum less than 10,please check"
	cd /data/app/muxsvr/tools/cron
	./check_dcc.sh
	./check_dcc2.sh
	./check_submcd_log.sh
	./check_mcd.sh
	./check_ccd.sh
	./check_ccd2.sh
	./check_ccd3.sh
	./check_ccd4.sh
	./check_ccd5.sh
	./check_ccd6.sh
fi

