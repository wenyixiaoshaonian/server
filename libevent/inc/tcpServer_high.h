#pragma
#include <event2/listener.h>
#include <event2/bufferevent.h>
#include <event2/buffer.h>
#include <event2/thread.h>

#include <arpa/inet.h>

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>

#define PORT 8111
namespace cbx_avdance {

class Hserver {
public:
    Hserver(){}
    ~Hserver(){}

    int TCPServer_event();
    static void accept_conn_cb(struct evconnlistener *listener,evutil_socket_t fd, struct sockaddr *address, int socklen,void *ctx);
    static void echo_read_cb(struct bufferevent *bev,void *ctx);
    static void echo_event_cb(struct bufferevent *bev, short events, void *ctx);
private:
    struct event_base *base;
    struct sockaddr_in server_sin;
    struct evconnlistener *listener;
};





}