==============
senior-project
==============

>>>>>>>

==============
What is it?
==============

kernel functions and userspace programs for facilitating IPC
between dm-cache and a server script, to dynamically spin up/down
a storage server disk

==============
How to use it?
==============

1. make sure that you are running version 2.6 of the Linux kernel
   (check uname -r to see currently running version), then run 'make';
2. verify that the script files are configured properly and start dm-cache
   from this directory: './scripts/start.sh'
3. go to 'applications' and run 'make'
4. run server, as follows: './server <portno>', where portno is the port number
5. run daemon, as follows: './daemon <hostname> <portno>', where hostname is the IP
   address or host name of the server
6. the device should be mounted under '/dev/mapper/foodev'
7. run: 'gcc -o test test.c' to compile the test file, or use other workload
tested in dm-cache 2.6.29 (stable version)
