
#ifndef DEF_QUEUE
#define DEF_QUEUE

#include <xcb/xcb.h>
#include "notif.h"
#include "screen.h"
#include "timer.h"

struct _srv_queue_t;

typedef struct _srv_queue_item_t {
    srv_notif_t* notif;
    int on_screen;
    struct _srv_queue_item_t* next;
    struct _srv_queue_item_t* prev;
    struct _srv_queue_t* parent;
    srv_timer_t* timer;
} srv_queue_item_t;

typedef struct _srv_queue_t {
    xcb_connection_t* c;
    srv_screen_t* scr;
    uint32_t vert_dec;
    uint32_t hori_dec;
    srv_queue_item_t* first;
} srv_queue_t;

srv_queue_t* init_queue(xcb_connection_t* c, srv_screen_t* scr);
void close_queue(srv_queue_t* q);
srv_queue_item_t* add_notif(srv_queue_t* q, uint32_t time, const char* name, const char* text);
srv_queue_item_t* add_notif_str(srv_queue_t* q, const char* str);
void rm_notif_cond(srv_queue_t* q);
uint32_t nearest_end(srv_queue_t* q);
void rm_notif(srv_queue_item_t* item);
void draw_queue(srv_queue_t* q);
void rm_top(srv_queue_t* q);
void clear_queue(srv_queue_t* q);

#endif

