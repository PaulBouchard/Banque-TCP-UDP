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

// Structure d'un client
struct client{
    char id[50];
    char mdp[50];
    char num[10][50];
    int solde[10];
    char operations[10][10][50];
};


int main(){
    // Déclaration des variables
    char buffer[256];
    struct sockaddr_in serv_addr, cli_addr;
    struct client c1 = {"Paul\n", "AH\n", {"111\n","222\n","333\n"}, {2000,3000,4000},{{"12/03/2023 : débit de 200€\n","24/04/2023 : crédit de 1000€\n","31/05/2023 : débit de 1300€\n"},{"23/08/2019 : débit de 34000€\n","02/09/2020 : crédit de 100€\n","11/07/2021 : débit de 3,29\n"}}};
    struct client c2 = {"Chaima\n", "bababanks\n", {"1234\n","0987\n","4567\n"}, {2000,3000,4000},{{"12/03/2023 : débit de 200€\n","24/04/2023 : crédit de 1000€\n","31/05/2023 : débit de 1300€\n"},{"23/08/2019 : débit de 34000€\n","02/09/2020 : crédit de 100€\n","11/07/2021 : débit de 3,29\n"}}};
    struct client c3 = {"Matteo\n", "kha\n", {"0000\n","999\n","111\n"}, {2000,3000,4000},{{"12/03/2023 : débit de 200€\n","24/04/2023 : crédit de 1000€\n","31/05/2023 : débit de 1300€\n"},{"23/08/2019 : débit de 34000€\n","02/09/2020 : crédit de 100€\n","11/07/2021 : débit de 3,29\n"}}};
    struct client c4 = {"Noe\n", "grand\n", {"22\n","23\n","24\n"}, {2000,3000,4000},{{"12/03/2023 : débit de 200€\n","24/04/2023 : crédit de 1000€\n","31/05/2023 : débit de 1300€\n"},{"23/08/2019 : débit de 34000€\n","02/09/2020 : crédit de 100€\n","11/07/2021 : débit de 3,29\n"}}};
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
    // Cette partie du programme a été pris puis légèrement modifié depuis le site internet du lien numéro 4 de l'énoncé    
    // Création de la socket + vérification
    int sockfd =  socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) error("ERROR opening socket");

    // Configuration du type d'adresses utilisables, de l'adresse et du port
    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;  
    serv_addr.sin_addr.s_addr = INADDR_ANY;  
    serv_addr.sin_port = htons(20001);

    // Assignation d'une adresse à la socket + vérification
    int valBind = bind(sockfd, (struct sockaddr *) &serv_addr,sizeof(serv_addr));
    if (valBind < 0) error("ERROR on binding");

    // Création d'une file d'attente pour les 5 prochaines différentes connexions 
    listen(sockfd,5);

    // Récupération des informations sur les adresses du client et connexion avec ce dernier + vérification
    socklen_t clilen = sizeof(cli_addr);
    int newsockfd = accept(sockfd,(struct sockaddr *) &cli_addr, &clilen);
    if (newsockfd < 0) error("ERROR on accept");

    // Affichage des informations importantes
    printf("server: got connection from %s port %d\n",inet_ntoa(cli_addr.sin_addr), ntohs(cli_addr.sin_port));


    //------------------------------Début de l'échange entre le serveur et le client--------------------------------------------------
    //-------------Demande des informations----------------------
    // Accueil de la banque + Demande id
    printf("----------------------------------------------------\n");
    write(newsockfd, "Bienvenue à la banque, veuillez vous connecter avant de faire une opération.\nEntrez votre identifiant : ", 110);
    printf("Interface de la banque\n");

    // Réception id
    char client_id[255];
    read(newsockfd,client_id,255);

    // Demande mdp
    write(newsockfd, "Entrez votre mot de passe : ", 33);

    // Réception mdp
    char client_mdp[255];
    read(newsockfd,client_mdp,255);

    // Demande numéro de compte
    write(newsockfd, "Entrez votre numéro de compte : ", 35);

    // Réception numéro de compte
    char client_num_cpt[255];
    read(newsockfd,client_num_cpt,255);

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
        write(newsockfd, "Votre identifiant est invalide. Veuillez le retaper : ", 70);
        printf("Identifiant invalide\n");

        // On reçoit le nouvel id du client
        bzero(client_id,255);
        read(newsockfd,client_id,255);

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
    write(newsockfd, "id_ok", 10);

    // On reçoit la confirmation du client (partie un peu transitoire)
    bzero(buffer,255);
    read(newsockfd,buffer,255);
    printf("%s\n",buffer);

    // Vérification du mot de passe, si il n'y a pas de correspondance, on redemande le mdp
    while(strcmp(client_mdp,p1.mdp) != 0){
        write(newsockfd, "Votre mot de passe est invalide. Veuillez le retaper : ", 70);
        printf("Mot de passe invalide\n");

        bzero(client_mdp,255);
        read(newsockfd,client_mdp,255);
    }

    // On envoie le feu vert à client.c pour qu'il sorte de la boucle
    printf("Mot de passe du client : %s",client_mdp);
    write(newsockfd, "mdp_ok", 10);

    // On reçoit la confirmation du client (partie un peu transitoire)
    bzero(buffer,255);
    read(newsockfd,buffer,255);
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
        write(newsockfd, "Votre numéro de compte est invalide. Veuillez le retaper : ", 70);
        printf("Numéro de compte invalide\n");

        // On reçoit le nouveau numéro de compte du client
        bzero(client_num_cpt,255);
        read(newsockfd,client_num_cpt,255);

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
    write(newsockfd, "num_ok", 10);

    // On reçoit la confirmation du client (partie un peu transitoire)
    bzero(buffer,255);
    read(newsockfd,buffer,255);
    printf("%s\n",buffer);

    //--------------------------Choix des opérations------------------------------
    printf("\n\n------------------------------------------------------------------------------\n");
    printf("Compte de %s",client_id);

    // On rentre dans une boucle où l'on demande à chaque itération l'opération que l'utilisateur veut faire
    // Si l'utlisateur choisit de quitter la banque, on sort de la boucle
    while (sortie_while != 1){
        // Envoi des opérations disponibles
        char choix[241] = "Tapez le numéro en face d'une manoeuvre pour exécuter cette dernière\n1 - Créditer votre compte\n2 - Débiter votre compte\n3 - Afficher la solde de votre compte\n4 - Afficher les 10 dernières opérations effectuées\n5 - Quitter la banque\n\n";
        write(newsockfd,choix, 241);
        
        // Réception du choix de l'utilisateur
        bzero(buffer,255);
        read(newsockfd,buffer,255); 

        // Switch case sur la valeur du choix
        switch(atoi(buffer)){
            // Si 1, on va faire un crédit
            case 1:
                write(newsockfd, "Entrez la somme que voulez créditer à votre compte ", 53);

                // On reçoit la somme que l'utilisateur veut ajouter et on l'additionne à sa solde
                bzero(buffer,255);
                read(newsockfd,buffer,255);
                p1.solde[index_cpt] += atoi(buffer);
                
                // On inscrit cette opération dans l'historique des opérations
                for (indice = 9;indice > 0;indice--){
                    snprintf(p1.operations[index_cpt][indice],50,(char *)p1.operations[index_cpt][indice - 1]);
                }
                snprintf(p1.operations[index_cpt][0],50,"%02d/%02d/%d : crédit de %d €\n", local->tm_mday, local->tm_mon +1, local->tm_year + 1900,atoi(buffer));

                printf("======================================\nAjout de %d € à la solde du compte\nNouvelle solde : %d €\n",atoi(buffer),p1.solde[index_cpt]);
                
                // On envoie le feu vert comme quoi l'opération s'est bien passée
                write(newsockfd, "OK\n", 255);
                read(newsockfd,buffer,255);
                break;

            // Si 2, on va faire un débit
            case 2:
                write(newsockfd, "Entrez la somme que voulez débiter de votre compte ", 53);

                // On reçoit la somme que l'utilisateur veut débiter et on la soustrait de sa solde
                bzero(buffer,255);
                read(newsockfd,buffer,255);
                p1.solde[index_cpt] -= atoi(buffer);

                // On inscrit cette opération dans l'historique des opérations
                for (indice = 9;indice > 0;indice--){
                    snprintf(p1.operations[index_cpt][indice],50,p1.operations[index_cpt][indice - 1]);
                }
                snprintf(p1.operations[index_cpt][0],50,"%02d/%02d/%d : débit de %d €\n", local->tm_mday, local->tm_mon +1, local->tm_year + 1900,atoi(buffer));

                printf("======================================\nDébit de %d € de la solde du compte\nNouvelle solde : %d €\n",atoi(buffer),p1.solde[index_cpt]);

                // On envoie le feu vert comme quoi l'opération s'est bien passée
                write(newsockfd, "OK\n", 5);
                read(newsockfd,buffer,255);
                break;

            // Si 3, on va envoyer la solde actuelle
            case 3:
                // On met la chaîne de caractères et la valeur de la solde dans le buffer que l'on va envoyer
                bzero(buffer,255);
                snprintf(buffer,255,"Votre solde actuelle est de %d €\n",p1.solde[index_cpt]);
                write(newsockfd,buffer, 255);
                read(newsockfd,buffer,255);
                printf("======================================\nAffichage du solde du compte\n");
                break;

            // Si 4, on va envoyer les 10 dernières opérations faites
            case 4:
                // On met chaque opération (chaîne de caractères) dans le buffer que l'on va envoyer
                bzero(buffer,255);
                for (int i = 0;i < 10;i++){
                    strcat(buffer,p1.operations[index_cpt][i]);
                }
                write(newsockfd,buffer, 255);
                read(newsockfd,buffer,255);
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
                write(newsockfd,"Numéro invalide, veuillez réessayer\n\n",41);
                printf("\n**************************\n** Choix invalide **\n**************************\n");

                bzero(buffer,255);
                read(newsockfd,buffer,255);
        }
    }

    // On ferme les sockets ouvertes
    close(newsockfd);
    close(sockfd);
    return 0; 
    }