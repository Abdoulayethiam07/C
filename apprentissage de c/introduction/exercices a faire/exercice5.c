#include <stdio.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

int main()
{
    char *mot1 = "Bonjour";
    char *mot2 = "Au revoir";

    // 1. Initialiser le générateur avec le temps actuel
    srand(time(NULL));

    // 2. Générer 0 ou 1
    int choix = rand() % 2;

    // 3. Afficher le mot correspondant
    if (choix == 0)
    {
        printf("%s\n", mot1);
    }
    else
    {
        printf("%s\n", mot2);
    }

    return 0;
}
