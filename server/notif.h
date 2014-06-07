
#ifndef DEF_NOTIF
#define DEF_NOTIF

#include <xcb/xcb.h>
#include "window.h"

typedef struct _srv_notif_t {
    const char** lines;
    srv_window_t* win;
} srv_notif_t;

srv_notif_t* createNotif(uint32_t y, uint32_t w, const char* text);
void drawNotif(srv_notif_t* notif);
void freeNotif(srv_notif_t* notif);

#endif

