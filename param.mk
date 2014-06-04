CFLAGS=-Wall -Wextra `pkg-config --cflags xcb xcb-ewmh`
LDFLAGS=`pkg-config --libs xcb xcb-ewmh`
SERVER=server.prog
CLIENT=client.prog
CC=gcc

