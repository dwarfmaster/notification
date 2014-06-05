
#ifndef DEF_SCREEN
#define DEF_SCREEN

#include <xcb/xcb.h>

typedef struct _srv_screen_t {
    xcb_screen_t* xcbscr;
    uint32_t x,y;
    uint32_t w,h;
    struct _srv_screen_t* next;
} srv_screen_t;

srv_screen_t* load_screens(xcb_connection_t* c);
void free_screens(srv_screen_t* scr);
srv_screen_t* focused(xcb_connection_t* c, srv_screen_t* scr);

#endif

