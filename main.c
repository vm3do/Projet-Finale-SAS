#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <time.h>
#include <unistd.h>

struct User {
    char username[50];
    char password[50];
    int role; // 0 = Client, 1 = Agent , 2 = Admin
};

struct Complaint {
    int id;
    char username[50];
    char motive[100];
    char description[256];
    char category[50];
    char status[20]; // "en cours", "resolu", "rejete"
    char date[11];
    char priorite[10];
    time_t submission_time;
};

struct User users[100];
struct Complaint complaints[100];
int user_count = 0;
int complaint_count = 0;

int special_char(char c) {
    return (c >= 33 && c <= 47) ||
           (c >= 58 && c <= 64) ||
           (c >= 91 && c <= 96) ||
           (c >= 123 && c <= 126);
}

int check_password(const char *password, const char *username) {
    int majuscule = 0, minus = 0, digit = 0, has_special = 0;
    int length = strlen(password);
    
    if (length < 8) return 0;

    for (int i = 0; i < length; i++) {
        if (isupper(password[i])) majuscule = 1;
        if (islower(password[i])) minus = 1;
        if (isdigit(password[i])) digit = 1;
        if (special_char(password[i])) has_special = 1;
    }

    if (strstr(password, username) != NULL) return 0;

    return majuscule && minus && digit && has_special;
}

void signup() {
    char username[50], password[50];
    int valid = 0;

    printf("--- S'inscrire ---\n");
    printf("Entrez le nom d'utilisateur : ");
    scanf("%s", username);

    while(!valid) {
        printf("Entrez le mot de passe : ");
        scanf("%s", password);

        if (check_password(password, username)) {
            if (user_count < 100) {
                valid = 1;
                strcpy(users[user_count].username, username);
                strcpy(users[user_count].password, password);
                users[user_count].role = 0; // default dima client
                user_count++;
                printf("Utilisateur enregistre avec succes !\n");
                printf("----------------------------------------------------------\n");
            } else {
                printf("Limite d'utilisateurs atteinte.\n");
                printf("----------------------------------------------------------\n");
                return;
            }
        } else {
            printf("-le mot de passe doit contenir au moins 8 caracteres, une majuscule, une minuscule, un chiffre et un caractere special.\n");
            printf("-il ne doit pas contenir le nom d'utilisateur.\n");
            printf("----------------------------------------------------------\n");
        }
    }
}

int connexion(char *username_out) {
    char username[50], password[50];
    int attempts = 0;

    while (attempts < 3) {
        printf("Nom d'utilisateur : ");
        scanf("%s", username);
        printf("Mot de passe : ");
        scanf("%s", password);

        // Verifier si c'est l'administrateur
        if (strcmp(username, "admin") == 0 && strcmp(password, "admin") == 0) {
            strcpy(username_out, username);
            return 2; // admin
        }
        
        // verifier les utilisateurs enregistres
        for (int i = 0; i < user_count; i++) {
            if (strcmp(users[i].username, username) == 0 && strcmp(users[i].password, password) == 0) {
                strcpy(username_out, username);  // store username
                return users[i].role; // return the user's role
            }
        }

        attempts++;
        printf("Nom d'utilisateur ou mot de passe incorrect. Tentatives restantes : %d\n", 3 - attempts);

        if (attempts == 3) {
            printf("Trop de tentatives echouees. Veuillez patienter 30 min...\n");
            sleep(5); // pause de 5 secondes wla li bghit
            attempts = 0;
            break;
        }
    }

    return 4;
}

void set_priority(struct Complaint *complaint) {
    
    char motive_lower[100], description_lower[256];
    strcpy(motive_lower, complaint->motive);    // kanrdohom lowercase bach tshal mo9arana
    strcpy(description_lower, complaint->description);

    for (int i = 0; motive_lower[i]; i++) {
        motive_lower[i] = tolower(motive_lower[i]);
    }
    for (int i = 0; description_lower[i]; i++) {
        description_lower[i] = tolower(description_lower[i]);
    }

    if (strstr(motive_lower, "urgent") || strstr(description_lower, "urgent") ||
        strstr(motive_lower, "immediat") || strstr(description_lower, "immediat") ||
        strstr(motive_lower, "danger") || strstr(description_lower, "danger") ||
        strstr(motive_lower, "important") || strstr(description_lower, "important")) {
        strcpy(complaint->priorite, "haute");
    } else if (strstr(motive_lower, "probleme") || strstr(description_lower, "probleme") ||
             strstr(motive_lower, "souci") || strstr(description_lower, "souci") ||
             strstr(motive_lower, "inconvenient") || strstr(description_lower, "inconvenient")) {
        strcpy(complaint->priorite, "moyenne");
    } else { ///// default katkon basse
        strcpy(complaint->priorite, "basse");
    }
}

