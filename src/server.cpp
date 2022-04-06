#include <iostream>
#include <unistd.h>

#include "server.h"


namespace avdance {

Server::Server(){
  std::cout << "Server construct..." << std::endl;
}

Server::~Server(){
  std::cout << "Server destruct..." << std::endl;
}

void Server::run(){
  while(1){
  
    std::cout << "the server is runing..." << std::endl;
    ::usleep(1000000); //us sleep 1 second 

  }
}

} // namesapce avdance

