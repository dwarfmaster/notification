#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <xcb/xcb.h>
#include "window.h"
#include "screen.h"
#include "config.h"
#include "graphic.h"

int main(int argc, char *argv[])
{
    xcb_connection_t* c;
    srv_screen_t* scr;
    srv_screen_t* act;
    srv_window_t win;

    /* Loading the config. */
    if(!load_config()) {
        printf("Couldn't load config.\n");
        return 1;
    }
    else
        dump_to_stdout();

    /* Opening the connection. */
    c = xcb_connect(NULL, NULL);

    /* Getting the screens. */
    scr = load_screens(c);
    act = scr;
    while(act) {
        printf("#%p : %ux%u (%u;%u)\n", act->xcbscr, act->x, act->y, act->w, act->h);
        act = act->next;
    }

    /* Opening the GCs. */
    if(!load_gcontexts(c, scr)) {
        printf("Couldn't load the graphics contexts.\n");
        return 1;
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

    free_gcontexts();
    free_config();
    free_screens(scr);
    xcb_disconnect(c);
    return 0;
}

