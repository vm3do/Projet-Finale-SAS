#include <stdio.h>
#include <string.h>
#include <ctype.h>

typedef struct {
    char username[50];
    char password[50];
} User;

int is_special_char(char c) {
    return (c >= 33 && c <= 47) ||  // ! " # $ % & ' ( ) * + , - . /
           (c >= 58 && c <= 64) ||  // : ; < = > ? @
           (c >= 91 && c <= 96) ||  // [ \ ] ^ _ `
           (c >= 123 && c <= 126);  // { | } ~
}

int check_password(char password[], char username[]) {
    int has_upper = 0, has_lower = 0, has_digit = 0, has_special = 0;

    if(strlen(password) < 8 )
     return 0;
    if (strstr(password, username)) {
        printf("le mot de pass ne doit pas contenir le nom d'utilisateur!\n");
        return 0;
    }

    for (int i = 0; password[i]; i++) {
        if (isupper(password[i])) has_upper = 1;
        if (islower(password[i])) has_lower = 1;
        if (isdigit(password[i])) has_digit = 1;
        if (is_special_char(password[i])) has_special = 1;
    }

    return has_upper && has_lower && has_digit && has_special;
}

int sign_in(User user[], int num_user) {
    char username[50], password[50];
    int attempts = 0;
    int max_attempts = 3;

    printf("Entrez le nom d'utilisateur : ");
    scanf("%s", username);

    while (attempts < max_attempts) {
        printf("Entrez le mot de passe : ");
        scanf("%s", password);

        for (int i = 0; i < num_user; i++) {
            if (strcmp(user[i].username, username) == 0 && strcmp(user[i].password, password) == 0) {
                return 1;
            }
        }

        printf("Nom d'utilisateur ou mot de passe incorrect. Reessayez.\n");
        attempts++;
    }

    printf("Compte verrouille en raison de trop nombreuses tentatives de connexion echouees.\n");
    return 0;
}

void signup() {
    User user[100];
    int num_user = 0;
    char username[50], password[50];
    char choice;
    int max_attempts = 3;

    while (1) {
        printf("\n--- Menu ---\n");
        printf("1. S'inscrire\n2. Se connecter\n3. Quitter\n");
        printf("Entrez votre choix : ");
        scanf(" %c", &choice);

        if (choice == '1') {
            printf("\n--- S'inscrire ---\n");
            printf("Entrez le nom d'utilisateur : ");
            scanf("%s", username);

            do {
                printf("Entrez le mot de passe : ");
                scanf("%s", password);

                if (check_password(password, username)) {
                    strcpy(user[num_user].username, username);
                    strcpy(user[num_user].password, password);
                    num_user++;
                    printf("Utilisateur enregistre avec succes !\n");
                    break;
                } else {
                    printf("Le mot de passe doit contenir au moins : \nUne lettre majuscule, Une lettre minuscule, Un chiffre, Un caractere special \n");
                }
            } while (1);

        } else if (choice == '2') {

            printf("\n--- Se connecter ---\n");
            if (sign_in(user, num_user)) {
                printf("Connexion reussie !\n");
            } else {
                printf("Echec de la connexion !\n");
            }
        } else if (choice == '3') {
            break;
        } else {
            printf("Choix invalide !\n");
        }
    }
}

int main() {
    signup();
    return 0;
}