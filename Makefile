OBJS=main.o window.o screen.o config.o graphic.o \
	 notif.o queue.o timer.o fifo.o
include ../param.mk

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


