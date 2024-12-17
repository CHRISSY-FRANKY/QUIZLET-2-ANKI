#include <stdio.h>
#include <stdlib.h>
#include <microhttpd.h>
#include <stdbool.h>

#define PORT 8080

struct MHD_PostProcessor *submitLinkPOSTProcessor = NULL;
int submitLinkPOSTProcessorFlag = 0;

char *readFileContent(char *fileName);
static int requestHandler(void *cls, struct MHD_Connection *connection, const char *url, const char *method, const char *version, const char *upload_data, size_t *upload_data_size, void **ptr);

int main(void)
{
    printf("STARTING SERVER!!!\n");
    struct MHD_Daemon *daemon;
    daemon = MHD_start_daemon(MHD_USE_THREAD_PER_CONNECTION, PORT, NULL, NULL, (MHD_AccessHandlerCallback)&requestHandler, NULL, MHD_OPTION_END); // Start the HTTP daemon
    if (NULL == daemon)                                                                                                                           // Daemon failed to start (reminds me of the Vampire Diaries)
    {
        fprintf(stderr, "FAILED TO START DAEMON!\n");
        return 1;
    }
    printf("SERVER RUNNING ON PORT %d! PRESS ENTER TO QUIT!\n", PORT);
    getchar();               // Keep the server alive until user presses anything really
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

bool submitLinkIsValid(const char *link)
{
    printf("PROCESSING LINK!\n");
    const char *validDomain = "https://quizlet.com\0";
    while (*link == *validDomain) // Process the validation of the domain
    {
        *link++;
        *validDomain++;
    }
    if (*link++ != '/') // Domain is invalid
    {
        return false;
    }
    printf("THE DOMAIN IS VALID!\n");
    while (isdigit(*link)) // Process the validation of the unique ID
    {
        *link++;
    };
    if (*link++ != '/') // Unique ID is invalid
    {
        return false;
    }
    printf("THE ID IS VALID!");
    for (; *link == '-' || isalnum(*link); link++)
        ; // Process the validation of the deck title
    return *link++ == '/';
}

static int processPOSTsubmitLink(void *coninfo_cls, enum MHD_ValueKind kind, const char *key, const char *filename, const char *content_type, const char *transfer_encoding, const char *data, uint64_t off, size_t size)
{
    bool *flag = (bool *)coninfo_cls;
    if (submitLinkIsValid(data))
    {
        *flag = 1;
        printf("THE LINK PROVIDED IS VALID!\n");
    }
    else
    {
        *flag = 0;
        printf("THE LINK PROVIDED IS INVALID!\n");
    }
    return MHD_YES;
}

static int requestHandler(void *cls, struct MHD_Connection *connection, const char *url, const char *method, const char *version, const char *upload_data, size_t *upload_data_size, void **ptr)
{
    static bool initialConnectionEstablished = false;
    if (!initialConnectionEstablished) // Only establishes the initial connection once
    {
        initialConnectionEstablished = true; // Mark that the initial connection has been established
        printf("INITIAL CONNECTION CALL!\n");
        return MHD_YES; // Continue processing the request
    }
    printf("RECEIVED REQUEST FOR %s USING %s!\n", url, method);
    if (0 != strcmp(method, "GET") && 0 != strcmp(method, "POST")) // Ensuring a GET or a POST request is received
    {
        printf("INVALID REQUEST METHOD!\n");
        return MHD_NO;
    }
    char *page = NULL;
    if (strcmp(url, "/") == 0 && strcmp(method, "GET") == 0) // Generating response body for / GET request
    {
        page = readFileContent("index.html");
    }
    else if (strcmp(url, "/submitLink") == 0 && strcmp(method, "POST") == 0) // Generating response body for /submitLink POST request
    {
        if (*ptr == NULL) // A post hasn't been received
        {
            submitLinkPOSTProcessor = MHD_create_post_processor(connection, 1024, (MHD_PostDataIterator)processPOSTsubmitLink, &submitLinkPOSTProcessorFlag); // Setup the post processor
            *ptr = submitLinkPOSTProcessor;                                                                                                                  // Point to the location of the post processor
            return MHD_YES;                                                                                                                                  // Yes we're expecting more data
        }
        submitLinkPOSTProcessor = (struct MHD_PostProcessor *)*ptr; // A POST has been received and we're converting the generic pointer back into the post processor
        if (*upload_data_size > 0)                                  // If data was actually sent, it will be processed
        {
            MHD_post_process(submitLinkPOSTProcessor, upload_data, *upload_data_size); // Process the data
            *upload_data_size = 0;                                                     // We've finished processing the data
            return MHD_YES;                                                            // Wait for more data
        }
        MHD_destroy_post_processor(submitLinkPOSTProcessor); // We're done processing the data sent
        *ptr = NULL;                                         // Reset the generic pointer that aided
        if (submitLinkPOSTProcessorFlag)
        {
            page = readFileContent("index-valid-link.html");
        }
        else
        {
            page = readFileContent("index-invalid-link.html");
        }
    }
    else // Generating response body for ...
    {
        page = "^-^"; // Acting all kawaii and ditzy
    }
    if (page == NULL) // Reading file content failed
    {
        fprintf(stderr, "FAILED TO READ index.html!");
        page = "Welcome to Quizlet 2 Anki! Designed by Chrissy Franky!<br>Oops! There was a problem fetching this page!";
    }
    struct MHD_Response *response;
    int result;
    response = MHD_create_response_from_buffer(strlen(page), (void *)page, MHD_RESPMEM_PERSISTENT); // Build the actual response
    result = MHD_queue_response(connection, MHD_HTTP_OK, response);                                 // Create the status indicator/result
    MHD_destroy_response(response);                                                                 // Destroy response since its already been queued
    printf("RESPONSE SENT\n");
    return result; // Return the conclusion to the queued response
}