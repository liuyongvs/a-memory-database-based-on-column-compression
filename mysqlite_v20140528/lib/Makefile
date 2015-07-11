CC      := gcc
LD      := ld
CFLAGS  := -fPIC
LDFLAGS := -shared -Wall -g
SOURCE  := $(wildcard *.c)
OBJS    := $(patsubst %.c,%.o,$(SOURCE))
TARGET_LIB := libdbcommon.so
 
all:$(OBJS)
	    echo $(OBJS)
	        $(CC) $(LDFLAGS) -o $(TARGET_LIB) $(OBJS) 
		 
%.o:%.c
	    @echo Compiling $< ...
	        $(CC) -c $(CFLAGS)  $< -o $*.o
		 
.PHONY: clean
	 
clean:
	    rm *.so *.o -rf
