
#include "fifo.h"
#include "config.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>

static int _fifo = -1;
static const char* _fifo_path = "/tmp/notifier.fifo";

int init_fifo()
{
    struct stat buffer;

    if(_fifo >= 0)
        return 1;

    if(has_entry("global.fifo"))
        _fifo_path = get_string("global.fifo");
    if(stat(_fifo_path, &buffer) < 0) {
        if(mkfifo(_fifo_path, 0777) != 0)
            return 0;
    }
    else if(!S_ISFIFO(buffer.st_mode))
        return 0;

    _fifo = open(_fifo_path, O_RDWR | O_NONBLOCK);
    if(_fifo < 0)
        return 0;
    return 1;
}

void close_fifo()
{
    close(_fifo);
    remove(_fifo_path);
}

int get_fifo_id()
{
    return _fifo;
}

srv_order_t get_order_fifo(int size, char* buffer)
{
    ssize_t rd;
    char buf[256];
    char* str;

    rd = read(_fifo, buf, 256);
    buf[rd] = '\0';

    str = strtok(buf, " \n");
    if(!str)
        return NONE;

    if(strcmp(str, "close") == 0)
        return CLOSE;
    else if(strcmp(str, "close_all") == 0)
        return CLOSE_ALL;
    else if(strcmp(str, "end") == 0 || strcmp(str, "kill") == 0)
        return END;
    else if(strcmp(str, "notif") != 0)
        return NONE;

    str = strtok(NULL, "\n");
    if(!str)
        return NONE;
    if(buffer)
        memcpy(buffer, str, size);
    return NOTIF;
}

