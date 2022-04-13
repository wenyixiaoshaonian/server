#include "tcpServer_high.h"


int main(int argc, char* argv[])
{
    cbx_avdance::Hserver* hserver = new cbx_avdance::Hserver();

    hserver->TCPServer_event();
    return 0;
}