#include <stdio.h>
#include <stdlib.h>

char* getUnFormattedText(char* filename)
{
    
    return NULL;
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

    }
    return 0;
}