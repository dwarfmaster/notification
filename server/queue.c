
#include "queue.h"
#include <stdlib.h>
#include <string.h>

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

    if(item->timer)
        free(item->timer);
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

srv_queue_item_t* add_notif(srv_queue_t* q, uint32_t time, const char* name, const char* text)
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
    it->timer = start_timer(time);
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

void rm_notif_cond(srv_queue_t* q)
{
    srv_queue_item_t* it = q->first;
    srv_queue_item_t* next;

    while(it) {
        next = it->next;
        if(timer_ended(it->timer))
            free_queued(it);
        it = next;
    }
    queue_update(q);
}

uint32_t nearest_end(srv_queue_t* q)
{
    srv_queue_item_t* it = q->first;
    uint32_t nearest = 60000;
    uint32_t left;

    while(it) {
        left = time_left(it->timer);
        nearest = (nearest > left ? left : nearest);
        it = it->next;
    }

    return nearest;
}

static int is_number(const char* str)
{
    uint32_t i;
    for(i = 0; i < strlen(str); ++i) {
        if(str[i] < '0' || str[i] > '9')
            return 0;
    }
    return 1;
}

static uint32_t to_number(const char* str)
{
    uint32_t nb, i, exp, j;
    nb = 0;

    for(i = 0; i < strlen(str); ++i) {
        exp = 1;
        for(j = 0; j < strlen(str) - i - 1; ++j)
            exp *= 10;
        nb += (str[i] - '0') * exp;
    }
    return nb;
}

srv_queue_item_t* add_notif_str(srv_queue_t* q, const char* str)
{
    uint32_t time;
    const char* name;
    const char* text;
    char* buffer;
    const char* part;
    srv_queue_item_t* it;

    buffer = malloc(strlen(str) + 1);
    strcpy(buffer, str);

    part = strtok(buffer, " ");
    if(!part) {
        free(buffer);
        return NULL;
    }
    if(is_number(part)) {
        time = to_number(part);
        part = strtok(NULL, " ");
    }
    else
        time = 60000;

    if(!part || !has_gcontext(part)) {
        free(buffer);
        return NULL;
    }
    name = part;

    part = strtok(NULL, "\n");
    if(!part) {
        free(buffer);
        return NULL;
    }
    text = part;

    it = add_notif(q, time, name, text);
    free(buffer);
    return it;
}

