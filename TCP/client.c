#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>

void error(const char *msg){
    perror(msg);
    exit(0);
}

int main(){
    // Déclaration des variables
    char buffer[256];
    struct sockaddr_in serv_addr;
    char proposition_choix[255];
    int sortie_while = 0;
    char prob_choix[255];
    char somme[255];

    //------------------------------Début du programme - configuration générale--------------------------------------------------
    // Cette partie du programme a été pris puis légèrement modifié depuis le site internet du lien numéro 4 de l'énoncé
    // Création de la socket + vérification
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) error("ERROR opening socket");

    // Configuration du type d'adresses utilisables, de l'adresse et du port
    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(20001);

    // Connexion au serveur
    connect(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr));

    //------------------------------Début de l'échange entre le serveur et le client--------------------------------------------------
    // Accueil de la banque + demande id
    printf("-------------------------------------------------------------------------------------------\n");
    char id[255];
    read(sockfd,id,255);
    printf("%s",id);

    // On écrit l'id et on l'envoie
    bzero(buffer,255);
    fgets(buffer,255,stdin);
    write(sockfd, buffer, strlen(buffer)+1);

    // Demande mdp
    char mdp[255];
    read(sockfd,mdp,255);
    printf("%s",mdp);

    // On écrit le mdp et on l'envoie
    bzero(buffer,255);
    fgets(buffer,255,stdin);
    write(sockfd, buffer, strlen(buffer)+1);

    // Demande numéro de compte
    char num_cpt[255];
    read(sockfd,num_cpt,255);
    printf("%s",num_cpt);

    // On écrit le numéro de compte et on l'envoie
    bzero(buffer,255);
    fgets(buffer,255,stdin);
    write(sockfd, buffer, strlen(buffer)+1);

    // Vérification de l'identifiant
    char idok[255] = "id_ok";
    char ridok[255];
    read(sockfd,ridok,255);

    // Tant qu'on a pas le feu vert dans ridok, on reste dans la boucle
    while(strcmp(idok,ridok) != 0){
        // On affiche la demande de réécrire l'id
        printf("%s",ridok);
        
        // On réécrit l'id et on l'envoie
        bzero(buffer,255);
        fgets(buffer,255,stdin);
        write(sockfd, buffer, strlen(buffer)+1);

        // On reçoit le feu vert ou la demande de réécrire l'id
        bzero(ridok,255);
        read(sockfd,ridok,255);
    }

    // On envoie la confirmation au serveur (partie un peu transitoire)
    bzero(buffer,255);
    strcpy(buffer,"Identifiant validé");
    write(sockfd, buffer, strlen(buffer)+1);
    printf("%s\n",buffer);

    // Vérification du mot de passe
    char mdpok[255] = "mdp_ok";
    char rmdpok[255];
    read(sockfd,rmdpok,255);

    // Tant qu'on a pas le feu vert dans rmdpok, on reste dans la boucle
    while(strcmp(mdpok,rmdpok) != 0){
        // On affiche la demande de réécrire le mdp
        printf("%s",rmdpok);
        
        // On réécrit le mdp et on l'envoie
        bzero(buffer,255);
        fgets(buffer,255,stdin);
        write(sockfd, buffer, strlen(buffer)+1);

        // On reçoit le feu vert ou la demande de réécrire le mdp
        bzero(rmdpok,255);
        read(sockfd,rmdpok,255);
    }

    // On envoie la confirmation au serveur (partie un peu transitoire)
    bzero(buffer,255);
    strcpy(buffer,"Mot de passe validé");
    write(sockfd, buffer, strlen(buffer)+1);
    printf("%s\n",buffer);

    // Vérification du numéro de compte
    char numok[255] = "num_ok";
    char rnumok[255];
    read(sockfd,rnumok,255);

    // Tant qu'on a pas le feu vert dans rnumok, on reste dans la boucle
    while(strcmp(numok,rnumok) != 0){
        // On affiche la demande de réécrire le numéro de compte
        printf("%s",rnumok);
        
        // On réécrit le numéro de compte et on l'envoie
        bzero(buffer,255);
        fgets(buffer,255,stdin);
        write(sockfd, buffer, strlen(buffer)+1);

        // On reçoit le feu vert ou la demande de réécrire le numéro de compte
        bzero(rnumok,255);
        read(sockfd,rnumok,255);
    }

    // On envoie la confirmation au serveur (partie un peu transitoire)
    bzero(buffer,255);
    strcpy(buffer,"Numéro de compte validé");
    write(sockfd, buffer, strlen(buffer)+1);
    printf("%s\n",buffer);


    //--------------------------Choix des opérations------------------------------
    printf("\n\n------------------------------------------------------------------------------\n");

    // On rentre dans une boucle où l'on envoie à chaque itération l'opération que l'utilisateur veut faire
    // Si l'utlisateur choisit de quitter la banque, on sort de la boucle
    while (sortie_while != 1){
        // Réception des opérations disponibles
        bzero(proposition_choix,255);
        read(sockfd,proposition_choix,255);
        printf("%s",proposition_choix);

        // Envoi du choix fait
        bzero(buffer,255);
        fgets(buffer,255,stdin);
        buffer[strcspn(buffer,"\n")] = 0;
        write(sockfd, buffer, strlen(buffer)+1);
        
        // Switch case sur la valeur du choix
        switch(atoi(buffer)){
            // Si 1, on va faire un crédit
            case 1:
                // On reçoit la demande du montant du crédit
                bzero(somme,255);
                read(sockfd,somme,255);

                bzero(buffer,255);
                while(atoi(buffer) < 1 || atoi(buffer) > 1000000){
                    bzero(buffer,255);
                    printf("\n%s (entre 1€ et 1000000€) : ",somme);
                    fgets(buffer,255,stdin);
                }
                
                // On envoie la somme que l'on veut créditer
                write(sockfd, buffer, 255);

                // On reçoit le feu vert du bon fonctionnement de l'opération
                bzero(buffer,255);
                read(sockfd,buffer,255);
                printf("%s\n",buffer);

                write(sockfd, buffer, 255);
                break;

            // Si 2, on va faire un débit
            case 2:
                // On reçoit la demande du montant du débit
                bzero(somme,255);
                read(sockfd,somme,255);

                bzero(buffer,255);
                while(atoi(buffer) < 1 || atoi(buffer) > 10000){
                    bzero(buffer,255);
                    printf("\n%s (entre 1€ et 10000€) : ",somme);
                    fgets(buffer,255,stdin);
                }
                
                // On envoie la somme que l'on veut débiter
                write(sockfd, buffer, strlen(buffer)+1);

                // On reçoit le feu vert du bon fonctionnement de l'opération
                bzero(buffer,255);
                read(sockfd,buffer,255);
                printf("%s\n\n",buffer);

                write(sockfd, buffer, 255);
                break;

            // Si 3, on va afficher la solde actuelle
            case 3:
                bzero(somme,255);
                read(sockfd,somme,255);
                printf("\n%s\n\n",somme);

                write(sockfd, buffer, 255);
                break;
            // Si 4, on va afficher les 10 dernières opérations faites
            case 4:
                bzero(somme,255);
                read(sockfd,somme,255);
                printf("\n%s\n\n",somme);

                write(sockfd, buffer, 255);
                break;

            // Si 5, on quitte la banque (on sort de la boucle)
            case 5:
                // On modifie la variable qui va nous permettre de sortir de la boucle
                sortie_while = 1;
                break;

            // Si un nombre autre que 1 à 5, on est signalé que le nombre est invalide et on réécrit le numéro de l'opération
            default:
                read(sockfd,prob_choix,255);
                printf("%s",prob_choix);

                bzero(buffer,255);
                write(sockfd, buffer, strlen(buffer)+1);
        }  
    }

    // On ferme les sockets ouvertes
    close(sockfd);
    return 0;
}