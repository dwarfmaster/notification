
#ifndef DEF_FIFO
#define DEF_FIFO

typedef enum _srv_order_t {
    CLOSE,     /** Close the actual notification. */
    CLOSE_ALL, /** Close all notifications. */
    NOTIF,     /** Open a new notification. */
    END,       /** End the server. */
    NONE       /** If the order syntax is invalid. */
} srv_order_t;

int init_fifo();
void close_fifo();
int get_fifo_id();
/* Put the argument on the buffer. */
srv_order_t get_order_fifo(int size, char* buffer);

#endif

