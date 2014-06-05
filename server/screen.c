
#include "screen.h"
#include <stdlib.h>

srv_screen_t* load_screens(xcb_connection_t* c)
{
    srv_screen_t* scr;
    srv_screen_t* act;
    srv_screen_t* first = NULL;
    xcb_screen_t* xcbscr;
    xcb_screen_iterator_t it;

    it = xcb_setup_roots_iterator(xcb_get_setup(c));
    for(; it.rem; xcb_screen_next(&it)) {
        xcbscr = it.data;
        act = malloc(sizeof(srv_screen_t));
        act->x = 0;
        act->y = 0;
        act->w = xcbscr->width_in_pixels;
        act->h = xcbscr->height_in_pixels;
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

