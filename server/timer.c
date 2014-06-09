
#include "timer.h"
#include <stdlib.h>
#include <unistd.h>

srv_timer_t* start_timer(uint32_t tm)
{
    srv_timer_t* timer = malloc(sizeof(srv_timer_t));
    timer->ended = 0;
    timer->time  = tm;
    timer->start = time(NULL);
    return timer;
}

int timer_ended(srv_timer_t* timer)
{
    if(timer->ended)
        return 1;
    else if(time_left(timer) == 0)
        return 1;
    else
        return 0;
}

uint32_t time_left(srv_timer_t* timer)
{
    uint32_t spent = difftime(time(NULL), timer->start);
    if(spent >= timer->time)
        return 0;
    else
        return timer->time - spent;
}

void end_timer(srv_timer_t* timer)
{
    free(timer);
}

