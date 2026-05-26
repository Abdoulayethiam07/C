#include <stdio.h>
#include <string.h>

int main()
{
    char mot[100];
    printf("Entrez un mot : ");
    scanf("%s", mot);

    int longueur = strlen(mot);

    printf("Le mot '%s' contient %d caracteres.\n", mot, longueur);
    return 0;
}
