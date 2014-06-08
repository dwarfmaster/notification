#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <xcb/xcb.h>
#include "window.h"
#include "screen.h"
#include "config.h"
#include "graphic.h"
#include "notif.h"

int main(int argc, char *argv[])
{
    xcb_connection_t* c;
    srv_gcontext_t gc;
    srv_screen_t* scr;
    srv_screen_t* act;
    srv_notif_t* notif;
    xcb_generic_event_t* e;
    const char* text = "Hello world. This is a big text to test my new notification system, and automatic line cut. See you !";
    time_t prev, new;

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
    get_gcontext("normal", &gc);

    /* Getting EWMH */
    request_ewmh(c);
    xcb_flush(c);
    if(!has_ewmh())
        return 1;

    /* Opening the window. */
    notif = create_notif(c, scr, 50, "normal", text);
    xcb_flush(c);

    prev = time(NULL);
    while(1) {
        while((e = xcb_poll_for_event(c))) {
            switch(e->response_type & ~0x80) {
                case XCB_EXPOSE:
                    draw_notif(c, notif);
                    xcb_flush(c);
                    break;
                default:
                    break;
            }
            free(e);
        }

        new = time(NULL);
        if(difftime(new, prev) > 5)
            break;
    }

    free_notif(c, notif);
    free_gcontexts();
    free_config();
    free_screens(scr);
    close_ewmh();
    xcb_disconnect(c);
    return 0;
}

