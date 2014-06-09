
#ifndef DEF_TIMER
#define DEF_TIMER

#include <time.h>
#include <sys/times.h>
#include <stdint.h>

typedef struct _srv_timer_t {
    clock_t start;
    clock_t time;
    int ended;
} srv_timer_t;

/* time in ms */
srv_timer_t* start_timer(uint32_t time);
int timer_ended(srv_timer_t* timer);
/* time in ms */
uint32_t time_left(srv_timer_t* timer);
void end_timer(srv_timer_t* timer);

#endif

