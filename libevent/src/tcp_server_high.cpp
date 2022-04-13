#include "tcpServer_high.h"

namespace cbx_avdance {

void Hserver::echo_read_cb(struct bufferevent *bev,void *ctx) {
    printf("recv event... \n");
    struct evbuffer *input = bufferevent_get_input(bev);
    struct evbuffer *output = bufferevent_get_output(bev);
    //将input数据全部复制到output中,会触发写事件
    evbuffer_add_buffer(output,input);
}

void Hserver::echo_event_cb(struct bufferevent *bev, short events, void *ctx) {
    if (events & BEV_EVENT_ERROR)
        perror("Error from bufferevent");
    if (events & (BEV_EVENT_EOF | BEV_EVENT_ERROR)) {
        bufferevent_free(bev);
    }
}

void Hserver::accept_conn_cb(struct evconnlistener *listener,evutil_socket_t fd, struct sockaddr *address, int socklen,void *ctx) {
    struct event_base *base = evconnlistener_get_base(listener);
    struct bufferevent *bev = bufferevent_socket_new(base, fd, BEV_OPT_CLOSE_ON_FREE);
    printf("new accept connection...\n");
    /*设置buferevent的回调函数，这里设置了读和事件的回调函数*/
    bufferevent_setcb(bev, echo_read_cb, NULL, echo_event_cb, NULL);
    /* 启用该bufevent写和读  */
     bufferevent_enable(bev, EV_READ|EV_WRITE);

}
int Hserver::TCPServer_event() {

    base = event_base_new();
    if(!base) {
        perror("Couldn't open event base:");
        return -1;
    }
    memset(&server_sin,0,sizeof(server_sin));
    server_sin.sin_family = AF_INET;
    server_sin.sin_addr.s_addr = INADDR_ANY;
    server_sin.sin_port = htons(PORT);
    bzero(&(server_sin.sin_zero), 8);

    listener = evconnlistener_new_bind(base,accept_conn_cb,NULL,
                                        LEV_OPT_CLOSE_ON_FREE|LEV_OPT_REUSEABLE,-1,
                                        (struct sockaddr*)&server_sin, sizeof(server_sin));
    if(!listener) {
        perror("Couldn't create listener");
        return 1;
    }
    printf("Server construct...\n");
    //开始循环
    event_base_dispatch(base);

    return 0;
}


}