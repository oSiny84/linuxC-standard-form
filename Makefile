.SUFFIXES:.c .o
CC= gcc
#CC= arm-linux-gnueabihf-gcc

INC=-I /include/
CFLAGS= -g -Woverflow $(INC)

#LIBS= -lpthread -L/nfs/usr/lib/socketcan
LIBS= -lpthread

OBJS=./main.o ./uart.o ./util.o ./tcp.o
SRCS=./main.c ./uart.c ./util.c ./tcp.c

TARGET=sw.out

all:	$(TARGET)
$(TARGET):$(OBJS)
	$(CC) -DLINUXPC -o $@ $(OBJS) $(LIBS) 

dep:
	gccmakedep $(INC) $(SRCS)

clean:
	rm -rf $(OBJS) $(TARGET)

new:
	$(MAKE) clean
	$(MAKE)
# DO NOT DELETE
