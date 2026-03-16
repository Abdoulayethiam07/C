#include <stdio.h>
#include <stdlib.h>
int main(void){
    char nom[20];
    float moyenne;
    char ville[20];

    printf("salut, comment t'appelles tu ?\n");

    scanf("%s", nom);
    printf("bienvenue a notre universite %s !\n", nom);
    printf("avec quelle moyenne as tu obtenu ton bac ?\n");
    scanf("%f", &moyenne);
    printf("dis moi, %s ou habites-tu  ?\n", nom);
    scanf("%s", ville);
    printf("%s est un etudiant de %s\n qui a eu son bac avec une moyenne de %.2f/20", nom, ville, moyenne);
    

}