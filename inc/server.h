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
#define NB_PROCESS 4

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
    pid_t pid;
    int sSocket_fd;
    int sAccept_fd;
    struct sockaddr_in local_addr, remote_addr;
    char in_buf[MESSAGE_SIZE];
    int mPort;
    int backlog;
    //select
    fd_set fd_sets;
    int accept_fds[FD_SIZE];
    int max_fd;
    int maxpos;
    int events;
    int curpos;

    //epoll
    int epoll_fd;
    struct epoll_event ev, pevents[MAX_EVENTS];
    int event_number;
    int status;

    int cSocket_fd;
    struct sockaddr_in serverAddr;
    char sendbuf[MESSAGE_SIZE];
    char recvbuf[MESSAGE_SIZE];

    //UDP
    int usSocket_fd;
    int ucSocket_fd;
    char usendbuf[MESSAGE_SIZE];
    struct sockaddr_in ulocal_addr;
    char urecvbuf[MESSAGE_SIZE];
    int umPort;
    char uin_buf[MESSAGE_SIZE];
};

} //namespace avdance

#endif //__SERVER_H__
