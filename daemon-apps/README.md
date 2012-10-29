==============
What is it?
==============

kernel functions and userspace programs for facilitating IPC
between dm-cache and a server script, to dynamically spin up/down
a storage server disk

==============
How to use it?
==============

1. hit 'make'
2. run server, as follows: './server \<portno\>', where portno is the port number
3. run daemon, as follows: './daemon \<hostname\> \<portno\>', where hostname is the IP
   address or host name of the server
5. run: 'gcc -o test test.c' to compile the test file, or use other workload
