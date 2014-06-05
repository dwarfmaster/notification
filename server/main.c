#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <xcb/xcb.h>
#include "window.h"
#include "screen.h"

int main(int argc, char *argv[])
{
    xcb_connection_t* c;
    srv_screen_t* scr;
    srv_screen_t* act;
    srv_window_t win;

    /* Opening the connection. */
    c = xcb_connect(NULL, NULL);

    /* Getting the screens. */
    scr = load_screens(c);
    act = scr;
    while(act) {
        printf("#%p : %ux%u (%u;%u)\n", act->xcbscr, act->x, act->y, act->w, act->h);
        act = act->next;
    }

    /* Getting EWMH */
    request_ewmh(c);
    xcb_flush(c);
    if(!has_ewmh())
        return 1;

    /* Opening the window. */
    win = open_window(c, scr->xcbscr, 800, 450, 100, 100, "Test window");
    xcb_flush(c);
    if(!opened(win))
        return 1;

    pause();

    free_screens(scr);
    xcb_disconnect(c);
    return 0;
}

