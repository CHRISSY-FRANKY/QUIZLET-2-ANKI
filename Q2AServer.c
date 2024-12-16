#include <stdio.h>
#include <stdlib.h>
#include <microhttpd.h>

#define PORT 8080

static int indexResponse(void *cls, struct MHD_Connection *connection, const char *url, const char *method, const char *version, const char *upload_data, size_t *upload_data_size, void **connection_closure)
{
    static int initialConnectionCall;
    if (initialConnectionCall != *connection_closure) // Check if this is the initial connection call / headers only
    {
        *connection_closure = initialConnectionCall;
        return MHD_YES;
    }

    return 0;
}

int main(void)
{
    printf("Hello, World!\n");
}