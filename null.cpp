#include <pthread.h>
#include <sys/types.h>
#include <memory.h>
#include "express.h"
#include "express_server.h"

extern int m_threads_running;

static unsigned char m_null[TP_SIZE];

//
// Format a transport packet
//
void null_fmt( void )
{
    m_null[0] = 0x47;
    m_null[1] = 0;
    // Add the 13 bit pid
	m_null[1] = 0x1F;
	m_null[2] = 0xFF;
	m_null[3] = 0x10;
    for( int i = 4; i < TP_SIZE; i++ ) m_null[i] = 0xFF;
}
void update_cont_counter( void )
{
   unsigned char c;

   c = m_null[3]&0x0F;
   c = (c+1)&0x0F;
   m_null[3] = (m_null[3]&0xF0) | c;
}
//
// Send a NULL packet
//
void null_pkt( void )
{
    update_cont_counter();
    uchar *b = alloc_buff();
    memcpy(b,m_null,TP_SIZE);
    post_buff(b);
}