void add_complaint(const char* username) {
    if (complaint_count >= 100) {
        printf("Limite de reclamations atteinte.\n");
        printf("----------------------------------------------------------\n");
        return;
    }

    struct Complaint new_complaint;
    new_complaint.id = complaint_count + 1;
    strcpy(new_complaint.username, username);  // stores the client's username

    printf("Entrez le motif de la reclamation : ");
    scanf(" %[^\n]", new_complaint.motive);
    
    printf("Entrez la description : ");
    scanf(" %[^\n]", new_complaint.description);
    
    printf("Entrez la categorie :\n");
    printf("------------------------\n");
    printf(" - Service Client\n - Livraison\n - Produit Defectueux\n - Facturation\n - Qualite du Produit\n - Autre(preciser)\n");
    printf("------------------------\n");
    printf("Entrez votre choix : ");
    scanf(" %[^\n]", new_complaint.category);

    strcpy(new_complaint.status, "en cours");  // default status

    set_priority(&new_complaint); // kat3ti priority

    time_t t = time(NULL);
    struct tm tm = *localtime(&t);
    sprintf(new_complaint.date, "%04d-%02d-%02d", tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday);
    new_complaint.submission_time = t;

    complaints[complaint_count++] = new_complaint;
    printf("Reclamation soumise avec succes !\n");
    printf("----------------------------------------------------------\n");
}

void promote_client() {
    char username[50];
    printf("Entrez le nom d'utilisateur du client a promouvoir : ");
    scanf("%s", username);

    for (int i = 0; i < user_count; i++) {
        if (strcmp(users[i].username, username) == 0 && users[i].role == 0) {
            users[i].role = 1; // promote to agent de reclamation :D
            printf("Utilisateur %s promu a Agent de Reclamation.\n", username);
            printf("----------------------------------------------------------\n");
            return;
        }
    }
    printf("Utilisateur non trouve ou deja promu.\n");
    printf("----------------------------------------------------------\n");
}

void afficher_toutes_reclamations() {
    if (complaint_count == 0) {
        printf("Aucune reclamation disponible.\n");
        printf("----------------------------------------------------------\n");
        return;
    }
    printf("-------------------------------------------------------------------------\n");
    printf("--- Liste de toutes les reclamations ---\n");
    printf("-------------------------------------------------------------------------\n");
    for (int i = 0; i < complaint_count; i++) {
        printf("| ID: %d | Motif: %s | Description: %s | Categorie: %s | Statut: %s | Date: %s |\n",
               complaints[i].id, complaints[i].motive, complaints[i].description,
               complaints[i].category, complaints[i].status, complaints[i].date);
        printf("-------------------------------------------------------------------------\n");   
    }
}

