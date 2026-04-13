#include <stdio.h>
int somme(int a, int b)
{
    return a + b;
}
int diff(int a, int b)
{
    return a - b;
}
int multiplication(int a, int b)
{
    return a * b;
}
int div(int a, int b)
{

    return a / b;
}
int main()
{
    int x, y;
    printf("entrez deux nombres svp :");
    scanf("%d%d", &x, &y);
    printf("la somme de %d et %d est : %d\n", x, y, somme(x, y));
    printf("la difference de %d et %d est : %d\n", x, y, diff(x, y));
    printf("le produit de %d et %d est : %d\n", x, y, multiplication(x, y));
    printf("le quotient de %d et %d est : %d\n", x, y, div(x, y));
    return 0;
}