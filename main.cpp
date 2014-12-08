#include <QCoreApplication>
#include <stdio.h>
#include "express_server.h"
#include "express.h"
#include "cli.h"

extern int m_threads_running;

int main(int argc, char *argv[])
{
//    QCoreApplication a(argc, argv);
   if( express_main( argc, argv ) < 0 )
   {
       express_deinit();
       exit(-1);
   }
//       a.exec();
   express_deinit();

   return 0;
}