void modifier_supprimer_reclamation(int role, const char* utilisateur) {
    int id, choix;
    printf("----------------------------------------------------------\n");
    printf("Entrez l'ID de la reclamation a modifier ou supprimer: ");
    scanf("%d", &id);

    for (int i = 0; i < complaint_count; i++) {
        if (complaints[i].id == id) {
            if (role == 2 || role == 1 || 
                (role == 0 && strcmp(complaints[i].username, utilisateur) == 0)) {

                time_t now = time(NULL);
                double time_diff = difftime(now, complaints[i].submission_time);

                if (role == 2 || role == 1 || 
                    (role == 0 && time_diff <= 86400 && strcmp(complaints[i].status, "en cours") == 0)) {

                    printf("Voulez-vous (1) Modifier ou (2) Supprimer la reclamation? ");
                    scanf("%d", &choix);

                    if (choix == 1) {
                        // ymodifier
                        printf("Entrez le nouveau motif: ");
                        scanf(" %[^\n]", complaints[i].motive);

                        printf("Entrez la nouvelle description: ");
                        scanf(" %[^\n]", complaints[i].description);

                        printf("Entrez la nouvelle categorie: ");
                        printf("------------------------\n");
                        printf(" - Service Client\n - Livraison\n - Produit Defectueux\n - Facturation\n - Qualite du Produit\n - Autre(preciser)\n");
                        printf("------------------------\n");
                        printf("Entrez votre choix : ");
                        scanf(" %[^\n]", complaints[i].category);

                        printf("Reclamation modifiee avec succes.\n");
                        printf("----------------------------------------------------------\n");
                    } else if (choix == 2) {
                        // ymse7
                        for (int j = i; j < complaint_count - 1; j++) {
                            complaints[j] = complaints[j + 1];
                        }
                        complaint_count--;
                        printf("Reclamation supprimee avec succes.\n");
                        printf("----------------------------------------------------------\n");
                    } else {
                        printf("Choix invalide.\n");
                        printf("----------------------------------------------------------\n");
                    }
                } else {
                    printf("Vous ne pouvez pas modifier ou supprimer cette reclamation apres 24 heures ou si elle est deja traitee.\n");
                    printf("-----------------------------------------------------------------------------------------------------------\n");
                }
            } else {
                printf("Vous n'avez pas les droits pour modifier ou supprimer cette reclamation.\n");
                printf("----------------------------------------------------------------------------\n");
            }
            return;
        }
    }
    printf("Reclamation non trouvee.\n");
    printf("----------------------------------------------------------\n");
}

void traiter_reclamation(int id, int role) {
    for (int i = 0; i < complaint_count; i++) {
        if (complaints[i].id == id) {
            
            if (role == 2 || role == 1) {
                printf("----------------------------------------------------------\n");
                printf("Statut actuel: %s\n", complaints[i].status);
                printf("Choisissez un nouveau statut:\n");
                printf("1. En cours\n2. Resolue\n3. Fermee\n");
                printf("Entrez votre choix : ");
                int choix_statut;
                scanf("%d", &choix_statut);

                if (choix_statut == 1) {
                    strcpy(complaints[i].status, "en cours");
                } else if (choix_statut == 2) {
                    strcpy(complaints[i].status, "resolue");
                } else if (choix_statut == 3) {
                    strcpy(complaints[i].status, "fermee");
                } else {
                    printf("Choix invalide !\n");
                    return;
                }

                printf("Voulez-vous ajouter une note interne ? (o/n) : "); // optional
                char note_choix;
                scanf(" %c", &note_choix);
                if (note_choix == 'o') {
                    char notes[256];
                    printf("Entrez vos notes : ");
                    scanf(" %[^\n]", notes);
                    printf("Note ajoutee avec succes : %s\n", notes); // Just displaying the note for now
                    printf("----------------------------------------------------------\n");
                }

                printf("Reclamation mise a jour avec succes.\n");
                printf("----------------------------------------------------------\n");
            } else {
                printf("Vous n'avez pas les droits pour traiter cette reclamation.\n");
                printf("----------------------------------------------------------\n");
            }
            return;
        }
    }
    printf("Reclamation non trouvee.\n");
    printf("----------------------------------------------------------\n");
}

