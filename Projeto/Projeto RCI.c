#include <stdio.h>

int main(int argc, char **argv)
{
    switch (argc)
    {
    case 1:
        printf("Sem argumentos\n");
        break;

    case 2:
        printf("Um argumento\n");
    }
}