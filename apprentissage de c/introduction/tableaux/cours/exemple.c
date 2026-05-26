#include <stdio.h>
void main()
{
    int tab[][3] = {{1, 2, 3}, {4, 5, 6}, {7, 8, 9}};
    for (int i = 0; i < 3; i++)
    {
        for (int j = 0; j < 3; j++)
        {
            printf("%d ", tab[i][j]);
        }
        printf("\n");
    }
}