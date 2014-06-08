
#include "queue.h"
#include <stdlib.h>

srv_queue_t* init_queue(xcb_connection_t* c, srv_screen_t* scr)
{
    srv_queue_t* q = malloc(sizeof(srv_queue_t));
    q->c        = c;
    q->scr      = scr;
    q->first    = NULL;
    q->vert_dec = scr->w - 15;
    q->hori_dec = 15;
    return q;
}

static void free_queued(srv_queue_item_t* item)
{
    if(item->next)
        item->next->prev = item->prev;
    if(item->prev)
        item->prev->next = item->next;
    if(item == item->parent->first)
        item->parent->first = item->next;

    free_notif(item->parent->c, item->notif);
    free(item);
}

void close_queue(srv_queue_t* q)
{
    srv_queue_item_t* it = q->first;
    srv_queue_item_t* tmp;
    while(it) {
        tmp = it->next;
        free_queued(it);
        it = tmp;
    }
    free(q);
}

static void queue_update(srv_queue_t* q)
{
    uint32_t x, y;
    int toshow = 1;
    srv_queue_item_t* it = q->first;

    y = 15;
    while(it) {
        if(!toshow) {
            it->on_screen = 0;
            show_window(q->c, it->notif->win, 0);
            it = it->next;
            continue;
        }

        x = q->vert_dec - it->notif->win.width;
        if(y + it->notif->win.height > q->scr->h) {
            it->on_screen = 0;
            show_window(q->c, it->notif->win, 0);
            toshow = 0;
            it = it->next;
            continue;
        }

        it->on_screen = 1;
        show_window(q->c, it->notif->win, 1);
        move_window(q->c, it->notif->win, x, y);
        y += it->notif->win.height;
        y += 15;
        it = it->next;
    }
}

srv_queue_item_t* add_notif(srv_queue_t* q, const char* name, const char* text)
{
    srv_queue_item_t* last = q->first;
    srv_queue_item_t* it = malloc(sizeof(srv_queue_item_t));

    if(last) {
        while(last->next) last = last->next;
        it->prev   = last;
        last->next = it;
    }
    else {
        it->prev = NULL;
        q->first = it;
    }

    it->parent = q;
    it->next   = NULL;

    it->notif = create_notif(q->c, q->scr, 0, name, text);
    queue_update(q);
    return it;
}

void rm_notif(srv_queue_item_t* item)
{
    srv_queue_t* q = item->parent;
    free_queued(item);
    queue_update(q);
}

void draw_queue(srv_queue_t* q)
{
    srv_queue_item_t* it = q->first;
    while(it && it->on_screen) {
        draw_notif(q->c, it->notif);
        it = it->next;
    }
}

