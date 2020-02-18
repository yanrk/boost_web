#include "echo_service.h"

int main(int, char *[])
{
    EchoService server;

    server.init();

    getchar();

    server.exit();

    return (0);
}
