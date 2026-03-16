#include <stdio.h>
int main(void)
{
    char type_de_client;
    int nombre_de_copies;
    float prix;
    printf("Etes vous un etudiant ou non etudiant ? (e/n) :\n");
    printf("saisissez e pour etudiant et n pour non etudiant ! :\n");
    scanf(" %c", &type_de_client);
    while (type_de_client != 'e' && type_de_client != 'n')
    {
        printf("vous avez commis une erreur, veuillez saisir (e / n) : ");
        scanf(" %c", &type_de_client);
    }

    if (type_de_client == 'e')
    {
        printf("vous etes un etudiant\n\n");
        printf("combien de copies voulez-vous ?\n\n");
        scanf(" %d", &nombre_de_copies);

        if (nombre_de_copies <= 1000)
        {

            prix = nombre_de_copies * 0.25;
            printf("le prix total est de %.2f DH\n", prix);
        }
        else if (nombre_de_copies > 1000)
        {
            prix = nombre_de_copies * 0.20;
            printf("le prix total est de %.2f DH\n", prix);
        }
    }
    else if (type_de_client == 'n')
    {
        printf("vous n'etes pas un etudiant !\n\n");
        printf("combien de copies voulez-vous ?\n\n");
        scanf(" %d", &nombre_de_copies);
        prix = nombre_de_copies * 0.50;
        printf("le prix total est de %.2f DH\n", prix);
    }
    return 0;
}
