dev='foodev'

umount /dev/mapper/$dev > /dev/null 2>&1
dmsetup remove /dev/mapper/$dev
rmmod dm_cache > /dev/null 2>&1
