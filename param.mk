LIBS=xcb xcb-ewmh xcb-icccm xcb-xinerama
CFLAGS=-Wall -Wextra `pkg-config --cflags $(LIBS)` -g
LDFLAGS=`pkg-config --libs $(LIBS)`
SERVER=server.prog
CLIENT=client.prog
CC=gcc

