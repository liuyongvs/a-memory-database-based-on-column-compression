#CC = gcc
CC=LD_LIBRARY_PATH=/usr/src/work/TeamWork/mysqlite/lib gcc
SOURCES := $(wildcard *.c)
OBJS:=$(patsubst %.c,%.o,$(SOURCES))
OTHERS:= "*.out *.swp *.swp *.gch"
LFLAGS = -Wall  -g  -L./lib -ldbcommon -pthread
CFLAGS = -Wall  -g  
CURENT_DIR = $(shell pwd)
SPACE:= #
TMP=$(subst $(SPACE),,$(CURENT_DIR))
LIB_DIR = $(TMP)/lib

all:=mysqlite

$(all):$(OBJS)
	cd $(LIB_DIR);make;cd ..
	$(CC) -o $(all) $(OBJS) $(LFLAGS)
%.o:%.c
	$(CC) -c $< -o $*.o $(CFLAGS) 

.PHONY:clean 
clean:
	rm -f $(OBJS) $(all) $(OTHERS) *.out *.time *.mem ;clear 
