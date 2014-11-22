#ifndef CLI_H
#define CLI_H

#include "clitypes.h"

void cli_string( const char *text );
void cli_read_file( const char *filename );
void cli_parse_buffer( char *b, int length );

#endif // CLI_H
