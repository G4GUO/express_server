//
// This is called and starts everything running
//
#include <pthread.h>
#include <stdio.h>
#include <stdarg.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <string.h>
#include "express_server.h"
#include "cli.h"
#include "express.h"

static pthread_t m_threads[3];
int m_threads_running;
static int m_web;

#define CTRL_PIPE_MESG_LEN 40

//
// Common print routine
//
void printcon( const char *fmt, ... )
{
    if(!m_web)
    {
        va_list ap;
        va_start(ap,fmt);
        vprintf(fmt,ap);
        va_end(ap);
    }
}
//
// This uses a named pipe and accepts commands
//
void *command_thread_blocking( void *arg )
{
    // This blocks
    char msg[CTRL_PIPE_MESG_LEN];
    char pipename[40];
    int fd;
    struct timespec tim;

    tim.tv_sec  = 0;
    tim.tv_nsec = 100000000; // 10 ms

    strcpy(pipename,"/tmp/expctrl");

    mkfifo( pipename, 0666);

    if((fd = open( pipename, O_RDONLY )) < 0) printcon("Unable to open named pipe\n");

    while( m_threads_running )
    {
        int len = read( fd, msg, CTRL_PIPE_MESG_LEN );

        if( len > 0 )
        {
            cli_parse_buffer( msg, len );
        }
        else
        {
            nanosleep( &tim, NULL);
        }
    }
    close(fd);
    unlink(pipename);
    return arg;
}
//
// This processes the transport stream from the UDP socket
//
void *udp_thread_blocking( void *arg )
{
    // This blocks
    uchar *b;

    while( m_threads_running )
    {
        b = alloc_buff();
        get_udp_buffer( b, TP_SIZE );
        if( b[0] != TP_SYNC ){ // Try to achieve sync
            for( int i = 0; i < TP_SIZE-1; i++ ){
                get_udp_buffer( b, 1 );
                if( b[0] == TP_SYNC ){
                    get_udp_buffer( b, TP_SIZE - 1 );
                }
            }
        }
        // Queue
        post_buff( b );
    }
    return arg;
}
//
// This processes the transport stream from STDIN
//
void *stdin_thread_blocking( void *arg )
{
    // This blocks
    uchar *b;

    while( m_threads_running )
    {
        b = alloc_buff();
        int len = fread(b, 1, TP_SIZE, stdin);
//        printcon("packet received %d\n",len);
        if( b[0] == TP_SYNC )
        {
            // Aligned to TP (probably)
            post_buff( b );
        }
        else
        {
            // Slip bytes until TP alignment
            printcon("STDIN Invalid sync byte %.2X %d\n",b[0],len);
            len = fread(b, 1, 1, stdin);
            rel_buff( b );
        }
    }
    return arg;
}
//
// This keeps the USB2 link full
//
void *tx_thread_blocking( void *arg )
{
    // This blocks
    uchar *b;

    while( m_threads_running )
    {
        if((b=get_buff())!= NULL)
            express_write_transport_stream( b, TP_SIZE );
        else
        {
            // Underrun
           null_pkt();
        }
    }
    return arg;
}

int initialise_hw(const char *fname)
{
    const char *rbf;
 
    printcon("Initialising hardware please wait....\n");

 
    if(express_init( "datvexpress8.ihx", "datvexpressdvbs.rbf" ) > 0 )
    {
        if( fname == NULL)
            cli_read_file("datvexpress.txt");
        else
            cli_read_file(fname);
        return 0;
    }
    return -1;
}
//
// If this is sucessfull it blocks in a forever loop
// otherwise it returns an error
//
int express_main(int argc, char *argv[])
{
    int stdin_flag = 0;
    int nb_flag    = 0;
    int si570_flag = 0;

    const char *fname = NULL;

    m_web          = 0;
    // Display the version number
    printcon("%s\n",S_VERSION);
    // Set the umask (so fifo can bw written to by others)
    umask(0000);

    // parse command line
    for( int i = 1; i < argc; i++ )
    {
        int unknown_option = 1;
        if(strcmp(argv[i],"-i")==0)
        {
            if(argc > i+1)
                if(strcmp(argv[i+1],"stdin")==0) stdin_flag = 1;
            unknown_option = 0;
        }
        if(strcmp(argv[i],"-nb")==0)
        {
            nb_flag = 1;
            unknown_option = 0;
        }
        if(strcmp(argv[i],"-si570")==0)
        {
            si570_flag = 1;
            unknown_option = 0;
        }
        if(strcmp(argv[i],"-web")==0)
        {
            m_web = 1;
            unknown_option = 0;
        }
        if(strcmp(argv[i],"-f")==0)
        {
            if( argc > i+1 ) fname = argv[i+1];
            unknown_option = 0;
        }
        if(unknown_option) printcon("Unknown option %s\n",argv[i]);
    }
    if(m_web)
    {
        fclose(stdin);
        fclose(stdout);
        fclose(stderr);
    }
    buf_init();
    null_fmt();

    if( initialise_hw(fname) == 0 )
    {
        // Start the process threads
        m_threads_running = 1;

        if(pthread_create( &m_threads[0], NULL, tx_thread_blocking, NULL ) != 0 )
        {
            printcon("Unable to start tx thread\n");
            m_threads_running = 0;
            return -1;
        }

        if(pthread_create( &m_threads[1], NULL, command_thread_blocking, NULL ) != 0 )
        {
            printcon("Unable to start command thread\n");
            m_threads_running = 0;
            return -1;
        }

        if( stdin_flag )
        {
            if(pthread_create( &m_threads[0], NULL, stdin_thread_blocking, NULL ) != 0 )
            {
                printcon("Unable to start stdin thread\n");
                m_threads_running = 0;
                return -1;
            }
            printcon("STDIN and process threads are now running\n");
        }
        else
        {
            if( udp_init() == 0 )
            {

                if(pthread_create( &m_threads[0], NULL, udp_thread_blocking, NULL ) != 0 )
                {
                    printcon("Unable to start udp thread\n");
                    m_threads_running = 0;
                    return -1;
                }
                printcon("UDP sockets and process threads are now running\n");
            }
        }

        while( m_threads_running )
        {
            sleep(1);
        }
        printcon("Express Terminated\n");
        return 0;// Success!
    }
    return -1;
}
