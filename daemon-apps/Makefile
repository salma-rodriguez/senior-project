APPS = daemon server test
COMPILER = gcc
DEBUG = -03

define CC
	@echo " [CC] $@" && ${COMPILER}
endef

CFLAGS = -Wall -Wextra

all: ${APPS}
server: server.o
daemon: daemon.o
test: test.o

clean:
	rm -rf ${APPS} *.o *~ *.swp
