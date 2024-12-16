#include <stdio.h>
#include <stdlib.h>
#include <microhttpd.h>
#include <stdbool.h>

#define PORT 8080

char *readFileContent(char *fileName);
static int indexResponse(void *cls, struct MHD_Connection *connection, const char *url, const char *method, const char *version, const char *upload_data, size_t *upload_data_size, void **ptr);

int main(void)
{
    printf("STARTING SERVER!!!\n");
    struct MHD_Daemon *daemon;
    daemon = MHD_start_daemon(MHD_USE_THREAD_PER_CONNECTION, PORT, NULL, NULL, (MHD_AccessHandlerCallback)&indexResponse, NULL, MHD_OPTION_END); // Start the HTTP daemon
    if (NULL == daemon)                                                                                                                          // Daemon failed to start (reminds me of the Vampire Diaries)
    {
        fprintf(stderr, "FAILED TO START DAEMON!\n");
        return 1;
    }
    printf("SERVER RUNNING ON PORT %d! PRESS ENTER TO QUIT!\n", PORT);
    getchar();               // Keep the server alive until user presses ENTER
    MHD_stop_daemon(daemon); // Stop the server when done
    return 0;
}

char *readFileContent(char *fileName)
{
    FILE *file = fopen(fileName, "rb"); // Open the file in read binary mode
    if (file == NULL)                   // Failed to open file to being with
    {
        fprintf(stderr, "FILE FAILED TO OPEN!");
        return NULL;
    }
    fseek(file, 0, SEEK_END);                         // Move to the end of the file
    unsigned long fileSize = ftell(file);             // Get the lengh of the file
    fseek(file, 0, SEEK_SET);                         // Reset the file pointer to the start
    char *fileContent = (char *)malloc(fileSize + 1); // Allocate memory for the content of the file plus the null temrinator
    if (fileContent == NULL)                          // Failed to allocate memory to begin with
    {
        fprintf(stderr, "FAILED TO ALLOCATE MEMORY!");
        fclose(file);
        return NULL;
    }
    size_t readSize = fread(fileContent, 1, fileSize, file); // Read file contents
    if (readSize != fileSize)                                // Reading file failed
    {
        fprintf(stderr, "Reading file FAILED!");
        free(fileContent); // No leaks
        fclose(file);
        return NULL;
    }
    fileContent[fileSize] = '\0'; // Add the null terminator
    fclose(file);
    return fileContent;
}

static int indexResponse(void *cls, struct MHD_Connection *connection, const char *url, const char *method, const char *version, const char *upload_data, size_t *upload_data_size, void **ptr)
{
    static bool initialConnectionEstablished = false;
    if (!initialConnectionEstablished) // Only establishes the initial connection once
    {
        initialConnectionEstablished = true; // Mark that the initial connection has been established
        printf("INITIAL CONNECTION CALL!\n");
        return MHD_YES; // Continue processing the request
    }
    printf("RECEIVED REQUEST FOR %s USING %s!\n", url, method);
    if (0 != strcmp(method, "GET")) // Ensuring a GET request is received
    {
        printf("INVALID REQUEST METHOD!\n");
        return MHD_NO;
    }
    const char *page = readFileContent("index.html"); // Building the response body
    if (page == NULL)                                 // Reading file content failed
    {
        fprintf(stderr, "FAILED TO READ index.html!");
        page = "Welcome to Quizlet 2 Anki! Designed by Chrisy Franky!<br>Oops! There was a problem fetching this page!";
    }
    struct MHD_Response *response;
    int result;
    response = MHD_create_response_from_buffer(strlen(page), (void *)page, MHD_RESPMEM_PERSISTENT); // Build the actual response
    result = MHD_queue_response(connection, MHD_HTTP_OK, response);                                 // Create the status indicator
    MHD_destroy_response(response);                                                                 // Destroy response since its already been queued
    printf("RESPONSE SENT\n");
    return result; // Return the conclusion to the queued response
}