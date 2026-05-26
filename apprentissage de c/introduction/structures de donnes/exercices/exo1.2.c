#include <stdio.h>
#include <string.h>

#define NB_PAYS 10
#define NB_VILLES 10

// Structure pour une Ville
typedef struct
{
    char code[4]; // 3 caractères + '\0'
    char nom[50];
    int nbHabitants;
} Ville;

// Structure pour un Pays (contient un tableau de Villes)
typedef struct
{
    char code[3]; // 2 caractères + '\0'
    char nom[50];
    Ville villes[NB_VILLES];
} Pays;

int main()
{
    Pays listePays[NB_PAYS];
    char rechercheCode[3];
    int trouve = 0;

    // --- 1. SAISIE DES DONNÉES ---
    for (int i = 0; i < NB_PAYS; i++)
    {
        printf("\n--- Saisie du Pays n %d ---\n", i + 1);
        printf("Code (2 lettres) : ");
        scanf("%s", listePays[i].code);
        printf("Nom du pays : ");
        scanf("%s", listePays[i].nom);

        for (int j = 0; j < NB_VILLES; j++)
        {
            printf("  Ville %d - Code (3 lettres) : ", j + 1);
            scanf("%s", listePays[i].villes[j].code);
            printf("  Ville %d - Nom : ", j + 1);
            scanf("%s", listePays[i].villes[j].nom);
            printf("  Ville %d - Nb habitants : ", j + 1);
            scanf("%d", &listePays[i].villes[j].nbHabitants);
        }
    }

    // --- 2. AFFICHAGE GLOBAL ---
    printf("\n========================================");
    printf("\nLISTE COMPLETE DES PAYS ET VILLES");
    printf("\n========================================\n");
    for (int i = 0; i < NB_PAYS; i++)
    {
        printf("\nPays : %s (%s)\n", listePays[i].nom, listePays[i].code);
        printf("Villes :\n");
        for (int j = 0; j < NB_VILLES; j++)
        {
            printf("  - [%s] %-15s : %d hab.\n",
                   listePays[i].villes[j].code,
                   listePays[i].villes[j].nom,
                   listePays[i].villes[j].nbHabitants);
        }
    }

    // --- 3. RECHERCHE D'UN PAYS ---
    printf("\n----------------------------------------");
    printf("\nRecherche de pays par code (2 lettres) : ");
    scanf("%s", rechercheCode);

    for (int i = 0; i < NB_PAYS; i++)
    {
        if (strcmp(listePays[i].code, rechercheCode) == 0)
        {
            printf("\nPays Trouve : %s\n", listePays[i].nom);
            printf("Details des villes :\n");
            for (int j = 0; j < NB_VILLES; j++)
            {
                printf("  Code: %s | Nom: %s | Habitants: %d\n",
                       listePays[i].villes[j].code,
                       listePays[i].villes[j].nom,
                       listePays[i].villes[j].nbHabitants);
            }
            trouve = 1;
            break; // On arrête la boucle dès qu'on a trouvé
        }
    }

    if (!trouve)
    {
        printf("\nErreur : Aucun pays trouve avec le code '%s'.\n", rechercheCode);
    }

    return 0;
}