void rechercher_reclamation() {
    int choix;
    printf("--- Rechercher une Reclamation ---\n");
    printf("1. Par ID\n");
    printf("2. Par nom de client\n");
    printf("3. Par date de soumission\n");
    printf("4. Par statut\n");
    printf("5. Par categorie\n");
    printf("----------------------------------------------------------\n");
    printf("Entrez votre choix : ");
    scanf("%d", &choix);
    printf("----------------------------------------------------------\n");

    if (choix == 1) {
        int id;
        printf("Entrez l'ID de la reclamation : ");
        scanf("%d", &id);
        for (int i = 0; i < complaint_count; i++) {
            if (complaints[i].id == id) {
                printf("| ID: %d | Motif: %s | Description: %s | Categorie: %s | Statut: %s | Date: %s |\n",
                    complaints[i].id, complaints[i].motive, complaints[i].description,
                    complaints[i].category, complaints[i].status, complaints[i].date);
                printf("-----------------------------------------------------------------------\n");
                return;
            }
        }
        printf("Reclamation non trouvee.\n");
        printf("----------------------------------------------------------\n");
    } else if (choix == 2) {
        char client[50];
        printf("Entrez le nom du client : ");
        scanf("%s", client);
        for (int i = 0; i < complaint_count; i++) {
            if (strcmp(users[i].username, client) == 0) {
                printf("| ID: %d | Motif: %s | Description: %s | Categorie: %s | Statut: %s | Date: %s |\n",
                    complaints[i].id, complaints[i].motive, complaints[i].description,
                    complaints[i].category, complaints[i].status, complaints[i].date);
                printf("-----------------------------------------------------------------------\n");
            }
        }
    } else if (choix == 3) {
        char date[11];
        printf("Entrez la date de soumission (YYYY-MM-DD) : ");
        scanf("%s", date);
        for (int i = 0; i < complaint_count; i++) {
            if (strcmp(complaints[i].date, date) == 0) {
                printf("| ID: %d | Motif: %s | Description: %s | Categorie: %s | Statut: %s | Date: %s |\n",
                    complaints[i].id, complaints[i].motive, complaints[i].description,
                    complaints[i].category, complaints[i].status, complaints[i].date);
                printf("-----------------------------------------------------------------------\n");
            }
        }
    } else if (choix == 4) {
        char statut[20];
        printf("Entrez le statut (en attente/en cours/resolue/fermee) : ");
        scanf("%s", statut);
        for (int i = 0; i < complaint_count; i++) {
            if (strcmp(complaints[i].status, statut) == 0) {
                printf("| ID: %d | Motif: %s | Description: %s | Categorie: %s | Statut: %s | Date: %s |\n",
                    complaints[i].id, complaints[i].motive, complaints[i].description,
                    complaints[i].category, complaints[i].status, complaints[i].date);
                printf("-----------------------------------------------------------------------\n");
            }
        }
    } else if (choix == 5) {
        char categorie[50];
        printf("Entrez la categorie : ");
        scanf("%s", categorie);
        for (int i = 0; i < complaint_count; i++) {
            if (strcmp(complaints[i].category, categorie) == 0) {
                printf("| ID: %d | Motif: %s | Description: %s | Categorie: %s | Statut: %s | Date: %s |\n",
                    complaints[i].id, complaints[i].motive, complaints[i].description,
                    complaints[i].category, complaints[i].status, complaints[i].date);
                printf("-----------------------------------------------------------------------\n");
            }
        }
    } else {
        printf("Choix invalide !\n");
        printf("----------------------------------------------------------\n");
    }
}

void afficher_total_reclamations() {
    printf("Nombre total de reclamations: %d\n", complaint_count);
    printf("----------------------------------------------------------\n");
}

void afficher_taux_resolution() {
    int resolved_count = 0;

    for (int i = 0; i < complaint_count; i++) {
        if (strcmp(complaints[i].status, "resolue") == 0) {
            resolved_count++;
        }
    }

    if (complaint_count > 0) {
        float taux = (float)resolved_count / complaint_count * 100;
        printf("Taux de resolution des reclamations: %.2f%%\n", taux);
    } else {
        printf("Aucune reclamation soumise.\n");
        printf("----------------------------------------------------------\n");
    }
}

void afficher_duree_moyenne_traitement() {
    int total_time = 0;
    int resolved_count = 0;

    for (int i = 0; i < complaint_count; i++) {
        if (strcmp(complaints[i].status, "resolue") == 0) {
            time_t current_time = time(NULL);
            total_time += difftime(current_time, complaints[i].submission_time);
            resolved_count++;
        }
    }

    if (resolved_count > 0) {
        float average_time = (float)total_time / resolved_count;
        printf("Delai moyen de traitement des reclamations: %.2f secondes\n", average_time);
        printf("----------------------------------------------------------\n");
    } else {
        printf("Aucune reclamation resolue.\n");
        printf("----------------------------------------------------------\n");
    }
}

