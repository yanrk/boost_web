#include "test_service.h"

int main(int, char *[])
{
    TestService server(true, 1000, 10);

    server.init();

    getchar();

    server.exit();

    return (0);
}
