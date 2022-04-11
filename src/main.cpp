#include <iostream>
#include "server.h"
#include "string.h"
int main(int argc, char* argv[])
{
  if(argc < 3) {
    printf("Usage: %s TCP/UDP server/client\n",argv[0]);
    return 0;
  }
  avdance::Server* server = new avdance::Server();

  if(!strcmp(argv[1],"TCP")) {
    if(!strcmp(argv[2],"server")) {
        server->sTcpserver();
    }
    else {
        server->sTcpclient();
    }
  }
  else if(!strcmp(argv[1],"UDP")) {
    if(!strcmp(argv[2],"server")) {
        server->sUdpserver();
    }
    else {
        server->sUdpclient();
    }
  }
  else 
    printf("params error!!\n");
   
  return 0;
}
