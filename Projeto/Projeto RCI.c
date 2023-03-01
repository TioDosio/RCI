#include <stdio.h>

int main(int argc, char *argv[])
{
    switch (argc)
    {
    case 1:
        printf("Sem argumentos\n");
        break;

    case 2:
        char arg = argv[1];
        printf("Argumento: %s\n", arg);
    }
}