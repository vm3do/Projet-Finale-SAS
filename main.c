#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <time.h>
#include <unistd.h>

struct User {
    char username[50];
    char password[50];
    int role; // 0 = Client, 1 = Agent de Reclamation, 2 = Administrateur
};

struct Complaint {
    int id;
    char motive[100];
    char description[256];
    char category[50];
    char status[20]; // "en cours", "resolu", "rejete"
    char date[11];
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

// Fonction pour afficher le menu selon le role
void afficher_menu(int role) {
    if (role == 2) { // Admin
        printf("\n--- Menu Administrateur ---\n");
        printf("1. Gerer les utilisateurs\n");
        printf("2. Gerer les reclamations\n");
        printf("3. Generer des statistiques\n");
    } else if (role == 1) { // Agent de Reclamation
        printf("\n--- Menu Agent de Reclamation ---\n");
        printf("1. Traiter les reclamations\n");
        printf("2. Afficher les reclamations\n");
    } else { // Client
        printf("\n--- Menu Client ---\n");
        printf("1. Soumettre une reclamation\n");
        printf("2. Consulter mes reclamations\n");
    }
}

void signup() {
    char username[50], password[50];
    int valid = 0;

    printf("\n--- S'inscrire ---\n");
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
                users[user_count].role = 0; // default role is Client
                user_count++;
                printf("Utilisateur enregistre avec succes !\n");
            } else {
                printf("Limite d'utilisateurs atteinte.\n");
                return;
            }
        } else {
            printf("-le mot de passe doit contenir au moins 8 caracteres, une majuscule, une minuscule, un chiffre et un caractère special.\n");
            printf("-il ne doit pas contenir le nom d'utilisateur.\n");
        }
    }
}

int connexion() {
    char username[50], password[50];
    int attempts = 0;

    while (attempts < 3) {
        printf("Nom d'utilisateur : ");
        scanf("%s", username);
        printf("Mot de passe : ");
        scanf("%s", password);

        // Vérifier si c'est l'administrateur
        if (strcmp(username, "admin") == 0 && strcmp(password, "admin") == 0) {
            return 2; // role admin
        }
        
        // Verifier les utilisateurs enregistres
        for (int i = 0; i < user_count; i++) {
            if (strcmp(users[i].username, username) == 0 && strcmp(users[i].password, password) == 0) {
                return users[i].role; // Retourner le role de l'utilisateur
            }
        }

        attempts++;
        printf("Nom d'utilisateur ou mot de passe incorrect. Tentatives restantes : %d\n", 3 - attempts);

        if (attempts == 3) {
            printf("Trop de tentatives echouees. Veuillez patienter 30 min...\n");
            sleep(5); // pause de 5 secondes
            attempts = 0; // attempts yt3awdo lmera jaya :D
            break;
        }
    }

    return 0;
}

void add_complaint() {
    if (complaint_count >= 100) {
        printf("Limite de reclamations atteinte.\n");
        return;
    }

    struct Complaint new_complaint;
    new_complaint.id = complaint_count + 1; // generation simple
    printf("Entrez le motif de la reclamation : ");
    scanf(" %[^\n]", new_complaint.motive);
    printf("Entrez la description : ");
    scanf(" %[^\n]", new_complaint.description);
    printf("Entrez la categorie : ");
    scanf(" %[^\n]", new_complaint.category);
    strcpy(new_complaint.status, "en cours"); // Default status
    // Get current date
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);
    sprintf(new_complaint.date, "%04d-%02d-%02d", tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday);

    complaints[complaint_count++] = new_complaint; // Store the complaint
    printf("Reclamation soumise avec succes !\n");
}

void view_complaints() {
    printf("\n--- Liste des reclamations ---\n");
    for (int i = 0; i < complaint_count; i++) {
        printf("ID: %d, Motif: %s, Description: %s, Categorie: %s, Statut: %s, Date: %s\n",
               complaints[i].id, complaints[i].motive, complaints[i].description,
               complaints[i].category, complaints[i].status, complaints[i].date);
    }
}

void promote_client() {
    char username[50];
    printf("Entrez le nom d'utilisateur du client a promouvoir : ");
    scanf("%s", username);

    for (int i = 0; i < user_count; i++) {
        if (strcmp(users[i].username, username) == 0 && users[i].role == 0) {
            users[i].role = 1; // promote to agent de reclamation :D
            printf("Utilisateur %s promu a Agent de Reclamation.\n", username);
            return;
        }
    }
    printf("Utilisateur non trouve ou deja promu.\n");
}

void admin_menu() {
    char choice;
    while (1) {
        printf("\n--- Menu Administrateur ---\n");
        printf("1. Promouvoir un utilisateur à Agent de Reclamation\n");
        printf("2. Gerer les Reclamations\n");
        printf("3. Quitter\n");
        printf("Entrez votre choix : ");
        scanf(" %c", &choice);

        if (choice == '1') {
            promote_client();
        } else if (choice == '2') {
            // mazal madrtha :D
        } else if (choice == '3') {
            break;
        } else {
            printf("Choix invalide !\n");
        }
    }
}

int main() {
    char choice;

    while (1) {
        printf("\n--- Menu ---\n");
        printf("1. S'inscrire\n2. Se connecter\n3. Quitter\n");
        printf("Entrez votre choix : ");
        scanf(" %c", &choice);

        if (choice == '1') {
            signup();
        } else if (choice == '2') {
            int login_result = connexion();
            if (login_result == 1) {
                printf("Connexion reussie en tant qu'utilisateur !\n");
            } else if (login_result == 2) {
                printf("Connexion reussie en tant qu'administrateur !\n");
                admin_menu();
            }
        } else if (choice == '3') {
            break;
        } else {
            printf("Choix invalide !\n");
        }
    }

    return 0;
}