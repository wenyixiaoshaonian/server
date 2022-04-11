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

namespace avdance {

Server::Server(){
  in_buf[MESSAGE_SIZE] = {0,};
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

void Server::sTcpserver(){

  int ret = -1;
  int on = 1;
  
  socklen_t addr_len = sizeof( struct sockaddr_in );
  sSocket_fd = socket(AF_INET,SOCK_STREAM,0);
  if ( sSocket_fd == -1 ){
      perror("create socket error");
      exit(1);
  }

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

  while(1) {
    sAccept_fd = accept(sSocket_fd,(struct sockaddr *)&remote_addr,&addr_len);
    while(1) {
      memset(in_buf, 0, MESSAGE_SIZE);

      ret = recv(sAccept_fd,(void*)in_buf,MESSAGE_SIZE,0);
      if(ret == 0) {
        break;
      }
      printf( "receive message:%s", in_buf );
      send(sAccept_fd, (void*)in_buf, MESSAGE_SIZE, 0); 
    }
    printf("close client connection...\n");
    close(sAccept_fd);
  }
  printf("quit server...\n");
  close(sSocket_fd);

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
  serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");

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
    memset(in_buf, 0, MESSAGE_SIZE);

    n = recvfrom(usSocket_fd, in_buf, 1023, 0, (struct sockaddr *) &remote_addr, &addr_len);
    if (n > 0)
    {
//      uin_buf[n] = 0;
      printf("recv data from client:%s %u says: %s\n", inet_ntoa(remote_addr.sin_addr), ntohs(remote_addr.sin_port), in_buf);

      n = sendto(usSocket_fd, in_buf, n, 0, (struct sockaddr *) &remote_addr, sizeof(remote_addr));
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