void afficher_rapport_journalier() {
    // date dyal lyom
    time_t current_time = time(NULL);
    struct tm *tm_info = localtime(&current_time);
    char date[11];
    sprintf(date, "%04d-%02d-%02d", tm_info->tm_year + 1900, tm_info->tm_mon + 1, tm_info->tm_mday);

    // generate file b smiya unique dyal kola yom
    char filename[50];
    sprintf(filename, "rapport_journalier_%s.txt", date);

    FILE *file = fopen(filename, "w");
    if (file == NULL) {
        printf("Erreur lors de la création du fichier.\n");
        return;
    }

    fprintf(file, "Rapport Journalier - %s\n\n", date);
    fprintf(file, "--- Reclamations Nouvelles ---\n");

    // Loop through complaints and add new complaints (submitted today) to the report
    for (int i = 0; i < complaint_count; i++) {
        if (strcmp(complaints[i].date, date) == 0) {
            fprintf(file, "ID: %d, Motif: %s, Description: %s, Statut: %s, Date: %s\n",
                    complaints[i].id, complaints[i].motive, complaints[i].description,
                    complaints[i].status, complaints[i].date);
        }
    }

    fprintf(file, "\n--- Reclamations Resolues ---\n");

    for (int i = 0; i < complaint_count; i++) {
        if (strcmp(complaints[i].status, "resolue") == 0) {
            fprintf(file, "ID: %d, Motif: %s, Description: %s, Date: %s\n",
                    complaints[i].id, complaints[i].motive, complaints[i].description,
                    complaints[i].date);
        }
    }

    fclose(file);

    printf("Rapport journalier généré avec succès dans '%s'.\n", filename);
    printf("----------------------------------------------------------\n");
}

void statistiques_rapports(){
    char choix;

    while(1){
        printf("--- Menu Statistiques et Rapports ---\n");
        printf("1. Afficher le nombre total de reclamations\n");
        printf("2. Afficher le taux de resolution des reclamations\n");
        printf("3. Afficher le delai moyen de traitement des reclamations \n");
        printf("4. Afficher un rapport journalier\n");
        printf("5. Retour au menu principal\n");
        printf("----------------------------------------------------------\n");
        scanf(" %c", &choix);
        printf("----------------------------------------------------------\n");
        switch (choix) {
            case '1':
                afficher_total_reclamations();
            break;
            case '2':
                afficher_taux_resolution();
            break;
            case '3':
                afficher_duree_moyenne_traitement();
            break;
            case '4':
                afficher_rapport_journalier();
            break;
            case '5':
                return;
            break;
            default:
                printf("Choix invalide.\n");
                break;
        }
    }
    return;
}

void afficher_reclamations_client(const char* utilisateur) {
    printf("Vos reclamations:\n");
    int found = 0;

    for (int i = 0; i < complaint_count; i++) {

        if (strcmp(complaints[i].username, utilisateur) == 0) {  // y9aren lusernames :D
            printf("| ID: %d | Motif: %s | Description: %s | Categorie: %s | Statut: %s | Date: %s |\n",
                   complaints[i].id, complaints[i].motive, complaints[i].description, 
                   complaints[i].category, complaints[i].status, complaints[i].date);
            found = 1;
        }
    }

    if (!found) {
        printf("Aucune reclamation trouvee pour cet utilisateur.\n");
    }
}

void afficher_reclamations_par_priorite() {
    printf("----------------------------------------------------------\n");
    printf("--- Reclamations par priorite ---\n");
    
    for (int i = 0; i < complaint_count; i++) {
        if (strcmp(complaints[i].priorite, "haute") == 0) {
            printf("ID: %d | Motif: %s | Description: %s | Priorite: %s | Statut: %s | Date: %s\n",
                   complaints[i].id, complaints[i].motive, complaints[i].description, 
                   complaints[i].priorite, complaints[i].status, complaints[i].date);
        }
    }
    
    for (int i = 0; i < complaint_count; i++) {
        if (strcmp(complaints[i].priorite, "moyenne") == 0) {
            printf("ID: %d | Motif: %s | Description: %s | Priorite: %s | Statut: %s | Date: %s\n",
                   complaints[i].id, complaints[i].motive, complaints[i].description, 
                   complaints[i].priorite, complaints[i].status, complaints[i].date);
        }
    }
    
    for (int i = 0; i < complaint_count; i++) {
        if (strcmp(complaints[i].priorite, "basse") == 0) {
            printf("ID: %d | Motif: %s | Description: %s | Priorite: %s | Statut: %s | Date: %s\n",
                   complaints[i].id, complaints[i].motive, complaints[i].description, 
                   complaints[i].priorite, complaints[i].status, complaints[i].date);
        }
    }
}

