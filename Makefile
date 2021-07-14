.SUFFIXES:.c .o
CC= gcc
#CC= arm-linux-gnueabihf-gcc

INC=-I /include/
CFLAGS= -g -Woverflow $(INC)

#LIBS= -lpthread -L/nfs/usr/lib/socketcan
LIBS= -lpthread

OBJS=./main.o 
SRCS=./main.c

TARGET=sw.out

all:	$(TARGET)
$(TARGET):$(OBJS)
	$(CC) -o $@ $(OBJS) $(LIBS)

dep:
	gccmakedep $(INC) $(SRCS)

clean:
	rm -rf $(OBJS) $(TARGET)

new:
	$(MAKE) clean
	$(MAKE)
# DO NOT DELETE
