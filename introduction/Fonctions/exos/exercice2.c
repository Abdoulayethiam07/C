#include <stdio.h>
int paire(int a)
{
    return a % 2 == 0;
}
int main()
{
    int x;
    printf("entrez un nombre svp :");
    scanf("%d", &x);
    if (paire(x))
        printf("le nombre est paire");
    else
        printf("le nombre est impaire");
    return 0;
}