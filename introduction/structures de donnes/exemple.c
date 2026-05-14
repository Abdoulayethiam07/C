#include <stdio.h>
struct etudiant
{
    int id;
    int age;
    float moyenne;
};
struct etudiant e1;
int main()
{
    printf("Entrer l'id de l'etudiant: ");
    scanf("%d", &e1.id);
    printf("Entrer l'age de l'etudiant: ");
    scanf("%d", &e1.age);
    printf("Entrer la moyenne de l'etudiant: ");
    scanf("%f", &e1.moyenne);
    printf("Les informations de l'etudiant sont: \n");
    printf("ID: %d\n", e1.id);
    printf("Age: %d\n", e1.age);
    printf("Moyenne: %.2f\n", e1.moyenne);
}