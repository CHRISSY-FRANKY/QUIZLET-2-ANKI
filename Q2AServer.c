#include <stdio.h>
#include <stdlib.h>
#include <microhttpd.h>

#define PORT 8080

static int indexResponse(void *cls, struct MHD_Connection *connection, const char *url, const char *method, const char *version, const char *upload_data, size_t *upload_data_size, void **con_cls)
{
    return 0;
}

int main(void)
{
    printf("Hello, World!\n");
}