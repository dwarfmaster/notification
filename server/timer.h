
#ifndef DEF_TIMER
#define DEF_TIMER

#include <time.h>
#include <stdint.h>

typedef struct _srv_timer_t {
    time_t start;
    uint32_t time;
    int ended;
} srv_timer_t;

/* time in seconds */
srv_timer_t* start_timer(uint32_t tm);
int timer_ended(srv_timer_t* timer);
/* time in seconds */
uint32_t time_left(srv_timer_t* timer);
void end_timer(srv_timer_t* timer);

#endif

