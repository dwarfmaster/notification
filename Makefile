OBJS=main.o window.o screen.o config.o graphic.o \
	 notif.o queue.o timer.o fifo.o
LIBS=xcb xcb-ewmh xcb-icccm xcb-xinerama
CFLAGS=-Wall -Wextra `pkg-config --cflags $(LIBS)` -g -O0
LDFLAGS=`pkg-config --libs $(LIBS)`
SERVER=xcbnotif
CC=gcc

all : $(SERVER)

$(SERVER) : $(OBJS)
	$(CC) $(CFLAGS)    -o $@ $^ $(LDFLAGS)

%.o : %.c
	$(CC) $(CFLAGS) -c -o $@ $<

clean :
	@touch $(OBJS) $(SERVER)
	rm $(OBJS) $(SERVER)

rec : clean all

.PHONY: all clean rec


