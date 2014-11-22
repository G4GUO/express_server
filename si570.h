#ifndef SI570_H
#define SI570_H

#include <math.h>
#include "express.h"

#define MIN_DCO 4850000000.0L
#define MAX_DCO 5670000000.0L
#define FCLK    114285000.0L
#define N_HSDIV 6

typedef struct{
    unsigned char r;
    unsigned char n;
}HsDiv;

void si570_set_clock( long double fclk );
void si570_rx( unsigned char rer, unsigned char val );
void si570_query( unsigned char reg);
void si570_initialise( void );

#endif // SI570_H
