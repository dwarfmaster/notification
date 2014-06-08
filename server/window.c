
#include "window.h"
#include <string.h>
#include <xcb/xcb_ewmh.h>
#include <xcb/xcb_icccm.h>

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
    xcb_icccm_wm_hints_t hints;
    xcb_size_hints_t size;
    uint32_t mask;
    uint32_t values[3];

    if(!has_ewmh()) {
        win.opened = 0;
        return win;
    }
    win.opened = 1;
    win.width = w;
    win.height = h;

    /* Creating the window. */
    win.xcbwin = xcb_generate_id(c);
    mask = XCB_CW_BACK_PIXEL
        | XCB_CW_OVERRIDE_REDIRECT
        | XCB_CW_EVENT_MASK;
    values[0] = scr->black_pixel;
    values[1] = 1;
    values[2] = XCB_EVENT_MASK_EXPOSURE;
    xcb_create_window(c,
            XCB_COPY_FROM_PARENT,
            win.xcbwin,
            scr->root,
            x, y, w, h, 1,
            XCB_WINDOW_CLASS_INPUT_OUTPUT,
            scr->root_visual,
            mask, values
            );

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
    xcb_change_property(c, XCB_PROP_MODE_APPEND, win.xcbwin, _ewmh._NET_WM_WINDOW_TYPE,
            XCB_ATOM_ATOM, 32, 1, &(_ewmh._NET_WM_WINDOW_TYPE_TOOLTIP));
    xcb_change_property(c, XCB_PROP_MODE_APPEND, win.xcbwin, _ewmh._NET_WM_WINDOW_TYPE,
            XCB_ATOM_ATOM, 32, 1, &(_ewmh._NET_WM_WINDOW_TYPE_DIALOG));
    xcb_change_property(c, XCB_PROP_MODE_APPEND, win.xcbwin, _ewmh._NET_WM_WINDOW_TYPE,
            XCB_ATOM_ATOM, 32, 1, &(_ewmh._NET_WM_WINDOW_TYPE_NORMAL));

    xcb_change_property(c, XCB_PROP_MODE_REPLACE, win.xcbwin, _ewmh._NET_WM_STATE,
            XCB_ATOM_ATOM, 32, 1, &(_ewmh._NET_WM_STATE_ABOVE));
    xcb_change_property(c, XCB_PROP_MODE_APPEND, win.xcbwin, _ewmh._NET_WM_STATE,
            XCB_ATOM_ATOM, 32, 1, &(_ewmh._NET_WM_STATE_STICKY));
    xcb_change_property(c, XCB_PROP_MODE_APPEND, win.xcbwin, _ewmh._NET_WM_STATE,
            XCB_ATOM_ATOM, 32, 1, &(_ewmh._NET_WM_STATE_SKIP_TASKBAR));

    xcb_change_property(c, XCB_PROP_MODE_REPLACE, win.xcbwin, _ewmh.WM_PROTOCOLS,
            XCB_ATOM_ATOM, 32, 1, &(_ewmh._NET_WM_PING));
    xcb_change_property(c, XCB_PROP_MODE_APPEND, win.xcbwin, _ewmh.WM_PROTOCOLS,
            XCB_ATOM_ATOM, 32, 1, &(_ewmh._NET_WM_SYNC_REQUEST));

    /* Setting ICCCM hints. */
    hints.flags = XCB_ICCCM_WM_HINT_INPUT | XCB_ICCCM_WM_HINT_STATE;
    hints.input = 0;
    hints.initial_state = XCB_ICCCM_WM_STATE_NORMAL;
    xcb_icccm_set_wm_hints(c, win.xcbwin, &hints);

    size.flags = XCB_ICCCM_SIZE_HINT_P_POSITION
        | XCB_ICCCM_SIZE_HINT_US_POSITION
        | XCB_ICCCM_SIZE_HINT_US_SIZE
        | XCB_ICCCM_SIZE_HINT_P_SIZE
        | XCB_ICCCM_SIZE_HINT_P_MIN_SIZE
        | XCB_ICCCM_SIZE_HINT_P_MAX_SIZE
        | XCB_ICCCM_SIZE_HINT_P_WIN_GRAVITY;
    size.x = x;
    size.y = y;
    size.width  = size.max_width  = size.min_width  = w;
    size.height = size.max_height = size.min_height = h;
    size.win_gravity = XCB_GRAVITY_STATIC;
    xcb_icccm_set_wm_size_hints(c, win.xcbwin, XCB_ATOM_WM_NORMAL_HINTS, &size);

    xcb_map_window(c, win.xcbwin);
    return win;
}

int opened(srv_window_t win)
{
    return win.opened;
}

void close_window(xcb_connection_t* c, srv_window_t win)
{
    xcb_destroy_window(c, win.xcbwin);
}

