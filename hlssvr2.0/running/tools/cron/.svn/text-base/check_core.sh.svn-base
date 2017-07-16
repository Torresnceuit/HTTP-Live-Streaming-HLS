#!/bin/sh

cd ../../
process=`pwd | xargs -i basename {}`
core_dir=${process}"_core"
cd bin/

# check how many core files exist in bin/
count=`ls -t core* 2> /dev/null | wc -l`
if [ $count -eq 0 ]; then
	exit 0
fi
echo "["`date +'%Y-%m-%d %T'`"] core files number:${count}"

# leave at most 3 core files
if [ $count -gt 3 ]; then
	count=3
fi

# check how many core files exist in /data/***_core
if [ ! -d /data/${core_dir}/ ]; then
	mkdir /data/${core_dir}/
fi

core_count=`ls -t /data/${core_dir}/core* 2> /dev/null | wc -l`
del_count=`expr ${core_count} + ${count} - 3`
if [ $del_count -gt 0 ]; then
	# delete old core files
	ls -t /data/${core_dir}/core* 2> /dev/null | tail -n ${del_count} | xargs -i rm -f {}
fi

# save new core files
for save_core in `ls -t core* | head -n ${count}`; do
	mv $save_core /data/${core_dir}/
done

# delete all core files in bin/
ls -t core* 2> /dev/null | xargs -i rm -f {}

exit 0

