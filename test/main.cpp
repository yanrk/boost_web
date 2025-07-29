#include "test_service.h"

int main(int argc, char * argv[])
{
    if (argc < 2)
    {
        TestService server(true, false, true, 1000, 10);

        server.init();

        getchar();

        server.exit();
    }
    else
    {
        TestService client(false, false, false, 1000, 10);

        client.init();

        getchar();

        client.exit();
    }

    return (0);
}
