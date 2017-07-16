#!/bin/sh

dir_exe=../cron

# Enter ../cron
cd $dir_exe
if [ $? -ne 0 ]; then
    echo "cant change to $dir_exe"
    exit 1
fi

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

exit 0
