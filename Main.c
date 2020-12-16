#include <unistd.h>
#include <stdio.h> 
#include <sys/types.h> 
#include <sys/wait.h>
#include <stdlib.h>
#include <math.h>
#include <ctype.h>
#include <string.h>

//Prototypes
int addition(char * donnees);
int soustraction(char * donnees);
int multiplication(char * donnees);
int puissance(char * donnees);

int main()
{
	//les ports des pipes utilisés pour l'addition
	//p1[0] -> lecture du message du client par le serveur
	//p1[1] -> ecriture du client vers le serveur

	//p2[0] -> lecture du message du serveur par le client
	//p2[1] -> ecriture du serveur vers le client

	//Les ports des pipes pour la multiplication 
	//p3[0] -> lecture du message du client par le serveur
	//p3[1] -> ecriture du client vers le serveur

	//p4[0] -> lecture du message du serveur par le client
	//p4[1] -> ecriture du serveur vers le client

	//creation des pipe
	int p1[2];
	int p2[2];
	int p3[2];//Pipe qui envoie le choix de l'opération au serveur
	int p4[4];//pipe pour envoyer la taille des donnees client(fils) au serveur(pere)	

	pipe(p1);
	pipe(p2);
	pipe(p3); 
	pipe(p4);

	//variable identification processus
	pid_t pcs;

	//variables pour les fonctions de calculs
	char * donnees; //Taille des données entrées par l'utilisateur
	int tailleDonnees; //strlen(donnees)
	int tailleDonneesLuServer;
	int resultat;
	int produit;
	int choixOpe;
	char ope;

	//le nombre de caractères dans le mot
	char mot[30];

	//creation du processus client
	pcs = fork();

	if (pcs == 0){
			//demande les chiffres à additionner
			do{
				printf("Veuillez choisir votre opération; [addition -> 1 : soustraction -> 2 : multiplication -> 3 : puissance -> 4 : distance -> 5\n");
				scanf("%d",&choixOpe);
			}while(choixOpe > 5 || choixOpe < 1);

			
			printf("Veuillez les valeurs à calculer separees de ;\n");
			scanf("%s",donnees); //5;4;4;6;9
			tailleDonnees = strlen(donnees);
				
			//taille des données: sizeof(char)

			//le client est le pcs qui écrit dans le tube
			close(p1[0]); // vérouille la lecture du père 
			close(p2[1]); //fermer l'ecriture sur le pipe du père
			close(p3[0]);//fermer la lecture du pipe sur le choix de l'operation 
			close(p4[0]);//fermer la lecture du pipe sur la taille des données

			write(p3[1],&choixOpe,sizeof(int));
			write(p4[1],&tailleDonnees,sizeof(int));//envoyer la taille des donnees au serveur
			write(p1[1],&donnees,tailleDonnees);//envoyer les donnees brutes au serveur
			
			read(p2[0],&resultat,sizeof(int));
			printf(" le resultat renvoyé par le serveur: %d\n",resultat);

			exit(0);
		}

	else{
		//le serveur lit le tube et répond 
		printf("Vous êtes maintenant dans le serveur" );

		close(p1[1]);//femer l'écriture du client
		close(p2[0]);//fermer la lecture du client
		close(p3[1]);//fermer l'ecriture dans le pipe du choix de l'operation 
		close(p4[1]);//fermer l'ecriture dans le pipe de la taille des donnees

		read(p3[0],&choixOpe,sizeof(int));
		read(p4[0],&tailleDonneesLuServer,sizeof(int));// on lit la taille des données
		read(p1[0],&donnees,tailleDonneesLuServer);//on lit ce qui se trouve dans le tube p1 et on le met dans donnees avec une taille de tailleDonneesLuServer

		switch(choixOpe){
			case 1 : 
				resultat = addition(donnees);
				printf("Le resultat: %d\n",resultat );
				ope = '+';
			break;
			case 2 : 
				resultat = soustraction(donnees);
				printf("Le resultat: %d\n",resultat );
				ope = '-';
			break;
			case 3 : 
				resultat = multiplication(donnees);
				printf("Le resultat: %d\n",resultat );
				ope = '*';
			break;
			case 4 : 
				resultat = puissance(donnees);
				printf("Le resultat: %d\n",resultat );
				ope = '^';
			break; 

		}

		printf(" le client a dit: %s",donnees);
		write(p2[1],&resultat,sizeof(int));
		close(p2[1]);//fermer l'écriture
		wait(0);
	}

	return 0;
}

int addition(char * donnees){
	printf("Les donnees envoyees par le client: %s\n",donnees );
	return 1;
}

int soustraction(char * donnees){
	printf("Les donnees envoyees par le client: %s\n",donnees );
	return 1;
}

int multiplication(char * donnees){
	printf("Les donnees envoyees par le client: %s\n",donnees );
	return 1;
}

int puissance(char * donnees){
	printf("Les donnees envoyees par le client: %s\n",donnees );
	return 1;
}