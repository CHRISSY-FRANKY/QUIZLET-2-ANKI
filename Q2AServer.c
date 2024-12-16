#include <stdio.h>
#include <stdlib.h>
#include <microhttpd.h>

#define PORT 8080

static int indexResponse(void *cls, struct MHD_Connection *connection, const char *url, const char *method, const char *version, const char *upload_data, size_t *upload_data_size, void **ptr)
{
    printf("INDEX RESPONSE CALLED!\n");
    static int initialConnectionCall;
    if (&initialConnectionCall != (int*)ptr) // Check if this is the initial connection call / headers only
    {
        *ptr = &initialConnectionCall;
        printf("INITIAL CONNECTION CALL\n");
        return MHD_YES;
    }

    printf("RECEIVED REQUEST FOR %s USING %s\n", url, method);
    if (0 != strcmp(method, "GET")) // Ensuring a GET request is received
    {
        printf("INVALID REQUEST METHOD\n");
        return MHD_NO;
    }

    const char *page = "Welcome to Quizlet 2 Anki! Designed by Chriss Franky!"; // Building the response body
    struct MHD_Response *response;
    int result;
    response = MHD_create_response_from_buffer(strlen(page), (void *)page, MHD_RESPMEM_PERSISTENT); // Build the actual response
    result = MHD_queue_response(connection, MHD_HTTP_OK, response);                                 // Create the status indicator
    MHD_destroy_response(response);                                                                 // Destroy response since its already been queue
     printf("Response sent\n");
    return result;                                                                                  // Return the conclusion to the queued response
}
int main(void)
{
    printf("Starting server...\n");
    fflush(stdout);
    struct MHD_Daemon *daemon;

    // Start the HTTP daemon
    daemon = MHD_start_daemon(MHD_USE_THREAD_PER_CONNECTION, PORT, NULL, NULL, (MHD_AccessHandlerCallback)&indexResponse, NULL, MHD_OPTION_END);

    if (NULL == daemon) // Daemon failed to start (reminds me of the Vampire Diaries)                                                                                                                                                   
    {
        fprintf(stderr, "FAILED TO START DAEMON!\n");
        return 1;
    }

    printf("SERVER RUNNING ON PORT %d! PRESS ENTER TO QUIT!\n", PORT); 

    getchar(); // Keep the server alive until user presses ENTER
    MHD_stop_daemon(daemon); // Stop the server when done

    return 0;
}