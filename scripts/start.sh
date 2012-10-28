#! /bin/bash

# source & cache devices
src='/dev/sdc1'
cac='/dev/sdd'

# dm-cache directory
tk="dm-cache.ko"

# parm 4: source device
# parm 5: cache device
# parm 6: load previous cache configuration; otherwise, start from scratch
# parm 7 & 8: block size used by the cache and its capacity
# parm 9: cache associativity; doesn't matter what we put here
# parm 10: enable write back

# name for virtual machine
vnm='node1'

insmod $tk
echo 0 16771797 cache $src $cac 0 8 1048576 1 0 | dmsetup create foodev
mount /dev/mapper/foodev /media/dmmount
