
#include "queue.h"
#include <stdlib.h>

srv_queue_t* init_queue(xcb_connection_t* c, srv_screen_t* scr)
{
    srv_queue_t* q = malloc(sizeof(srv_queue_t));
    q->c        = c;
    q->scr      = scr;
    q->used     = 0;
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

    free_notif(item->parent->c, &item->notif);
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

srv_queue_item_t* add_notif(srv_queue_t* q, const char* name, const char* text)
{
}

void rm_notif(srv_queue_item_t* item)
{
    free_queued(item);
    /* TODO update showed. */
}

void draw_queue(srv_queue_t* q)
{
    srv_queue_item_t* it = q->first;
    while(it && it->on_screen) {
        draw_notif(q->c, &it->notif);
        it = it->next;
    }
}

