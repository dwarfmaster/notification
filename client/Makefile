OBJS=main.o
include ../param.mk

all : $(CLIENT)

$(CLIENT) : $(OBJS)
	$(CC) $(CFLAGS)    -o $@ $^ $(LDFLAGS)

%.o : %.c
	$(CC) $(CFLAGS) -c -o $@ $<

clean :
	@touch $(OBJS) $(SERVER)
	rm $(OBJS) $(SERVER)

rec : clean all

.PHONY: all clean rec


