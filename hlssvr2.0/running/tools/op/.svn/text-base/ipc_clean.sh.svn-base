#! /bin/sh

#clean share memory

cd ../../etc/
#clean shm
SHM_IDS=`grep "shm_key" * -R|awk -F = '{print $2}'| sed 's/ //g' |sort -u`

for SHM_ID in $SHM_IDS; do
        echo "ipcrm -M $SHM_ID"
    ipcrm -M $SHM_ID
done

#clean sem

SEM_IDS=`grep "sem_key" * -R|awk -F = '{print $2}'| sed 's/ //g' | sort -u`

for SEM_ID in $SEM_IDS; do
        echo "ipcrm -S $SEM_ID"
    ipcrm -S $SEM_ID
done
