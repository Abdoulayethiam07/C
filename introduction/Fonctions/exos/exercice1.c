#include <stdio.h>
int carré(int a)
{
    return a * a;
}
int main()
{
    int x;
    printf("Entrez un nombre : ");
    scanf("%d", &x);
    printf(" %d ^ 2 = %d\n", x, carré(x));
}
