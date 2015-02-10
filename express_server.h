#ifndef EXPRESS_SERVER_H
#define EXPRESS_SERVER_H

typedef unsigned char uchar;

#define TP_SIZE 188
#define TP_SYNC 0x47
#define MAX_Q_LEN 1000

//
// Allocate a new buffer
//
uchar *alloc_buff(void);
//
// Release a buffer
//
void rel_buff(uchar *b);
//
// Post a buffer to the tx queue
//
void post_buff( uchar *b);
//
// Get a buffer from the tx queue
//
uchar *get_buff(void);
//
// Initialise the buffers
//
void buf_init(void);
//
// Initialise the null packet
void null_fmt( void );
void null_pkt(void);


int udp_init( void );
int get_udp_buffer( uchar *b, int len );

int express_main(int argc, char *argv[]);
void printcon( const char *fmt, ... );

#endif // EXPRESS_SERVER_H
