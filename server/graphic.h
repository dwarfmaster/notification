
#ifndef DEF_GRAPHIC
#define DEF_GRAPHIC

#include <xcb/xcb.h>
#include "screen.h"
#include "window.h"

/** Must be called after load_config. */
int load_gcontexts(xcb_connection_t* c, srv_screen_t* scr);
void free_gcontexts();
int has_gcontext(const char* name);
int get_gcontext(const char* name, xcb_gcontext_t* gc);

void draw_notif(xcb_connection_t *c, srv_window_t* win, xcb_gcontext_t gc,
        uint32_t hline,    /* The height of a line. */
        const char** lines /* An array of lines to be drawn. */
        );

#endif

