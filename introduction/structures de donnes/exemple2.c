#include <stdio.h>
struct heure
{
    int h;
    int m;
};
struct heure ajouter_min(struct heure sh)
{

    sh.m = (sh.m < 59) ? ++sh.m : 0;

    if (sh.m == 0)
    {

        sh.h = (sh.h < 23) ? ++sh.h : 0;
    }

    return sh;
}

int main()
{

    struct heure h1 = {22, 59}, h2;

    h2 = ajouter_min(h1);

    printf("%dh%d", h2.h, h2.m);
}