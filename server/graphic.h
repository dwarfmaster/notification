
#ifndef DEF_GRAPHIC
#define DEF_GRAPHIC

#include <xcb/xcb.h>
#include "screen.h"
#include "window.h"

typedef struct _srv_gcontext_t {
    xcb_gcontext_t fg;
    xcb_gcontext_t bg;
    xcb_gcontext_t bc;
    uint32_t width;
    uint32_t font_height;
} srv_gcontext_t;

/** Must be called after load_config. */
int load_gcontexts(xcb_connection_t* c, srv_screen_t* scr);
void free_gcontexts();
int has_gcontext(const char* name);
int get_gcontext(const char* name, srv_gcontext_t* gc);

void draw_notif(xcb_connection_t *c, srv_window_t* win, srv_gcontext_t gc,
        const char** lines /* An array of lines to be drawn. */
        );

#endif

