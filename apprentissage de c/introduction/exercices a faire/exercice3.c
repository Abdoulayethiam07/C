
#include <stdio.h>
#include <string.h> // Nécessaire pour strcmp

int main()
{
    char mot1[50], mot2[50];
    printf("Entrez le premier mot : ");
    scanf("%s", mot1);
    printf("Entrez le deuxieme mot : ");
    scanf("%s", mot2);

    // La structure cmp (comparaison)
    if (strcmp(mot1, mot2) == 0)
    {
        printf("Les mots sont identiques.\n");
    }
    else if (strcmp(mot1, mot2) < 0)
    {
        printf("%s est avant %s.\n", mot1, mot2);
    }
    else
    {
        printf("%s est apres %s.\n", mot1, mot2);
    }
    return 0;
}
