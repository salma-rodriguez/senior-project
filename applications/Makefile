APPS = daemon server
COMPILER = gcc
DEBUG = -03

define CC
	@echo " [CC] $@" && ${COMPILER}
endef

CFLAGS = -Wall -Wextra

all: ${APPS}
server: server.o
daemon: daemon.o

clean:
	rm -rf ${APPS} *.o *~ *.swp
