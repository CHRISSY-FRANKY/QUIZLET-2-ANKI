#include <stdio.h>
#include <stdlib.h>

char* getUnFormattedContent(char* filename)
{
    FILE* file = fopen(filename, "rb"); // open the file
    if (file == NULL) // Opening file failed
    {
        fprintf(stderr, "Opening file %s FAILED!", filename);
        return NULL;
    }
    fseek(file, 0, SEEK_END); // Set file pointer to the end
    unsigned int size = ftell(file); // Determine file size
    fseek(file, 0, SEEK_SET); // Reset file pointer to the start
    char* unFormattedContent = malloc(sizeof(char) * (size + 1)); // Allocate for file content and null terminator
    if (unFormattedContent == NULL) // Memory allocation failed
    {
        fprintf(stderr, "Allocating memory FAILED!");
        return NULL;
    }
    size_t read_size = fread(unFormattedContent, 1, size, file); // Read file contents
    if (read_size != size) // Didn't read entire file
    {
        fprintf(stderr, "Reading file FAILED!");
        free(unFormattedContent); // no leaks
        fclose(file);
        return NULL;
    }
    unFormattedContent[size] = '\0'; // add the null terminator
    fclose(file);
    return unFormattedContent;
}

char* getFormattedText(char** unFormattedText)
{
    return NULL;
}

char* generateFormattedTextDirectoryFile(char** formattedText)
{
    return NULL;
}

int main(int nvar, char** vars)
{
    printf("Welcome to Quizlet Cloner\n"); // Welcome
    if (nvar < 2) // Assure file directory entered
    {
        fprintf(stderr, "Must enter file directory after filename %s", vars[0]);
        return 1;
    } else {
        char* unFormattedContent = getUnFormattedContent(vars[1]);
    }
    return 0;
}