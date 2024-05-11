#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <time.h>

void error(const char *msg){
    perror(msg);
    exit(1);
}

struct client{
    char id[50];
    char mdp[50];
    char num[10][50];
    int solde[10];
    char operations[10][10][50];
};

int main(){// Déclaration des variables
    struct sockaddr_in sin = { 0 };
    struct sockaddr_in from = { 0 };
    char buffer[255];
    unsigned int fromsize = sizeof(from);
    struct client c1 = {"Paul\n", "AH\n", {"111\n","222\n","333\n"}, {2000,3000,4000},{{"12 mars 2023 : solde de 200€\n","24 avril 2023 : crédit de 1000€\n","31 mai 2023 : solde de 1300€\n"},{"23 août 2019 : solde de 34000€\n","2 septembre 2020 : crédit de 100€\n","11 juillet 2021 : solde de 3,29\n"}}};
    struct client c2 = {"Chaima\n", "bababanks\n", {"1234\n","0987\n","4567\n"}, {2000,3000,4000},{{"12 mars 2023 : solde de 200€\n","24 avril 2023 : crédit de 1000€\n","31 mai 2023 : solde de 1300€\n"},{"23 août 2019 : solde de 34000€\n","2 septembre 2020 : crédit de 100€\n","11 juillet 2021 : solde de 3,29\n"}}};
    struct client c3 = {"Matteo\n", "kha\n", {"0000\n","999\n","111\n"}, {2000,3000,4000},{{"12 mars 2023 : solde de 200€\n","24 avril 2023 : crédit de 1000€\n","31 mai 2023 : solde de 1300€\n"},{"23 août 2019 : solde de 34000€\n","2 septembre 2020 : crédit de 100€\n","11 juillet 2021 : solde de 3,29\n"}}};
    struct client c4 = {"Noe\n", "grand\n", {"22\n","23\n","24\n"}, {2000,3000,4000},{{"12 mars 2023 : solde de 200€\n","24 avril 2023 : crédit de 1000€\n","31 mai 2023 : solde de 1300€\n"},{"23 août 2019 : solde de 34000€\n","2 septembre 2020 : crédit de 100€\n","11 juillet 2021 : solde de 3,29\n"}}};
    struct client tab[4] = {c1,c2,c3,c4};
    struct client p1;
    int index_cpt,k;
    int indice;
    int sortie_while = 0; 

    // Variables du temps
    time_t now;
    time(&now);
    struct tm *local = localtime(&now);

    //------------------------------Début du programme - configuration générale--------------------------------------------------
    // Cette partie du programme a été pris puis légèrement modifié depuis le site internet du lien numéro 3 de l'énoncé    
    // Création de la socket + vérification
    int sock = socket(AF_INET, SOCK_DGRAM, 0);
    if(sock < 0) perror("socket()");
    
    // Configuration du type d'adresses utilisables, de l'adresse et du port
    sin.sin_addr.s_addr = htonl(INADDR_ANY);
    sin.sin_family = AF_INET;
    sin.sin_port = htons(20001);

    // Assignation d'une adresse à la socket + vérification
    int valBind = bind (sock, (struct sockaddr *) &sin, sizeof sin);  
    if(valBind < 0) perror("bind()");

    recvfrom(sock, buffer, sizeof(buffer) - 1, 0, (struct sockaddr *)&from, &fromsize);
    sendto(sock, buffer, strlen(buffer), 0, (struct sockaddr *)&from, fromsize);

    //------------------------------Début de l'échange entre le serveur et le client--------------------------------------------------
    //-------------Demande des informations----------------------
    // Accueil de la banque + Demande id
    printf("----------------------------------------------------\n");
    sendto(sock, "Bienvenue à la banque, veuillez vous connecter avant de faire une opération.\nEntrez votre identifiant : ", 110, 0, (struct sockaddr *)&from, fromsize);
    printf("Interface de la banque\n");

    // Réception id
    char client_id[255];
    recvfrom(sock, client_id, 255, 0, (struct sockaddr *)&from, &fromsize);

    // Demande mdp
    sendto(sock,"Entrez votre mot de passe : ", 33, 0, (struct sockaddr *)&from, fromsize);

    // Réception mdp
    char client_mdp[255];
    recvfrom(sock, client_mdp, 255, 0, (struct sockaddr *)&from, &fromsize);

    // Demande numéro de compte
    sendto(sock,"Entrez votre numéro de compte : ", 35, 0, (struct sockaddr *)&from, fromsize);

    // Réception numéro de compte
    char client_num_cpt[255];
    recvfrom(sock, client_num_cpt, 255, 0, (struct sockaddr *)&from, &fromsize);

    //---------------Vérification des informations--------------------------
    // Vérification de l'identifiant parmi le tableau des clients et leur id
    int l,index_client;
    int indic_verif = 0;
    for (l = 0; l < 4;l++){
        if (strcmp(client_id,tab[l].id) == 0){
            indic_verif = 1;
            index_client = l;
        }
    }

    // Si il n'y a aucune correspondance, on rentre dans la boucle et on demande à l'utilisateur de réentrer son identifiant
    while(indic_verif == 0){
        // On envoie la nouvelle demande
        sendto(sock,"Votre identifiant est invalide. Veuillez le retaper : ", 60, 0, (struct sockaddr *)&from, fromsize);
        printf("Identifiant invalide\n");

        // On reçoit le nouvel id du client
        bzero(client_id,255);
        recvfrom(sock, client_id, 255, 0, (struct sockaddr *)&from, &fromsize);

        // Nouvelle vérification de l'identifiant récupéré
        for (l = 0; l < 4;l++){
            if (strcmp(client_id,tab[l].id) == 0){
                indic_verif = 1;
                index_client = l;
            }
        }
    }

    // Prise du client selon l'utilisateur reconnu
    p1 = tab[index_client];

    // On envoie le feu vert à client.c pour qu'il sorte de la boucle
    printf("Identifiant du client : %s",client_id);
    sendto(sock,"id_ok", 10, 0, (struct sockaddr *)&from, fromsize);

    // On reçoit la confirmation du client (partie un peu transitoire)
    bzero(buffer,255);
    recvfrom(sock, buffer, 255, 0, (struct sockaddr *)&from, &fromsize);
    printf("%s\n",buffer);

    // Vérification du mot de passe, si il n'y a pas de correspondance, on redemande le mdp
    while(strcmp(client_mdp,p1.mdp) != 0){
        sendto(sock,"Votre mot de passe est invalide. Veuillez le retaper : ", 60, 0, (struct sockaddr *)&from, fromsize);
        printf("Mot de passe invalide\n");

        bzero(client_mdp,255);
        recvfrom(sock, client_mdp, 255, 0, (struct sockaddr *)&from, &fromsize);
    }

    // On envoie le feu vert à client.c pour qu'il sorte de la boucle
    printf("Mot de passe du client : %s",client_mdp);
    sendto(sock,"mdp_ok", 10, 0, (struct sockaddr *)&from, fromsize);

    // On reçoit la confirmation du client (partie un peu transitoire)
    bzero(buffer,255);
    recvfrom(sock, buffer, 255, 0, (struct sockaddr *)&from, &fromsize);
    printf("%s\n",buffer);

    // Vérification du numéro de compte parmi les différents comptes du client
    indic_verif = 0;

    for (k = 0; k < 10;k++){
        if (strcmp(client_num_cpt,p1.num[k]) == 0){
            indic_verif = 1;
            index_cpt = k;
        }
    }

    // Si il n'y a aucune correspondance, on rentre dans la boucle et on demande à l'utilisateur de réentrer son numéro de compte
    while(indic_verif == 0){
        // On envoie la nouvelle demande
        sendto(sock,"Votre numéro de compte est invalide. Veuillez le retaper : ", 70, 0, (struct sockaddr *)&from, fromsize);
        printf("Numéro de compte invalide\n");

        // On reçoit le nouveau numéro de compte du client
        bzero(client_num_cpt,255);
        recvfrom(sock, client_num_cpt, 255, 0, (struct sockaddr *)&from, &fromsize);

        // Nouvelle vérification du numéro de compte récupéré
        for (k = 0; k < 10;k++){
            if (strcmp(client_num_cpt,p1.num[k]) == 0){
                indic_verif = 1;
                index_cpt = k;
            }
        }        
    }


    // On envoie le feu vert à client.c pour qu'il sorte de la boucle
    printf("Numéro de compte du client : %s",client_num_cpt);
    sendto(sock,"num_ok", 10, 0, (struct sockaddr *)&from, fromsize);

    // On reçoit la confirmation du client (partie un peu transitoire)
    bzero(buffer,255);
    recvfrom(sock, buffer, 255, 0, (struct sockaddr *)&from, &fromsize);
    printf("%s\n",buffer);

    //--------------------------Choix des opérations------------------------------
    printf("\n\n------------------------------------------------------------------------------\n");
    printf("Compte de %s",client_id);

    // On rentre dans une boucle où l'on demande à chaque itération l'opération que l'utilisateur veut faire
    // Si l'utlisateur choisit de quitter la banque, on sort de la boucle
    while (sortie_while != 1){
        // Envoi des opérations disponibles
        char choix[241] = "Tapez le numéro en face d'une manoeuvre pour exécuter cette dernière\n1 - Créditer votre compte\n2 - Débiter votre compte\n3 - Afficher la solde de votre compte\n4 - Afficher les 10 dernières opérations effectuées\n5 - Quitter la banque\n\n";
        sendto(sock,choix, 241, 0, (struct sockaddr *)&from, fromsize);
        
        // Réception du choix de l'utilisateur
        bzero(buffer,255);
        recvfrom(sock, buffer, 255, 0, (struct sockaddr *)&from, &fromsize);

        // Switch case sur la valeur du choix
        switch(atoi(buffer)){
            // Si 1, on va faire un crédit
            case 1:
                sendto(sock,"Entrez la somme que voulez créditer à votre compte ", 53, 0, (struct sockaddr *)&from, fromsize);
                
                // On reçoit la somme que l'utilisateur veut ajouter et on l'additionne à sa solde
                bzero(buffer,255);
                recvfrom(sock, buffer, 255, 0, (struct sockaddr *)&from, &fromsize);
                p1.solde[index_cpt] += atoi(buffer);

                // On inscrit cette opération dans l'historique des opérations
                for (indice = 9;indice > 0;indice--){
                    snprintf(p1.operations[index_cpt][indice],50,(char *)p1.operations[index_cpt][indice - 1]);
                }
                snprintf(p1.operations[index_cpt][0],50,"\n%02d/%02d/%d : crédit de %d €\n", local->tm_mday, local->tm_mon +1, local->tm_year + 1900,atoi(buffer));

                printf("======================================\nAjout de %d € à la solde du compte\nNouvelle solde : %d €\n",atoi(buffer),p1.solde[index_cpt]);

                // On envoie le feu vert comme quoi l'opération s'est bien passée
                sendto(sock,"OK\n", 4, 0, (struct sockaddr *)&from, fromsize);
                recvfrom(sock, buffer, 255, 0, (struct sockaddr *)&from, &fromsize);
                break;

            // Si 2, on va faire un débit
            case 2:
                sendto(sock,"Entrez la somme que voulez débiter de votre compte: ", 53, 0, (struct sockaddr *)&from, fromsize);

                // On reçoit la somme que l'utilisateur veut débiter et on la soustrait de sa solde
                bzero(buffer,255);
                recvfrom(sock, buffer, 255, 0, (struct sockaddr *)&from, &fromsize);
                p1.solde[index_cpt] -= atoi(buffer);

                // On inscrit cette opération dans l'historique des opérations
                for (indice = 9;indice > 0;indice--){
                    snprintf(p1.operations[index_cpt][indice],50,p1.operations[index_cpt][indice - 1]);
                }
                snprintf(p1.operations[index_cpt][0],50,"\n%02d/%02d/%d : débit de %d €\n", local->tm_mday, local->tm_mon +1, local->tm_year + 1900,atoi(buffer));

                printf("======================================\nDébit de %d € de la solde du compte\nNouvelle solde : %d €\n",atoi(buffer),p1.solde[index_cpt]);

                // On envoie le feu vert comme quoi l'opération s'est bien passée
                sendto(sock,"OK\n", 4, 0, (struct sockaddr *)&from, fromsize);
                recvfrom(sock, buffer, 255, 0, (struct sockaddr *)&from, &fromsize);
                break;
            
            // Si 3, on va envoyer la solde actuelle
            case 3:
                // On met la chaîne de caractères et la valeur de la solde dans le buffer que l'on va envoyer
                bzero(buffer,255);
                snprintf(buffer,255,"Votre solde actuelle est de %d €\n\n",p1.solde[index_cpt]);
                sendto(sock,buffer, 60, 0, (struct sockaddr *)&from, fromsize);
                recvfrom(sock, buffer, 255, 0, (struct sockaddr *)&from, &fromsize);
                printf("======================================\nAffichage du solde du compte\n");
                break;

            // Si 4, on va envoyer les 10 dernières opérations faites
            case 4:
                // On met chaque opération (chaîne de caractères) dans le buffer que l'on va envoyer
                bzero(buffer,255);
                for (int i = 0;i < 10;i++){
                    strcat(buffer,p1.operations[index_cpt][i]);
                }
                sendto(sock,buffer, 255, 0, (struct sockaddr *)&from, fromsize);
                recvfrom(sock, buffer, 255, 0, (struct sockaddr *)&from, &fromsize);
                printf("======================================\nAffichage des 10 dernières opérations\n");
                break;

            // Si 5, on quitte la banque (on sort de la boucle)
            case 5:
                // On modifie la variable qui va nous permettre de sortir de la boucle
                printf("\n****************************\n* Déconnexion de la banque *\n****************************\n");
                sortie_while = 1;
                break;

            // Si un nombre autre que 1 à 5, on signale que le nombre est invalide et on redemande le numéro de l'opération
            default:
                sendto(sock,"Numéro invalide, veuillez réessayer\n", 40, 0, (struct sockaddr *)&from, fromsize);
                printf("\n**************************\n** Choix invalide **\n**************************\n");

                bzero(buffer,255);
                recvfrom(sock, buffer, 255, 0, (struct sockaddr *)&from, &fromsize);
        }  
    }
    
    // On ferme la socket ouverte
    close(sock);
    return 0; 
    }