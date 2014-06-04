
all : server client

client :
	make -C client

server :
	make -C server

clean :
	make clean -C client
	make clean -C server

rec : clean all

.PHONY: all client server clean rec


