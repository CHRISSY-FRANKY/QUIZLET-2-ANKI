#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

typedef enum // Custom type to track repeats
{
    none = 0,
    once = 1,
    twice = 2,
    thrice = 3,
    quadrice = 4
} Repeats;

Repeats carriageReturnRepeat = none; // Carriage Return Counter

char *getContent(char *filename) // Load the manually scraped text onto the system
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

char *getFormattedContent(char *unFormattedContent) // Format the loaded text for anki to interpret it correctly
{
    unsigned long long int contentLength = strlen(unFormattedContent) * 1.1;     // get the length of the content and add some trailing space
    char *formattedContent = (char *)malloc(sizeof(char) * contentLength); // allocate memory for the formmated content string
    if (formattedContent == NULL)                                          // Memory allocation failed
    {
        fprintf(stderr, "Allocating memory FAILED!");
        return NULL;
    }
    char *temp = formattedContent;
    while (*unFormattedContent != '\0') // Iterate through manually scraped text
    {
        char ch = *unFormattedContent;               // Save current char
        if (ispunct(ch) || isalnum(ch) || ch == ' ') // Is punctuation or alpha numeric or space, save char
        {
            if (carriageReturnRepeat == twice && *(unFormattedContent + 1) != '\n')
            {
                carriageReturnRepeat = once;
                *temp++ = '<';
                *temp++ = 'b';
                *temp++ = 'r';
                *temp++ = '>';
            }
            *temp++ = *unFormattedContent;
        }
        else if (ch == '\n' && carriageReturnRepeat++ < 1) // Is carriage return
        {
            *temp++ = ';';
        }
        else if (carriageReturnRepeat > 3)
        {
            *temp++ = '\n';
            carriageReturnRepeat = none;
        }
        *unFormattedContent++; // Move on to next character
    }
    *temp = '\0';
    return formattedContent;
}

void createFormattedContentTextFile(char *formattedText)
{
    FILE *file = fopen("formattedContent.txt", "w"); // open a new file
    fputs(formattedText, file);                      // Write the string to the file
    if (fputs(formattedText, file) == EOF)
    {
        perror("Error writing to file");
    }
    else
    {
        printf("Text has been written to formattedContent.txt\n");
    }
    fclose(file); // close the file
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
        char *rawContent = getContent(vars[1]);                   // Get the raw content
        char *formattedContent = getFormattedContent(rawContent); // Formate the raw content for Anki to interpret
        createFormattedContentTextFile(formattedContent);         // Creates the file with the formattedContent
        free(rawContent);                                         // Free the allocated memory to avoid dangling pointer
        free(formattedContent);
        printf("formattedContent.txt has been created! Now import it into Anki!");
    }
    return 0;
}