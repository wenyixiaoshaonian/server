#ifndef __SERVER_H__
#define __SERVER_H__

#include <netinet/in.h>
#include <fcntl.h>
#include <sys/epoll.h>
#include <errno.h>

#define MESSAGE_SIZE 1024
#define FD_SIZE 1024
#define MAX_EVENTS 20
#define TIME_OUT 500

namespace avdance {

class Server {

  public:
    Server();  //consrtuct
    ~Server(); //destruct

  public:
    void run();
    void sFork();
    void sDaemon();

    void sTcpserver_select();
    void sTcpserver_epoll();
    void sUdpserver();

    void sTcpclient();
    void sUdpclient();
  private:
    //TCP
    pid_t pid = -1;
    int sSocket_fd = -1;
    int sAccept_fd = -1;
    struct sockaddr_in local_addr, remote_addr;
    char in_buf[MESSAGE_SIZE];
    int mPort = 8444;
    int backlog = 10;
    //select
    fd_set fd_sets;
    int accept_fds[FD_SIZE];
    int max_fd = -1;
    int maxpos = 0;
    int events=0;
    int curpos = -1;

    //epoll
    int epoll_fd = -1;
    struct epoll_event ev, pevents[MAX_EVENTS];
    int event_number = 0;

    int cSocket_fd = -1;
    struct sockaddr_in serverAddr;
    char sendbuf[MESSAGE_SIZE];
    char recvbuf[MESSAGE_SIZE];

    //UDP
    int usSocket_fd = -1;
    int ucSocket_fd = -1;
    char usendbuf[MESSAGE_SIZE];
    struct sockaddr_in ulocal_addr;
    char urecvbuf[MESSAGE_SIZE];
    int umPort = 9876;
    char uin_buf[MESSAGE_SIZE];
};

} //namespace avdance

#endif //__SERVER_H__
