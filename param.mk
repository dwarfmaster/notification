CFLAGS=-Wall -Wextra `pkg-config --cflags xcb`
LDFLAGS=`pkg-config --libs xcb`
SERVER=server.prog
CLIENT=client.prog
CC=gcc

