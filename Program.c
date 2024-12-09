#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

typedef enum // Custom type to track repeats
{
    none = 0,
    once = 1,
    twice = 2
} Repeats;

Repeats dleCtrlCounter = twice; // Data Link Escape Control Char Counter

char *getUnFormattedContent(char *filename) // Load the manually scraped text onto the system
{
    FILE *file = fopen(filename, "rb"); // Open the file
    if (file == NULL)                   // Opening file failed
    {
        fprintf(stderr, "Opening file %s FAILED!", filename);
        return NULL;
    }
    fseek(file, 0, SEEK_END);                                     // Set file pointer to the end
    unsigned int size = ftell(file);                              // Determine file size
    fseek(file, 0, SEEK_SET);                                     // Reset file pointer to the start
    char *unFormattedContent = malloc(sizeof(char) * (size + 1)); // Allocate for file content and null terminator
    if (unFormattedContent == NULL)                               // Memory allocation failed
    {
        fprintf(stderr, "Allocating memory FAILED!");
        return NULL;
    }
    size_t read_size = fread(unFormattedContent, 1, size, file); // Read file contents
    if (read_size != size)                                       // Reading file failed
    {
        fprintf(stderr, "Reading file FAILED!"); 
        free(unFormattedContent); // No leaks
        fclose(file);
        return NULL;
    }
    unFormattedContent[size] = '\0'; // Add the null terminator
    fclose(file);
    return unFormattedContent;
}

char *createFormattedText(char *unFormattedContent) // Format the loaded text for quizlet/anki to interpret it correctly
{
    unsigned long long int contentLength = strlen(unFormattedContent);
    char *text = (char *)(malloc(sizeof(char) * (1024 + contentLength))); // Allocate memory for text
    char *textPtr = text;                               // Create pointer for text
    while (*unFormattedContent != '\0') // Iterate through manually scraped text
    {
        char ch = *unFormattedContent; // Save current char
        if (ispunct(ch) || isalnum(ch) || ch == ' ') // Is punctuation or alpha numeric or space, save char
        {
            *text++ = ch;
            *text = '\0';
        }
        else if (ch == '\r') // Is carriage return
        {
            if ((int)*textPtr == 16) // Is Data Link Escape Control Char, track repeats
            {
                if (dleCtrlCounter == none)
                {
                    dleCtrlCounter = once;
                }
                else if (dleCtrlCounter == once)
                {
                    dleCtrlCounter = twice;
                }
            }
            else
            {
                if (dleCtrlCounter == twice) // DLE Char repeats twice right before term
                {
                    printf("Term: ");
                }
                else // Otherwise it's the definition
                {
                    printf("Definition: ");
                }
                dleCtrlCounter = none; // Reset DLE Char repats
                int length = 0;
                for (; *textPtr != '\0'; printf("%c", *textPtr++), length++) // Print text
                    ;
                for (; length > 0; length--, *textPtr--) // Reset pointer to properly free memory
                    ;
                free(textPtr); // Free memory of text
                text = malloc(sizeof(char) * 1024); // Allocate memory for text once again
                textPtr = text; // Point to text start
                printf("\n");
            }
        }
        *unFormattedContent++; // Move on to next character
    }
    return NULL;
}

char *generateFormattedTextDirectoryFile(char **formattedText)
{
    return NULL;
}

int main(int nvar, char **vars)
{
    printf("Welcome to Quizlet Cloner\n"); // Welcome
    if (nvar < 2)                          // Assure file directory entered
    {
        fprintf(stderr, "Must enter file directory after filename %s", vars[0]);
        return 1;
    }
    else
    {
        char *unFormattedContent = getUnFormattedContent(vars[1]);
        char *formattedContent = createFormattedText(unFormattedContent);
        free(unFormattedContent);
    }
    return 0;
}