void admin_menu() {
    char choice;
    int id;

    while (1) {
        printf("--- Menu Administrateur ---\n");
        printf("1. Promouvoir un utilisateur a Agent de Reclamation\n");
        printf("2. Afficher la liste des reclamations\n");
        printf("3. Modifier ou supprimer une reclamation\n");
        printf("4. Traiter une reclamation\n");
        printf("5. Rechercher une reclamation\n");
        printf("6. Afficher les reclamations par priorite\n");
        printf("7. Afficher statistiques et rapports\n");
        printf("8. Se deconnecter\n");
        printf("----------------------------------------------------------\n");
        printf("Entrez votre choix : ");
        scanf(" %c", &choice);
        printf("----------------------------------------------------------\n");

        switch (choice) {
            case '1':
                promote_client();
                break;
            case '2':
                afficher_toutes_reclamations();
                break;
            case '3':
                modifier_supprimer_reclamation(1, "");
                break;
            case '4':
                printf("Entrez l'ID de la reclamation a traiter : ");
                scanf("%d", &id);
                traiter_reclamation(id, 2);
                break;
            case '5':
                rechercher_reclamation();
                break;
            case '6':
                afficher_reclamations_par_priorite();
                break;
            case '7':
                statistiques_rapports();
                break;
            case '8':
                return;
            default:
                printf("Choix invalide.\n");
                break;
        }
    }
}

void agent_menu() {
    char choice;
    int id;
    while (1) {
        printf("----------------------------------------------------------\n");
        printf("--- Menu Agent de Reclamation ---\n");
        printf("1. Afficher la liste des reclamations\n");
        printf("2. Modifier ou supprimer une reclamation\n");
        printf("3. Traiter une reclamation\n");
        printf("4. Rechercher une reclamation\n");
        printf("5. Se deconnecter\n");
        printf("----------------------------------------------------------\n");
        printf("Entrez votre choix : ");
        scanf(" %c", &choice);
        printf("----------------------------------------------------------\n");

        switch (choice) {
            case '1':
                afficher_toutes_reclamations();
                break;
            case '2':
                modifier_supprimer_reclamation(1, "");
                break;
            case '3':
                printf("Entrez l'ID de la reclamation a traiter : ");
                scanf("%d", &id);
                traiter_reclamation(id, 1);
                break;
            case '4':
                rechercher_reclamation();
                break;
            case '5':
                return;
            default:
                printf("Choix invalide.\n");
                break;
        }
    }
}

void client_menu(const char* username) {
    char choice;
    while (1) {
        printf("----------------------------------------------------------\n");
        printf("--- Menu Client ---\n");
        printf("1. Soumettre une reclamation\n");
        printf("2. Consulter mes reclamations\n");
        printf("3. Modifier ou supprimer ma reclamation\n");
        printf("4. Se deconnecter\n");
        printf("----------------------------------------------------------\n");
        printf("Entrez votre choix : ");
        scanf(" %c", &choice);
        printf("----------------------------------------------------------\n");

        switch (choice) {
            case '1':
                add_complaint(username);
                break;
            case '2':
                afficher_reclamations_client(username);
                break;
            case '3':
                modifier_supprimer_reclamation(0, username); // 0 dyal client
                break;
            case '4':
                return;  // Exit the menu
            default:
                printf("Choix invalide.\n");
                printf("----------------------------------------------------------\n");
                break;
        }
    }
}

int main() {
    char choice;
    char username[50];

    while (1) {
        printf("----------------------------------------------------------\n");
        printf("--- Menu ---\n");
        printf("1. S'inscrire\n2. Se connecter\n3. Quitter\n");
        printf("----------------------------------------------------------\n");
        printf("Entrez votre choix : ");
        scanf(" %c", &choice);
        printf("----------------------------------------------------------\n");

        if (choice == '1') {
            signup();
        } else if (choice == '2') {
            int role_check = connexion(username);

            if (role_check == 2) {
                printf("----------------------------------------------------------\n");
                printf("Connexion reussie en tant qu'administrateur !\n");
                printf("----------------------------------------------------------\n");
                admin_menu();
            } else if (role_check == 1) {
                printf("----------------------------------------------------------\n");
                printf("Connexion reussie en tant qu'agent de reclamation !\n");
                printf("----------------------------------------------------------\n");
                agent_menu();
            } else if (role_check == 0) {
                printf("----------------------------------------------------------\n");
                printf("Connexion reussie en tant que client !\n");
                printf("----------------------------------------------------------\n");
                client_menu(username);
            } else {

                printf("echec de la connexion !\n");
            }
        } else if (choice == '3') {
            break;
        } else {
            printf("Choix invalide !\n");
        }
    }

    return 0;
}