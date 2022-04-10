#include <iostream>
#include "server.h"

int main(int argc, char* argv[])
{
  
  avdance::Server* server = new avdance::Server();

  // if(server){
  //   server->run();
  // }
//    server->sFork();
//    server->sDaemon();
//    server->run();
   server->sTcpserver();
  return 0;
}
