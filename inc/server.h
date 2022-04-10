#ifndef __SERVER_H__
#define __SERVER_H__

#include <netinet/in.h>

#define MESSAGE_SIZE 1024

namespace avdance {

class Server {

  public:
    Server();  //consrtuct
    ~Server(); //destruct

  public:
    void run();
    void sFork();
    void sDaemon();

    void sTcpserver();
    void sUdpserver();

    void sTcpclient();
    void sUdpclient();
  private:
    pid_t pid = -1;
    int sSocket_fd = -1;
    int sAccept_fd = -1;
    struct sockaddr_in local_addr, remote_addr;
    char in_buf[MESSAGE_SIZE];
    int mPort = 8444;
    int backlog = 10;

    int cSocket_fd = -1;
    struct sockaddr_in serverAddr;
  char sendbuf[MESSAGE_SIZE];
  char recvbuf[MESSAGE_SIZE];
};

} //namespace avdance

#endif //__SERVER_H__
