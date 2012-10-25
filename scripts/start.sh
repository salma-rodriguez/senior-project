#! /bin/bash

# source & cache devices
src='/dev/sdb1'
cac='/dev/sdc'

# dm-cache directory
# dir="/root/senior-project/dm-cache/dm-cache.ko"
# dir='/root/dm-cache/stable/2.6.29/dm-cache.ko'
dir='/home/salma/Documents/dm-cache.ko'

# parm 4: source device
# parm 5: cache device
# parm 6: load previous cache configuration; otherwise, start from scratch
# parm 7 & 8: block size used by the cache and its capacity
# parm 9: cache associativity; doesn't matter what we put here
# parm 10: enable write back

# name for virtual machine
vnm='node1'

insmod $dir
echo 0 16771797 cache $src $cac 0 8 1048576 1 0 | dmsetup create foodev
# mount /dev/mapper/foodev /media/dmmount
