
#ifndef DEF_GRAPHIC
#define DEF_GRAPHIC

#include <xcb/xcb.h>
#include "screen.h"

/** Must be called after load_config. */
int load_gcontexts(xcb_connection_t* c, srv_screen_t* scr);
void free_gcontexts();
int has_gcontext(const char* name);
int get_gcontext(const char* name, xcb_gcontext_t* gc);

#endif

