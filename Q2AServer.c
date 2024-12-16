#include <stdio.h>
#include <stdlib.h>
#include <microhttpd.h>

#define PORT 8080

static int indexResponse(void *cls, struct MHD_Connection *connection, const char *url, const char *method, const char *version, const char *upload_data, size_t *upload_data_size, void **connection_closure)
{
    static int initialConnectionCall;
    if (initialConnectionCall != *connection_closure) // Check if this is the initial connection call / headers only
    {
        *connection_closure = &initialConnectionCall;
        return MHD_YES;
    }

    if (0 != strcmp(method, "GET")) // Ensuring a GET request is received
    {
        return MHD_NO;
    }

    const char *page = "Welcome to Quizlet 2 Anki! Designed by Chriss Franky!"; // Building the response body
    struct MHD_Response *response;
    int ret;
    response = MHD_create_response_from_buffer(strlen(page), (void *)page, MHD_RESPMEM_PERSISTENT); // Build the actual response
    ret = MHD_queue_response(connection, MHD_HTTP_OK, response);                                    // Create the status indicator
    MHD_destroy_response(response);                                                                 // Destroy response since its already been queue
    return ret;                                                                                     // Return the conclusion to the queued response
}

int main(void)
{
    printf("Hello, World!\n");
}