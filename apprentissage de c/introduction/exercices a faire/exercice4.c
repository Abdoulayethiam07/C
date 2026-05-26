#include <stdio.h>
#include <ctype.h> // Bibliothèque nécessaire pour toupper
#include <string.h>

int main()
{
    char mot1[50], mot2[50];
    int i;

    printf("Entrez deux mots (séparés par un espace) : ");
    scanf("%49s %49s", mot1, mot2);

    // Conversion du premier mot
    for (i = 0; mot1[i] != '\0'; i++)
    {
        mot1[i] = toupper(mot1[i]);
    }

    // Conversion du deuxième mot
    for (i = 0; mot2[i] != '\0'; i++)
    {
        mot2[i] = toupper(mot2[i]);
    }

    printf("Mots en majuscules : %s %s\n", mot1, mot2);

    return 0;
}
