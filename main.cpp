#include <stdio.h>
#include "express_server.h"
#include "express.h"
#include "cli.h"

extern int m_threads_running;

int main(int argc, char *argv[])
{
   if( express_main( argc, argv ) < 0 )
   {
       express_deinit();
       return(-1);
   }
   express_deinit();

   return 0;
}
