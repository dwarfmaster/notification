#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <sys/select.h>
#include <xcb/xcb.h>
#include "window.h"
#include "screen.h"
#include "config.h"
#include "graphic.h"
#include "notif.h"
#include "queue.h"
#include "fifo.h"

int main(int argc, char *argv[])
{
    xcb_connection_t* c;
    srv_screen_t* scr;
    srv_queue_t* queue;
    xcb_generic_event_t* e;
    fd_set toread;
    struct timeval timeout;
    int cfd, fifofd, maxfd;
    uint32_t end;
    srv_order_t order;
    char buffer[256];
    int cont;

    /* Loading the config. */
    if(!load_config()) {
        fprintf(stderr, "Couldn't load config.\n");
        return 1;
    }

    /* Opening the connection. */
    c = xcb_connect(NULL, NULL);
    cfd = xcb_get_file_descriptor(c);

    /* Getting the screens. */
    scr = load_screens(c);

    /* Opening the GCs. */
    if(!load_gcontexts(c, scr)) {
        fprintf(stderr, "Couldn't load the graphics contexts.\n");
        return 1;
    }

    /* Getting EWMH */
    request_ewmh(c);
    xcb_flush(c);
    if(!has_ewmh())
        return 1;

    /* Opening the queue. */
    queue = init_queue(c, scr);

    /* Opening the fifo. */
    if(!init_fifo()) {
        fprintf(stderr, "Couldn't open the fifo.\n");
        return 1;
    }
    fifofd = get_fifo_id();

    FD_ZERO(&toread);
    FD_SET(cfd, &toread);
    FD_SET(fifofd, &toread);
    timeout.tv_sec  = nearest_end(queue);
    timeout.tv_usec = 0;
    maxfd = (cfd > fifofd ? cfd : fifofd) + 1;

    cont = 1;
    while(cont) {
        end = select(maxfd, &toread, NULL, NULL, &timeout);
        if(end == 0) {
            rm_notif_cond(queue);
            xcb_flush(c);
        }
        else if(end > 0) {
            if(FD_ISSET(cfd, &toread)) {
                while((e = xcb_poll_for_event(c))) {
                    switch(e->response_type & ~0x80) {
                        case XCB_EXPOSE:
                            draw_queue(queue);
                            xcb_flush(c);
                            break;
                        default:
                            break;
                    }
                    free(e);
                }
            } else if(FD_ISSET(fifofd, &toread)) {
                order = get_order_fifo(256, buffer);
                switch(order) {
                    case CLOSE:
                        /* TODO */
                        break;
                    case CLOSE_ALL:
                        /* TODO */
                        break;
                    case NOTIF:
                        add_notif_str(queue, buffer);
                        xcb_flush(c);
                        break;
                    case END:
                        cont = 0;
                        break;
                    default:
                        break;
                }
            }
        }

        FD_ZERO(&toread);
        FD_SET(cfd, &toread);
        FD_SET(fifofd, &toread);
        timeout.tv_sec  = nearest_end(queue);
        timeout.tv_usec = 0;
    }

    close_fifo();
    close_queue(queue);
    free_gcontexts();
    free_config();
    free_screens(scr);
    close_ewmh();
    xcb_disconnect(c);
    return 0;
}

