
#include "timer.h"
#include <stdlib.h>
#include <unistd.h>

srv_timer_t* start_timer(uint32_t time)
{
    srv_timer_t* timer = malloc(sizeof(srv_timer_t));
    timer->ended = 0;
    timer->time  = (clock_t)((double)time / 1000.0 * (double)sysconf(_SC_CLK_TCK));
    timer->start = times(NULL);
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
    clock_t spent = times(NULL) - timer->start;
    if(spent >= timer->time)
        return 0;
    else
        return (uint32_t)((double)(timer->time - spent) / (double)sysconf(_SC_CLK_TCK) * 1000.0);
}

void end_timer(srv_timer_t* timer)
{
    free(timer);
}

