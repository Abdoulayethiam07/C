#include <stdio.h>
main(void)
{
   int x;
   printf("saisir un nombre entier svp :\n");
   scanf("%d", &x);
   if (x < 0)
   {

      printf("%d est negatif\n",x);
   }
   else if (x > 0)
   {
      printf("le nombre que vous avez saisi est positif\n");
   }
   else
   {
      printf("le nombre que vous avez saisi est nul\n");
   }
}
