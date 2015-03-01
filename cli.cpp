#include <stdio.h>
#include <string.h>
#include <math.h>
#include <stdlib.h>

#include "clitypes.h"
#include "cli.h"
#include "express.h"
#include "express_server.h"

extern int m_threads_running;

double m_frequency;
double m_symbol_rate;
int    m_level;
int    m_csock;
int    m_tsock;
int    m_interpolate;

int cli_set_cmd( char *wa, char *wb, char *wc, char *wd )
{
    if(strncmp(wa,"freq", 4) == 0 )
    {
        m_frequency = atof(wb);
        express_set_freq( m_frequency );
        return 0;
    }
    if(strncmp(wa,"srate", 5) == 0 )
    {
        m_symbol_rate = atof(wb);
        express_set_sr( m_symbol_rate );
        return 0;
    }
    if(strncmp(wa,"fec", 3) == 0 )
    {
        //
        // This has to be done to get the interpolation rate
        //

        if(strncmp(wb,"1/2",3) == 0 )
        {
            express_set_fec( FEC_RATE_12 );
            return 0;
        }
        if(strncmp(wb,"2/3",3) == 0 )
        {
            express_set_fec( FEC_RATE_23 );
            return 0;
        }
        if(strncmp(wb,"3/4",3) == 0 )
        {
            express_set_fec( FEC_RATE_34 );
            return 0;
        }
        if(strncmp(wb,"5/6",3) == 0 )
        {
            express_set_fec( FEC_RATE_56 );
            return 0;
        }
        if(strncmp(wb,"7/8",3) == 0 )
        {
            express_set_fec( FEC_RATE_78 );
            return 0;
        }
    }
    if(strncmp(wa,"level", 5) == 0 )
    {
        double level = atof(wb);
        m_level = (level*47.0)/100;
        if(m_level > 47 ) m_level = 47;
        express_set_level( m_level );
        return 0;
    }
    if(strncmp(wa,"ipadd", 5) == 0 )
    {
//        m_frequency = atof(wb);// place marker
        return 0;
    }
    if(strncmp(wa,"tsock", 5) == 0 )
    {
        m_tsock = atoi(wb);
        return 0;
    }

    if(strncmp(wa,"ptt", 3) == 0 )
    {
        if(strncmp(wb,"tx", 2) == 0 )
        {
            express_transmit();
            return 0;
        }
        if(strncmp(wb,"rx", 2) == 0 )
        {
            express_receive();
            return 0;
        }
    }
    if(strncmp(wa,"iqcal", 3) == 0 )
    {
        if(strncmp(wb,"on", 2) == 0 )
        {
            express_set_iqcalibrate(true);
            return 0;
        }
        if(strncmp(wb,"off", 2) == 0 )
        {
            express_set_iqcalibrate(false);
            return 0;
        }
    }
    if(strncmp(wa,"ical", 3) == 0 )
    {
        int offset = atoi(wb);
        express_set_ical(offset);
        return 0;
    }
    if(strncmp(wa,"qcal", 3) == 0 )
    {
        int offset = atoi(wb);
        express_set_qcal(offset);
        return 0;
    }
    if(strncmp(wa,"carrier", 3) == 0 )
    {
        if(strncmp(wb,"on", 2) == 0 )
        {
            express_set_carrier(true);
            return 0;
        }
        if(strncmp(wb,"off", 2) == 0 )
        {
            express_set_carrier(false);
            return 0;
        }
    }
    if(strncmp(wa,"ramp", 3) == 0 )
    {
        if(strncmp(wb,"on", 2) == 0 )
        {
            express_set_ramp(true);
            return 0;
        }
        if(strncmp(wb,"off", 2) == 0 )
        {
            express_set_ramp(false);
            return 0;
        }
    }
    if(strncmp(wa,"predist", 3) == 0 )
    {
        if(strncmp(wb,"on", 2) == 0 )
        {
            express_set_predist(true);
            return 0;
        }
        if(strncmp(wb,"off", 2) == 0 )
        {
            express_set_predist(false);
            return 0;
        }
    }
    if(strncmp(wa,"ptab", 3) == 0 )
    {
        int add = atoi(wb);
        int ival = atoi(wc);
        int qval = atoi(wd);
        express_load_ptab(add,ival,qval);
        return 0;
    }

    if(strncmp(wa,"config", 4) == 0 )
    {
        return LOAD_CONFIG;
    }

    if(strncmp(wa,"kill", 4) == 0 )
    {
        m_threads_running = 0;
        return 0;
    }
    printcon("Syntax error %s %s\n",wa ,wb);
    return -1;
}
//
// Process commands for retrieving data
//

int cli_get_cmd( char *wa  )
{
    wa = wa;
    return 0;
}

int cli_string( const char *text )
{
    char word[5][255];
    int res = 0;

    word[0][0] = word[1][0] = word[2][0] = word[3][0] = word[4][0] = 0;

    if(text[0] != '#')
    {
        sscanf(text,"%s %s %s %s %s", word[0], word[1], word[2], word[3], word[4]);
        if(strncmp(word[0],"set", 3) == 0 ) res = cli_set_cmd( word[1], word[2], word[3], word[4]);
        if(strncmp(word[0],"get", 3) == 0 ) res = cli_get_cmd( word[1]);
    }
    return res;
}
//
// Read in a process a buffer from the pipe or UDP interface, commands are terminated in \n
//
void cli_parse_buffer( char *b, int length )
{
    static char buf[1024];
    static int  buf_len;

    for( int i = 0; (i < length) && (buf_len < 1023) ; i++ )
    {
        buf[buf_len++] = b[i];
        if(b[i] == '\n')
        {
            buf[buf_len] = 0;// Terminate the string
            if( cli_string(buf)==LOAD_CONFIG) cli_read_file( CONFIG_FILENAME_S );
            buf_len = 0;// Start a new string
        }
    }
}

void cli_read_file( const char *filename )
{
    FILE *fp;
    char text[255];

    if((fp=fopen( filename, "r" ))!=NULL)
    {
        while(fgets(text,200,fp) != NULL)
        {
            cli_string(text);
        }
    }
    else
    {
        printcon("Unable to open configuration file %s\n",filename);
    }
}
