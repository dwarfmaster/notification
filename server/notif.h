
#ifndef DEF_NOTIF
#define DEF_NOTIF

#include <xcb/xcb.h>
#include "window.h"
#include "screen.h"
#include "graphic.h"

typedef struct _srv_notif_t {
    char** lines;
    srv_window_t win;
    srv_gcontext_t gc;
} srv_notif_t;

srv_notif_t* create_notif(xcb_connection_t* c, srv_screen_t* scr, uint32_t y, const char* name, const char* text);
void draw_notif(xcb_connection_t* c, srv_notif_t* notif);
void free_notif(xcb_connection_t* c, srv_notif_t* notif);

#endif

