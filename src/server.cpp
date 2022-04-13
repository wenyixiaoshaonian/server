#include <iostream>
#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include "server.h"
#include <cstring>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/wait.h>

namespace avdance {

Server::Server(){
  in_buf[MESSAGE_SIZE] = {0,};
  accept_fds[FD_SIZE] = {-1, };
  char usendbuf[MESSAGE_SIZE] = {0,};
  char urecvbuf[MESSAGE_SIZE] = {0,};
  char uin_buf[MESSAGE_SIZE] = {0,};
  pid = -1;
  sSocket_fd = -1;
  sAccept_fd = -1;
  mPort = 8111;
  backlog = 10;
  umPort = 9876;
  max_fd = -1;
  maxpos = 0;
  events=0;
  curpos = -1;

  epoll_fd = -1;
  event_number = 0;
  cSocket_fd = -1;
  usSocket_fd = -1;
  ucSocket_fd = -1;
  umPort = 9876;
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

void Server::sFork(){
  int fd0;
  
  if ((pid = fork()) < 0) {//注释2
    printf("can't fork \n");
    exit(-1);
  }
  else if (pid != 0) /* parent */
    exit(0);

  else {
    std::cout << "child process is runing..." << std::endl;
    ::usleep(1000000); //us sleep 1 second 
  }

  setsid();//脱离父进程的进程组 独立开来

  if (chdir("/") < 0) {
    printf("can't change directory to / \n");
    exit(-1);
  }

  fd0 = open("/dev/null", O_RDWR);
  dup2(fd0, STDIN_FILENO);
  dup2(fd0, STDOUT_FILENO);
  dup2(fd0, STDERR_FILENO);
}

void Server::sDaemon(){
    if(daemon(0,0) == -1)
        exit(EXIT_FAILURE);

}

void Server::sTcpserver_select(){

  int ret = -1;
  int on = 1;
  int flags = 1; //open REUSEADDR option

  socklen_t addr_len = sizeof( struct sockaddr_in );
  sSocket_fd = socket(AF_INET,SOCK_STREAM,0);
  if ( sSocket_fd == -1 ){
      perror("create socket error");
      exit(1);
  }

  flags = fcntl(sSocket_fd,F_GETFL,0);
  fcntl(sSocket_fd,F_SETFL,flags | O_NONBLOCK);

  ret = setsockopt(sSocket_fd,SOL_SOCKET,SO_REUSEADDR,&on,sizeof(on));
  if ( ret == -1 ){
    perror("setsockopt error");
  }

  //set local address
  local_addr.sin_family = AF_INET;
  local_addr.sin_port = htons(mPort);
  local_addr.sin_addr.s_addr = INADDR_ANY;
  bzero(&(local_addr.sin_zero), 8);

  ret = bind(sSocket_fd,(struct sockaddr *)&local_addr, sizeof(struct sockaddr_in));
  if(ret == -1 ) {
    perror("bind error");
    exit(1);
  }

  ret = listen(sSocket_fd,backlog);
  if ( ret == -1 ){
    perror("listen error");
    exit(1);
  }
  max_fd = sSocket_fd; //每次都重新设置 max_fd
  for(int i=0; i< FD_SIZE; i++){
    accept_fds[i] = -1; 
  }  
  while(1) {
    FD_ZERO(&fd_sets);
    FD_SET(sSocket_fd,&fd_sets);

    //修改最大值 并加入句柄集合
    for(int k=0; k<maxpos;k++) {
      if(accept_fds[k] != -1) {
        if (accept_fds[k] > max_fd) {
          max_fd = accept_fds[k];
        }
        printf("fd:%d, k:%d, max_fd:%d\n", accept_fds[k], k, max_fd);
        FD_SET(accept_fds[k], &fd_sets); //继续向sets添加fd
      }
    }

    events = select(max_fd + 1,&fd_sets,NULL,NULL,NULL);
    if(events < 0) {
      perror("select");
      break;
    }
    else if(events == 0) {
      printf("select time out ......");
      continue;
    }
    else if (events) {
      printf("events:%d\n", events);
      //新连接
      if(FD_ISSET(sSocket_fd,&fd_sets)) {
        int a = 0;
        for( ; a < FD_SIZE; a++){
          if(accept_fds[a] == -1){
            curpos = a;
            break;
          }
        }
      if(a == FD_SIZE){
          printf("the connection is full!\n");
          continue;
        }
      sAccept_fd = accept(sSocket_fd,(struct sockaddr *)&remote_addr,&addr_len);

      //将新连接设置为非阻塞
      flags = fcntl(sAccept_fd,F_GETFL,0);
      fcntl(sAccept_fd,F_SETFL,flags|O_NONBLOCK);
      
      accept_fds[curpos] = sAccept_fd;

      if(curpos+1 > maxpos){
          maxpos = curpos + 1; 
        }
      if(sAccept_fd > max_fd){
          max_fd = sAccept_fd; 
        }  
              printf("new connection fd:%d, curpos = %d \n",sSocket_fd, curpos);
  
      }
    }
    //有数据事件来时
    for(int j = 0;j < maxpos;j++) {
      if((accept_fds[j] != -1) && FD_ISSET(accept_fds[j],&fd_sets)) {
        printf("accept event :%d, accept_fd %d \n",j, accept_fds[j]);
        memset(in_buf, 0, MESSAGE_SIZE);
        ret = recv(accept_fds[j],(void*)in_buf,MESSAGE_SIZE,0);
        if(ret == 0) {
          close(accept_fds[j]);
          accept_fds[j] = -1;
        }
      printf( "receive message:%s", in_buf );
      send(accept_fds[j], (void*)in_buf, MESSAGE_SIZE, 0); 
      }
    }
  }
  printf("quit server...\n");
  close(sSocket_fd);

  return;
}
void Server::sTcpserver_epoll(){
  int ret = -1;
  int on = 1;
  int flags = 1; //open REUSEADDR option

  socklen_t addr_len = sizeof( struct sockaddr_in );
  

  for(int a = 0;a < NB_PROCESS;a++) {
    if(pid != 0) {
      pid = fork();
    }
  }
  if(pid == 0) {
    sSocket_fd = socket(AF_INET,SOCK_STREAM,0);
    if ( sSocket_fd == -1 ){
        perror("create socket error");
        exit(1);
    }

    ret = setsockopt(sSocket_fd,SOL_SOCKET,SO_REUSEADDR,&on,sizeof(on));
    if ( ret == -1 ){
      perror("setsockopt SO_REUSEADDR error");
    }
    ret = setsockopt(sSocket_fd, SOL_SOCKET, SO_REUSEPORT, &on, sizeof(on));
    if ( ret == -1 ){
      perror("setsockopt SO_REUSEPORT error");
    }
   flags = fcntl(sSocket_fd, F_GETFL, 0);
   fcntl(sSocket_fd, F_SETFL, flags|O_NONBLOCK);
    //set local address
    bzero(&local_addr, sizeof(local_addr));  
    local_addr.sin_family = AF_INET;
    local_addr.sin_port = htons(mPort);
    local_addr.sin_addr.s_addr = INADDR_ANY;
    bzero(&(local_addr.sin_zero), 8);
    ret = bind(sSocket_fd,(struct sockaddr *)&local_addr, sizeof(struct sockaddr_in));
    if(ret == -1 ) {
      perror("bind error");
      exit(1);
    }

    ret = listen(sSocket_fd,backlog);
    if ( ret == -1 ){
      perror("listen error");
      exit(1);
    }
  
    //create epoll
    epoll_fd = epoll_create(256); 
    ev.data.fd = sSocket_fd;
    ev.events = EPOLLIN;
    epoll_ctl(epoll_fd,EPOLL_CTL_ADD,sSocket_fd,&ev);

    while(1) {
      event_number = epoll_wait(epoll_fd,pevents,MAX_EVENTS,TIME_OUT);
      for(int k = 0;k < event_number;k++) {
        //如果是监听端口的事件
        if(pevents[k].data.fd == sSocket_fd) {
          printf("listen event... \n");
          sAccept_fd = accept(sSocket_fd,(struct sockaddr *)&remote_addr,&addr_len);
          //将新创建的socket设置为 NONBLOCK 模式
          flags = fcntl(sAccept_fd, F_GETFL, 0);
          fcntl(sAccept_fd, F_SETFL, flags|O_NONBLOCK);

          ev.data.fd=sAccept_fd;
          //设置为边缘触发
          ev.events=EPOLLIN | EPOLLET;
          epoll_ctl(epoll_fd, EPOLL_CTL_ADD, sAccept_fd, &ev);

          printf("new accept fd:%d\n",sAccept_fd);
        }
        else if(pevents[k].events & EPOLLIN) {
          memset(in_buf, 0, MESSAGE_SIZE);

          ret = recv(pevents[k].data.fd,(void*)in_buf,MESSAGE_SIZE,0);
          if(ret == MESSAGE_SIZE ){
            printf("maybe have data....");
          }
          else if(ret <= 0){
            switch (errno){
              case EAGAIN: //说明暂时已经没有数据了，要等通知
                break;
              case EINTR: //被终断了，再来一次
                printf("recv EINTR... \n");
                ret = recv(pevents[k].data.fd, &in_buf, MESSAGE_SIZE, 0);
                break;
              default:
                printf("the client is closed, fd:%d\n", pevents[k].data.fd);
                epoll_ctl(epoll_fd, EPOLL_CTL_DEL, pevents[k].data.fd, &ev); 
                close(pevents[k].data.fd);
            }
        }
        else if(ret > 0) {
          printf(">>>receive message:%s\n", in_buf);
          send(pevents[k].data.fd, &in_buf, ret, 0);
        }
        
        }
      }
    }
  close(sSocket_fd);
  printf("process:%d close sSocket_fd\n", getpid());
   
  }
  else {
    wait(&status);
  }
  return;
}
void Server::sTcpclient(){
  int ret;
  int data_len;
  if ((cSocket_fd = socket(AF_INET,SOCK_STREAM,0)) < 0) {
    perror("socket");
    return;
  }
  serverAddr.sin_family = AF_INET;
  serverAddr.sin_port = htons(mPort);
  //inet_addr()函数，将点分十进制IP转换成网络字节序IP
  serverAddr.sin_addr.s_addr = INADDR_ANY;

  if(connect(cSocket_fd,(struct sockaddr *)&serverAddr,sizeof(serverAddr)) < 0) {
    perror("connect");
    return;
  }

  printf("success to connect server...\n");

  while(1) {
    memset(sendbuf,0,MESSAGE_SIZE);
    printf("<<<<send message:\n");
    fgets(sendbuf,50, stdin);

    ret = send(cSocket_fd,sendbuf,strlen(sendbuf),0);
    if(ret <= 0 ){
      printf("the connection is disconnection!\n"); 
      break;
    }

    if(strcmp(sendbuf, "quit") == 0){
      break;
    }

    printf(">>> echo message:");

    recvbuf[0] = '\0';
    data_len = recv(cSocket_fd, recvbuf, MESSAGE_SIZE, 0);

    recvbuf[data_len] = '\0';

    printf("%s", recvbuf);
  }
  close(cSocket_fd);
  return;
}

void Server::sUdpserver(){
  int ret = -1;
  int n;
  int on = 1;
  socklen_t addr_len = sizeof( struct sockaddr_in );

  ulocal_addr.sin_family     = AF_INET;
  ulocal_addr.sin_port       = htons(umPort);
  ulocal_addr.sin_addr.s_addr = htonl(INADDR_ANY);
  usSocket_fd = socket(AF_INET,SOCK_DGRAM,0);

  if ( usSocket_fd == -1 ){
    perror("create socket error");
    exit(1);
  }
  ret = setsockopt(usSocket_fd,SOL_SOCKET,SO_REUSEADDR,&on,sizeof(on));
  if ( ret == -1 ){
    perror("setsockopt error");
  }
  ret = bind(usSocket_fd,(struct sockaddr *)&ulocal_addr, sizeof(struct sockaddr_in));
  if(ret == -1 ) {
    perror("bind error");
    exit(1);
  }

  while(1) {
    memset(uin_buf, 0, MESSAGE_SIZE);

    n = recvfrom(usSocket_fd, uin_buf, 1023, 0, (struct sockaddr *) &remote_addr, &addr_len);
    if (n > 0)
    {
//      uin_buf[n] = 0;
      printf("recv data from client:%s %u says: %s\n", inet_ntoa(remote_addr.sin_addr), ntohs(remote_addr.sin_port), uin_buf);

      n = sendto(usSocket_fd, uin_buf, n, 0, (struct sockaddr *) &remote_addr, sizeof(remote_addr));
      if (n < 0)
      {
        printf("sendto error.\n");
        break;

      }
    }
  } 
  return; 
}

void Server::sUdpclient(){
  int n = -1;
  socklen_t addr_len = sizeof( struct sockaddr_in );
  serverAddr.sin_family = AF_INET;
  serverAddr.sin_port = htons(umPort);

  //inet_addr()函数，将点分十进制IP转换成网络字节序IP
  serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");

  if ((ucSocket_fd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
  {
    printf("socket error.\n");
    exit(1);
  }

  while(1) {
    printf("<<<<send message:\n");
    fgets(urecvbuf,50, stdin);
    n = sendto(ucSocket_fd, urecvbuf, strlen(urecvbuf), 0, (struct sockaddr *) &serverAddr, sizeof(serverAddr));
    if (n < 0)
    {
      printf("sendto error.\n");
      close(ucSocket_fd);
    }

    n = recvfrom(ucSocket_fd, urecvbuf, 1023, 0, (struct sockaddr *) &serverAddr, &addr_len);
    if (n > 0)
    {
        urecvbuf[n] = 0;
        printf("received from sever:");
        puts(urecvbuf);
    }
    else if (n == 0) {
        printf("server closed.\n");
        return;
    }
        
    else if (n == -1) {
        printf("recvfrom error.\n");
        return;
    }
        
  }
  close(ucSocket_fd);
  return;
}
} // namesapce avdance

