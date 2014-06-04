
#include "window.h"
#include <string.h>
#include <xcb/xcb_ewmh.h>

/** Hold EWMH protocol information. */
static xcb_ewmh_connection_t _ewmh;
int _has_ewmh = 0;
static xcb_intern_atom_cookie_t* _ewmh_cookie = NULL;

void request_ewmh(xcb_connection_t* c)
{
    if(_has_ewmh)
        return;
    _ewmh_cookie = xcb_ewmh_init_atoms(c, &_ewmh);
}

int has_ewmh()
{
    if(_has_ewmh)
        return 1;
    else if(!_ewmh_cookie || !xcb_ewmh_init_atoms_replies(&_ewmh, _ewmh_cookie, NULL))
        return 0;

    _has_ewmh = 1;
    return 1;
}

srv_window_t open_window(xcb_connection_t* c, xcb_screen_t* scr,
        uint32_t x, uint32_t y,
        uint32_t w, uint32_t h,
        const char* title)
{
    srv_window_t win;
    uint32_t mask;
    uint32_t values[2];

    if(!has_ewmh()) {
        win.opened = 0;
        return win;
    }
    win.opened = 1;

    /* Creating the window. */
    win.xcbwin = xcb_generate_id(c);
    mask = XCB_CW_BACK_PIXEL | XCB_CW_EVENT_MASK;
    values[0] = scr->black_pixel;
    values[1] = XCB_EVENT_MASK_EXPOSURE;
    xcb_create_window(c,
            XCB_COPY_FROM_PARENT,
            win.xcbwin,
            scr->root,
            x, y, w, h, 1,
            XCB_WINDOW_CLASS_INPUT_OUTPUT,
            scr->root_visual,
            mask, values
            );
    xcb_map_window(c, win.xcbwin);

    /* Setting its name. */
    xcb_change_property(c, XCB_PROP_MODE_REPLACE, win.xcbwin, XCB_ATOM_WM_NAME,
            XCB_ATOM_STRING, 8, strlen(title), title);
    xcb_change_property(c, XCB_PROP_MODE_REPLACE, win.xcbwin, _ewmh._NET_WM_NAME,
            XCB_ATOM_STRING, 8, strlen(title), title);
    xcb_change_property(c, XCB_PROP_MODE_REPLACE, win.xcbwin, _ewmh._NET_WM_VISIBLE_NAME,
            XCB_ATOM_STRING, 8, strlen(title), title);

    /* Setting EWMH parameters. */
    values[0] = 0xFFFFFFFF;
    xcb_change_property(c, XCB_PROP_MODE_REPLACE, win.xcbwin, _ewmh._NET_WM_DESKTOP,
            XCB_ATOM_INTEGER, 32, 1, &values[0]);
    xcb_change_property(c, XCB_PROP_MODE_REPLACE, win.xcbwin, _ewmh._NET_WM_WINDOW_TYPE,
            XCB_ATOM_ATOM, 32, 1, &(_ewmh._NET_WM_WINDOW_TYPE_NOTIFICATION));
    xcb_change_property(c, XCB_PROP_MODE_REPLACE, win.xcbwin, _ewmh._NET_WM_STATE,
            XCB_ATOM_ATOM, 32, 1, &(_ewmh._NET_WM_STATE_STICKY));
    xcb_change_property(c, XCB_PROP_MODE_APPEND, win.xcbwin, _ewmh._NET_WM_STATE,
            XCB_ATOM_ATOM, 32, 1, &(_ewmh._NET_WM_STATE_ABOVE));

    return win;
}

int opened(srv_window_t win)
{
    return win.opened;
}

