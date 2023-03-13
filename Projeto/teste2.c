#include <stdio.h>
#include <string.h>

int main()
{
    char buffer[] = "1 2 3\n4 5 6\n7 8 9\n68 69 70";
    char *saveptr;
    char *line = strtok_r(buffer, "\n", &saveptr);
    char IDv[100], IPv[100], Portv[100];
    while (line != NULL)
    {
        sscanf(line, "%s %s %s", IDv, IPv, Portv);
        line = strtok_r(NULL, "\n", &saveptr);
    }
    if (line == NULL)
    {
        printf("%s %s %s", IDv, IPv, Portv);
    }

    return 0;
}
