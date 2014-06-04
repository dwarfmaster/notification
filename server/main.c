#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <xcb/xcb.h>
#include "window.h"

int main(int argc, char *argv[])
{
    xcb_connection_t* c;
    xcb_screen_t* scr;
    srv_window_t win;

    /* Opening the connection. */
    c = xcb_connect(NULL, NULL);
    scr = xcb_setup_roots_iterator(xcb_get_setup(c)).data;

    /* Getting EWMH */
    request_ewmh(c);
    xcb_flush(c);
    if(!has_ewmh(c))
        return 1;

    /* Opening the window. */
    open_window(c, scr, 800, 450, 100, 100, "Test window");
    xcb_flush(c);
    if(opened(c, win) < 0)
        return 1;

    pause();

    xcb_disconnect(c);
    return 0;
}

