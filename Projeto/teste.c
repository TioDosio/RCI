#include <stdio.h>
#include <string.h>

int main()
{
    char buffer[] = "01 192.168.1.65 58001\n02 192.168.1.65 58002\n03 192.168.1.65 58003\n04 192.168.1.65 58004\n05 192.168.1.65 58005";
    char *saveptr;
    char *line = strtok_r(buffer, "\n", &saveptr); // Skip the first line
    line = strtok_r(NULL, "\n", &saveptr);         // Start with the second line
    char IDar[100][3];
    char IPar[100][15];
    char Portar[100][6];
    int i = 0;
    while (line != NULL)
    {
        sscanf(line, "%s %s %s", IDar[i], IPar[i], Portar[i]);
        printf("%s\n", line);
        line = strtok_r(NULL, "\n", &saveptr);
        i++;
    }
    int count = i;
    for (size_t i = 0; i < count; i++)
    {
        printf("First arguments: %s %s %s\n", IDar[i], IPar[i], Portar[i]);
    }

    return 0;
}
