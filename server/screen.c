
#include "screen.h"
#include <xcb/xinerama.h>
#include <xcb/xinput.h>
#include <stdlib.h>

srv_screen_t* load_screens(xcb_connection_t* c)
{
    srv_screen_t* scr;
    srv_screen_t* act;
    srv_screen_t* first = NULL;
    xcb_screen_t* xcbscr = xcb_setup_roots_iterator(xcb_get_setup(c)).data;
    xcb_xinerama_screen_info_iterator_t it;
    xcb_xinerama_query_screens_reply_t* reply;
    xcb_xinerama_query_screens_cookie_t cookie;

    cookie = xcb_xinerama_query_screens(c);
    reply  = xcb_xinerama_query_screens_reply(c, cookie, NULL);
    it     = xcb_xinerama_query_screens_screen_info_iterator(reply);
    free(reply);

    for(; it.rem; xcb_xinerama_screen_info_next(&it)) {
        act = malloc(sizeof(srv_screen_t));
        act->x = it.data->x_org;
        act->y = it.data->y_org;
        act->w = it.data->width;
        act->h = it.data->height;
        act->xcbscr = xcbscr;
        act->next = NULL;

        if(!first) {
            first = act;
            scr = first;
        } else {
            scr->next = act;
            scr = act;
        }
    }

    return first;
}

void free_screens(srv_screen_t* scr)
{
    srv_screen_t* next;
    while(scr) {
        next = scr->next;
        free(scr);
        scr = next;
    }
}

srv_screen_t* focused(xcb_connection_t* c, srv_screen_t* scr)
{
    /* TODO find a way to detect focused screen. */
    return scr;
}

