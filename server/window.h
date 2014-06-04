
#ifndef DEF_WINDOW
#define DEF_WINDOW

#include <xcb/xcb.h>

typedef struct _srv_window_t {
    xcb_window_t xcbwin;
    xcb_void_cookie_t cookie;
    int opened;
    int valid;
} srv_window_t;

/** Request the EWMH data. */
void request_ewmh(xcb_connection_t* c);
/** Check if has EWMH : needed to open windows. */
int has_ewmh();

/** Request the opening of a window. */
srv_window_t open_window(xcb_connection_t* c, xcb_screen_t* scr,
        uint32_t x, uint32_t y,
        uint32_t w, uint32_t h,
        const char* title
        );
/** Check if the window has been openend : return -1 on failure,
 * 0 if its still pending and 1 if the window has been opened. */
int opened(xcb_connection_t* c, srv_window_t win);

#endif

