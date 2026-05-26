#include <stdio.h>
void main()
{
    int somme = 0;
    int i;

    for (i = 0; i < 10; i++)
    {
        printf("entrez un nombre svp ! \n");
        int nombre;
        scanf("%d", &nombre);
        somme += nombre;

        printf("la somme de ces  10 nombres est : %d\n", somme);
    }
}